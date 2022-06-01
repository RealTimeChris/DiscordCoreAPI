/*
*
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

	constexpr uint16_t webSocketMaxPayloadLengthLarge{ 65535 };
	constexpr uint8_t webSocketPayloadLengthMagicLarge{ 126 };
	constexpr uint8_t webSocketPayloadLengthMagicHuge{ 127 };
	constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2 };
	constexpr uint8_t webSocketMaxPayloadLengthSmall{ 125 };
	constexpr uint8_t webSocketFinishBit{ (1u << 7u) };
	constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

	WSMessageCollector::WSMessageCollector(WebSocketSSLClient* thePtr){
		this->theClientPtr = thePtr;
	};

	WSMessageCollectorReturnData WSMessageCollector::collectFinalMessage() noexcept {
		WSMessageCollectorReturnData theMessage{};
		if (this->finalMessages.size() > 0) {
			theMessage = this->finalMessages.front();
			this->finalMessages.pop();
			this->theState = WSMessageCollectorState::Initializing;
		}
		return theMessage;
	}

	bool WSMessageCollector::runMessageCollector() noexcept {
		if (this->currentRecursionDepth == this->maxRecursionDepth) {
			this->currentRecursionDepth = 0;
			return true;
		}
		switch (this->theState) {
			case WSMessageCollectorState::Connecting: {
				this->collectData();
				return this->parseConnectionHeader();
			}
			case WSMessageCollectorState::Initializing: {
				this->dataOpCode = WebSocketOpCode::Op_Binary;
				this->theState = WSMessageCollectorState::Collecting;
				this->currentFinalMessage = WSMessageCollectorReturnData{};
				this->theOffsets.clear();
				return this->runMessageCollector();
			}
			case WSMessageCollectorState::Collecting: {
				return this->collectData();
			}
			case WSMessageCollectorState::Parsing: {
				return this->parseHeaderAndMessage();
			}
			case WSMessageCollectorState::Serving: {
				this->theState = WSMessageCollectorState::Initializing;
				return true;
			}
			default: {
				return true;
			}
		}
		return true;
	}

	bool WSMessageCollector ::parseConnectionHeader() noexcept {
		std::string newVector = this->currentMessage;
		if (newVector.find("\r\n\r\n") != std::string::npos) {
			std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
			newVector.erase(0, newVector.find("\r\n\r\n") + 4);
			this->theState = WSMessageCollectorState::Initializing;
			this->currentMessage.clear();
			return this->runMessageCollector();
		} else {
			this->theState = WSMessageCollectorState::Collecting;
			this->currentRecursionDepth += 1;
			return this->runMessageCollector();
		}
	}

	bool WSMessageCollector::parseHeaderAndMessage() noexcept {
		if (this->currentMessage.size() < 4) {
			this->theState = WSMessageCollectorState::Collecting;
			this->currentRecursionDepth += 1;
			return this->runMessageCollector();
		} else {
			this->dataOpCode = static_cast<WebSocketOpCode>(this->currentMessage[0] & ~webSocketFinishBit);
			std::cout << "WERE HERE THIS IS IT! THE OPCODE: " << +static_cast<uint8_t>(this->dataOpCode) << std::endl;
			switch (this->dataOpCode) {
				case WebSocketOpCode::Op_Continuation:
					[[fallthrough]];
				case WebSocketOpCode::Op_Text:
					[[fallthrough]];
				case WebSocketOpCode::Op_Binary:
					[[fallthrough]];
				case WebSocketOpCode::Op_Ping:
					[[fallthrough]];
				case WebSocketOpCode::Op_Pong: {
					uint8_t length01 = this->currentMessage[1];
					this->messageOffset = 2;
					if (length01 & webSocketMaskBit) {
						this->theState = WSMessageCollectorState::Collecting;
						this->currentRecursionDepth += 1;
						return this->runMessageCollector();
					}
					this->messageLength = length01;
					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (this->currentMessage.size() < 8) {
							this->theState = WSMessageCollectorState::Collecting;
							this->currentRecursionDepth += 1;
							return this->runMessageCollector();
						}
						uint8_t length03 = this->currentMessage[2];
						uint8_t length04 = this->currentMessage[3];
						this->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
						this->messageOffset += 2;
					} else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (this->currentMessage.size() < 10) {
							this->theState = WSMessageCollectorState::Collecting;
							this->currentRecursionDepth += 1;
							return this->runMessageCollector();
						}
						this->messageLength = 0;
						for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
							uint8_t lengthNew = static_cast<uint8_t>(this->currentMessage[x]);
							this->messageLength |= static_cast<uint64_t>((lengthNew & 0xff) << shift);
						}
						this->messageOffset += 8;
					}
					if (this->currentMessage.size() < static_cast<uint64_t>(this->messageOffset + this->messageLength)) {
						this->theState = WSMessageCollectorState::Collecting;
						this->currentRecursionDepth += 1;
						return this->runMessageCollector();
					} else {
						WSMessageCollectorReturnData returnData{};
						returnData.theMessage.insert(returnData.theMessage.begin(), this->currentMessage.begin() + this->messageOffset,
							this->currentMessage.begin() + this->messageOffset + this->messageLength);
						this->finalMessages.push(returnData);
						this->currentMessage.erase(this->currentMessage.begin(), this->currentMessage.begin() + this->messageOffset + this->messageLength);
						this->theState = WSMessageCollectorState::Serving;
						return this->runMessageCollector();
					}
					this->theState = WSMessageCollectorState::Collecting;
					this->currentRecursionDepth += 1;
					return this->runMessageCollector();
				}
				case WebSocketOpCode::Op_Close: {
					uint16_t close = this->currentMessage[2] & 0xff;
					close <<= 8;
					close |= this->currentMessage[3] & 0xff;
					while (this->finalMessages.size() > 0) {
						this->finalMessages.pop();
					}
					this->finalMessages.push(WSMessageCollectorReturnData{ .closeCode = static_cast<uint8_t>(close) });
					this->currentMessage.clear();
					this->theState = WSMessageCollectorState::Serving;
					return false;
				}
				default: {
					this->theState = WSMessageCollectorState::Collecting;
					this->currentRecursionDepth += 1;
					return true;
				}
			}
		}
	}

	bool WSMessageCollector::collectData() noexcept {
		if (this->theClientPtr != nullptr) {
			auto theBool = this->theClientPtr->processIO(100000);
			auto newMessage = this->theClientPtr->getInputBuffer();
			this->currentMessage.insert(this->currentMessage.end(), newMessage.begin(), newMessage.end());
			this->theOffsets.push_back(newMessage.size());
			this->theState = WSMessageCollectorState::Parsing;
			if (!theBool) {
				return theBool;
			} else {
				return this->runMessageCollector();
			}
		} else {
			return false;
		}
	}

	BaseSocketAgent::BaseSocketAgent(const std::string& botTokenNew, const std::string& baseUrl, DiscordCoreAPI::EventManager* eventManager,
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient, DiscordCoreAPI::CommandController* commandController, std::atomic_bool* doWeQuitNew, bool doWePrintSuccessMessages,
		bool doWePrintErrorMessages, int32_t shardNumber, int32_t numberOfShards) noexcept {
		this->printSuccessMessages = doWePrintSuccessMessages;
		this->printErrorMessages = doWePrintErrorMessages;
		this->commandController = commandController;
		this->discordCoreClient = discordCoreClient;
		this->shard = nlohmann::json::array();
		this->shard.push_back(shardNumber);
		this->shard.push_back(numberOfShards);
		this->eventManager = eventManager;
		this->botToken = botTokenNew;
		this->doWeQuit = doWeQuitNew;
		this->theFormat = this->discordCoreClient->theFormat;
		if (this->theFormat == DiscordCoreAPI::TextFormat::Etf) {
			this->dataOpcode = WebSocketOpCode::Op_Binary;
		} else {
			this->dataOpcode = WebSocketOpCode::Op_Text;
		}
		this->baseUrl = baseUrl;
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
		while (!this->areWeConnected.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
		}
	}

	BaseSocketAgent::BaseSocketAgent(nullptr_t) noexcept {};

	void BaseSocketAgent::sendMessage(const std::string& dataToSend) noexcept {
		try {
			std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
			if (this->printSuccessMessages) {
				std::lock_guard<std::mutex> theLock{ this->discordCoreClient->coutMutex };
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->shard.dump() + std::string("'s Message: ") << std::endl
						  << dataToSend << DiscordCoreAPI::reset();
			}
			this->webSocket->writeData(dataToSend);
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed();
		}
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->theTask.get();
	}

	void BaseSocketAgent::sendCloseFrame() noexcept {
		std::string theString{};
		theString.push_back(static_cast<int8_t>(WebSocketOpCode::Op_Close) | static_cast<int8_t>(webSocketFinishBit));
		theString.push_back(0);
		theString.push_back(static_cast<int8_t>(static_cast<uint16_t>(1000) >> 8));
		theString.push_back(static_cast<int8_t>(1000 & 0xff));
		if (this->webSocket != nullptr) {
			this->webSocket->writeData(theString);
			for (int32_t x = 0; x < 10; x += 1) {
				if (!this->webSocket->processIO(100000)) {
					break;
				}
			}
		}
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theStopWatch{ std::chrono::milliseconds{ 5000 } };
		while ((static_cast<int8_t>(this->dataOpcode) != (static_cast<int8_t>(WebSocketOpCode::Op_Close) | static_cast<int8_t>(webSocketFinishBit)))) {
			if (theStopWatch.hasTimePassed()) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			this->messageCollector.runMessageCollector();
			auto returnData = this->messageCollector.collectFinalMessage();
			if (returnData.theMessage != "") {
				break;
			}
		};
	}

	void BaseSocketAgent::sendMessage(const nlohmann::json& dataToSend, WebSocketOpCode opCode) noexcept {
		try {
			if (this->printSuccessMessages) {
				std::lock_guard<std::mutex> theLock{ this->discordCoreClient->coutMutex };
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->shard.dump() + std::string("'s Message: ") << dataToSend.dump() << std::endl
						  << DiscordCoreAPI::reset() << std::endl;
			}
			uint64_t theHeaderLength{};
			std::string theVector{};
			if (this->theFormat == DiscordCoreAPI::TextFormat::Etf) {
				theVector = this->erlPacker.parseJsonToEtf(dataToSend);
			} else {
				theVector = dataToSend.dump();
			}
			if (theVector.size() <= webSocketMaxPayloadLengthSmall) {
				theHeaderLength = 2;
			} else if (theVector.size() <= webSocketMaxPayloadLengthLarge) {
				theHeaderLength = 4;
			} else {
				theHeaderLength = 10;
			}
			uint64_t totalLength{ theVector.size() };
			std::vector<std::string> theStrings{};
			if (theVector.size() > (static_cast<uint64_t>(16) * 1024) - theHeaderLength) {
				uint64_t incrementAmount{ (static_cast<uint64_t>(16) * 1024) - theHeaderLength };
				uint64_t remainingBytes{ theVector.size() };
				while (remainingBytes > 0) {
					
					std::string theString{};
					theString.insert(theString.begin(), theVector.end() - remainingBytes, theVector.end() - remainingBytes + incrementAmount);
					remainingBytes -= incrementAmount;
					theStrings.push_back(std::move(theString));
					if (remainingBytes < (static_cast<uint64_t>(16) * 1024) - theHeaderLength) {
						incrementAmount = remainingBytes;
					}
				}
				
			} else {
				std::string theString{};
				theString.insert(theString.begin(), theVector.begin(), theVector.end());
				totalLength = theString.size();
				theStrings.push_back(std::move(theString));
			}
			for (uint64_t x = 0; x < static_cast<uint64_t>(theStrings.size()); x += 1) {
				std::string header{};
				if (x == 0) {
					this->createHeader(header, totalLength, opCode);
				}
				std::string theVectorNew{};
				theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
				theVectorNew.insert(theVectorNew.begin() + header.size(), theStrings[x].begin(), theStrings[x].end());
				this->webSocket->writeData(theVectorNew);
			}
			
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			}
			this->onClosed();
		}
	}

	void BaseSocketAgent::createHeader(std::string& outBuffer, uint64_t sendlength, WebSocketOpCode opCode) noexcept {
		try {
			uint32_t indexCount{ 0 };
			outBuffer.push_back(webSocketFinishBit | static_cast<uint8_t>(opCode));
			if (sendlength <= webSocketMaxPayloadLengthSmall) {
				outBuffer.push_back(static_cast<uint8_t>(sendlength));
			} else if (sendlength <= webSocketMaxPayloadLengthLarge) {
				outBuffer.push_back(static_cast<uint8_t>(webSocketPayloadLengthMagicLarge));
				indexCount = 2;
			} else {
				outBuffer.push_back(webSocketPayloadLengthMagicHuge);
				indexCount = 8;
			}
			for (int32_t x = indexCount - 1; x >= 0; x--) {
				outBuffer.push_back(static_cast<uint8_t>(sendlength >> x * 8));
			}
			outBuffer.push_back(static_cast<int8_t>(0 | webSocketMaskBit));
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::createHeader()");
			}
			this->onClosed();
		}
	}

	void BaseSocketAgent::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept {
		try {
			this->semaphore.acquire();
			DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
			dataPackage.channelId = 0;
			dataPackage.guildId = doWeCollect.guildId;
			dataPackage.selfDeaf = doWeCollect.selfDeaf;
			dataPackage.selfMute = doWeCollect.selfMute;
			this->userId = doWeCollect.userId;
			nlohmann::json newData = JSONIFY(dataPackage);
			this->sendMessage(newData, this->dataOpcode);
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
			if (doWeCollect.channelId == 0) {
				return;
			}
			dataPackage.channelId = doWeCollect.channelId;
			newData = JSONIFY(dataPackage);
			this->areWeCollectingData = true;
			this->sendMessage(newData, this->dataOpcode);
			while (this->areWeCollectingData) {
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}
			this->semaphore.release();
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
			}
			this->onClosed();
		}
	}

	void BaseSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			this->connect();
			DiscordCoreAPI::StopWatch stopWatch{ std::chrono::milliseconds{ 0 } };
			while (!theToken.stop_requested() && !this->doWeQuit->load()) {
				if (this->heartbeatInterval != 0 && !this->areWeHeartBeating) {
					this->areWeHeartBeating = true;
					stopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
				}
				if (stopWatch.hasTimePassed() && this->areWeHeartBeating) {
					stopWatch.resetTimer();
					this->sendHeartBeat();
				}
				if (!this->messageCollector.runMessageCollector()) {
					std::cout << "WERE HERE THIS IS IT!"<< std::endl;
					this->onClosed();
				}
				auto theReturnMessage = this->messageCollector.collectFinalMessage();
				if (theReturnMessage.theMessage != "") {
					this->onMessageReceived(theReturnMessage.theMessage);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}
			this->sendCloseFrame();
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			}
			this->onClosed();
		}
	}

	void BaseSocketAgent::onMessageReceived(std::string theMessage) noexcept {
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

			if (payload["s"] >= 0) {
				this->lastNumberReceived = payload["s"];
			}

			if (payload["t"] == "RESUMED") {
				this->areWeConnected.store(true);
				this->currentReconnectTries = 0;
				this->areWeReadyToConnectEvent.set();
			}

			if (payload["t"] == "READY") {
				this->areWeConnected.store(true);
				this->sessionId = payload["d"]["session_id"];
				DiscordCoreAPI::UserData theUser{};
				DataParser::parseObject(payload["d"]["user"], theUser);
				this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ theUser, this };
				DiscordCoreAPI::Users::insertUser(theUser);
				std::vector<DiscordCoreAPI::GuildData> theGuilds{};
				DataParser::parseObject(payload["d"]["guilds"], theGuilds);
				for (auto& value: theGuilds) {
					value.discordCoreClient = this->discordCoreClient;
					DiscordCoreAPI::Guilds::insertGuild(value);
				}
				this->currentReconnectTries = 0;
				this->areWeReadyToConnectEvent.set();
				this->areWeAuthenticated = true;
			}

			if (payload["op"] == 1) {
				this->sendHeartBeat();
			}

			if (payload["op"] == 7) {
				if (this->printSuccessMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->shard.dump() + " Reconnecting (Type 7)!" << std::endl
							  << DiscordCoreAPI::reset() << std::endl;
				}
				this->areWeResuming = true;
				this->currentReconnectTries += 1;
				this->areWeConnected.store(false);
				this->webSocket.reset(nullptr);
				this->connect();
			}

			if (payload["op"] == 9) {
				if (this->printSuccessMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->shard.dump() + " Reconnecting (Type 9)!" << std::endl
							  << DiscordCoreAPI::reset() << std::endl;
				}
				this->currentReconnectTries += 1;
				std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
				int32_t numOfMsToWait =
					static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
				std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
				if (payload["d"] == true) {
					nlohmann::json identityJson = JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue), this->shard[0], this->shard[1]);
					this->sendMessage(identityJson, this->dataOpcode);
				} else {
					this->areWeConnected.store(false);
					this->webSocket.reset(nullptr);
					this->areWeResuming = false;
					this->areWeAuthenticated = false;
					this->connect();
				}
			}

			if (payload["op"] == 10) {
				this->heartbeatInterval = payload["d"]["heartbeat_interval"];
				this->areWeHeartBeating = false;
				if (!this->areWeAuthenticated) {
					nlohmann::json identityJson = JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue), this->shard[0], this->shard[1]);
					this->sendMessage(identityJson, this->dataOpcode);
				}
				if (this->areWeResuming) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
					nlohmann::json resumePayload = JSONIFY(this->botToken, this->sessionId, this->lastNumberReceived);
					this->sendMessage(resumePayload, this->dataOpcode);
				}
			}
			if (payload["op"] == 11) {
				this->haveWeReceivedHeartbeatAck = true;
			}

			if (payload.contains("d") && !payload["d"].is_null() && payload.contains("t") && !payload["t"].is_null()) {
				if (payload["t"] == "APPLICATION_COMMAND_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandCreationEvent(*dataPackage);
				} else if (payload["t"] == "APPLICATION_COMMAND_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandUpdateEvent(*dataPackage);
				} else if (payload["t"] == "APPLICATION_COMMAND_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandDeletionEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->channel);
					this->eventManager->onChannelCreationEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>() };
					if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
						dataPackage->channelOld = DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(payload["d"]["id"].get<std::string>()) }).get();
						dataPackage->channelNew = dataPackage->channelOld;
					}
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->channelNew);
					this->eventManager->onChannelUpdateEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->channel);
					this->eventManager->onChannelDeletionEvent(*dataPackage);
				} else if (payload["t"] == "CHANNEL_PINS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->dataPackage);
					this->eventManager->onChannelPinsUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->thread);
					this->eventManager->onThreadCreationEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_UPATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->thread);
					this->eventManager->onThreadUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->thread);
					this->eventManager->onThreadDeletionEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_LIST_SYNC") {
					std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->threadListSyncData);
					this->eventManager->onThreadListSyncEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_MEMBER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->threadMember);
					this->eventManager->onThreadMemberUpdateEvent(*dataPackage);
				} else if (payload["t"] == "THREAD_MEMBERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->threadMembersUpdateData);
					this->eventManager->onThreadMembersUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_CREATE") {
					DiscordCoreAPI::GuildData guildNew{};
					std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], guildNew);
					guildNew.discordCoreClient = this->discordCoreClient;
					dataPackage->guild = guildNew;
					this->eventManager->onGuildCreationEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>() };
					if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
						dataPackage->guildOld = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(payload["d"]["id"].get<std::string>()) }).get();
						dataPackage->guildNew = dataPackage->guildOld;
					}
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->guildNew);
					this->eventManager->onGuildUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->guild);
					this->eventManager->onGuildDeletionEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_BAN_ADD") {
					std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
						DiscordCoreAPI::User newData{};
						DiscordCoreInternal::DataParser::parseObject(payload["d"]["user"], newData);
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
						DiscordCoreInternal::DataParser::parseObject(payload["d"]["user"], newData);
						dataPackage->user = newData;
					}
					this->eventManager->onGuildBanRemoveEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_EMOJIS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->updateData);
					this->eventManager->onGuildEmojisUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_STICKERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->updateData);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->guildMember);
					dataPackage->discordCoreClient = this->discordCoreClient;
					this->eventManager->onGuildMemberAddEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_MEMBER_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
						DiscordCoreAPI::User newData{};
						DiscordCoreInternal::DataParser::parseObject(payload["d"]["user"], newData);
						dataPackage->user = newData;
					}
					this->eventManager->onGuildMemberRemoveEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_MEMBER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildMemberNew.guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("user") && payload["d"]["user"].contains("id")) {
						dataPackage->guildMemberOld = DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync(
							{ .guildMemberId = stoull(payload["d"]["user"]["id"].get<std::string>()), .guildId = stoull(payload["d"]["guild_id"].get<std::string>()) })
														  .get();
						dataPackage->guildMemberNew = dataPackage->guildMemberOld;
					}
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->guildMemberNew);
					this->eventManager->onGuildMemberUpdateEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_MEMBERS_CHUNK") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->chunkEventData);
					this->eventManager->onGuildMembersChunkEvent(*dataPackage);
				} else if (payload["t"] == "GUILD_ROLE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>() };
					if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
						DiscordCoreInternal::DataParser::parseObject(payload["d"]["role"], dataPackage->role);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"]["role"], dataPackage->roleNew);
					if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
						dataPackage->roleOld =
							DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role_id"].get<std::string>()) }).get();
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
						DiscordCoreInternal::DataParser::parseObject(payload["d"]["integration"], dataPackage->integrationData);
					}
					this->eventManager->onIntegrationCreationEvent(*dataPackage);
				} else if (payload["t"] == "INTEGRATION_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
					if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
						dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
					}
					if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
						DiscordCoreInternal::DataParser::parseObject(payload["d"]["integration"], dataPackage->integrationData);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"], *interactionData);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->invite);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"], *message);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->messageNew);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->reaction);
					this->eventManager->onReactionAddEvent(*dataPackage);
				} else if (payload["t"] == "MESSAGE_REACTION_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->reactionRemoveData);
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
					DiscordCoreInternal::DataParser::parseObject(payload["d"]["emoji"], dataPackage->emoji);
					this->eventManager->onReactionRemoveEmojiEvent(*dataPackage);
				} else if (payload["t"] == "PRESENCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->presenceData);
					this->eventManager->onPresenceUpdateEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->stageInstance);
					this->eventManager->onStageInstanceCreationEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->stageInstance);
					this->eventManager->onStageInstanceUpdateEvent(*dataPackage);
				} else if (payload["t"] == "STAGE_INSTANCE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->stageInstance);
					this->eventManager->onStageInstanceDeletionEvent(*dataPackage);
				} else if (payload["t"] == "TYPING_START") {
					std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->typingStartData);
					this->eventManager->onTypingStartEvent(*dataPackage);
				} else if (payload["t"] == "USER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
					DiscordCoreAPI::User newData{};
					DiscordCoreInternal::DataParser::parseObject(payload["d"]["user"], newData);
					dataPackage->userNew = newData;
					dataPackage->userOld = DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get();
					this->eventManager->onUserUpdateEvent(*dataPackage);
				} else if (payload["t"] == "VOICE_STATE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(payload["d"], dataPackage->voiceStateData);
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
			if (this->printSuccessMessages) {
				std::lock_guard<std::mutex> theLock{ this->discordCoreClient->coutMutex };
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from WebSocket " + this->shard.dump() + std::string(": ") << payload.dump() << std::endl
						  << DiscordCoreAPI::reset() << std::endl;
			}
			return;
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
			}
			this->onClosed();
			return;
		}
	}

	void BaseSocketAgent::sendHeartBeat() noexcept {
		try {
			if (this->haveWeReceivedHeartbeatAck) {
				nlohmann::json heartbeat = JSONIFY(this->lastNumberReceived);
				this->sendMessage(heartbeat, this->dataOpcode);
				this->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosed();
			}
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::sendHeartBeat()");
			}
			this->onClosed();
		}
	}

	void BaseSocketAgent::onClosed() noexcept {
		this->areWeReadyToConnectEvent.reset();
		if (this->maxReconnectTries > this->currentReconnectTries) {
			if (this->printErrorMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket Closed; Code: " << this->closeCode << DiscordCoreAPI::reset() << std::endl;
			}
			this->closeCode = 0;
			this->sendCloseFrame();
			this->areWeConnected.store(false);
			this->currentReconnectTries += 1;
			this->areWeAuthenticated = false;
			this->haveWeReceivedHeartbeatAck = true;
			this->webSocket.reset(nullptr);
			if (this->closeCode & static_cast<int16_t>(ReconnectPossible::Yes)) {
				this->connect();
			} else {
				this->doWeQuit->store(true);
			}
		} else if (this->maxReconnectTries <= this->currentReconnectTries) {
			this->theTask->request_stop();
		}
	}

	WebSocketOpCode BaseSocketAgent::getOpCode() noexcept {
		return this->dataOpcode;
	}

	void BaseSocketAgent::connect() noexcept {
		try {
			this->webSocket = std::make_unique<WebSocketSSLClient>(this->baseUrl, "443", this->printErrorMessages);
			this->messageCollector = WSMessageCollector{ this->webSocket.get() };
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
			this->sendMessage(sendString);
			std::string theResult{};
			while (theResult == "") {
				this->messageCollector.runMessageCollector();
				theResult = this->messageCollector.collectFinalMessage().theMessage;
			}
			this->onMessageReceived(theResult);
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("BaseSocketAgent::connect()");
			}
			this->onClosed();
		}
	}

	BaseSocketAgent::~BaseSocketAgent() noexcept {
		this->theTask->request_stop();
		if (this->theTask->joinable()) {
			this->theTask->join();
		}
	}

	VoiceSocketAgent::VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, bool printMessagesNew) noexcept {
		this->baseSocketAgent = baseBaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->printSuccessMessages = baseBaseSocketAgentNew->printSuccessMessages;
		this->printErrorMessages = baseBaseSocketAgentNew->printErrorMessages;
		this->baseSocketAgent->voiceConnectionDataBufferMap.insert_or_assign(std::to_string(this->voiceConnectInitData.guildId), &this->voiceConnectionDataBuffer);
		this->baseSocketAgent->getVoiceConnectionData(this->voiceConnectInitData);
		this->areWeConnected.reset();
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
	}

	void VoiceSocketAgent::sendVoiceData(std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->printErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Please specify voice data to send" << std::endl << DiscordCoreAPI::reset() << std::endl;
				}
				return;
			} else {
				if (!this->voiceSocket->writeData(responseData)) {
					this->onClosed();
				}
			}
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendVoiceData()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::sendMessage(const std::vector<uint8_t>& dataToSend) noexcept {
		try {
			std::string newString{};
			newString.insert(newString.begin(), dataToSend.begin(), dataToSend.end());
			if (this->printSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << newString << std::endl << DiscordCoreAPI::reset() << std::endl;
			}
			std::vector<char> out{};
			out.resize(maxHeaderSize);
			size_t size = this->createHeader(out.data(), newString.size(), this->dataOpcode);
			std::string header(out.data(), size);
			std::string theVectorNew{};
			theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
			theVectorNew.insert(theVectorNew.begin() + header.size(), newString.begin(), newString.end());
			this->webSocket->writeData(theVectorNew);
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::sendMessage(const std::string& dataToSend) noexcept {
		try {
			if (this->printSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << std::endl << dataToSend << DiscordCoreAPI::reset();
			}
			this->webSocket->writeData(dataToSend);
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosed();
		}
	}

	uint64_t VoiceSocketAgent::createHeader(char* outBuffer, uint64_t sendlength, WebSocketOpCode opCode) noexcept {
		try {
			size_t position = 0;
			outBuffer[position++] = webSocketFinishBit | static_cast<unsigned char>(opCode);
			if (sendlength <= webSocketMaxPayloadLengthSmall) {
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			} else if (sendlength <= webSocketMaxPayloadLengthLarge) {
				outBuffer[position++] = static_cast<unsigned char>(webSocketPayloadLengthMagicLarge);
				outBuffer[position++] = static_cast<unsigned char>(sendlength >> 8);
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			} else {
				outBuffer[position++] = webSocketPayloadLengthMagicHuge;
				const uint64_t length02 = sendlength;
				for (int32_t x = sizeof(uint64_t) - 1; x >= 0; x--) {
					outBuffer[position++] = static_cast<unsigned char>(length02 >> x * 8);
				}
			}
			outBuffer[1] |= webSocketMaskBit;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			return position;
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::createHeader()");
			}
			this->onClosed();
			return size_t{};
		}
	}

	void VoiceSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			this->connect();
			DiscordCoreAPI::StopWatch stopWatch{ std::chrono::milliseconds{ 0 } };
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
					this->onClosed();
				}
				auto theReturnMessage = this->messageCollector.collectFinalMessage();
				if (theReturnMessage.theMessage != "") {
					this->onMessageReceived(theReturnMessage.theMessage);
				}
				if (this->voiceSocket) {
					this->voiceSocket->readData(true);
				}
			}
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::run()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::onMessageReceived(std::string theMessage) noexcept {
		try {
			std::string message = theMessage;
			nlohmann::json payload = payload.parse(message);
			if (this->printSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from Voice WebSocket: " << message << std::endl << DiscordCoreAPI::reset() << std::endl;
			}
			if (payload.contains("op")) {
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
					if (this->webSocket) {
						this->sendMessage(protocolPayloadSelectString);
					}
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
					if (this->webSocket) {
						this->sendMessage(identifyPayload);
					}
				}
			}
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::onMessageReceived()");
			}
			this->onClosed();
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
			if (!this->voiceSocket->writeData(packet)) {
				this->onClosed();
				return;
			};
			while (this->voiceSocket->getInputBuffer().size() < 74) {
				this->voiceSocket->readData(false);
			}
			std::string message{};
			message.insert(message.begin(), this->voiceSocket->getInputBuffer().begin() + 8, this->voiceSocket->getInputBuffer().begin() + 64);
			if (message.find('\u0000') != std::string::npos) {
				message = message.substr(0, message.find('\u0000', 5));
			}
			this->voiceSocket->getInputBuffer().clear();
			this->voiceConnectionData.externalIp = message;
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::collectExternalIP()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::sendHeartBeat() noexcept {
		try {
			if (this->haveWeReceivedHeartbeatAck) {
				std::vector<uint8_t> heartbeatPayload = JSONIFY(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				if (this->webSocket) {
					this->sendMessage(heartbeatPayload);
				}
				this->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosed();
			}
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendHeartBeat()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::voiceConnect() noexcept {
		try {
			this->voiceSocket = std::make_unique<DatagramSocketSSLClient>(this->voiceConnectionData.voiceIp, this->voiceConnectionData.voicePort, this->printErrorMessages);
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::voiceConnect()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::onClosed() noexcept {
		this->doWeReconnect.store(true);
	}

	void VoiceSocketAgent::onClosedInternal() noexcept {
		if (this->printErrorMessages) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "Voice WebSocket Closed; Code: " << this->closeCode << DiscordCoreAPI::reset() << std::endl;
		}
		this->closeCode = 0;
		this->voiceSocket.reset(nullptr);
		this->webSocket.reset(nullptr);
	}

	void VoiceSocketAgent::connect() noexcept {
		try {
			DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 20000);
			this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
			this->webSocket = std::make_unique<WebSocketSSLClient>(this->baseUrl, "443", this->printErrorMessages);
			this->messageCollector = WSMessageCollector{ this->webSocket.get() };
			std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->sendMessage(sendVector);
			std::string theResult{};
			while (theResult == "") {
				this->messageCollector.runMessageCollector();
				theResult = this->messageCollector.collectFinalMessage().theMessage;
			}
			this->onMessageReceived(theResult);
		} catch (...) {
			if (this->printErrorMessages) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
			}
			this->onClosed();
		}
	}

	VoiceSocketAgent::~VoiceSocketAgent() noexcept {
		this->theTask->request_stop();
		if (this->theTask->joinable()) {
			this->theTask->join();
			this->theTask.reset(nullptr);
		}
		this->baseSocketAgent->voiceConnectionDataBufferMap.erase(std::to_string(this->voiceConnectInitData.guildId));
	};
}