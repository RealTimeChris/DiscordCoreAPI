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

	/// For getting a GuildMember, from the library's cache or Discord server. \brief For getting a GuildMember, from the library's cache or Discord server.
	struct DiscordCoreAPI_Dll GetGuildMemberData {
		string guildMemberId{ "" };///< The user id of the desired GuildMember.
		string guildId{ "" };///< The id of the Guild from which you would like to acquire a member.
	};

	/// For listing the GuildMembers of a chosen Guild. \brief For listing the GuildMembers of a chosen Guild.
	struct DiscordCoreAPI_Dll ListGuildMembersData {
		string guildId{ "" };///< Guild from which to list the GuildMembers.
		int32_t limit{ 0 };///< Max number of members to return (1 - 1000).
		string after{ "" };///< The highest user id in the previous page.
	};

	/// For searching for one or more GuildMembers within a chosen Guild. \brief For searching for one or more GuildMembers within a chosen Guild.
	struct DiscordCoreAPI_Dll SearchGuildMembersData {
		string guildId{ "" };///< Guild within which to search for the GuildMembers.
		string query{ "" };///< Query string to match username(s) and nickname(s) against.
		int32_t limit{ 0 };///< Max number of members to return (1 - 1000).
	};

	/// For adding a new GuildMember to a chosen Guild. \brief For adding a new GuildMember to a chosen Guild.
	struct DiscordCoreAPI_Dll AddGuildMemberData {
		string accessToken{ "" };///< An oauth2 access token granted with the guilds.join to the bot's application for the user you want to add to the Guild.
		vector<string>roles{};///< Array of role ids the member is assigned.
		string guildId{ "" };///< The Guild to add the new GuildMember to.
		string userId{ "" };///< The User id of the user you wish to add.
		string nick{ "" };///< Value to set users nickname to.
		bool mute{};///< Whether the user is muted in voice channels.
		bool deaf{};///< Whether the user is deafened in voice channels.
	};

	/// For modifying the Current GuildMember's values. \brief For modifying the Current GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyCurrentGuildMemberData {
		string guildId{ "" };///< The Guild within which to modify the current user's values.
		string reason{ "" };///< A reason for modifying the current user's values.
		string nick{ "" };///< A new nickname for the current user.
	};

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

	/// For removing a GuildMember from a chosen Guild. \brief For removing a GuildMember from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildMemberData {
		string guildMemberId{ "" };///< Id of the chosen GuildMember to kick.
		string guildId{ "" };///< Guild from which to kick the chosen GuildMember.
		string reason{ "" };///< Reason for kicking the GuildMember.
	};

	/// A single GuildMember. \brief A single GuildMember.
	class DiscordCoreAPI_Dll GuildMember : public GuildMemberData {
	public:

		GuildMember();

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

		static shared_ptr<DiscordCoreInternal::HttpClient> httpClient;
		static void initialize(shared_ptr<DiscordCoreInternal::HttpClient>);

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
		/// \returns A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<vector<GuildMember>> listGuildMembersAsync(ListGuildMembersData dataPackage);

		/// Searches for a list of GuildMembers of a chosen Guild. \brief Searches for a list of GuildMembers of a chosen Guild.
		/// \param dataPackage A SearchGuildMembersData structure.
		/// \returns A CoRoutine containing a vector<GuildMembers>.
		static CoRoutine<vector<GuildMember>> searchGuildMembersAsync(SearchGuildMembersData dataPackage);

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

	protected:
		static unordered_map<string, GuildMember> cache;

		static void insertGuildMember(GuildMember dataPackage);

		static void removeGuildMember(GuildMember dataPackage);
	};
	/**@}*/
};