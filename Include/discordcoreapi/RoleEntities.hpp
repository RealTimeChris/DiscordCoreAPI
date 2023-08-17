/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// RoleEntities.hpp - Header for the RoleData related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For addin a RoleData to a GuildMember.
	struct AddGuildMemberRoleData {
		Snowflake guildId{};///< The id of the Guild within which to assign the RoleData.
		std::string reason{};///< Reason for adding the GuildMemberData's RoleData.
		Snowflake userId{};///< The id of the UserData to whom to assign the RoleData.
		Snowflake roleId{};///< The id of the RoleData to be assigned.
	};

	/// @brief For removing a RoleData from a GuildMember.
	struct RemoveGuildMemberRoleData {
		std::string reason{};///< Reason for removing the GuildMemberData's RoleData.
		Snowflake guildId{};///< The id of the Guild within which to remove the RoleData.
		Snowflake userId{};///< The id of the UserData from whom to remove the RoleData.
		Snowflake roleId{};///< The id of the RoleData to be removed.
	};

	/// @brief For getting a chosen Guild's Roles.
	struct GetGuildRolesData {
		Snowflake guildId{};///< Guild id for which Guild to get the roles from.
	};

	/// @brief For creating a new RoleData within a chosen Guild.
	struct CreateGuildRoleData {
		PermissionsParse permissions{};///< The base permissions to give the RoleData.
		std::string hexColorValue{};///< Hex color-value between 0 and ffffff.
		std::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		std::string reason{};///< Reason for creating the RoleData.
		Snowflake guildId{};///< Which Guild to make the RoleData in.
		std::string icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		bool mentionable{};///< Is it mentionable by others in the Guild.
		int32_t position{};///< The position amongst the other roles.
		std::string name{};///< The name of the RoleData.
		bool hoist{};///< Is this RoleData hoisted above the rest of them?
	};

	/// @brief For updating the positions of the roles.
	struct RolePositionData {
		int32_t rolePosition{};///< The new RoleData position.
		Snowflake roleId{};///< The RoleData to update.
	};

	/// @brief For updating the RoleData positions.
	struct ModifyGuildRolePositionsData {
		friend struct Jsonifier::Core<DiscordCoreAPI::ModifyGuildRolePositionsData>;
		friend class Roles;

		int32_t newPosition{};///< The new position of the RoleData.
		std::string reason{};///< Reason for modifying the RoleData positions.
		Snowflake guildId{};///< The Guild within which to move the RoleData.
		Snowflake roleId{};///< The id of the RoleData to move.

	  protected:
		Jsonifier::Vector<RolePositionData> rolePositions;
	};

	/// @brief For updating a RoleData's options within a chosen Guild.
	struct ModifyGuildRoleData {
		PermissionsParse permissions{ "0" };///< Base Guild permissions for the RoleData.
		std::string hexColorValue{};///< A hex-color value between 0x00 and 0xFFFFFF.
		std::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		std::string reason{};///<< Reason for modifying the RoleData.
		Snowflake guildId{};///< The id of the Guild within which to update the RoleData.
		std::string icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		bool mentionable{};///< Is it mentionable?
		Snowflake roleId{};///< The id of the RoleData to update.
		std::string name{};///< What the name of the RoleData is going to be.
		bool hoist{};///< Is this RoleData hoisted above the others?
	};

	/// @brief For removing a RoleData from a chosen Guild.
	struct RemoveGuildRoleData {
		std::string reason{};///< Reason for removing this RoleData.
		Snowflake guildId{};///< The id of the Guild from which to remove the RoleData.
		Snowflake roleId{};///< The id of the RoleData to remove.
	};

	/// @brief For getting a chosen GuildMemberData's Roles.
	struct GetGuildMemberRolesData {
		GuildMemberData guildMember{};///< Which Guild member to collect the roles from.
		Snowflake guildId{};///< Which Guild to collect their roles from.
	};

	/// @brief For getting a RoleData from the library's cache, or the Discord server.
	struct GetRoleData {
		Snowflake guildId{};///< Which Guild to collect the RoleData from.
		Snowflake roleId{};///< Which RoleData to collect.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the RoleData related Discord endpoints.
	class DiscordCoreAPI_Dll Roles {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreClient;
		friend class RoleData;
		friend class GuildData;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManagerNew);

		/// @brief Adds a RoleData to a chosen Guild member.
		/// @param dataPackage An AddGuildMemberRoleData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage);

		/// @brief Removes a given Role from a chosen GuildMemberData.
		/// @param dataPackage A RemoveGuildMemberRoleData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage);

		/// @brief Collects the Roles that a Guild has.
		/// @param dataPackage A GetGuildRolesData structure.
		/// @return A CoRoutine containing a Jsonifier::Vector<RoleData>.
		static CoRoutine<Jsonifier::Vector<RoleData>> getGuildRolesAsync(GetGuildRolesData dataPackage);

		/// @brief Creates a new RoleData within the given Guild.
		/// @param dataPackage A CreateGuildRoleData structure.
		/// @return A CoRoutine containing a RoleData.
		static CoRoutine<RoleData> createGuildRoleAsync(CreateGuildRoleData dataPackage);

		/// @brief Updates a RoleData's positions.
		/// @param dataPackage A ModifyGuildRolePositionsData structure.
		/// @return A CoRoutine containing a Jsonifier::Vector<RoleData>.
		static CoRoutine<Jsonifier::Vector<RoleData>> modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage);

		/// @brief Updates a given Role's properties.
		/// @param dataPackage A ModifyGuildRoleData structure.
		/// @return A CoRoutine containing a RoleData.
		static CoRoutine<RoleData> modifyGuildRoleAsync(ModifyGuildRoleData dataPackage);

		/// @brief Removes a given Role from a Guild.
		/// @param dataPackage A RemoveGuildRoleData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> removeGuildRoleAsync(RemoveGuildRoleData dataPackage);

		/// @brief Collects the Roles that a GuildMember has.
		/// @param dataPackage A GetGuildMemberRolesData structure.
		/// @return A CoRoutine containing a Jsonifier::Vector<RoleData>.
		static CoRoutine<Jsonifier::Vector<RoleData>> getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage);

		/// @brief Collects a RoleData from the Discord servers.
		/// @param dataPackage A GetRoleData structure.
		/// @return A CoRoutine containing a RoleData.
		static CoRoutine<RoleData> getRoleAsync(GetRoleData dataPackage);

		/// @brief Collects a given Role from the library's cache.
		/// @param dataPackage A GetRoleData structure.
		/// @return A CoRoutine containing a RoleData.
		static RoleCacheData getCachedRole(GetRoleData dataPackage);

		template<typename RoleType> inline static void insertRole(RoleType&& role) {
			if (doWeCacheRolesBool) {
				if (role.id == 0) {
					throw DCAException{ "Sorry, but there was no id set for that role." };
				}
				cache.emplace(static_cast<RoleCacheData>(std::forward<RoleType>(role)));
			}
		}

		static void removeRole(const RoleCacheData& roleId);

		static bool doWeCacheRoles();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<RoleCacheData> cache;
		static bool doWeCacheRolesBool;
	};
	/**@}*/

}
