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

	template<>
	class CoRoutine<void>;

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

		VoiceConnection();

		VoiceConnection(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew, DiscordCoreInternal::MsgWebSocketAgent* baseWebsocketAgentNew);
				
		/// Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this. \brief Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this.
		/// \param frameData A single frame worth of audio data.
		/// \returns void. 
		void sendSingleFrame(AudioFrameData frameData);

		/// For setting up behavior in response to a completed song. \brief For setting up behavior in response to a completed song.
		/// \param handler A delegate taking a SongCompletionEventData structure as an argument.
		/// \returns An event_token for later de-registering the event. 
		DiscordCoreAPI::EventDelegateToken onSongCompletion(DiscordCoreAPI::EventDelegate<CoRoutine<void>, SongCompletionEventData> handler);

		/// For de-registering the event-handler function that was previously registered. \brief For de-registering the event-handler function that was previously registered.
		/// \param token The event_token that was returned from the registration function.
		/// \returns void.
		void onSongCompletion(DiscordCoreAPI::EventDelegateToken token);

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns A string containing the Channel's id. 
		string getChannelId();

		~VoiceConnection();

	protected:

		unique_ptr<DiscordCoreAPI::Event<CoRoutine<void>, SongCompletionEventData>> onSongCompletionEvent{ make_unique<DiscordCoreAPI::Event<CoRoutine<void>, SongCompletionEventData>>() };
		unique_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voiceChannelWebSocketAgent{ nullptr };
		unique_ptr<Event<void, void> > reconnectionEvent{ make_unique<Event<void, void>>() };
		DiscordCoreInternal::MsgWebSocketAgent* baseWebsocketAgent{ nullptr };
		TSUnboundedMessageBlock<AudioFrameData>* audioDataBuffer{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		Event<void, void> connectionReadyEvent{};
		Event<void, void> disconnectionEvent {};
		unique_ptr<CoRoutine<void>> theTask{};
		const int32_t maxBufferSize{ 1276 };
		Event<void, void> playWaitEvent {};
		Event<void, void> stopWaitEvent {};
		Event<void, void> playSetEvent {};
		Event<void, void> stopSetEvent{};
		Event<void, void> pauseEvent {};
		bool areWeConnectedBool{ false };
		OpusEncoder* encoder{ nullptr };
		bool areWeInstantiated{ false };
		bool hasTerminateRun{ false };
		bool areWeStopping{ false };
		bool doWeReconnect{ false };
		uint16_t sequenceIndex{ 0 };
		bool areWeWaiting{ false };
		bool areWePlaying{ false };
		AudioFrameData audioData{};
		bool areWePaused{ false };
		uint32_t timestamp{ 0 };
		bool doWeQuit{ false };
		string channelId{ "" };
		string guildId{ "" };

		vector<uint8_t> encryptSingleAudioFrame(EncodedFrameData bufferToSend);

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame);

		void sendSingleAudioFrame(vector<uint8_t> audioDataPacketNew);

		void sendSpeakingMessage(bool isSpeaking);

		static void reconnect(string guildId);

		bool areWeCurrentlyPlaying();

		bool areWeConnected();
		
		void clearAudioData();

		CoRoutine<void> run();

		void pauseToggle();

		bool stop();

		bool skip();

		bool play();
	};
	/**@}*/

};