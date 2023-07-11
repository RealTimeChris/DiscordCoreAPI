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
/// GuildMemberEntities.hpp - Header for the GuildMember related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildMemberEntities.hpp

#pragma once

#include <discordcoreapi/JsonSpecializations.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief For getting a GuildMember, from the library's cache or Discord server.
	struct DiscordCoreAPI_Dll GetGuildMemberData {
		Snowflake guildMemberId{};///< The user id of the desired GuildMember.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire a member.
	};

	/// \brief For listing the GuildMembers of a chosen Guild.
	struct DiscordCoreAPI_Dll ListGuildMembersData {
		Snowflake guildId{};///< Guild from which to list the GuildMembers.
		Snowflake after{};///< The highest user id in the previous page.
		int32_t limit{};///< Max number of members to return (1 - 1000).
	};

	/// \brief For searching for one or more GuildMembers within a chosen Guild.
	struct DiscordCoreAPI_Dll SearchGuildMembersData {
		Snowflake guildId{};///< Guild within which to search for the GuildMembers.
		std::string query{};///< Query std::string to match userName(s) and nickname(s) against.
		int32_t limit{};///< Max number of members to return (1 - 1000).
	};

	/// \brief For adding a new GuildMember to a chosen Guild.
	struct DiscordCoreAPI_Dll AddGuildMemberData {
		std::vector<Snowflake> roles{};///< Array of Role ids the member is assigned.
		std::string accessToken{};///< An oauth2 access token granted with the guilds.join to the bot's application for the user you want to add.
		Snowflake guildId{};///< The Guild to add the new GuildMember to.
		Snowflake userId{};///< The User id of the user you wish to add.
		std::string nick{};///< Value to set users nickname to.
		bool mute{};///< Whether the user is muted in voice channels.
		bool deaf{};///< Whether the user is deafened in voice channels.
	};

	/// \brief For modifying the current GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyCurrentGuildMemberData {
		std::string reason{};///< A reason for modifying the current user's values.
		Snowflake guildId{};///< The Guild within which to modify the current user's values.
		std::string nick{};///< A new nickname for the current user.
	};

	/// \brief For modifying a GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyGuildMemberData {
		TimeStamp communicationDisabledUntil{};///< When the user's timeout will expire and the user will be able to communicate in the guild again.
		std::vector<Snowflake> roleIds{};///< A collection of Role id's to be applied to them.
		Snowflake newVoiceChannelId{};///< The new voice Channel to move them into.
		Snowflake currentChannelId{};///< The current voice Channel, if applicaple.
		Snowflake guildMemberId{};///< The user id of the desired Guild memeber.
		Snowflake guildId{};///< The id of the Guild for which you would like to modify a member.
		std::string reason{};///< Reason for modifying this GuildMember.
		std::string nick{};///< Their new display/nick name.
		bool mute{};///< Whether or not to mute them in voice.
		bool deaf{};///< Whether or not to deafen them, in voice.
	};

	/// \brief For removing a GuildMember from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildMemberData {
		Snowflake guildMemberId{};///< Snowflake of the chosen GuildMember to kick.
		std::string reason{};///< Reason for kicking the GuildMember.
		Snowflake guildId{};///< Guild from which to kick the chosen GuildMember.
	};

	/// \brief For timing out a GuildMember.
	struct DiscordCoreAPI_Dll TimeoutGuildMemberData {
		TimeoutDurations numOfMinutesToTimeoutFor{};///< The number of minutes to time-out the GuildMember for.
		Snowflake guildMemberId{};///< The id of the GuildMember to be timed-out.
		std::string reason{};///< Reason for timing them out.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire a member.
	};

	/**@}*/


	inline bool operator==(const GuildMemberKey& lhs, const GuildMemberKey& rhs) noexcept {
		return lhs.guildId == rhs.guildId && lhs.userId == rhs.userId;
	}

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the GuildMember related Discord endpoints.
	class DiscordCoreAPI_Dll GuildMembers {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreClient;
		friend class GuildMemberData;
		friend class GuildData;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

		/// \brief Collects a GuildMember from the Discord servers.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage);

		/// \brief Collects a GuildMember from the library's cache.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static GuildMemberData getCachedGuildMember(GetGuildMemberData dataPackage);

		/// \brief Lists all of the GuildMembers of a chosen Guild.
		/// \param dataPackage A ListGuildMembersData structure.
		/// \returns A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<std::vector<GuildMember>> listGuildMembersAsync(ListGuildMembersData dataPackage);

		/// \brief Searches for a list of GuildMembers of a chosen Guild.
		/// \param dataPackage A SearchGuildMembersData structure.
		/// \returns A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<std::vector<GuildMember>> searchGuildMembersAsync(SearchGuildMembersData dataPackage);

		/// \brief Adds a GuildMember to a chosen Guild.
		/// \param dataPackage An AddGuildMemberData structure.
		/// \returns A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<GuildMember> addGuildMemberAsync(AddGuildMemberData dataPackage);

		/// \brief Modifies a GuildMember's properties.
		/// \param dataPackage A ModifyGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage);

		/// \brief Modifies the current GuildMember's properties.
		/// \param dataPackage A ModifyCurrentGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage);

		/// \brief Removes a chosen GuildMember from a chosen Guild.
		/// \param dataPackage A RemoveGuildMemberData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildMemberAsync(RemoveGuildMemberData dataPackage);

		/// \brief Times-out a chosen GuildMember from a chosen Guild.
		/// \param dataPackage A TimeoutGuildMemberData structure.
		/// \returns A CoRoutine containing GuildMember.
		static CoRoutine<GuildMember> timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage);

		static VoiceStateDataLight getVoiceStateData(const GuildMemberKey& voiceState);

		static VoiceStateDataLight& insertVoiceState(VoiceStateDataLight&& voiceState);

		static ObjectCache<GuildMemberKey, GuildMemberData>& getCache();

		static void removeVoiceState(const GuildMemberKey& voiceState);

		static GuildMemberData& insertGuildMember(GuildMemberData&& guildMember);

		static void removeGuildMember(const GuildMemberKey& guildMemberId);

		static bool doWeCacheGuildMembers();

	  protected:
		static ObjectCache<GuildMemberKey, VoiceStateDataLight> vsCache;
		static ObjectCache<GuildMemberKey, GuildMemberData> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static bool doWeCacheGuildMembersBool;
	};
	/**@}*/
};
