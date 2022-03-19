/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

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
/// May 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file DiscordCoreClient.hpp

#pragma once

#include <FoundationEntities.hpp>
#include <EventEntities.hpp>
#include <ApplicationCommandEntities.hpp>
#include <Http.hpp>
#include <CommandController.hpp>
#include <ChannelEntities.hpp>
#include <GuildEntities.hpp>
#include <GuildMemberEntities.hpp>
#include <MessageEntities.hpp>
#include <ReactionEntities.hpp>
#include <RoleEntities.hpp>
#include <ThreadEntities.hpp>
#include <InputEvents.hpp>
#include <InteractionEntities.hpp>
#include <EventManager.hpp>
#include <WebSocketEntities.hpp>
#include <GuildScheduledEventEntities.hpp>
#include <CoRoutine.hpp>
#include <VoiceConnection.hpp>
#include <SoundCloudAPI.hpp>
#include <StageInstanceEntities.hpp>
#include <YouTubeAPI.hpp>
#include <StickerEntities.hpp>
#include <UserEntities.hpp>
#include <ThreadPool.hpp>
#include <SSLClients.hpp>
#include <WebHookEntities.hpp>
#include <SongAPI.hpp>
#include <unordered_map>

void signalHandler(int32_t);

namespace DiscordCoreAPI {

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<VoiceConnection>>& getVoiceConnectionMap();

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<SoundCloudAPI>>& getSoundCloudAPIMap();

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<YouTubeAPI>>& getYouTubeAPIMap();

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<SongAPI>>& getSongAPIMap();

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// DiscordCoreClient - The main class for this library. \brief DiscordCoreClient - The main class for this library.
	class DiscordCoreAPI_Dll DiscordCoreClient {
	public:

		friend Guild;

		CommandController commandController{ nullptr };///< A command controller.
		EventManager eventManager{};///< An EventManager.

		/// DiscordCoreClient constructor. \brief DiscordCoreClient constructor.
		/// \param botTokenNew A string containing your bot's token.
		/// \param functionsToExecuteNew A vector of DiscordCoreAPI::RepeatedFunctionData, for potentially repeated functions.
		/// \param cacheOptionsNew A DiscordCoreAPI::CacheOptions structure for cache settings.
		/// \param shardOptionsNew A DiscordCoreAPI::ShardingOptions structure to select the shard configuration for this given process.
		/// \param loggingOptionsNew A DiscordCoreAPI::LoggingOptions structure to select logging configuration options.
		DiscordCoreClient(std::string botTokenNew, std::vector<RepeatedFunctionData> functionsToExecuteNew = std::vector<RepeatedFunctionData>{}, CacheOptions cacheOptionsNew = CacheOptions{}, ShardingOptions shardOptionsNew = ShardingOptions{}, LoggingOptions loggingOptionsNew = LoggingOptions{});

		DiscordCoreClient(nullptr_t);

		/// For registering a function with the CommandController. \brief For registering a function with the CommandController.
		/// \param functionNames A vector containing the possible names for activating this command/function.
		/// \param baseFunction A unique_ptr to the command to be registered.
		void registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction);

		/// For collecting a copy of the current bot's User.
		/// \returns BotUser An instance of BotUser.
		BotUser getBotUser();

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient();

	protected:

		std::unordered_map<std::string, std::unique_ptr<DiscordCoreInternal::BaseSocketAgent>> theWebSockets{};
		std::unique_ptr<DiscordCoreInternal::HttpClient> httpClient{};
		std::vector<RepeatedFunctionData> functionsToExecute{};
		std::vector<ThreadPoolTimer> threadPoolTimers{};
		ShardingOptions shardingOptions{};
		LoggingOptions loggingOptions{};
		bool didWeStartFine{ false };
		CacheOptions cacheOptions{};
		BotUser currentUser{};
		
		GatewayBotData getGateWayBot();

		void run();
	};
	/**@}*/
}
