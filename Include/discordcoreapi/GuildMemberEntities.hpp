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
/// GuildMemberEntities.hpp - Header for the GuildMemberData related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildMemberEntities.hpp

#pragma once

#include <discordcoreapi/JsonSpecializations.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For getting a GuildMember, from the library's cache or Discord server.
	struct GetGuildMemberData {
		Snowflake guildMemberId{};///< The user id of the desired GuildMemberData.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire a member.
	};

	/// @brief For listing the GuildMembers of a chosen Guild.
	struct ListGuildMembersData {
		Snowflake guildId{};///< Guild from which to list the GuildMembers.
		Snowflake after{};///< The highest user id in the previous page.
		int32_t limit{};///< Max number of members to return (1 - 1000).
	};

	/// @brief For searching for one or more GuildMembers within a chosen Guild.
	struct SearchGuildMembersData {
		Snowflake guildId{};///< Guild within which to search for the GuildMembers.
		std::string query{};///< Query std::string to match username(s) and nickname(s) against.
		int32_t limit{};///< Max number of members to return (1 - 1000).
	};

	/// @brief For adding a new GuildMemberData to a chosen Guild.
	struct AddGuildMemberData {
		Jsonifier::Vector<Snowflake> roles{};///< Array of RoleData ids the member is assigned.
		std::string accessToken{};///< An oauth2 access token granted with the guilds.join to the bot's application for the user you want to add.
		Snowflake guildId{};///< The Guild to add the new GuildMemberData to.
		Snowflake userId{};///< The UserData id of the user you wish to add.
		std::string nick{};///< Value to set users nickname to.
		bool mute{};///< Whether the user is muted in voice channels.
		bool deaf{};///< Whether the user is deafened in voice channels.
	};

	/// @brief For modifying the current GuildMemberData's values.
	struct ModifyCurrentGuildMemberData {
		std::string reason{};///< A reason for modifying the current user's values.
		Snowflake guildId{};///< The Guild within which to modify the current user's values.
		std::string nick{};///< A new nickname for the current user.
	};

	/// @brief For modifying a GuildMember's values.
	struct ModifyGuildMemberData {
		std::string communicationDisabledUntil{};///< When the user's timeout will expire and the user will be able to communicate in the guild again.
		Jsonifier::Vector<Snowflake> roleIds{};///< A collection of RoleData id's to be applied to them.
		Snowflake newVoiceChannelId{};///< The new voice ChannelData to move them into.
		Snowflake currentChannelId{};///< The current voice ChannelData, if applicaple.
		Snowflake guildMemberId{};///< The user id of the desired Guild memeber.
		Snowflake guildId{};///< The id of the Guild for which you would like to modify a member.
		std::string reason{};///< Reason for modifying this GuildMemberData.
		std::string nick{};///< Their new display/nick name.
		bool mute{};///< Whether or not to mute them in voice.
		bool deaf{};///< Whether or not to deafen them, in voice.
	};

	/// @brief For removing a GuildMember from a chosen Guild.
	struct RemoveGuildMemberData {
		Snowflake guildMemberId{};///< Snowflake of the chosen GuildMemberData to kick.
		std::string reason{};///< Reason for kicking the GuildMemberData.
		Snowflake guildId{};///< Guild from which to kick the chosen GuildMemberData.
	};

	/// @brief For timing out a GuildMember.
	struct TimeoutGuildMemberData {
		TimeoutDurations numOfMinutesToTimeoutFor{};///< The number of minutes to time-out the GuildMemberData for.
		Snowflake guildMemberId{};///< The id of the GuildMemberData to be timed-out.
		std::string reason{};///< Reason for timing them out.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire a member.
	};

	/**@}*/


	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the GuildMemberData related Discord endpoints.
	class DiscordCoreAPI_Dll GuildMembers {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreClient;
		friend class GuildMemberData;
		friend class GuildData;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

		/// @brief Collects a GuildMember from the Discord servers.
		/// @param dataPackage A GetGuildMemberData structure.
		/// @return A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMemberData> getGuildMemberAsync(GetGuildMemberData dataPackage);

		/// @brief Collects a GuildMember from the library's cache.
		/// @param dataPackage A GetGuildMemberData structure.
		/// @return A CoRoutine containing a GuildMember.
		static GuildMemberCacheData getCachedGuildMember(GetGuildMemberData dataPackage);

		/// @brief Lists all of the GuildMembers of a chosen Guild.
		/// @param dataPackage A ListGuildMembersData structure.
		/// @return A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<Jsonifier::Vector<GuildMemberData>> listGuildMembersAsync(ListGuildMembersData dataPackage);

		/// @brief Searches for a list of GuildMembers of a chosen Guild.
		/// @param dataPackage A SearchGuildMembersData structure.
		/// @return A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<Jsonifier::Vector<GuildMemberData>> searchGuildMembersAsync(SearchGuildMembersData dataPackage);

		/// @brief Adds a GuildMember to a chosen Guild.
		/// @param dataPackage An AddGuildMemberData structure.
		/// @return A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<GuildMemberData> addGuildMemberAsync(AddGuildMemberData dataPackage);

		/// @brief Modifies a GuildMember's properties.
		/// @param dataPackage A ModifyGuildMemberData structure.
		/// @return A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMemberData> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage);

		/// @brief Modifies the current GuildMemberData's properties.
		/// @param dataPackage A ModifyCurrentGuildMemberData structure.
		/// @return A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMemberData> modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage);

		/// @brief Removes a chosen GuildMemberData from a chosen Guild.
		/// @param dataPackage A RemoveGuildMemberData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> removeGuildMemberAsync(RemoveGuildMemberData dataPackage);

		/// @brief Times-out a chosen GuildMemberData from a chosen Guild.
		/// @param dataPackage A TimeoutGuildMemberData structure.
		/// @return A CoRoutine containing GuildMemberData.
		static CoRoutine<GuildMemberData> timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage);

		template<typename VoiceStateType> inline static VoiceStateDataLight& insertVoiceState(VoiceStateType&& voiceState) {
			if (voiceState.userId == 0) {
				throw DCAException{ "Sorry, but there was no id set for that voice state." };
			}
			return *vsCache.emplace(std::forward<VoiceStateType>(voiceState));
		}

		template<typename GuildMemberType> inline static void insertGuildMember(GuildMemberType&& guildMember) {
			if (doWeCacheGuildMembersBool) {
				if (guildMember.guildId == 0 || guildMember.user.id == 0) {
					throw DCAException{ "Sorry, but there was no id set for that guildmember." };
				}
				auto newMember = guildMember;
				cache.emplace(static_cast<GuildMemberCacheData>(std::forward<GuildMemberType>(guildMember)));
			}
		}

		static VoiceStateDataLight getVoiceStateData(const TwoIdKey& voiceState);

		static void removeGuildMember(const TwoIdKey& guildMemberId);

		static void removeVoiceState(const TwoIdKey& voiceState);

		static bool doWeCacheGuildMembers();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<VoiceStateDataLight> vsCache;
		static ObjectCache<GuildMemberCacheData> cache;
		static bool doWeCacheGuildMembersBool;
	};
	/**@}*/
};
