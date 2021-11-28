// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "EventManager.hpp"
#include "WebSocketEntities.hpp"
#include "GuildScheduledEventEntities.hpp"
#include "CoRoutine.hpp"
#include "VoiceConnection.hpp"
#include "SoundCloudAPI.hpp"
#include "YouTubeAPI.hpp"
#include "SongAPI.hpp"

DiscordCoreAPI_Dll void myPurecallHandler(void);

DiscordCoreAPI_Dll BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

DiscordCoreAPI_Dll void terminateWrapper();

namespace DiscordCoreAPI {

	namespace Statics {
		namespace {
			unique_ptr<map<string, unique_ptr<unbounded_buffer<AudioFrameData>>>> audioBuffersMap{ nullptr };
			unique_ptr<map<string, unique_ptr<VoiceConnection>>> voiceConnectionMap{ nullptr };
			unique_ptr<map<string, unique_ptr<SoundCloudAPI>>> soundCloudAPIMap{ nullptr };
			unique_ptr<map<string, unique_ptr<YouTubeAPI>>> youtubeAPIMap{ nullptr };
			unique_ptr<map<string, unique_ptr<SongAPI>>> songAPIMap{ nullptr };
		}
	}

	DiscordCoreAPI_Dll map<string, unique_ptr<unbounded_buffer<AudioFrameData>>>* getAudioBuffersMap();

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

		template <typename ...T>
		friend CoRoutine<void> executeFunctionAfterTimePeriod(function<void(T...)>theFunction, int32_t timeDelayInMs, bool isRepeating, T... args);
		friend BOOL WINAPI::HandlerRoutine(_In_ DWORD dwCtrlType);
		DiscordCoreAPI_Dll friend BotUser getBotUser();
		friend class ApplicationCommands;
		friend void ::terminateWrapper();
		friend class CommandController;
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

		static vector<unique_ptr<CoRoutine<void>>> theTaskVector;
		static vector<RepeatedFunctionData> functionsToExecute;
		static vector<ThreadPoolTimer> threadPoolTimers;

		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkloadTarget{};
		unique_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebSocketAgent{ nullptr };
		function<void(void)> theFunction{};
		CacheOptions cacheOptions{};
		bool doWeQuit{ false };
		BotUser currentUser{};
		string botToken{ "" };

		void initialize();

		static void clearTaskVector();

		string getGateWayUrl();

		static void cleanup();
		
		void terminate();

		void run();
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
