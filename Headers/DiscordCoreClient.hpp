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

		std::unique_ptr<DiscordCoreClient> thisPointer{ nullptr };
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
		DiscordCoreInternal::WSADATAWrapper wsaData{};
		CacheOptions cacheOptions{};
		std::string botToken{ "" };
		bool doWeQuit{ false };
		BotUser currentUser{};

		std::string getGateWayBot();

		void initialize();

		void run();
	};
	/**@}*/
}
