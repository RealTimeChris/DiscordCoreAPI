/*
*
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
/// RoleEntities.cpp - Source file for the Role related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.cpp

#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	Role& Role::operator=(RoleData&& other) {
		this->unicodeEmoji = other.unicodeEmoji;
		this->permissions = other.permissions;
		this->position = other.position;
		this->flags = other.flags;
		this->color = other.color;
		this->name = other.name;
		this->id = other.id;
		return *this;
	}

	Role::Role(RoleData&& other) {
		*this = std::move(other);
	}

	Role& Role::operator=(RoleData& other) {
		this->unicodeEmoji = other.unicodeEmoji;
		this->permissions = other.permissions;
		this->position = other.position;
		this->flags = other.flags;
		this->color = other.color;
		this->name = other.name;
		this->id = other.id;
		return *this;
	}

	Role::Role(RoleData& other) {
		*this = other;
	}


	void Roles::initialize(DiscordCoreInternal::HttpClient* theClient, bool doWeCacheNew) {
		Roles::doWeCache = doWeCacheNew;
		Roles::httpClient = theClient;
	}

	CoRoutine<void> Roles::addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member_Role);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member_Role;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
		workload.relativePath =
			"/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.userId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.callStack = "Roles::addGuildMemberRoleAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return Roles::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Roles::removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member_Role);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member_Role;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath =
			"/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.userId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.callStack = "Roles::removeGuildMemberRoleAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return Roles::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Role>> Roles::getGuildRolesAsync(GetGuildRolesData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Roles);
		co_await NewThreadAwaitable<std::vector<Role>>();
		if (dataPackage.guildId == 0) {
			std::runtime_error failError("Roles::getRoleAsync Error: Sorry, but you forgot to set the guildId!");
			throw failError;
		}
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Roles;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles";
		workload.callStack = "Roles::getGuildRolesAsync";
		co_return Roles::httpClient->submitWorkloadAndGetResult<std::vector<Role>>(workload);
	}

	CoRoutine<Role> Roles::createGuildRoleAsync(CreateGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Role);
		co_await NewThreadAwaitable<Role>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Role;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Roles::createGuildRoleAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		auto roleNew = Roles::httpClient->submitWorkloadAndGetResult<Role>(workload);
		Role result{ roleNew };
		ModifyGuildRolePositionsData newDataPackage{};
		newDataPackage.guildId = dataPackage.guildId;
		newDataPackage.newPosition = dataPackage.position;
		newDataPackage.roleId = result.id;
		modifyGuildRolePositionsAsync(newDataPackage).get();
		co_return result;
	}

	CoRoutine<std::vector<Role>> Roles::modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Role_Positions);
		co_await NewThreadAwaitable<std::vector<Role>>();
		std::vector<Role> currentRoles = Roles::getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		Role newRole = Roles::getCachedRoleAsync({ .roleId = dataPackage.roleId }).get();
		for (auto& value: currentRoles) {
			if (value.id == newRole.id) {
				continue;
			}
			RolePositionData newData;
			if (newRole.position < dataPackage.newPosition) {
				if (value.position <= dataPackage.newPosition && value.position > newRole.position) {
					newData.roleId = value.id;
					newData.rolePosition = value.position - 1;
					dataPackage.rolePositions.push_back(newData);
				}
			} else {
				if (value.position >= dataPackage.newPosition && value.position < newRole.position) {
					newData.roleId = value.id;
					newData.rolePosition = value.position + 1;
					dataPackage.rolePositions.push_back(newData);
				}
			}
		}
		RolePositionData newDataPos;
		newDataPos.roleId = dataPackage.roleId;
		newDataPos.rolePosition = dataPackage.newPosition;
		dataPackage.rolePositions.push_back(newDataPos);
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Role_Positions;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Roles::modifyGuildRolePositionsAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return Roles::httpClient->submitWorkloadAndGetResult<std::vector<Role>>(workload);
	}

	CoRoutine<Role> Roles::modifyGuildRoleAsync(ModifyGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Role);
		co_await NewThreadAwaitable<Role>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Role;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Roles::modifyGuildRoleAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return Roles::httpClient->submitWorkloadAndGetResult<Role>(workload);
	}

	CoRoutine<void> Roles::removeGuildRoleAsync(RemoveGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Role);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Role;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.callStack = "Roles::removeGuildRoleAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return Roles::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Role>> Roles::getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage) {
		co_await NewThreadAwaitable<std::vector<Role>>();
		std::vector<Role> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		std::vector<Role> rolesVectorNew{};
		for (auto& value: rolesVector) {
			for (auto& value2: dataPackage.guildMember.roles) {
				if (value2 == value.id) {
					rolesVectorNew.push_back(value);
				}
			}
		}
		co_return rolesVectorNew;
	}

	CoRoutine<Role> Roles::getRoleAsync(GetRoleData dataPackage) {
		co_await NewThreadAwaitable<Role>();
		auto roles = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		if (dataPackage.guildId == 0) {
			std::runtime_error failError("Roles::getAsync Error: Sorry, but you forgot to set the guildId!");
			throw failError;
		}
		Role newRole{};
		for (auto& value: roles) {
			if (value.id == dataPackage.roleId) {
				newRole = value;
			}
		}
		if (newRole.id != 0) {
			Roles::insertRole(newRole);
		}
		co_return newRole;
	}

	CoRoutine<RoleData> Roles::getCachedRoleAsync(GetRoleData dataPackage) {
		co_await NewThreadAwaitable<RoleData>();
		if (Roles::cache.contains(dataPackage.roleId)) {
			co_return Roles::cache[dataPackage.roleId];
		} else {
			co_return Roles::getRoleAsync(dataPackage).get();
		}
	}

	void Roles::insertRole(RoleData role) {
		std::lock_guard<std::mutex> theLock{ Roles::theMutex };
		if (role.id == 0) {
			return;
		}
		if (Roles::doWeCache) {
			Roles::cache.insert_or_assign(role.id, role);
			std::cout << "THE ROLE COUNT: " << Roles::cache.size() << ", THE ROLE SIZE: " << sizeof(role) << std::endl;
		}
	}

	void Roles::removeRole(const uint64_t& roleId) {
		std::lock_guard<std::mutex> theLock{ Roles::theMutex };
		Roles::cache.erase(roleId);
	};

	DiscordCoreInternal::HttpClient* Roles::httpClient{ nullptr };
	std::unordered_map<uint64_t, RoleData> Roles::cache{};
	bool Roles::doWeCache{ false };
	std::mutex Roles::theMutex{};
}
