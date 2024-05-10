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

namespace discord_core_api {

	/// @brief Voice websocket close codes.
	class voice_websocket_close {
	  public:
		/// @brief Voice websocket close codes.
		enum class voice_websocket_close_code : uint16_t {
			Unset					= 1 << 0,///< Unset.
			Normal_Close			= 1 << 1,///< Normal close.
			Unknown_Opcode			= 1 << 2,///< You sent an invalid opcode.
			Failed_To_Decode		= 1 << 3,///< You sent an invalid payload in your identifying to the gateway.
			Not_Authenticated		= 1 << 4,///< You sent a payload before identifying with the gateway.
			Authentication_Failed	= 1 << 5,///<	the token you sent in your identify payload is incorrect.
			Already_Authenticated	= 1 << 6,///<	you sent more than one identify payload. stahp.
			Session_No_Longer_Valid = 1 << 7,///< Your session is no longer valid.
			Session_Timeout			= 1 << 8,///< Your session has timed out.
			Server_Not_Found		= 1 << 9,///< We can't find the server you're trying to connect to.
			Unknown_Protocol		= 1 << 10,///< We didn't recognize the protocol you sent.
			disconnected			= 1 << 11,///< channel_data was deleted, you were kicked, voice server changed, or the main gateway session was dropped. should not reconnect.
			Voice_Server_Crashed	= 1 << 12,///< The server crashed. our bad! try resuming.
			Unknown_Encryption_Mode = 1 << 13///< We didn't recognize your encryption.
		};

		DCA_INLINE static unordered_map<int32_t, voice_websocket_close_code> mappingValues{ { 0, voice_websocket_close_code::Unset },
			{ 1000, voice_websocket_close_code::Normal_Close }, { 4001, voice_websocket_close_code::Unknown_Opcode }, { 4002, voice_websocket_close_code::Failed_To_Decode },
			{ 4003, voice_websocket_close_code::Not_Authenticated }, { 4004, voice_websocket_close_code::Authentication_Failed },
			{ 4005, voice_websocket_close_code::Already_Authenticated }, { 4006, voice_websocket_close_code::Session_No_Longer_Valid },
			{ 4009, voice_websocket_close_code::Session_Timeout }, { 4011, voice_websocket_close_code::Server_Not_Found }, { 4012, voice_websocket_close_code::Unknown_Protocol },
			{ 4014, voice_websocket_close_code::disconnected }, { 4015, voice_websocket_close_code::Voice_Server_Crashed },
			{ 4016, voice_websocket_close_code::Unknown_Encryption_Mode } };

		DCA_INLINE static unordered_map<voice_websocket_close_code, jsonifier::string> outputErrorValues{ { voice_websocket_close_code::Unset, "Unset." },
			{ voice_websocket_close_code::Normal_Close, "normal close." }, { voice_websocket_close_code::Unknown_Opcode, "you sent an invalid opcode." },
			{ voice_websocket_close_code::Failed_To_Decode, "you sent an invalid payload in your identifying to the gateway." },
			{ voice_websocket_close_code::Not_Authenticated, "you sent a payload before identifying with the gateway." },
			{ voice_websocket_close_code::Authentication_Failed, "the token you sent in your identify payload is incorrect." },
			{ voice_websocket_close_code::Already_Authenticated, "you sent more than one identify payload. stahp." },
			{ voice_websocket_close_code::Session_No_Longer_Valid, "your session is no longer valid." },
			{ voice_websocket_close_code::Session_Timeout, "your session has timed out." },
			{ voice_websocket_close_code::Server_Not_Found, "we can't find the server you're trying to connect to." },
			{ voice_websocket_close_code::Unknown_Protocol, "we didn't recognize the protocol you sent." },
			{ voice_websocket_close_code::disconnected,
				"channel_data was deleted, you were kicked, voice server changed, or the main gateway session was dropped. should not "
				"reconnect." },
			{ voice_websocket_close_code::Voice_Server_Crashed, "the server crashed. our bad! try resuming." },
			{ voice_websocket_close_code::Unknown_Encryption_Mode, "we didn't recognize your encryption." } };
		voice_websocket_close_code value{};

		DCA_INLINE voice_websocket_close& operator=(uint16_t valueNew) {
			value = static_cast<voice_websocket_close_code>(valueNew);
			return *this;
		};

		DCA_INLINE voice_websocket_close(uint16_t value) {
			*this = value;
		};

		DCA_INLINE operator jsonifier::string_view() {
			return voice_websocket_close::outputErrorValues[mappingValues[static_cast<uint16_t>(value)]];
		}

		DCA_INLINE operator bool() {
			return true;
		}
	};

	struct voice_socket_ready_data {
		jsonifier::vector<jsonifier::string> modes{};
		jsonifier::string ip{};
		uint16_t port{};
		uint32_t ssrc{};
	};

	struct voice_session_description_data {
		jsonifier::vector<uint8_t> secretKey{};
	};

	struct speaking_data {
		snowflake userId{};
		uint32_t ssrc{};
	};

	struct voice_connection_hello_data {
		uint32_t heartBeatInterval{};
	};

	struct voice_user_disconnect_data {
		snowflake userId{};
	};

	/// @brief The voice data for a given user, as raw PCM data, along with their Id.
	struct voice_user_payload {
		jsonifier::string_view_base<uint8_t> voiceData{};
		snowflake userId{};
	};

	struct DiscordCoreAPI_Dll voice_user {
		voice_user() = default;

		voice_user(snowflake userId);

		voice_user& operator=(voice_user&& data) noexcept;

		voice_user& operator=(const voice_user&) = delete;

		voice_user(const voice_user&) = delete;

		discord_core_internal::opus_decoder_wrapper& getDecoder();

		jsonifier::string_view_base<uint8_t> extractPayload();

		void insertPayload(jsonifier::string_view_base<uint8_t>);

		snowflake getUserId();

	  protected:
		discord_core_internal::ring_buffer<uint8_t, 10> payloads{};
		discord_core_internal::opus_decoder_wrapper decoder{};
		snowflake userId{};
	};

	struct DiscordCoreAPI_Dll rtp_packet_encrypter {
		rtp_packet_encrypter() = default;

		rtp_packet_encrypter(uint32_t ssrcNew, const jsonifier::string_base<uint8_t>& keysNew);

		jsonifier::string_view_base<uint8_t> encryptPacket(discord_core_internal::encoder_return_data& audioData);

	  protected:
		jsonifier::string_base<uint8_t> data{};
		jsonifier::string_base<uint8_t> keys{};
		uint32_t timeStamp{};
		uint16_t sequence{};
		uint32_t ssrc{};
	};

	struct DiscordCoreAPI_Dll moving_averager {
		moving_averager(uint64_t collectionCountNew);

		moving_averager operator+=(int64_t value);

		operator float();

	  protected:
		std::deque<int64_t> values{};
		uint64_t collectionCount{};
	};

	/// @brief The various opcodes that could be sent/received by the voice-websocket.
	enum class voice_socket_op_codes {
		identify			= 0,///< Begin a voice websocket connection.
		Select_Protocol		= 1,///< Select the voice protocol.
		Ready_Server		= 2,///< complete the websocket handshake.
		heartbeat			= 3,///< Keep the websocket connection alive.
		Session_Description = 4,///< Describe the session.
		speaking			= 5,///< Indicate which users are speaking.
		Heartbeat_ACK		= 6,///< Sent to acknowledge a received client heartbeat.
		resume				= 7,///< Resume a connection.
		hello				= 8,///< Time to wait between sending heartbeats in milliseconds.
		resumed				= 9,///< Acknowledge a successful session resume.
		Client_Disconnect	= 13,///< A client has disconnected from the voice channel.
	};

	/// @brief For the various connection states of the voice_connection class.
	enum class voice_connection_state : uint8_t {
		Collecting_Init_Data		   = 0,///< collecting initialization data.
		Initializing_WebSocket		   = 1,///< Initializing the websocket.
		Collecting_Hello			   = 2,///< collecting the client hello.
		Sending_Identify			   = 3,///< Sending the identify payload.
		Collecting_Ready			   = 4,///< collecting the client ready.
		Initializing_DatagramSocket	   = 5,///< Initializing the datagram udp SOCKET.
		Sending_Select_Protocol		   = 6,///< Sending the select-protocol payload.
		Collecting_Session_Description = 7///< collecting the session-description payload.
	};

	/// @brief For the various active states of the voice_connection class.
	enum class voice_active_state : int8_t {
		connecting = 0,///< connecting.
		playing	   = 1,///< Playing.
		stopped	   = 2,///< Stopped.
		paused	   = 3,///< Paused.
		exiting	   = 4//< exiting.
	};

	class DiscordCoreAPI_Dll voice_connection_bridge : public discord_core_internal::udp_connection {
	  public:
		friend class voice_connection;

		voice_connection_bridge(unordered_map<uint64_t, unique_ptr<voice_user>>* voiceUsersPtrNew, jsonifier::string_base<uint8_t>& encryptionKeyNew, stream_type streamType,
			const jsonifier::string& baseUrlNew, const uint16_t portNew, snowflake guildIdNew,
			std::coroutine_handle<discord_core_api::co_routine<void, false>::promise_type>* tokenNew);

		DCA_INLINE void applyGainRamp(int64_t sampleCount);

		void parseOutgoingVoiceData();

		void handleAudioBuffer() override;

		void mixAudio();

		void disconnect() override;

	  protected:
		std::coroutine_handle<discord_core_api::co_routine<void, false>::promise_type>* token{};
		unordered_map<uint64_t, unique_ptr<voice_user>>* voiceUsersPtr{};
		jsonifier::string_base<uint8_t> decryptedDataString{};
		std::array<opus_int16, 23040> downSampledVector{};
		jsonifier::string_base<uint8_t> encryptionKey{};
		std::array<opus_int32, 23040> upSampledVector{};
		jsonifier::vector<uint8_t> resampleVector{};
		moving_averager voiceUserCountAverage{ 25 };
		bool doWeKeepAudioData{ false };
		snowflake guildId{};
		float currentGain{};
		float increment{};
		float endGain{};
	};

	class DiscordCoreAPI_Dll voice_udpconnection : public discord_core_internal::udp_connection {
	  public:
		voice_udpconnection() = default;

		voice_udpconnection(const jsonifier::string& baseUrlNew, uint16_t portNew, stream_type streamType, voice_connection* ptrNew,
			std::coroutine_handle<discord_core_api::co_routine<void, false>::promise_type>* stopToken);

		void handleAudioBuffer() override;

		void disconnect() override;

	  protected:
		voice_connection* voiceConnection{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// @brief voice_connection class - represents the connection to a given voice channel_data.
	class DiscordCoreAPI_Dll voice_connection : public discord_core_internal::websocket_core {
	  public:
		friend class discord_core_internal::base_socket_agent;
		friend class discord_core_internal::sound_cloud_api;
		friend class discord_core_internal::you_tube_api;
		friend class voice_connection_bridge;
		friend class voice_udpconnection;
		friend class discord_core_client;
		friend class guild_cache_data;
		friend class guild_data;
		friend class song_api;

		/// the constructor.
		/// @param baseShardNew a pointer to the base shard that this voice connection belongs to.
		/// @param doWeQuitNew a pointer to the global signalling boolean for exiting the application.
		voice_connection(discord_core_internal::websocket_client* baseShardNew, std::atomic_bool* doWeQuitNew);

		bool areWeConnected();

		/// @brief Collects the currently connected-to voice channel_data's id.
		/// @return snowflake a snowflake containing the channel_data's id.
		snowflake getChannelId();

		/// @brief Collects some voice data for a given user.
		/// @return voice_user_payload The raw PCM audio data, along with that user's id.
		voice_user_payload extractVoicePayload();

		/// @brief Connects to a currently held voice channel.
		/// @param initData a discord_coer_api::voice_connect_init_dat structure.
		void connect(const voice_connect_init_data& initData);

		~voice_connection() = default;

	  protected:
		std::atomic<voice_connection_state> connectionState{ voice_connection_state::Collecting_Init_Data };
		unbounded_message_block<discord_core_internal::voice_connection_data> voiceConnectionDataBuffer{};
		std::coroutine_handle<discord_core_api::co_routine<void, false>::promise_type> token{};
		nanoseconds intervalCount{ static_cast<int64_t>(960.0l / 48000.0l * 1000000000.0l) };
		std::atomic<voice_active_state> prevActiveState{ voice_active_state::stopped };
		std::atomic<voice_active_state> activeState{ voice_active_state::connecting };
		discord_core_internal::voice_connection_data voiceConnectionData{};
		unordered_map<uint64_t, unique_ptr<voice_user>> voiceUsers{};
		discord_core_internal::opus_encoder_wrapper encoder{};
		discord_core_internal::websocket_client* baseShard{};
		unique_ptr<voice_connection_bridge> streamSocket{};
		jsonifier::string_base<uint8_t> encryptionKey{};
		voice_connect_init_data voiceConnectInitData{};
		jsonifier::string audioEncryptionMode{};
		rtp_packet_encrypter packetEncrypter{};
		int64_t sampleRatePerSecond{ 48000 };
		co_routine<void, false> taskThread{};
		voice_udpconnection udpConnection{};
		int64_t nsPerSecond{ 1000000000 };
		audio_frame_data xferAudioData{};
		jsonifier::string externalIp{};
		std::atomic_bool wasItAFail{};
		std::atomic_bool* doWeQuit{};
		std::atomic_bool doWeSkip{};
		jsonifier::string voiceIp{};
		jsonifier::string baseUrl{};
		int64_t samplesPerPacket{};
		snowflake currentUserId{};
		int64_t msPerPacket{};
		uint32_t audioSSRC{};
		uint16_t port{};

		void parseIncomingVoiceData(jsonifier::string_view_base<uint8_t> rawDataBufferNew);

		bool onMessageReceived(jsonifier::string_view_base<uint8_t> data);

		unbounded_message_block<audio_frame_data>& getAudioBuffer();

		void skipInternal(uint32_t currentRecursionDepth = 0);

		void checkForAndSendHeartBeat(const bool isImmedate);

		void sendSpeakingMessage(const bool isSpeaking);

		co_routine<void, false> runVoice();

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
