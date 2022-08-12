/*
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

namespace DiscordCoreInternal {

	constexpr uint16_t webSocketMaxPayloadLengthLarge{ 65535u };
	constexpr uint8_t webSocketPayloadLengthMagicLarge{ 126u };
	constexpr uint8_t webSocketPayloadLengthMagicHuge{ 127u };
	constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2u };
	constexpr uint8_t webSocketMaxPayloadLengthSmall{ 125u };
	constexpr uint8_t webSocketFinishBit{ (1u << 7u) };
	constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

	WebSocketMessageHandler::WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManagerNew) {
		this->configManager = configManagerNew;
	}

	void WebSocketMessageHandler::stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, WebSocketOpCode theOpCode) noexcept {
		std::string theVector{};
		std::string header{};
		if (theOpCode == WebSocketOpCode::Op_Binary) {
			theVector = this->parseJsonToEtf(dataToSend);
		} else {
			theVector = dataToSend.dump();
		}
		this->createHeader(header, theVector.size(), theOpCode);
		std::string theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		theString = theVectorNew;
	}

	void WebSocketMessageHandler::createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept {
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
				DiscordCoreAPI::reportException("WebSocketMessageHandler::createHeader()");
			}
		}
	}

	bool WebSocketMessageHandler::parseConnectionHeaders() noexcept {
		auto theFindValue = this->inputBuffer.find("\r\n\r\n");
		if (theFindValue != std::string::npos) {
			this->inputBuffer.erase(0, theFindValue + 4);
			return true;
		}
		return false;
	}

	bool WebSocketMessageHandler::parseMessage(DiscordCoreAPI::StopWatch<std::chrono::microseconds>& theStopWatch) noexcept {
		if (this->inputBuffer.size() < 4) {
			return true;
		}
		auto dataOpCode = static_cast<WebSocketOpCode>(this->inputBuffer[0] & ~webSocketFinishBit);
		switch (dataOpCode) {
			case WebSocketOpCode::Op_Continuation:
				[[fallthrough]];
			case WebSocketOpCode::Op_Text:
				[[fallthrough]];
			case WebSocketOpCode::Op_Binary:
				[[fallthrough]];
			case WebSocketOpCode::Op_Ping:
				[[fallthrough]];
			case WebSocketOpCode::Op_Pong: {
				uint8_t length01 = this->inputBuffer[1];
				this->messageOffset = 2;
				if (length01 & webSocketMaskBit) {
					return true;
				}
				this->messageLength = length01;
				if (length01 == webSocketPayloadLengthMagicLarge) {
					if (this->inputBuffer.size() < 8) {
						return true;
					}
					uint8_t length03 = this->inputBuffer[2];
					uint8_t length04 = this->inputBuffer[3];
					this->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
					this->messageOffset += 2;
				} else if (length01 == webSocketPayloadLengthMagicHuge) {
					if (this->inputBuffer.size() < 10) {
						return true;
					}
					this->messageLength = 0;
					for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
						uint8_t lengthNew = static_cast<uint8_t>(this->inputBuffer[x]);
						this->messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
					}
					this->messageOffset += 8;
				}
				if (this->inputBuffer.size() < static_cast<uint64_t>(this->messageOffset) + static_cast<uint64_t>(this->messageLength)) {
					return true;
				} else {
					theStopWatch.resetTimer();
					this->inputBuffer.erase(this->inputBuffer.begin(), this->inputBuffer.begin() + this->messageOffset);
					if (!this->onMessageReceived(theStopWatch)) {
						return false;
					}
					theStopWatch.resetTimer();
					this->inputBuffer.erase(this->inputBuffer.begin(), this->inputBuffer.begin() + this->messageLength);
					return true;
				}
			}
			case WebSocketOpCode::Op_Close: {
				uint16_t close = this->inputBuffer[2] & 0xff;
				close <<= 8;
				close |= this->inputBuffer[3] & 0xff;
				auto closeCode = close;
				if (closeCode) {
				}
				this->inputBuffer.erase(this->inputBuffer.begin(), this->inputBuffer.begin() + 4);
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket " << " Closed; Code: " << +static_cast<uint16_t>(closeCode) << DiscordCoreAPI::reset() << endl << endl;
				}

				return false;
			}
			default: {
				return true;
			}
		}
	}

	WebSocketSSLShard::WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* theClient, std::queue<DiscordCoreAPI::ConnectionPackage>* connectionsNew, int32_t currentShardNew,
		std::atomic_bool* doWeQuitNew) noexcept
		: WebSocketMessageHandler(&theClient->configManager) {
		this->heartBeatStopWatch = DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ 10000ms };
		this->configManager = &theClient->configManager;
		this->discordCoreClient = theClient;
		this->shard.push_back(currentShardNew);
		this->discordCoreClient->theBaseShardCount.store(this->discordCoreClient->theBaseShardCount.load() + 1);
		this->connections = connectionsNew;
		this->doWeQuit = doWeQuitNew;
		if (theClient) {
			this->shard.push_back(this->discordCoreClient->configManager.getTotalShardCount());
			if (this->discordCoreClient->configManager.getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
				this->dataOpCode = WebSocketOpCode::Op_Binary;
			} else {
				this->dataOpCode = WebSocketOpCode::Op_Text;
			}
		}
	}

	void WebSocketSSLShard::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept {
		if (this->theWebSocketState.load() == WebSocketSSLShardState::Authenticated) {
			try {
				int32_t theCurrentIndex = this->shard[0];
				DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
				dataPackage.channelId = 0;
				dataPackage.guildId = doWeCollect.guildId;
				dataPackage.selfDeaf = doWeCollect.selfDeaf;
				dataPackage.selfMute = doWeCollect.selfMute;
				this->userId = doWeCollect.userId;
				nlohmann::json newData = dataPackage;
				std::string theString{};
				this->stringifyJsonData(newData, theString, this->dataOpCode);
				bool didWeWrite{ false };
				if (!this->sendMessage(theString, true)) {
					return;
				}
				if (doWeCollect.channelId == 0) {
					return;
				}
				dataPackage.channelId = doWeCollect.channelId;
				nlohmann::json newData02 = dataPackage;
				std::string theString02{};
				this->stringifyJsonData(newData02, theString02, this->dataOpCode);
				this->areWeCollectingData = true;
				if (!this->sendMessage(theString02, true)) {
					return;
				}
				DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theStopWatch{ 5500ms };
				while (this->areWeCollectingData) {
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					std::this_thread::sleep_for(1ms);
				}
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
				}
				this->onClosed();
			}
		}
	}

	bool WebSocketSSLShard::sendMessage(std::string& dataToSend, bool priority) noexcept {
		if (this->theSSLState.load() == SSLConnectionState::Connected) {
			try {
				if (dataToSend.size() == 0) {
					return false;
				}
				if (true) {
					cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->shard.dump() + std::string("'s Message: ") << dataToSend << endl
						 << endl
						 << DiscordCoreAPI::reset();
				}
				ProcessIOResult didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				do {
					if (theStopWatch.hasTimePassed()) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							DiscordCoreAPI::reportException("WebSocketSSLShard::sendMessage()");
						}
						this->onClosed();
						return false;
					}
					didWeWrite = this->writeData(dataToSend, true);
				} while (didWeWrite != ProcessIOResult::No_Error);
				if (didWeWrite != ProcessIOResult::No_Error) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						DiscordCoreAPI::reportException("WebSocketSSLShard::sendMessage()");
					}
					this->onClosed();
					return false;
				}
				return true;
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("WebSocketSSLShard::sendMessage()");
				}
				this->onClosed();
				return false;
			}
		}
		return false;
	}

	void WebSocketSSLShard::checkForAndSendHeartBeat(bool isImmediate) noexcept {
		try {
			nlohmann::json heartbeat{};
			heartbeat["d"] = this->lastNumberReceived;
			heartbeat["op"] = 1;
			std::string theString{};
			this->stringifyJsonData(heartbeat, theString, this->dataOpCode);
			if (!this->sendMessage(theString, true)) {
				return;
			}
			this->haveWeReceivedHeartbeatAck = false;
			
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::checkForAndSendHeartBeat()");
			}
			this->onClosed();
		}
	}

	void WebSocketSSLShard::disconnect(bool doWeReconnect) noexcept {
		if (this->theSSLState.load() == SSLConnectionState::Connected) {
			std::unique_lock theLock{ this->connectionMutex };
			this->theSSLState.store(SSLConnectionState::Disconnected);
			this->theWebSocketState.store(WebSocketSSLShardState::Disconnected);
			this->theSocket = SOCKET_ERROR;
			this->inputBuffer.clear();
			this->outputBuffers.clear();
			this->closeCode = 0;
			this->areWeHeartBeating = false;
			if (this->connections && doWeReconnect) {
				DiscordCoreAPI::ConnectionPackage theData{};
				theData.voiceConnectionDataBufferMap = std::move(this->voiceConnectionDataBufferMap);
				theData.currentReconnectTries = this->currentReconnectTries;
				theData.currentShard = this->shard[0];
				this->connections->push(theData);
			}
		}
	};
	
	void WebSocketSSLShard::dispatchBuffer()noexcept {
		DiscordCoreAPI::StopWatch theStopWatch{ 50us };
		this->parseMessage(theStopWatch);
	}

	bool WebSocketSSLShard::onMessageReceived( DiscordCoreAPI::StopWatch<std::chrono::microseconds>& theStopWatch) noexcept {
		if (this) {
			if (this->theSSLState.load() == SSLConnectionState::Connected) {
				try {
					bool returnValue{ true };
					if (this->inputBuffer.size() > 1) {
						if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
							try {
								theStopWatch.resetTimer();
								std::string theStringNew{};
								theStringNew.insert(theStringNew.begin(), this->inputBuffer.begin(), this->inputBuffer.end());
								std::cout << "THE TIME TO COPY THE STRING: " << theStopWatch.totalTimePassed() << std::endl;
								theStopWatch.resetTimer();
								this->payload = this->parseEtfToJson(this->inputBuffer);
								std::cout << "THE TIME TO PARSE THE STRING: " << theStopWatch.totalTimePassed() << std::endl;
								theStopWatch.resetTimer();
								theStringNew.clear();
								std::cout << "THE TIME TO CLEAR THE STRING: " << theStopWatch.totalTimePassed() << std::endl;
								theStopWatch.resetTimer();
							} catch (...) {
								if (this->configManager->doWePrintGeneralErrorMessages()) {
									DiscordCoreAPI::reportException("ErlPacker::parseEtfToJson()");
								}
								return false;
							}
						} else {
							this->payload = nlohmann::json::parse(this->inputBuffer);
						}
						
						if (this->configManager->doWePrintWebSocketSuccessMessages()) {
							cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from WebSocket " + this->shard.dump() + std::string(": ") << this->payload .dump()
								 << DiscordCoreAPI::reset() << endl
								 << endl;
						}
						if (this->payload .contains("t") && !this->payload ["t"].is_null()) {
							if (this->payload ["t"] == "RESUMED") {
								this->theWebSocketState.store(WebSocketSSLShardState::Authenticated);
								this->currentReconnectTries = 0;
							}

							if (this->payload ["t"] == "READY") {
								this->theWebSocketState.store(WebSocketSSLShardState::Authenticated);
								this->sessionId = this->payload ["d"]["session_id"].get<std::string>();
								DiscordCoreAPI::UserData theUser{ this->payload["d"]["user"] };
								this->discordCoreClient->currentUser =
									DiscordCoreAPI::BotUser{ theUser, this->discordCoreClient->baseSocketAgentMap[std::to_string(this->shard[0].get<int32_t>())].get() };
								DiscordCoreAPI::Users::insertUser(std::make_unique<DiscordCoreAPI::UserData>(theUser));
								this->currentReconnectTries = 0;
							}
						}

						if (this->payload .contains("s") && !this->payload ["s"].is_null()) {
							this->lastNumberReceived = this->payload ["s"].get<int32_t>();
						}
						if (this->payload .contains("op") && !this->payload ["op"].is_null()) {
							switch (this->payload ["op"].get<int32_t>()) {
								case 0: {
									if (this->payload .contains("d") && !this->payload ["d"].is_null() && this->payload .contains("t") && !this->payload ["t"].is_null()) {
										if (this->payload ["t"] == "APPLICATION_COMMAND_PERMISSIONS_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage {
												std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>()
											};
											dataPackage->permissionData = this->payload["d"];
											this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "AUTO_MODERATION_RULE_CREATE") {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>()
											};
											dataPackage->theRule = this->payload["d"];
											this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
										} else if (this->payload ["t"] == "AUTO_MODERATION_RULE_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>()
											};
											dataPackage->theRule = this->payload["d"];
											this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "AUTO_MODERATION_RULE_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>()
											};
											dataPackage->theRule = this->payload["d"];
											this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "AUTO_MODERATION_ACTION_EXECUTION") {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>()
											};
											dataPackage->theData = this->payload["d"];
											this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
										} else if (this->payload ["t"] == "CHANNEL_CREATE") {
											std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>() };
											dataPackage->channel = this->payload["d"];
											this->discordCoreClient->eventManager.onChannelCreationEvent(*dataPackage);
										} else if (this->payload ["t"] == "CHANNEL_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>() };
											if (this->payload ["d"].contains("id") && !this->payload ["d"]["id"].is_null()) {
												dataPackage->channelOld = DiscordCoreAPI::Channel(
													DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(this->payload ["d"]["id"].get<std::string>()) }).get());
												dataPackage->channelNew = DiscordCoreAPI::Channel(dataPackage->channelOld);
											}
											dataPackage->channelNew = this->payload["d"];
											this->discordCoreClient->eventManager.onChannelUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "CHANNEL_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>() };
											dataPackage->channel = this->payload["d"];
											this->discordCoreClient->eventManager.onChannelDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "CHANNEL_PINS_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
											dataPackage->dataPackage = this->payload["d"];
											this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "THREAD_CREATE") {
											std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
											dataPackage->thread = this->payload["d"];
											this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
										} else if (this->payload ["t"] == "THREAD_UPATE") {
											std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
											dataPackage->thread = this->payload["d"];
											this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "THREAD_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
											dataPackage->thread = this->payload["d"];
											this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "THREAD_LIST_SYNC") {
											std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
											dataPackage->threadListSyncData = this->payload["d"];
											this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
										} else if (this->payload ["t"] == "THREAD_MEMBER_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
											dataPackage->threadMember = this->payload["d"];
											this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "THREAD_MEMBERS_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
											dataPackage->threadMembersUpdateData = this->payload["d"];
											this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_CREATE") {
											theStopWatch.resetTimer();
											std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>() };
											
											std::cout << "TIME PASSED 0202: " << theStopWatch.totalTimePassed() << std::endl;
											theStopWatch.resetTimer();
											DiscordCoreAPI::GuildData* theGuild = new DiscordCoreAPI::GuildData{ this->payload["d"] };
											std::cout << "TIME PASSED 0303: " << theStopWatch.totalTimePassed() << std::endl;
											theStopWatch.resetTimer();
											Snowflake guildId = theGuild->id;
											theGuild->discordCoreClient = this->discordCoreClient;
											std::cout << "TIME PASSED 0404: " << theStopWatch.totalTimePassed() << std::endl;
											theStopWatch.resetTimer();
											auto theGuildNew = std::make_unique<DiscordCoreAPI::GuildData>();
											std::cout << "TIME PASSED 0505: " << theStopWatch.totalTimePassed() << std::endl;
											theStopWatch.resetTimer();
											theGuildNew.reset(theGuild);
											DiscordCoreAPI::Guilds::insertGuild(std::move(theGuildNew));
											std::cout << "TIME PASSED 0606: " << theStopWatch.totalTimePassed() << std::endl;
											//dataPackage->guild = (*DiscordCoreAPI::Guilds::cache)[guildId].get();
											//dataPackage->guild->discordCoreClient = this->discordCoreClient;
											//this->discordCoreClient->eventManager.onGuildCreationEvent(std::move(*dataPackage));
											theStopWatch.resetTimer();
										} else if (this->payload ["t"] == "GUILD_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>() };
											std::unique_ptr<DiscordCoreAPI::GuildData> theGuild{ std::make_unique<DiscordCoreAPI::GuildData>(this->payload["d"]) };
											if (this->payload ["d"].contains("id") && !this->payload ["d"]["id"].is_null()) {
												dataPackage->guildOld = std::make_unique<DiscordCoreAPI::GuildData>(DiscordCoreAPI::Guild(
													DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(this->payload ["d"]["id"].get<std::string>()) }).get()));
												dataPackage->guildNew = theGuild.get();
											}
											DiscordCoreAPI::GuildMemberId theKey{};
											theKey.guildId = dataPackage->guildOld->id;
											for (auto& value: dataPackage->guildOld->members) {
												theKey.guildMemberId = value;
												DiscordCoreAPI::GuildMemberData* guildMember = (*DiscordCoreAPI::GuildMembers::cache)[theKey].get();
												DiscordCoreAPI::GuildMembers::removeGuildMember(std::make_unique<DiscordCoreAPI::GuildMemberData>(*guildMember));
											}
											for (auto& value: dataPackage->guildOld->channels) {
												DiscordCoreAPI::Channels::removeChannel(value);
											}
											for (auto& value: dataPackage->guildOld->roles) {
												DiscordCoreAPI::Roles::removeRole(value);
											}
											DiscordCoreAPI::Guilds::insertGuild(std::move(theGuild));
											this->discordCoreClient->eventManager.onGuildUpdateEvent(std::move(*dataPackage));
										} else if (this->payload ["t"] == "GUILD_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>() };
											dataPackage->guild = std::make_unique<DiscordCoreAPI::GuildData>(this->payload["d"]);
											DiscordCoreAPI::Guilds::removeGuild(dataPackage->guild->id);
											this->discordCoreClient->eventManager.onGuildDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_BAN_ADD") {
											std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("user") && !this->payload ["d"]["user"].is_null()) {
												DiscordCoreAPI::User newData{};
												newData = this->payload["d"]["user"];
												dataPackage->user = newData;
											}
											this->discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_BAN_REMOVE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("user") && !this->payload ["d"]["user"].is_null()) {
												DiscordCoreAPI::User newData{};
												newData = this->payload["d"]["user"];
												dataPackage->user = newData;
											}
											this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_EMOJIS_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
											dataPackage->updateData = this->payload["d"];
											this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_STICKERS_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
											dataPackage->updateData = this->payload["d"];
											this->discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_INTEGRATIONS_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>()
											};
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_MEMBER_ADD") {
											std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberAddData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildMember.guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											dataPackage->guildMember = this->payload["d"];
											dataPackage->discordCoreClient = this->discordCoreClient;
											this->discordCoreClient->eventManager.onGuildMemberAddEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_MEMBER_REMOVE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("user") && !this->payload ["d"]["user"].is_null()) {
												dataPackage->user = DiscordCoreAPI::User(this->payload["d"]["user"]);
											}
											this->discordCoreClient->eventManager.onGuildMemberRemoveEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_MEMBER_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildMemberNew.guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("user") && this->payload ["d"]["user"].contains("id") && !this->payload ["d"]["user"]["id"].is_null()) {
												dataPackage->guildMemberOld = DiscordCoreAPI::GuildMember(DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync(
													{ .guildMemberId = stoull(this->payload ["d"]["user"]["id"].get<std::string>()),
														.guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>()) })
																											  .get());
												dataPackage->guildMemberNew = DiscordCoreAPI::GuildMember(dataPackage->guildMemberOld);
											}
											dataPackage->guildMemberNew = this->payload["d"];
											this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_MEMBERS_CHUNK") {
											std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
											dataPackage->chunkEventData = this->payload["d"];
											this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_ROLE_CREATE") {
											std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>() };
											if (this->payload ["d"].contains("role") && !this->payload ["d"]["role"].is_null()) {
												dataPackage->role = this->payload["d"]["role"];
											}
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											this->discordCoreClient->eventManager.onRoleCreationEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_ROLE_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleUpdateData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											dataPackage->roleNew = this->payload["d"]["role"];
											if (this->payload ["d"].contains("role") && !this->payload ["d"]["role"].is_null()) {
												dataPackage->roleOld = DiscordCoreAPI::Role(DiscordCoreAPI::Roles::getCachedRoleAsync(
													{ .guildId = dataPackage->guildId, .roleId = stoull(this->payload ["d"]["role"]["id"].get<std::string>()) })
																								.get());
												dataPackage->roleNew = DiscordCoreAPI::Role(dataPackage->roleOld);
											}
											this->discordCoreClient->eventManager.onRoleUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "GUILD_ROLE_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleDeletionData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("role_id") && !this->payload ["d"]["role_id"].is_null()) {
												dataPackage->roleOld = DiscordCoreAPI::Role(DiscordCoreAPI::Roles::getCachedRoleAsync(
													{ .guildId = dataPackage->guildId, .roleId = stoull(this->payload ["d"]["role_id"].get<std::string>()) })
																								.get());
											}
											this->discordCoreClient->eventManager.onRoleDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "INTEGRATION_CREATE") {
											std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("integration") && !this->payload ["d"]["integration"].is_null()) {
												dataPackage->integrationData = this->payload["d"]["integration"];
											}
											this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
										} else if (this->payload ["t"] == "INTEGRATION_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("integration") && !this->payload ["d"]["integration"].is_null()) {
												dataPackage->integrationData = this->payload["d"]["integration"];
											}
											this->discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "INTEGRATION_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("application_id") && !this->payload ["d"]["application_id"].is_null()) {
												dataPackage->applicationId = stoull(this->payload ["d"]["application_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("id") && !this->payload ["d"]["id"].is_null()) {
												dataPackage->id = stoull(this->payload ["d"]["id"].get<std::string>());
											}
											this->discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "INTERACTION_CREATE") {
											std::unique_ptr<DiscordCoreAPI::InteractionData> interactionData{ std::make_unique<DiscordCoreAPI::InteractionData>() };
											*interactionData = this->payload["d"];
											std::unique_ptr<DiscordCoreAPI::InputEventData> eventData{ std::make_unique<DiscordCoreAPI::InputEventData>(*interactionData) };
											if (interactionData->type == DiscordCoreAPI::InteractionType::Application_Command) {
												eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
												*eventData->interactionData = *interactionData;
												std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
													std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
												};
												dataPackage->interactionData = *interactionData;
												std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>(*eventData) };
												DiscordCoreAPI::CommandData commandDataNew = *commandData;
												this->discordCoreClient->commandController.checkForAndRunCommand(commandDataNew);
												this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
												std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
													std::make_unique<DiscordCoreAPI::OnInputEventCreationData>()
												};
												eventCreationData->inputEventData = *eventData;
												this->discordCoreClient->eventManager.onInputEventCreationEvent(*eventCreationData);
											} else if (interactionData->type == DiscordCoreAPI::InteractionType::Message_Component) {
												if (interactionData->data.componentData.componentType == DiscordCoreAPI::ComponentType::Button) {
													eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
													*eventData->interactionData = *interactionData;
													std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
														std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
													};
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
													std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
														std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
													};
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
												std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
													std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
												};
												dataPackage->interactionData = *interactionData;
												std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
													std::make_unique<DiscordCoreAPI::OnInputEventCreationData>()
												};
												eventCreationData->inputEventData = *eventData;
												if (DiscordCoreAPI::ModalCollector::modalInteractionBufferMap.contains(std::to_string(eventData->getChannelId()))) {
													DiscordCoreAPI::ModalCollector::modalInteractionBufferMap[std::to_string(eventData->getChannelId())]->send(
														eventData->getInteractionData());
												}
												this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
											} else if (interactionData->type == DiscordCoreAPI::InteractionType::Application_Command_Autocomplete) {
												eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
												*eventData->interactionData = *interactionData;
												std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
													std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
												};
												dataPackage->interactionData = *interactionData;
												std::unique_ptr<DiscordCoreAPI::OnAutoCompleteEntryData> autocompleteEntryData{
													std::make_unique<DiscordCoreAPI::OnAutoCompleteEntryData>()
												};
												autocompleteEntryData->inputEvent = *eventData;
												this->discordCoreClient->eventManager.onAutoCompleteEntryEvent(*autocompleteEntryData);
												this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
											}
										} else if (this->payload ["t"] == "INVITE_CREATE") {
											std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
											dataPackage->invite = this->payload["d"];
											this->discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
										} else if (this->payload ["t"] == "INVITE_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteDeletionData>() };
											if (this->payload ["d"].contains("channel_id") && !this->payload ["d"]["channel_id"].is_null()) {
												dataPackage->channelId = stoull(this->payload ["d"]["channel_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("code") && !this->payload ["d"]["code"].is_null()) {
												dataPackage->code = this->payload ["d"]["code"];
											}
											this->discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "MESSAGE_CREATE") {
											std::unique_ptr<DiscordCoreAPI::Message> message{ std::make_unique<DiscordCoreAPI::Message>() };
											*message = this->payload["d"];
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
											std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
												std::make_unique<DiscordCoreAPI::OnInputEventCreationData>()
											};
										} else if (this->payload ["t"] == "MESSAGE_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
											dataPackage->messageNew = this->payload["d"];
											for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
												value->send(dataPackage->messageNew);
											}
											this->discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "MESSAGE_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeletionData>() };
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("channel_id") && !this->payload ["d"]["channel_id"].is_null()) {
												dataPackage->channelId = stoull(this->payload ["d"]["channel_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("id") && !this->payload ["d"]["id"].is_null()) {
												dataPackage->messageId = stoull(this->payload ["d"]["id"].get<std::string>());
											}
											this->discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "MESSAGE_DELETE_BULK") {
											std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
											if (this->payload ["d"].contains("channel_id") && !this->payload ["d"]["channel_id"].is_null()) {
												dataPackage->channelId = stoull(this->payload ["d"]["channel_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("ids") && !this->payload ["d"]["ids"].is_null()) {
												for (auto& value: this->payload ["d"]["ids"]) {
													dataPackage->ids.push_back(stoull(value.get<std::string>()));
												}
											}
											this->discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
										} else if (this->payload ["t"] == "MESSAGE_REACTION_ADD") {
											std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
											dataPackage->reaction = this->payload["d"];
											this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
										} else if (this->payload ["t"] == "MESSAGE_REACTION_REMOVE") {
											std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
											dataPackage->reactionRemoveData = this->payload["d"];
											this->discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
										} else if (this->payload ["t"] == "MESSAGE_REACTION_REMOVE_ALL") {
											std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>() };
											if (this->payload ["d"].contains("channel_id") && !this->payload ["d"]["channel_id"].is_null()) {
												dataPackage->channelId = stoull(this->payload ["d"]["channel_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("message_id") && !this->payload ["d"]["message_id"].is_null()) {
												dataPackage->messageId = stoull(this->payload ["d"]["message_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											this->discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
										} else if (this->payload ["t"] == "MESSAGE_REACTION_REMOVE_EMOJI") {
											std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>() };
											if (this->payload ["d"].contains("channel_id") && !this->payload ["d"]["channel_id"].is_null()) {
												dataPackage->channelId = stoull(this->payload ["d"]["channel_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("message_id") && !this->payload ["d"]["message_id"].is_null()) {
												dataPackage->messageId = stoull(this->payload ["d"]["message_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											dataPackage->emoji = this->payload["d"]["emoji"];
											this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
										} else if (this->payload ["t"] == "PRESENCE_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
											dataPackage->presenceData = this->payload["d"];
											this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "STAGE_INSTANCE_CREATE") {
											std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>()
											};
											dataPackage->stageInstance = this->payload["d"];
											this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
										} else if (this->payload ["t"] == "STAGE_INSTANCE_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
											dataPackage->stageInstance = this->payload["d"];
											this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "STAGE_INSTANCE_DELETE") {
											std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>()
											};
											dataPackage->stageInstance = this->payload["d"];
											this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
										} else if (this->payload ["t"] == "TYPING_START") {
											std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
											dataPackage->typingStartData = this->payload["d"];
											this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
										} else if (this->payload ["t"] == "USER_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
											dataPackage->userNew = DiscordCoreAPI::User(this->payload["d"]["user"]);
											dataPackage->userOld = DiscordCoreAPI::User(DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get());
											this->discordCoreClient->eventManager.onUserUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "VOICE_STATE_UPDATE") {
											if (this->areWeCollectingData && !this->stateUpdateCollected && !this->serverUpdateCollected &&
												this->payload ["d"]["member"]["user"]["id"] == std::to_string(this->userId)) {
												this->voiceConnectionData = VoiceConnectionData{};
												this->voiceConnectionData.sessionId = this->payload ["d"]["session_id"].get<std::string>();
												this->stateUpdateCollected = true;
											} else if (this->areWeCollectingData && !this->stateUpdateCollected) {
												this->voiceConnectionData.sessionId = this->payload ["d"]["session_id"].get<std::string>();
												if (this->voiceConnectionDataBufferMap.contains(stoull(this->payload ["d"]["guild_id"].get<std::string>()))) {
													this->voiceConnectionDataBufferMap[stoull(this->payload ["d"]["guild_id"].get<std::string>())]->send(this->voiceConnectionData);
												}
												this->serverUpdateCollected = false;
												this->stateUpdateCollected = false;
												this->areWeCollectingData = false;
											}
											std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
											dataPackage->voiceStateData = this->payload["d"];
											this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "VOICE_SERVER_UPDATE") {
											if (this->areWeCollectingData && !this->serverUpdateCollected && !this->stateUpdateCollected) {
												this->voiceConnectionData = VoiceConnectionData{};
												this->voiceConnectionData.endPoint = this->payload ["d"]["endpoint"].get<std::string>();
												this->voiceConnectionData.token = this->payload ["d"]["token"].get<std::string>();
												this->serverUpdateCollected = true;
											} else if (this->areWeCollectingData && !this->serverUpdateCollected) {
												this->voiceConnectionData.endPoint = this->payload ["d"]["endpoint"].get<std::string>();
												this->voiceConnectionData.token = this->payload ["d"]["token"].get<std::string>();
												if (this->voiceConnectionDataBufferMap.contains(stoull(this->payload ["d"]["guild_id"].get<std::string>()))) {
													this->voiceConnectionDataBufferMap[stoull(this->payload ["d"]["guild_id"].get<std::string>())]->send(this->voiceConnectionData);
												}
												this->serverUpdateCollected = false;
												this->stateUpdateCollected = false;
												this->areWeCollectingData = false;
											}
											std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>() };
											if (this->payload ["d"].contains("endpoint") && !this->payload ["d"]["endpoint"].is_null()) {
												dataPackage->endpoint = this->payload ["d"]["endpoint"];
											}
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("token") && !this->payload ["d"]["token"].is_null()) {
												dataPackage->token = this->payload ["d"]["token"];
											}
											this->discordCoreClient->eventManager.onVoiceServerUpdateEvent(*dataPackage);
										} else if (this->payload ["t"] == "WEBHOOKS_UPDATE") {
											std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
											if (this->payload ["d"].contains("channel_id") && !this->payload ["d"]["channel_id"].is_null()) {
												dataPackage->channelId = stoull(this->payload ["d"]["channel_id"].get<std::string>());
											}
											if (this->payload ["d"].contains("guild_id") && !this->payload ["d"]["guild_id"].is_null()) {
												dataPackage->guildId = stoull(this->payload ["d"]["guild_id"].get<std::string>());
											}
											this->discordCoreClient->eventManager.onWebhookUpdateEvent(*dataPackage);
										}
									}
									break;
								}
								case 1: {
									this->checkForAndSendHeartBeat(true);
									break;
								}
								case 7: {
									if (this->configManager->doWePrintWebSocketErrorMessages()) {
										cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->shard.dump() + " Reconnecting (Type 7)!" << DiscordCoreAPI::reset() << endl
											 << endl;
									}
									this->areWeResuming = true;
									this->onClosed();
									return true;
								}
								case 9: {
									if (this->configManager->doWePrintWebSocketErrorMessages()) {
										cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->shard.dump() + " Reconnecting (Type 9)!" << DiscordCoreAPI::reset() << endl
											 << endl;
									}
									std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
									int32_t numOfMsToWait = static_cast<int32_t>(
										1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
									std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
									if (this->payload ["d"] == true) {
										this->areWeResuming = true;
									} else {
										this->areWeResuming = false;
									}
									this->onClosed();
									return true;
								}
								case 10: {
									if (this->payload ["d"].contains("heartbeat_interval") && !this->payload ["d"]["heartbeat_interval"].is_null()) {
										this->heartBeatStopWatch =
											DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ std::chrono::milliseconds{ this->payload ["d"]["heartbeat_interval"] } };
										this->areWeHeartBeating = true;
									}
									if (this->areWeResuming) {
										WebSocketResumeData resumeData{};
										resumeData.botToken = this->configManager->getBotToken();
										resumeData.sessionId = this->sessionId;
										resumeData.lastNumberReceived = this->lastNumberReceived;
										nlohmann::json resumePayload = resumeData;
										std::string theString{};
										this->stringifyJsonData(resumePayload, theString, this->dataOpCode);
										if (!this->sendMessage(theString, true)) {
											return false;
										}
										this->theWebSocketState.store(WebSocketSSLShardState::Sending_Identify);
									} else {
										WebSocketIdentifyData identityData{};
										identityData.botToken = this->configManager->getBotToken();
										identityData.currentShard = this->shard[0];
										identityData.numberOfShards = this->shard[1];
										identityData.intents = static_cast<int64_t>(this->configManager->getGatewayIntents());
										identityData.presence = this->configManager->getPresenceData();
										nlohmann::json identityJson = identityData;
										std::string theString{};
										this->stringifyJsonData(identityJson, theString, this->dataOpCode);
										if (!this->sendMessage(theString, true)) {
											return false;
										}
										this->theWebSocketState.store(WebSocketSSLShardState::Sending_Identify);
									}
									this->areWeHeartBeating = false;
									break;
								}
								case 11: {
									this->haveWeReceivedHeartbeatAck = true;
									break;
								}
							}
						}
						return returnValue;
					}

				} catch (...) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
					}
					this->onClosed();
					return false;
				}
			}
		}	
		return false;
	}

	void WebSocketSSLShard::onClosed() noexcept {
		if (this->maxReconnectTries > this->currentReconnectTries) {
			this->disconnect(true);
		} else {
			if (this->doWeQuit) {
				this->doWeQuit->store(true);
			}
		}
	}

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentShardNew) noexcept {
		this->configManager = &discordCoreClientNew->configManager;
		this->discordCoreClient = discordCoreClientNew;
		this->currentShard = currentShardNew;
		this->doWeQuit = doWeQuitNew;
		this->taskThread = std::make_unique<std::jthread>([this](std::stop_token stopToken) {
			this->run(stopToken);
		});
	}

	void BaseSocketAgent::connectVoiceChannel(VoiceConnectInitData theData) noexcept {
		std::lock_guard theLock{ this->theMutex };
		this->voiceConnections.push(theData);
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage thePackage) noexcept {
		while (!this->discordCoreClient->theStopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
		}
		this->discordCoreClient->theStopWatch.resetTimer();
		std::unique_lock theLock{ this->theMutex };
		this->connections.push(thePackage);
		theLock.unlock();
		DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
		while (!this->sslShard) {
			if (theStopWatch.hasTimePassed()) {
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
		theStopWatch.resetTimer();
		while (this->sslShard->theWebSocketState.load() != WebSocketSSLShardState::Authenticated) {
			if (theStopWatch.hasTimePassed()) {
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->taskThread.get();
	}

	void BaseSocketAgent::connectVoiceInternal() noexcept {
		while (!this->theVCStopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
		}
		this->theVCStopWatch.resetTimer();
		while (!this->theVCStopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
		}
		this->theVCStopWatch.resetTimer();
		VoiceConnectInitData theConnectionData = this->voiceConnections.front();
		this->voiceConnections.pop();
		DiscordCoreAPI::getVoiceConnectionMap()[theConnectionData.guildId] = std::make_unique<DiscordCoreAPI::VoiceConnection>(this, theConnectionData,
			&this->discordCoreClient->configManager, this->doWeQuit, theConnectionData.streamType, theConnectionData.streamInfo);
		DiscordCoreAPI::getVoiceConnectionMap()[theConnectionData.guildId]->connect();
	}

	void BaseSocketAgent::run(std::stop_token& stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !this->doWeQuit->load()) {
				std::unique_lock theLock{ this->theMutex };
				if (this->connections.size() > 0) {
					this->connectInternal();
				}
				theLock.unlock();
				if (this->voiceConnectionsToDisconnect.size() > 0) {
					this->disconnectVoice();
				}
				if (this->voiceConnections.size() > 0) {
					this->connectVoiceInternal();
				}
				if (this->sslShard) {
					this->sslShard->processIO();
					if (this->sslShard->heartBeatStopWatch.hasTimePassed()) {
						this->sslShard->heartBeatStopWatch.resetTimer();
						this->sslShard->checkForAndSendHeartBeat();
					}
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			}
		}
	}

	void BaseSocketAgent::disconnectVoice() noexcept {
		std::lock_guard theLock{ this->theMutex };
		uint64_t theDCData = this->voiceConnectionsToDisconnect.front();
		this->voiceConnectionsToDisconnect.pop();
		DiscordCoreAPI::getVoiceConnectionMap()[theDCData]->disconnectInternal();
	}

	void BaseSocketAgent::connectInternal() noexcept {
		try {
			if (this->connections.size() > 0) {
				DiscordCoreAPI::ConnectionPackage connectData = this->connections.front();
				this->connections.pop();
				if (!this->sslShard) {
					this->sslShard = std::make_unique<WebSocketSSLShard>(this->discordCoreClient, &this->connections, connectData.currentShard, this->doWeQuit);
				}
				this->sslShard->currentReconnectTries = connectData.currentReconnectTries;
				this->sslShard->currentReconnectTries++;
				this->sslShard->voiceConnectionDataBufferMap = std::move(connectData.voiceConnectionDataBufferMap);

				if (this->sslShard->currentReconnectTries >= this->maxReconnectTries) {
					this->doWeQuit->store(true);
					return;
				}

				if (!this->sslShard->connect(this->configManager->getConnectionAddress(), this->configManager->getConnectionPort())) {
					this->sslShard->onClosed();
					return;
				}

				this->sslShard->theWebSocketState.store(WebSocketSSLShardState::Upgrading);
				std::string sendString{};
				sendString = "GET /?v=10&encoding=";
				sendString += this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf ? "etf" : "json";
				sendString += " HTTP/1.1\r\nHost: " + this->configManager->getConnectionAddress() +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
					DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				ProcessIOResult didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				do {
					if (theStopWatch.hasTimePassed()) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							DiscordCoreAPI::reportException("WebSocketSSLShard::connectInternal()");
						}
						this->sslShard->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
					didWeWrite = this->sslShard->writeData(sendString, true);
				} while (didWeWrite != ProcessIOResult::No_Error);
				if (didWeWrite != ProcessIOResult::No_Error) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						DiscordCoreAPI::reportException("WebSocketSSLShard::connectInternal()");
					}
					this->sslShard->onClosed();
					return;
				}

				while (!this->doWeQuit->load()) {
					if (this->sslShard->theWebSocketState.load() == WebSocketSSLShardState::Collecting_Hello) {
						break;
					}
					this->sslShard->processIO();
					if (this->sslShard->areWeStillConnected()) {
						this->sslShard->parseConnectionHeaders();
						this->sslShard->theWebSocketState.store(WebSocketSSLShardState::Collecting_Hello);
					}
					if (this->sslShard->areWeStillConnected()) {
						DiscordCoreAPI::StopWatch<std::chrono::microseconds> theStopWatch{};
					}
					std::this_thread::sleep_for(1ms);
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::connectInternal()");
			}
		}
	}

	BaseSocketAgent::~BaseSocketAgent() noexcept {};
}