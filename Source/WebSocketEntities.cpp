/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// WebSocketEntities.cpp - Source file for the webSocket related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file WebSocketEntities.cpp

#include <discordcoreapi/WebSocketEntities.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <random>

namespace DiscordCoreAPI {
	namespace Globals {
		extern VoiceConnectionMap voiceConnectionMap;
	}
}

namespace DiscordCoreInternal {

	constexpr Uint16 webSocketMaxPayloadLengthLarge{ 65535u };
	constexpr Uint8 webSocketPayloadLengthMagicLarge{ 126u };
	constexpr Uint8 webSocketPayloadLengthMagicHuge{ 127u };
	constexpr Uint8 maxHeaderSize{ sizeof(Uint64) + 2u };
	constexpr Uint8 webSocketMaxPayloadLengthSmall{ 125u };
	constexpr Uint8 webSocketFinishBit{ (1u << 7u) };
	constexpr Uint8 webSocketMaskBit{ (1u << 7u) };

	EventConverter::EventConverter(String theNewEvent) {
		this->theEvent = theNewEvent;
	}

	EventConverter::operator Int32() {
		if (this->theEvent == "READY") {
			return 1;
		} else if (this->theEvent == "RESUMED") {
			return 2;
		} else if (this->theEvent == "APPLICATION_COMMAND_PERMISSIONS_UPDATE") {
			return 3;
		} else if (this->theEvent == "AUTO_MODERATION_RULE_CREATE") {
			return 4;
		} else if (this->theEvent == "AUTO_MODERATION_RULE_UPDATE") {
			return 5;
		} else if (this->theEvent == "AUTO_MODERATION_RULE_DELETE") {
			return 6;
		} else if (this->theEvent == "AUTO_MODERATION_ACTION_EXECUTION") {
			return 7;
		} else if (this->theEvent == "CHANNEL_CREATE") {
			return 8;
		} else if (this->theEvent == "CHANNEL_UPDATE") {
			return 9;
		} else if (this->theEvent == "CHANNEL_DELETE") {
			return 10;
		} else if (this->theEvent == "CHANNEL_PINS_UPDATE") {
			return 11;
		} else if (this->theEvent == "THREAD_CREATE") {
			return 12;
		} else if (this->theEvent == "THREAD_UPDATE") {
			return 13;
		} else if (this->theEvent == "THREAD_DELETE") {
			return 14;
		} else if (this->theEvent == "THREAD_LIST_SYNC") {
			return 15;
		} else if (this->theEvent == "THREAD_MEMBER_UPDATE") {
			return 16;
		} else if (this->theEvent == "THREAD_MEMBERS_UPDATE") {
			return 17;
		} else if (this->theEvent == "GUILD_CREATE") {
			return 18;
		} else if (this->theEvent == "GUILD_UPDATE") {
			return 19;
		} else if (this->theEvent == "GUILD_DELETE") {
			return 20;
		} else if (this->theEvent == "GUILD_BAN_ADD") {
			return 21;
		} else if (this->theEvent == "GUILD_BAN_REMOVE") {
			return 22;
		} else if (this->theEvent == "GUILD_EMOJIS_UPDATE") {
			return 23;
		} else if (this->theEvent == "GUILD_STICKERS_UPDATE") {
			return 24;
		} else if (this->theEvent == "GUILD_INTEGRATIONS_UPDATE") {
			return 25;
		} else if (this->theEvent == "GUILD_MEMBER_ADD") {
			return 26;
		} else if (this->theEvent == "GUILD_MEMBER_REMOVE") {
			return 27;
		} else if (this->theEvent == "GUILD_MEMBER_UPDATE") {
			return 28;
		} else if (this->theEvent == "GUILD_MEMBERS_CHUNK") {
			return 29;
		} else if (this->theEvent == "GUILD_ROLE_CREATE") {
			return 30;
		} else if (this->theEvent == "GUILD_ROLE_UPDATE") {
			return 31;
		} else if (this->theEvent == "GUILD_ROLE_DELETE") {
			return 32;
		} else if (this->theEvent == "GUILD_SCHEDULED_EVENT_CREATE") {
			return 33;
		} else if (this->theEvent == "GUILD_SCHEDULED_EVENT_UPDATE") {
			return 34;
		} else if (this->theEvent == "GUILD_SCHEDULED_EVENT_DELETE") {
			return 35;
		} else if (this->theEvent == "GUILD_SCHEDULED_EVENT_USER_ADD") {
			return 36;
		} else if (this->theEvent == "GUILD_SCHEDULED_EVENT_USER_REMOVE") {
			return 37;
		} else if (this->theEvent == "INTEGRATION_CREATE") {
			return 38;
		} else if (this->theEvent == "INTEGRATION_UPDATE") {
			return 39;
		} else if (this->theEvent == "INTEGRATION_DELETE") {
			return 40;
		} else if (this->theEvent == "INTERACTION_CREATE") {
			return 41;
		} else if (this->theEvent == "INVITE_CREATE") {
			return 42;
		} else if (this->theEvent == "INVITE_DELETE") {
			return 43;
		} else if (this->theEvent == "MESSAGE_CREATE") {
			return 44;
		} else if (this->theEvent == "MESSAGE_UPDATE") {
			return 45;
		} else if (this->theEvent == "MESSAGE_DELETE") {
			return 46;
		} else if (this->theEvent == "MESSAGE_DELETE_BULK") {
			return 47;
		} else if (this->theEvent == "MESSAGE_REACTION_ADD") {
			return 48;
		} else if (this->theEvent == "MESSAGE_REACTION_REMOVE") {
			return 49;
		} else if (this->theEvent == "MESSAGE_REACTION_REMOVE_ALL") {
			return 50;
		} else if (this->theEvent == "MESSAGE_REACTION_REMOVE_EMOJI") {
			return 51;
		} else if (this->theEvent == "PRESENCE_UPDATE") {
			return 52;
		} else if (this->theEvent == "STAGE_INSTANCE_CREATE") {
			return 53;
		} else if (this->theEvent == "STAGE_INSTANCE_UPDATE") {
			return 54;
		} else if (this->theEvent == "STAGE_INSTANCE_DELETE") {
			return 55;
		} else if (this->theEvent == "TYPING_START") {
			return 56;
		} else if (this->theEvent == "USER_UPDATE") {
			return 57;
		} else if (this->theEvent == "VOICE_STATE_UPDATE") {
			return 58;
		} else if (this->theEvent == "VOICE_SERVER_UPDATE") {
			return 59;
		} else if (this->theEvent == "WEBHOOKS_UPDATE") {
			return 60;
		} else {
			return 0;
		}
	}

	WebSocketCore::WebSocketCore(DiscordCoreAPI::ConfigManager* configManagerNew, String typeOfWebSocketNew) {
		this->typeOfWebSocket = typeOfWebSocketNew;
		this->configManager = configManagerNew;
	}

	String WebSocketCore::stringifyJsonData(DiscordCoreAPI::JsonObject&& dataToSend, WebSocketOpCode theOpCode) noexcept {
		String theVector{};
		String header{};

		if (theOpCode == WebSocketOpCode::Op_Binary) {
			theVector = ErlPacker::parseJsonToEtf(std::move(dataToSend));
		} else {
			theVector = static_cast<std::string>(dataToSend);
		}
		this->createHeader(header, theVector.size(), theOpCode);
		String theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		return theVectorNew;
	}

	Void WebSocketCore::createHeader(String& outBuffer, Uint64 sendLength, WebSocketOpCode opCode) noexcept {
		try {
			outBuffer.push_back(static_cast<Uint8>(opCode) | webSocketFinishBit);

			Int32 indexCount{ 0 };
			if (sendLength <= webSocketMaxPayloadLengthSmall) {
				outBuffer.push_back(static_cast<Uint8>(sendLength));
				indexCount = 0;
			} else if (sendLength <= webSocketMaxPayloadLengthLarge) {
				outBuffer.push_back(webSocketPayloadLengthMagicLarge);
				indexCount = 2;
			} else {
				outBuffer.push_back(webSocketPayloadLengthMagicHuge);
				indexCount = 8;
			}
			for (Int32 x = indexCount - 1; x >= 0; x--) {
				outBuffer.push_back(static_cast<Uint8>(sendLength >> x * 8));
			}

			outBuffer[1] |= webSocketMaskBit;
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("WebSocketCore::createHeader()");
			}
		}
	}

	Void WebSocketCore::parseConnectionHeaders() noexcept {
		if (this->areWeStillConnected() && this->currentState.load() == WebSocketState::Upgrading && this->inputBuffer.getCurrentTail()->getUsedSpace() > 100) {
			auto theString = this->getInputBuffer();
			this->currentMessage.writeData(theString.data(), theString.size());
			auto theFindValue = static_cast<StringView>(this->currentMessage).find("\r\n\r\n");
			if (theFindValue != String::npos) {
				this->currentMessage.clear();
				this->currentState.store(WebSocketState::Collecting_Hello);
				return;
			}
		}
		return;
	}

	Void WebSocketCore::parseMessage() noexcept {
		if (this->inputBuffer.getUsedSpace() > 0) {
			if (this->currentMessage.size() < this->messageLength + this->messageOffset || this->currentMessage.size() == 0) {
				auto theString = this->getInputBuffer();
				this->currentMessage.writeData(theString.data(), theString.size());
			}
			if (this->currentMessage.size() < 4) {
				return;
			}

			this->dataOpCode = static_cast<WebSocketOpCode>(this->currentMessage[0] & ~webSocketFinishBit);
			this->messageLength = 0;
			this->messageOffset = 0;
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
					Uint8 length01 = this->currentMessage[1];
					this->messageOffset = 2;
					if (length01 & webSocketMaskBit) {
						return;
					}
					this->messageLength = length01;
					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (this->currentMessage.size() < 8) {
							return;
						}
						Uint8 length03 = this->currentMessage[2];
						Uint8 length04 = this->currentMessage[3];
						this->messageLength = static_cast<Uint64>((length03 << 8) | length04);
						this->messageOffset += 2;
					} else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (this->currentMessage.size() < 10) {
							return;
						}
						this->messageLength = 0;
						for (Uint64 x = 2, shift = 56; x < 10; ++x, shift -= 8) {
							Uint8 lengthNew = static_cast<Uint8>(this->currentMessage[x]);
							this->messageLength |= static_cast<Uint64>((lengthNew & static_cast<Uint64>(0xff)) << static_cast<Uint64>(shift));
						}
						this->messageOffset += 8;
					}
					if (this->currentMessage.size() < this->messageOffset + this->messageLength) {
						return;
					} else {
						
						if (this->onMessageReceived(this->currentMessage[LengthData{ .offSet = this->messageOffset, .length = this->messageLength }])) {
							this->currentMessage.erase(this->messageLength + this->messageOffset);
							this->messageOffset = 0;
							this->messageLength = 0;
						}
						return;
					}
				}
				case WebSocketOpCode::Op_Close: {
					Uint16 close = this->currentMessage[2] & 0xff;
					close <<= 8;
					close |= this->currentMessage[3] & 0xff;
					this->closeCode = close;
					if (this->closeCode) {
						this->areWeResuming = true;
					}
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed()
							 << this->typeOfWebSocket + " [" + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[0]) + "," +
								std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[1]) + "]" + " Closed; Code: "
							 << +static_cast<Uint16>(this->closeCode) << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					this->onClosed();
					return;
				}
			}
		}
	}

	WebSocketSSLShard::WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* theClient, Int32 currentShardNew, AtomicBool* doWeQuitNew)
		: WebSocketCore(&theClient->configManager, "WebSocket") {
		this->configManager = &theClient->configManager;
		this->shard[0] = currentShardNew;
		if (this->theParser.allocate(1024ull * 1024ull) != simdjson::error_code::SUCCESS) {
			throw std::runtime_error{ "Failed to allocate the parser's memory." };
		}
		this->discordCoreClient = theClient;
		this->doWeQuit = doWeQuitNew;
		if (this->discordCoreClient) {
			this->shard[1] = this->discordCoreClient->configManager.getTotalShardCount();
			if (this->discordCoreClient->configManager.getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
				this->dataOpCode = WebSocketOpCode::Op_Binary;
			} else {
				this->dataOpCode = WebSocketOpCode::Op_Text;
			}
		}
	}

	Void WebSocketSSLShard::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept {
		try {
			while (this->currentState.load() != WebSocketState::Authenticated) {
				std::this_thread::sleep_for(1ms);
			}
			Int32 theCurrentIndex = this->shard[0];
			DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
			dataPackage.channelId = 0;
			dataPackage.guildId = static_cast<VoiceConnectInitData>(doWeCollect).guildId;
			dataPackage.selfDeaf = doWeCollect.selfDeaf;
			dataPackage.selfMute = doWeCollect.selfMute;
			this->userId = doWeCollect.userId;
			String theString = this->stringifyJsonData(dataPackage, this->dataOpCode);
			Bool didWeWrite{ false };
			if (!this->sendMessage(theString, true)) {
				return;
			}
			if (DiscordCoreAPI::Snowflake{ doWeCollect.channelId } == 0) {
				return;
			}
			dataPackage.channelId = static_cast<VoiceConnectInitData>(doWeCollect).channelId;
			String theString02 = this->stringifyJsonData(dataPackage, this->dataOpCode);
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

	Bool WebSocketCore::sendMessage(StringView dataToSend, Bool priority) noexcept {
		if (this->areWeStillConnected()) {
			try {
				if (dataToSend.size() == 0) {
					return false;
				}
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					cout << DiscordCoreAPI::shiftToBrightBlue()
						 << "Sending " + this->typeOfWebSocket + " [" + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[0]) + "," +
							std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[1]) + "]" + String("'s Message: ")
						 << static_cast<std::string>(dataToSend) << endl
						 << endl
						 << DiscordCoreAPI::reset();
				}
				ProcessIOResult didWeWrite{ false };
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				theStopWatch.resetTimer();
				do {
					if (theStopWatch.hasTimePassed()) {
						this->onClosed();
						return false;
					}
					didWeWrite = this->writeData(dataToSend, priority);
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

	DiscordCoreAPI::StopWatch theStopWatch{ 5s };
	DiscordCoreAPI::StopWatch theStopWatchReal{ 50us };
	AtomicInt32 theInt{};
	Bool WebSocketSSLShard::onMessageReceived(StringView theDataNew) noexcept {
		if (this->discordCoreClient) {
			String theString{};
			String& payload{ theString };
			if (this->areWeStillConnected() && this->currentMessage.size() > 0) {
				try {
					Bool returnValue{ false };

					simdjson::ondemand::value thePayload{};
					WebSocketMessage theMessage{};
					if (theDataNew.size() > 0) {
						returnValue = true;

						if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
							try {
								theStopWatchReal.resetTimer();
								payload = ErlPacker::parseEtfToJson(theDataNew);
								payload.reserve(payload.size() + simdjson::SIMDJSON_PADDING);
								simdjson::ondemand::value theValue{};
								if (this->theParser.iterate(simdjson::padded_string_view(payload.data(), payload.length(), payload.capacity())).get(theValue) ==
									simdjson::error_code::SUCCESS) {
									theMessage = WebSocketMessage{ theValue };
								}
							} catch (...) {
								if (this->configManager->doWePrintGeneralErrorMessages()) {
									DiscordCoreAPI::reportException("ErlPacker::parseEtfToJson()");
								}
								this->currentMessage.clear();
								this->inputBuffer.clear();
								this->messageLength = 0;
								this->messageOffset = 0;
								return false;
							}
						} else {
							std::string theData{ theDataNew };
							payload = theData;
							payload.reserve(payload.size() + simdjson::SIMDJSON_PADDING);
							simdjson::ondemand::value theValue{};
							if (this->theParser.iterate(simdjson::padded_string_view(payload.data(), payload.length(), payload.capacity())).get(theValue) ==
								simdjson::error_code::SUCCESS) {
								theMessage = WebSocketMessage{ theValue };
							}
						}
					}

					if (theMessage.s != 0) {
						this->lastNumberReceived = theMessage.s;
					}

					if (this->configManager->doWePrintWebSocketSuccessMessages()) {
						cout << DiscordCoreAPI::shiftToBrightGreen()
							 << "Message received from WebSocket [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" + String(": ") << payload
							 << DiscordCoreAPI::reset() << endl
							 << endl;
					}

					switch (theMessage.op) {
						case 0: {
							if (theMessage.t != "") {
								switch (EventConverter{ theMessage.t }) {
									case 1: {
										ReadyData theData{ theMessage.processJsonMessage<ReadyData>() };
										this->currentState.store(WebSocketState::Authenticated);
										this->sessionId = theData.sessionId;
										String theResumeUrl = theData.resumeGatewayUrl;
										theResumeUrl = theResumeUrl.substr(theResumeUrl.find("wss://") + String{ "wss://" }.size());
										theResumeUrl = theResumeUrl.substr(0, theResumeUrl.find("/"));
										this->resumeUrl = theResumeUrl;
										DiscordCoreAPI::UserData theUser{ theData.user };
										this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ theUser,
											this->discordCoreClient
												->baseSocketAgentMap[static_cast<Int32>(
													floor(static_cast<Int32>(this->shard[0]) % static_cast<Int32>(this->discordCoreClient->configManager.getTotalShardCount())))]
												.get() };
										DiscordCoreAPI::Users::insertUser(std::move(theUser));
										this->currentReconnectTries = 0;
										break;
									}
									case 2: {
										this->currentState.store(WebSocketState::Authenticated);
										this->currentReconnectTries = 0;
										break;
									}
									case 3: {
										UniquePtr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>()
										};
										dataPackage->permissionData =
											DiscordCoreAPI::GuildApplicationCommandPermissionsData{ theMessage.processJsonMessage<DiscordCoreAPI::GuildApplicationCommandPermissionsData>() };
										this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
										break;
									}

									case 4: {
										UniquePtr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>()
										};
										dataPackage->theRule = DiscordCoreAPI::AutoModerationRule{ theMessage.processJsonMessage<DiscordCoreAPI::AutoModerationRule>() };
										this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
										break;
									}
									case 5: {
										UniquePtr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>()
										};
										dataPackage->theRule = DiscordCoreAPI::AutoModerationRule{ theMessage.processJsonMessage<DiscordCoreAPI::AutoModerationRule>() };
										this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
										break;
									}
									case 6: {
										UniquePtr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>()
										};
										dataPackage->theRule = DiscordCoreAPI::AutoModerationRule{ theMessage.processJsonMessage<DiscordCoreAPI::AutoModerationRule>() };
										this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
										break;
									}
									case 7: {
										UniquePtr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>()
										};
										dataPackage->theData = DiscordCoreAPI::AutoModerationActionExecutionEventData{
											theMessage.processJsonMessage<DiscordCoreAPI::AutoModerationActionExecutionEventData>()
										};
										this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
										break;
									}
									case 8: {
										if (DiscordCoreAPI::Channels::doWeCacheChannels || this->discordCoreClient->eventManager.onChannelCreationEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::ChannelData> theChannel{ std::make_unique<DiscordCoreAPI::ChannelData>(
												theMessage.processJsonMessage<DiscordCoreAPI::ChannelData>()) };
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = theChannel->guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::Guilds::cache[theGuild].channels.emplace_back(theChannel->id);
											}
											if (DiscordCoreAPI::Channels::doWeCacheChannels) {
												DiscordCoreAPI::Channels::insertChannel(*theChannel);
											}
											if (this->discordCoreClient->eventManager.onChannelCreationEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnChannelCreationData dataPackage{ std::move(theChannel) };
												this->discordCoreClient->eventManager.onChannelCreationEvent(dataPackage);
											}
										}
										break;
									}
									case 9: {
										if (DiscordCoreAPI::Channels::doWeCacheChannels || this->discordCoreClient->eventManager.onChannelUpdateEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::ChannelData> theChannel{ std::make_unique<DiscordCoreAPI::ChannelData>(theMessage.processJsonMessage<DiscordCoreAPI::ChannelData>()) };
											if (DiscordCoreAPI::Channels::doWeCacheChannels) {
												DiscordCoreAPI::Channels::insertChannel(*theChannel);
											}
											if (this->discordCoreClient->eventManager.onChannelUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnChannelUpdateData dataPackage{ std::move(theChannel) };
												this->discordCoreClient->eventManager.onChannelUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 10: {
										if (DiscordCoreAPI::Channels::doWeCacheChannels || this->discordCoreClient->eventManager.onChannelDeletionEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::ChannelData> theChannel{ std::make_unique<DiscordCoreAPI::ChannelData>(theMessage.processJsonMessage<DiscordCoreAPI::ChannelData>()) };
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = theChannel->guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												for (Uint64 x = 0; x < DiscordCoreAPI::Guilds::cache[theGuild].channels.size(); ++x) {
													if (DiscordCoreAPI::Guilds::cache[theGuild].channels[x] == theChannel->id) {
														DiscordCoreAPI::Guilds::cache[theGuild].channels.erase(DiscordCoreAPI::Guilds::cache[theGuild].channels.begin() + x);
													}
												}
											}
											if (DiscordCoreAPI::Channels::doWeCacheChannels) {
												DiscordCoreAPI::Channels::removeChannel(theChannel->id);
											}
											if (this->discordCoreClient->eventManager.onChannelDeletionEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnChannelDeletionData dataPackage{ std::move(theChannel) };
												this->discordCoreClient->eventManager.onChannelDeletionEvent(dataPackage);
											}
										}
										break;
									}
									case 11: {
										UniquePtr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
										dataPackage->dataPackage =
											DiscordCoreAPI::ChannelPinsUpdateEventData{ theMessage.processJsonMessage<DiscordCoreAPI::ChannelPinsUpdateEventData>() };
										this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
										break;
									}
									case 12: {
										UniquePtr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
										dataPackage->thread = DiscordCoreAPI::Thread{ theMessage.processJsonMessage<DiscordCoreAPI::Thread>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->thread.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											DiscordCoreAPI::Guilds::cache[theGuild].threads.emplace_back(dataPackage->thread.id);
										}
										this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
										break;
									}
									case 13: {
										UniquePtr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
										dataPackage->thread = DiscordCoreAPI::Thread{ theMessage.processJsonMessage<DiscordCoreAPI::Thread>() };
										this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
										break;
									}
									case 14: {
										UniquePtr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
										dataPackage->thread = DiscordCoreAPI::Thread{ theMessage.processJsonMessage<DiscordCoreAPI::Thread>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->thread.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											for (Uint64 x = 0; x < DiscordCoreAPI::Guilds::cache[theGuild].threads.size(); ++x) {
												if (DiscordCoreAPI::Guilds::cache[theGuild].threads[x] == dataPackage->thread.id) {
													DiscordCoreAPI::Guilds::cache[theGuild].threads.erase(DiscordCoreAPI::Guilds::cache[theGuild].threads.begin() + x);
												}
											}
										}
										this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
										break;
									}
									case 15: {
										UniquePtr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
										dataPackage->threadListSyncData = DiscordCoreAPI::ThreadListSyncData{ theMessage.processJsonMessage<DiscordCoreAPI::ThreadListSyncData>() };
										this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
										break;
									}
									case 16: {
										UniquePtr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
										dataPackage->threadMember = DiscordCoreAPI::ThreadMemberData{ theMessage.processJsonMessage<DiscordCoreAPI::ThreadMemberData>() };
										this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
										break;
									}
									case 17: {
										UniquePtr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
										dataPackage->threadMembersUpdateData =
											DiscordCoreAPI::ThreadMembersUpdateData{ theMessage.processJsonMessage<DiscordCoreAPI::ThreadMembersUpdateData>() };
										this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
										break;
									}
									case 18: {
										if (theInt.load() % 100 == 0 || theStopWatch.totalTimePassed() % 10 == 0) {
											//std::cout << "THE GUILD COUNT: " << theInt.load() << ", TOTAL TIME: " << theStopWatch.totalTimePassed() << std::endl;
										}
										theInt.store(theInt.load() + 1);
										UniquePtr<DiscordCoreAPI::GuildData> theGuildPtr{ std::make_unique<DiscordCoreAPI::GuildData>(
											theMessage.processJsonMessage<DiscordCoreAPI::GuildData>()) };
										DiscordCoreAPI::Snowflake guildId{};
										theStopWatchReal.resetTimer();
										guildId = theGuildPtr->id;
										if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildCreationEvent.theFunctions.size() > 0) {
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
												DiscordCoreAPI::Guilds::insertGuild(*theGuildPtr);
											}
											if (this->discordCoreClient->eventManager.onGuildCreationEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnGuildCreationData dataPackage{ std::move(theGuildPtr), this->discordCoreClient };
												this->discordCoreClient->eventManager.onGuildCreationEvent(dataPackage);
											}
										}
										break;
									}
									case 19: {
										if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildUpdateEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::GuildData> theGuildPtr{ std::make_unique<DiscordCoreAPI::GuildData>(theMessage.processJsonMessage<DiscordCoreAPI::GuildData>()) };
											DiscordCoreAPI::Snowflake guildId{};
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
												DiscordCoreAPI::Guilds::insertGuild(*theGuildPtr);
											}
											if (this->discordCoreClient->eventManager.onGuildUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnGuildUpdateData dataPackage{ std::make_unique<DiscordCoreAPI::GuildData>(*theGuildPtr), this->discordCoreClient };
												this->discordCoreClient->eventManager.onGuildUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 20: {
										if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildDeletionEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::GuildData> theGuild =
												std::make_unique<DiscordCoreAPI::GuildData>(theMessage.processJsonMessage<DiscordCoreAPI::GuildData>());
											DiscordCoreAPI::Snowflake guildId{};
											*theGuild = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = guildId }).get();
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
												DiscordCoreAPI::Guilds::removeGuild(theGuild->id);
											}
											for (auto& value: theGuild->members) {
												DiscordCoreAPI::GuildMemberData theGuildMember =
													DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = value, .guildId = guildId }).get();
												DiscordCoreAPI::GuildMembers::removeGuildMember(theGuildMember);
											}
											for (auto& value: theGuild->channels) {
												DiscordCoreAPI::Channels::removeChannel(value);
											}
											for (auto& value: theGuild->roles) {
												DiscordCoreAPI::Roles::removeRole(value);
											}
											if (this->discordCoreClient->eventManager.onGuildDeletionEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnGuildDeletionData dataPackage{ std::move(theGuild) };
												this->discordCoreClient->eventManager.onGuildDeletionEvent(dataPackage);
											}
										}
										break;
									}
									case 21: {
										UniquePtr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->user = DiscordCoreAPI::UserData{ theMessage.processJsonMessage<DiscordCoreAPI::UserData>() };
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
										break;
									}
									case 22: {
										UniquePtr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->user = DiscordCoreAPI::UserData{ theMessage.processJsonMessage<DiscordCoreAPI::UserData>() };
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
										break;
									}
									case 23: {
										UniquePtr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
										dataPackage->updateData =
											DiscordCoreAPI::GuildEmojisUpdateEventData{ theMessage.processJsonMessage<DiscordCoreAPI::GuildEmojisUpdateEventData>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->updateData.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											DiscordCoreAPI::Guilds::cache[theGuild].emoji.clear();
											for (auto& value: dataPackage->updateData.emojis) {
												DiscordCoreAPI::Guilds::cache[theGuild].emoji.emplace_back(value.id);
											}
										}
										this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
										break;
									}
									case 24: {
										UniquePtr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
										dataPackage->updateData =
											DiscordCoreAPI::GuildStickersUpdateEventData{ theMessage.processJsonMessage<DiscordCoreAPI::GuildStickersUpdateEventData>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->updateData.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											DiscordCoreAPI::Guilds::cache[theGuild].stickers.clear();
											for (auto& value: dataPackage->updateData.stickers) {
												DiscordCoreAPI::Guilds::cache[theGuild].stickers.emplace_back(value.id);
											}
										}
										this->discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
										break;
									}
									case 25: {
										UniquePtr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>()
										};
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
										break;
									}
									case 26: {
										if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
											this->discordCoreClient->eventManager.onGuildMemberAddEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::GuildMemberData> theGuildMember{ std::make_unique<DiscordCoreAPI::GuildMemberData>(
												theMessage.processJsonMessage<DiscordCoreAPI::GuildMemberData>()) };
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
												DiscordCoreAPI::GuildMembers::insertGuildMember(*theGuildMember);
												DiscordCoreAPI::GuildData theGuild{};
												theGuild.id = theGuildMember->guildId;
												if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
													DiscordCoreAPI::Guilds::cache[theGuild].memberCount++;
												}
											}
											if (this->discordCoreClient->eventManager.onGuildMemberAddEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnGuildMemberAddData dataPackage{ std::move(theGuildMember), this->discordCoreClient };
												this->discordCoreClient->eventManager.onGuildMemberAddEvent(dataPackage);
											}
										}
										break;
									}
									case 27: {
										if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
											this->discordCoreClient->eventManager.onGuildMemberRemoveEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::GuildMemberData> theGuildMember{ std::make_unique<DiscordCoreAPI::GuildMemberData>(
												theMessage.processJsonMessage<DiscordCoreAPI::GuildMemberData>()) };
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
												DiscordCoreAPI::GuildData theGuild{};
												theGuild.id = theGuildMember->guildId;
												DiscordCoreAPI::GuildMembers::removeGuildMember(*theGuildMember);
												if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
													for (Uint64 x = 0; x < DiscordCoreAPI::Guilds::cache[theGuild].members.size(); ++x) {
														if (DiscordCoreAPI::Guilds::cache[theGuild].members[x] == userId) {
															DiscordCoreAPI::Guilds::cache[theGuild].memberCount--;
															DiscordCoreAPI::Guilds::cache[theGuild].members.erase(DiscordCoreAPI::Guilds::cache[theGuild].members.begin() + x);
														}
													}
												}
											}
											if (this->discordCoreClient->eventManager.onGuildMemberRemoveEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnGuildMemberRemoveData dataPackage{ std::make_unique<DiscordCoreAPI::UserData>(
																										 DiscordCoreAPI::Users::getCachedUserAsync({ .userId = userId }).get()),
													this->discordCoreClient, userId };
												this->discordCoreClient->eventManager.onGuildMemberRemoveEvent(dataPackage);
											}
										}
										break;
									}
									case 28: {
										if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
											this->discordCoreClient->eventManager.onGuildMemberUpdateEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::GuildMemberData> theGuildMember{ std::make_unique<DiscordCoreAPI::GuildMemberData>(
												theMessage.processJsonMessage<DiscordCoreAPI::GuildMemberData>()) };
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
												DiscordCoreAPI::GuildMembers::insertGuildMember(*theGuildMember);
											}
											if (this->discordCoreClient->eventManager.onGuildMemberUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnGuildMemberUpdateData dataPackage{ std::move(theGuildMember) };
												this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 29: {
										UniquePtr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
										dataPackage->chunkEventData =
											DiscordCoreAPI::GuildMembersChunkEventData{ theMessage.processJsonMessage<DiscordCoreAPI::GuildMembersChunkEventData>() };
										this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
										break;
									}
									case 30: {
										if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleCreationEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::RoleData> theRolePtr{ std::make_unique<DiscordCoreAPI::RoleData>(theMessage.d["role"]) };
											StringView theString{};
											DiscordCoreAPI::Snowflake guildId{};
											if (theMessage.d["guild_id"].get(theString) == simdjson::error_code::SUCCESS) {
												guildId = stoull(static_cast<String>(theString));
											}
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::Guilds::cache[theGuild].roles.emplace_back(theRolePtr->id);
											}
											if (DiscordCoreAPI::Roles::doWeCacheRoles) {
												DiscordCoreAPI::Roles::insertRole(*theRolePtr);
											}
											if (this->discordCoreClient->eventManager.onRoleCreationEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnRoleCreationData dataPackage{ std::move(theRolePtr), guildId };
												this->discordCoreClient->eventManager.onRoleCreationEvent(dataPackage);
											}
										}
										break;
									}
									case 31: {
										if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleUpdateEvent.theFunctions.size() > 0) {
											UniquePtr<DiscordCoreAPI::RoleData> theRolePtr{ std::make_unique<DiscordCoreAPI::RoleData>(theMessage.d["role"]) };
											StringView theString{};
											DiscordCoreAPI::Snowflake guildId{};
											if (theMessage.d["guild_id"].get(theString) == simdjson::error_code::SUCCESS) {
												guildId = stoull(static_cast<String>(theString));
											}
											if (DiscordCoreAPI::Roles::doWeCacheRoles) {
												DiscordCoreAPI::Roles::insertRole(*theRolePtr);
											}
											if (this->discordCoreClient->eventManager.onRoleUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnRoleUpdateData dataPackage{ std::move(theRolePtr), guildId };
												this->discordCoreClient->eventManager.onRoleUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 32: {
										if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleDeletionEvent.theFunctions.size() > 0) {
											StringView theString{};
											theMessage.d["role_id"].get(theString);
											DiscordCoreAPI::Snowflake roleId = DiscordCoreAPI::Snowflake{ stoull(static_cast<String>(theString)) };
											theMessage.d["guild_hashes"]["guild_id"].get(theString);
											DiscordCoreAPI::Snowflake guildId = DiscordCoreAPI::Snowflake{ stoull(static_cast<String>(theString)) };

											DiscordCoreAPI::OnRoleDeletionData dataPackage{ std::make_unique<DiscordCoreAPI::RoleData>(), guildId };
											DiscordCoreAPI::RoleData theRole = DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = guildId, .roleId = roleId }).get();
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = guildId;
											if (DiscordCoreAPI::Roles::doWeCacheRoles) {
												DiscordCoreAPI::Roles::removeRole(roleId);
												if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
													for (Uint64 x = 0; x < DiscordCoreAPI::Guilds::cache[theGuild].roles.size(); ++x) {
														if (DiscordCoreAPI::Guilds::cache[theGuild].roles[x] == roleId) {
															DiscordCoreAPI::Guilds::cache[theGuild].roles.erase(DiscordCoreAPI::Guilds::cache[theGuild].roles.begin() + x);
														}
													}
												}
											}
											if (this->discordCoreClient->eventManager.onRoleDeletionEvent.theFunctions.size() > 0) {
												this->discordCoreClient->eventManager.onRoleDeletionEvent(dataPackage);
											}
										}
										break;
									}
									case 33: {
										UniquePtr<DiscordCoreAPI::OnGuildScheduledEventCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventCreationData>()
										};
										dataPackage->guildScheduledEvent =
											DiscordCoreAPI::GuildScheduledEventData{ theMessage.processJsonMessage<DiscordCoreAPI::GuildScheduledEventData>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->guildScheduledEvent.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											DiscordCoreAPI::Guilds::cache[theGuild].guildScheduledEvents.emplace_back(dataPackage->guildScheduledEvent.id);
										}
										this->discordCoreClient->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
										break;
									}
									case 34: {
										UniquePtr<DiscordCoreAPI::OnGuildScheduledEventUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUpdateData>()
										};
										dataPackage->guildScheduledEvent =
											DiscordCoreAPI::GuildScheduledEventData{ theMessage.processJsonMessage<DiscordCoreAPI::GuildScheduledEventData>() };
										this->discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
										break;
									}
									case 35: {
										UniquePtr<DiscordCoreAPI::OnGuildScheduledEventDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventDeletionData>()
										};
										dataPackage->guildScheduledEvent =
											DiscordCoreAPI::GuildScheduledEventData{ theMessage.processJsonMessage<DiscordCoreAPI::GuildScheduledEventData>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->guildScheduledEvent.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											for (Uint64 x = 0; x < DiscordCoreAPI::Guilds::cache[theGuild].guildScheduledEvents.size(); ++x) {
												if (DiscordCoreAPI::Guilds::cache[theGuild].guildScheduledEvents[x] == dataPackage->guildScheduledEvent.id) {
													DiscordCoreAPI::Guilds::cache[theGuild].guildScheduledEvents.erase(
														DiscordCoreAPI::Guilds::cache[theGuild].guildScheduledEvents.begin() + x);
												}
											}
										}
										this->discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
										break;
									}
									case 36: {
										UniquePtr<DiscordCoreAPI::OnGuildScheduledEventUserAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserAddData>()
										};
										StringView theString{};
										theMessage.d["user_id"].get(theString);
										dataPackage->userId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["guild_scheduled_event_id"].get(theString);
										dataPackage->guildScheduledEventId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
										break;
									}
									case 37: {
										UniquePtr<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData>()
										};
										StringView theString{};
										theMessage.d["user_id"].get(theString);
										dataPackage->userId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["guild_scheduled_event_id"].get(theString);
										dataPackage->guildScheduledEventId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
										break;
									}
									case 38: {
										UniquePtr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										simdjson::ondemand::value theObjectNew = theMessage.d["integration"].value();
										dataPackage->integrationData = DiscordCoreAPI::IntegrationData{ theObjectNew };
										this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
										break;
									}
									case 39: {
										UniquePtr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										simdjson::ondemand::value theObjectNew = theMessage.d["integration"].value();
										dataPackage->integrationData = DiscordCoreAPI::IntegrationData{ theObjectNew };
										this->discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
										break;
									}
									case 40: {
										UniquePtr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["application_id"].get(theString);
										dataPackage->applicationId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["id"].get(theString);
										dataPackage->id = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
										break;
									}
									case 41: {
										UniquePtr<DiscordCoreAPI::InteractionData> interactionData{ std::make_unique<DiscordCoreAPI::InteractionData>() };
										*interactionData = DiscordCoreAPI::InteractionData{ theMessage.processJsonMessage<DiscordCoreAPI::InteractionData>() };
										UniquePtr<DiscordCoreAPI::InputEventData> eventData{ std::make_unique<DiscordCoreAPI::InputEventData>(*interactionData) };
										switch (interactionData->type) {
											case DiscordCoreAPI::InteractionType::Application_Command: {
												eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
												*eventData->interactionData = *interactionData;
												UniquePtr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
													std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
												};
												dataPackage->interactionData = *interactionData;
												UniquePtr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>(*eventData) };
												DiscordCoreAPI::CommandData commandDataNew = *commandData;
												this->discordCoreClient->commandController.checkForAndRunCommand(commandDataNew);
												this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
												UniquePtr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
													std::make_unique<DiscordCoreAPI::OnInputEventCreationData>()
												};
												eventCreationData->inputEventData = *eventData;
												this->discordCoreClient->eventManager.onInputEventCreationEvent(*eventCreationData);
												break;
											}
											case DiscordCoreAPI::InteractionType::Message_Component: {
												switch (interactionData->data.componentData.componentType) {
													case DiscordCoreAPI::ComponentType::Button: {
														eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
														*eventData->interactionData = *interactionData;
														UniquePtr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
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
														break;
													}
													case DiscordCoreAPI::ComponentType::SelectMenu: {
														eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
														*eventData->interactionData = *interactionData;
														UniquePtr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
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
														break;
													}
												}
												break;
											}
											case DiscordCoreAPI::InteractionType::Modal_Submit: {
												eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
												*eventData->interactionData = *interactionData;
												UniquePtr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
													std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
												};
												dataPackage->interactionData = *interactionData;
												UniquePtr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
													std::make_unique<DiscordCoreAPI::OnInputEventCreationData>()
												};
												eventCreationData->inputEventData = *eventData;
												if (DiscordCoreAPI::ModalCollector::modalInteractionBufferMap.contains(std::to_string(eventData->getChannelId()))) {
													DiscordCoreAPI::ModalCollector::modalInteractionBufferMap[std::to_string(eventData->getChannelId())]->send(
														eventData->getInteractionData());
												}
												this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
												break;
											}
											case DiscordCoreAPI::InteractionType::Application_Command_Autocomplete: {
												eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
												*eventData->interactionData = *interactionData;
												UniquePtr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
													std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
												};
												dataPackage->interactionData = *interactionData;
												UniquePtr<DiscordCoreAPI::OnAutoCompleteEntryData> autocompleteEntryData{
													std::make_unique<DiscordCoreAPI::OnAutoCompleteEntryData>()
												};
												autocompleteEntryData->inputEvent = *eventData;
												this->discordCoreClient->eventManager.onAutoCompleteEntryEvent(*autocompleteEntryData);
												this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
												break;
											}
										}
										break;
									}
									case 42: {
										UniquePtr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
										dataPackage->invite = DiscordCoreAPI::InviteData{ theMessage.processJsonMessage<DiscordCoreAPI::InviteData>() };
										this->discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
										break;
									}
									case 43: {
										UniquePtr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteDeletionData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["channel_id"].get(theString);
										dataPackage->channelId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["code"].get(theString);
										dataPackage->code = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
										break;
									}
									case 44: {
										UniquePtr<DiscordCoreAPI::Message> message{ std::make_unique<DiscordCoreAPI::Message>(
											theMessage.processJsonMessage<DiscordCoreAPI::Message>()) };
										UniquePtr<DiscordCoreAPI::OnMessageCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageCreationData>() };
										dataPackage->message = *message;
										for (auto& [key, value]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBufferMap) {
											value->send(*message);
										}
										this->discordCoreClient->eventManager.onMessageCreationEvent(*dataPackage);
										if (message->content.find("!registerapplicationcommands") != String::npos) {
											UniquePtr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>() };
											commandData->commandName = "registerapplicationcommands";
											DiscordCoreAPI::CommandData commandDataNew = *commandData;
											this->discordCoreClient->commandController.checkForAndRunCommand(commandDataNew);
										}
										UniquePtr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
										break;
									}
									case 45: {
										UniquePtr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
										dataPackage->messageNew = DiscordCoreAPI::Message{ theMessage.processJsonMessage<DiscordCoreAPI::Message>() };
										for (auto& [key, value]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBufferMap) {
											value->send(dataPackage->messageNew);
										}
										this->discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
										break;
									}
									case 46: {
										UniquePtr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeletionData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["channel_id"].get(theString);
										dataPackage->channelId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["id"].get(theString);
										dataPackage->messageId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
										break;
									}
									case 47: {
										UniquePtr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["channel_id"].get(theString);
										dataPackage->channelId = DiscordCoreAPI::strtoull(theString);
										simdjson::ondemand::array theArray{};
										if (theMessage.d["ids"].get(theArray) == simdjson::error_code::SUCCESS) {
											for (auto value: theArray) {
												dataPackage->ids.emplace_back(DiscordCoreAPI::strtoull(value.get_string().take_value()));
											}
										}
										this->discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
										break;
									}
									case 48: {
										UniquePtr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
										dataPackage->reaction = DiscordCoreAPI::Reaction{ theMessage.processJsonMessage<DiscordCoreAPI::Reaction>() };
										this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
										break;
									}
									case 49: {
										UniquePtr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
										dataPackage->reactionRemoveData = DiscordCoreAPI::ReactionRemoveData{ theMessage.processJsonMessage<DiscordCoreAPI::ReactionRemoveData>() };
										this->discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
										break;
									}
									case 50: {
										UniquePtr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["channel_id"].get(theString);
										dataPackage->channelId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["message_id"].get(theString);
										dataPackage->messageId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
										break;
									}
									case 51: {
										UniquePtr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["channel_id"].get(theString);
										dataPackage->channelId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["message_id"].get(theString);
										dataPackage->messageId = DiscordCoreAPI::strtoull(theString);
										simdjson::ondemand::value theObjectNew = theMessage.d["emoji"].value();
										dataPackage->emoji = DiscordCoreAPI::EmojiData{ theMessage.d };
										this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
										break;
									}
									case 52: {
										UniquePtr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
										dataPackage->presenceData = DiscordCoreAPI::PresenceUpdateData{ theMessage.processJsonMessage<DiscordCoreAPI::PresenceUpdateData>() };
										this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
										break;
									}
									case 53: {
										UniquePtr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
										dataPackage->stageInstance = DiscordCoreAPI::StageInstance{ theMessage.processJsonMessage<DiscordCoreAPI::StageInstance>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->stageInstance.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											DiscordCoreAPI::Guilds::cache[theGuild].stageInstances.emplace_back(dataPackage->stageInstance.id);
										}
										this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
										break;
									}
									case 54: {
										UniquePtr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
										dataPackage->stageInstance = DiscordCoreAPI::StageInstance{ theMessage.processJsonMessage<DiscordCoreAPI::StageInstance>() };
										this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
										break;
									}
									case 55: {
										UniquePtr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
										dataPackage->stageInstance = DiscordCoreAPI::StageInstance{ theMessage.processJsonMessage<DiscordCoreAPI::StageInstance>() };
										DiscordCoreAPI::GuildData theGuild{};
										theGuild.id = dataPackage->stageInstance.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
											for (Uint64 x = 0; x < DiscordCoreAPI::Guilds::cache[theGuild].stageInstances.size(); ++x) {
												if (DiscordCoreAPI::Guilds::cache[theGuild].stageInstances[x] == dataPackage->stageInstance.id) {
													DiscordCoreAPI::Guilds::cache[theGuild].stageInstances.erase(
														DiscordCoreAPI::Guilds::cache[theGuild].stageInstances.begin() + x);
												}
											}
										}
										this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
										break;
									}
									case 56: {
										UniquePtr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
										dataPackage->typingStartData = DiscordCoreAPI::TypingStartData{ theMessage.processJsonMessage<DiscordCoreAPI::TypingStartData>() };
										this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
										break;
									}
									case 57: {
										UniquePtr<DiscordCoreAPI::UserData> theUserPtr{ std::make_unique<DiscordCoreAPI::UserData>(
											theMessage.processJsonMessage<DiscordCoreAPI::UserData>()) };
										if (DiscordCoreAPI::Users::doWeCacheUsers || this->discordCoreClient->eventManager.onUserUpdateEvent.theFunctions.size() > 0) {
											DiscordCoreAPI::Snowflake userId{ theUserPtr->id };
											if (DiscordCoreAPI::Users::doWeCacheUsers) {
												DiscordCoreAPI::Users::insertUser(*theUserPtr);
											}
											if (this->discordCoreClient->eventManager.onUserUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::OnUserUpdateData dataPackage{ std::move(theUserPtr) };
												this->discordCoreClient->eventManager.onUserUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 58: {
										UniquePtr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
										dataPackage->voiceStateData = DiscordCoreAPI::VoiceStateData{ theMessage.processJsonMessage<DiscordCoreAPI::VoiceStateData>() };
										this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
										if (this->areWeCollectingData && !this->stateUpdateCollected && !this->serverUpdateCollected &&
											dataPackage->voiceStateData.userId == this->userId) {
											this->voiceConnectionData = VoiceConnectionData{};
											this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
											this->stateUpdateCollected = true;
										} else if (this->areWeCollectingData && !this->stateUpdateCollected) {
											this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
											if (this->voiceConnectionDataBufferMap.contains(dataPackage->voiceStateData.guildId)) {
												this->voiceConnectionDataBufferMap[dataPackage->voiceStateData.guildId]->send(this->voiceConnectionData);
											}
											this->serverUpdateCollected = false;
											this->stateUpdateCollected = false;
											this->areWeCollectingData = false;
										}
										if (this->discordCoreClient->configManager.doWeCacheUsers() && this->discordCoreClient->configManager.doWeCacheGuilds()) {
											DiscordCoreAPI::GuildMemberData theData{};
											theData.id = dataPackage->voiceStateData.userId;
											theData.guildId = dataPackage->voiceStateData.guildId;
											if (DiscordCoreAPI::GuildMembers::cache.contains(theData)) {
												DiscordCoreAPI::GuildMembers::cache[theData].voiceChannelId = dataPackage->voiceStateData.channelId;
											}
										}
										this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
										break;
									}
									case 59: {
										UniquePtr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>() };
										*dataPackage = DiscordCoreAPI::OnVoiceServerUpdateData{ theMessage.processJsonMessage<DiscordCoreAPI::OnVoiceServerUpdateData>() };
										this->voiceConnectionData.endPoint = dataPackage->endpoint;
										this->voiceConnectionData.token = dataPackage->token;
										if (this->areWeCollectingData && !this->serverUpdateCollected && !this->stateUpdateCollected) {
											this->voiceConnectionData = VoiceConnectionData{};
											this->serverUpdateCollected = true;
										} else if (this->areWeCollectingData && !this->serverUpdateCollected) {
											if (this->voiceConnectionDataBufferMap.contains(dataPackage->guildId)) {
												this->voiceConnectionDataBufferMap[dataPackage->guildId]->send(this->voiceConnectionData);
											}
											this->serverUpdateCollected = false;
											this->stateUpdateCollected = false;
											this->areWeCollectingData = false;
										}

										this->discordCoreClient->eventManager.onVoiceServerUpdateEvent(*dataPackage);
										break;
									}
									case 60: {
										UniquePtr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
										StringView theString{};
										theMessage.d["guild_id"].get(theString);
										dataPackage->guildId = DiscordCoreAPI::strtoull(theString);
										theMessage.d["channel_id"].get(theString);
										dataPackage->channelId = DiscordCoreAPI::strtoull(theString);
										this->discordCoreClient->eventManager.onWebhookUpdateEvent(*dataPackage);
										break;
									}
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
								cout << DiscordCoreAPI::shiftToBrightBlue()
									 << "Shard [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" + " Reconnecting (Type 7)!"
									 << DiscordCoreAPI::reset() << endl
									 << endl;
							}
							this->areWeResuming = true;
							this->onClosed();
							returnValue = true;
							break;
						}
						case 9: {
							InvalidSessionData theData{ theMessage.processJsonMessage<InvalidSessionData>() };
							if (this->configManager->doWePrintWebSocketErrorMessages()) {
								cout << DiscordCoreAPI::shiftToBrightBlue()
									 << "Shard [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" + " Reconnecting (Type 9)!"
									 << DiscordCoreAPI::reset() << endl
									 << endl;
							}
							std::mt19937_64 randomEngine{ static_cast<Uint64>(std::chrono::steady_clock::now().time_since_epoch().count()) };
							Int32 numOfMsToWait =
								static_cast<Int32>(1000.0f + ((static_cast<Float>(randomEngine()) / static_cast<Float>(randomEngine.max())) * static_cast<Float>(4000.0f)));
							std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
							if (theData.d == true) {
								this->areWeResuming = true;
							} else {
								this->areWeResuming = false;
							}
							this->onClosed();
							returnValue = true;
							break;
						}
						case 10: {
							HelloData theData{ theMessage.processJsonMessage<HelloData>() };
							if (theData.heartbeatInterval != 0) {
								this->areWeHeartBeating = true;
								this->heartBeatStopWatch = DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ std::chrono::milliseconds{ theData.heartbeatInterval } };
								this->heartBeatStopWatch.resetTimer();
								this->haveWeReceivedHeartbeatAck = true;
							}
							if (this->areWeResuming) {
								WebSocketResumeData resumeData{};
								resumeData.botToken = this->configManager->getBotToken();
								resumeData.sessionId = this->sessionId;
								resumeData.lastNumberReceived = this->lastNumberReceived;
								String theString = this->stringifyJsonData(resumeData, this->dataOpCode);
								if (!this->sendMessage(theString, true)) {
									returnValue = true;
								}
								this->currentState.store(WebSocketState::Sending_Identify);
							} else {
								WebSocketIdentifyData identityData{};
								identityData.botToken = this->configManager->getBotToken();
								identityData.currentShard = this->shard[0];
								identityData.numberOfShards = this->shard[1];
								identityData.intents = static_cast<Int64>(this->configManager->getGatewayIntents());
								identityData.presence = this->configManager->getPresenceData();
								String theString = this->stringifyJsonData(identityData, this->dataOpCode);
								if (!this->sendMessage(theString, true)) {
									returnValue = true;
								}
								this->currentState.store(WebSocketState::Sending_Identify);
							}
							break;
						}
						case 11: {
							this->haveWeReceivedHeartbeatAck = true;
							break;
						}
						default: {
							break;
						}
					}
					return returnValue;
				} catch (...) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
						cout << "The payload: " << payload << std::endl;
					}
					this->currentMessage.clear();
					this->inputBuffer.clear();
					this->messageLength = 0;
					this->messageOffset = 0;
					return false;
				}
			}
		}
		this->currentMessage.clear();
		this->inputBuffer.clear();
		this->messageLength = 0;
		this->messageOffset = 0;
		return false;
	}

	Void WebSocketCore::checkForAndSendHeartBeat(Bool isImmediate) noexcept {
		try {
			if ((this->currentState.load() == WebSocketState::Authenticated && this->heartBeatStopWatch.hasTimePassed() && this->haveWeReceivedHeartbeatAck) || isImmediate) {
				String theString{};
				if (this->typeOfWebSocket != "Voice WebSocket") {
					DiscordCoreAPI::JsonObject heartbeat{};
					heartbeat["d"] = this->lastNumberReceived;
					heartbeat["op"] = 1;
					theString = this->stringifyJsonData(std::move(heartbeat), this->dataOpCode);
				} else {
					DiscordCoreAPI::JsonObject theData{};
					theData["d"] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
					theData["op"] = 3;
					String theNewString{ theData };
					theString = this->stringifyJsonData(std::move(theData), this->dataOpCode);
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

	Void WebSocketCore::handleBuffer() noexcept {
		if (this->currentState.load() == WebSocketState::Upgrading) {
			this->parseConnectionHeaders();
		}
		this->parseMessage();
	}

	Void WebSocketSSLShard::disconnect() noexcept {
		if (this->theSocket != SOCKET_ERROR) {
			this->theSocket = SOCKET_ERROR;
			this->ssl = nullptr;
			this->currentState.store(WebSocketState::Disconnected);
			this->areWeConnecting.store(true);
			this->outputBuffer.clear();
			this->inputBuffer.clear();
			this->closeCode = 0;
			this->areWeHeartBeating = false;
			this->theConnections = std::make_unique<DiscordCoreAPI::ConnectionPackage>();
			this->theConnections->currentReconnectTries = this->currentReconnectTries;
			this->theConnections->areWeResuming = this->areWeResuming;
			this->theConnections->currentShard = this->shard[0];
		}
	}

	Void WebSocketSSLShard::onClosed() noexcept {
		if (this->maxReconnectTries > this->currentReconnectTries) {
			this->disconnect();
		} else {
			if (this->doWeQuit) {
				this->doWeQuit->store(true);
			}
		}
	}

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, AtomicBool* doWeQuitNew, Int32 currentBaseSocketAgentNew) noexcept {
		this->configManager = &discordCoreClientNew->configManager;
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->discordCoreClient = discordCoreClientNew;
		this->doWeQuit = doWeQuitNew;
		this->taskThread = std::make_unique<std::jthread>([this](std::stop_token stopToken) {
			this->run(stopToken);
		});
	}

	Void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage thePackageNew) noexcept {
		try {
			if (thePackageNew.currentShard != -1) {
				if (!this->theShardMap.contains(thePackageNew.currentShard)) {
					this->theShardMap[thePackageNew.currentShard] = std::make_unique<WebSocketSSLShard>(this->discordCoreClient, thePackageNew.currentShard, this->doWeQuit);
				}
				this->theShardMap[thePackageNew.currentShard]->currentReconnectTries = thePackageNew.currentReconnectTries;
				this->theShardMap[thePackageNew.currentShard]->currentReconnectTries++;
				String connectionUrl = thePackageNew.areWeResuming ? this->theShardMap[thePackageNew.currentShard]->resumeUrl : this->configManager->getConnectionAddress();
				Bool isItFirstIteraion{ true };
				Bool didWeConnect{};
				DiscordCoreAPI::StopWatch theStopWatch{ 5s };
				do {
					if (theStopWatch.hasTimePassed()) {
						theShardMap[thePackageNew.currentShard]->onClosed();
						break;
					}
					if (!isItFirstIteraion) {
						std::this_thread::sleep_for(5s);
					}
					if (this->configManager->doWePrintGeneralSuccessMessages()) {
						cout << DiscordCoreAPI::shiftToBrightBlue() << "Connecting Shard " + std::to_string(thePackageNew.currentShard + 1) << " of "
							 << this->configManager->getShardCountForThisProcess()
							 << String(" Shards for this process. (") + std::to_string(thePackageNew.currentShard + 1) + " of " +
								std::to_string(this->configManager->getTotalShardCount()) + String(" Shards total across all processes)")
							 << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					isItFirstIteraion = false;
					didWeConnect = this->theShardMap[thePackageNew.currentShard]->connect(connectionUrl, this->configManager->getConnectionPort(),
						this->configManager->doWePrintWebSocketErrorMessages(), false);
					if (didWeConnect == false) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection failed for WebSocket [" << thePackageNew.currentShard << ","
								 << this->configManager->getTotalShardCount() << "]"
								 << " reconnecting in 5 seconds." << DiscordCoreAPI::reset() << endl
								 << endl;
						}
					}
				} while (didWeConnect != true && !this->doWeQuit->load());

				this->theShardMap[thePackageNew.currentShard]->currentState.store(WebSocketState::Upgrading);
				String sendString{};
				sendString = "GET /?v=10&encoding=";
				sendString += this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf ? "etf" : "json";
				sendString += " HTTP/1.1\r\nHost: " + this->configManager->getConnectionAddress() +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
					DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				ProcessIOResult didWeWrite{ false };
				do {
					if (theStopWatch.hasTimePassed()) {
						this->theShardMap[thePackageNew.currentShard]->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
					didWeWrite = this->theShardMap[thePackageNew.currentShard]->writeData(sendString, true);
				} while (didWeWrite != ProcessIOResult::No_Error);
				if (didWeWrite != ProcessIOResult::No_Error) {
					this->theShardMap[thePackageNew.currentShard]->onClosed();
					return;
				}

				while (!this->doWeQuit->load()) {
					if (this->theShardMap[thePackageNew.currentShard]->currentState.load() == WebSocketState::Collecting_Hello) {
						break;
					}
					auto theResult = this->theShardMap[thePackageNew.currentShard]->processIO(10);
					if (theResult != ProcessIOResult::No_Error) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" + thePackageNew.currentShard << ","
								 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
								 << endl;
							this->theShardMap[thePackageNew.currentShard]->onClosed();
						}
						break;
					}
					if (theStopWatch.hasTimePassed()) {
						this->theShardMap[thePackageNew.currentShard]->onClosed();
						return;
					}
					if (this->theShardMap[thePackageNew.currentShard]->areWeStillConnected()) {
						while (this->theShardMap[thePackageNew.currentShard]->currentState.load() == WebSocketState::Upgrading) {
							if (theStopWatch.hasTimePassed()) {
								this->theShardMap[thePackageNew.currentShard]->onClosed();
								return;
							}
							auto theResult = this->theShardMap[thePackageNew.currentShard]->processIO(10);
							if (theResult != ProcessIOResult::No_Error) {
								if (this->configManager->doWePrintWebSocketErrorMessages()) {
									cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" + thePackageNew.currentShard << ","
										 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
										 << endl;
									this->theShardMap[thePackageNew.currentShard]->onClosed();
								}
								break;
							}
							std::this_thread::sleep_for(1ms);
						}
					}
					if (this->theShardMap[thePackageNew.currentShard]->areWeStillConnected()) {
						this->theShardMap[thePackageNew.currentShard]->parseMessage();
					}
					std::this_thread::sleep_for(1ms);
				}
				this->theShardMap[thePackageNew.currentShard]->areWeConnecting.store(false);
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::connectInternal()");
			}
		}
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->taskThread.get();
	}

	Void BaseSocketAgent::run(std::stop_token stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !this->doWeQuit->load()) {
				auto theResult = SSLClient::processIO(this->theShardMap);
				for (auto& value: theResult) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" << static_cast<WebSocketSSLShard*>(value)->shard[0] << ","
							 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
							 << endl;
					}
				}
				Bool areWeConnected{ false };
				for (auto& [key, value]: this->theShardMap) {
					if (value->areWeStillConnected()) {
						static_cast<WebSocketSSLShard*>(value.get())->checkForAndSendHeartBeat();
						areWeConnected = true;
					}
					if (value->theConnections) {
						auto theConnectionData = *value->theConnections;
						value->theConnections.reset(nullptr);
						this->connect(theConnectionData);
					}
				}
				if (!areWeConnected) {
					std::this_thread::sleep_for(1ms);
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			}
		}
	}

	BaseSocketAgent::~BaseSocketAgent() {
		if (this->taskThread) {
			this->taskThread->request_stop();
			if (this->taskThread->joinable()) {
				this->taskThread->join();
			}
		}
	}
}