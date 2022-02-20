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
/// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class DiscordCoreAPI_Dll.
/// May 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <FoundationEntities>
#include <EventEntities>
#include <ApplicationCommandEntities>
#include <Http>
#include <CommandController>
#include <ChannelEntities>
#include <GuildEntities>
#include <GuildMemberEntities>
#include <MessageEntities>
#include <ReactionEntities>
#include <RoleEntities>
#include <ThreadEntities>
#include <InteractionEntities>
#include <EventManager>
#include <WebSocketEntities>
#include <GuildScheduledEventEntities>
#include <CoRoutine>
#include <VoiceConnection>
#include <SoundCloudAPI>
#include <StageInstanceEntities>
#include <YouTubeAPI>
#include <StickerEntities>
#include <UserEntities>
#include <ThreadPool>
#include <SSLClients>
#include <WebHookEntities>
#include <SongAPI>

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
#endif

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

	DiscordCoreAPI_Dll std::unordered_map<std::string, TSUnboundedMessageBlock<AudioFrameData>*>* getAudioBufferMap();

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<VoiceConnection>>* getVoiceConnectionMap();

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<SoundCloudAPI>>* getSoundCloudAPIMap();

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<YouTubeAPI>>* getYouTubeAPIMap();

	DiscordCoreAPI_Dll std::unordered_map<std::string, std::unique_ptr<SongAPI>>* getSongAPIMap();

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	class DiscordCoreAPI_Dll DiscordCoreClient {
	public:

		friend Guild;

		std::unique_ptr<EventManager> eventManager{ nullptr };
		CommandController commandController{ "" , nullptr };
		std::string commandPrefix{};

		DiscordCoreClient(std::string botTokenNew, std::string commandPrefixNew, std::vector<RepeatedFunctionData> functionsToExecuteNew = std::vector<RepeatedFunctionData>{}, CacheOptions cacheOptionsNew = CacheOptions{});

		DiscordCoreClient(nullptr_t);

		void registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction);

		BotUser getBotUser();

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient();

	protected:

		std::unique_ptr<DiscordCoreInternal::BaseSocketAgent> baseSocketAgent{ nullptr };
		std::unique_ptr<DiscordCoreInternal::HttpClient> httpClient{ nullptr };
		std::vector<RepeatedFunctionData> functionsToExecute{};
		std::vector<ThreadPoolTimer> threadPoolTimers{};
		std::unique_ptr<BotUser> currentUser{ nullptr };
		CacheOptions cacheOptions{};
		std::string botToken{ "" };

		std::string getGateWayBot();

		void run();
	};
	/**@}*/
}
