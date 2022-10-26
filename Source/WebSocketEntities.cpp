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
		extern VoiceConnectionsMap voiceConnectionMap;
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

	WebSocketCore::WebSocketCore(DiscordCoreAPI::ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew) {
		this->typeOfWebSocket = typeOfWebSocketNew;
		this->configManager = configManagerNew;
	}

	std::string WebSocketCore::prepMessageData(std::string&& dataToSend, WebSocketOpCode theOpCode) noexcept {
		std::string header{};
		this->createHeader(header, dataToSend.size(), theOpCode);
		header.insert(header.end(), dataToSend.begin(), dataToSend.end());
		return header;
	}

	void WebSocketCore::createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept {
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
				DiscordCoreAPI::reportException("WebSocketCore::createHeader()");
			}
		}
	}

	void WebSocketCore::parseConnectionHeaders() noexcept {
		if (this->areWeStillConnected() && this->currentState.load() == WebSocketState::Upgrading && this->inputBuffer.getCurrentTail()->getUsedSpace() > 100) {
			auto string = this->getInputBuffer();
			this->currentMessage.writeData(string.data(), string.size());
			auto theFindValue = static_cast<std::string_view>(this->currentMessage).find("\r\n\r\n");
			if (theFindValue != std::string::npos) {
				this->currentMessage.clear();
				this->currentState.store(WebSocketState::Collecting_Hello);
				return;
			}
		}
		return;
	}

	void WebSocketCore::parseMessage() noexcept {
		if (this->inputBuffer.getUsedSpace() > 0) {
			if (this->currentMessage.size() < this->messageLength + this->messageOffset || this->currentMessage.size() == 0) {
				auto string = this->getInputBuffer();
				this->currentMessage.writeData(string.data(), string.size());
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
					uint8_t length01 = this->currentMessage[1];
					this->messageOffset = 2;
					if (length01 & webSocketMaskBit) {
						return;
					}
					this->messageLength = length01;
					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (this->currentMessage.size() < 8) {
							return;
						}
						uint8_t length03 = this->currentMessage[2];
						uint8_t length04 = this->currentMessage[3];
						this->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
						this->messageOffset += 2;
					} else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (this->currentMessage.size() < 10) {
							return;
						}
						this->messageLength = 0;
						for (uint64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
							uint8_t lengthNew = static_cast<uint8_t>(this->currentMessage[x]);
							this->messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
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
					uint16_t close = this->currentMessage[2] & 0xff;
					close <<= 8;
					close |= this->currentMessage[3] & 0xff;
					uint16_t closeCode{};
					std::string closeString{};
					if (this->typeOfWebSocket == WebSocketType::Voice) {
						VoiceWebSocketClose voiceClose{ close };
						closeCode = voiceClose.operator uint16_t();
						closeString = voiceClose.operator std::string();
					} else {
						WebSocketClose wsClose{ close };
						closeCode = wsClose.operator uint16_t();
						closeString = wsClose.operator std::string();
					}
					if (closeCode) {
						this->areWeResuming = true;
					}
					std::string webSocketTitle = this->typeOfWebSocket == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket";
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed()
							 << webSocketTitle + " [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" + " Closed; Code: " << +closeCode << ", "
							 << closeString << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					this->onClosed();
					return;
				}
			}
		}
	}

	WebSocketSSLShard::WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* client, int32_t currentShardNew, std::atomic_bool* doWeQuitNew)
		: WebSocketCore(&client->configManager, WebSocketType::Normal) {
		this->configManager = &client->configManager;
		this->shard[0] = currentShardNew;
		if (this->parser.allocate(1024ull * 1024ull) != simdjson::error_code::SUCCESS) {
			throw std::runtime_error{ "Failed to allocate the parser's memory." };
		}
		this->discordCoreClient = client;
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

	void WebSocketSSLShard::getVoiceConnectionData(const DiscordCoreAPI::VoiceConnectInitData& doWeCollect) noexcept {
		try {
			while (this->currentState.load() != WebSocketState::Authenticated) {
				std::this_thread::sleep_for(1ms);
			}
			DiscordCoreAPI::UpdateVoiceStateData data{};
			data.channelId = 0;
			data.guildId = doWeCollect.guildId;
			data.selfDeaf = doWeCollect.selfDeaf;
			data.selfMute = doWeCollect.selfMute;
			this->userId = doWeCollect.userId;
			auto serializer = data.operator DiscordCoreAPI::Jsonifier();
			if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
				serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Etf);
			} else {
				serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
			}
			std::string string = this->prepMessageData(serializer.operator std::string(), this->dataOpCode);
			if (!this->sendMessage(string, true)) {
				return;
			}
			if (DiscordCoreAPI::Snowflake{ doWeCollect.channelId } == 0) {
				return;
			}
			data.channelId = doWeCollect.channelId;
			serializer = data.operator DiscordCoreAPI::Jsonifier();
			if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
				serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Etf);
			} else {
				serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
			}
			string = this->prepMessageData(serializer.operator std::string(), this->dataOpCode);
			this->areWeCollectingData = true;
			if (!this->sendMessage(string, true)) {
				return;
			}
			DiscordCoreAPI::StopWatch<std::chrono::milliseconds> stopWatch{ 5500ms };
			while (this->areWeCollectingData) {
				if (stopWatch.hasTimePassed()) {
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

	bool WebSocketCore::sendMessage(std::string& dataToSend, bool priority) noexcept {
		if (this->areWeStillConnected()) {
			try {
				if (dataToSend.size() == 0) {
					return false;
				}
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::string webSocketTitle = this->typeOfWebSocket == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket";
					cout << DiscordCoreAPI::shiftToBrightBlue()
						 << "Sending " + webSocketTitle + " [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" + std::string("'s Message: ")
						 << static_cast<std::string>(dataToSend) << endl
						 << endl
						 << DiscordCoreAPI::reset();
				}
				ProcessIOResult didWeWrite{ false };
				DiscordCoreAPI::StopWatch stopWatch{ 5000ms };
				stopWatch.resetTimer();
				do {
					if (stopWatch.hasTimePassed()) {
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

	DiscordCoreAPI::StopWatch stopWatch{ 5s };
	DiscordCoreAPI::StopWatch stopWatchReal{ 50us };
	std::atomic_int32_t integer{};
	bool WebSocketSSLShard::onMessageReceived(std::string_view dataNew) noexcept {
		if (this->discordCoreClient) {
			if (this->areWeStillConnected() && this->currentMessage.size() > 0 &&
					(this->currentState.load() == WebSocketState::Authenticated || this->currentState.load() == WebSocketState::Sending_Identify) ||
				this->currentState.load() == WebSocketState::Collecting_Hello) {
				std::string string{};
				std::string& payload{ string };
				try {
					simdjson::ondemand::value value{};
					bool returnValue{ false };
					WebSocketMessage message{};
					if (dataNew.size() > 0) {
						returnValue = true;

						if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
							try {
								stopWatchReal.resetTimer();
								payload = ErlParser::parseEtfToJson(dataNew);
								payload.reserve(payload.size() + simdjson::SIMDJSON_PADDING);
								if (auto result = this->parser.iterate(simdjson::padded_string_view(payload.data(), payload.length(), payload.capacity())).get(value);
									result == simdjson::error_code::SUCCESS) {
									message = WebSocketMessage{ value };
								}
							} catch (...) {
								if (this->configManager->doWePrintGeneralErrorMessages()) {
									DiscordCoreAPI::reportException("ErlParser::parseEtfToJson()");
									std::cout << "The Payload: " << dataNew << std::endl;
								}
								this->currentMessage.clear();
								this->inputBuffer.clear();
								this->messageLength = 0;
								this->messageOffset = 0;
								return false;
							}
						} else {
							payload = dataNew;
							payload.reserve(payload.size() + simdjson::SIMDJSON_PADDING);
							if (this->parser.iterate(simdjson::padded_string_view(payload.data(), payload.length(), payload.capacity())).get(value) ==
								simdjson::error_code::SUCCESS) {
								message = WebSocketMessage{ value };
							}
						}
					}

					if (message.s != 0) {
						this->lastNumberReceived = message.s;
					}

					if (this->configManager->doWePrintWebSocketSuccessMessages()) {
						cout << DiscordCoreAPI::shiftToBrightGreen()
							 << "Message received from WebSocket [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" + std::string(": ") << payload
							 << DiscordCoreAPI::reset() << endl
							 << endl;
					}

					switch (message.op) {
						case 0: {
							if (message.t != "") {
								switch (EventConverter{ message.t }) {
									case 1: {
										ReadyData data{ message.processJsonMessage<ReadyData>(value) };
										this->currentState.store(WebSocketState::Authenticated);
										this->sessionId = data.sessionId;
										std::string theResumeUrl = data.resumeGatewayUrl;
										theResumeUrl = theResumeUrl.substr(theResumeUrl.find("wss://") + std::string{ "wss://" }.size());
										theResumeUrl = theResumeUrl.substr(0, theResumeUrl.find("/"));
										this->resumeUrl = theResumeUrl;
										DiscordCoreAPI::UserData user{ data.user };
										this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ user,
											this->discordCoreClient
												->baseSocketAgentsMap[static_cast<int32_t>(floor(
													static_cast<int32_t>(this->shard[0]) % static_cast<int32_t>(this->discordCoreClient->configManager.getTotalShardCount())))]
												.get() };
										DiscordCoreAPI::Users::insertUser(std::move(user));
										this->currentReconnectTries = 0;
										break;
									}
									case 2: {
										this->currentState.store(WebSocketState::Authenticated);
										this->currentReconnectTries = 0;
										break;
									}
									case 3: {
										std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>()
										};
										dataPackage->permissionData = DiscordCoreAPI::GuildApplicationCommandPermissionsData{
											message.processJsonMessage<DiscordCoreAPI::GuildApplicationCommandPermissionsData>(value)
										};
										this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
										break;
									}
									case 4: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>()
										};
										dataPackage->theRule = DiscordCoreAPI::AutoModerationRule{ message.processJsonMessage<DiscordCoreAPI::AutoModerationRule>(value) };
										this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
										break;
									}
									case 5: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>()
										};
										dataPackage->theRule = DiscordCoreAPI::AutoModerationRule{ message.processJsonMessage<DiscordCoreAPI::AutoModerationRule>(value) };
										this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
										break;
									}
									case 6: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>()
										};
										dataPackage->theRule = DiscordCoreAPI::AutoModerationRule{ message.processJsonMessage<DiscordCoreAPI::AutoModerationRule>(value) };
										this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
										break;
									}
									case 7: {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>()
										};
										dataPackage->data = DiscordCoreAPI::AutoModerationActionExecutionEventData{
											message.processJsonMessage<DiscordCoreAPI::AutoModerationActionExecutionEventData>(value)
										};
										this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
										break;
									}
									case 8: {
										if (DiscordCoreAPI::Channels::doWeCacheChannels || this->discordCoreClient->eventManager.onChannelCreationEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::ChannelData> channel{ std::make_unique<DiscordCoreAPI::ChannelData>(
												message.processJsonMessage<DiscordCoreAPI::ChannelData>(value)) };
											DiscordCoreAPI::GuildData guild{};
											guild.id = channel->guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
												DiscordCoreAPI::Guilds::cache[guild].channels.emplace_back(channel->id);
											}
											if (DiscordCoreAPI::Channels::doWeCacheChannels) {
												DiscordCoreAPI::Channels::insertChannel(*channel);
											}
											if (this->discordCoreClient->eventManager.onChannelCreationEvent.functions.size() > 0) {
												DiscordCoreAPI::OnChannelCreationData dataPackage{ std::move(channel) };
												this->discordCoreClient->eventManager.onChannelCreationEvent(dataPackage);
											}
										}
										break;
									}
									case 9: {
										if (DiscordCoreAPI::Channels::doWeCacheChannels || this->discordCoreClient->eventManager.onChannelUpdateEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::ChannelData> channel{ std::make_unique<DiscordCoreAPI::ChannelData>(
												message.processJsonMessage<DiscordCoreAPI::ChannelData>(value)) };
											if (DiscordCoreAPI::Channels::doWeCacheChannels) {
												DiscordCoreAPI::Channels::insertChannel(*channel);
											}
											if (this->discordCoreClient->eventManager.onChannelUpdateEvent.functions.size() > 0) {
												DiscordCoreAPI::OnChannelUpdateData dataPackage{ std::move(channel) };
												this->discordCoreClient->eventManager.onChannelUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 10: {
										if (DiscordCoreAPI::Channels::doWeCacheChannels || this->discordCoreClient->eventManager.onChannelDeletionEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::ChannelData> channel{ std::make_unique<DiscordCoreAPI::ChannelData>(
												message.processJsonMessage<DiscordCoreAPI::ChannelData>(value)) };
											DiscordCoreAPI::GuildData guild{};
											guild.id = channel->guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
												for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::cache[guild].channels.size(); ++x) {
													if (DiscordCoreAPI::Guilds::cache[guild].channels[x] == channel->id) {
														DiscordCoreAPI::Guilds::cache[guild].channels.erase(DiscordCoreAPI::Guilds::cache[guild].channels.begin() + x);
													}
												}
											}
											if (DiscordCoreAPI::Channels::doWeCacheChannels) {
												DiscordCoreAPI::Channels::removeChannel(channel->id);
											}
											if (this->discordCoreClient->eventManager.onChannelDeletionEvent.functions.size() > 0) {
												DiscordCoreAPI::OnChannelDeletionData dataPackage{ std::move(channel) };
												this->discordCoreClient->eventManager.onChannelDeletionEvent(dataPackage);
											}
										}
										break;
									}
									case 11: {
										std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>() };
										dataPackage->dataPackage =
											DiscordCoreAPI::ChannelPinsUpdateEventData{ message.processJsonMessage<DiscordCoreAPI::ChannelPinsUpdateEventData>(value) };
										this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
										break;
									}
									case 12: {
										std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadCreationData>() };
										dataPackage->thread = DiscordCoreAPI::Thread{ message.processJsonMessage<DiscordCoreAPI::Thread>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->thread.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											DiscordCoreAPI::Guilds::cache[guild].threads.emplace_back(dataPackage->thread.id);
										}
										this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
										break;
									}
									case 13: {
										std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadUpdateData>() };
										dataPackage->thread = DiscordCoreAPI::Thread{ message.processJsonMessage<DiscordCoreAPI::Thread>(value) };
										this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
										break;
									}
									case 14: {
										std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadDeletionData>() };
										dataPackage->thread = DiscordCoreAPI::Thread{ message.processJsonMessage<DiscordCoreAPI::Thread>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->thread.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::cache[guild].threads.size(); ++x) {
												if (DiscordCoreAPI::Guilds::cache[guild].threads[x] == dataPackage->thread.id) {
													DiscordCoreAPI::Guilds::cache[guild].threads.erase(DiscordCoreAPI::Guilds::cache[guild].threads.begin() + x);
												}
											}
										}
										this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
										break;
									}
									case 15: {
										std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadListSyncData>() };
										dataPackage->threadListSyncData =
											DiscordCoreAPI::ThreadListSyncData{ message.processJsonMessage<DiscordCoreAPI::ThreadListSyncData>(value) };
										this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
										break;
									}
									case 16: {
										std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>() };
										dataPackage->threadMember = DiscordCoreAPI::ThreadMemberData{ message.processJsonMessage<DiscordCoreAPI::ThreadMemberData>(value) };
										this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
										break;
									}
									case 17: {
										std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>() };
										dataPackage->threadMembersUpdateData =
											DiscordCoreAPI::ThreadMembersUpdateData{ message.processJsonMessage<DiscordCoreAPI::ThreadMembersUpdateData>(value) };
										this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
										break;
									}
									case 18: {
										if (integer.load() % 100 == 0 || stopWatch.totalTimePassed() % 10 == 0) {
											//std::cout << "THE GUILD COUNT: " << integer.load() << ", TOTAL TIME: " << stopWatch.totalTimePassed() << std::endl;
										}
										integer.store(integer.load() + 1);
										std::unique_ptr<DiscordCoreAPI::GuildData> guildPtr{ std::make_unique<DiscordCoreAPI::GuildData>(
											message.processJsonMessage<DiscordCoreAPI::GuildData>(value)) };
										DiscordCoreAPI::Snowflake guildId{};
										stopWatchReal.resetTimer();
										guildId = guildPtr->id;
										if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildCreationEvent.functions.size() > 0) {
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
												DiscordCoreAPI::Guilds::insertGuild(*guildPtr);
											}
											if (this->discordCoreClient->eventManager.onGuildCreationEvent.functions.size() > 0) {
												DiscordCoreAPI::OnGuildCreationData dataPackage{ std::move(guildPtr), this->discordCoreClient };
												this->discordCoreClient->eventManager.onGuildCreationEvent(dataPackage);
											}
										}
										break;
									}
									case 19: {
										if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildUpdateEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::GuildData> guildPtr{ std::make_unique<DiscordCoreAPI::GuildData>(
												message.processJsonMessage<DiscordCoreAPI::GuildData>(value)) };
											DiscordCoreAPI::Snowflake guildId{};
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
												DiscordCoreAPI::Guilds::insertGuild(*guildPtr);
											}
											if (this->discordCoreClient->eventManager.onGuildUpdateEvent.functions.size() > 0) {
												DiscordCoreAPI::OnGuildUpdateData dataPackage{ std::make_unique<DiscordCoreAPI::GuildData>(*guildPtr), this->discordCoreClient };
												this->discordCoreClient->eventManager.onGuildUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 20: {
										if (DiscordCoreAPI::Guilds::doWeCacheGuilds || this->discordCoreClient->eventManager.onGuildDeletionEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::GuildData> guild =
												std::make_unique<DiscordCoreAPI::GuildData>(message.processJsonMessage<DiscordCoreAPI::GuildData>(value));
											DiscordCoreAPI::Snowflake guildId{};
											*guild = DiscordCoreAPI::Guilds::getCachedGuild({ .guildId = guildId });
											if (DiscordCoreAPI::Guilds::doWeCacheGuilds) {
												DiscordCoreAPI::Guilds::removeGuild(guild->id);
											}
											for (auto& valueNew: guild->members) {
												DiscordCoreAPI::GuildMemberData guildMember =
													DiscordCoreAPI::GuildMembers::getCachedGuildMember({ .guildMemberId = valueNew, .guildId = guildId });
												DiscordCoreAPI::GuildMembers::removeGuildMember(guildMember);
											}
											for (auto& valueNew: guild->channels) {
												DiscordCoreAPI::Channels::removeChannel(valueNew);
											}
											for (auto& valueNew: guild->roles) {
												DiscordCoreAPI::Roles::removeRole(valueNew);
											}
											if (this->discordCoreClient->eventManager.onGuildDeletionEvent.functions.size() > 0) {
												DiscordCoreAPI::OnGuildDeletionData dataPackage{ std::move(guild) };
												this->discordCoreClient->eventManager.onGuildDeletionEvent(dataPackage);
											}
										}
										break;
									}
									case 21: {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanAddData>() };
										dataPackage->user = DiscordCoreAPI::UserData{ message.processJsonMessage<DiscordCoreAPI::UserData>(value) };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										this->discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
										break;
									}
									case 22: {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>() };
										dataPackage->user = DiscordCoreAPI::UserData{ message.processJsonMessage<DiscordCoreAPI::UserData>(value) };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
										break;
									}
									case 23: {
										std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>() };
										dataPackage->updateData =
											DiscordCoreAPI::GuildEmojisUpdateEventData{ message.processJsonMessage<DiscordCoreAPI::GuildEmojisUpdateEventData>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->updateData.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											DiscordCoreAPI::Guilds::cache[guild].emoji.clear();
											for (auto& valueNew: dataPackage->updateData.emojis) {
												DiscordCoreAPI::Guilds::cache[guild].emoji.emplace_back(valueNew.id);
											}
										}
										this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
										break;
									}
									case 24: {
										std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>() };
										dataPackage->updateData =
											DiscordCoreAPI::GuildStickersUpdateEventData{ message.processJsonMessage<DiscordCoreAPI::GuildStickersUpdateEventData>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->updateData.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											DiscordCoreAPI::Guilds::cache[guild].stickers.clear();
											for (auto& valueNew: dataPackage->updateData.stickers) {
												DiscordCoreAPI::Guilds::cache[guild].stickers.emplace_back(valueNew.id);
											}
										}
										this->discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
										break;
									}
									case 25: {
										std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>()
										};
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
										break;
									}
									case 26: {
										if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
											this->discordCoreClient->eventManager.onGuildMemberAddEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::GuildMemberData> guildMember{ std::make_unique<DiscordCoreAPI::GuildMemberData>(
												message.processJsonMessage<DiscordCoreAPI::GuildMemberData>(value)) };
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
												DiscordCoreAPI::GuildMembers::insertGuildMember(*guildMember);
												DiscordCoreAPI::GuildData guild{};
												guild.id = guildMember->guildId;
												if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
													DiscordCoreAPI::Guilds::cache[guild].memberCount++;
												}
											}
											if (this->discordCoreClient->eventManager.onGuildMemberAddEvent.functions.size() > 0) {
												DiscordCoreAPI::OnGuildMemberAddData dataPackage{ std::move(guildMember), this->discordCoreClient };
												this->discordCoreClient->eventManager.onGuildMemberAddEvent(dataPackage);
											}
										}
										break;
									}
									case 27: {
										if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
											this->discordCoreClient->eventManager.onGuildMemberRemoveEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::GuildMemberData> guildMember{ std::make_unique<DiscordCoreAPI::GuildMemberData>(
												message.processJsonMessage<DiscordCoreAPI::GuildMemberData>(value)) };
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
												DiscordCoreAPI::GuildData guild{};
												guild.id = guildMember->guildId;
												DiscordCoreAPI::GuildMembers::removeGuildMember(*guildMember);
												if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
													for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::cache[guild].members.size(); ++x) {
														if (DiscordCoreAPI::Guilds::cache[guild].members[x] == userId) {
															DiscordCoreAPI::Guilds::cache[guild].memberCount--;
															DiscordCoreAPI::Guilds::cache[guild].members.erase(DiscordCoreAPI::Guilds::cache[guild].members.begin() + x);
														}
													}
												}
											}
											if (this->discordCoreClient->eventManager.onGuildMemberRemoveEvent.functions.size() > 0) {
												DiscordCoreAPI::OnGuildMemberRemoveData dataPackage{ std::make_unique<DiscordCoreAPI::UserData>(
																										 DiscordCoreAPI::Users::getCachedUser({ .userId = userId })),
													this->discordCoreClient, userId };
												this->discordCoreClient->eventManager.onGuildMemberRemoveEvent(dataPackage);
											}
										}
										break;
									}
									case 28: {
										if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers ||
											this->discordCoreClient->eventManager.onGuildMemberUpdateEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::GuildMemberData> guildMember{ std::make_unique<DiscordCoreAPI::GuildMemberData>(
												message.processJsonMessage<DiscordCoreAPI::GuildMemberData>(value)) };
											if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers) {
												DiscordCoreAPI::GuildMembers::insertGuildMember(*guildMember);
											}
											if (this->discordCoreClient->eventManager.onGuildMemberUpdateEvent.functions.size() > 0) {
												DiscordCoreAPI::OnGuildMemberUpdateData dataPackage{ std::move(guildMember) };
												this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 29: {
										std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>() };
										dataPackage->chunkEventData =
											DiscordCoreAPI::GuildMembersChunkEventData{ message.processJsonMessage<DiscordCoreAPI::GuildMembersChunkEventData>(value) };
										this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
										break;
									}
									case 30: {
										if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleCreationEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::RoleData> rolePtr{ std::make_unique<DiscordCoreAPI::RoleData>(value["d"]["role"]) };
											DiscordCoreAPI::Snowflake guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
											DiscordCoreAPI::GuildData guild{};
											guild.id = guildId;
											if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
												DiscordCoreAPI::Guilds::cache[guild].roles.emplace_back(rolePtr->id);
											}
											if (DiscordCoreAPI::Roles::doWeCacheRoles) {
												DiscordCoreAPI::Roles::insertRole(*rolePtr);
											}
											if (this->discordCoreClient->eventManager.onRoleCreationEvent.functions.size() > 0) {
												DiscordCoreAPI::OnRoleCreationData dataPackage{ std::move(rolePtr), guildId };
												this->discordCoreClient->eventManager.onRoleCreationEvent(dataPackage);
											}
										}
										break;
									}
									case 31: {
										if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleUpdateEvent.functions.size() > 0) {
											std::unique_ptr<DiscordCoreAPI::RoleData> rolePtr{ std::make_unique<DiscordCoreAPI::RoleData>(value["d"]["role"]) };
											DiscordCoreAPI::Snowflake guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
											if (DiscordCoreAPI::Roles::doWeCacheRoles) {
												DiscordCoreAPI::Roles::insertRole(*rolePtr);
											}
											if (this->discordCoreClient->eventManager.onRoleUpdateEvent.functions.size() > 0) {
												DiscordCoreAPI::OnRoleUpdateData dataPackage{ std::move(rolePtr), guildId };
												this->discordCoreClient->eventManager.onRoleUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 32: {
										if (DiscordCoreAPI::Roles::doWeCacheRoles || this->discordCoreClient->eventManager.onRoleDeletionEvent.functions.size() > 0) {
											DiscordCoreAPI::Snowflake roleId = DiscordCoreAPI::getId(value["d"], "role_id");
											DiscordCoreAPI::Snowflake guildId = DiscordCoreAPI::getId(value["d"]["guild_hashes"].value(), "guild_id");
											DiscordCoreAPI::OnRoleDeletionData dataPackage{ std::make_unique<DiscordCoreAPI::RoleData>(), guildId };
											DiscordCoreAPI::RoleData role = DiscordCoreAPI::Roles::getCachedRole({ .guildId = guildId, .roleId = roleId });
											DiscordCoreAPI::GuildData guild{};
											guild.id = guildId;
											if (DiscordCoreAPI::Roles::doWeCacheRoles) {
												DiscordCoreAPI::Roles::removeRole(roleId);
												if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
													for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::cache[guild].roles.size(); ++x) {
														if (DiscordCoreAPI::Guilds::cache[guild].roles[x] == roleId) {
															DiscordCoreAPI::Guilds::cache[guild].roles.erase(DiscordCoreAPI::Guilds::cache[guild].roles.begin() + x);
														}
													}
												}
											}
											if (this->discordCoreClient->eventManager.onRoleDeletionEvent.functions.size() > 0) {
												this->discordCoreClient->eventManager.onRoleDeletionEvent(dataPackage);
											}
										}
										break;
									}
									case 33: {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventCreationData>()
										};
										dataPackage->guildScheduledEvent =
											DiscordCoreAPI::GuildScheduledEventData{ message.processJsonMessage<DiscordCoreAPI::GuildScheduledEventData>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->guildScheduledEvent.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											DiscordCoreAPI::Guilds::cache[guild].guildScheduledEvents.emplace_back(dataPackage->guildScheduledEvent.id);
										}
										this->discordCoreClient->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
										break;
									}
									case 34: {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUpdateData>()
										};
										dataPackage->guildScheduledEvent =
											DiscordCoreAPI::GuildScheduledEventData{ message.processJsonMessage<DiscordCoreAPI::GuildScheduledEventData>(value) };
										this->discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
										break;
									}
									case 35: {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventDeletionData>()
										};
										dataPackage->guildScheduledEvent =
											DiscordCoreAPI::GuildScheduledEventData{ message.processJsonMessage<DiscordCoreAPI::GuildScheduledEventData>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->guildScheduledEvent.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::cache[guild].guildScheduledEvents.size(); ++x) {
												if (DiscordCoreAPI::Guilds::cache[guild].guildScheduledEvents[x] == dataPackage->guildScheduledEvent.id) {
													DiscordCoreAPI::Guilds::cache[guild].guildScheduledEvents.erase(
														DiscordCoreAPI::Guilds::cache[guild].guildScheduledEvents.begin() + x);
												}
											}
										}
										this->discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
										break;
									}
									case 36: {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserAddData>()
										};
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->userId = DiscordCoreAPI::getId(value["d"], "user_id");
										dataPackage->guildScheduledEventId = DiscordCoreAPI::getId(value["d"], "guild_scheduled_event_id");
										this->discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
										break;
									}
									case 37: {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData>()
										};
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->userId = DiscordCoreAPI::getId(value["d"], "user_id");
										dataPackage->guildScheduledEventId = DiscordCoreAPI::getId(value["d"], "guild_scheduled_event_id");
										this->discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
										break;
									}
									case 38: {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										simdjson::ondemand::value objectNew{};
										if (value["d"]["integration"].get(objectNew) == simdjson::error_code::SUCCESS) {
											dataPackage->integrationData = DiscordCoreAPI::IntegrationData{ objectNew };
										}
										this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
										break;
									}
									case 39: {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										simdjson::ondemand::value objectNew{};
										if (value["d"]["integration"].get(objectNew) == simdjson::error_code::SUCCESS) {
											dataPackage->integrationData = DiscordCoreAPI::IntegrationData{ objectNew };
										}
										this->discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
										break;
									}
									case 40: {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->applicationId = DiscordCoreAPI::getId(value["d"], "application_id");
										dataPackage->id = DiscordCoreAPI::getId(value["d"], "id");
										this->discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
										break;
									}
									case 41: {
										std::unique_ptr<DiscordCoreAPI::InteractionData> interactionData{ std::make_unique<DiscordCoreAPI::InteractionData>() };
										*interactionData = DiscordCoreAPI::InteractionData{ message.processJsonMessage<DiscordCoreAPI::InteractionData>(value) };
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
												DiscordCoreAPI::CommandData commanddataNew = *commandData;
												this->discordCoreClient->commandController.checkForAndRunCommand(commanddataNew);
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
														if (DiscordCoreAPI::ButtonCollector::buttonInteractionBuffersMap.contains(
																eventData->getChannelId() + eventData->getMessageId())) {
															DiscordCoreAPI::ButtonCollector::buttonInteractionBuffersMap[eventData->getChannelId() + eventData->getMessageId()]
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
														if (DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBuffersMap.contains(
																eventData->getChannelId() + eventData->getMessageId())) {
															DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBuffersMap[eventData->getChannelId() +
																eventData->getMessageId()]
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
												std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
													std::make_unique<DiscordCoreAPI::OnInteractionCreationData>()
												};
												dataPackage->interactionData = *interactionData;
												std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
													std::make_unique<DiscordCoreAPI::OnInputEventCreationData>()
												};
												eventCreationData->inputEventData = *eventData;
												if (DiscordCoreAPI::ModalCollector::modalInteractionBuffersMap.contains(eventData->getChannelId())) {
													DiscordCoreAPI::ModalCollector::modalInteractionBuffersMap[eventData->getChannelId()]->send(eventData->getInteractionData());
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
										dataPackage->invite = DiscordCoreAPI::InviteData{ message.processJsonMessage<DiscordCoreAPI::InviteData>(value) };
										this->discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
										break;
									}
									case 43: {
										std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteDeletionData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->channelId = DiscordCoreAPI::getId(value["d"], "channel_id");
										dataPackage->code = DiscordCoreAPI::getId(value["d"], "code");
										this->discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
										break;
									}
									case 44: {
										std::unique_ptr<DiscordCoreAPI::Message> messageNew{ std::make_unique<DiscordCoreAPI::Message>(
											message.processJsonMessage<DiscordCoreAPI::Message>(value)) };
										std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageCreationData>() };
										dataPackage->message = *messageNew;
										for (auto& [key, value]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBuffersMap) {
											value->send(*messageNew);
										}
										this->discordCoreClient->eventManager.onMessageCreationEvent(*dataPackage);
										if (messageNew->content.find("!registerapplicationcommands") != std::string::npos) {
											std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>() };
											commandData->commandName = "registerapplicationcommands";
											DiscordCoreAPI::CommandData commanddataNew = *commandData;
											this->discordCoreClient->commandController.checkForAndRunCommand(commanddataNew);
										}
										std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
										break;
									}
									case 45: {
										std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
										dataPackage->messageNew = DiscordCoreAPI::Message{ message.processJsonMessage<DiscordCoreAPI::Message>(value) };
										for (auto& [key, value]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBuffersMap) {
											value->send(dataPackage->messageNew);
										}
										this->discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
										break;
									}
									case 46: {
										std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeletionData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->channelId = DiscordCoreAPI::getId(value["d"], "channel_id");
										dataPackage->messageId = DiscordCoreAPI::getId(value["d"], "id");
										this->discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
										break;
									}
									case 47: {
										std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->channelId = DiscordCoreAPI::getId(value["d"], "channel_id");
										simdjson::ondemand::array arrayValue{};
										if (value["d"]["ids"].get(arrayValue) == simdjson::error_code::SUCCESS) {
											for (auto value: arrayValue) {
												dataPackage->ids.emplace_back(DiscordCoreAPI::strtoull(value.get_string().take_value()));
											}
										}
										this->discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
										break;
									}
									case 48: {
										std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
										dataPackage->reaction = DiscordCoreAPI::Reaction{ message.processJsonMessage<DiscordCoreAPI::Reaction>(value) };
										this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
										break;
									}
									case 49: {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
										dataPackage->reactionRemoveData =
											DiscordCoreAPI::ReactionRemoveData{ message.processJsonMessage<DiscordCoreAPI::ReactionRemoveData>(value) };
										this->discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
										break;
									}
									case 50: {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->channelId = DiscordCoreAPI::getId(value["d"], "channel_id");
										dataPackage->messageId = DiscordCoreAPI::getId(value["d"], "message_id");
										this->discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
										break;
									}
									case 51: {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>() };
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->channelId = DiscordCoreAPI::getId(value["d"], "channel_id");
										dataPackage->messageId = DiscordCoreAPI::getId(value["d"], "message_id");
										simdjson::ondemand::value objectNew{};
										if (value["d"]["emoji"].get(objectNew) == simdjson::error_code::SUCCESS) {
											dataPackage->emoji = DiscordCoreAPI::EmojiData{ objectNew };
										}
										this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
										break;
									}
									case 52: {
										std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
										dataPackage->presenceData = DiscordCoreAPI::PresenceUpdateData{ message.processJsonMessage<DiscordCoreAPI::PresenceUpdateData>(value) };
										this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
										break;
									}
									case 53: {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
										dataPackage->stageInstance = DiscordCoreAPI::StageInstance{ message.processJsonMessage<DiscordCoreAPI::StageInstance>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->stageInstance.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											DiscordCoreAPI::Guilds::cache[guild].stageInstances.emplace_back(dataPackage->stageInstance.id);
										}
										this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
										break;
									}
									case 54: {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
										dataPackage->stageInstance = DiscordCoreAPI::StageInstance{ message.processJsonMessage<DiscordCoreAPI::StageInstance>(value) };
										this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
										break;
									}
									case 55: {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
										dataPackage->stageInstance = DiscordCoreAPI::StageInstance{ message.processJsonMessage<DiscordCoreAPI::StageInstance>(value) };
										DiscordCoreAPI::GuildData guild{};
										guild.id = dataPackage->stageInstance.guildId;
										if (DiscordCoreAPI::Guilds::cache.contains(guild)) {
											for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::cache[guild].stageInstances.size(); ++x) {
												if (DiscordCoreAPI::Guilds::cache[guild].stageInstances[x] == dataPackage->stageInstance.id) {
													DiscordCoreAPI::Guilds::cache[guild].stageInstances.erase(DiscordCoreAPI::Guilds::cache[guild].stageInstances.begin() + x);
												}
											}
										}
										this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
										break;
									}
									case 56: {
										std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
										dataPackage->typingStartData = DiscordCoreAPI::TypingStartData{ message.processJsonMessage<DiscordCoreAPI::TypingStartData>(value) };
										this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
										break;
									}
									case 57: {
										std::unique_ptr<DiscordCoreAPI::UserData> userPtr{ std::make_unique<DiscordCoreAPI::UserData>(
											message.processJsonMessage<DiscordCoreAPI::UserData>(value)) };
										if (DiscordCoreAPI::Users::doWeCacheUsers || this->discordCoreClient->eventManager.onUserUpdateEvent.functions.size() > 0) {
											DiscordCoreAPI::Snowflake userId{ userPtr->id };
											if (DiscordCoreAPI::Users::doWeCacheUsers) {
												DiscordCoreAPI::Users::insertUser(*userPtr);
											}
											if (this->discordCoreClient->eventManager.onUserUpdateEvent.functions.size() > 0) {
												DiscordCoreAPI::OnUserUpdateData dataPackage{ std::move(userPtr) };
												this->discordCoreClient->eventManager.onUserUpdateEvent(dataPackage);
											}
										}
										break;
									}
									case 58: {
										std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
										dataPackage->voiceStateData = DiscordCoreAPI::VoiceStateData{ message.processJsonMessage<DiscordCoreAPI::VoiceStateData>(value) };
										this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
										if (this->areWeCollectingData && !this->stateUpdateCollected && !this->serverUpdateCollected &&
											dataPackage->voiceStateData.userId == this->userId) {
											this->voiceConnectionData = VoiceConnectionData{};
											this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
											this->stateUpdateCollected = true;
										} else if (this->areWeCollectingData && !this->stateUpdateCollected) {
											this->voiceConnectionData.sessionId = dataPackage->voiceStateData.sessionId;
											if (this->voiceConnectionDataBuffersMap.contains(dataPackage->voiceStateData.guildId.operator size_t())) {
												this->voiceConnectionDataBuffersMap[dataPackage->voiceStateData.guildId.operator size_t()]->send(this->voiceConnectionData);
											}
											this->serverUpdateCollected = false;
											this->stateUpdateCollected = false;
											this->areWeCollectingData = false;
										}
										if (this->discordCoreClient->configManager.doWeCacheUsers() && this->discordCoreClient->configManager.doWeCacheGuilds()) {
											DiscordCoreAPI::GuildMemberData data{};
											data.id = dataPackage->voiceStateData.userId;
											data.guildId = dataPackage->voiceStateData.guildId;
											if (DiscordCoreAPI::GuildMembers::cache.contains(data)) {
												DiscordCoreAPI::GuildMembers::cache[data].voiceChannelId = dataPackage->voiceStateData.channelId;
											}
										}
										this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
										break;
									}
									case 59: {
										std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>() };
										*dataPackage = DiscordCoreAPI::OnVoiceServerUpdateData{ message.processJsonMessage<DiscordCoreAPI::OnVoiceServerUpdateData>(value) };
										this->voiceConnectionData.endPoint = dataPackage->endpoint;
										this->voiceConnectionData.token = dataPackage->token;
										if (this->areWeCollectingData && !this->serverUpdateCollected && !this->stateUpdateCollected) {
											this->voiceConnectionData = VoiceConnectionData{};
											this->serverUpdateCollected = true;
										} else if (this->areWeCollectingData && !this->serverUpdateCollected) {
											if (this->voiceConnectionDataBuffersMap.contains(dataPackage->guildId.operator size_t())) {
												this->voiceConnectionDataBuffersMap[dataPackage->guildId.operator size_t()]->send(this->voiceConnectionData);
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
										dataPackage->guildId = DiscordCoreAPI::getId(value["d"], "guild_id");
										dataPackage->channelId = DiscordCoreAPI::getId(value["d"], "channel_id");
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
							InvalidSessionData data{ message.processJsonMessage<InvalidSessionData>(value) };
							if (this->configManager->doWePrintWebSocketErrorMessages()) {
								cout << DiscordCoreAPI::shiftToBrightBlue()
									 << "Shard [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" + " Reconnecting (Type 9)!"
									 << DiscordCoreAPI::reset() << endl
									 << endl;
							}
							std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()) };
							int32_t numOfMsToWait =
								static_cast<int32_t>(1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
							std::this_thread::sleep_for(std::chrono::milliseconds{ numOfMsToWait });
							if (data.d == true) {
								this->areWeResuming = true;
							} else {
								this->areWeResuming = false;
							}
							this->onClosed();
							returnValue = true;
							break;
						}
						case 10: {
							HelloData data{ message.processJsonMessage<HelloData>(value) };
							if (data.heartbeatInterval != 0) {
								this->areWeHeartBeating = true;
								this->heartBeatStopWatch = DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ std::chrono::milliseconds{ data.heartbeatInterval } };
								this->heartBeatStopWatch.resetTimer();
								this->haveWeReceivedHeartbeatAck = true;
							}
							if (this->areWeResuming) {
								WebSocketResumeData data{};
								data.botToken = this->configManager->getBotToken();
								data.sessionId = this->sessionId;
								data.lastNumberReceived = this->lastNumberReceived;
								auto serializer = data.operator DiscordCoreAPI::Jsonifier();
								if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
									serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Etf);
								} else {
									serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
								}
								std::string string = this->prepMessageData(serializer.operator std::string(), this->dataOpCode);
								if (!this->sendMessage(string, true)) {
									returnValue = true;
								}
								this->currentState.store(WebSocketState::Sending_Identify);
							} else {
								WebSocketIdentifyData data{};
								data.botToken = this->configManager->getBotToken();
								data.currentShard = this->shard[0];
								data.numberOfShards = this->shard[1];
								data.intents = static_cast<int64_t>(this->configManager->getGatewayIntents());
								data.presence = this->configManager->getPresenceData();
								auto serializer = data.operator DiscordCoreAPI::Jsonifier();
								if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
									serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Etf);
								} else {
									serializer.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
								}
								std::string string = this->prepMessageData(serializer.operator std::string(), this->dataOpCode);
								if (!this->sendMessage(string, true)) {
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

	void WebSocketCore::checkForAndSendHeartBeat(bool isImmediate) noexcept {
		try {
			if ((this->currentState.load() == WebSocketState::Authenticated && this->heartBeatStopWatch.hasTimePassed() && this->haveWeReceivedHeartbeatAck) || isImmediate) {
				std::string string{};
				if (this->typeOfWebSocket == WebSocketType::Voice) {
					DiscordCoreAPI::Jsonifier data{};
					data["d"] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
					data["op"] = 3;
					if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
						data.refreshString(DiscordCoreAPI::JsonifierSerializeType::Etf);
					} else {
						data.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
					}
					string = this->prepMessageData(data.operator std::string(), this->dataOpCode);

				} else {
					DiscordCoreAPI::Jsonifier data{};
					data["d"] = this->lastNumberReceived;
					data["op"] = 1;
					if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
						data.refreshString(DiscordCoreAPI::JsonifierSerializeType::Etf);
					} else {
						data.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
					}
					string = this->prepMessageData(data.operator std::string(), this->dataOpCode);
				}
				if (!this->sendMessage(string, true)) {
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

	void WebSocketCore::handleBuffer() noexcept {
		if (this->currentState.load() == WebSocketState::Upgrading) {
			this->parseConnectionHeaders();
		}
		this->parseMessage();
	}

	void WebSocketSSLShard::disconnect() noexcept {
		if (this->socket != SOCKET_ERROR) {
			this->socket = SOCKET_ERROR;
			this->ssl = nullptr;
			this->currentState.store(WebSocketState::Disconnected);
			this->areWeConnecting.store(true);
			this->outputBuffer.clear();
			this->inputBuffer.clear();
			this->closeCode = 0;
			this->areWeHeartBeating = false;
			this->connections = std::make_unique<DiscordCoreAPI::ConnectionPackage>();
			this->connections->currentReconnectTries = this->currentReconnectTries;
			this->connections->areWeResuming = this->areWeResuming;
			this->connections->currentShard = this->shard[0];
		}
	}

	void WebSocketSSLShard::onClosed() noexcept {
		if (this->maxReconnectTries > this->currentReconnectTries) {
			this->disconnect();
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

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage packageNew) noexcept {
		try {
			while (!this->discordCoreClient->isItSafeToConnect.load()) {
				std::this_thread::sleep_for(1ms);
			}
			this->discordCoreClient->isItSafeToConnect.store(false);
			if (packageNew.currentShard != -1) {
				if (!this->shardMap.contains(packageNew.currentShard)) {
					this->shardMap[packageNew.currentShard] = std::make_unique<WebSocketSSLShard>(this->discordCoreClient, packageNew.currentShard, this->doWeQuit);
				}
				this->shardMap[packageNew.currentShard]->currentReconnectTries = packageNew.currentReconnectTries;
				this->shardMap[packageNew.currentShard]->currentReconnectTries++;
				std::string connectionUrl = packageNew.areWeResuming ? this->shardMap[packageNew.currentShard]->resumeUrl : this->configManager->getConnectionAddress();
				bool isItFirstIteraion{ true };
				bool didWeConnect{};
				DiscordCoreAPI::StopWatch stopWatch{ 5s };
				do {
					if (stopWatch.hasTimePassed()) {
						shardMap[packageNew.currentShard]->onClosed();
						break;
					}
					if (!isItFirstIteraion) {
						std::this_thread::sleep_for(5s);
					}
					if (this->configManager->doWePrintGeneralSuccessMessages()) {
						cout << DiscordCoreAPI::shiftToBrightBlue() << "Connecting Shard " + std::to_string(packageNew.currentShard + 1) << " of "
							 << this->configManager->getShardCountForThisProcess()
							 << std::string(" Shards for this process. (") + std::to_string(packageNew.currentShard + 1) + " of " +
								std::to_string(this->configManager->getTotalShardCount()) + std::string(" Shards total across all processes)")
							 << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					isItFirstIteraion = false;
					didWeConnect = this->shardMap[packageNew.currentShard]->connect(connectionUrl, this->configManager->getConnectionPort(),
						this->configManager->doWePrintWebSocketErrorMessages(), false);
					if (didWeConnect == false) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection failed for WebSocket [" << packageNew.currentShard << ","
								 << this->configManager->getTotalShardCount() << "]"
								 << " reconnecting in 5 seconds." << DiscordCoreAPI::reset() << endl
								 << endl;
						}
					}
				} while (didWeConnect != true && !this->doWeQuit->load());
				stopWatch.resetTimer();
				this->shardMap[packageNew.currentShard]->currentState.store(WebSocketState::Upgrading);
				std::string sendString{};
				sendString = "GET /?v=10&encoding=";
				sendString += this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf ? "etf" : "json";
				sendString += " HTTP/1.1\r\nHost: " + this->configManager->getConnectionAddress() +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
					DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				ProcessIOResult didWeWrite{ false };
				do {
					if (stopWatch.hasTimePassed()) {
						this->shardMap[packageNew.currentShard]->onClosed();
						this->discordCoreClient->isItSafeToConnect.store(true);
						return;
					}
					std::this_thread::sleep_for(1ms);
					didWeWrite = this->shardMap[packageNew.currentShard]->writeData(sendString, true);
				} while (didWeWrite != ProcessIOResult::No_Error);
				if (didWeWrite != ProcessIOResult::No_Error) {
					this->shardMap[packageNew.currentShard]->onClosed();
					this->discordCoreClient->isItSafeToConnect.store(true);
					return;
				}
				stopWatch.resetTimer();
				while (!this->doWeQuit->load()) {
					if (this->shardMap[packageNew.currentShard]->currentState.load() != WebSocketState::Collecting_Hello) {
						break;
					}
					auto result = this->shardMap[packageNew.currentShard]->processIO(10);
					if (result != ProcessIOResult::No_Error) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" + packageNew.currentShard << ","
								 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
								 << endl;
							this->shardMap[packageNew.currentShard]->onClosed();
						}
						break;
					}
					if (stopWatch.hasTimePassed()) {
						this->shardMap[packageNew.currentShard]->onClosed();
						this->discordCoreClient->isItSafeToConnect.store(true);
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				stopWatch.resetTimer();
				while (!this->doWeQuit->load()) {
					if (this->shardMap[packageNew.currentShard]->currentState.load() == WebSocketState::Authenticated) {
						break;
					}
					if (stopWatch.hasTimePassed()) {
						this->shardMap[packageNew.currentShard]->onClosed();
						this->discordCoreClient->isItSafeToConnect.store(true);
						return;
					}
					auto result = this->shardMap[packageNew.currentShard]->processIO(10);
					if (result != ProcessIOResult::No_Error) {
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" + packageNew.currentShard << ","
								 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
								 << endl;
							this->shardMap[packageNew.currentShard]->onClosed();
						}
						break;
					}
					std::this_thread::sleep_for(1ms);
				}
				this->shardMap[packageNew.currentShard]->areWeConnecting.store(false);
				this->discordCoreClient->isItSafeToConnect.store(true);
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

	void BaseSocketAgent::run(std::stop_token stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !this->doWeQuit->load()) {
				std::unique_lock lock{ this->mutex };
				auto result = SSLClient::processIO(this->shardMap);
				for (auto& valueNew: result) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" << static_cast<WebSocketSSLShard*>(valueNew)->shard[0] << ","
							 << this->configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
							 << endl;
					}
				}
				bool areWeConnected{ false };
				for (auto& [key, value]: this->shardMap) {
					if (value->connections) {
						DiscordCoreAPI::ConnectionPackage connectionData = *value->connections;
						value->connections.reset(nullptr);
						this->connect(connectionData);
					}
					if (value->areWeStillConnected()) {
						value->checkForAndSendHeartBeat();
						areWeConnected = true;
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