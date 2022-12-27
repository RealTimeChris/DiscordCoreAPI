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

	CreateGuildRoleData::operator Jsonifier() {
		int32_t roleColorInt = stol(this->hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		Jsonifier data{};
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
		return data;
	}

	ModifyGuildRolePositionsData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->rolePositions) {
			Jsonifier newData{};
			newData["position"] = value.rolePosition;
			newData["id"] = value.roleId;
			data.emplaceBack(newData);
		}
		return data;
	}

	ModifyGuildRoleData::operator Jsonifier() {
		int32_t roleColorInt = stol(this->hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		Jsonifier data{};
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
		return data;
	}

	Role::Role(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		std::stringstream stream{};
		stream << getString(jsonObjectData, "unicode_emoji");
		for (auto& value: stream.str()) {
			this->unicodeEmoji.emplace_back(value);
		}
		if (this->unicodeEmoji.size() > 3) {
			this->unicodeEmoji = static_cast<std::string>(this->unicodeEmoji).substr(1, this->unicodeEmoji.size() - 3);
		}

		this->color = getUint32(jsonObjectData, "color");

		uint8_t newFlags{};

		newFlags = setBool(newFlags, RoleFlags::Hoist, getBool(jsonObjectData, "hoist"));

		this->position = getUint32(jsonObjectData, "position");

		this->permissions = getString(jsonObjectData, "permissions");

		newFlags = setBool(newFlags, RoleFlags::Managed, getBool(jsonObjectData, "managed"));

		newFlags = setBool(newFlags, RoleFlags::Mentionable, getBool(jsonObjectData, "mentionable"));

		this->flags = static_cast<RoleFlags>(newFlags);

		simdjson::ondemand::value roleTagsNew{};
		if (jsonObjectData["tags"].get(roleTagsNew) == simdjson::error_code::SUCCESS) {
			RoleTagsData roleTags{ roleTagsNew };
			this->tags = std::move(roleTags);
		}
	}

	RoleVector::RoleVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					Role newData{ value.value() };
					this->roles.emplace_back(std::move(newData));
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
		return this->roles;
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
		co_return Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
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
		co_return Roles::httpsClient->submitWorkloadAndGetResult<void>(workload);
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
		RoleVector returnValue{};
		co_return Roles::httpsClient->submitWorkloadAndGetResult<RoleVector>(workload, &returnValue);
	}

	CoRoutine<Role> Roles::createGuildRoleAsync(CreateGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Role };
		co_await NewThreadAwaitable<Role>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Roles::createGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Role returnValue{};
		auto roleNew = Roles::httpsClient->submitWorkloadAndGetResult<Role>(workload, &returnValue);
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
		co_return std::move(roleNew);
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
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Roles::modifyGuildRolePositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		RoleVector returnValue{};
		co_return Roles::httpsClient->submitWorkloadAndGetResult<RoleVector>(workload, &returnValue);
	}

	CoRoutine<Role> Roles::modifyGuildRoleAsync(ModifyGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Role };
		co_await NewThreadAwaitable<Role>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Roles::modifyGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Role data{};
		data = Roles::httpsClient->submitWorkloadAndGetResult<Role>(workload, &data);
		if (Roles::cache.contains(data)) {
			data = Roles::cache.at(data);
		} else {
			Roles::insertRole(data);
		}
		co_return std::move(data);
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
		if (!Roles::cache.contains(data)) {
			return Roles::getRoleAsync(dataPackage).get();
		} else {
			data = Roles::cache.at(data);
			return data;
		}
	}
	StopWatch<Milliseconds> stopWatchNew{ 5ms };
	void Roles::insertRole(RoleData role) {
		if (role.id == 0) {
			return;
		}
		if (Roles::doWeCacheRoles()) {
			if (!Roles::cache.contains(role)) {
				Roles::cache.emplace(std::move(role));
			} else {
				Roles::cache[role] = std::move(role);
			}
			if (Roles::cache.size() % 10000 == 0) {
				//std::cout << "ROLE COUNT: " << Roles::cache.size() << ", AFTER: " << stopWatchNew.totalTimePassed().count() << "s"
				//<< std::endl;
			}
		}
	}

	void Roles::removeRole(const Snowflake roleId) {
		RoleData data{};
		data.id = roleId;
		Roles::cache.erase(data);
	};

	bool Roles::doWeCacheRoles() {
		return Roles::doWeCacheRolesBool;
	}

	DiscordCoreInternal::HttpsClient* Roles::httpsClient{ nullptr };
	bool Roles::doWeCacheRolesBool{};
	ObjectCache<RoleData> Roles::cache{};
}
