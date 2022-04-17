/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

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

	enum class WebSocketState : uint8_t { Initializing = 0, Connected = 1 };

	enum class WebSocketOpCode : uint8_t { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend VoiceSocketAgent;

		BaseSocketAgent(std::string botToken, std::string baseUrl, DiscordCoreAPI::EventManager* eventManager,
			DiscordCoreAPI::DiscordCoreClient* discordCoreClient, DiscordCoreAPI::CommandController* commandController, std::atomic_bool* theBool,
			bool doWePrintMessages = false, int32_t shardNumber = 0, int32_t numberOfShards = 1) noexcept;

		BaseSocketAgent(nullptr_t) noexcept;

		std::jthread* getTheTask();

		void sendMessage(nlohmann::json& dataToSend) noexcept;

		void sendMessage(std::string& dataToSend) noexcept;

		void onClosedExternal() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<std::string, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		const DiscordCoreAPI::GatewayIntents intentsValue{ DiscordCoreAPI::GatewayIntents::All_Intents };
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		DiscordCoreAPI::CommandController* commandController{ nullptr };
		WebSocketOpCode dataOpcode{ WebSocketOpCode::Op_Binary };
		std::unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		DiscordCoreAPI::EventWaiter areWeReadyToConnectEvent{};
		std::unique_ptr<std::jthread> theTask{ nullptr };
		DiscordCoreAPI::EventManager* eventManager{};
		DiscordCoreAPI::EventWaiter doWeReconnect{};
		VoiceConnectionData voiceConnectionData{};
		std::atomic_bool areWeConnected{ false };
		bool haveWeReceivedHeartbeatAck{ true };
		std::atomic_bool* doWeQuit{ nullptr };
		const int32_t maxReconnectTries{ 10 };
		std::binary_semaphore semaphore{ 1 };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		int32_t currentReconnectTries{ 0 };
		bool areWeCollectingData{ false };
		bool areWeAuthenticated{ false };
		bool areWeHeartBeating{ false };
		int32_t lastNumberReceived{ 0 };
		int32_t heartbeatInterval{ 0 };
		std::mutex accessorMutex01{};
		std::string sessionId{ "" };
		bool areWeResuming{ false };
		bool printMessages{ false };
		std::string botToken{ "" };
		std::string baseUrl{ "" };
		int32_t currentShard{ 0 };
		int32_t numOfShards{ 0 };
		std::string userId{ "" };
		uint32_t closeCode{ 0 };
		WebSocketState state{};
		ErlPacker erlPacker{};

		uint64_t createHeader(char* outbuf, uint64_t sendlength, WebSocketOpCode opCode) noexcept;

		std::vector<std::string> tokenize(std::string&, std::string = "\r\n") noexcept;

		void getVoiceConnectionData(VoiceConnectInitData doWeCollect) noexcept;

		void run(std::stop_token) noexcept;

		void onMessageReceived() noexcept;

		void onClosedInternal() noexcept;

		void sendHeartBeat() noexcept;

		void handleBuffer() noexcept;

		bool parseHeader() noexcept;

		void connect() noexcept;
	};

	class DiscordCoreAPI_Dll VoiceSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, bool doWePrintMessages = false) noexcept;

		void sendMessage(std::vector<uint8_t>& responseData) noexcept;

		void sendVoiceData(std::string& responseData) noexcept;

		void sendMessage(std::string& dataToSend) noexcept;

		void onClosedExternal() noexcept;

		~VoiceSocketAgent() noexcept;

	  protected:
		DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::unique_ptr<DatagramSocketSSLClient> voiceSocket{ nullptr };
		std::unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		WebSocketOpCode dataOpcode{ WebSocketOpCode::Op_Text };
		WebSocketState state{ WebSocketState::Initializing };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		DiscordCoreAPI::EventWaiter areWeConnected{};
		DiscordCoreAPI::EventWaiter doWeReconnect{};
		VoiceConnectInitData voiceConnectInitData{};
		BaseSocketAgent* baseSocketAgent{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		bool haveWeReceivedHeartbeatAck{ true };
		int32_t currentReconnectionTries{ 0 };
		int32_t maxReconnectionTries{ 10 };
		int32_t lastNumberReceived{ 0 };
		bool areWeHeartBeating{ false };
		int32_t heartbeatInterval{ 0 };
		bool printMessages{ false };
		std::string baseUrl{ "" };
		std::string hostIp{ "" };
		uint32_t closeCode{ 0 };

		uint64_t createHeader(char* outbuf, uint64_t sendlength, WebSocketOpCode opCode) noexcept;

		std::vector<std::string> tokenize(std::string&, std::string = "\r\n") noexcept;

		void run(std::stop_token) noexcept;

		void collectExternalIP() noexcept;

		void onMessageReceived() noexcept;

		void onClosedInternal() noexcept;

		void sendHeartBeat() noexcept;

		void voiceConnect() noexcept;

		void handleBuffer() noexcept;

		bool parseHeader() noexcept;

		void connect() noexcept;
	};
}// namespace DiscordCoreInternal
