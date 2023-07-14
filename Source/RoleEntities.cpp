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
/// RoleEntities.cpp - Source file for the RoleData related classes and structs.
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

	RoleCacheData& RoleCacheData::operator=(const RoleData& other) {
		setFlagValue<RoleFlags>(RoleFlags::Mentionable, other.mentionable);
		setFlagValue<RoleFlags>(RoleFlags::Managed, other.managed);
		setFlagValue<RoleFlags>(RoleFlags::Hoist, other.hoist);
		if (other.unicodeEmoji != "") {
			unicodeEmoji = other.unicodeEmoji;
		}
		if (other.permissions != "") {
			permissions = other.permissions;
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
		flags = other.flags;
		return *this;
	}

	RoleCacheData::RoleCacheData(const RoleData& other) {
		*this = other;
	}

	RoleCacheData& RoleCacheData::operator=(RoleData&& other) {
		setFlagValue<RoleFlags>(RoleFlags::Mentionable, other.mentionable);
		setFlagValue<RoleFlags>(RoleFlags::Managed, other.managed);
		setFlagValue<RoleFlags>(RoleFlags::Hoist, other.hoist);
		if (other.unicodeEmoji != "") {
			unicodeEmoji = std::move(other.unicodeEmoji);
		}
		if (other.permissions != "") {
			permissions = std::move(other.permissions);
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
		flags = other.flags;
		return *this;
	}

	RoleCacheData::operator DiscordCoreAPI::RoleData() {
		RoleData returnData{};
		returnData.mentionable = getFlagValue<RoleFlags>(RoleFlags::Mentionable);
		returnData.managed = getFlagValue<RoleFlags>(RoleFlags::Managed);
		returnData.unicodeEmoji = unicodeEmoji.operator std::string();
		returnData.hoist = getFlagValue<RoleFlags>(RoleFlags::Hoist);
		returnData.permissions = permissions.operator std::string();
		returnData.name = name.operator std::string();
		returnData.position = position;
		returnData.flags = flags;
		returnData.color = color;
		returnData.id = id;
		return returnData;
	}

	RoleCacheData::RoleCacheData(RoleData&& other) {
		*this = std::move(other);
	}

	RoleData::RoleData(Snowflake newId) {
		id = newId;
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
		Roles::httpsClient->submitWorkloadAndGetResult(std::move(workload));
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
		Roles::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<std::vector<RoleData>> Roles::getGuildRolesAsync(GetGuildRolesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Roles };
		co_await NewThreadAwaitable<std::vector<RoleData>>();
		if (dataPackage.guildId == 0) {
			throw DCAException{ "Roles::getGuildRolesAsync() Error: Sorry, but you forgot to set the guildId!", std::source_location::current() };
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
		workload.callStack = "Roles::getGuildRolesAsync()";
		std::vector<RoleData> returnData{};
		Roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<RoleData> Roles::createGuildRoleAsync(CreateGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Role };
		co_await NewThreadAwaitable<RoleData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Roles::createGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		RoleData returnData{};
		Roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
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
		co_return returnData;
	}

	CoRoutine<std::vector<RoleData>> Roles::modifyGuildRolePositionsAsync(ModifyGuildRolePositionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Role_Positions };
		co_await NewThreadAwaitable<std::vector<RoleData>>();
		std::vector<RoleData> currentRoles = Roles::getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
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
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Roles::modifyGuildRolePositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		std::vector<RoleData> returnData{};
		Roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<RoleData> Roles::modifyGuildRoleAsync(ModifyGuildRoleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Role };
		co_await NewThreadAwaitable<RoleData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Roles::modifyGuildRoleAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		RoleData data{ dataPackage.roleId };
		if (cache.contains(dataPackage.roleId)) {
			data = cache[dataPackage.roleId];
		}
		Roles::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheRolesBool) {
			insertRole(data);
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
		Roles::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<std::vector<RoleData>> Roles::getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage) {
		co_await NewThreadAwaitable<std::vector<RoleData>>();
		std::vector<RoleData> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		std::vector<RoleData> rolesVectorNew{};
		for (auto& value: rolesVector) {
			for (auto& value2: dataPackage.guildMember.roles) {
				if (value2 == value.id) {
					rolesVectorNew.emplace_back(value);
				}
			}
		}
		co_return std::move(rolesVectorNew);
	}

	CoRoutine<RoleData> Roles::getRoleAsync(GetRoleData dataPackage) {
		co_await NewThreadAwaitable<RoleData>();
		auto roles = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
		if (dataPackage.guildId == 0) {
			throw DCAException{ "Roles::getRoleAsync() Error: Sorry, but you forgot to set the guildId!", std::source_location::current() };
		}
		RoleData data{ dataPackage.roleId };
		if (cache.contains(dataPackage.roleId)) {
			data = cache[dataPackage.roleId];
		}
		for (auto& value: roles) {
			if (value.id == dataPackage.roleId) {
				data = std::move(value);
			}
		}
		if (doWeCacheRolesBool) {
			insertRole(std::move(data));
			data = cache[dataPackage.roleId];
		}
		co_return data;
	}

	RoleCacheData Roles::getCachedRole(GetRoleData dataPackage) {
		RoleData data{};
		data.id = dataPackage.roleId;
		if (!cache.contains(dataPackage.roleId)) {
			return Roles::getRoleAsync(dataPackage).get();
		} else {
			return cache[data.id];
		}
	}

	StopWatch<Milliseconds> stopWatchNew{ 5ms };

	void Roles::insertRole(const RoleData& role) {
		if (doWeCacheRolesBool) {
			if (role.id == 0) {
				throw DCAException{ "Sorry, but there was no id set for that role.", std::source_location::current() };
			}
			cache.emplace(role);
			if (cache.count() % 1000 == 0) {
				std::cout << "ROLE COUNT: " << cache.count() << std::endl;
			}
		}
	}

	void Roles::removeRole(const Snowflake roleId) {
		cache.erase(roleId);
	};

	bool Roles::doWeCacheRoles() {
		return Roles::doWeCacheRolesBool;
	}

	DiscordCoreInternal::HttpsClient* Roles::httpsClient{};
	ObjectCache<Snowflake, RoleCacheData> Roles::cache{};
	bool Roles::doWeCacheRolesBool{};
};
