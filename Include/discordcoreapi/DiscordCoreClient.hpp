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
/// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
/// May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file DiscordCoreClient.hpp

#pragma once

#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Http.hpp>
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

	extern "C" void signalHandler(int32_t);

	using VoiceConnectionMap = std::unordered_map<uint64_t, std::unique_ptr<VoiceConnection>>;

	using SoundCloudAPIMap = std::unordered_map<uint64_t, std::unique_ptr<DiscordCoreInternal::SoundCloudAPI>>;

	using YouTubeAPIMap = std::unordered_map<uint64_t, std::unique_ptr<DiscordCoreInternal::YouTubeAPI>>;

	using SongAPIMap = std::unordered_map<uint64_t, std::unique_ptr<SongAPI>>;

	DiscordCoreAPI_Dll VoiceConnectionMap& getVoiceConnectionMap();

	DiscordCoreAPI_Dll SoundCloudAPIMap& getSoundCloudAPIMap();

	DiscordCoreAPI_Dll YouTubeAPIMap& getYouTubeAPIMap();

	DiscordCoreAPI_Dll SongAPIMap& getSongAPIMap();

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// DiscordCoreClient - The main class for this library. \brief DiscordCoreClient - The main class for this library.
	class DiscordCoreAPI_Dll DiscordCoreClient {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend GuildData;
		friend Guilds;
		friend Guild;

		CommandController commandController{ this };///< A command controller.
		EventManager eventManager{};///< An EventManager.

		/// DiscordCoreClient constructor. \brief DiscordCoreClient constructor.
		/// \param configData A DiscordCoreAPI::DiscordCoreClientConfig structure to select various library options.
		DiscordCoreClient(DiscordCoreClientConfig configData);

		/// For registering a function with the CommandController. \brief For registering a function with the CommandController.
		/// \param functionNames A vector containing the possible names for activating this command/function.
		/// \param baseFunction A unique_ptr to the command to be registered.
		void registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction);

		/// For collecting a copy of the current bot's User. \brief For collecting a copy of the current bot's User.
		/// \returns BotUser An instance of BotUser.
		BotUser getBotUser();

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient();

	  protected:
		std::unordered_map<std::string, std::unique_ptr<DiscordCoreInternal::BaseSocketAgent>> webSocketMap{};
		std::unique_ptr<DiscordCoreInternal::HttpClient> httpClient{};
		std::vector<RepeatedFunctionData> functionsToExecute{};

#ifdef _WIN32
		DiscordCoreInternal::WSADataWrapper theWSAData{};
#endif
		std::vector<std::string> threadIds{};
		ShardingOptions shardingOptions{};
		LoggingOptions loggingOptions{};
		CacheOptions cacheOptions{};
		ThreadPool threadPool{};
		std::string botToken{};
		TextFormat theFormat{};
		BotUser currentUser{};

		GatewayBotData getGateWayBot();

		bool instantiateWebSockets();

		void run();
	};
	/**@}*/
}// namespace DiscordCoreAPI
