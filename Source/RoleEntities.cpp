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
		using OTy2 = DiscordCoreAPI::ModifyGuildRolePositionsData;
		static constexpr auto parseValue = object("guild_id", &OTy2::guildId, "id", &OTy2::roleId, "positions", &OTy2::rolePositions, "reason",
			&OTy2::reason, "position", &OTy2::newPosition);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildRoleData> {
		using OTy2 = DiscordCoreAPI::ModifyGuildRoleData;
		static constexpr auto parseValue = object("color", &OTy2::hexColorValue, "hoist", &OTy2::hoist, "id", &OTy2::roleId, "mentionable",
			&OTy2::mentionable, "name", &OTy2::name, "permissions", &OTy2::permissions, "reason", &OTy2::reason, "unicode_emoji", &OTy2::unicodeEmoji,
			"guild_id", &OTy2::guildId, "icon", &OTy2::icon);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildRoleData> {
		using OTy2 = DiscordCoreAPI::CreateGuildRoleData;
		static constexpr auto parseValue = object("color", &OTy2::hexColorValue, "hoist", &OTy2::hoist, "id", &OTy2::guildId, "mentionable",
			&OTy2::mentionable, "name", &OTy2::name, "permissions", &OTy2::permissions, "position", &OTy2::position, "reason", &OTy2::reason,
			"unicode_emoji", &OTy2::unicodeEmoji, "icon", &OTy2::icon);
	};

	template<> struct Core<DiscordCoreAPI::RolePositionData> {
		using OTy2 = DiscordCoreAPI::RolePositionData;
		static constexpr auto parseValue = object("position", &OTy2::rolePosition, "id", &OTy2::roleId);
	};
}

namespace DiscordCoreAPI {

	Role& Role::operator=(RoleData&& other) noexcept {
		if (this != &other) {
			this->unicodeEmoji = std::move(other.unicodeEmoji);
			this->permissions = std::move(other.permissions);
			this->name = std::move(other.name);
			this->position = other.position;
			this->guildId = other.guildId;
			this->flags = other.flags;
			this->color = other.color;
			this->id = other.id;
		}
		return *this;
	}

	Role::Role(RoleData&& other) noexcept {
		*this = std::move(other);
	}

	Role& Role::operator=(const RoleData& other) noexcept {
		if (this != &other) {
			this->unicodeEmoji = other.unicodeEmoji;
			this->permissions = other.permissions;
			this->position = other.position;
			this->guildId = other.guildId;
			this->flags = other.flags;
			this->color = other.color;
			this->name = other.name;
			this->id = other.id;
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
		Roles::httpsClient->submitWorkloadAndGetResult(workload);
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
		Roles::httpsClient->submitWorkloadAndGetResult(workload);
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
		Roles::httpsClient->submitWorkloadAndGetResult(workload);
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
			Roles::cache.emplace(std::move(data));
			return data;
		}
	}

	StopWatch<Milliseconds> stopWatchNew{ 5ms };
	void Roles::insertRole(const RoleData& role) {
		if (role.id == 0) {
			return;
		}
		if (Roles::doWeCacheRoles()) {
			RoleData newRole{ role };
			Roles::cache.emplace(std::move(newRole));
			if (Roles::cache.count() % 10 == 0) {
				std::cout << "ROLE COUNT: " << Roles::cache.count() << ", AFTER: " << stopWatchNew.totalTimePassed().count() << "s" << std::endl;
			}
		}
	}

	void Roles::insertRole(RoleData&& role) {
		if (role.id == 0) {
			return;
		}
		if (Roles::doWeCacheRoles()) {
			Roles::cache.emplace(std::forward<RoleData>(role));
			if (Roles::cache.count() % 10 == 0) {
				std::cout << "ROLE COUNT: " << Roles::cache.count() << ", AFTER: " << stopWatchNew.totalTimePassed().count() << "s" << std::endl;
			}
		}
	}

	void Roles::removeRole(const Snowflake roleId) {
		Roles::cache.erase(roleId);
	};

	bool Roles::doWeCacheRoles() {
		return Roles::doWeCacheRolesBool;
	}

	DiscordCoreInternal::HttpsClient* Roles::httpsClient{ nullptr };
	ObjectCache<Snowflake, RoleData> Roles::cache{};
	bool Roles::doWeCacheRolesBool{};
};