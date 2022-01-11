// VoiceConnection.hpp - Header for the voice connection class.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "WebSocketEntities.hpp"
#include "AudioEncoder.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// VoiceConnection class - represents the connection to a given voice channel. \brief VoiceConnection class - represents the connection to a given voice channel.
	class DiscordCoreAPI_Dll VoiceConnection {
	public:

		friend class  DiscordCoreClient;
		friend class  SoundCloudAPI;
		friend class  YouTubeAPI;
		friend class  SongAPI;
		friend class  Guild;
		friend class  Test;
		
		VoiceConnection() = default;

		VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew);

		/// Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this. \brief Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this.
		/// \param frameData A single frame worth of audio data.
		void sendSingleFrame(AudioFrameData frameData);

		/// For setting up behavior in response to a completed song. \brief For setting up behavior in response to a completed song.
		/// \param handler A delegate taking a SongCompletionEventData structure as an argument.
		/// \returns An event_token for later de-registering the event. 
		DiscordCoreAPI::EventDelegateToken onSongCompletion(DiscordCoreAPI::EventDelegate<CoRoutine<void>, SongCompletionEventData> handler);

		/// For de-registering the event-handler function that was previously registered. \brief For de-registering the event-handler function that was previously registered.
		/// \param token The event_token that was returned from the registration function.
		void onSongCompletion(DiscordCoreAPI::EventDelegateToken token);

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns A string containing the Channel's id. 
		string getChannelId();

	protected:

		DiscordCoreAPI::Event<CoRoutine<void>, SongCompletionEventData> onSongCompletionEvent{};
		unique_ptr<DiscordCoreInternal::VoiceSocketAgent> voiceSocketAgent{ nullptr };
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		TSUnboundedMessageBlock<AudioFrameData> audioBuffer{};
		unique_ptr<AudioEncoder> encoder{ nullptr };
		atomic<bool*> doWeReconnect{ nullptr };
		const int32_t maxBufferSize{ 1276 };
		CoRoutine<void> theTask{ nullptr };
		Event<void, void> playWaitEvent{};
		Event<void, void> stopWaitEvent{};
		Event<void, void> playSetEvent{};
		Event<void, void> stopSetEvent{};
		bool areWeConnectedBool{ false };
		bool areWeInstantiated{ false };
		Event<void, void> pauseEvent{};
		bool hasTerminateRun{ false };
		bool areWeStopping{ false };
		uint16_t sequenceIndex{ 0 };
		bool areWePlaying{ false };
		bool areWeWaiting{ false };
		AudioFrameData audioData{};
		bool areWePaused{ false };
		uint32_t timestamp{ 0 };
		bool doWeQuit{ false };

		void connect(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew);

		vector<uint8_t> encryptSingleAudioFrame(EncodedFrameData& bufferToSend);

		void sendSingleAudioFrame(string& audioDataPacketNew);

		void sendSpeakingMessage(bool isSpeaking);

		bool areWeCurrentlyPlaying();

		bool areWeConnected();
		
		void clearAudioData();

		CoRoutine<void> run();

		void pauseToggle();

		void sendSilence();

		void disconnect();

		void reconnect();

		bool stop();

		bool skip();

		bool play();
	};
	/**@}*/

};