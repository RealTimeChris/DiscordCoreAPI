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

namespace DiscordCoreAPI {
	extern std::atomic_int32_t theCount;
}

namespace DiscordCoreInternal {

	constexpr uint16_t webSocketMaxPayloadLengthLarge{ 65535u };
	constexpr uint8_t webSocketPayloadLengthMagicLarge{ 126u };
	constexpr uint8_t webSocketPayloadLengthMagicHuge{ 127u };
	constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2u };
	constexpr uint8_t webSocketMaxPayloadLengthSmall{ 125u };
	constexpr uint8_t webSocketFinishBit{ (1u << 7u) };
	constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

	EventConverter::EventConverter(std::string theNewEvent) {
		this->theEvent = theNewEvent;
	}

	EventConverter::operator int32_t() {
		if (this->theEvent == "APPLICATION_COMMAND_PERMISSIONS_UPDATE") {
			return 1;
		} else if (this->theEvent == "AUTO_MODERATION_RULE_CREATE") {
			return 2;
		} else if (this->theEvent == "AUTO_MODERATION_RULE_UPDATE") {
			return 3;
		} else if (this->theEvent == "AUTO_MODERATION_RULE_DELETE") {
			return 4;
		} else if (this->theEvent == "AUTO_MODERATION_ACTION_EXECUTION") {
			return 5;
		} else if (this->theEvent == "CHANNEL_CREATE") {
			return 6;
		} else if (this->theEvent == "CHANNEL_UPDATE") {
			return 7;
		} else if (this->theEvent == "CHANNEL_DELETE") {
			return 8;
		} else if (this->theEvent == "CHANNEL_PINS_UPDATE") {
			return 9;
		} else if (this->theEvent == "THREAD_CREATE") {
			return 10;
		} else if (this->theEvent == "THREAD_UPATE") {
			return 11;
		} else if (this->theEvent == "THREAD_DELETE") {
			return 12;
		} else if (this->theEvent == "THREAD_LIST_SYNC") {
			return 13;
		} else if (this->theEvent == "THREAD_MEMBER_UPDATE") {
			return 14;
		} else if (this->theEvent == "THREAD_MEMBERS_UPDATE") {
			return 15;
		} else if (this->theEvent == "GUILD_CREATE") {
			return 16;
		} else if (this->theEvent == "GUILD_UPDATE") {
			return 17;
		} else if (this->theEvent == "GUILD_DELETE") {
			return 18;
		} else if (this->theEvent == "GUILD_BAN_ADD") {
			return 19;
		} else if (this->theEvent == "GUILD_BAN_REMOVE") {
			return 20;
		} else if (this->theEvent == "GUILD_EMOJIS_UPDATE") {
			return 21;
		} else if (this->theEvent == "GUILD_STICKERS_UPDATE") {
			return 22;
		} else if (this->theEvent == "GUILD_INTEGRATIONS_UPDATE") {
			return 23;
		} else if (this->theEvent == "GUILD_MEMBER_ADD") {
			return 24;
		} else if (this->theEvent == "GUILD_MEMBER_UPDATE") {
			return 25;
		} else if (this->theEvent == "GUILD_MEMBER_REMOVE") {
			return 26;
		} else if (this->theEvent == "GUILD_MEMBERS_CHUNK") {
			return 27;
		} else if (this->theEvent == "GUILD_ROLE_CREATE") {
			return 28;
		} else if (this->theEvent == "GUILD_ROLE_UPDATE") {
			return 29;
		} else if (this->theEvent == "GUILD_ROLE_DELETE") {
			return 30;
		} else if (this->theEvent == "INTEGRATION_CREATE") {
			return 31;
		} else if (this->theEvent == "INTEGRATION_UPDATE") {
			return 32;
		} else if (this->theEvent == "INTEGRATION_DELETE") {
			return 33;
		} else if (this->theEvent == "INTERACTION_CREATE") {
			return 34;
		} else if (this->theEvent == "INVITE_CREATE") {
			return 35;
		} else if (this->theEvent == "INVITE_DELETE") {
			return 36;
		} else if (this->theEvent == "MESSAGE_CREATE") {
			return 37;
		} else if (this->theEvent == "MESSAGE_UPDATE") {
			return 38;
		} else if (this->theEvent == "MESSAGE_DELETE") {
			return 39;
		} else if (this->theEvent == "MESSAGE_DELETE_BULK") {
			return 40;
		} else if (this->theEvent == "MESSAGE_REACTION_ADD") {
			return 41;
		} else if (this->theEvent == "MESSAGE_REACTION_REMOVE") {
			return 42;
		} else if (this->theEvent == "MESSAGE_REACTION_REMOVE_ALL") {
			return 43;
		} else if (this->theEvent == "MESSAGE_REACTION_REMOVE_EMOJI") {
			return 44;
		} else if (this->theEvent == "PRESENCE_UPDATE") {
			return 45;
		} else if (this->theEvent == "STAGE_INSTANCE_CREATE") {
			return 46;
		} else if (this->theEvent == "STAGE_INSTANCE_UPDATE") {
			return 47;
		} else if (this->theEvent == "STAGE_INSTANCE_DELETE") {
			return 48;
		} else if (this->theEvent == "TYPING_START") {
			return 49;
		} else if (this->theEvent == "USER_UPDATE") {
			return 50;
		} else if (this->theEvent == "VOICE_STATE_UPDATE") {
			return 51;
		} else if (this->theEvent == "VOICE_SERVER_UPDATE") {
			return 52;
		} else if (this->theEvent == "WEBHOOKS_UPDATE") {
			return 53;
		} else {
			return 0;
		}
	}

	WebSocketMessageHandler::WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManagerNew) {
		this->configManager = configManagerNew;
	}

	void WebSocketMessageHandler::stringifyJsonData(nlohmann::json& dataToSend, std::string& theString, WebSocketOpCode theOpCode) noexcept {
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

	bool WebSocketMessageHandler::parseConnectionHeaders(WebSocketSSLShard* theShard) noexcept {
		if (theShard->theSSLState.load() == SSLConnectionState::Connected && theShard->theWebSocketState.load() == WebSocketSSLShardState::Upgrading) {
			std::string newVector = theShard->getInputBuffer();
			auto theFindValue = newVector.find("\r\n\r\n");
			if (theFindValue != std::string::npos) {
				theShard->inputBuffer.clear();
				theShard->theWebSocketState.store(WebSocketSSLShardState::Collecting_Hello);
				return true;
			}
		}
		return false;
	}

	bool WebSocketMessageHandler::parseMessage(SSLClient* theShard) noexcept {
		if (static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.size() < 4) {
			return true;
		}
		static_cast<WebSocketSSLShard*>(theShard)->dataOpCode = static_cast<WebSocketOpCode>(static_cast<WebSocketSSLShard*>(theShard)->inputBuffer[0] & ~webSocketFinishBit);
		switch (static_cast<WebSocketSSLShard*>(theShard)->dataOpCode) {
			case WebSocketOpCode::Op_Continuation:
				[[fallthrough]];
			case WebSocketOpCode::Op_Text:
				[[fallthrough]];
			case WebSocketOpCode::Op_Binary:
				[[fallthrough]];
			case WebSocketOpCode::Op_Ping:
				[[fallthrough]];
			case WebSocketOpCode::Op_Pong: {
				uint8_t length01 = static_cast<WebSocketSSLShard*>(theShard)->inputBuffer[1];
				static_cast<WebSocketSSLShard*>(theShard)->messageOffset = 2;
				if (length01 & webSocketMaskBit) {
					return true;
				}
				static_cast<WebSocketSSLShard*>(theShard)->messageLength = length01;
				if (length01 == webSocketPayloadLengthMagicLarge) {
					if (static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.size() < 8) {
						return true;
					}
					uint8_t length03 = static_cast<WebSocketSSLShard*>(theShard)->inputBuffer[2];
					uint8_t length04 = static_cast<WebSocketSSLShard*>(theShard)->inputBuffer[3];
					static_cast<WebSocketSSLShard*>(theShard)->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
					static_cast<WebSocketSSLShard*>(theShard)->messageOffset += 2;
				} else if (length01 == webSocketPayloadLengthMagicHuge) {
					if (static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.size() < 10) {
						return true;
					}
					static_cast<WebSocketSSLShard*>(theShard)->messageLength = 0;
					for (uint64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
						uint8_t lengthNew = static_cast<uint8_t>(static_cast<WebSocketSSLShard*>(theShard)->inputBuffer[x]);
						static_cast<WebSocketSSLShard*>(theShard)->messageLength |=
							static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
					}
					static_cast<WebSocketSSLShard*>(theShard)->messageOffset += 8;
				}
				if (static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.size() > 0 &&
					static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.size() < static_cast<uint64_t>(static_cast<WebSocketSSLShard*>(theShard)->messageOffset) +
							static_cast<uint64_t>(static_cast<WebSocketSSLShard*>(theShard)->messageLength)) {
					return true;
				} else {
					auto theValue = this->onMessageReceived(static_cast<WebSocketSSLShard*>(theShard)->messageOffset, static_cast<WebSocketSSLShard*>(theShard)->messageLength);
					static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.erase(static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.begin(),
						static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.begin() + static_cast<WebSocketSSLShard*>(theShard)->messageOffset +
							static_cast<WebSocketSSLShard*>(theShard)->messageLength);
					return false;
				}
			}
			case WebSocketOpCode::Op_Close: {
				uint16_t close = static_cast<WebSocketSSLShard*>(theShard)->inputBuffer[2] & 0xff;
				close <<= 8;
				close |= static_cast<WebSocketSSLShard*>(theShard)->inputBuffer[3] & 0xff;
				static_cast<WebSocketSSLShard*>(theShard)->closeCode = close;
				if (static_cast<WebSocketSSLShard*>(theShard)->closeCode) {
					static_cast<WebSocketSSLShard*>(theShard)->areWeResuming = true;
				}
				static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.erase(static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.begin(),
					static_cast<WebSocketSSLShard*>(theShard)->inputBuffer.begin() + 4);
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "WebSocket " + static_cast<WebSocketSSLShard*>(theShard)->shard.dump() + " Closed; Code: "
						 << +static_cast<uint16_t>(static_cast<WebSocketSSLShard*>(theShard)->closeCode) << DiscordCoreAPI::reset() << endl
						 << endl;
				}
				this->onClosed();
				return false;
			}
		}
		return true;
	}

	WebSocketSSLShard::WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* theClient, int32_t currentShardNew,
		std::atomic_bool* doWeQuitNew) noexcept
		: WebSocketMessageHandler(&theClient->configManager) {
		this->configManager = &theClient->configManager;
		this->thePackage.currentShard = currentShardNew;
		this->discordCoreClient = theClient;
		this->shard.emplace_back(currentShardNew);
		this->doWeQuit = doWeQuitNew;
		if (theClient) {
			this->discordCoreClient->theBaseShardCount.store(this->discordCoreClient->theBaseShardCount.load() + 1);
			this->shard.emplace_back(this->discordCoreClient->configManager.getTotalShardCount());
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
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending WebSocket " + this->shard.dump() + std::string("'s Message: ") << dataToSend << endl
						 << DiscordCoreAPI::reset();
				}
				ProcessIOResult didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				do {
					if (theStopWatch.hasTimePassed()) {
						this->onClosed();
						return false;
					}
					didWeWrite = this->writeData(dataToSend, true);
				} while (didWeWrite != ProcessIOResult::No_Error);
				if (didWeWrite != ProcessIOResult::No_Error) {
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
	
	std::atomic_uint64_t theInt{};
	bool WebSocketSSLShard::onMessageReceived(int64_t offSet, int64_t length) noexcept {
		if (this->theSSLState.load() == SSLConnectionState::Connected) {
			try {
				bool returnValue{ true };
				nlohmann::json payload{};
				if (WebSocketSSLShard::inputBuffer.size() > 0) {
					returnValue = true;

					if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
						try {
							DiscordCoreAPI::StopWatch theStopWatch{ 50us };
							theStopWatch.resetTimer();
							payload = this->parseEtfToJson(WebSocketSSLShard::inputBuffer.substr(offSet, length));
							theInt.store(theInt.load() + theStopWatch.totalTimePassed());
							if (DiscordCoreAPI::theCount.load() != 0) {
								std::cout << "THE TIME TO COMPLETE (AVERAGE): " << theInt.load() / DiscordCoreAPI::theCount.load() << std::endl;
							}
						} catch (...) {
							if (this->configManager->doWePrintGeneralErrorMessages()) {
								DiscordCoreAPI::reportException("ErlPacker::parseEtfToJson()");
							}
							returnValue = true;
						}
					} else {
						DiscordCoreAPI::StopWatch theStopWatch{ 50us };
						theStopWatch.resetTimer();
						payload = nlohmann::json::parse(WebSocketSSLShard::inputBuffer.substr(offSet, length));
						std::cout << "THE TIME TO COMPLETE: " << theStopWatch.totalTimePassed() << std::endl;
					}
				} else {
					returnValue = true;
				}

				if (payload.contains("t") && !payload["t"].is_null()) {
					if (payload["t"] == "RESUMED") {
						this->theWebSocketState.store(WebSocketSSLShardState::Authenticated);
						this->currentReconnectTries = 0;
					}

					if (payload["t"] == "READY") {
						this->theWebSocketState.store(WebSocketSSLShardState::Authenticated);
						this->sessionId = payload["d"]["session_id"].get<std::string>();
						DiscordCoreAPI::UserData theUser{ payload["d"]["user"] };
						this->discordCoreClient->currentUser =
							DiscordCoreAPI::BotUser{ theUser, this->discordCoreClient->baseSocketAgentMap[std::to_string(this->shard[0].get<int32_t>())].get() };
						DiscordCoreAPI::Users::insertUser(std::make_unique<DiscordCoreAPI::UserData>(theUser));
						this->currentReconnectTries = 0;
					}
				}

				if (payload.contains("s") && !payload["s"].is_null()) {
					this->lastNumberReceived = payload["s"].get<int32_t>();
				}
				
				if (this->configManager->doWePrintWebSocketSuccessMessages() && !payload.is_null()) {
					cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from WebSocket " + this->shard.dump() + std::string(": ") << payload.dump()
						 << DiscordCoreAPI::reset() << endl
						 << endl;
				}

				if (payload.contains("op") && !payload["op"].is_null()) {
					switch (payload["op"].get<int32_t>()) {
						case 0: {
							if (payload.contains("d") && !payload["d"].is_null() && payload.contains("t") && !payload["t"].is_null()) {
								switch (EventConverter{ payload["t"].get<std::string>() }) {
									case 1: {
										std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>(payload["d"])
										};
										dataPackage->permissionData = payload["d"];
										this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
										break;
									}
									case 2: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>()
										};
										dataPackage->theRule = payload["d"];
										this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
										break;
									}
									case 3: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>()
										};
										dataPackage->theRule = payload["d"];
										this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
										break;
									}
									case 4: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>()
										};
										dataPackage->theRule = payload["d"];
										this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
									}
									case 5: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>()
										};
										dataPackage->theData = payload["d"];
										this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
										break;
									}
									case 6: {
										if (this->discordCoreClient->configManager.doWeCacheChannels()) {
											DiscordCoreAPI::Channels::insertChannel(std::make_unique<DiscordCoreAPI::ChannelData>(payload["d"]));
											std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelCreationData>(
												DiscordCoreAPI::Channels::cache[stoull(payload["d"]["id"].get<std::string>())].get()) };
											dataPackage->channel = DiscordCoreAPI::Channels::cache[stoull(payload["d"]["id"].get<std::string>())].get();
											auto guildId = dataPackage->channel->guildId;
											auto channelId = dataPackage->channel->id;
											DiscordCoreAPI::GuildData* guild = DiscordCoreAPI::Guilds::cache[guildId].get();
											guild->channels.emplace_back(channelId);
											this->discordCoreClient->eventManager.onChannelCreationEvent(*dataPackage);
										}
										break;
									}
									case 7: {
										if (this->discordCoreClient->configManager.doWeCacheChannels()) {
											auto oldChannel = DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(payload["d"]["id"].get<std::string>()) }).get();
											DiscordCoreAPI::Channels::insertChannel(std::make_unique<DiscordCoreAPI::ChannelData>(payload["d"]));
											std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelUpdateData>(
												DiscordCoreAPI::Channels::cache[stoull(payload["d"]["id"].get<std::string>())].get()) };
											this->discordCoreClient->eventManager.onChannelUpdateEvent(*dataPackage);
										}
										break;
									}
									case 8: {
										if (this->discordCoreClient->configManager.doWeCacheChannels()) {
											std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelDeletionData>(
												std::make_unique<DiscordCoreAPI::ChannelData>(
													DiscordCoreAPI::Channels::getCachedChannelAsync({ .channelId = stoull(payload["d"]["id"].get<std::string>()) }).get())) };
											DiscordCoreAPI::Channels::removeChannel(dataPackage->channel->id);
											DiscordCoreAPI::GuildData* guild = DiscordCoreAPI::Guilds::cache[dataPackage->channel->guildId].get();
											for (uint64_t x = 0; x < guild->channels.size(); ++x) {
												if (guild->channels[x] == dataPackage->channel->id) {
													guild->channels.erase(guild->channels.begin() + x);
												}
											}
											this->discordCoreClient->eventManager.onChannelDeletionEvent(*dataPackage);
										}
										break;
									}
									case 9: {
										std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
										dataPackage->dataPackage = payload["d"];
										this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
										break;
									}
									case 10: {
										std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
										dataPackage->thread = payload["d"];
										this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
										break;
									}
									case 11: {
										std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
										dataPackage->thread = payload["d"];
										this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
										break;
									}
									case 12: {
										std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
										dataPackage->thread = payload["d"];
										this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
										break;
									}
									case 13: {
										std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
										dataPackage->threadListSyncData = payload["d"];
										this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
										break;
									}
									case 14: {
										std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
										dataPackage->threadMember = payload["d"];
										this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
										break;
									}
									case 15: {
										std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
										dataPackage->threadMembersUpdateData = payload["d"];
										this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
										break;
									}
									case 16: {
										if (this->discordCoreClient->configManager.doWeCacheGuilds()) {
											auto theGuild = std::make_unique<DiscordCoreAPI::GuildData>(payload["d"]);
											theGuild->discordCoreClient = this->discordCoreClient;
											DiscordCoreAPI::Guilds::insertGuild(std::move(theGuild));
											std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildCreationData>(
												DiscordCoreAPI::Guilds::cache[stoull(payload["d"]["id"].get<std::string>())].get(), this->discordCoreClient) };
											this->discordCoreClient->eventManager.onGuildCreationEvent(*dataPackage);
										}
										break;
									}
									case 17: {
										if (this->discordCoreClient->configManager.doWeCacheGuilds()) {
											auto oldGuild = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(payload["d"]["id"].get<std::string>()) }).get();
											auto theGuild = std::make_unique<DiscordCoreAPI::GuildData>(payload["d"]);
											theGuild->discordCoreClient = this->discordCoreClient;
											DiscordCoreAPI::Guilds::insertGuild(std::move(theGuild));
											std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildUpdateData>(
												DiscordCoreAPI::Guilds::cache[stoull(payload["d"]["id"].get<std::string>())].get(), this->discordCoreClient) };
											this->discordCoreClient->eventManager.onGuildUpdateEvent(*dataPackage);
										}
										break;
									}
									case 18: {
										if (this->discordCoreClient->configManager.doWeCacheGuilds()) {
											auto oldGuild = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = stoull(payload["d"]["id"].get<std::string>()) }).get();
											std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildDeletionData>(
												std::make_unique<DiscordCoreAPI::GuildData>(oldGuild)) };
											DiscordCoreAPI::GuildMemberId theKey{};
											theKey.guildId = dataPackage->guild->id;
											for (auto& value: dataPackage->guild->members) {
												theKey.guildMemberId = value;
												DiscordCoreAPI::GuildMemberData* guildMember = DiscordCoreAPI::GuildMembers::cache[theKey].get();
												DiscordCoreAPI::GuildMembers::removeGuildMember(std::make_unique<DiscordCoreAPI::GuildMemberData>(*guildMember));
											}
											for (auto& value: dataPackage->guild->channels) {
												DiscordCoreAPI::Channels::removeChannel(value);
											}
											for (auto& value: dataPackage->guild->roles) {
												DiscordCoreAPI::Roles::removeRole(value);
											}
											DiscordCoreAPI::Guilds::removeGuild(dataPackage->guild->id);
											this->discordCoreClient->eventManager.onGuildDeletionEvent(*dataPackage);
										}
										break;
									}
									case 19: {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
										if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
											dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
										}
										if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
											dataPackage->user = payload["d"]["user"];
										}
										this->discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
										break;
									}
									case 20: {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>() };
										if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
											dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
										}
										if (payload["d"].contains("user") && !payload["d"]["user"].is_null()) {
											dataPackage->user = payload["d"]["user"];
										}
										this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
										break;
									}
									case 21: {
										std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
										dataPackage->updateData = payload["d"];
										this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
										break;
									}
									case 22: {
										std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
										dataPackage->updateData = payload["d"];
										this->discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
										break;
									}
									case 23: {
										std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>()
										};
										if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
											dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
										}
										this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
										break;
									}
									case 24: {
										if (this->discordCoreClient->configManager.doWeCacheGuildMembers()) {
											DiscordCoreAPI::GuildMemberId theKey{};
											theKey.guildMemberId = stoull(payload["d"]["user"]["id"].get<std::string>());
											theKey.guildId = stoull(payload["d"]["guild_id"].get<std::string>());
											DiscordCoreAPI::GuildMembers::insertGuildMember(std::make_unique<DiscordCoreAPI::GuildMemberData>(payload["d"]));
											std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberAddData>(
												DiscordCoreAPI::GuildMembers::cache[theKey].get(), this->discordCoreClient) };
											dataPackage->guildMember = DiscordCoreAPI::GuildMembers::cache[theKey].get();
											auto guildId = dataPackage->guildMember->guildId;
											auto guildMemberId = dataPackage->guildMember->id;
											DiscordCoreAPI::GuildData* guild = DiscordCoreAPI::Guilds::cache[guildId].get();
											guild->members.emplace_back(guildMemberId);
											this->discordCoreClient->eventManager.onGuildMemberAddEvent(*dataPackage);
										}
									}
									case 25: {
										if (this->discordCoreClient->configManager.doWeCacheGuildMembers()) {
											auto oldGuildMember =
												DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = stoull(payload["d"]["user"]["id"].get<std::string>()),
																											.guildId = stoull(payload["d"]["guild_id"].get<std::string>()) })
													.get();
											DiscordCoreAPI::GuildMemberId theKey{};
											theKey.guildMemberId = oldGuildMember.id;
											theKey.guildId = oldGuildMember.guildId;
											DiscordCoreAPI::GuildMembers::insertGuildMember(std::make_unique<DiscordCoreAPI::GuildMemberData>(payload["d"]));
											std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>(
												DiscordCoreAPI::GuildMembers::cache[theKey].get()) };
											this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(*dataPackage);
										}
										break;
									}
									case 26: {
										if (this->discordCoreClient->configManager.doWeCacheGuildMembers()) {
											std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>(
												std::make_unique<DiscordCoreAPI::UserData>(payload["d"]["user"]), this->discordCoreClient,
												stoull(payload["d"]["guild_id"].get<std::string>())) };
											DiscordCoreAPI::GuildMemberId theKey{};
											theKey.guildMemberId = dataPackage->user->id;
											theKey.guildId = dataPackage->guildId;
											DiscordCoreAPI::GuildMemberData* guildMember = DiscordCoreAPI::GuildMembers::cache[theKey].get();
											DiscordCoreAPI::GuildMembers::removeGuildMember(std::make_unique<DiscordCoreAPI::GuildMemberData>(*guildMember));
											DiscordCoreAPI::GuildData* guild = DiscordCoreAPI::Guilds::cache[dataPackage->guildId].get();
											for (uint64_t x = 0; x < guild->members.size(); ++x) {
												if (guild->members[x] == dataPackage->user->id) {
													guild->members.erase(guild->members.begin() + x);
												}
											}
											guild->memberCount--;
											this->discordCoreClient->eventManager.onGuildMemberRemoveEvent(*dataPackage);
										}
										break;
									}
									case 27: {
										std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
										dataPackage->chunkEventData = payload["d"];
										this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
										break;
									}
									case 28: {
										if (this->discordCoreClient->configManager.doWeCacheRoles()) {
											DiscordCoreAPI::Roles::insertRole(std::make_unique<DiscordCoreAPI::RoleData>(payload["d"]["role"]));
											std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleCreationData>(
												DiscordCoreAPI::Roles::cache[stoull(payload["d"]["role"]["id"].get<std::string>())].get(),
												stoull(payload["d"]["guild_id"].get<std::string>())) };
											dataPackage->role = DiscordCoreAPI::Roles::cache[stoull(payload["d"]["role"]["id"].get<std::string>())].get();
											auto guildId = dataPackage->guildId;
											auto roleId = dataPackage->role->id;
											DiscordCoreAPI::GuildData* guild = DiscordCoreAPI::Guilds::cache[guildId].get();
											guild->roles.emplace_back(roleId);
											this->discordCoreClient->eventManager.onRoleCreationEvent(*dataPackage);
										}
										break;
									}
									case 29: {
										if (this->discordCoreClient->configManager.doWeCacheRoles()) {
											auto oldRole = DiscordCoreAPI::Roles::getCachedRoleAsync({ .roleId = stoull(payload["d"]["role"]["id"].get<std::string>()) }).get();
											DiscordCoreAPI::Roles::insertRole(std::make_unique<DiscordCoreAPI::RoleData>(payload["d"]["role"]));
											std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleUpdateData>(
												DiscordCoreAPI::Roles::cache[stoull(payload["d"]["role"]["id"].get<std::string>())].get(),
												stoull(payload["d"]["guild_id"].get<std::string>())) };
											this->discordCoreClient->eventManager.onRoleUpdateEvent(*dataPackage);
										}
									}
									case 30: {
										if (this->discordCoreClient->configManager.doWeCacheRoles()) {
											std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleDeletionData>(
												std::make_unique<DiscordCoreAPI::RoleData>(
													DiscordCoreAPI::Roles::getCachedRoleAsync({ .roleId = stoull(payload["d"]["role_id"].get<std::string>()) }).get()),
												stoull(payload["d"]["guild_id"].get<std::string>())) };
											DiscordCoreAPI::Roles::removeRole(dataPackage->role->id);
											DiscordCoreAPI::GuildData* guild = DiscordCoreAPI::Guilds::cache[dataPackage->guildId].get();
											for (uint64_t x = 0; x < guild->roles.size(); ++x) {
												if (guild->roles[x] == dataPackage->role->id) {
													guild->roles.erase(guild->roles.begin() + x);
												}
											}
											this->discordCoreClient->eventManager.onRoleDeletionEvent(*dataPackage);
										}
										break;
									}
									case 31: {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
										if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
											dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
										}
										if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
											dataPackage->integrationData = payload["d"]["integration"];
										}
										this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
										break;
									}
									case 32: {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
										if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
											dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
										}
										if (payload["d"].contains("integration") && !payload["d"]["integration"].is_null()) {
											dataPackage->integrationData = payload["d"]["integration"];
										}
										this->discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
										break;
									}
									case 33: {
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
										break;
									}
									case 34: {
										std::unique_ptr<DiscordCoreAPI::InteractionData> interactionData{ std::make_unique<DiscordCoreAPI::InteractionData>() };
										*interactionData = payload["d"];
										std::unique_ptr<DiscordCoreAPI::InputEventData> eventData{ std::make_unique<DiscordCoreAPI::InputEventData>(*interactionData) };
										switch (interactionData->type) {
											case DiscordCoreAPI::InteractionType::Application_Command: {
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
											}
											case DiscordCoreAPI::InteractionType::Message_Component: {
												switch (interactionData->data.componentData.componentType) {
													case DiscordCoreAPI::ComponentType::Button: {
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
													}
													case DiscordCoreAPI::ComponentType::SelectMenu: {
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
												}
											}
											case DiscordCoreAPI::InteractionType::Modal_Submit: {
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
											}

											case DiscordCoreAPI::InteractionType::Application_Command_Autocomplete: {
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
										}
									}
									case 35: {
										std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
										dataPackage->invite = payload["d"];
										this->discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
										break;
									}
									case 36: {
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
										break;
									}
									case 37: {
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
									}
									case 38: {
										std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
										dataPackage->messageNew = payload["d"];
										for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
											value->send(dataPackage->messageNew);
										}
										this->discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
										break;
									}
									case 39: {
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
										break;
									}
									case 40: {
										std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
										if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
											dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
										}
										if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
											dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
										}
										if (payload["d"].contains("ids") && !payload["d"]["ids"].is_null()) {
											for (auto& value: payload["d"]["ids"]) {
												dataPackage->ids.emplace_back(stoull(value.get<std::string>()));
											}
										}
										this->discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
										break;
									}
									case 41: {
										std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
										dataPackage->reaction = payload["d"];
										this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
										break;
									}
									case 42: {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
										dataPackage->reactionRemoveData = payload["d"];
										this->discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
										break;
									}
									case 43: {
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
										break;
									}
									case 44: {
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
										break;
									}
									case 45: {
										std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
										dataPackage->presenceData = payload["d"];
										this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
										break;
									}
									case 46: {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
										dataPackage->stageInstance = payload["d"];
										this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
										break;
									}
									case 47: {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
										dataPackage->stageInstance = payload["d"];
										this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
										break;
									}
									case 48: {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
										dataPackage->stageInstance = payload["d"];
										this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
										break;
									}
									case 49: {
										std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
										dataPackage->typingStartData = payload["d"];
										this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
										break;
									}
									case 50: {
										if (this->discordCoreClient->configManager.doWeCacheUsers()) {
											auto oldUser = DiscordCoreAPI::Users::getCachedUserAsync({ .userId = stoull(payload["d"]["id"].get<std::string>()) }).get();
											DiscordCoreAPI::Users::insertUser(std::make_unique<DiscordCoreAPI::UserData>(payload["d"]));
											std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>(
												DiscordCoreAPI::Users::cache[stoull(payload["d"]["id"].get<std::string>())].get()) };
											this->discordCoreClient->eventManager.onUserUpdateEvent(*dataPackage);
										}
										break;
									}
									case 51: {
										if (this->areWeCollectingData && !this->stateUpdateCollected && !this->serverUpdateCollected &&
											payload["d"]["member"]["user"]["id"] == std::to_string(this->userId)) {
											this->voiceConnectionData = VoiceConnectionData{};
											this->voiceConnectionData.sessionId = payload["d"]["session_id"].get<std::string>();
											this->stateUpdateCollected = true;
										} else if (this->areWeCollectingData && !this->stateUpdateCollected) {
											this->voiceConnectionData.sessionId = payload["d"]["session_id"].get<std::string>();
											if (this->voiceConnectionDataBufferMap.contains(stoull(payload["d"]["guild_id"].get<std::string>()))) {
												this->voiceConnectionDataBufferMap[stoull(payload["d"]["guild_id"].get<std::string>())]->send(this->voiceConnectionData);
											}
											this->serverUpdateCollected = false;
											this->stateUpdateCollected = false;
											this->areWeCollectingData = false;
										}
										std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
										dataPackage->voiceStateData = payload["d"];
										if (this->discordCoreClient->configManager.doWeCacheGuildMembers() && this->discordCoreClient->configManager.doWeCacheGuilds()) {
											DiscordCoreAPI::GuildData guild = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = dataPackage->voiceStateData.guildId }).get();
											guild.voiceStates[dataPackage->voiceStateData.userId] = dataPackage->voiceStateData;
											DiscordCoreAPI::Guilds::insertGuild(std::make_unique<DiscordCoreAPI::GuildData>(guild));
										}
										this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
										break;
									}
									case 52: {
										if (this->areWeCollectingData && !this->serverUpdateCollected && !this->stateUpdateCollected) {
											this->voiceConnectionData = VoiceConnectionData{};
											this->voiceConnectionData.endPoint = payload["d"]["endpoint"].get<std::string>();
											this->voiceConnectionData.token = payload["d"]["token"].get<std::string>();
											this->serverUpdateCollected = true;
										} else if (this->areWeCollectingData && !this->serverUpdateCollected) {
											this->voiceConnectionData.endPoint = payload["d"]["endpoint"].get<std::string>();
											this->voiceConnectionData.token = payload["d"]["token"].get<std::string>();
											if (this->voiceConnectionDataBufferMap.contains(stoull(payload["d"]["guild_id"].get<std::string>()))) {
												this->voiceConnectionDataBufferMap[stoull(payload["d"]["guild_id"].get<std::string>())]->send(this->voiceConnectionData);
											}
											this->serverUpdateCollected = false;
											this->stateUpdateCollected = false;
											this->areWeCollectingData = false;
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
										break;
									}
									case 53: {
										std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
										if (payload["d"].contains("channel_id") && !payload["d"]["channel_id"].is_null()) {
											dataPackage->channelId = stoull(payload["d"]["channel_id"].get<std::string>());
										}
										if (payload["d"].contains("guild_id") && !payload["d"]["guild_id"].is_null()) {
											dataPackage->guildId = stoull(payload["d"]["guild_id"].get<std::string>());
										}
										this->discordCoreClient->eventManager.onWebhookUpdateEvent(*dataPackage);
										break;
									}

									break;
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
								cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->shard.dump() + " Reconnecting (Type 7)!" << DiscordCoreAPI::reset() << endl << endl;
							}
							this->areWeResuming = true;
							this->onClosed();
							returnValue = true;
						}
						case 9: {
							if (this->configManager->doWePrintWebSocketErrorMessages()) {
								cout << DiscordCoreAPI::shiftToBrightBlue() << "Shard " + this->shard.dump() + " Reconnecting (Type 9)!" << DiscordCoreAPI::reset() << endl << endl;
							}
							std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
							int32_t numOfMsToWait =
								static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
							std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
							if (payload["d"] == true) {
								this->areWeResuming = true;
							} else {
								this->areWeResuming = false;
							}
							this->onClosed();
							returnValue = true;
						}
						case 10: {
							if (payload["d"].contains("heartbeat_interval") && !payload["d"]["heartbeat_interval"].is_null()) {
								this->areWeHeartBeating = true;
								this->heartBeatStopWatch = DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ std::chrono::milliseconds{ payload["d"]["heartbeat_interval"] } };
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
									returnValue = true;
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
									returnValue = true;
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
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
				}
				this->onClosed();
				return true;
			}
		}
		return true;
	}
	
	void WebSocketSSLShard::checkForAndSendHeartBeat(bool isImmediate) noexcept {
		if (this->theWebSocketState.load() == WebSocketSSLShardState::Authenticated) {
			try {
				if ((this->heartBeatStopWatch.hasTimePassed() && this->haveWeReceivedHeartbeatAck) || isImmediate) {
					nlohmann::json heartbeat{};
					heartbeat["d"] = this->lastNumberReceived;
					heartbeat["op"] = 1;
					std::string theString{};
					if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
						this->stringifyJsonData(heartbeat, theString, WebSocketOpCode::Op_Binary);
					} else {
						this->stringifyJsonData(heartbeat, theString, WebSocketOpCode::Op_Text);
					}
					if (!this->sendMessage(theString, true)) {
						return;
					}
					this->haveWeReceivedHeartbeatAck = false;
					this->heartBeatStopWatch.resetTimer();
				}
			} catch (...) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::checkForAndSendHeartBeat()");
				}
				this->onClosed();
			}
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
			this->wantWrite = true;
			this->wantRead = false;
			this->closeCode = 0;
			this->areWeHeartBeating = false;
			if (doWeReconnect) {
				this->thePackage.voiceConnectionDataBufferMap = std::move(this->voiceConnectionDataBufferMap);
				this->thePackage.currentReconnectTries = this->currentReconnectTries;
				this->thePackage.currentShard = this->shard[0];
			}
		}
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

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentShardNew) noexcept
		: WebSocketSSLShard(discordCoreClientNew, currentShardNew, doWeQuitNew) {
		this->configManager = &discordCoreClientNew->configManager;
		this->discordCoreClient = discordCoreClientNew;
		this->doWeQuit = doWeQuitNew;
		this->taskThread = std::make_unique<std::jthread>([this](std::stop_token stopToken) {
			this->run(stopToken);
		});
	}

	void BaseSocketAgent::connectVoiceChannel(VoiceConnectInitData theData) noexcept {
		std::unique_lock theLock{ this->theConnectDisconnectMutex };
		this->voiceConnections.push(theData);
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage thePackageNew) noexcept {
		while (!this->discordCoreClient->theStopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
		}
		this->discordCoreClient->theStopWatch.resetTimer();
		std::unique_lock theLock{ this->theConnectDisconnectMutex };
		this->doWeReconnect.store(true);
		this->thePackage = thePackageNew;
		DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
		theStopWatch.resetTimer();
		while (this->theWebSocketState.load() != WebSocketSSLShardState::Authenticated) {
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

	void BaseSocketAgent::run(std::stop_token stopToken) noexcept {
		try {
			this->doWeReconnect.store(true);
			while (!stopToken.stop_requested() && !this->doWeQuit->load()) {
				{ std::unique_lock theLock{ this->theConnectDisconnectMutex };
					if (this->voiceConnectionsToDisconnect.size() > 0) {
						this->disconnectVoice();
					}
					if (this->voiceConnections.size() > 0) {
						this->connectVoiceInternal();
					}
				}
				if (this->doWeReconnect.load()) {
					this->connectInternal();
				}
				this->processIO(1000);
				if (this->areWeStillConnected() && this->inputBuffer.size() > 0) {
					this->parseMessage(this);
				}
				if (this->areWeStillConnected()) {
					this->checkForAndSendHeartBeat();
				}
				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			}
		}
	}

	void BaseSocketAgent::disconnectVoice() noexcept {
		std::unique_lock theLock{ this->theConnectDisconnectMutex };
		uint64_t theDCData = this->voiceConnectionsToDisconnect.front();
		this->voiceConnectionsToDisconnect.pop();
		DiscordCoreAPI::getVoiceConnectionMap()[theDCData]->disconnectInternal();
	}

	void BaseSocketAgent::connectInternal() noexcept {
		try {
			if (this->doWeReconnect.load() && this->thePackage.currentShard != -1) {
				this->currentReconnectTries = thePackage.currentReconnectTries;
				this->currentReconnectTries++;
				this->voiceConnectionDataBufferMap = std::move(thePackage.voiceConnectionDataBufferMap);

				if (this->currentReconnectTries >= this->maxReconnectTries) {
					this->doWeQuit->store(true);
					return;
				}

				if (!WebSocketSSLShard::connect(this->configManager->getConnectionAddress(), this->configManager->getConnectionPort())) {
					this->onClosed();
					return;
				}

				this->theWebSocketState.store(WebSocketSSLShardState::Upgrading);
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
						this->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
					didWeWrite = this->writeData(sendString, true);
				} while (didWeWrite != ProcessIOResult::No_Error);
				if (didWeWrite != ProcessIOResult::No_Error) {
					this->onClosed();
					return;
				}

				while (!this->doWeQuit->load()) {
					if (this->theWebSocketState.load() == WebSocketSSLShardState::Collecting_Hello) {
						break;
					}
					this->processIO(1000000);
					if (this->areWeStillConnected()) {
						this->parseConnectionHeaders(this);
					}
					if (this->areWeStillConnected()) {
						this->parseMessage(this);
					}
					std::this_thread::sleep_for(1ms);
				}
				this->thePackage.currentShard = -1;
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::connectInternal()");
			}
		}
	}

	BaseSocketAgent::~BaseSocketAgent() noexcept {
	}
}