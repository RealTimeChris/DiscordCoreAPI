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
/// RoleEntities.cpp - Source file for the Role related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.cpp

#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::ModifyGuildRolePositionsData> {
		using ValueType = DiscordCoreAPI::ModifyGuildRolePositionsData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "id", &ValueType::roleId, "positions", &ValueType::rolePositions,
			"reason", &ValueType::reason, "position", &ValueType::newPosition);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildRoleData> {
		using ValueType = DiscordCoreAPI::ModifyGuildRoleData;
		static constexpr auto parseValue = object("color", &ValueType::hexColorValue, "hoist", &ValueType::hoist, "id", &ValueType::roleId,
			"mentionable", &ValueType::mentionable, "name", &ValueType::name, "permissions", &ValueType::permissions, "reason", &ValueType::reason,
			"unicode_emoji", &ValueType::unicodeEmoji, "guild_id", &ValueType::guildId, "icon", &ValueType::icon);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildRoleData> {
		using ValueType = DiscordCoreAPI::CreateGuildRoleData;
		static constexpr auto parseValue = object("color", &ValueType::hexColorValue, "hoist", &ValueType::hoist, "id", &ValueType::guildId,
			"mentionable", &ValueType::mentionable, "name", &ValueType::name, "permissions", &ValueType::permissions, "position",
			&ValueType::position, "reason", &ValueType::reason, "unicode_emoji", &ValueType::unicodeEmoji, "icon", &ValueType::icon);
	};

	template<> struct Core<DiscordCoreAPI::RolePositionData> {
		using ValueType = DiscordCoreAPI::RolePositionData;
		static constexpr auto parseValue = object("position", &ValueType::rolePosition, "id", &ValueType::roleId);
	};
}

namespace DiscordCoreAPI {

	Role& Role::operator=(RoleData&& other) noexcept {
		if (this != &other) {
			unicodeEmoji = std::move(other.unicodeEmoji);
			permissions = std::move(other.permissions);
			name = std::move(other.name);
			position = other.position;
			guildId = other.guildId;
			flags = other.flags;
			color = other.color;
			id = other.id;
		}
		return *this;
	}

	Role::Role(RoleData&& other) noexcept {
		*this = std::move(other);
	}

	Role& Role::operator=(const RoleData& other) noexcept {
		if (this != &other) {
			unicodeEmoji = other.unicodeEmoji;
			permissions = other.permissions;
			position = other.position;
			guildId = other.guildId;
			flags = other.flags;
			color = other.color;
			name = other.name;
			id = other.id;
		}
		return *this;
	}

	Role::Role(const RoleData& other) noexcept {
		*this = other;
	}

	void Roles::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		Roles::doWeCacheRolesBool = configManagerNew->doWeCacheRoles();
		Roles::httpsClient = client;
	}

	CoRoutine<void> Roles::addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Member_Role };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
		workload.callStack = "Roles::addGuildMemberRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<void> Roles::removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Member_Role };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
		workload.callStack = "Roles::removeGuildMemberRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<std::vector<Role>> Roles::getGuildRolesAsync(GetGuildRolesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Roles };
		co_await NewThreadAwaitable<std::vector<Role>>();
		if (dataPackage.guildId == 0) {
			throw DCAException{ "Roles::getGuildRolesAsync() Error: Sorry, but you forgot to set the guildId!" };
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
		workload.callStack = "Roles::getGuildRolesAsync()";
		RoleVector returnData{};
		Roles::httpsClient->submitWorkloadAndGetResult<RoleVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Role> Roles::createGuildRoleAsync(CreateGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Role };
		co_await NewThreadAwaitable<Role>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Roles::createGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Role returnData{};
		Roles::httpsClient->submitWorkloadAndGetResult<Role>(workload, returnData);
		ModifyGuildRolePositionsData newDataPackage{};
		newDataPackage.guildId = dataPackage.guildId;
		newDataPackage.newPosition = dataPackage.position;
		newDataPackage.roleId = returnData.id;
		auto results = modifyGuildRolePositionsAsync(newDataPackage).get();
		for (auto& value: results) {
			if (value.id == returnData.id) {
				returnData = value;
			}
		}
		co_return std::move(returnData);
	}

	CoRoutine<std::vector<Role>> Roles::modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Role_Positions };
		co_await NewThreadAwaitable<std::vector<Role>>();
		std::vector<Role> currentRoles = Roles::getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		RoleData newRole = Roles::getCachedRole({ .roleId = dataPackage.roleId });
		for (auto& value: currentRoles) {
			if (value.id == newRole.id) {
				continue;
			}
			RolePositionData newData;
			if (newRole.position < dataPackage.newPosition) {
				if (value.position <= dataPackage.newPosition && value.position > newRole.position) {
					newData.roleId = value.id;
					newData.rolePosition = value.position - 1;
					dataPackage.rolePositions.emplace_back(newData);
				}
			} else {
				if (value.position >= dataPackage.newPosition && value.position < newRole.position) {
					newData.roleId = value.id;
					newData.rolePosition = value.position + 1;
					dataPackage.rolePositions.emplace_back(newData);
				}
			}
		}
		RolePositionData newDataPos;
		newDataPos.roleId = dataPackage.roleId;
		newDataPos.rolePosition = dataPackage.newPosition;
		dataPackage.rolePositions.emplace_back(newDataPos);
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Roles::modifyGuildRolePositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		RoleVector returnData{};
		Roles::httpsClient->submitWorkloadAndGetResult<RoleVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Role> Roles::modifyGuildRoleAsync(ModifyGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Role };
		co_await NewThreadAwaitable<Role>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Roles::modifyGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Role data{};
		Roles::httpsClient->submitWorkloadAndGetResult<Role>(workload, data);
		if (Roles::cache.contains(dataPackage.roleId)) {
			data = Roles::cache.find(dataPackage.roleId);
		} else {
			Roles::insertRole(std::move(data));
		}
		co_return data;
	}

	CoRoutine<void> Roles::removeGuildRoleAsync(RemoveGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Role };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
		workload.callStack = "Roles::removeGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<std::vector<Role>> Roles::getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage) {
		co_await NewThreadAwaitable<std::vector<Role>>();
		std::vector<Role> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		std::vector<Role> rolesVectorNew{};
		for (auto& value: rolesVector) {
			for (auto& value2: dataPackage.guildMember.roles) {
				if (value2 == value.id) {
					rolesVectorNew.emplace_back(value);
				}
			}
		}
		co_return std::move(rolesVectorNew);
	}

	CoRoutine<Role> Roles::getRoleAsync(GetRoleData dataPackage) {
		co_await NewThreadAwaitable<Role>();
		auto roles = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		if (dataPackage.guildId == 0) {
			throw DCAException{ "Roles::getRoleAsync() Error: Sorry, but you forgot to set the guildId!" };
		}
		for (auto& value: roles) {
			if (value.id == dataPackage.roleId) {
				value.guildId = dataPackage.guildId;
				co_return std::move(value);
			}
		}
		co_return {};
	}

	RoleData Roles::getCachedRole(GetRoleData dataPackage) {
		RoleData data{};
		data.id = dataPackage.roleId;
		if (!Roles::cache.contains(dataPackage.roleId)) {
			return Roles::getRoleAsync(dataPackage).get();
		} else {
			return Roles::cache.find(data.id);
		}
	}

	StopWatch<Milliseconds> stopWatchNew{ 5ms };

	RoleData& Roles::insertRole(RoleData&& role) {
		if (role.id == 0) {
			throw DCAException{ "Sorry, but there was no id set for that role." };
		}
		auto id = role.id;
		Roles::cache.emplace(std::forward<RoleData>(role));
		if (Roles::cache.count() % 100 == 0) {
			std::cout << "ROLE COUNT: " << Roles::cache.count() << ", AFTER: " << stopWatchNew.totalTimePassed().count() << "s" << std::endl;
		}
		return cache.find(id);
	}

	void Roles::removeRole(const Snowflake roleId) {
		Roles::cache.erase(roleId);
	};

	bool Roles::doWeCacheRoles() {
		return Roles::doWeCacheRolesBool;
	}

	DiscordCoreInternal::HttpsClient* Roles::httpsClient{};
	ObjectCache<Snowflake, RoleData> Roles::cache{};
	bool Roles::doWeCacheRolesBool{};
};