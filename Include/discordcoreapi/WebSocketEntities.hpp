/*
*
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

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class WebSocketSSLShard;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::BotUser;
		friend class WSMessageCollector;
		friend VoiceSocketAgent;

		BaseSocketAgent(DiscordCoreAPI::DiscordCoreClient* discordCoreClientNew, std::atomic_bool* doWeQuitNew, int32_t currentBaseSocketAgentNew) noexcept;

		void sendMessage(const nlohmann::json& dataToSend, WebSocketSSLShard* theIndex, bool priority) noexcept;

		void sendMessage(std::string& dataToSend, WebSocketSSLShard* theIndex, bool priority) noexcept;

		void connect(DiscordCoreAPI::ConnectionPackage) noexcept;

		void onClosed(WebSocketSSLShard* theShard) noexcept;

		std::jthread* getTheTask() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>> theClients{};
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		std::queue<DiscordCoreAPI::ConnectionPackage> connections{};
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		std::atomic_bool* doWeQuit{ nullptr };
		const int32_t maxReconnectTries{ 10 };
		int32_t currentBaseSocketAgent{ 0 };
		int32_t heartbeatInterval{ 0 };
		ErlPacker erlPacker{};

		void stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, WebSocketOpCode theOpCode) noexcept;

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect, WebSocketSSLShard* theIndex) noexcept;

		void createHeader(std::string& outBuffer, uint64_t sendLength, WebSocketOpCode opCode) noexcept;

		void checkForAndSendHeartBeat(WebSocketSSLShard* theIndex, bool = false) noexcept;

		void parseHeadersAndMessage(WebSocketSSLShard* theShard) noexcept;

		void onMessageReceived(WebSocketSSLShard* theIndex) noexcept;

		void run(std::stop_token) noexcept;

		void internalConnect() noexcept;
	};

	class DiscordCoreAPI_Dll VoiceSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, WebSocketSSLShard* theIndex, DiscordCoreAPI::ConfigManager* configManagerNew,
			std::atomic_bool* doWeQuit) noexcept;

		void sendMessage(const nlohmann::json& responseData) noexcept;

		void sendVoiceData(std::string& responseData) noexcept;

		void sendMessage(std::string& dataToSend) noexcept;

		void onClosed() noexcept;

		~VoiceSocketAgent() noexcept;

	  protected:
		DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData> voiceConnectionDataBuffer{};
		std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>> theClients{};
		std::unique_ptr<DatagramSocketSSLClient> voiceSocket{ nullptr };
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		VoiceConnectInitData voiceConnectInitData{};
		WebSocketSSLShard* theBaseShard{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		std::atomic_bool areWeConnected{ false };
		std::atomic_bool doWeDisconnect{ false };
		std::atomic_bool doWeReconnect{ false };
		std::atomic_bool* doWeQuit{ nullptr };
		int32_t maxReconnectionTries{ 10 };
		int32_t heartbeatInterval{ 0 };
		std::string baseUrl{};
		std::string hostIp{};

		void stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, WebSocketOpCode theOpCode) noexcept;

		void createHeader(std::string& outbuf, uint64_t sendlength, WebSocketOpCode opCode) noexcept;

		void parseHeadersAndMessage(WebSocketSSLShard* theShard) noexcept;

		void onMessageReceived(const std::string& theMessage) noexcept;

		void run(std::stop_token) noexcept;

		void collectExternalIP() noexcept;

		void sendHeartBeat() noexcept;

		void voiceConnect() noexcept;

		void connect() noexcept;
	};
}// namespace DiscordCoreInternal
