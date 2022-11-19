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

	const uint16_t webSocketMaxPayloadLengthLarge{ 65535u };
	const uint8_t webSocketPayloadLengthMagicLarge{ 126u };
	const uint8_t webSocketPayloadLengthMagicHuge{ 127u };
	const uint8_t maxHeaderSize{ sizeof(uint64_t) + 2u };
	const uint8_t webSocketMaxPayloadLengthSmall{ 125u };
	const uint8_t webSocketFinishBit{ (1u << 7u) };
	const uint8_t webSocketMaskBit{ (1u << 7u) };

	EventConverter::EventConverter(std::string newEvent) {
		this->eventValue = newEvent;
	}

	EventConverter::operator int32_t() {
		if (this->eventValue == "READY") {
			return 1;
		} else if (this->eventValue == "RESUMED") {
			return 2;
		} else if (this->eventValue == "APPLICATION_COMMAND_PERMISSIONS_UPDATE") {
			return 3;
		} else if (this->eventValue == "AUTO_MODERATION_RULE_CREATE") {
			return 4;
		} else if (this->eventValue == "AUTO_MODERATION_RULE_UPDATE") {
			return 5;
		} else if (this->eventValue == "AUTO_MODERATION_RULE_DELETE") {
			return 6;
		} else if (this->eventValue == "AUTO_MODERATION_ACTION_EXECUTION") {
			return 7;
		} else if (this->eventValue == "CHANNEL_CREATE") {
			return 8;
		} else if (this->eventValue == "CHANNEL_UPDATE") {
			return 9;
		} else if (this->eventValue == "CHANNEL_DELETE") {
			return 10;
		} else if (this->eventValue == "CHANNEL_PINS_UPDATE") {
			return 11;
		} else if (this->eventValue == "THREAD_CREATE") {
			return 12;
		} else if (this->eventValue == "THREAD_UPDATE") {
			return 13;
		} else if (this->eventValue == "THREAD_DELETE") {
			return 14;
		} else if (this->eventValue == "THREAD_LIST_SYNC") {
			return 15;
		} else if (this->eventValue == "THREAD_MEMBER_UPDATE") {
			return 16;
		} else if (this->eventValue == "THREAD_MEMBERS_UPDATE") {
			return 17;
		} else if (this->eventValue == "GUILD_CREATE") {
			return 18;
		} else if (this->eventValue == "GUILD_UPDATE") {
			return 19;
		} else if (this->eventValue == "GUILD_DELETE") {
			return 20;
		} else if (this->eventValue == "GUILD_BAN_ADD") {
			return 21;
		} else if (this->eventValue == "GUILD_BAN_REMOVE") {
			return 22;
		} else if (this->eventValue == "GUILD_EMOJIS_UPDATE") {
			return 23;
		} else if (this->eventValue == "GUILD_STICKERS_UPDATE") {
			return 24;
		} else if (this->eventValue == "GUILD_INTEGRATIONS_UPDATE") {
			return 25;
		} else if (this->eventValue == "GUILD_MEMBER_ADD") {
			return 26;
		} else if (this->eventValue == "GUILD_MEMBER_REMOVE") {
			return 27;
		} else if (this->eventValue == "GUILD_MEMBER_UPDATE") {
			return 28;
		} else if (this->eventValue == "GUILD_MEMBERS_CHUNK") {
			return 29;
		} else if (this->eventValue == "GUILD_ROLE_CREATE") {
			return 30;
		} else if (this->eventValue == "GUILD_ROLE_UPDATE") {
			return 31;
		} else if (this->eventValue == "GUILD_ROLE_DELETE") {
			return 32;
		} else if (this->eventValue == "GUILD_SCHEDULED_EVENT_CREATE") {
			return 33;
		} else if (this->eventValue == "GUILD_SCHEDULED_EVENT_UPDATE") {
			return 34;
		} else if (this->eventValue == "GUILD_SCHEDULED_EVENT_DELETE") {
			return 35;
		} else if (this->eventValue == "GUILD_SCHEDULED_EVENT_USER_ADD") {
			return 36;
		} else if (this->eventValue == "GUILD_SCHEDULED_EVENT_USER_REMOVE") {
			return 37;
		} else if (this->eventValue == "INTEGRATION_CREATE") {
			return 38;
		} else if (this->eventValue == "INTEGRATION_UPDATE") {
			return 39;
		} else if (this->eventValue == "INTEGRATION_DELETE") {
			return 40;
		} else if (this->eventValue == "INTERACTION_CREATE") {
			return 41;
		} else if (this->eventValue == "INVITE_CREATE") {
			return 42;
		} else if (this->eventValue == "INVITE_DELETE") {
			return 43;
		} else if (this->eventValue == "MESSAGE_CREATE") {
			return 44;
		} else if (this->eventValue == "MESSAGE_UPDATE") {
			return 45;
		} else if (this->eventValue == "MESSAGE_DELETE") {
			return 46;
		} else if (this->eventValue == "MESSAGE_DELETE_BULK") {
			return 47;
		} else if (this->eventValue == "MESSAGE_REACTION_ADD") {
			return 48;
		} else if (this->eventValue == "MESSAGE_REACTION_REMOVE") {
			return 49;
		} else if (this->eventValue == "MESSAGE_REACTION_REMOVE_ALL") {
			return 50;
		} else if (this->eventValue == "MESSAGE_REACTION_REMOVE_EMOJI") {
			return 51;
		} else if (this->eventValue == "PRESENCE_UPDATE") {
			return 52;
		} else if (this->eventValue == "STAGE_INSTANCE_CREATE") {
			return 53;
		} else if (this->eventValue == "STAGE_INSTANCE_UPDATE") {
			return 54;
		} else if (this->eventValue == "STAGE_INSTANCE_DELETE") {
			return 55;
		} else if (this->eventValue == "TYPING_START") {
			return 56;
		} else if (this->eventValue == "USER_UPDATE") {
			return 57;
		} else if (this->eventValue == "VOICE_STATE_UPDATE") {
			return 58;
		} else if (this->eventValue == "VOICE_SERVER_UPDATE") {
			return 59;
		} else if (this->eventValue == "WEBHOOKS_UPDATE") {
			return 60;
		} else {
			return 0;
		}
	}

	WebSocketCore::WebSocketCore(DiscordCoreAPI::ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew) {
		this->wsType = typeOfWebSocketNew;
		this->configManager = configManagerNew;
	}

	bool WebSocketCore::connect(const std::string& baseUrl, const std::string& relativePath, const uint16_t portNew, bool doWePrintErrorsNew,
		bool areWeAStandaloneSocketNew) noexcept {
		SSLClient::connect(baseUrl, portNew, doWePrintErrorsNew, areWeAStandaloneSocket);
		DiscordCoreAPI::StopWatch stopWatch{ 5s };
		std::string sendString{ "GET " + relativePath + " HTTP/1.1\r\nHost: " + baseUrl +
			"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: "
			"Upgrade\r\nSec-WebSocket-Key: " +
			DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n" };
		size_t readOrWrittenBytes{};
		this->writeData(sendString, true);

		std::string_view buffer{};
		do {
			if (stopWatch.hasTimePassed()) {
				return false;
			}
			this->processIO(10);
			buffer = this->getInputBuffer();
			this->parseConnectionHeaders(buffer);
			std::this_thread::sleep_for(1ms);
		} while (buffer.size() == 0);
		return true;
	}

	void WebSocketCore::parseConnectionHeaders(std::string_view stringNew) noexcept {
		if (this->areWeStillConnected() && this->currentState.load() == WebSocketState::Upgrading) {
			auto theFindValue = stringNew.find("\r\n\r\n");
			if (theFindValue != std::string::npos) {
				this->currentMessage.clear();
				this->currentState.store(WebSocketState::Collecting_Hello);
				return;
			}
		}
		return;
	}

	void WebSocketCore::createHeader(std::string& outBuffer, WebSocketOpCode opCode) noexcept {
		size_t originalSize{ outBuffer.size() };
		outBuffer.insert(outBuffer.begin(), static_cast<uint8_t>(opCode) | webSocketFinishBit);

		int32_t indexCount{ 0 };
		if (outBuffer.size() <= webSocketMaxPayloadLengthSmall) {
			outBuffer.insert(outBuffer.begin() + 1, static_cast<uint8_t>(originalSize));
			indexCount = 0;
		} else if (outBuffer.size() <= webSocketMaxPayloadLengthLarge) {
			outBuffer.insert(outBuffer.begin() + 1, static_cast<uint8_t>(webSocketPayloadLengthMagicLarge));
			indexCount = 2;
		} else {
			outBuffer.insert(outBuffer.begin() + 1, static_cast<uint8_t>(webSocketPayloadLengthMagicHuge));
			indexCount = 8;
		}
		for (int32_t x = indexCount - 1; x >= 0; x--) {
			outBuffer.insert(outBuffer.begin() + 1 + indexCount - x, static_cast<uint8_t>(originalSize >> x * 8));
		}

		outBuffer[1] |= webSocketMaskBit;
		outBuffer.insert(outBuffer.begin() + 2 + indexCount, 0);
		outBuffer.insert(outBuffer.begin() + 3 + indexCount, 0);
		outBuffer.insert(outBuffer.begin() + 4 + indexCount, 0);
		outBuffer.insert(outBuffer.begin() + 5 + indexCount, 0);
	}

	bool WebSocketCore::sendMessage(std::string& dataToSend, bool priority) noexcept {
		if (dataToSend.size() == 0) {
			return false;
		}
		if (this->configManager->doWePrintWebSocketSuccessMessages()) {
			std::string webSocketTitle = this->wsType == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket";
			cout << DiscordCoreAPI::shiftToBrightBlue()
				 << "Sending " + webSocketTitle + " [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" +
					std::string("'s Message: ")
				 << static_cast<std::string>(dataToSend) << DiscordCoreAPI::reset() << endl
				 << endl;
		}
		ProcessIOResult didWeWrite{ false };
		DiscordCoreAPI::StopWatch stopWatch{ 5000ms };
		do {
			if (stopWatch.hasTimePassed()) {
				this->onClosed();
				return false;
			}
			didWeWrite = this->writeData(dataToSend, priority);
		} while (didWeWrite == ProcessIOResult::Error);
		if (didWeWrite == ProcessIOResult::Error) {
			this->onClosed();
			return false;
		}
		return true;
	}

	bool WebSocketCore::checkForAndSendHeartBeat(bool isImmediate) noexcept {
		if ((this->currentState.load() == WebSocketState::Authenticated && this->heartBeatStopWatch.hasTimePassed() &&
				this->haveWeReceivedHeartbeatAck) ||
			isImmediate) {
			std::string string{};
			DiscordCoreAPI::Jsonifier data{};
			data["d"] = this->lastNumberReceived;
			data["op"] = 1;
			if (this->dataOpCode == WebSocketOpCode::Op_Binary) {
				data.refreshString(DiscordCoreAPI::JsonifierSerializeType::Etf);
			} else {
				data.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
			}
			string = data.operator std::string();
			this->createHeader(string, this->dataOpCode);
			if (!this->sendMessage(string, true)) {
				return false;
			}
			this->haveWeReceivedHeartbeatAck = false;
			this->heartBeatStopWatch.resetTimer();
			return true;
		} else {
			return false;
		}
	}

	void WebSocketCore::handleBuffer() noexcept {
		if (this->currentState != WebSocketState::Upgrading) {
			this->parseMessage();
		}
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
							this->messageLength |= static_cast<uint64_t>(lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift);
						}
						this->messageOffset += 8;
					}
					if (this->currentMessage.size() < this->messageOffset + this->messageLength) {
						return;
					} else {
						if (this->onMessageReceived(
								this->currentMessage[LengthData{ .offSet = this->messageOffset, .length = this->messageLength }])) {
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
					if (this->wsType == WebSocketType::Voice) {
						VoiceWebSocketClose voiceClose{ close };
						closeString = voiceClose.operator std::string();
					} else {
						WebSocketClose wsClose{ close };
						closeString = wsClose.operator std::string();
					}
					if (closeCode) {
						this->areWeResuming = true;
					}
					std::string webSocketTitle = this->wsType == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket";
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed()
							 << webSocketTitle + " [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" +
								" Closed; Code: "
							 << +closeCode << ", " << closeString << DiscordCoreAPI::reset() << endl
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
			throw DiscordCoreAPI::DCAException{ "Failed to allocate the parser's memory." };
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
		std::string string = serializer.operator std::string();
		this->createHeader(string, this->dataOpCode);
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
		string = serializer.operator std::string();
		this->createHeader(string, this->dataOpCode);
		this->areWeCollectingData = true;
		if (!this->sendMessage(string, true)) {
			return;
		}
		DiscordCoreAPI::StopWatch<Milliseconds> stopWatch{ 5500ms };
		while (this->areWeCollectingData) {
			if (stopWatch.hasTimePassed()) {
				break;
			}
			std::this_thread::sleep_for(1ms);
		}
	}

	bool WebSocketSSLShard::onMessageReceived(std::string_view dataNew) noexcept {
		try {
			if (this->areWeStillConnected() && this->currentMessage.size() > 0 && dataNew.size() > 0) {
				std::string payload{};
				simdjson::ondemand::value dValue{};
				WebSocketMessage message{};
				if (this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
					try {
						payload = ErlParser::parseEtfToJson(dataNew);
						payload.reserve(payload.size() + simdjson::SIMDJSON_PADDING);
						if (auto result =
								this->parser.iterate(simdjson::padded_string_view(payload.data(), payload.length(), payload.capacity())).get(dValue);
							result == simdjson::error_code::SUCCESS) {
							message = WebSocketMessage{ dValue };
						}
					} catch (...) {
						if (this->configManager->doWePrintGeneralErrorMessages()) {
							DiscordCoreAPI::reportException("ErlParser::parseEtfToJson()");
							cout << "The Payload: " << dataNew << endl;
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
					if (this->parser.iterate(simdjson::padded_string_view(payload.data(), payload.length(), payload.capacity())).get(dValue) ==
						simdjson::error_code::SUCCESS) {
						message = WebSocketMessage{ dValue };
					}
				}
				if (message.s != 0) {
					this->lastNumberReceived = message.s;
				}
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					cout << DiscordCoreAPI::shiftToBrightGreen()
						 << "Message received from WebSocket [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" +
							std::string(": ")
						 << payload << DiscordCoreAPI::reset() << endl
						 << endl;
				}
				switch (message.op) {
					case 0: {
						if (message.t != "") {
							switch (EventConverter{ message.t }) {
								case 1: {
									ReadyData data{ message.processJsonMessage<ReadyData>(dValue, "d") };
									this->currentState.store(WebSocketState::Authenticated);
									this->sessionId = data.sessionId;
									this->resumeUrl = data.resumeGatewayUrl;
									this->discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ data.user,
										this->discordCoreClient
											->baseSocketAgentsMap[static_cast<int32_t>(floor(static_cast<int32_t>(this->shard[0]) %
												static_cast<int32_t>(this->discordCoreClient->configManager.getTotalShardCount())))]
											.get() };
									DiscordCoreAPI::Users::insertUser(std::move(data.user));
									this->currentReconnectTries = 0;
									break;
								}
								case 2: {
									this->currentState.store(WebSocketState::Authenticated);
									this->currentReconnectTries = 0;
									break;
								}
								case 3: {
									if (this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 4: {
									if (this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
									}
									break;
								}
								case 5: {
									if (this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
									}
									break;
								}
								case 6: {
									if (this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
									}
									break;
								}
								case 7: {
									if (this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
									}
									break;
								}
								case 8: {
									std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnChannelCreationData>(message, dValue)
									};
									if (this->discordCoreClient->eventManager.onChannelCreationEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onChannelCreationEvent(*dataPackage);
									}
									break;
								}
								case 9: {
									std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnChannelUpdateData>(message, dValue)
									};
									if (this->discordCoreClient->eventManager.onChannelUpdateEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onChannelUpdateEvent(*dataPackage);
									}
									break;
								}
								case 10: {
									std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnChannelDeletionData>(message, dValue)
									};
									if (this->discordCoreClient->eventManager.onChannelDeletionEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onChannelDeletionEvent(*dataPackage);
									}
									break;
								}
								case 11: {
									if (this->discordCoreClient->eventManager.onChannelPinsUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 12: {
									if (this->discordCoreClient->eventManager.onThreadCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadCreationData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
									}
									break;
								}
								case 13: {
									if (this->discordCoreClient->eventManager.onThreadUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
									}
									break;
								}
								case 14: {
									if (this->discordCoreClient->eventManager.onThreadDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadDeletionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
									}
									break;
								}
								case 15: {
									if (this->discordCoreClient->eventManager.onThreadListSyncEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadListSyncData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
									}
									break;
								}
								case 16: {
									if (this->discordCoreClient->eventManager.onThreadMemberUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
									}
									break;
								}
								case 17: {
									if (this->discordCoreClient->eventManager.onThreadMembersUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
									}
									break;
								}
								case 18: {
									std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildCreationData>(message, dValue, this->discordCoreClient)
									};
									if (this->discordCoreClient->eventManager.onGuildCreationEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onGuildCreationEvent(*dataPackage);
									}
									break;
								}
								case 19: {
									std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildUpdateData>(message, dValue, this->discordCoreClient)
									};
									if (this->discordCoreClient->eventManager.onGuildUpdateEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onGuildUpdateEvent(*dataPackage);
									}
									break;
								}
								case 20: {
									std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildDeletionData>(message, dValue, this->discordCoreClient)
									};
									if (this->discordCoreClient->eventManager.onGuildDeletionEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onGuildDeletionEvent(*dataPackage);
									}
									break;
								}
								case 21: {
									if (this->discordCoreClient->eventManager.onGuildBanAddEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildBanAddData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
									}
									break;
								}
								case 22: {
									if (this->discordCoreClient->eventManager.onGuildBanRemoveEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
									}
									break;
								}
								case 23: {
									if (this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
									}
									break;
								}
								case 24: {
									if (this->discordCoreClient->eventManager.onGuildStickersUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
									}
									break;
								}
								case 25: {
									if (this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 26: {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildMemberAddData>(message, dValue, this->discordCoreClient)
									};
									if (this->discordCoreClient->eventManager.onGuildMemberAddEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onGuildMemberAddEvent(*dataPackage);
									}
									break;
								}
								case 27: {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>(message, dValue, this->discordCoreClient)
									};
									if (this->discordCoreClient->eventManager.onGuildMemberRemoveEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onGuildMemberRemoveEvent(*dataPackage);
									}
									break;
								}
								case 28: {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>(message, dValue, this->discordCoreClient)
									};
									if (this->discordCoreClient->eventManager.onGuildMemberUpdateEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onGuildMemberUpdateEvent(*dataPackage);
									}
									break;
								}
								case 29: {
									if (this->discordCoreClient->eventManager.onGuildMembersChunkEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
									}
									break;
								}
								case 30: {
									std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnRoleCreationData>(message, dValue)
									};
									if (this->discordCoreClient->eventManager.onRoleCreationEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onRoleCreationEvent(*dataPackage);
									}
									break;
								}
								case 31: {
									std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleUpdateData>(
										message, dValue) };
									if (this->discordCoreClient->eventManager.onRoleUpdateEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onRoleUpdateEvent(*dataPackage);
									}
									break;
								}
								case 32: {
									std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnRoleDeletionData>(message, dValue)
									};
									if (this->discordCoreClient->eventManager.onRoleDeletionEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onRoleDeletionEvent(*dataPackage);
									}
									break;
								}
								case 33: {
									if (this->discordCoreClient->eventManager.onGuildScheduledEventCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventCreationData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
									}
									break;
								}
								case 34: {
									if (this->discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
									}
									break;
								}
								case 35: {
									if (this->discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventDeletionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
									}
									break;
								}
								case 36: {
									if (this->discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserAddData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
									}
									break;
								}
								case 37: {
									if (this->discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
									}
									break;
								}
								case 38: {
									if (this->discordCoreClient->eventManager.onIntegrationCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
									}
									break;
								}
								case 39: {
									if (this->discordCoreClient->eventManager.onIntegrationUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
									}
									break;
								}
								case 40: {
									if (this->discordCoreClient->eventManager.onIntegrationDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
									}
									break;
								}
								case 41: {
									std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnInteractionCreationData>(message, dValue, this->discordCoreClient)
									};
									if (this->discordCoreClient->eventManager.onInteractionCreationEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
									}
									break;
								}
								case 42: {
									if (this->discordCoreClient->eventManager.onInviteCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnInviteCreationData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
									}
									break;
								}
								case 43: {
									if (this->discordCoreClient->eventManager.onInviteDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnInviteDeletionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
									}
									break;
								}
								case 44: {
									std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnMessageCreationData>(message, dValue)
									};
									if (this->discordCoreClient->eventManager.onMessageCreationEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onMessageCreationEvent(*dataPackage);
									}
									break;
								}
								case 45: {
									std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnMessageUpdateData>(message, dValue)
									};
									if (this->discordCoreClient->eventManager.onMessageUpdateEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
									}
									break;
								}
								case 46: {
									if (this->discordCoreClient->eventManager.onMessageDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnMessageDeletionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
									}
									break;
								}
								case 47: {
									if (this->discordCoreClient->eventManager.onMessageDeleteBulkEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
									}
									break;
								}
								case 48: {
									if (this->discordCoreClient->eventManager.onReactionAddEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionAddData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
									}
									break;
								}
								case 49: {
									if (this->discordCoreClient->eventManager.onReactionRemoveEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionRemoveData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
									}
									break;
								}
								case 50: {
									if (this->discordCoreClient->eventManager.onReactionRemoveAllEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
									}
									break;
								}
								case 51: {
									if (this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
									}
									break;
								}
								case 52: {
									if (this->discordCoreClient->eventManager.onPresenceUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
									}
									break;
								}
								case 53: {
									if (this->discordCoreClient->eventManager.onStageInstanceCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
									}
									break;
								}
								case 54: {
									if (this->discordCoreClient->eventManager.onStageInstanceUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
									}
									break;
								}
								case 55: {
									if (this->discordCoreClient->eventManager.onStageInstanceDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
									}
									break;
								}
								case 56: {
									if (this->discordCoreClient->eventManager.onTypingStartEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnTypingStartData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
									}
									break;
								}
								case 57: {
									if (this->discordCoreClient->eventManager.onUserUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnUserUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onUserUpdateEvent(*dataPackage);
									}
									break;
								}
								case 58: {
									std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>(message, dValue, this)
									};
									if (this->discordCoreClient->eventManager.onVoiceStateUpdateEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
									}
									break;
								}
								case 59: {
									std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>(message, dValue, this)
									};
									if (this->discordCoreClient->eventManager.onVoiceServerUpdateEvent.functions.size() > 0) {
										this->discordCoreClient->eventManager.onVoiceServerUpdateEvent(*dataPackage);
									}
									break;
								}
								case 60: {
									if (this->discordCoreClient->eventManager.onWebhookUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>(message, dValue)
										};
										this->discordCoreClient->eventManager.onWebhookUpdateEvent(*dataPackage);
									}
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
								 << "Shard [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" +
									" Reconnecting (Type 7)!"
								 << DiscordCoreAPI::reset() << endl
								 << endl;
						}
						this->areWeResuming = true;
						this->onClosed();
						return true;
					}
					case 9: {
						InvalidSessionData data{ message.processJsonMessage<InvalidSessionData>(dValue, "d") };
						if (this->configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightBlue()
								 << "Shard [" + std::to_string(this->shard[0]) + "," + std::to_string(this->shard[1]) + "]" +
									" Reconnecting (Type 9)!"
								 << DiscordCoreAPI::reset() << endl
								 << endl;
						}
						std::mt19937_64 randomEngine{ static_cast<uint64_t>(HRClock::now().time_since_epoch().count()) };
						int32_t numOfMsToWait = static_cast<int32_t>(
							1000.0f + ((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * static_cast<float>(4000.0f)));
						if (numOfMsToWait <= 5000 && numOfMsToWait > 0) {
							std::this_thread::sleep_for(Milliseconds{ numOfMsToWait });
						}
						if (data.d == true) {
							this->areWeResuming = true;
						} else {
							this->areWeResuming = false;
						}
						this->onClosed();
						return true;
					}
					case 10: {
						HelloData data{ message.processJsonMessage<HelloData>(dValue, "d") };
						if (data.heartbeatInterval != 0) {
							this->areWeHeartBeating = true;
							this->heartBeatStopWatch = DiscordCoreAPI::StopWatch<Milliseconds>{ Milliseconds{ data.heartbeatInterval } };
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
							std::string string = serializer.operator std::string();
							this->createHeader(string, this->dataOpCode);
							if (!this->sendMessage(string, true)) {
								return false;
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
							std::string string = serializer.operator std::string();
							this->createHeader(string, this->dataOpCode);
							if (!this->sendMessage(string, true)) {
								return false;
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
				return true;
			}

		} catch (...) {
			DiscordCoreAPI::reportException("WebSocketSSLShard::onMessageReceived()");
		}
		this->currentMessage.clear();
		this->inputBuffer.clear();
		this->messageLength = 0;
		this->messageOffset = 0;
		return false;
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

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew,
		int32_t currentBaseSocketAgentNew) noexcept {
		this->configManager = &discordCoreClientNew->configManager;
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->discordCoreClient = discordCoreClientNew;
		this->doWeQuit = doWeQuitNew;
		this->taskThread = std::make_unique<std::jthread>([this](std::stop_token token) {
			this->run(token);
		});
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage packageNew) noexcept {
		while (!this->discordCoreClient->isItSafeToConnect.load()) {
			std::this_thread::sleep_for(1ms);
		}
		this->discordCoreClient->isItSafeToConnect.store(false);
		if (packageNew.currentShard != -1) {
			if (!this->shardMap.contains(packageNew.currentShard)) {
				this->shardMap[packageNew.currentShard] =
					std::make_unique<WebSocketSSLShard>(this->discordCoreClient, packageNew.currentShard, this->doWeQuit);
			}
			this->shardMap[packageNew.currentShard]->currentReconnectTries = packageNew.currentReconnectTries;
			this->shardMap[packageNew.currentShard]->currentReconnectTries++;
			std::string connectionUrl =
				packageNew.areWeResuming ? this->shardMap[packageNew.currentShard]->resumeUrl : this->configManager->getConnectionAddress();
			bool isItFirstIteraion{ true };
			bool didWeConnect{};
			DiscordCoreAPI::StopWatch stopWatch{ 5s };
			if (stopWatch.hasTimePassed()) {
				shardMap[packageNew.currentShard]->onClosed();
				return;
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
			std::string relativePath{};
			this->shardMap[packageNew.currentShard]->currentState.store(WebSocketState::Upgrading);
			relativePath = "/?v=10&encoding=";
			relativePath += this->configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf ? "etf" : "json";
			didWeConnect = this->shardMap[packageNew.currentShard]->connect(connectionUrl, relativePath, this->configManager->getConnectionPort(),
				this->configManager->doWePrintWebSocketErrorMessages(), false);
			if (didWeConnect == false) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "Connection failed for WebSocket [" << packageNew.currentShard << ","
						 << this->configManager->getTotalShardCount() << "]"
						 << " reconnecting in 5 seconds." << DiscordCoreAPI::reset() << endl
						 << endl;
				}
			}

			stopWatch.resetTimer();
			while (!this->doWeQuit->load()) {
				if (this->shardMap[packageNew.currentShard]->currentState.load() != WebSocketState::Collecting_Hello) {
					break;
				}
				auto result = this->shardMap[packageNew.currentShard]->processIO(10);
				if (result == ProcessIOResult::Error) {
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
				if (result == ProcessIOResult::Error) {
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
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return this->taskThread.get();
	}

	void BaseSocketAgent::run(std::stop_token token) noexcept {
		while (!token.stop_requested() && !this->doWeQuit->load()) {
			try {
				std::unique_lock lock{ this->accessMutex };
				auto result = SSLClient::processIO(this->shardMap);
				for (auto& valueNew: result) {
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket ["
							 << static_cast<WebSocketSSLShard*>(valueNew)->shard[0] << "," << this->configManager->getTotalShardCount()
							 << "]... reconnecting." << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					static_cast<WebSocketSSLShard*>(valueNew)->onClosed();
				}
				bool areWeConnected{ false };
				for (auto& [key, dValue]: this->shardMap) {
					if (dValue->connections) {
						DiscordCoreAPI::ConnectionPackage connectionData = *dValue->connections;
						dValue->connections.reset(nullptr);
						this->connect(connectionData);
					}
					if (dValue->areWeStillConnected()) {
						if (dValue->checkForAndSendHeartBeat()) {
							DiscordCoreAPI::OnGatewayPingData dataNew{};
							dataNew.timeUntilNextPing = dValue->heartBeatStopWatch.getTotalWaitTime().count();
							this->discordCoreClient->eventManager.onGatewayPingEvent(dataNew);
						}
						areWeConnected = true;
					}
				}
				if (!areWeConnected) {
					std::this_thread::sleep_for(1ms);
				}
			} catch (...) {
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
};