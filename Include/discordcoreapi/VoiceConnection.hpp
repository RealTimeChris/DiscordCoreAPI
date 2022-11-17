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

#ifndef VOICE_CONNECTION
	#define VOICE_CONNECTION

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

		VoiceUser(std::atomic_int8_t* voiceUserCount, std::atomic<Nanoseconds>* leftOverVoiceTime) noexcept;

		VoiceUser& operator=(VoiceUser&&) noexcept;

		VoiceUser(VoiceUser&&) noexcept = default;

		VoiceUser& operator=(const VoiceUser&) noexcept = delete;

		VoiceUser(const VoiceUser&) noexcept = delete;

		DiscordCoreInternal::OpusDecoderWrapper& getDecoder();

		void insertPayload(std::string&&);

		std::string extractPayload();

		void setEndingStatus(bool);

		void setUserId(Snowflake);

		bool getEndingStatus();

		Snowflake getUserId();

	  protected:
		DiscordCoreInternal::OpusDecoderWrapper decoder{};
		UnboundedMessageBlock<std::string> payloads{};
		std::atomic<Nanoseconds>* leftOverVoiceTime{};
		std::atomic_bool wereWeEnding{ false };
		std::atomic_int8_t* voiceUserCount{};
		Snowflake userId{};
	};

	using TimePoint = std::chrono::time_point<HRClock, Nanoseconds>;

	struct DiscordCoreAPI_Dll RTPPacketEncrypter {
		RTPPacketEncrypter() noexcept = default;

		RTPPacketEncrypter(uint32_t ssrcNew, const std::string& keysNew) noexcept;

		std::string_view encryptPacket(const AudioFrameData& audioData) noexcept;

	  protected:
		std::string keys{};
		std::string data{};
		uint8_t version{ 0x80 };
		uint8_t flags{ 0x78 };
		uint32_t timeStamp{};
		uint16_t sequence{};
		uint32_t ssrc{};
	};

	/// For the varuious WebSocket messages. \brief For the varuious WebSocket messages.
	enum class OnMessageReceivedTypes {
		Ready = 2,///< Complete the websocket handshake.
		Session_Description = 4,///< Describe the session.
		Speaking = 5,///< Indicate which users are speaking.
		Heartbeat_ACK = 6,///< Sent to acknowledge a received client heartbeat.
		Hello = 8,///< Time to wait between sending heartbeats in milliseconds.
		Resumed = 9,///< Acknowledge a successful session resume.
		Client_Disconnect = 13///< A client has disconnected from the voice channel.
	};

	/// For the various connection states of the VoiceConnection class. \brief For the various connection states of the VoiceConnection class.
	enum class VoiceConnectionState : int8_t {
		Collecting_Init_Data = 0,///< Collecting initialization data.
		Initializing_WebSocket = 1,///< Initializing the WebSocket.
		Collecting_Hello = 2,///< Collecting the client hello.
		Sending_Identify = 3,///< Sending the identify payload.
		Collecting_Ready = 4,///< Collecting the client ready.
		Initializing_DatagramSocket = 5,///< Initializing the datagram udp socket.
		Sending_Select_Protocol = 6,///< Sending the select-protocol payload.
		Collecting_Session_Description = 7///< Collecting the session-description payload.
	};

	/// For the various active states of the VoiceConnection class. \brief For the various active states of the VoiceConnection class.
	enum class VoiceActiveState : int8_t {
		Connecting = -1,///< Connecting - it hasn't started or it's reconnecting.
		Playing = 1,///< Playing.
		Stopped = 2,///< Stopped.
		Paused = 3,///< Paused.
		Exiting = 4///< Exiting.
	};

	class DiscordCoreAPI_Dll VoiceConnection;

	class DiscordCoreAPI_Dll VoiceConnectionBridge : public DiscordCoreInternal::DatagramSocketClient {
	  public:
		VoiceConnectionBridge(DiscordCoreClient* clientPtrNew, VoiceConnection* voiceConnectionPtrNew, StreamType streamType, Snowflake guildIdNew);

		void parseOutGoingVoiceData() noexcept;

		void handleAudioBuffer() noexcept;

	  protected:
		VoiceConnection* voiceConnectionPtr{ nullptr };
		DiscordCoreClient* clientPtr{ nullptr };
		Snowflake guildId{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// VoiceConnection class - represents the connection to a given voice Channel. \brief VoiceConnection class - represents the connection to a given voice Channel.
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::WebSocketCore, public DiscordCoreInternal::DatagramSocketClient {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class VoiceConnectionBridge;
		friend class DiscordCoreClient;
		friend class GuildData;
		friend class SongAPI;

		/// The constructor.
		VoiceConnection(DiscordCoreClient* clientPtrNew, DiscordCoreInternal::WebSocketSSLShard* baseShardNew,
			std::atomic_bool* doWeQuitNew) noexcept;

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns DiscordCoreAPI::Snowflake A Snowflake containing the Channel's id.
		Snowflake getChannelId() noexcept;

		~VoiceConnection() noexcept;

	  protected:
		std::atomic<VoiceConnectionState> connectionState{ VoiceConnectionState::Collecting_Init_Data };
		UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::atomic<VoiceActiveState> activeState{ VoiceActiveState::Connecting };
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		DiscordCoreInternal::WebSocketSSLShard* baseShard{ nullptr };
		std::unique_ptr<VoiceConnectionBridge> streamSocket{};
		std::unique_ptr<std::jthread> taskThread01{ nullptr };
		std::unique_ptr<std::jthread> taskThread02{ nullptr };
		std::unordered_map<uint64_t, VoiceUser> voiceUsers{};
		DiscordCoreClient* discordCoreClient{ nullptr };
		std::atomic<Nanoseconds> leftOverVoiceTime{};
		VoiceConnectInitData voiceConnectInitData{};
		std::vector<opus_int16> downSampledVector{};
		std::vector<opus_int32> upSampledVector{};
		std::atomic_bool canWeSendAudio{ true };
		std::atomic_bool areWePlaying{ false };
		std::atomic_bool* doWeQuit{ nullptr };
		RTPPacketEncrypter packetEncrypter{};
		simdjson::ondemand::parser parser{};
		std::atomic_int8_t voiceUserCount{};
		bool haveWeGottenSignaled{ false };
		std::string decryptedDataString{};
		std::string audioEncryptionMode{};
		Snowflake currentGuildMemberId{};
		OpusEncoderWrapper encoder{};
		std::mutex voiceUserMutex{};
		std::string encryptionKey{};
		AudioFrameData audioData{};
		std::string externalIp{};
		std::string voiceIp{};
		std::string baseUrl{};
		uint32_t audioSSRC{};
		uint64_t port{};

		void parseIncomingVoiceData(const std::string_view rawDataBufferNew) noexcept;

		void connect(const DiscordCoreAPI::VoiceConnectInitData& initData) noexcept;

		void sendVoiceData(const std::string_view responseData) noexcept;

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer() noexcept;

		void checkForAndSendHeartBeat(const bool isItImmediage) noexcept;

		bool onMessageReceived(const std::string_view data) noexcept;

		void sendSingleFrame(AudioFrameData& frameData) noexcept;

		void sendSpeakingMessage(const bool isSpeaking) noexcept;

		void runBridge(std::stop_token) noexcept;

		void runVoice(std::stop_token) noexcept;

		bool areWeCurrentlyPlaying() noexcept;

		void checkForConnections() noexcept;

		void handleAudioBuffer() noexcept;

		void connectInternal() noexcept;

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
#endif