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

#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/ErlPacker.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/SSLClients.hpp>
#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreInternal {

	constexpr uint16_t webSocketMaxPayloadLengthLarge{ 65535u };
	constexpr uint8_t webSocketPayloadLengthMagicLarge{ 126u };
	constexpr uint8_t webSocketPayloadLengthMagicHuge{ 127u };
	constexpr uint8_t maxHeaderSize{ sizeof(uint64_t) + 2u };
	constexpr uint8_t webSocketMaxPayloadLengthSmall{ 125u };
	constexpr uint8_t webSocketFinishBit{ (1u << 7u) };
	constexpr uint8_t webSocketMaskBit{ (1u << 7u) };

	class DiscordCoreAPI_Dll WebSocketMessageHandler {
	  public:

		WebSocketMessageHandler(DiscordCoreAPI::ConfigManager* configManager);

		void stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, DiscordCoreInternal::WebSocketOpCode theOpCode) noexcept;

		void createHeader(std::string& outBuffer, uint64_t sendLength, DiscordCoreInternal::WebSocketOpCode opCode) noexcept;

		bool parseConnectionHeaders(DiscordCoreInternal::WebSocketSSLShard* theShard) noexcept;

		bool parseMessage(DiscordCoreInternal::WebSocketSSLShard* theShard) noexcept;

		virtual void onMessageReceived(WebSocketSSLShard* theShard) noexcept = 0;

		virtual ~WebSocketMessageHandler() = default;

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{};
		ErlPacker erlPacker{};
	};

	class DiscordCoreAPI_Dll BaseSocketAgent : public WebSocketMessageHandler {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::BotUser;
		friend class WebSocketSSLShard;

		BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocketAgentNew) noexcept;

		void sendMessage(std::string& dataToSend, WebSocketSSLShard* theIndex, bool priority) noexcept;

		void connectVoiceChannel(VoiceConnectInitData theData) noexcept;

		void connect(DiscordCoreAPI::ConnectionPackage) noexcept;

		void onClosed(WebSocketSSLShard* theShard) noexcept;

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
		std::atomic_bool* doWeQuit{ nullptr };
		const int32_t maxReconnectTries{ 10 };
		int32_t currentBaseSocketAgent{ 0 };
		int32_t heartbeatInterval{ 0 };
		std::mutex theMutex{};

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect, WebSocketSSLShard* theIndex) noexcept;

		void checkForAndSendHeartBeat(WebSocketSSLShard* theIndex, bool = false) noexcept;

		void onMessageReceived(WebSocketSSLShard* theShard) noexcept;

		void connectVoiceInternal() noexcept;

		void run(std::stop_token) noexcept;

		void disconnectVoice() noexcept;

		void connectInternal() noexcept;
	};

}// namespace DiscordCoreInternal
