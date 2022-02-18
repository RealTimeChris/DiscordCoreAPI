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
/// GuildScheduledEventEntities.cpp - Source file for the Guild Scheduled Events stuff.
/// Nov 27, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <GuildScheduledEventEntities.hpp>
#include <Http.hpp>
#include <JSONIfier.hpp>

namespace DiscordCoreAPI {

	GuildScheduledEvent::GuildScheduledEvent(GuildScheduledEventData dataNew) {
		this->scheduledStartTime = dataNew.scheduledStartTime;
		this->scheduledEndTime = dataNew.scheduledEndTime;
		this->entityMetadata = dataNew.entityMetadata;
		this->privacyLevel = dataNew.privacyLevel;
		this->description = dataNew.description;
		this->entityType = dataNew.entityType;
		this->userCount = dataNew.userCount;
		this->creatorId = dataNew.creatorId;
		this->channelId = dataNew.channelId;
		this->entityId = dataNew.entityId;
		this->creator = dataNew.creator;
		this->guildId = dataNew.guildId;
		this->status = dataNew.status;
		this->name = dataNew.name;
		this->id = dataNew.id;
	}

	void GuildScheduledEvents::initialize(DiscordCoreInternal::HttpClient*theClient) {
		GuildScheduledEvents::httpClient = theClient;
	}

	CoRoutine<std::vector<GuildScheduledEvent>> GuildScheduledEvents::getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<GuildScheduledEvent>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Events;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
			workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildScheduledEvent>>(*GuildScheduledEvents::httpClient, workload);
		}
		catch (...) {
			reportException("GuildScheduledEvents::getGuildScheduledEventAsync()");
		}
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildScheduledEvent>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "GuildScheduledEvents::createGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildScheduledEvent>(*GuildScheduledEvents::httpClient, workload);
		}
		catch (...) {
			reportException("GuildScheduledEvents::createGuildScheduledEventAsync()");
		}
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildScheduledEvent>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
			workload.relativePath += "?with_user_count=";
			std::stringstream stream{};
			stream << std::boolalpha << dataPackage.withUserCount;
			workload.relativePath += stream.str();
			workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildScheduledEvent>(*GuildScheduledEvents::httpClient, workload);
		}
		catch (...) {
			reportException("GuildScheduledEvents::getGuildScheduledEventAsync()");
		}
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildScheduledEvent>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "GuildScheduledEvents::modifyGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildScheduledEvent>(*GuildScheduledEvents::httpClient, workload);
		}
		catch (...) {
			reportException("GuildScheduledEvents::modifyGuildScheduledEventAsync()");
		}
	}

	CoRoutine<void> GuildScheduledEvents::deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Scheduled_Event;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
			workload.callStack = "GuildScheduledEvents::deleteGuildScheduledEventAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*GuildScheduledEvents::httpClient, workload);
		}
		catch (...) {
			reportException("GuildScheduledEvents::deleteGuildScheduledEventAsync()");
		}
	}

	CoRoutine<std::vector<GuildScheduledEventUserData>> GuildScheduledEvents::getGuildScheduledEventUsersAsync(GetGuildScheduledEventUsersData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<GuildScheduledEventUserData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
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
		}
		catch (...) {
			reportException("GuildScheduledEvents::getGuildScheduledEventUsersAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* GuildScheduledEvents::httpClient{ nullptr };
}
