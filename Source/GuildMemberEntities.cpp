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
/// GuildMemberEntities.cpp - Source file for the GuildMember related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	void GuildMembers::initialize(DiscordCoreInternal::HttpClient* theClient) {
		GuildMembers::httpClient = theClient;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Member);
			co_await NewThreadAwaitable<GuildMember>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.callStack = "GuildMembers::getGuildMemberAsync";
			auto guildMemberNew = DiscordCoreInternal::submitWorkloadAndGetResult<GuildMember>(*GuildMembers::httpClient, workload);
			GuildMembers::insertGuildMember(guildMemberNew);
			std::cout << "WERE NOT HERE010101" << std::endl;
			guildMemberNew = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
			std::cout << "WERE NOT HERE02020202" << std::endl;
			co_return guildMemberNew;
		} catch (...) {
			reportException("GuildMembers::getGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMemberData> GuildMembers::getCachedGuildMemberAsync(GetGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMemberData>();
			std::cout << "WERE NOT 030303" << std::endl;
			std::cout << "GUILD ID: " << dataPackage.guildId << " MEMBER ID: " << dataPackage.guildMemberId << std::endl;
			if (GuildMembers::cache.contains(dataPackage.guildId + " + " + dataPackage.guildMemberId)) {
				co_return GuildMembers::cache[dataPackage.guildId + " + " + dataPackage.guildMemberId];
			} else {
				std::cout << "WERE NOT 404040" << std::endl;
				co_return GuildMembers::getGuildMemberAsync(dataPackage).get();
			}
		} catch (...) {
			reportException("GuildMembers::getCachedGuildMemberAsync()");
		}
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Members);
			co_await NewThreadAwaitable<std::vector<GuildMember>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Members;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members";
			if (dataPackage.after != "") {
				workload.relativePath += "?after=" + dataPackage.after;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			} else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "GuildMembers::listGuildMembersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildMember>>(*GuildMembers::httpClient, workload);
		} catch (...) {
			reportException("GuildMembers::listGuildMembersAsync()");
		}
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::searchGuildMembersAsync(SearchGuildMembersData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Search_Guild_Members);
			co_await NewThreadAwaitable<std::vector<GuildMember>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Search_Guild_Members;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/search";
			if (dataPackage.query != "") {
				workload.relativePath += "?query=" + dataPackage.query;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			} else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "GuildMembers::searchGuildMembersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildMember>>(*GuildMembers::httpClient, workload);
		} catch (...) {
			reportException("GuildMembers::searchGuildMembersAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::addGuildMemberAsync(AddGuildMemberData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member);
			co_await NewThreadAwaitable<GuildMember>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "GuildMembers::addGuildMemberAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildMember>(*GuildMembers::httpClient, workload);
		} catch (...) {
			reportException("GuildMembers::addGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Current_Guild_Member);
			co_await NewThreadAwaitable<GuildMember>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Current_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/@me";
			workload.content = nlohmann::json{ { "nick", dataPackage.nick } }.dump();
			workload.callStack = "GuildMembers::modifyCurrentGuildMemberAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildMember>(*GuildMembers::httpClient, workload);
		} catch (...) {
			reportException("GuildMembers::modifyCurrentGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Member);
			co_await NewThreadAwaitable<GuildMember>();
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
		} catch (...) {
			reportException("GuildMembers::modifyGuildMemberAsync()");
		}
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.callStack = "GuildMembers::removeGuildMemberAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*GuildMembers::httpClient, workload);
		} catch (...) {
			reportException("GuildMembers::removeGuildMemberAsync()");
		}
	}

	CoRoutine<GuildMember> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildMember>();
			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
			ModifyGuildMemberData dataPackage01{};
			dataPackage01.currentChannelId = guildMember.voiceData.channelId;
			dataPackage01.deaf = guildMember.getDeaf();
			dataPackage01.guildId = guildMember.guildId;
			dataPackage01.guildMemberId = guildMember.user.id;
			dataPackage01.mute = guildMember.getMute();
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
		} catch (...) {
			reportException("GuildMembers::modifyGuildMemberAsync()");
		}
	}

	void GuildMembers::insertGuildMember(GuildMemberData guildMember) {
		try {
			std::lock_guard<std::mutex> theLock{ GuildMembers::accessMutex };
			std::cout << "THE GUYILD NEW NEW NEW : " << guildMember.guildId << " GUILD MEMBER: " << guildMember.user.id << std::endl;
			if (std::string(guildMember.user.id) == "") {
				return;
			}
			GuildMembers::cache.insert_or_assign(guildMember.guildId + " + " + guildMember.user.id, guildMember);
		} catch (...) {
			reportException("GuildMembers::insertGuildMember()");
		}
	}

	void GuildMembers::removeGuildMember(std::string globalId) {
		try {
			std::lock_guard<std::mutex> theLock{ GuildMembers::accessMutex };
			GuildMembers::cache.erase(globalId);
		} catch (...) {
			reportException("Guilds::removeGuild()");
		}
	};
	DiscordCoreInternal::HttpClient* GuildMembers::httpClient{ nullptr };
	std::unordered_map<std::string, GuildMemberData> GuildMembers::cache{};
	std::mutex GuildMembers::accessMutex{};
};
