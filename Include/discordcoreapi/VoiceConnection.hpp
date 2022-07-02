/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
/// https://discordcoreapi.com
/// \file VoiceConnection.hpp

#pragma once

#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>
#include <sodium.h>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// VoiceConnection class DiscordCoreAPI_Dll - represents the connection to a given voice Channel. \brief VoiceConnection class DiscordCoreAPI_Dll - represents the connection to a given voice Channel.
	class DiscordCoreAPI_Dll VoiceConnection {
	  public:
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class DiscordCoreClient;
		friend class GuildData;
		friend class SongAPI;

		VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew);

		VoiceConnection() = default;

		/// Collects the currently connected-to voice Channel's id. \brief Collects the currently connected-to voice Channel's id.
		/// \returns uint64_t A Snowflake containing the Channel's id.
		Snowflake getChannelId();

		~VoiceConnection();

	  protected:
		DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData> voiceConnectionDataBuffer{};
		std::unordered_map<int32_t, std::unique_ptr<DiscordCoreInternal::SSLEntity>> theClients{};
		std::unique_ptr<DiscordCoreInternal::AudioEncoder> encoder{ nullptr };
		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::WebSocketSSLShard* theBaseShard{ nullptr };
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		UnboundedMessageBlock<AudioFrameData> audioBuffer{};
		std::unique_ptr<std::jthread> theTask{ nullptr };
		DiscordCoreInternal::EventWaiter playSetEvent{};
		DiscordCoreInternal::EventWaiter stopSetEvent{};
		DiscordCoreInternal::EventWaiter pauseEvent{};
		std::atomic_bool areWeConnectedBool{ false };
		std::atomic_bool doWeDisconnect{ false };
		std::atomic_bool doWeReconnect{ false };
		std::atomic_bool areWeStopping{ false };
		std::atomic_bool areWePlaying{ false };
		int32_t maxReconnectionTries{ 10 };
		int64_t disconnectStartTime{ 0 };
		Snowflake currentGuildMemberId{};
		int32_t heartbeatInterval{ 0 };
		bool didWeJustConnect{ true };
		int16_t sequenceIndex{ 0 };
		AudioFrameData audioData{};
		int32_t timeStamp{ 0 };
		std::string baseUrl{};
		std::string hostIp{};

		void stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, DiscordCoreInternal::WebSocketOpCode theOpCode) noexcept;

		void parseHeadersAndMessage(DiscordCoreInternal::WebSocketSSLShard* theShard) noexcept;

		void sendVoiceData(std::string& responseData) noexcept;

		void sendMessage(std::string& dataToSend) noexcept;

		void onClosed() noexcept;

		void createHeader(std::string& outBuffer, uint64_t sendLength, DiscordCoreInternal::WebSocketOpCode opCode) noexcept;

		void onMessageReceived(const std::string& theMessage) noexcept;
		
		void collectExternalIP() noexcept;
		
		void sendHeartBeat() noexcept;

		void voiceConnect() noexcept;

		void connect() noexcept;

		std::string encryptSingleAudioFrame(EncodedFrameData& bufferToSend, int32_t audioSSRC, const std::string& keys);

		bool connect(const DiscordCoreInternal::VoiceConnectInitData& voiceConnectInitDataNew);

		void sendSingleAudioFrame(std::string& audioDataPacketNew);

		UnboundedMessageBlock<AudioFrameData>& getAudioBuffer();

		void sendSingleFrame(const AudioFrameData& frameData);

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
