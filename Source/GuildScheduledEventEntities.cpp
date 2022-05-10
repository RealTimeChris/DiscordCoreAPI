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
/// GuildScheduledEventEntities.cpp - Source file for the Guild Scheduled Events stuff.
/// Nov 27, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.cpp

#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	void GuildScheduledEvents::initialize(DiscordCoreInternal::HttpClient* theClient) {
		GuildScheduledEvents::httpClient = theClient;
	}

	CoRoutine<std::vector<GuildScheduledEvent>> GuildScheduledEvents::getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Events);
			co_await NewThreadAwaitable<std::vector<GuildScheduledEvent>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Events;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
			workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildScheduledEvent>>(*GuildScheduledEvents::httpClient, workload);
		} catch (...) {
			reportException("GuildScheduledEvents::getGuildScheduledEventAsync()");
		}
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Scheduled_Event);
			co_await NewThreadAwaitable<GuildScheduledEvent>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "GuildScheduledEvents::createGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildScheduledEvent>(*GuildScheduledEvents::httpClient, workload);
		} catch (...) {
			reportException("GuildScheduledEvents::createGuildScheduledEventAsync()");
		}
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Event);
			co_await NewThreadAwaitable<GuildScheduledEvent>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
			workload.relativePath += "?with_user_count=";
			std::stringstream stream{};
			stream << std::boolalpha << dataPackage.withUserCount;
			workload.relativePath += stream.str();
			workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildScheduledEvent>(*GuildScheduledEvents::httpClient, workload);
		} catch (...) {
			reportException("GuildScheduledEvents::getGuildScheduledEventAsync()");
		}
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Scheduled_Event);
			co_await NewThreadAwaitable<GuildScheduledEvent>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "GuildScheduledEvents::modifyGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildScheduledEvent>(*GuildScheduledEvents::httpClient, workload);
		} catch (...) {
			reportException("GuildScheduledEvents::modifyGuildScheduledEventAsync()");
		}
	}

	CoRoutine<void> GuildScheduledEvents::deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Scheduled_Event);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
			workload.callStack = "GuildScheduledEvents::deleteGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*GuildScheduledEvents::httpClient, workload);
		} catch (...) {
			reportException("GuildScheduledEvents::deleteGuildScheduledEventAsync()");
		}
	}

	CoRoutine<std::vector<GuildScheduledEventUserData>> GuildScheduledEvents::getGuildScheduledEventUsersAsync(GetGuildScheduledEventUsersData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Event_Users);
			co_await NewThreadAwaitable<std::vector<GuildScheduledEventUserData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Event_Users;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId + "/users";
			if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
				if (dataPackage.after != "") {
					workload.relativePath += "&after=" + dataPackage.after;
				}
				if (dataPackage.before != "") {
					workload.relativePath += "&before=" + dataPackage.before;
				}
				if (dataPackage.withMember) {
					workload.relativePath += "&with_member=true";
				}
			}
			if (dataPackage.after != "") {
				workload.relativePath += "?after=" + dataPackage.after;
				if (dataPackage.before != "") {
					workload.relativePath += "&before=" + dataPackage.before;
				}
				if (dataPackage.withMember) {
					workload.relativePath += "&with_member=true";
				}
			}
			if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
				if (dataPackage.withMember) {
					workload.relativePath += "&with_member=true";
				}
			}
			if (dataPackage.withMember) {
				workload.relativePath += "?with_member=true";
			}
			workload.callStack = "GuildScheduledEvents::getGuildScheduledEventUsersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildScheduledEventUserData>>(*GuildScheduledEvents::httpClient, workload);
		} catch (...) {
			reportException("GuildScheduledEvents::getGuildScheduledEventUsersAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* GuildScheduledEvents::httpClient{ nullptr };
}
