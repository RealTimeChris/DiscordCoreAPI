// WebSocketEntities.hpp - Header for the websocket related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	enum class WebSocketEventType {
		Application_Command_Create = 0,
		Application_Command_Update = 1,
		Application_Command_Delete = 2,
		Channel_Create = 3,
		Channel_Update = 4,
		Channel_Delete = 5,
		Channel_Pins_Update = 6,
		Thread_Create = 7,
		Thread_Update = 8,
		Thread_Delete = 9,
		Thread_List_Sync = 10,
		Thread_Member_Update = 11,
		Thread_Members_Update = 12,
		Guild_Create = 13,
		Guild_Update = 14,
		Guild_Delete = 15,
		Guild_Ban_Add = 16,
		Guild_Ban_Remove = 17,
		Guild_Emojis_Update = 18,
		Guild_Stickers_Update = 19,
		Guild_Integrations_Update = 20,
		Guild_Member_Add = 21,
		Guild_Member_Remove = 22,
		Guild_Member_Update = 23,
		Guild_Members_Chunk = 24,
		Guild_Role_Create = 25,
		Guild_Role_Update = 26,
		Guild_Role_Delete = 27,
		Integration_Create = 28,
		Integration_Update = 29,
		Integration_Delete = 30,
		Interaction_Create = 31,
		Invite_Create = 32,
		Invite_Delete = 33,
		Message_Create = 34,
		Message_Update = 35,
		Message_Delete = 36,
		Message_Delete_Bulk = 37,
		Message_Reaction_Add = 38,
		Message_Reaction_Remove = 39,
		Message_Reaction_Remove_All = 40,
		Message_Reaction_Remove_Emoji = 41,
		Presence_Update = 42,
		Stage_Instance_Create = 43,
		Stage_Instance_Delete = 44,
		Stage_Instance_Update = 45,
		Typing_Start = 46,
		User_Update = 47,
		Voice_State_Update = 48,
		Voice_Server_Update = 49,
		Webhooks_Update = 50
	};

	enum class ConnectionWebSocketType {
		Receive = 0,
		Send = 1
	};

	struct DiscordCoreAPI_Dll WebSocketWorkload {
		WebSocketEventType eventType{};
		json payLoad{};
	};

	struct DiscordCoreAPI_Dll GetVoiceConnectionData {
		string channelId{ "" };
		string guildId{ "" };
		string userId{ "" };
	};

	class DiscordCoreAPI_Dll BaseWebSocketAgent {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class VoiceChannelWebSocketAgent;
		friend class DiscordCoreAPI::BotUser;

		BaseWebSocketAgent(string botTokenNew, string socketPathBase, unbounded_buffer<WebSocketWorkload>* workloadTarget);

		void connect();

		~BaseWebSocketAgent();

	protected:

		const __int32 intentsValue{ ((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 8) + (1 << 9) + (1 << 10) + (1 << 11) + (1 << 12) + (1 << 13) + (1 << 14)) };
		shared_ptr<unbounded_buffer<VoiceConnectionData>> voiceConnectionDataBuffer{ nullptr };
		unbounded_buffer<WebSocketWorkload>* webSocketWorkloadTarget{ nullptr };
		GetVoiceConnectionData voiceConnectInitData{};
		map<string, bool*> areWeReadyToConnectPtrs{};
		ThreadPoolTimer heartbeatTimer{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		concurrency::event disconnectionEvent {};
		const __int32 maxReconnectTries{ 10 };
		MessageWebSocket webSocket{ nullptr };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		event_token messageReceivedToken{};
		__int32 currentReconnectTries{ 0 };
		bool areWeCollectingData{ false };
		bool areWeAuthenticated{ false };
		bool areWeReconnecting{ false };
		__int32 lastNumberReceived{ 0 };
		__int32 heartbeatInterval{ 0 };
		event_token closedToken{};
		string socketPath{ "" };
		string sessionId{ "" };
		string botToken{ "" };

		void onMessageReceived(MessageWebSocket const&, MessageWebSocketMessageReceivedEventArgs const& args);

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args);

		void getVoiceConnectionData(GetVoiceConnectionData doWeCollect);

		void sendMessage(string& text);

		void sendHeartBeat();

		void cleanup();
	};

	class DiscordCoreAPI_Dll VoiceChannelWebSocketAgent {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::Guild;

		VoiceChannelWebSocketAgent(shared_ptr<concurrency::event> readyEventNew, VoiceConnectInitData initDataNew, shared_ptr<BaseWebSocketAgent> baseWebSocketAgentNew, bool* doWeReconnectNew, shared_ptr<concurrency::event> reconnectionEvent);

		void sendVoiceData(vector<unsigned __int8> data);

		void sendMessage(string text);

		~VoiceChannelWebSocketAgent();

	protected:

		shared_ptr<unbounded_buffer<VoiceConnectionData>> voiceConnectionDataBuffer{ nullptr };
		shared_ptr<BaseWebSocketAgent> baseWebSocketAgent{ nullptr };
		shared_ptr<concurrency::event> reconnectionEvent{ nullptr };
		shared_ptr<concurrency::event> readyEvent{ nullptr };
		shared_ptr<DatagramSocket> voiceSocket{ nullptr };
		shared_ptr<MessageWebSocket> webSocket{ nullptr };
		VoiceConnectInitData voiceConnectInitData{};
		ThreadPoolTimer heartbeatTimer{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		const __int32 maxReconnectTries{ 10 };		
		event_token voiceDataReceivedToken{};
		event_token messageReceivedToken{};
		__int32 currentReconnectTries{ 0 };
		bool areWeReadyToConnect{ true };
		bool areWeAuthenticated{ false };
		__int32 lastNumberReceived{ 0 };
		__int32 heartbeatInterval{ 0 };
		bool areWeTerminating{ false };		
		bool areWeWaitingForIp{ true };
		bool* doWeReconnect{ nullptr };		
		event_token closedToken{};

		void connect();

		void collectExternalIP();

		void voiceConnect();

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args);

		void sendHeartBeat();

		void onMessageReceived(MessageWebSocket msgWebSocket, MessageWebSocketMessageReceivedEventArgs args);

		void onVoiceDataReceived(DatagramSocket const&, DatagramSocketMessageReceivedEventArgs const& args);

		void cleanup();

		void terminate();
	};

	class DiscordCoreAPI_Dll ConnectionWebSocketAgent {
	public:
		ConnectionWebSocketAgent(string endpoint, ConnectionWebSocketType type, string port, VoiceConnectInitData initDataNew, shared_ptr<BaseWebSocketAgent> baseWebSocketAgentNew);

		void sendData(vector<unsigned __int8> data);

		~ConnectionWebSocketAgent();

	protected:
		shared_ptr<VoiceChannelWebSocketAgent> voiceConnectionAgent{ nullptr };
		shared_ptr<DatagramSocket> datagramSocket{ nullptr };
		event_token onDataReceivedToken{};
		DataWriter dataWriter{ nullptr };
		ConnectionWebSocketType type{};
		string endpoint{ "" };
		string port{ "" };

		void connect(string endpoint, ConnectionWebSocketType type, string port);

		void onDataReceived(DatagramSocket const&, DatagramSocketMessageReceivedEventArgs const& args);

		void run();
	};

}
