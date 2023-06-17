/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// RoleEntities.hpp - Header for the Role related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief For addin a Role to a GuildMember.
	struct DiscordCoreAPI_Dll AddGuildMemberRoleData {
		Snowflake guildId{};///< The id of the Guild within which to assign the Role.
		std::string reason{};///< Reason for adding the GuildMember's Role.
		Snowflake userId{};///< The id of the User to whom to assign the Role.
		Snowflake roleId{};///< The id of the Role to be assigned.
	};

	/// \brief For removing a Role from a GuildMember.
	struct DiscordCoreAPI_Dll RemoveGuildMemberRoleData {
		std::string reason{};///< Reason for removing the GuildMember's Role.
		Snowflake guildId{};///< The id of the Guild within which to remove the Role.
		Snowflake userId{};///< The id of the User from whom to remove the Role.
		Snowflake roleId{};///< The id of the Role to be removed.
	};

	/// \brief For getting a chosen Guild's Roles.
	struct DiscordCoreAPI_Dll GetGuildRolesData {
		Snowflake guildId{};///< Guild id for which Guild to get the roles from.
	};

	/// \brief For creating a new Role within a chosen Guild.
	struct DiscordCoreAPI_Dll CreateGuildRoleData {
		std::string hexColorValue{};///< Hex color-value between 0 and ffffff.
		std::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		Permissions permissions{};///< The base permissions to give the Role.
		std::string reason{};///< Reason for creating the Role.
		Snowflake guildId{};///< Which Guild to make the Role in.
		std::string icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		bool mentionable{};///< Is it mentionable by others in the Guild.
		int32_t position{};///< The position amongst the other roles.
		std::string name{};///< The name of the Role.
		bool hoist{};///< Is this Role hoisted above the rest of them?
	};

	/// \brief For updating the positions of the roles.
	struct DiscordCoreAPI_Dll RolePositionData {
		int32_t rolePosition{};///< The new Role position.
		Snowflake roleId{};///< The Role to update.
	};

	/// \brief For updating the Role positions.
	struct DiscordCoreAPI_Dll ModifyGuildRolePositionsData {
		friend struct DiscordCoreAPI_Dll Jsonifier::Core<ModifyGuildRolePositionsData>;
		friend class DiscordCoreAPI_Dll Roles;

		int32_t newPosition{};///< The new position of the Role.
		std::string reason{};///< Reason for modifying the Role positions.
		Snowflake guildId{};///< The Guild within which to move the Role.
		Snowflake roleId{};///< The id of the Role to move.

	  protected:
		std::vector<RolePositionData> rolePositions;
	};

	/// \brief For updating a Role's options within a chosen Guild.
	struct DiscordCoreAPI_Dll ModifyGuildRoleData {
		Permissions permissions{ "0" };///< Base Guild permissions for the Role.
		std::string hexColorValue{};///< A hex-color value between 0x00 and 0xFFFFFF.
		std::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		std::string reason{};///<< Reason for modifying the Role.
		Snowflake guildId{};///< The id of the Guild within which to update the Role.
		std::string icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		bool mentionable{};///< Is it mentionable?
		Snowflake roleId{};///< The id of the Role to update.
		std::string name{};///< What the name of the Role is going to be.
		bool hoist{};///< Is this Role hoisted above the others?
	};

	/// \brief For removing a Role from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildRoleData {
		std::string reason{};///< Reason for removing this Role.
		Snowflake guildId{};///< The id of the Guild from which to remove the Role.
		Snowflake roleId{};///< The id of the Role to remove.
	};

	/// \brief For getting a chosen GuildMember's Roles.
	struct DiscordCoreAPI_Dll GetGuildMemberRolesData {
		GuildMember guildMember{};///< Which Guild member to collect the roles from.
		Snowflake guildId{};///< Which Guild to collect their roles from.
	};

	/// \brief For getting a Role from the library's cache, or the Discord server.
	struct DiscordCoreAPI_Dll GetRoleData {
		Snowflake guildId{};///< Which Guild to collect the Role from.
		Snowflake roleId{};///< Which Role to collect.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the Role related Discord endpoints.
	class DiscordCoreAPI_Dll Roles {
	  public:
		friend class DiscordCoreAPI_Dll DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreAPI_Dll DiscordCoreClient;
		friend class DiscordCoreAPI_Dll RoleData;
		friend class DiscordCoreAPI_Dll GuildData;
		friend class DiscordCoreAPI_Dll Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

		/// \brief Adds a Role to a chosen Guild member.
		/// \param dataPackage An AddGuildMemberRoleData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage);

		/// \brief Removes a given Role from a chosen GuildMember.
		/// \param dataPackage A RemoveGuildMemberRoleData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage);

		/// \brief Collects the Roles that a Guild has.
		/// \param dataPackage A GetGuildRolesData structure.
		/// \returns A CoRoutine containing a RoleVector.
		static CoRoutine<RoleVector> getGuildRolesAsync(GetGuildRolesData dataPackage);

		/// \brief Creates a new Role within the given Guild.
		/// \param dataPackage A CreateGuildRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> createGuildRoleAsync(CreateGuildRoleData dataPackage);

		/// \brief Updates a Role's positions.
		/// \param dataPackage A ModifyGuildRolePositionsData structure.
		/// \returns A CoRoutine containing a RoleVector.
		static CoRoutine<RoleVector> modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage);

		/// \brief Updates a given Role's properties.
		/// \param dataPackage A ModifyGuildRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> modifyGuildRoleAsync(ModifyGuildRoleData dataPackage);

		/// \brief Removes a given Role from a Guild.
		/// \param dataPackage A RemoveGuildRoleData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildRoleAsync(RemoveGuildRoleData dataPackage);

		/// \brief Collects the Roles that a GuildMember has.
		/// \param dataPackage A GetGuildMemberRolesData structure.
		/// \returns A CoRoutine containing a RoleVector.
		static CoRoutine<RoleVector> getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage);

		/// \brief Collects a Role from the Discord servers.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> getRoleAsync(GetRoleData dataPackage);

		/// \brief Collects a given Role from the library's cache.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static RoleData getCachedRole(GetRoleData dataPackage);

		static RoleData& insertRole(RoleData&& role);

		static void removeRole(const Snowflake roleId);

		static bool doWeCacheRoles();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<Snowflake, RoleData> cache;
		static bool doWeCacheRolesBool;
	};
	/**@}*/

}
