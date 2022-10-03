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

	struct DiscordCoreAPI_Dll SessionDescriptionData {
		String theKey{};
	};

	struct DiscordCoreAPI_Dll OpusDecoderWrapper {
		struct DiscordCoreAPI_Dll OpusDecoderDeleter {
			void operator()(OpusDecoder*) noexcept;
		};

		OpusDecoderWrapper& operator=(OpusDecoderWrapper&&) noexcept;

		OpusDecoderWrapper(OpusDecoderWrapper&&) noexcept;

		OpusDecoderWrapper();

		operator OpusDecoder*() noexcept;

	  protected:
		std::unique_ptr<OpusDecoder, OpusDecoderDeleter> thePtr{ nullptr, OpusDecoderDeleter{} };
	};

	struct DiscordCoreAPI_Dll VoicePayload {
		std::vector<opus_int16> decodedData{};
		std::vector<Uint8> theRawData{};
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

	struct DiscordCoreAPI_Dll RTPPacket {
		std::vector<Uint8> audioData{};
		Uint8 version{ 0x80 };
		Uint8 flags{ 0x78 };
		String theKeys{};
		Uint32 timestamp{};
		Uint16 sequence{};
		Uint32 ssrc{};

		RTPPacket(Uint32 timestampNew, Uint16 sequenceNew, Uint32 ssrcNew, std::vector<Uint8>& audioDataNew, const String& theKeys) noexcept;

		operator String() noexcept;
	};

	/// For the various connection states of the VoiceConnection class. \brief For the various connection states of the VoiceConnection class.
	enum class VoiceConnectionState : Int8 {
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
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::WebSocketSSLShard, public DiscordCoreInternal::DatagramSocketClient {
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
		std::atomic<VoiceConnectionState> connectionState{ VoiceConnectionState::Collecting_Init_Data };
		UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::atomic<VoiceActiveState> lastActiveState{ VoiceActiveState::Connecting };
		std::unique_ptr<DiscordCoreInternal::DatagramSocketClient> streamSocket{};
		std::atomic<VoiceActiveState> activeState{ VoiceActiveState::Connecting };
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		DiscordCoreInternal::WebSocketSSLShard* baseShard{ nullptr };
		std::unique_ptr<std::jthread> taskThread01{ nullptr };
		std::unique_ptr<std::jthread> taskThread02{ nullptr };
		std::unique_ptr<std::jthread> taskThread03{ nullptr };
		std::unordered_map<Uint64, VoiceUser> voiceUsers{};
		std::deque<ConnectionPackage> theConnections{};
		AtomicBool areWeConnectedBool{ false };
		std::deque<VoicePayload> theFrameQueue{};
		AtomicBool areWePlaying{ false };
		const Int64 maxReconnectTries{ 10 };
		Int64 currentReconnectTries{ 0 };
		String audioEncryptionMode{};
		Snowflake currentGuildMemberId{};
		ConnectionPackage thePackage{};
		String secretKeySend{};
		std::mutex voiceUserMutex{};
		Uint16 sequenceIndex{ 0 };
		AudioFrameData audioData{};
		StreamInfo theStreamInfo{};
		AudioEncoder theEncoder{};
		String externalIp{};
		StreamType streamType{};
		Uint32 timeStamp{ 0 };
		String voiceIp{};
		String baseUrl{};
		Uint32 audioSSRC{};
		Uint64 port{};

		Bool collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept;

		String encryptSingleAudioFrame(AudioFrameData& bufferToSend) noexcept;

		Bool sendTextMessage(String& theMessage, Bool priority) noexcept;

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer() noexcept;

		Bool onMessageReceived(StringView theData) noexcept;

		void sendSingleFrame(AudioFrameData& frameData) noexcept;

		void sendSpeakingMessage(const Bool isSpeaking) noexcept;

		void sendVoiceData(String& responseData) noexcept;

		void runWebSocket(std::stop_token) noexcept;

		void runBridge(std::stop_token) noexcept;

		void runVoice(std::stop_token) noexcept;

		void parseIncomingVoiceData() noexcept;

		Bool areWeCurrentlyPlaying() noexcept;

		void disconnectInternal() noexcept;

		void reconnectStream() noexcept;

		void connectInternal() noexcept;

		void clearAudioData() noexcept;

		Bool areWeConnected() noexcept;

		void sendHeartBeat() noexcept;

		Bool parseMessage() noexcept;

		Bool voiceConnect() noexcept;

		void handleBuffer() noexcept;

		void sendSilence() noexcept;

		void pauseToggle() noexcept;

		void disconnect() noexcept;

		void reconnect() noexcept;

		void onClosed() noexcept;

		void mixAudio() noexcept;

		void connect() noexcept;

		Bool stop() noexcept;

		Bool play() noexcept;
	};
	/**@}*/

};// namespace DiscordCoreAPI
#endif