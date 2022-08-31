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

	/// For adding a Role to a GuildMember. \brief For addin a Role to a GuildMember.
	struct DiscordCoreAPI_Dll AddGuildMemberRoleData {
		Snowflake guildId{};///< The id of the Guild within which to assign the Role.
		Snowflake userId{};///< The id of the User to whom to assign the Role.
		Snowflake roleId{};///< The id of the Role to be assigned.
		std::string reason{};///< Reason for adding the GuildMember's Role.
	};

	/// For removing a Role from a GuildMember. \brief For removing a Role from a GuildMember.
	struct DiscordCoreAPI_Dll RemoveGuildMemberRoleData {
		Snowflake guildId{};///< The id of the Guild within which to remove the Role.
		Snowflake userId{};///< The id of the User from whom to remove the Role.
		Snowflake roleId{};///< The id of the Role to be removed.
		std::string reason{};///< Reason for removing the GuildMember's Role.
	};

	/// For getting a chosen Guild's Roles. \brief For getting a chosen Guild's Roles.
	struct DiscordCoreAPI_Dll GetGuildRolesData {
		Snowflake guildId{};///< Guild id for which Guild to get the roles from.
	};

	/// For creating a new Role within a chosen Guild. \brief For creating a new Role within a chosen Guild.
	struct DiscordCoreAPI_Dll CreateGuildRoleData {
		std::string hexColorValue{};///< Hex color-value between 0 and ffffff.
		std::vector<uint8_t> icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		std::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		Permissions permissions{};///< The base permissions to give the Role.
		bool mentionable{ false };///< Is it mentionable by others in the Guild.
		int32_t position{ 0 };///< The position amongst the other roles.
		Snowflake guildId{};///< Which Guild to make the Role in.
		std::string reason{};///< Reason for creating the Role.
		bool hoist{ false };///< Is this Role hoisted above the rest of them?
		std::string name{};///< The name of the Role.

		operator std::string();
	};

	/// For updating the positions of the roles. \brief For updating the positions of the roles.
	struct DiscordCoreAPI_Dll RolePositionData {
		int32_t rolePosition{ 0 };///< The new Role position.
		Snowflake roleId{};///< The Role to update.
	};

	/// For updating the Role positions. \brief For updating the Role positions.
	struct DiscordCoreAPI_Dll ModifyGuildRolePositionsData {
		friend class Roles;

		Snowflake guildId{};///< The Guild within which to move the Role.
		int32_t newPosition{ 0 };///< The new position of the Role.
		Snowflake roleId{};///< The id of the Role to move.
		std::string reason{};///< Reason for modifying the Role positions.

		operator std::string();

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
		Snowflake guildId{};///< The id of the Guild within which to update the Role.
		Snowflake roleId{};///< The id of the Role to update.
		std::string reason{};///<< Reason for modifying the Role.
		std::string name{};///< What the name of the Role is going to be.
		bool hoist{ false };///< Is this Role hoisted above the others?

		operator std::string();
	};

	/// For removing a Role from a chosen Guild. \brief For removing a Role from a chosen Guild.
	struct DiscordCoreAPI_Dll RemoveGuildRoleData {
		Snowflake guildId{};///< The id of the Guild from which to remove the Role.
		Snowflake roleId{};///< The id of the Role to remove.
		std::string reason{};///< Reason for removing this Role.
	};

	/// For getting a chosen GuildMember's Roles. \brief For getting a chosen GuildMember's Roles.
	struct DiscordCoreAPI_Dll GetGuildMemberRolesData {
		GuildMember guildMember{};///< Which Guild member to collect the roles from.
		Snowflake guildId{};///< Which Guild to collect their roles from.
	};

	/// For getting a Role from the library's cache, or the Discord server. \brief For getting a Role from the library's cache, or the Discord server.
	struct DiscordCoreAPI_Dll GetRoleData {
		Snowflake guildId{};///< Which Guild to collect the Role from.
		Snowflake roleId{};///< Which Role to collect.
	};

	/// A single Role.
	class DiscordCoreAPI_Dll Role : public RoleData {
	  public:
		RoleTagsData tags{};///< Role tags for the Role.
		IconHash icon{};///< Icon representing the Role.

		Role() noexcept = default;

		Role& operator=(RoleData&& other) noexcept;

		Role(RoleData&& other) noexcept;

		Role& operator=(const RoleData& other) noexcept;

		Role(const RoleData& other) noexcept;

		virtual ~Role() noexcept = default;
	};

	template<> DiscordCoreAPI_Dll void parseObject(nlohmann::json& jsonObjectData, Role& theData);

	class DiscordCoreAPI_Dll RoleVector {
	  public:
		template<typename ReturnType> friend DiscordCoreAPI_Dll void parseObject(nlohmann::json& jsonObjectData, ReturnType& theData);

		RoleVector() noexcept = default;

		operator std::vector<Role>();

		virtual ~RoleVector() noexcept = default;

	  protected:
		std::vector<Role> theRoles{};
	};

	template<> DiscordCoreAPI_Dll void parseObject(nlohmann::json& jsonObjectData, RoleVector& theData);

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the Role related Discord endpoints. \brief An interface class for the Role related Discord endpoints.
	class DiscordCoreAPI_Dll Roles {
	  public:
		template<typename ReturnType> DiscordCoreAPI_Dll friend void parseObject(nlohmann::json& jsonObjectData, ReturnType& theData);
		friend class DiscordCoreInternal::WebSocketSSLShard;
		friend class DiscordCoreClient;
		friend class RoleData;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

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
		/// \returns A CoRoutine containing a RoleVector.
		static CoRoutine<std::vector<Role>> getGuildRolesAsync(GetGuildRolesData dataPackage);

		/// Creates a new Role within the given Guild. \brief Creates a new Role within the given Guild.
		/// \param dataPackage A CreateGuildRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> createGuildRoleAsync(CreateGuildRoleData dataPackage);

		/// Updates a Role's positions. \brief Updates a Role's positions.
		/// \param dataPackage A ModifyGuildRolePositionsData structure.
		/// \returns A CoRoutine containing a RoleVector.
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
		/// \returns A CoRoutine containing a RoleVector.
		static CoRoutine<std::vector<Role>> getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage);

		/// Collects a Role from the Discord servers. \brief Collects a Role from the Discord servers.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> getRoleAsync(GetRoleData dataPackage);

		/// Collects a given Role from the library's cache. \brief Collects a given Role from the library's cache.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A CoRoutine containing a Role.
		static CoRoutine<Role> getCachedRoleAsync(GetRoleData dataPackage);

		static void insertRole(std::unique_ptr<RoleData> role);

		static void removeRole(const Snowflake roleId);

	  protected:
		static std::unordered_map<Snowflake, std::unique_ptr<RoleData>> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static std::shared_mutex theMutex;
		static bool doWeCacheRoles;
	};
	/**@}*/

}// namespace DiscordCoreAPI
