// VoiceConnection.hpp - Header for the voice connection class DiscordCoreAPI_Dll.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#pragma comment(lib, "libsodium.lib")

#include "FoundationEntities.hpp"
#include "WebSocketEntities.hpp"
#include "AudioEncoder.hpp"
#include "CoRoutine.hpp"
#include <sodium.h>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll AudioEncrypter {
	public:

		AudioEncrypter();

		std::vector<int8_t> encryptSingleAudioFrame(EncodedFrameData bufferToSend, int32_t audioSSRC, std::string keys);

	protected:

		int32_t sequenceIndex{ 0 };
		int32_t timeStamp{ 0 };
	};

	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// VoiceConnection class DiscordCoreAPI_Dll - represents the connection to a given voice channel. \brief VoiceConnection class DiscordCoreAPI_Dll - represents the connection to a given voice channel.
	class DiscordCoreAPI_Dll VoiceConnection {
	public:
		
		friend SoundCloudAPI;
		friend YouTubeAPI;
		friend SongAPI;
		friend Guild;

		VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew);

		VoiceConnection() = default;

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns A std::string containing the Channel's id. 
		std::string getChannelId();

		~VoiceConnection();

	protected:

		std::unique_ptr<DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData>> voiceConnectionBuffer{ std::make_unique<DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData>>() };
		std::unique_ptr<DiscordCoreInternal::VoiceSocketAgent> voiceSocketAgent{ nullptr };
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData* voiceConnectionData{};
		TSUnboundedMessageBlock<AudioFrameData> audioBuffer{};
		std::unique_ptr<CoRoutine<void>> theTask{ nullptr };
		std::unique_ptr<AudioEncoder> encoder{ nullptr };
		std::string currentGuildMemberId{ "" };
		EventWaiter* doWeReconnect{ nullptr };
		const int32_t maxBufferSize{ 1276 };
		bool areWeConnectedBool{ false };
		AudioEncrypter audioEncrypter{};
		bool areWeInstantiated{ false };
		bool didWeJustConnect{ true };
		bool areWeStopping{ false };
		int16_t sequenceIndex{ 0 };
		EventWaiter playSetEvent{};
		EventWaiter stopSetEvent{};
		bool areWePlaying{ false };
		AudioFrameData audioData{};
		bool areWePaused{ false };
		EventWaiter pauseEvent{};
		int32_t timestamp{ 0 };
		bool doWeQuit{ false };

		void connect(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew);

		void sendSingleAudioFrame(std::vector<int8_t>& audioDataPacketNew);

		TSUnboundedMessageBlock<AudioFrameData>& getAudioBuffer();

		void sendSingleFrame(AudioFrameData frameData);

		void sendSpeakingMessage(bool isSpeaking);
		
		bool areWeCurrentlyPlaying();

		void clearAudioData();

		CoRoutine<void> run();

		bool areWeConnected();

		void sendSilence();

		void pauseToggle();

		void disconnect();

		void reconnect();

		bool stop();

		bool play();
	};
	/**@}*/

};
