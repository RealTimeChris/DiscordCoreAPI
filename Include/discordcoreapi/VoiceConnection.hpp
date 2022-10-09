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
	#include <discordcoreapi/CoRoutine.hpp>
	#include <discordcoreapi/WebSocketEntities.hpp>
	#include <sodium.h>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll VoiceSocketReadyData {
		VoiceSocketReadyData(simdjson::ondemand::value);
		String mode{};
		String ip{};
		Uint64 port{};
		Uint32 ssrc{};
	};

	struct DiscordCoreAPI_Dll VoiceResumeData {
		Snowflake serverId{};
		String sessionId{};
		String token{};

		operator JsonObject();
	};

	struct DiscordCoreAPI_Dll SessionDescriptionData {
		String theKey{};
	};

	struct DecodeData {
		Vector<opus_int16> theData{};
		Uint32 sampleCount{};
	};

	struct DiscordCoreAPI_Dll OpusDecoderWrapper {
		struct DiscordCoreAPI_Dll OpusDecoderDeleter {
			Void operator()(OpusDecoder*) noexcept;
		};

		OpusDecoderWrapper& operator=(OpusDecoderWrapper&&) noexcept;

		OpusDecoderWrapper(OpusDecoderWrapper&&) noexcept;

		OpusDecoderWrapper();

		DecodeData decodeData(Vector<Uint8>&& dataToDecode);

		operator OpusDecoder*() noexcept;

	  protected:
		UniquePtrD<OpusDecoder, OpusDecoderDeleter> thePtr{ nullptr, OpusDecoderDeleter{} };
	};

	struct DiscordCoreAPI_Dll VoicePayload {
		Vector<opus_int16> decodedData{};
		Vector<Uint8> theRawData{};
	};

	struct DiscordCoreAPI_Dll VoiceUser {
		std::queue<VoicePayload> thePayloads{};
		OpusDecoderWrapper theDecoder{};
		OpusEncoderWrapper theEncoder{};
		Snowflake theUserId{};
	};

	using DoubleNanoSecond = std::chrono::duration<Double, std::nano>;

	using DoubleMilliSecond = std::chrono::duration<Double, std::milli>;

	using DoubleTimePointNs = std::chrono::time_point<std::chrono::steady_clock, DoubleNanoSecond>;

	using DoubleTimePointMs = std::chrono::time_point<std::chrono::steady_clock, DoubleMilliSecond>;

	struct DiscordCoreAPI_Dll RTPPacketEncrypter {
		Vector<unsigned char> theKeys{};
		Uint8 version{ 0x80 };
		Uint8 flags{ 0x78 };
		Uint32 timeStamp{};
		Uint16 sequence{};
		String theData{};
		Uint32 ssrc{};

		RTPPacketEncrypter() noexcept = default;

		RTPPacketEncrypter(Uint32 ssrcNew, const Vector<unsigned char>& theKeysNew) noexcept;

		StringView encryptPacket(AudioFrameData& audioDataNew) noexcept;
	};

	/// For the various connection states of the VoiceConnection class. \brief For the various connection states of the VoiceConnection class.
	enum class VoiceConnectionState : Int8 {
		Collecting_Init_Data = 0,///< Collecting initialization data.
		Initializing_WebSocket = 1,///< Initializing the WebSocket.
		Collecting_Hello = 2,///< Collecting the client hello.
		Sending_Identify = 3,///< Sending the identify payload.
		Sending_Resume = 4,///< Sending the resume voice-connection payload.
		Collecting_Resumed = 6,///< Collecting the client ready.
		Collecting_Ready = 7,///< Collecting the client ready.
		Initializing_DatagramSocket = 8,///< Initializing the datagram udp socket.
		Sending_Select_Protocol = 9,///< Sending the select-protocol payload.
		Collecting_Session_Description = 10///< Collecting the session-description payload.
	};

	/// For the various active states of the VoiceConnection class. \brief For the various active states of the VoiceConnection class.
	enum class VoiceActiveState : Int8 {
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
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::WebSocketCore, public DiscordCoreInternal::DatagramSocketClient {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class DiscordCoreClient;
		friend class GuildData;
		friend class SongAPI;

		VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, DiscordCoreInternal::WebSocketSSLShard* baseShard,
			const DiscordCoreInternal::VoiceConnectInitData& initDataNew, DiscordCoreAPI::ConfigManager* configManagerNew, AtomicBool* doWeQuitNew, StreamType streamTypeNew,
			StreamInfo streamInfoNew = StreamInfo{}) noexcept;

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns DiscordCoreAPI::Snowflake A Snowflake containing the Channel's id.
		Snowflake getChannelId() noexcept;

		~VoiceConnection() noexcept;

	  protected:
		UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		Atomic<VoiceConnectionState> connectionState{ VoiceConnectionState::Collecting_Init_Data };
		Atomic<VoiceActiveState> activeState{ VoiceActiveState::Connecting };
		UniquePtr<DiscordCoreInternal::DatagramSocketClient> streamSocket{};
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		DiscordCoreInternal::WebSocketSSLShard* baseShard{ nullptr };
		UniquePtr<std::jthread> taskThread01{ nullptr };
		UniquePtr<std::jthread> taskThread02{ nullptr };
		DiscordCoreClient* discordCoreClient{ nullptr };
		simdjson::ondemand::parser theParser{};
		Vector<unsigned char> secretKeySend{};
		UMap<Uint64, VoiceUser> voiceUsers{};
		RTPPacketEncrypter packetEncrypter{};
		Deque<VoicePayload> theFrameQueue{};
		AtomicBool areWePlaying{ false };
		Snowflake currentGuildMemberId{};
		AtomicBool* doWeQuit{ nullptr };
		ConnectionPackage thePackage{};
		String audioEncryptionMode{};
		std::mutex voiceUserMutex{};
		AudioFrameData audioData{};
		StreamInfo theStreamInfo{};
		Uint16 sequenceIndex{ 0 };
		AudioEncoder theEncoder{};
		StreamType streamType{};
		String externalIp{};
		Uint32 audioSSRC{};
		String voiceIp{};
		String baseUrl{};
		Uint64 port{};

		Bool collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept;

		StringView encryptSingleAudioFrame(AudioFrameData& bufferToSend) noexcept;

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer() noexcept;

		Void connect(DiscordCoreInternal::VoiceConnectInitData) noexcept;

		Void sendSpeakingMessage(const Bool isSpeaking) noexcept;

		Void sendSingleFrame(AudioFrameData& frameData) noexcept;

		Void sendVoiceData(StringView responseData) noexcept;

		Bool onMessageReceived(StringView theData) noexcept;

		Void runBridge(std::stop_token) noexcept;

		Void runVoice(std::stop_token) noexcept;

		Void parseIncomingVoiceData() noexcept;

		Bool areWeCurrentlyPlaying() noexcept;

		Void reconnectStream() noexcept;

		Void connect() noexcept;

		Void clearAudioData() noexcept;

		Bool areWeConnected() noexcept;

		Bool voiceConnect() noexcept;

		Void sendSilence() noexcept;

		Void pauseToggle() noexcept;

		Void checkForConnections();

		Void disconnect() noexcept;

		Void reconnect() noexcept;

		Void onClosed() noexcept;

		Void mixAudio() noexcept;

		Bool stop() noexcept;

		Bool play() noexcept;
	};
	/**@}*/

};// namespace DiscordCoreAPI
#endif