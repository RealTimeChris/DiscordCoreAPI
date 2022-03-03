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
/// GuildMemberEntities.cpp - Source file for the GuildMember related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file GuildMemberEntities.cpp

#include <FoundationEntities.hpp>
#include <GuildMemberEntities.hpp>
#include <GuildEntities.hpp>
#include <CoRoutine.hpp>
#include <Http.hpp>
#include <JSONIfier.hpp>

namespace DiscordCoreAPI {

	GuildMember::GuildMember(GuildMemberData dataNew) {
		this->premiumSince = dataNew.premiumSince;
		this->permissions = dataNew.permissions;
		this->userMention = dataNew.userMention;
		this->joinedAt = dataNew.joinedAt;
		this->pending = dataNew.pending;
		this->guildId = dataNew.guildId;
		this->roles = dataNew.roles;
		this->user = dataNew.user;
		this->nick = dataNew.nick;
		this->deaf = dataNew.deaf;
		this->mute = dataNew.mute;
	};

	void GuildMembers::initialize(DiscordCoreInternal::HttpClient*theClient) {
		GuildMembers::httpClient = theClient;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMember>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.callStack = "GuildMembers::getGuildMemberAsync";
			auto guildMember = DiscordCoreInternal::submitWorkloadAndGetResult<GuildMember>(*GuildMembers::httpClient, workload);
			guildMember.guildId = dataPackage.guildId;
			GuildMembers::insertGuildMember(guildMember);
			co_return guildMember;
		}
		catch (...) {
			reportException("GuildMembers::getGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::getCachedGuildMemberAsync(GetGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMember>();
			GuildMember guildMember{};
			if (GuildMembers::cache.contains(dataPackage.guildId + " + " + dataPackage.guildMemberId)) {
				guildMember = GuildMembers::cache[dataPackage.guildId + " + " + dataPackage.guildMemberId];
				guildMember.guildId = dataPackage.guildId;
				co_return guildMember;
			}
			else {
				guildMember = GuildMembers::getGuildMemberAsync(dataPackage).get();
				guildMember.guildId = dataPackage.guildId;
				co_return guildMember;
			}
		}
		catch (...) {
			reportException("GuildMembers::getCachedGuildMemberAsync()");
		}
	}
	
	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<GuildMember>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Members;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members";
			if (dataPackage.after != "") {
				workload.relativePath += "?after=" + dataPackage.after;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "GuildMembers::listGuildMembersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildMember>>(*GuildMembers::httpClient, workload);
		}
		catch (...) {
			reportException("GuildMembers::listGuildMembersAsync()");
		}
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::searchGuildMembersAsync(SearchGuildMembersData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<GuildMember>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Search_Guild_Members;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/search";
			if (dataPackage.query != "") {
				workload.relativePath += "?query=" + dataPackage.query;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "GuildMembers::searchGuildMembersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildMember>>(*GuildMembers::httpClient, workload);
		}
		catch (...) {
			reportException("GuildMembers::searchGuildMembersAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::addGuildMemberAsync(AddGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMember>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "GuildMembers::addGuildMemberAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildMember>(*GuildMembers::httpClient, workload);
		}
		catch (...) {
			reportException("GuildMembers::addGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMember>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Current_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/@me";
			workload.content = nlohmann::json{ {"nick",dataPackage.nick } }.dump();
			workload.callStack = "GuildMembers::modifyCurrentGuildMemberAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildMember>(*GuildMembers::httpClient, workload);
		}
		catch (...) {
			reportException("GuildMembers::modifyCurrentGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMember>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "GuildMembers::modifyGuildMemberAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			auto guildMember = DiscordCoreInternal::submitWorkloadAndGetResult<GuildMember>(*GuildMembers::httpClient, workload);
			GuildMembers::insertGuildMember(guildMember);
			co_return guildMember;
		}
		catch (...) {
			reportException("GuildMembers::modifyGuildMemberAsync()");
		}
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.callStack = "GuildMembers::removeGuildMemberAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*GuildMembers::httpClient, workload);
		}
		catch (...) {
			reportException("GuildMembers::removeGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMember>();
			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
			ModifyGuildMemberData dataPackage01{};
			dataPackage01.currentChannelId = guildMember.voiceData.channelId;
			dataPackage01.deaf = guildMember.deaf;
			dataPackage01.guildId = guildMember.guildId;
			dataPackage01.guildMemberId = guildMember.user.id;
			dataPackage01.mute = guildMember.mute;
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
		catch (...) {
			reportException("GuildMembers::modifyGuildMemberAsync()");
		}
	}

	void GuildMembers::insertGuildMember(GuildMember guildMember) {
		try {
			if (guildMember.user.id == "") {
				return;
			}
			GuildMembers::cache.insert_or_assign(guildMember.guildId + " + " + guildMember.user.id, guildMember);
		}
		catch (...) {
			reportException("GuildMembers::insertGuildMember()");
		}
	}

	void GuildMembers::removeGuildMember(GuildMember guildMember) {
		try {
			GuildMembers::cache.erase(guildMember.guildId + " + " + guildMember.user.id);
		}
		catch (...) {
			reportException("Guilds::removeGuild()");
		}
	};
	DiscordCoreInternal::HttpClient* GuildMembers::httpClient{ nullptr };
	std::unordered_map<std::string, GuildMember> GuildMembers::cache{};
};
