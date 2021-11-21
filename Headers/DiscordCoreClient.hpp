// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "EventManager.hpp"
#include "WebSocketEntities.hpp"
#include "CoRoutine.hpp"
#include "VoiceConnection.hpp"
#include "SoundCloudAPI.hpp"
#include "YouTubeAPI.hpp"
#include "SongAPI.hpp"

DiscordCoreAPI_Dll void myPurecallHandler(void);

DiscordCoreAPI_Dll BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

DiscordCoreAPI_Dll void terminateWrapper();

namespace DiscordCoreAPI {

	struct CacheOptions {
		bool cacheGuildMembers{ false };
		bool cacheChannels{ false };
		bool cacheGuilds{ false };
		bool cacheRoles{ false };
		bool cacheUsers{ false };
	};

	namespace Statics {
		namespace {
			unique_ptr<map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>> audioBuffersMap{ nullptr };
			unique_ptr<map<string, unique_ptr<VoiceConnection>>> voiceConnectionMap{ nullptr };
			unique_ptr<map<string, unique_ptr<SoundCloudAPI>>> soundCloudAPIMap{ nullptr };
			unique_ptr<map<string, unique_ptr<YouTubeAPI>>> youtubeAPIMap{ nullptr };
			unique_ptr<map<string, unique_ptr<SongAPI>>> songAPIMap{ nullptr };
		}
	}

	DiscordCoreAPI_Dll map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* getAudioBuffersMap();

	DiscordCoreAPI_Dll map<string, unique_ptr<VoiceConnection>>* getVoiceConnectionMap();

	DiscordCoreAPI_Dll map<string, unique_ptr<SoundCloudAPI>>* getSoundCloudAPIMap();

	DiscordCoreAPI_Dll map<string, unique_ptr<YouTubeAPI>>* getYouTubeAPIMap();

	DiscordCoreAPI_Dll map<string, unique_ptr<SongAPI>>* getSongAPIMap();

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	class DiscordCoreAPI_Dll DiscordCoreClient {
	public:

		friend BOOL WINAPI::HandlerRoutine(_In_ DWORD dwCtrlType);
		DiscordCoreAPI_Dll friend BotUser getBotUser();
		friend class ApplicationCommands;
		friend void ::terminateWrapper();
		friend class GuildMembers;
		friend class Interactions;
		friend class InputEvents;
		friend class Reactions;
		friend class Messages;
		friend class Stickers;
		friend class Channels;
		friend class Guilds;
		friend class Roles;
		friend class Users;
		friend class Guild;
		friend class Test;

		static unique_ptr<DiscordCoreClient> thisPointer;
		static string commandPrefix;

		unique_ptr<EventManager> eventManager{ nullptr };

		/// Sets up some resources for the library. \brief Sets up some resources for the library.
		/// \param botTokenNew Your bot token. 
		/// \param commandPrefixNew The prefix you would like to use for triggering command activiation via chat. 
		/// \param functionVector A pointer to a vector of function pointers to be run on timers.
		/// \returns void
		static void setup(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData> functionVector = vector<RepeatedFunctionData>(), CacheOptions cacheOptions = CacheOptions());

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		/// \returns void
		static void runBot();

		DiscordCoreClient(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData> functionsToExecuteNew);

		~DiscordCoreClient();

	protected:

		static vector<RepeatedFunctionData> functionsToExecute;

		unique_ptr<unbounded_buffer<DiscordCoreInternal::WebSocketWorkload>> webSocketWorkloadTarget{ make_unique<unbounded_buffer<DiscordCoreInternal::WebSocketWorkload>>() };
		unique_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebSocketAgent{ nullptr };
		unique_ptr<BotUser> currentUser{ nullptr };
		CacheOptions cacheOptions{};
		bool doWeQuit{ false };
		string botToken{ "" };

		CoRoutine<void> initialize();

		static void cleanup();

		CoRoutine<string> getGateWayUrl();

		void terminate();

		CoRoutine<void> run();
	};
	/**@}*/
	/**
	* \addtogroup utilities
	* @{
	*/

	/// Returns the bot's User data. \brief Returns the bot's User data.
	/// \returns A BotUser which is a type of User.
	DiscordCoreAPI_Dll BotUser getBotUser();
	/**@}*/
}
