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
/// GuildMemberEntities.hpp - Header for the GuildMember related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildMemberEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For getting a GuildMember, from the library's cache or Discord server. \brief For getting a GuildMember, from the library's cache or Discord server.
	struct DiscordCoreAPI_Dll GetGuildMemberData {
		Snowflake guildMemberId{};///< The user id of the desired GuildMember.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire a member.
	};

	/// For listing the GuildMembers of a chosen Guild. \brief For listing the GuildMembers of a chosen Guild.
	struct DiscordCoreAPI_Dll ListGuildMembersData {
		Snowflake guildId{};///< Guild from which to list the GuildMembers.
		Snowflake after{};///< The highest user id in the previous page.
		int32_t limit{ 0 };///< Max number of members to return (1 - 1000).
	};

	/// For searching for one or more GuildMembers within a chosen Guild. \brief For searching for one or more GuildMembers within a chosen Guild.
	struct DiscordCoreAPI_Dll SearchGuildMembersData {
		Snowflake guildId{};///< Guild within which to search for the GuildMembers.
		std::string query{};///< Query std::string to match userName(s) and nickname(s) against.
		int32_t limit{ 0 };///< Max number of members to return (1 - 1000).
	};

	/// For adding a new GuildMember to a chosen Guild. \brief For adding a new GuildMember to a chosen Guild.
	struct DiscordCoreAPI_Dll AddGuildMemberData {
		std::vector<Snowflake> roles{};///< Array of Role ids the member is assigned.
		std::string accessToken{};///< An oauth2 access token granted with the guilds.join to the bot's application for the user you want to add.
		Snowflake guildId{};///< The Guild to add the new GuildMember to.
		Snowflake userId{};///< The User id of the user you wish to add.
		std::string nick{};///< Value to set users nickname to.
		bool mute{};///< Whether the user is muted in voice channels.
		bool deaf{};///< Whether the user is deafened in voice channels.

		operator std::string();
	};

	/// For modifying the Current GuildMember's values. \brief For modifying the current GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyCurrentGuildMemberData {
		Snowflake guildId{};///< The Guild within which to modify the current user's values.
		std::string reason{};///< A reason for modifying the current user's values.
		std::string nick{};///< A new nickname for the current user.
	};

	/// For modifying a GuildMember's values. \brief For modifying a GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyGuildMemberData {
		TimeStamp<std::chrono::milliseconds> communicationDisabledUntil{};///< When the user's timeout will expire and the user will be able to communicate in the guild again.
		std::vector<Snowflake> roleIds{};///< A collection of Role id's to be applied to them.
		Snowflake newVoiceChannelId{};///< The new voice Channel to move them into.
		Snowflake currentChannelId{};///< The current voice Channel, if applicaple.
		Snowflake guildMemberId{};///< The user id of the desired Guild memeber.
		Snowflake guildId{};///< The id of the Guild for which you would like to modify a member.
		std::string reason{};///< Reason for modifying this GuildMember.
		std::string nick{};///< Their new display/nick name.
		bool mute{ false };///< Whether or not to mute them in voice.
		bool deaf{ false };///< Whether or not to deafen them, in voice.

		operator std::string();
	};

	/// For removing a GuildMember from a chosen Guild. \brief For removing a GuildMember from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildMemberData {
		Snowflake guildMemberId{};///< Id of the chosen GuildMember to kick.
		Snowflake guildId{};///< Guild from which to kick the chosen GuildMember.
		std::string reason{};///< Reason for kicking the GuildMember.
	};

	/// For timing out a GuildMember. \brief For timing out a GuildMember.
	struct DiscordCoreAPI_Dll TimeoutGuildMemberData {
		TimeoutDurations numOfMinutesToTimeoutFor{};///< The number of minutes to time-out the GuildMember for.
		Snowflake guildMemberId{};///< The id of the GuildMember to be timed-out.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire a member.
		std::string reason{};///< Reason for timing them out.
	};

	/// A single GuildMember. \brief A single GuildMember.
	class DiscordCoreAPI_Dll GuildMember : public GuildMemberData, public DataParser<GuildMember> {
	  public:
		TimeStamp<std::chrono::milliseconds> communicationDisabledUntil{};///< When the user's timeout will expire and the user will be able to communicate in the guild again.
		std::string premiumSince{};///< If applicable, when they first boosted the server.
		std::string avatar{};///< The member's guild avatar hash.

		GuildMember& operator=(GuildMemberData&&);

		GuildMember(GuildMemberData&&);

		GuildMember& operator=(GuildMemberData&);

		GuildMember(GuildMemberData&);

		GuildMember() = default;

		GuildMember& operator=(const nlohmann::json& jsonObjectData);

		GuildMember(const nlohmann::json& jsonObjectData);

		~GuildMember() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildMember* pDataStructure);
	};

	class DiscordCoreAPI_Dll GuildMemberVector : public DataParser<GuildMemberVector> {
	  public:
		GuildMemberVector() = default;

		operator std::vector<GuildMember>();

		GuildMemberVector& operator=(const nlohmann::json& jsonObjectData);

		GuildMemberVector(const nlohmann::json& jsonObjectData);

		virtual ~GuildMemberVector() = default;

	  protected:
		std::vector<GuildMember> theGuildMembers{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildMemberVector* pDataStructure);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the GuildMember related Discord endpoints. \brief An interface class for the GuildMember related Discord endpoints.
	class DiscordCoreAPI_Dll GuildMembers {
	  public:
		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

		/// Collects a GuildMember from the Discord servers. \brief Collects a GuildMember from the Discord servers.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Collects a GuildMember from the library's cache. \brief Collects a GuildMember from the library's cache.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMemberData> getCachedGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Lists all of the GuildMembers of a chosen Guild. \brief Lists all of the GuildMembers of a chosen Guild.
		/// \param dataPackage A ListGuildMembersData structure.
		/// \returns A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<std::vector<GuildMember>> listGuildMembersAsync(ListGuildMembersData dataPackage);

		/// Searches for a list of GuildMembers of a chosen Guild. \brief Searches for a list of GuildMembers of a chosen Guild.
		/// \param dataPackage A SearchGuildMembersData structure.
		/// \returns A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<std::vector<GuildMember>> searchGuildMembersAsync(SearchGuildMembersData dataPackage);

		/// Adds a GuildMember to a chosen Guild. \brief Adds a GuildMember to a chosen Guild.
		/// \param dataPackage An AddGuildMemberData structure.
		/// \returns A CoRoutine containing a vector<GuildMembers>.
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

		static void insertGuildMember(GuildMemberData dataPackage);

		static void removeGuildMember(GuildMemberData& globalId);

	  protected:
		static std::unique_ptr<std::map<GuildMemberId, std::unique_ptr<GuildMemberData>>> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ConfigManager* configManager;
		static std::shared_mutex theMutex;
	};
	/**@}*/
};// namespace DiscordCoreAPI
