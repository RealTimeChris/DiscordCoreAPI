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
	/// VoiceConnection class DiscordCoreAPI_Dll - represents the connection to a given voice channel. \brief VoiceConnection class DiscordCoreAPI_Dll - represents the connection to a given voice channel.
	class DiscordCoreAPI_Dll VoiceConnection { 
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
		EventToken onSongCompletion(EventHandler<SongCompletionEventData>& handler);

		/// For de-registering the event-handler function that was previously registered. \brief For de-registering the event-handler function that was previously registered.
		/// \param token The event_token that was returned from the registration function.
		/// \returns void.
		void onSongCompletion(EventToken const& token);

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns A string containing the Channel's id. 
		string getChannelId();

		~VoiceConnection();

	protected:

		Event<EventHandler<SongCompletionEventData>> onSongCompletionEvent{ Event<EventHandler<SongCompletionEventData>>() };
		unique_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voiceChannelWebSocketAgent{ nullptr };
		unique_ptr<Event<void>> reconnectionEvent{ make_unique<Event<void>>() };
		DiscordCoreInternal::BaseWebSocketAgent* baseWebsocketAgent{ nullptr };
		UnboundedMessageBlock<AudioFrameData>* audioDataBuffer{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		const int32_t maxBufferSize{ 1276 };
		Event<void> connectionReadyEvent{};
		CoRoutine<void> theTask{ nullptr };
		Event<void> disconnectionEvent{};
		bool areWeConnectedBool{ false };
		OpusEncoder* encoder{ nullptr };
		bool areWeInstantiated{ false };
		bool hasTerminateRun{ false };
		bool areWeStopping{ false };
		bool doWeReconnect{ false };
		uint16_t sequenceIndex{ 0 };
		Event<void> playWaitEvent{};
		Event<void> stopWaitEvent{};
		Event<void> playSetEvent{};
		Event<void> stopSetEvent{};
		bool areWeWaiting{ false };
		bool areWePlaying{ false };
		AudioFrameData audioData{};
		bool areWePaused{ false };
		Event<void> pauseEvent{};
		uint32_t timestamp{ 0 };
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