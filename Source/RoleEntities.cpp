/*
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
/// RoleEntities.cpp - Source file for the Role related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.cpp

#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	CreateGuildRoleData::operator std::string() {
		int32_t roleColorInt = stol(this->hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		nlohmann::json data{};
		data["permissions"] = this->permissions.getCurrentPermissionString();
		data["mentionable"] = this->mentionable;
		data["hoist"] = this->hoist;
		data["name"] = this->name;
		data["color"] = roleColorReal;
		if (this->icon.size() > 0) {
			data["icon"] = this->icon;
		}
		if (this->unicodeEmoji.size() > 0) {
			data["unicode_emoji"] = this->unicodeEmoji;
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	ModifyGuildRolePositionsData::operator std::string() {
		nlohmann::json dataArray{};
		for (auto& value: this->rolePositions) {
			nlohmann::json data{};
			data["position"] = value.rolePosition;
			data["id"] = std::to_string(value.roleId);
			dataArray.emplace_back(data);
		}
		return dataArray.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	ModifyGuildRoleData::operator std::string() {
		int32_t roleColorInt = stol(this->hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		nlohmann::json data{};
		data["permissions"] = this->permissions.getCurrentPermissionString();
		data["mentionable"] = this->mentionable;
		data["hoist"] = this->hoist;
		data["name"] = this->name;
		data["color"] = roleColorReal;
		if (this->icon.size() > 0) {
			data["icon"] = this->icon;
		}
		if (this->unicodeEmoji.size() > 0) {
			data["unicode_emoji"] = this->unicodeEmoji;
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	void RoleData::insertRole(std::unique_ptr<RoleData> theRole) {
		Roles::insertRole(std::move(theRole));
	}

	Role& Role::operator=(RoleData&& other) noexcept {
		if (this != &other) {
			this->unicodeEmoji = std::move(other.unicodeEmoji);
			this->permissions = std::move(other.permissions);
			this->name = std::move(other.name);
			this->position = other.position;
			this->flags = other.flags;
			this->color = other.color;
			this->id = other.id;
		}
		return *this;
	}

	Role::Role(RoleData&& other) noexcept {
		*this = std::move(other);
	}

	Role& Role::operator=(RoleData& other) noexcept {
		if (this != &other) {
			this->unicodeEmoji = other.unicodeEmoji;
			this->permissions = other.permissions;
			this->position = other.position;
			this->flags = other.flags;
			this->color = other.color;
			this->name = other.name;
			this->id = other.id;
		}
		return *this;
	}

	Role::Role(RoleData& other) noexcept {
		*this = other;
	}

	Role& Role::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	Role::Role(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	RoleVector::operator std::vector<Role>() {
		return this->theRoles;
	}

	RoleVector& RoleVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	RoleVector::RoleVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	void Roles::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		Roles::configManager = configManagerNew;
		Roles::httpsClient = theClient;
	}

	CoRoutine<void> Roles::addGuildMemberRoleAsync(AddGuildMemberRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Member_Role };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath =
			"/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.userId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.callStack = "Roles::addGuildMemberRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Roles::removeGuildMemberRoleAsync(RemoveGuildMemberRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Member_Role };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath =
			"/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.userId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.callStack = "Roles::removeGuildMemberRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Role>> Roles::getGuildRolesAsync(GetGuildRolesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Roles };
		co_await NewThreadAwaitable<std::vector<Role>>();
		if (dataPackage.guildId == 0) {
			throw std::runtime_error{ "Roles::getRoleAsync() Error: Sorry, but you forgot to set the guildId!\n\n" };
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles";
		workload.callStack = "Roles::getGuildRolesAsync()";
		co_return Roles::httpsClient->submitWorkloadAndGetResult<RoleVector>(workload);
	}

	CoRoutine<Role> Roles::createGuildRoleAsync(CreateGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Role };
		co_await NewThreadAwaitable<Role>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles";
		workload.content = dataPackage;
		workload.callStack = "Roles::createGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto roleNew = Roles::httpsClient->submitWorkloadAndGetResult<Role>(workload);
		ModifyGuildRolePositionsData newDataPackage{};
		newDataPackage.guildId = dataPackage.guildId;
		newDataPackage.newPosition = dataPackage.position;
		newDataPackage.roleId = roleNew.id;
		auto results = modifyGuildRolePositionsAsync(newDataPackage).get();
		for (auto& value: results) {
			if (value.id == roleNew.id) {
				roleNew = value;
			}
		}
		co_return roleNew;
	}

	CoRoutine<std::vector<Role>> Roles::modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Role_Positions };
		co_await NewThreadAwaitable<std::vector<Role>>();
		std::vector<Role> currentRoles = Roles::getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		RoleData newRole = Roles::getCachedRoleAsync({ .roleId = dataPackage.roleId }).get();
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
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles";
		workload.content = dataPackage;
		workload.callStack = "Roles::modifyGuildRolePositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Roles::httpsClient->submitWorkloadAndGetResult<RoleVector>(workload);
	}

	CoRoutine<Role> Roles::modifyGuildRoleAsync(ModifyGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Role };
		co_await NewThreadAwaitable<Role>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.content = dataPackage;
		workload.callStack = "Roles::modifyGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Role theData{};
		if (Roles::cache.contains(dataPackage.roleId)) {
			theData = Roles::getCachedRoleAsync({ .guildId = dataPackage.guildId, .roleId = dataPackage.roleId }).get();
		}
		theData = Roles::httpsClient->submitWorkloadAndGetResult<Role>(workload, &theData);
		co_return theData;
	}

	CoRoutine<void> Roles::removeGuildRoleAsync(RemoveGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Role };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/roles/" + std::to_string(dataPackage.roleId);
		workload.callStack = "Roles::removeGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
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
		co_return rolesVectorNew;
	}

	CoRoutine<Role> Roles::getRoleAsync(GetRoleData dataPackage) {
		co_await NewThreadAwaitable<Role>();
		auto roles = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		if (dataPackage.guildId == 0) {
			throw std::runtime_error{ "Roles::getAsync() Error: Sorry, but you forgot to set the guildId!\n\n" };
		}
		for (auto& value: roles) {
			if (value.id == dataPackage.roleId) {
				co_return value;
			}
		}
		co_return {};
	}

	CoRoutine<RoleData> Roles::getCachedRoleAsync(GetRoleData dataPackage) {
		co_await NewThreadAwaitable<RoleData>();
		std::shared_lock theLock{ Roles::theMutex };
		if (!Roles::cache.contains(dataPackage.roleId)) {
			theLock.unlock();
			co_return Roles::getRoleAsync(dataPackage).get();
		} else if (Roles::cache.contains(dataPackage.roleId)) {
			co_return *Roles::cache[dataPackage.roleId].get();
		}
	}

	void Roles::insertRole(std::unique_ptr<RoleData> role) {
		std::unique_lock theLock{ Roles::theMutex };
		if (role->id == 0) {
			return;
		}
		if (Roles::configManager->doWeCacheRoles()) {
			auto roleId = role->id;
			if (!Roles::cache.contains(roleId)) {
				Roles::cache.emplace(roleId, std::move(role));
			} else {
				Roles::cache.insert_or_assign(roleId, std::move(role));
			}
		}
	}

	void Roles::removeRole(const Snowflake roleId) {
		std::unique_lock theLock{ Roles::theMutex };
		if (Roles::cache.contains(roleId)) {
			Roles::cache.erase(roleId);
		}
	};

	std::unordered_map<Snowflake, std::unique_ptr<RoleData>> Roles::cache{};
	DiscordCoreInternal::HttpsClient* Roles::httpsClient{ nullptr };
	ConfigManager* Roles::configManager{ nullptr };
	std::shared_mutex Roles::theMutex{};
}
