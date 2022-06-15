/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// WebSocketEntities.cpp - Source file for the webSocket related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file WebSocketEntities.cpp

#include <discordcoreapi/WebSocketEntities.hpp>
#include <discordcoreapi/JSONIfier.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <concepts>

namespace DiscordCoreInternal {

	constexpr uint16_t webSocketMaxPayloadLengthLarge{ 65535u };
	constexpr uint8_t webSocketPayloadLengthMagicLarge{ 126u };
	constexpr uint8_t webSocketPayloadLengthMagicHuge{ 127u };
	constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2u };
	constexpr uint8_t webSocketMaxPayloadLengthSmall{ 125u };
	constexpr uint8_t webSocketFinishBit{ (1u << 7u) };
	constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

	BaseSocketAgent::BaseSocketAgent(const std::string& botTokenNew, const std::string& baseUrlNew, DiscordCoreAPI::EventManager* eventManagerNew,
		DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, DiscordCoreAPI::CommandController* commandControllerNew, std::atomic_bool* doWeQuitNew,
		bool doWePrintSuccessMessagesNew, bool doWePrintErrorMessagesNew, int32_t currentBaseSocketAgentNew) noexcept {
		this->doWePrintSuccessMessages = doWePrintSuccessMessagesNew;
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->doWePrintErrorMessages = doWePrintErrorMessagesNew;
		this->commandController = commandControllerNew;
		this->discordCoreClient = discordCoreClientNew;
		this->eventManager = eventManagerNew;
		this->botToken = botTokenNew;
		this->doWeQuit = doWeQuitNew;
		this->theFormat = this->discordCoreClient->theFormat;
		if (this->theFormat == DiscordCoreAPI::TextFormat::Etf) {
			this->dataOpcode = WebSocketOpCode::Op_Binary;
		} else {
			this->dataOpcode = WebSocketOpCode::Op_Text;
		}
		this->baseUrl = baseUrlNew;
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
	}

	void BaseSocketAgent::sendMessage(const nlohmann::json& dataToSend, int32_t theIndex) noexcept {
		try {
			if (this->theClients.contains(theIndex)) {
				DiscordCoreAPI::StopWatch stopWatch{ 5500ms };
				while (!this->theClients[theIndex]->areWeConnected.load() && !(dataToSend.contains("op") && (dataToSend["op"] == 2 || dataToSend["op"] == 6))) {
					if (stopWatch.hasTimePassed()) {
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
				if (this->doWePrintSuccessMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->theClients[theIndex]->shard.dump() + std::string("'s Message: ")
							  << dataToSend.dump() << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				std::string theVectorNew{};
				this->stringifyJsonData(dataToSend, theVectorNew);
			
				this->theClients[theIndex]->writeData(theVectorNew);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed(*this->theClients[theIndex]);
		}
	}

	void BaseSocketAgent::sendMessage(std::string& dataToSend, int32_t theIndex) noexcept {
		try {
			std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
			if (this->doWePrintSuccessMessages) {
				std::lock_guard<std::mutex> theLock{ this->discordCoreClient->coutMutex };
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->theClients[theIndex]->shard.dump() + std::string("'s Message: ") << std::endl
						  << dataToSend << DiscordCoreAPI::reset();
			}
			if (this->theClients.contains(theIndex)) {
				this->theClients[theIndex]->writeData(dataToSend);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed(*this->theClients[theIndex]);
		}
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage thePackage) noexcept {
		this->connections.push(thePackage);
		while (!this->discordCoreClient->theStopWatch.hasTimePassed() || !this->theClients.contains(thePackage.currentShard)) {
			std::this_thread::sleep_for(1ms);
		}
		this->discordCoreClient->theStopWatch.resetTimer();
	}

	void BaseSocketAgent::onClosed(WebSocketSSLShard& theShard) noexcept {
		this->areWeReadyToConnectEvent.reset();
		if (this->maxReconnectTries > theShard.currentRecursionDepth) {
			std::this_thread::sleep_for(500ms);
			if (this->doWePrintErrorMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket " + theShard.shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(theShard.closeCode)
						  << DiscordCoreAPI::reset() << std::endl
						  << std::endl;
			}
			if (this->theClients.contains(theShard.shard[0])) {
				this->sendCloseFrame(theShard.shard[0]);
			}			
			DiscordCoreAPI::ConnectionPackage theData{};
			theData.currentShard = theShard.shard[0];
			theData.currentReconnectionDepth = theShard.currentRecursionDepth;
			theData.sessionId = theShard.sessionId;
			theData.areWeResuming = theShard.areWeResuming;
			theData.currentBaseSocketAgent = this->currentBaseSocketAgent;
			theData.lastNumberReceived = theShard.lastNumberReceived;
			this->theClients.erase(theShard.shard[0]);
			this->connections.push(theData);
		} else if (this->maxReconnectTries <= theShard.currentRecursionDepth) {
			this->doWeQuit->store(true);
			this->theTask->request_stop();
		}
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->theTask.get();
	}

	void BaseSocketAgent::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect, int32_t theIndex) noexcept {
		try {
			this->semaphore.acquire();
			DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
			dataPackage.channelId = 0;
			dataPackage.guildId = doWeCollect.guildId;
			dataPackage.selfDeaf = doWeCollect.selfDeaf;
			dataPackage.selfMute = doWeCollect.selfMute;
			this->userId = doWeCollect.userId;
			nlohmann::json newData = JSONIFY(dataPackage);
			this->sendMessage(newData, theIndex);
			try {
				WebSocketSSLShard::processIO(this->theClients, 100000);
			} catch (...) {
				if (this->doWePrintErrorMessages) {
					DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
				}
			}
			std::this_thread::sleep_for(500ms);
			if (doWeCollect.channelId == 0) {
				return;
			}
			dataPackage.channelId = doWeCollect.channelId;
			newData = JSONIFY(dataPackage);
			this->areWeCollectingData = true;
			this->sendMessage(newData, theIndex);
			try {
				WebSocketSSLShard::processIO(this->theClients, 100000);
			} catch (...) {
				if (this->doWePrintErrorMessages) {
					DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
				}
			}
			DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theStopWatch{ 5000ms };
			while (this->areWeCollectingData) {
				if (theStopWatch.hasTimePassed()) {
					break;
				}
				std::this_thread::sleep_for(1ms);
			}
			this->semaphore.release();
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
			}
			this->onClosed(*this->theClients[theIndex]);
		}
	}

	void BaseSocketAgent::createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept {
		try {
			outBuffer.push_back(static_cast<uint8_t>(opCode) | webSocketFinishBit);

			int32_t indexCount{ 0 };
			if (sendLength <= webSocketMaxPayloadLengthSmall) {
				outBuffer.push_back(static_cast<uint8_t>(sendLength));
				indexCount = 0;
			} else if (sendLength <= webSocketMaxPayloadLengthLarge) {
				outBuffer.push_back(webSocketPayloadLengthMagicLarge);
				indexCount = 2;
			} else {
				outBuffer.push_back(webSocketPayloadLengthMagicHuge);
				indexCount = 8;
			}
			for (int32_t x = indexCount - 1; x >= 0; x--) {
				outBuffer.push_back(static_cast<uint8_t>(sendLength >> x * 8));
			}

			outBuffer[1] |= webSocketMaskBit;
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::createHeader()");
			}
		}
	}

	void BaseSocketAgent::stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString) noexcept {
		std::string theVector{};
		std::string header{};
		if (this->theFormat == DiscordCoreAPI::TextFormat::Etf) {
			theVector = this->erlPacker.parseJsonToEtf(dataToSend);
		} else {
			theVector = dataToSend.dump();
		}
		this->createHeader(header, theVector.size(), this->dataOpcode);
		std::string theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		theString = theVectorNew;
	}

	void BaseSocketAgent::parseHeadersAndMessage(WebSocketSSLShard& theShard) noexcept {
		if (theShard.theState == WebSocketState::Connecting) {
			std::string newVector{};
			if (theShard.inputBuffer.find("\r\n\r\n") != std::string::npos) {
				std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
				newVector.erase(0, newVector.find("\r\n\r\n") + 4);
				theShard.inputBuffer.clear();
				theShard.inputBuffer.insert(theShard.inputBuffer.end(), newVector.begin(), newVector.end());
				theShard.theState = WebSocketState::Connected;
			}
		}
		if (theShard.inputBuffer.size() < 4) {
			return;
		}
		theShard.dataOpCode = static_cast<WebSocketOpCode>(theShard.inputBuffer[0] & ~webSocketFinishBit);
		switch (theShard.dataOpCode) {
			case WebSocketOpCode::Op_Continuation:
				[[fallthrough]];
			case WebSocketOpCode::Op_Text:
				[[fallthrough]];
			case WebSocketOpCode::Op_Binary:
				[[fallthrough]];
			case WebSocketOpCode::Op_Ping:
				[[fallthrough]];
			case WebSocketOpCode::Op_Pong: {
				uint8_t length01 = theShard.inputBuffer[1];
				theShard.messageOffset = 2;
				if (length01 & webSocketMaskBit) {
					return;
				}
				theShard.messageLength = length01;
				if (length01 == webSocketPayloadLengthMagicLarge) {
					if (theShard.inputBuffer.size() < 8) {
						return;
					}
					uint8_t length03 = theShard.inputBuffer[2];
					uint8_t length04 = theShard.inputBuffer[3];
					theShard.messageLength = static_cast<uint64_t>((length03 << 8) | length04);
					theShard.messageOffset += 2;
				} else if (length01 == webSocketPayloadLengthMagicHuge) {
					if (theShard.inputBuffer.size() < 10) {
						return;
					}
					theShard.messageLength = 0;
					for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
						uint8_t lengthNew = static_cast<uint8_t>(theShard.inputBuffer[x]);
						theShard.messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
					}
					theShard.messageOffset += 8;
				}
				if (theShard.inputBuffer.size() < static_cast<uint64_t>(theShard.messageOffset) + static_cast<uint64_t>(theShard.messageLength)) {
					return;
				} else {
					std::string finalMessage{};
					finalMessage.insert(finalMessage.begin(), theShard.inputBuffer.begin() + theShard.messageOffset,
						theShard.inputBuffer.begin() + theShard.messageOffset + theShard.messageLength);
					theShard.processedMessages.push(finalMessage);
					theShard.inputBuffer.erase(theShard.inputBuffer.begin(), theShard.inputBuffer.begin() + theShard.messageOffset + theShard.messageLength);
					return;
				}
			}
			case WebSocketOpCode::Op_Close: {
				uint16_t close = theShard.inputBuffer[2] & 0xff;
				close <<= 8;
				close |= theShard.inputBuffer[3] & 0xff;
				theShard.closeCode = static_cast<WebSocketCloseCode>(close);
				theShard.inputBuffer.erase(theShard.inputBuffer.begin(), theShard.inputBuffer.begin() + 4);
				this->onClosed(*this->theClients[theShard.shard[0]]);
				return;
			}
			default: {
				return;
			}
		}
	}

	void BaseSocketAgent::checkForAndSendHeartBeat(int32_t theIndex, bool isImmediate) noexcept {
		try {
			if ((this->theClients.contains(theIndex) && this->theClients[theIndex]->heartBeatStopWatch.hasTimePassed() && this->theClients[theIndex]->haveWeReceivedHeartbeatAck) ||
				isImmediate) {
				nlohmann::json heartbeat = JSONIFY(this->theClients[theIndex]->lastNumberReceived);
				if (this->doWePrintSuccessMessages) {
					std::lock_guard<std::mutex> theLock{ this->discordCoreClient->coutMutex };
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->theClients[theIndex]->shard.dump() + std::string("'s Message: ")
							  << heartbeat.dump() << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				std::string theString{};
				this->stringifyJsonData(heartbeat, theString);
				this->theClients[theIndex]->writeData(theString);
				this->theClients[theIndex]->haveWeReceivedHeartbeatAck = false;
				this->theClients[theIndex]->heartBeatStopWatch.resetTimer();
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::checkForAndSendHeartBeat()");
			}
			this->onClosed(*this->theClients[theIndex]);
		}
	}	

	void BaseSocketAgent::onMessageReceived(int32_t theIndex) noexcept {
		if (this->theClients.contains(theIndex)) {
			try {
				std::string messageNew = this->theClients[theIndex]->processedMessages.front();
				this->theClients[theIndex]->processedMessages.pop();
				nlohmann::json payload{};

				if (this->theFormat == DiscordCoreAPI::TextFormat::Etf) {
					try {
						payload = this->erlPacker.parseEtfToJson(&messageNew);
					} catch (...) {
						DiscordCoreAPI::reportException("ErlPacker::parseEtfToJson()");
						return;
					}
				} else {
					payload = nlohmann::json::parse(messageNew);
				}

				if (payload.contains("t") && !payload["t"].is_null()) {
					if (this->areWeCollectingData && payload["t"] == "VOICE_SERVER_UPDATE" && !this->serverUpdateCollected) {
						if (!this->serverUpdateCollected && !this->stateUpdateCollected) {
							this->voiceConnectionData = VoiceConnectionData{};
							this->voiceConnectionData.endPoint = payload["d"]["endpoint"].get<std::string>();
							this->voiceConnectionData.token = payload["d"]["token"].get<std::string>();
							this->serverUpdateCollected = true;
						} else {
							this->voiceConnectionData.endPoint = payload["d"]["endpoint"].get<std::string>();
							this->voiceConnectionData.token = payload["d"]["token"].get<std::string>();
							if (this->voiceConnectionDataBufferMap.contains(payload["d"]["guild_id"])) {
								this->voiceConnectionDataBufferMap[payload["d"]["guild_id"]]->send(this->voiceConnectionData);
							}
							this->serverUpdateCollected = false;
							this->stateUpdateCollected = false;
							this->areWeCollectingData = false;
						}
					}

					if (this->areWeCollectingData && payload["t"] == "VOICE_STATE_UPDATE" && !this->stateUpdateCollected &&
						payload["d"]["member"]["user"]["id"] == std::to_string(this->userId)) {
						if (!this->stateUpdateCollected && !this->serverUpdateCollected) {
							this->voiceConnectionData = VoiceConnectionData{};
							this->voiceConnectionData.sessionId = payload["d"]["session_id"].get<std::string>();
							this->stateUpdateCollected = true;
						} else {
							this->voiceConnectionData.sessionId = payload["d"]["session_id"].get<std::string>();
							if (this->voiceConnectionDataBufferMap.contains(payload["d"]["guild_id"])) {
								this->voiceConnectionDataBufferMap[payload["d"]["guild_id"]]->send(this->voiceConnectionData);
							}
							this->serverUpdateCollected = false;
							this->stateUpdateCollected = false;
							this->areWeCollectingData = false;
						}
					}

					if (payload["t"] == "RESUMED") {
						this->theClients[theIndex]->areWeConnected.store(true);
						this->theClients[theIndex]->currentRecursionDepth = 0;
						this->areWeReadyToConnectEvent.set();
					}

					if (payload["t"] == "READY") {
						this->theClients[theIndex]->areWeConnected.store(true);
						this->theClients[theIndex]->sessionId = payload["d"]["session_id"].get<std::string>();
						DiscordCoreAPI::UserData theUser{};
						parseObject(payload["d"]["user"], theUser);
						this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ theUser, this };
						DiscordCoreAPI::Users::insertUser(theUser);
						std::vector<DiscordCoreAPI::GuildData> theGuilds{};
						parseObject(payload["d"]["guilds"], theGuilds);
						for (auto& value: theGuilds) {
							value.discordCoreClient = this->discordCoreClient;
							DiscordCoreAPI::Guilds::insertGuild(value);
						}
						this->theClients[theIndex]->currentRecursionDepth = 0;
						this->areWeReadyToConnectEvent.set();
					}
				}

				if (payload.contains("s") && !payload["s"].is_null()) {
					this->theClients[theIndex]->lastNumberReceived = payload["s"].get<int32_t>();
				}

				if (payload.contains("op") && !payload["op"].is_null()) {
					if (payload["op"] == 1) {
						this->checkForAndSendHeartBeat(theIndex, true);
					}

					if (payload["op"] == 7) {
						if (this->doWePrintErrorMessages) {
							std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->theClients[theIndex]->shard.dump() + " Reconnecting (Type 7)!"
										<< DiscordCoreAPI::reset() << std::endl
										<< std::endl;
						}
						this->theClients[theIndex]->areWeResuming = true;
						this->onClosed(*this->theClients[theIndex]);
					}

					if (payload["op"] == 9) {
						if (this->doWePrintErrorMessages) {
							std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->theClients[theIndex]->shard.dump() + " Reconnecting (Type 9)!"
										<< DiscordCoreAPI::reset() << std::endl
										<< std::endl;
						}
						this->theClients[theIndex]->currentRecursionDepth += 1;
						std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
						int32_t numOfMsToWait =
							static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
						std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
						if (payload["d"] == true) {
							nlohmann::json identityJson =
								JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue), this->theClients[theIndex]->shard[0], this->theClients[theIndex]->shard[1]);
							this->sendMessage(identityJson, theIndex);
						} else {
							this->theClients[theIndex]->areWeResuming = false;
							this->onClosed(*this->theClients[theIndex]);
						}
					}

					if (payload["op"] == 10) {
						if (payload["d"].contains("heartbeat_interval") && !payload["d"]["heartbeat_interval"].is_null()) {
							this->heartbeatInterval = payload["d"]["heartbeat_interval"];
						}
						if (this->theClients[theIndex]->areWeResuming) {
							std::this_thread::sleep_for(1500ms);
							nlohmann::json resumePayload = JSONIFY(this->botToken, this->theClients[theIndex]->sessionId, this->theClients[theIndex]->lastNumberReceived);
							this->sendMessage(resumePayload, theIndex);
						} else {
							nlohmann::json identityJson =
								JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue), this->theClients[theIndex]->shard[0], this->theClients[theIndex]->shard[1]);
							this->sendMessage(identityJson, theIndex);
						}
						this->theClients[theIndex]->areWeHeartBeating = false;
					}
					if (payload["op"] == 11) {
						this->theClients[theIndex]->haveWeReceivedHeartbeatAck = true;
					}
				}

				if (payload.contains("d") && !payload["d"].is_null() && payload.contains("t") && !payload["t"].is_null()) {
					if (payload["t"] == "APPLICATION_COMMAND_CREATE") {
						std::unique_ptr<DiscordCoreAPI::OnApplicationCommandCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandCreationData>() };
						parseObject(payload["d"], dataPackage->applicationCommand);
						this->eventManager->onApplicationCommandCreationEvent(*dataPackage);
					} else if (payload["t"] == "APPLICATION_COMMAND_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnApplicationCommandUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandUpdateData>() };
						parseObject(payload["d"], dataPackage->applicationCommand);
						this->eventManager->onApplicationCommandUpdateEvent(*dataPackage);
					} else if (payload["t"] == "APPLICATION_COMMAND_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnApplicationCommandDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandDeletionData>() };
						parseObject(payload["d"], dataPackage->applicationCommand);
						this->eventManager->onApplicationCommandDeletionEvent(*dataPackage);
					} else if (payload["t"] == "CHANNEL_CREATE") {
						std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>() };
						parseObject(payload["d"], dataPackage->channel);
						this->eventManager->onChannelCreationEvent(*dataPackage);
					} else if (payload["t"] == "CHANNEL_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>() };
						if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
							dataPackage->channelOld = DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(payload["d"]["id"].get<std::string>()) }).get();
							dataPackage->channelNew = dataPackage->channelOld;
						}
						parseObject(payload["d"], dataPackage->channelNew);
						this->eventManager->onChannelUpdateEvent(*dataPackage);
					} else if (payload["t"] == "CHANNEL_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>() };
						parseObject(payload["d"], dataPackage->channel);
						this->eventManager->onChannelDeletionEvent(*dataPackage);
					} else if (payload["t"] == "CHANNEL_PINS_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
						parseObject(payload["d"], dataPackage->dataPackage);
						this->eventManager->onChannelPinsUpdateEvent(*dataPackage);
					} else if (payload["t"] == "THREAD_CREATE") {
						std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
						parseObject(payload["d"], dataPackage->thread);
						this->eventManager->onThreadCreationEvent(*dataPackage);
					} else if (payload["t"] == "THREAD_UPATE") {
						std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
						parseObject(payload["d"], dataPackage->thread);
						this->eventManager->onThreadUpdateEvent(*dataPackage);
					} else if (payload["t"] == "THREAD_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
						parseObject(payload["d"], dataPackage->thread);
						this->eventManager->onThreadDeletionEvent(*dataPackage);
					} else if (payload["t"] == "THREAD_LIST_SYNC") {
						std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
						parseObject(payload["d"], dataPackage->threadListSyncData);
						this->eventManager->onThreadListSyncEvent(*dataPackage);
					} else if (payload["t"] == "THREAD_MEMBER_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
						parseObject(payload["d"], dataPackage->threadMember);
						this->eventManager->onThreadMemberUpdateEvent(*dataPackage);
					} else if (payload["t"] == "THREAD_MEMBERS_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
						parseObject(payload["d"], dataPackage->threadMembersUpdateData);
						this->eventManager->onThreadMembersUpdateEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_CREATE") {
						DiscordCoreAPI::GuildData guildNew{};
						std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>() };
						parseObject(payload["d"], guildNew);
						guildNew.discordCoreClient = this->discordCoreClient;
						dataPackage->guild = guildNew;
						this->eventManager->onGuildCreationEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>() };
						if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
							dataPackage->guildOld = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(payload["d"]["id"].get<std::string>()) }).get();
							dataPackage->guildNew = dataPackage->guildOld;
						}
						parseObject(payload["d"], dataPackage->guildNew);
						this->eventManager->onGuildUpdateEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>() };
						parseObject(payload["d"], dataPackage->guild);
						this->eventManager->onGuildDeletionEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_BAN_ADD") {
						std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
							DiscordCoreAPI::User newData{};
							parseObject(payload["d"]["user"], newData);
							dataPackage->user = newData;
						}
						this->eventManager->onGuildBanAddEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_BAN_REMOVE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
							DiscordCoreAPI::User newData{};
							parseObject(payload["d"]["user"], newData);
							dataPackage->user = newData;
						}
						this->eventManager->onGuildBanRemoveEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_EMOJIS_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
						parseObject(payload["d"], dataPackage->updateData);
						this->eventManager->onGuildEmojisUpdateEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_STICKERS_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
						parseObject(payload["d"], dataPackage->updateData);
						this->eventManager->onGuildStickersUpdateEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_INTEGRATIONS_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						this->eventManager->onGuildIntegrationsUpdateEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_MEMBER_ADD") {
						std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberAddData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildMember.guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						parseObject(payload["d"], dataPackage->guildMember);
						dataPackage->discordCoreClient = this->discordCoreClient;
						this->eventManager->onGuildMemberAddEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_MEMBER_REMOVE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
							DiscordCoreAPI::User newData{};
							parseObject(payload["d"]["user"], newData);
							dataPackage->user = newData;
						}
						this->eventManager->onGuildMemberRemoveEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_MEMBER_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildMemberNew.guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("user") && payload["d"]["user"].contains("id") && !payload["d"]["user"]["id"].is_null()) {
							dataPackage->guildMemberOld = DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync(
								{ .guildMemberId = stoull(payload["d"]["user"]["id"].get<std::string>()), .guildId = stoull(payload["d"]["guild_id"].get<std::string>()) })
																.get();
							dataPackage->guildMemberNew = dataPackage->guildMemberOld;
						}
						parseObject(payload["d"], dataPackage->guildMemberNew);
						this->eventManager->onGuildMemberUpdateEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_MEMBERS_CHUNK") {
						std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
						parseObject(payload["d"], dataPackage->chunkEventData);
						this->eventManager->onGuildMembersChunkEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_ROLE_CREATE") {
						std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>() };
						if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
							parseObject(payload["d"]["role"], dataPackage->role);
						}
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						this->eventManager->onRoleCreationEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_ROLE_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleUpdateData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						parseObject(payload["d"]["role"], dataPackage->roleNew);
						if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
							dataPackage->roleOld =
								DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role"]["id"].get<std::string>()) })
									.get();
							dataPackage->roleNew = dataPackage->roleOld;
						}
						this->eventManager->onRoleUpdateEvent(*dataPackage);
					} else if (payload["t"] == "GUILD_ROLE_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleDeletionData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("role_id") && !payload["d"]["role_id"].is_null()) {
							dataPackage->roleOld =
								DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role_id"].get<std::string>()) }).get();
						}
						this->eventManager->onRoleDeletionEvent(*dataPackage);
					} else if (payload["t"] == "INTEGRATION_CREATE") {
						std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
							parseObject(payload["d"]["integration"], dataPackage->integrationData);
						}
						this->eventManager->onIntegrationCreationEvent(*dataPackage);
					} else if (payload["t"] == "INTEGRATION_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
							parseObject(payload["d"]["integration"], dataPackage->integrationData);
						}
						this->eventManager->onIntegrationUpdateEvent(*dataPackage);
					} else if (payload["t"] == "INTEGRATION_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("application_id") && !payload["d"]["application_id"].is_null()) {
							dataPackage->applicationId = stoull(payload["d"]["application_id"].get<std::string>());
						}
						if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
							dataPackage->id = stoull(payload["d"]["id"].get<std::string>());
						}
						this->eventManager->onIntegrationDeletionEvent(*dataPackage);
					} else if (payload["t"] == "INTERACTION_CREATE") {
						std::unique_ptr<DiscordCoreAPI::InteractionData> interactionData{ std::make_unique<DiscordCoreAPI::InteractionData>() };
						parseObject(payload["d"], *interactionData);
						std::unique_ptr<DiscordCoreAPI::InputEventData> eventData{ std::make_unique<DiscordCoreAPI::InputEventData>(*interactionData) };
						if (interactionData->type == DiscordCoreAPI::InteractionType::Application_Command) {
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							*eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>(*eventData) };
							this->commandController->checkForAndRunCommand(*commandData);
							this->eventManager->onInteractionCreationEvent(*dataPackage);
							std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							this->eventManager->onInputEventCreationEvent(*eventCreationData);
						} else if (interactionData->type == DiscordCoreAPI::InteractionType::Message_Component) {
							if (interactionData->data.componentData.componentType == DiscordCoreAPI::ComponentType::Button) {
								eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
								*eventData->interactionData = *interactionData;
								std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
								dataPackage->interactionData = *interactionData;
								if (DiscordCoreAPI::ButtonCollector::buttonInteractionBufferMap.contains(
										std::to_string(eventData->getChannelId()) + std::to_string(eventData->getMessageId()))) {
									DiscordCoreAPI::ButtonCollector::buttonInteractionBufferMap[std::to_string(eventData->getChannelId()) +
										std::to_string(eventData->getMessageId())]
										->send(*interactionData);
								}
								this->eventManager->onInteractionCreationEvent(*dataPackage);
							} else if (interactionData->data.componentData.componentType == DiscordCoreAPI::ComponentType::SelectMenu) {
								eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
								*eventData->interactionData = *interactionData;
								std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
								dataPackage->interactionData = *interactionData;
								if (DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBufferMap.contains(
										std::to_string(eventData->getChannelId()) + std::to_string(eventData->getMessageId()))) {
									DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBufferMap[std::to_string(eventData->getChannelId()) +
										std::to_string(eventData->getMessageId())]
										->send(*interactionData);
								}
								this->eventManager->onInteractionCreationEvent(*dataPackage);
							}
						} else if (interactionData->type == DiscordCoreAPI::InteractionType::Modal_Submit) {
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							*eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							if (DiscordCoreAPI::ModalCollector::modalInteractionBufferMap.contains(std::to_string(eventData->getChannelId()))) {
								DiscordCoreAPI::ModalCollector::modalInteractionBufferMap[std::to_string(eventData->getChannelId())]->send(eventData->getInteractionData());
							}
							this->eventManager->onInteractionCreationEvent(*dataPackage);
						} else if (interactionData->type == DiscordCoreAPI::InteractionType::Application_Command_Autocomplete) {
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							*eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnAutoCompleteEntryData> autocompleteEntryData{ std::make_unique<DiscordCoreAPI::OnAutoCompleteEntryData>() };
							autocompleteEntryData->inputEvent = *eventData;
							this->eventManager->onAutoCompleteEntryEvent(*autocompleteEntryData);
							this->eventManager->onInteractionCreationEvent(*dataPackage);
						}
					} else if (payload["t"] == "INVITE_CREATE") {
						std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
						parseObject(payload["d"], dataPackage->invite);
						this->eventManager->onInviteCreationEvent(*dataPackage);
					} else if (payload["t"] == "INVITE_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteDeletionData>() };
						if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
							dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
						}
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("code") && !payload["d"]["code"].is_null()) {
							dataPackage->code = payload["d"]["code"];
						}
						this->eventManager->onInviteDeletionEvent(*dataPackage);
					} else if (payload["t"] == "MESSAGE_CREATE") {
						std::unique_ptr<DiscordCoreAPI::Message> message{ std::make_unique<DiscordCoreAPI::Message>() };
						parseObject(payload["d"], *message);
						std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageCreationData>() };
						dataPackage->message = *message;
						for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
							value->send(*message);
						}
						this->eventManager->onMessageCreationEvent(*dataPackage);
						if (message->content.find("!registerapplicationcommands") != std::string::npos) {
							std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>() };
							commandData->commandName = "registerapplicationcommands";
							DiscordCoreAPI::CommandData commandDataNew = *commandData;
							this->commandController->checkForAndRunCommand(commandDataNew);
						}
						std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
					} else if (payload["t"] == "MESSAGE_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
						parseObject(payload["d"], dataPackage->messageNew);
						for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
							value->send(dataPackage->messageNew);
						}
						this->eventManager->onMessageUpdateEvent(*dataPackage);
					} else if (payload["t"] == "MESSAGE_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeletionData>() };
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
							dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
						}
						if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
							dataPackage->messageId = stoull(payload["d"]["id"].get<std::string>());
						}
						this->eventManager->onMessageDeletionEvent(*dataPackage);
					} else if (payload["t"] == "MESSAGE_DELETE_BULK") {
						std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
						if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
							dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
						}
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("ids") && !payload["d"]["ids"].is_null()) {
							for (auto& value: payload["d"]["ids"]) {
								dataPackage->ids.push_back(stoull(value.get<std::string>()));
							}
						}
						this->eventManager->onMessageDeleteBulkEvent(*dataPackage);
					} else if (payload["t"] == "MESSAGE_REACTION_ADD") {
						std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
						parseObject(payload["d"], dataPackage->reaction);
						this->eventManager->onReactionAddEvent(*dataPackage);
					} else if (payload["t"] == "MESSAGE_REACTION_REMOVE") {
						std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
						parseObject(payload["d"], dataPackage->reactionRemoveData);
						this->eventManager->onReactionRemoveEvent(*dataPackage);
					} else if (payload["t"] == "MESSAGE_REACTION_REMOVE_ALL") {
						std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>() };
						if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
							dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
						}
						if (payload["d"].contains("message_id") && !payload["d"]["message_id"].is_null()) {
							dataPackage->messageId = stoull(payload["d"]["message_id"].get<std::string>());
						}
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						this->eventManager->onReactionRemoveAllEvent(*dataPackage);
					} else if (payload["t"] == "MESSAGE_REACTION_REMOVE_EMOJI") {
						std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>() };
						if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
							dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
						}
						if (payload["d"].contains("message_id") && !payload["d"]["message_id"].is_null()) {
							dataPackage->messageId = stoull(payload["d"]["message_id"].get<std::string>());
						}
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						parseObject(payload["d"]["emoji"], dataPackage->emoji);
						this->eventManager->onReactionRemoveEmojiEvent(*dataPackage);
					} else if (payload["t"] == "PRESENCE_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
						parseObject(payload["d"], dataPackage->presenceData);
						this->eventManager->onPresenceUpdateEvent(*dataPackage);
					} else if (payload["t"] == "STAGE_INSTANCE_CREATE") {
						std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
						parseObject(payload["d"], dataPackage->stageInstance);
						this->eventManager->onStageInstanceCreationEvent(*dataPackage);
					} else if (payload["t"] == "STAGE_INSTANCE_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
						parseObject(payload["d"], dataPackage->stageInstance);
						this->eventManager->onStageInstanceUpdateEvent(*dataPackage);
					} else if (payload["t"] == "STAGE_INSTANCE_DELETE") {
						std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
						parseObject(payload["d"], dataPackage->stageInstance);
						this->eventManager->onStageInstanceDeletionEvent(*dataPackage);
					} else if (payload["t"] == "TYPING_START") {
						std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
						parseObject(payload["d"], dataPackage->typingStartData);
						this->eventManager->onTypingStartEvent(*dataPackage);
					} else if (payload["t"] == "USER_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
						DiscordCoreAPI::User newData{};
						parseObject(payload["d"]["user"], newData);
						dataPackage->userNew = newData;
						dataPackage->userOld = DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get();
						this->eventManager->onUserUpdateEvent(*dataPackage);
					} else if (payload["t"] == "VOICE_STATE_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
						parseObject(payload["d"], dataPackage->voiceStateData);
						this->eventManager->onVoiceStateUpdateEvent(*dataPackage);
					} else if (payload["t"] == "VOICE_SERVER_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>() };
						if (payload["d"].contains("endpoint") && !payload["d"]["endpoint"].is_null()) {
							dataPackage->endpoint = payload["d"]["endpoint"];
						}
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						if (payload["d"].contains("token") && !payload["d"]["token"].is_null()) {
							dataPackage->token = payload["d"]["token"];
						}
						this->eventManager->onVoiceServerUpdateEvent(*dataPackage);
					} else if (payload["t"] == "WEBHOOKS_UPDATE") {
						std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
						if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
							dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
						}
						if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
							dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
						}
						this->eventManager->onWebhookUpdateEvent(*dataPackage);
					}
				}
				if (this->doWePrintSuccessMessages && !payload.is_null()) {
					std::lock_guard<std::mutex> theLock{ this->discordCoreClient->coutMutex };
					std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from WebSocket " + this->theClients[theIndex]->shard.dump() + std::string(": ")
							  << payload.dump() << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				return;
			}
			catch (...) {
				if (this->doWePrintErrorMessages) {
					DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
				}
				this->onClosed(*this->theClients[theIndex]);
				return;
			}
		}
	}
	
	void BaseSocketAgent::sendCloseFrame(int32_t theIndex) noexcept {
		if (this->theClients.contains(theIndex)) {
			std::string theString{};
			theString.push_back(static_cast<int8_t>(WebSocketOpCode::Op_Close) | static_cast<int8_t>(webSocketFinishBit));
			theString.push_back(0);
			theString.push_back(static_cast<int8_t>(static_cast<uint16_t>(1000) >> 8));
			theString.push_back(static_cast<int8_t>(1000 & 0xff));
			if (this->theClients[theIndex]) {
				this->theClients[theIndex]->writeData(theString);
				try {
					WebSocketSSLShard::processIO(this->theClients);
				} catch (ProcessingError&) {
					if (this->doWePrintErrorMessages) {
						DiscordCoreAPI::reportException("BaseSocketAgent::sendCloseFrame()");
					}
				}
			}
		}
	}

	void BaseSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			while (!theToken.stop_requested() && !this->doWeQuit->load()) {
				if (this->connections.size() > 0) {
					this->internalConnect();
				}
				for (auto& [key, value]: this->theClients) {
					try {
						WebSocketSSLShard::processIO(this->theClients);
					} catch (...) {
						if (this->doWePrintErrorMessages) {
							DiscordCoreAPI::reportException("BaseSocketAgent::run()");
						}
					}
					if (this->theClients.contains(key) &&value->inputBuffer.size() > 0) {
						this->parseHeadersAndMessage(*value);
					}
					if (this->theClients.contains(key) && value->processedMessages.size() > 0) {
						this->onMessageReceived(key);
					}
					if (this->theClients.contains(key)) {
						this->checkForAndSendHeartBeat(key);
						if (this->theClients[key] != nullptr && this->heartbeatInterval != 0 && !this->theClients[key]->areWeHeartBeating) {
							this->theClients[key]->areWeHeartBeating = true;
							this->theClients[key]->heartBeatStopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
						}
					}
				}
				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			}
		}
	}

	void BaseSocketAgent::internalConnect() noexcept {
		try {
			if (this->discordCoreClient->theStopWatch.hasTimePassed()) {
				if (this->connections.size() > 0) {
					DiscordCoreAPI::ConnectionPackage reconnectionData = this->connections.front();
					this->connections.pop();
					reconnectionData.currentReconnectionDepth += 1;
					this->theClients[reconnectionData.currentShard] = std::make_unique<WebSocketSSLShard>(&this->connections, this->currentBaseSocketAgent,
						reconnectionData.currentShard, this->discordCoreClient->shardingOptions.totalNumberOfShards, this->doWePrintErrorMessages);
					this->theClients[reconnectionData.currentShard]->currentRecursionDepth = reconnectionData.currentReconnectionDepth;
					this->theClients[reconnectionData.currentShard]->currentBaseSocketAgent = reconnectionData.currentBaseSocketAgent;
					this->theClients[reconnectionData.currentShard]->lastNumberReceived = reconnectionData.lastNumberReceived;
					this->theClients[reconnectionData.currentShard]->areWeResuming = reconnectionData.areWeResuming;
					this->theClients[reconnectionData.currentShard]->sessionId = reconnectionData.sessionId;

					try {
						this->theClients[reconnectionData.currentShard]->connect(this->baseUrl, "443");
					} catch (...) {
						if (this->doWePrintErrorMessages) {
							DiscordCoreAPI::reportException("BaseSocketAgent::internalConnect()");
						}
						if (reconnectionData.currentReconnectionDepth >= this->maxReconnectTries) {
							this->doWeQuit->store(true);
						} else {
							this->connections.push(reconnectionData);
						}
						return;
					}
					
					std::string sendString{};
					if (this->theFormat == DiscordCoreAPI::TextFormat::Etf) {
						sendString = "GET /?v=10&encoding=etf HTTP/1.1\r\nHost: " + this->baseUrl +
							"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
							DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
					} else {
						sendString = "GET /?v=10&encoding=json HTTP/1.1\r\nHost: " + this->baseUrl +
							"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
							DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
					}

					this->sendMessage(sendString, reconnectionData.currentShard);
					int32_t currentDepth{ 0 };
					while (true) {
						currentDepth += 1;
						if (this->theClients[reconnectionData.currentShard]->inputBuffer.size() > 0) {
							this->parseHeadersAndMessage(*this->theClients[reconnectionData.currentShard]);
						}
						if (this->theClients[reconnectionData.currentShard]->theState == WebSocketState::Connected) {
							break;
						}
						try {
							auto thePtr = this->theClients[reconnectionData.currentShard].get();
							thePtr->processIO(this->theClients, 10000);
						} catch (...) {
							if (this->doWePrintErrorMessages) {
								DiscordCoreAPI::reportException("BaseSocketAgent::internalConnect()");
							}
							if (reconnectionData.currentReconnectionDepth >= this->maxReconnectTries) {
								this->doWeQuit->store(true);
							} else {
								this->connections.push(reconnectionData);
							}
							return;
						}
						std::this_thread::sleep_for(1ms);
						if (currentDepth >= 5000) {
							this->theClients.erase(reconnectionData.currentShard);
							this->discordCoreClient->theStopWatch.resetTimer();
							return;
						}
						if (!this->theClients.contains(reconnectionData.currentShard)) {
							if (reconnectionData.currentReconnectionDepth >= this->maxReconnectTries) {
								this->doWeQuit->store(true);
							}
							else {
								this->connections.push(reconnectionData);
							}
						}
					}
				}
				this->discordCoreClient->theStopWatch.resetTimer();
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::internalConnect()");
			}
		}
	}
	
	BaseSocketAgent::~BaseSocketAgent() noexcept {
		this->theTask->request_stop();
		if (this->theTask->joinable()) {
			this->theTask->join();
		}
	}

	VoiceSocketAgent::VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew,int32_t theIndex, bool printMessagesNew) noexcept {
		this->baseSocketAgent = baseBaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->doWePrintSuccessMessages = baseBaseSocketAgentNew->doWePrintSuccessMessages;
		this->doWePrintErrorMessages = baseBaseSocketAgentNew->doWePrintErrorMessages;
		this->baseSocketAgent->voiceConnectionDataBufferMap[std::to_string(this->voiceConnectInitData.guildId)] = &this->voiceConnectionDataBuffer;
		this->baseSocketAgent->getVoiceConnectionData(this->voiceConnectInitData, theIndex);
		this->areWeConnected.reset();
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
	}

	void VoiceSocketAgent::sendMessage(const std::vector<uint8_t>& dataToSend) noexcept {
		try {
			std::string newString{};
			newString.insert(newString.begin(), dataToSend.begin(), dataToSend.end());
			if (this->doWePrintSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
			}
			std::string header{};
			this->createHeader(header, newString.size(), this->dataOpcode);
			std::string theVectorNew{};
			theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
			theVectorNew.insert(theVectorNew.begin() + header.size(), newString.begin(), newString.end());
			if (this->theClients[3]) {
				this->theClients[3]->writeData(theVectorNew);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::onClosed(WebSocketSSLShard& theShard) noexcept {
		std::this_thread::sleep_for(500ms);
		if (this->doWePrintErrorMessages) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket " + theShard.shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(theShard.closeCode)
					  << DiscordCoreAPI::reset() << std::endl
					  << std::endl;
		}
		this->theClients.erase(theShard.shard[0]);
	}

	void VoiceSocketAgent::sendVoiceData(std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Please specify voice data to send" << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			} else {
				this->voiceSocket->writeData(responseData);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendVoiceData()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendMessage(std::string& dataToSend) noexcept {
		try {
			if (this->doWePrintSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << std::endl << dataToSend << DiscordCoreAPI::reset();
			}
			if (this->theClients[3]) {
				this->theClients[3]->writeData(dataToSend);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::onClosedExternal() noexcept {
		this->doWeReconnect.store(true);
	}

	void VoiceSocketAgent::createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept {
		try {
			outBuffer.push_back(static_cast<uint8_t>(opCode) | webSocketFinishBit);

			int32_t indexCount{ 0 };
			if (sendLength <= webSocketMaxPayloadLengthSmall) {
				outBuffer.push_back(static_cast<uint8_t>(sendLength));
				indexCount = 0;
			} else if (sendLength <= webSocketMaxPayloadLengthLarge) {
				outBuffer.push_back(webSocketPayloadLengthMagicLarge);
				indexCount = 2;
			} else {
				outBuffer.push_back(webSocketPayloadLengthMagicHuge);
				indexCount = 8;
			}
			for (int32_t x = indexCount - 1; x >= 0; x--) {
				outBuffer.push_back(static_cast<uint8_t>(sendLength >> x * 8));
			}

			outBuffer[1] |= webSocketMaskBit;
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::createHeader()");
			}
		}
	}

	void VoiceSocketAgent::parseHeadersAndMessage(WebSocketSSLShard& theShard) noexcept {
		if (theShard.theState == WebSocketState::Connecting) {
			std::string newVector{};
			if (theShard.inputBuffer.find("\r\n\r\n") != std::string::npos) {
				std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
				newVector.erase(0, newVector.find("\r\n\r\n") + 4);
				theShard.inputBuffer.clear();
				theShard.inputBuffer.insert(theShard.inputBuffer.end(), newVector.begin(), newVector.end());
				theShard.theState = WebSocketState::Connected;
			}
		}
		if (theShard.inputBuffer.size() < 4) {
			return;
		}
		theShard.dataOpCode = static_cast<WebSocketOpCode>(theShard.inputBuffer[0] & ~webSocketFinishBit);
		switch (theShard.dataOpCode) {
			case WebSocketOpCode::Op_Continuation:
				[[fallthrough]];
			case WebSocketOpCode::Op_Text:
				[[fallthrough]];
			case WebSocketOpCode::Op_Binary:
				[[fallthrough]];
			case WebSocketOpCode::Op_Ping:
				[[fallthrough]];
			case WebSocketOpCode::Op_Pong: {
				uint8_t length01 = theShard.inputBuffer[1];
				theShard.messageOffset = 2;
				if (length01 & webSocketMaskBit) {
					return;
				}
				theShard.messageLength = length01;
				if (length01 == webSocketPayloadLengthMagicLarge) {
					if (theShard.inputBuffer.size() < 8) {
						return;
					}
					uint8_t length03 = theShard.inputBuffer[2];
					uint8_t length04 = theShard.inputBuffer[3];
					theShard.messageLength = static_cast<uint64_t>((length03 << 8) | length04);
					theShard.messageOffset += 2;
				} else if (length01 == webSocketPayloadLengthMagicHuge) {
					if (theShard.inputBuffer.size() < 10) {
						return;
					}
					theShard.messageLength = 0;
					for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
						uint8_t lengthNew = static_cast<uint8_t>(theShard.inputBuffer[x]);
						theShard.messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
					}
					theShard.messageOffset += 8;
				}
				if (theShard.inputBuffer.size() < static_cast<uint64_t>(theShard.messageOffset) + static_cast<uint64_t>(theShard.messageLength)) {
					return;
				} else {
					std::string finalMessage{};
					finalMessage.insert(finalMessage.begin(), theShard.inputBuffer.begin() + theShard.messageOffset,
						theShard.inputBuffer.begin() + theShard.messageOffset + theShard.messageLength);
					theShard.processedMessages.push(finalMessage);
					theShard.inputBuffer.erase(theShard.inputBuffer.begin(), theShard.inputBuffer.begin() + theShard.messageOffset + theShard.messageLength);
					return;
				}
			}
			case WebSocketOpCode::Op_Close: {
				uint16_t close = theShard.inputBuffer[2] & 0xff;
				close <<= 8;
				close |= theShard.inputBuffer[3] & 0xff;
				theShard.closeCode = static_cast<WebSocketCloseCode>(close);
				theShard.inputBuffer.erase(theShard.inputBuffer.begin(), theShard.inputBuffer.begin() + 4);
				this->onClosed(*this->theClients[theShard.shard[0]]);
				return;
			}
			default: {
				return;
			}
		}
	}

	void VoiceSocketAgent::onMessageReceived(const std::string& theMessage) noexcept {
		try {
			nlohmann::json payload = payload.parse(theMessage);
			if (this->doWePrintSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from Voice WebSocket: " << theMessage << DiscordCoreAPI::reset() << std::endl << std::endl;
			}
			if (payload.contains("op") && !payload["op"].is_null()) {
				if (payload["op"] == 6) {
					this->haveWeReceivedHeartbeatAck = true;
				};
				if (payload["op"] == 2) {
					this->voiceConnectionData.audioSSRC = payload["d"]["ssrc"].get<uint32_t>();
					this->voiceConnectionData.voiceIp = payload["d"]["ip"].get<std::string>();
					this->voiceConnectionData.voicePort = std::to_string(payload["d"]["port"].get<int64_t>());
					for (auto& value: payload["d"]["modes"]) {
						if (value == "xsalsa20_poly1305") {
							this->voiceConnectionData.voiceEncryptionMode = value;
						}
					}
					this->voiceConnect();
					this->collectExternalIP();
					std::vector<uint8_t> protocolPayloadSelectString =
						JSONIFY(this->voiceConnectionData.voicePort, this->voiceConnectionData.externalIp, this->voiceConnectionData.voiceEncryptionMode, 0);
					this->sendMessage(protocolPayloadSelectString);
				}
				if (payload["op"] == 4) {
					for (uint32_t x = 0; x < payload["d"]["secret_key"].size(); x += 1) {
						this->voiceConnectionData.secretKey.push_back(payload["d"]["secret_key"][x].get<uint8_t>());
					}
					this->areWeConnected.set();
				}
				if (payload["op"] == 9) {
				};
				if (payload["op"] == 8) {
					if (payload["d"].contains("heartbeat_interval")) {
						this->heartbeatInterval = static_cast<int32_t>(payload["d"]["heartbeat_interval"].get<float>());
						this->areWeHeartBeating = false;
					}
					this->haveWeReceivedHeartbeatAck = true;
					std::vector<uint8_t> identifyPayload = JSONIFY(this->voiceConnectionData, this->voiceConnectInitData);
					this->sendMessage(identifyPayload);
				}
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::onMessageReceived()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			this->connect();
			DiscordCoreAPI::StopWatch stopWatch{ 0ms };
			while (!theToken.stop_requested()) {
				if (this->heartbeatInterval != 0 && !this->areWeHeartBeating) {
					this->areWeHeartBeating = true;
					stopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
				}
				if (stopWatch.hasTimePassed() && this->areWeHeartBeating) {
					stopWatch.resetTimer();
					this->sendHeartBeat();
				}
				try {
					WebSocketSSLShard::processIO(this->theClients);
				} catch (...) {
					if (this->doWePrintErrorMessages) {
						DiscordCoreAPI::reportException("VoiceSocketAgent::run()");
					}
				}
				if (this->theClients.contains(3)) {
					this->parseHeadersAndMessage(*this->theClients[3]);
					if (this->theClients.contains(3) && this->theClients[3]->processedMessages.size() > 0) {
						this->onMessageReceived(this->theClients[3]->processedMessages.front());
						this->theClients[3]->processedMessages.pop();
					}
					if (this->voiceSocket) {
						this->voiceSocket->readData(true);
					}
				} else {
					this->onClosedExternal();
				}
				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::run()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::collectExternalIP() noexcept {
		try {
			std::string packet{};
			packet.resize(74);
			uint16_t val1601{ 0x01 };
			uint16_t val1602{ 70 };
			packet[0] = static_cast<uint8_t>(val1601 >> 8);
			packet[1] = static_cast<uint8_t>(val1601 >> 0);
			packet[2] = static_cast<uint8_t>(val1602 >> 8);
			packet[3] = static_cast<uint8_t>(val1602 >> 0);
			packet[4] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 24);
			packet[5] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 16);
			packet[6] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 8);
			packet[7] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC);
			this->voiceSocket->writeData(packet);
			while (this->voiceSocket->getInputBuffer().size() < 74) {
				this->voiceSocket->readData(false);
				std::this_thread::sleep_for(1ms);
			}
			std::string message{};
			message.insert(message.begin(), this->voiceSocket->getInputBuffer().begin() + 8, this->voiceSocket->getInputBuffer().begin() + 64);
			if (message.find('\u0000') != std::string::npos) {
				message = message.substr(0, message.find('\u0000', 5));
			}
			this->voiceSocket->getInputBuffer().clear();
			this->voiceConnectionData.externalIp = message;
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::collectExternalIP()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendHeartBeat() noexcept {
		try {
			if (this->haveWeReceivedHeartbeatAck) {
				std::vector<uint8_t> heartbeatPayload = JSONIFY(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				this->sendMessage(heartbeatPayload);
				this->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosedExternal();
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendHeartBeat()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::voiceConnect() noexcept {
		try {
			this->voiceSocket = std::make_unique<DatagramSocketSSLClient>();
			this->voiceSocket->connect(this->voiceConnectionData.voiceIp, this->voiceConnectionData.voicePort);
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::voiceConnect()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::connect() noexcept {
		try {
			DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 20000);
			this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
			auto theClient = std::make_unique<WebSocketSSLShard>(nullptr, 0, 3, 0, this->doWePrintErrorMessages);
			theClient->connect(this->baseUrl, "443");
			this->theClients[3] = std::move(theClient);
			std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->sendMessage(sendVector);
			try {
				WebSocketSSLShard::processIO(this->theClients);
			} catch (...) {
				if (this->doWePrintErrorMessages) {
					DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
				}
			}
			int32_t currentDepth{ 0 };
			while (true) {
				currentDepth += 1;
				if (this->theClients[3]->inputBuffer.size() > 0) {
					this->parseHeadersAndMessage(*this->theClients[3]);
				}
				if (this->theClients[3]->theState == WebSocketState::Connected) {
					break;
				}
				try {
					WebSocketSSLShard::processIO(this->theClients);
				} catch (...) {
					if (this->doWePrintErrorMessages) {
						DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
					}
				}
				std::this_thread::sleep_for(1ms);
				if (currentDepth >= 5000) {
					this->theClients.erase(3);
				}
				if (!this->theClients.contains(3)) {
					if (this->currentReconnectionTries>= this->maxReconnectionTries) {
						return;
					}
				}
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
			}
			this->onClosedExternal();
		}
	}

	VoiceSocketAgent::~VoiceSocketAgent() noexcept {
		this->theTask->request_stop();
		if (this->theTask->joinable()) {
			this->theTask->join();
			this->theTask.reset(nullptr);
		}
	};
}