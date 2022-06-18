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

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocketAgentNew,
		ParserAgent* parserAgentNew) noexcept {
		this->doWePrintSuccessMessages = discordCoreClientNew->loggingOptions.logWebSocketSuccessMessages;
		this->doWePrintErrorMessages = discordCoreClientNew->loggingOptions.logWebSocketErrorMessages;
		if (discordCoreClientNew->theFormat == DiscordCoreAPI::TextFormat::Etf) {
			this->dataOpcode = WebSocketOpCode::Op_Binary;
		} else {
			this->dataOpcode = WebSocketOpCode::Op_Text;
		}
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->discordCoreClient = discordCoreClientNew;
		this->parserAgent = parserAgentNew;
		this->doWeQuit = doWeQuitNew;
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
	}

	ParserAgent::ParserAgent(DiscordCoreAPI::DiscordCoreClient* theClientNew){
		this->discordCoreClient = theClientNew;
		this->theTask = std::jthread{ [this](std::stop_token theToken) {
			this->run(theToken);
		} };
	}

	void ParserAgent::run(std::stop_token theToken) noexcept {
		while (!theToken.stop_requested()) {
			for (auto& [key, value]: this->messagePackages) {
				if (value.inputBuffer.size() > 0) {
					this->parseHeadersAndMessage(key);
				}
				if (value.processedMessages->size() > 0) {
					this->onMessageReceived(key);
				}
			}
			std::this_thread::sleep_for(1ms);
		}
	}

	void ParserAgent::collectOutputData(WebSocketSSLShard& theShard, int32_t theIndex) noexcept {
		std::lock_guard<std::mutex> theLock{ this->theMutex };
		if (this->messagePackages.contains(theIndex)) {
			auto theStringNew = this->messagePackages[theIndex].outputBuffer;
			theShard.outputBuffer->push_back(theStringNew);
			this->messagePackages[theIndex].outputBuffer.clear();
		}
	}

	void ParserAgent::submitStringForProcessing(WebSocketSSLShard& theShard, int32_t theIndex) noexcept {
		WebSocketMessagePackage theMessagePackage{};
		if (this->discordCoreClient != nullptr && this->discordCoreClient->baseSocketAgent != nullptr) {
			theMessagePackage.areWeReadyToConnectEvent = &this->discordCoreClient->baseSocketAgent->areWeReadyToConnectEvent;
		}
		theMessagePackage.haveWeReceivedHeartbeatAck = theShard.haveWeReceivedHeartbeatAck.get();
		theMessagePackage.serverUpdateCollected = theShard.serverUpdateCollected.get();
		theMessagePackage.currentRecursionDepth = theShard.currentRecursionDepth.get();
		theMessagePackage.stateUpdateCollected = theShard.stateUpdateCollected.get();
		theMessagePackage.areWeCollectingData = theShard.areWeCollectingData.get();
		theMessagePackage.lastNumberReceived = theShard.lastNumberReceived.get();
		theMessagePackage.areWeHeartBeating = theShard.areWeHeartBeating.get();
		theMessagePackage.processedMessages = theShard.processedMessages.get();
		theMessagePackage.heartBeatStopWatch = &theShard.heartBeatStopWatch;
		theMessagePackage.commandBufferPtr = theShard.commandBuffer.get();
		theMessagePackage.areWeConnected = theShard.areWeConnected.get();
		theMessagePackage.areWeResuming = theShard.areWeResuming.get();
		theMessagePackage.messageLength = theShard.messageLength.get();
		theMessagePackage.messageOffset = theShard.messageOffset.get();
		theMessagePackage.closeCode = theShard.closeCode.get();
		theMessagePackage.theState = theShard.theState.get();
		theMessagePackage.opCode = theShard.opCode.get();
		theMessagePackage.sessionId = theShard.sessionId;
		theMessagePackage.shard = theShard.shard;
		this->messagePackages[theIndex] = std::move(theMessagePackage);
		this->messagePackages[theIndex].inputBuffer.insert(this->messagePackages[theIndex].inputBuffer.end(), theShard.inputBuffer->begin(), theShard.inputBuffer->end());
		theShard.inputBuffer->clear();
	}

	void BaseSocketAgent::sendMessage(const nlohmann::json& dataToSend, WebSocketSSLShard& theIndex) noexcept {
		try {
			DiscordCoreAPI::StopWatch stopWatch{ 5500ms };
			while (!theIndex.areWeConnected->load() && !(dataToSend.contains("op") && (dataToSend["op"] == 2 || dataToSend["op"] == 6))) {
				if (stopWatch.hasTimePassed()) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
			std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
			if (this->doWePrintSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + theIndex.shard.dump() + std::string("'s Message: ")
							<< dataToSend.dump() << DiscordCoreAPI::reset() << std::endl
							<< std::endl;
			}
			std::string theVectorNew{};
			this->parserAgent->stringifyJsonData(dataToSend, theVectorNew);

			theIndex.writeData(theVectorNew, false);
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed(theIndex);
		}
	}

	void BaseSocketAgent::sendMessage(std::string& dataToSend, WebSocketSSLShard& theIndex) noexcept {
		try {
			std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
			if (this->doWePrintSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + theIndex.shard.dump() + std::string("'s Message: ") << std::endl
						  << dataToSend << DiscordCoreAPI::reset();
			}
			theIndex.writeData(dataToSend, false);
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed(theIndex);
		}
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage thePackage) noexcept {
		while (!this->discordCoreClient->theStopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
		}
		this->discordCoreClient->theStopWatch.resetTimer();
		this->connections.push(thePackage);
		while (!this->theClients.contains(thePackage.currentShard)) {
			std::this_thread::sleep_for(1ms);
		}
	}

	void BaseSocketAgent::onClosed(WebSocketSSLShard& theShard) noexcept {
		this->areWeReadyToConnectEvent.reset();
		if (this->maxReconnectTries > *theShard.currentRecursionDepth) {
			std::this_thread::sleep_for(500ms);
			if (this->doWePrintErrorMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket " + theShard.shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(*theShard.closeCode)
						  << DiscordCoreAPI::reset() << std::endl
						  << std::endl;
			}
			this->sendCloseFrame(theShard);
			DiscordCoreAPI::ConnectionPackage theData{};
			theData.currentShard = theShard.shard[0];
			theData.currentReconnectionDepth = *theShard.currentRecursionDepth;
			theData.sessionId = theShard.sessionId;
			theData.areWeResuming = *theShard.areWeResuming;
			theData.currentBaseSocketAgent = this->currentBaseSocketAgent;
			theData.lastNumberReceived = *theShard.lastNumberReceived;
			if (this->theClients.contains(theShard.shard[0])) {
				this->theClients.erase(theShard.shard[0]);
			}
			this->connections.push(theData);
		} else if (this->maxReconnectTries <= *theShard.currentRecursionDepth) {
			this->doWeQuit->store(true);
			this->theTask->request_stop();
		}
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->theTask.get();
	}

	void BaseSocketAgent::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect, WebSocketSSLShard& theIndex) noexcept {
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
			theIndex.areWeCollectingData->store(true);
			this->sendMessage(newData, theIndex);
			try {
				WebSocketSSLShard::processIO(this->theClients, 100000);
			} catch (...) {
				if (this->doWePrintErrorMessages) {
					DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
				}
			}
			DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theStopWatch{ 5000ms };
			while (theIndex.areWeCollectingData->load()) {
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
			this->onClosed(theIndex);
		}
	}

	void ParserAgent::createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept {
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
			if (this->discordCoreClient->loggingOptions.logWebSocketErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::createHeader()");
			}
		}
	}

	void ParserAgent::stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString) noexcept {
		std::string theVector{};
		std::string header{};
		WebSocketOpCode theOpCode{};
		if (this->discordCoreClient->theFormat == DiscordCoreAPI::TextFormat::Etf) {
			theVector = this->erlPacker.parseJsonToEtf(dataToSend);
			theOpCode = WebSocketOpCode::Op_Binary;
		} else {
			theVector = dataToSend.dump();
			theOpCode = WebSocketOpCode::Op_Text;
		}
		this->createHeader(header, theVector.size(), theOpCode);
		std::string theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		theString = theVectorNew;
	}

	void ParserAgent::parseHeadersAndMessage(int32_t theIndex) noexcept {
		if (*this->messagePackages[theIndex].theState == WebSocketState::Connecting01) {
			std::string newVector{};
			if (this->messagePackages[theIndex].inputBuffer.find("\r\n\r\n") != std::string::npos) {
				std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
				newVector.erase(0, newVector.find("\r\n\r\n") + 4);
				this->messagePackages[theIndex].inputBuffer.clear();
				this->messagePackages[theIndex].inputBuffer.insert(messagePackages[theIndex].inputBuffer.end(), newVector.begin(), newVector.end());
				*this->messagePackages[theIndex].theState = WebSocketState::Connecting02;
			}
		}
		if (messagePackages[theIndex].inputBuffer.size() < 4) {
			return;
		}
		*this->messagePackages[theIndex].opCode = static_cast<WebSocketOpCode>(messagePackages[theIndex].inputBuffer[0] & ~webSocketFinishBit);
		switch (*this->messagePackages[theIndex].opCode) {
			case WebSocketOpCode::Op_Continuation:
				[[fallthrough]];
			case WebSocketOpCode::Op_Text:
				[[fallthrough]];
			case WebSocketOpCode::Op_Binary:
				[[fallthrough]];
			case WebSocketOpCode::Op_Ping:
				[[fallthrough]];
			case WebSocketOpCode::Op_Pong: {
				uint8_t length01 = messagePackages[theIndex].inputBuffer[1];
				messagePackages[theIndex].messageOffset->store(2);
				if (length01 & webSocketMaskBit) {
					return;
				}
				messagePackages[theIndex].messageLength->store(length01);
				if (length01 == webSocketPayloadLengthMagicLarge) {
					if (messagePackages[theIndex].inputBuffer.size() < 8) {
						return;
					}
					uint8_t length03 = messagePackages[theIndex].inputBuffer[2];
					uint8_t length04 = messagePackages[theIndex].inputBuffer[3];
					messagePackages[theIndex].messageLength->store(static_cast<uint64_t>((length03 << 8) | length04));
					messagePackages[theIndex].messageOffset->fetch_add(2);
				} else if (length01 == webSocketPayloadLengthMagicHuge) {
					if (messagePackages[theIndex].inputBuffer.size() < 10) {
						return;
					}
					messagePackages[theIndex].messageLength->store(0);
					auto theValue = messagePackages[theIndex].messageLength->load();
					for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
						uint8_t lengthNew = static_cast<uint8_t>(messagePackages[theIndex].inputBuffer[x]);
						theValue |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
					}
					messagePackages[theIndex].messageLength->store();
					messagePackages[theIndex].messageOffset->fetch_add(8);
				}
				if (messagePackages[theIndex].inputBuffer.size() < static_cast<uint64_t>(messagePackages[theIndex].messageOffset->load()) + static_cast<uint64_t>(messagePackages[theIndex].messageLength->load())) {
					return;
				} else {
					std::string finalMessage{};
					finalMessage.insert(finalMessage.begin(), messagePackages[theIndex].inputBuffer.begin() + messagePackages[theIndex].messageOffset->load(),
						messagePackages[theIndex].inputBuffer.begin() + messagePackages[theIndex].messageOffset->load() + messagePackages[theIndex].messageLength->load());
					messagePackages[theIndex].processedMessages->push(finalMessage);
					messagePackages[theIndex].inputBuffer.erase(messagePackages[theIndex].inputBuffer.begin(),
						messagePackages[theIndex].inputBuffer.begin() + messagePackages[theIndex].messageOffset->load() + messagePackages[theIndex].messageLength->load());
					return;
				}
			}
			case WebSocketOpCode::Op_Close: {
				uint16_t close = messagePackages[theIndex].inputBuffer[2] & 0xff;
				close <<= 8;
				close |= messagePackages[theIndex].inputBuffer[3] & 0xff;
				*messagePackages[theIndex].closeCode = static_cast<WebSocketCloseCode>(close);
				messagePackages[theIndex].inputBuffer.erase(messagePackages[theIndex].inputBuffer.begin(), messagePackages[theIndex].inputBuffer.begin() + 4);
				return;
			}
			default: {
				return;
			}
		}
	}

	void BaseSocketAgent::checkForAndSendHeartBeat(WebSocketSSLShard& theIndex, bool isImmediate) noexcept {
		try {
			if (this->parserAgent->messagePackages[theIndex.shard[0]].heartBeatStopWatch->hasTimePassed() && theIndex.haveWeReceivedHeartbeatAck || isImmediate) {
				nlohmann::json heartbeat = JSONIFY(*theIndex.lastNumberReceived);
				if (this->doWePrintSuccessMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + theIndex.shard.dump() + std::string("'s Message: ") << heartbeat.dump()
							  << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				std::string theString{};
				this->parserAgent->stringifyJsonData(heartbeat, theString);
				theIndex.writeData(theString, true);
				theIndex.haveWeReceivedHeartbeatAck->store(false);
				this->parserAgent->messagePackages[theIndex.shard[0]].heartBeatStopWatch->resetTimer();
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::checkForAndSendHeartBeat()");
			}
			this->onClosed(theIndex);
		}
	}

	void ParserAgent::onMessageReceived(int32_t theIndex) noexcept {
		try {
			std::string messageNew{}; 
			if (this->messagePackages[theIndex].processedMessages->size() > 0) {
				messageNew = this->messagePackages[theIndex].processedMessages->front();
				this->messagePackages[theIndex].processedMessages->pop();
			} else {
				return;
			}
			
			nlohmann::json payload{};

			if (this->discordCoreClient->theFormat == DiscordCoreAPI::TextFormat::Etf) {
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
				if (this->messagePackages[theIndex].areWeCollectingData && payload["t"] == "VOICE_SERVER_UPDATE" && !this->messagePackages[theIndex].serverUpdateCollected) {
					if (!this->messagePackages[theIndex].serverUpdateCollected && !this->messagePackages[theIndex].stateUpdateCollected) {
						*this->messagePackages[theIndex].voiceConnectionData = VoiceConnectionData{};
						this->messagePackages[theIndex].voiceConnectionData->endPoint = payload["d"]["endpoint"].get<std::string>();
						this->messagePackages[theIndex].voiceConnectionData->token = payload["d"]["token"].get<std::string>();
						*this->messagePackages[theIndex].serverUpdateCollected = true;
					} else {
						this->messagePackages[theIndex].voiceConnectionData->endPoint = payload["d"]["endpoint"].get<std::string>();
						this->messagePackages[theIndex].voiceConnectionData->token = payload["d"]["token"].get<std::string>();
						if (this->voiceConnectionDataBufferMap.contains(payload["d"]["guild_id"])) {
							this->voiceConnectionDataBufferMap[payload["d"]["guild_id"]]->send(*this->messagePackages[theIndex].voiceConnectionData);
						}
						this->messagePackages[theIndex].serverUpdateCollected->store(false);
						this->messagePackages[theIndex].stateUpdateCollected->store(false);
						this->messagePackages[theIndex].areWeCollectingData->store(false);
					}
				}

				if (this->messagePackages[theIndex].areWeCollectingData && payload["t"] == "VOICE_STATE_UPDATE" && !this->messagePackages[theIndex].stateUpdateCollected &&
					payload["d"]["member"]["user"]["id"] == std::to_string(this->messagePackages[theIndex].userId->load())) {
					if (!this->messagePackages[theIndex].stateUpdateCollected && !this->messagePackages[theIndex].serverUpdateCollected) {
						*this->messagePackages[theIndex].voiceConnectionData = VoiceConnectionData{};
						this->messagePackages[theIndex].voiceConnectionData->sessionId = payload["d"]["session_id"].get<std::string>();
						this->messagePackages[theIndex].stateUpdateCollected->store(true);
					} else {
						this->messagePackages[theIndex].voiceConnectionData->sessionId = payload["d"]["session_id"].get<std::string>();
						if (this->voiceConnectionDataBufferMap.contains(payload["d"]["guild_id"])) {
							this->voiceConnectionDataBufferMap[payload["d"]["guild_id"]]->send(*this->messagePackages[theIndex].voiceConnectionData);
						}
						this->messagePackages[theIndex].serverUpdateCollected->store(false);
						this->messagePackages[theIndex].stateUpdateCollected->store(false);
						this->messagePackages[theIndex].areWeCollectingData->store(false);
					}
				}

				if (payload["t"] == "RESUMED") {
					this->messagePackages[theIndex].areWeConnected->store(true);
					this->messagePackages[theIndex].currentRecursionDepth->store(0);
					this->messagePackages[theIndex].areWeReadyToConnectEvent->set();
				}

				if (payload["t"] == "READY") {
					this->messagePackages[theIndex].areWeConnected->store(true);
					this->messagePackages[theIndex].sessionId = payload["d"]["session_id"].get<std::string>();
					DiscordCoreAPI::UserData theUser{};
					parseObject(payload["d"]["user"], theUser);
					this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ theUser, this->discordCoreClient->baseSocketAgent.get() };
					DiscordCoreAPI::Users::insertUser(theUser);
					this->messagePackages[theIndex].currentRecursionDepth->store(0);
					this->messagePackages[theIndex].areWeReadyToConnectEvent->set();
				}
			}

			if (payload.contains("s") && !payload["s"].is_null()) {
				this->messagePackages[theIndex].lastNumberReceived->store(payload["s"].get<int32_t>());
			}

			if (payload.contains("op") && !payload["op"].is_null()) {
				if (payload["op"] == 1) {
					this->messagePackages[theIndex].commandBufferPtr->send(WebSocketCommand::Heartbeat);
				}

				if (payload["op"] == 7) {
					if (this->discordCoreClient->loggingOptions.logWebSocketErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->messagePackages[theIndex].shard.dump() + " Reconnecting (Type 7)!" << DiscordCoreAPI::reset() << std::endl
								  << std::endl;
					}
					this->messagePackages[theIndex].areWeResuming->store(true);
					this->messagePackages[theIndex].commandBufferPtr->send(WebSocketCommand::Close);
					return;
				}

				if (payload["op"] == 9) {
					if (this->discordCoreClient->loggingOptions.logWebSocketErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->messagePackages[theIndex].shard.dump() + " Reconnecting (Type 9)!" << DiscordCoreAPI::reset() << std::endl
								  << std::endl;
					}
					this->messagePackages[theIndex].currentRecursionDepth->fetch_add(1);
					std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
					int32_t numOfMsToWait =
						static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
					std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
					if (payload["d"] == true) {
						nlohmann::json identityJson =
							JSONIFY(this->discordCoreClient->botToken, static_cast<int32_t>(this->discordCoreClient->theIntents), this->messagePackages[theIndex].shard[0], this->messagePackages[theIndex].shard[1]);
						std::string theString{};
						this->stringifyJsonData(identityJson, theString);
						this->messagePackages[theIndex].outputBuffer.insert(this->messagePackages[theIndex].outputBuffer.end(), theString.begin(), theString.end());
						this->messagePackages[theIndex].commandBufferPtr->send(WebSocketCommand::Send_Message);
					} else {
						this->messagePackages[theIndex].areWeResuming->store(false);
						this->messagePackages[theIndex].commandBufferPtr->send(WebSocketCommand::Close);
						return;
					}
				}

				if (payload["op"] == 10) {
					if (payload["d"].contains("heartbeat_interval") && !payload["d"]["heartbeat_interval"].is_null()) {
						*this->messagePackages[theIndex].heartBeatStopWatch = DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ std::chrono::milliseconds{ payload["d"]["heartbeat_interval"] } };
						this->messagePackages[theIndex].heartBeatStopWatch->resetTimer();
					}
					if (this->messagePackages[theIndex].areWeResuming->load()) {
						std::this_thread::sleep_for(1500ms);
						nlohmann::json resumePayload =
							JSONIFY(this->discordCoreClient->botToken, this->messagePackages[theIndex].sessionId, this->messagePackages[theIndex].lastNumberReceived->load());
						std::string theString{};
						this->stringifyJsonData(resumePayload, theString);
						this->messagePackages[theIndex].outputBuffer.insert(this->messagePackages[theIndex].outputBuffer.end(), theString.begin(), theString.end());
						this->messagePackages[theIndex].commandBufferPtr->send(WebSocketCommand::Send_Message);
						*this->messagePackages[theIndex].theState = WebSocketState::Connected;
					} else {
						nlohmann::json identityJson = JSONIFY(this->discordCoreClient->botToken, static_cast<int32_t>(this->discordCoreClient->theIntents),
							this->messagePackages[theIndex].shard[0], this->messagePackages[theIndex].shard[1]);
						std::string theString{};
						this->stringifyJsonData(identityJson, theString);
						this->messagePackages[theIndex].outputBuffer.insert(this->messagePackages[theIndex].outputBuffer.end(), theString.begin(), theString.end());
						this->messagePackages[theIndex].commandBufferPtr->send(WebSocketCommand::Send_Message);
						*this->messagePackages[theIndex].theState = WebSocketState::Connected;
					}
					this->messagePackages[theIndex].areWeHeartBeating->store(false);
				}
				if (payload["op"] == 11) {
					this->messagePackages[theIndex].haveWeReceivedHeartbeatAck->store(true);
				}
			}

			if (payload.contains("d") && !payload["d"].is_null() && payload.contains("t") && !payload["t"].is_null()) {
				if (payload["t"] == "APPLICATION_COMMAND_PERMISSIONS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
						std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>()
					};
					parseObject(payload["d"], dataPackage->permissionData);
					this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
				} else if (payload["t"] == "AUTO_MODERATION_RULE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>() };
					parseObject(payload["d"], dataPackage->theRule);
					this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
				} else if (payload["t"] == "AUTO_MODERATION_RULE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>() };
					parseObject(payload["d"], dataPackage->theRule);
					this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
				} else if (payload["t"] == "AUTO_MODERATION_RULE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>() };
					parseObject(payload["d"], dataPackage->theRule);
					this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
				} else if (payload["t"] == "AUTO_MODERATION_ACTION_EXECUTION") {
					std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>() };
					parseObject(payload["d"], dataPackage->theData);
					this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>() };
					parseObject(payload["d"], dataPackage->channel);
					this->discordCoreClient->eventManager.onChannelCreationEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>() };
					if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
						dataPackage->channelOld = DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(payload["d"]["id"].get<std::string>()) }).get();
						dataPackage->channelNew = dataPackage->channelOld;
					}
					parseObject(payload["d"], dataPackage->channelNew);
					this->discordCoreClient->eventManager.onChannelUpdateEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>() };
					parseObject(payload["d"], dataPackage->channel);
					this->discordCoreClient->eventManager.onChannelDeletionEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_PINS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
					parseObject(payload["d"], dataPackage->dataPackage);
					this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
					parseObject(payload["d"], dataPackage->thread);
					this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_UPATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
					parseObject(payload["d"], dataPackage->thread);
					this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
					parseObject(payload["d"], dataPackage->thread);
					this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_LIST_SYNC") {
					std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
					parseObject(payload["d"], dataPackage->threadListSyncData);
					this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_MEMBER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
					parseObject(payload["d"], dataPackage->threadMember);
					this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_MEMBERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
					parseObject(payload["d"], dataPackage->threadMembersUpdateData);
					this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_CREATE") {
					DiscordCoreAPI::GuildData guildNew{};
					std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>() };
					parseObject(payload["d"], guildNew);
					guildNew.discordCoreClient = this->discordCoreClient;
					dataPackage->guild = guildNew;
					this->discordCoreClient->eventManager.onGuildCreationEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>() };
					if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
						dataPackage->guildOld = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(payload["d"]["id"].get<std::string>()) }).get();
						dataPackage->guildNew = dataPackage->guildOld;
					}
					parseObject(payload["d"], dataPackage->guildNew);
					this->discordCoreClient->eventManager.onGuildUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>() };
					parseObject(payload["d"], dataPackage->guild);
					this->discordCoreClient->eventManager.onGuildDeletionEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_EMOJIS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
					parseObject(payload["d"], dataPackage->updateData);
					this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_STICKERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
					parseObject(payload["d"], dataPackage->updateData);
					this->discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_INTEGRATIONS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_MEMBER_ADD") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberAddData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildMember.guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					parseObject(payload["d"], dataPackage->guildMember);
					dataPackage->discordCoreClient = this->discordCoreClient;
					this->discordCoreClient->eventManager.onGuildMemberAddEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onGuildMemberRemoveEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_MEMBERS_CHUNK") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
					parseObject(payload["d"], dataPackage->chunkEventData);
					this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_ROLE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>() };
					if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
						parseObject(payload["d"]["role"], dataPackage->role);
					}
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					this->discordCoreClient->eventManager.onRoleCreationEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_ROLE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleUpdateData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					parseObject(payload["d"]["role"], dataPackage->roleNew);
					if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
						dataPackage->roleOld =
							DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role"]["id"].get<std::string>()) }).get();
						dataPackage->roleNew = dataPackage->roleOld;
					}
					this->discordCoreClient->eventManager.onRoleUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_ROLE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleDeletionData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("role_id") && !payload["d"]["role_id"].is_null()) {
						dataPackage->roleOld =
							DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role_id"].get<std::string>()) }).get();
					}
					this->discordCoreClient->eventManager.onRoleDeletionEvent(*dataPackage);
				} else if (payload["t"] == "INTEGRATION_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
						parseObject(payload["d"]["integration"], dataPackage->integrationData);
					}
					this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
				} else if (payload["t"] == "INTEGRATION_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
						parseObject(payload["d"]["integration"], dataPackage->integrationData);
					}
					this->discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
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
						this->discordCoreClient->commandController.checkForAndRunCommand(*commandData);
						this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
						std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
						eventCreationData->inputEventData = *eventData;
						this->discordCoreClient->eventManager.onInputEventCreationEvent(*eventCreationData);
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
							this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
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
							this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
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
						this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
					} else if (interactionData->type == DiscordCoreAPI::InteractionType::Application_Command_Autocomplete) {
						eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
						*eventData->interactionData = *interactionData;
						std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
						dataPackage->interactionData = *interactionData;
						std::unique_ptr<DiscordCoreAPI::OnAutoCompleteEntryData> autocompleteEntryData{ std::make_unique<DiscordCoreAPI::OnAutoCompleteEntryData>() };
						autocompleteEntryData->inputEvent = *eventData;
						this->discordCoreClient->eventManager.onAutoCompleteEntryEvent(*autocompleteEntryData);
						this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
					}
				} else if (payload["t"] == "INVITE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
					parseObject(payload["d"], dataPackage->invite);
					this->discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
				} else if (payload["t"] == "MESSAGE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::Message> message{ std::make_unique<DiscordCoreAPI::Message>() };
					parseObject(payload["d"], *message);
					std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageCreationData>() };
					dataPackage->message = *message;
					for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
						value->send(*message);
					}
					this->discordCoreClient->eventManager.onMessageCreationEvent(*dataPackage);
					if (message->content.find("!registerapplicationcommands") != std::string::npos) {
						std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>() };
						commandData->commandName = "registerapplicationcommands";
						DiscordCoreAPI::CommandData commandDataNew = *commandData;
						this->discordCoreClient->commandController.checkForAndRunCommand(commandDataNew);
					}
					std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
				} else if (payload["t"] == "MESSAGE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
					parseObject(payload["d"], dataPackage->messageNew);
					for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
						value->send(dataPackage->messageNew);
					}
					this->discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
				} else if (payload["t"] == "MESSAGE_REACTION_ADD") {
					std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
					parseObject(payload["d"], dataPackage->reaction);
					this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
				} else if (payload["t"] == "MESSAGE_REACTION_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
					parseObject(payload["d"], dataPackage->reactionRemoveData);
					this->discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
				} else if (payload["t"] == "PRESENCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
					parseObject(payload["d"], dataPackage->presenceData);
					this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
					parseObject(payload["d"], dataPackage->stageInstance);
					this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
					parseObject(payload["d"], dataPackage->stageInstance);
					this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
					parseObject(payload["d"], dataPackage->stageInstance);
					this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
				} else if (payload["t"] == "TYPING_START") {
					std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
					parseObject(payload["d"], dataPackage->typingStartData);
					this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
				} else if (payload["t"] == "USER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
					DiscordCoreAPI::User newData{};
					parseObject(payload["d"]["user"], newData);
					dataPackage->userNew = newData;
					dataPackage->userOld = DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get();
					this->discordCoreClient->eventManager.onUserUpdateEvent(*dataPackage);
				} else if (payload["t"] == "VOICE_STATE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
					parseObject(payload["d"], dataPackage->voiceStateData);
					this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
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
					this->discordCoreClient->eventManager.onVoiceServerUpdateEvent(*dataPackage);
				} else if (payload["t"] == "WEBHOOKS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
					if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
						dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
					}
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					this->discordCoreClient->eventManager.onWebhookUpdateEvent(*dataPackage);
				}
			}
			if (this->discordCoreClient->loggingOptions.logWebSocketSuccessMessages && !payload.is_null()) {
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from WebSocket " + this->messagePackages[theIndex].shard.dump() + std::string(": ") << payload.dump()
						  << DiscordCoreAPI::reset() << std::endl
						  << std::endl;
			}
			} catch (...) {
				if (this->discordCoreClient->loggingOptions.logWebSocketErrorMessages) {
					DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
				}
			}
		
	}

	void BaseSocketAgent::sendCloseFrame(WebSocketSSLShard& theIndex) noexcept {
		std::string theString{};
		theString.push_back(static_cast<int8_t>(WebSocketOpCode::Op_Close) | static_cast<int8_t>(webSocketFinishBit));
		theString.push_back(0);
		theString.push_back(static_cast<int8_t>(static_cast<uint16_t>(1000) >> 8));
		theString.push_back(static_cast<int8_t>(1000 & 0xff));
		theIndex.writeData(theString, true);
		try {
			WebSocketSSLShard::processIO(this->theClients);
		} catch (ProcessingError&) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendCloseFrame()");
			}
		}
	}

	void BaseSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			while (!theToken.stop_requested() && !this->doWeQuit->load()) {
				if (this->connections.size() > 0) {
					this->internalConnect();
				}
				try {
					WebSocketSSLShard::processIO(this->theClients);
				} catch (...) {
					if (this->doWePrintErrorMessages) {
						DiscordCoreAPI::reportException("BaseSocketAgent::run()");
					}
				}
				for (auto& [key, value]: this->theClients) {
					try {
						WebSocketSSLShard::processIO(this->theClients);
					} catch (...) {
						if (this->doWePrintErrorMessages) {
							DiscordCoreAPI::reportException("BaseSocketAgent::run()");
						}
					}
					if (this->theClients.contains(key) && value->inputBuffer->size() > 0) {
						this->parserAgent->submitStringForProcessing(*value, key);
					}
					WebSocketCommand theCommand{};
					if (value->commandBuffer->tryReceive(theCommand)) {
						if (theCommand == WebSocketCommand::Send_Message) {
							this->parserAgent->collectOutputData(*value, key);
						}
					}
					if (this->theClients.contains(key)) {
						this->checkForAndSendHeartBeat(*value);
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
			if (this->connections.size() > 0) {
				DiscordCoreAPI::ConnectionPackage connectData = this->connections.front();
				this->connections.pop();
				connectData.currentReconnectionDepth += 1;
				std::unordered_map<int32_t, std::unique_ptr<DiscordCoreInternal::WebSocketSSLShard>> theMap{};
				theMap[connectData.currentShard] = std::make_unique<WebSocketSSLShard>(&this->connections, this->currentBaseSocketAgent,
					connectData.currentShard, this->discordCoreClient->shardingOptions.totalNumberOfShards, this->doWePrintErrorMessages);
				*theMap[connectData.currentShard]->currentRecursionDepth = connectData.currentReconnectionDepth;
				theMap[connectData.currentShard]->currentBaseSocketAgent = connectData.currentBaseSocketAgent;
				*theMap[connectData.currentShard]->lastNumberReceived = connectData.lastNumberReceived;
				*theMap[connectData.currentShard]->areWeResuming = connectData.areWeResuming;
				theMap[connectData.currentShard]->sessionId = connectData.sessionId;

				try {
					theMap[connectData.currentShard]->connect(this->discordCoreClient->theAddress, this->discordCoreClient->thePort);
				} catch (...) {
					if (this->doWePrintErrorMessages) {
						DiscordCoreAPI::reportException("BaseSocketAgent::internalConnect()");
					}
					if (connectData.currentReconnectionDepth >= this->maxReconnectTries) {
						this->doWeQuit->store(true);
					} else {
						this->connections.push(connectData);
					}
					return;
				}

				std::string sendString{};
				if (this->discordCoreClient->theFormat == DiscordCoreAPI::TextFormat::Etf) {
					sendString = "GET /?v=10&encoding=etf HTTP/1.1\r\nHost: " + this->discordCoreClient->theAddress +
						"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
						DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				} else {
					sendString = "GET /?v=10&encoding=json HTTP/1.1\r\nHost: " + this->discordCoreClient->theAddress +
						"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
						DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				}
				theMap[connectData.currentShard]->writeData(sendString, true);
				int32_t currentDepth{ 0 };
				while (!this->doWeQuit->load()) {
					currentDepth += 1;
					if (*theMap[connectData.currentShard]->theState == WebSocketState::Connected) {
						break;
					}
					try {
						WebSocketSSLShard::processIO(theMap, 10000);
					} catch (...) {
						if (this->doWePrintErrorMessages) {
							DiscordCoreAPI::reportException("BaseSocketAgent::internalConnect()");
						}
						if (connectData.currentReconnectionDepth >= this->maxReconnectTries) {
							this->doWeQuit->store(true);
						} else {
							this->connections.push(connectData);
						}
						return;
					}
					if (theMap.contains(connectData.currentShard)) {
						if (theMap[connectData.currentShard]->inputBuffer->size() > 0) {
							this->parserAgent->submitStringForProcessing(*theMap[connectData.currentShard], connectData.currentShard);
							this->parserAgent->parseHeadersAndMessage(connectData.currentShard);
							this->parserAgent->onMessageReceived(connectData.currentShard);
						}
					}
					WebSocketCommand theCommand{};
					if (theMap[connectData.currentShard]->commandBuffer->tryReceive(theCommand)) {
						if (theCommand == WebSocketCommand::Send_Message) {
							this->parserAgent->collectOutputData(*theMap[connectData.currentShard], connectData.currentShard);
						}
					}
					std::this_thread::sleep_for(1ms);
				}
				int32_t currentCount{ 0 };
				while (currentCount < 10) {
					currentCount += 1;
					try {
						WebSocketSSLShard::processIO(theMap, 10000);
					} catch (...) {
						if (this->doWePrintErrorMessages) {
							DiscordCoreAPI::reportException("BaseSocketAgent::internalConnect()");
						}
						if (connectData.currentReconnectionDepth >= this->maxReconnectTries) {
							this->doWeQuit->store(true);
						} else {
							this->connections.push(connectData);
						}
						return;
					}
				}
				this->theClients[connectData.currentShard] = std::move(theMap[connectData.currentShard]);
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

	VoiceSocketAgent::VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew,WebSocketSSLShard& theIndex, bool printMessagesNew) noexcept {
		this->doWePrintSuccessMessages = baseBaseSocketAgentNew->doWePrintSuccessMessages;
		this->doWePrintErrorMessages = baseBaseSocketAgentNew->doWePrintErrorMessages;
		this->baseSocketAgent = baseBaseSocketAgentNew;
		this->baseSocketAgent->parserAgent->voiceConnectionDataBufferMap[initDataNew.guildId] = &this->voiceConnectionDataBuffer;
		this->baseSocketAgent->getVoiceConnectionData(initDataNew, theIndex);
		this->voiceConnectInitData = initDataNew;
		this->areWeConnected.reset();
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
	}

	void VoiceSocketAgent::parseHeadersAndMessage(WebSocketSSLShard& theShard) noexcept {
		if (*theShard.theState == WebSocketState::Connecting01) {
			std::string newVector{};
			if (theShard.inputBuffer->find("\r\n\r\n") != std::string::npos) {
				std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
				newVector.erase(0, newVector.find("\r\n\r\n") + 4);
				theShard.inputBuffer->clear();
				theShard.inputBuffer->insert(theShard.inputBuffer->end(), newVector.begin(), newVector.end());
				*theShard.theState = WebSocketState::Connecting02;
			}
		}
		if (theShard.inputBuffer->size() < 4) {
			return;
		}
		*theShard.opCode = static_cast<WebSocketOpCode>((*theShard.inputBuffer)[0] & ~webSocketFinishBit);
		switch (*theShard.opCode) {
			case WebSocketOpCode::Op_Continuation:
				[[fallthrough]];
			case WebSocketOpCode::Op_Text:
				[[fallthrough]];
			case WebSocketOpCode::Op_Binary:
				[[fallthrough]];
			case WebSocketOpCode::Op_Ping:
				[[fallthrough]];
			case WebSocketOpCode::Op_Pong: {
				uint8_t length01 = (*theShard.inputBuffer)[1];
				theShard.messageOffset->store(2);
				if (length01 & webSocketMaskBit) {
					return;
				}
				theShard.messageLength->store(length01);
				if (length01 == webSocketPayloadLengthMagicLarge) {
					if (theShard.inputBuffer->size() < 8) {
						return;
					}
					uint8_t length03 = (*theShard.inputBuffer)[2];
					uint8_t length04 = (*theShard.inputBuffer)[3];
					theShard.messageLength->store(static_cast<uint64_t>((length03 << 8) | length04));
					theShard.messageOffset->fetch_add(2);
				} else if (length01 == webSocketPayloadLengthMagicHuge) {
					if (theShard.inputBuffer->size() < 10) {
						return;
					}
					theShard.messageLength->store(0);
					for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
						uint8_t lengthNew = static_cast<uint8_t>((*theShard.inputBuffer)[x]);
						theShard.messageLength->fetch_add(static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift)));
					}
					theShard.messageOffset->fetch_add(8);
				}
				if (theShard.inputBuffer->size() < static_cast<uint64_t>(theShard.messageOffset->load()) + static_cast<uint64_t>(theShard.messageLength->load())) {
					return;
				} else {
					std::string finalMessage{};
					finalMessage.insert(finalMessage.begin(), theShard.inputBuffer->begin() + theShard.messageOffset->load(),
						theShard.inputBuffer->begin() + theShard.messageOffset->load() + theShard.messageLength->load());
					theShard.processedMessages->push(finalMessage);
					theShard.inputBuffer->erase(theShard.inputBuffer->begin(), theShard.inputBuffer->begin() + theShard.messageOffset->load() + theShard.messageLength->load());
					return;
				}
			}
			case WebSocketOpCode::Op_Close: {
				uint16_t close = (*theShard.inputBuffer)[2] & 0xff;
				close <<= 8;
				close |= (*theShard.inputBuffer)[3] & 0xff;
				*theShard.closeCode = static_cast<WebSocketCloseCode>(close);
				theShard.inputBuffer->erase(theShard.inputBuffer->begin(), theShard.inputBuffer->begin() + 4);
				this->onClosed(theShard);
			}
			default: {
			}
		}
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
				this->theClients[3]->writeData(theVectorNew, false);
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
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket " + theShard.shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(*theShard.closeCode)
					  << DiscordCoreAPI::reset() << std::endl
					  << std::endl;
		}
		this->theClients.erase(3);
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
				this->theClients[3]->writeData(dataToSend, false);
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
					*this->theClients[3]->theState = WebSocketState::Connected;
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
			while (!theToken.stop_requested() && !this->doWeQuit.load()) {
				if (this->theClients.contains(3) && this->heartbeatInterval != 0 && !this->areWeHeartBeating) {
					this->areWeHeartBeating = true;
					this->theClients[3]->heartBeatStopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
				}
				if (this->theClients.contains(3) && this->theClients[3]->heartBeatStopWatch.hasTimePassed() && this->areWeHeartBeating) {
					this->theClients[3]->heartBeatStopWatch.resetTimer();
					this->sendHeartBeat();
				}
				try {
					if (this->theClients.contains(3) && !this->doWeQuit.load()) {
						WebSocketSSLShard::processIO(this->theClients, 1000);
					}
				} catch (...) {
					if (this->doWePrintErrorMessages) {
						DiscordCoreAPI::reportException("VoiceSocketAgent::run()");
					}
				}
				if (this->theClients.contains(3) && this->theClients[3] != nullptr && !this->doWeQuit.load()) {
					this->parseHeadersAndMessage(*this->theClients[3]);
					if (this->theClients.contains(3) && this->theClients[3] != nullptr && this->theClients[3]->processedMessages->size() > 0) {
						this->onMessageReceived(this->theClients[3]->processedMessages->front());
						this->theClients[3]->processedMessages->pop();
					}
				} else {
					this->onClosedExternal();
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
			std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->theClients[3] = std::move(theClient);
			this->sendMessage(sendVector);
			try {
				WebSocketSSLShard::processIO(this->theClients);
			} catch (...) {
				if (this->doWePrintErrorMessages) {
					DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
				}
				return;
			}
			int32_t currentDepth{ 0 };
			while (!this->doWeQuit.load()) {
				currentDepth += 1;
				if (this->theClients.contains(3) && this->theClients[3]->inputBuffer->size() > 0) {
					this->parseHeadersAndMessage(*this->theClients[3]);
				}
				if (this->theClients[3]->processedMessages->size() > 0) {
					auto theMessage = this->theClients[3]->processedMessages->front();
					this->theClients[3]->processedMessages->pop();
					this->onMessageReceived(theMessage);
				}
				if (*this->theClients[3]->theState == WebSocketState::Connected) {
					break;
				}
				try {
					WebSocketSSLShard::processIO(this->theClients);
				} catch (...) {
					if (this->doWePrintErrorMessages) {
						DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
					}
					return;
				}
				std::this_thread::sleep_for(1ms);
				if (this->theClients.contains(3) && currentDepth >= 5000) {
					this->theClients.erase(3);
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
		this->doWeQuit.store(true);
		this->theTask->request_stop();
		if (this->theTask->joinable()) {
			this->theTask->join();
			this->theTask.reset(nullptr);
		}
	};
}