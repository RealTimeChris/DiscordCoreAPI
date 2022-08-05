/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
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

	struct OpusEncoderWrapper {
		struct OpusEncoderDeleter {
			void operator()(OpusEncoder*);
		};

		OpusEncoderWrapper& operator=(OpusEncoderWrapper&&);

		OpusEncoderWrapper(OpusEncoderWrapper&&);

		OpusEncoderWrapper();

		operator OpusEncoder*();

	  protected:
		std::unique_ptr<OpusEncoder, OpusEncoderDeleter> thePtr{ nullptr, OpusEncoderDeleter{} };
	};

	struct OpusDecoderWrapper {
		struct OpusDecoderDeleter {
			void operator()(OpusDecoder*);
		};

		OpusDecoderWrapper& operator=(OpusDecoderWrapper&&);

		OpusDecoderWrapper(OpusDecoderWrapper&&);

		OpusDecoderWrapper();

		operator OpusDecoder*();

	  protected:
		std::unique_ptr<OpusDecoder, OpusDecoderDeleter> thePtr{ nullptr, OpusDecoderDeleter{} };
	};

	struct VoicePayload {
		std::vector<opus_int16> decodedData{};
		std::vector<uint8_t> theRawData{};
	};

	struct VoiceUser {
		UnboundedMessageBlock<std::vector<opus_int16>> theDecodedDataBuffer{};
		UnboundedMessageBlock<VoicePayload> thePayloadBuffer{};
		std::unique_ptr<std::jthread> theThread{};
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

		RTPPacket(uint32_t timestampNew, uint16_t sequenceNew, uint32_t ssrcNew, const std::vector<uint8_t>& audioDataNew, const std::string& theKeys);

		operator std::string();
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

		VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, const DiscordCoreInternal::VoiceConnectInitData& initDataNew,
			DiscordCoreAPI::ConfigManager* configManagerNew, std::atomic_bool* doWeQuitNew, StreamType streamTypeNew, StreamInfo streamInfoNew = StreamInfo{}) noexcept;

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
		std::unordered_map<uint32_t, std::unique_ptr<VoiceUser>> voiceUsers{};
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		DiscordCoreInternal::WebSocketSSLShard* baseShard{ nullptr };
		UnboundedMessageBlock<AudioFrameData> audioDataBuffer{};
		std::unique_ptr<std::jthread> taskThread01{ nullptr };
		std::unique_ptr<std::jthread> taskThread02{ nullptr };
		std::unique_ptr<std::jthread> taskThread03{ nullptr };
		std::atomic_bool areWeConnectedBool{ false };
		std::queue<ConnectionPackage> connections{};
		std::queue<std::string> theFrameQueue{};
		std::atomic_bool areWePlaying{ false };
		const int64_t maxReconnectTries{ 10 };
		int64_t currentReconnectTries{ 0 };
		std::string audioEncryptionMode{};
		Snowflake currentGuildMemberId{};
		OpusEncoderWrapper theEncoder{};
		int64_t heartbeatInterval{ 0 };
		std::mutex voiceUserMutex{};
		std::string secretKeySend{};
		uint16_t sequenceIndex{ 0 };
		AudioFrameData audioData{};
		StreamInfo theStreamInfo{};
		std::string externalIp{};
		StreamType streamType{};
		uint32_t timeStamp{ 0 };
		std::string voiceIp{};
		std::string baseUrl{};
		uint32_t audioSSRC{};
		std::string port{};

		std::string encryptSingleAudioFrame(const EncodedFrameData& bufferToSend) noexcept;

		bool collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept;

		void runVoiceThread(std::stop_token& theToken, VoiceUser* theUser) noexcept;

		void sendSingleAudioFrame(std::string& audioDataPacketNew) noexcept;

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer() noexcept;

		EncodedFrameData encodeSingleAudioFrame(RawFrameData&) noexcept;

		void sendSingleFrame(const AudioFrameData& frameData) noexcept;

		void sendVoiceData(const std::string& responseData) noexcept;

		void sendSpeakingMessage(const bool isSpeaking) noexcept;

		void runWebSocket(std::stop_token&) noexcept;

		void runBridge(std::stop_token&) noexcept;

		void runVoice(std::stop_token&) noexcept;

		void parseIncomingVoiceData() noexcept;

		bool areWeCurrentlyPlaying() noexcept;

		void disconnectInternal() noexcept;

		void onMessageReceived() noexcept;

		void reconnectStream() noexcept;

		void connectInternal() noexcept;

		void clearAudioData() noexcept;

		bool areWeConnected() noexcept;

		void sendHeartBeat() noexcept;

		void onClosedVoice() noexcept;

		bool voiceConnect() noexcept;

		void sendSilence() noexcept;

		void pauseToggle() noexcept;

		void disconnect() noexcept;

		void reconnect() noexcept;

		void mixAudio() noexcept;

		void connect() noexcept;

		bool stop() noexcept;

		bool play() noexcept;
	};
	/**@}*/

};// namespace DiscordCoreAPI
