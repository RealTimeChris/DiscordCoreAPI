// GuildMemberEntities.hpp - Header for the GuildMember related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// For modifying a GuildMember's values. \brief For modifying a GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyGuildMemberData {
		string newVoiceChannelId{ "" }; ///< The new voice channel to move them into.
		string currentChannelId{ "" }; ///< The current voice channel, if applicaple.
		string guildMemberId{ "" };	///< The user id of the desired Guild memeber.
		vector<string> roleIds{}; ///<A collection of role id's to be applied to them.
		string guildId{ "" }; ///< The id of the Guild for which you would like to modify a member.
		string reason{ "" };///< Reason for modifying this GuildMember.
		bool mute{ false }; ///< Whether or not to mute them in voice.
		bool deaf{ false };	///< Whether or not to deafen them, in voice.
		string nick{ "" };	///< Their new display/nick name.
	};

	/// For getting a GuildMember, from the library's cache or Discord server. \brief For getting a GuildMember, from the library's cache or Discord server.
	struct DiscordCoreAPI_Dll GetGuildMemberData {
		string guildMemberId{ "" };///< The user id of the desired GuildMember.
		string guildId{ "" };///< The id of the Guild from which you would like to acquire a member.
	};

	/// For listing the GuildMembers of a chosen Guild. \brief For listing the GuildMembers of a chosen Guild.
	struct ListGuildMembersData {
		string guildId{ "" };///< Guild from which to list the GuildMembers.
		__int32 limit{ 0 };///< Max number of members to return (1 - 1000).
		string after{ "" };///< The highest user id in the previous page.
	};

	/// For searching for one or more GuildMembers within a chosen Guild. \brief For searching for one or more GuildMembers within a chosen Guild.
	struct SearchGuildMembersData {
		string guildId{ "" };///< Guild within which to search for the GuildMembers.
		string query{ "" };///< Query string to match username(s) and nickname(s) against.
		__int32 limit{ 0 };///< Max number of members to return (1 - 1000).
	};

	/// A single GuildMember. \brief A single GuildMember.
	class DiscordCoreAPI_Dll GuildMember : public GuildMemberData {
	public:
		GuildMember(GuildMemberData dataNew, string guildIdNew) noexcept;

		GuildMember(GuildMemberData dataNew) noexcept;

	protected:

		friend struct Concurrency::details::_ResultHolder<GuildMember>;
		friend class DiscordCoreInternal::HttpRequestAgent;
		template <typename returnVal>
		friend class CoRoutine;
		friend struct GetGuildMemberRolesData;
		friend struct OnGuildMemberUpdateData;
		friend struct OnGuildMemberAddData;
		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class GuildMembers;
		friend class Guild;

		GuildMember() noexcept;
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the GuildMember related Discord endpoints. \brief An interface class for the GuildMember related Discord endpoints.
	class DiscordCoreAPI_Dll GuildMembers {
	public:

		friend class EventHandler;
		friend class Guild;

		/// Collects a GuildMember from the library's cache. \brief Collects a GuildMember from the library's cache.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> getCachedGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Collects a GuildMember from the Discord servers. \brief Collects a GuildMember from the Discord servers.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Modify's a GuildMember's properties. \brief Modify's a GuildMember's properties.
		/// \param dataPackage A ModifyGuildMemberData structure.
		/// \returns A CoRoutine containing a GuildMember.
		static CoRoutine<GuildMember> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage);

		/// Lists all of the GuildMembers of a chosen Guild. \brief Lists all of the GuildMembers of a chosen Guild.
		/// \param dataPackage A ListGuildMembersData structure.
		/// \returns A CoRoutine containing a vector of GuildMembers.
		static CoRoutine<vector<GuildMember>> listGuildMembersAsync(ListGuildMembersData dataPackage);

		/// Searches for a list of GuildMembers of a chosen Guild. \brief Searches for a list of GuildMembers of a chosen Guild.
		/// \param dataPackage A ListGuildMembersData structure.
		/// \returns A CoRoutine containing a vector of GuildMembers.
		static CoRoutine<vector<GuildMember>> searchGuildMembersAsync(SearchGuildMembersData dataPackage);

	protected:
		static shared_ptr<concurrent_unordered_map<string, GuildMember>> cache;

		static void insertGuildMember(GuildMember dataPackage);

		static void removeGuildMember(GuildMember dataPackage);
	};
	/**@}*/
};