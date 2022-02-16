// WebSocketEntities.hpp - Header for the webSocket related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include <AudioDecoder.hpp>
#include <FoundationEntities.hpp>
#include <ErlPacker.hpp>
#include <SSLClients.hpp>
#include <EventEntities.hpp>
#include <ThreadPool.hpp>

namespace DiscordCoreInternal {

	enum class WebSocketEventType {
		Unset = 0,
		Application_Command_Create = 1,
		Application_Command_Update = 2,
		Application_Command_Delete = 3,
		Channel_Create = 4,
		Channel_Update = 5,
		Channel_Delete = 6,
		Channel_Pins_Update = 7,
		Thread_Create = 8,
		Thread_Update = 9,
		Thread_Delete = 10,
		Thread_List_Sync = 11,
		Thread_Member_Update = 12,
		Thread_Members_Update = 13,
		Guild_Create = 14,
		Guild_Update = 15,
		Guild_Delete = 16,
		Guild_Ban_Add = 17,
		Guild_Ban_Remove = 18,
		Guild_Emojis_Update = 19,
		Guild_Stickers_Update = 20,
		Guild_Integrations_Update = 21,
		Guild_Member_Add = 22,
		Guild_Member_Remove = 23,
		Guild_Member_Update = 24,
		Guild_Members_Chunk = 25,
		Guild_Role_Create = 26,
		Guild_Role_Update = 27,
		Guild_Role_Delete = 28,
		Integration_Create = 29,
		Integration_Update = 30,
		Integration_Delete = 31,
		Interaction_Create = 32,
		Invite_Create = 33,
		Invite_Delete = 34,
		Message_Create = 35,
		Message_Update = 36,
		Message_Delete = 37,
		Message_Delete_Bulk = 38,
		Message_Reaction_Add = 39,
		Message_Reaction_Remove = 40,
		Message_Reaction_Remove_All = 41,
		Message_Reaction_Remove_Emoji = 42,
		Presence_Update = 43,
		Stage_Instance_Create = 44,
		Stage_Instance_Delete = 45,
		Stage_Instance_Update = 46,
		Typing_Start = 47,
		User_Update = 48,
		Voice_State_Update = 49,
		Voice_Server_Update = 50,
		Webhooks_Update = 51
	};

	enum class WebSocketState : uint8_t {
		Initializing = 0,
		Connected = 1
	};

	enum class WebSocketOpCode : uint8_t {
		Ws_Op_Continuation = 0x00,
		Op_Text = 0x01,
		Op_Binary = 0x02,
		Op_Close = 0x08,
		Op_Ping = 0x09,
		Op_Pong = 0x0a
	};

	struct DiscordCoreAPI_Dll WebSocketWorkload {
		WebSocketEventType eventType{ WebSocketEventType::Unset };
		nlohmann::json payLoad{};
		WebSocketWorkload() {}
		WebSocketWorkload& operator=(WebSocketWorkload&& other) noexcept {
			if (this != &other) {
				this->payLoad = std::move(other.payLoad);
				other.payLoad = nlohmann::json();
				this->eventType = other.eventType;
				other.eventType = WebSocketEventType::Unset;
			}
			return *this;
		}
		WebSocketWorkload(WebSocketWorkload&& other) noexcept {
			*this = std::move(other);
		}
		WebSocketWorkload& operator=(const WebSocketWorkload& other) noexcept {
			this->eventType = other.eventType;
			this->payLoad = other.payLoad;
			return *this;
		}
		WebSocketWorkload(const WebSocketWorkload& other) noexcept {
			*this = other;
		}
	};

	class DiscordCoreAPI_Dll BaseSocketAgent {
	public:

		friend class DiscordCoreAPI::VoiceConnection;
		friend VoiceSocketAgent;

		BaseSocketAgent(std::string botToken, std::string baseUrl, WebSocketOpCode opCode = WebSocketOpCode::Op_Binary);

		BaseSocketAgent(nullptr_t);

		DiscordCoreAPI::TSUnboundedMessageBlock<WebSocketWorkload>& getWorkloadTarget();

		void sendMessage(nlohmann::json& dataToSend);

		void sendMessage(std::string& dataToSend);

		void onClosedExternal();

		~BaseSocketAgent();

	protected:

		const int32_t intentsValue{ ((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 8) + (1 << 9) + (1 << 10) + (1 << 11) + (1 << 12) + (1 << 13) + (1 << 14)) };
		std::unordered_map<std::string, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		DiscordCoreAPI::TSUnboundedMessageBlock<WebSocketWorkload> webSocketWorkloadTarget{};
		DiscordCoreAPI::ThreadPoolTimer heartbeatTimer{ nullptr };
		std::unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		const uint64_t webSocketMaxPayloadLengthLarge{ 65535 };
		DiscordCoreAPI::EventWaiter areWeReadyToConnectEvent{};
		unsigned char webSocketPayloadLengthMagicLarge{ 126 };
		unsigned char webSocketPayloadLengthMagicHuge{ 127 };
		const uint64_t webSocketMaxPayloadLengthSmall{ 125 };
		const unsigned char webSocketFinishBit{ (1u << 7u) };
		const uint8_t maxHeaderSize{ sizeof(uint64_t) + 2 };
		const unsigned char webSocketMaskBit{ (1u << 7u) };
		DiscordCoreAPI::CoRoutine<void> theTask{ nullptr };
		std::string relativePath{ "/?v=9&encoding=etf" };
		DiscordCoreAPI::EventWaiter doWeReconnect{};
		VoiceConnectInitData voiceConnectInitData{};
		std::atomic<bool> areWeConnected{ false };
		VoiceConnectionData voiceConnectionData{};
		bool haveWeReceivedHeartbeatAck{ true };
		std::recursive_mutex accessorMutex01{};
		const int32_t maxReconnectTries{ 10 };
		bool serverUpdateCollected{ false };
		std::atomic<bool> doWeQuit{ false };
		std::vector<uint8_t> inputBuffer{};
		bool stateUpdateCollected{ false };
		int32_t currentReconnectTries{ 0 };
		bool areWeCollectingData{ false };
		bool areWeAuthenticated{ false };
		int32_t lastNumberReceived{ 0 };
		int32_t heartbeatInterval{ 0 };
		WebSocketOpCode dataOpcode{};
		std::string sessionId{ "" };
		bool areWeResuming{ false };
		std::string botToken{ "" };
		std::string baseUrl{ "" };
		std::string authKey{ "" };
		std::string port{ "443" };
		uint32_t closeCode{ 0 };
		WebSocketState state{};
		ErlPacker erlPacker{};

		uint64_t createHeader(char* outbuf, uint64_t sendlength, WebSocketOpCode opCode);

		std::vector<std::string> tokenize(std::string&, std::string = "\r\n");

		void getVoiceConnectionData(VoiceConnectInitData doWeCollect);

		DiscordCoreAPI::CoRoutine<void> run();

		void onMessageReceived();

		void onClosedInternal();

		void sendHeartBeat();

		void handleBuffer();

		bool parseHeader();

		void connect();
	};

	class DiscordCoreAPI_Dll VoiceSocketAgent {
	public:

		friend class DiscordCoreAPI::VoiceConnection;

		VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent* baseBaseSocketAgentNew);

		void sendVoiceData(std::vector<uint8_t>& responseData);

		void sendMessage(std::vector<uint8_t>& responseData);

		void sendMessage(std::string& dataToSend);

		void onClosedExternal();

		~VoiceSocketAgent();

	protected:

		std::unique_ptr<DiscordCoreAPI::CoRoutine<void>> theTask{ nullptr };
		std::unique_ptr<DatagramSocketSSLClient> voiceSocket{ nullptr };
		const unsigned char webSocketPayloadLengthMagicLarge{ 126 };
		const unsigned char webSocketPayloadLengthMagicHuge{ 127 };
		DiscordCoreAPI::ThreadPoolTimer heartbeatTimer{ nullptr };
		std::unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		WebSocketOpCode dataOpcode{ WebSocketOpCode::Op_Text };
		const uint64_t webSocketMaxPayloadLengthLarge{ 65535 };
		const uint64_t webSocketMaxPayloadLengthSmall{ 125 };
		WebSocketState state{ WebSocketState::Initializing };
		const unsigned char webSocketFinishBit{ (1u << 7u) };
		const uint8_t maxHeaderSize{ sizeof(uint64_t) + 2 };
		const unsigned char webSocketMaskBit{ (1u << 7u) };
		DiscordCoreAPI::EventWaiter doWeReconnect{};
		VoiceConnectInitData voiceConnectInitData{};
		BaseSocketAgent* baseSocketAgent{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		bool haveWeReceivedHeartbeatAck{ true };
		std::vector<uint8_t> inputBuffer00{};
		std::vector<uint8_t> inputBuffer01{};
		std::atomic<bool> doWeQuit{ false };
		int32_t lastNumberReceived{ 0 };
		int32_t heartbeatInterval{ 0 };
		std::string relativePath{ "" };
		std::string baseUrl{ "" };
		std::string authKey{ "" };
		std::string hostIp{ "" };
		uint32_t closeCode{ 0 };

		uint64_t createHeader(char* outbuf, uint64_t sendlength, WebSocketOpCode opCode);

		std::vector<std::string> tokenize(std::string&, std::string = "\r\n");

		DiscordCoreAPI::CoRoutine<void> run();

		void collectExternalIP();

		void onMessageReceived();

		void onClosedInternal();

		void sendHeartBeat();

		void voiceConnect();

		void handleBuffer();

		bool parseHeader();

		void connect();
	};
}
