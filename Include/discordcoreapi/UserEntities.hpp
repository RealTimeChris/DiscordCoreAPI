/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// UserEntities.hpp - Header for user_data related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file UserEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/WebSocketClient.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For adding a user to a group dm.
	struct add_recipient_to_group_dmdata {
		snowflake channelId{};///< The channel_data snowflake of the dm.
		jsonifier::string token{};///< The user's access token.
		jsonifier::string nick{};///< The user's nickname.
		snowflake userId{};///< The user's snowflake.
	};

	/// @brief For removing a user from a group dm.
	struct remove_recipient_from_group_dmdata {
		snowflake channelId{};///< The channel_data snowflake of the dm.
		snowflake userId{};///< The user's snowflake.
	};

	/// @brief For updating the bot's current voice state.
	struct modify_current_user_voice_state_data {
		jsonifier::string requestToSpeakTimestamp{};///< Iso8601 timeStamp.
		snowflake channelId{};///< The id of the channel_data the user is currently in.
		snowflake guildId{};///< The guild within which to update the bot's voice state.
		bool suppress{};///< Toggles the user's suppress state.
	};

	/// @brief For modifying a user's voice state.
	struct modify_user_voice_state_data {
		snowflake channelId{};///< The id of the channel_data the user is currently in.
		snowflake guildId{};///< The guild within which you would like to modify their voice state.
		snowflake userId{};///< The user for which you would like to modify the voice state of.
		bool suppress{};///< Toggles the user's suppress state.
	};

	/// @brief For getting user_data responseData from the library's cache or the discord server.
	struct get_user_data {
		snowflake userId{};///< The id of the desired user_data.
	};

	/// @brief For modifying the bot's user_data responseData.
	struct modify_current_user_data {
		jsonifier::string userName{};///< User_data's username, if changed may cause the user's discriminator to be randomized.
		jsonifier::string avatar{};///< If passed, modifies the user's avatar.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the user_data related discord endpoints.
	class DiscordCoreAPI_Dll users {
	  public:
		friend class discord_core_internal::websocket_client;
		friend class discord_core_internal::base_socket_agent;
		friend class discord_core_client;
		friend class user_data;
		friend class guild;

		static void initialize(discord_core_internal::https_client*, config_manager* configManagerNew);

		/// @brief Adds a chosen recipient to a group dm.
		/// @param dataPackage an add_recipient_to_group_dmdata  structure.
		/// @return A co_routine containing void.
		static co_routine<void> addRecipientToGroupDMAsync(add_recipient_to_group_dmdata dataPackage);

		/// @brief Removes a chosen recipient from a group dm.
		/// @param dataPackage a remove_recipient_from_group_dmdata structure.
		/// @return A co_routine containing void.
		static co_routine<void> removeRecipientFromGroupDMAsync(remove_recipient_from_group_dmdata dataPackage);

		/// @brief Sets the bot's current voice state.
		/// @param dataPackage a modify_current_user_voice_state_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> modifyCurrentUserVoiceStateAsync(modify_current_user_voice_state_data dataPackage);

		/// @brief Sets another user's current voice state.
		/// @param dataPackage a modify_user_voice_state_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> modifyUserVoiceStateAsync(modify_user_voice_state_data dataPackage);

		/// @brief Collects the bot's current user_data responseData.
		/// @return A co_routine containing a user.
		static co_routine<user_data> getCurrentUserAsync();

		/// @brief Collects a given user from the library's cache.
		/// @param dataPackage a get_user_data structure.
		/// @return A co_routine containing a user.
		static user_cache_data getCachedUser(get_user_data dataPackage);

		/// @brief Collects a given user from the discord servers.
		/// @param dataPackage a get_user_data structure.
		/// @return A co_routine containing a user.
		static co_routine<user_data> getUserAsync(get_user_data dataPackage);

		/// @brief Modifies the bot's user_data responseData.
		/// @param dataPackage a modify_current_user_data structure.
		/// @return A co_routine containing a user.
		static co_routine<user_data> modifyCurrentUserAsync(modify_current_user_data dataPackage);

		/// @brief Collects the user_data's connections.
		/// @return A co_routine containing a vector<connection_data>.
		static co_routine<jsonifier::vector<connection_data>> getUserConnectionsAsync();

		/// @brief Collects the application responseData associated with the current bot.
		/// @return A co_routine containing an application_data.
		static co_routine<application_data> getCurrentUserApplicationInfoAsync();

		/// @brief Collects the authorization info associated with the current bot.
		/// @return A co_routine containing an authorization_info_data.
		static co_routine<authorization_info_data> getCurrentUserAuthorizationInfoAsync();

		template<typename user_type> DCA_INLINE static void insertUser(user_type&& user) {
			if (doWeCacheUsersBool) {
				if (user.id == 0) {
					throw dca_exception{ "Sorry, but there was no id set for that user." };
				}
				cache.emplace(std::forward<user_type>(user));
				if (cache.count() % 1000 == 0) {
					std::cout << "current user count: " << cache.count() << std::endl;
				}
			}
		}

		static bool doWeCacheUsers();

	  protected:
		static discord_core_internal::https_client* httpsClient;
		static object_cache<user_cache_data> cache;
		static bool doWeCacheUsersBool;
	};
	/**@}*/

}
