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

	/// For updating a User's presence. \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		vector<ActivityData> activities{};///< A vector of activities.
		string status{ "" };///< Current status.
		bool afk{ false };///< Are we afk?
	};

	/// Adds a user to a group DM. \brief Adds a user to a group DM.
	struct DiscordCoreAPI_Dll AddRecipientToGroupDMData {
		string token{ "" };///< The user's access token.
		string nick{ "" };///< The user's nickname.
		string channelId{ "" };///< The channel Id of the DM.
		string userId{ "" };///< The user's Id.
	};

	struct DiscordCoreAPI_Dll RemoveRecipientFromGroupDMData {
		string channelId{ "" };///< The channel Id of the DM.
		string userId{ "" };///< The user's Id.
	};

	/// For updating the current voice state. \brief For updating the current voice state.
	struct DiscordCoreAPI_Dll UpdateVoiceStateData {
		string channelId{ "" };///< Id of the desired voice Channel. Leave blank to disconnect.
		bool selfMute{ false };///< Whether or not we self-mute ourselves.
		bool selfDeaf{ false };///< Whether or not we self-deafen ourselves.
		string guildId{ "" };///< The id of the Guild fo which we would like to establish a voice connection.
	};

	/// For updating the bot's current voice state. \brief For updating the bot's current voice state.
	struct ModifyCurrentUserVoiceStateData {
		string requestToSpeakTimestamp{ "" };///< ISO8601 timestamp.
		string channelId{ "" };///< The id of the channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		string guildId{ "" };///< The guild within which to update the bot's voice state.
	};

	struct ModifyUserVoiceStateData {
		string channelId{ "" };///< The id of the channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		string guildId{ "" };///< The Guild within which you would like to modify their voice state.
		string userId{ "" };///< The user for which you would like to modify the voice state of.
	};	

	/// A single User. \brief A single User.
	class DiscordCoreAPI_Dll User : public UserData {
	public:

		friend struct Concurrency::details::_ResultHolder<User>;
		friend class DiscordCoreInternal::HttpRequestAgent;
		friend class DiscordCoreInternal::DataParser;
		template<typename returnValueType>
		friend class CoRoutine;
		friend struct OnGuildMemberRemoveData;
		friend struct OnGuildBanRemoveData;
		friend struct OnGuildBanAddData;
		friend struct OnUserUpdateData;
		friend class DiscordCoreClient;
		friend class Reactions;
		friend class Users;
		friend class Guild;

		User();

		virtual ~User() {};

	protected:

		User(UserData dataNew);
	};

	/// A type of User, to represent the Bot and some of its associated endpoints. \brief A type of User, to represent the Bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll BotUser : public UserData {
	public:

		friend class DiscordCoreClient;

		BotUser(UserData dataPackage, DiscordCoreInternal::BaseWebSocketAgent* pBaseWebSocketAgentNew);

		/// Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes. \brief Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes.
		/// \returns void.
		void updateVoiceStatus(UpdateVoiceStateData datdataPackageaPackage);

		/// Updates the bot's current activity status, to be viewed by others in the same server as the bot. \brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		/// \returns void.
		void updatePresence(UpdatePresenceData dataPackage);

	protected:

		BotUser();

		DiscordCoreInternal::BaseWebSocketAgent* pBaseWebSocketAgent{ nullptr };
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the User related Discord endpoints. \brief An interface class for the User related Discord endpoints.
	class DiscordCoreAPI_Dll Users {
	public:

		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Users;
		friend class Guild;
		
		/// Collects a given User from the library's cache. \brief Collects a given User from the library's cache.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCachedUserAsync(GetUserData dataPackage);

		/// Sets the bot's current voice state. \brief Sets the bot's current voice state.
		/// \param dataPackage A ModifyCurrentUserVoiceStateData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage);

		/// Sets another user's current voice state. \brief Sets another user's current voice state.
		/// \param dataPackage A ModifyUserVoiceStateData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage);

		/// Collects a given User from the Discord servers. \brief Collects a given User from the Discord servers.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getUserAsync(GetUserData dataPackage);

		/// Adds a chosen recipient to a group DM. \brief Adds a chosen recipient to a group DM.
		/// \param dataPackage A AddRecipientToGroupDMData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage);

		/// Removes a chosen recipient from a group DM. \brief Removes a chosen recipient from a group DM.
		/// \param dataPackage A RemoveRecipientFromGroupDMData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage);

		/// Collects the Bot's current User data. \brief Collects the Bot's current User data.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCurrentUserAsync();

		/// Removes the bot from a chosen Guild. \brief Removes the bot from a chosen Guild.
		/// \param dataPackage A LeaveGuildData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> leaveGuildAsync(LeaveGuildData dataPackage);

		/// Collects the Application data associated with the current Bot. \brief Collects the Application data associated with the current Bot.
		/// \returns A CoRoutine containing an Application.
		static CoRoutine<ApplicationData> getApplicationDataAsync();

	protected:
		static ObjectCache<User> cache;

		static void insertUser(User user);
	};
	/**@}*/

}