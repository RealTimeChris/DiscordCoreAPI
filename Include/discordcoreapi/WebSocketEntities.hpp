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
/// WebSocketEntities.hpp - Header for the webSocket related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file WebSocketEntities.hpp

#pragma once

#ifndef WEBSOCKET_ENTITIES
	#define WEBSOCKET_ENTITIES

	#include <discordcoreapi/FoundationEntities.hpp>
	#include <discordcoreapi/AudioDecoder.hpp>
	#include <discordcoreapi/CommandController.hpp>
	#include <discordcoreapi/ErlParser.hpp>
	#include <discordcoreapi/EventEntities.hpp>
	#include <discordcoreapi/SSLClients.hpp>
	#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll EventConverter {
	  public:
		EventConverter(std::string eventNew);

		operator int32_t();

	  protected:
		std::string eventValue{};
	};

	enum class WebSocketType { Normal = 0, Voice = 1 };

	enum class WebSocketState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

	class DiscordCoreAPI_Dll WebSocketCore : public ErlParser, public SSLClient {
	  public:
		WebSocketCore(DiscordCoreAPI::ConfigManager* configManagerNew, WebSocketType typeOfWebSocketNew);

		bool connect(const std::string& baseUrl, const std::string& relativePath, const std::string& portNew, bool doWePrintErrorsNew,
			bool areWeAStandaloneSocketNew) noexcept;

		void createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept;

		std::string prepMessageData(std::string&& dataToSend, WebSocketOpCode theOpCode) noexcept;

		virtual bool onMessageReceived(std::string_view message) noexcept = 0;

		bool sendMessage(std::string& dataToSend, bool priority) noexcept;

		void parseConnectionHeaders(std::string_view stringNew) noexcept;

		bool checkForAndSendHeartBeat(bool = false) noexcept;

		virtual void onClosed() noexcept = 0;

		void handleBuffer() noexcept;

		void parseMessage() noexcept;

		virtual ~WebSocketCore() noexcept = default;

	  protected:
		DiscordCoreAPI::StopWatch<Milliseconds> heartBeatStopWatch{ 20000ms };
		std::unique_ptr<DiscordCoreAPI::ConnectionPackage> connections{ nullptr };
		DiscordCoreAPI::ConfigManager* configManager{};
		std::atomic<WebSocketState> currentState{};
		std::atomic_bool areWeConnecting{ true };
		bool haveWeReceivedHeartbeatAck{ true };
		const uint32_t maxReconnectTries{ 10 };
		uint32_t currentReconnectTries{ 0 };
		uint32_t lastNumberReceived{ 0 };
		WebSocketType typeOfWebSocket{};
		bool areWeHeartBeating{ false };
		WebSocketClose closeCode{ 0 };
		StringBuffer currentMessage{};
		WebSocketOpCode dataOpCode{};
		bool areWeResuming{ false };
		uint64_t messageLength{};
		uint64_t messageOffset{};
		uint32_t shard[2]{};
	};

	class DiscordCoreAPI_Dll WebSocketSSLShard : public WebSocketCore {
	  public:
		friend class DiscordCoreAPI::OnVoiceServerUpdateData;
		friend class DiscordCoreAPI::OnVoiceStateUpdateData;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::BotUser;
		friend class BaseSocketAgent;
		friend class WebSocketCore;
		friend class YouTubeAPI;
		friend class SSLClient;

		WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* client, int32_t currentShardNew, std::atomic_bool* doWeQuitNew);

		void getVoiceConnectionData(const DiscordCoreAPI::VoiceConnectInitData& doWeCollect) noexcept;

		bool onMessageReceived(std::string_view message) noexcept;

		void disconnect() noexcept;

		void onClosed() noexcept;

		virtual ~WebSocketSSLShard() noexcept = default;

	  protected:
		std::unordered_map<uint64_t, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBuffersMap{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		std::atomic_bool* doWeQuit{ nullptr };
		DiscordCoreAPI::Snowflake userId{ 0 };
		simdjson::ondemand::parser parser{};
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		bool areWeCollectingData{ false };
		std::string resumeUrl{};
		std::string sessionId{};
	};

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::BotUser;
		friend class WebSocketSSLShard;

		BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocket) noexcept;

		void connect(DiscordCoreAPI::ConnectionPackage) noexcept;

		std::jthread* getTheTask() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<uint32_t, std::unique_ptr<WebSocketSSLShard>> shardMap{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		std::unique_ptr<std::jthread> taskThread{ nullptr };
		DiscordCoreAPI::ConfigManager* configManager{};
		std::atomic_bool* doWeQuit{ nullptr };
		uint32_t currentBaseSocketAgent{};
		std::mutex accessMutex{};

		void run(std::stop_token) noexcept;
	};

}// namespace DiscordCoreInternal
#endif