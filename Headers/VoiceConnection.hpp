// VoiceConnection.hpp - Header for the voice connection class.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "WebSocketEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {
	
	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// VoiceConnection class alignas(hardware_destructive_interference_size) - represents the connection to a given voice channel. \brief VoiceConnection class alignas(hardware_destructive_interference_size) - represents the connection to a given voice channel.
	class alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll VoiceConnection { 
	public:

		friend class DiscordCoreClient;
		friend class SoundCloudAPI;
		friend class YouTubeAPI;
		friend class SongAPI;
		friend class Guild;
		friend class Test;
		
		VoiceConnection(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew, DiscordCoreInternal::BaseWebSocketAgent* baseWebsocketAgentNew, DiscordCoreInternal::ConnectionWebSocketData* connectionData);

		VoiceConnection();

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

		unique_ptr<winrt::event<delegate<SongCompletionEventData>>> onSongCompletionEvent{ make_unique<winrt::event<delegate<SongCompletionEventData>>>() };
		 unbounded_buffer<AudioFrameData>* audioDataBuffer{ nullptr };
		unique_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voiceChannelWebSocketAgent{ nullptr };
		unique_ptr<concurrency::event> connectionReadyEvent{ make_unique<concurrency::event>() };
		unique_ptr<concurrency::event> disconnectionEvent{ make_unique<concurrency::event>() };
		unique_ptr<concurrency::event> reconnectionEvent{ make_unique<concurrency::event>() };
		unique_ptr<concurrency::event> playWaitEvent{ make_unique<concurrency::event>() };
		unique_ptr<concurrency::event> stopWaitEvent{ make_unique<concurrency::event>() };
		unique_ptr<concurrency::event> playSetEvent{ make_unique<concurrency::event>() };
		unique_ptr<concurrency::event> stopSetEvent{ make_unique<concurrency::event>() };
		unique_ptr<concurrency::event> pauseEvent{ make_unique<concurrency::event>() };
		 AudioFrameData audioData {};
		 CoRoutine<void> theTask{};
		DiscordCoreInternal::BaseWebSocketAgent* baseWebsocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		const int32_t maxBufferSize{ 1276 };
		uint16_t sequenceIndex{ 0 };
		bool areWeConnectedBool{ false };
		OpusEncoder* encoder{ nullptr };
		bool areWeInstantiated{ false };
		uint32_t timestamp{ 0 };
		bool hasTerminateRun{ false };
		bool areWeStopping{ false };
		bool doWeReconnect{ false };
		bool areWeWaiting{ false };
		bool areWePlaying{ false };
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

		vector<uint8_t> encryptSingleAudioFrame(EncodedFrameData bufferToSend);

		void sendSingleAudioFrame(vector<uint8_t> audioDataPacketNew);

		void sendSpeakingMessage(bool isSpeaking);

		CoRoutine<void> run();
	};
	/**@}*/
	
};