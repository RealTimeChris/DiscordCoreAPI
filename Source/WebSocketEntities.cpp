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

namespace DiscordCoreAPI {
	namespace Globals {
		extern VoiceConnectionMap voiceConnectionMap;
	}
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

	WebSocketMessageHandler::WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManagerNew) {
		this->configManager = configManagerNew;
	}

	std::string WebSocketMessageHandler::stringifyJsonData(DiscordCoreAPI::JsonSerializer& dataToSend, WebSocketOpCode theOpCode) noexcept {
		std::string theVector{};
		std::string header{};

		if (this->configManager->doWePrintWebSocketSuccessMessages()) {
			cout << DiscordCoreAPI::shiftToBrightBlue()
				 << "Sending WebSocket [" + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[0]) + "," +
					std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[1]) + "]" + std::string("'s Message: ")
				 << static_cast<std::string>(dataToSend.getString()) << endl
				 << endl
				 << DiscordCoreAPI::reset();
		}
		if (theOpCode == WebSocketOpCode::Op_Binary) {
			theVector = ErlPacker::parseJsonToEtf(dataToSend.getString());
		} else {
			theVector = dataToSend.getString();
		}
		this->createHeader(header, theVector.size(), theOpCode);
		std::string theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		return theVectorNew;
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
		if (theShard->areWeStillConnected() && theShard->currentState.load() == SSLShardState::Upgrading && theShard->inputBuffer.getUsedSpace() > 100) {
			std::string_view theString = theShard->getInputBuffer(0, theShard->inputBuffer.getUsedSpace());
			
			auto theFindValue = theString.find("\r\n\r\n");
			if (theFindValue != std::string::npos) {
				theShard->getInputBuffer(0, theShard->inputBuffer.getUsedSpace());
				theShard->currentState.store(SSLShardState::Collecting_Hello);
				return true;
			}
		}
		return false;
	}

	bool WebSocketMessageHandler::parseMessage(WebSocketSSLShard* theShard) noexcept {
		if (theShard->inputBuffer.getUsedSpace() < 4) {
			return false;
		}
		theShard->dataOpCode = static_cast<WebSocketOpCode>(theShard->inputBuffer.getCurrentTail()[0] & ~webSocketFinishBit);
		this->messageLength = 0;
		this->messageOffset = 0;
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
				uint8_t length01 = theShard->inputBuffer.getCurrentTail()[1];
				theShard->messageOffset = 2;
				if (length01 & webSocketMaskBit) {
					return false;
				}
				theShard->messageLength = length01;
				if (length01 == webSocketPayloadLengthMagicLarge) {
					if (theShard->inputBuffer.getUsedSpace() < 8) {
						return false;
					}
					uint8_t length03 = theShard->inputBuffer.getCurrentTail()[2];
					uint8_t length04 = theShard->inputBuffer.getCurrentTail()[3];
					theShard->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
					theShard->messageOffset += 2;
				} else if (length01 == webSocketPayloadLengthMagicHuge) {
					if (theShard->inputBuffer.getUsedSpace() < 10) {
						return false;
					}
					theShard->messageLength = 0;
					for (uint64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
						uint8_t lengthNew = static_cast<uint8_t>(theShard->inputBuffer.getCurrentTail()[x]);
						theShard->messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
					}
					theShard->messageOffset += 8;
				}
				if (theShard->inputBuffer.getUsedSpace() < theShard->messageOffset + theShard->messageLength) {
					return false;
				} else {
					this->onMessageReceived(theShard->getInputBuffer(theShard->messageOffset, theShard->messageLength));
					return true;
				}
			}
			case WebSocketOpCode::Op_Close: {
				uint16_t close = theShard->inputBuffer.getCurrentTail()[2] & 0xff;
				close <<= 8;
				close |= theShard->inputBuffer.getCurrentTail()[3] & 0xff;
				theShard->closeCode = close;
				if (theShard->closeCode) {
					theShard->areWeResuming = true;
				}
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed()
						 << "WebSocket [" + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[0]) + "," + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[1]) +
							"]" + " Closed; Code: "
						 << +static_cast<uint16_t>(theShard->closeCode) << DiscordCoreAPI::reset() << endl
						 << endl;
				}
				this->onClosed();
				return false;
			}
		}
		return false;
	}

	WebSocketSSLShard::WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* theClient, std::deque<DiscordCoreAPI::ConnectionPackage>* theConnectionsNew, int32_t currentShardNew,
		std::atomic_bool* doWeQuitNew) noexcept
		: WebSocketMessageHandler(&theClient->configManager) {
		this->configManager = &theClient->configManager;
		this->shard[0] = currentShardNew;
		this->theConnections = theConnectionsNew;
		this->theParser.allocate(1024ull * 1024ull);
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

	void WebSocketSSLShard::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept {
		if (this->currentState.load() == SSLShardState::Authenticated) {
			try {
				int32_t theCurrentIndex = this->shard[0];
				DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
				dataPackage.channelId = 0;
				dataPackage.guildId = doWeCollect.guildId;
				dataPackage.selfDeaf = doWeCollect.selfDeaf;
				dataPackage.selfMute = doWeCollect.selfMute;
				this->userId = doWeCollect.userId;
				DiscordCoreAPI::JsonSerializer newData = dataPackage;
				std::string theString = this->stringifyJsonData(newData, this->dataOpCode);
				bool didWeWrite{ false };
				if (!this->sendMessage(theString, true)) {
					return;
				}
				if (doWeCollect.channelId == 0) {
					return;
				}
				dataPackage.channelId = doWeCollect.channelId;
				newData = dataPackage;
				std::string theString02 = this->stringifyJsonData(newData, this->dataOpCode);
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
		if (this->areWeStillConnected()) {
			try {
				if (dataToSend.size() == 0) {
					return false;
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
	std::atomic_int32_t theInt{};
	bool WebSocketSSLShard::onMessageReceived(std::string_view theDataNew) noexcept {
		if (this->discordCoreClient) {
			std::string theString{};
			std::string& payload{ theString };
			if (this->areWeStillConnected()) {
				try {
					bool returnValue{ false };
					
					simdjson::ondemand::document_stream::iterator::value_type theDocument{};
					if (theDataNew.size() > 0) {
						returnValue = true;

						if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
							try {
								theStopWatchReal.resetTimer();
								payload = ErlPacker::parseEtfToJson(theDataNew);
								
								payload.reserve(payload.size() + simdjson::SIMDJSON_PADDING);
								theDocument = this->theParser.iterate(simdjson::padded_string_view(payload.data(), payload.length(), payload.capacity()));
							} catch (...) {
								if (this->configManager->doWePrintGeneralErrorMessages()) {
									DiscordCoreAPI::reportException("ErlPacker::parseEtfToJson()");
									this->messageLength = 0;
									this->messageOffset = 0;
								}
								returnValue = false;
							}
						} else {
							std::string payloadJson = static_cast<std::string>(theDataNew);
							payloadJson.reserve(payloadJson.size() + simdjson::SIMDJSON_PADDING);
							theDocument = this->theParser.iterate(simdjson::padded_string_view(payloadJson.data(), payloadJson.length(), payloadJson.capacity()));
						}
					} else {
						returnValue = false;
					}

					auto thePayload = theDocument.value().get_value();
					WebSocketMessage theMessage{};
					uint64_t s{};
					uint64_t op{};
					std::string_view t{};
					simdjson::ondemand::value theObject{};
					if (thePayload["d"].get(theObject) != simdjson::error_code::SUCCESS) {
						throw std::runtime_error{ "Failed to collect the 'd'." };
					}
					if (thePayload["s"].get(s) == simdjson::error_code::SUCCESS) {
						theMessage.s = s;
					}
					if (thePayload["t"].get(t) == simdjson::error_code::SUCCESS) {
						theMessage.t = t;
					}
					if (thePayload["op"].get(op) == simdjson::error_code::SUCCESS) {
						theMessage.op = op;
					}

					if (theMessage.s != 0) {
						this->lastNumberReceived = theMessage.s;
					}

					if (this->configManager->doWePrintWebSocketSuccessMessages()) {
						cout << DiscordCoreAPI::shiftToBrightGreen()
							 << "Message received from WebSocket [" + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[0]) + "," +
								std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[1]) + "]" + std::string(": ")
							 << payload << DiscordCoreAPI::reset() << endl
							 << endl;
					}

					if (theMessage.op != -1) {
						switch (theMessage.op) {
							case 0: {
								if (theMessage.t != "") {
										
									switch (EventConverter{ theMessage.t }) {
											
										case 1: { 
											ReadyData theData{};
											simdjson::ondemand::value theObjectNew = theObject;
											parseObject(theObjectNew, theData);
											this->currentState.store(SSLShardState::Authenticated);
											this->sessionId = theData.sessionId;
											std::string theResumeUrl = theData.resumeGatewayUrl;
											theResumeUrl = theResumeUrl.substr(theResumeUrl.find("wss://") + std::string{ "wss://" }.size());
											theResumeUrl = theResumeUrl.substr(0, theResumeUrl.find("/"));
											this->resumeUrl = theResumeUrl;
											DiscordCoreAPI::UserData theUser{ theData.user };
											this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ theUser,
												this->discordCoreClient
													->baseSocketAgentMap[static_cast<int32_t>(floor(
														static_cast<int32_t>(this->shard[0]) % static_cast<int32_t>(this->discordCoreClient->configManager.getTotalShardCount())))]
													.get() };
											DiscordCoreAPI::Users::insertUser(std::move(theUser));
											this->currentReconnectTries = 0;
											break;
											} 
										case 2: {
											this->currentState.store(SSLShardState::Authenticated);
											this->currentReconnectTries = 0;
											break;
										}											
										case 3: {
											std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->permissionData);
											this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
											break;
										}
										case 4: {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->theRule);
											this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
											break;
										}
										case 5: {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->theRule);
											this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
											break;
										}
										case 6: {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->theRule);
											this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
											break;
										}
										case 7: {
											std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->theData);
											this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
											break;
										}
										case 8: {
											if (DiscordCoreAPI::Channels::doWeCacheChannels ||
												this->discordCoreClient->eventManager.onChannelCreationEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::ChannelData* theChannelPtr{ nullptr };
												DiscordCoreAPI::ChannelData theChannel{};
												DiscordCoreAPI::parseObject(theObject, theChannel);
												Snowflake channelId{ theChannel.id };
												Snowflake guildId{ theChannel.guildId };
												DiscordCoreAPI::GuildData theGuild{};
												theGuild.id = guildId;
												if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
													DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
													guild->channels.emplace_back(channelId);
												}
												if (DiscordCoreAPI::Channels::doWeCacheChannels) {
													DiscordCoreAPI::Channels::insertChannel(theChannel);
													theChannelPtr = &DiscordCoreAPI::Channels::cache.at(theChannel);
												} else {
													theChannelPtr = &theChannel;
												}
												if (this->discordCoreClient->eventManager.onChannelCreationEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnChannelCreationData dataPackage{ std::make_unique<DiscordCoreAPI::ChannelData>(*theChannelPtr) };
													this->discordCoreClient->eventManager.onChannelCreationEvent(dataPackage);
												}
											}
											break;
										}
										case 9: {
											if (DiscordCoreAPI::Channels::doWeCacheChannels || this->discordCoreClient->eventManager.onChannelUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::ChannelData* theChannelPtr{ nullptr };
												DiscordCoreAPI::ChannelData theChannel{};
												DiscordCoreAPI::parseObject(theObject, theChannel);
												Snowflake channelId{ theChannel.id };
												Snowflake guildId{ theChannel.guildId };
												if (DiscordCoreAPI::Channels::doWeCacheChannels) {
													DiscordCoreAPI::Channels::insertChannel(theChannel);
													theChannelPtr = &DiscordCoreAPI::Channels::cache.at(theChannel);
												} else {
													theChannelPtr = &theChannel;
												}
												if (this->discordCoreClient->eventManager.onChannelUpdateEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnChannelUpdateData dataPackage{ std::make_unique<DiscordCoreAPI::ChannelData>(*theChannelPtr) };
													this->discordCoreClient->eventManager.onChannelUpdateEvent(dataPackage);
												}
											}
											break;
										}
										case 10: {
											if (DiscordCoreAPI::Channels::doWeCacheChannels ||
												this->discordCoreClient->eventManager.onChannelDeletionEvent.theFunctions.size() > 0) {
												std::unique_ptr<DiscordCoreAPI::ChannelData> theChannel = std::make_unique<DiscordCoreAPI::ChannelData>();
												DiscordCoreAPI::parseObject(theObject, *theChannel);
												Snowflake channelId{ theChannel->id };
												Snowflake guildId{ theChannel->guildId };
												DiscordCoreAPI::GuildData theGuild{};
												theGuild.id = guildId;
												if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
													DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
													for (uint64_t x = 0; x < guild->channels.size(); ++x) {
														if (guild->channels[x] == channelId) {
															guild->channels.erase(guild->channels.begin() + x);
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
											std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->dataPackage);
											this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
											break;
										}
										case 12: {
											std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->thread);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->thread.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
												guild->threads.emplace_back(dataPackage->thread.id);
											}
											this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
											break;
										}
										case 13: {
											std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->thread);
											this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
											break;
										}
										case 14: {
											std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->thread);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->thread.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
												for (uint64_t x = 0; x < guild->threads.size(); ++x) {
													if (guild->threads[x] == dataPackage->thread.id) {
														guild->threads.erase(guild->threads.begin() + x);
													}
												}
											}
											this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
											break;
										}
										case 15: {
											std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->threadListSyncData);
											this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
											break;
										}
										case 16: {
											std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->threadMember);
											this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
											break;
										}
										case 17: {
											std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->threadMembersUpdateData);
											this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
											break;
										}
										case 18: {
											if (theInt.load() % 100 == 0 || theStopWatch.totalTimePassed() % 10 == 0) {
												std::cout << "THE GUILD COUNT: " << theInt.load() << ", TOTAL TIME: " << theStopWatch.totalTimePassed() << std::endl;
											}
											theInt.store(theInt.load() + 1);
											std::unique_ptr<DiscordCoreAPI::GuildData> theGuildPtr{ std::make_unique<DiscordCoreAPI::GuildData>() };
											Snowflake guildId{};
											theStopWatchReal.resetTimer();
											DiscordCoreAPI::parseObject(theObject, *theGuildPtr);
											guildId = theGuildPtr->id;
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildCreationEvent.theFunctions.size() > 0) {
												
												if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
													DiscordCoreAPI::Guilds::insertGuild(*theGuildPtr);
												}
												if (this->discordCoreClient->eventManager.onGuildCreationEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnGuildCreationData dataPackage{ std::make_unique<DiscordCoreAPI::GuildData>(*theGuildPtr),
														this->discordCoreClient };
													this->discordCoreClient->eventManager.onGuildCreationEvent(dataPackage);
												}
											}
											break;
										} 
										case 19: {
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildUpdateEvent.theFunctions.size() > 0) {
												std::unique_ptr<DiscordCoreAPI::GuildData> theGuildPtr{ std::make_unique<DiscordCoreAPI::GuildData>() };
												Snowflake guildId{};
												DiscordCoreAPI::parseObject(theObject, *theGuildPtr);
												if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
													DiscordCoreAPI::Guilds::insertGuild(*theGuildPtr);
												}
												if (this->discordCoreClient->eventManager.onGuildUpdateEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnGuildUpdateData dataPackage{ std::make_unique<DiscordCoreAPI::GuildData>(*theGuildPtr),
														this->discordCoreClient };
													this->discordCoreClient->eventManager.onGuildUpdateEvent(dataPackage);
												}
											}
											break;
										}
										case 20: {
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildDeletionEvent.theFunctions.size() > 0) {
												std::unique_ptr<DiscordCoreAPI::GuildData> theGuild = std::make_unique<DiscordCoreAPI::GuildData>();
												Snowflake guildId{};
												DiscordCoreAPI::parseObject(theObject, *theGuild);
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
											std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											DiscordCoreAPI::parseObject(theObject, dataPackage->user);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
											break;
										}
										case 22: {
											std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											DiscordCoreAPI::parseObject(theObject, dataPackage->user);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
											break;
										}
										case 23: {
											std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->updateData);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->updateData.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::Guilds::cache.at(theGuild).emoji.clear();
												for (auto& value: dataPackage->updateData.emojis) {
													DiscordCoreAPI::Guilds::cache.at(theGuild).emoji.emplace_back(value.id);
												}
											}
											this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
											break;
										}
										case 24: {
											std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->updateData);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->updateData.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::Guilds::cache.at(theGuild).stickers.clear();
												for (auto& value: dataPackage->updateData.stickers) {
													DiscordCoreAPI::Guilds::cache.at(theGuild).stickers.emplace_back(value.id);
												}
											}
											this->discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
											break;
										}
										case 25: {
											std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>()
											};
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
											break;
										}
										case 26: {
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
												this->discordCoreClient->eventManager.onGuildMemberAddEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::GuildMemberData* theGuildMemberPtr{ nullptr };
												DiscordCoreAPI::GuildMemberData theGuildMember{};
												DiscordCoreAPI::parseObject(theObject, theGuildMember);
												Snowflake userId{theGuildMember.id};
												Snowflake guildId{ theGuildMember.guildId };
												if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
													DiscordCoreAPI::GuildMembers::insertGuildMember(std::move(theGuildMember));
													DiscordCoreAPI::GuildMemberData theDataNew{};
													theDataNew.id = userId;
													theDataNew.guildId = guildId;
													DiscordCoreAPI::GuildData theGuild{};
													theGuild.id = guildId;
													if (DiscordCoreAPI::GuildMembers::cache.contains(theDataNew)) {
														theGuildMemberPtr = &DiscordCoreAPI::GuildMembers::cache.at(theDataNew);
													}
													if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
														DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
														guild->memberCount++;
													}
												} else {
													theGuildMemberPtr = &theGuildMember;
												}
												if (this->discordCoreClient->eventManager.onGuildMemberAddEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnGuildMemberAddData dataPackage{ std::make_unique<DiscordCoreAPI::GuildMemberData>(*theGuildMemberPtr),
														this->discordCoreClient };
													this->discordCoreClient->eventManager.onGuildMemberAddEvent(dataPackage);
												}
											}
											break;
										}
										case 27: {
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
												this->discordCoreClient->eventManager.onGuildMemberRemoveEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::GuildMemberData theGuildMember{};
												DiscordCoreAPI::parseObject(theObject, theGuildMember);
												Snowflake userId{ theGuildMember.id };
												Snowflake guildId{ theGuildMember.guildId };
												DiscordCoreAPI::GuildData theGuild{};
												theGuild.id = guildId;
												if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
													DiscordCoreAPI::GuildMembers::removeGuildMember(theGuildMember);
													if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
														DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
														for (uint64_t x = 0; x < guild->members.size(); ++x) {
															if (guild->members[x] == userId) {
																guild->memberCount--;
																guild->members.erase(guild->members.begin() + x);
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
												DiscordCoreAPI::GuildMemberData* theGuildMemberPtr{ nullptr };
												DiscordCoreAPI::GuildMemberData theGuildMember{};
												DiscordCoreAPI::parseObject(theObject, theGuildMember);
												Snowflake userId{ theGuildMember.id };
												Snowflake guildId{ theGuildMember.guildId };
												if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
													DiscordCoreAPI::GuildMembers::insertGuildMember(std::move(theGuildMember));
													DiscordCoreAPI::GuildMemberData theDataNew{};
													theDataNew.id = userId;
													theDataNew.guildId = guildId;
													if (DiscordCoreAPI::GuildMembers::cache.contains(theDataNew)) {
														theGuildMemberPtr = &DiscordCoreAPI::GuildMembers::cache.at(theDataNew);
													}
												} else {
													theGuildMemberPtr = &theGuildMember;
												}
												if (this->discordCoreClient->eventManager.onGuildMemberUpdateEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnGuildMemberUpdateData dataPackage{ std::make_unique<DiscordCoreAPI::GuildMemberData>(*theGuildMemberPtr) };
													this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(dataPackage);
												}
											}
											break;
										}
										case 29: {
											std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->chunkEventData);
											this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
											break;
										}
										case 30: {
											if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleCreationEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::RoleData* theRolePtr{ nullptr };
												DiscordCoreAPI::RoleData theRole{};
												DiscordCoreAPI::parseObject(theObject, theRole);
												Snowflake guildId{}; 
												std::string_view theString{};
												if (theObject["guild_id"].get(theString) == simdjson::error_code::SUCCESS) {
													guildId = stoull(static_cast<std::string>(theString));
												}
												Snowflake roleId{ theRole.id };
												std::cout << "THE ROLE ID: " << roleId << ", THE GUILD ID: " << guildId << std::endl;
												DiscordCoreAPI::GuildData theGuild{};
												theGuild.id = guildId;
												if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
													DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
													guild->roles.emplace_back(roleId);
												}
												if (DiscordCoreAPI::Roles::doWeCacheRoles) {
													DiscordCoreAPI::Roles::insertRole(std::move(theRole));
													theRolePtr =
														&DiscordCoreAPI::Roles::cache.at(DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = guildId, .roleId = roleId }).get());
												} else {
													theRolePtr = &theRole;
												}
												if (this->discordCoreClient->eventManager.onRoleCreationEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnRoleCreationData dataPackage{ std::make_unique<DiscordCoreAPI::RoleData>(*theRolePtr), guildId };
													this->discordCoreClient->eventManager.onRoleCreationEvent(dataPackage);
												}
											}
											break;
										}
										case 31: {
											if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::RoleData* theRolePtr{ nullptr };
												DiscordCoreAPI::RoleData theRole{};
												DiscordCoreAPI::parseObject(theObject, theRole);
												Snowflake roleId{ theRole.id };
												Snowflake guildId{ theRole.guildId };
												if (DiscordCoreAPI::Roles::doWeCacheRoles) {
													DiscordCoreAPI::Roles::insertRole(std::move(theRole));
													theRolePtr =
														&DiscordCoreAPI::Roles::cache.at(DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = guildId, .roleId = roleId }).get());
												} else {
													theRolePtr = &theRole;
												}
												if (this->discordCoreClient->eventManager.onRoleUpdateEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnRoleUpdateData dataPackage{ std::make_unique<DiscordCoreAPI::RoleData>(*theRolePtr), guildId };
													this->discordCoreClient->eventManager.onRoleUpdateEvent(dataPackage);
												}
											}
											break;
										}
										case 32: {
											if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleDeletionEvent.theFunctions.size() > 0) {
												std::string_view theString{};
												theObject["role_id"].get(theString);
												Snowflake roleId = stoull(static_cast<std::string>(theString));
												theObject["guild_hashes"]["guild_id"].get(theString);
												Snowflake guildId = stoull(static_cast<std::string>(theString));
												
												DiscordCoreAPI::OnRoleDeletionData dataPackage{ std::make_unique<DiscordCoreAPI::RoleData>(), guildId };
												DiscordCoreAPI::RoleData theRole = DiscordCoreAPI::Roles::getCachedRoleAsync({ .guildId = guildId, .roleId = roleId }).get();
												DiscordCoreAPI::GuildData theGuild{};
												theGuild.id = guildId;
												if (DiscordCoreAPI::Roles::doWeCacheRoles) {
													DiscordCoreAPI::Roles::removeRole(roleId);
													if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
														DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
														for (uint64_t x = 0; x < guild->roles.size(); ++x) {
															if (guild->roles[x] == roleId) {
																guild->roles.erase(guild->roles.begin() + x);
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
											std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventCreationData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnGuildScheduledEventCreationData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->guildScheduledEvent);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->guildScheduledEvent.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
												guild->guildScheduledEvents.emplace_back(dataPackage->guildScheduledEvent.id);
											}
											this->discordCoreClient->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
										}
										case 34: {
											std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUpdateData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUpdateData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->guildScheduledEvent);
											this->discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
										}
										case 35: {
											std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventDeletionData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnGuildScheduledEventDeletionData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->guildScheduledEvent);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->guildScheduledEvent.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
												for (uint64_t x = 0; x < guild->guildScheduledEvents.size(); ++x) {
													if (guild->guildScheduledEvents[x] == dataPackage->guildScheduledEvent.id) {
														guild->guildScheduledEvents.erase(guild->guildScheduledEvents.begin() + x);
													}
												}
											}
											this->discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
										}
										case 36: {
											std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserAddData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserAddData>()
											};
											std::string_view theString{};
											theObject["user_id"].get(theString);
											dataPackage->userId= DiscordCoreAPI::strtoull(theString.data());
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["guild_scheduled_event_id"].get(theString);
											dataPackage->guildScheduledEventId = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
										}
										case 37: {
											std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData>()
											};
											std::string_view theString{};
											theObject["user_id"].get(theString);
											dataPackage->userId = DiscordCoreAPI::strtoull(theString.data());
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["guild_scheduled_event_id"].get(theString);
											dataPackage->guildScheduledEventId = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
										}
										case 38: {
											std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											simdjson::ondemand::value theObjectNew = theObject["integration"].value();
											DiscordCoreAPI::parseObject(theObjectNew, dataPackage->integrationData);
											this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
											break;
										}
										case 39: {
											std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											simdjson::ondemand::value theObjectNew = theObject["integration"].value();
											DiscordCoreAPI::parseObject(theObjectNew, dataPackage->integrationData);
											this->discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
											break;
										}
										case 40: {
											std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["application_id"].get(theString);
											dataPackage->applicationId = DiscordCoreAPI::strtoull(theString.data());
											theObject["id"].get(theString);
											dataPackage->id = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
											break;
										}
										case 41: {
											std::unique_ptr<DiscordCoreAPI::InteractionData> interactionData{ std::make_unique<DiscordCoreAPI::InteractionData>() };
											DiscordCoreAPI::parseObject(theObject, *interactionData);
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
													break;
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
															break;
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
															break;
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
													break;
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
													break;
												}
											}
											break;
										} 
										case 42: {
											std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->invite);
											this->discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
											break;
										}
										case 43: {
											std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteDeletionData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["channel_id"].get(theString);
											dataPackage->channelId = DiscordCoreAPI::strtoull(theString.data());
											theObject["code"].get(theString);
											dataPackage->code = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
											break;
										}
										case 44: {
											std::unique_ptr<DiscordCoreAPI::Message> message{ std::make_unique<DiscordCoreAPI::Message>() };
											DiscordCoreAPI::parseObject(theObject, *message);
											std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageCreationData>() };
											dataPackage->message = *message;
											for (auto& [key, value]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBufferMap) {
												std::cout << "SENDING THE MESSAGE TO: " << key << std::endl;
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
											break;
										}
										case 45: {
											std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->messageNew);
											for (auto& [key, value]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBufferMap) {
												std::cout << "SENDING THE UPDATE MESSAGE TO: " << key << std::endl;
												value->send(dataPackage->messageNew);
											}
											this->discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
											break;
										}
										case 46: {
											std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeletionData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["channel_id"].get(theString);
											dataPackage->channelId = DiscordCoreAPI::strtoull(theString.data());
											theObject["id"].get(theString);
											dataPackage->messageId = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
											break;
										}
										case 47: {
											std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["channel_id"].get(theString);
											dataPackage->channelId = DiscordCoreAPI::strtoull(theString.data());
											simdjson::ondemand::array theArray{};
											theObject["ids"].get(theArray);
											for (auto value : theArray) {
												dataPackage->ids.emplace_back(DiscordCoreAPI::strtoull(value.get_string().take_value().data()));
											}
											this->discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
											break;
										}
										case 48: {
											std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->reaction);
											this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
											break;
										}
										case 49: {
											std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->reactionRemoveData);
											this->discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
											break;
										}
										case 50: {
											std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["channel_id"].get(theString);
											dataPackage->channelId = DiscordCoreAPI::strtoull(theString.data());
											theObject["message_id"].get(theString);
											dataPackage->messageId = DiscordCoreAPI::strtoull(theString.data());
											this->discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
											break;
										}
										case 51: {
											std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["channel_id"].get(theString);
											dataPackage->channelId = DiscordCoreAPI::strtoull(theString.data());
											theObject["message_id"].get(theString);
											dataPackage->messageId = DiscordCoreAPI::strtoull(theString.data());
											simdjson::ondemand::value theObjectNew = theObject["emoji"].value();
											DiscordCoreAPI::parseObject(theObject, dataPackage->emoji);
											this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
											break;
										}
										case 52: {
											std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->presenceData);
											this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
											break;
										} 
										case 53: {
											std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->stageInstance);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->stageInstance.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
												guild->stageInstances.emplace_back(dataPackage->stageInstance.id);
											}
											this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
											break;
										}
										case 54: {
											std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->stageInstance);
											this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
											break;
										}
										case 55: {
											std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{
												std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>()
											};
											DiscordCoreAPI::parseObject(theObject, dataPackage->stageInstance);
											DiscordCoreAPI::GuildData theGuild{};
											theGuild.id = dataPackage->stageInstance.guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(theGuild)) {
												DiscordCoreAPI::GuildData* guild = &DiscordCoreAPI::Guilds::cache.at(theGuild);
												for (uint64_t x = 0; x < guild->stageInstances.size(); ++x) {
													if (guild->stageInstances[x] == dataPackage->stageInstance.id) {
														guild->stageInstances.erase(guild->stageInstances.begin() + x);
													}
												}
											}
											this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
											break;
										}
										case 56: {
											std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->typingStartData);
											this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
											break;
										}
										case 57: {
											DiscordCoreAPI::UserData* theUserPtr{ nullptr };
											if (DiscordCoreAPI::Users::doWeCacheUsers || this->discordCoreClient->eventManager.onUserUpdateEvent.theFunctions.size() > 0) {
												DiscordCoreAPI::UserData theUser{};
												DiscordCoreAPI::parseObject(theObject, theUser);
												Snowflake userId{ theUser.id };
												if (DiscordCoreAPI::Users::doWeCacheUsers) {
													DiscordCoreAPI::Users::insertUser(theUser);
													theUserPtr = &DiscordCoreAPI::Users::cache.at(theUser);
												} else {
													theUserPtr = &theUser;
												}
												if (this->discordCoreClient->eventManager.onUserUpdateEvent.theFunctions.size() > 0) {
													DiscordCoreAPI::OnUserUpdateData dataPackage{ std::make_unique<DiscordCoreAPI::UserData>(*theUserPtr) };
													this->discordCoreClient->eventManager.onUserUpdateEvent(dataPackage);
												}
											}
											break;
										}
										case 58: {
											std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, dataPackage->voiceStateData);
											this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
											if (this->areWeCollectingData && !this->stateUpdateCollected && !this->serverUpdateCollected &&
												dataPackage->voiceStateData.userId == this->userId) {
												std::cout << "WERE COLLECTING VOICE STATE UPDATE DATA!" << std::endl;
												this->voiceConnectionData = VoiceConnectionData{};
												this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
												this->stateUpdateCollected = true;
											} else if (this->areWeCollectingData && !this->stateUpdateCollected) {
												std::cout << "WERE COLLECTING VOICE STATE UPDATE DATA!" << std::endl;
												this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
												if (this->voiceConnectionDataBufferMap.contains(dataPackage->voiceStateData.guildId)) {
													this->voiceConnectionDataBufferMap[dataPackage->voiceStateData.guildId]->send(this->voiceConnectionData);
												}
											}
											if (this->discordCoreClient->configManager.doWeCacheUsers() && this->discordCoreClient->configManager.doWeCacheGuilds()) {
												DiscordCoreAPI::GuildMemberData theData{};
												theData.id = dataPackage->voiceStateData.userId;
												theData.guildId = dataPackage->voiceStateData.guildId;
												if (DiscordCoreAPI::GuildMembers::cache.contains(theData)) {
													DiscordCoreAPI::GuildMembers::cache.at(theData).voiceChannelId = dataPackage->voiceStateData.channelId;
												}
											}

											this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
											break;
										}
										case 59: {
											std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>() };
											DiscordCoreAPI::parseObject(theObject, *dataPackage);
											this->voiceConnectionData.endPoint = dataPackage->endpoint;
											this->voiceConnectionData.token = dataPackage->token;
											if (this->areWeCollectingData && !this->serverUpdateCollected && !this->stateUpdateCollected) {
												std::cout << "WERE COLLECTING VOICE SERVER UPDATE DATA!" << std::endl;
												this->voiceConnectionData = VoiceConnectionData{};
												this->serverUpdateCollected = true;
											} else if (this->areWeCollectingData && !this->serverUpdateCollected) {
												std::cout << "WERE COLLECTING VOICE SERVER UPDATE DATA!" << std::endl;
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
											std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
											std::string_view theString{};
											theObject["guild_id"].get(theString);
											dataPackage->guildId = DiscordCoreAPI::strtoull(theString.data());
											theObject["channel_id"].get(theString);
											dataPackage->channelId = DiscordCoreAPI::strtoull(theString.data());
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
										 << "Shard [" + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[0]) + "," +
											std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[1]) + "]" + " Reconnecting (Type 7)!"
										 << DiscordCoreAPI::reset() << endl
										 << endl;
								}
								this->areWeResuming = true;
								this->onClosed();
								returnValue = true;
								break;
							}
							case 9: {
								InvalidSessionData theData{};
								parseObject(theObject, theData);
								if (this->configManager->doWePrintWebSocketErrorMessages()) {
									cout << DiscordCoreAPI::shiftToBrightBlue()
										 << "Shard [" + std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[0]) + "," +
											std::to_string(static_cast<WebSocketSSLShard*>(this)->shard[1]) + "]" + " Reconnecting (Type 9)!"
										 << DiscordCoreAPI::reset() << endl
										 << endl;
								}
								std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
								int32_t numOfMsToWait =
									static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
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
								HelloData theData{};
								parseObject(theObject, theData);
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
									DiscordCoreAPI::JsonSerializer resumePayload = resumeData;
									std::string theString = this->stringifyJsonData(resumePayload, this->dataOpCode);
									if (!this->sendMessage(theString, true)) {
										returnValue = true;
									}
									this->currentState.store(SSLShardState::Sending_Identify);
								} else {
									WebSocketIdentifyData identityData{};
									identityData.botToken = this->configManager->getBotToken();
									identityData.currentShard = this->shard[0];
									identityData.numberOfShards = this->shard[1];
									identityData.intents = static_cast<int64_t>(this->configManager->getGatewayIntents());
									identityData.presence = this->configManager->getPresenceData();
									DiscordCoreAPI::JsonSerializer identityJson = identityData;
									std::string theString = this->stringifyJsonData(identityJson, this->dataOpCode);
									if (!this->sendMessage(theString, true)) {
										returnValue = true;
									}
									this->currentState.store(SSLShardState::Sending_Identify);
								}
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
					return false;
				}
			}
		}
		return false;
	}

	void WebSocketSSLShard::checkForAndSendHeartBeat(bool isImmediate) noexcept {
		if (this->currentState.load() == SSLShardState::Authenticated) {
			try {
				if ((this->heartBeatStopWatch.hasTimePassed() && this->haveWeReceivedHeartbeatAck) || isImmediate) {
					DiscordCoreAPI::JsonSerializer heartbeat{};
					heartbeat.appendStructElement("d", this->lastNumberReceived);
					heartbeat.appendStructElement("op", 1);
					std::string theString = this->stringifyJsonData(heartbeat, this->dataOpCode);
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

	bool WebSocketSSLShard::handleBuffer(WebSocketSSLClient* theClient) noexcept {
		if (static_cast<WebSocketSSLShard*>(theClient)->currentState.load() == SSLShardState::Upgrading) {
			return this->parseConnectionHeaders(static_cast<WebSocketSSLShard*>(theClient));
		}
		return this->parseMessage(static_cast<WebSocketSSLShard*>(theClient));
	}

	void WebSocketSSLShard::disconnect(bool doWeReconnect) noexcept {
		if (this->theSocket != SOCKET_ERROR) {
			this->theSocket = SOCKET_ERROR;
			this->currentState.store(SSLShardState::Disconnected);
			this->areWeConnecting.store(true);
			this->outputBuffers.clear();
			this->inputBuffer.clear();
			this->closeCode = 0;
			this->areWeHeartBeating = false;
			if (doWeReconnect && this->theConnections) {
				DiscordCoreAPI::ConnectionPackage theData{};
				theData.currentReconnectTries = this->currentReconnectTries;
				theData.areWeResuming = this->areWeResuming;
				theData.currentShard = this->shard[0];
				this->theConnections->emplace_back(theData);
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

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocketAgentNew) noexcept {
		this->configManager = &discordCoreClientNew->configManager;
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->discordCoreClient = discordCoreClientNew;
		this->doWeQuit = doWeQuitNew;
		this->taskThread = std::make_unique<std::jthread>([this](std::stop_token stopToken) {
			this->run(stopToken);
		});
	}

	void BaseSocketAgent::connectVoiceChannel(VoiceConnectInitData theData) noexcept {
		std::unique_lock theLock{ this->theMutex };
		this->voiceConnections.emplace_back(theData);
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage thePackageNew) noexcept {
		try {
			if (thePackageNew.currentShard != -1) {
				if (!this->theShardMap.contains(thePackageNew.currentShard)) {
					this->theShardMap[thePackageNew.currentShard] =
						std::make_unique<WebSocketSSLShard>(this->discordCoreClient, &this->discordCoreClient->theConnections, thePackageNew.currentShard, this->doWeQuit);
				}
				this->theShardMap[thePackageNew.currentShard]->currentReconnectTries = thePackageNew.currentReconnectTries;
				this->theShardMap[thePackageNew.currentShard]->currentReconnectTries++;
				std::string connectionUrl = thePackageNew.areWeResuming ? this->theShardMap[thePackageNew.currentShard]->resumeUrl : this->configManager->getConnectionAddress();
				bool isItFirstIteraion{ true };
				ConnectionResult didWeConnect{};
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
							 << std::string(" Shards for this process. (") + std::to_string(thePackageNew.currentShard + 1) + " of " +
								std::to_string(this->configManager->getTotalShardCount()) + std::string(" Shards total across all processes)")
							 << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					isItFirstIteraion = false;
					didWeConnect = this->theShardMap[thePackageNew.currentShard]->connect(connectionUrl, this->configManager->getConnectionPort(),
						this->configManager->doWePrintWebSocketErrorMessages(), false);
					if (didWeConnect == ConnectionResult::Error) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection failed for WebSocket [" << thePackageNew.currentShard << ","
								 << this->configManager->getTotalShardCount() << "]"
								 << " reconnecting in 5 seconds." << DiscordCoreAPI::reset() << endl
								 << endl;
						}
					}

				} while (didWeConnect != ConnectionResult::No_Error && !this->doWeQuit->load());

				this->theShardMap[thePackageNew.currentShard]->currentState.store(SSLShardState::Upgrading);
				std::string sendString{};
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
					if (this->theShardMap[thePackageNew.currentShard]->currentState.load() == SSLShardState::Collecting_Hello) {
						break;
					}
					auto theResult = this->theShardMap[thePackageNew.currentShard]->processIO(10);
					if (theResult != ProcessIOResult::No_Error) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" + thePackageNew.currentShard << ","
								 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
								 << endl;
						}
						break;
					}
					if (this->theShardMap[thePackageNew.currentShard]->areWeStillConnected()) {
						while (this->theShardMap[thePackageNew.currentShard]->currentState.load() == SSLShardState::Upgrading) {
							if (theStopWatch.hasTimePassed()) {
								this->theShardMap[thePackageNew.currentShard]->onClosed();
								return;
							}
							this->theShardMap[thePackageNew.currentShard]->processIO(10);
							std::this_thread::sleep_for(1ms);
						}
					}
					if (this->theShardMap[thePackageNew.currentShard]->areWeStillConnected()) {
						this->theShardMap[thePackageNew.currentShard]->parseMessage(this->theShardMap[thePackageNew.currentShard].get());
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

	void BaseSocketAgent::disconnectVoiceInternal() noexcept {
		if (this->voiceConnectionsToDisconnect.size() > 0) {
			std::unique_lock theLock{ this->theMutex };
			auto theDCData = this->voiceConnectionsToDisconnect.front();
			this->voiceConnectionsToDisconnect.pop_front();
			theLock.unlock();
			DiscordCoreAPI::Globals::voiceConnectionMap[theDCData]->disconnectInternal();
		}
	}

	void BaseSocketAgent::disconnectVoice(uint64_t theDCData) noexcept {
		std::unique_lock theLock{ this->theMutex };
		this->voiceConnectionsToDisconnect.emplace_back(theDCData);
	}

	void BaseSocketAgent::connectVoiceInternal() noexcept {
		if (this->voiceConnections.size() > 0) {
			while (!this->theVCStopWatch.hasTimePassed()) {
				std::this_thread::sleep_for(1ms);
			}
			this->theVCStopWatch.resetTimer();
			std::unique_lock theLock{ this->theMutex };
			VoiceConnectInitData theConnectionData = this->voiceConnections.front();
			this->voiceConnections.pop_front();
			theLock.unlock();
			DiscordCoreAPI::Globals::voiceConnectionMap[theConnectionData.guildId] =
				std::make_unique<DiscordCoreAPI::VoiceConnection>(this, this->theShardMap[theConnectionData.currentShard].get(), theConnectionData,
					&this->discordCoreClient->configManager, this->doWeQuit, theConnectionData.streamType, theConnectionData.streamInfo);
			DiscordCoreAPI::Globals::voiceConnectionMap[theConnectionData.guildId]->connect();
		}
	}

	void BaseSocketAgent::run(std::stop_token stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !this->doWeQuit->load()) {
				this->disconnectVoiceInternal();
				this->connectVoiceInternal();
				std::vector<WebSocketSSLClient*> theVector{};
				for (auto& [key, value]: this->theShardMap) {
					if (!static_cast<WebSocketSSLShard*>(value.get())->areWeConnecting.load()) {
						theVector.emplace_back(value.get());
					}
				}
				auto theResult = WebSocketSSLClient::processIO(theVector);
				if (theResult.size() > 0) {
					for (auto& value: theResult) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" << static_cast<WebSocketSSLShard*>(value)->shard[0] << ","
								 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
								 << endl;
						}
					}
				}

				for (auto& value: theVector) {
					if (!static_cast<WebSocketSSLShard*>(value)->areWeConnecting.load()) {
						if (value->areWeStillConnected()) {
							static_cast<WebSocketSSLShard*>(value)->checkForAndSendHeartBeat();
						}
					}
				}
				if (theVector.size() == 0) {
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