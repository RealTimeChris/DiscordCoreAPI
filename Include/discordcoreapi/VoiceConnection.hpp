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
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>
#include <sodium.h>

namespace DiscordCoreAPI {

	struct OpusDecoderWrapper {
		struct OpusDecoderDeleter {
			void operator()(OpusDecoder*) noexcept;
		};

		OpusDecoderWrapper& operator=(OpusDecoderWrapper&&) noexcept;

		OpusDecoderWrapper(OpusDecoderWrapper&&) noexcept;

		OpusDecoderWrapper();

		operator OpusDecoder*() noexcept;

	  protected:
		std::unique_ptr<OpusDecoder, OpusDecoderDeleter> thePtr{ nullptr, OpusDecoderDeleter{} };
	};

	struct VoicePayload {
		std::vector<opus_int16> decodedData{};
		std::vector<uint8_t> theRawData{};
	};

	struct VoiceUser {
		std::queue<VoicePayload> thePayloads{};
		OpusDecoderWrapper theDecoder{};
		OpusEncoderWrapper theEncoder{};
		Snowflake theUserId{};
	};

	using DoubleNanoSecond = std::chrono::duration<double, std::nano>;

	using DoubleMilliSecond = std::chrono::duration<double, std::milli>;

	using DoubleTimePointNs = std::chrono::time_point<std::chrono::system_clock, DoubleNanoSecond>;

	using DoubleTimePointMs = std::chrono::time_point<std::chrono::system_clock, DoubleMilliSecond>;

	struct RTPPacket {
		std::vector<uint8_t> audioData{};
		uint8_t version{ 0x80 };
		uint8_t flags{ 0x78 };
		std::string theKeys{};
		uint32_t timestamp{};
		uint16_t sequence{};
		uint32_t ssrc{};

		RTPPacket(uint32_t timestampNew, uint16_t sequenceNew, uint32_t ssrcNew, std::vector<uint8_t>& audioDataNew, const std::string& theKeys) noexcept;

		operator std::string() noexcept;
	};

	/// For the various connection states of the VoiceConnection class. \brief For the various connection states of the VoiceConnection class.
	enum class VoiceConnectionState : int8_t {
		Collecting_Init_Data = 0,///< Collecting initialization data.
		Initializing_WebSocket = 1,///< Initializing the WebSocket.
		Collecting_Hello = 2,///< Collecting the client hello.
		Sending_Identify = 3,///< Sending the identify payload.
		Collecting_Ready = 4,///< Collecting the client ready.
		Initializing_DatagramSocket = 5,///< Initializing the datagram udp socket.
		Sending_Select_Protocol = 7,///< Sending the select-protocol payload.
		Collecting_Session_Description = 8///< Collecting the session-description payload.
	};

	/// For the various active states of the VoiceConnection class. \brief For the various active states of the VoiceConnection class.
	enum class VoiceActiveState : int8_t {
		Connecting = -1,///< Connecting - it hasn't started or it's reconnecting.
		Playing = 1,///< Playing.
		Stopped = 2,///< Stopped.
		Paused = 3,///< Paused.
		Exiting = 4///< Exiting.
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// VoiceConnection class - represents the connection to a given voice Channel. \brief VoiceConnection class - represents the connection to a given voice Channel.
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::WebSocketSSLShard, public DiscordCoreInternal::DatagramSocketClient {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class DiscordCoreClient;
		friend class GuildData;
		friend class SongAPI;

		VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, DiscordCoreInternal::WebSocketSSLShard* baseShard,
			const DiscordCoreInternal::VoiceConnectInitData& initDataNew, DiscordCoreAPI::ConfigManager* configManagerNew, std::atomic_bool* doWeQuitNew, StreamType streamTypeNew,
			StreamInfo streamInfoNew = StreamInfo{}) noexcept;

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns DiscordCoreAPI::Snowflake A Snowflake containing the Channel's id.
		Snowflake getChannelId() noexcept;

		~VoiceConnection() noexcept;

	  protected:
		std::atomic<VoiceConnectionState> connectionState{ VoiceConnectionState::Collecting_Init_Data };
		UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::atomic<VoiceActiveState> lastActiveState{ VoiceActiveState::Connecting };
		std::unique_ptr<DiscordCoreInternal::DatagramSocketClient> streamSocket{};
		std::atomic<VoiceActiveState> activeState{ VoiceActiveState::Connecting };
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		DiscordCoreInternal::WebSocketSSLShard* baseShard{ nullptr };
		UnboundedMessageBlock<AudioFrameData> audioDataBuffer{};
		std::unique_ptr<std::jthread> taskThread01{ nullptr };
		std::unique_ptr<std::jthread> taskThread02{ nullptr };
		std::unique_ptr<std::jthread> taskThread03{ nullptr };
		std::unordered_map<uint64_t, VoiceUser> voiceUsers{};
		std::deque<ConnectionPackage> theConnections{};
		std::atomic_bool areWeConnectedBool{ false };
		std::deque<VoicePayload> theFrameQueue{};
		std::atomic_bool areWePlaying{ false };
		const int64_t maxReconnectTries{ 10 };
		int64_t currentReconnectTries{ 0 };
		std::string audioEncryptionMode{};
		Snowflake currentGuildMemberId{};
		ConnectionPackage thePackage{};
		std::string secretKeySend{};
		std::mutex voiceUserMutex{};
		uint16_t sequenceIndex{ 0 };
		AudioFrameData audioData{};
		StreamInfo theStreamInfo{};
		AudioEncoder theEncoder{};
		std::string externalIp{};
		StreamType streamType{};
		uint32_t timeStamp{ 0 };
		std::string voiceIp{};
		std::string baseUrl{};
		uint32_t audioSSRC{};
		std::string port{};

		bool collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept;

		std::string encryptSingleAudioFrame(AudioFrameData& bufferToSend) noexcept;

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer() noexcept;

		bool onMessageReceived(const std::string& theData) noexcept;

		void sendSingleFrame(AudioFrameData& frameData) noexcept;

		void sendSpeakingMessage(const bool isSpeaking) noexcept;

		void sendVoiceData(std::string& responseData) noexcept;

		bool handleBuffer(SSLClient* theClient) noexcept;

		void runWebSocket(std::stop_token) noexcept;

		void runBridge(std::stop_token) noexcept;

		void runVoice(std::stop_token) noexcept;

		void parseIncomingVoiceData() noexcept;

		bool areWeCurrentlyPlaying() noexcept;

		void disconnectInternal() noexcept;

		void reconnectStream() noexcept;

		void connectInternal() noexcept;

		void clearAudioData() noexcept;

		bool areWeConnected() noexcept;

		void sendHeartBeat() noexcept;

		bool voiceConnect() noexcept;

		void sendSilence() noexcept;

		void pauseToggle() noexcept;

		void disconnect() noexcept;

		void reconnect() noexcept;

		void onClosed() noexcept;

		void mixAudio() noexcept;

		void connect() noexcept;

		bool stop() noexcept;

		bool play() noexcept;
	};
	/**@}*/

};// namespace DiscordCoreAPI
