/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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

#ifndef DISCORD_CORE_CLIENT
#define DISCORD_CORE_CLIENT

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

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SIGTERMError : public std::runtime_error {
	  public:
		SIGTERMError(String theString);
	};

	class DiscordCoreAPI_Dll SIGSEGVError : public std::runtime_error {
	  public:
		SIGSEGVError(String theString);
	};

	class DiscordCoreAPI_Dll SIGINTError : public std::runtime_error {
	  public:
		SIGINTError(String theString);
	};

	class DiscordCoreAPI_Dll SIGILLError : public std::runtime_error {
	  public:
		SIGILLError(String theString);
	};

	class DiscordCoreAPI_Dll SIGABRTError : public std::runtime_error {
	  public:
		SIGABRTError(String theString);
	};

	class DiscordCoreAPI_Dll SIGFPEError : public std::runtime_error {
	  public:
		SIGFPEError(String theString);
	};

	using SoundCloudAPIMap = std::unordered_map<Uint64, std::unique_ptr<DiscordCoreInternal::SoundCloudAPI>>;

	using YouTubeAPIMap = std::unordered_map<Uint64, std::unique_ptr<DiscordCoreInternal::YouTubeAPI>>;

	using VoiceConnectionMap = std::unordered_map<Uint64, std::unique_ptr<VoiceConnection>>;

	using SongAPIMap = std::unordered_map<Uint64, std::unique_ptr<SongAPI>>;

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// DiscordCoreClient - The main class for this library. \brief DiscordCoreClient - The main class for this library.
	class DiscordCoreAPI_Dll DiscordCoreClient {
	  public:
		friend DiscordCoreAPI_Dll DiscordCoreInternal::SoundCloudAPI* getSoundCloudAPI(Snowflake guildId);
		friend DiscordCoreAPI_Dll DiscordCoreInternal::YouTubeAPI* getYouTubeAPI(Snowflake guildId);
		friend DiscordCoreAPI_Dll VoiceConnection* getVoiceConnection(Snowflake guildId);
		friend DiscordCoreAPI_Dll SongAPI* getSongAPI(Snowflake guildId);
		friend class DiscordCoreInternal::WebSocketMessageHandler;
		friend class DiscordCoreInternal::WebSocketSSLShard;
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class VoiceConnection;
		friend class GuildData;
		friend class BotUser;
		friend class Guilds;

		static DiscordCoreInternal::SoundCloudAPI* getSoundCloudAPI(Snowflake guildId);

		static DiscordCoreInternal::YouTubeAPI* getYouTubeAPI(Snowflake guildId);

		static VoiceConnection* getVoiceConnection(Snowflake guildId);

		static SongAPI* getSongAPI(Snowflake guildId);

		/// DiscordCoreClient constructor. \brief DiscordCoreClient constructor.
		/// \param configData A DiscordCoreClientConfig structure to select various library options.
		DiscordCoreClient(DiscordCoreClientConfig configData);

		/// For registering a function with the CommandController. \brief For registering a function with the CommandController.
		/// \param functionNames A vector containing the possible names for activating this command/function.
		/// \param baseFunction A unique_ptr to the command to be registered.
		/// \param commandData A CreateApplicationCommandData structure describing the current function.
		/// \param alwaysRegister Whether or not it gets registered every time the bot boots up, or only when it's missing from the bot's list of registered commands.
		void registerFunction(const std::vector<String>& functionNames, std::unique_ptr<BaseFunction> baseFunction, CreateApplicationCommandData commandData,
			Bool alwaysRegister = false);

		/// For collecting a reference to the CommandController. \brief For collecting a reference to the CommandController.
		/// \returns CommandController& A reference to the CommandController.
		CommandController& getCommandController();

		/// For collecting a reference to the EventManager. \brief For collecting a reference to the EventManager.
		/// \returns EventManager& A reference to the EventManager.
		EventManager& getEventManager();

		/// For collecting a copy of the current bot's User. \brief For collecting a copy of the current bot's User.
		/// \returns BotUser An instance of BotUser.
		BotUser getBotUser();

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient() noexcept;

	  protected:
		std::unordered_map<Uint32, std::unique_ptr<DiscordCoreInternal::BaseSocketAgent>> baseSocketAgentMap{};
		std::unique_ptr<DiscordCoreInternal::HttpsClient> httpsClient{ nullptr };
		StopWatch<std::chrono::milliseconds> theConnectionStopWatch{ 5300ms };
		std::deque<CreateApplicationCommandData> commandsToRegister{};
#ifdef _WIN32
		DiscordCoreInternal::WSADataWrapper theWSAData{};
#endif
		std::deque<ConnectionPackage> theConnections{};
		CommandController commandController{ this };
		Bool didWeStartCorrectly{ false };
		ConfigManager configManager{};
		EventManager eventManager{};
		BotUser currentUser{};

		void registerFunctionsInternal();

		GatewayBotData getGateWayBot();

		Bool instantiateWebSockets();
	};
	/**@}*/
}// namespace DiscordCoreAPI
#endif