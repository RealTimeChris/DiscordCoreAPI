/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// VoiceConnection.hpp - Header for the voice connection class.
/// Jul 15, 2021
/// https://discordcoreapi.com
/// \file VoiceConnection.hpp

#pragma once

#include <discordcoreapi/Utilities/UDPConnection.hpp>
#include <discordcoreapi/Utilities/AudioEncoder.hpp>
#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/Utilities/RingBuffer.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/WebSocketClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <sodium.h>

namespace DiscordCoreAPI {

	/// @brief Voice Websocket close codes.
	class VoiceWebSocketClose {
	  public:
		/// @brief Voice Websocket close codes.
		enum class VoiceWebSocketCloseCode : uint16_t {
			Unset = 1 << 0,///< Unset.
			Normal_Close = 1 << 1,///< Normal close.
			Unknown_Opcode = 1 << 2,///< You sent an invalid opcode.
			Failed_To_Decode = 1 << 3,///< You sent an invalid payload in your identifying to the Gateway.
			Not_Authenticated = 1 << 4,///< You sent a payload before identifying with the Gateway.
			Authentication_Failed = 1 << 5,///<	The token you sent in your identify payload is incorrect.
			Already_Authenticated = 1 << 6,///<	You sent more than one identify payload. Stahp.
			Session_No_Longer_Valid = 1 << 7,///< Your session is no longer valid.
			Session_Timeout = 1 << 8,///< Your session has timed out.
			Server_Not_Found = 1 << 9,///< We can't find the server you're trying to connect to.
			Unknown_Protocol = 1 << 10,///< We didn't recognize the protocol you sent.
			Disconnected = 1
				<< 11,///< ChannelData was deleted, you were kicked, voice server changed, or the main gateway session was dropped. Should not reconnect.
			Voice_Server_Crashed = 1 << 12,///< The server crashed. Our bad! Try resuming.
			Unknown_Encryption_Mode = 1 << 13///< We didn't recognize your encryption.
		};

		inline static UnorderedMap<int32_t, VoiceWebSocketCloseCode> mappingValues{ { 0, VoiceWebSocketCloseCode::Unset },
			{ 1000, VoiceWebSocketCloseCode::Normal_Close }, { 4001, VoiceWebSocketCloseCode::Unknown_Opcode },
			{ 4002, VoiceWebSocketCloseCode::Failed_To_Decode }, { 4003, VoiceWebSocketCloseCode::Not_Authenticated },
			{ 4004, VoiceWebSocketCloseCode::Authentication_Failed }, { 4005, VoiceWebSocketCloseCode::Already_Authenticated },
			{ 4006, VoiceWebSocketCloseCode::Session_No_Longer_Valid }, { 4009, VoiceWebSocketCloseCode::Session_Timeout },
			{ 4011, VoiceWebSocketCloseCode::Server_Not_Found }, { 4012, VoiceWebSocketCloseCode::Unknown_Protocol },
			{ 4014, VoiceWebSocketCloseCode::Disconnected }, { 4015, VoiceWebSocketCloseCode::Voice_Server_Crashed },
			{ 4016, VoiceWebSocketCloseCode::Unknown_Encryption_Mode } };

		inline static UnorderedMap<VoiceWebSocketCloseCode, std::string_view> outputErrorValues{ { VoiceWebSocketCloseCode::Unset, "Unset." },
			{ VoiceWebSocketCloseCode::Normal_Close, "Normal close." }, { VoiceWebSocketCloseCode::Unknown_Opcode, "You sent an invalid opcode." },
			{ VoiceWebSocketCloseCode::Failed_To_Decode, "You sent an invalid payload in your identifying to the Gateway." },
			{ VoiceWebSocketCloseCode::Not_Authenticated, "You sent a payload before identifying with the Gateway." },
			{ VoiceWebSocketCloseCode::Authentication_Failed, "The token you sent in your identify payload is incorrect." },
			{ VoiceWebSocketCloseCode::Already_Authenticated, "You sent more than one identify payload. Stahp." },
			{ VoiceWebSocketCloseCode::Session_No_Longer_Valid, "Your session is no longer valid." },
			{ VoiceWebSocketCloseCode::Session_Timeout, "Your session has timed out." },
			{ VoiceWebSocketCloseCode::Server_Not_Found, "We can't find the server you're trying to connect to." },
			{ VoiceWebSocketCloseCode::Unknown_Protocol, "We didn't recognize the protocol you sent." },
			{ VoiceWebSocketCloseCode::Disconnected,
				"ChannelData was deleted, you were kicked, voice server changed, or the main gateway session was dropped. Should not "
				"reconnect." },
			{ VoiceWebSocketCloseCode::Voice_Server_Crashed, "The server crashed. Our bad! Try resuming." },
			{ VoiceWebSocketCloseCode::Unknown_Encryption_Mode, "We didn't recognize your encryption." } };

		VoiceWebSocketCloseCode value{};

		inline VoiceWebSocketClose& operator=(uint16_t valueNew) {
			value = static_cast<VoiceWebSocketCloseCode>(valueNew);
			return *this;
		};

		inline VoiceWebSocketClose(uint16_t value) {
			*this = value;
		};

		inline operator std::string_view() {
			return VoiceWebSocketClose::outputErrorValues[mappingValues[static_cast<uint16_t>(value)]];
		}

		inline operator bool() {
			return true;
		}
	};

	struct VoiceSocketReadyData {
		std::vector<std::string> modes{};
		std::string ip{};
		uint64_t port{};
		uint32_t ssrc{};
	};

	struct VoiceSessionDescriptionData {
		std::vector<uint8_t> secretKey{};
	};

	struct SpeakingData {
		Snowflake userId{};
		uint32_t ssrc{};
	};

	struct VoiceConnectionHelloData {
		uint32_t heartBeatInterval{};
	};

	struct VoiceUserDisconnectData {
		Snowflake userId{};
	};

	struct DiscordCoreAPI_Dll VoiceUser {
		VoiceUser() = default;

		VoiceUser(Snowflake userId);

		VoiceUser& operator=(VoiceUser&&) noexcept;

		VoiceUser& operator=(const VoiceUser&) = delete;

		VoiceUser(const VoiceUser&) = delete;

		DiscordCoreInternal::OpusDecoderWrapper& getDecoder();

		std::basic_string_view<uint8_t> extractPayload();

		void insertPayload(std::basic_string_view<uint8_t>);

		Snowflake getUserId();

	  protected:
		DiscordCoreInternal::RingBuffer<uint8_t, 10> payloads{};
		DiscordCoreInternal::OpusDecoderWrapper decoder{};
		Snowflake userId{};
	};

	struct DiscordCoreAPI_Dll RTPPacketEncrypter {
		RTPPacketEncrypter() = default;

		RTPPacketEncrypter(uint32_t ssrcNew, const std::basic_string<uint8_t>& keysNew);

		std::basic_string_view<uint8_t> encryptPacket(DiscordCoreInternal::EncoderReturnData& audioData);

	  protected:
		std::basic_string<uint8_t> data{};
		std::basic_string<uint8_t> keys{};
		uint8_t version{ 0x80 };
		uint8_t flags{ 0x78 };
		uint32_t timeStamp{};
		uint16_t sequence{};
		uint32_t ssrc{};
	};

	struct DiscordCoreAPI_Dll MovingAverager {
		MovingAverager(uint64_t collectionCountNew);

		MovingAverager operator+=(int64_t value);

		operator float();

	  protected:
		std::deque<int64_t> values{};
		uint64_t collectionCount{};
	};

	/// @brief The various opcodes that could be sent/received by the voice-websocket.
	enum class VoiceSocketOpCodes {
		Identify = 0,///< Begin a voice websocket connection.
		Select_Protocol = 1,///< Select the voice protocol.
		Ready_Server = 2,///< Complete the websocket handshake.
		Heartbeat = 3,///< Keep the websocket connection alive.
		Session_Description = 4,///< Describe the session.
		Speaking = 5,///< Indicate which users are speaking.
		Heartbeat_ACK = 6,///< Sent to acknowledge a received client heartbeat.
		Resume = 7,///< Resume a connection.
		Hello = 8,///< Time to wait between sending heartbeats in milliseconds.
		Resumed = 9,///< Acknowledge a successful session resume.
		Client_Disconnect = 13,///< A client has disconnected from the voice channel.
	};

	/// @brief For the various connection states of the VoiceConnection class.
	enum class VoiceConnectionState : uint8_t {
		Collecting_Init_Data = 0,///< Collecting initialization data.
		Initializing_WebSocket = 1,///< Initializing the WebSocket.
		Collecting_Hello = 2,///< Collecting the client hello.
		Sending_Identify = 3,///< Sending the identify payload.
		Collecting_Ready = 4,///< Collecting the client ready.
		Initializing_DatagramSocket = 5,///< Initializing the datagram udp socket.
		Sending_Select_Protocol = 6,///< Sending the select-protocol payload.
		Collecting_Session_Description = 7///< Collecting the session-description payload.
	};

	/// @brief For the various active states of the VoiceConnection class.
	enum class VoiceActiveState : int8_t {
		Connecting = 0,///< Connecting.
		Playing = 1,///< Playing.
		Stopped = 2,///< Stopped.
		Paused = 3,///< Paused.
		Exiting = 4//< Exiting.
	};

	class DiscordCoreAPI_Dll VoiceConnectionBridge : public DiscordCoreInternal::UDPConnection {
	  public:
		friend class VoiceConnection;

		VoiceConnectionBridge(DiscordCoreClient* voiceConnectionNew, std::basic_string<uint8_t>& encryptionKeyNew, StreamType streamType,
			const std::string& baseUrlNew, const uint16_t portNew, Snowflake guildIdNew,
			std::coroutine_handle<DiscordCoreAPI::CoRoutine<void, false>::promise_type>* tokenNew);

		inline void applyGainRamp(int64_t sampleCount);

		void parseOutgoingVoiceData();

		void handleAudioBuffer() override;

		void mixAudio();

		void disconnect() override;

	  protected:
		std::coroutine_handle<DiscordCoreAPI::CoRoutine<void, false>::promise_type>* token{};
		std::array<opus_int16, 23040> downSampledVector{};
		std::basic_string<uint8_t> decryptedDataString{};
		std::array<opus_int32, 23040> upSampledVector{};
		std::basic_string<uint8_t> encryptionKey{};
		MovingAverager voiceUserCountAverage{ 25 };
		DiscordCoreClient* discordCoreClient{};
		std::vector<uint8_t> resampleVector{};
		Snowflake guildId{};
		float currentGain{};
		float increment{};
		float endGain{};
	};

	class DiscordCoreAPI_Dll VoiceUDPConnection : public DiscordCoreInternal::UDPConnection {
	  public:
		VoiceUDPConnection() = default;

		VoiceUDPConnection(const std::string& baseUrlNew, uint16_t portNew, StreamType streamType, VoiceConnection* ptrNew,
			std::coroutine_handle<DiscordCoreAPI::CoRoutine<void, false>::promise_type>* stopToken);

		void handleAudioBuffer() override;

		void disconnect() override;

	  protected:
		VoiceConnection* voiceConnection{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// @brief VoiceConnection class - represents the connection to a given voice ChannelData.
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::WebSocketCore {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class VoiceConnectionBridge;
		friend class VoiceUDPConnection;
		friend class DiscordCoreClient;
		friend class GuildCacheData;
		friend class GuildData;
		friend class SongAPI;

		/// The constructor.
		/// @param discordCoreClientNew A pointer to the main isntance of DiscordCoreClient.
		/// @param baseShardNew A pointer to the base shard that this voice connection belongs to.
		/// @param doWeQuitNew A pointer to the global signalling boolean for exiting the application.
		VoiceConnection(DiscordCoreClient* discordCoreClientNew, DiscordCoreInternal::WebSocketClient* baseShardNew, std::atomic_bool* doWeQuitNew);

		bool areWeConnected();

		/// @brief Collects the currently connected-to voice ChannelData's id.
		/// @returns Snowflake A Snowflake containing the ChannelData's id.
		Snowflake getChannelId();

		/// @brief Connects to a currently held voice channel.
		/// @param initData A DiscordCoerAPI::VoiceConnectInitDat structure.
		void connect(const VoiceConnectInitData& initData);

		~VoiceConnection() = default;

	  protected:
		std::atomic<VoiceConnectionState> connectionState{ VoiceConnectionState::Collecting_Init_Data };
		UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		Nanoseconds intervalCount{ static_cast<int64_t>(960.0l / 48000.0l * 1000000000.0l) };
		std::coroutine_handle<DiscordCoreAPI::CoRoutine<void, false>::promise_type> token{};
		std::atomic<VoiceActiveState> prevActiveState{ VoiceActiveState::Stopped };
		std::atomic<VoiceActiveState> activeState{ VoiceActiveState::Connecting };
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		UnorderedMap<uint64_t, UniquePtr<VoiceUser>> voiceUsers{};
		DiscordCoreInternal::OpusEncoderWrapper encoder{};
		DiscordCoreInternal::WebSocketClient* baseShard{};
		UniquePtr<VoiceConnectionBridge> streamSocket{};
		VoiceConnectInitData voiceConnectInitData{};
		std::basic_string<uint8_t> encryptionKey{};
		DiscordCoreClient* discordCoreClient{};
		int64_t sampleRatePerSecond{ 48000 };
		RTPPacketEncrypter packetEncrypter{};
		CoRoutine<void, false> taskThread{};
		VoiceUDPConnection udpConnection{};
		int64_t nsPerSecond{ 1000000000 };
		std::string audioEncryptionMode{};
		AudioFrameData xferAudioData{};
		std::atomic_bool wasItAFail{};
		std::atomic_bool* doWeQuit{};
		std::atomic_bool doWeSkip{};
		int64_t samplesPerPacket{};
		std::string externalIp{};
		int64_t msPerPacket{};
		std::string voiceIp{};
		std::string baseUrl{};
		uint32_t audioSSRC{};
		uint16_t port{};

		void parseIncomingVoiceData(std::basic_string_view<uint8_t> rawDataBufferNew);

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer();

		void skipInternal(uint32_t currentRecursionDepth = 0);

		void checkForAndSendHeartBeat(const bool isImmedate);

		void sendSpeakingMessage(const bool isSpeaking);

		bool onMessageReceived(std::string_view data);

		CoRoutine<void, false> runVoice();

		void sendVoiceConnectionData();

		bool areWeCurrentlyPlaying();

		bool skip(bool wasItAFail);

		void connectInternal();

		bool voiceConnect();

		void sendSilence();

		bool pauseToggle();

		void disconnect();

		void reconnect();

		void onClosed();

		bool stop();

		bool play();
	};
	/**@}*/

};
