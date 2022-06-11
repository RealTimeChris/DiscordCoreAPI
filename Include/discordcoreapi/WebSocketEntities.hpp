/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M.

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

	enum class WebSocketOpCode : uint8_t { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

	/// Websocket close codes. \brief Websocket close codes.
	enum class WebSocketCloseCode : uint16_t {
		Normal_Close = 1000,///< Normal close.
		Unknown_Error = 4000,///<	We're not sure what went wrong. Try reconnecting?
		Unknown_Opcode = 4001,///< You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
		Decode_Error = 4002,///< You sent an invalid payload to us. Don't do that!
		Not_Authenticated = 4003,///< You sent us a payload prior to identifying.
		Authentication_Failed = 4004,///< The account token sent with your identify payload is incorrect.
		Already_Authenticated = 4005,///< You sent more than one identify payload. Don't do that!
		Invalid_Seq = 4007,///<	The sequence sent when resuming the session was invalid. Reconnect and start a new session.
		Rate_Limited = 4008,///< Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
		Session_Timed = 4009,///< Your session timed out. Reconnect and start a new one.
		Invalid_Shard = 4010,///< You sent us an invalid shard when identifying.
		Sharding_Required = 4011,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.
		Invalid_API_Version = 4012,///< You sent an invalid version for the gateway.
		Invalid_Intent = 4013,///< You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
		Disallowed_Intent = 4014,///< You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
	};

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

	enum class WSMessageCollectorState { Connecting = 0, Initializing = 1, Collecting = 2, Parsing = 3, Serving = 4 };

	struct DiscordCoreAPI_Dll WSMessageCollectorReturnData {
		WebSocketCloseCode closeCode{};
		WebSocketOpCode opCode{};
		std::string theMessage{};
	};

	class DiscordCoreAPI_Dll WSMessageCollector {
	  public:
		WSMessageCollector(WebSocketSSLClient*, bool);

		WSMessageCollector() = default;

		WSMessageCollectorReturnData collectFinalMessage() noexcept;

		bool runMessageCollector() noexcept;

	  protected:
		WSMessageCollectorState theState{ WSMessageCollectorState::Connecting };
		std::queue<WSMessageCollectorReturnData> finalMessages{};
		WebSocketSSLClient* theClientPtr{ nullptr };
		bool doWePrintErrorMessages{ false };
		int8_t maxRecursionDepth{ 10 };
		int8_t currentRecursionDepth{};
		std::string currentMessage{};
		WebSocketOpCode dataOpCode{};
		int64_t messageLength{};
		int64_t messageOffset{};

		bool parseConnectionHeader() noexcept;

		bool parseHeaderAndMessage() noexcept;

		bool collectData() noexcept;
	};

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend VoiceSocketAgent;

		BaseSocketAgent(const std::string& botTokenNew, const std::string& baseUrl, DiscordCoreAPI::EventManager* eventManager,
			DiscordCoreAPI::DiscordCoreClient* discordCoreClient, DiscordCoreAPI::CommandController* commandController, std::atomic_bool* doWeQuitNew,
			bool doWePrintSuccessMessages, bool doWePrintErrorMessages, int32_t shardNumber, int32_t numberOfShards) noexcept;

		BaseSocketAgent(std::nullptr_t) noexcept;

		void sendMessage(const nlohmann::json& dataToSend) noexcept;

		void sendMessage(std::string& dataToSend) noexcept;

		std::jthread* getTheTask() noexcept;

		void onClosed() noexcept;

		~BaseSocketAgent() noexcept;

	  protected:
		std::unordered_map<std::string, DiscordCoreAPI::TSUnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		const DiscordCoreAPI::GatewayIntents intentsValue{ DiscordCoreAPI::GatewayIntents::All_Intents };
		DiscordCoreAPI::TextFormat theFormat{ DiscordCoreAPI::TextFormat::Etf };
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{ nullptr };
		DiscordCoreAPI::CommandController* commandController{ nullptr };
		WebSocketOpCode dataOpcode{ WebSocketOpCode::Op_Binary };
		std::unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		DiscordCoreAPI::EventManager* eventManager{};
		VoiceConnectionData voiceConnectionData{};
		std::atomic_bool areWeConnected{ false };
		bool haveWeReceivedHeartbeatAck{ true };
		EventWaiter areWeReadyToConnectEvent{};
		bool doWePrintSuccessMessages{ false };
		std::atomic_bool* doWeQuit{ nullptr };
		WSMessageCollector messageCollector{};
		const int32_t maxReconnectTries{ 10 };
		bool doWePrintErrorMessages{ false };
		std::binary_semaphore semaphore{ 1 };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		int32_t currentReconnectTries{ 0 };
		bool areWeCollectingData{ false };
		bool areWeAuthenticated{ false };
		bool areWeHeartBeating{ false };
		int32_t lastNumberReceived{ 0 };
		WebSocketCloseCode closeCode{};
		int32_t heartbeatInterval{ 0 };
		std::mutex accessorMutex01{};
		bool areWeResuming{ false };
		bool didWeFail{ false };
		std::string sessionId{};
		nlohmann::json shard{};
		std::string botToken{};
		ErlPacker erlPacker{};
		std::string baseUrl{};
		uint64_t userId{};

		void createHeader(std::string& outbuf, uint64_t sendlength, WebSocketOpCode opCode) noexcept;

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept;

		void onMessageReceived(std::string theMessage) noexcept;

		void run(std::stop_token) noexcept;

		void sendCloseFrame() noexcept;

		void sendHeartBeat() noexcept;

		void connect() noexcept;
	};

	class DiscordCoreAPI_Dll VoiceSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, bool doWePrintMessages = false) noexcept;

		void sendMessage(const std::vector<uint8_t>& responseData) noexcept;

		void sendMessage(std::string& dataToSend) noexcept;

		void sendVoiceData(std::string& responseData) noexcept;

		void onClosedExternal() noexcept;

		~VoiceSocketAgent() noexcept;

	  protected:
		DiscordCoreAPI::TSUnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::unique_ptr<DatagramSocketSSLClient> voiceSocket{ nullptr };
		std::unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
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
