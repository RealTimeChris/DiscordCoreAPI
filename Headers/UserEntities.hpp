// UserEntities.hpp - Header for User related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

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
		friend class DiscordCoreInternal::ReactionManager;
		friend class DiscordCoreInternal::UserManager;
		template<typename returnValueType>
		friend class DiscordCoreAPI::CoRoutine;
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

	class DiscordCoreAPI_Dll UserManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Users;
		friend class DiscordCoreAPI::Guild;

	protected:

		concurrent_unordered_map<string, DiscordCoreAPI::User> cache{};

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::User> getCurrentUserAsync();

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::User> getCachedUserAsync(DiscordCoreAPI::GetUserData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::User> getUserAsync(DiscordCoreAPI::GetUserData dataPackage);

		DiscordCoreAPI::CoRoutine<void> leaveGuildAsync(DiscordCoreAPI::LeaveGuildData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::ApplicationData> getApplicationDataAsync();

		void insertUser(DiscordCoreAPI::User useruser);
	};
}