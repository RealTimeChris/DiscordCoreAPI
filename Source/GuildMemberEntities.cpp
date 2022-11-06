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
/// GuildMemberEntities.cpp - Source file for the GuildMember related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	GuildMemberVector::GuildMemberVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (jsonObjectData.get(arrayValue) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildMember newData{ value.value() };
					this->guildMembers.emplace_back(std::move(newData));
				}
			}
		}
	}

	AddGuildMemberData::operator Jsonifier() {
		Jsonifier data{};
		data["access_token"] = this->accessToken;
		for (auto& value: this->roles) {
			data["roles"].emplaceBack(value.operator std::string());
		}
		data["deaf"] = this->deaf;
		data["mute"] = this->mute;
		data["nick"] = this->nick;
		return data;
	}

	ModifyGuildMemberData::operator Jsonifier() {
		Jsonifier data{};
		data["nick"] = this->nick;
		data["communication_disabled_until"] = static_cast<std::string>(this->communicationDisabledUntil);
		for (auto& value: this->roleIds) {
			data["roles"].emplaceBack(value.operator std::string());
		}
		if (this->newVoiceChannelId != 0) {
			data["channel_id"] = this->newVoiceChannelId;
			data["mute"] = this->mute;
			data["deaf"] = this->deaf;
		}
		return data;
	}

	GuildMember& GuildMember::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			this->permissions = std::move(other.permissions);
			this->voiceChannelId = other.voiceChannelId;
			this->joinedAt = std::move(other.joinedAt);
			this->avatar = std::move(other.avatar);
			this->roles = std::move(other.roles);
			this->nick = std::move(other.nick);
			this->guildId = other.guildId;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	};

	GuildMember::GuildMember(GuildMemberData&& other) noexcept {
		*this = std::move(other);
	}

	GuildMember& GuildMember::operator=(const GuildMemberData& other) noexcept {
		if (this != &other) {
			this->voiceChannelId = other.voiceChannelId;
			this->permissions = other.permissions;
			this->joinedAt = other.joinedAt;
			this->guildId = other.guildId;
			this->avatar = other.avatar;
			this->roles = other.roles;
			this->flags = other.flags;
			this->nick = other.nick;
			this->id = other.id;
		}
		return *this;
	};

	GuildMember::GuildMember(const GuildMemberData& other) noexcept {
		*this = other;
	}

	GuildMember::GuildMember(simdjson::ondemand::value jsonObjectData) {
		this->flags = setBool(this->flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		this->flags = setBool(this->flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		this->flags = setBool(this->flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array arrayValue{};
		if (jsonObjectData["roles"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->roles.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->roles.emplace_back(getId(value.value()));
			}
		}

		this->permissions = getString(jsonObjectData, "permissions");

		simdjson::ondemand::value object{};
		if (jsonObjectData["user"].get(object) == simdjson::error_code::SUCCESS) {
			UserData user{ object };
			this->id = user.id;
			Users::insertUser(std::move(user));
		}

		this->avatar = getString(jsonObjectData, "avatar");

		auto theFlags = getUint8(jsonObjectData, "flags");

		this->flags = setBool(this->flags, static_cast<GuildMemberFlags>(theFlags), true);

		this->nick = getString(jsonObjectData, "nick");

		this->communicationDisabledUntil = getString(jsonObjectData, "communication_disabled_until");

		this->premiumSince = getString(jsonObjectData, "premium_since");
	}

	GuildMemberVector::operator std::vector<GuildMember>() {
		return this->guildMembers;
	}

	void GuildMembers::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		GuildMembers::doWeCacheGuildMembersBool = configManagerNew->doWeCacheUsers();
		GuildMembers::httpsClient = client;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack = "GuildMembers::getGuildMemberAsync()";
		GuildMember data{};
		data = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, &data);
		if (GuildMembers::cache.contains(data)) {
			data = GuildMembers::cache.at(data);
		} else {
			GuildMembers::cache.emplace(data);
			GuildMembers::insertGuildMember(data);
		}
		co_return std::move(data);
	}

	GuildMemberData GuildMembers::getCachedGuildMember(GetGuildMemberData dataPackage) {
		GuildMemberData key{};
		key.id = dataPackage.guildMemberId;
		key.guildId = dataPackage.guildId;
		if (GuildMembers::cache.contains(key)) {
			return GuildMembers::cache.at(key);
		}
		return GuildMembers::getGuildMemberAsync(dataPackage).get();
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::listGuildMembersAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMemberVector>(workload);
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::searchGuildMembersAsync(SearchGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Search_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/search";
		if (dataPackage.query != "") {
			workload.relativePath += "?query=" + dataPackage.query;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::searchGuildMembersAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMemberVector>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::addGuildMemberAsync(AddGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "GuildMembers::addGuildMemberAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/@me";
		Jsonifier data{};
		data["nick"] = dataPackage.nick;
		data.refreshString(JsonifierSerializeType::Json);
		workload.content = data.operator std::string();
		workload.callStack = "GuildMembers::modifyCurrentGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "GuildMembers::modifyGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMember data{};
		data = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, &data);
		if (GuildMembers::cache.contains(data)) {
			data = GuildMembers::cache.at(data);
		} else {
			GuildMembers::cache.emplace(data);
			GuildMembers::insertGuildMember(data);
		}
		co_return std::move(data);
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack = "GuildMembers::removeGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto guildMember = GuildMembers::getCachedGuildMember({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId });
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMember>();
		GuildMemberData guildMember =
			GuildMembers::getCachedGuildMember({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId });
		ModifyGuildMemberData dataPackage01{};
		dataPackage01.deaf = getBool<GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Deaf);
		dataPackage01.guildId = guildMember.guildId;
		dataPackage01.guildMemberId = guildMember.id;
		dataPackage01.mute = getBool<GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Mute);
		dataPackage01.roleIds = guildMember.roles;
		dataPackage01.nick = guildMember.nick;
		dataPackage01.reason = dataPackage.reason;
		TimeStamp<std::chrono::milliseconds> timeStamp{};
		switch (dataPackage.numOfMinutesToTimeoutFor) {
			case TimeoutDurations::Day: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 1, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Five_Minutes: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(5, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Hour: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 1, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Ten_Minutes: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(10, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Week: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 7, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Minute: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(1, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::None: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
		}
		co_return GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
	}

	void GuildMembers::insertGuildMember(GuildMemberData guildMember) {
		if (guildMember.id == 0) {
			return;
		}
		if (GuildMembers::doWeCacheGuildMembers()) {
			if (!GuildMembers::cache.contains(guildMember)) {
				GuildMembers::cache.emplace(std::move(guildMember));
			} else {
				GuildMembers::cache[guildMember] = std::move(guildMember);
			}
			if (GuildMembers::cache.size() % 10000 == 0) {
				//std::cout << "THE GUILDMEMBER COUNT: " << GuildMembers::cache.size() << std::endl;
			}
		}
	}

	void GuildMembers::removeGuildMember(GuildMemberData guildMember) {
		GuildMembers::cache.erase(guildMember);
	};

	bool GuildMembers::doWeCacheGuildMembers() {
		return GuildMembers::doWeCacheGuildMembersBool;
	}

	DiscordCoreInternal::HttpsClient* GuildMembers::httpsClient{ nullptr };
	bool GuildMembers::doWeCacheGuildMembersBool{ false };
	ObjectCache<GuildMemberData> GuildMembers::cache{};
};
