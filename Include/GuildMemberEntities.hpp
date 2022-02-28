/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// GuildMemberEntities.hpp - Header for the GuildMember related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file GuildMemberEntities.hpp

#pragma once

#include <FoundationEntities.hpp>

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For getting a GuildMember, from the library's cache or Discord server. \brief For getting a GuildMember, from the library's cache or Discord server.
	struct DiscordCoreAPI_Dll GetGuildMemberData {
		std::string guildMemberId{ "" };///< The user id of the desired GuildMember.
		std::string guildId{ "" };///< The id of the Guild from which you would like to acquire a member.
	};

	/// For listing the GuildMembers of a chosen Guild. \brief For listing the GuildMembers of a chosen Guild.
	struct DiscordCoreAPI_Dll ListGuildMembersData {
		std::string guildId{ "" };///< Guild from which to list the GuildMembers.
		std::string after{ "" };///< The highest user id in the previous page.
		int32_t limit{ 0 };///< Max number of members to return (1 - 1000).
	};

	/// For searching for one or more GuildMembers within a chosen Guild. \brief For searching for one or more GuildMembers within a chosen Guild.
	struct DiscordCoreAPI_Dll SearchGuildMembersData {
		std::string guildId{ "" };///< Guild within which to search for the GuildMembers.
		std::string query{ "" };///< Query std::string to match username(s) and nickname(s) against.
		int32_t limit{ 0 };///< Max number of members to return (1 - 1000).
	};

	/// For adding a new GuildMember to a chosen Guild. \brief For adding a new GuildMember to a chosen Guild.
	struct DiscordCoreAPI_Dll AddGuildMemberData {
		std::vector<std::string>roles{};///< Array of Role ids the member is assigned.
		std::string accessToken{ "" };///< An oauth2 access token granted with the guilds.join to the bot's application for the user you want to add to the Guild.
		std::string guildId{ "" };///< The Guild to add the new GuildMember to.
		std::string userId{ "" };///< The User id of the user you wish to add.
		std::string nick{ "" };///< Value to set users nickname to.
		bool mute{};///< Whether the user is muted in voice channels.
		bool deaf{};///< Whether the user is deafened in voice channels.
	};

	/// For modifying the Current GuildMember's values. \brief For modifying the Current GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyCurrentGuildMemberData {
		std::string guildId{ "" };///< The Guild within which to modify the current user's values.
		std::string reason{ "" };///< A reason for modifying the current user's values.
		std::string nick{ "" };///< A new nickname for the current user.
	};

	/// For modifying a GuildMember's values. \brief For modifying a GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyGuildMemberData {
		TimeStamp communicationDisabledUntil{ "" };///< When the user's timeout will expire and the user will be able to communicate in the guild again, null or a time in the past if the user is not timed out.
		std::string newVoiceChannelId{ "" };///< The new voice Channel to std::move them into.
		std::string currentChannelId{ "" };///< The current voice Channel, if applicaple.
		std::vector<std::string> roleIds{};///<A collection of Role id's to be applied to them.
		std::string guildMemberId{ "" };///< The user id of the desired Guild memeber.
		std::string guildId{ "" };///< The id of the Guild for which you would like to modify a member.
		std::string reason{ "" };///< Reason for modifying this GuildMember.
		std::string nick{ "" };///< Their new display/nick name.
		bool mute{ false };///< Whether or not to mute them in voice.
		bool deaf{ false };///< Whether or not to deafen them, in voice.
	};

	/// For removing a GuildMember from a chosen Guild. \brief For removing a GuildMember from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildMemberData {
		std::string guildMemberId{ "" };///< Id of the chosen GuildMember to kick.
		std::string guildId{ "" };///< Guild from which to kick the chosen GuildMember.
		std::string reason{ "" };///< Reason for kicking the GuildMember.
	};

	/// For timing out a GuildMember. \brief For timing out a GuildMember.
	struct DiscordCoreAPI_Dll TimeoutGuildMemberData {
		int32_t numOfMinutesToTimeoutFor{ 0 };///< The number of minutes to time-out the GuildMember for.
		std::string guildMemberId{ "" };///< The id of the GuildMember to be timed-out.
		std::string guildId{ "" };///< The id of the Guild from which you would like to acquire a member.
		std::string reason{ "" };///< Reason for timing them out.
	};

	/// A single GuildMember. \brief A single GuildMember.
	class DiscordCoreAPI_Dll GuildMember : public GuildMemberData {
	public:

		GuildMember() = default;

		GuildMember(GuildMemberData dataNew);
	};
	/**@}*/

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class for the GuildMember related Discord endpoints. \brief An interface class for the GuildMember related Discord endpoints.
	class DiscordCoreAPI_Dll GuildMembers {
	public:

		friend DiscordCoreClient;
		friend EventHandler;
		friend Guild;

		static void initialize(DiscordCoreInternal::HttpClient*);

		/// Collects a GuildMember from the Discord servers. \brief Collects a GuildMember from the Discord servers.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Collects a GuildMember from the library's cache. \brief Collects a GuildMember from the library's cache.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> getCachedGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Lists all of the GuildMembers of a chosen Guild. \brief Lists all of the GuildMembers of a chosen Guild.
		/// \param dataPackage A ListGuildMembersData structure.
		/// \returns A CoRoutine containing a std::vector<GuildMembers>.
		static CoRoutine<std::vector<GuildMember>> listGuildMembersAsync(ListGuildMembersData dataPackage);

		/// Searches for a list of GuildMembers of a chosen Guild. \brief Searches for a list of GuildMembers of a chosen Guild.
		/// \param dataPackage A SearchGuildMembersData structure.
		/// \returns A CoRoutine containing a std::vector<GuildMembers>.
		static CoRoutine<std::vector<GuildMember>> searchGuildMembersAsync(SearchGuildMembersData dataPackage);

		/// Adds a GuildMember to a chosen Guild. \brief Adds a GuildMember to a chosen Guild.
		/// \param dataPackage An AddGuildMemberData structure.
		/// \returns A CoRoutine containing a std::vector<GuildMembers>.
		static CoRoutine<GuildMember> addGuildMemberAsync(AddGuildMemberData dataPackage);

		/// Modifies a GuildMember's properties. \brief Modifies a GuildMember's properties.
		/// \param dataPackage A ModifyGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage);

		/// Modifies the current GuildMember's properties. \brief Modifies the current GuildMember's properties.
		/// \param dataPackage A ModifyCurrentGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage);

		/// Removes a chosen GuildMember from a chosen Guild. \brief Removes a chosen GuildMember from a chosen Guild.
		/// \param dataPackage A RemoveGuildMemberData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildMemberAsync(RemoveGuildMemberData dataPackage);

		/// Times-out a chosen GuildMember from a chosen Guild. \brief Times-out a chosen GuildMember from a chosen Guild.
		/// \param dataPackage A TimeoutGuildMemberData structure.
		/// \returns A CoRoutine containing GuildMember.
		static CoRoutine<GuildMember> timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage);

	protected:

		static std::unordered_map<std::string, GuildMember> cache;
		static DiscordCoreInternal::HttpClient* httpClient;

		static void insertGuildMember(GuildMember dataPackage);

		static void removeGuildMember(GuildMember dataPackage);
	};
	/**@}*/
};
