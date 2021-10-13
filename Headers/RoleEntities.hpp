// RoleEntities.hpp - Header for the role related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_ENTITIES_
#define _ROLE_ENTITIES_

#include "IndexInitial.hpp"
#include "JSONifier.hpp"
#include "GuildMemberEntities.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// Get Guild roles data. \brief Get Guild roles data.
	struct DiscordCoreAPI_Dll GetGuildRolesData {
		string guildId{ "" };///< Guild id for which Guild to get the roles from.
	};

	/// Get Guild member roles data. \brief Get Guild member roles data.
	struct DiscordCoreAPI_Dll GetGuildMemberRolesData {
		GuildMember guildMember{};///< Which Guild member to collect the roles from.
		string guildId{ "" };///< Which Guild to collect their roles from.
	};

	/// Get a Role from the library's cache, or the Discord server. \brief Get a Role from the library's cache, or the Discord server.
	struct DiscordCoreAPI_Dll GetRoleData {
		string guildId{ "" };///<Which Guild to collect the Role from.
		string roleId{ "" };///< Which Role to collect.
	};

	/// Create Role data. \brief Create Role data.
	struct DiscordCoreAPI_Dll CreateRoleData {
		string hexColorValue{ "" };///< Hex color-value between 0 and ffffff.
		bool mentionable{ false };///< Is it mentionable by others in the Guild?
		string permissions{ "0" };///< The base permissions to give the Role.
		string guildId{ "" };///< Which Guild to make the Role in.
		bool hoist{ false };///< Is this Role hoisted above the rest of them?
		string name{ "" };///< The name of the Role.
		__int32 position{ 0 };///< The position amongst the other roles.
	};

	/// Update Role position data.
	struct DiscordCoreAPI_Dll UpdateRolePositionData {
		__int32 newPosition{ 0 };///< The new position of the Role.
		string guildId{ "" };///< The Guild within which to move the Role.
		string roleId{ "" };///< The id of the Role to move/
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

	/// Add Role to GuildMember data.
	struct DiscordCoreAPI_Dll AddRoleToGuildMemberData {
		string guildId{ "" };///< The id of the Guild within which to assign the Role.
		string userId{ "" };///< The id of the User to whom to assign the Role.
		string roleId{ "" };///< The id of the Role to be assigned.
	};

	/// Remove Role from GuildMember data.
	struct DiscordCoreAPI_Dll RemoveRoleFromGuildMemberData {
		string guildId{ "" };///< The id of the Guild within which to remove the Role.
		string userId{ "" };///< The id of the User from whom to remove the Role.
		string roleId{ "" };///< The id of the Role to be removed.
	};

	/// A single Role.
	class DiscordCoreAPI_Dll Role : public RoleData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Role>;
		friend class DiscordCoreInternal::RoleManagerAgent;
		friend class DiscordCoreInternal::RoleManager;
		friend struct OnRoleDeletionData;
		friend struct OnRoleCreationData;
		friend struct OnRoleUpdateData;
		friend class DiscordCoreClient;
		friend class Guild;

		Role();

		Role(RoleData dataNew);
	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll RoleManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class RoleManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<PatchRolePositionData> requestPatchGuildRolesBuffer{ nullptr };
		unbounded_buffer<DeleteGuildMemberRoleData> requestDeleteRoleBuffer{ nullptr };
		unbounded_buffer<DeleteGuildRoleData> requestDeleteGuildRoleBuffer{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Role>> outRolesBuffer{ nullptr };
		unbounded_buffer<PatchRoleData> requestPatchRoleBuffer{ nullptr };
		unbounded_buffer<PostRoleData> requestPostRoleBuffer{ nullptr };
		unbounded_buffer<GetRolesData> requestGetRolesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Role> outRoleBuffer{ nullptr };
		unbounded_buffer<PutRoleData> requestPutRoleBuffer{ nullptr };

		RoleManagerAgent();

		static void initialize();

		static void cleanup();

		vector<DiscordCoreAPI::Role> getObjectData(GetRolesData  dataPackage);

		DiscordCoreAPI::Role patchObjectData(PatchRoleData dataPackage);

		vector<DiscordCoreAPI::Role> patchObjectData(PatchRolePositionData dataPackage);

		DiscordCoreAPI::Role postObjectData(PostRoleData dataPackage);

		void putObjectData(PutRoleData dataPackage);

		void deleteObjectData(DeleteGuildMemberRoleData dataPackage);

		void deleteObjectData(DeleteGuildRoleData dataPackage);

		void run();
	};

	class DiscordCoreAPI_Dll RoleManager : ThreadContext {
	public:

		friend class DiscordCoreAPI::PermissionsConverter;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;
		friend class DiscordCoreAPI::Roles;

		RoleManager(RoleManager* pointer);

		~RoleManager();

	protected:

		overwrite_buffer<map<string, DiscordCoreAPI::Role>>* cache{};

		task<vector<DiscordCoreAPI::Role>> getGuildRolesAsync(DiscordCoreAPI::GetGuildRolesData dataPackage);

		vector<DiscordCoreAPI::Role> getGuildMemberRoles(DiscordCoreAPI::GetGuildMemberRolesData dataPackage);

		DiscordCoreAPI::Role getRole(DiscordCoreAPI::GetRoleData dataPackage);

		task<DiscordCoreAPI::Role> getCachedRoleAsync(DiscordCoreAPI::GetRoleData dataPackage);

		DiscordCoreAPI::Role createRole(DiscordCoreAPI::CreateRoleData dataPackage);

		vector<DiscordCoreAPI::Role> updateRolePositions(DiscordCoreAPI::UpdateRolePositionData dataPackage);
		
		task<DiscordCoreAPI::Role> updateRoleAsync(DiscordCoreAPI::UpdateRoleData dataPackage);

		task<void> removeRoleFromGuildAsync(DiscordCoreAPI::RemoveRoleFromGuildData dataPackage);

		task<void> addRoleToGuildMemberAsync(DiscordCoreAPI::AddRoleToGuildMemberData dataPackage);

		task<void> removeRoleFromGuildMemberAsync(DiscordCoreAPI::RemoveRoleFromGuildMemberData dataPackage);

		task<void> insertRoleAsync(DiscordCoreAPI::Role role);

		task<void> removeRoleAsync(string roleId);
	};
}
#endif