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
/// RoleEntities.hpp - Header for the role_data related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For addin a role_data to a guild_member.
	struct add_guild_member_role_data {
		snowflake guildId{};///< The id of the guild within which to assign the role_data.
		jsonifier::string reason{};///< Reason for adding the guild_member_data's role_data.
		snowflake userId{};///< The id of the user_data to whom to assign the role_data.
		snowflake roleId{};///< The id of the role_data to be assigned.
	};

	/// @brief For removing a role_data from a guild_member.
	struct remove_guild_member_role_data {
		jsonifier::string reason{};///< Reason for removing the guild_member_data's role_data.
		snowflake guildId{};///< The id of the guild within which to remove the role_data.
		snowflake userId{};///< The id of the user_data from whom to remove the role_data.
		snowflake roleId{};///< The id of the role_data to be removed.
	};

	/// @brief For getting a chosen guild's roles.
	struct get_guild_roles_data {
		snowflake guildId{};///< Guild id for which guild to get the roles from.
	};

	/// @brief For creating a new role_data within a chosen guild.
	struct create_guild_role_data {
		permissions_parse permissions{};///< The base permissions to give the role_data.
		jsonifier::string hexColorValue{};///< Hex color-value between 0 and ffffff.
		jsonifier::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		jsonifier::string reason{};///< Reason for creating the role_data.
		snowflake guildId{};///< Which guild to make the role_data in.
		jsonifier::string icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		bool mentionable{};///< Is it mentionable by others in the guild.
		uint32_t position{};///< The position amongst the other roles.
		jsonifier::string name{};///< The name of the role_data.
		bool hoist{};///< Is this role_data hoisted above the rest of them?
	};

	/// @brief For updating the positions of the roles.
	struct role_position_data {
		uint32_t rolePosition{};///< The new role_data position.
		snowflake roleId{};///< The role_data to update.
	};

	/// @brief For updating the role_data positions.
	struct modify_guild_role_positions_data {
		template<typename value_type> friend struct jsonifier::core;
		friend class roles;

		uint32_t newPosition{};///< The new position of the role_data.
		jsonifier::string reason{};///< Reason for modifying the role_data positions.
		snowflake guildId{};///< The guild within which to move the role_data.
		snowflake roleId{};///< The id of the role_data to move.

	  protected:
		jsonifier::vector<role_position_data> rolePositions;
	};

	/// @brief For updating a role_data's options within a chosen guild.
	struct modify_guild_role_data {
		permissions_parse permissions{ "0" };///< Base guild permissions for the role_data.
		jsonifier::string hexColorValue{};///< A hex-color value between 0x00 and 0xFFFFFF.
		jsonifier::string unicodeEmoji{};///< The role's unicode emoji as a standard emoji.
		jsonifier::string reason{};///<< reason for modifying the role_data.
		snowflake guildId{};///< The id of the guild within which to update the role_data.
		jsonifier::string icon{};///< Image data	the role's icon image (if the guild has the ROLE_ICONS feature)	null.
		bool mentionable{};///< Is it mentionable?
		snowflake roleId{};///< The id of the role_data to update.
		jsonifier::string name{};///< What the name of the role_data is going to be.
		bool hoist{};///< Is this role_data hoisted above the others?
	};

	/// @brief For removing a role_data from a chosen guild.
	struct remove_guild_role_data {
		jsonifier::string reason{};///< Reason for removing this role_data.
		snowflake guildId{};///< The id of the guild from which to remove the role_data.
		snowflake roleId{};///< The id of the role_data to remove.
	};

	/// @brief For getting a chosen guild_member_data's roles.
	struct get_guild_member_roles_data {
		guild_member_data guildMember{};///< Which guild member to collect the roles from.
		snowflake guildId{};///< Which guild to collect their roles from.
	};

	/// @brief For getting a role_data from the library's cache, or the discord server.
	struct get_role_data {
		snowflake guildId{};///< Which guild to collect the role_data from.
		snowflake roleId{};///< Which role_data to collect.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the role_data related discord endpoints.
	class DiscordCoreAPI_Dll roles {
	  public:
		friend class discord_core_internal::websocket_client;
		friend class discord_core_client;
		friend class role_data;
		friend class guild_data;
		friend class guild;

		static void initialize(discord_core_internal::https_client*, config_manager* configManagerNew);

		/// @brief Adds a role_data to a chosen guild member.
		/// @param dataPackage an add_guild_member_role_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> addGuildMemberRoleAsync(add_guild_member_role_data dataPackage);

		/// @brief Removes a given role from a chosen guild_member_data.
		/// @param dataPackage a remove_guild_member_role_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> removeGuildMemberRoleAsync(remove_guild_member_role_data dataPackage);

		/// @brief Collects the roles that a guild has.
		/// @param dataPackage a get_guild_roles_data structure.
		/// @return a co_routine containing a jsonifier::vector<role_data>.
		static co_routine<jsonifier::vector<role_data>> getGuildRolesAsync(get_guild_roles_data dataPackage);

		/// @brief Creates a new role_data within the given guild.
		/// @param dataPackage a create_guild_role_data structure.
		/// @return a co_routine containing a role_data.
		static co_routine<role_data> createGuildRoleAsync(create_guild_role_data dataPackage);

		/// @brief Updates a role_data's positions.
		/// @param dataPackage a modify_guild_role_positions_data structure.
		/// @return a co_routine containing a jsonifier::vector<role_data>.
		static co_routine<jsonifier::vector<role_data>> modifyGuildRolePositionsAsync(modify_guild_role_positions_data dataPackage);

		/// @brief Updates a given role's properties.
		/// @param dataPackage a modify_guild_role_data structure.
		/// @return a co_routine containing a role_data.
		static co_routine<role_data> modifyGuildRoleAsync(modify_guild_role_data dataPackage);

		/// @brief Removes a given role from a guild.
		/// @param dataPackage a remove_guild_role_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> removeGuildRoleAsync(remove_guild_role_data dataPackage);

		/// @brief Collects the roles that a guild_member has.
		/// @param dataPackage a get_guild_member_roles_data structure.
		/// @return a co_routine containing a jsonifier::vector<role_data>.
		static co_routine<jsonifier::vector<role_data>> getGuildMemberRolesAsync(get_guild_member_roles_data dataPackage);

		/// @brief Collects a role_data from the discord servers.
		/// @param dataPackage a get_role_data structure.
		/// @return a co_routine containing a role_data.
		static co_routine<role_data> getRoleAsync(get_role_data dataPackage);

		/// @brief Collects a given role from the library's cache.
		/// @param dataPackage a get_role_data structure.
		/// @return a co_routine containing a role_data.
		static role_cache_data getCachedRole(get_role_data dataPackage);

		template<typename role_type> DCA_INLINE static void insertRole(role_type&& role) {
			if (doWeCacheRolesBool) {
				if (role.id == 0) {
					throw dca_exception{ "Sorry, but there was no id set for that role." };
				}
				cache.emplace(static_cast<role_cache_data>(std::forward<role_type>(role)));
				if (cache.count() % 1000 == 0) {
					std::cout << "current role count: " << cache.count() << std::endl;
				}
			}
		}

		static void removeRole(const role_cache_data& roleId);

		static bool doWeCacheRoles();

	  protected:
		static discord_core_internal::https_client* httpsClient;
		static object_cache<role_cache_data> cache;
		static bool doWeCacheRolesBool;
	};
	/**@}*/

}
