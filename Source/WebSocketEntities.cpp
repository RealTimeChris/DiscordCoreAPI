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

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocketAgentNew) noexcept {
		this->configManager = &discordCoreClientNew->configManager;
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->discordCoreClient = discordCoreClientNew;
		this->doWeQuit = doWeQuitNew;
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
	}

	void BaseSocketAgent::sendMessage(const nlohmann::json& dataToSend, WebSocketSSLShard* theShard, bool priority) noexcept {
		if (theShard) {
			try {
				DiscordCoreAPI::StopWatch stopWatch{ 5500ms };
				while (!theShard->areWeConnected01.load() && !(dataToSend.contains("op") && (dataToSend["op"] == 2 || dataToSend["op"] == 6))) {
					if (stopWatch.hasTimePassed()) {
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + theShard->shard.dump() + std::string("'s Message: ") << dataToSend.dump()
							  << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				std::string theVectorNew{};
				this->stringifyJsonData(dataToSend, theVectorNew, theShard->dataOpCode);
				bool didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				do {
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					didWeWrite = theShard->writeData(theVectorNew, priority);
				} while (!didWeWrite);
				if (!didWeWrite) {
					throw ProcessingError{ "Failed to write to the websocket." };
				}
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
				}
				this->onClosed(theShard);
			}
		}
	}

	void BaseSocketAgent::sendMessage(std::string& dataToSend, WebSocketSSLShard* theShard, bool priority) noexcept {
		if (theShard && theShard->areWeConnected01.load()) {
			try {
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + theShard->shard.dump() + std::string("'s Message: ") << std::endl
							  << dataToSend << DiscordCoreAPI::reset();
				}
				bool didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				do {
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					didWeWrite = theShard->writeData(dataToSend, priority);
				} while (!didWeWrite);
				if (!didWeWrite) {
					throw ProcessingError{ "Failed to write to the websocket." };
				}
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
				}
				this->onClosed(theShard);
			}
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
		while (!this->theClients[thePackage.currentShard]->areWeConnected01.load()) {
			std::this_thread::sleep_for(1ms);
		}
	}

	void BaseSocketAgent::onClosed(WebSocketSSLShard* theShard) noexcept {
		if (theShard && this->theClients.contains(theShard->shard[0])) {
			if (this->maxReconnectTries > theShard->currentReconnectionTries) {
				theShard->disconnect();
				this->theClients.erase(theShard->shard[0]);
			} else if (this->maxReconnectTries <= theShard->currentReconnectionTries) {
				this->doWeQuit->store(true);
				this->theTask->request_stop();
			}
		}
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->theTask.get();
	}

	void BaseSocketAgent::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect, WebSocketSSLShard* theShard) noexcept {
		if (theShard && theShard->areWeConnected02.load()) {
			try {
				DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theStopWatch{ 2500ms };
				int32_t theCurrentIndex = theShard->shard[0];
				DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
				dataPackage.channelId = 0;
				dataPackage.guildId = doWeCollect.guildId;
				dataPackage.selfDeaf = doWeCollect.selfDeaf;
				dataPackage.selfMute = doWeCollect.selfMute;
				theShard->userId = doWeCollect.userId;
				nlohmann::json newData = dataPackage;
				std::string theString{};
				this->stringifyJsonData(newData, theString, theShard->dataOpCode);
				theShard->writeData(theString, true);
				this->sendMessage(newData, theShard, true);
				if (doWeCollect.channelId == 0) {
					return;
				}
				dataPackage.channelId = doWeCollect.channelId;
				newData = dataPackage;
				this->stringifyJsonData(newData, theString, theShard->dataOpCode);
				theShard->writeData(theString, true);
				theShard->areWeCollectingData = true;
				theStopWatch.resetTimer();
				while (theShard->areWeCollectingData) {
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					std::this_thread::sleep_for(1ms);
				}
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
				}
				this->onClosed(theShard);
			}
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
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::createHeader()");
			}
		}
	}

	void BaseSocketAgent::stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, WebSocketOpCode theOpCode) noexcept {
		std::string theVector{};
		std::string header{};
		if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
			theVector = this->erlPacker.parseJsonToEtf(dataToSend);
		} else {
			theVector = dataToSend.dump();
		}
		this->createHeader(header, theVector.size(), theOpCode);
		std::string theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		theString = theVectorNew;
	}

	void BaseSocketAgent::parseHeadersAndMessage(WebSocketSSLShard* theShard) noexcept {
		if (theShard && theShard->areWeConnected01.load()) {
			if (theShard->theState == WebSocketState::Connecting01) {
				std::string newVector = theShard->getInputBuffer();
				if (newVector.find("\r\n\r\n") != std::string::npos) {
					newVector.erase(0, newVector.find("\r\n\r\n") + 4);
					theShard->inputBuffer.clear();
					theShard->inputBuffer.insert(theShard->inputBuffer.end(), newVector.begin(), newVector.end());
					theShard->theState = WebSocketState::Connecting02;
				}
			}
			if (theShard->inputBuffer.size() < 4) {
				return;
			}
			theShard->dataOpCode = static_cast<WebSocketOpCode>(theShard->inputBuffer[0] & ~webSocketFinishBit);
			switch (theShard->dataOpCode) {
				case WebSocketOpCode::Op_Continuation:
					[[fallthrough]];
				case WebSocketOpCode::Op_Text:
					[[fallthrough]];
				case WebSocketOpCode::Op_Binary:
					[[fallthrough]];
				case WebSocketOpCode::Op_Ping:
					[[fallthrough]];
				case WebSocketOpCode::Op_Pong: {
					uint8_t length01 = theShard->inputBuffer[1];
					theShard->messageOffset = 2;
					if (length01 & webSocketMaskBit) {
						return;
					}
					theShard->messageLength = length01;
					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (theShard->inputBuffer.size() < 8) {
							return;
						}
						uint8_t length03 = theShard->inputBuffer[2];
						uint8_t length04 = theShard->inputBuffer[3];
						theShard->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
						theShard->messageOffset += 2;
					} else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (theShard->inputBuffer.size() < 10) {
							return;
						}
						theShard->messageLength = 0;
						for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
							uint8_t lengthNew = static_cast<uint8_t>(theShard->inputBuffer[x]);
							theShard->messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
						}
						theShard->messageOffset += 8;
					}
					if (theShard->inputBuffer.size() < static_cast<uint64_t>(theShard->messageOffset) + static_cast<uint64_t>(theShard->messageLength)) {
						return;
					} else {
						std::string finalMessage{};
						finalMessage.insert(finalMessage.begin(), theShard->inputBuffer.begin() + theShard->messageOffset,
							theShard->inputBuffer.begin() + theShard->messageOffset + theShard->messageLength);
						theShard->processedMessages.push(finalMessage);
						theShard->inputBuffer.erase(theShard->inputBuffer.begin(), theShard->inputBuffer.begin() + theShard->messageOffset + theShard->messageLength);
						return;
					}
				}
				case WebSocketOpCode::Op_Close: {
					uint16_t close = theShard->inputBuffer[2] & 0xff;
					close <<= 8;
					close |= theShard->inputBuffer[3] & 0xff;
					theShard->closeCode = static_cast<WebSocketCloseCode>(close);
					theShard->inputBuffer.erase(theShard->inputBuffer.begin(), theShard->inputBuffer.begin() + 4);
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						std::cout << DiscordCoreAPI::shiftToBrightRed()
								  << "WebSocket " + this->theClients[0]->shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(this->theClients[0]->closeCode)
								  << DiscordCoreAPI::reset() << std::endl
								  << std::endl;
					}
					this->onClosed(theShard);
					return;
				}
				default: {
					return;
				}
			}
		}
	}

	void BaseSocketAgent::checkForAndSendHeartBeat(WebSocketSSLShard* theShard, bool isImmediate) noexcept {
		if (theShard && theShard->areWeConnected02.load()) {
			try {
				if ((theShard->heartBeatStopWatch.hasTimePassed() && theShard->haveWeReceivedHeartbeatAck) || isImmediate) {
					nlohmann::json heartbeat{};
					heartbeat["d"] = theShard->lastNumberReceived;
					heartbeat["op"] = 1;
					this->sendMessage(heartbeat, theShard, true);
					theShard->haveWeReceivedHeartbeatAck = false;
					theShard->heartBeatStopWatch.resetTimer();
				}
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::checkForAndSendHeartBeat()");
				}
				this->onClosed(theShard);
			}
		}
	}

	void BaseSocketAgent::onMessageReceived(WebSocketSSLShard* theShard) noexcept {
		if (theShard && theShard->areWeConnected01.load()) {
			try {
				std::string messageNew{};
				if (theShard->processedMessages.size() > 0) {
					messageNew = theShard->processedMessages.front();
					theShard->processedMessages.pop();
				} else {
					return;
				}
				if (messageNew.size() == 0) {
					return;
				}
				nlohmann::json payload{};

				if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
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
					if (payload["t"] == "RESUMED") {
						theShard->areWeConnected02.store(true);
						theShard->currentReconnectionTries = 0;
					}

					if (payload["t"] == "READY") {
						theShard->areWeConnected02.store(true);
						theShard->sessionId = payload["d"]["session_id"].get<std::string>();
						DiscordCoreAPI::UserData theUser{ payload["d"]["user"] };
						this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ theUser, this };
						DiscordCoreAPI::Users::insertUser(theUser);
						theShard->currentReconnectionTries = 0;
					}
				}

				if (payload.contains("s") && !payload["s"].is_null()) {
					theShard->lastNumberReceived = payload["s"].get<int32_t>();
				}

				if (payload.contains("op") && !payload["op"].is_null()) {
					switch (payload["op"].get<int32_t>()) {
						case 0: {
							if (payload.contains("d") && !payload["d"].is_null() && payload.contains("t") && !payload["t"].is_null()) {
								if (payload["t"] == "APPLICATION_COMMAND_PERMISSIONS_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>(payload["d"])
									};
									dataPackage->permissionData = payload["d"];
									this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
								} else if (payload["t"] == "AUTO_MODERATION_RULE_CREATE") {
									std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>()
									};
									dataPackage->theRule = payload["d"];
									this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
								} else if (payload["t"] == "AUTO_MODERATION_RULE_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>()
									};
									dataPackage->theRule = payload["d"];
									this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
								} else if (payload["t"] == "AUTO_MODERATION_RULE_DELETE") {
									std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>()
									};
									dataPackage->theRule = payload["d"];
									this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
								} else if (payload["t"] == "AUTO_MODERATION_ACTION_EXECUTION") {
									std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>()
									};
									dataPackage->theData = payload["d"];
									this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
								} else if (payload["t"] == "CHANNEL_CREATE") {
									std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>() };
									dataPackage->channel = payload["d"];
									this->discordCoreClient->eventManager.onChannelCreationEvent(*dataPackage);
								} else if (payload["t"] == "CHANNEL_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>() };
									if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
										dataPackage->channelOld =
											DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(payload["d"]["id"].get<std::string>()) }).get();
										dataPackage->channelNew = dataPackage->channelOld;
									}
									dataPackage->channelNew = payload["d"];
									this->discordCoreClient->eventManager.onChannelUpdateEvent(*dataPackage);
								} else if (payload["t"] == "CHANNEL_DELETE") {
									std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>() };
									dataPackage->channel = payload["d"];
									this->discordCoreClient->eventManager.onChannelDeletionEvent(*dataPackage);
								} else if (payload["t"] == "CHANNEL_PINS_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
									dataPackage->dataPackage = payload["d"];
									this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
								} else if (payload["t"] == "THREAD_CREATE") {
									std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
									dataPackage->thread = payload["d"];
									this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
								} else if (payload["t"] == "THREAD_UPATE") {
									std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
									dataPackage->thread = payload["d"];
									this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
								} else if (payload["t"] == "THREAD_DELETE") {
									std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
									dataPackage->thread = payload["d"];
									this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
								} else if (payload["t"] == "THREAD_LIST_SYNC") {
									std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
									dataPackage->threadListSyncData = payload["d"];
									this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
								} else if (payload["t"] == "THREAD_MEMBER_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
									dataPackage->threadMember = payload["d"];
									this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
								} else if (payload["t"] == "THREAD_MEMBERS_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
									dataPackage->threadMembersUpdateData = payload["d"];
									this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_CREATE") {
									DiscordCoreAPI::GuildData guildNew{};
									std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>() };
									guildNew = payload["d"];
									guildNew.discordCoreClient = this->discordCoreClient;
									dataPackage->guild = guildNew;
									this->discordCoreClient->eventManager.onGuildCreationEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>() };
									if (payload["d"].contains("id") && !payload["d"]["id"].is_null()) {
										dataPackage->guildOld = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(payload["d"]["id"].get<std::string>()) }).get();
										dataPackage->guildNew = dataPackage->guildOld;
									}
									dataPackage->guildNew = payload["d"];
									this->discordCoreClient->eventManager.onGuildUpdateEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_DELETE") {
									std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>() };
									dataPackage->guild = payload["d"];
									this->discordCoreClient->eventManager.onGuildDeletionEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_BAN_ADD") {
									std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
									if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
										dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
									}
									if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
										DiscordCoreAPI::User newData{};
										newData = payload["d"]["user"];
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
										newData = payload["d"]["user"];
										dataPackage->user = newData;
									}
									this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_EMOJIS_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
									dataPackage->updateData = payload["d"];
									this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_STICKERS_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
									dataPackage->updateData = payload["d"];
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
									dataPackage->guildMember = payload["d"];
									dataPackage->discordCoreClient = this->discordCoreClient;
									this->discordCoreClient->eventManager.onGuildMemberAddEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_MEMBER_REMOVE") {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>() };
									if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
										dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
									}
									if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
										DiscordCoreAPI::User newData{};
										newData = payload["d"]["user"];
										dataPackage->user = newData;
									}
									this->discordCoreClient->eventManager.onGuildMemberRemoveEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_MEMBER_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>() };
									if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
										dataPackage->guildMemberNew.guildId = stoull(payload["d"]["guild_id"].get<std::string>());
									}
									if (payload["d"].contains("user") && payload["d"]["user"].contains("id") && !payload["d"]["user"]["id"].is_null()) {
										dataPackage->guildMemberOld =
											DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = stoull(payload["d"]["user"]["id"].get<std::string>()),
																										.guildId = stoull(payload["d"]["guild_id"].get<std::string>()) })
												.get();
										dataPackage->guildMemberNew = dataPackage->guildMemberOld;
									}
									dataPackage->guildMemberNew = payload["d"];
									this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_MEMBERS_CHUNK") {
									std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
									dataPackage->chunkEventData = payload["d"];
									this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_ROLE_CREATE") {
									std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>() };
									if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
										dataPackage->role = payload["d"]["role"];
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
									dataPackage->roleNew = payload["d"]["role"];
									if (payload["d"].contains("role") && !payload["d"]["role"].is_null()) {
										dataPackage->roleOld = DiscordCoreAPI::Roles::getCachedRoleAsync(
											{ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role"]["id"].get<std::string>()) })
																   .get();
										dataPackage->roleNew = dataPackage->roleOld;
									}
									this->discordCoreClient->eventManager.onRoleUpdateEvent(*dataPackage);
								} else if (payload["t"] == "GUILD_ROLE_DELETE") {
									std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleDeletionData>() };
									if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
										dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
									}
									if (payload["d"].contains("role_id") && !payload["d"]["role_id"].is_null()) {
										dataPackage->roleOld = DiscordCoreAPI::Roles::getCachedRoleAsync(
											{ .guildId = dataPackage->guildId, .roleId = stoull(payload["d"]["role_id"].get<std::string>()) })
																   .get();
									}
									this->discordCoreClient->eventManager.onRoleDeletionEvent(*dataPackage);
								} else if (payload["t"] == "INTEGRATION_CREATE") {
									std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
									if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
										dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
									}
									if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
										dataPackage->integrationData = payload["d"]["integration"];
									}
									this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
								} else if (payload["t"] == "INTEGRATION_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
									if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
										dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
									}
									if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
										dataPackage->integrationData = payload["d"]["integration"];
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
									*interactionData = payload["d"];
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
												DiscordCoreAPI::ButtonCollector::buttonInteractionBufferMap[std::to_string(eventData->getChannelId()) +
													std::to_string(eventData->getMessageId())]
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
											DiscordCoreAPI::ModalCollector::modalInteractionBufferMap[std::to_string(eventData->getChannelId())]->send(
												eventData->getInteractionData());
										}
										this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
									} else if (interactionData->type == DiscordCoreAPI::InteractionType::Application_Command_Autocomplete) {
										eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
										*eventData->interactionData = *interactionData;
										std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInteractionCreationData>() };
										dataPackage->interactionData = *interactionData;
										std::unique_ptr<DiscordCoreAPI::OnAutoCompleteEntryData> autocompleteEntryData{
											std::make_unique<DiscordCoreAPI::OnAutoCompleteEntryData>()
										};
										autocompleteEntryData->inputEvent = *eventData;
										this->discordCoreClient->eventManager.onAutoCompleteEntryEvent(*autocompleteEntryData);
										this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
									}
								} else if (payload["t"] == "INVITE_CREATE") {
									std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
									dataPackage->invite = payload["d"];
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
									*message = payload["d"];
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
									dataPackage->messageNew = payload["d"];
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
									dataPackage->reaction = payload["d"];
									this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
								} else if (payload["t"] == "MESSAGE_REACTION_REMOVE") {
									std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
									dataPackage->reactionRemoveData = payload["d"];
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
									dataPackage->emoji = payload["d"]["emoji"];
									this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
								} else if (payload["t"] == "PRESENCE_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
									dataPackage->presenceData = payload["d"];
									this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
								} else if (payload["t"] == "STAGE_INSTANCE_CREATE") {
									std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
									dataPackage->stageInstance = payload["d"];
									this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
								} else if (payload["t"] == "STAGE_INSTANCE_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
									dataPackage->stageInstance = payload["d"];
									this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
								} else if (payload["t"] == "STAGE_INSTANCE_DELETE") {
									std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
									dataPackage->stageInstance = payload["d"];
									this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
								} else if (payload["t"] == "TYPING_START") {
									std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
									dataPackage->typingStartData = payload["d"];
									this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
								} else if (payload["t"] == "USER_UPDATE") {
									std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
									DiscordCoreAPI::User newData{};
									newData = payload["d"]["user"];
									dataPackage->userNew = newData;
									dataPackage->userOld = DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get();
									this->discordCoreClient->eventManager.onUserUpdateEvent(*dataPackage);
								} else if (payload["t"] == "VOICE_STATE_UPDATE") {
									if (theShard->areWeCollectingData && !theShard->stateUpdateCollected && !theShard->serverUpdateCollected &&
										payload["d"]["member"]["user"]["id"] == std::to_string(theShard->userId)) {
										theShard->voiceConnectionData = VoiceConnectionData{};
										theShard->voiceConnectionData.sessionId = payload["d"]["session_id"].get<std::string>();
										theShard->stateUpdateCollected = true;
									} else if (theShard->areWeCollectingData && !theShard->stateUpdateCollected) {
										theShard->voiceConnectionData.sessionId = payload["d"]["session_id"].get<std::string>();
										if (theShard->voiceConnectionDataBufferMap.contains(stoull(payload["d"]["guild_id"].get<std::string>()))) {
											theShard->voiceConnectionDataBufferMap[stoull(payload["d"]["guild_id"].get<std::string>())]->send(theShard->voiceConnectionData);
										}
										theShard->serverUpdateCollected = false;
										theShard->stateUpdateCollected = false;
										theShard->areWeCollectingData = false;
									}
									std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
									dataPackage->voiceStateData = payload["d"];
									this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
								} else if (payload["t"] == "VOICE_SERVER_UPDATE") {
									if (theShard->areWeCollectingData && !theShard->serverUpdateCollected && !theShard->stateUpdateCollected) {
										theShard->voiceConnectionData = VoiceConnectionData{};
										theShard->voiceConnectionData.endPoint = payload["d"]["endpoint"].get<std::string>();
										theShard->voiceConnectionData.token = payload["d"]["token"].get<std::string>();
										theShard->serverUpdateCollected = true;
									} else if (theShard->areWeCollectingData && !theShard->serverUpdateCollected) {
										theShard->voiceConnectionData.endPoint = payload["d"]["endpoint"].get<std::string>();
										theShard->voiceConnectionData.token = payload["d"]["token"].get<std::string>();
										if (theShard->voiceConnectionDataBufferMap.contains(stoull(payload["d"]["guild_id"].get<std::string>()))) {
											theShard->voiceConnectionDataBufferMap[stoull(payload["d"]["guild_id"].get<std::string>())]->send(theShard->voiceConnectionData);
										}
										theShard->serverUpdateCollected = false;
										theShard->stateUpdateCollected = false;
										theShard->areWeCollectingData = false;
									}
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
							break;
						}
						case 1: {
							this->checkForAndSendHeartBeat(theShard, true);
							break;
						}
						case 7: {
							if (this->configManager->doWePrintWebSocketErrorMessages()) {
								std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + theShard->shard.dump() + " Reconnecting (Type 7)!" << DiscordCoreAPI::reset()
										  << std::endl
										  << std::endl;
							}
							theShard->areWeResuming = true;
							this->onClosed(theShard);
							return;
						}
						case 9: {
							if (this->configManager->doWePrintWebSocketErrorMessages()) {
								std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + theShard->shard.dump() + " Reconnecting (Type 9)!" << DiscordCoreAPI::reset()
										  << std::endl
										  << std::endl;
							}
							std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
							int32_t numOfMsToWait =
								static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
							std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
							if (payload["d"] == true) {
								theShard->areWeResuming = true;
							} else {
								theShard->areWeResuming = false;
							}
							this->onClosed(theShard);
							return;
						}
						case 10: {
							if (payload["d"].contains("heartbeat_interval") && !payload["d"]["heartbeat_interval"].is_null()) {
								theShard->heartBeatStopWatch =
									DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ std::chrono::milliseconds{ payload["d"]["heartbeat_interval"] } };
							}
							if (theShard->areWeResuming) {
								WebSocketResumeData resumeData{};
								resumeData.botToken = this->configManager->getBotToken();
								resumeData.sessionId = theShard->sessionId;
								resumeData.lastNumberReceived = theShard->lastNumberReceived;
								nlohmann::json resumePayload = resumeData;
								this->sendMessage(resumePayload, theShard, true);
								theShard->theState = WebSocketState::Connected;
							} else {
								WebSocketIdentifyData identityData{};
								identityData.botToken = this->configManager->getBotToken();
								identityData.currentShard = theShard->shard[0];
								identityData.numberOfShards = theShard->shard[1];
								identityData.intents = static_cast<int64_t>(this->configManager->getGatewayIntents());
								nlohmann::json identityJson = identityData;
								this->sendMessage(identityJson, theShard, true);
								theShard->theState = WebSocketState::Connected;
							}
							theShard->areWeHeartBeating = false;
							break;
						}
						case 11: {
							theShard->haveWeReceivedHeartbeatAck = true;
							break;
						}
					}
				}

				if (this->configManager->doWePrintWebSocketSuccessMessages() && !payload.is_null()) {
					std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from WebSocket " + theShard->shard.dump() + std::string(": ") << payload.dump()
							  << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
				}
				this->onClosed(theShard);
			}
		}
	}

	void BaseSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			while (!theToken.stop_requested() && !this->doWeQuit->load()) {
				if (this->connections.size() > 0) {
					this->internalConnect();
				}
				WebSocketSSLShard::processIO(this->theClients);
				for (auto& [key, value]: this->theClients) {
					if (this->theClients.contains(key) && this->theClients[key] && value->inputBuffer.size() > 0) {
						if (value) {
							this->parseHeadersAndMessage(value.get());
						}
					}
					if (this->theClients.contains(key) && this->theClients[key] && value->processedMessages.size() > 0) {
						while (value->processedMessages.size() > 0) {
							if (value) {
								this->onMessageReceived(value.get());
							}
						}
					}
					if (this->theClients.contains(key) && this->theClients[key]) {
						this->checkForAndSendHeartBeat(value.get());
						if (value && this->heartbeatInterval != 0 && !value->areWeHeartBeating) {
							value->areWeHeartBeating = true;
							value->heartBeatStopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
						}
					}
				}
				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			}
		}
	}

	void BaseSocketAgent::internalConnect() noexcept {
		try {
			if (this->connections.size() > 0) {
				DiscordCoreAPI::ConnectionPackage connectData = this->connections.front();
				this->connections.pop();
				if (!this->theClients.contains(connectData.currentShard)) {
					this->theClients[connectData.currentShard] =
						std::make_unique<WebSocketSSLShard>(&this->connections, this->currentBaseSocketAgent, connectData.currentShard, this->configManager);
				}
				this->theClients[connectData.currentShard]->currentReconnectionTries = connectData.currentReconnectionTries;
				this->theClients[connectData.currentShard]->currentReconnectionTries++;
				this->theClients[connectData.currentShard]->voiceConnectionDataBufferMap = std::move(connectData.voiceConnectionDataBufferMap);

				if (this->theClients[connectData.currentShard]->currentReconnectionTries >= this->maxReconnectTries) {
					this->doWeQuit->store(true);
					return;
				}

				try {
					this->theClients[connectData.currentShard]->connect(this->configManager->getConnectionAddress(), this->configManager->getConnectionPort());
				} catch (...) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						DiscordCoreAPI::reportException("BaseSocketAgent::internalConnect()");
					}
					this->onClosed(this->theClients[connectData.currentShard].get());
					return;
				}
				this->theClients[connectData.currentShard]->areWeConnected01.store(true);
				std::string sendString{};
				sendString = "GET /?v=10&encoding=";
				sendString += this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf ? "etf" : "json";
				sendString += " HTTP/1.1\r\nHost: " + this->configManager->getConnectionAddress() +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
					DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				bool didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				do {
					if (theStopWatch.hasTimePassed()) {
						return;
					}
					didWeWrite = this->theClients[connectData.currentShard]->writeData(sendString, true);
				} while (!didWeWrite);
				if (!didWeWrite) {
					throw ProcessingError{ "Failed to write to the websocket." };
				}

				while (!this->doWeQuit->load()) {
					if (this->theClients[connectData.currentShard]->theState == WebSocketState::Connected) {
						break;
					}
					WebSocketSSLShard::processIO(this->theClients, 10000);
					if (this->theClients.contains(connectData.currentShard)) {
						this->parseHeadersAndMessage(this->theClients[connectData.currentShard].get());
					}
					if (this->theClients.contains(connectData.currentShard)) {
						this->onMessageReceived(this->theClients[connectData.currentShard].get());
					}
					std::this_thread::sleep_for(1ms);
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
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

	VoiceSocketAgent::VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, WebSocketSSLShard* theShard,
		DiscordCoreAPI::ConfigManager* configManagerNew, std::atomic_bool* doWeQuitNew) noexcept {
		theShard->voiceConnectionDataBufferMap[initDataNew.guildId] = &this->voiceConnectionDataBuffer;
		this->voiceConnectInitData = initDataNew;
		this->configManager = configManagerNew;
		this->theBaseShard = theShard;
		this->doWeQuit = doWeQuitNew;
		this->theTask = std::make_unique<std::jthread>([this](std::stop_token theToken) {
			this->run(theToken);
		});
	}

	void VoiceSocketAgent::stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, WebSocketOpCode theOpCode) noexcept {
		std::string theVector{};
		std::string header{};
		theVector = dataToSend.dump();
		this->createHeader(header, theVector.size(), theOpCode);
		std::string theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		theString = theVectorNew;
	}

	void VoiceSocketAgent::parseHeadersAndMessage(WebSocketSSLShard* theShard) noexcept {
		if (theShard) {
			if (theShard->theState == WebSocketState::Connecting01) {
				std::string newVector = theShard->getInputBuffer();
				if (newVector.find("\r\n\r\n") != std::string::npos) {
					newVector.erase(0, newVector.find("\r\n\r\n") + 4);
					theShard->inputBuffer.clear();
					theShard->inputBuffer.insert(theShard->inputBuffer.end(), newVector.begin(), newVector.end());
					theShard->theState = WebSocketState::Connecting02;
				}
			}
			if (theShard->inputBuffer.size() < 4) {
				return;
			}
			theShard->dataOpCode = static_cast<WebSocketOpCode>(theShard->inputBuffer[0] & ~webSocketFinishBit);
			switch (theShard->dataOpCode) {
				case WebSocketOpCode::Op_Continuation:
					[[fallthrough]];
				case WebSocketOpCode::Op_Text:
					[[fallthrough]];
				case WebSocketOpCode::Op_Binary:
					[[fallthrough]];
				case WebSocketOpCode::Op_Ping:
					[[fallthrough]];
				case WebSocketOpCode::Op_Pong: {
					uint8_t length01 = theShard->inputBuffer[1];
					theShard->messageOffset = 2;
					if (length01 & webSocketMaskBit) {
						return;
					}
					theShard->messageLength = length01;
					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (theShard->inputBuffer.size() < 8) {
							return;
						}
						uint8_t length03 = theShard->inputBuffer[2];
						uint8_t length04 = theShard->inputBuffer[3];
						theShard->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
						theShard->messageOffset += 2;
					} else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (theShard->inputBuffer.size() < 10) {
							return;
						}
						theShard->messageLength = 0;
						for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
							uint8_t lengthNew = static_cast<uint8_t>(theShard->inputBuffer[x]);
							theShard->messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
						}
						theShard->messageOffset += 8;
					}
					if (theShard->inputBuffer.size() < static_cast<uint64_t>(theShard->messageOffset) + static_cast<uint64_t>(theShard->messageLength)) {
						return;
					} else {
						std::string finalMessage{};
						finalMessage.insert(finalMessage.begin(), theShard->inputBuffer.begin() + theShard->messageOffset,
							theShard->inputBuffer.begin() + theShard->messageOffset + theShard->messageLength);
						theShard->processedMessages.push(finalMessage);
						theShard->inputBuffer.erase(theShard->inputBuffer.begin(), theShard->inputBuffer.begin() + theShard->messageOffset + theShard->messageLength);
						return;
					}
				}
				case WebSocketOpCode::Op_Close: {
					uint16_t close = theShard->inputBuffer[2] & 0xff;
					close <<= 8;
					close |= theShard->inputBuffer[3] & 0xff;
					theShard->closeCode = static_cast<WebSocketCloseCode>(close);
					theShard->inputBuffer.erase(theShard->inputBuffer.begin(), theShard->inputBuffer.begin() + 4);
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						std::cout << DiscordCoreAPI::shiftToBrightRed()
								  << "Voice WebSocket " + this->theClients[0]->shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(this->theClients[0]->closeCode)
								  << DiscordCoreAPI::reset() << std::endl
								  << std::endl;
					}
					this->onClosed();
				}
				default: {
				}
			}
		}
	}

	void VoiceSocketAgent::sendMessage(const nlohmann::json& dataToSend) noexcept {
		try {
			if (this->configManager->doWePrintWebSocketSuccessMessages()) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << dataToSend.dump() << DiscordCoreAPI::reset() << std::endl << std::endl;
			}
			while (!this->theClients[0]->areWeConnected01.load()) {
				std::this_thread::sleep_for(1ms);
			}
			std::string theString{};
			this->stringifyJsonData(dataToSend, theString, WebSocketOpCode::Op_Text);
			DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
			bool didWeWrite{ false };
			do {
				if (theStopWatch.hasTimePassed()) {
					break;
				}
				didWeWrite = this->theClients[0]->writeData(theString, false);
			} while (!didWeWrite);
			if (!didWeWrite) {
				throw ProcessingError{ "Failed to write to the websocket." };
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::sendVoiceData(std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Please specify voice data to send" << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			} else {
				if (this->voiceSocket) {
					this->voiceSocket->writeData(responseData);
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendVoiceData()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::sendMessage(std::string& dataToSend) noexcept {
		try {
			if (this->configManager->doWePrintWebSocketSuccessMessages()) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << std::endl << dataToSend << DiscordCoreAPI::reset();
			}
			while (!this->theClients[0]->areWeConnected01.load()) {
				std::this_thread::sleep_for(1ms);
			}
			if (this->theClients[0] && this->theClients[0]->areWeStillConnected()) {
				bool didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				do {
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					didWeWrite = this->theClients[0]->writeData(dataToSend, false);
				} while (!didWeWrite);
				if (!didWeWrite) {
					throw ProcessingError{ "Failed to write to the websocket." };
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::onClosed() noexcept {
		if (this->theClients.contains(0) && this->theClients[0] && !this->doWeReconnect.load() ) {
			this->theClients[0]->disconnect();
			this->doWeReconnect.store(true);
			this->areWeConnected.store(false);
			this->theClients[0]->areWeHeartBeating = false;
			this->theClients[0]->areWeConnected01.store(false);
			this->voiceSocket->areWeConnected.store(false);
		}
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
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::createHeader()");
			}
		}
	}

	void VoiceSocketAgent::onMessageReceived(const std::string& theMessage) noexcept {
		try {
			if (this->theClients[0]) {
				nlohmann::json payload = payload.parse(theMessage);
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from Voice WebSocket: " << theMessage << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				if (payload.contains("op") && !payload["op"].is_null()) {
					switch (payload["op"].get<int32_t>()) {
						case 2: {
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
							VoiceSocketProtocolPayloadData protocolPayloadData{};
							protocolPayloadData.externalIp = this->voiceConnectionData.externalIp;
							protocolPayloadData.voiceEncryptionMode = this->voiceConnectionData.voiceEncryptionMode;
							protocolPayloadData.voicePort = this->voiceConnectionData.voicePort;
							nlohmann::json protocolPayloadSelectString = protocolPayloadData;
							this->sendMessage(protocolPayloadSelectString);
							break;
						}
						case 4: {
							this->areWeConnected.store(true);
							for (uint32_t x = 0; x < payload["d"]["secret_key"].size(); x++) {
								this->voiceConnectionData.secretKey.push_back(payload["d"]["secret_key"][x].get<uint8_t>());
							}
							break;
						}
						case 6: {
							this->theClients[0]->haveWeReceivedHeartbeatAck = true;
							break;
						}
						case 8: {
							this->theClients[0]->theState = WebSocketState::Connected;
							if (payload["d"].contains("heartbeat_interval")) {
								this->heartbeatInterval = static_cast<int32_t>(payload["d"]["heartbeat_interval"].get<float>());
								this->theClients[0]->areWeHeartBeating = false;
							}
							this->theClients[0]->haveWeReceivedHeartbeatAck = true;
							VoiceIdentifyData identifyData{};
							identifyData.connectInitData = this->voiceConnectInitData;
							identifyData.connectionData = this->voiceConnectionData;
							nlohmann::json identityDataFinal = identifyData;
							this->sendMessage(identityDataFinal);
							break;
						}
					}
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::onMessageReceived()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::run(std::stop_token theToken) noexcept {
		try {
			DiscordCoreAPI::StopWatch theStopWatch{ 10000ms };
			while (!this->theClients.contains(0)) {
				if (theStopWatch.hasTimePassed()) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
			theStopWatch.resetTimer();
			while (!this->theClients[0]->areWeConnected01.load()) {
				if (theStopWatch.hasTimePassed()) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
			while (!theToken.stop_requested() && !this->doWeQuit->load() && !this->doWeDisconnect.load()) {
				if (!theToken.stop_requested() && !this->doWeReconnect.load() && this->voiceSocket) {
					this->voiceSocket->processIO();
					this->voiceSocket->getInputBuffer();
				}
				if (this->heartbeatInterval != 0 && !this->doWeReconnect.load() && !this->theClients[0]->areWeHeartBeating) {
					this->theClients[0]->areWeHeartBeating = true;
					this->theClients[0]->heartBeatStopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
				}
				if (!theToken.stop_requested() && !this->doWeReconnect.load() && this->theClients.contains(0) && this->theClients[0]->heartBeatStopWatch.hasTimePassed() &&
					this->theClients[0]->areWeHeartBeating) {
					this->sendHeartBeat();
					this->theClients[0]->heartBeatStopWatch.resetTimer();
				}
				if (!theToken.stop_requested() && !this->doWeReconnect.load() && this->theClients.contains(0) && this->theClients[0]->areWeStillConnected() &&
					!this->doWeQuit->load()) {
					WebSocketSSLShard::processIO(this->theClients, 1000);
				}
				if (!theToken.stop_requested() && !this->doWeReconnect.load() && this->voiceSocket) {
					this->voiceSocket->processIO();
					this->voiceSocket->getInputBuffer();
				}
				if (!theToken.stop_requested() && !this->doWeReconnect.load() && this->theClients.contains(0) && this->theClients[0] && !this->doWeQuit->load()) {
					this->parseHeadersAndMessage(this->theClients[0].get());
					if (this->theClients.contains(0) && this->theClients[0] && this->theClients[0]->processedMessages.size() > 0) {
						std::string theMessage = this->theClients[0]->processedMessages.front();
						if (theMessage.size() > 0) {
							this->onMessageReceived(theMessage);
						}
						this->theClients[0]->processedMessages.pop();
					}
				}
				if (!theToken.stop_requested() && !this->doWeReconnect.load() && this->voiceSocket) {
					this->voiceSocket->processIO();
					this->voiceSocket->getInputBuffer();
				}
				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::run()");
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
			this->voiceSocket->writeData(packet);
			std::this_thread::sleep_for(100ms);
			std::string inputString{};
			while (inputString.size() < 74) {
				this->voiceSocket->processIO();
				std::string theNewString = this->voiceSocket->getInputBuffer();
				inputString.insert(inputString.end(), theNewString.begin(), theNewString.end());
				std::this_thread::sleep_for(1ms);
			}
			std::string message{};
			message.insert(message.begin(), inputString.begin() + 8, inputString.begin() + 64);
			if (message.find('\u0000') != std::string::npos) {
				message = message.substr(0, message.find('\u0000', 5));
			}
			this->voiceConnectionData.externalIp = message;
			this->voiceSocket->areWeConnected.store(true);
			this->voiceConnectionDataBuffer.clearContents();
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::collectExternalIP()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::sendHeartBeat() noexcept {
		try {
			if (this->theClients[0] && this->theClients[0]->areWeStillConnected() && this->theClients[0]->haveWeReceivedHeartbeatAck) {
				nlohmann::json data{};
				data["d"] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				data["op"] = int32_t(3);
				this->sendMessage(data);
				this->theClients[0]->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosed();
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::sendHeartBeat()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::voiceConnect() noexcept {
		try {
			this->voiceSocket = std::make_unique<DatagramSocketSSLClient>();
			this->voiceSocket->connect(this->voiceConnectionData.voiceIp, this->voiceConnectionData.voicePort);
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceSocketAgent::voiceConnect()");
			}
			this->onClosed();
		}
	}

	void VoiceSocketAgent::connect() noexcept {
		try {
			this->voiceConnectionData = VoiceConnectionData{};
			DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 20000);
			this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
			auto theClient = std::make_unique<WebSocketSSLShard>(nullptr, 0, 0, this->configManager);
			theClient->connect(this->baseUrl, "443");
			std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->theClients[0] = std::move(theClient);
			bool didWeWrite{ false };
			DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
			do {
				if (theStopWatch.hasTimePassed()) {
					break;
				}
				didWeWrite = this->theClients[0]->writeData(sendVector, true);
			} while (!didWeWrite);
			if (!didWeWrite) {
				throw ProcessingError{ "Failed to write to the websocket." };
			}

			this->theClients[0]->areWeConnected01.store(true);
			WebSocketSSLShard::processIO(this->theClients);
			DiscordCoreAPI::StopWatch theStopWatch02{ 10000ms };
			while (!this->doWeQuit->load()) {
				if (this->theClients[0]->theState == WebSocketState::Connected) {
					break;
				}
				if (this->theClients.contains(0) && this->theClients[0]->inputBuffer.size() > 0) {
					this->parseHeadersAndMessage(this->theClients[0].get());
				}
				if (this->theClients[0]->processedMessages.size() > 0) {
					std::string theMessage = this->theClients[0]->processedMessages.front();
					this->theClients[0]->processedMessages.pop();
					this->onMessageReceived(theMessage);
				}
				WebSocketSSLShard::processIO(this->theClients);
				std::this_thread::sleep_for(1ms);
				if (this->theClients.contains(0) && theStopWatch02.hasTimePassed()) {
					this->theClients.erase(0);
					this->doWeReconnect.store(true);
					return;
				}
			}
			this->doWeReconnect.store(false);
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
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
	};
}