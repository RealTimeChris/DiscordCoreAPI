/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// WebSocketEntities.hpp - Header for the webSocket related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file WebSocketEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/ErlPacker.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/SSLClients.hpp>
#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll WebSocketMessageHandler : public ErlPacker {
	  public:
		WebSocketMessageHandler() = default;

		WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManager);

		void stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, DiscordCoreInternal::WebSocketOpCode theOpCode) noexcept;

		void createHeader(std::string& outBuffer, uint64_t sendLength, DiscordCoreInternal::WebSocketOpCode opCode) noexcept;

		bool parseConnectionHeaders(DiscordCoreInternal::WebSocketSSLShard* theShard) noexcept;

		bool parseMessage(DiscordCoreInternal::WebSocketSSLShard* theShard) noexcept;

		virtual void onMessageReceived() noexcept = 0;

		virtual ~WebSocketMessageHandler() = default;

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{};
	};

	enum class WebSocketSSLShardState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

	class DiscordCoreAPI_Dll WebSocketSSLShard : public SSLClient, public WebSocketMessageHandler {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend class WebSocketMessageHandler;
		friend class DiscordCoreAPI::BotUser;
		friend class BaseSocketAgent;
		friend class YouTubeAPI;

		WebSocketSSLShard() = default;

		WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* theClient, std::queue<DiscordCoreAPI::ConnectionPackage>* connectionsNew, int32_t currentBaseSocketAgentNew,
			int32_t currentShardNew, std::atomic_bool* doWeQuitNew) noexcept;

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept;

		bool sendMessage(std::string& dataToSend, bool priority) noexcept;

		void checkForAndSendHeartBeat(bool = false) noexcept;

		void disconnect(bool doWeReconnect) noexcept;

		void onMessageReceived() noexcept;

		void onClosed() noexcept;

		~WebSocketSSLShard() noexcept = default;

	  protected:
		std::unordered_map<Snowflake, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		std::atomic<WebSocketSSLShardState> theWebSocketState{ WebSocketSSLShardState ::Connecting };
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> heartBeatStopWatch{ 0ms };
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		std::queue<std::string> processedMessages{};
		VoiceConnectionData voiceConnectionData{};
		bool haveWeReceivedHeartbeatAck{ true };
		const int32_t maxReconnectTries{ 10 };
		std::atomic_bool* doWeQuit{ nullptr };
		bool serverUpdateCollected{ false };
		int32_t currentBaseSocketAgent{ 0 };
		int32_t currentReconnectTries{ 0 };
		bool stateUpdateCollected{ false };
		bool areWeCollectingData{ false };
		std::recursive_mutex theMutex{};
		bool areWeHeartBeating{ false };
		int32_t lastNumberReceived{ 0 };
		WebSocketCloseCode closeCode{};
		WebSocketOpCode dataOpCode{};
		bool areWeResuming{ false };
		int64_t messageLength{};
		int64_t messageOffset{};
		std::string sessionId{};
		nlohmann::json shard{};
		Snowflake userId{ 0 };
	};

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::BotUser;
		friend class WebSocketSSLShard;

		BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocketAgentNew) noexcept;

		void connectVoiceChannel(VoiceConnectInitData theData) noexcept;

		void connect(DiscordCoreAPI::ConnectionPackage) noexcept;

		std::jthread* getTheTask() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theVCStopWatch{ 550ms };
		std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>> sslShards{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		std::queue<DiscordCoreAPI::ConnectionPackage> connections{};
		std::unique_ptr<std::jthread> taskThread{ nullptr };
		std::queue<VoiceConnectInitData> voiceConnections{};
		std::queue<uint64_t> voiceConnectionsToDisconnect{};
		DiscordCoreAPI::ConfigManager* configManager{};
		std::atomic_bool* doWeQuit{ nullptr };
		const int32_t maxReconnectTries{ 10 };
		int32_t currentBaseSocketAgent{ 0 };
		int32_t heartbeatInterval{ 0 };
		std::mutex theMutex{};

		void connectVoiceInternal() noexcept;

		void run(std::stop_token&) noexcept;

		void disconnectVoice() noexcept;

		void connectInternal() noexcept;
	};

}// namespace DiscordCoreInternal
