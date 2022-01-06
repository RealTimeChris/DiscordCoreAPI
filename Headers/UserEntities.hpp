// UserEntities.hpp - Header for User related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "WebSocketEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For updating a User's presence. \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		vector<ActivityData> activities{};///< A vector of activities.
		string status{ "" };///< Current status.
		bool afk{ false };///< Are we afk?
	};

	/// For adding a user to a group DM. \brief For adding a user to a group DM.
	struct DiscordCoreAPI_Dll AddRecipientToGroupDMData {
		string token{ "" };///< The user's access token.
		string nick{ "" };///< The user's nickname.
		string channelId{ "" };///< The channel Id of the DM.
		string userId{ "" };///< The user's Id.
	};

	/// For removing a User from a group DM. \brief For removing a User from a group DM.
	struct DiscordCoreAPI_Dll RemoveRecipientFromGroupDMData {
		string channelId{ "" };///< The channel Id of the DM.
		string userId{ "" };///< The user's Id.
	};

	/// For updating the bot's current voice state. \brief For updating the bot's current voice state.
	struct DiscordCoreAPI_Dll ModifyCurrentUserVoiceStateData {
		string requestToSpeakTimestamp{ "" };///< ISO8601 timestamp.
		string channelId{ "" };///< The id of the channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		string guildId{ "" };///< The Guild within which to update the bot's voice state.
	};

	/// For modifying a User's voice state. \brief For modifying a User's voice state.
	struct DiscordCoreAPI_Dll ModifyUserVoiceStateData {
		string channelId{ "" };///< The id of the channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		string guildId{ "" };///< The Guild within which you would like to modify their voice state.
		string userId{ "" };///< The user for which you would like to modify the voice state of.
	};

	/// For getting User responseData from the library's cache or the Discord server. \brief For getting User responseData from the library's cache or the Discord server.
	struct DiscordCoreAPI_Dll GetUserData {
		string userId{ "" };///< The id of the desired User.
	};

	/// For modifying the Bot's User responseData. \brief For modifying the Bot's User responseData.
	struct DiscordCoreAPI_Dll ModifyCurrentUserData {
		vector<uint8_t> avatar{};///< If passed, modifies the user's avatar.
		string username{ "" };///< User's username, if changed may cause the user's discriminator to be randomized.
	};
	
	/// A single User. \brief A single User.
	class DiscordCoreAPI_Dll User : public UserData {
	public:

		User();

		User(UserData dataNew);

		virtual ~User();
	};

	/// A type of User, to represent the Bot and some of its associated endpoints. \brief A type of User, to represent the Bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll BotUser : public UserData {
	public:

		friend class DiscordCoreClient;

		BotUser(UserData dataPackage, DiscordCoreInternal::WebSocketAgent* pBaseWebSocketAgentNew);

		/// Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes. \brief Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes.
		/// \returns void.
		void updateVoiceStatus(UpdateVoiceStateData datdataPackageaPackage);

		/// Updates the bot's current activity status, to be viewed by others in the same server as the bot. \brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		/// \returns void.
		void updatePresence(UpdatePresenceData dataPackage);

		virtual ~BotUser();

	protected:

		BotUser();

		DiscordCoreInternal::WebSocketAgent* webSocketAgent{ nullptr };
	};
	/**@}*/

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class for the User related Discord endpoints. \brief An interface class for the User related Discord endpoints.
	class DiscordCoreAPI_Dll Users {
	public:

		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Users;
		friend class Guild;

		/// Adds a chosen recipient to a group DM. \brief Adds a chosen recipient to a group DM.
		/// \param dataPackage An AddRecipientToGroupDMData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage);

		/// Removes a chosen recipient from a group DM. \brief Removes a chosen recipient from a group DM.
		/// \param dataPackage A RemoveRecipientFromGroupDMData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage);

		/// Sets the bot's current voice state. \brief Sets the bot's current voice state.
		/// \param dataPackage A ModifyCurrentUserVoiceStateData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage);

		/// Sets another user's current voice state. \brief Sets another user's current voice state.
		/// \param dataPackage A ModifyUserVoiceStateData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage);

		/// Collects the Bot's current User responseData. \brief Collects the Bot's current User responseData.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCurrentUserAsync();

		/// Collects a given User from the library's cache. \brief Collects a given User from the library's cache.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCachedUserAsync(GetUserData dataPackage);

		/// Collects a given User from the Discord servers. \brief Collects a given User from the Discord servers.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getUserAsync(GetUserData dataPackage);

		/// Modifies the Bot's User responseData. \brief Modifies the Bot's User responseData.
		/// \param dataPackage A ModifyCurrentUserData  structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> modifyCurrentUserAsync(ModifyCurrentUserData dataPackage);

		/// Collects the User's Connections. \brief Collects the User's Connections.
		/// \returns A CoRoutine containing a vector<ConnectionData>.
		static CoRoutine<vector<ConnectionData>> getUserConnections();

		/// Collects the Application responseData associated with the current Bot. \brief Collects the Application responseData associated with the current Bot.
		/// \returns A CoRoutine containing an ApplicationData.
		static CoRoutine<ApplicationData> getCurrentUserApplicationInfoAsync();

		/// Collects the Authorization info associated with the current Bot. \brief Collects the Authorization info associated with the current Bot.
		/// \returns A CoRoutine containing an AuthorizationInfoData.
		static CoRoutine<AuthorizationInfoData> getCurrentUserAuthorizationInfoAsync();


	protected:
		static map<string, User> cache;

		static void insertUser(User user);
	};
	/**@}*/

}