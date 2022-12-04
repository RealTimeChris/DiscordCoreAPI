/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// UserEntities.hpp - Header for User related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file UserEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>

namespace DiscordCoreAPI {

	inline bool operator==(const UserData& lhs, const UserData& rhs) {
		return lhs.id == rhs.id;
	}

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief For adding a user to a group DM.
	struct DiscordCoreAPI_Dll AddRecipientToGroupDMData {
		Snowflake channelId{};///< The Channel Id of the DM.
		std::string token{};///< The user's access token.
		std::string nick{};///< The user's nickname.
		Snowflake userId{};///< The user's Id.

		operator Jsonifier();
	};

	/// \brief For removing a User from a group DM.
	struct DiscordCoreAPI_Dll RemoveRecipientFromGroupDMData {
		Snowflake channelId{};///< The Channel Id of the DM.
		Snowflake userId{};///< The user's Id.
	};

	/// \brief For updating the bot's current voice state.
	struct DiscordCoreAPI_Dll ModifyCurrentUserVoiceStateData {
		std::string requestToSpeakTimestamp{};///< ISO8601 timeStamp.
		bool suppress{};///< Toggles the user's suppress state.
		Snowflake channelId{};///< The id of the Channel the user is currently in.
		Snowflake guildId{};///< The Guild within which to update the bot's voice state.
	};

	/// \brief For modifying a User's voice state.
	struct DiscordCoreAPI_Dll ModifyUserVoiceStateData {
		bool suppress{};///< Toggles the user's suppress state.
		Snowflake channelId{};///< The id of the Channel the user is currently in.
		Snowflake guildId{};///< The Guild within which you would like to modify their voice state.
		Snowflake userId{};///< The user for which you would like to modify the voice state of.
	};

	/// \brief For getting User responseData from the library's cache or the Discord server.
	struct DiscordCoreAPI_Dll GetUserData {
		Snowflake userId{};///< The id of the desired User.
	};

	/// \brief For modifying the Bot's User responseData.
	struct DiscordCoreAPI_Dll ModifyCurrentUserData {
		std::string userName{};///< User's userName, if changed may cause the user's discriminator to be randomized.
		std::string avatar{};///< If passed, modifies the user's avatar.
	};

	/// \brief A single User.
	class DiscordCoreAPI_Dll User : public UserData {
	  public:
		PremiumType premiumType{};///< The type of Nitro subscription on a user ' s account.
		int32_t accentColor{};///< The user 's banner color encoded as an integer representation of hexadecimal color code.
		std::string locale{};///< The user' s chosen language option.
		std::string email{};///< The user's email.
		IconHash banner{};///< The user's banner hash.

		User() noexcept = default;

		User& operator=(UserData&& other) noexcept;

		User(UserData&&) noexcept;

		User& operator=(const UserData& other) noexcept;

		User(const UserData&) noexcept;

		User(simdjson::ondemand::value jsonObjectData);

		virtual ~User() noexcept = default;
	};

	class DiscordCoreAPI_Dll UserVector {
	  public:
		UserVector() noexcept = default;

		operator std::vector<User>();

		UserVector(simdjson::ondemand::value jsonObjectData);

		virtual ~UserVector() noexcept = default;

	  protected:
		std::vector<User> users{};
	};

	/// \brief A type of User, to represent the Bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll BotUser : public User {
	  public:
		friend class DiscordCoreClient;

		BotUser& operator=(const BotUser& other) noexcept = default;

		BotUser(const BotUser& other) noexcept = default;

		BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* pBaseBaseSocketAgentNew);

		/// \brief Updates the bot's current voice-status. Joins/leaves a Channel, and/or self deafens/mutes.
		void updateVoiceStatus(UpdateVoiceStateData& datdataPackageaPackage);

		/// \brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		void updatePresence(UpdatePresenceData& dataPackage);

	  protected:
		BotUser() noexcept = default;

		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
	};
	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the User related Discord endpoints.
	class DiscordCoreAPI_Dll Users {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreClient;
		friend class UserData;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

		/// \brief Adds a chosen recipient to a group DM.
		/// \param dataPackage An AddRecipientToGroupDMData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage);

		/// \brief Removes a chosen recipient from a group DM.
		/// \param dataPackage A RemoveRecipientFromGroupDMData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage);

		/// \brief Sets the bot's current voice state.
		/// \param dataPackage A ModifyCurrentUserVoiceStateData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage);

		/// \brief Sets another user's current voice state.
		/// \param dataPackage A ModifyUserVoiceStateData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage);

		/// \brief Collects the Bot's current User responseData.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCurrentUserAsync();

		/// \brief Collects a given User from the library's cache.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static UserData getCachedUser(GetUserData dataPackage);

		/// \brief Collects a given User from the Discord servers.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getUserAsync(GetUserData dataPackage);

		/// \brief Modifies the Bot's User responseData.
		/// \param dataPackage A ModifyCurrentUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> modifyCurrentUserAsync(ModifyCurrentUserData dataPackage);

		/// \brief Collects the User's Connections.
		/// \returns A CoRoutine containing a vector<ConnectionData>.
		static CoRoutine<std::vector<ConnectionData>> getUserConnectionsAsync();

		/// \brief Collects the Application responseData associated with the current Bot.
		/// \returns A CoRoutine containing an ApplicationData.
		static CoRoutine<ApplicationData> getCurrentUserApplicationInfoAsync();

		/// \brief Collects the Authorization info associated with the current Bot.
		/// \returns A CoRoutine containing an AuthorizationInfoData.
		static CoRoutine<AuthorizationInfoData> getCurrentUserAuthorizationInfoAsync();

		static void insertUser(UserData user);

		static bool doWeCacheUsers();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<UserData> cache;
		static bool doWeCacheUsersBool;
	};
	/**@}*/

}// namespace DiscordCoreAPI