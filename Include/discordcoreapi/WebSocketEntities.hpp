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
	#include <discordcoreapi/ErlPacker.hpp>
	#include <discordcoreapi/EventEntities.hpp>
	#include <discordcoreapi/SSLClients.hpp>
	#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll EventConverter {
	  public:
		EventConverter(String theEventNew);

		operator Int32();

	  protected:
		String theEvent{};
	};

	enum class SSLShardState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

	class DiscordCoreAPI_Dll WebSocketMessageHandler : public ErlPacker, public SSLClient {
	  public:
		WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManager, std::deque<DiscordCoreAPI::ConnectionPackage>* theConnections, String typeOfWebSocket);

		String stringifyJsonData(DiscordCoreAPI::JsonObject&& dataToSend, WebSocketOpCode theOpCode) noexcept;

		void createHeader(String& outBuffer, Uint64 sendLength, WebSocketOpCode opCode) noexcept;

		void sendMessage(String& dataToSend, Bool priority) noexcept;

		void checkForAndSendHeartBeat(Bool = false) noexcept;

		void parseConnectionHeaders() noexcept;

		void handleBuffer() noexcept;

		Bool parseMessage() noexcept;

		virtual Bool onMessageReceived(StringView theMessage) noexcept = 0;

		virtual void disconnect() noexcept = 0;

		virtual void onClosed() noexcept = 0;

		virtual ~WebSocketMessageHandler() noexcept = default;

	  protected:
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> heartBeatStopWatch{ 20000ms };
		std::deque<DiscordCoreAPI::ConnectionPackage>* theConnections{ nullptr };
		DiscordCoreAPI::ConfigManager* configManager{};
		std::atomic<SSLShardState> currentState{};
		Bool haveWeReceivedHeartbeatAck{ true };
		AtomicBool areWeConnecting{ true };
		Bool areWeHeartBeating{ false };
		Uint32 lastNumberReceived{ 0 };
		WebSocketClose closeCode{ 0 };
		StringBuffer currentMessage{};
		WebSocketOpCode dataOpCode{};
		Bool areWeResuming{ false };
		String typeOfWebSocket{};
		Uint64 messageLength{};
		Uint64 messageOffset{};
		Uint32 shard[2]{};
	};

	class DiscordCoreAPI_Dll WebSocketSSLShard : public WebSocketMessageHandler {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class WebSocketMessageHandler;
		friend class DiscordCoreAPI::BotUser;
		friend class BaseSocketAgent;
		friend class YouTubeAPI;
		friend class SSLClient;

		WebSocketSSLShard(DiscordCoreAPI::DiscordCoreClient* theClient, std::deque<DiscordCoreAPI::ConnectionPackage>* theConnections, Int32 currentShardNew,
			AtomicBool* doWeQuitNew);

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept;

		Bool onMessageReceived(StringView theMessage) noexcept;

		void disconnect() noexcept;

		void onClosed() noexcept;

		virtual ~WebSocketSSLShard() noexcept = default;

	  protected:
		std::unordered_map<Uint64, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		simdjson::ondemand::parser theParser{};
		DiscordCoreAPI::Snowflake userId{ 0 };
		const Uint32 maxReconnectTries{ 10 };
		Bool serverUpdateCollected{ false };
		Bool stateUpdateCollected{ false };
		Bool areWeCollectingData{ false };
		Uint32 currentReconnectTries{ 0 };
		AtomicBool* doWeQuit{ nullptr };
		String resumeUrl{};
		String sessionId{};
	};

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::BotUser;
		friend class WebSocketSSLShard;

		BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, AtomicBool* doWeQuitNew, Int32 currentBaseSocket) noexcept;

		void connectVoiceChannel(VoiceConnectInitData theData) noexcept;

		void connect(DiscordCoreAPI::ConnectionPackage) noexcept;

		std::jthread* getTheTask() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<Uint32, std::unique_ptr<WebSocketSSLShard>> theShardMap{};
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theVCStopWatch{ 250ms };
		std::deque<DiscordCoreAPI::Snowflake> voiceConnectionsToDisconnect{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		std::unique_ptr<std::jthread> taskThread{ nullptr };
		std::deque<VoiceConnectInitData> voiceConnections{};
		DiscordCoreAPI::ConfigManager* configManager{};
		AtomicBool* doWeQuit{ nullptr };
		Uint32 currentBaseSocketAgent{};
		Int32 heartbeatInterval{ 0 };
		std::mutex theMutex{};

		void disconnectVoiceInternal() noexcept;

		void disconnectVoice(Uint64) noexcept;

		void connectVoiceInternal() noexcept;

		void run(std::stop_token) noexcept;
	};

}// namespace DiscordCoreInternal
#endif