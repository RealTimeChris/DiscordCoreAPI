/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
/// May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file DiscordCoreClient.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/SSLClients.hpp>
#include <discordcoreapi/SongAPI.hpp>
#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/ThreadPool.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>
#include <discordcoreapi/YouTubeAPI.hpp>
#include <discordcoreapi/JsonSpecializations2.hpp>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SIGTERMError : public DCAException {
	  public:
		SIGTERMError(const std::string& string);
	};

	class DiscordCoreAPI_Dll SIGSEGVError : public DCAException {
	  public:
		SIGSEGVError(const std::string& string);
	};

	class DiscordCoreAPI_Dll SIGINTError : public DCAException {
	  public:
		SIGINTError(const std::string& string);
	};

	class DiscordCoreAPI_Dll SIGILLError : public DCAException {
	  public:
		SIGILLError(const std::string& string);
	};

	class DiscordCoreAPI_Dll SIGABRTError : public DCAException {
	  public:
		SIGABRTError(const std::string& string);
	};

	class DiscordCoreAPI_Dll SIGFPEError : public DCAException {
	  public:
		SIGFPEError(const std::string& string);
	};

	using SoundCloudAPIMap = std::unordered_map<uint64_t, std::unique_ptr<DiscordCoreInternal::SoundCloudAPI>>;

	using YouTubeAPIMap = std::unordered_map<uint64_t, std::unique_ptr<DiscordCoreInternal::YouTubeAPI>>;

	using VoiceConnectionsMap = std::unordered_map<uint64_t, std::unique_ptr<VoiceConnection>>;

	using SongAPIMap = std::unordered_map<uint64_t, std::unique_ptr<SongAPI>>;

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief DiscordCoreClient - The main class for this library.
	class DiscordCoreAPI_Dll DiscordCoreClient {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreInternal::WebSocketCore;
		friend class VoiceConnection;
		friend class GuildData;
		friend class BotUser;
		friend class Guilds;

		static DiscordCoreInternal::SoundCloudAPI* getSoundCloudAPI(Snowflake guildId);

		static DiscordCoreInternal::YouTubeAPI* getYouTubeAPI(Snowflake guildId);

		static VoiceConnection* getVoiceConnection(Snowflake guildId);

		static SongAPI* getSongAPI(Snowflake guildId);

		/// \brief DiscordCoreClient constructor.
		/// \param configData A DiscordCoreClientConfig structure to select various library options.
		DiscordCoreClient(DiscordCoreClientConfig configData);

		/// \brief For registering a function with the CommandController.
		/// \param functionNames A vector containing the possible names for activating this command/function.
		/// \param baseFunction A unique_ptr to the command to be registered.
		/// \param commandData A CreateApplicationCommandData structure describing the current function.
		/// \param alwaysRegister Whether or not it gets registered every time the bot boots up, or only when it's missing from the bot's list of registered commands.
		void registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction,
			CreateApplicationCommandData commandData, bool alwaysRegister = false);

		/// \brief For collecting a reference to the CommandController.
		/// \returns CommandController& A reference to the CommandController.
		CommandController& getCommandController();

		/// \brief For collecting a reference to the ConfigManager.
		/// \returns ConfigManager& A reference to the ConfigManager.
		ConfigManager& getConfigManager();

		/// \brief For collecting a reference to the EventManager.
		/// \returns EventManager& A reference to the EventManager.
		EventManager& getEventManager();

		/// \brief For collecting, the total time in milliseconds that this bot has been up for.
		/// \returns Milliseconds A count, in milliseconds, since the bot has come online.
		Milliseconds getTotalUpTime();

		/// \brief For collecting a copy of the current bot's User.
		/// \returns BotUser An instance of BotUser.
		static BotUser getBotUser();

		/// \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient() noexcept;

	  protected:
		static BotUser currentUser;

		std::unordered_map<uint32_t, std::unique_ptr<DiscordCoreInternal::BaseSocketAgent>> baseSocketAgentsMap{};
		std::unique_ptr<DiscordCoreInternal::HttpsClient> httpsClient{ nullptr };
		std::deque<CreateApplicationCommandData> commandsToRegister{};
		StopWatch<Milliseconds> connectionStopWatch00{ 5000 };
		StopWatch<Milliseconds> connectionStopWatch01{ 5000 };
#ifdef _WIN32
		DiscordCoreInternal::WSADataWrapper theWSAData{};
#endif
		CommandController commandController{ this };
		std::atomic_bool isItSafeToConnect{ true };
		Milliseconds startupTimeSinceEpoch{};
		ConfigManager configManager{};
		std::mutex connectionMutex{};
		EventManager eventManager{};///< An event-manager, for hooking into Discord-API-Events sent over the Websockets.
		bool didWeStartCorrectly{};

		void registerFunctionsInternal();

		GatewayBotData getGateWayBot();

		bool instantiateWebSockets();
	};
	/**@}*/
}
