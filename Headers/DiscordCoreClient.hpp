// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_
#define _DISCORD_CORE_CLIENT_

#include "IndexInitial.hpp"
#include "Http.hpp"
#include "GuildEntities.hpp"
#include "InteractionEntities.hpp"
#include "ApplicationCommandEntities.hpp"
#include "InputEvents.hpp"
#include "SongAPI.hpp"
#include "StickerEntities.hpp"
#include "WebSocketEntities.hpp"
#include "EventManager.hpp"

DiscordCoreAPI_Dll void myPurecallHandler(void);

DiscordCoreAPI_Dll BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

DiscordCoreAPI_Dll void terminateWrapper();

namespace DiscordCoreAPI {

	DiscordCoreAPI_Dll map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* getAudioBuffersMap();

	DiscordCoreAPI_Dll map<string, shared_ptr<VoiceConnection>>* getVoiceConnectionMap();

	DiscordCoreAPI_Dll map<string, shared_ptr<SoundCloudAPI>>* getSoundCloudAPIMap();

	DiscordCoreAPI_Dll map<string, shared_ptr<YouTubeAPI>>* getYouTubeAPIMap();

	DiscordCoreAPI_Dll map<string, shared_ptr<SongAPI>>* getSongAPIMap();

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	class DiscordCoreAPI_Dll DiscordCoreClient :public DiscordCoreInternal::ThreadContext, public agent {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend BOOL WINAPI::HandlerRoutine(_In_ DWORD dwCtrlType);
		friend class PermissionsConverter;
		friend class ApplicationCommands;
		friend void ::terminateWrapper();
		friend class InputEventHandler;
		friend class EventHandler;
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

		static shared_ptr<BotUser> currentUser;
		static DiscordCoreClient* thisPointer;
		static string commandPrefix;

		shared_ptr<EventManager> eventManager{ nullptr };

		/// Sets up some resources for the library. \brief Sets up some resources for the library.
		/// \param botTokenNew Your bot token. 
		/// \param commandPrefixNew The prefix you would like to use for triggering command activiation via chat. 
		/// \param functionVector A pointer to a vector of function pointers to be run on timers.
		/// \returns void
		static void setup(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData>* functionVector);

		/// Executes the library, and waits for completion. \brief Executes the library, and waits for completion.
		/// \returns void
		static void runBot();

	protected:

		static vector<RepeatedFunctionData>* functionsToExecute;

		shared_ptr<unbounded_buffer<DiscordCoreInternal::WebSocketWorkload>> webSocketWorkloadTarget{ nullptr };
		shared_ptr<DiscordCoreInternal::ApplicationCommandManager> applicationCommands{ nullptr };
		shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> baseWebSocketAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<DiscordCoreInternal::InteractionManager> interactions{ nullptr };
		shared_ptr<DiscordCoreInternal::ReactionManager> reactions{ nullptr };
		shared_ptr<DiscordCoreInternal::MessageManager> messages{ nullptr };
		shared_ptr<DiscordCoreInternal::StickerManager> stickers{ nullptr };
		shared_ptr<DiscordCoreInternal::ChannelManager> channels{ nullptr };
		shared_ptr<DiscordCoreInternal::GuildManager> guilds{ nullptr };
		shared_ptr<DiscordCoreInternal::RoleManager> roles{ nullptr };
		shared_ptr<DiscordCoreInternal::UserManager> users{ nullptr };
		shared_ptr<InputEventHandler> inputEvents{ nullptr };
		string baseURL{ "https://discord.com/api/v9" };
		bool doWeQuit{ false };
		string botToken{ "" };

		DiscordCoreClient(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData>* functionsToExecuteNew);

		void initialize();

		string getGateWayUrl();

		Guild createGuild(GuildData guildData);

		void removeGuild(GuildData guildData);

		void run();

		void terminate();

		~DiscordCoreClient();
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
#endif
