/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute iter and/or
	modify iter under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that iter will be useful,
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
#include <discordcoreapi/JsonSpecializations.hpp>
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
	constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

	EventConverter::EventConverter(std::string newEvent) {
		eventValue = newEvent;
	}

	EventConverter::operator int32_t() {
		if (eventValue == "READY") {
			return 1;
		} else if (eventValue == "RESUMED") {
			return 2;
		} else if (eventValue == "APPLICATION_COMMAND_PERMISSIONS_UPDATE") {
			return 3;
		} else if (eventValue == "AUTO_MODERATION_RULE_CREATE") {
			return 4;
		} else if (eventValue == "AUTO_MODERATION_RULE_UPDATE") {
			return 5;
		} else if (eventValue == "AUTO_MODERATION_RULE_DELETE") {
			return 6;
		} else if (eventValue == "AUTO_MODERATION_ACTION_EXECUTION") {
			return 7;
		} else if (eventValue == "CHANNEL_CREATE") {
			return 8;
		} else if (eventValue == "CHANNEL_UPDATE") {
			return 9;
		} else if (eventValue == "CHANNEL_DELETE") {
			return 10;
		} else if (eventValue == "CHANNEL_PINS_UPDATE") {
			return 11;
		} else if (eventValue == "THREAD_CREATE") {
			return 12;
		} else if (eventValue == "THREAD_UPDATE") {
			return 13;
		} else if (eventValue == "THREAD_DELETE") {
			return 14;
		} else if (eventValue == "THREAD_LIST_SYNC") {
			return 15;
		} else if (eventValue == "THREAD_MEMBER_UPDATE") {
			return 16;
		} else if (eventValue == "THREAD_MEMBERS_UPDATE") {
			return 17;
		} else if (eventValue == "GUILD_CREATE") {
			return 18;
		} else if (eventValue == "GUILD_UPDATE") {
			return 19;
		} else if (eventValue == "GUILD_DELETE") {
			return 20;
		} else if (eventValue == "GUILD_BAN_ADD") {
			return 21;
		} else if (eventValue == "GUILD_BAN_REMOVE") {
			return 22;
		} else if (eventValue == "GUILD_EMOJIS_UPDATE") {
			return 23;
		} else if (eventValue == "GUILD_STICKERS_UPDATE") {
			return 24;
		} else if (eventValue == "GUILD_INTEGRATIONS_UPDATE") {
			return 25;
		} else if (eventValue == "GUILD_MEMBER_ADD") {
			return 26;
		} else if (eventValue == "GUILD_MEMBER_REMOVE") {
			return 27;
		} else if (eventValue == "GUILD_MEMBER_UPDATE") {
			return 28;
		} else if (eventValue == "GUILD_MEMBERS_CHUNK") {
			return 29;
		} else if (eventValue == "GUILD_ROLE_CREATE") {
			return 30;
		} else if (eventValue == "GUILD_ROLE_UPDATE") {
			return 31;
		} else if (eventValue == "GUILD_ROLE_DELETE") {
			return 32;
		} else if (eventValue == "GUILD_SCHEDULED_EVENT_CREATE") {
			return 33;
		} else if (eventValue == "GUILD_SCHEDULED_EVENT_UPDATE") {
			return 34;
		} else if (eventValue == "GUILD_SCHEDULED_EVENT_DELETE") {
			return 35;
		} else if (eventValue == "GUILD_SCHEDULED_EVENT_USER_ADD") {
			return 36;
		} else if (eventValue == "GUILD_SCHEDULED_EVENT_USER_REMOVE") {
			return 37;
		} else if (eventValue == "INTEGRATION_CREATE") {
			return 38;
		} else if (eventValue == "INTEGRATION_UPDATE") {
			return 39;
		} else if (eventValue == "INTEGRATION_DELETE") {
			return 40;
		} else if (eventValue == "INTERACTION_CREATE") {
			return 41;
		} else if (eventValue == "INVITE_CREATE") {
			return 42;
		} else if (eventValue == "INVITE_DELETE") {
			return 43;
		} else if (eventValue == "MESSAGE_CREATE") {
			return 44;
		} else if (eventValue == "MESSAGE_UPDATE") {
			return 45;
		} else if (eventValue == "MESSAGE_DELETE") {
			return 46;
		} else if (eventValue == "MESSAGE_DELETE_BULK") {
			return 47;
		} else if (eventValue == "MESSAGE_REACTION_ADD") {
			return 48;
		} else if (eventValue == "MESSAGE_REACTION_REMOVE") {
			return 49;
		} else if (eventValue == "MESSAGE_REACTION_REMOVE_ALL") {
			return 50;
		} else if (eventValue == "MESSAGE_REACTION_REMOVE_EMOJI") {
			return 51;
		} else if (eventValue == "PRESENCE_UPDATE") {
			return 52;
		} else if (eventValue == "STAGE_INSTANCE_CREATE") {
			return 53;
		} else if (eventValue == "STAGE_INSTANCE_UPDATE") {
			return 54;
		} else if (eventValue == "STAGE_INSTANCE_DELETE") {
			return 55;
		} else if (eventValue == "TYPING_START") {
			return 56;
		} else if (eventValue == "USER_UPDATE") {
			return 57;
		} else if (eventValue == "VOICE_STATE_UPDATE") {
			return 58;
		} else if (eventValue == "VOICE_SERVER_UPDATE") {
			return 59;
		} else if (eventValue == "WEBHOOKS_UPDATE") {
			return 60;
		} else {
			return 0;
		}
	}

	WebSocketCore::WebSocketCore(DiscordCoreAPI::ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew, bool areWeAStandaloneSocketNew) {
		areWeAStandaloneSocket = areWeAStandaloneSocketNew;
		configManager = configManagerNew;
		wsType = typeOfWebSocketNew;
	}

	bool WebSocketCore::connect(const std::string& baseUrlNew, const std::string& relativePath, const uint16_t portNew,
		bool doWePrintErrorsNew) noexcept {
		try {
			tcpConnection = std::make_unique<WebSocketTCPConnection>(baseUrlNew, portNew, doWePrintErrorsNew, this, areWeAStandaloneSocket);
		} catch (...) {
			if (configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("WebSocketCore::connect()");
			}
			return false;
		}
		currentState.store(WebSocketState::Upgrading);
		std::string sendString{ "GET " + relativePath + " HTTP/1.1\r\nHost: " + baseUrlNew +
			"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: " + "Upgrade\r\nSec-WebSocket-Key: " +
			DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n" };
		try {
			tcpConnection->writeData(sendString, true);
		} catch (...) {
			if (configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("WebSocketCore::connect()");
			}
			return false;
		}
		DiscordCoreAPI::StopWatch stopWatch{ 5s };
		do {
			if (stopWatch.hasTimePassed()) {
				return false;
			}
			try {
				tcpConnection->processIO(10);
			} catch (...) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("WebSocketCore::connect()");
				}
				return false;
			}

			std::this_thread::sleep_for(1ms);
		} while (currentState.load() == WebSocketState::Upgrading);
		return true;
	}

	WebSocketTCPConnection::WebSocketTCPConnection(const std::string& baseUrlNew, uint16_t portNew, bool doWePrintErrors, WebSocketCore* ptrNew,
		bool areWeAStandaloneSocket)
		: TCPConnection{ baseUrlNew, portNew, doWePrintErrors, areWeAStandaloneSocket } {
		ptr = ptrNew;
	};

	void WebSocketTCPConnection::disconnect() noexcept {
		socket = INVALID_SOCKET;
		ssl = nullptr;
		reset();
	}

	void WebSocketCore::createHeader(std::string& outBuffer, WebSocketOpCode opCode) noexcept {
		uint64_t originalSize{ outBuffer.size() };
		outBuffer.insert(outBuffer.begin(), static_cast<uint8_t>(opCode) | webSocketMaskBit);

		int32_t indexCount{};
		if (originalSize <= webSocketMaxPayloadLengthSmall) {
			outBuffer.insert(outBuffer.begin() + 1, static_cast<uint8_t>(originalSize));
			indexCount = 0;
		} else if (originalSize <= webSocketMaxPayloadLengthLarge) {
			outBuffer.insert(outBuffer.begin() + 1, static_cast<uint8_t>(webSocketPayloadLengthMagicLarge));
			indexCount = 2;
		} else {
			outBuffer.insert(outBuffer.begin() + 1, static_cast<uint8_t>(webSocketPayloadLengthMagicHuge));
			indexCount = 8;
		}
		for (int32_t x = indexCount - 1; x >= 0; x--) {
			outBuffer.insert(outBuffer.begin() + 1 + indexCount - x, static_cast<uint8_t>(originalSize >> (x * 8)));
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
		if (configManager->doWePrintWebSocketSuccessMessages()) {
			std::string webSocketTitle = wsType == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket";
			cout << DiscordCoreAPI::shiftToBrightBlue()
				 << "Sending " + webSocketTitle + " [" + std::to_string(shard[0]) + "," + std::to_string(shard[1]) + "]" + std::string("'s Message: ")
				 << static_cast<std::string>(dataToSend) << DiscordCoreAPI::reset() << endl
				 << endl;
		}
		try {
			tcpConnection->writeData(dataToSend, priority);
		} catch (...) {
			if (configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("WebSocketCore::sendMessage()");
			}
			onClosed();
			return false;
		}
		return true;
	}

	void WebSocketCore::parseConnectionHeaders(std::string_view stringNew) noexcept {
		if (tcpConnection->areWeStillConnected() && currentState.load() == WebSocketState::Upgrading) {
			auto theFindValue = stringNew.find("\r\n\r\n");
			if (theFindValue != std::string::npos) {
				currentMessage.clear();
				currentState.store(WebSocketState::Collecting_Hello);
				return;
			}
		}
	}

	bool WebSocketCore::checkForAndSendHeartBeat(bool isImmediate) noexcept {
		if ((currentState.load() == WebSocketState::Authenticated && heartBeatStopWatch.hasTimePassed() && haveWeReceivedHeartbeatAck) ||
			isImmediate) {
			std::string string{};
			if (dataOpCode == WebSocketOpCode::Op_Binary) {
				EtfSerializer data{};
				data["d"] = lastNumberReceived;
				data["op"] = 1;
				data.refreshString();
				string = data.operator std::string();
			} else {
				WebSocketMessageData<int32_t> message{};
				message.d = lastNumberReceived;
				message.op = 1;
				parser.serializeJson(message, string);
			}
			haveWeReceivedHeartbeatAck = false;
			heartBeatStopWatch.resetTimer();
			createHeader(string, dataOpCode);
			return sendMessage(string, true);
		}
		return false;
	}

	bool WebSocketCore::parseMessage() noexcept {
		if (currentMessage.size() < 4) {
			return false;
		} else {
			WebSocketOpCode opcode = static_cast<WebSocketOpCode>(currentMessage[0] & ~webSocketMaskBit);
			switch (opcode) {
				case WebSocketOpCode::Op_Continuation:
				case WebSocketOpCode::Op_Text:
				case WebSocketOpCode::Op_Binary:
				case WebSocketOpCode::Op_Ping:
				case WebSocketOpCode::Op_Pong: {
					uint8_t length01 = currentMessage[1];
					uint32_t messageOffset = 2;

					if (length01 & webSocketMaskBit) {
						length01 &= ~webSocketMaskBit;
						messageOffset += 2;
						return true;
					}

					uint64_t length = length01;

					if (length01 == webSocketPayloadLengthMagicLarge) {
						if (currentMessage.size() < 8) {
							return false;
						}

						uint8_t len2 = static_cast<uint8_t>(currentMessage[2]);
						uint8_t len3 = static_cast<uint8_t>(currentMessage[3]);
						length = (len2 << 8) | len3;

						messageOffset += 2;
					} else if (length01 == webSocketPayloadLengthMagicHuge) {
						if (currentMessage.size() < 10) {
							return false;
						}
						length = 0;
						for (int32_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
							uint8_t l = static_cast<uint8_t>(currentMessage[x]);
							length |= static_cast<uint64_t>(l & 0xff) << shift;
						}
						messageOffset += 8;
					}

					if (currentMessage.size() < messageOffset + length) {
						return false;
					}

					if (opcode == WebSocketOpCode::Op_Ping || opcode == WebSocketOpCode::Op_Pong) {
						return false;
					} else {
						onMessageReceived(currentMessage.stringView(messageOffset, length));
					}
					currentMessage.erase(messageOffset + length);

					return true;
				} break;
				case WebSocketOpCode::Op_Close: {
					uint16_t closeValue = currentMessage[2] & 0xff;
					closeValue <<= 8;
					closeValue |= currentMessage[3] & 0xff;
					std::string closeString{};
					if (wsType == WebSocketType::Voice) {
						VoiceWebSocketClose voiceClose{ closeValue };
						closeString = voiceClose.operator std::string();
					} else {
						WebSocketClose wsClose{ closeValue };
						closeString = wsClose.operator std::string();
					}
					std::string webSocketTitle = wsType == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket";
					if (configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed()
							 << webSocketTitle + " [" + std::to_string(shard[0]) + "," + std::to_string(shard[1]) + "]" + " Closed; Code: "
							 << +closeCode << ", " << closeString << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					return false;
				} break;

				default: {
					return false;
				} break;
			}
		}
		return false;
	}

	void WebSocketTCPConnection::handleBuffer() noexcept {
		if (ptr->currentState.load() == WebSocketState::Upgrading) {
			auto inputBuffer = getInputBuffer();
			ptr->parseConnectionHeaders({ reinterpret_cast<const char*>(inputBuffer.data()), inputBuffer.size() });
		} else {
			auto inputBuffer = getInputBuffer();
			ptr->currentMessage.writeData(reinterpret_cast<const char*>(inputBuffer.data()), inputBuffer.size());
			while (ptr->parseMessage()) {
				std::this_thread::sleep_for(1us);
			}
		}
	}

	WebSocketClient::WebSocketClient(DiscordCoreAPI::DiscordCoreClient* client, int32_t currentShardNew, std::atomic_bool* doWeQuitNew,
		bool areWeAStandaloneSocketNew)
		: WebSocketCore(&client->configManager, WebSocketType::Normal, areWeAStandaloneSocketNew) {
		configManager = &client->configManager;
		shard[0] = currentShardNew;
		discordCoreClient = client;
		doWeQuit = doWeQuitNew;
		if (discordCoreClient) {
			shard[1] = discordCoreClient->configManager.getTotalShardCount();
			if (discordCoreClient->configManager.getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
				dataOpCode = WebSocketOpCode::Op_Binary;
			} else {
				dataOpCode = WebSocketOpCode::Op_Text;
			}
		}
	}

	void WebSocketClient::getVoiceConnectionData(const DiscordCoreAPI::VoiceConnectInitData& doWeCollect) noexcept {
		while (currentState.load() != WebSocketState::Authenticated) {
			std::this_thread::sleep_for(1ms);
		}
		WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateDataDC> data01{};
		data01.excludedKeys.emplace("t");
		data01.excludedKeys.emplace("s");
		data01.d.channelId = 0;
		data01.d.guildId = doWeCollect.guildId;
		data01.d.selfDeaf = doWeCollect.selfDeaf;
		data01.d.selfMute = doWeCollect.selfMute;
		data01.op = 4;
		userId = doWeCollect.userId;

		WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData> data02{};
		data02.excludedKeys.emplace("t");
		data02.excludedKeys.emplace("s");
		data02.d.channelId = doWeCollect.channelId;
		data02.d.guildId = doWeCollect.guildId;
		data02.d.selfDeaf = doWeCollect.selfDeaf;
		data02.d.selfMute = doWeCollect.selfMute;
		data02.op = 4;

		auto botUser = DiscordCoreAPI::DiscordCoreClient::getBotUser();
		DiscordCoreAPI::GuildMember guildMember{};
		DiscordCoreAPI::GuildMembers::getCachedGuildMember({ .guildMemberId = botUser.id, .guildId = doWeCollect.guildId });
		std::string string{};
		auto serializer = data01.d.operator EtfSerializer();
		if (dataOpCode == WebSocketOpCode::Op_Binary) {
			serializer.refreshString();
			string = serializer.operator std::string();
		} else {
			parser.serializeJson<true>(data01, string);
		}
		createHeader(string, dataOpCode);
		if (!sendMessage(string, true)) {
			return;
		}
		if (DiscordCoreAPI::Snowflake{ doWeCollect.channelId } == 0) {
			return;
		}
		serializer = data02.d.operator EtfSerializer();
		if (dataOpCode == WebSocketOpCode::Op_Binary) {
			serializer.refreshString();
			string = serializer.operator std::string();
		} else {
			parser.serializeJson<true>(data02, string);
		}
		createHeader(string, dataOpCode);
		areWeCollectingData = true;
		if (!sendMessage(string, true)) {
			return;
		}
		DiscordCoreAPI::StopWatch<Milliseconds> stopWatch{ 5500ms };
		while (areWeCollectingData) {
			if (stopWatch.hasTimePassed()) {
				break;
			}
			std::this_thread::sleep_for(1ms);
		}
	}

	void validateJson(std::string& outputData, std::string_view input) {
		for (auto iter = input.begin(); iter != input.end();) {
			switch (*iter) {
				case '\\': {
					if ((*(iter + 1)) == '\\') {
						++iter;
						++iter;
						continue;
					} else {
						outputData.push_back(*iter);
						++iter;
					}
					break;
				}
				default: {
					outputData.push_back(*iter);
					++iter;
				}
			}
		}
	}

	bool WebSocketClient::onMessageReceived(std::string_view dataNew) noexcept {
		std::string payload{};
		try {
			if (tcpConnection->areWeStillConnected() && currentMessage.size() > 0 && dataNew.size() > 0) {
				WebSocketMessage message{};
				if (configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
					try {
						payload = EtfParser::parseEtfToJson(dataNew);
						parser.parseJson<true>(message, payload);
					} catch (...) {
						if (configManager->doWePrintGeneralErrorMessages()) {
							DiscordCoreAPI::reportException("EtfParser::parseEtfToJson()");
							cout << "The Payload: " << payload << endl;
						}
						tcpConnection->getInputBuffer();
						currentMessage.clear();
						return false;
					}
				} else {
					validateJson(payload, dataNew);
					parser.parseJson<true>(message, payload);
				}
				if (message.s != 0) {
					lastNumberReceived = message.s;
				}
				if (configManager->doWePrintWebSocketSuccessMessages()) {
					cout << DiscordCoreAPI::shiftToBrightGreen()
						 << "Message received from WebSocket [" + std::to_string(shard[0]) + "," + std::to_string(shard[1]) + "]" + std::string(": ")
						 << payload << DiscordCoreAPI::reset() << endl
						 << endl;
				}
				switch (static_cast<WebSocketOpCodes>(message.op)) {
					case WebSocketOpCodes::Dispatch: {
						if (message.t != "") {
							switch (EventConverter{ message.t }) {
								case 1: {
									if (configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
										WebSocketMessageData<ReadyData<DiscordCoreAPI::TextFormat::Etf>> data{};
										currentState.store(WebSocketState::Authenticated);
										parser.parseJson<true>(data, payload);
										sessionId = data.d.sessionId;
										resumeUrl =
											data.d.resumeGatewayUrl.substr(data.d.resumeGatewayUrl.find("wss://") + std::string("wss://").size());
										discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ data.d.user,
											discordCoreClient
												->baseSocketAgentsMap[static_cast<int32_t>(floor(static_cast<int32_t>(shard[0]) %
													static_cast<int32_t>(discordCoreClient->configManager.getTotalShardCount())))]
												.get() };
										DiscordCoreAPI::Users::insertUser(std::move(data.d.user));
									} else {
										WebSocketMessageData<ReadyData<DiscordCoreAPI::TextFormat::Json>> data{};
										currentState.store(WebSocketState::Authenticated);
										parser.parseJson<true>(data, payload);
										sessionId = data.d.sessionId;
										resumeUrl =
											data.d.resumeGatewayUrl.substr(data.d.resumeGatewayUrl.find("wss://") + std::string("wss://").size());
										discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ data.d.user,
											discordCoreClient
												->baseSocketAgentsMap[static_cast<int32_t>(floor(static_cast<int32_t>(shard[0]) %
													static_cast<int32_t>(discordCoreClient->configManager.getTotalShardCount())))]
												.get() };
										DiscordCoreAPI::Users::insertUser(std::move(data.d.user));
									}
									currentReconnectTries = 0;
									break;
								}
								case 2: {
									currentState.store(WebSocketState::Authenticated);
									currentReconnectTries = 0;
									break;
								}
								case 3: {
									if (discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 4: {
									if (discordCoreClient->eventManager.onAutoModerationRuleCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleCreationData>(parser, payload)
										};
										discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
									}
									break;
								}
								case 5: {
									if (discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
									}
									break;
								}
								case 6: {
									if (discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>(parser, payload)
										};
										discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
									}
									break;
								}
								case 7: {
									if (discordCoreClient->eventManager.onAutoModerationActionExecutionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnAutoModerationActionExecutionData>(parser, payload)
										};
										discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
									}
									break;
								}
								case 8: {
									std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnChannelCreationData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onChannelCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onChannelCreationEvent(*dataPackage);
									}
									break;
								}
								case 9: {
									std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnChannelUpdateData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onChannelUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onChannelUpdateEvent(*dataPackage);
									}
									break;
								}
								case 10: {
									std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnChannelDeletionData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onChannelDeletionEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onChannelDeletionEvent(*dataPackage);
									}
									break;
								}
								case 11: {
									if (discordCoreClient->eventManager.onChannelPinsUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnChannelPinsUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 12: {
									if (discordCoreClient->eventManager.onThreadCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadCreationData>(parser, payload)
										};
										discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
									}
									break;
								}
								case 13: {
									if (discordCoreClient->eventManager.onThreadUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
									}
									break;
								}
								case 14: {
									if (discordCoreClient->eventManager.onThreadDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadDeletionData>(parser, payload)
										};
										discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
									}
									break;
								}
								case 15: {
									if (discordCoreClient->eventManager.onThreadListSyncEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadListSyncData>(parser, payload)
										};
										discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
									}
									break;
								}
								case 16: {
									if (discordCoreClient->eventManager.onThreadMemberUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadMemberUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
									}
									break;
								}
								case 17: {
									if (discordCoreClient->eventManager.onThreadMembersUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnThreadMembersUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
									}
									break;
								}
								case 18: {
									std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildCreationData>(parser, payload, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildCreationEvent(*dataPackage);
									}
									break;
								}
								case 19: {
									std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildUpdateData>(parser, payload, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildUpdateEvent(*dataPackage);
									}
									break;
								}
								case 20: {
									std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildDeletionData>(parser, payload, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildDeletionEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildDeletionEvent(*dataPackage);
									}
									break;
								}
								case 21: {
									if (discordCoreClient->eventManager.onGuildBanAddEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildBanAddData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
									}
									break;
								}
								case 22: {
									if (discordCoreClient->eventManager.onGuildBanRemoveEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildBanRemoveData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
									}
									break;
								}
								case 23: {
									if (discordCoreClient->eventManager.onGuildEmojisUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildEmojisUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
									}
									break;
								}
								case 24: {
									if (discordCoreClient->eventManager.onGuildStickersUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildStickersUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
									}
									break;
								}
								case 25: {
									if (discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 26: {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildMemberAddData>(parser, payload, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildMemberAddEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildMemberAddEvent(*dataPackage);
									}
									break;
								}
								case 27: {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildMemberRemoveData>(parser, payload, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildMemberRemoveEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildMemberRemoveEvent(*dataPackage);
									}
									break;
								}
								case 28: {
									std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnGuildMemberUpdateData>(parser, payload, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildMemberUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildMemberUpdateEvent(*dataPackage);
									}
									break;
								}
								case 29: {
									if (discordCoreClient->eventManager.onGuildMembersChunkEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildMembersChunkData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
									}
									break;
								}
								case 30: {
									std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnRoleCreationData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onRoleCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onRoleCreationEvent(*dataPackage);
									}
									break;
								}
								case 31: {
									std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnRoleUpdateData>(
										parser, payload) };
									if (discordCoreClient->eventManager.onRoleUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onRoleUpdateEvent(*dataPackage);
									}
									break;
								}
								case 32: {
									std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnRoleDeletionData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onRoleDeletionEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onRoleDeletionEvent(*dataPackage);
									}
									break;
								}
								case 33: {
									if (discordCoreClient->eventManager.onGuildScheduledEventCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventCreationData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
									}
									break;
								}
								case 34: {
									if (discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
									}
									break;
								}
								case 35: {
									if (discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventDeletionData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
									}
									break;
								}
								case 36: {
									if (discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserAddData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
									}
									break;
								}
								case 37: {
									if (discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData>(parser, payload)
										};
										discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
									}
									break;
								}
								case 38: {
									if (discordCoreClient->eventManager.onIntegrationCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnIntegrationCreationData>(parser, payload)
										};
										discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
									}
									break;
								}
								case 39: {
									if (discordCoreClient->eventManager.onIntegrationUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnIntegrationUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
									}
									break;
								}
								case 40: {
									if (discordCoreClient->eventManager.onIntegrationDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnIntegrationDeletionData>(parser, payload)
										};
										discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
									}
									break;
								}
								case 41: {
									std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnInteractionCreationData>(parser, payload, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onInteractionCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
									}
									break;
								}
								case 42: {
									if (discordCoreClient->eventManager.onInviteCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnInviteCreationData>(parser, payload)
										};
										discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
									}
									break;
								}
								case 43: {
									if (discordCoreClient->eventManager.onInviteDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnInviteDeletionData>(parser, payload)
										};
										discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
									}
									break;
								}
								case 44: {
									std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnMessageCreationData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onMessageCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onMessageCreationEvent(*dataPackage);
									}
									break;
								}
								case 45: {
									std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnMessageUpdateData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onMessageUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
									}
									break;
								}
								case 46: {
									if (discordCoreClient->eventManager.onMessageDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnMessageDeletionData>(parser, payload)
										};
										discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
									}
									break;
								}
								case 47: {
									if (discordCoreClient->eventManager.onMessageDeleteBulkEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>(parser, payload)
										};
										discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
									}
									break;
								}
								case 48: {
									if (discordCoreClient->eventManager.onReactionAddEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionAddData>(parser, payload)
										};
										discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
									}
									break;
								}
								case 49: {
									if (discordCoreClient->eventManager.onReactionRemoveEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionRemoveData>(parser, payload)
										};
										discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
									}
									break;
								}
								case 50: {
									if (discordCoreClient->eventManager.onReactionRemoveAllEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>(parser, payload)
										};
										discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
									}
									break;
								}
								case 51: {
									if (discordCoreClient->eventManager.onReactionRemoveEmojiEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>(parser, payload)
										};
										discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
									}
									break;
								}
								case 52: {
									std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>(parser, payload)
									};
									if (discordCoreClient->eventManager.onPresenceUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
									}
									break;
								}
								case 53: {
									if (discordCoreClient->eventManager.onStageInstanceCreationEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>(parser, payload)
										};
										discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
									}
									break;
								}
								case 54: {
									if (discordCoreClient->eventManager.onStageInstanceUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
									}
									break;
								}
								case 55: {
									if (discordCoreClient->eventManager.onStageInstanceDeletionEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>(parser, payload)
										};
										discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
									}
									break;
								}
								case 56: {
									if (discordCoreClient->eventManager.onTypingStartEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnTypingStartData>(parser, payload)
										};
										discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
									}
									break;
								}
								case 57: {
									if (discordCoreClient->eventManager.onUserUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnUserUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onUserUpdateEvent(*dataPackage);
									}
									break;
								}
								case 58: {
									std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>(parser, payload, this)
									};
									if (discordCoreClient->eventManager.onVoiceStateUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
									}
									break;
								}
								case 59: {
									std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{
										std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>(parser, payload, this)
									};
									if (discordCoreClient->eventManager.onVoiceServerUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onVoiceServerUpdateEvent(*dataPackage);
									}
									break;
								}
								case 60: {
									if (discordCoreClient->eventManager.onWebhookUpdateEvent.functions.size() > 0) {
										std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{
											std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>(parser, payload)
										};
										discordCoreClient->eventManager.onWebhookUpdateEvent(*dataPackage);
									}
									break;
								}
							}
						}
						break;
					}
					case WebSocketOpCodes::Heartbeat: {
						if (!checkForAndSendHeartBeat(true)) {
							return false;
						}
						break;
					}
					case WebSocketOpCodes::Reconnect: {
						if (configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightBlue()
								 << "Shard [" + std::to_string(shard[0]) + "," + std::to_string(shard[1]) + "]" + " Reconnecting (Type 7)!"
								 << DiscordCoreAPI::reset() << endl
								 << endl;
						}
						areWeResuming = true;
						onClosed();
						return true;
					}
					case WebSocketOpCodes::Invalid_Session: {
						WebSocketMessageData<InvalidSessionData> data{};
						parser.parseJson<true>(data, payload);
						if (configManager->doWePrintWebSocketErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightBlue()
								 << "Shard [" + std::to_string(shard[0]) + "," + std::to_string(shard[1]) + "]" + " Reconnecting (Type 9)!"
								 << DiscordCoreAPI::reset() << endl
								 << endl;
						}
						std::mt19937_64 randomEngine{ static_cast<uint64_t>(HRClock::now().time_since_epoch().count()) };
						int32_t numOfMsToWait = static_cast<int32_t>(1000.0f +
							((static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max())) * static_cast<double>(4000.0f)));
						if (numOfMsToWait <= 5000 && numOfMsToWait > 0) {
							std::this_thread::sleep_for(Milliseconds{ numOfMsToWait });
						}
						if (data.d == true) {
							areWeResuming = true;
						} else {
							areWeResuming = false;
						}
						onClosed();
						return true;
					}
					case WebSocketOpCodes::Hello: {
						WebSocketMessageData<HelloData> data{};
						parser.parseJson<true>(data, payload);
						if (data.d.heartbeatInterval != 0) {
							areWeHeartBeating = true;
							heartBeatStopWatch = DiscordCoreAPI::StopWatch<Milliseconds>{ Milliseconds{ data.d.heartbeatInterval } };
							heartBeatStopWatch.resetTimer();
							haveWeReceivedHeartbeatAck = true;
						}
						if (areWeResuming) {
							WebSocketMessageData<WebSocketResumeData> data{};
							data.d.botToken = configManager->getBotToken();
							data.d.lastNumberReceived = lastNumberReceived;
							data.d.sessionId = sessionId;
							data.op = 6;
							std::string string{};
							if (dataOpCode == WebSocketOpCode::Op_Binary) {
								auto serializer = data.d.operator EtfSerializer();
								serializer.refreshString();
								string = serializer.operator std::string();
							} else {
								parser.serializeJson(data, string);
							}
							createHeader(string, dataOpCode);
							currentState.store(WebSocketState::Sending_Identify);
							if (!sendMessage(string, true)) {
								return false;
							}
						} else {
							WebSocketMessageData<WebSocketIdentifyData> data{};
							data.d.botToken = configManager->getBotToken();
							data.d.shard[0] = shard[0];
							data.d.shard[1] = shard[1];
							data.d.intents = static_cast<int64_t>(configManager->getGatewayIntents());
							data.d.presence = configManager->getPresenceData();
							data.op = 2;
							std::string string{};
							if (dataOpCode == WebSocketOpCode::Op_Binary) {
								auto serializer = data.d.operator EtfSerializer();
								serializer.refreshString();
								string = serializer.operator std::string();
							} else {
								parser.serializeJson(data, string);
							}
							createHeader(string, dataOpCode);
							currentState.store(WebSocketState::Sending_Identify);
							if (!sendMessage(string, true)) {
								return false;
							}
						}
						break;
					}
					case WebSocketOpCodes::Heartbeat_ACK: {
						haveWeReceivedHeartbeatAck = true;
						break;
					}
					default: {
						break;
					}
				}
				return true;
			}

		} catch (...) {
			if (configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("WebSocketClient::onMessageReceived()");
				std::cout << "The payload: " << payload << std::endl;
			}
		}
		tcpConnection->reset();
		return false;
	}

	void WebSocketCore::disconnect() noexcept {
		if (tcpConnection && tcpConnection->areWeStillConnected()) {
			std::string payload{ "\x03\xE8" };
			createHeader(payload, WebSocketOpCode::Op_Close);
			try {
				tcpConnection->writeData(payload, true);
			} catch (...) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("WebSocketCore::disconnect()");
				}
			}
			tcpConnection->disconnect();
			currentState.store(WebSocketState::Disconnected);
			areWeHeartBeating = false;
			closeCode = 0;
		}
	}

	void WebSocketClient::disconnect() noexcept {
		WebSocketCore::disconnect();
	}

	void WebSocketClient::onClosed() noexcept {
		if (maxReconnectTries > currentReconnectTries) {
			disconnect();
		} else {
			if (doWeQuit) {
				doWeQuit->store(true);
			}
		}
	}

	WebSocketClient::~WebSocketClient() noexcept {
		WebSocketCore::disconnect();
	}

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew,
		int32_t currentBaseSocketAgentNew) noexcept {
		configManager = &discordCoreClientNew->configManager;
		currentBaseSocketAgent = currentBaseSocketAgentNew;
		discordCoreClient = discordCoreClientNew;
		doWeQuit = doWeQuitNew;
		taskThread = std::make_unique<std::jthread>([this](std::stop_token token) {
			run(token);
		});
	}

	bool BaseSocketAgent::waitForState(DiscordCoreAPI::ConnectionPackage& packageNew, WebSocketState state) noexcept {
		DiscordCoreAPI::StopWatch stopWatch{ 10000ms };
		stopWatch.resetTimer();
		while (!doWeQuit->load()) {
			try {
				getClient(packageNew.currentShard).tcpConnection->processIO(10);
			} catch (...) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::waitForState()");
				}
				return false;
			}

			if (getClient(packageNew.currentShard).currentState.load() != state) {
				break;
			}
			if (stopWatch.hasTimePassed()) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "Connection failed for WebSocket [" + std::to_string(packageNew.currentShard) << ","
						 << configManager->getTotalShardCount() << "], while waiting for the state: " << std::to_string(static_cast<int32_t>(state))
						 << "... reconnecting." << DiscordCoreAPI::reset() << endl
						 << endl;
				}
				getClient(packageNew.currentShard).onClosed();
				return false;
			}
			std::this_thread::sleep_for(1ms);
		}
		return true;
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage packageNew) noexcept {
		std::unique_lock lock{ discordCoreClient->connectionMutex };
		while (!discordCoreClient->connectionStopWatch00.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
		}
		discordCoreClient->connectionStopWatch00.resetTimer();
		if (packageNew.currentShard != -1) {
			if (!shardMap.contains(packageNew.currentShard)) {
				shardMap[packageNew.currentShard] = std::make_unique<WebSocketClient>(discordCoreClient, packageNew.currentShard, doWeQuit, false);
			}
			getClient(packageNew.currentShard).currentReconnectTries = packageNew.currentReconnectTries;
			++getClient(packageNew.currentShard).currentReconnectTries;
			std::string connectionUrl{ packageNew.areWeResuming ? getClient(packageNew.currentShard).resumeUrl
																: configManager->getConnectionAddress() };
			if (configManager->doWePrintGeneralSuccessMessages()) {
				cout << DiscordCoreAPI::shiftToBrightBlue() << "Connecting Shard " + std::to_string(packageNew.currentShard + 1) << " of "
					 << configManager->getShardCountForThisProcess()
					 << std::string(" Shards for this process. (") + std::to_string(packageNew.currentShard + 1) + " of " +
						std::to_string(configManager->getTotalShardCount()) + std::string(" Shards total across all processes)")
					 << DiscordCoreAPI::reset() << endl
					 << endl;
			}
			std::string relativePath{ "/?v=10&encoding=" +
				std::string{ configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf ? "etf" : "json" } };
			bool didWeConnect{ getClient(packageNew.currentShard)
								   .connect(connectionUrl, relativePath, configManager->getConnectionPort(),
									   configManager->doWePrintWebSocketErrorMessages()) };
			if (!didWeConnect) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "Connection failed to initialize for WebSocket [" << packageNew.currentShard << ","
						 << configManager->getTotalShardCount() << "]"
						 << " reconnecting in 5 seconds." << DiscordCoreAPI::reset() << endl
						 << endl;
					getClient(packageNew.currentShard).onClosed();
					return;
				}
			}
			if (!waitForState(packageNew, WebSocketState::Collecting_Hello)) {
				getClient(packageNew.currentShard).onClosed();
				return;
			}
			if (!waitForState(packageNew, WebSocketState::Sending_Identify)) {
				getClient(packageNew.currentShard).onClosed();
				return;
			}
			getClient(packageNew.currentShard).connections.reset(nullptr);
		}
	}

	WebSocketClient& BaseSocketAgent::getClient(uint32_t index) noexcept {
		return *static_cast<WebSocketClient*>(shardMap[index].get());
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return taskThread.get();
	}

	void BaseSocketAgent::run(std::stop_token token) noexcept {
		while (!token.stop_requested() && !doWeQuit->load()) {
			try {
				std::unique_lock lock{ accessMutex };
				std::unordered_map<uint32_t, WebSocketTCPConnection*> shardMapNew{};
				for (auto& [key, value]: shardMap) {
					shardMapNew.emplace(key, value.get()->tcpConnection.get());
				}
				auto result = TCPConnection::processIO(shardMapNew);
				for (auto& [key, value]: result) {
					if (configManager->doWePrintWebSocketErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << "Connection lost for WebSocket [" << shardMap[key]->shard[0] << ","
							 << configManager->getTotalShardCount() << "]... reconnecting." << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					shardMap[key]->onClosed();
				}
				bool areWeConnected{};
				for (auto& [key, dValueNew]: shardMap) {
					auto dValue = static_cast<WebSocketClient*>(dValueNew.get());
					if (dValue->tcpConnection && dValue->tcpConnection->areWeStillConnected()) {
						if (dValue->checkForAndSendHeartBeat()) {
							DiscordCoreAPI::OnGatewayPingData dataNew{};
							dataNew.timeUntilNextPing = dValue->heartBeatStopWatch.getTotalWaitTime().count();
							discordCoreClient->eventManager.onGatewayPingEvent(dataNew);
						}
						areWeConnected = true;
					} else {
						dValue->connections = std::make_unique<DiscordCoreAPI::ConnectionPackage>();
						++dValue->currentReconnectTries;
						dValue->connections->currentReconnectTries = dValue->currentReconnectTries;
						dValue->connections->areWeResuming = dValue->areWeResuming;
						dValue->connections->currentShard = dValue->shard[0];
						DiscordCoreAPI::ConnectionPackage connectionData = *dValue->connections;
						connect(connectionData);
					}
				}
				if (!areWeConnected) {
					std::this_thread::sleep_for(1ms);
				}
			} catch (...) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					DiscordCoreAPI::reportException("BaseSocketAgent::run()");
				}
			}
		}
	}

	BaseSocketAgent::~BaseSocketAgent() noexcept {
		if (taskThread) {
			taskThread->request_stop();
			if (taskThread->joinable()) {
				taskThread->join();
			}
		}
	}
};