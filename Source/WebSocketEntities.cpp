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

namespace DiscordCoreInternal {

	constexpr uint16_t webSocketMaxPayloadLengthLarge{ 65535u };
	constexpr uint8_t webSocketPayloadLengthMagicLarge{ 126u };
	constexpr uint8_t webSocketPayloadLengthMagicHuge{ 127u };
	constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2u };
	constexpr uint8_t webSocketMaxPayloadLengthSmall{ 125u };
	constexpr uint8_t webSocketFinishBit{ (1u << 7u) };
	constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

	WSMessageCollector::WSMessageCollector(bool doWePrintErrorMessagesNew, std::unordered_map<SOCKET, std::unique_ptr<WebSocketSSLShard>>* theClientsNew) {
		this->doWePrintErrorMessages = doWePrintErrorMessagesNew;
		this->theClients = theClientsNew;
	};

	std::unordered_map<SOCKET,WSMessageCollectorReturnData> WSMessageCollector::collectFinalMessage() noexcept {
		std::unordered_map<SOCKET, WSMessageCollectorReturnData> theMapToReturn{};
		for (auto& [key, value]: *this->theClients) {
			WSMessageCollectorReturnData theMessage{};
			if (this->finalMessages[key].size() > 0) {
				theMessage = this->finalMessages[key].front();
				this->finalMessages[key].pop();
				this->theState[key] = WSMessageCollectorState::Initializing;
				theMapToReturn[key] = theMessage;
			}
		}
		return theMapToReturn;
	}

	bool WSMessageCollector::runMessageCollector() noexcept {
		if (this->currentRecursionDepth == this->maxRecursionDepth) {
			this->currentRecursionDepth = 0;
			return true;
		}
		for (auto& [key, value]: *this->theClients) {
			switch (this->theState[key]) {
				case WSMessageCollectorState::Connecting: {
					auto returnValue = this->collectData();
					if (!returnValue) {
						return returnValue;
					}
					return this->parseConnectionHeader();
				}
				case WSMessageCollectorState::Initializing: {
					this->dataOpCodes[key] = WebSocketOpCode::Op_Binary;
					this->theState[key] = WSMessageCollectorState::Collecting;
					return this->runMessageCollector();
				}
				case WSMessageCollectorState::Collecting: {
					return this->collectData();
				}
				case WSMessageCollectorState::Parsing: {
					return this->parseHeaderAndMessage();
				}
				case WSMessageCollectorState::Serving: {
					this->theState[key] = WSMessageCollectorState::Initializing;
					return true;
				}
				default: {
					return true;
				}
			}
		}
		return true;
	}

	bool WSMessageCollector::parseConnectionHeader() noexcept {
		for (auto& [key, value]: *this->theClients) {
			std::string newVector = this->currentMessage[key];
			if (newVector.find("\r\n\r\n") != std::string::npos) {
				std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
				newVector.erase(0, newVector.find("\r\n\r\n") + 4);
				this->theState[key] = WSMessageCollectorState::Initializing;
				this->currentMessage[key].clear();
				this->currentMessage[key].insert(this->currentMessage[key].end(), newVector.begin(), newVector.end());
				return this->runMessageCollector();
			} else {
				this->theState[key] = WSMessageCollectorState::Collecting;
				this->currentRecursionDepth += 1;
				return this->runMessageCollector();
			}
		}
		return true;
	}

	bool WSMessageCollector::parseHeaderAndMessage() noexcept {
		for (auto& [key, value]: *this->theClients) {
			if (this->currentMessage[key].size() < 4) {
				this->theState[key] = WSMessageCollectorState::Collecting;
				this->currentRecursionDepth += 1;
				return this->runMessageCollector();
			} else {
				this->dataOpCodes[key] = static_cast<WebSocketOpCode>(this->currentMessage[key][0] & ~webSocketFinishBit);
				switch (this->dataOpCodes[key]) {
					case WebSocketOpCode::Op_Continuation:
						[[fallthrough]];
					case WebSocketOpCode::Op_Text:
						[[fallthrough]];
					case WebSocketOpCode::Op_Binary:
						[[fallthrough]];
					case WebSocketOpCode::Op_Ping:
						[[fallthrough]];
					case WebSocketOpCode::Op_Pong: {
						uint8_t length01 = this->currentMessage[key][1];
						this->messageOffset[key] = 2;
						if (length01 & webSocketMaskBit) {
							this->theState[key] = WSMessageCollectorState::Collecting;
							this->currentRecursionDepth += 1;
							return this->runMessageCollector();
						}
						this->messageLength[key] = length01;
						if (length01 == webSocketPayloadLengthMagicLarge) {
							if (this->currentMessage[key].size() < 8) {
								this->theState[key] = WSMessageCollectorState::Collecting;
								this->currentRecursionDepth += 1;
								return this->runMessageCollector();
							}
							uint8_t length03 = this->currentMessage[key][2];
							uint8_t length04 = this->currentMessage[key][3];
							this->messageLength[key] = static_cast<uint64_t>((length03 << 8) | length04);
							this->messageOffset[key] += 2;
						} else if (length01 == webSocketPayloadLengthMagicHuge) {
							if (this->currentMessage[key].size() < 10) {
								this->theState[key] = WSMessageCollectorState::Collecting;
								this->currentRecursionDepth += 1;
								return this->runMessageCollector();
							}
							this->messageLength[key] = 0;
							for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
								uint8_t lengthNew = static_cast<uint8_t>(this->currentMessage[key][x]);
								this->messageLength[key] |= static_cast<uint64_t>((lengthNew & 0xff) << shift);
							}
							this->messageOffset[key] += 8;
						}
						if (this->currentMessage[key].size() < static_cast<uint64_t>(this->messageOffset[key] + this->messageLength[key])) {
							this->theState[key] = WSMessageCollectorState::Collecting;
							this->currentRecursionDepth += 1;
							return this->runMessageCollector();
						} else {
							WSMessageCollectorReturnData returnData{};
							returnData.theMessage.insert(returnData.theMessage.begin(), this->currentMessage[key].begin() + this->messageOffset[key],
								this->currentMessage[key].begin() + this->messageOffset[key] + this->messageLength[key]);
							returnData.opCode = this->dataOpCodes[key];
							this->finalMessages[key].push(returnData);
							this->currentMessage[key].erase(this->currentMessage[key].begin(),
								this->currentMessage[key].begin() + this->messageOffset[key] + this->messageLength[key]);
							this->theState[key] = WSMessageCollectorState::Serving;
							return this->runMessageCollector();
						}
						this->theState[key] = WSMessageCollectorState::Collecting;
						this->currentRecursionDepth += 1;
						return this->runMessageCollector();
					}
					case WebSocketOpCode::Op_Close: {
						uint16_t close = this->currentMessage[key][2] & 0xff;
						close <<= 8;
						close |= this->currentMessage[key][3] & 0xff;
						while (this->finalMessages[key].size() > 0) {
							this->finalMessages[key].pop();
						}
						WSMessageCollectorReturnData returnData{};
						returnData.closeCode = static_cast<WebSocketCloseCode>(close);
						returnData.opCode = this->dataOpCodes[key];
						this->finalMessages[key].push(returnData);
						this->currentMessage[key].clear();
						this->theState[key] = WSMessageCollectorState::Serving;
						return false;
					}
					default: {
						this->theState[key] = WSMessageCollectorState::Collecting;
						this->currentRecursionDepth += 1;
						return true;
					}
				}
			}
			this->theState[key] = WSMessageCollectorState::Collecting;
			return true;
		}
		return true;
	}

	bool WSMessageCollector::collectData() noexcept {
		try {
			WebSocketSSLShard::processIO(*this->theClients);
			for (auto& [key, value]: *this->theClients) {
				if (value->areWeConnected.load()) {
					if (value != nullptr) {
						std::string newMessage = value->getInputBuffer();
						this->theState[key] = WSMessageCollectorState::Parsing;
						this->currentMessage[key].insert(this->currentMessage[key].end(), newMessage.begin(), newMessage.end());
						return this->runMessageCollector();
					}
				} else {
					return false;
				}
			}
		} catch (ProcessingError&) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("WSMessageCollector::collectData()");
			}
			return false;
		}
		return true;
	}

	BaseSocketAgent::BaseSocketAgent(const std::string& botTokenNew, const std::string& baseUrlNew, DiscordCoreAPI::EventManager* eventManagerNew,
		DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew,
		DiscordCoreAPI::CommandController* commandControllerNew, std::atomic_bool* doWeQuitNew, bool doWePrintSuccessMessagesNew, bool doWePrintErrorMessagesNew, int32_t currentBaseSocketAgentNew) noexcept {
		this->doWePrintSuccessMessages = doWePrintSuccessMessagesNew;
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->doWePrintErrorMessages = doWePrintErrorMessagesNew;
		this->commandController = commandControllerNew;
		this->discordCoreClient = discordCoreClientNew;
		this->eventManager = eventManagerNew;
		this->botToken = botTokenNew;
		this->doWeQuit = doWeQuitNew;
		this->theFormat = this->discordCoreClient->theFormat;
		this->messageCollector = WSMessageCollector{ this->doWePrintErrorMessages, &this->theClients };
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
			DiscordCoreAPI::StopWatch stopWatch{ 5500ms };
			while (!this->theClients[theIndex]->areWeConnected.load() && !(dataToSend.contains("op") && (dataToSend["op"] == 2 || dataToSend["op"] == 6))) {
				if (stopWatch.hasTimePassed()) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
			std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
			if (this->doWePrintSuccessMessages) {
				std::lock_guard<std::mutex> theLock{ this->discordCoreClient->coutMutex };
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->theClients[theIndex]->shard.dump() + std::string("'s Message: ")
						  << dataToSend.dump() << DiscordCoreAPI::reset() << std::endl
						  << std::endl;
			}
			std::string theVector{};
			std::string header{};
			if (this->theFormat == DiscordCoreAPI::TextFormat::Etf) {
				theVector = this->erlPacker.parseJsonToEtf(dataToSend);
			} else {
				theVector = dataToSend.dump();
			}
			this->createHeader(header, theVector.size(), this->dataOpcode, theIndex);
			std::string theVectorNew{};
			theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
			theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
			if (this->theClients[theIndex]){
				this->theClients[theIndex]->writeData(theVectorNew);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed(theIndex);
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
			if (this->theClients[theIndex]) {
				this->theClients[theIndex]->writeData(dataToSend);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed(theIndex);
		}
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->theTask.get();
	}

	void BaseSocketAgent::onClosed(int32_t theIndex) noexcept {
		this->areWeReadyToConnectEvent.reset();
		if (this->maxReconnectTries > this->theClients[theIndex]->currentRecursionDepth) {
			std::this_thread::sleep_for(500ms);
			if (this->doWePrintErrorMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightRed()
						  << "WebSocket " + this->theClients[theIndex]->shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(this->closeCode) << DiscordCoreAPI::reset()
						  << std::endl
						  << std::endl;
			}
			if (!this->didWeFail) {
				this->sendCloseFrame(theIndex);
			}
			this->theClients[theIndex]->areWeConnected.store(false);
			this->theClients[theIndex]->currentRecursionDepth += 1;
			if (!this->theClients[theIndex]->areWeResuming) {
				this->theClients[theIndex]->areWeAuthenticated = false;
			}
			DiscordCoreAPI::ReconnectionPackage theData{};
			theData.currentShard = this->theClients[theIndex]->shard[0];
			theData.currentBaseSocketAgent = this->currentBaseSocketAgent;
			this->theClients[theIndex]->haveWeReceivedHeartbeatAck = true;
			this->reconnections.push(theData);
			this->closeCode = static_cast<WebSocketCloseCode>(0);
		} else {
			this->doWeQuit->store(true);
			this->theTask->request_stop();
		}
	}

	void BaseSocketAgent::createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode, int32_t theIndex) noexcept {
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
			this->onClosed(theIndex);
		}
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
			std::this_thread::sleep_for(500ms);
			if (doWeCollect.channelId == 0) {
				return;
			}
			dataPackage.channelId = doWeCollect.channelId;
			newData = JSONIFY(dataPackage);
			this->areWeCollectingData = true;
			this->sendMessage(newData, theIndex);
			while (this->areWeCollectingData) {
				std::this_thread::sleep_for(1ms);
			}
			this->semaphore.release();
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
			}
			this->onClosed(theIndex);
		}
	}

	void BaseSocketAgent::onMessageReceived(std::string theMessage, int32_t theIndex) noexcept {
		try {
			std::string messageNew = theMessage;
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
					this->theClients[theIndex]->sessionId = stoull(payload["d"]["session_id"].get<std::string>());
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
					this->theClients[theIndex] ->areWeAuthenticated = true;
				}
			}

			if (payload.contains("s") && !payload["s"].is_null()) {
				this->theClients[theIndex]->lastNumberReceived = payload["s"].get<int32_t>();
			}

			if (payload.contains("op") && !payload["op"].is_null()) {
				if (payload["op"] == 1) {
					this->sendHeartBeat(theIndex);
				}

				if (payload["op"] == 7) {
					if (this->doWePrintErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->theClients[theIndex]->shard.dump() + " Reconnecting (Type 7)!" << DiscordCoreAPI::reset() << std::endl
								  << std::endl;
					}
					this->theClients[theIndex]->areWeResuming = true;
					this->onClosed(theIndex);
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
						this->theClients[theIndex]->areWeAuthenticated = false;
						this->onClosed(theIndex);
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
						if (!this->theClients[theIndex]->areWeAuthenticated) {
							nlohmann::json identityJson =
								JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue), this->theClients[theIndex]->shard[0], this->theClients[theIndex]->shard[1]);
							this->sendMessage(identityJson, theIndex);
						}
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
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandCreationEvent(*dataPackage);
				} else if (payload["t"] == "APPLICATION_COMMAND_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandUpdateEvent(*dataPackage);
				} else if (payload["t"] == "APPLICATION_COMMAND_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandDeletionData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandDeletionEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->channel);
					this->eventManager->onChannelCreationEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>() };
					if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
						dataPackage->channelOld = DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(payload["d"]["id"].get<std::string>()) }).get();
						dataPackage->channelNew = dataPackage->channelOld;
					}
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->channelNew);
					this->eventManager->onChannelUpdateEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->channel);
					this->eventManager->onChannelDeletionEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_PINS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->dataPackage);
					this->eventManager->onChannelPinsUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->thread);
					this->eventManager->onThreadCreationEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_UPATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->thread);
					this->eventManager->onThreadUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->thread);
					this->eventManager->onThreadDeletionEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_LIST_SYNC") {
					std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->threadListSyncData);
					this->eventManager->onThreadListSyncEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_MEMBER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->threadMember);
					this->eventManager->onThreadMemberUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_MEMBERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->threadMembersUpdateData);
					this->eventManager->onThreadMembersUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_CREATE") {
					DiscordCoreAPI::GuildData guildNew{};
					std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>() };
					DiscordCoreInternal::parseObject(payload["d"], guildNew);
					guildNew.discordCoreClient = this->discordCoreClient;
					dataPackage->guild = guildNew;
					this->eventManager->onGuildCreationEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>() };
					if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
						dataPackage->guildOld = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(payload["d"]["id"].get<std::string>()) }).get();
						dataPackage->guildNew = dataPackage->guildOld;
					}
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->guildNew);
					this->eventManager->onGuildUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->guild);
					this->eventManager->onGuildDeletionEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_BAN_ADD") {
					std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
						DiscordCoreAPI::User newData{};
						DiscordCoreInternal::parseObject(payload["d"]["user"], newData);
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
						DiscordCoreInternal::parseObject(payload["d"]["user"], newData);
						dataPackage->user = newData;
					}
					this->eventManager->onGuildBanRemoveEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_EMOJIS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->updateData);
					this->eventManager->onGuildEmojisUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_STICKERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->updateData);
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
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->guildMember);
					dataPackage->discordCoreClient = this->discordCoreClient;
					this->eventManager->onGuildMemberAddEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_MEMBER_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
						DiscordCoreAPI::User newData{};
						DiscordCoreInternal::parseObject(payload["d"]["user"], newData);
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
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->guildMemberNew);
					this->eventManager->onGuildMemberUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_MEMBERS_CHUNK") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->chunkEventData);
					this->eventManager->onGuildMembersChunkEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_ROLE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>() };
					if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
						DiscordCoreInternal::parseObject(payload["d"]["role"], dataPackage->role);
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
					DiscordCoreInternal::parseObject(payload["d"]["role"], dataPackage->roleNew);
					if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
						dataPackage->roleOld =
							DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role"]["id"].get<std::string>()) }).get();
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
						DiscordCoreInternal::parseObject(payload["d"]["integration"], dataPackage->integrationData);
					}
					this->eventManager->onIntegrationCreationEvent(*dataPackage);
				} else if (payload["t"] == "INTEGRATION_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
						DiscordCoreInternal::parseObject(payload["d"]["integration"], dataPackage->integrationData);
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
					DiscordCoreInternal::parseObject(payload["d"], *interactionData);
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
								DiscordCoreAPI::ButtonCollector::buttonInteractionBufferMap[std::to_string(eventData->getChannelId()) + std::to_string(eventData->getMessageId())]
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
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->invite);
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
					DiscordCoreInternal::parseObject(payload["d"], *message);
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
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->messageNew);
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
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->reaction);
					this->eventManager->onReactionAddEvent(*dataPackage);
				} else if (payload["t"] == "MESSAGE_REACTION_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->reactionRemoveData);
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
					DiscordCoreInternal::parseObject(payload["d"]["emoji"], dataPackage->emoji);
					this->eventManager->onReactionRemoveEmojiEvent(*dataPackage);
				} else if (payload["t"] == "PRESENCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->presenceData);
					this->eventManager->onPresenceUpdateEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->stageInstance);
					this->eventManager->onStageInstanceCreationEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->stageInstance);
					this->eventManager->onStageInstanceUpdateEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->stageInstance);
					this->eventManager->onStageInstanceDeletionEvent(*dataPackage);
				} else if (payload["t"] == "TYPING_START") {
					std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->typingStartData);
					this->eventManager->onTypingStartEvent(*dataPackage);
				} else if (payload["t"] == "USER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
					DiscordCoreAPI::User newData{};
					DiscordCoreInternal::parseObject(payload["d"]["user"], newData);
					dataPackage->userNew = newData;
					dataPackage->userOld = DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get();
					this->eventManager->onUserUpdateEvent(*dataPackage);
				} else if (payload["t"] == "VOICE_STATE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
					DiscordCoreInternal::parseObject(payload["d"], dataPackage->voiceStateData);
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
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
			}
			this->onClosed(theIndex);
			return;
		}
	}

	void BaseSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			while (!theToken.stop_requested() && !this->doWeQuit->load()) {
				if (this->reconnections.size() > 0) {
					this->internalConnect();
				}
				this->messageCollector.runMessageCollector();
				std::unordered_map<SOCKET, WSMessageCollectorReturnData> theMessages = this->messageCollector.collectFinalMessage();
				for (auto& [key, value]: theMessages) {
					if (value.opCode == WebSocketOpCode::Op_Close) {
						this->dataOpcode = value.opCode;
						this->closeCode = value.closeCode;
						this->didWeFail = true;
						this->onClosed(key);
					} else {
						if (value.theMessage != "") {
							this->onMessageReceived(value.theMessage, key);
						}
						if (this->theClients[key] != nullptr && this->theClients[key]->stopWatch.hasTimePassed() && this->theClients[key]->areWeHeartBeating) {
							this->theClients[key]->stopWatch.resetTimer();
							this->sendHeartBeat(key);
						}
						if (this->theClients[key] != nullptr && this->heartbeatInterval != 0 && !this->theClients[key]->areWeHeartBeating) {
							this->theClients[key]->areWeHeartBeating = true;
							this->theClients[key]->stopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
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

	void BaseSocketAgent::sendCloseFrame(int32_t theIndex) noexcept {
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

	void BaseSocketAgent::sendHeartBeat(int32_t theIndex) noexcept {
		try {
			if (this->theClients[theIndex]->haveWeReceivedHeartbeatAck) {
				nlohmann::json heartbeat = JSONIFY(this->theClients[theIndex]->lastNumberReceived);
				this->sendMessage(heartbeat, theIndex);
				this->theClients[theIndex]->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosed(theIndex);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendHeartBeat()");
			}
			this->onClosed(theIndex);
		}
	}

	void BaseSocketAgent::internalConnect() noexcept {
		try {
			if (this->theReconnectionTimer.hasTimePassed()) {
				if (this->reconnections.size() > 0) {
					auto reconnectionData = this->reconnections.front();
					this->reconnections.pop();
					std::this_thread::sleep_for(1ms);
					try {
						this->theClients[reconnectionData.currentShard]->connect(this->baseUrl, "443");
					} catch (...) {
						this->reconnections.push(reconnectionData);
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
					std::this_thread::sleep_for(1ms);
					this->sendMessage(sendString, reconnectionData.currentShard);
					std::string theResult{};
					while (theResult == "") {
						auto returnValue = this->messageCollector.runMessageCollector();
						if (!returnValue) {
							return;
						}
						theResult = this->messageCollector.collectFinalMessage()[reconnectionData.currentShard].theMessage;
						std::this_thread::sleep_for(1ms);
					}
					this->onMessageReceived(theResult, reconnectionData.currentShard);
					this->theClients[reconnectionData.currentShard]->areWeConnected.store(true);
				}
				this->theReconnectionTimer.resetTimer();
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::connect()");
			}
		}
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ReconnectionPackage thePackage) noexcept {
		int32_t currentRecursionDepth{};
		int32_t lastSentNumber{};
		int64_t sessionId{};
		if (this->theClients.contains(thePackage.currentShard) && this->theClients[thePackage.currentShard] != nullptr) {
			currentRecursionDepth = this->theClients[thePackage.currentShard]->currentRecursionDepth;
			lastSentNumber = this->theClients[thePackage.currentShard]->lastNumberReceived;
			sessionId = this->theClients[thePackage.currentShard]->sessionId;
		}
		currentRecursionDepth += 1;
		this->theClients[thePackage.currentShard] =
			std::make_unique<WebSocketSSLShard>(1024 * 16, thePackage.currentShard, this->discordCoreClient->shardingOptions.totalNumberOfShards, this->doWePrintErrorMessages);
		this->theClients[thePackage.currentShard]->currentRecursionDepth = currentRecursionDepth;
		this->theClients[thePackage.currentShard]->lastNumberReceived = lastSentNumber;
		this->theClients[thePackage.currentShard]->sessionId = sessionId;
		this->theClients[thePackage.currentShard]->areWeConnected.store(false);
		this->reconnections.push(thePackage);
		while (!this->theClients[thePackage.currentShard]->areWeConnected.load()) {
			std::this_thread::sleep_for(1ms);
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
			if (this->theClients[0]) {
				this->theClients[0]->writeData(theVectorNew);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendMessage(std::string& dataToSend) noexcept {
		try {
			if (this->doWePrintSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << std::endl << dataToSend << DiscordCoreAPI::reset();
			}
			if (this->theClients[0]) {
				this->theClients[0]->writeData(dataToSend);
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosedExternal();
		}
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

	void VoiceSocketAgent::onClosedExternal() noexcept {
		this->doWeReconnect.store(true);
	}

	void VoiceSocketAgent::createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept {
		try {
			outBuffer.push_back(static_cast<uint8_t>(opCode) | webSocketFinishBit);

			uint32_t indexCount{ 0 };
			if (sendLength <= webSocketMaxPayloadLengthSmall) {
				outBuffer.push_back(static_cast<uint8_t>(sendLength));
				indexCount = 0;
			} else if (sendLength <= webSocketMaxPayloadLengthLarge) {
				outBuffer.push_back(static_cast<uint8_t>(webSocketPayloadLengthMagicLarge));
				indexCount = 2;
			} else {
				outBuffer.push_back(static_cast<uint8_t>(webSocketPayloadLengthMagicHuge));
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
			this->onClosedExternal();
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
				if (!this->messageCollector.runMessageCollector()) {
					this->onClosedExternal();
				}
				auto theReturnMessage = this->messageCollector.collectFinalMessage();
				if (theReturnMessage[0].opCode == WebSocketOpCode::Op_Close) {
					this->dataOpcode = theReturnMessage[0].opCode;
					this->closeCode = theReturnMessage[0].closeCode;
					this->onClosedExternal();
				}
				if (theReturnMessage[0].theMessage != "") {
					this->onMessageReceived(theReturnMessage[0].theMessage);
				}
				if (this->voiceSocket) {
					this->voiceSocket->readData(true);
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

	void VoiceSocketAgent::onClosedInternal() noexcept {
		if (this->doWePrintErrorMessages) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "Voice WebSocket Closed; Code: " << +static_cast<uint16_t>(this->closeCode) << DiscordCoreAPI::reset() << std::endl
					  << std::endl;
		}
		this->closeCode = static_cast<WebSocketCloseCode>(0);
		this->voiceSocket.reset(nullptr);
		this->theClients[0].reset(nullptr);
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
			auto theClient = std::make_unique<WebSocketSSLShard>(1024 * 16, 0, 0, this->doWePrintErrorMessages);
			SOCKET theSocket = theClient->theSocket;
			theClient->connect(this->baseUrl, "443");
			this->theClients[0] = std::move(theClient);
			this->messageCollector = WSMessageCollector{ this->doWePrintErrorMessages, &this->theClients };
			std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->sendMessage(sendVector);
			std::string theResult{};
			while (theResult == "") {
				this->messageCollector.runMessageCollector();
				theResult = this->messageCollector.collectFinalMessage()[theSocket].theMessage;
				std::this_thread::sleep_for(1ms);
			}
			this->onMessageReceived(theResult);
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