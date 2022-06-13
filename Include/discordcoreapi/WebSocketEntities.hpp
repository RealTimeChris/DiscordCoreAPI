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

	enum class ReconnectPossible : uint16_t {
		Yes = static_cast<uint16_t>(WebSocketCloseCode::Unknown_Error) | static_cast<uint16_t>(WebSocketCloseCode::Unknown_Opcode) |
			static_cast<uint16_t>(WebSocketCloseCode::Decode_Error) | static_cast<uint16_t>(WebSocketCloseCode::Not_Authenticated) |
			static_cast<uint16_t>(WebSocketCloseCode::Already_Authenticated) | static_cast<uint16_t>(WebSocketCloseCode::Invalid_Seq) |
			static_cast<uint16_t>(WebSocketCloseCode::Session_Timed),
		No = static_cast<uint16_t>(WebSocketCloseCode::Authentication_Failed) | static_cast<uint16_t>(WebSocketCloseCode::Invalid_Shard) |
			static_cast<uint16_t>(WebSocketCloseCode::Sharding_Required) | static_cast<uint16_t>(WebSocketCloseCode::Invalid_API_Version) |
			static_cast<uint16_t>(WebSocketCloseCode::Disallowed_Intent) | static_cast<uint16_t>(WebSocketCloseCode::Invalid_Intent) |
			static_cast<uint16_t>(WebSocketCloseCode::Normal_Close)
	};

	class DiscordCoreAPI_Dll WSMessageCollector {
	  public:
		friend class BaseSocketAgent;

		WSMessageCollector(BaseSocketAgent*, bool doWePrintErrorMessagesNew, std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>>* theClientsNew,
			WebSocketOpCode theOpCode);

		WSMessageCollector() = default;

		std::unordered_map<int32_t, WSMessageCollectorReturnData> collectFinalMessage() noexcept;

		bool runMessageCollector() noexcept;

	  protected:
		std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>>* theClients{};
		BaseSocketAgent* theAgent{ nullptr };
		bool doWePrintErrorMessages{ false };
		int8_t maxRecursionDepth{ 10 };
		int8_t currentRecursionDepth{};
		WebSocketOpCode theOpCode{};
		
		bool parseConnectionHeader() noexcept;

		bool parseHeaderAndMessage() noexcept;

		bool collectData() noexcept;
	};

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::BotUser;
		friend class WSMessageCollector;
		friend VoiceSocketAgent;

		BaseSocketAgent(const std::string& botTokenNew, const std::string& baseUrl, DiscordCoreAPI::EventManager* eventManager,
			DiscordCoreAPI::DiscordCoreClient* discordCoreClient, DiscordCoreAPI::CommandController* commandController, std::atomic_bool* doWeQuitNew,
			bool doWePrintSuccessMessages, bool doWePrintErrorMessages, int32_t currentBaseSocketAgent) noexcept;

		void sendMessage(const nlohmann::json& dataToSend, int32_t theIndex) noexcept;

		void sendMessage(std::string& dataToSend, int32_t theIndex) noexcept;

		void connect(DiscordCoreAPI::ReconnectionPackage) noexcept;

		void onClosed(int32_t theIndex) noexcept;

		std::jthread* getTheTask() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<std::string, DiscordCoreAPI::TSUnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		const DiscordCoreAPI::GatewayIntents intentsValue{ DiscordCoreAPI::GatewayIntents::All_Intents };
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theReconnectionTimer{ 5000ms };
		std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>> theClients{};
		DiscordCoreAPI::TextFormat theFormat{ DiscordCoreAPI::TextFormat::Etf };
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		DiscordCoreAPI::CommandController* commandController{ nullptr };
		std::queue<DiscordCoreAPI::ReconnectionPackage> connections{};
		WebSocketOpCode dataOpcode{ WebSocketOpCode::Op_Binary };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		DiscordCoreAPI::EventManager* eventManager{};
		VoiceConnectionData voiceConnectionData{};
		EventWaiter areWeReadyToConnectEvent{};
		bool doWePrintSuccessMessages{ false };
		std::recursive_mutex accessorMutex01{};
		std::recursive_mutex accessorMutex02{};
		WSMessageCollector messageCollector{};
		std::atomic_bool* doWeQuit{ nullptr };
		const int32_t maxReconnectTries{ 10 };
		bool doWePrintErrorMessages{ false };
		std::binary_semaphore semaphore{ 1 };
		int32_t currentBaseSocketAgent{ 0 };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		bool areWeCollectingData{ false };
		WebSocketCloseCode closeCode{};
		int32_t heartbeatInterval{ 0 };
		bool didWeFail{ false };
		std::string botToken{};
		ErlPacker erlPacker{};
		std::string baseUrl{};
		uint64_t userId{};

		void createHeader(std::string& outbuf, uint64_t sendlength, WebSocketOpCode opCode, int32_t theIndex) noexcept;

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect, int32_t theIndex) noexcept;

		void onMessageReceived(std::string theMessage, int32_t theIndex) noexcept;

		void sendCloseFrame(int32_t theIndex) noexcept;

		void sendHeartBeat(int32_t theIndex) noexcept;

		void run(std::stop_token) noexcept;

		void internalConnect() noexcept;
	};

	class DiscordCoreAPI_Dll VoiceSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, int32_t theIndex, bool doWePrintMessages = false) noexcept;

		void sendMessage(const std::vector<uint8_t>& responseData) noexcept;

		void sendMessage(std::string& dataToSend) noexcept;

		void sendVoiceData(std::string& responseData) noexcept;

		void onClosedExternal() noexcept;

		~VoiceSocketAgent() noexcept;

	  protected:
		DiscordCoreAPI::TSUnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>> theClients{};
		std::unique_ptr<DatagramSocketSSLClient> voiceSocket{ nullptr };
		WebSocketOpCode dataOpcode{ WebSocketOpCode::Op_Text };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		VoiceConnectInitData voiceConnectInitData{};
		BaseSocketAgent* baseSocketAgent{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		std::atomic_bool doWeReconnect{ false };
		bool haveWeReceivedHeartbeatAck{ true };
		bool doWePrintSuccessMessages{ false };
		WSMessageCollector messageCollector{};
		int32_t currentReconnectionTries{ 0 };
		bool doWePrintErrorMessages{ false };
		int32_t maxReconnectionTries{ 10 };
		int32_t lastNumberReceived{ 0 };
		bool areWeHeartBeating{ false };
		int32_t heartbeatInterval{ 0 };
		WebSocketCloseCode closeCode{};
		EventWaiter areWeConnected{};
		std::string baseUrl{};
		std::string hostIp{};

		void createHeader(std::string& outbuf, uint64_t sendlength, WebSocketOpCode opCode) noexcept;

		void onMessageReceived(const std::string& theMessage) noexcept;

		void run(std::stop_token) noexcept;

		void collectExternalIP() noexcept;

		void onClosedInternal() noexcept;

		void sendHeartBeat() noexcept;

		void voiceConnect() noexcept;

		void connect() noexcept;
	};
}// namespace DiscordCoreInternal
