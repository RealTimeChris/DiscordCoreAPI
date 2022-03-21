/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

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
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file WebSocketEntities.cpp

#include <WebSocketEntities.hpp>
#include <JSONIfier.hpp>
#include <EventManager.hpp>
#include <DiscordCoreClient.hpp>

namespace DiscordCoreInternal {

	constexpr unsigned char webSocketPayloadLengthMagicLarge{ 126 };
	constexpr unsigned char webSocketPayloadLengthMagicHuge{ 127 };
	constexpr uint64_t webSocketMaxPayloadLengthLarge{ 65535 };
	constexpr unsigned char webSocketFinishBit{ (1u << 7u) };
	constexpr uint64_t webSocketMaxPayloadLengthSmall{ 125 };
	constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2 };
	constexpr unsigned char webSocketMaskBit{ (1u << 7u) };

	BaseSocketAgent::BaseSocketAgent(std::string botTokenNew, std::string baseUrl, DiscordCoreAPI::EventManager* eventManager, DiscordCoreAPI::DiscordCoreClient* discordCoreClient, DiscordCoreAPI::CommandController* commandController, std::atomic_bool* doWeQuitNew, bool doWePrintMessages , int32_t shardNumber , int32_t numberOfShards ) noexcept{
		this->doWeQuit = doWeQuitNew;
		this->commandController = commandController;
		this->discordCoreClient = discordCoreClient;
		this->eventManager = eventManager;
		this->printMessages = doWePrintMessages;
		this->currentShard = shardNumber;
		this->numOfShards = numberOfShards;
		this->state = WebSocketState::Initializing;
		this->botToken = botTokenNew;
		this->baseUrl = baseUrl;
		this->doWeReconnect.set();
		this->theTask = std::make_unique<DiscordCoreAPI::CoRoutine<void>>(this->run());
	}

	BaseSocketAgent::BaseSocketAgent(nullptr_t) noexcept {};

	void BaseSocketAgent::sendMessage(std::string& dataToSend) noexcept {
		try {
			std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
			if (this->printMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket DiscordCoreAPIMessage: " << std::endl << dataToSend << DiscordCoreAPI::reset();
			}
			this->webSocket->writeData(dataToSend);
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	void BaseSocketAgent::sendMessage(nlohmann::json& dataToSend) noexcept {
		try {
			DiscordCoreAPI::StopWatch stopWatch{ std::chrono::milliseconds{5500} };
			while (!this->areWeConnected.load(std::memory_order_seq_cst) && !(dataToSend.contains("op") && (dataToSend.at("op") == 2 || dataToSend.at("op") == 6))) {
				if (stopWatch.hasTimePassed()) {
					return;
				}
			}
			std::lock_guard<std::mutex> accessLock{ this->accessorMutex01 };
			if (this->printMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket DiscordCoreAPIMessage: " << dataToSend.dump() << std::endl << DiscordCoreAPI::reset() << std::endl;
			}
			std::string theVector = this->erlPacker.parseJsonToEtf(dataToSend);
			std::string out{};
			out.resize(maxHeaderSize);
			size_t size = this->createHeader(out.data(), theVector.size(), this->dataOpcode);
			std::string header(out.data(), size);
			std::string theVectorNew{};
			theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
			theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
			this->webSocket->writeData(theVectorNew);
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	uint64_t BaseSocketAgent::createHeader(char* outBuffer, uint64_t sendlength, WebSocketOpCode opCode) noexcept {
		try {
			size_t position{ 0 };
			int32_t indexCount{ 0 };
			outBuffer[position++] = webSocketFinishBit | static_cast<unsigned char>(opCode);
			if (sendlength <=webSocketMaxPayloadLengthSmall) {
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			}
			else if (sendlength <= webSocketMaxPayloadLengthLarge) {
				outBuffer[position++] = static_cast<unsigned char>(webSocketPayloadLengthMagicLarge);
				indexCount = 2;
			}
			else {
				outBuffer[position++] = webSocketPayloadLengthMagicHuge;
				indexCount = 8;
			}
			for (int32_t x = indexCount - 1; x >= 0; x--) {
				outBuffer[position++] = static_cast<unsigned char>(sendlength >> x * 8);
			}
			outBuffer[1] |= webSocketMaskBit;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			return position;
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::createHeader()");
			this->onClosedExternal();
			return uint64_t{};
		}
	}

	std::vector<std::string> BaseSocketAgent::tokenize(std::string& dataIn, std::string separator) noexcept {
		try {
			std::string::size_type value{ 0 };
			std::vector<std::string> dataOut{};
			while ((value = dataIn.find_first_not_of(separator, value)) != std::string::npos) {
				auto output = dataIn.find(separator, value);
				dataOut.push_back(dataIn.substr(value, output - value));
				value = output;
			}
			return dataOut;
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::tokenize()");
			this->onClosedExternal();
			return std::vector<std::string>{};
		}
	}

	void BaseSocketAgent::getVoiceConnectionData(VoiceConnectInitData doWeCollect) noexcept {
		try {
			this->semaphore.acquire();
			this->voiceConnectInitData = doWeCollect;
			DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
			dataPackage.channelId = "";
			dataPackage.guildId = this->voiceConnectInitData.guildId;
			dataPackage.selfDeaf = this->voiceConnectInitData.selfDeaf;
			dataPackage.selfMute = this->voiceConnectInitData.selfMute;
			nlohmann::json newData = JSONIFY(dataPackage);
			this->sendMessage(newData);
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
			dataPackage.channelId = this->voiceConnectInitData.channelId;
			newData = JSONIFY(dataPackage);
			this->areWeCollectingData = true;
			this->sendMessage(newData);
			while (this->areWeCollectingData) {
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}
			this->semaphore.release();
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
			this->onClosedExternal();
		}
	}

	DiscordCoreAPI::CoRoutine<void> BaseSocketAgent::run() noexcept {
		try {
			auto cancelHandle = co_await DiscordCoreAPI::NewThreadAwaitable<void>();
			this->connect();
			DiscordCoreAPI::StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{0} };
			while (!cancelHandle.promise().isItStopped() && !this->doWeQuit->load(std::memory_order_seq_cst)) {				
					if (this->heartbeatInterval != 0 && !this->areWeHeartBeating) {
						this->areWeHeartBeating = true;
						stopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{this->heartbeatInterval} };
					}
					if (stopWatch.hasTimePassed() && this->areWeHeartBeating) {
						stopWatch.resetTimer();
						this->sendHeartBeat();
					}
				if (!this->doWeReconnect.wait(0)) {
					this->onClosedInternal();
				}
				if (this->webSocket != nullptr) {
					if (!this->webSocket->processIO(1000)) {
						this->onClosedExternal();
					}
					this->handleBuffer();
				}
			}
			co_return;
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			this->onClosedExternal();
			co_return;
		}
	}

	void BaseSocketAgent::onMessageReceived() noexcept {
		try {
			std::string messageNew = this->webSocket->getInputBuffer();
			this->webSocket->getInputBuffer().clear();
			nlohmann::json payload{};

			try {
				payload = this->erlPacker.parseEtfToJson(&messageNew);
			}
			catch (...) {
				return;
			}

			if (this->areWeCollectingData && payload.at("t") == "VOICE_SERVER_UPDATE" && !this->serverUpdateCollected) {
				if (!this->serverUpdateCollected && !this->stateUpdateCollected) {
					this->voiceConnectionData = VoiceConnectionData();
					this->voiceConnectionData.endPoint = std::move(payload.at("d")).at("endpoint").get<std::string>();
					this->voiceConnectionData.token = std::move(payload.at("d")).at("token").get<std::string>();
					this->serverUpdateCollected = true;
				}
				else {
					this->voiceConnectionData.endPoint = std::move(payload.at("d")).at("endpoint").get<std::string>();
					this->voiceConnectionData.token = std::move(payload.at("d")).at("token").get<std::string>();
					if (this->voiceConnectionDataBufferMap.contains(std::move(payload.at("d")).at("guild_id"))) {
						this->voiceConnectionDataBufferMap.at(std::move(payload.at("d")).at("guild_id"))->send(this->voiceConnectionData);
					}
					this->serverUpdateCollected = false;
					this->stateUpdateCollected = false;
					this->areWeCollectingData = false;
				}
			}

			if (this->areWeCollectingData && payload.at("t") == "VOICE_STATE_UPDATE" && !this->stateUpdateCollected && std::move(payload.at("d")).at("member").at("user").at("id") == this->voiceConnectInitData.userId) {
				if (!this->stateUpdateCollected && !this->serverUpdateCollected) {
					this->voiceConnectionData = VoiceConnectionData();
					this->voiceConnectionData.sessionId = std::move(payload.at("d")).at("session_id").get<std::string>();
					this->stateUpdateCollected = true;
				}
				else {
					this->voiceConnectionData.sessionId = std::move(payload.at("d")).at("session_id").get<std::string>();
					if (this->voiceConnectionDataBufferMap.contains(std::move(payload.at("d")).at("guild_id"))) {
						this->voiceConnectionDataBufferMap.at(std::move(payload.at("d")).at("guild_id"))->send(this->voiceConnectionData);
					}
					this->serverUpdateCollected = false;
					this->stateUpdateCollected = false;
					this->areWeCollectingData = false;
				}
			}

			if (payload.at("s") >= 0) {
				this->lastNumberReceived = payload.at("s");
			}

			if (payload.at("t") == "RESUMED") {
				this->areWeConnected.store(true, std::memory_order_seq_cst);
				this->currentReconnectTries = 0;
				this->areWeReadyToConnectEvent.set();
			}

			if (payload.at("t") == "READY") {
				this->areWeConnected.store(true, std::memory_order_seq_cst);
				this->sessionId = std::move(payload.at("d")).at("session_id");
				this->currentReconnectTries = 0;
				this->areWeReadyToConnectEvent.set();
				this->areWeAuthenticated = true;
			}

			if (payload.at("op") == 1) {
				this->sendHeartBeat();
			}

			if (payload.at("op") == 7) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard [" + std::to_string(this->currentShard) + ", " + std::to_string(this->numOfShards) + "] - Reconnecting (Type 7)!" << std::endl << DiscordCoreAPI::reset() << std::endl;
				this->areWeResuming = true;
				this->currentReconnectTries += 1;
				this->areWeConnected.store(false, std::memory_order_seq_cst);
				this->webSocket.reset(nullptr);
				this->connect();
			}

			if (payload.at("op") == 9) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard [" + std::to_string(this->currentShard) + ", " + std::to_string(this->numOfShards) + "] - Reconnecting (Type 9)!" << std::endl << DiscordCoreAPI::reset() << std::endl;
				this->currentReconnectTries += 1;
				std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
				int32_t numOfMsToWait = static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
				std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
				if (std::move(payload.at("d")) == true) {
					nlohmann::json identityJson = JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue), this->currentShard, this->numOfShards);
					this->sendMessage(identityJson);
				}
				else {
					this->areWeConnected.store(false, std::memory_order_seq_cst);
					this->webSocket.reset(nullptr);
					this->areWeResuming = false;
					this->areWeAuthenticated = false;
					this->connect();
				}
			}

			if (payload.at("op") == 10) {
				this->heartbeatInterval = std::move(payload.at("d")).at("heartbeat_interval");
				this->areWeHeartBeating = false;
				if (!this->areWeAuthenticated) {
					nlohmann::json identityJson = JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue), this->currentShard, this->numOfShards);
					this->sendMessage(identityJson);
				}
				if (this->areWeResuming) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
					nlohmann::json resumePayload = JSONIFY(this->botToken, this->sessionId, this->lastNumberReceived);
					this->sendMessage(resumePayload);
				}
			}

			if (payload.at("op") == 11) {
				this->haveWeReceivedHeartbeatAck = true;
			}

			if (payload.contains("d") && !payload.at("d").is_null() && payload.contains("t") && !payload.at("t").is_null()) {
				if (payload.at("t") == "APPLICATION_COMMAND_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t")=="APPLICATION_COMMAND_UPDATE"){
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "APPLICATION_COMMAND_DELETE")
				{
					std::unique_ptr<DiscordCoreAPI::OnApplicationCommandDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnApplicationCommandDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->applicationCommand);
					this->eventManager->onApplicationCommandDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "CHANNEL_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->channel);
					this->eventManager->onChannelCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "CHANNEL_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>() };
					if (std::move(payload.at("d")).contains("id")) {
						dataPackage->channelOld = DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = std::move(payload.at("d"))["id"] }).get();
						dataPackage->channelNew = dataPackage->channelOld;
					}
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->channelNew);
					this->eventManager->onChannelUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "CHANNEL_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->channel);
					this->eventManager->onChannelDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "CHANNEL_PINS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->dataPackage);
					this->eventManager->onChannelPinsUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "THREAD_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->thread);
					this->eventManager->onThreadCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "THREAD_UPATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->thread);
					this->eventManager->onThreadUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "THREAD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->thread);
					this->eventManager->onThreadDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "THREAD_LIST_SYNC") {
					std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->threadListSyncData);
					this->eventManager->onThreadListSyncEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "THREAD_MEMBER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->threadMember);
					this->eventManager->onThreadMemberUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "THREAD_MEMBERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->threadMembersUpdateData);
					this->eventManager->onThreadMembersUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_CREATE") {
					DiscordCoreAPI::Guild guildNew{};
					std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), guildNew);
					guildNew.discordCoreClient = this->discordCoreClient;
					dataPackage->guild = guildNew;
					this->eventManager->onGuildCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>() };
					if (std::move(payload.at("d")).contains("id")) {
						dataPackage->guildOld = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = std::move(payload.at("d"))["id"] }).get();
						dataPackage->guildNew = dataPackage->guildOld;
					}
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->guildNew);
					this->eventManager->onGuildUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->guild);
					this->eventManager->onGuildDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_BAN_ADD") {
					std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
					if (std::move(payload.at("d")).contains("user")) {
						DiscordCoreAPI::UserData newData{};
						DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["user"], newData);
						dataPackage->user = newData;
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onGuildBanAddEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_BAN_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>() };
					if (std::move(payload.at("d")).contains("user")) {
						DiscordCoreAPI::UserData newData{};
						DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["user"], newData);
						dataPackage->user = newData;
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onGuildBanRemoveEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_EMOJIS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->updateData);
					this->eventManager->onGuildEmojisUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_STICKERS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->updateData);
					this->eventManager->onGuildStickersUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_INTEGRATIONS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>() };
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onGuildIntegrationsUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_MEMBER_ADD") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberAddData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->guildMember);
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildMember.guildId = std::move(payload.at("d"))["guild_id"];
					}
					dataPackage->discordCoreClient = this->discordCoreClient;
					this->eventManager->onGuildMemberAddEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_MEMBER_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>() };
					if (std::move(payload.at("d")).contains("user")) {
						DiscordCoreAPI::UserData newData{};
						DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["user"], newData);
						dataPackage->user = newData;
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onGuildMemberRemoveEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_MEMBER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>() };
					if (std::move(payload.at("d")).contains("user") && std::move(payload.at("d"))["user"].contains("id")) {
						dataPackage->guildMemberOld = DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = std::move(payload.at("d"))["user"].at("id"),.guildId = std::move(payload.at("d"))["guild_id"] }).get();
						dataPackage->guildMemberNew = dataPackage->guildMemberOld;
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildMemberNew.guildId = std::move(payload.at("d"))["guild_id"];
					}
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->guildMemberNew);
					this->eventManager->onGuildMemberUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_MEMBERS_CHUNK") {
					std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->chunkEventData);
					this->eventManager->onGuildMembersChunkEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_ROLE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>() };
					if (std::move(payload.at("d")).contains("Role")) {
						DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["Role"], dataPackage->role);
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onRoleCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_ROLE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleUpdateData>() };
					if (std::move(payload.at("d")).contains("Role")) {
						dataPackage->roleOld = DiscordCoreAPI::Roles::getCachedRoleAsync({ .roleId = std::move(payload.at("d"))["Role"].at("id") }).get();
						dataPackage->roleNew = dataPackage->roleOld;
					}
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["Role"], dataPackage->roleNew);
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onRoleUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "GUILD_ROLE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleDeletionData>() };
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("role_id")) {
						dataPackage->roleOld = DiscordCoreAPI::Roles::getCachedRoleAsync({ .roleId = std::move(payload.at("d"))["role_id"] }).get();
					}
					this->eventManager->onRoleDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "INTEGRATION_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("integration")) {
						DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["integration"], dataPackage->integrationData);
					}
					this->eventManager->onIntegrationCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "INTEGRATION_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("integration")) {
						DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["integration"], dataPackage->integrationData);
					}
					this->eventManager->onIntegrationUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "INTEGRATION_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>() };
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("application_id")) {
						dataPackage->applicationId = std::move(payload.at("d"))["application_id"];
					}
					if (std::move(payload.at("d")).contains("id")) {
						dataPackage->id = std::move(payload.at("d"))["id"];
					}
					this->eventManager->onIntegrationDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "INTERACTION_CREATE") {
					std::unique_ptr<DiscordCoreAPI::InteractionData> interactionData{ std::make_unique<DiscordCoreAPI::InteractionData>() };
					if (std::move(payload.at("d")).contains("user")) {
						*interactionData = DiscordCoreAPI::InteractionData(std::move(payload.at("d"))["user"].at("id").get<std::string>());
					}
					else if (std::move(payload.at("d")).contains("member")) {
						*interactionData = DiscordCoreAPI::InteractionData(std::move(payload.at("d"))["member"].at("user").at("id").get<std::string>());
					}
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), *interactionData.get());
					std::unique_ptr<DiscordCoreAPI::InputEventData> eventData{ std::make_unique<DiscordCoreAPI::InputEventData>(DiscordCoreAPI::MessageData(), *interactionData, DiscordCoreAPI::InputEventType::Unset) };
					if (interactionData->type == DiscordCoreAPI::InteractionType::Application_Command) {
						if (interactionData->data.applicationCommanddata.type == DiscordCoreAPI::ApplicationCommandType::Chat_Input) {
							eventData->eventType = DiscordCoreAPI::InputEventType::Application_Command_Interaction;
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							eventData->requesterId = interactionData->requesterId;
							eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>(*eventData) };
							this->commandController->checkForAndRunCommand(std::move(commandData)).detachThread();
							this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
							std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							this->eventManager->onInputEventCreationEvent(*eventCreationData);
						}
						else if (interactionData->data.applicationCommanddata.type == DiscordCoreAPI::ApplicationCommandType::Message) {
							eventData->eventType = DiscordCoreAPI::InputEventType::Message_Command_Interaction;
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							eventData->requesterId = interactionData->requesterId;
							eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>(*eventData) };
							this->commandController->checkForAndRunCommand(std::move(commandData)).detachThread();
							this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
							std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							this->eventManager->onInputEventCreationEvent(*eventCreationData);
						}
						else if (interactionData->data.applicationCommanddata.type == DiscordCoreAPI::ApplicationCommandType::User) {
							eventData->eventType = DiscordCoreAPI::InputEventType::User_Command_Interaction;
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							eventData->requesterId = interactionData->requesterId;
							eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>(*eventData) };
							this->commandController->checkForAndRunCommand(std::move(commandData)).detachThread();
							this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
							std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							this->eventManager->onInputEventCreationEvent(*eventCreationData);
						}
					}
					else if (interactionData->type == DiscordCoreAPI::InteractionType::Message_Component) {
						if (interactionData->data.componentData.componentType == DiscordCoreAPI::ComponentType::Button) {
							eventData->eventType = DiscordCoreAPI::InputEventType::Button_Interaction;
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							eventData->requesterId = interactionData->requesterId;
							eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							if (DiscordCoreAPI::ButtonCollector::buttonInteractionBufferMap.contains(eventData->getChannelId() + eventData->getMessageId())) {
								DiscordCoreAPI::ButtonCollector::buttonInteractionBufferMap[eventData->getChannelId() + eventData->getMessageId()]->send(eventData->getInteractionData());
							}
							this->eventManager->onInputEventCreationEvent(*eventCreationData);
							this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
						}
						else if (interactionData->data.componentData.componentType == DiscordCoreAPI::ComponentType::SelectMenu) {
							eventData->eventType = DiscordCoreAPI::InputEventType::Select_Menu_Interaction;
							eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
							eventData->requesterId = interactionData->requesterId;
							eventData->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							if (DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBufferMap.contains(eventData->getChannelId() + eventData->getMessageId())) {
								DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBufferMap[eventData->getChannelId() + eventData->getMessageId()]->send(eventData->getInteractionData());
							}
							this->eventManager->onInputEventCreationEvent(*eventCreationData);
							this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
						}
					}
					else if (interactionData->type == DiscordCoreAPI::InteractionType::Modal_Submit) {
						eventData->eventType = DiscordCoreAPI::InputEventType::Modal_Interaction;
						eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
						eventData->requesterId = interactionData->requesterId;
						eventData->interactionData = *interactionData;
						std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
						dataPackage->interactionData = *interactionData;
						std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
						eventCreationData->inputEventData = *eventData;
						if (DiscordCoreAPI::ModalCollector::modalInteractionBufferMap.contains(eventData->getChannelId())) {
							DiscordCoreAPI::ModalCollector::modalInteractionBufferMap[eventData->getChannelId()]->send(eventData->getInteractionData());
						}
						this->eventManager->onInputEventCreationEvent(*eventCreationData);
						this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
					}
				}
				else if (payload.at("t") == "INVITE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->invite);
					this->eventManager->onInviteCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "INVITE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteDeletionData>() };
					if (std::move(payload.at("d")).contains("channel_id")) {
						dataPackage->channelId = std::move(payload.at("d"))["channel_id"];
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("code")) {
						dataPackage->code = std::move(payload.at("d"))["code"];
					}
					this->eventManager->onInviteDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "MESSAGE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::Message> message{ std::make_unique<DiscordCoreAPI::Message>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), *message);
					std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageCreationData>() };
					dataPackage->message = *message;
					for (auto& [key, value] : DiscordCoreAPI::MessageCollector::messagesBufferMap) {
						value->send(*message);
					}
					this->eventManager->onMessageCreationEvent(std::move(*dataPackage));
					std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
				}
				else if (payload.at("t") == "MESSAGE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->messageNew);
					for (auto& [key, value] : DiscordCoreAPI::MessageCollector::messagesBufferMap) {
						value->send(dataPackage->messageNew);
					}
					this->eventManager->onMessageUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "MESSAGE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeletionData>() };
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("channel_id")) {
						dataPackage->channelId = std::move(payload.at("d"))["channel_id"];
					}
					if (std::move(payload.at("d")).contains("id")) {
						dataPackage->messageId = std::move(payload.at("d"))["id"];
					}
					this->eventManager->onMessageDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "MESSAGE_DELETE_BULK") {
					std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
					if (std::move(payload.at("d")).contains("channel_id")) {
						dataPackage->channelId = std::move(payload.at("d"))["channel_id"];
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("ids")) {
						for (auto& value : std::move(payload.at("d"))["ids"]) {
							dataPackage->ids.push_back(value);
						}
					}
					this->eventManager->onMessageDeleteBulkEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "MESSAGE_REACTION_ADD") {
					std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->reaction);
					this->eventManager->onReactionAddEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "MESSAGE_REACTION_REMOVE") {
					std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->reactionRemoveData);
					this->eventManager->onReactionRemoveEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "MESSAGE_REACTION_REMOVE_ALL") {
					std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>() };
					if (std::move(payload.at("d")).contains("channel_id")) {
						dataPackage->channelId = std::move(payload.at("d"))["channel_id"];
					}
					if (std::move(payload.at("d")).contains("message_id")) {
						dataPackage->messageId = std::move(payload.at("d"))["message_id"];
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onReactionRemoveAllEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "MESSAGE_REACTION_REMOVE_EMOJI") {
					std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>() };
					if (std::move(payload.at("d")).contains("channel_id")) {
						dataPackage->channelId = std::move(payload.at("d"))["channel_id"];
					}
					if (std::move(payload.at("d")).contains("message_id")) {
						dataPackage->messageId = std::move(payload.at("d"))["message_id"];
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["emoji"], dataPackage->emoji);
					this->eventManager->onReactionRemoveEmojiEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "PRESENCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->presenceData);
					this->eventManager->onPresenceUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "STAGE_INSTANCE_CREATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->stageInstance);
					this->eventManager->onStageInstanceCreationEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "STAGE_INSTANCE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->stageInstance);
					this->eventManager->onStageInstanceUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "STAGE_INSTANCE_DELETE") {
					std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->stageInstance);
					this->eventManager->onStageInstanceDeletionEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "TYPING_START") {
					std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->typingStartData);
					this->eventManager->onTypingStartEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "USER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
					DiscordCoreAPI::UserData newData{};
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d"))["user"], newData);
					dataPackage->userNew = newData;
					dataPackage->userOld = DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get();
					this->eventManager->onUserUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "VOICE_STATE_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
					DiscordCoreInternal::DataParser::parseObject(std::move(payload.at("d")), dataPackage->voiceStateData);
					this->eventManager->onVoiceStateUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "VOICE_SERVER_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>() };
					if (std::move(payload.at("d")).contains("endpoint")) {
						dataPackage->endpoint = std::move(payload.at("d"))["endpoint"];
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					if (std::move(payload.at("d")).contains("token")) {
						dataPackage->token = std::move(payload.at("d"))["token"];
					}
					this->eventManager->onVoiceServerUpdateEvent(std::move(*dataPackage));
				}
				else if (payload.at("t") == "WEBHOOKS_UPDATE") {
					std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
					if (std::move(payload.at("d")).contains("channel_id")) {
						dataPackage->channelId = std::move(payload.at("d"))["channel_id"];
					}
					if (std::move(payload.at("d")).contains("guild_id")) {
						dataPackage->guildId = std::move(payload.at("d"))["guild_id"];
					}
					this->eventManager->onWebhookUpdateEvent(std::move(*dataPackage));
				}
			}
			if (this->printMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << "DiscordCoreAPIMessage received from WebSocket: " << payload.dump() << std::endl << DiscordCoreAPI::reset() << std::endl;
			}
			return;
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
			this->onClosedExternal();
			return;
		}
	}

	void BaseSocketAgent::sendHeartBeat() noexcept {
		try {
			if (this->haveWeReceivedHeartbeatAck) {
				nlohmann::json heartbeat = JSONIFY(this->lastNumberReceived);
				this->sendMessage(heartbeat);
				this->haveWeReceivedHeartbeatAck = false;
			}
			else {
				this->onClosedExternal();
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::sendHeartBeat()");
			this->onClosedExternal();
		}
	}

	void BaseSocketAgent::handleBuffer() noexcept {
		try {
			std::string newVector = this->webSocket->getInputBuffer();
			switch (this->state) {
			case WebSocketState::Initializing:
				if (newVector.find("\r\n\r\n") != std::string::npos) {
					std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
					newVector.erase(0, newVector.find("\r\n\r\n") + 4);
					std::vector<std::string> headerOut = tokenize(headers);
					if (headerOut.size()) {
						std::string statusLine = headerOut[0];
						headerOut.erase(headerOut.begin());
						std::vector<std::string> status = tokenize(statusLine, " ");
						if (status.size() >= 3 && status[1] == "101") {
							this->state = WebSocketState::Connected;
							this->webSocket->getInputBuffer().clear();
							this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().end(), newVector.begin(), newVector.end());
							this->parseHeader();
						}
						else {
							return;
						}
					}
				}
				break;
			case WebSocketState::Connected:
				while (this->parseHeader()) {};
				return;
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::handleBuffer()");
			this->onClosedExternal();
		}
	}

	bool BaseSocketAgent::parseHeader() noexcept {
		try {
			std::string newVector = this->webSocket->getInputBuffer();
			if (this->webSocket->getInputBuffer().size() < 4) {
				return false;
			}
			else {
				switch (static_cast<WebSocketOpCode>(this->webSocket->getInputBuffer()[0] & ~webSocketMaskBit))
				{
				case WebSocketOpCode::Op_Continuation:
				case WebSocketOpCode::Op_Text:
				case WebSocketOpCode::Op_Binary:
				case WebSocketOpCode::Op_Ping:
				case WebSocketOpCode::Op_Pong:
				{
					uint8_t length01 = this->webSocket->getInputBuffer()[1];
					int32_t payloadStartOffset = 2;
					if (length01 & webSocketMaskBit) {
						return false;
					}
					uint64_t length02 = length01;
					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (this->webSocket->getInputBuffer().size() < 8) {
							return false;
						}
						uint8_t length03 = this->webSocket->getInputBuffer()[2];
						uint8_t length04 = this->webSocket->getInputBuffer()[3];
						length02 = static_cast<uint64_t>((length03 << 8) | length04);
						payloadStartOffset += 2;
					}
					else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (this->webSocket->getInputBuffer().size() < 10) {
							return false;
						}
						length02 = 0;
						for (int32_t value = 2, shift = 56; value < 10; ++value, shift -= 8) {
							uint8_t length05 = static_cast<uint8_t>(this->webSocket->getInputBuffer()[value]);
							length02 |= static_cast<uint64_t>(length05) << static_cast<uint64_t>(shift);
						}
						payloadStartOffset += 8;
					}
					if (this->webSocket->getInputBuffer().size() < payloadStartOffset + length02) {
						return false;
					}
					else {
						std::string newerVector{};
						newerVector.reserve(length02);
						for (uint32_t x = payloadStartOffset; x < payloadStartOffset + length02; x += 1) {
							newerVector.push_back(this->webSocket->getInputBuffer()[x]);
						}
						this->webSocket->getInputBuffer() = std::move(newerVector);
						this->onMessageReceived();
						this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().begin(), newVector.begin() + payloadStartOffset + length02, newVector.end());
					}
					return true;
				}
				case WebSocketOpCode::Op_Close: {
					uint16_t close = this->webSocket->getInputBuffer()[2] & 0xff;
					close <<= 8;
					close |= this->webSocket->getInputBuffer()[3] & 0xff;
					this->closeCode = close;
					this->onClosedExternal();
					return false;
				}
				default: {
					this->closeCode = 0;
					return false;
				}
				}
			}
			return false;
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::parseHeader()");
			this->onClosedExternal();
			return false;
		}
	}

	void BaseSocketAgent::onClosedExternal() noexcept {
		this->doWeReconnect.reset();
	}

	void BaseSocketAgent::onClosedInternal() noexcept {
		try {
			this->areWeReadyToConnectEvent.reset();
			if (this->maxReconnectTries > this->currentReconnectTries) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket Closed; Code: " << this->closeCode << DiscordCoreAPI::reset() << std::endl;
				this->closeCode = 0;
				this->areWeConnected.store(false, std::memory_order_seq_cst);
				this->currentReconnectTries += 1;
				this->webSocket.reset(nullptr);
				this->areWeAuthenticated = false;
				this->haveWeReceivedHeartbeatAck = true;
				this->connect();
			}
			else if (this->maxReconnectTries <= this->currentReconnectTries) {
				this->theTask->cancel();
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::onClosedInternal()");
		}
	}

	void BaseSocketAgent::connect() noexcept {
		try {
			this->webSocket = std::make_unique<WebSocketSSLClient>(this->baseUrl, this->port);
			this->state = WebSocketState::Initializing;
			this->doWeReconnect.set();
			std::string sendString = "GET /?v=10&encoding=etf HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generate64BaseEncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->sendMessage(sendString);
		}
		catch (...) {
			DiscordCoreAPI::reportException("BaseSocketAgent::connect()");
			this->onClosedExternal();
		}
	}

	BaseSocketAgent::~BaseSocketAgent() noexcept {
		this->theTask->cancel();
	}

	VoiceSocketAgent::VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, bool printMessagesNew) noexcept {
		this->printMessages = printMessagesNew;
		this->baseSocketAgent = baseBaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->baseSocketAgent->voiceConnectionDataBufferMap.insert_or_assign(this->voiceConnectInitData.guildId, &this->voiceConnectionDataBuffer);
		this->baseSocketAgent->getVoiceConnectionData(this->voiceConnectInitData);
		this->doWeReconnect.set();
		this->areWeConnected.reset();
		this->theTask = std::make_unique<DiscordCoreAPI::CoRoutine<void>>(this->run());
	}

	void VoiceSocketAgent::sendVoiceData(std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "Please specify voice data to send" << std::endl << DiscordCoreAPI::reset() << std::endl;
				return;
			}
			else {
				if (!this->voiceSocket->writeData(responseData)) {
					this->onClosedExternal();
				}
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendVoiceData()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendMessage(std::vector<uint8_t>& dataToSend) noexcept {
		try {
			std::string newString{};
			newString.insert(newString.begin(), dataToSend.begin(), dataToSend.end());
			if (this->printMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket DiscordCoreAPIMessage: " << newString << std::endl << DiscordCoreAPI::reset() << std::endl;
			}
			std::vector<char> out{};
			out.resize(maxHeaderSize);
			size_t size = this->createHeader(out.data(), newString.size(), this->dataOpcode);
			std::string header(out.data(), size);
			std::string theVectorNew{};
			theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
			theVectorNew.insert(theVectorNew.begin() + header.size(), newString.begin(), newString.end());
			this->webSocket->writeData(theVectorNew);
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendMessage(std::string& dataToSend) noexcept {
		try {
			if (this->printMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket DiscordCoreAPIMessage: " << std::endl << dataToSend << DiscordCoreAPI::reset();
			}
			this->webSocket->writeData(dataToSend);
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	uint64_t VoiceSocketAgent::createHeader(char* outBuffer, uint64_t sendlength, WebSocketOpCode opCode) noexcept {
		try {
			size_t position = 0;
			outBuffer[position++] = webSocketFinishBit | static_cast<unsigned char>(opCode);
			if (sendlength <=webSocketMaxPayloadLengthSmall)
			{
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			}
			else if (sendlength <= webSocketMaxPayloadLengthLarge)
			{
				outBuffer[position++] = static_cast<unsigned char>(webSocketPayloadLengthMagicLarge);
				outBuffer[position++] = static_cast<unsigned char>(sendlength >> 8);
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			}
			else
			{
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
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::createHeader()");
			this->onClosedExternal();
			return size_t{};
		}
	}

	std::vector<std::string> VoiceSocketAgent::tokenize(std::string& dataIn, std::string separator) noexcept {
		try {
			std::vector<std::string> dataOut{};
			for (auto value = 0; value != std::string::npos; value = static_cast<int32_t>(dataIn.find_first_not_of(separator, value))) {
				auto output = static_cast<int32_t>(dataIn.find(separator, value));
				dataOut.push_back(dataIn.substr(value, output - value));
				value = output;
			}
			return dataOut;
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::tokenize()");
			this->onClosedExternal();
			return std::vector<std::string>{};
		}
	}

	DiscordCoreAPI::CoRoutine<void> VoiceSocketAgent::run() noexcept {
		try {
			auto cancelHandle = co_await DiscordCoreAPI::NewThreadAwaitable<void>();
			this->connect();
			DiscordCoreAPI::StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{0} };
			while (!cancelHandle.promise().isItStopped()) {
				if (this->heartbeatInterval != 0 && !this->areWeHeartBeating) {
					this->areWeHeartBeating = true;
					stopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{this->heartbeatInterval} };
				}
				if (stopWatch.hasTimePassed() && this->areWeHeartBeating) {
					stopWatch.resetTimer();
					this->sendHeartBeat();
				}
				if (!this->doWeReconnect.wait(0)) {
					this->onClosedInternal();
					co_return;
				}
				if (this->webSocket != nullptr) {
					if (!this->webSocket->processIO(1000)) {
						this->onClosedExternal();
					}
				}
				if (this->voiceSocket != nullptr) {
					this->voiceSocket->readData(true);
				}
				this->handleBuffer();
			}
			co_return;
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::run()");
			this->onClosedExternal();
			co_return;
		}
	}

	void VoiceSocketAgent::onMessageReceived() noexcept {
		try {
			std::string message = this->webSocket->getInputBuffer();
			this->webSocket->getInputBuffer().clear();
			nlohmann::json payload = payload.parse(message);
			if (this->printMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << "DiscordCoreAPIMessage received from Voice WebSocket: " << message << std::endl << DiscordCoreAPI::reset() << std::endl;
			}
			if (payload.contains("op")) {
				if (payload.at("op") == 6) {
					this->haveWeReceivedHeartbeatAck = true;
				};
				if (payload.at("op") == 2) {
					this->voiceConnectionData.audioSSRC = std::move(payload.at("d")).at("ssrc").get<uint32_t>();
					this->voiceConnectionData.voiceIp = std::move(payload.at("d")).at("ip").get<std::string>();
					this->voiceConnectionData.voicePort = std::to_string(std::move(payload.at("d")).at("port").get<int64_t>());
					for (auto& value : std::move(payload.at("d")).at("modes")) {
						if (value == "xsalsa20_poly1305") {
							this->voiceConnectionData.voiceEncryptionMode = value;
						}
					}
					this->voiceConnect();
					this->collectExternalIP();
					std::vector<uint8_t> protocolPayloadSelectString = JSONIFY(this->voiceConnectionData.voicePort, this->voiceConnectionData.externalIp, this->voiceConnectionData.voiceEncryptionMode, 0);
					if (this->webSocket != nullptr) {
						this->sendMessage(protocolPayloadSelectString);
					}
				}
				if (payload.at("op") == 4) {
					for (uint32_t x = 0; x < std::move(payload.at("d")).at("secret_key").size(); x += 1) {
						this->voiceConnectionData.secretKey.push_back(std::move(payload.at("d")).at("secret_key").at(x).get<uint8_t>());
					}
					this->areWeConnected.set();
				}
				if (payload.at("op") == 9) {};
				if (payload.at("op") == 8) {
					if (std::move(payload.at("d")).contains("heartbeat_interval")) {
						this->heartbeatInterval = static_cast<int32_t>(std::move(payload.at("d")).at("heartbeat_interval").get<float>());
						this->areWeHeartBeating = false;
					}
					this->haveWeReceivedHeartbeatAck = true;
					std::vector<uint8_t> identifyPayload = JSONIFY(this->voiceConnectionData, this->voiceConnectInitData);
					if (this->webSocket != nullptr) {
						this->sendMessage(identifyPayload);
					}
				}
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::onMessageReceived()");
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
			if (!this->voiceSocket->writeData(packet)) {
				this->onClosedExternal();
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
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::collectExternalIP()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendHeartBeat() noexcept {
		try {
			if (this->haveWeReceivedHeartbeatAck) {
				std::vector<uint8_t> heartbeatPayload = JSONIFY(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				if (this->webSocket != nullptr) {
					this->sendMessage(heartbeatPayload);
				}
				this->haveWeReceivedHeartbeatAck = false;
			}
			else {
				this->onClosedExternal();
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendHeartBeat()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::voiceConnect() noexcept {
		try {
			this->voiceSocket = std::make_unique<DatagramSocketSSLClient>(this->voiceConnectionData.voiceIp, this->voiceConnectionData.voicePort);
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::voiceConnect()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::handleBuffer() noexcept {
		try {
			std::string newVector{};
			switch (this->state) {
			case WebSocketState::Initializing:
				newVector.insert(newVector.begin(), this->webSocket->getInputBuffer().begin(), this->webSocket->getInputBuffer().end());
				if (newVector.find("\r\n\r\n") != std::string::npos) {
					std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
					newVector.erase(0, newVector.find("\r\n\r\n") + 4);
					std::vector<std::string> headerOut = tokenize(headers);
					if (headerOut.size()) {
						std::string statusLine = headerOut[0];
						headerOut.erase(headerOut.begin());
						std::vector<std::string> status = tokenize(statusLine, " ");
						if (status.size() >= 3 && status[1] == "101") {
							this->state = WebSocketState::Connected;
							this->webSocket->getInputBuffer().clear();
							this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().end(), newVector.begin(), newVector.end());
							this->parseHeader();
						}
						else {
							return;
						}
					}
				}
				break;
			case WebSocketState::Connected:
				while (this->parseHeader()) {};
				return;
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::handleBuffer()");
			this->onClosedExternal();
		}
	}

	bool VoiceSocketAgent::parseHeader() noexcept {
		try {
			std::string newVector = this->webSocket->getInputBuffer();
			if (this->webSocket->getInputBuffer().size() < 4) {
				return false;
			}
			else {
				switch (static_cast<WebSocketOpCode>(this->webSocket->getInputBuffer()[0] & ~webSocketMaskBit))
				{
				case WebSocketOpCode::Op_Continuation:
				case WebSocketOpCode::Op_Text:
				case WebSocketOpCode::Op_Binary:
				case WebSocketOpCode::Op_Ping:
				case WebSocketOpCode::Op_Pong:
				{
					uint8_t length01 = this->webSocket->getInputBuffer()[1];
					int32_t payloadStartOffset = 2;
					if (length01 & webSocketMaskBit) {
						return false;
					}
					uint64_t length02 = length01;
					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (this->webSocket->getInputBuffer().size() < 8) {
							return false;
						}
						uint8_t length03 = this->webSocket->getInputBuffer()[2];
						uint8_t length04 = this->webSocket->getInputBuffer()[3];
						length02 = static_cast<uint64_t>((length03 << 8) | length04);
						payloadStartOffset += 2;
					}
					else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (this->webSocket->getInputBuffer().size() < 10) {
							return false;
						}
						length02 = 0;
						for (int32_t value = 2, shift = 56; value < 10; ++value, shift -= 8) {
							uint8_t length05 = static_cast<uint8_t>(this->webSocket->getInputBuffer()[value]);
							length02 |= static_cast<uint64_t>(length05) << static_cast<uint64_t>(shift);
						}
						payloadStartOffset += 8;
					}
					if (this->webSocket->getInputBuffer().size() < payloadStartOffset + length02) {
						return false;
					}
					else {
						std::string newerVector{};
						newerVector.reserve(length02);
						for (uint32_t x = payloadStartOffset; x < payloadStartOffset + length02; x += 1) {
							newerVector.push_back(this->webSocket->getInputBuffer()[x]);
						}
						this->webSocket->getInputBuffer() = std::move(newerVector);
						this->onMessageReceived();
						this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().begin(), newVector.begin() + payloadStartOffset + length02, newVector.end());
					}
					return true;
				}
				case WebSocketOpCode::Op_Close: {
					uint16_t close = this->webSocket->getInputBuffer()[2] & 0xff;
					close <<= 8;
					close |= this->webSocket->getInputBuffer()[3] & 0xff;
					this->closeCode = close;
					this->onClosedExternal();
					return false;
				}
				default: {
					this->closeCode = 0;
					return false;
				}
				}
			}
			return false;
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::parseHeader()");
			this->onClosedExternal();
			return false;
		}
	}

	void VoiceSocketAgent::onClosedExternal() noexcept {
		this->doWeReconnect.reset();
	}

	void VoiceSocketAgent::onClosedInternal() noexcept {
		try {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "Voice WebSocket Closed; Code: " << this->closeCode << DiscordCoreAPI::reset() << std::endl;
			this->closeCode = 0;
			this->voiceSocket.reset(nullptr);
			this->webSocket.reset(nullptr);
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::onClosedInternal()");
		}
	}

	void VoiceSocketAgent::connect() noexcept {
		try {
			DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 20000);
			this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
			this->webSocket = std::make_unique<WebSocketSSLClient>(this->baseUrl, "443");
			this->state = WebSocketState::Initializing;
			std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generate64BaseEncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->sendMessage(sendVector);
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
			this->onClosedExternal();
		}
	}

	VoiceSocketAgent::~VoiceSocketAgent() noexcept {
		this->theTask->cancel();
		this->theTask.reset(nullptr);
	};
}