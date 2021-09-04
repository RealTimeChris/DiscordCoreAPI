// WebSocketStuff.hpp - Header for the Websocket things of the library.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _WEBSOCKET_STUFF_
#define _WEBSOCKET_STUFF_

#include "../pch.h"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreAPI {

	class VoiceConnection;

}

namespace DiscordCoreInternal {

	class WebSocketConnectionAgent;
	
	enum class WebSocketEventType {
		CHANNEL_CREATE = 0,
		CHANNEL_UPDATE = 1,
		CHANNEL_DELETE = 2,
		GUILD_CREATE = 3,
		GUILD_UPDATE = 4,
		GUILD_DELETE = 5,
		GUILD_BAN_ADD = 6,
		GUILD_BAN_REMOVE = 7,
		GUILD_MEMBER_ADD = 8,
		GUILD_MEMBER_REMOVE = 9,
		GUILD_MEMBER_UPDATE = 10,
		ROLE_CREATE = 11,
		ROLE_UPDATE = 12,
		ROLE_DELETE = 13,
		INVITE_CREATE = 14,
		INVITE_DELETE = 15,
		MESSAGE_CREATE = 16,
		MESSAGE_UPDATE = 17,
		MESSAGE_DELETE = 18,
		MESSAGE_DELETE_BULK = 19,
		REACTION_ADD = 20,
		REACTION_REMOVE = 21,
		REACTION_REMOVE_ALL = 22,
		REACTION_REMOVE_EMOJI = 23,
		PRESENCE_UPDATE = 24,
		TYPING_START = 25,
		USER_UPDATE = 26,
		VOICE_STATE_UPDATE = 27,
		VOICE_SERVER_UPDATE = 28,
		INTERACTION_CREATE = 29
	};

	struct WebSocketWorkload {
		WebSocketEventType eventType{};
		json payLoad{};
	};

	class VoiceChannelWebSocketAgent : ThreadContext, agent {
	public:

		friend class DiscordCoreAPI::VoiceConnection;
		friend class WebSocketConnectionAgent;
		friend class Guild;

		VoiceChannelWebSocketAgent(VoiceConnectionData voiceConnectionDataNew, unbounded_buffer<bool>* readyBufferNew)
			:
			ThreadContext(*ThreadManager::getThreadContext(ThreadType::Music).get()),
			agent(*this->scheduler->scheduler) {
			this->voiceConnectionData = voiceConnectionDataNew;
			this->readyBuffer = readyBufferNew;
			return;
		}

		void sendVoiceData(vector<uint8_t> data) {

			vector<uint8_t> message = data;
			if (message.size() == 0) {
				cout << "Please specify voice data to send" << endl << endl;
				return;
			}
			else {
				this->dataWriter.WriteBytes(message);
				this->dataWriter.StoreAsync().get();
			}
			return;
		}

		void sendMessage(string text) {

			string message = text;
			if (message.empty()) {
				cout << "Please specify text to send" << endl;
				return;
			}

			cout << "Sending Voice Message: ";
			cout << message << endl;

			// Buffer any data we want to send.
			winrt::Windows::Storage::Streams::InMemoryRandomAccessStream randomAccessStream;
			DataWriter dataWriterMessage(randomAccessStream);
			dataWriterMessage.UnicodeEncoding(UnicodeEncoding::Utf8);
			dataWriterMessage.WriteString(to_hstring(message));
			dataWriterMessage.StoreAsync().get();
			randomAccessStream.FlushAsync().get();
			if (randomAccessStream.CanRead()) {
				DataReader dataReader(randomAccessStream.GetInputStreamAt(0));
				dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
				dataReader.LoadAsync((uint32_t)message.length()).get();
				IBuffer buffer = dataReader.ReadBuffer((uint32_t)message.length());
				this->webSocket.OutputStream().WriteAsync(buffer).get();
			}

			cout << "Send Complete" << endl << endl;
		}

		~VoiceChannelWebSocketAgent() {
			this->terminate();
			this->getError();
		}

	protected:
		unbounded_buffer<VoiceConnectionData> voiceConnectionDataBuffer{ nullptr };
		unbounded_buffer<bool> connectReadyBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		unbounded_buffer<bool>* readyBuffer{ nullptr };
		ThreadPoolTimer heartbeatTimer{ nullptr };
		VoiceConnectionData voiceConnectionData{};
		DatagramSocket voiceSocket{ nullptr };
		MessageWebSocket webSocket{ nullptr };
		bool didWeReceiveHeartbeatAck{ true };
		event_token voiceDataReceivedToken{};
		event_token messageReceivedToken{};
		const int maxReconnectTries{ 10 };
		DataWriter dataWriter{ nullptr };
		int currentReconnectTries{ 0 };
		bool areWeWaitingForIp{ true };
		bool areWeConnected{ false };
		int lastNumberReceived{ 0 };
		int heartbeatInterval{ 0 };
		event_token closedToken{};
		
		void getError() {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << "VoiceChannelWebSocketAgent Error: " << error.what() << endl << endl;
			}
			return;
		}

		void connect() {
			this->voiceConnectionData = receive(this->voiceConnectionDataBuffer);
			this->heartbeatTimer = ThreadPoolTimer(nullptr);
			this->webSocket = MessageWebSocket();
			this->webSocket.Control().MessageType(SocketMessageType::Utf8);
			this->closedToken = this->webSocket.Closed({ this, &VoiceChannelWebSocketAgent::onClosed });
			this->messageReceivedToken = this->webSocket.MessageReceived({ this, &VoiceChannelWebSocketAgent::onMessageReceived });
			this->webSocket.ConnectAsync(winrt::Windows::Foundation::Uri(to_hstring(this->voiceConnectionData.endpoint))).get();
			this->areWeConnected = true;
		}

		void collectExternalIP() {
			unsigned char packet[70] = { 0 };
			packet[0] = (this->voiceConnectionData.audioSSRC >> 24) & 0xff;
			packet[1] = (this->voiceConnectionData.audioSSRC>> 16) & 0xff;
			packet[2] = (this->voiceConnectionData.audioSSRC >> 8) & 0xff;
			packet[3] = (this->voiceConnectionData.audioSSRC) & 0xff;
			vector<uint8_t> sendVector;
			for (unsigned int x = 0; x < 70; x += 1) {
				sendVector.push_back(packet[x]);
			}
			this->sendVoiceData(sendVector);
		}

		void voiceConnect() {
			receive(this->connectReadyBuffer);
			this->voiceSocket = DatagramSocket();
			this->voiceSocket.Control().QualityOfService(SocketQualityOfService::LowLatency);
			winrt::Windows::Networking::HostName hostName(to_hstring(this->voiceConnectionData.voiceIp));
			auto endpointPair = this->voiceSocket.GetEndpointPairsAsync(hostName, to_hstring(this->voiceConnectionData.voicePort)).get();
			this->voiceDataReceivedToken = this->voiceSocket.MessageReceived({ this,&VoiceChannelWebSocketAgent::onVoiceDataReceived });
			this->voiceSocket.ConnectAsync(endpointPair.First().Current()).get();
			this->dataWriter = DataWriter(this->voiceSocket.GetOutputStreamAsync(hostName, to_hstring(this->voiceConnectionData.voicePort)).get());
			this->dataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
		}

		void run() {
			try {
				this->connect();
			}
			catch (const exception& e) {
				send(errorBuffer, e);
				done();
			}
		}

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args) {
			wcout << L"Voice WebSocket Closed; Code: " << args.Code() << ", Reason: " << args.Reason().c_str() << endl;
			if (args.Code() != 1000 && args.Code() != 4014) {
				if (this->maxReconnectTries > this->currentReconnectTries) {
					this->currentReconnectTries += 1;
					send(this->voiceConnectionDataBuffer, this->voiceConnectionData);
					this->connect();
				}
				else {
					this->terminate();
				}
				string resumePayload = getResumeVoicePayload(this->voiceConnectionData.guildId, this->voiceConnectionData.sessionId, this->voiceConnectionData.token);
				this->sendMessage(resumePayload);
			}
		}

		void sendHeartBeat(bool* didWeReceiveHeartbeatAckNew) {
			if (this->didWeReceiveHeartbeatAck == false) {
				if (this->maxReconnectTries > this->currentReconnectTries) {
					this->currentReconnectTries += 1;
					send(this->voiceConnectionDataBuffer, this->voiceConnectionData);
					this->connect();
				}
				else {
					this->terminate();
				}
				this->didWeReceiveHeartbeatAck = true;
				return;
			}
			string heartbeatPayload = getVoiceHeartbeatPayload(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			this->sendMessage(heartbeatPayload);
			*didWeReceiveHeartbeatAckNew = true;
		}

		void onMessageReceived(MessageWebSocket msgWebSocket, MessageWebSocketMessageReceivedEventArgs args) {
			
			hstring message;
			DataReader dataReader{ nullptr };
			if (args.GetDataReader() != nullptr) {
				dataReader = args.GetDataReader();
				dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
				if (dataReader.UnconsumedBufferLength() > 0) {
					message = dataReader.ReadString(dataReader.UnconsumedBufferLength());
				}
			}
			json payload = payload.parse(to_string(message));
			cout << "Message received from Voice WebSocket: " << to_string(message) << endl << endl;

			if (payload.contains("op")) {
				if (payload.at("op") == 6) {
					this->didWeReceiveHeartbeatAck = true;
				}

				if (payload.at("op") == 2) {
					this->voiceConnectionData.audioSSRC = payload.at("d").at("ssrc");
					this->voiceConnectionData.audioSSRC = this->voiceConnectionData.audioSSRC;
					this->voiceConnectionData.voiceIp = payload.at("d").at("ip");
					this->voiceConnectionData.voicePort = to_string(payload.at("d").at("port"));
					for (auto value : payload.at("d").at("modes")) {
						if (value == "xsalsa20_poly1305") {
							this->voiceConnectionData.voiceEncryptionMode = value;
						}
					}
					send(this->connectReadyBuffer, true);
					this->voiceConnect();
					this->collectExternalIP();
					while (this->areWeWaitingForIp) { cout << "Were here!" << endl; };
					string protocolPayloadSelectString = getSelectProtocolPayload(this->voiceConnectionData.voicePort, this->voiceConnectionData.externalIp, this->voiceConnectionData.voiceEncryptionMode);
					this->sendMessage(protocolPayloadSelectString);
				}

				if (payload.at("op") == 4) {
					for (unsigned int x = 0; x < payload.at("d").at("secret_key").size(); x += 1) {
						this->voiceConnectionData.secretKey.push_back(payload.at("d").at("secret_key").at(x).get<uint8_t>());
					}
					for (auto value : this->voiceConnectionData.secretKey) {
						this->voiceConnectionData.keys.push_back(value);
					}
					send(this->readyBuffer, true);
				}

				if (payload.at("op") == 8) {
					if (payload.at("d").contains("heartbeat_interval")) {
						this->heartbeatInterval = (int)payload.at("d").at("heartbeat_interval").get<float>();
					}
					string identifyPayload = getVoiceIdentifyPayload(this->voiceConnectionData);
					this->sendMessage(identifyPayload);
					TimerElapsedHandler onHeartBeat = [&, this](ThreadPoolTimer timer) ->void{
						VoiceChannelWebSocketAgent::sendHeartBeat(&this->didWeReceiveHeartbeatAck);
						this->didWeReceiveHeartbeatAck = false;
					};
					this->heartbeatTimer = this->heartbeatTimer.CreatePeriodicTimer(onHeartBeat, winrt::Windows::Foundation::TimeSpan(this->heartbeatInterval * 10000));
				}
			}
			return;
		}

		void onVoiceDataReceived(DatagramSocket const& , DatagramSocketMessageReceivedEventArgs const& args) {

			string message;
			DataReader dataReader{ nullptr };
			if (args.GetDataReader() != nullptr) {
				dataReader = args.GetDataReader();
				dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
				vector<uint8_t> newVector(dataReader.UnconsumedBufferLength());
				if (dataReader.UnconsumedBufferLength() > 0) {
					dataReader.ReadBytes(newVector);
				}
				for (auto value : newVector) {
					message.push_back(value);
				}
			}

			//cout << "Message received from VoiceDatagramSocket: " << to_string(to_hstring(message.c_str())) << endl << endl;

			if (this->areWeWaitingForIp) {
				this->areWeWaitingForIp = false;
				this->voiceConnectionData.externalIp = message.substr(4, message.find('\u0000', 4) - 4);
			}
		}

		void cleanup() {
			if (this != nullptr) {
				if (this->webSocket != nullptr) {
					this->webSocket.Close(1000, L"Disconnecting.");
					this->webSocket = nullptr;
				}
				if (this->heartbeatTimer != nullptr) {
					this->heartbeatTimer.Cancel();
					this->heartbeatTimer = nullptr;
				}
				if (this->voiceSocket != nullptr) {
					this->voiceSocket.Close();
					this->voiceSocket = nullptr;
				}
				
			}
		}

		void terminate() {
			done();
			this->cleanup();
		}

	};

	class WebSocketReceiverAgent : ThreadContext, agent {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;

		WebSocketReceiverAgent()
			: ThreadContext(*ThreadManager::getThreadContext().get()), 
			agent(*this->scheduler->scheduler)
		{
			return;
		}

		~WebSocketReceiverAgent() {
			this->terminate();
			this->getError();
			return;
		}

	protected:
		unbounded_buffer<WebSocketWorkload> workloadTarget{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		unbounded_buffer<json> workloadSource{ nullptr };
		bool doWeQuit{ false };

		void getError() {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << "WebSocketReceiverAgent Error: " << error.what() << endl << endl;
			}
			return;
		}

		void run() {
			try {
				while (doWeQuit == false) {
					json payload = receive(this->workloadSource, INFINITE);
					this->onMessageReceived(payload);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}

		void sendWorkload(WebSocketWorkload workload) {
			send(this->workloadTarget, workload);
		}

		void onMessageReceived(json payload) {
			WebSocketWorkload workload;
			workload.payLoad = payload.at("d");

			if (payload.at("t") == "CHANNEL_CREATE") {
				workload.eventType = WebSocketEventType::CHANNEL_CREATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "CHANNEL_UPDATE") {
				workload.eventType = WebSocketEventType::CHANNEL_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "CHANNEL_DELETE") {
				workload.eventType = WebSocketEventType::CHANNEL_DELETE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_CREATE") {
				workload.eventType = WebSocketEventType::GUILD_CREATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_UPDATE") {
				workload.eventType = WebSocketEventType::GUILD_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_DELETE") {
				workload.eventType = WebSocketEventType::GUILD_DELETE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_BAN_ADD") {
				workload.eventType = WebSocketEventType::GUILD_BAN_ADD;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_BAN_REMOVE") {
				workload.eventType = WebSocketEventType::GUILD_BAN_REMOVE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_MEMBER_ADD") {
				workload.eventType = WebSocketEventType::GUILD_MEMBER_ADD;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_MEMBER_REMOVE") {
				workload.eventType = WebSocketEventType::GUILD_MEMBER_REMOVE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_MEMBER_UPDATE") {
				workload.eventType = WebSocketEventType::GUILD_MEMBER_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_ROLE_CREATE") {
				workload.eventType = WebSocketEventType::ROLE_CREATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_ROLE_UPDATE") {
				workload.eventType = WebSocketEventType::ROLE_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "GUILD_ROLE_DELETE") {
				workload.eventType = WebSocketEventType::ROLE_DELETE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "INVITE_CREATE") {
				workload.eventType = WebSocketEventType::INVITE_CREATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "INVITE_DELETE") {
				workload.eventType = WebSocketEventType::INVITE_DELETE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "INTERACTION_CREATE") {
				workload.eventType = WebSocketEventType::INTERACTION_CREATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_CREATE") {
				workload.eventType = WebSocketEventType::MESSAGE_CREATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_UPDATE") {
				workload.eventType = WebSocketEventType::MESSAGE_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_DELETE") {
				workload.eventType = WebSocketEventType::MESSAGE_DELETE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_DELETE_BULK") {
				workload.eventType = WebSocketEventType::MESSAGE_DELETE_BULK;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_REACTION_ADD") {
				workload.eventType = WebSocketEventType::REACTION_ADD;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_REACTION_REMOVE") {
				workload.eventType = WebSocketEventType::REACTION_REMOVE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_REACTION_REMOVE_ALL") {
				workload.eventType = WebSocketEventType::REACTION_REMOVE_ALL;
				sendWorkload(workload);
			}

			if (payload.at("t") == "MESSAGE_REACTION_REMOVE_EMOJI") {
				workload.eventType = WebSocketEventType::REACTION_REMOVE_EMOJI;
				sendWorkload(workload);
			}

			if (payload.at("t") == "PRESENCE_UPDATE") {
				workload.eventType = WebSocketEventType::PRESENCE_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "TYPING_START") {
				workload.eventType = WebSocketEventType::TYPING_START;
				sendWorkload(workload);
			}

			if (payload.at("t") == "USER_UPDATE") {
				workload.eventType = WebSocketEventType::USER_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "VOICE_STATE_UPDATE") {
				workload.eventType = WebSocketEventType::VOICE_STATE_UPDATE;
				sendWorkload(workload);
			}

			if (payload.at("t") == "VOICE_SERVER_UPDATE") {
				workload.eventType = WebSocketEventType::VOICE_SERVER_UPDATE;
				sendWorkload(workload);
			}
		}

		void terminate() {
			this->doWeQuit = true;
		}
	};

	class WebSocketConnectionAgent : ThreadContext, agent {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class VoiceChannelWebSocketAgent;

		WebSocketConnectionAgent(unbounded_buffer<json>* target, hstring botTokenNew, bool* doWeQuitNew)
			:ThreadContext(*ThreadManager::getThreadContext().get()), agent(*this->scheduler->scheduler) {
			this->webSocketMessageTarget = target;
			this->botToken = botTokenNew;
			this->doWeQuit = doWeQuitNew;
			return;
		}

		void setSocketPath(string socketPathBase) {
			std::wstringstream stream;
			stream << DiscordCoreInternal::getSocketPath(to_hstring(socketPathBase)).c_str();
			stream << L"/?v=9&encoding=json";
			this->socketPath = stream.str();
			return;
		}

		void sendMessage(string& text) {

			string message = text;
			if (message.empty()) {
				cout << "Please specify text to send" << endl;
				return;
			}

			cout << "Sending Message: ";
			cout << message << endl;

			if (this->messageWriter != nullptr) {
				this->messageWriter.WriteString(to_hstring(message));
			}

			if (this->messageWriter != nullptr) {
				this->messageWriter.StoreAsync().get();
			}

			cout << "Send Complete" << endl << endl;
			return;
		}

		VoiceConnectionData getVoiceConnectionData(string channelId, string guildId) {
			UpdateVoiceStateData dataPackage;
			dataPackage.channelId = channelId;
			dataPackage.guildId = guildId;
			dataPackage.selfDeaf = false;
			dataPackage.selfMute = false;
			string newString = getVoiceStateUpdatePayload(dataPackage);
			this->areWeCollectingData = true;
			this->sendMessage(newString);
			VoiceConnectionData newData = receive(this->voiceConnectionDataBuffer);
			return newData;
		}

		~WebSocketConnectionAgent() {
			this->terminate();
			this->getError();
		}

	protected:
		int intentsValue{ ((1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 8) + (1 << 9) + (1 << 10) + (1 << 11) + (1 << 12) + (1 << 13) + (1 << 14)) };
		unbounded_buffer<VoiceConnectionData> voiceConnectionDataBuffer{ nullptr };
		unbounded_buffer<json>* webSocketMessageTarget{ nullptr };
		VoiceConnectionData* pVoiceConnectionData{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		ThreadPoolTimer heartbeatTimer{ nullptr };
		MessageWebSocket webSocket{ nullptr };
		bool didWeReceiveHeartbeatAck{ true };
		DataWriter messageWriter{ nullptr };
		bool serverUpdateCollected{ false };
		bool stateUpdateCollected{ false };
		event_token messageReceivedToken{};
		const int maxReconnectTries{ 10 };
		bool areWeCollectingData{ false };
		int currentReconnectTries{ 0 };
		bool isThisConnected{ false };
		int lastNumberReceived{ 0 };
		int heartbeatInterval{ 0 };
		hstring socketPath{ L"" };
		event_token closedToken{};
		hstring sessionID{ L"" };
		hstring botToken{ L"" };
		bool* doWeQuit{};
		
		void getError() {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << "WebSocketConnectionAgent Error: " << error.what() << endl;
			}
		}

		void run() {
			try {
				if (!this->isThisConnected) {
					this->connect();
					return;
				}
			}
			catch (exception& e) {
				send(this->errorBuffer, e);
				done();
				return;
			}
		}

		void connect() {
			this->webSocket = MessageWebSocket();
			this->messageWriter = DataWriter(this->webSocket.OutputStream());
			this->webSocket.Control().MessageType(SocketMessageType::Utf8);
			this->messageWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
			this->closedToken = this->webSocket.Closed({ this, &WebSocketConnectionAgent::onClosed });
			this->messageReceivedToken = this->webSocket.MessageReceived({ this, &WebSocketConnectionAgent::onMessageReceived });
			this->webSocket.ConnectAsync(winrt::Windows::Foundation::Uri(this->socketPath)).get();
			this->isThisConnected = true;
		}

		void onClosed(IWebSocket const&, WebSocketClosedEventArgs const& args) {
			wcout << L"WebSocket Closed; Code: " << args.Code() << ", Reason: " << args.Reason().c_str() << endl;
			if (this->maxReconnectTries > this->currentReconnectTries) {
				this->currentReconnectTries += 1;
				this->connect();
			}
			else {
				this->terminate();
			}
			return;
		}

		void sendHeartBeat() {
			if (this->didWeReceiveHeartbeatAck == false) {
				if (this->maxReconnectTries > this->currentReconnectTries) {
					this->currentReconnectTries += 1;
					this->connect();
				}
				else {
					this->terminate();
				}
				this->didWeReceiveHeartbeatAck = true;
				return;
			}
			string heartbeat = getHeartbeatPayload(this->lastNumberReceived);
			this->sendMessage(heartbeat);
			this->didWeReceiveHeartbeatAck = false;
		}

		void onMessageReceived(MessageWebSocket const&, MessageWebSocketMessageReceivedEventArgs const& args) {
			DataReader dataReader{ nullptr };
			hstring message;
			if (args != nullptr) {
				if (args.IsMessageComplete()) {
					if (args.GetDataReader() != nullptr) {
						dataReader = args.GetDataReader();
						dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
						if (dataReader.UnconsumedBufferLength() > 0) {
							message = dataReader.ReadString(dataReader.UnconsumedBufferLength());
						}
					}
				}
			}
			json payload = payload.parse(to_string(message));
			
			send(*this->webSocketMessageTarget, payload);

			if (this->areWeCollectingData == true && payload.at("t") == "VOICE_SERVER_UPDATE" && !this->serverUpdateCollected) {
				if (this->serverUpdateCollected != true && this->stateUpdateCollected != true) {
					this->pVoiceConnectionData = new VoiceConnectionData;
					this->pVoiceConnectionData->endpoint = payload.at("d").at("endpoint").get<string>();
					this->pVoiceConnectionData->token = payload.at("d").at("token").get<string>();
					this->serverUpdateCollected = true;
				}
				else {
					this->pVoiceConnectionData->endpoint = payload.at("d").at("endpoint").get<string>();
					this->pVoiceConnectionData->token = payload.at("d").at("token").get<string>();
					send(this->voiceConnectionDataBuffer, *this->pVoiceConnectionData);
					delete this->pVoiceConnectionData;
					this->pVoiceConnectionData = nullptr;
					this->areWeCollectingData = false;
					this->serverUpdateCollected = false;
					this->stateUpdateCollected = false;
				}
			}

			if (this->areWeCollectingData == true && payload.at("t") == "VOICE_STATE_UPDATE" && !this->stateUpdateCollected) {
				if (this->stateUpdateCollected != true && this->serverUpdateCollected != true) {
					this->pVoiceConnectionData = new VoiceConnectionData;
					this->pVoiceConnectionData->sessionId = payload.at("d").at("session_id").get<string>();
					this->stateUpdateCollected = true;
				}
				else {
					this->pVoiceConnectionData->sessionId = payload.at("d").at("session_id").get<string>();
					send(this->voiceConnectionDataBuffer, *this->pVoiceConnectionData);
					delete this->pVoiceConnectionData;
					this->pVoiceConnectionData = nullptr;
					this->areWeCollectingData = false;
					this->serverUpdateCollected = false;
					this->stateUpdateCollected = false;
				}
			}

			if (payload.at("s") >= 0) {
				this->lastNumberReceived = payload.at("s");
			}

			if (payload.at("t") == "PRESENCE_UPDATE") {
				return;
			}

			if (payload.at("t") == "GUILD_CREATE") {
				return;
			}

			if (payload.at("t") == "READY") {
				string sessionIDTemp;
				sessionIDTemp = payload.at("d").at("session_id");
				this->sessionID = to_hstring(sessionIDTemp);
			}

			if (payload.at("op") == 1) {
				this->sendHeartBeat();
			}

			if (payload.at("op") == 6) {
				cout << "Resuming!" << endl << endl;
				string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
				this->sendMessage(resume);
			}

			if (payload.at("op") == 7) {
				cout << "Reconnecting (Type 7)!" << endl << endl;
				string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
				this->sendMessage(resume);
				if (this->maxReconnectTries > this->currentReconnectTries) {
					this->currentReconnectTries += 1;
					this->connect();
				}
				else {
					this->terminate();
				}
			}

			if (payload.at("op") == 9) {
				cout << "Reconnecting (Type 9)!" << endl << endl;
				string resume = getResumePayload(to_string(this->botToken), to_string(this->sessionID), this->lastNumberReceived);
				this->sendMessage(resume);
				if (this->maxReconnectTries > this->currentReconnectTries) {
					this->currentReconnectTries += 1;
					this->connect();
				}
				else {
					this->terminate();
				}
			}

			if (payload.at("op") == 10) {
				this->heartbeatInterval = payload.at("d").at("heartbeat_interval");
				TimerElapsedHandler onHeartBeat = [this](ThreadPoolTimer timer) {
					WebSocketConnectionAgent::sendHeartBeat();
					return;
				};
				this->heartbeatTimer = this->heartbeatTimer.CreatePeriodicTimer(onHeartBeat, winrt::Windows::Foundation::TimeSpan(this->heartbeatInterval * 10000));
				std::string identity = getIdentifyPayload(to_string(this->botToken), this->intentsValue);
				this->sendMessage(identity);
			}

			if (payload.at("op") == 11) {
				this->didWeReceiveHeartbeatAck = true;
			}

			cout << "Message received from WebSocket: " << to_string(message) << endl << endl;
		};

		void cleanup() {
			if (this->messageWriter != nullptr) {
				this->messageWriter.DetachStream();
				this->messageWriter.Close();
				this->messageWriter = nullptr;
			}

			if (this->webSocket != nullptr) {
				this->webSocket.Close(1000, L"Closed due to user request.");
				this->webSocket = nullptr;
			}

			if (this->heartbeatTimer) {
				this->heartbeatTimer.Cancel();
				this->heartbeatTimer = nullptr;
			}
		}

		void terminate() {
			*this->doWeQuit = true;
			this->done();
			this->cleanup();
		}

	};
}
#endif

