// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "EventManager.hpp"
#include "WebSocketEntities.hpp"

DiscordCoreAPI_Dll void myPurecallHandler(void);

DiscordCoreAPI_Dll BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

DiscordCoreAPI_Dll void terminateWrapper();

namespace DiscordCoreAPI {

	DiscordCoreAPI_Dll map<string, shared_ptr<concurrent_queue<AudioFrameData>>>* getAudioBuffersMap();

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
		static void setup(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData> functionVector = vector<RepeatedFunctionData>());

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		/// \returns void
		static void runBot();

		DiscordCoreClient(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData> functionsToExecuteNew);

		~DiscordCoreClient();

	protected:

		static vector<RepeatedFunctionData> functionsToExecute;

		shared_ptr<BotUser> currentUser{ nullptr };
		unique_ptr<concurrent_queue<DiscordCoreInternal::WebSocketWorkload>> webSocketWorkloadTarget{ nullptr };
		unique_ptr<DiscordCoreInternal::ApplicationCommandManager> applicationCommands{ nullptr };
		shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebSocketAgent{ nullptr };
		unique_ptr<DiscordCoreInternal::GuildMemberManager> guildMembers{ nullptr };
		unique_ptr<DiscordCoreInternal::InteractionManager> interactions{ nullptr };
		unique_ptr<DiscordCoreInternal::ReactionManager> reactions{ nullptr };
		unique_ptr<DiscordCoreInternal::MessageManager> messages{ nullptr };
		unique_ptr<DiscordCoreInternal::StickerManager> stickers{ nullptr };
		unique_ptr<DiscordCoreInternal::ChannelManager> channels{ nullptr };
		unique_ptr<DiscordCoreInternal::RoleManager> roles{ nullptr };
		unique_ptr<DiscordCoreInternal::UserManager> users{ nullptr };
		unique_ptr<InputEventHandler> inputEvents{ nullptr };
		string baseURL{ "https://discord.com/api/v9" };
		bool doWeQuit{ false };
		string botToken{ "" };
		unique_ptr<DiscordCoreInternal::GuildManager> guilds{ nullptr };

		void initialize();

		string getGateWayUrl();

		Guild createGuild(GuildData guildData);

		void terminate();

		void removeGuild(GuildData guildData);

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
