/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// UserEntities.hpp - Header for User related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file UserEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For updating a User's presence. \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		std::vector<ActivityData> activities{};///< A std::vector of activities.
		std::string status{};///< Current status.
		int64_t since{ 0 };///< When was the activity started?
		bool afk{ false };///< Are we afk.

		operator nlohmann::json();
	};

	/// For adding a user to a group Dm. \brief For adding a user to a group Dm.
	struct DiscordCoreAPI_Dll AddRecipientToGroupDMData {
		Snowflake channelId{};///< The Channel Id of the Dm.
		Snowflake userId{};///< The user's Id.
		std::string token{};///< The user's access token.
		std::string nick{};///< The user's nickname.

		operator std::string();
	};

	/// For removing a User from a group Dm. \brief For removing a User from a group Dm.
	struct DiscordCoreAPI_Dll RemoveRecipientFromGroupDMData {
		Snowflake channelId{};///< The Channel Id of the Dm.
		Snowflake userId{};///< The user's Id.
	};

	/// For updating the bot's current voice state. \brief For updating the bot's current voice state.
	struct DiscordCoreAPI_Dll ModifyCurrentUserVoiceStateData {
		std::string requestToSpeakTimestamp{};///< ISO8601 timestamp.
		Snowflake channelId{};///< The id of the Channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		Snowflake guildId{};///< The Guild within which to update the bot's voice state.
	};

	/// For modifying a User's voice state. \brief For modifying a User's voice state.
	struct DiscordCoreAPI_Dll ModifyUserVoiceStateData {
		Snowflake channelId{};///< The id of the Channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		Snowflake guildId{};///< The Guild within which you would like to modify their voice state.
		Snowflake userId{};///< The user for which you would like to modify the voice state of.
	};

	/// For getting User responseData from the library's cache or the Discord server. \brief For getting User responseData from the library's cache or the Discord server.
	struct DiscordCoreAPI_Dll GetUserData {
		Snowflake userId{};///< The id of the desired User.
	};

	/// For modifying the Bot's User responseData. \brief For modifying the Bot's User responseData.
	struct DiscordCoreAPI_Dll ModifyCurrentUserData {
		std::vector<uint8_t> avatar{};///< If passed, modifies the user's avatar.
		std::string userName{};///< User's userName, if changed may cause the user's discriminator to be randomized.
	};

	/// A single User. \brief A single User.
	class DiscordCoreAPI_Dll User : public UserData, public DataParser<User> {
	  public:
		PremiumType premiumType{};///< The type of Nitro subscription on a user ' s account.
		int32_t accentColor{ 0 };///< The user 's banner color encoded as an integer representation of hexadecimal color code.
		std::string banner{};///< The user's banner hash.
		std::string locale{};///< The user' s chosen language option.
		std::string email{};///< The user's email.

		User() = default;

		User& operator=(UserData&& other);

		User(UserData&&);

		User& operator=(UserData& other);

		User(UserData&);

		User& operator=(const nlohmann::json& jsonObjectData);

		User(const nlohmann::json& jsonObjectData);

		virtual ~User() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, DiscordCoreAPI::User* pDataStructure);
	};

	class DiscordCoreAPI_Dll UserVector : public DataParser<UserVector> {
	  public:
		UserVector() = default;

		operator std::vector<User>();

		UserVector& operator=(const nlohmann::json& jsonObjectData);

		UserVector(const nlohmann::json& jsonObjectData);

		virtual ~UserVector() = default;

	  protected:
		std::vector<User> theUsers{};

		void parseObject(const nlohmann::json& jsonObjectData, UserVector* pDataStructure);
	};

	/// A type of User, to represent the Bot and some of its associated endpoints. \brief A type of User, to represent the Bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll BotUser : public User {
	  public:
		friend DiscordCoreClient;

		BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* pBaseBaseSocketAgentNew);

		/// Updates the bot's current voice-status. Joins/leaves a Channel, and/or self deafens/mutes. \brief Updates the bot's current voice-status. Joins/leaves a Channel, and/or self deafens/mutes.
		void updateVoiceStatus(UpdateVoiceStateData& datdataPackageaPackage);

		/// Updates the bot's current activity status, to be viewed by others in the same server as the bot. \brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		void updatePresence(UpdatePresenceData& dataPackage);

	  protected:
		BotUser() = default;

		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
	};
	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the User related Discord endpoints. \brief An interface class for the User related Discord endpoints.
	class DiscordCoreAPI_Dll Users {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend DiscordCoreClient;
		friend EventHandler;
		friend Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

		/// Adds a chosen recipient to a group Dm. \brief Adds a chosen recipient to a group Dm.
		/// \param dataPackage An AddRecipientToGroupDMData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage);

		/// Removes a chosen recipient from a group Dm. \brief Removes a chosen recipient from a group Dm.
		/// \param dataPackage A RemoveRecipientFromGroupDMData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage);

		/// Sets the bot's current voice state. \brief Sets the bot's current voice state.
		/// \param dataPackage A ModifyCurrentUserVoiceStateData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage);

		/// Sets another user's current voice state. \brief Sets another user's current voice state.
		/// \param dataPackage A ModifyUserVoiceStateData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage);

		/// Collects the Bot's current User responseData. \brief Collects the Bot's current User responseData.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<UserData> getCurrentUserAsync();

		/// Collects a given User from the library's cache. \brief Collects a given User from the library's cache.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<UserData> getCachedUserAsync(GetUserData dataPackage);

		/// Collects a given User from the Discord servers. \brief Collects a given User from the Discord servers.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getUserAsync(GetUserData dataPackage);

		/// Modifies the Bot's User responseData. \brief Modifies the Bot's User responseData.
		/// \param dataPackage A ModifyCurrentUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> modifyCurrentUserAsync(ModifyCurrentUserData dataPackage);

		/// Collects the User's Connections. \brief Collects the User's Connections.
		/// \returns A CoRoutine containing a std::vector<ConnectionData>.
		static CoRoutine<std::vector<ConnectionData>> getUserConnectionsAsync();

		/// Collects the Application responseData associated with the current Bot.
		/// \brief Collects the Application responseData associated with the current Bot.
		/// \returns A CoRoutine containing an ApplicationData.
		static CoRoutine<ApplicationData> getCurrentUserApplicationInfoAsync();

		/// Collects the Authorization info associated with the current Bot. \brief Collects the Authorization info associated with the current Bot.
		/// \returns A CoRoutine containing an AuthorizationInfoData.
		static CoRoutine<AuthorizationInfoData> getCurrentUserAuthorizationInfoAsync();

		static void insertUser(UserData user);

	  protected:
		static std::unique_ptr<std::unordered_map<Snowflake, std::unique_ptr<UserData>>> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ConfigManager* configManager;
		static std::shared_mutex theMutex;
	};
	/**@}*/

}// namespace DiscordCoreAPI
