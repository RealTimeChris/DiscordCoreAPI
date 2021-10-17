// VoiceConnection.hpp - Header for the voice connection class.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _VOICE_CONNECTION_
#define _VOICE_CONNECTION_

#include "IndexInitial.hpp"
#include "WebSocketEntities.hpp"
#include "SongDecoder.hpp"
#include "SongEncoder.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// VoiceConnection class - represents the connection to a given voice channel. \brief VoiceConnection class - represents the connection to a given voice channel.
	class DiscordCoreAPI_Dll VoiceConnection: DiscordCoreInternal::ThreadContext, agent{
	public:

		friend class DiscordCoreClient;
		friend class SoundCloudAPI;
		friend class YouTubeAPI;
		friend class SongAPI;
		friend class Guild;

		VoiceConnection(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew, shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebsocketAgentNew, string guildIdNew);

		/// Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this. \brief Send a single frame of audio data. Be sure to send one frame every x ms apart where x is the duration of each frame, and also be sure to call SongAPI::play() before calling this.
		/// \param frameData A single frame worth of audio data.
		/// \returns void
		void sendSingleFrame(AudioFrameData frameData);

		/// For setting up behavior in response to a completed song. \brief For setting up behavior in response to a completed song.
		/// \param handler A delegate taking a SongCompletionEventData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onSongCompletion(delegate<SongCompletionEventData> const& handler);

		/// For de-registering the event-handler function that was previously registered. \brief For de-registering the event-handler function that was previously registered.
		/// \param token The event_token that was returned from the registration function.
		/// \returns void
		void onSongCompletion(event_token const& token);

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns A string containing the Channel's id.
		string getChannelId();

		~VoiceConnection();

	protected:

		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;

		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voiceChannelWebSocketAgent{ nullptr };
		map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* receiveAudioBufferMap{ nullptr };
		shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebsocketAgent{ nullptr };
		winrt::event<delegate<SongCompletionEventData>>* onSongCompletionEvent {};
		shared_ptr<unbounded_buffer<AudioFrameData>> audioDataBuffer{ nullptr };
		concurrency::event* connectionReadyEvent {new concurrency::event()};
		concurrency::event* disconnectionEvent {new concurrency::event()};
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		concurrency::event* playWaitEvent {new concurrency::event()};
		concurrency::event* stopWaitEvent {new concurrency::event()};
		concurrency::event* playSetEvent {new concurrency::event()};
		concurrency::event* stopSetEvent {new concurrency::event()};
		concurrency::event* pauseEvent {new concurrency::event()};
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

		unbounded_buffer<AudioFrameData>* getSendAudioBuffer();

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
#endif