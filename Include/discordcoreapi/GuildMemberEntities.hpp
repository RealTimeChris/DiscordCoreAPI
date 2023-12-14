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
/// GuildMemberEntities.hpp - Header for the guild_member_data related classes and
/// structs. may 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildMemberEntities.hpp

#pragma once

#include <discordcoreapi/JsonSpecializations.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <unordered_set>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For getting a guild_member, from the library's cache or discord server.
	struct get_guild_member_data {
		snowflake guildMemberId{};///< The user id of the desired guild_member_data.
		snowflake guildId{};///< The id of the guild from which you would like to acquire a member.
	};

	/// @brief For listing the guild_members of a chosen guild.
	struct list_guild_members_data {
		snowflake guildId{};///< Guild from which to list the guild_members.
		snowflake after{};///< The highest user id in the previous page.
		int32_t limit{};///< Max number of members to return (1 - 1000).
	};

	/// @brief For searching for one or more guild_members within a chosen guild.
	struct search_guild_members_data {
		snowflake guildId{};///< Guild within which to search for the guild_members.
		jsonifier::string query{};///< Query jsonifier::string to match username(s) and nickname(s) against.
		int32_t limit{};///< Max number of members to return (1 - 1000).
	};

	/// @brief For adding a new guild_member_data to a chosen guild.
	struct add_guild_member_data {
		jsonifier::vector<snowflake> roles{};///< Array of role_data ids the member is assigned.
		jsonifier::string accessToken{};///< An oauth2 access token granted with the guilds.join to the bot's application for the user you want to add.
		snowflake guildId{};///< The guild to add the new guild_member_data to.
		snowflake userId{};///< The user_data id of the user you wish to add.
		jsonifier::string nick{};///< Value to set users nickname to.
		bool mute{};///< Whether the user is muted in voice channels.
		bool deaf{};///< Whether the user is deafened in voice channels.
	};

	/// @brief For modifying the current guild_member_data's values.
	struct modify_current_guild_member_data {
		jsonifier::string reason{};///< A reason for modifying the current user's values.
		snowflake guildId{};///< The guild within which to modify the current user's values.
		jsonifier::string nick{};///< A new nickname for the current user.
	};

	/// @brief For modifying a guild_member's values.
	struct DiscordCoreAPI_Dll modify_guild_member_data {
		modify_guild_member_data(const guild_member_data& other);
		std::unordered_set<jsonifier::string> jsonifierExcludedKeys{};
		jsonifier::string communicationDisabledUntil{};///< When the user's timeout will expire and the user will be able to communicate in the guild again.
		jsonifier::vector<snowflake> roleIds{};///< A collection of role_data id's to be applied to them.
		jsonifier::string reason{};///< Reason for modifying this guild_member_data.
		snowflake guildMemberId{};///< The user id of the desired guild memeber.
		jsonifier::string nick{};///< Their new display/nick name.
		snowflake channelId{};///< The current voice channel_data, if applicaple.
		snowflake guildId{};///< The id of the guild for which you would like to modify a member.
		uint32_t flags{};
		bool mute{};///< Whether or not to mute them in voice.
		bool deaf{};///< Whether or not to deafen them, in voice.
	};

	/// @brief For removing a guild_member from a chosen guild.
	struct remove_guild_member_data {
		snowflake guildMemberId{};///< snowflake of the chosen guild_member_data to kick.
		jsonifier::string reason{};///< Reason for kicking the guild_member_data.
		snowflake guildId{};///< Guild from which to kick the chosen guild_member_data.
	};

	/// @brief For timing out a guild_member.
	struct timeout_guild_member_data {
		timeout_durations numOfMinutesToTimeoutFor{};///< The number of minutes to time-out the guild_member_data for.
		snowflake guildMemberId{};///< The id of the guild_member_data to be timed-out.
		jsonifier::string reason{};///< Reason for timing them out.
		snowflake guildId{};///< The id of the guild from which you would like to acquire a member.
	};

	/**@}*/


	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the guild_member_data related discord endpoints.
	class DiscordCoreAPI_Dll guild_members {
	  public:
		friend class discord_core_internal::websocket_client;
		friend class discord_core_client;
		friend class guild_member_data;
		friend class guild_data;
		friend class guild;

		static void initialize(discord_core_internal::https_client*, config_manager* configManagerNew);

		/// @brief Collects a guild_member from the discord servers.
		/// @param dataPackage a get_guild_member_data structure.
		/// @return a co_routine containing a guild_member.
		static co_routine<guild_member_data> getGuildMemberAsync(get_guild_member_data dataPackage);

		/// @brief Collects a guild_member from the library's cache.
		/// @param dataPackage a get_guild_member_data structure.
		/// @return a co_routine containing a guild_member.
		static guild_member_cache_data getCachedGuildMember(get_guild_member_data dataPackage);

		/// @brief Lists all of the guild_members of a chosen guild.
		/// @param dataPackage a list_guild_members_data structure.
		/// @return a co_routine containing a vector<guild_members>.
		static co_routine<jsonifier::vector<guild_member_data>> listGuildMembersAsync(list_guild_members_data dataPackage);

		/// @brief Searches for a list of guild_members of a chosen guild.
		/// @param dataPackage a search_guild_members_data structure.
		/// @return a co_routine containing a vector<guild_members>.
		static co_routine<jsonifier::vector<guild_member_data>> searchGuildMembersAsync(search_guild_members_data dataPackage);

		/// @brief Adds a guild_member to a chosen guild.
		/// @param dataPackage an add_guild_member_data structure.
		/// @return a co_routine containing a vector<guild_members>.
		static co_routine<guild_member_data> addGuildMemberAsync(add_guild_member_data dataPackage);

		/// @brief Modifies a guild_member's properties.
		/// @param dataPackage a modify_guild_member_data structure.
		/// @return a co_routine containing a guild_member.
		static co_routine<guild_member_data> modifyGuildMemberAsync(modify_guild_member_data dataPackage);

		/// @brief Modifies the current guild_member_data's properties.
		/// @param dataPackage a modify_current_guild_member_data structure.
		/// @return a co_routine containing a guild_member.
		static co_routine<guild_member_data> modifyCurrentGuildMemberAsync(modify_current_guild_member_data dataPackage);

		/// @brief Removes a chosen guild_member_data from a chosen guild.
		/// @param dataPackage a remove_guild_member_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> removeGuildMemberAsync(remove_guild_member_data dataPackage);

		/// @brief Times-out a chosen guild_member_data from a chosen guild.
		/// @param dataPackage a timeout_guild_member_data structure.
		/// @return a co_routine containing guild_member_data.
		static co_routine<guild_member_data> timeoutGuildMemberAsync(timeout_guild_member_data dataPackage);

		template<typename voice_state_type> DCA_INLINE static void insertVoiceState(voice_state_type&& voiceState) {
			if (doWeCacheVoiceStatesBool) {
				if (voiceState.userId == 0) {
					throw dca_exception{ "Sorry, but there was no id set for that voice state." };
				}
				vsCache.emplace(std::forward<voice_state_type>(voiceState));
			}
		}

		template<typename guild_member_type> DCA_INLINE static void insertGuildMember(guild_member_type&& guildMember) {
			if (doWeCacheGuildMembersBool) {
				if (guildMember.guildId == 0 || guildMember.user.id == 0) {
					throw dca_exception{ "Sorry, but there was no id set for that guildmember." };
				}
				cache.emplace(static_cast<guild_member_cache_data>(std::forward<guild_member_type>(guildMember)));
			}
		}

		/// @brief Collect a given guild_member's voice state data.
		/// @param voiceState A two-id-key representing their user-id and guild-id.
		/// @return voice_state_data_light The guild_member's voice_state_data.
		static voice_state_data_light getVoiceStateData(const two_id_key& voiceState);

		static void removeGuildMember(const two_id_key& guildMemberId);

		static void removeVoiceState(const two_id_key& voiceState);

		static bool doWeCacheGuildMembers();

		static bool doWeCacheVoiceStates();

	  protected:
		static discord_core_internal::https_client* httpsClient;
		static object_cache<voice_state_data_light> vsCache;
		static object_cache<guild_member_cache_data> cache;
		static bool doWeCacheGuildMembersBool;
		static bool doWeCacheVoiceStatesBool;
	};
	/**@}*/
};
