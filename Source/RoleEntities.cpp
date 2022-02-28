/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// RoleEntities.cpp - Source file for the Role related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file RoleEntities.cpp

#include <RoleEntities.hpp>
#include <CoRoutine.hpp>
#include <Http.hpp>

namespace DiscordCoreAPI {

	Role::Role(RoleData dataNew) {
		this->unicodeEmoji = dataNew.unicodeEmoji;
		this->permissions = dataNew.permissions;
		this->mentionable = dataNew.mentionable;
		this->position = dataNew.position;
		this->managed = dataNew.managed;
		this->color = dataNew.color;
		this->hoist = dataNew.hoist;
		this->name = dataNew.name;
		this->icon = dataNew.icon;
		this->tags = dataNew.tags;
		this->id = dataNew.id;
	}

	void Roles::initialize(DiscordCoreInternal::HttpClient*theClient) {
		Roles::httpClient = theClient;
	}

	CoRoutine<void> Roles::addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member_Role;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			workload.callStack = "Roles::addGuildMemberRoleAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Roles::httpClient, workload);
		}
		catch (...) {
			reportException("Roles::addGuildMemberRoleAsync()");
		}
	}

	CoRoutine<void> Roles::removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member_Role;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			workload.callStack = "Roles::removeGuildMemberRoleAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Roles::httpClient, workload);
		}
		catch (...) {
			reportException("Roles::removeGuildMemberRoleAsync()");
		}
	}

	CoRoutine<std::vector<Role>> Roles::getGuildRolesAsync(GetGuildRolesData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<Role>>();
			if (dataPackage.guildId == "") {
				std::runtime_error failError("Roles::getRoleAsync Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Roles;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.callStack = "Roles::getGuildRolesAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Role>>(*Roles::httpClient, workload);
		}
		catch (...) {
			reportException("Roles::getGuildRolesAsync()");
		}
	}

	CoRoutine<Role> Roles::createGuildRoleAsync(CreateGuildRoleData dataPackage) {
		try {
			co_await NewThreadAwaitable<Role>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Role;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Roles::createGuildRoleAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			auto roleNew = DiscordCoreInternal::submitWorkloadAndGetResult<Role>(*Roles::httpClient, workload);
			Role result{ roleNew };
			ModifyGuildRolePositionsData newDataPackage{};
			newDataPackage.guildId = dataPackage.guildId;
			newDataPackage.newPosition = dataPackage.position;
			newDataPackage.roleId = result.id;
			modifyGuildRolePositionsAsync(newDataPackage).get();
			co_return result;
		}
		catch (...) {
			reportException("Roles::createGuildRoleAsync()");
		}
	}

	CoRoutine<std::vector<Role>> Roles::modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<Role>>();
			std::vector<Role> currentRoles = Roles::getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			Role newRole = Roles::getCachedRoleAsync({ .roleId = dataPackage.roleId }).get();
			for (auto& value : currentRoles) {
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
				}
				else {
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
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Role_Positions;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Roles::modifyGuildRolePositionsAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Role>>(*Roles::httpClient, workload);
		}
		catch (...) {
			reportException("Roles::modifyGuildRolePositionsAsync()");
		}
	}

	CoRoutine<Role> Roles::modifyGuildRoleAsync(ModifyGuildRoleData dataPackage) {
		try {
			co_await NewThreadAwaitable<Role>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Role;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Roles::modifyGuildRoleAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Role>(*Roles::httpClient, workload);
		}
		catch (...) {
			reportException("Roles::modifyGuildRoleAsync()");
		}
	}

	CoRoutine<void> Roles::removeGuildRoleAsync(RemoveGuildRoleData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Role;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.callStack = "Roles::removeGuildRoleAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Roles::httpClient, workload);
		}
		catch (...) {
			reportException("Roles::removeGuildRoleAsync()");
		}
	}

	CoRoutine<std::vector<Role>> Roles::getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage) {
		try {
			co_await NewThreadAwaitable< std::vector<Role>>();
			std::vector<Role> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			std::vector<Role> rolesVectorNew{};
			for (auto& value : rolesVector) {
				for (auto& value2 : dataPackage.guildMember.roles) {
					if (value2 == value.id) {
						rolesVectorNew.push_back(value);
					}
				}
			}
			co_return rolesVectorNew;
		}
		catch (...) {
			reportException("Roles::getGuildMemberRolesAsync()");
		}
	}

	CoRoutine<Role> Roles::getRoleAsync(GetRoleData dataPackage) {
		try {
			co_await NewThreadAwaitable<Role>();
			auto roles = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			if (dataPackage.guildId == "") {
				std::runtime_error failError("Roles::getAsync Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			Role newRole{};
			for (auto& value : roles) {
				if (value.id == dataPackage.roleId) {
					newRole = value;
				}
			}
			if (newRole.id != "") {
				Roles::insertRole(newRole);
			}
			co_return newRole;
		}
		catch (...) {
			reportException("Roles::getRoleAsync()");
		}
	}

	CoRoutine<Role> Roles::getCachedRoleAsync(GetRoleData dataPackage) {
		try {
			co_await NewThreadAwaitable<Role>();
			Role role{};
			if (Roles::cache.contains(dataPackage.roleId)) {
				role = Roles::cache.at(dataPackage.roleId);
			}
			else {
				role = Roles::getRoleAsync(dataPackage).get();
			}
			co_return role;
		}
		catch (...) {
			reportException("Roles::getCachedRoleAsync()");
		}
	}

	void Roles::insertRole(Role role) {
		try {
			if (role.id == "") {
				return;
			}
			Roles::cache.insert_or_assign(role.id, role);
		}
		catch (...) {
			reportException("Roles::insertRole()");
		}
	}

	void Roles::removeRole(std::string roleId) {
		try {
			Roles::cache.erase(roleId);
		}
		catch (...) {
			reportException("Roles::removeRole()");
		}
	};
	DiscordCoreInternal::HttpClient* Roles::httpClient{ nullptr };
	std::unordered_map<std::string, Role> Roles::cache{};
}
