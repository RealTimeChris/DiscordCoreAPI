/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// VoiceConnection.hpp - Header for the voice connection class.
/// Jul 15, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file VoiceConnection.hpp

#pragma once

#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>
#include <sodium.h>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll AudioEncrypter {
	  public:
		AudioEncrypter();

		std::string encryptSingleAudioFrame(EncodedFrameData bufferToSend, int32_t audioSSRC, std::string keys);

		void resetValues();

	  protected:
		int32_t sequenceIndex{ 0 };
		int32_t timeStamp{ 0 };
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// VoiceConnection class - represents the connection to a given voice Channel. \brief VoiceConnection class - represents the connection to a given voice Channel.
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
		std::unique_ptr<DiscordCoreInternal::VoiceSocketAgent> voiceSocketAgent{ nullptr };
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData* voiceConnectionData{};
		TSUnboundedMessageBlock<AudioFrameData> audioBuffer{};
		std::unique_ptr<std::jthread> theTask{ nullptr };
		std::unique_ptr<AudioEncoder> encoder{ nullptr };
		std::atomic_bool areWeStopping{ false };
		std::atomic_bool areWePlaying{ false };
		std::string currentGuildMemberId{ "" };
		EventWaiter* doWeReconnect{ nullptr };
		const int32_t maxBufferSize{ 1276 };
		bool areWeConnectedBool{ false };
		AudioEncrypter audioEncrypter{};
		bool didWeJustConnect{ true };
		uint16_t sequenceIndex{ 0 };
		EventWaiter playSetEvent{};
		EventWaiter stopSetEvent{};
		AudioFrameData audioData{};
		EventWaiter pauseEvent{};
		uint32_t timestamp{ 0 };

		void connect(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew);

		void sendSingleAudioFrame(std::string& audioDataPacketNew);

		TSUnboundedMessageBlock<AudioFrameData>& getAudioBuffer();

		void sendSingleFrame(AudioFrameData frameData);

		void sendSpeakingMessage(bool isSpeaking);

		bool areWeCurrentlyPlaying();

		void run(std::stop_token);

		void clearAudioData();

		bool areWeConnected();

		void sendSilence();

		void pauseToggle();

		void disconnect();

		void reconnect();

		bool stop();

		bool play();
	};
	/**@}*/

};// namespace DiscordCoreAPI
