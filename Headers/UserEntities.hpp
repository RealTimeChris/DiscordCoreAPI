// UserEntities.hpp - Header for User related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _USER_ENTITIES_
#define _USER_ENTITIES_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "ChannelEntities.hpp"
#include "WebSocketEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// Get User data. For collecting a User's data from the library's cache or the Discord server. \brief Get User data. For collecting a User's data from the library's cache or the Discord server. 
	struct DiscordCoreAPI_Dll GetUserData {
		string userId{ "" };///< The id of the desired User.
	};

	/// Leave Guild data - for leaving a particular Guild. \brief Leave Guild data - for leaving a particular Guild.
	struct DiscordCoreAPI_Dll LeaveGuildData {
		string guildId{ "" };///< The id of the Guild you would like the bot to leave.
	};

	/// A single User. \brief A single User.
	class DiscordCoreAPI_Dll User : public UserData {
	protected:

		friend struct Concurrency::details::_ResultHolder<User>;
		friend class DiscordCoreInternal::UserManagerAgent;
		friend class DiscordCoreInternal::UserManager;
		friend struct OnGuildMemberRemoveData;
		friend struct OnGuildBanRemoveData;
		friend struct OnGuildBanAddData;
		friend struct OnUserUpdateData;
		friend class DiscordCoreClient;
		friend class Guild;

		DiscordCoreAPI::User();

		User(UserData dataNew);
	};

	/// For updating a User's presence. \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		vector<ActivityData> activities{};///< A vector of activities.
		string status{ "" };///< Current status.
		bool afk{ false };///< Are we afk?
	};

	/// For updating the current voice state. \brief For updating the current voice state.
	struct DiscordCoreAPI_Dll UpdateVoiceStateData {
		string channelId{ "" };///< Id of the desired voice Channel. Leave blank to disconnect.
		bool selfMute{ false };///< Whether or not we self-mute ourselves.
		bool selfDeaf{ false };///< Whether or not we self-deafen ourselves.
		string guildId{ "" };///< The id of the Guild fo which we would like to establish a voice connection.
	};

	/// A type of User, to represent the Bot and some of its associated endpoints. \brief A type of User, to represent the Bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll BotUser : public UserData {
	public:

		friend class DiscordCoreClient;

		BotUser(UserData dataPackage, shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> pBaseWebSocketAgentNew);

		/// Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes. \brief Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes.
		/// \returns void.
		void updateVoiceStatus(UpdateVoiceStateData datdataPackageaPackage);

		/// Updates the bot's current activity status, to be viewed by others in the same server as the bot. \brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		/// \returns void.
		void updatePresence(UpdatePresenceData dataPackage);

	protected:

		BotUser();

		shared_ptr<DiscordCoreInternal::BaseWebSocketAgent> pBaseWebSocketAgent{ nullptr };
	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll UserManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class UserManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<DiscordCoreAPI::ApplicationData> outApplicationBuffer{ nullptr };
		unbounded_buffer<LeaveGuildData> requestLeaveGuildBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::User> outUserBuffer{ nullptr };
		unbounded_buffer<bool> requestGetApplicationBuffer{ nullptr };
		unbounded_buffer<GetUserData> requestGetUserBuffer{ nullptr };

		UserManagerAgent();

		static void intialize();

		DiscordCoreAPI::User getObjectData(GetUserData dataPackage);

		DiscordCoreAPI::ApplicationData getObjectData();

		void deleteObjectData(LeaveGuildData dataPackage);

		void run();

		static void cleanup();

	};

	class DiscordCoreAPI_Dll UserManager : ThreadContext {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Users;
		friend class DiscordCoreAPI::Guild;

		UserManager(UserManager* pointer);

		~UserManager();

	protected:

		overwrite_buffer<map<string, DiscordCoreAPI::User>>* cache{};

		task<DiscordCoreAPI::User> getCurrentUserAsync();

		task<DiscordCoreAPI::User> getCachedUserAsync(DiscordCoreAPI::GetUserData dataPackage);

		task<DiscordCoreAPI::User> getUserAsync(DiscordCoreAPI::GetUserData dataPackage);

		task<void> leaveGuildAsync(DiscordCoreAPI::LeaveGuildData dataPackage);

		task<DiscordCoreAPI::ApplicationData> getApplicationDataAsync();

		task<void> insertUserAsync(DiscordCoreAPI::User useruser);
	};
}
#endif