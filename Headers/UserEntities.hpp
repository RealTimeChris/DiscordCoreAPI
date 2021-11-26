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
	struct alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll GetUserData {
		string userId{ "" };///< The id of the desired User.
	};

	/// Leave Guild data - for leaving a particular Guild. \brief Leave Guild data - for leaving a particular Guild.
	struct alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll LeaveGuildData {
		string guildId{ "" };///< The id of the Guild you would like the bot to leave.
	};

	/// For updating a User's presence. \brief For updating a User's presence.
	struct alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll UpdatePresenceData {
		vector<ActivityData> activities{};///< A vector of activities.
		string status{ "" };///< Current status.
		bool afk{ false };///< Are we afk?
	};

	/// Adds a user to a group DM. \brief Adds a user to a group DM.
	struct alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll AddRecipientToGroupDMData {
		string token{ "" };///< The user's access token.
		string nick{ "" };///< The user's nickname.
		string channelId{ "" };///< The channel Id of the DM.
		string userId{ "" };///< The user's Id.
	};

	struct alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll RemoveRecipientFromGroupDMData {
		string channelId{ "" };///< The channel Id of the DM.
		string userId{ "" };///< The user's Id.
	};

	/// For updating the current voice state. \brief For updating the current voice state.
	struct alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll UpdateVoiceStateData {
		string channelId{ "" };///< Id of the desired voice Channel. Leave blank to disconnect.
		bool selfMute{ false };///< Whether or not we self-mute ourselves.
		bool selfDeaf{ false };///< Whether or not we self-deafen ourselves.
		string guildId{ "" };///< The id of the Guild fo which we would like to establish a voice connection.
	};

	/// A type of User, to represent the Bot and some of its associated endpoints. \brief A type of User, to represent the Bot and some of its associated endpoints.
	class alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll BotUser : public UserData {
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
	/// An interface class alignas(hardware_destructive_interference_size) for the User related Discord endpoints. \brief An interface class alignas(hardware_destructive_interference_size) for the User related Discord endpoints.
	class alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll Users {
	public:

		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Users;
		friend class Guild;
		
		/// Collects a given User from the library's cache. \brief Collects a given User from the library's cache.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCachedUserAsync(GetUserData dataPackage);

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