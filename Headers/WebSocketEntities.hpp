// WebSocketEntities.hpp - Header for the webSocket related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "SongDecoder.hpp"
#include "FoundationEntities.hpp"
#include "ErlPacker.hpp"
#include "SSLClients.hpp"
#include "EventEntities.hpp"
#include "ThreadPool.hpp"

namespace DiscordCoreInternal {

	constexpr uint64_t WebSocketMaxPayloadLengthLarge = 65535;
	constexpr unsigned char WebSocketPayloadLengthMagicLarge = 126;
	constexpr unsigned char WebSocketPayloadLengthMagicHuge = 127;
	constexpr uint64_t WebSocketMaxPayloadLengthSmall = 125;
	constexpr uint8_t MaxHeaderSize = sizeof(uint64_t) + 2;
	constexpr unsigned char WebSocketFinishBit = (1u << 7u);
	constexpr unsigned char WebSocketMaskBit = (1u << 7u);

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

	enum class WebSocketOpCodes : uint8_t {
		WS_OP_CONTINUATION = 0x00,
		WS_OP_TEXT = 0x01,
		WS_OP_BINARY = 0x02,
		WS_OP_CLOSE = 0x08,
		WS_OP_PING = 0x09,
		WS_OP_PONG = 0x0a
	};

	struct DiscordCoreAPI_Dll WebSocketWorkload {

		WebSocketEventType eventType{ WebSocketEventType::Unset };
		json payLoad{};

		WebSocketWorkload() {}

		WebSocketWorkload& operator=(WebSocketWorkload&& other) noexcept {
			if (this != &other) {
				this->payLoad = move(other.payLoad);
				other.payLoad = json();
				this->eventType = other.eventType;
				other.eventType = WebSocketEventType::Unset;
			}
			return *this;
		}

		WebSocketWorkload(WebSocketWorkload&& other) noexcept {
			*this = move(other);
		}

		WebSocketWorkload& operator=(const WebSocketWorkload& other) noexcept {
			WebSocketWorkload& theVal = const_cast<WebSocketWorkload&>(other);
			this->payLoad = move(theVal.payLoad);
			this->eventType = other.eventType;
			return *this;
		}

		WebSocketWorkload(const WebSocketWorkload& other) {
			WebSocketWorkload& theVal = const_cast<WebSocketWorkload&>(other);
			*this = move(theVal);
		}

		~WebSocketWorkload() {};
	};

	class DiscordCoreAPI_Dll WebSocketAgent {
	public:

		friend class DatagramSocketAgent;

		WebSocketAgent(string botToken, string hostname, string port = "443", string urlpath = "", DiscordCoreAPI::UnboundedMessageBlock<WebSocketWorkload>* workloadTarget = nullptr, WebSocketOpCodes opCode = WebSocketOpCodes::WS_OP_BINARY);

		WebSocketAgent(nullptr_t);

		void sendMessage(string& dataToSend);

		void sendMessage(json& responseData);

		~WebSocketAgent();

	protected:

		const int32_t intentsValue{ ((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 8) + (1 << 9) + (1 << 10) + (1 << 11) + (1 << 12) + (1 << 13) + (1 << 14)) };
		DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreInternal::WebSocketWorkload>* webSocketWorkloadTarget{ nullptr };
		DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData> voiceConnectionDataBuffer{};
		DiscordCoreAPI::ThreadPoolTimer heartbeatTimer{ nullptr };
		DiscordCoreAPI::Event<void, void> disconnectionEvent{};
		unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		DiscordCoreAPI::CoRoutineWrapper theTask{ nullptr };
		map<string, bool*> areWeReadyToConnectPtrs{};
		VoiceConnectInitData voiceConnectInitData{};
		VoiceConnectionData voiceConnectionData{};
		bool haveWeReceivedHeartbeatAck{ true };
		const int32_t maxReconnectTries{ 10 };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		int32_t currentReconnectTries{ 0 };
		map<string, string> HttpHeaders{};
		bool areWeCollectingData{ false };
		bool areWeAuthenticated{ false };
		int32_t lastNumberReceived{ 0 };
		int32_t heartbeatInterval{ 0 };
		WebSocketOpCodes dataOpcode{};
		string connectionPath{ "" };
		mutex accessorMutex02{};
		mutex accessorMutex01{};
		string socketPath{ "" };
		uint32_t errorCode{ 0 };
		bool doWeQuit{ false };
		WebSocketState state{};
		string sessionId{ "" };
		string botToken{ "" };
		string authKey{ "" };
		string port{ "" };

		uint64_t createHeader(unsigned char* outbuf, uint64_t sendlength, WebSocketOpCodes opCode);

		void tokenize(const string&, vector<string>&, string = "\r\n");

		void getVoiceConnectionData(VoiceConnectInitData doWeCollect);

		DiscordCoreAPI::CoRoutine<void> run();

		bool onMessageReceived();

		void sendHeartBeat();

		void handleBuffer();

		bool parseHeader();

		void onClosed();

		void connect();
	};

	class DiscordCoreAPI_Dll DatagramSocketAgent {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;

		DatagramSocketAgent(DiscordCoreAPI::Event<void, void>* readyEventNew, DiscordCoreAPI::Event<void, void>* reconnectionEventNew, VoiceConnectInitData initDataNew, WebSocketAgent* baseWebSocketAgentNew, bool* doWeReconnectNew);

		void sendVoiceData(string& responseData);

		void sendMessage(vector<uint8_t>& responseData);

		void sendMessage(string& dataToSend);

		~DatagramSocketAgent();

	protected:

		DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>* voiceConnectionDataBuffer{ nullptr };
		DiscordCoreAPI::Event<void, void>* reconnectionEvent{ nullptr };
		unique_ptr<DatagramSocketSSLClient> voiceSocket{ nullptr };
		unique_ptr<bool> areWeReadyToConnect{ make_unique<bool>() };
		WebSocketOpCodes dataOpcode{ WebSocketOpCodes::WS_OP_TEXT };
		DiscordCoreAPI::ThreadPoolTimer heartbeatTimer{ nullptr };
		DiscordCoreAPI::Event<void, void>* readyEvent{ nullptr };
		unique_ptr<WebSocketSSLClient> webSocket{ nullptr };
		WebSocketState state{ WebSocketState::Initializing };
		DiscordCoreAPI::CoRoutineWrapper theTask{ nullptr };
		WebSocketAgent* webSocketAgent{ nullptr };
		VoiceConnectInitData voiceConnectInitData{};
		VoiceConnectionData voiceConnectionData{};
		const int32_t maxReconnectTries{ 10 };
		int32_t currentReconnectTries{ 0 };
		map<string, string> HttpHeaders{};
		bool areWeAuthenticated{ false };
		int32_t lastNumberReceived{ 0 };
		int32_t heartbeatInterval{ 0 };
		bool areWeTerminating{ false };
		bool areWeWaitingForIp{ true };
		bool* doWeReconnect{ nullptr };
		string relativePath{ "" };
		mutex accessorMutex02{};
		mutex accessorMutex01{};
		uint32_t errorCode{ 0 };
		bool doWeQuit{ false };
		string hostName{ "" };
		string port{ "443" };
		string authKey{ "" };
		string hostIp{ "" };

		uint64_t createHeader(char* outbuf, uint64_t sendlength, WebSocketOpCodes opCode);

		void tokenize(const string&, vector<string>&, string = "\r\n");

		DiscordCoreAPI::CoRoutine<void> run();

		void collectExternalIP();

		void onMessageReceived();

		void sendHeartBeat();

		void voiceConnect();

		void handleBuffer();

		bool parseHeader();

		void onClosed();

		void connect();
	};

}