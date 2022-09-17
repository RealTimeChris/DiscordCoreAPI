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

#ifndef WEBSOCKET_ENTITIES
#define WEBSOCKET_ENTITIES

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/ErlPacker.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/SSLClients.hpp>
#include <discordcoreapi/ThreadPool.hpp>
#include <simdjson.h>

namespace DiscordCoreInternal {

	class EventConverter {
	  public:
		EventConverter(std::string theEventNew);

		operator int32_t();

	  protected:
		std::string theEvent{};
	};

	class DiscordCoreAPI_Dll WebSocketMessageHandler : public ErlPacker {
	  public:
		WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManager);

		void stringifyJsonData(DiscordCoreAPI::JsonSerializer& dataToSend, std::string& theString, WebSocketOpCode theOpCode) noexcept;

		void createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept;

		bool parseConnectionHeaders(WebSocketSSLShard* theShard) noexcept;

		virtual bool onMessageReceived(std::string_view theMessage) noexcept = 0;

		virtual bool handleBuffer(WebSocketSSLClient* theClient) noexcept = 0;

		bool parseMessage(WebSocketSSLShard* theShard) noexcept;

		virtual void onClosed() noexcept = 0;

		virtual ~WebSocketMessageHandler() noexcept = default;

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{};
		uint64_t messageLength{};
		uint64_t messageOffset{};
	};

	class DiscordCoreAPI_Dll WebSocketSSLShard : public WebSocketSSLClient, public WebSocketMessageHandler {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class WebSocketMessageHandler;
		friend class DiscordCoreAPI::BotUser;
		friend class BaseSocketAgent;
		friend class YouTubeAPI;
		friend class SSLClient;

		WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* theClient, std::deque<DiscordCoreAPI::ConnectionPackage>* theConnections, int32_t currentShardNew,
			std::atomic_bool* doWeQuitNew) noexcept;

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept;

		virtual bool onMessageReceived(std::string_view theMessage) noexcept;

		bool sendMessage(std::string& dataToSend, bool priority) noexcept;

		bool handleBuffer(WebSocketSSLClient* theClient) noexcept;

		void checkForAndSendHeartBeat(bool = false) noexcept;

		void disconnect(bool doWeReconnect) noexcept;

		void onClosed() noexcept;

		~WebSocketSSLShard() noexcept = default;

	  protected:
		std::unordered_map<Snowflake, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> heartBeatStopWatch{ 20000ms };
		std::deque<DiscordCoreAPI::ConnectionPackage>* theConnections{ nullptr };
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		std::atomic_bool areWeConnecting{ true };
		bool haveWeReceivedHeartbeatAck{ true };
		const uint32_t maxReconnectTries{ 10 };
		simdjson::ondemand::parser theParser{};
		std::atomic_bool* doWeQuit{ nullptr };
		bool serverUpdateCollected{ false };
		uint32_t currentReconnectTries{ 0 };
		bool stateUpdateCollected{ false };
		bool areWeCollectingData{ false };
		uint32_t lastNumberReceived{ 0 };
		bool areWeHeartBeating{ false };
		WebSocketClose closeCode{ 0 };
		WebSocketOpCode dataOpCode{};
		bool areWeResuming{ false };
		std::string resumeUrl{};
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

		BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocket) noexcept;

		void connectVoiceChannel(VoiceConnectInitData theData) noexcept;

		void connect(DiscordCoreAPI::ConnectionPackage) noexcept;

		std::jthread* getTheTask() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<uint32_t, std::unique_ptr<WebSocketSSLShard>> theShardMap{};
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theVCStopWatch{ 250ms };
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		std::deque<Snowflake> voiceConnectionsToDisconnect{};
		std::unique_ptr<std::jthread> taskThread{ nullptr };
		std::deque<VoiceConnectInitData> voiceConnections{};
		DiscordCoreAPI::ConfigManager* configManager{};
		std::atomic_bool* doWeQuit{ nullptr };
		uint32_t currentBaseSocketAgent{};
		int32_t heartbeatInterval{ 0 };
		std::mutex theMutex{};

		void disconnectVoiceInternal() noexcept;

		void disconnectVoice(uint64_t) noexcept;

		void connectVoiceInternal() noexcept;

		void run(std::stop_token) noexcept;
	};

}// namespace DiscordCoreInternal
#endif