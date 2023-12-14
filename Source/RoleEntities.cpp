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
/// RoleEntities.cpp - Source file for the role_data related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.cpp

#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::modify_guild_role_positions_data> {
		using value_type				 = discord_core_api::modify_guild_role_positions_data;
		static constexpr auto parseValue = createValue("guild_id", &value_type::guildId, "id", &value_type::roleId, "positions", &value_type::rolePositions, "reason",
			&value_type::reason, "position", &value_type::newPosition);
	};

	template<> struct core<discord_core_api::modify_guild_role_data> {
		using value_type				 = discord_core_api::modify_guild_role_data;
		static constexpr auto parseValue = createValue("color", &value_type::hexColorValue, "hoist", &value_type::hoist, "id", &value_type::roleId, "mentionable",
			&value_type::mentionable, "name", &value_type::name, "permissions", &value_type::permissions, "unicode_emoji", &value_type::unicodeEmoji,
			"guild_id", &value_type::guildId, "icon", &value_type::icon);
	};

	template<> struct core<discord_core_api::create_guild_role_data> {
		using value_type				 = discord_core_api::create_guild_role_data;
		static constexpr auto parseValue = createValue("color", &value_type::hexColorValue, "hoist", &value_type::hoist, "id", &value_type::guildId, "mentionable",
			&value_type::mentionable, "name", &value_type::name, "permissions", &value_type::permissions, "position", &value_type::position,
			"unicode_emoji", &value_type::unicodeEmoji, "icon", &value_type::icon);
	};

	template<> struct core<discord_core_api::role_position_data> {
		using value_type				 = discord_core_api::role_position_data;
		static constexpr auto parseValue = createValue("position", &value_type::rolePosition, "id", &value_type::roleId);
	};
}

namespace discord_core_api {

	role_cache_data& role_cache_data::operator=(const role_data& other) {
		flags = other.flags;
		setFlagValue<role_flags>(role_flags::mentionable, other.mentionable);
		setFlagValue<role_flags>(role_flags::managed, other.managed);
		setFlagValue<role_flags>(role_flags::hoist, other.hoist);
		if (other.unicodeEmoji != "") {
			unicodeEmoji = other.unicodeEmoji;
		}
		if (other.permissions.operator std::string_view() != "") {
			permissionsVal = other.permissions;
		}
		if (other.position != 0) {
			position = other.position;
		}
		if (other.color != 0) {
			color = other.color;
		}
		if (other.name != "") {
			name = other.name;
		}
		if (other.id != 0) {
			id = other.id;
		}
		return *this;
	}

	role_cache_data::role_cache_data(const role_data& other) {
		*this = other;
	}

	role_cache_data& role_cache_data::operator=(role_data&& other) noexcept {
		flags = other.flags;
		setFlagValue<role_flags>(role_flags::mentionable, other.mentionable);
		setFlagValue<role_flags>(role_flags::managed, other.managed);
		setFlagValue<role_flags>(role_flags::hoist, other.hoist);
		if (other.unicodeEmoji != "") {
			unicodeEmoji = std::move(other.unicodeEmoji);
		}
		if (other.permissions.operator std::string_view() != "") {
			permissionsVal = std::move(other.permissions);
		}
		if (other.name != "") {
			name = std::move(other.name);
		}
		if (other.position != 0) {
			position = other.position;
		}
		if (other.color != 0) {
			color = other.color;
		}
		if (other.id != 0) {
			id = other.id;
		}
		return *this;
	}

	role_cache_data::operator role_data() {
		role_data returnData{};
		returnData.mentionable	= getFlagValue<role_flags>(role_flags::mentionable);
		returnData.managed		= getFlagValue<role_flags>(role_flags::managed);
		returnData.hoist		= getFlagValue<role_flags>(role_flags::hoist);
		returnData.permissions	= permissionsVal.operator jsonifier::string();
		returnData.unicodeEmoji = unicodeEmoji;
		returnData.position		= position;
		returnData.flags		= flags;
		returnData.color		= color;
		returnData.name			= name;
		returnData.id			= id;
		return returnData;
	}

	role_cache_data::role_cache_data(role_data&& other) noexcept {
		*this = std::move(other);
	}

	role_data::role_data(snowflake newId) {
		id = newId;
	}

	void roles::initialize(discord_core_internal::https_client* client, config_manager* configManagerNew) {
		roles::doWeCacheRolesBool = configManagerNew->doWeCacheRoles();
		roles::httpsClient		  = client;
	}

	co_routine<void> roles::addGuildMemberRoleAsync(add_guild_member_role_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Guild_Member_Role };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
		workload.callStack	   = "roles::addGuildMemberRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		roles::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> roles::removeGuildMemberRoleAsync(remove_guild_member_role_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Member_Role };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
		workload.callStack	   = "roles::removeGuildMemberRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		roles::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<role_data>> roles::getGuildRolesAsync(get_guild_roles_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Roles };
		co_await newThreadAwaitable<jsonifier::vector<role_data>>();
		if (dataPackage.guildId == 0) {
			throw dca_exception{ "roles::getGuildRolesAsync() error: sorry, but you forgot to set the guildId!" };
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/roles";
		workload.callStack	   = "roles::getGuildRolesAsync()";
		jsonifier::vector<role_data> returnData{};
		roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<role_data> roles::createGuildRoleAsync(create_guild_role_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Role };
		co_await newThreadAwaitable<role_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/roles";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "roles::createGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		role_data returnData{};
		roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		modify_guild_role_positions_data newDataPackage{};
		newDataPackage.guildId	   = dataPackage.guildId;
		newDataPackage.newPosition = dataPackage.position;
		newDataPackage.roleId	   = returnData.id;
		auto results			   = modifyGuildRolePositionsAsync(newDataPackage).get();
		for (auto& value: results) {
			if (value.id == returnData.id) {
				returnData = value;
			}
		}
		co_return returnData;
	}

	co_routine<jsonifier::vector<role_data>> roles::modifyGuildRolePositionsAsync(modify_guild_role_positions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Role_Positions };
		co_await newThreadAwaitable<jsonifier::vector<role_data>>();
		jsonifier::vector<role_data> currentRoles = roles::getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		role_data newRole						  = roles::getCachedRole({ .roleId = dataPackage.roleId });
		for (auto& value: currentRoles) {
			if (value.id == newRole.id) {
				continue;
			}
			role_position_data newData;
			if (newRole.position < dataPackage.newPosition) {
				if (value.position <= dataPackage.newPosition && value.position > newRole.position) {
					newData.roleId		 = value.id;
					newData.rolePosition = value.position - 1;
					dataPackage.rolePositions.emplace_back(newData);
				}
			} else {
				if (value.position >= dataPackage.newPosition && value.position < newRole.position) {
					newData.roleId		 = value.id;
					newData.rolePosition = value.position + 1;
					dataPackage.rolePositions.emplace_back(newData);
				}
			}
		}
		role_position_data newDataPos;
		newDataPos.roleId		= dataPackage.roleId;
		newDataPos.rolePosition = dataPackage.newPosition;
		dataPackage.rolePositions.emplace_back(newDataPos);
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/roles";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "roles::modifyGuildRolePositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		jsonifier::vector<role_data> returnData{};
		roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<role_data> roles::modifyGuildRoleAsync(modify_guild_role_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Role };
		co_await newThreadAwaitable<role_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "roles::modifyGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		role_data data{ dataPackage.roleId };
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheRolesBool) {
			insertRole(static_cast<role_cache_data>(data));
		}
		co_return data;
	}

	co_routine<void> roles::removeGuildRoleAsync(remove_guild_role_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Role };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
		workload.callStack	   = "roles::removeGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		roles::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<role_data>> roles::getGuildMemberRolesAsync(get_guild_member_roles_data dataPackage) {
		co_await newThreadAwaitable<jsonifier::vector<role_data>>();
		jsonifier::vector<role_data> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		jsonifier::vector<role_data> rolesVectorNew{};
		for (auto& value: rolesVector) {
			for (auto& value2: dataPackage.guildMember.roles) {
				if (value2 == value.id) {
					rolesVectorNew.emplace_back(value);
				}
			}
		}
		co_return std::move(rolesVectorNew);
	}

	co_routine<role_data> roles::getRoleAsync(get_role_data dataPackage) {
		co_await newThreadAwaitable<role_data>();
		auto roles = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		if (dataPackage.guildId == 0) {
			throw dca_exception{ "roles::getRoleAsync() error: sorry, but you forgot to set the guildId!" };
		}
		role_data data{ dataPackage.roleId };
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		for (auto& value: roles) {
			if (value.id == dataPackage.roleId) {
				data = std::move(value);
			}
		}
		if (doWeCacheRolesBool) {
			insertRole(static_cast<role_cache_data>(data));
		}
		co_return data;
	}

	role_cache_data roles::getCachedRole(const get_role_data dataPackage) {
		if (cache.contains(dataPackage.roleId)) {
			return cache[dataPackage.roleId];
		} else {
			return getRoleAsync({ .guildId = dataPackage.guildId, .roleId = dataPackage.roleId }).get();
		}
	}

	void roles::removeRole(const role_cache_data& roleId) {
		cache.erase(roleId);
	};

	bool roles::doWeCacheRoles() {
		return roles::doWeCacheRolesBool;
	}

	discord_core_internal::https_client* roles::httpsClient{};
	object_cache<role_cache_data> roles::cache{};
	bool roles::doWeCacheRolesBool{};
};
