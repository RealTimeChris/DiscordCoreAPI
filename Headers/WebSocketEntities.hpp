// WebSocketEntities.hpp - Header for the websocket related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "SongDecoder.hpp"
#include "ErlPacker.hpp"

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

	struct DiscordCoreAPI_Dll WebSocketWorkload {
		WebSocketEventType eventType{ WebSocketEventType::Unset };
		json payLoad{};
		~WebSocketWorkload() {};
	};

	class DiscordCoreAPI_Dll BaseWebSocketAgent {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class VoiceChannelWebSocketAgent;
		friend class DiscordCoreAPI::BotUser;

		BaseWebSocketAgent(string botTokenNew, string socketPathBase, unbounded_buffer<WebSocketWorkload>* workloadBufferNew);

		void connect();

		~BaseWebSocketAgent();

	protected:

		const int32_t intentsValue{ ((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 8) + (1 << 9) + (1 << 10) + (1 << 11) + (1 << 12) + (1 << 13) + (1 << 14)) };
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload>* webSocketWorkloadTarget{ nullptr };
		unbounded_buffer<VoiceConnectionData> voiceConnectionDataBuffer{};
		map<string, bool*> areWeReadyToConnectPtrs{};
		VoiceConnectInitData voiceConnectInitData{};
		ThreadPoolTimer heartbeatTimer{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		concurrency::event disconnectionEvent {};
		bool haveWeReceivedHeartbeatAck{ true };
		const int32_t maxReconnectTries{ 10 };
		MessageWebSocket webSocket{ nullptr };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		event_token messageReceivedToken{};
		int32_t currentReconnectTries{ 0 };
		bool areWeCollectingData{ false };
		bool areWeAuthenticated{ false };
		bool areWeReconnecting{ false };
		recursive_mutex accessorMutex{};
		int32_t lastNumberReceived{ 0 };
		int32_t heartbeatInterval{ 0 };
		event_token closedToken{};
		string socketPath{ "" };
		string sessionId{ "" };
		string botToken{ "" };
		

		void onMessageReceived(MessageWebSocket const&, MessageWebSocketMessageReceivedEventArgs args);

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args);

		void getVoiceConnectionData(VoiceConnectInitData doWeCollect);

		void sendMessage(json& text);

		void sendHeartBeat();

		void cleanup();
	};

	class DiscordCoreAPI_Dll VoiceChannelWebSocketAgent {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::VoiceConnection;
		friend class DiscordCoreAPI::Guild;

		VoiceChannelWebSocketAgent(concurrency::event* readyEventNew, concurrency::event* reconnectionEventNew, VoiceConnectInitData initDataNew, BaseWebSocketAgent* baseWebSocketAgentNew, bool* doWeReconnectNew);

		void sendMessage(string& text);

		void sendVoiceData(vector<uint8_t>& data);
		
		void sendConnectionData(string& message);

		void otherAgentConnect(ConnectionWebSocketData* connectionData);

		~VoiceChannelWebSocketAgent();

	protected:

		unbounded_buffer<VoiceConnectionData>* voiceConnectionDataBuffer{ nullptr };
		ConnectionWebSocketData* connectionData{ nullptr };
		DatagramSocket connectionDatagramSocket{ nullptr };
		BaseWebSocketAgent* baseWebSocketAgent{ nullptr };
		concurrency::event* reconnectionEvent { nullptr };
		event_token onConnectionDataReceivedToken{};
		VoiceConnectInitData voiceConnectInitData{};
		concurrency::event* readyEvent { nullptr };
		ThreadPoolTimer heartbeatTimer{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		DatagramSocket voiceSocket{ nullptr };
		MessageWebSocket webSocket{ nullptr };
		const int32_t maxReconnectTries{ 10 };
		event_token voiceDataReceivedToken{};
		event_token messageReceivedToken{};
		int32_t currentReconnectTries{ 0 };
		bool areWeReadyToConnect{ true };
		bool areWeAuthenticated{ false };
		int32_t lastNumberReceived{ 0 };
		int32_t heartbeatInterval{ 0 };
		bool areWeTerminating{ false };
		bool areWeWaitingForIp{ true };
		bool* doWeReconnect{ nullptr };
		event_token closedToken{};

		void connect();

		void voiceConnect();

		void collectExternalIP();

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args);

		void sendHeartBeat();

		void onMessageReceived(MessageWebSocket msgWebSocket, MessageWebSocketMessageReceivedEventArgs args);

		void onVoiceDataReceived(DatagramSocket const&, DatagramSocketMessageReceivedEventArgs const& args);

		void onConnectionDataReceived(DatagramSocket const&, DatagramSocketMessageReceivedEventArgs const& args);

		void cleanup();
	};

}
