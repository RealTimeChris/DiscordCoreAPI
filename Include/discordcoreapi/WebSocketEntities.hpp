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

	class DiscordCoreAPI_Dll WebSocketMessageHandler : public ErlPacker {
	  public:
		WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManager);

		void createHeader(String& outBuffer, Uint64 sendLength, WebSocketOpCode opCode) noexcept;

		String stringifyJsonData(DiscordCoreAPI::JsonObject&& dataToSend, WebSocketOpCode theOpCode) noexcept;

		virtual Bool onMessageReceived(StringView theMessage) noexcept = 0;

		void parseConnectionHeaders(WebSocketSSLShard* theShard) noexcept;

		void parseMessage(WebSocketSSLShard* theShard) noexcept;

		virtual void onClosed() noexcept = 0;

		virtual ~WebSocketMessageHandler() noexcept = default;

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{};
		Uint64 messageLength{};
		Uint64 messageOffset{};
	};

	enum class SSLShardState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

	class DiscordCoreAPI_Dll WebSocketSSLShard : public SSLClient, public WebSocketMessageHandler {
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

		virtual Bool onMessageReceived(StringView theMessage) noexcept;

		Bool sendMessage(String& dataToSend, Bool priority) noexcept;

		void checkForAndSendHeartBeat(Bool = false) noexcept;

		virtual void handleBuffer() noexcept;

		void disconnect() noexcept;

		void onClosed() noexcept;

		virtual ~WebSocketSSLShard() noexcept = default;

	  protected:
		std::unordered_map<Uint64, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> heartBeatStopWatch{ 20000ms };
		std::deque<DiscordCoreAPI::ConnectionPackage>* theConnections{ nullptr };
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		std::atomic<SSLShardState> currentState{};
		AtomicBool areWeConnecting{ true };
		Bool haveWeReceivedHeartbeatAck{ true };
		const Uint32 maxReconnectTries{ 10 };
		simdjson::ondemand::parser theParser{};
		AtomicBool* doWeQuit{ nullptr };
		DiscordCoreAPI::Snowflake userId{ 0 };
		Bool serverUpdateCollected{ false };
		Uint32 currentReconnectTries{ 0 };
		Bool stateUpdateCollected{ false };
		Bool areWeCollectingData{ false };
		Uint32 lastNumberReceived{ 0 };
		Bool areWeHeartBeating{ false };
		WebSocketClose closeCode{ 0 };
		StringBuffer currentMessage{};
		WebSocketOpCode dataOpCode{};
		Bool areWeResuming{ false };
		String resumeUrl{};
		String sessionId{};
		Uint32 shard[2]{};
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