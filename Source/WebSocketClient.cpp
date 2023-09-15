/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++
   CoRoutines.

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
/// WebSocketClient.cpp - Source file for the webSocket related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file WebSocketClient.cpp

#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <discordcoreapi/Utilities/Base.hpp>
#include <discordcoreapi/Utilities/WebSocketClient.hpp>
#include <random>

namespace DiscordCoreAPI {

	extern VoiceConnectionsMap voiceConnectionMap;

	namespace DiscordCoreInternal {

		EventConverter::EventConverter(std::string newEvent) {
			eventValue = newEvent;
			for (uint64_t x = 0; x < eventValue.size(); ++x) {
				if (eventValue.at(x) == '\0') {
					eventValue.erase(eventValue.begin() + x);
				}
			}
		}

		EventConverter::operator uint64_t() {
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

		WebSocketCore::WebSocketCore(ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew) : EtfParser{} {
			configManager = configManagerNew;
			wsType		  = typeOfWebSocketNew;
		}

		WebSocketCore& WebSocketCore::operator=(WebSocketCore&& other) noexcept {
			areWeCollectingData.store(other.areWeCollectingData.load(std::memory_order_acquire), std::memory_order_release);
			currentState.store(other.currentState.load(std::memory_order_acquire), std::memory_order_release);
			heartBeatStopWatch		   = std::move(other.heartBeatStopWatch);
			haveWeReceivedHeartbeatAck = other.haveWeReceivedHeartbeatAck;
			currentMessage			   = std::move(other.currentMessage);
			tcpConnection			   = std::move(other.tcpConnection);
			finalString				   = std::move(other.finalString);
			currentReconnectTries	   = other.currentReconnectTries;
			dataBuffer				   = std::move(other.dataBuffer);
			lastNumberReceived		   = other.lastNumberReceived;
			maxReconnectTries		   = other.maxReconnectTries;
			areWeHeartBeating		   = other.areWeHeartBeating;
			areWeResuming			   = other.areWeResuming;
			configManager			   = other.configManager;
			dataOpCode				   = other.dataOpCode;
			shard.at(0)				   = other.shard.at(0);
			shard.at(1)				   = other.shard.at(1);
			dataSize				   = other.dataSize;
			offSet					   = other.offSet;
			wsType					   = other.wsType;
			return *this;
		}

		WebSocketCore::WebSocketCore(WebSocketCore&& other) noexcept : EtfParser{} {
			*this = std::move(other);
		}

		bool WebSocketCore::connect(const std::string& baseUrlNew, const std::string& relativePath, const uint16_t portNew) {
			tcpConnection = WebSocketTCPConnection{ baseUrlNew, portNew, this };
			if (tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
				std::this_thread::sleep_for(1s);
				return false;
			}
			currentState.store(WebSocketState::Upgrading, std::memory_order_release);
			std::string sendString{ "GET " + relativePath + " HTTP/1.1\r\nHost: " + baseUrlNew +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: " + "Upgrade\r\nSec-WebSocket-Key: " + generateBase64EncodedKey() +
				"\r\nSec-WebSocket-Version: 13\r\n\r\n" };
			tcpConnection.writeData(static_cast<std::string_view>(sendString), true);

			if (tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
				std::this_thread::sleep_for(1s);
				return false;
			}
			return true;
		}

		WebSocketTCPConnection::WebSocketTCPConnection(const std::string& baseUrlNew, uint16_t portNew, WebSocketCore* ptrNew) : TCPConnection{ baseUrlNew, portNew } {
			ptr = ptrNew;
		}

		bool WebSocketCore::sendMessage(std::basic_string<uint8_t>& dataToSend, bool priority) {
			if (dataToSend.size() == 0) {
				return false;
			}
			std::string webSocketTitle{ wsType == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket" };
			MessagePrinter::printSuccess<PrintMessageType::WebSocket>(
				jsonifier::string{ "Sending " + webSocketTitle + " [" + std::to_string(shard.at(0)) + "," + std::to_string(shard.at(1)) + "]" + std::string{ "'s Message: " } });
			std::cout << static_cast<std::basic_string<uint8_t>>(dataToSend.data()).data();
			std::unique_lock lock{ accessMutex };
			if (areWeConnected()) {
				tcpConnection.writeData(static_cast<std::basic_string_view<uint8_t>>(dataToSend), priority);
				if (tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
					onClosed();
					return false;
				}
			}
			return true;
		}

		void WebSocketCore::parseConnectionHeaders() {
			if (areWeConnected() && currentState.load(std::memory_order_acquire) == WebSocketState::Upgrading) {
				auto theFindValue = currentMessage.operator std::string().find("\r\n\r\n");
				if (theFindValue != std::string::npos) {
					currentMessage.clear();
					currentState.store(WebSocketState::Collecting_Hello, std::memory_order_release);
					return;
				}
			}
		}

		bool WebSocketCore::checkForAndSendHeartBeat(bool isImmediate) {
			if ((currentState.load(std::memory_order_acquire) == WebSocketState::Authenticated && heartBeatStopWatch.hasTimeElapsed() && haveWeReceivedHeartbeatAck) ||
				isImmediate) {
				std::basic_string<uint8_t> string{};
				if (dataOpCode == WebSocketOpCode::Op_Binary) {
					EtfSerializer data{};
					data["d"]  = lastNumberReceived;
					data["op"] = 1;
					string	   = data.operator std::basic_string<uint8_t>();
				} else {
					WebSocketMessageData<uint64_t> message{};
					message.d  = lastNumberReceived;
					message.op = 1;
					parser.serializeJson(message, string);
				}
				haveWeReceivedHeartbeatAck = false;
				heartBeatStopWatch.reset();
				createHeader(string, dataOpCode);
				return sendMessage(string, true);
			}
			return false;
		}

		bool WebSocketCore::parseMessage() {
			if (currentMessage.size() < 4) {
				return false;
			} else {
				WebSocketOpCode opcode = static_cast<WebSocketOpCode>(currentMessage.at(0) & ~webSocketMaskBit);
				switch (opcode) {
					case WebSocketOpCode::Op_Continuation:
						[[fallthrough]];
					case WebSocketOpCode::Op_Text:
						[[fallthrough]];
					case WebSocketOpCode::Op_Binary:
						[[fallthrough]];
					case WebSocketOpCode::Op_Ping:
						[[fallthrough]];
					case WebSocketOpCode::Op_Pong: {
						uint8_t length00	   = currentMessage.at(1);
						uint32_t messageOffset = 2;

						if (length00 & webSocketMaskBit) {
							return true;
						}

						uint64_t lengthFinal = length00;

						if (length00 == webSocketPayloadLengthMagicLarge) {
							if (currentMessage.size() < 8) {
								return false;
							}

							uint8_t length01 = static_cast<uint8_t>(currentMessage.at(2));
							uint8_t length02 = static_cast<uint8_t>(currentMessage.at(3));
							lengthFinal		 = static_cast<uint64_t>((length01 << 8ull) | length02);

							messageOffset += 2;
						} else if (length00 == webSocketPayloadLengthMagicHuge) {
							if (currentMessage.size() < 10) {
								return false;
							}
							lengthFinal = 0;
							for (uint64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
								uint8_t length03 = static_cast<uint8_t>(currentMessage.at(x));
								lengthFinal |= static_cast<uint64_t>(length03 & 0xff) << shift;
							}
							messageOffset += 8;
						}

						if (currentMessage.size() < messageOffset + lengthFinal) {
							return false;
						}

						if (opcode == WebSocketOpCode::Op_Ping || opcode == WebSocketOpCode::Op_Pong) {
							return false;
						} else {
							onMessageReceived({ currentMessage.data() + messageOffset, lengthFinal });
							currentMessage.erase(messageOffset + lengthFinal);
						}
						return true;
					} break;
					case WebSocketOpCode::Op_Close: {
						uint16_t closeValue = currentMessage.at(2) & 0xff;
						closeValue <<= 8;
						closeValue |= currentMessage.at(3) & 0xff;
						std::string closeString{};
						if (wsType == WebSocketType::Voice) {
							VoiceWebSocketClose voiceClose{ closeValue };
							closeString = static_cast<std::string>(voiceClose.operator std::string_view());
						} else {
							WebSocketClose wsClose{ closeValue };
							closeString = static_cast<std::string>(wsClose.operator std::string_view());
						}
						std::string webSocketTitle = wsType == WebSocketType::Voice ? "Voice WebSocket" : "WebSocket";
						MessagePrinter::printError<PrintMessageType::WebSocket>(jsonifier::string{ webSocketTitle + " [" + std::to_string(shard.at(0)) + "," +
							std::to_string(shard.at(1)) + "]" + " Closed; Code: " + std::to_string(closeValue) + ", " + closeString });
						return false;
					} break;

					default: {
						return false;
					} break;
				}
			}
		}

		bool WebSocketCore::areWeConnected() {
			return tcpConnection.areWeStillConnected();
		}

		void WebSocketTCPConnection::handleBuffer() {
			if (ptr->currentState.load(std::memory_order_acquire) == WebSocketState::Upgrading) {
				auto inputBufferNew = getInputBuffer();
				auto oldSize		= ptr->currentMessage.size();
				if (ptr->currentMessage.size() < oldSize + inputBufferNew.size()) {
					ptr->currentMessage.resize(oldSize + inputBufferNew.size());
				}
				std::memcpy(ptr->currentMessage.data() + oldSize, inputBufferNew.data(), inputBufferNew.size());
				ptr->parseConnectionHeaders();
			} else {
				auto inputBufferNew = getInputBuffer();
				auto oldSize		= ptr->currentMessage.size();
				if (ptr->currentMessage.size() < oldSize + inputBufferNew.size()) {
					ptr->currentMessage.resize(oldSize + inputBufferNew.size());
				}
				std::memcpy(ptr->currentMessage.data() + oldSize, inputBufferNew.data(), inputBufferNew.size());
				while (ptr->parseMessage()) {
				};
			}
		}

		WebSocketClient::WebSocketClient(uint32_t currentShardNew, std::atomic_bool* doWeQuitNew)
			: WebSocketCore(&DiscordCoreClient::getInstance()->configManager, WebSocketType::Normal) {
			configManager = &DiscordCoreClient::getInstance()->configManager;
			shard.at(0)	  = currentShardNew;
			doWeQuit	  = doWeQuitNew;
			if (DiscordCoreClient::getInstance()) {
				shard.at(1) = DiscordCoreClient::getInstance()->configManager.getTotalShardCount();
				if (DiscordCoreClient::getInstance()->configManager.getTextFormat() == TextFormat::Etf) {
					dataOpCode = WebSocketOpCode::Op_Binary;
				} else {
					dataOpCode = WebSocketOpCode::Op_Text;
				}
			}
		}

		void WebSocketClient::getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) {
			while (currentState.load(std::memory_order_acquire) != WebSocketState::Authenticated) {
				std::this_thread::sleep_for(1ms);
			}
			WebSocketMessageData<UpdateVoiceStateDataDC> data01{};
			data01.excludedKeys.emplace("t");
			data01.excludedKeys.emplace("s");
			data01.d.channelId = std::nullptr_t{};
			data01.d.guildId   = doWeCollect.guildId;
			data01.d.selfDeaf  = doWeCollect.selfDeaf;
			data01.d.selfMute  = doWeCollect.selfMute;
			data01.op		   = 4;
			userId			   = doWeCollect.userId;

			WebSocketMessageData<UpdateVoiceStateData> data02{};
			data02.excludedKeys.emplace("t");
			data02.excludedKeys.emplace("s");
			data02.d.channelId = doWeCollect.channelId;
			data02.d.guildId   = doWeCollect.guildId;
			data02.d.selfDeaf  = doWeCollect.selfDeaf;
			data02.d.selfMute  = doWeCollect.selfMute;
			data02.op		   = 4;
			std::basic_string<uint8_t> string{};
			auto serializer = data01.operator EtfSerializer();
			if (dataOpCode == WebSocketOpCode::Op_Binary) {
				string = serializer.operator std::basic_string<uint8_t>();
			} else {
				parser.serializeJson<true>(data01, string);
			}
			createHeader(string, dataOpCode);
			if (!sendMessage(string, true)) {
				return;
			}
			if (Snowflake{ doWeCollect.channelId } == 0) {
				return;
			}
			serializer = data02.operator EtfSerializer();
			if (dataOpCode == WebSocketOpCode::Op_Binary) {
				string = serializer.operator std::basic_string<uint8_t>();
			} else {
				parser.serializeJson<true>(data02, string);
			}
			createHeader(string, dataOpCode);
			areWeCollectingData.store(true, std::memory_order_release);
			if (!sendMessage(string, true)) {
				return;
			}
		}

		bool WebSocketClient::onMessageReceived(std::basic_string_view<uint8_t> dataNew) {
			try {
				if (areWeConnected() && currentMessage.size() > 0 && dataNew.size() > 0) {
					WebSocketMessage message{};
					if (configManager->getTextFormat() == TextFormat::Etf) {
						try {
							dataNew = EtfParser::parseEtfToJson(dataNew);
							parser.parseJson<true, true>(message, dataNew);
						} catch (const DCAException& error) {
							MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
							tcpConnection.getInputBuffer();
							currentMessage.clear();
							return false;
						}
					} else {
						parser.parseJson<true, true>(message, dataNew);
					}

					if (message.s != 0) {
						lastNumberReceived = static_cast<uint32_t>(message.s);
					}
					MessagePrinter::printSuccess<PrintMessageType::WebSocket>(jsonifier::string{ "Message received from WebSocket [" + std::to_string(shard.at(0)) + "," +
						std::to_string(shard.at(1)) + std::string("]: ") + jsonifier::string{ dataNew } });
					switch (static_cast<WebSocketOpCodes>(message.op)) {
						case WebSocketOpCodes::Dispatch: {
							if (message.t != "") {
								switch (EventConverter{ message.t }) {
									case 1: {
										WebSocketMessageData<ReadyData> data{};
										if (dataOpCode == WebSocketOpCode::Op_Text) {
											data.d.excludedKeys.emplace("shard");
										}
										currentState.store(WebSocketState::Authenticated, std::memory_order_release);
										parser.parseJson<true, true, true>(data, dataNew);

										sessionId = data.d.sessionId;
										if (data.d.resumeGatewayUrl.find("wss://") != std::string::npos) {
											resumeUrl = data.d.resumeGatewayUrl.substr(data.d.resumeGatewayUrl.find("wss://") + std::string{ "wss://" }.size());
										}
										DiscordCoreClient::getInstance()->currentUser = BotUser{ data.d.user,
											DiscordCoreClient::getInstance()
												->baseSocketAgentsMap[static_cast<uint64_t>(floor(
													static_cast<uint64_t>(shard.at(0)) % static_cast<uint64_t>(DiscordCoreClient::getInstance()->baseSocketAgentsMap.size())))]
												.get() };
										Users::insertUser(static_cast<UserCacheData>(std::move(data.d.user)));
										currentReconnectTries = 0;
										break;
									}
									case 2: {
										currentState.store(WebSocketState::Authenticated, std::memory_order_release);
										currentReconnectTries = 0;
										break;
									}
									case 3: {
										if (DiscordCoreClient::getInstance()->eventManager.onApplicationCommandPermissionsUpdateEvent.functions.size() > 0) {
											UniquePtr<OnApplicationCommandPermissionsUpdateData> dataPackage{ makeUnique<OnApplicationCommandPermissionsUpdateData>(parser,
												dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
										}
										break;
									}
									case 4: {
										if (DiscordCoreClient::getInstance()->eventManager.onAutoModerationRuleCreationEvent.functions.size() > 0) {
											UniquePtr<OnAutoModerationRuleCreationData> dataPackage{ makeUnique<OnAutoModerationRuleCreationData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
										}
										break;
									}
									case 5: {
										if (DiscordCoreClient::getInstance()->eventManager.onAutoModerationRuleUpdateEvent.functions.size() > 0) {
											UniquePtr<OnAutoModerationRuleUpdateData> dataPackage{ makeUnique<OnAutoModerationRuleUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
										}
										break;
									}
									case 6: {
										if (DiscordCoreClient::getInstance()->eventManager.onAutoModerationRuleDeletionEvent.functions.size() > 0) {
											UniquePtr<OnAutoModerationRuleDeletionData> dataPackage{ makeUnique<OnAutoModerationRuleDeletionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
										}
										break;
									}
									case 7: {
										if (DiscordCoreClient::getInstance()->eventManager.onAutoModerationActionExecutionEvent.functions.size() > 0) {
											UniquePtr<OnAutoModerationActionExecutionData> dataPackage{ makeUnique<OnAutoModerationActionExecutionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
										}
										break;
									}
									case 8: {
										UniquePtr<OnChannelCreationData> dataPackage{ makeUnique<OnChannelCreationData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onChannelCreationEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onChannelCreationEvent(*dataPackage);
										}
										break;
									}
									case 9: {
										UniquePtr<OnChannelUpdateData> dataPackage{ makeUnique<OnChannelUpdateData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onChannelUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onChannelUpdateEvent(*dataPackage);
										}
										break;
									}
									case 10: {
										UniquePtr<OnChannelDeletionData> dataPackage{ makeUnique<OnChannelDeletionData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onChannelDeletionEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onChannelDeletionEvent(*dataPackage);
										}
										break;
									}
									case 11: {
										if (DiscordCoreClient::getInstance()->eventManager.onChannelPinsUpdateEvent.functions.size() > 0) {
											UniquePtr<OnChannelPinsUpdateData> dataPackage{ makeUnique<OnChannelPinsUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onChannelPinsUpdateEvent(*dataPackage);
										}
										break;
									}
									case 12: {
										if (DiscordCoreClient::getInstance()->eventManager.onThreadCreationEvent.functions.size() > 0) {
											UniquePtr<OnThreadCreationData> dataPackage{ makeUnique<OnThreadCreationData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onThreadCreationEvent(*dataPackage);
										}
										break;
									}
									case 13: {
										if (DiscordCoreClient::getInstance()->eventManager.onThreadUpdateEvent.functions.size() > 0) {
											UniquePtr<OnThreadUpdateData> dataPackage{ makeUnique<OnThreadUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onThreadUpdateEvent(*dataPackage);
										}
										break;
									}
									case 14: {
										if (DiscordCoreClient::getInstance()->eventManager.onThreadDeletionEvent.functions.size() > 0) {
											UniquePtr<OnThreadDeletionData> dataPackage{ makeUnique<OnThreadDeletionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onThreadDeletionEvent(*dataPackage);
										}
										break;
									}
									case 15: {
										if (DiscordCoreClient::getInstance()->eventManager.onThreadListSyncEvent.functions.size() > 0) {
											UniquePtr<OnThreadListSyncData> dataPackage{ makeUnique<OnThreadListSyncData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onThreadListSyncEvent(*dataPackage);
										}
										break;
									}
									case 16: {
										if (DiscordCoreClient::getInstance()->eventManager.onThreadMemberUpdateEvent.functions.size() > 0) {
											UniquePtr<OnThreadMemberUpdateData> dataPackage{ makeUnique<OnThreadMemberUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onThreadMemberUpdateEvent(*dataPackage);
										}
										break;
									}
									case 17: {
										if (DiscordCoreClient::getInstance()->eventManager.onThreadMembersUpdateEvent.functions.size() > 0) {
											UniquePtr<OnThreadMembersUpdateData> dataPackage{ makeUnique<OnThreadMembersUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onThreadMembersUpdateEvent(*dataPackage);
										}
										break;
									}
									case 18: {
										UniquePtr<OnGuildCreationData> dataPackage{ makeUnique<OnGuildCreationData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onGuildCreationEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onGuildCreationEvent(*dataPackage);
										}
										break;
									}
									case 19: {
										UniquePtr<OnGuildUpdateData> dataPackage{ makeUnique<OnGuildUpdateData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onGuildUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onGuildUpdateEvent(*dataPackage);
										}
										break;
									}
									case 20: {
										UniquePtr<OnGuildDeletionData> dataPackage{ makeUnique<OnGuildDeletionData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onGuildDeletionEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onGuildDeletionEvent(*dataPackage);
										}
										break;
									}
									case 21: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildBanAddEvent.functions.size() > 0) {
											UniquePtr<OnGuildBanAddData> dataPackage{ makeUnique<OnGuildBanAddData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildBanAddEvent(*dataPackage);
										}
										break;
									}
									case 22: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildBanRemoveEvent.functions.size() > 0) {
											UniquePtr<OnGuildBanRemoveData> dataPackage{ makeUnique<OnGuildBanRemoveData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildBanRemoveEvent(*dataPackage);
										}
										break;
									}
									case 23: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildEmojisUpdateEvent.functions.size() > 0) {
											UniquePtr<OnGuildEmojisUpdateData> dataPackage{ makeUnique<OnGuildEmojisUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
										}
										break;
									}
									case 24: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildStickersUpdateEvent.functions.size() > 0) {
											UniquePtr<OnGuildStickersUpdateData> dataPackage{ makeUnique<OnGuildStickersUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildStickersUpdateEvent(*dataPackage);
										}
										break;
									}
									case 25: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildIntegrationsUpdateEvent.functions.size() > 0) {
											UniquePtr<OnGuildIntegrationsUpdateData> dataPackage{ makeUnique<OnGuildIntegrationsUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
										}
										break;
									}
									case 26: {
										UniquePtr<OnGuildMemberAddData> dataPackage{ makeUnique<OnGuildMemberAddData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onGuildMemberAddEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onGuildMemberAddEvent(*dataPackage);
										}
										break;
									}
									case 27: {
										UniquePtr<OnGuildMemberRemoveData> dataPackage{ makeUnique<OnGuildMemberRemoveData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onGuildMemberRemoveEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onGuildMemberRemoveEvent(*dataPackage);
										}
										break;
									}
									case 28: {
										UniquePtr<OnGuildMemberUpdateData> dataPackage{ makeUnique<OnGuildMemberUpdateData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onGuildMemberUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onGuildMemberUpdateEvent(*dataPackage);
										}
										break;
									}
									case 29: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildMembersChunkEvent.functions.size() > 0) {
											UniquePtr<OnGuildMembersChunkData> dataPackage{ makeUnique<OnGuildMembersChunkData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildMembersChunkEvent(*dataPackage);
										}
										break;
									}
									case 30: {
										UniquePtr<OnRoleCreationData> dataPackage{ makeUnique<OnRoleCreationData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onRoleCreationEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onRoleCreationEvent(*dataPackage);
										}
										break;
									}
									case 31: {
										UniquePtr<OnRoleUpdateData> dataPackage{ makeUnique<OnRoleUpdateData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onRoleUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onRoleUpdateEvent(*dataPackage);
										}
										break;
									}
									case 32: {
										UniquePtr<OnRoleDeletionData> dataPackage{ makeUnique<OnRoleDeletionData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onRoleDeletionEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onRoleDeletionEvent(*dataPackage);
										}
										break;
									}
									case 33: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventCreationEvent.functions.size() > 0) {
											UniquePtr<OnGuildScheduledEventCreationData> dataPackage{ makeUnique<OnGuildScheduledEventCreationData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
										}
										break;
									}
									case 34: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventUpdateEvent.functions.size() > 0) {
											UniquePtr<OnGuildScheduledEventUpdateData> dataPackage{ makeUnique<OnGuildScheduledEventUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
										}
										break;
									}
									case 35: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventDeletionEvent.functions.size() > 0) {
											UniquePtr<OnGuildScheduledEventDeletionData> dataPackage{ makeUnique<OnGuildScheduledEventDeletionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
										}
										break;
									}
									case 36: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventUserAddEvent.functions.size() > 0) {
											UniquePtr<OnGuildScheduledEventUserAddData> dataPackage{ makeUnique<OnGuildScheduledEventUserAddData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
										}
										break;
									}
									case 37: {
										if (DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventUserRemoveEvent.functions.size() > 0) {
											UniquePtr<OnGuildScheduledEventUserRemoveData> dataPackage{ makeUnique<OnGuildScheduledEventUserRemoveData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
										}
										break;
									}
									case 38: {
										if (DiscordCoreClient::getInstance()->eventManager.onIntegrationCreationEvent.functions.size() > 0) {
											UniquePtr<OnIntegrationCreationData> dataPackage{ makeUnique<OnIntegrationCreationData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onIntegrationCreationEvent(*dataPackage);
										}
										break;
									}
									case 39: {
										if (DiscordCoreClient::getInstance()->eventManager.onIntegrationUpdateEvent.functions.size() > 0) {
											UniquePtr<OnIntegrationUpdateData> dataPackage{ makeUnique<OnIntegrationUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onIntegrationUpdateEvent(*dataPackage);
										}
										break;
									}
									case 40: {
										if (DiscordCoreClient::getInstance()->eventManager.onIntegrationDeletionEvent.functions.size() > 0) {
											UniquePtr<OnIntegrationDeletionData> dataPackage{ makeUnique<OnIntegrationDeletionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onIntegrationDeletionEvent(*dataPackage);
										}
										break;
									}
									case 41: {
										UniquePtr<OnInteractionCreationData> dataPackage{ makeUnique<OnInteractionCreationData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onInteractionCreationEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onInteractionCreationEvent(*dataPackage);
										}
										break;
									}
									case 42: {
										if (DiscordCoreClient::getInstance()->eventManager.onInviteCreationEvent.functions.size() > 0) {
											UniquePtr<OnInviteCreationData> dataPackage{ makeUnique<OnInviteCreationData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onInviteCreationEvent(*dataPackage);
										}
										break;
									}
									case 43: {
										if (DiscordCoreClient::getInstance()->eventManager.onInviteDeletionEvent.functions.size() > 0) {
											UniquePtr<OnInviteDeletionData> dataPackage{ makeUnique<OnInviteDeletionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onInviteDeletionEvent(*dataPackage);
										}
										break;
									}
									case 44: {
										UniquePtr<OnMessageCreationData> dataPackage{ makeUnique<OnMessageCreationData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onMessageCreationEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onMessageCreationEvent(*dataPackage);
										}
										break;
									}
									case 45: {
										UniquePtr<OnMessageUpdateData> dataPackage{ makeUnique<OnMessageUpdateData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onMessageUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onMessageUpdateEvent(*dataPackage);
										}
										break;
									}
									case 46: {
										if (DiscordCoreClient::getInstance()->eventManager.onMessageDeletionEvent.functions.size() > 0) {
											UniquePtr<OnMessageDeletionData> dataPackage{ makeUnique<OnMessageDeletionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onMessageDeletionEvent(*dataPackage);
										}
										break;
									}
									case 47: {
										if (DiscordCoreClient::getInstance()->eventManager.onMessageDeleteBulkEvent.functions.size() > 0) {
											UniquePtr<OnMessageDeleteBulkData> dataPackage{ makeUnique<OnMessageDeleteBulkData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onMessageDeleteBulkEvent(*dataPackage);
										}
										break;
									}
									case 48: {
										if (DiscordCoreClient::getInstance()->eventManager.onReactionAddEvent.functions.size() > 0) {
											UniquePtr<OnReactionAddData> dataPackage{ makeUnique<OnReactionAddData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onReactionAddEvent(*dataPackage);
										}
										break;
									}
									case 49: {
										if (DiscordCoreClient::getInstance()->eventManager.onReactionRemoveEvent.functions.size() > 0) {
											UniquePtr<OnReactionRemoveData> dataPackage{ makeUnique<OnReactionRemoveData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onReactionRemoveEvent(*dataPackage);
										}
										break;
									}
									case 50: {
										if (DiscordCoreClient::getInstance()->eventManager.onReactionRemoveAllEvent.functions.size() > 0) {
											UniquePtr<OnReactionRemoveAllData> dataPackage{ makeUnique<OnReactionRemoveAllData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onReactionRemoveAllEvent(*dataPackage);
										}
										break;
									}
									case 51: {
										if (DiscordCoreClient::getInstance()->eventManager.onReactionRemoveEmojiEvent.functions.size() > 0) {
											UniquePtr<OnReactionRemoveEmojiData> dataPackage{ makeUnique<OnReactionRemoveEmojiData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
										}
										break;
									}
									case 52: {
										UniquePtr<OnPresenceUpdateData> dataPackage{ makeUnique<OnPresenceUpdateData>(parser, dataNew) };
										if (DiscordCoreClient::getInstance()->eventManager.onPresenceUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onPresenceUpdateEvent(*dataPackage);
										}
										break;
									}
									case 53: {
										if (DiscordCoreClient::getInstance()->eventManager.onStageInstanceCreationEvent.functions.size() > 0) {
											UniquePtr<OnStageInstanceCreationData> dataPackage{ makeUnique<OnStageInstanceCreationData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onStageInstanceCreationEvent(*dataPackage);
										}
										break;
									}
									case 54: {
										if (DiscordCoreClient::getInstance()->eventManager.onStageInstanceUpdateEvent.functions.size() > 0) {
											UniquePtr<OnStageInstanceUpdateData> dataPackage{ makeUnique<OnStageInstanceUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onStageInstanceUpdateEvent(*dataPackage);
										}
										break;
									}
									case 55: {
										if (DiscordCoreClient::getInstance()->eventManager.onStageInstanceDeletionEvent.functions.size() > 0) {
											UniquePtr<OnStageInstanceDeletionData> dataPackage{ makeUnique<OnStageInstanceDeletionData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onStageInstanceDeletionEvent(*dataPackage);
										}
										break;
									}
									case 56: {
										if (DiscordCoreClient::getInstance()->eventManager.onTypingStartEvent.functions.size() > 0) {
											UniquePtr<OnTypingStartData> dataPackage{ makeUnique<OnTypingStartData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onTypingStartEvent(*dataPackage);
										}
										break;
									}
									case 57: {
										if (DiscordCoreClient::getInstance()->eventManager.onUserUpdateEvent.functions.size() > 0) {
											UniquePtr<OnUserUpdateData> dataPackage{ makeUnique<OnUserUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onUserUpdateEvent(*dataPackage);
										}
										break;
									}
									case 58: {
										UniquePtr<OnVoiceStateUpdateData> dataPackage{ makeUnique<OnVoiceStateUpdateData>(parser, dataNew, this) };
										if (DiscordCoreClient::getInstance()->eventManager.onVoiceStateUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onVoiceStateUpdateEvent(*dataPackage);
										}
										break;
									}
									case 59: {
										UniquePtr<OnVoiceServerUpdateData> dataPackage{ makeUnique<OnVoiceServerUpdateData>(parser, dataNew, this) };
										if (DiscordCoreClient::getInstance()->eventManager.onVoiceServerUpdateEvent.functions.size() > 0) {
											DiscordCoreClient::getInstance()->eventManager.onVoiceServerUpdateEvent(*dataPackage);
										}
										break;
									}
									case 60: {
										if (DiscordCoreClient::getInstance()->eventManager.onWebhookUpdateEvent.functions.size() > 0) {
											UniquePtr<OnWebhookUpdateData> dataPackage{ makeUnique<OnWebhookUpdateData>(parser, dataNew) };
											DiscordCoreClient::getInstance()->eventManager.onWebhookUpdateEvent(*dataPackage);
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
							MessagePrinter::printError<PrintMessageType::WebSocket>(
								jsonifier::string{ "Shard [" + std::to_string(shard.at(0)) + "," + std::to_string(shard.at(1)) + "]" + " Reconnecting (Type 7)!" });
							areWeResuming = true;
							tcpConnection.disconnect();
							return true;
						}
						case WebSocketOpCodes::Invalid_Session: {
							WebSocketMessageData<bool> data{};
							parser.parseJson<true, true>(data, dataNew);
							MessagePrinter::printError<PrintMessageType::WebSocket>(
								jsonifier::string{ "Shard [" + std::to_string(shard.at(0)) + "," + std::to_string(shard.at(1)) + "]" + " Reconnecting (Type 9)!" });
							std::mt19937_64 randomEngine{ static_cast<uint64_t>(HRClock::now().time_since_epoch().count()) };
							uint64_t numOfMsToWait =
								static_cast<uint64_t>(1000.0f + ((static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max())) * static_cast<double>(4000.0f)));
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
								areWeHeartBeating  = true;
								heartBeatStopWatch = StopWatch<Milliseconds>{ Milliseconds{ data.d.heartbeatInterval } };
								heartBeatStopWatch.reset();
								haveWeReceivedHeartbeatAck = true;
							}
							if (areWeResuming) {
								WebSocketMessageData<WebSocketResumeData> dataNewer{};
								dataNewer.d.botToken		   = configManager->getBotToken();
								dataNewer.d.lastNumberReceived = lastNumberReceived;
								dataNewer.d.sessionId		   = sessionId;
								dataNewer.op				   = 6;
								std::basic_string<uint8_t> string{};
								if (dataOpCode == WebSocketOpCode::Op_Binary) {
									auto serializer = dataNewer.operator EtfSerializer();
									string			= serializer.operator std::basic_string<uint8_t>();
								} else {
									parser.serializeJson(dataNewer, string);
								}
								createHeader(string, dataOpCode);
								currentState.store(WebSocketState::Sending_Identify, std::memory_order_release);
								if (!sendMessage(string, true)) {
									return false;
								}
							} else {
								WebSocketMessageData<WebSocketIdentifyData> dataNewer{};
								dataNewer.d.botToken	= configManager->getBotToken();
								dataNewer.d.shard.at(0) = shard.at(0);
								dataNewer.d.shard.at(1) = shard.at(1);
								dataNewer.d.intents		= static_cast<int64_t>(configManager->getGatewayIntents());
								dataNewer.d.presence	= configManager->getPresenceData();
								for (auto& value: dataNewer.d.presence.activities) {
									if (value.url == "") {
										value.excludedKeys.emplace("url");
									}
								}
								dataNewer.op = 2;
								std::basic_string<uint8_t> string{};
								if (dataOpCode == WebSocketOpCode::Op_Binary) {
									auto serializer = dataNewer.operator EtfSerializer();
									string			= serializer.operator std::basic_string<uint8_t>();
								} else {
									parser.serializeJson<true>(dataNewer, string);
								}
								createHeader(string, dataOpCode);
								currentState.store(WebSocketState::Sending_Identify, std::memory_order_release);
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

			} catch (const DCAException& error) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
			}
			return false;
		}

		void WebSocketCore::disconnect() {
			if (areWeConnected()) {
				std::string dataNew{ "\x03\xE8" };
				createHeader(dataNew, WebSocketOpCode::Op_Close);
				tcpConnection.writeData(static_cast<std::string_view>(dataNew), true);
				tcpConnection.disconnect();
				currentState.store(WebSocketState::Disconnected, std::memory_order_release);
				areWeHeartBeating = false;
			}
		}

		void WebSocketClient::disconnect() {
			WebSocketCore::disconnect();
		}

		void WebSocketClient::onClosed() {
			if (maxReconnectTries > currentReconnectTries) {
				disconnect();
			} else {
				if (doWeQuit) {
					doWeQuit->store(true, std::memory_order_release);
				}
			}
		}

		WebSocketClient::~WebSocketClient() {
			disconnect();
		}

		BaseSocketAgent::BaseSocketAgent(std::atomic_bool* doWeQuitNew, uint64_t currentBaseSocketAgentNew) {
			currentBaseSocketAgent = currentBaseSocketAgentNew;
			doWeQuit			   = doWeQuitNew;
			taskThread			   = ThreadWrapper([this](StopToken token) {
				run(token);
			});
		}

		void BaseSocketAgent::connect(WebSocketClient& value) {
			std::string connectionUrl{ value.areWeResuming ? value.resumeUrl : DiscordCoreClient::getInstance()->configManager.getConnectionAddress() };
			MessagePrinter::printSuccess<PrintMessageType::General>(jsonifier::string{ "Connecting Shard " + std::to_string(value.shard.at(0) + 1) + " of " +
				std::to_string(DiscordCoreClient::getInstance()->configManager.getShardCountForThisProcess()) + std::string{ " Shards for this process. (" } +
				std::to_string(value.shard.at(0) + 1) + " of " + std::to_string(DiscordCoreClient::getInstance()->configManager.getTotalShardCount()) +
				std::string{ " Shards total across all processes)" } });
			std::string relativePath{ "/?v=10&encoding=" + std::string{ DiscordCoreClient::getInstance()->configManager.getTextFormat() == TextFormat::Etf ? "etf" : "json" } };

			value = WebSocketClient{ value.shard.at(0), doWeQuit };
			value.connect(connectionUrl, relativePath, DiscordCoreClient::getInstance()->configManager.getConnectionPort());
			if (value.tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
				value.onClosed();
			}
			value.tcpConnection.processIO(0);
			DiscordCoreClient::getInstance()->connectionStopWatch01.reset();
		}

		void BaseSocketAgent::run(StopToken token) {
			UnorderedMap<uint64_t, WebSocketTCPConnection*> processIOMapNew{};
			while (!DiscordCoreClient::getInstance()->areWeReadyToConnect.load(std::memory_order_acquire)) {
				std::this_thread::sleep_for(1ms);
			}
			for (auto& [key, value]: shardMap) {
				while (key != DiscordCoreClient::getInstance()->currentlyConnectingShard.load(std::memory_order_acquire) ||
					!DiscordCoreClient::getInstance()->connectionStopWatch01.hasTimeElapsed()) {
					processIOMapNew.reserve(shardMap.size());
					for (auto& [keyNew, valueNew]: shardMap) {
						if (valueNew.areWeConnected()) {
							processIOMapNew.emplace(keyNew, &valueNew.tcpConnection);
						}
					}
					TCPConnection<WebSocketTCPConnection>::processIO(processIOMapNew);
					processIOMapNew.clear();
					std::this_thread::sleep_for(1ms);
				}
				connect(value);
				DiscordCoreClient::getInstance()->currentlyConnectingShard.fetch_add(1, std::memory_order_release);
			}
			while (!token.stopRequested() && !doWeQuit->load(std::memory_order_acquire)) {
				try {
					for (auto& [key, value]: shardMap) {
						if (value.areWeConnected()) {
							processIOMapNew.emplace(key, &value.tcpConnection);
						}
					}
					TCPConnection<WebSocketTCPConnection>::processIO(processIOMapNew);
					processIOMapNew.clear();
					bool areWeConnected{};
					for (auto& [key, value]: shardMap) {
						if (value.areWeConnected()) {
							if (value.checkForAndSendHeartBeat()) {
								OnGatewayPingData dataNew{};
								dataNew.timeUntilNextPing = static_cast<int32_t>(value.heartBeatStopWatch.getTotalWaitTime().count());
								DiscordCoreClient::getInstance()->eventManager.onGatewayPingEvent(dataNew);
							}
							areWeConnected = true;
						} else {
							MessagePrinter::printError<PrintMessageType::WebSocket>(jsonifier::string{ "Connection lost for WebSocket [" + std::to_string(value.shard.at(0)) + "," +
								std::to_string(DiscordCoreClient::getInstance()->configManager.getTotalShardCount()) + "]... reconnecting." });
							std::this_thread::sleep_for(1s);
							if (DiscordCoreClient::getInstance()->connectionStopWatch01.hasTimeElapsed()) {
								connect(value);
							}
						}
					}
					if (!areWeConnected) {
						std::this_thread::sleep_for(1ms);
					}
				} catch (const DCAException& error) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
				}
			}
		}

		BaseSocketAgent::~BaseSocketAgent() {
			taskThread.requestStop();
			if (taskThread.joinable()) {
				taskThread.join();
			}
		}
	}// namespace DiscordCoreInternal
}// namespace DiscordCoreAPI