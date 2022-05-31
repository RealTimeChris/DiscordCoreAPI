/*
*
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

	enum class WebSocketState : uint8_t { Initializing = 0, Connected = 1 };

	enum class WebSocketOpCode : uint8_t { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

	/// Websocket close codes. \brief Websocket close codes.
	enum class WebSocketCloseCode : int16_t {
		Normal_Close = 1000,///< Normal close.
		Unknown_Error = 4000,///<	We're not sure what went wrong. Try reconnecting?	true
		Unknown_Opcode = 4001,///< You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!	true
		Decode_Error = 4002,///< You sent an invalid payload to us. Don't do that!	true
		Not_Authenticated = 4003,///< You sent us a payload prior to identifying.	true
		Authentication_Failed = 4004,///< The account token sent with your identify payload is incorrect.	false
		Already_Authenticated = 4005,///< You sent more than one identify payload. Don't do that!	true
		Invalid_Seq = 4007,///<	The sequence sent when resuming the session was invalid. Reconnect and start a new session.	true
		Rate_Limited = 4008,///< Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.	true
		Session_Timed = 4009,///< Your session timed out. Reconnect and start a new one.	true
		Invalid_Shard = 4010,///< You sent us an invalid shard when identifying.	false
		Sharding_Required = 4011,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.	false
		Invalid_API_Version = 4012,///< You sent an invalid version for the gateway.	false
		Invalid_Intent = 4013,///< You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.	false
		Disallowed_Intent =
			4014,///< You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.	false
	};

	enum class ReconnectPossible : int16_t {
		Yes = static_cast<int16_t>(WebSocketCloseCode::Unknown_Error) | static_cast<int16_t>(WebSocketCloseCode::Unknown_Opcode) |
			static_cast<int16_t>(WebSocketCloseCode::Decode_Error) | static_cast<int16_t>(WebSocketCloseCode::Not_Authenticated) |
			static_cast<int16_t>(WebSocketCloseCode::Already_Authenticated) | static_cast<int16_t>(WebSocketCloseCode::Invalid_Seq) |
			static_cast<int16_t>(WebSocketCloseCode::Session_Timed),
		No = static_cast<int16_t>(WebSocketCloseCode::Authentication_Failed) | static_cast<int16_t>(WebSocketCloseCode::Invalid_Shard) |
			static_cast<int16_t>(WebSocketCloseCode::Sharding_Required) | static_cast<int16_t>(WebSocketCloseCode::Invalid_API_Version) |
			static_cast<int16_t>(WebSocketCloseCode::Disallowed_Intent) | static_cast<int16_t>(WebSocketCloseCode::Invalid_Intent) |
			static_cast<int16_t>(WebSocketCloseCode::Normal_Close)
	};

	class DiscordCoreAPI_Dll BaseSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend VoiceSocketAgent;

		BaseSocketAgent(const std::string& botTokenNew, const std::string& baseUrl, DiscordCoreAPI::EventManager* eventManager,
			DiscordCoreAPI::DiscordCoreClient* discordCoreClient, DiscordCoreAPI::CommandController* commandController, std::atomic_bool* doWeQuitNew,
			bool doWePrintSuccessMessages, bool doWePrintErrorMessages, int32_t shardNumber, int32_t numberOfShards) noexcept;

		BaseSocketAgent(nullptr_t) noexcept;

		void sendMessage(const nlohmann::json& dataToSend) noexcept;

		void sendMessage(const std::string& dataToSend) noexcept;

		std::jthread* getTheTask() noexcept;

		void onClosedExternal() noexcept;

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
		std::atomic_bool* doWeQuit{ nullptr };
		const int32_t maxReconnectTries{ 10 };
		std::binary_semaphore semaphore{ 1 };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		int32_t currentReconnectTries{ 0 };
		bool printSuccessMessages{ false };
		bool areWeCollectingData{ false };
		uint32_t lastNumberReceived{ 0 };
		bool areWeAuthenticated{ false };
		bool printErrorMessages{ false };
		bool areWeHeartBeating{ false };
		int32_t heartbeatInterval{ 0 };
		std::mutex accessorMutex01{};
		bool areWeResuming{ false };
		std::string sessionId{};
		uint32_t closeCode{ 0 };
		nlohmann::json shard{};
		WebSocketState state{};
		std::string botToken{};
		ErlPacker erlPacker{};
		std::string baseUrl{};
		int32_t offset{ 0 };
		uint64_t userId{};

		std::vector<std::string> tokenize(const std::string&, const std::string& = "\r\n") noexcept;

		uint64_t createHeader(char* outbuf, uint64_t sendlength, WebSocketOpCode opCode) noexcept;

		void getVoiceConnectionData(const VoiceConnectInitData& doWeCollect) noexcept;

		void run(std::stop_token) noexcept;

		void onMessageReceived() noexcept;

		void sendCloseFrame() noexcept;

		void sendHeartBeat() noexcept;

		void handleBuffer() noexcept;

		bool parseHeader() noexcept;

		void connect() noexcept;
	};

	class DiscordCoreAPI_Dll VoiceSocketAgent {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew, bool doWePrintMessages = false) noexcept;

		void sendMessage(const std::vector<uint8_t>& responseData) noexcept;

		void sendVoiceData(std::string& responseData) noexcept;

		void sendMessage(const std::string& dataToSend) noexcept;

		void onClosedExternal() noexcept;

		~VoiceSocketAgent() noexcept;

	  protected:
		DiscordCoreAPI::TSUnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::unique_ptr<DatagramSocketSSLClient> voiceSocket{ nullptr };
		std::unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		WebSocketOpCode dataOpcode{ WebSocketOpCode::Op_Text };
		WebSocketState state{ WebSocketState::Initializing };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		VoiceConnectInitData voiceConnectInitData{};
		BaseSocketAgent* baseSocketAgent{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		std::atomic_bool doWeReconnect{ false };
		bool haveWeReceivedHeartbeatAck{ true };
		int32_t currentReconnectionTries{ 0 };
		int32_t maxReconnectionTries{ 10 };
		bool printSuccessMessages{ false };
		bool printErrorMessages{ false };
		int32_t lastNumberReceived{ 0 };
		bool areWeHeartBeating{ false };
		int32_t heartbeatInterval{ 0 };
		EventWaiter areWeConnected{};
		uint32_t closeCode{ 0 };
		std::string baseUrl{};
		std::string hostIp{};

		uint64_t createHeader(char* outbuf, uint64_t sendlength, WebSocketOpCode opCode) noexcept;

		std::vector<std::string> tokenize(const std::string&, const std::string& = "\r\n") noexcept;

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
