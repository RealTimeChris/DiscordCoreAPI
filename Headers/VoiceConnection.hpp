// VoiceConnection.hpp - Header for the voice connection class.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "SongDecoder.hpp"
#include "WebSocketEntities.hpp"
#include "SongEncoder.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// VoiceConnection class - represents the connection to a given voice channel. \brief VoiceConnection class - represents the connection to a given voice channel.
	class DiscordCoreAPI_Dll VoiceConnection : public DiscordCoreInternal::ThreadContext, public agent {
	public:

		friend class DiscordCoreClient;
		friend class SoundCloudAPI;
		friend class YouTubeAPI;
		friend class SongAPI;
		friend class Guild;
		friend class Test;

		VoiceConnection(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew, shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebsocketAgentNew, DiscordCoreInternal::ConnectionWebSocketData* connectionData);

		/// Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this. \brief Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this.
		/// \param frameData A single frame worth of audio data.
		/// \returns void. 
		void sendSingleFrame(AudioFrameData frameData);

		/// For setting up behavior in response to a completed song. \brief For setting up behavior in response to a completed song.
		/// \param handler A delegate taking a SongCompletionEventData structure as an argument.
		/// \returns An event_token for later de-registering the event. 
		event_token onSongCompletion(delegate<SongCompletionEventData> const& handler);

		/// For de-registering the event-handler function that was previously registered. \brief For de-registering the event-handler function that was previously registered.
		/// \param token The event_token that was returned from the registration function.
		/// \returns void.
		void onSongCompletion(event_token const& token);

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns A string containing the Channel's id. 
		string getChannelId();

		~VoiceConnection();

	protected:

		shared_ptr<winrt::event<delegate<SongCompletionEventData>>> onSongCompletionEvent{ new winrt::event<delegate<SongCompletionEventData>>() };
		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voiceChannelWebSocketAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebsocketAgent{ nullptr };
		shared_ptr<concurrency::event> connectionReadyEvent{ new concurrency::event() };
		shared_ptr<concurrency::event> disconnectionEvent{ new concurrency::event() };
		shared_ptr<concurrency::event> reconnectionEvent{ new concurrency::event() };
		shared_ptr<concurrency::event> playWaitEvent{ new concurrency::event() };
		shared_ptr<concurrency::event> stopWaitEvent{ new concurrency::event() };
		shared_ptr<unbounded_buffer<AudioFrameData>> audioDataBuffer{ nullptr };
		shared_ptr<concurrency::event> playSetEvent{ new concurrency::event() };
		shared_ptr<concurrency::event> stopSetEvent{ new concurrency::event() };
		shared_ptr<concurrency::event> pauseEvent{ new concurrency::event() };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		const __int32 maxBufferSize{ 1276 };
		unsigned __int16 sequenceIndex{ 0 };
		bool areWeConnectedBool{ false };
		OpusEncoder* encoder{ nullptr };
		bool areWeInstantiated{ false };
		unsigned __int32 timestamp{ 0 };
		bool hasTerminateRun{ false };
		bool areWeStopping{ false };
		bool doWeReconnect{ false };
		bool areWeWaiting{ false };
		bool areWePlaying{ false };
		AudioFrameData audioData{};
		bool areWePaused{ false };
		bool doWeQuit{ false };
		string channelId{ "" };
		string guildId{ "" };
		bool areWeConnected();

		bool areWeCurrentlyPlaying();
		bool stop();
		bool skip();
		bool play();
		void pauseToggle();
		static void reconnect(string guildId);
		void clearAudioData();
		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame);
		vector<unsigned __int8> encryptSingleAudioFrame(EncodedFrameData bufferToSend);
		void sendSingleAudioFrame(vector<unsigned __int8> audioDataPacketNew);

		void sendSpeakingMessage(bool isSpeaking);

		void run();
	};
	/**@}*/
};