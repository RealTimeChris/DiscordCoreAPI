/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>
#include <sodium.h>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll VoiceSocketReadyData {
		VoiceSocketReadyData(simdjson::ondemand::value);
		std::string mode{};
		std::string ip{};
		uint64_t port{};
		uint32_t ssrc{};
	};

	struct DiscordCoreAPI_Dll VoiceUser {
		VoiceUser() noexcept = default;

		VoiceUser(std::atomic_int8_t* voiceUserCount) noexcept;

		VoiceUser& operator=(VoiceUser&&) noexcept;

		VoiceUser(VoiceUser&&) noexcept;

		VoiceUser& operator=(const VoiceUser&) noexcept = delete;

		VoiceUser(const VoiceUser&) noexcept = delete;

		DiscordCoreInternal::OpusDecoderWrapper& getDecoder() noexcept;

		void insertPayload(std::string_view) noexcept;

		std::string_view extractPayload() noexcept;

		void setUserId(Snowflake) noexcept;

		Snowflake getUserId() noexcept;

	  protected:
		DiscordCoreInternal::RingBuffer<char, 4> payloads{};
		DiscordCoreInternal::OpusDecoderWrapper decoder{};
		std::atomic_int8_t* voiceUserCount{ nullptr };
		Snowflake userId{};
	};

	struct DiscordCoreAPI_Dll RTPPacketEncrypter {
		RTPPacketEncrypter() noexcept = default;

		RTPPacketEncrypter(uint32_t ssrcNew, const std::string& keysNew) noexcept;

		std::string_view encryptPacket(const AudioFrameData& audioData) noexcept;

	  protected:
		uint8_t version{ 0x80 };
		uint8_t flags{ 0x78 };
		uint32_t timeStamp{};
		uint16_t sequence{};
		std::string keys{};
		std::string data{};
		uint32_t ssrc{};
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
		VoiceConnectionBridge(DiscordCoreClient* voiceConnectionPtrNew, StreamType streamType, Snowflake guildIdNew);

		void parseOutgoingVoiceData() noexcept;

		void handleAudioBuffer() noexcept;

	  protected:
		DiscordCoreClient* clientPtr{ nullptr };
		Snowflake guildId{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// \brief VoiceConnection class - represents the connection to a given voice Channel.
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::WebSocketCore, public DiscordCoreInternal::UDPConnection {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class VoiceConnectionBridge;
		friend class DiscordCoreClient;
		friend class GuildData;
		friend class SongAPI;

		/// The constructor.
		VoiceConnection(DiscordCoreClient* clientPtrNew, DiscordCoreInternal::WebSocketClient* baseShardNew, std::atomic_bool* doWeQuitNew) noexcept;

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
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		std::unique_ptr<VoiceConnectionBridge> streamSocket{ nullptr };
		DiscordCoreInternal::WebSocketClient* baseShard{ nullptr };
		std::unique_ptr<std::jthread> taskThread01{ nullptr };
		std::unordered_map<uint64_t, VoiceUser> voiceUsers{};
		DiscordCoreInternal::OpusEncoderWrapper encoder{};
		DiscordCoreClient* discordCoreClient{ nullptr };
		VoiceConnectInitData voiceConnectInitData{};
		std::vector<opus_int16> downSampledVector{};
		std::vector<opus_int32> upSampledVector{};
		std::atomic_bool areWePlaying{ false };
		std::atomic_bool* doWeQuit{ nullptr };
		int64_t sampleRatePerSecond{ 48000 };
		RTPPacketEncrypter packetEncrypter{};
		simdjson::ondemand::parser parser{};
		std::atomic_int8_t voiceUserCount{};
		int64_t nsPerSecond{ 1000000000 };
		std::string audioEncryptionMode{};
		std::string decryptedDataString{};
		Snowflake currentGuildMemberId{};
		std::string encryptionKey{};
		AudioFrameData audioData{};
		std::string externalIp{};
		std::string voiceIp{};
		std::string baseUrl{};
		uint32_t audioSSRC{};
		uint16_t port{};

		void parseIncomingVoiceData(std::string_view rawDataBufferNew) noexcept;

		void sendVoiceData(std::string_view responseData) noexcept;

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer() noexcept;

		void checkForAndSendHeartBeat(const bool isItImmediage) noexcept;

		bool onMessageReceived(std::string_view data) noexcept;

		void sendSingleFrame(AudioFrameData& frameData) noexcept;

		void sendSpeakingMessage(const bool isSpeaking) noexcept;

		void checkForConnections(std::stop_token token) noexcept;

		void connectInternal(std::stop_token token) noexcept;

		void runVoice(std::stop_token) noexcept;

		bool areWeCurrentlyPlaying() noexcept;

		void cancelBridgeThread() noexcept;

		void handleAudioBuffer() noexcept;

		void cancelMainThread() noexcept;

		void clearAudioData() noexcept;

		bool areWeConnected() noexcept;

		bool voiceConnect() noexcept;

		void sendSilence() noexcept;

		void pauseToggle() noexcept;

		void disconnect() noexcept;

		void reconnect() noexcept;

		void onClosed() noexcept;

		void mixAudio() noexcept;

		bool stop() noexcept;

		bool play() noexcept;
	};
	/**@}*/

};// namespace DiscordCoreAPI