/*
*
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
/// RoleEntities.hpp - Header for the Role related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For adding a Role to a GuildMember. \brief For addin a Role to a GuildMember.
	struct DiscordCoreAPI_Dll AddGuildMemberRoleData {
		uint64_t guildId{};///< The id of the Guild within which to assign the Role.
		uint64_t userId{};///< The id of the User to whom to assign the Role.
		uint64_t roleId{};///< The id of the Role to be assigned.
		std::string reason{};///< Reason for adding the GuildMember's Role.
	};

	/// For removing a Role from a GuildMember. \brief For removing a Role from a GuildMember.
	struct DiscordCoreAPI_Dll RemoveGuildMemberRoleData {
		uint64_t guildId{};///< The id of the Guild within which to remove the Role.
		uint64_t userId{};///< The id of the User from whom to remove the Role.
		uint64_t roleId{};///< The id of the Role to be removed.
		std::string reason{};///< Reason for removing the GuildMember's Role.
	};

	/// For getting a chosen Guild's Roles. \brief For getting a chosen Guild's Roles.
	struct DiscordCoreAPI_Dll GetGuildRolesData {
		uint64_t guildId{};///< Guild id for which Guild to get the roles from.
	};

	/// For creating a new Role within a chosen Guild. \brief For creating a new Role within a chosen Guild.
	struct DiscordCoreAPI_Dll CreateGuildRoleData {
		std::string hexColorValue{};///< Hex color-value between 0 and ffffff.
		std::vector<uint8_t> icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		std::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		Permissions permissions{};///< The base permissions to give the Role.
		bool mentionable{ false };///< Is it mentionable by others in the Guild.
		int32_t position{ 0 };///< The position amongst the other roles.
		uint64_t guildId{};///< Which Guild to make the Role in.
		std::string reason{};///< Reason for creating the Role.
		bool hoist{ false };///< Is this Role hoisted above the rest of them?
		std::string name{};///< The name of the Role.
	};

	/// For updating the positions of the roles. \brief For updating the positions of the roles.
	struct DiscordCoreAPI_Dll RolePositionData {
		int32_t rolePosition{ 0 };///< The new Role position.
		uint64_t roleId{};///< The Role to update.
	};

	/// For updating the Role positions. \brief For updating the Role positions.
	struct DiscordCoreAPI_Dll ModifyGuildRolePositionsData {
		friend std::string DiscordCoreInternal::JSONIFY(const ModifyGuildRolePositionsData& dataPackage);
		friend Roles;

		uint64_t guildId{};///< The Guild within which to move the Role.
		int32_t newPosition{ 0 };///< The new position of the Role.
		uint64_t roleId{};///< The id of the Role to move.
		std::string reason{};///< Reason for modifying the Role positions.

	  protected:
		std::vector<RolePositionData> rolePositions;
	};

	/// For updating a Role's options within a chosen Guild. \brief For updating a Role's options within a chosen Guild.
	struct DiscordCoreAPI_Dll ModifyGuildRoleData {
		Permissions permissions{ "0" };///< Base Guild permissions for the Role.
		std::string hexColorValue{};///< A hex-color value between 0x00 and 0xFFFFFF.
		std::vector<uint8_t> icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		std::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		bool mentionable{ false };///< Is it mentionable?
		uint64_t guildId{};///< The id of the Guild within which to update the Role.
		uint64_t roleId{};///< The id of the Role to update.
		std::string reason{};///<< Reason for modifying the Role.
		std::string name{};///< What the name of the Role is going to be.
		bool hoist{ false };///< Is this Role hoisted above the others?
	};

	/// For removing a Role from a chosen Guild. \brief For removing a Role from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildRoleData {
		uint64_t guildId{};///< The id of the Guild from which to remove the Role.
		uint64_t roleId{};///< The id of the Role to remove.
		std::string reason{};///< Reason for removing this Role.
	};

	/// For getting a chosen GuildMember's Roles. \brief For getting a chosen GuildMember's Roles.
	struct DiscordCoreAPI_Dll GetGuildMemberRolesData {
		GuildMember guildMember{};///< Which Guild member to collect the roles from.
		uint64_t guildId{};///< Which Guild to collect their roles from.
	};

	/// For getting a Role from the library's cache, or the Discord server. \brief For getting a Role from the library's cache, or the Discord server.
	struct DiscordCoreAPI_Dll GetRoleData {
		uint64_t guildId{};///< Which Guild to collect the Role from.
		uint64_t roleId{};///< Which Role to collect.
	};

	/// A single Role.
	class DiscordCoreAPI_Dll Role : public RoleData {
	  public:
		RoleTagsData tags{};///< Role tags for the Role.
		std::string icon{};///< Icon representing the Role.

		Role& operator=(RoleData&& other);

		Role(RoleData&& other);

		Role& operator=(RoleData& other);

		Role(RoleData& other);

		Role() = default;

		virtual ~Role() = default;
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the Role related Discord endpoints. \brief An interface class for the Role related Discord endpoints.
	class DiscordCoreAPI_Dll Roles {
	  public:
		template<typename ParseType> friend void DiscordCoreInternal::parseObject(const nlohmann::json&, ParseType&);
		friend DiscordCoreClient;
		friend EventHandler;
		friend Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, bool doWeCacheNew);

		/// Adds a Role to a chosen Guild member. \brief Adds a Role to a chosen Guild member.
		/// \param dataPackage An AddGuildMemberRoleData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage);

		/// Removes a given Role from a chosen GuildMember. \brief Removes a given Role from a chosen GuildMember.
		/// \param dataPackage A RemoveGuildMemberRoleData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage);

		/// Collects the Roles that a Guild has. \brief Collects the Roles that a Guild has.
		/// \param dataPackage A GetGuildRolesData structure.
		/// \returns A CoRoutine containing a std::vector<Role>.
		static CoRoutine<std::vector<Role>> getGuildRolesAsync(GetGuildRolesData dataPackage);

		/// Creates a new Role within the given Guild. \brief Creates a new Role within the given Guild.
		/// \param dataPackage A CreateGuildRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> createGuildRoleAsync(CreateGuildRoleData dataPackage);

		/// Updates a Role's positions. \brief Updates a Role's positions.
		/// \param dataPackage A ModifyGuildRolePositionsData structure.
		/// \returns A CoRoutine containing a std::vector<Role>.
		static CoRoutine<std::vector<Role>> modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage);

		/// Updates a given Role's properties. \brief Updates a given Role's properties.
		/// \param dataPackage A ModifyGuildRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> modifyGuildRoleAsync(ModifyGuildRoleData dataPackage);

		/// Removes a given Role from a Guild. \brief Removes a given Role from a Guild.
		/// \param dataPackage A RemoveGuildRoleData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildRoleAsync(RemoveGuildRoleData dataPackage);

		/// Collects the Roles that a GuildMember has. \brief Collects the Roles that a GuildMember has.
		/// \param dataPackage A GetGuildMemberRolesData structure.
		/// \returns A CoRoutine containing a std::vector<Role>.
		static CoRoutine<std::vector<Role>> getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage);

		/// Collects a Role from the Discord servers. \brief Collects a Role from the Discord servers.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> getRoleAsync(GetRoleData dataPackage);

		/// Collects a given Role from the library's cache. \brief Collects a given Role from the library's cache.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<RoleData> getCachedRoleAsync(GetRoleData dataPackage);

	  protected:
		static std::unique_ptr<std::unordered_map<uint64_t, std::unique_ptr<RoleData>>> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static std::shared_mutex theMutex;
		static bool doWeCache;

		static void insertRole(RoleData dataPackage);

		static void removeRole(const uint64_t& roleId);
	};
	/**@}*/

}// namespace DiscordCoreAPI
