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
/// WebSocketClient.hpp - Header for the webSocket related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file WebSocketClient.hpp

#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/Utilities/LightString.hpp>
#include <discordcoreapi/Utilities/Etf.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		enum class WebSocketOpCode : uint8_t {
			Op_Continuation = 0x00,
			Op_Text = 0x01,
			Op_Binary = 0x02,
			Op_Close = 0x08,
			Op_Ping = 0x09,
			Op_Pong = 0x0a
		};

		/// @brief Websocket close codes.
		class WebSocketClose {
		  public:
			/// @brief Websocket close codes.
			enum class WebSocketCloseCode : uint16_t {
				Unset = 1 << 0,///< Unset.
				Normal_Close = 1 << 1,///< Normal close.
				Unknown_Error = 1 << 2,///< We're not sure what went wrong. Try reconnecting?
				Unknown_Opcode = 1 << 3,///< You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
				Decode_Error = 1 << 4,///< You sent an invalid payload to us. Don't do that!
				Not_Authenticated = 1 << 5,///< You sent us a payload prior to identifying.
				Authentication_Failed = 1 << 6,///< The account token sent with your identify payload is incorrect.
				Already_Authenticated = 1 << 7,///< You sent more than one identify payload. Don't do that!
				Invalid_Seq = 1 << 8,///<	The sequence sent when resuming the session was invalid. Reconnect and start a new session.
				Rate_Limited =
					1 << 9,///< Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
				Session_Timed = 1 << 10,///< Your session timed out. Reconnect and start a new one.
				Invalid_Shard = 1 << 11,///< You sent us an invalid shard when identifying.
				Sharding_Required =
					1 << 12,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.
				Invalid_API_Version = 1 << 13,///< You sent an invalid version for the gateway.
				Invalid_Intent = 1 << 14,///< You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
				Disallowed_Intent = 1
					<< 15,///< You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
				We_Do_Reconnect = Normal_Close | Unknown_Error | Unknown_Opcode | Decode_Error | Not_Authenticated | Already_Authenticated |
					Invalid_Seq | Rate_Limited | Session_Timed,
				We_Do_Not_Reconnect =
					Authentication_Failed | Invalid_Shard | Sharding_Required | Invalid_API_Version | Invalid_Intent | Disallowed_Intent
			};

			inline static UnorderedMap<int32_t, WebSocketCloseCode> mappingValues{ { 0, WebSocketCloseCode::Unset },
				{ 1000, WebSocketCloseCode::Normal_Close }, { 4000, WebSocketCloseCode::Unknown_Error }, { 4001, WebSocketCloseCode::Unknown_Opcode },
				{ 4002, WebSocketCloseCode::Decode_Error }, { 4003, WebSocketCloseCode::Not_Authenticated },
				{ 4004, WebSocketCloseCode::Authentication_Failed }, { 4005, WebSocketCloseCode::Already_Authenticated },
				{ 4007, WebSocketCloseCode::Invalid_Seq }, { 4008, WebSocketCloseCode::Rate_Limited }, { 4009, WebSocketCloseCode::Session_Timed },
				{ 4010, WebSocketCloseCode::Invalid_Shard }, { 4011, WebSocketCloseCode::Sharding_Required },
				{ 4012, WebSocketCloseCode::Invalid_API_Version }, { 4013, WebSocketCloseCode::Invalid_Intent },
				{ 4014, WebSocketCloseCode::Disallowed_Intent } };

			inline static UnorderedMap<WebSocketCloseCode, std::string_view> outputErrorValues{ {
																									WebSocketCloseCode::Unknown_Error,
																									"We're not sure what went wrong.",
																								},
				{ WebSocketCloseCode::Unknown_Opcode, "You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!" },
				{ WebSocketCloseCode::Decode_Error, "You sent an invalid payload to Discord. Don't do that!" },
				{ WebSocketCloseCode::Not_Authenticated, "You sent us a payload prior to identifying." },
				{ WebSocketCloseCode::Authentication_Failed, "The account token sent with your identify payload is incorrect." },
				{ WebSocketCloseCode::Already_Authenticated, "You sent more than one identify payload. Don't do that!" },
				{ WebSocketCloseCode::Invalid_Seq, "The sequence sent when resuming the session was invalid. Reconnect and start a new session." },
				{ WebSocketCloseCode::Rate_Limited,
					"Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this." },
				{ WebSocketCloseCode::Session_Timed, "Your session timed out. Reconnect and start a new one." },
				{ WebSocketCloseCode::Invalid_Shard, "You sent us an invalid shard when identifying." },
				{ WebSocketCloseCode::Sharding_Required,
					"The session would have handled too many guilds - you are required to shard your connection in order to connect." },
				{ WebSocketCloseCode::Invalid_API_Version, "You sent an invalid version for the gateway." },
				{ WebSocketCloseCode::Invalid_Intent,
					"You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value." },
				{ WebSocketCloseCode::Disallowed_Intent,
					"You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not "
					"enabled or are "
					"not "
					"approved for." } };

			WebSocketCloseCode value{};

			inline WebSocketClose& operator=(uint16_t valueNew) {
				value = static_cast<WebSocketCloseCode>(valueNew);
				return *this;
			};

			inline WebSocketClose(uint16_t valueNew) {
				*this = valueNew;
			};

			inline operator std::string_view() {
				return WebSocketClose::outputErrorValues[mappingValues[static_cast<uint16_t>(value)]];
			}

			inline operator bool() {
				return static_cast<std::underlying_type_t<decltype(value)>>(value) &
					static_cast<std::underlying_type_t<decltype(value)>>(WebSocketCloseCode::We_Do_Reconnect);
			}
		};

		class DiscordCoreAPI_Dll EventConverter {
		  public:
			EventConverter(std::string eventNew);

			operator uint64_t();

		  protected:
			std::string eventValue{};
		};

		/// @brief For the opcodes that could be sent/received via Discord's websockets.
		enum class WebSocketOpCodes {
			Dispatch = 0,///< An event was dispatched.
			Heartbeat = 1,///< Fired periodically by the client to keep the connection alive.
			Identify = 2,///< Starts a new session during the initial handshake.
			Presence_Update = 3,///< Update the client's presence.
			Voice_State_Update = 4,///< Used to join/leave or move between voice channels.
			Resume = 6,///< Resume a previous session that was disconnected.
			Reconnect = 7,///< You should attempt to reconnect and resume immediately.
			Request_Guild_Members = 8,///< Request information about offline guild members in a large guild.
			Invalid_Session = 9,/// The session has been invalidated. You should reconnect and identify/resume accordingly.
			Hello = 10,///< Sent immediately after connecting, contains the heartbeat_interval to use.
			Heartbeat_ACK = 11,///<Sent in response to receiving a heartbeat to acknowledge that it has been received.
		};

		class WebSocketCore;

		class DiscordCoreAPI_Dll WebSocketTCPConnection : public TCPConnection<WebSocketTCPConnection>,
														  public SSLDataInterface<WebSocketTCPConnection> {
		  public:
			friend class WebSocketCore;

			inline WebSocketTCPConnection() = default;

			inline WebSocketTCPConnection& operator=(WebSocketTCPConnection&& other) = default;
			inline WebSocketTCPConnection(WebSocketTCPConnection&& other) = default;

			WebSocketTCPConnection(const std::string& baseUrlNew, uint16_t portNew, WebSocketCore* ptrNew);

			void handleBuffer();

		  protected:
			std::vector<char> resampleBuffer{};
			WebSocketCore* ptr{};
		};

		enum class WebSocketType { Normal = 0, Voice = 1 };

		enum class WebSocketState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

		class DiscordCoreAPI_Dll WebSocketCore : public EtfParser {
		  public:
			friend class DiscordCoreAPI::VoiceConnection;
			friend class WebSocketTCPConnection;

			inline WebSocketCore() = default;

			WebSocketCore& operator=(WebSocketCore&&) noexcept;
			WebSocketCore(WebSocketCore&&) noexcept;

			WebSocketCore(ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew);

			bool connect(const std::string& baseUrlNew, const std::string& relativePath, const uint16_t portNew);

			void createHeader(std::string& outBuffer, WebSocketOpCode opCode);

			virtual bool onMessageReceived(std::string_view message) = 0;

			bool sendMessage(std::string& dataToSend, bool priority);

			bool checkForAndSendHeartBeat(bool = false);

			void parseConnectionHeaders();

			virtual void onClosed() = 0;

			bool areWeConnected();

			bool parseMessage();

			void disconnect();

			virtual ~WebSocketCore() = default;

		  protected:
			StopWatch<Milliseconds> heartBeatStopWatch{ 20000ms };
			std::atomic<WebSocketState> currentState{};
			bool haveWeReceivedHeartbeatAck{ true };
			std::atomic_bool areWeCollectingData{};
			WebSocketTCPConnection tcpConnection{};
			LightString<char> currentMessage{};
			uint32_t maxReconnectTries{ 10 };
			uint32_t currentReconnectTries{};
			std::array<uint32_t, 2> shard{};
			ConfigManager* configManager{};
			uint32_t lastNumberReceived{};
			WebSocketOpCode dataOpCode{};
			bool areWeHeartBeating{};
			WebSocketType wsType{};
			bool areWeResuming{};
		};

		class WebSocketClient : public WebSocketCore {
		  public:
			friend struct DiscordCoreAPI::OnVoiceServerUpdateData;
			friend struct DiscordCoreAPI::OnVoiceStateUpdateData;
			friend class TCPConnection<WebSocketTCPConnection>;
			friend class DiscordCoreAPI::DiscordCoreClient;
			friend class DiscordCoreAPI::VoiceConnection;
			friend class DiscordCoreAPI::BotUser;
			friend class BaseSocketAgent;
			friend class SoundCloudAPI;
			friend class WebSocketCore;
			friend class YouTubeAPI;

			inline WebSocketClient() = default;

			inline WebSocketClient& operator=(WebSocketClient&&) = default;
			inline WebSocketClient(WebSocketClient&&) = default;

			WebSocketClient(DiscordCoreClient* client, uint32_t currentShardNew, std::atomic_bool* doWeQuitNew);

			void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect);

			bool onMessageReceived(std::string_view message);

			void disconnect();

			void onClosed();

			virtual ~WebSocketClient();

		  protected:
			UnorderedMap<uint64_t, UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
			VoiceConnectionData voiceConnectionData{};
			DiscordCoreClient* discordCoreClient{};
			std::atomic_bool* doWeQuit{};
			bool serverUpdateCollected{};
			bool stateUpdateCollected{};
			std::string resumeUrl{};
			std::string sessionId{};
			Snowflake userId{};
		};

		class BaseSocketAgent {
		  public:
			friend class DiscordCoreAPI::DiscordCoreClient;
			friend class DiscordCoreAPI::BotUser;

			BaseSocketAgent(DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, uint64_t currentBaseSocket);

			bool waitForState(ConnectionPackage& packageNew, WebSocketState state);

			void connect(ConnectionPackage);

			WebSocketClient& getClient(uint32_t index);

			~BaseSocketAgent();

		  protected:
			UnorderedMap<uint64_t, WebSocketClient> shardMap{};
			DiscordCoreClient* discordCoreClient{};
			UniquePtr<std::jthread> taskThread{};
			uint64_t currentBaseSocketAgent{};
			std::atomic_bool* doWeQuit{};

			void run(std::stop_token);
		};

	}// namespace
}
