// RoleEntities.hpp - Header for the role related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "GuildMemberEntities.hpp"
#include "JSONIfier.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// For addin a Role to a GuildMember. \brief For addin a Role to a GuildMember.
	struct DiscordCoreAPI_Dll AddGuildMemberRoleData {
		string guildId{ "" };///< The id of the Guild within which to assign the Role.
		string userId{ "" };///< The id of the User to whom to assign the Role.
		string roleId{ "" };///< The id of the Role to be assigned.
		string reason{ "" };///< Reason for adding the GuildMember's Role.
	};

	/// For removing a Role from a GuildMember. \brief For removing a Role from a GuildMember.
	struct DiscordCoreAPI_Dll RemoveGuildMemberRoleData {
		string guildId{ "" };///< The id of the Guild within which to remove the Role.
		string userId{ "" };///< The id of the User from whom to remove the Role.
		string roleId{ "" };///< The id of the Role to be removed.
		string reason{ "" };///< Reason for removing the GuildMember's Role.
	};

	/// Get Guild roles data. \brief Get Guild roles data.
	struct DiscordCoreAPI_Dll GetGuildRolesData {
		string guildId{ "" };///< Guild id for which Guild to get the roles from.
	};

	/// Get Guild member roles data. \brief Get Guild member roles data.
	struct DiscordCoreAPI_Dll GetGuildMemberRolesData {
		GuildMember guildMember{};///< Which Guild member to collect the roles from.
		string guildId{ "" };///< Which Guild to collect their roles from.
	};

	/// For updating the positions of the roles. \brief For updating the positions of the roles.
	struct DiscordCoreAPI_Dll RolePositionData {
		__int32 rolePosition{ 0 };
		string roleId{ "" };
	};

	/// Get a Role from the library's cache, or the Discord server. \brief Get a Role from the library's cache, or the Discord server.
	struct DiscordCoreAPI_Dll GetRoleData {
		string guildId{ "" };///<Which Guild to collect the Role from.
		string roleId{ "" };///< Which Role to collect.
	};

	/// Create Role data. \brief Create Role data.
	struct DiscordCoreAPI_Dll CreateGuildRoleData {
		string hexColorValue{ "" };///< Hex color-value between 0 and ffffff.
		bool mentionable{ false };///< Is it mentionable by others in the Guild?
		string permissions{ "0" };///< The base permissions to give the Role.
		__int32 position{ 0 };///< The position amongst the other roles.
		string guildId{ "" };///< Which Guild to make the Role in.
		bool hoist{ false };///< Is this Role hoisted above the rest of them?
		string reason{ "" };///< Reason for creating the Role.
		string name{ "" };///< The name of the Role.
	};

	/// Update Role position data.
	struct DiscordCoreAPI_Dll ModifyGuildRolePositionsData {
		friend string DiscordCoreInternal::JSONIFY(ModifyGuildRolePositionsData dataPackage);
		friend class Roles;
		__int32 newPosition{ 0 };///< The new position of the Role.
		string guildId{ "" };///< The Guild within which to move the Role.
		string roleId{ "" };///< The id of the Role to move/
		string reason{ "" };///< Reason for modifying the Role positions.
	protected:
		vector<RolePositionData> rolePositions;
	};

	/// Update Role data.
	struct DiscordCoreAPI_Dll UpdateRoleData {
		string hexColorValue{ "" };///< A hex-color value between 0x00 and 0xFFFFFF.
		bool mentionable{ false };///< Is it mentionable?
		string permissions{ "" };///< Base Guild permissions for the Role.
		string guildId{ "" };///< The id of the Guild within which to update the Role.
		string roleId{ "" };///< The id of the Role to update.
		bool hoist{ false };///< Is this Role hoisted above the others?
		string name{ "" };///< What the name of the Role is going to be.
	};

	/// Removes a Role from a given Guild.
	struct DiscordCoreAPI_Dll RemoveRoleFromGuildData {
		string guildId{ "" };///< The id of the Guild from which to remove the Role.
		string roleId{ "" };///< The id of the Role to remove.
	};

	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Role related Discord endpoints. \brief An interface class for the Role related Discord endpoints.
	class DiscordCoreAPI_Dll Roles {
	public:
		
		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Guild;

		/// Adds a role to a chosen Guild member. \brief Adds a Role to a chosen Guild member.
		/// \param dataPackage A AddRoleToGuildMemberData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage);

		/// Removes a given Role from a chosen GuildMember. \brief Removes a given Role from a chosen GuildMember.
		/// \param dataPackage A RemoveGuildMemberRoleData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage);

		/// Collects a given Role from the library's cache. \brief Collects a given Role from the library's cache.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> getCachedRoleAsync(GetRoleData dataPackage);

		/// Creates a new Role within the given Guild. \brief Creates a new Role within the given Guild.
		/// \param dataPackage A CreateRoleData structure.
		/// \returns A Role.
		static CoRoutine<Role> createGuildRoleAsync(CreateGuildRoleData dataPackage);

		/// Collects a Role from the Discord servers. \brief Collects a Role from the Discord servers.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A Role.
		static CoRoutine<Role> getRoleAsync(GetRoleData dataPackage);

		/// Collects the Roles that a GuildMember has. \brief Collects the Roles that a GuildMember has.
		/// \param dataPackage A GetGuildMemberRolesData structure.
		/// \returns A vector of Roles.
		static CoRoutine<vector<Role>> getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage);

		/// Collects the Roles that a Guild has. \brief Collects the Roles that a Guild has.
		/// \param dataPackage A GetGuildRolesData structure.
		/// \returns A CoRoutine containing a vector of Roles.
		static CoRoutine<vector<Role>> getGuildRolesAsync(GetGuildRolesData dataPackage);

		/// Removes a given Role from a Guild. \brief Removes a given Role from a Guild.
		/// \param dataPackage A RemoveRoleFromGuildData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeRoleFromGuildAsync(RemoveRoleFromGuildData dataPackage);

		/// Updates a given Role's properties. \brief Updates a given Role's properties.
		/// \param dataPackage A UpdateRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> updateRoleAsync(UpdateRoleData dataPackage);

		/// Updates a Role's positions. \brief Updates a Role's positions.
		/// \param dataPackage A UpdateRolePositionData structure.
		/// \returns A vector containing Roles.
		static CoRoutine<vector<Role>> modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage);

	protected:

		static unique_ptr<concurrent_unordered_map<string, Role>> cache;

		static void insertRole(Role dataPackage);

		static void removeRole(string roleId);
	};
	/**@}*/

}