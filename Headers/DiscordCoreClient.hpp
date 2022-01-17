// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "EventEntities.hpp"
#include "ApplicationCommandEntities.hpp"
#include "Http.hpp"
#include "CommandController.hpp"
#include "ChannelEntities.hpp"
#include "GuildEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "MessageEntities.hpp"
#include "ReactionEntities.hpp"
#include "RoleEntities.hpp"
#include "ThreadEntities.hpp"
#include "InteractionEntities.hpp"
#include "EventManager.hpp"
#include "WebSocketEntities.hpp"
#include "GuildScheduledEventEntities.hpp"
#include "CoRoutine.hpp"
#include "VoiceConnection.hpp"
#include "SoundCloudAPI.hpp"
#include "StageInstanceEntities.hpp"
#include "YouTubeAPI.hpp"
#include "StickerEntities.hpp"
#include "UserEntities.hpp"
#include "ThreadPool.hpp"
#include "SSLClients.hpp"
#include "WebHookEntities.hpp"
#include "SongAPI.hpp"

DiscordCoreAPI_Dll void myPurecallHandler(void);

namespace DiscordCoreAPI {

	namespace Statics {
		namespace {
			unordered_map<string, TSUnboundedMessageBlock<AudioFrameData>*> audioBufferMap{};
			unordered_map<string, unique_ptr<VoiceConnection>> voiceConnectionMap{};
			unordered_map<string, unique_ptr<SoundCloudAPI>> soundCloudAPIMap{};
			unordered_map<string, unique_ptr<YouTubeAPI>> youtubeAPIMap{};
			unordered_map<string, unique_ptr<SongAPI>> songAPIMap{};
		}
	}

	DiscordCoreAPI_Dll unordered_map<string, TSUnboundedMessageBlock<AudioFrameData>*>* getAudioBufferMap();

	DiscordCoreAPI_Dll unordered_map<string, unique_ptr<VoiceConnection>>* getVoiceConnectionMap();

	DiscordCoreAPI_Dll unordered_map<string, unique_ptr<SoundCloudAPI>>* getSoundCloudAPIMap();

	DiscordCoreAPI_Dll unordered_map<string, unique_ptr<YouTubeAPI>>* getYouTubeAPIMap();

	DiscordCoreAPI_Dll unordered_map<string, unique_ptr<SongAPI>>* getSongAPIMap();

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	class DiscordCoreAPI_Dll DiscordCoreClient {
	public:

		template <typename ...T>
		friend void executeFunctionAfterTimePeriod(function<void(T...)>, int32_t, bool, T...);
		DiscordCoreAPI_Dll friend BotUser getBotUser();
		friend class Interactions;
		friend class Messages;
		friend class Guild;
		friend class Test;

		shared_ptr<DiscordCoreClient> thisPointer{ nullptr };
		CommandController commandController{ "" , nullptr };
		unique_ptr<EventManager> eventManager{ nullptr };
		string commandPrefix{};

		DiscordCoreClient(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData> functionsToExecuteNew = vector<RepeatedFunctionData>{}, CacheOptions cacheOptionsNew = CacheOptions{});

		DiscordCoreClient(nullptr_t);

		void registerFunction(vector<string> functionNames, shared_ptr<BaseFunction> baseFunction);

		BotUser getBotUser();

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient();

	protected:

		unique_ptr<DiscordCoreInternal::BaseSocketAgent> baseSocketAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::HttpClient> httpClient{ nullptr };
		vector<RepeatedFunctionData> functionsToExecute{};
		DiscordCoreInternal::WSADATAWrapper wsaData{};		
		CacheOptions cacheOptions{};
		bool doWeQuit{ false };
		BotUser currentUser{};
		string botToken{ "" };

		string getGateWayBot();

		void initialize();

		void run();
	};
	/**@}*/
}
