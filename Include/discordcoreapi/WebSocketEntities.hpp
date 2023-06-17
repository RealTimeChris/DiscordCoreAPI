/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// WebSocketEntities.hpp - Header for the webSocket related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file WebSocketEntities.hpp

#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/TCPConnection.hpp>
#include <discordcoreapi/Utilities/ThreadPool.hpp>
#include <discordcoreapi/Utilities/Etf.hpp>

namespace DiscordCoreInternal {

	class EventConverter {
	  public:
		EventConverter(std::string eventNew);

		operator int32_t();

	  protected:
		std::string eventValue{};
	};

	/// \brief For the opcodes that could be sent/received via Discord's websockets.
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

	class WebSocketTCPConnection : public TCPConnection<WebSocketTCPConnection>, public SSLDataInterface<WebSocketTCPConnection> {
	  public:
		friend class WebSocketCore;

		WebSocketTCPConnection() noexcept = default;

		WebSocketTCPConnection(const std::string& baseUrlNew, uint16_t portNew, WebSocketCore* ptrNew);

		void handleBuffer() noexcept;

		void disconnect() noexcept;

	  protected:
		WebSocketCore* ptr{};
	};

	enum class WebSocketType { Normal = 0, Voice = 1 };

	enum class WebSocketState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

	class WebSocketCore : public EtfParser {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend class WebSocketTCPConnection;

		WebSocketCore() noexcept = default;

		WebSocketCore& operator=(WebSocketCore&& other) noexcept;
		WebSocketCore(WebSocketCore&& other) noexcept;

		WebSocketCore(DiscordCoreAPI::ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew);

		bool connect(const std::string& baseUrlNew, const std::string& relativePath, const uint16_t portNew) noexcept;

		void createHeader(std::string& outBuffer, WebSocketOpCode opCode) noexcept;

		virtual bool onMessageReceived(std::string_view message) noexcept = 0;

		bool sendMessage(std::string& dataToSend, bool priority) noexcept;

		void parseConnectionHeaders(std::string_view stringNew) noexcept;

		bool checkForAndSendHeartBeat(bool = false) noexcept;

		virtual void onClosed() noexcept = 0;

		bool parseMessage() noexcept;

		void disconnect() noexcept;

		virtual ~WebSocketCore() noexcept = default;

	  protected:
		DiscordCoreAPI::StopWatch<Milliseconds> heartBeatStopWatch{ 20000ms };
		DiscordCoreAPI::ConfigManager* configManager{};
		std::atomic<WebSocketState> currentState{};
		DiscordCoreAPI::String currentMessage{};
		bool haveWeReceivedHeartbeatAck{ true };
		WebSocketTCPConnection tcpConnection{};
		uint32_t maxReconnectTries{ 10 };
		uint32_t currentReconnectTries{};
		uint32_t lastNumberReceived{};
		WebSocketOpCode dataOpCode{};
		bool areWeHeartBeating{};
		WebSocketType wsType{};
		size_t currentIndex{};
		bool areWeResuming{};
		uint32_t shard[2]{};
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

		WebSocketClient() noexcept = default;

		WebSocketClient& operator=(WebSocketClient&& other) noexcept = default;
		WebSocketClient(WebSocketClient&& other) noexcept = default;

		WebSocketClient(DiscordCoreAPI::DiscordCoreClient* client, int32_t currentShardNew, std::atomic_bool* doWeQuitNew);

		void getVoiceConnectionData(const DiscordCoreAPI::VoiceConnectInitData& doWeCollect) noexcept;

		bool onMessageReceived(std::string_view message) noexcept;

		void disconnect() noexcept;

		void onClosed() noexcept;

		virtual ~WebSocketClient() noexcept;

	  protected:
		std::unordered_map<uint64_t, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{};
		VoiceConnectionData voiceConnectionData{};
		DiscordCoreAPI::Snowflake userId{};
		std::atomic_bool* doWeQuit{};
		bool serverUpdateCollected{};
		bool stateUpdateCollected{};
		bool areWeCollectingData{};
		std::string resumeUrl{};
		std::string sessionId{};
	};

	class BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::BotUser;

		BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocket) noexcept;

		bool waitForState(DiscordCoreAPI::ConnectionPackage& packageNew, WebSocketState state) noexcept;

		void connect(DiscordCoreAPI::ConnectionPackage) noexcept;

		WebSocketClient& getClient(uint32_t index) noexcept;

		std::jthread* getTheTask() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<uint32_t, WebSocketClient> shardMap{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{};
		DiscordCoreAPI::UniquePtr<std::jthread> taskThread{};
		uint32_t currentBaseSocketAgent{};
		std::atomic_bool* doWeQuit{};

		void run(std::stop_token) noexcept;
	};

}// namespace
