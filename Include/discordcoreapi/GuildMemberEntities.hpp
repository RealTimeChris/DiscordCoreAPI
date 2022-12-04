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
/// GuildMemberEntities.hpp - Header for the GuildMember related classes and
/// structs. May 13, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildMemberEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	inline bool operator==(const VoiceStateDataLight& lhs, const VoiceStateDataLight& rhs) {
		return (lhs.guildId == rhs.guildId) && (lhs.userId == rhs.userId);
	}

	inline bool operator==(const GuildMemberData& lhs, const GuildMemberData& rhs) {
		return (lhs.guildId == rhs.guildId) && (lhs.id == rhs.id);
	}

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

		operator Jsonifier();
	};

	/// \brief For modifying the current GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyCurrentGuildMemberData {
		Snowflake guildId{};///< The Guild within which to modify the current user's values.
		std::string reason{};///< A reason for modifying the current user's values.
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

		operator Jsonifier();
	};

	/// \brief For removing a GuildMember from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildMemberData {
		Snowflake guildMemberId{};///< Id of the chosen GuildMember to kick.
		Snowflake guildId{};///< Guild from which to kick the chosen GuildMember.
		std::string reason{};///< Reason for kicking the GuildMember.
	};

	/// \brief For timing out a GuildMember.
	struct DiscordCoreAPI_Dll TimeoutGuildMemberData {
		TimeoutDurations numOfMinutesToTimeoutFor{};///< The number of minutes to time-out the GuildMember for.
		Snowflake guildMemberId{};///< The id of the GuildMember to be timed-out.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire a member.
		std::string reason{};///< Reason for timing them out.
	};

	/// \brief A single GuildMember.
	class DiscordCoreAPI_Dll GuildMember : public GuildMemberData {
	  public:
		TimeStamp communicationDisabledUntil{};///< When the user's timeout will expire and the user will be able to communicate in the guild again.
		std::string premiumSince{};///< If applicable, when they first boosted the server.

		GuildMember() noexcept = default;

		GuildMember& operator=(GuildMemberData&&) noexcept;

		GuildMember(GuildMemberData&&) noexcept;

		GuildMember& operator=(const GuildMemberData&) noexcept;

		GuildMember(const GuildMemberData&) noexcept;

		GuildMember(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildMember() noexcept = default;
	};

	class DiscordCoreAPI_Dll GuildMemberVector {
	  public:
		GuildMemberVector() noexcept = default;

		operator std::vector<GuildMember>();

		GuildMemberVector(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildMemberVector() noexcept = default;

	  protected:
		std::vector<GuildMember> guildMembers{};
	};

	/**@}*/


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

		static ObjectCache<VoiceStateDataLight>& getVsCache();

		static ObjectCache<GuildMemberData>& getCache();

		static void insertVoiceState(VoiceStateData voiceState);

		static void removeVoiceState(VoiceStateData voiceState);

		static void insertGuildMember(GuildMemberData guildMember);

		static void removeGuildMember(GuildMemberData guildMemberId);

		static bool doWeCacheGuildMembers();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<VoiceStateDataLight> vsCache;
		static ObjectCache<GuildMemberData> cache;
		static bool doWeCacheGuildMembersBool;
	};
	/**@}*/
};// namespace DiscordCoreAPI