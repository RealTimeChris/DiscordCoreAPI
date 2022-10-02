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
/// RoleEntities.cpp - Source file for the Role related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file RoleEntities.cpp

#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	CreateGuildRoleData::operator JsonObject() {
		int32_t roleColorInt = stol(this->hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		JsonObject theData{};
		theData["permissions"] = this->permissions.getCurrentPermissionString();
		theData["mentionable"] = this->mentionable;
		theData["hoist"] = this->hoist;
		theData["name"] = this->name;
		theData["color"] = roleColorReal;
		if (this->icon.size() > 0) {
			theData["icon"] = this->icon;
		}
		if (this->unicodeEmoji.size() > 0) {
			theData["unicode_emoji"] = this->unicodeEmoji;
		}
		return theData;
	}

	ModifyGuildRolePositionsData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->rolePositions) {
			JsonObject newData{};
			newData["position"] = value.rolePosition;
			newData["id"] = std::to_string(value.roleId);
			theData.pushBack(newData);
		}
		return theData;
	}

	ModifyGuildRoleData::operator JsonObject() {
		int32_t roleColorInt = stol(this->hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		JsonObject theData{};
		theData["permissions"] = this->permissions.getCurrentPermissionString();
		theData["mentionable"] = this->mentionable;
		theData["hoist"] = this->hoist;
		theData["name"] = this->name;
		theData["color"] = roleColorReal;
		if (this->icon.size() > 0) {
			theData["icon"] = this->icon;
		}
		if (this->unicodeEmoji.size() > 0) {
			theData["unicode_emoji"] = this->unicodeEmoji;
		}
		return theData;
	}

	Role::Role(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		std::stringstream theStream{};
		theStream << getString(jsonObjectData, "unicode_emoji");
		for (auto& value: theStream.str()) {
			this->unicodeEmoji.emplace_back(value);
		}
		if (this->unicodeEmoji.size() > 3) {
			this->unicodeEmoji = static_cast<std::string>(this->unicodeEmoji).substr(1, this->unicodeEmoji.size() - 3);
		}

		this->color = getUint32(jsonObjectData, "color");

		this->flags |= setBool(this->flags, RoleFlags::Hoist, getBool(jsonObjectData, "hoist"));

		this->position = getUint32(jsonObjectData, "position");

		this->permissions = getString(jsonObjectData, "permissions");

		this->flags |= setBool(this->flags, RoleFlags::Managed, getBool(jsonObjectData, "managed"));

		this->flags |= setBool(this->flags, RoleFlags::Mentionable, getBool(jsonObjectData, "mentionable"));

		simdjson::ondemand::value theRoleTagsNew{};
		auto theResult = jsonObjectData["tags"].get(theRoleTagsNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			RoleTagsData theRoleTags{ theRoleTagsNew };
			this->tags = std::move(theRoleTags);
		}
	}

	RoleVector::RoleVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Role newData{ value.value() };
					this->theRoles.emplace_back(std::move(newData));
				}
			}
		}
	}

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

	RoleVector::operator std::vector<Role>() {
		return this->theRoles;
	}

	void Roles::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		Roles::doWeCacheRoles = configManagerNew->doWeCacheRoles();
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
		if (dataPackage.guildId.operator size_t() == 0) {
			throw std::runtime_error{ "Roles::getGuildRolesAsync() Error: Sorry, but you forgot to set the guildId!\n\n" };
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Roles::modifyGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		RoleData theData{};
		theData.id = dataPackage.roleId;
		if (!Roles::cache.contains(theData)) {
			Roles::cache.emplace(theData);
		}
		Role theDataNew{};
		theDataNew = Roles::cache.at(theData);
		theDataNew = Roles::httpsClient->submitWorkloadAndGetResult<Role>(workload, &theDataNew);
		Roles::insertRole(theDataNew);
		co_return theDataNew;
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
		if (dataPackage.guildId.operator size_t() == 0) {
			throw std::runtime_error{ "Roles::getRoleAsync() Error: Sorry, but you forgot to set the guildId!\n\n" };
		}
		for (auto& value: roles) {
			if (value.id == dataPackage.roleId) {
				value.guildId = dataPackage.guildId;
				co_return value;
			}
		}
		co_return {};
	}

	CoRoutine<RoleData> Roles::getCachedRoleAsync(GetRoleData dataPackage) {
		co_await NewThreadAwaitable<RoleData>();
		RoleData theData{};
		theData.id = dataPackage.roleId;
		if (!Roles::cache.contains(theData)) {
			co_return Roles::getRoleAsync(dataPackage).get();
		} else if (Roles::cache.contains(theData)) {
			theData = Roles::cache.at(theData);
			co_return theData;
		}
	}
	StopWatch theStopWatchNew{ 5s };
	void Roles::insertRole(RoleData role) {
		if (role.id.operator size_t() == 0) {
			return;
		}
		if (Roles::doWeCacheRoles) {
			Roles::cache.emplace(std::move(role));
			if (Roles::cache.size() % 1000 == 0) {
				std::cout << "ROLE COUNT: " << Roles::cache.size() << ", AFTER: " << theStopWatchNew.totalTimePassed() << "s" << std::endl;
			}
		}
	}

	void Roles::removeRole(const Snowflake roleId) {
		RoleData theData{};
		theData.id = roleId;
		Roles::cache.erase(theData);
	};

	DiscordCoreInternal::HttpsClient* Roles::httpsClient{ nullptr };
	ObjectCache<RoleData> Roles::cache{};
	bool Roles::doWeCacheRoles{ false };
}
