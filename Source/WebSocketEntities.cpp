/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
		for (size_t x = 0; x < eventValue.size(); ++x) {
			if (eventValue[x] == '\0') {
				eventValue.erase(eventValue.begin() + x);
			}
		}
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

	WebSocketCore::WebSocketCore(DiscordCoreAPI::ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew) {
		configManager = configManagerNew;
		wsType = typeOfWebSocketNew;
	}

	WebSocketCore& WebSocketCore::operator=(WebSocketCore&& other) noexcept {
		this->haveWeReceivedHeartbeatAck = other.haveWeReceivedHeartbeatAck;
		this->heartBeatStopWatch = std::move(other.heartBeatStopWatch);
		this->currentReconnectTries = other.currentReconnectTries;
		this->currentMessage = std::move(other.currentMessage);
		this->tcpConnection = std::move(other.tcpConnection);
		this->lastNumberReceived = other.lastNumberReceived;
		this->currentState.store(other.currentState.load());
		this->maxReconnectTries = other.maxReconnectTries;
		this->areWeHeartBeating = other.areWeHeartBeating;
		this->finalString = std::move(other.finalString);
		this->dataBuffer = std::move(other.dataBuffer);
		this->areWeResuming = other.areWeResuming;
		this->configManager = other.configManager;
		this->currentIndex = other.currentIndex;
		this->currentSize = other.currentSize;
		this->dataOpCode = other.dataOpCode;
		this->dataSize = other.dataSize;
		this->shard[0] = other.shard[0];
		this->shard[1] = other.shard[1];
		this->wsType = other.wsType;
		this->offSet = other.offSet;
		return *this;
	};

	WebSocketCore::WebSocketCore(WebSocketCore&& other) noexcept {
		*this = std::move(other);
	};

	bool WebSocketCore::connect(const std::string& baseUrlNew, const std::string& relativePath, const uint16_t portNew) noexcept {
		tcpConnection = WebSocketTCPConnection{ baseUrlNew, portNew, this };
		if (tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
			std::this_thread::sleep_for(1s);
			return false;
		}
		currentState.store(WebSocketState::Upgrading);
		std::string sendString{ "GET " + relativePath + " HTTP/1.1\r\nHost: " + baseUrlNew +
			"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: " + "Upgrade\r\nSec-WebSocket-Key: " +
			DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n" };
		tcpConnection.writeData(sendString, true);

		if (tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
			std::this_thread::sleep_for(1s);
			return false;
		}
		DiscordCoreAPI::StopWatch stopWatch{ 5s };
		do {
			if (stopWatch.hasTimePassed()) {
				std::this_thread::sleep_for(1s);
				return false;
			}
			if (tcpConnection.processIO(10) != ConnectionStatus::NO_Error) {
				std::this_thread::sleep_for(1s);
				onClosed();
				return false;
			}
			std::this_thread::sleep_for(1ms);
		} while (currentState.load() == WebSocketState::Upgrading);
		return true;
	}

	WebSocketTCPConnection::WebSocketTCPConnection(const std::string& baseUrlNew, uint16_t portNew, WebSocketCore* ptrNew)
		: TCPConnection{ baseUrlNew, portNew } {
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
		std::string webSocketTitle{ wsType == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket" };
		Globals::MessagePrinter::printSuccess<Globals::MessageType::WebSocket>("Sending " + webSocketTitle + " [" + std::to_string(shard[0]) + "," +
			std::to_string(shard[1]) + "]" + std::string{ "'s Message: " } + static_cast<std::string>(dataToSend));
		tcpConnection.writeData(dataToSend, true);
		if (tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
			onClosed();
			return false;
		}
		return true;
	}

	void WebSocketCore::parseConnectionHeaders(std::string_view stringNew) noexcept {
		if (tcpConnection.areWeStillConnected() && currentState.load() == WebSocketState::Upgrading) {
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
						onMessageReceived({ currentMessage.data() + messageOffset, length });
						currentMessage.erase(messageOffset + length);
						auto currentSize = currentMessage.size();
						if (currentSize > 0) {
							if (currentMessage.size() < currentSize) {
								auto oldSize = currentMessage.size();
								currentMessage.resize(currentSize + oldSize);
								std::memcpy(currentMessage.data() + oldSize, currentMessage.data(), currentSize);
								currentMessage.clear();
							}
						}
						++currentIndex;
					}


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
					Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>(webSocketTitle + " [" + std::to_string(shard[0]) + "," +
						std::to_string(shard[1]) + "]" + " Closed; Code: " + std::to_string(closeValue) + ", " + closeString);
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
			auto oldSize = ptr->currentMessage.size();
			ptr->currentMessage.resize(oldSize + inputBuffer.size());
			std::memcpy(ptr->currentMessage.data() + oldSize, inputBuffer.data(), inputBuffer.size());
			while (ptr->parseMessage()) {
			};
		}
	}

	WebSocketClient::WebSocketClient(DiscordCoreAPI::DiscordCoreClient* client, int32_t currentShardNew, std::atomic_bool* doWeQuitNew)
		: WebSocketCore(&client->configManager, WebSocketType::Normal) {
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

	bool WebSocketClient::onMessageReceived(std::string_view dataNew) noexcept {
		try {
			if (tcpConnection.areWeStillConnected() && currentMessage.size() > 0 && dataNew.size() > 0) {
				WebSocketMessage message{};
				if (configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
					try {
						dataNew = EtfParser::parseEtfToJson(dataNew);
						parser.parseJson<true, true>(message, dataNew);
					} catch (const DiscordCoreAPI::DCAException& error) {
						Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>(error.what());
						tcpConnection.getInputBuffer();
						currentMessage.clear();
						return false;
					}
				} else {
					parser.parseJson<true, true>(message, dataNew);
				}

				if (message.s != 0) {
					lastNumberReceived = message.s;
				}
				Globals::MessagePrinter::printSuccess<Globals::MessageType::WebSocket>("Message received from WebSocket [" +
					std::to_string(shard[0]) + "," + std::to_string(shard[1]) + std::string("]: ") + std::string{ dataNew });
				switch (static_cast<WebSocketOpCodes>(message.op)) {
					case WebSocketOpCodes::Dispatch: {
						if (message.t != "") {
							switch (EventConverter{ message.t }) {
								case 1: {
									if (configManager->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
										WebSocketMessageData<ReadyData<DiscordCoreAPI::TextFormat::Etf>> data{};
										currentState.store(WebSocketState::Authenticated);
										parser.parseJson<true, true>(data, dataNew);
										sessionId = data.d.sessionId;
										if (data.d.resumeGatewayUrl.find("wss://") != std::string::npos) {
											resumeUrl = data.d.resumeGatewayUrl.substr(
												data.d.resumeGatewayUrl.find("wss://") + std::string{ "wss://" }.size());
										}
										discordCoreClient->currentUser = DiscordCoreAPI::BotUser{ data.d.user,
											discordCoreClient
												->baseSocketAgentsMap[static_cast<int32_t>(floor(static_cast<int32_t>(shard[0]) %
													static_cast<int32_t>(discordCoreClient->configManager.getTotalShardCount())))]
												.get() };
										DiscordCoreAPI::Users::insertUser(std::move(data.d.user));
									} else {
										WebSocketMessageData<ReadyData<DiscordCoreAPI::TextFormat::Json>> data{};
										currentState.store(WebSocketState::Authenticated);
										parser.parseJson<true, true>(data, dataNew);
										sessionId = data.d.sessionId;
										if (data.d.resumeGatewayUrl.find("wss://") != std::string::npos) {
											resumeUrl = data.d.resumeGatewayUrl.substr(
												data.d.resumeGatewayUrl.find("wss://") + std::string{ "wss://" }.size());
										}
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
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 4: {
									if (discordCoreClient->eventManager.onAutoModerationRuleCreationEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnAutoModerationRuleCreationData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnAutoModerationRuleCreationData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
									}
									break;
								}
								case 5: {
									if (discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnAutoModerationRuleUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
									}
									break;
								}
								case 6: {
									if (discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnAutoModerationRuleDeletionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
									}
									break;
								}
								case 7: {
									if (discordCoreClient->eventManager.onAutoModerationActionExecutionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnAutoModerationActionExecutionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnAutoModerationActionExecutionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
									}
									break;
								}
								case 8: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnChannelCreationData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnChannelCreationData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onChannelCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onChannelCreationEvent(*dataPackage);
									}
									break;
								}
								case 9: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnChannelUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnChannelUpdateData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onChannelUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onChannelUpdateEvent(*dataPackage);
									}
									break;
								}
								case 10: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnChannelDeletionData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnChannelDeletionData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onChannelDeletionEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onChannelDeletionEvent(*dataPackage);
									}
									break;
								}
								case 11: {
									if (discordCoreClient->eventManager.onChannelPinsUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnChannelPinsUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnChannelPinsUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onChannelPinsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 12: {
									if (discordCoreClient->eventManager.onThreadCreationEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnThreadCreationData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnThreadCreationData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onThreadCreationEvent(*dataPackage);
									}
									break;
								}
								case 13: {
									if (discordCoreClient->eventManager.onThreadUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnThreadUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnThreadUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onThreadUpdateEvent(*dataPackage);
									}
									break;
								}
								case 14: {
									if (discordCoreClient->eventManager.onThreadDeletionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnThreadDeletionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnThreadDeletionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onThreadDeletionEvent(*dataPackage);
									}
									break;
								}
								case 15: {
									if (discordCoreClient->eventManager.onThreadListSyncEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnThreadListSyncData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnThreadListSyncData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onThreadListSyncEvent(*dataPackage);
									}
									break;
								}
								case 16: {
									if (discordCoreClient->eventManager.onThreadMemberUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnThreadMemberUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnThreadMemberUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onThreadMemberUpdateEvent(*dataPackage);
									}
									break;
								}
								case 17: {
									if (discordCoreClient->eventManager.onThreadMembersUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnThreadMembersUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnThreadMembersUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onThreadMembersUpdateEvent(*dataPackage);
									}
									break;
								}
								case 18: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildCreationData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildCreationData>(parser, dataNew, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildCreationEvent(*dataPackage);
									}
									break;
								}
								case 19: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildUpdateData>(parser, dataNew, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildUpdateEvent(*dataPackage);
									}
									break;
								}
								case 20: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildDeletionData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildDeletionData>(parser, dataNew, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildDeletionEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildDeletionEvent(*dataPackage);
									}
									break;
								}
								case 21: {
									if (discordCoreClient->eventManager.onGuildBanAddEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildBanAddData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildBanAddData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildBanAddEvent(*dataPackage);
									}
									break;
								}
								case 22: {
									if (discordCoreClient->eventManager.onGuildBanRemoveEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildBanRemoveData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildBanRemoveData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildBanRemoveEvent(*dataPackage);
									}
									break;
								}
								case 23: {
									if (discordCoreClient->eventManager.onGuildEmojisUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildEmojisUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildEmojisUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
									}
									break;
								}
								case 24: {
									if (discordCoreClient->eventManager.onGuildStickersUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildStickersUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildStickersUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildStickersUpdateEvent(*dataPackage);
									}
									break;
								}
								case 25: {
									if (discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildIntegrationsUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
									}
									break;
								}
								case 26: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildMemberAddData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildMemberAddData>(parser, dataNew, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildMemberAddEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildMemberAddEvent(*dataPackage);
									}
									break;
								}
								case 27: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildMemberRemoveData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildMemberRemoveData>(parser, dataNew, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildMemberRemoveEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildMemberRemoveEvent(*dataPackage);
									}
									break;
								}
								case 28: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildMemberUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildMemberUpdateData>(parser, dataNew, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onGuildMemberUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onGuildMemberUpdateEvent(*dataPackage);
									}
									break;
								}
								case 29: {
									if (discordCoreClient->eventManager.onGuildMembersChunkEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildMembersChunkData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildMembersChunkData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildMembersChunkEvent(*dataPackage);
									}
									break;
								}
								case 30: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnRoleCreationData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnRoleCreationData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onRoleCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onRoleCreationEvent(*dataPackage);
									}
									break;
								}
								case 31: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnRoleUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnRoleUpdateData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onRoleUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onRoleUpdateEvent(*dataPackage);
									}
									break;
								}
								case 32: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnRoleDeletionData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnRoleDeletionData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onRoleDeletionEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onRoleDeletionEvent(*dataPackage);
									}
									break;
								}
								case 33: {
									if (discordCoreClient->eventManager.onGuildScheduledEventCreationEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildScheduledEventCreationData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildScheduledEventCreationData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
									}
									break;
								}
								case 34: {
									if (discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildScheduledEventUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildScheduledEventUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
									}
									break;
								}
								case 35: {
									if (discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildScheduledEventDeletionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildScheduledEventDeletionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
									}
									break;
								}
								case 36: {
									if (discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildScheduledEventUserAddData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildScheduledEventUserAddData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
									}
									break;
								}
								case 37: {
									if (discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnGuildScheduledEventUserRemoveData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
									}
									break;
								}
								case 38: {
									if (discordCoreClient->eventManager.onIntegrationCreationEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnIntegrationCreationData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnIntegrationCreationData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onIntegrationCreationEvent(*dataPackage);
									}
									break;
								}
								case 39: {
									if (discordCoreClient->eventManager.onIntegrationUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnIntegrationUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnIntegrationUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onIntegrationUpdateEvent(*dataPackage);
									}
									break;
								}
								case 40: {
									if (discordCoreClient->eventManager.onIntegrationDeletionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnIntegrationDeletionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnIntegrationDeletionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onIntegrationDeletionEvent(*dataPackage);
									}
									break;
								}
								case 41: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnInteractionCreationData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnInteractionCreationData>(parser, dataNew, discordCoreClient)
									};
									if (discordCoreClient->eventManager.onInteractionCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onInteractionCreationEvent(*dataPackage);
									}
									break;
								}
								case 42: {
									if (discordCoreClient->eventManager.onInviteCreationEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnInviteCreationData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnInviteCreationData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onInviteCreationEvent(*dataPackage);
									}
									break;
								}
								case 43: {
									if (discordCoreClient->eventManager.onInviteDeletionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnInviteDeletionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onInviteDeletionEvent(*dataPackage);
									}
									break;
								}
								case 44: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnMessageCreationData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnMessageCreationData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onMessageCreationEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onMessageCreationEvent(*dataPackage);
									}
									break;
								}
								case 45: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnMessageUpdateData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onMessageUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onMessageUpdateEvent(*dataPackage);
									}
									break;
								}
								case 46: {
									if (discordCoreClient->eventManager.onMessageDeletionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnMessageDeletionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onMessageDeletionEvent(*dataPackage);
									}
									break;
								}
								case 47: {
									if (discordCoreClient->eventManager.onMessageDeleteBulkEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnMessageDeleteBulkData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onMessageDeleteBulkEvent(*dataPackage);
									}
									break;
								}
								case 48: {
									if (discordCoreClient->eventManager.onReactionAddEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnReactionAddData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnReactionAddData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onReactionAddEvent(*dataPackage);
									}
									break;
								}
								case 49: {
									if (discordCoreClient->eventManager.onReactionRemoveEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnReactionRemoveData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onReactionRemoveEvent(*dataPackage);
									}
									break;
								}
								case 50: {
									if (discordCoreClient->eventManager.onReactionRemoveAllEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnReactionRemoveAllData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onReactionRemoveAllEvent(*dataPackage);
									}
									break;
								}
								case 51: {
									if (discordCoreClient->eventManager.onReactionRemoveEmojiEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnReactionRemoveEmojiData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
									}
									break;
								}
								case 52: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnPresenceUpdateData>(parser, dataNew)
									};
									if (discordCoreClient->eventManager.onPresenceUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onPresenceUpdateEvent(*dataPackage);
									}
									break;
								}
								case 53: {
									if (discordCoreClient->eventManager.onStageInstanceCreationEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnStageInstanceCreationData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onStageInstanceCreationEvent(*dataPackage);
									}
									break;
								}
								case 54: {
									if (discordCoreClient->eventManager.onStageInstanceUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnStageInstanceUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onStageInstanceUpdateEvent(*dataPackage);
									}
									break;
								}
								case 55: {
									if (discordCoreClient->eventManager.onStageInstanceDeletionEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnStageInstanceDeletionData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onStageInstanceDeletionEvent(*dataPackage);
									}
									break;
								}
								case 56: {
									if (discordCoreClient->eventManager.onTypingStartEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnTypingStartData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnTypingStartData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onTypingStartEvent(*dataPackage);
									}
									break;
								}
								case 57: {
									if (discordCoreClient->eventManager.onUserUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnUserUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnUserUpdateData>(parser, dataNew)
										};
										discordCoreClient->eventManager.onUserUpdateEvent(*dataPackage);
									}
									break;
								}
								case 58: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnVoiceStateUpdateData>(parser, dataNew, this)
									};
									if (discordCoreClient->eventManager.onVoiceStateUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onVoiceStateUpdateEvent(*dataPackage);
									}
									break;
								}
								case 59: {
									DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{
										DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnVoiceServerUpdateData>(parser, dataNew, this)
									};
									if (discordCoreClient->eventManager.onVoiceServerUpdateEvent.functions.size() > 0) {
										discordCoreClient->eventManager.onVoiceServerUpdateEvent(*dataPackage);
									}
									break;
								}
								case 60: {
									if (discordCoreClient->eventManager.onWebhookUpdateEvent.functions.size() > 0) {
										DiscordCoreAPI::UniquePtr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{
											DiscordCoreAPI::makeUnique<DiscordCoreAPI::OnWebhookUpdateData>(parser, dataNew)
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
						Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>(
							"Shard [" + std::to_string(shard[0]) + "," + std::to_string(shard[1]) + "]" + " Reconnecting (Type 7)!");
						areWeResuming = true;
						tcpConnection.disconnect();
						return true;
					}
					case WebSocketOpCodes::Invalid_Session: {
						WebSocketMessageData<InvalidSessionData> data{};
						parser.parseJson<true, true>(data, dataNew);
						Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>(
							"Shard [" + std::to_string(shard[0]) + "," + std::to_string(shard[1]) + "]" + " Reconnecting (Type 9)!");
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
						parser.parseJson<true, true>(data, dataNew);
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

		} catch (const DiscordCoreAPI::DCAException& error) {
			Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>(error.what());
		}
		return false;
	}

	void WebSocketCore::disconnect() noexcept {
		if (tcpConnection.areWeStillConnected()) {
			std::string dataNew{ "\x03\xE8" };
			createHeader(dataNew, WebSocketOpCode::Op_Close);
			tcpConnection.writeData(dataNew, true);
			tcpConnection.disconnect();
			currentState.store(WebSocketState::Disconnected);
			areWeHeartBeating = false;
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
		disconnect();
	}

	BaseSocketAgent::BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew,
		int32_t currentBaseSocketAgentNew) noexcept {
		currentBaseSocketAgent = currentBaseSocketAgentNew;
		discordCoreClient = discordCoreClientNew;
		doWeQuit = doWeQuitNew;
		taskThread = DiscordCoreAPI::makeUnique<std::jthread>([this](std::stop_token token) {
			run(token);
		});
	}

	bool BaseSocketAgent::waitForState(DiscordCoreAPI::ConnectionPackage& packageNew, WebSocketState state) noexcept {
		DiscordCoreAPI::StopWatch stopWatch{ 10000ms };
		stopWatch.resetTimer();
		while (!doWeQuit->load()) {
			if (getClient(packageNew.currentShard).tcpConnection.processIO(10) != ConnectionStatus::NO_Error) {
				return false;
			}

			if (getClient(packageNew.currentShard).currentState.load() != state) {
				break;
			}
			if (stopWatch.hasTimePassed()) {
				Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>("Connection failed for WebSocket [" +
					std::to_string(packageNew.currentShard) + "," + std::to_string(discordCoreClient->configManager.getTotalShardCount()) +
					"], while waiting for the state: " + std::to_string(static_cast<int32_t>(state)) + "... reconnecting.");
				getClient(packageNew.currentShard).onClosed();
				return false;
			}
			std::this_thread::sleep_for(1ms);
		}
		return true;
	}

	void BaseSocketAgent::connect(DiscordCoreAPI::ConnectionPackage packageNew) noexcept {
		while (!discordCoreClient->theSemaphore.try_acquire()) {
			std::this_thread::sleep_for(1ms);
		}
		if (packageNew.currentShard != -1) {
			if (!shardMap.contains(packageNew.currentShard)) {
				shardMap[packageNew.currentShard] = WebSocketClient{ discordCoreClient, static_cast<int32_t>(packageNew.currentShard), doWeQuit };
			}
			getClient(packageNew.currentShard).currentReconnectTries = packageNew.currentReconnectTries;
			++getClient(packageNew.currentShard).currentReconnectTries;
			std::string connectionUrl{ packageNew.areWeResuming ? getClient(packageNew.currentShard).resumeUrl
																: discordCoreClient->configManager.getConnectionAddress() };
			Globals::MessagePrinter::printSuccess<Globals::MessageType::General>("Connecting Shard " + std::to_string(packageNew.currentShard + 1) +
				" of " + std::to_string(discordCoreClient->configManager.getShardCountForThisProcess()) +
				std::string{ " Shards for this process. (" } + std::to_string(packageNew.currentShard + 1) + " of " +
				std::to_string(discordCoreClient->configManager.getTotalShardCount()) + std::string{ " Shards total across all processes)" });
			std::string relativePath{ "/?v=10&encoding=" +
				std::string{ discordCoreClient->configManager.getTextFormat() == DiscordCoreAPI::TextFormat::Etf ? "etf" : "json" } };
			bool didWeConnect{
				getClient(packageNew.currentShard).connect(connectionUrl, relativePath, discordCoreClient->configManager.getConnectionPort())
			};
			if (!didWeConnect) {
				getClient(packageNew.currentShard).areWeResuming = false;
				Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>("Connection failed to initialize for WebSocket [" +
					std::to_string(packageNew.currentShard) + "," + std::to_string(discordCoreClient->configManager.getTotalShardCount()) + "]" +
					" reconnecting in 5 seconds.");
				getClient(packageNew.currentShard).onClosed();
				discordCoreClient->theSemaphore.release();
			}
			if (!waitForState(packageNew, WebSocketState::Collecting_Hello)) {
				getClient(packageNew.currentShard).onClosed();
				discordCoreClient->theSemaphore.release();
				return;
			}
		}
		getClient(packageNew.currentShard).areWeResuming = false;
		discordCoreClient->connectionStopWatch00.resetTimer();
		discordCoreClient->theSemaphore.release();
	}

	WebSocketClient& BaseSocketAgent::getClient(uint32_t index) noexcept {
		return shardMap[index];
	}

	std::jthread* BaseSocketAgent::getTheTask() noexcept {
		return taskThread.get();
	}

	void BaseSocketAgent::run(std::stop_token token) noexcept {
		std::unordered_map<uint32_t, WebSocketTCPConnection*> processIOMapNew{};
		while (!token.stop_requested() && !doWeQuit->load()) {
			try {
				for (auto& [key, value]: shardMap) {
					if (value.tcpConnection.areWeStillConnected()) {
						processIOMapNew.emplace(key, &value.tcpConnection);
					}
				}
				auto result = TCPConnection<WebSocketTCPConnection>::processIO(processIOMapNew);
				processIOMapNew.clear();
				for (auto& [key, value]: result) {
					Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>("Connection lost for WebSocket [" +
						std::to_string(shardMap[key].shard[0]) + "," + std::to_string(discordCoreClient->configManager.getTotalShardCount()) +
						"]... reconnecting.");
					shardMap[key].onClosed();
				}
				bool areWeConnected{};
				for (auto& [key, dValueNew]: shardMap) {
					auto dValue = static_cast<WebSocketClient*>(&dValueNew);
					if (dValue->tcpConnection.areWeStillConnected()) {
						if (dValue->checkForAndSendHeartBeat()) {
							DiscordCoreAPI::OnGatewayPingData dataNew{};
							dataNew.timeUntilNextPing = dValue->heartBeatStopWatch.getTotalWaitTime().count();
							discordCoreClient->eventManager.onGatewayPingEvent(dataNew);
						}
						areWeConnected = true;
					} else {
						DiscordCoreAPI::ConnectionPackage connectionPackage{};
						++dValue->currentReconnectTries;
						connectionPackage.currentReconnectTries = dValue->currentReconnectTries;
						connectionPackage.areWeResuming = dValue->areWeResuming;
						connectionPackage.currentShard = dValue->shard[0];
						connect(connectionPackage);
					}
				}
				if (!areWeConnected) {
					std::this_thread::sleep_for(1ms);
				}
			} catch (const DiscordCoreAPI::DCAException& error) {
				Globals::MessagePrinter::printError<Globals::MessageType::WebSocket>(error.what());
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
}
