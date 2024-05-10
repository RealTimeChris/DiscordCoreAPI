/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// WebsocketClient.cpp - Source file for the webSocket related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file WebsocketClient.cpp


#include <discordcoreapi/Utilities/WebSocketClient.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Utilities/Base.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <random>
#include <stack>

namespace discord_core_api {

	extern voice_connections_map voiceConnectionMap;

	namespace discord_core_internal {

		event_converter::event_converter(const jsonifier::string& newEvent) {
			eventValue = newEvent;
		}

		event_converter::operator uint64_t() {
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

		websocket_core::websocket_core(config_manager* configManagerNew, websocket_type typeOfWebSocketNew) {
			configManager = configManagerNew;
			wsType		  = typeOfWebSocketNew;
		}

		websocket_core& websocket_core::operator=(websocket_core&& other) noexcept {
			areWeCollectingData.store(other.areWeCollectingData.load(std::memory_order_acquire), std::memory_order_release);
			currentState.store(other.currentState.load(std::memory_order_acquire), std::memory_order_release);
			heartBeatStopWatch		   = std::move(other.heartBeatStopWatch);
			haveWeReceivedHeartbeatAck = other.haveWeReceivedHeartbeatAck;
			currentMessage			   = std::move(other.currentMessage);
			tcpConnection			   = std::move(other.tcpConnection);
			currentReconnectTries	   = other.currentReconnectTries;
			lastNumberReceived		   = other.lastNumberReceived;
			maxReconnectTries		   = other.maxReconnectTries;
			areWeHeartBeating		   = other.areWeHeartBeating;
			areWeResuming			   = other.areWeResuming;
			configManager			   = other.configManager;
			dataOpCode				   = other.dataOpCode;
			shard.at(0)				   = other.shard.at(0);
			shard.at(1)				   = other.shard.at(1);
			wsType					   = other.wsType;
			return *this;
		}

		websocket_core::websocket_core(websocket_core&& other) noexcept {
			*this = std::move(other);
		}

		bool websocket_core::connect(const jsonifier::string& baseUrlNew, jsonifier::string_view relativePath, const uint16_t portNew) {
			tcpConnection = websocket_tcpconnection{ baseUrlNew, portNew, this };
			if (tcpConnection.currentStatus != connection_status::NO_Error) {
				std::this_thread::sleep_for(1s);
				return false;
			}
			currentState.store(websocket_state::upgrading, std::memory_order_release);
			jsonifier::string sendString{ "GET " + relativePath + " HTTP/1.1\r\nHost: " + baseUrlNew +
				"\r\nUpgrade: webSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " + generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n" };
			tcpConnection.writeData(static_cast<jsonifier::string_view>(sendString), true);

			if (tcpConnection.currentStatus != connection_status::NO_Error) {
				std::this_thread::sleep_for(1s);
				return false;
			}
			return true;
		}

		websocket_tcpconnection::websocket_tcpconnection(const jsonifier::string& baseUrlNew, uint16_t portNew, websocket_core* ptrNew) : tcp_connection{ baseUrlNew, portNew } {
			ptr = ptrNew;
		}

		bool websocket_core::sendMessage(jsonifier::string_base<uint8_t>& dataToSend, bool priority) {
			if (dataToSend.size() == 0) {
				return false;
			}
			jsonifier::string webSocketTitle{ wsType == websocket_type::voice ? "voice websocket" : "WebSocket" };
			message_printer::printSuccess<print_message_type::websocket>("sending " + webSocketTitle + " [" + jsonifier::toString(shard.at(0)) + "," +
				jsonifier::toString(shard.at(1)) + "]" + jsonifier::string{ "'s message: " } + dataToSend);
			std::unique_lock lock{ accessMutex };
			if (areWeConnected()) {
				tcpConnection.writeData(static_cast<jsonifier::string_view_base<uint8_t>>(dataToSend), priority);
				if (tcpConnection.currentStatus != connection_status::NO_Error) {
					onClosed();
					return false;
				}
			}
			return true;
		}

		void websocket_core::parseConnectionHeaders() {
			if (areWeConnected() && currentState.load(std::memory_order_acquire) == websocket_state::upgrading) {
				jsonifier::string_base<uint8_t> newString{};
				newString.resize(4);
				std::memcpy(newString.data(), "\r\n\r\n", 4);
				auto theFindValue = currentMessage.find(newString);
				if (theFindValue != jsonifier::string::npos) {
					currentMessage.clear();
					currentState.store(websocket_state::Collecting_Hello, std::memory_order_release);
					return;
				}
			}
		}

		bool websocket_core::checkForAndSendHeartBeat(bool isImmediate) {
			if ((currentState.load(std::memory_order_acquire) == websocket_state::authenticated && heartBeatStopWatch.hasTimeElapsed() && haveWeReceivedHeartbeatAck) ||
				isImmediate) {
				jsonifier::string_base<uint8_t> string{};
				if (dataOpCode == websocket_op_code::Op_Binary) {
					etf_serializer data{};
					data["d"]  = lastNumberReceived;
					data["op"] = 1;
					string	   = data.operator jsonifier::string_base<uint8_t>();
				} else {
					websocket_message_data<uint64_t> message{};
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

		bool websocket_core::parseMessage() {
			if (currentMessage.size() < 4) {
				return false;
			} else {
				websocket_op_code opcode = static_cast<websocket_op_code>(currentMessage.at(0) & ~webSocketMaskBit);
				switch (opcode) {
					case websocket_op_code::Op_Continuation:
						[[fallthrough]];
					case websocket_op_code::Op_Text:
						[[fallthrough]];
					case websocket_op_code::Op_Binary:
						[[fallthrough]];
					case websocket_op_code::Op_Ping:
						[[fallthrough]];
					case websocket_op_code::Op_Pong: {
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
							lengthFinal		 = static_cast<uint64_t>((length01 << 8ULL) | length02);

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

						if (opcode == websocket_op_code::Op_Ping || opcode == websocket_op_code::Op_Pong) {
							return false;
						} else {
							onMessageReceived({ currentMessage.data() + messageOffset, lengthFinal });
							currentMessage.erase(messageOffset + lengthFinal);
						}
						return true;
					} break;
					case websocket_op_code::Op_Close: {
						uint16_t closeValue = static_cast<uint16_t>(currentMessage.at(2) & 0xff);
						closeValue <<= 8;
						closeValue |= currentMessage.at(3) & 0xff;
						jsonifier::string closeString{};
						if (wsType == websocket_type::voice) {
							voice_websocket_close voiceClose{ closeValue };
							closeString = static_cast<jsonifier::string>(voiceClose.operator jsonifier::string_view());
						} else {
							websocket_close wsClose{ closeValue };
							closeString = static_cast<jsonifier::string>(wsClose.operator jsonifier::string_view());
						}
						jsonifier::string webSocketTitle = wsType == websocket_type::voice ? "voice websocket" : "WebSocket";
						message_printer::printError<print_message_type::websocket>(webSocketTitle + " [" + jsonifier::toString(shard.at(0)) + "," +
							jsonifier::toString(shard.at(1)) + "]" + " closed; code: " + jsonifier::toString(closeValue) + ", " + closeString);
						return false;
					} break;

					default: {
						return false;
					} break;
				}
			}
		}

		bool websocket_core::areWeConnected() {
			return tcpConnection.areWeStillConnected();
		}

		void websocket_tcpconnection::handleBuffer() {
			auto inputBufferNew = getInputBuffer();
			auto oldSize		= ptr->currentMessage.size();
			if (ptr->currentMessage.size() < oldSize + inputBufferNew.size()) {
				ptr->currentMessage.resize(oldSize + inputBufferNew.size());
			}
			std::memcpy(ptr->currentMessage.data() + oldSize, inputBufferNew.data(), inputBufferNew.size());
			if (ptr->currentState.load(std::memory_order_acquire) == websocket_state::upgrading) {
				ptr->parseConnectionHeaders();
			} else {
				while (ptr->parseMessage()) {
				};
			}
		}

		websocket_client::websocket_client(uint64_t currentShardNew, std::atomic_bool* doWeQuitNew)
			: websocket_core(&discord_core_client::getInstance()->configManager, websocket_type::normal) {
			configManager = &discord_core_client::getInstance()->configManager;
			shard.at(0)	  = currentShardNew;
			doWeQuit	  = doWeQuitNew;
			if (discord_core_client::getInstance()) {
				shard.at(1) = discord_core_client::getInstance()->configManager.getTotalShardCount();
				if (discord_core_client::getInstance()->configManager.getTextFormat() == text_format::etf) {
					dataOpCode = websocket_op_code::Op_Binary;
				} else {
					dataOpCode = websocket_op_code::Op_Text;
				}
			}
		}

		void websocket_client::getVoiceConnectionData(const voice_connect_init_data& doWeCollect) {
			while (currentState.load(std::memory_order_acquire) != websocket_state::authenticated) {
				std::this_thread::sleep_for(1ms);
			}
			websocket_message_data<update_voice_state_data_dc> data01{};
			data01.jsonifierExcludedKeys.emplace("t");
			data01.jsonifierExcludedKeys.emplace("s");
			data01.d.channelId = std::nullptr_t{};
			data01.d.guildId   = doWeCollect.guildId;
			data01.d.selfDeaf  = doWeCollect.selfDeaf;
			data01.d.selfMute  = doWeCollect.selfMute;
			data01.op		   = 4;
			userId			   = doWeCollect.userId;

			websocket_message_data<update_voice_state_data> data02{};
			data02.jsonifierExcludedKeys.emplace("t");
			data02.jsonifierExcludedKeys.emplace("s");
			data02.d.channelId = doWeCollect.channelId;
			data02.d.guildId   = doWeCollect.guildId;
			data02.d.selfDeaf  = doWeCollect.selfDeaf;
			data02.d.selfMute  = doWeCollect.selfMute;
			data02.op		   = 4;
			jsonifier::string_base<uint8_t> string{};
			auto serializer = data01.operator etf_serializer();
			if (dataOpCode == websocket_op_code::Op_Binary) {
				string = serializer.operator jsonifier::string_base<uint8_t>();
			} else {
				parser.serializeJson(data01, string);
			}
			createHeader(string, dataOpCode);
			if (!sendMessage(string, true)) {
				return;
			}
			if (snowflake{ doWeCollect.channelId } == 0) {
				return;
			}
			serializer = data02.operator etf_serializer();
			if (dataOpCode == websocket_op_code::Op_Binary) {
				string = serializer.operator jsonifier::string_base<uint8_t>();
			} else {
				parser.serializeJson(data02, string);
			}
			createHeader(string, dataOpCode);
			areWeCollectingData.store(true, std::memory_order_release);
			if (!sendMessage(string, true)) {
				return;
			}
		}

		uint64_t countBackSlashes(jsonifier::string_base<uint8_t>::iterator& iter, jsonifier::string_base<uint8_t>::iterator end, uint64_t currentCount = 0) {
			if (iter < end) {
				if (*iter == '\\') {
					++currentCount;
					++iter;
					return countBackSlashes(iter, end, currentCount);
				}
			}
			++iter;
			return currentCount;
		}

		bool websocket_client::onMessageReceived(jsonifier::string_view_base<uint8_t> dataNew) {
			try {
				if (areWeConnected() && currentMessage.size() > 0 && dataNew.size() > 0) {
					websocket_message message{};
					if (configManager->getTextFormat() == text_format::etf) {
						try {
							dataNew = etfParser.parseEtfToJson(dataNew);
							parser.parseJson(message, dataNew);
							for (auto& valueNew: parser.getErrors()) {
								message_printer::printError<print_message_type::websocket>(valueNew.reportError() + ", for data:" + dataNew);
							}
						} catch (const dca_exception& error) {
							message_printer::printError<print_message_type::websocket>(error.what());
							tcpConnection.getInputBuffer();
							currentMessage.clear();
							return false;
						}
					} else {
						parser.parseJson(message, dataNew);
						if (auto result = parser.getErrors(); result.size() > 0) {
							for (auto& valueNew: result) {
								message_printer::printError<print_message_type::websocket>(valueNew.reportError() + ", for data:" + dataNew);
							}
						}
					}

					if (message.s != 0) {
						lastNumberReceived = static_cast<uint32_t>(message.s);
					}
					message_printer::printSuccess<print_message_type::websocket>("Message received from websocket [" + jsonifier::toString(shard.at(0)) + "," +
						jsonifier::toString(shard.at(1)) + jsonifier::string("]: ") + jsonifier::string{ dataNew });
					switch (static_cast<websocket_op_codes>(message.op)) {
						case websocket_op_codes::dispatch: {
							if (message.t != "") {
								switch (event_converter{ message.t }) {
									case 1: {
										websocket_message_data<ready_data> data{};
										if (dataOpCode == websocket_op_code::Op_Text) {
											data.d.jsonifierExcludedKeys.emplace("shard");
										}
										currentState.store(websocket_state::authenticated, std::memory_order_release);
										parser.parseJson(data, dataNew);
										if (auto result = parser.getErrors(); result.size() > 0) {
											for (auto& valueNew: result) {
												message_printer::printError<print_message_type::websocket>(valueNew.reportError());
											}
										}
										sessionId = data.d.sessionId;
										if (data.d.resumeGatewayUrl.find("wss://") != jsonifier::string::npos) {
											resumeUrl = data.d.resumeGatewayUrl.substr(data.d.resumeGatewayUrl.find("wss://") + jsonifier::string{ "wss://" }.size());
										}
										discord_core_client::getInstance()->currentUser = bot_user{ data.d.user,
											discord_core_client::getInstance()
												->baseSocketAgentsMap[static_cast<uint64_t>(floor(
													static_cast<uint64_t>(shard.at(0)) % static_cast<uint64_t>(discord_core_client::getInstance()->baseSocketAgentsMap.size())))]
												.get() };
										users::insertUser(static_cast<user_cache_data>(std::move(data.d.user)));
										currentReconnectTries = 0;
										break;
									}
									case 2: {
										currentState.store(websocket_state::authenticated, std::memory_order_release);
										currentReconnectTries = 0;
										break;
									}
									case 3: {
										if (discord_core_client::getInstance()->eventManager.onApplicationCommandPermissionsUpdateEvent.functions.size() > 0) {
											unique_ptr<on_application_command_permissions_update_data> dataPackage{ makeUnique<on_application_command_permissions_update_data>(
												parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onApplicationCommandPermissionsUpdateEvent(*dataPackage);
										}
										break;
									}
									case 4: {
										if (discord_core_client::getInstance()->eventManager.onAutoModerationRuleCreationEvent.functions.size() > 0) {
											unique_ptr<on_auto_moderation_rule_creation_data> dataPackage{ makeUnique<on_auto_moderation_rule_creation_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onAutoModerationRuleCreationEvent(*dataPackage);
										}
										break;
									}
									case 5: {
										if (discord_core_client::getInstance()->eventManager.onAutoModerationRuleUpdateEvent.functions.size() > 0) {
											unique_ptr<on_auto_moderation_rule_update_data> dataPackage{ makeUnique<on_auto_moderation_rule_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onAutoModerationRuleUpdateEvent(*dataPackage);
										}
										break;
									}
									case 6: {
										if (discord_core_client::getInstance()->eventManager.onAutoModerationRuleDeletionEvent.functions.size() > 0) {
											unique_ptr<on_auto_moderation_rule_deletion_data> dataPackage{ makeUnique<on_auto_moderation_rule_deletion_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onAutoModerationRuleDeletionEvent(*dataPackage);
										}
										break;
									}
									case 7: {
										if (discord_core_client::getInstance()->eventManager.onAutoModerationActionExecutionEvent.functions.size() > 0) {
											unique_ptr<on_auto_moderation_action_execution_data> dataPackage{ makeUnique<on_auto_moderation_action_execution_data>(parser,
												dataNew) };
											discord_core_client::getInstance()->eventManager.onAutoModerationActionExecutionEvent(*dataPackage);
										}
										break;
									}
									case 8: {
										unique_ptr<on_channel_creation_data> dataPackage{ makeUnique<on_channel_creation_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onChannelCreationEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onChannelCreationEvent(*dataPackage);
										}
										break;
									}
									case 9: {
										unique_ptr<on_channel_update_data> dataPackage{ makeUnique<on_channel_update_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onChannelUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onChannelUpdateEvent(*dataPackage);
										}
										break;
									}
									case 10: {
										unique_ptr<on_channel_deletion_data> dataPackage{ makeUnique<on_channel_deletion_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onChannelDeletionEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onChannelDeletionEvent(*dataPackage);
										}
										break;
									}
									case 11: {
										if (discord_core_client::getInstance()->eventManager.onChannelPinsUpdateEvent.functions.size() > 0) {
											unique_ptr<on_channel_pins_update_data> dataPackage{ makeUnique<on_channel_pins_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onChannelPinsUpdateEvent(*dataPackage);
										}
										break;
									}
									case 12: {
										if (discord_core_client::getInstance()->eventManager.onThreadCreationEvent.functions.size() > 0) {
											unique_ptr<on_thread_creation_data> dataPackage{ makeUnique<on_thread_creation_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onThreadCreationEvent(*dataPackage);
										}
										break;
									}
									case 13: {
										if (discord_core_client::getInstance()->eventManager.onThreadUpdateEvent.functions.size() > 0) {
											unique_ptr<on_thread_update_data> dataPackage{ makeUnique<on_thread_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onThreadUpdateEvent(*dataPackage);
										}
										break;
									}
									case 14: {
										if (discord_core_client::getInstance()->eventManager.onThreadDeletionEvent.functions.size() > 0) {
											unique_ptr<on_thread_deletion_data> dataPackage{ makeUnique<on_thread_deletion_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onThreadDeletionEvent(*dataPackage);
										}
										break;
									}
									case 15: {
										if (discord_core_client::getInstance()->eventManager.onThreadListSyncEvent.functions.size() > 0) {
											unique_ptr<on_thread_list_sync_data> dataPackage{ makeUnique<on_thread_list_sync_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onThreadListSyncEvent(*dataPackage);
										}
										break;
									}
									case 16: {
										if (discord_core_client::getInstance()->eventManager.onThreadMemberUpdateEvent.functions.size() > 0) {
											unique_ptr<on_thread_member_update_data> dataPackage{ makeUnique<on_thread_member_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onThreadMemberUpdateEvent(*dataPackage);
										}
										break;
									}
									case 17: {
										if (discord_core_client::getInstance()->eventManager.onThreadMembersUpdateEvent.functions.size() > 0) {
											unique_ptr<on_thread_members_update_data> dataPackage{ makeUnique<on_thread_members_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onThreadMembersUpdateEvent(*dataPackage);
										}
										break;
									}
									case 18: {
										unique_ptr<on_guild_creation_data> dataPackage{ makeUnique<on_guild_creation_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onGuildCreationEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onGuildCreationEvent(*dataPackage);
										}
										break;
									}
									case 19: {
										unique_ptr<on_guild_update_data> dataPackage{ makeUnique<on_guild_update_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onGuildUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onGuildUpdateEvent(*dataPackage);
										}
										break;
									}
									case 20: {
										unique_ptr<on_guild_deletion_data> dataPackage{ makeUnique<on_guild_deletion_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onGuildDeletionEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onGuildDeletionEvent(*dataPackage);
										}
										break;
									}
									case 21: {
										if (discord_core_client::getInstance()->eventManager.onGuildBanAddEvent.functions.size() > 0) {
											unique_ptr<on_guild_ban_add_data> dataPackage{ makeUnique<on_guild_ban_add_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildBanAddEvent(*dataPackage);
										}
										break;
									}
									case 22: {
										if (discord_core_client::getInstance()->eventManager.onGuildBanRemoveEvent.functions.size() > 0) {
											unique_ptr<on_guild_ban_remove_data> dataPackage{ makeUnique<on_guild_ban_remove_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildBanRemoveEvent(*dataPackage);
										}
										break;
									}
									case 23: {
										if (discord_core_client::getInstance()->eventManager.onGuildEmojisUpdateEvent.functions.size() > 0) {
											unique_ptr<on_guild_emojis_update_data> dataPackage{ makeUnique<on_guild_emojis_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildEmojisUpdateEvent(*dataPackage);
										}
										break;
									}
									case 24: {
										if (discord_core_client::getInstance()->eventManager.onGuildStickersUpdateEvent.functions.size() > 0) {
											unique_ptr<on_guild_stickers_update_data> dataPackage{ makeUnique<on_guild_stickers_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildStickersUpdateEvent(*dataPackage);
										}
										break;
									}
									case 25: {
										if (discord_core_client::getInstance()->eventManager.onGuildIntegrationsUpdateEvent.functions.size() > 0) {
											unique_ptr<on_guild_integrations_update_data> dataPackage{ makeUnique<on_guild_integrations_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildIntegrationsUpdateEvent(*dataPackage);
										}
										break;
									}
									case 26: {
										unique_ptr<on_guild_member_add_data> dataPackage{ makeUnique<on_guild_member_add_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onGuildMemberAddEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onGuildMemberAddEvent(*dataPackage);
										}
										break;
									}
									case 27: {
										unique_ptr<on_guild_member_remove_data> dataPackage{ makeUnique<on_guild_member_remove_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onGuildMemberRemoveEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onGuildMemberRemoveEvent(*dataPackage);
										}
										break;
									}
									case 28: {
										unique_ptr<on_guild_member_update_data> dataPackage{ makeUnique<on_guild_member_update_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onGuildMemberUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onGuildMemberUpdateEvent(*dataPackage);
										}
										break;
									}
									case 29: {
										if (discord_core_client::getInstance()->eventManager.onGuildMembersChunkEvent.functions.size() > 0) {
											unique_ptr<on_guild_members_chunk_data> dataPackage{ makeUnique<on_guild_members_chunk_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildMembersChunkEvent(*dataPackage);
										}
										break;
									}
									case 30: {
										unique_ptr<on_role_creation_data> dataPackage{ makeUnique<on_role_creation_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onRoleCreationEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onRoleCreationEvent(*dataPackage);
										}
										break;
									}
									case 31: {
										unique_ptr<on_role_update_data> dataPackage{ makeUnique<on_role_update_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onRoleUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onRoleUpdateEvent(*dataPackage);
										}
										break;
									}
									case 32: {
										unique_ptr<on_role_deletion_data> dataPackage{ makeUnique<on_role_deletion_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onRoleDeletionEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onRoleDeletionEvent(*dataPackage);
										}
										break;
									}
									case 33: {
										if (discord_core_client::getInstance()->eventManager.onGuildScheduledEventCreationEvent.functions.size() > 0) {
											unique_ptr<on_guild_scheduled_event_creation_data> dataPackage{ makeUnique<on_guild_scheduled_event_creation_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildScheduledEventCreationEvent(*dataPackage);
										}
										break;
									}
									case 34: {
										if (discord_core_client::getInstance()->eventManager.onGuildScheduledEventUpdateEvent.functions.size() > 0) {
											unique_ptr<on_guild_scheduled_event_update_data> dataPackage{ makeUnique<on_guild_scheduled_event_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildScheduledEventUpdateEvent(*dataPackage);
										}
										break;
									}
									case 35: {
										if (discord_core_client::getInstance()->eventManager.onGuildScheduledEventDeletionEvent.functions.size() > 0) {
											unique_ptr<on_guild_scheduled_event_deletion_data> dataPackage{ makeUnique<on_guild_scheduled_event_deletion_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildScheduledEventDeletionEvent(*dataPackage);
										}
										break;
									}
									case 36: {
										if (discord_core_client::getInstance()->eventManager.onGuildScheduledEventUserAddEvent.functions.size() > 0) {
											unique_ptr<on_guild_scheduled_event_user_add_data> dataPackage{ makeUnique<on_guild_scheduled_event_user_add_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildScheduledEventUserAddEvent(*dataPackage);
										}
										break;
									}
									case 37: {
										if (discord_core_client::getInstance()->eventManager.onGuildScheduledEventUserRemoveEvent.functions.size() > 0) {
											unique_ptr<on_guild_scheduled_event_user_remove_data> dataPackage{ makeUnique<on_guild_scheduled_event_user_remove_data>(parser,
												dataNew) };
											discord_core_client::getInstance()->eventManager.onGuildScheduledEventUserRemoveEvent(*dataPackage);
										}
										break;
									}
									case 38: {
										if (discord_core_client::getInstance()->eventManager.onIntegrationCreationEvent.functions.size() > 0) {
											unique_ptr<on_integration_creation_data> dataPackage{ makeUnique<on_integration_creation_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onIntegrationCreationEvent(*dataPackage);
										}
										break;
									}
									case 39: {
										if (discord_core_client::getInstance()->eventManager.onIntegrationUpdateEvent.functions.size() > 0) {
											unique_ptr<on_integration_update_data> dataPackage{ makeUnique<on_integration_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onIntegrationUpdateEvent(*dataPackage);
										}
										break;
									}
									case 40: {
										if (discord_core_client::getInstance()->eventManager.onIntegrationDeletionEvent.functions.size() > 0) {
											unique_ptr<on_integration_deletion_data> dataPackage{ makeUnique<on_integration_deletion_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onIntegrationDeletionEvent(*dataPackage);
										}
										break;
									}
									case 41: {
										unique_ptr<on_interaction_creation_data> dataPackage{ makeUnique<on_interaction_creation_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onInteractionCreationEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onInteractionCreationEvent(*dataPackage);
										}
										break;
									}
									case 42: {
										if (discord_core_client::getInstance()->eventManager.onInviteCreationEvent.functions.size() > 0) {
											unique_ptr<on_invite_creation_data> dataPackage{ makeUnique<on_invite_creation_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onInviteCreationEvent(*dataPackage);
										}
										break;
									}
									case 43: {
										if (discord_core_client::getInstance()->eventManager.onInviteDeletionEvent.functions.size() > 0) {
											unique_ptr<on_invite_deletion_data> dataPackage{ makeUnique<on_invite_deletion_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onInviteDeletionEvent(*dataPackage);
										}
										break;
									}
									case 44: {
										unique_ptr<on_message_creation_data> dataPackage{ makeUnique<on_message_creation_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onMessageCreationEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onMessageCreationEvent(*dataPackage);
										}
										break;
									}
									case 45: {
										unique_ptr<on_message_update_data> dataPackage{ makeUnique<on_message_update_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onMessageUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onMessageUpdateEvent(*dataPackage);
										}
										break;
									}
									case 46: {
										if (discord_core_client::getInstance()->eventManager.onMessageDeletionEvent.functions.size() > 0) {
											unique_ptr<on_message_deletion_data> dataPackage{ makeUnique<on_message_deletion_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onMessageDeletionEvent(*dataPackage);
										}
										break;
									}
									case 47: {
										if (discord_core_client::getInstance()->eventManager.onMessageDeleteBulkEvent.functions.size() > 0) {
											unique_ptr<on_message_delete_bulk_data> dataPackage{ makeUnique<on_message_delete_bulk_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onMessageDeleteBulkEvent(*dataPackage);
										}
										break;
									}
									case 48: {
										if (discord_core_client::getInstance()->eventManager.onReactionAddEvent.functions.size() > 0) {
											unique_ptr<on_reaction_add_data> dataPackage{ makeUnique<on_reaction_add_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onReactionAddEvent(*dataPackage);
										}
										break;
									}
									case 49: {
										if (discord_core_client::getInstance()->eventManager.onReactionRemoveEvent.functions.size() > 0) {
											unique_ptr<on_reaction_remove_data> dataPackage{ makeUnique<on_reaction_remove_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onReactionRemoveEvent(*dataPackage);
										}
										break;
									}
									case 50: {
										if (discord_core_client::getInstance()->eventManager.onReactionRemoveAllEvent.functions.size() > 0) {
											unique_ptr<on_reaction_remove_all_data> dataPackage{ makeUnique<on_reaction_remove_all_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onReactionRemoveAllEvent(*dataPackage);
										}
										break;
									}
									case 51: {
										if (discord_core_client::getInstance()->eventManager.onReactionRemoveEmojiEvent.functions.size() > 0) {
											unique_ptr<on_reaction_remove_emoji_data> dataPackage{ makeUnique<on_reaction_remove_emoji_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onReactionRemoveEmojiEvent(*dataPackage);
										}
										break;
									}
									case 52: {
										unique_ptr<on_presence_update_data> dataPackage{ makeUnique<on_presence_update_data>(parser, dataNew) };
										if (discord_core_client::getInstance()->eventManager.onPresenceUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onPresenceUpdateEvent(*dataPackage);
										}
										break;
									}
									case 53: {
										if (discord_core_client::getInstance()->eventManager.onStageInstanceCreationEvent.functions.size() > 0) {
											unique_ptr<on_stage_instance_creation_data> dataPackage{ makeUnique<on_stage_instance_creation_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onStageInstanceCreationEvent(*dataPackage);
										}
										break;
									}
									case 54: {
										if (discord_core_client::getInstance()->eventManager.onStageInstanceUpdateEvent.functions.size() > 0) {
											unique_ptr<on_stage_instance_update_data> dataPackage{ makeUnique<on_stage_instance_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onStageInstanceUpdateEvent(*dataPackage);
										}
										break;
									}
									case 55: {
										if (discord_core_client::getInstance()->eventManager.onStageInstanceDeletionEvent.functions.size() > 0) {
											unique_ptr<on_stage_instance_deletion_data> dataPackage{ makeUnique<on_stage_instance_deletion_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onStageInstanceDeletionEvent(*dataPackage);
										}
										break;
									}
									case 56: {
										if (discord_core_client::getInstance()->eventManager.onTypingStartEvent.functions.size() > 0) {
											unique_ptr<on_typing_start_data> dataPackage{ makeUnique<on_typing_start_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onTypingStartEvent(*dataPackage);
										}
										break;
									}
									case 57: {
										if (discord_core_client::getInstance()->eventManager.onUserUpdateEvent.functions.size() > 0) {
											unique_ptr<on_user_update_data> dataPackage{ makeUnique<on_user_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onUserUpdateEvent(*dataPackage);
										}
										break;
									}
									case 58: {
										unique_ptr<on_voice_state_update_data> dataPackage{ makeUnique<on_voice_state_update_data>(parser, dataNew, this) };
										if (discord_core_client::getInstance()->eventManager.onVoiceStateUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onVoiceStateUpdateEvent(*dataPackage);
										}
										break;
									}
									case 59: {
										unique_ptr<on_voice_server_update_data> dataPackage{ makeUnique<on_voice_server_update_data>(parser, dataNew, this) };
										if (discord_core_client::getInstance()->eventManager.onVoiceServerUpdateEvent.functions.size() > 0) {
											discord_core_client::getInstance()->eventManager.onVoiceServerUpdateEvent(*dataPackage);
										}
										break;
									}
									case 60: {
										if (discord_core_client::getInstance()->eventManager.onWebhookUpdateEvent.functions.size() > 0) {
											unique_ptr<on_webhook_update_data> dataPackage{ makeUnique<on_webhook_update_data>(parser, dataNew) };
											discord_core_client::getInstance()->eventManager.onWebhookUpdateEvent(*dataPackage);
										}
										break;
									}
								}
							}
							break;
						}
						case websocket_op_codes::heartbeat: {
							if (!checkForAndSendHeartBeat(true)) {
								return false;
							}
							break;
						}
						case websocket_op_codes::reconnect: {
							message_printer::printError<print_message_type::websocket>(
								"Shard [" + jsonifier::toString(shard.at(0)) + "," + jsonifier::toString(shard.at(1)) + "]" + " reconnecting (type 7)!");
							areWeResuming = true;
							tcpConnection.disconnect();
							return true;
						}
						case websocket_op_codes::Invalid_Session: {
							websocket_message_data<bool> data{};
							parser.parseJson(data, dataNew);
							if (auto result = parser.getErrors(); result.size() > 0) {
								for (auto& valueNew: result) {
									message_printer::printError<print_message_type::websocket>(valueNew.reportError());
								}
							}
							message_printer::printError<print_message_type::websocket>(
								"Shard [" + jsonifier::toString(shard.at(0)) + "," + jsonifier::toString(shard.at(1)) + "]" + " reconnecting (type 9)!");
							std::mt19937_64 randomEngine{ static_cast<uint64_t>(sys_clock::now().time_since_epoch().count()) };
							uint64_t numOfMsToWait =
								static_cast<uint64_t>(1000.0f + ((static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max())) * static_cast<double>(4000.0f)));
							if (numOfMsToWait <= 5000 && numOfMsToWait > 0) {
								std::this_thread::sleep_for(milliseconds{ numOfMsToWait });
							}
							if (data.d == true) {
								areWeResuming = true;
							} else {
								areWeResuming = false;
							}
							onClosed();
							return true;
						}
						case websocket_op_codes::hello: {
							websocket_message_data<hello_data> data{};
							parser.parseJson(data, dataNew);
							if (auto result = parser.getErrors(); result.size() > 0) {
								for (auto& valueNew: result) {
									message_printer::printError<print_message_type::websocket>(valueNew.reportError());
								}
							}
							if (data.d.heartbeatInterval != 0) {
								areWeHeartBeating  = true;
								heartBeatStopWatch = stop_watch<milliseconds>{ milliseconds{ data.d.heartbeatInterval } };
								heartBeatStopWatch.reset();
								haveWeReceivedHeartbeatAck = true;
							}
							if (areWeResuming) {
								websocket_message_data<websocket_resume_data> dataNewer{};
								dataNewer.d.botToken		   = configManager->getBotToken();
								dataNewer.d.lastNumberReceived = lastNumberReceived;
								dataNewer.d.sessionId		   = sessionId;
								dataNewer.op				   = 6;
								jsonifier::string_base<uint8_t> string{};
								if (dataOpCode == websocket_op_code::Op_Binary) {
									auto serializer = dataNewer.operator etf_serializer();
									string			= serializer.operator jsonifier::string_base<uint8_t>();
								} else {
									parser.serializeJson(dataNewer, string);
								}
								createHeader(string, dataOpCode);
								currentState.store(websocket_state::Sending_Identify, std::memory_order_release);
								if (!sendMessage(string, true)) {
									return false;
								}
							} else {
								websocket_message_data<websocket_identify_data> dataNewer{};
								dataNewer.d.botToken	= configManager->getBotToken();
								dataNewer.d.shard.at(0) = shard.at(0);
								dataNewer.d.shard.at(1) = shard.at(1);
								dataNewer.d.intents		= static_cast<int64_t>(configManager->getGatewayIntents());
								dataNewer.d.presence	= configManager->getPresenceData();
								for (auto& value: dataNewer.d.presence.activities) {
									if (value.url == "") {
										value.jsonifierExcludedKeys.emplace("url");
									}
								}
								dataNewer.op = 2;
								jsonifier::string_base<char> stringNew{};
								jsonifier::string_base<uint8_t> string{};
								if (dataOpCode == websocket_op_code::Op_Binary) {
									auto serializer = dataNewer.operator etf_serializer();
									string			= serializer.operator jsonifier::string_base<uint8_t>();
								} else {
									parser.serializeJson(dataNewer, stringNew);
								}
								createHeader(string, dataOpCode);
								currentState.store(websocket_state::Sending_Identify, std::memory_order_release);
								if (!sendMessage(string, true)) {
									return false;
								}
							}
							break;
						}
						case websocket_op_codes::Heartbeat_ACK: {
							haveWeReceivedHeartbeatAck = true;
							break;
						}
						case websocket_op_codes::identify: {
							[[fallthrough]];
						}
						case websocket_op_codes::Presence_Update: {
							[[fallthrough]];
						}
						case websocket_op_codes::Voice_State_Update: {
							[[fallthrough]];
						}
						case websocket_op_codes::resume: {
							[[fallthrough]];
						}
						case websocket_op_codes::Request_Guild_Members: {
							[[fallthrough]];
						}
						default: {
							break;
						}
					}
					return true;
				}

			} catch (const dca_exception& error) {
				message_printer::printError<print_message_type::websocket>(error.what());
			}
			return false;
		}

		void websocket_core::disconnect() {
			if (areWeConnected()) {
				jsonifier::string dataNew{ "\x03\xE8" };
				createHeader(dataNew, websocket_op_code::Op_Close);
				tcpConnection.writeData(static_cast<jsonifier::string_view>(dataNew), true);
				tcpConnection.disconnect();
				currentState.store(websocket_state::disconnected, std::memory_order_release);
				areWeHeartBeating = false;
			}
		}

		void websocket_client::disconnect() {
			websocket_core::disconnect();
		}

		void websocket_client::onClosed() {
			if (maxReconnectTries > currentReconnectTries) {
				disconnect();
			} else {
				if (doWeQuit) {
					doWeQuit->store(true, std::memory_order_release);
				}
			}
		}

		websocket_client::~websocket_client() {
			disconnect();
		}

		base_socket_agent::base_socket_agent(std::atomic_bool* doWeQuitNew) {
			doWeQuit   = doWeQuitNew;
			taskThread = std::jthread([this](std::stop_token token) {
				run(token);
			});
		}

		void base_socket_agent::connect(websocket_client& value) {
			jsonifier::string connectionUrl{ value.areWeResuming ? value.resumeUrl : discord_core_client::getInstance()->configManager.getConnectionAddress() };
			message_printer::printSuccess<print_message_type::general>("Connecting shard " + jsonifier::toString(value.shard.at(0) + 1) + " of " +
				jsonifier::toString(discord_core_client::getInstance()->configManager.getShardCountForThisProcess()) + jsonifier::string{ " shards for this process. (" } +
				jsonifier::toString(value.shard.at(0) + 1) + " of " + jsonifier::toString(discord_core_client::getInstance()->configManager.getTotalShardCount()) +
				jsonifier::string{ " shards total across all processes)" });
			jsonifier::string relativePath{ "/?v=10&encoding=" +
				jsonifier::string{ discord_core_client::getInstance()->configManager.getTextFormat() == text_format::etf ? "etf" : "json" } };

			value = websocket_client{ value.shard.at(0), doWeQuit };
			value.connect(connectionUrl, relativePath, discord_core_client::getInstance()->configManager.getConnectionPort());
			if (value.tcpConnection.currentStatus != connection_status::NO_Error) {
				value.onClosed();
			}
			value.tcpConnection.processIO(0);
			discord_core_client::getInstance()->connectionStopWatch01.reset();
		}

		void base_socket_agent::run(std::stop_token token) {
			unordered_map<uint64_t, websocket_tcpconnection*> processIOMapNew{};
			while (!discord_core_client::getInstance()->areWeReadyToConnect.load(std::memory_order_acquire)) {
				std::this_thread::sleep_for(1ms);
			}
			for (auto& [key, value]: shardMap) {
				while (key != discord_core_client::getInstance()->currentlyConnectingShard.load(std::memory_order_acquire) ||
					!discord_core_client::getInstance()->connectionStopWatch01.hasTimeElapsed()) {
					processIOMapNew.reserve(shardMap.size());
					for (auto& [keyNew, valueNew]: shardMap) {
						if (valueNew.areWeConnected()) {
							processIOMapNew.emplace(keyNew, &valueNew.tcpConnection);
						}
					}
					tcp_connection<websocket_tcpconnection>::processIO(processIOMapNew);
					processIOMapNew.clear();
					std::this_thread::sleep_for(1ms);
				}
				connect(value);
				discord_core_client::getInstance()->currentlyConnectingShard.fetch_add(1, std::memory_order_release);
			}
			processIOMapNew.clear();
			while (!token.stop_requested() && !doWeQuit->load(std::memory_order_acquire)) {
				try {
					for (auto& [key, value]: shardMap) {
						if (value.areWeConnected()) {
							processIOMapNew.emplace(key, &value.tcpConnection);
						}
					}
					tcp_connection<websocket_tcpconnection>::processIO(processIOMapNew);
					processIOMapNew.clear();
					bool areWeConnected{};
					for (auto& [key, value]: shardMap) {
						if (value.areWeConnected()) {
							if (value.checkForAndSendHeartBeat()) {
								on_gateway_ping_data dataNew{};
								dataNew.timeUntilNextPing = static_cast<int32_t>(value.heartBeatStopWatch.getTotalWaitTime().count());
								discord_core_client::getInstance()->eventManager.onGatewayPingEvent(dataNew);
							}
							areWeConnected = true;
						} else {
							message_printer::printError<print_message_type::websocket>("Connection lost for websocket [" + jsonifier::toString(value.shard.at(0)) + "," +
								jsonifier::toString(discord_core_client::getInstance()->configManager.getTotalShardCount()) + "]... reconnecting.");
							std::this_thread::sleep_for(1s);
							if (discord_core_client::getInstance()->connectionStopWatch01.hasTimeElapsed()) {
								connect(value);
							}
						}
					}
					if (!areWeConnected) {
						std::this_thread::sleep_for(1ms);
					}
				} catch (const dca_exception& error) {
					message_printer::printError<print_message_type::websocket>(error.what());
				}
			}
		}

		base_socket_agent::~base_socket_agent(){}
	}// namespace discord_core_internal
}// namespace discord_core_api
