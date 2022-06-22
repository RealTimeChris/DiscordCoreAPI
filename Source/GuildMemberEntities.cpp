/*
*
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
#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	GuildMember& GuildMember::operator=(GuildMemberData&& other) {
		if (this != &other) {
			this->permissions = std::move(other.permissions);
			this->userAvatar = std::move(other.userAvatar);
			this->userName = std::move(other.userName);
			this->joinedAt = std::move(other.joinedAt);
			this->roles = std::move(other.roles);
			this->nick = std::move(other.nick);
			this->guildId = other.guildId;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	};

	GuildMember::GuildMember(GuildMemberData&& other) {
		*this = std::move(other);
	}

	GuildMember& GuildMember::operator=(GuildMemberData& other) {
		if (this != &other) {
			this->permissions = other.permissions;
			this->userAvatar = other.userAvatar;
			this->userName = other.userName;
			this->joinedAt = other.joinedAt;
			this->guildId = other.guildId;
			this->roles = other.roles;
			this->flags = other.flags;
			this->nick = other.nick;
			this->id = other.id;
		}
		return *this;
	};

	GuildMember::GuildMember(GuildMemberData& other) {
		*this = other;
	}

	void GuildMembers::initialize(DiscordCoreInternal::HttpClient* theClient, bool doWeCacheNew) {
		GuildMembers::cache = std::make_unique<std::map<GuildMemberId, std::unique_ptr<GuildMemberData>>>();
		GuildMembers::doWeCache = doWeCacheNew;
		GuildMembers::httpClient = theClient;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Member);
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Member;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.callStack = "GuildMembers::getGuildMemberAsync";
		auto guildMember = GuildMembers::httpClient->submitWorkloadAndGetResult<GuildMember>(workload);
		guildMember.guildId = dataPackage.guildId;
		GuildMembers::insertGuildMember(guildMember);
		co_return guildMember;
	}

	CoRoutine<GuildMemberData> GuildMembers::getCachedGuildMemberAsync(GetGuildMemberData dataPackage) {
		std::shared_lock<std::shared_mutex> theLock{ GuildMembers::theMutex };
		co_await NewThreadAwaitable<GuildMemberData>();
		GuildMemberId theKey{};
		theKey.guildId = dataPackage.guildId;
		theKey.guildMemberId = dataPackage.guildMemberId;
		if (GuildMembers::cache->contains(theKey)) {
			co_return *(*GuildMembers::cache)[theKey];
		} else {
			co_return GuildMembers::getGuildMemberAsync(dataPackage).get();
		}
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Members);
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Members;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::listGuildMembersAsync";
		co_return GuildMembers::httpClient->submitWorkloadAndGetResult<std::vector<GuildMember>>(workload);
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::searchGuildMembersAsync(SearchGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Search_Guild_Members);
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Search_Guild_Members;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/search";
		if (dataPackage.query != "") {
			workload.relativePath += "?query=" + dataPackage.query;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::searchGuildMembersAsync";
		co_return GuildMembers::httpClient->submitWorkloadAndGetResult<std::vector<GuildMember>>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::addGuildMemberAsync(AddGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member);
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.userId);
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "GuildMembers::addGuildMemberAsync";
		co_return GuildMembers::httpClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Current_Guild_Member);
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Current_Guild_Member;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/@me";
		workload.content = nlohmann::json{ { "nick", dataPackage.nick } }.dump();
		workload.callStack = "GuildMembers::modifyCurrentGuildMemberAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return GuildMembers::httpClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Member);
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Member;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "GuildMembers::modifyGuildMemberAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto guildMember = GuildMembers::httpClient->submitWorkloadAndGetResult<GuildMember>(workload);
		GuildMembers::insertGuildMember(guildMember);
		co_return guildMember;
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.callStack = "GuildMembers::removeGuildMemberAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return GuildMembers::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMember>();
		GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
		VoiceStateData voiceStateData = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get().voiceStates.at(dataPackage.guildId);
		ModifyGuildMemberData dataPackage01{};
		dataPackage01.currentChannelId = voiceStateData.channelId;
		dataPackage01.deaf = getBool<int8_t, GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Deaf);
		dataPackage01.guildId = guildMember.guildId;
		dataPackage01.guildMemberId = guildMember.id;
		dataPackage01.mute = getBool<int8_t, GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Mute);
		dataPackage01.nick = guildMember.nick;
		dataPackage01.roleIds = guildMember.roles;
		dataPackage01.reason = dataPackage.reason;
		switch (dataPackage.numOfMinutesToTimeoutFor) {
			case TimeoutDurations::Day: {
				dataPackage01.communicationDisabledUntil = getFutureISO8601TimeStamp(0, 0, 1, 0, 0);
				break;
			}
			case TimeoutDurations::Five_Minutes: {
				dataPackage01.communicationDisabledUntil = getFutureISO8601TimeStamp(5, 0, 0, 0, 0);
				break;
			}
			case TimeoutDurations::Hour: {
				dataPackage01.communicationDisabledUntil = getFutureISO8601TimeStamp(0, 1, 0, 0, 0);
				break;
			}
			case TimeoutDurations::Ten_Minutes: {
				dataPackage01.communicationDisabledUntil = getFutureISO8601TimeStamp(10, 0, 0, 0, 0);
				break;
			}
			case TimeoutDurations::Week: {
				dataPackage01.communicationDisabledUntil = getFutureISO8601TimeStamp(0, 0, 7, 0, 0);
				break;
			}
			case TimeoutDurations::Minute: {
				dataPackage01.communicationDisabledUntil = getFutureISO8601TimeStamp(1, 0, 0, 0, 0);
				break;
			}
			case TimeoutDurations::None: {
				dataPackage01.communicationDisabledUntil = getFutureISO8601TimeStamp(0, 0, 0, 0, 0);
				break;
			}
		}
		GuildMember newGuildMember = GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
		co_return newGuildMember;
	}

	void GuildMembers::insertGuildMember(GuildMemberData guildMember) {
		std::unique_lock<std::shared_mutex> theLock{ GuildMembers::theMutex };
		if (guildMember.id == 0) {
			return;
		}
		auto newCache = std::make_unique<std::map<GuildMemberId, std::unique_ptr<GuildMemberData>>>();
		for (auto& [key, value]: *GuildMembers::cache) {
			(*newCache)[key] = std::move(value);
		}
		if (GuildMembers::doWeCache) {
			GuildMemberId theKey{};
			theKey.guildId = guildMember.guildId;
			theKey.guildMemberId = guildMember.id;
			(*newCache)[theKey] = std::make_unique<GuildMemberData>(guildMember);
		}
		GuildMembers::cache.reset(nullptr);
		GuildMembers::cache = std::move(newCache);
	}

	void GuildMembers::removeGuildMember(GuildMember& guildMember) {
		std::unique_lock<std::shared_mutex> theLock{ GuildMembers::theMutex };
		GuildMemberId theKey{};
		theKey.guildId = guildMember.guildId;
		theKey.guildMemberId = guildMember.id;
		GuildMembers::cache->erase(theKey);
	};

	std::unique_ptr<std::map<GuildMemberId, std::unique_ptr<GuildMemberData>>> GuildMembers::cache{};
	DiscordCoreInternal::HttpClient* GuildMembers::httpClient{ nullptr };
	std::shared_mutex GuildMembers::theMutex{};
	bool GuildMembers::doWeCache{ false };
};
