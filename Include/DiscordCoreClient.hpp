/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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

	namespace Statics {
		namespace {
			std::unordered_map<std::string, TSUnboundedMessageBlock<AudioFrameData>*> audioBufferMap{};
			std::unordered_map<std::string, std::unique_ptr<VoiceConnection>> voiceConnectionMap{};
			std::unordered_map<std::string, std::unique_ptr<SoundCloudAPI>> soundCloudAPIMap{};
			std::unordered_map<std::string, std::unique_ptr<YouTubeAPI>> youtubeAPIMap{};
			std::unordered_map<std::string, std::unique_ptr<SongAPI>> songAPIMap{};
		}
	}

	DiscordCoreAPI_Dll std::unordered_map<std::string, TSUnboundedMessageBlock<AudioFrameData>*>& getAudioBufferMap();

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

		/// An EventManager. \brief An EventManager.
		std::unique_ptr<EventManager> eventManager{ nullptr };
		CommandController commandController{ "" , nullptr };
		std::string commandPrefix{};

		/// DiscordCoreClient constructor. \brief DiscordCoreClient constructor.
		/// \param botTokenNew A string containing your bot's token.
		/// \param commandPrefixNew A command prefix to use for the command handler.
		/// \param functionsToExecuteNew A vector of DiscordCoreAPI::RepeatedFunctionData, for potentially repeated functions.
		/// \param cacheOptionsNew A DiscordCoreAPI::CacheOptions structure for cache settings.
		/// \param shardOptionsNew A DiscordCoreAPI::ShardingOptions structure to select the shard configuration for this given process.
		/// \param loggingOptionsNew A DiscordCoreAPI::LoggingOptions structure to select logging configuration options.
		DiscordCoreClient(std::string botTokenNew, std::string commandPrefixNew, std::vector<RepeatedFunctionData> functionsToExecuteNew = std::vector<RepeatedFunctionData>{}, CacheOptions cacheOptionsNew = CacheOptions{}, ShardingOptions shardOptionsNew = ShardingOptions{}, LoggingOptions loggingOptionsNew = LoggingOptions{});

		DiscordCoreClient(nullptr_t);

		/// For registering a function with the CommandController. \brief For registering a function with the CommandController.
		/// \param functionNames A vector containing the possible names for activating this command/function.
		/// \param baseFunction A unique_ptr to the command to be registered.
		void registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction);

		BotUser getBotUser();

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient();

	protected:
		
		std::unordered_map<std::string, std::unique_ptr<DiscordCoreInternal::BaseSocketAgent>> theWebSockets{};
		TSUnboundedMessageBlock<DiscordCoreInternal::WebSocketWorkload> webSocketWorkloadTarget{};
		std::unique_ptr<DiscordCoreInternal::HttpClient> httpClient{ nullptr };
		std::vector<RepeatedFunctionData> functionsToExecute{};
		std::vector<ThreadPoolTimer> threadPoolTimers{};
		std::unique_ptr<BotUser> currentUser{ nullptr };
		ShardingOptions shardingOptions{};
		LoggingOptions loggingOptions{};
		bool didWeStartFine{ false };
		std::string gatewayUrl{ "" };
		int32_t maxConcurrency{ 0 };
		CacheOptions cacheOptions{};
		std::string botToken{ "" };

		void getGateWayBot();

		void run();
	};
	/**@}*/
}
