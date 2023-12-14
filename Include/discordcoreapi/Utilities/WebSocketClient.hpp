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
/// WebsocketClient.hpp - Header for the webSocket related classes and
/// structs. may 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file WebsocketClient.hpp
#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/Utilities/Etf.hpp>
#include <thread>

namespace discord_core_api {

	namespace discord_core_internal {

		constexpr uint16_t webSocketMaxPayloadLengthLarge{ 65535u };
		constexpr uint8_t webSocketPayloadLengthMagicLarge{ 126u };
		constexpr uint8_t webSocketPayloadLengthMagicHuge{ 127u };
		constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2u };
		constexpr uint8_t webSocketMaxPayloadLengthSmall{ 125u };
		constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

		enum class websocket_op_code : uint8_t { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

		/// @brief Websocket close codes.
		class websocket_close {
		  public:
			/// @brief Websocket close codes.
			enum class websocket_close_code : uint16_t {
				Unset				  = 0,///< Unset.
				Normal_Close		  = 1000,///< Normal close.
				Unknown_Error		  = 4000,///< We're not sure what went wrong. try reconnecting?
				Unknown_Opcode		  = 4001,///< You sent an invalid gateway opcode or an invalid payload for an opcode. don't do that!
				Decode_Error		  = 4002,///< You sent an invalid payload to us. don't do that!
				Not_Authenticated	  = 4003,///< You sent us a payload prior to identifying.
				Authentication_Failed = 4004,///< The account token sent with your identify payload is incorrect.
				Already_Authenticated = 4005,///< You sent more than one identify payload. don't do that!
				Invalid_Seq			  = 4007,///<	the sequence sent when resuming the session was invalid. reconnect and start a new session.
				Rate_Limited		  = 4008,///< Woah nelly! you're sending payloads to us too quickly. slow it down! you will be disconnected on receiving this.
				Session_Timed		  = 4009,///< Your session timed out. reconnect and start a new one.
				Invalid_Shard		  = 4010,///< You sent us an invalid shard when identifying.
				Sharding_Required	  = 4011,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.
				Invalid_API_Version	  = 4012,///< You sent an invalid version for the gateway.
				Invalid_Intent		  = 4013,///< You sent an invalid intent for a gateway intent. you may have incorrectly calculated the bitwise value.
				Disallowed_Intent	  = 4014,///< You sent a disallowed intent for a gateway intent. you may have tried to specify an intent that you have not enabled.
			};

			DCA_INLINE static unordered_map<websocket_close_code, jsonifier::string> outputErrorValues{ {
																										websocket_close_code::Unknown_Error,
																										"we're not sure what went wrong.",
																									},
				{ websocket_close_code::Unknown_Opcode, "you sent an invalid gateway opcode or an invalid payload for an opcode. don't do that!" },
				{ websocket_close_code::Decode_Error, "you sent an invalid payload to discord. don't do that!" },
				{ websocket_close_code::Not_Authenticated, "you sent us a payload prior to identifying." },
				{ websocket_close_code::Authentication_Failed, "The account token sent with your identify payload is incorrect." },
				{ websocket_close_code::Already_Authenticated, "you sent more than one identify payload. don't do that!" },
				{ websocket_close_code::Invalid_Seq, "The sequence sent when resuming the session was invalid. reconnect and start a new session." },
				{ websocket_close_code::Rate_Limited, "woah nelly! you're sending payloads to us too quickly. slow it down! you will be disconnected on receiving this." },
				{ websocket_close_code::Session_Timed, "your session timed out. reconnect and start a new one." },
				{ websocket_close_code::Invalid_Shard, "you sent us an invalid shard when identifying." },
				{ websocket_close_code::Sharding_Required, "The session would have handled too many guilds - you are required to shard your connection in order to connect." },
				{ websocket_close_code::Invalid_API_Version, "you sent an invalid version for the gateway." },
				{ websocket_close_code::Invalid_Intent, "you sent an invalid intent for a gateway intent. you may have incorrectly calculated the bitwise value." },
				{ websocket_close_code::Disallowed_Intent,
					"you sent a disallowed intent for a gateway intent. you may have tried to specify an intent that you have not enabled or are not "
					"approved for." } };

			websocket_close_code value{};

			DCA_INLINE websocket_close& operator=(uint16_t valueNew) {
				value = static_cast<websocket_close_code>(valueNew);
				return *this;
			};

			DCA_INLINE websocket_close(uint16_t valueNew) {
				*this = valueNew;
			};

			DCA_INLINE operator jsonifier::string_view() {
				return websocket_close::outputErrorValues[static_cast<websocket_close_code>(value)];
			}
		};

		class DiscordCoreAPI_Dll event_converter {
		  public:
			event_converter(const jsonifier::string& eventNew);

			operator uint64_t();

		  protected:
			jsonifier::string_view eventValue{};
		};

		/// @brief For the opcodes that could be sent/received via discord's websockets.
		enum class websocket_op_codes {
			dispatch			  = 0,///< An event was dispatched.
			heartbeat			  = 1,///< Fired periodically by the client to keep the connection alive.
			identify			  = 2,///< Starts a new session during the initial handshake.
			Presence_Update		  = 3,///< Update the client's presence.
			Voice_State_Update	  = 4,///< Used to join/leave or move between voice channels.
			resume				  = 6,///< Resume a previous session that was disconnected.
			reconnect			  = 7,///< You should attempt to reconnect and resume immediately.
			Request_Guild_Members = 8,///< Request information about offline guild members in a large guild.
			Invalid_Session		  = 9,/// the session has been invalidated. you should reconnect and identify/resume accordingly.
			hello				  = 10,///< Sent immediately after connecting, contains the heartbeat_interval to use.
			Heartbeat_ACK		  = 11,///<sent in response to receiving a heartbeat to acknowledge that it has been received.
		};

		class websocket_core;

		class DiscordCoreAPI_Dll websocket_tcpconnection : public tcp_connection<websocket_tcpconnection> {
		  public:
			friend class websocket_core;

			DCA_INLINE websocket_tcpconnection() = default;

			DCA_INLINE websocket_tcpconnection& operator=(websocket_tcpconnection&& other) = default;
			DCA_INLINE websocket_tcpconnection(websocket_tcpconnection&& other)			   = default;

			websocket_tcpconnection(const jsonifier::string& baseUrlNew, uint16_t portNew, websocket_core* ptrNew);

			void handleBuffer() override;

		  protected:
			websocket_core* ptr{};
		};

		enum class websocket_type { normal = 0, voice = 1 };

		enum class websocket_state { connecting = 0, upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, authenticated = 4, disconnected = 5 };

		class DiscordCoreAPI_Dll websocket_core {
		  public:
			friend class discord_core_api::voice_connection;
			friend class websocket_tcpconnection;

			DCA_INLINE websocket_core() = default;

			websocket_core& operator=(websocket_core&& data) noexcept;
			websocket_core(websocket_core&& data) noexcept;

			websocket_core(config_manager* configManagerNew, websocket_type typeOfWebSocketNew);

			template<typename value_type> void createHeader(jsonifier::string_base<value_type>& outBuffer, websocket_op_code opCode) {
				int64_t originalSize{ static_cast<int64_t>(outBuffer.size()) };
				outBuffer.insert(outBuffer.begin(), static_cast<value_type>(static_cast<uint8_t>(opCode) | webSocketMaskBit));

				int64_t indexCount{};
				if (originalSize <= webSocketMaxPayloadLengthSmall) {
					outBuffer.insert(outBuffer.begin() + 1, static_cast<value_type>(originalSize));
					indexCount = 0;
				} else if (originalSize <= webSocketMaxPayloadLengthLarge) {
					outBuffer.insert(outBuffer.begin() + 1, static_cast<value_type>(webSocketPayloadLengthMagicLarge));
					indexCount = 2;
				} else {
					outBuffer.insert(outBuffer.begin() + 1, static_cast<value_type>(webSocketPayloadLengthMagicHuge));
					indexCount = 8;
				}
				for (int64_t x = indexCount - 1; x >= 0; x--) {
					outBuffer.insert(outBuffer.begin() + 1 + indexCount - x, static_cast<value_type>(originalSize >> (x * 8)));
				}

				outBuffer.at(1) |= webSocketMaskBit;
				outBuffer.insert(outBuffer.begin() + 2 + indexCount, 0);
				outBuffer.insert(outBuffer.begin() + 3 + indexCount, 0);
				outBuffer.insert(outBuffer.begin() + 4 + indexCount, 0);
				outBuffer.insert(outBuffer.begin() + 5 + indexCount, 0);
			}

			bool connect(const jsonifier::string& baseUrlNew, jsonifier::string_view relativePath, const uint16_t portNew);

			virtual bool onMessageReceived(jsonifier::string_view_base<uint8_t> message) = 0;

			bool sendMessage(jsonifier::string_base<uint8_t>& dataToSend, bool priority);

			bool checkForAndSendHeartBeat(bool = false);

			void parseConnectionHeaders();

			virtual void onClosed() = 0;

			bool areWeConnected();

			bool parseMessage();

			void disconnect();

			virtual ~websocket_core() = default;

		  protected:
			stop_watch<milliseconds> heartBeatStopWatch{ 20000ms };
			jsonifier::string_base<uint8_t> currentMessage{};
			std::atomic<websocket_state> currentState{};
			bool haveWeReceivedHeartbeatAck{ true };
			std::atomic_bool areWeCollectingData{};
			websocket_tcpconnection tcpConnection{};
			uint32_t maxReconnectTries{ 10 };
			uint32_t currentReconnectTries{};
			std::array<uint64_t, 2> shard{};
			config_manager* configManager{};
			uint32_t lastNumberReceived{};
			websocket_op_code dataOpCode{};
			std::mutex accessMutex{};
			bool areWeHeartBeating{};
			websocket_type wsType{};
			bool areWeResuming{};
		};

		/// @brief A websocket client, for communication via a tcp-connection.
		class websocket_client : public websocket_core {
		  public:
			friend struct discord_core_api::on_voice_server_update_data;
			friend struct discord_core_api::on_voice_state_update_data;
			friend class tcp_connection<websocket_tcpconnection>;
			friend class discord_core_api::discord_core_client;
			friend class discord_core_api::voice_connection;
			friend class discord_core_api::bot_user;
			friend class base_socket_agent;
			friend class sound_cloud_api;
			friend class websocket_core;
			friend class you_tube_api;

			DCA_INLINE websocket_client() = default;

			DCA_INLINE websocket_client& operator=(websocket_client&&) = default;
			DCA_INLINE websocket_client(websocket_client&&)			   = default;

			websocket_client(uint64_t currentShardNew, std::atomic_bool* doWeQuitNew);

			void getVoiceConnectionData(const voice_connect_init_data& doWeCollect);

			bool onMessageReceived(jsonifier::string_view_base<uint8_t> message) override;

			void disconnect();

			void onClosed() override;

			virtual ~websocket_client();

		  protected:
			unordered_map<uint64_t, unbounded_message_block<voice_connection_data>*> voiceConnectionDataBufferMap{};
			voice_connection_data voiceConnectionData{};
			jsonifier::string resumeUrl{};
			jsonifier::string sessionId{};
			std::atomic_bool* doWeQuit{};
			bool serverUpdateCollected{};
			bool stateUpdateCollected{};
			snowflake userId{};
		};

		class base_socket_agent {
		  public:
			friend class discord_core_api::discord_core_client;
			friend class discord_core_api::bot_user;

			base_socket_agent(std::atomic_bool* doWeQuitNew);

			void connect(websocket_client& value);

			~base_socket_agent();

		  protected:
			unordered_map<uint64_t, websocket_client> shardMap{};
			std::deque<connection_package> connections{};
			std::atomic_bool* doWeQuit{};
			std::jthread taskThread{};

			void run(std::stop_token);
		};

	}// namespace
}
