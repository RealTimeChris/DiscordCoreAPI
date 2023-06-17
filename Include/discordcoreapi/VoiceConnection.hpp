/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// VoiceConnection.hpp - Header for the voice connection class.
/// Jul 15, 2021
/// https://discordcoreapi.com
/// \file VoiceConnection.hpp

#pragma once

#include <discordcoreapi/Utilities/AudioEncoder.hpp>
#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/Utilities/RingBuffer.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>
#include <discordcoreapi/UDPConnection.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <sodium.h>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll VoiceSocketReadyData {
		std::vector<std::string> modes{};
		std::string ip{};
		uint64_t port{};
		uint32_t ssrc{};
	};

	struct DiscordCoreAPI_Dll VoiceSessionDescriptionData {
		std::vector<uint8_t> secretKey{};
	};

	struct DiscordCoreAPI_Dll SpeakingData {
		Snowflake userId{};
		uint32_t ssrc{};
	};

	struct DiscordCoreAPI_Dll VoiceConnectionHelloData {
		uint32_t heartBeatInterval{};
	};

	struct DiscordCoreAPI_Dll VoiceUserDisconnectData {
		Snowflake userId{};
	};

	struct DiscordCoreAPI_Dll VoiceUser {
		VoiceUser() noexcept = default;

		VoiceUser(Snowflake userId) noexcept;

		VoiceUser& operator=(VoiceUser&&) noexcept;

		VoiceUser& operator=(const VoiceUser&) noexcept = delete;

		VoiceUser(const VoiceUser&) noexcept = delete;

		DiscordCoreInternal::OpusDecoderWrapper& getDecoder() noexcept;

		std::basic_string_view<uint8_t> extractPayload() noexcept;

		void insertPayload(std::basic_string_view<uint8_t>) noexcept;

		Snowflake getUserId() noexcept;

	  protected:
		DiscordCoreInternal::RingBuffer<uint8_t, 4> payloads{};
		DiscordCoreInternal::OpusDecoderWrapper decoder{};
		Snowflake userId{};
	};

	struct DiscordCoreAPI_Dll RTPPacketEncrypter {
		RTPPacketEncrypter() noexcept = default;

		RTPPacketEncrypter(uint32_t ssrcNew, const std::basic_string<uint8_t>& keysNew) noexcept;

		std::basic_string_view<uint8_t> encryptPacket(DiscordCoreInternal::EncoderReturnData& audioData) noexcept;

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
		MovingAverager(size_t collectionCountNew) noexcept;

		MovingAverager operator+=(int64_t value) noexcept;

		operator float() noexcept;

	  protected:
		std::deque<int64_t> values{};
		size_t collectionCount{};
	};

	/// \brief The various opcodes that could be sent/received by the voice-websocket.
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

	/// \brief For the various connection states of the VoiceConnection class.
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

	/// \brief For the various active states of the VoiceConnection class.
	enum class VoiceActiveState : int8_t {
		Connecting = -1,///< Connecting - it hasn't started or it's reconnecting.
		Playing = 1,///< Playing.
		Stopped = 2,///< Stopped.
		Paused = 3,///< Paused.
		Exiting = 4///< Exiting.
	};

	class DiscordCoreAPI_Dll VoiceConnectionBridge : public DiscordCoreInternal::UDPConnection {
	  public:
		friend class DiscordCoreAPI_Dll VoiceConnection;

		VoiceConnectionBridge(DiscordCoreClient* voiceConnectionNew, std::basic_string<uint8_t>& encryptionKeyNew, StreamType streamType,
			const std::string& baseUrlNew, const uint16_t portNew, Snowflake guildIdNew, std::stop_token* tokenNew);

		inline void collectEightElements(opus_int32* dataIn, opus_int16* dataOut) noexcept;

		inline void applyGainRamp(int64_t sampleCount) noexcept;

		void parseOutgoingVoiceData() noexcept;

		void handleAudioBuffer() noexcept;

		void mixAudio() noexcept;

	  protected:
		std::array<opus_int16, 23040> downSampledVector{};
		std::basic_string<uint8_t> decryptedDataString{};
		std::array<opus_int32, 23040> upSampledVector{};
		std::basic_string<uint8_t> encryptionKey{};
		MovingAverager voiceUserCountAverage{ 25 };
		DiscordCoreClient* discordCoreClient{};
		std::stop_token* token{};
		Snowflake guildId{};
		float currentGain{};
		float increment{};
		float endGain{};
	};

	class DiscordCoreAPI_Dll VoiceUDPConnection : public DiscordCoreInternal::UDPConnection {
	  public:
		VoiceUDPConnection() noexcept = default;

		VoiceUDPConnection(const std::string& baseUrlNew, uint16_t portNew, DiscordCoreAPI::StreamType streamType, VoiceConnection* ptrNew);

		void handleAudioBuffer() noexcept;

	  protected:
		VoiceConnection* voiceConnection{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// \brief VoiceConnection class DiscordCoreAPI_Dll - represents the connection to a given voice Channel.
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::WebSocketCore {
	  public:
		friend class DiscordCoreAPI_Dll DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreAPI_Dll DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreAPI_Dll DiscordCoreInternal::YouTubeAPI;
		friend class DiscordCoreAPI_Dll VoiceConnectionBridge;
		friend class DiscordCoreAPI_Dll VoiceUDPConnection;
		friend class DiscordCoreAPI_Dll DiscordCoreClient;
		friend class DiscordCoreAPI_Dll GuildData;
		friend class DiscordCoreAPI_Dll SongAPI;

		/// The constructor.
		VoiceConnection(DiscordCoreClient* discordCoreClientNew, DiscordCoreInternal::WebSocketClient* baseShardNew,
			std::atomic_bool* doWeQuitNew) noexcept;

		bool areWeConnected() noexcept;

		/// \brief Collects the currently connected-to voice Channel's id.
		/// \returns Snowflake A Snowflake containing the Channel's id.
		Snowflake getChannelId() noexcept;

		/// \brief Connects to a currently held voice channel.
		/// \param initData A DiscordCoerAPI::VoiceConnectInitDat structure.
		void connect(const VoiceConnectInitData& initData) noexcept;

		~VoiceConnection() noexcept;

	  protected:
		std::atomic<VoiceConnectionState> connectionState{ VoiceConnectionState::Collecting_Init_Data };
		UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		Nanoseconds intervalCount{ static_cast<int64_t>(960.0l / 48000.0l * 1000000000.0l) };
		std::atomic<VoiceActiveState> activeState{ VoiceActiveState::Connecting };
		std::unordered_map<uint64_t, UniquePtr<VoiceUser>> voiceUsers{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		DiscordCoreInternal::OpusEncoderWrapper encoder{};
		DiscordCoreInternal::WebSocketClient* baseShard{};
		UniquePtr<VoiceConnectionBridge> streamSocket{};
		VoiceConnectInitData voiceConnectInitData{};
		std::basic_string<uint8_t> encryptionKey{};
		DiscordCoreClient* discordCoreClient{};
		int64_t sampleRatePerSecond{ 48000 };
		RTPPacketEncrypter packetEncrypter{};
		UniquePtr<std::jthread> taskThread{};
		std::atomic_bool areWeStillActive{};
		VoiceUDPConnection udpConnection{};
		UniquePtr<std::stop_token> token{};
		int64_t nsPerSecond{ 1000000000 };
		std::string audioEncryptionMode{};
		std::atomic_bool areWePlaying{};
		std::atomic_bool doWeSkipNow{};
		AudioFrameData xferAudioData{};
		std::atomic_bool* doWeQuit{};
		std::atomic_bool doWeSkip{};
		int64_t samplesPerPacket{};
		std::string externalIp{};
		int64_t msPerPacket{};
		std::string voiceIp{};
		std::string baseUrl{};
		uint32_t audioSSRC{};
		uint16_t port{};

		void parseIncomingVoiceData(std::basic_string_view<uint8_t> rawDataBufferNew) noexcept;

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer() noexcept;

		void checkForAndSendHeartBeat(const bool isImmedate) noexcept;

		void sendSpeakingMessage(const bool isSpeaking) noexcept;

		bool onMessageReceived(std::string_view data) noexcept;

		void sendVoiceConnectionData() noexcept;

		bool areWeCurrentlyPlaying() noexcept;

		void checkForConnections() noexcept;

		void connectInternal() noexcept;

		void skipInternal() noexcept;

		bool voiceConnect() noexcept;

		void sendSilence() noexcept;

		void pauseToggle() noexcept;

		void disconnect() noexcept;

		void reconnect() noexcept;

		void runVoice() noexcept;

		void onClosed() noexcept;

		void skip() noexcept;

		bool stop() noexcept;

		bool play() noexcept;
	};
	/**@}*/

};
