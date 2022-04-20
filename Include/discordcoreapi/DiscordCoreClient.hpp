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

	void signalHandler(int32_t);

	using VoiceConnectionMap = std::unordered_map<std::string, std::unique_ptr<VoiceConnection>>;

	using SoundCloudAPIMap = std::unordered_map<std::string, std::unique_ptr<SoundCloudAPI>>;

	using YouTubeAPIMap = std::unordered_map<std::string, std::unique_ptr<YouTubeAPI>>;

	using SongAPIMap = std::unordered_map<std::string, std::unique_ptr<SongAPI>>;

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
		friend Guild;

		CommandController commandController{ this };///< A command controller.
		EventManager eventManager{};///< An EventManager.

		/// DiscordCoreClient constructor. \brief DiscordCoreClient constructor.
		/// \param botTokenNew A string containing your bot's token.
		/// \param functionsToExecuteNew A vector of DiscordCoreAPI::RepeatedFunctionData, for potentially repeated functions.
		/// \param cacheOptionsNew A DiscordCoreAPI::CacheOptions structure for cache settings.
		/// \param shardOptionsNew A DiscordCoreAPI::ShardingOptions structure to select the shard configuration for this given process.
		/// \param loggingOptionsNew A DiscordCoreAPI::LoggingOptions structure to select logging configuration options.
		DiscordCoreClient(std::string botTokenNew, std::vector<RepeatedFunctionData> functionsToExecuteNew = std::vector<RepeatedFunctionData>{},
			CacheOptions cacheOptionsNew = CacheOptions{}, ShardingOptions shardOptionsNew = ShardingOptions{},
			LoggingOptions loggingOptionsNew = LoggingOptions{});

		/// For registering a function with the CommandController. \brief For registering a function with the CommandController.
		/// \param functionNames A vector containing the possible names for activating this command/function.
		/// \param baseFunction A unique_ptr to the command to be registered.
		void registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction);

		/// For collecting a copy of the current bot's User. \brief For collecting a copy of the current bot's User.
		/// \returns BotUser An instance of BotUser.
		BotUser getBotUser();

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient();

	  protected:
		std::unordered_map<std::string, std::unique_ptr<DiscordCoreInternal::BaseSocketAgent>> theWebSockets{};
		std::unique_ptr<DiscordCoreInternal::HttpClient> httpClient{};
		std::vector<std::string> threadIds{};
		ShardingOptions shardingOptions{};
		LoggingOptions loggingOptions{};
		bool didWeStartFine{ false };
		CacheOptions cacheOptions{};
		ThreadPool threadPool{};
		BotUser currentUser{};

		void instantiateWebSockets(std::vector<RepeatedFunctionData> functionsToExecuteNew, std::string botTokenNew);

		GatewayBotData getGateWayBot();

		void run();
	};
	/**@}*/
}// namespace DiscordCoreAPI
