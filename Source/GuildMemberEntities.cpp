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
/// GuildMemberEntities.cpp - Source file for the GuildMember related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	AddGuildMemberData::operator std::string() {
		nlohmann::json data{};
		data["access_token"] = this->accessToken;
		data["roles"] = this->roles;
		data["deaf"] = this->deaf;
		data["mute"] = this->mute;
		data["nick"] = this->nick;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	ModifyGuildMemberData::operator std::string() {
		nlohmann::json data{};
		data["nick"] = this->nick;
		data["communication_disabled_until"] = std::string(this->communicationDisabledUntil);
		if (this->roleIds.size() == 0) {
			data["roles"] = nullptr;
		} else {
			nlohmann::json roleIdArray{};
			for (auto& value: this->roleIds) {
				roleIdArray.emplace_back(value);
			}
			data["roles"] = roleIdArray;
		}
		if (this->newVoiceChannelId != 0) {
			data["channel_id"] = std::to_string(this->newVoiceChannelId);
			data["mute"] = this->mute;
			data["deaf"] = this->deaf;
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	void GuildMemberData::insertGuildMember(std::unique_ptr<GuildMemberData> other) {
		GuildMembers::insertGuildMember(std::move(other));
	}

	void GuildMemberData::parseObject(const nlohmann::json* jsonObjectData) {
		DiscordCoreAPI::parseObject(jsonObjectData, *this);
	}

	std::string GuildMemberData::getAvatarUrl() {
		if (this->avatar.getHashUrl(this->id, this->guildId) != "") {
			return this->avatar.getHashUrl(this->id, this->guildId);
		} else if (this->userAvatar.getHashUrl(this->id, 0) != "") {
			return this->userAvatar.getHashUrl(this->id, 0);
		} else{
			return {};
		}
	}

	UserData GuildMemberData::getUserData() {
		return Users::getCachedUserAsync({ .userId = this->id }).get();
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

	GuildMember& GuildMember::operator=(GuildMemberData& other) noexcept {
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

	GuildMember::GuildMember(GuildMemberData& other) noexcept {
		*this = other;
	}

	void GuildMember::parseObject(const nlohmann::json* jsonObjectData) {
		DiscordCoreAPI::parseObject(jsonObjectData, *this);
	}

	GuildMemberVector::operator std::vector<GuildMember>() {
		return this->theGuildMembers;
	}

	GuildMemberVector& GuildMemberVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildMemberVector::GuildMemberVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	void GuildMembers::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		GuildMembers::configManager = configManagerNew;
		GuildMembers::httpsClient = theClient;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.callStack = "GuildMembers::getGuildMemberAsync()";
		GuildMember theData{};
		for (auto& value: Guilds::cache[dataPackage.guildId]->members) {
			if (dataPackage.guildMemberId == value->id) {
				theData = *value;
			}
		}
		theData = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, &theData);
		theData.guildId = dataPackage.guildId;
		co_return theData;
	}

	CoRoutine<GuildMemberData> GuildMembers::getCachedGuildMemberAsync(GetGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMemberData>();
		std::shared_lock theLock{ GuildMembers::theMutex };
		if (Guilds::cache.contains(dataPackage.guildId)) {
			for (auto& value: Guilds::cache[dataPackage.guildId]->members) {
				if (dataPackage.guildMemberId == value->id) {
					co_return *value;
				}
			}
		}
		co_return GuildMembers::getGuildMemberAsync(dataPackage).get();
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
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
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/search";
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
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.userId);
		workload.content = dataPackage;
		workload.callStack = "GuildMembers::addGuildMemberAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/@me";
		workload.content = nlohmann::json{ { "nick", dataPackage.nick } }.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
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
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.content = dataPackage;
		workload.callStack = "GuildMembers::modifyGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMember theData{};
		for (auto& value: Guilds::cache[dataPackage.guildId]->members) {
			if (dataPackage.guildMemberId == value->id) {
				theData = *value;
			}
		}
		theData = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, &theData);
		theData.guildId = dataPackage.guildId;
		co_return theData;
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.callStack = "GuildMembers::removeGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto theGuildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMember>();
		GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
		ModifyGuildMemberData dataPackage01{};
		dataPackage01.deaf = getBool<int8_t, GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Deaf);
		dataPackage01.guildId = guildMember.guildId;
		dataPackage01.guildMemberId = guildMember.id;
		dataPackage01.mute = getBool<int8_t, GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Mute);
		dataPackage01.roleIds = guildMember.roles;
		dataPackage01.nick = guildMember.nick;
		dataPackage01.reason = dataPackage.reason;
		TimeStamp<std::chrono::milliseconds> theTimeStamp{};
		switch (dataPackage.numOfMinutesToTimeoutFor) {
			case TimeoutDurations::Day: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 1, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Five_Minutes: {
				theTimeStamp.convertToFutureISO8601TimeStamp(5, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Hour: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 1, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Ten_Minutes: {
				theTimeStamp.convertToFutureISO8601TimeStamp(10, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Week: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 7, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Minute: {
				theTimeStamp.convertToFutureISO8601TimeStamp(1, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::None: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
		}
		co_return GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
	}

	void GuildMembers::insertGuildMember(std::unique_ptr<GuildMemberData> guildMember) {
		std::unique_lock theLock{ GuildMembers::theMutex };
		if (guildMember->id == 0) {
			return;
		}
		if (GuildMembers::configManager->doWeCacheGuildMembers()) {
			if (Guilds::cache.contains(guildMember->guildId)) {
				Guilds::cache[guildMember->guildId]->members.push_back(guildMember.release());
			}
		}
	}

	void GuildMembers::removeGuildMember(GuildMemberData guildMemberId) {
		std::unique_lock theLock{ GuildMembers::theMutex };
		if (Guilds::cache.contains(guildMemberId.guildId)) {
			for (int32_t x = 0; x < Guilds::cache[guildMemberId.guildId]->members.size(); ++x) {
				if (guildMemberId.id == Guilds::cache[guildMemberId.guildId]->members[x]->id) {
					delete Guilds::cache[guildMemberId.guildId]->members[x];
					Guilds::cache[guildMemberId.guildId]->members.erase(Guilds::cache[guildMemberId.guildId]->members.begin() + x);
				}
			}
		}
	};

	DiscordCoreInternal::HttpsClient* GuildMembers::httpsClient{ nullptr };
	ConfigManager* GuildMembers::configManager{ nullptr };
	std::shared_mutex GuildMembers::theMutex{};
};
