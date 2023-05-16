/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// GuildScheduledEventEntities.cpp - Source file for the Guild Scheduled Events stuff.
/// Nov 27, 2021
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.cpp

#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	void GuildScheduledEvents::initialize(DiscordCoreInternal::HttpsClient* client) {
		GuildScheduledEvents::httpsClient = client;
	}
	/*
	CoRoutine<std::vector<GuildScheduledEvent>> GuildScheduledEvents::getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Events };
		co_await NewThreadAwaitable<std::vector<GuildScheduledEvent>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync()";
		GuildScheduledEventVector returnData{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEventVector>(workload, returnData);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildScheduledEvents::createGuildScheduledEventAsync()";
		GuildScheduledEvent returnData{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload, returnData);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		workload.relativePath += "?with_user_count=";
		std::stringstream stream{};
		stream << std::boolalpha << dataPackage.withUserCount;
		workload.relativePath += stream.str();
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync()";
		GuildScheduledEvent returnData{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload, returnData);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildScheduledEvents::modifyGuildScheduledEventAsync()";
		GuildScheduledEvent returnData{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload, returnData);
	}

	CoRoutine<void> GuildScheduledEvents::deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		workload.callStack = "GuildScheduledEvents::deleteGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult(workload);
	}

	CoRoutine<std::vector<GuildScheduledEventUserData>> GuildScheduledEvents::getGuildScheduledEventUsersAsync(
		GetGuildScheduledEventUsersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Event_Users };
		co_await NewThreadAwaitable<std::vector<GuildScheduledEventUserData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId + "/users";
		if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			if (dataPackage.after != 0) {
				workload.relativePath += "&after=" + dataPackage.after;
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
			if (dataPackage.withMember) {
				workload.relativePath += "&with_member=true";
			}
		}
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
			if (dataPackage.withMember) {
				workload.relativePath += "&with_member=true";
			}
		}
		if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.withMember) {
				workload.relativePath += "&with_member=true";
			}
		}
		if (dataPackage.withMember) {
			workload.relativePath += "?with_member=true";
		}
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventUsersAsync()";
		GuildScheduledEventUserDataVector returnData{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEventUserDataVector>(workload, returnData);
	}
	*/
	DiscordCoreInternal::HttpsClient* GuildScheduledEvents::httpsClient{ nullptr };

}
