// GuildScheduledEventEntities.cpp - Source file for the Guild Scheduled Events stuff.
// Nov 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "GuildScheduledEventEntities.hpp"
#include "Http.hpp"
#include "JSONIfier.hpp"

namespace DiscordCoreAPI {

	GuildScheduledEvent::GuildScheduledEvent() {}

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

	void GuildScheduledEvents::initialize(DiscordCoreInternal::HttpClientManager*theClient) {
		GuildScheduledEvents::httpClientManager = theClient;
	}

	CoRoutine<std::vector<GuildScheduledEvent>> GuildScheduledEvents::getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<GuildScheduledEvent>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Scheduled_Events;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
			workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync";
			DiscordCoreInternal::HttpClient<std::vector<GuildScheduledEvent>> httpClient{};
			co_return httpClient.submitWorkloadAndGetResult<std::vector<GuildScheduledEvent>>(workload, *GuildScheduledEvents::httpClientManager);
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
			DiscordCoreInternal::HttpClient<GuildScheduledEvent> httpClient{};
			co_return httpClient.submitWorkloadAndGetResult<GuildScheduledEvent>(workload, *GuildScheduledEvents::httpClientManager);
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
			DiscordCoreInternal::HttpClient<GuildScheduledEvent> httpClient{};
			co_return httpClient.submitWorkloadAndGetResult<GuildScheduledEvent>(workload, *GuildScheduledEvents::httpClientManager);
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
			DiscordCoreInternal::HttpClient<GuildScheduledEvent> httpClient{};
			co_return httpClient.submitWorkloadAndGetResult<GuildScheduledEvent>(workload, *GuildScheduledEvents::httpClientManager);
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
			DiscordCoreInternal::HttpClient<void> httpClient{};
			co_return httpClient.submitWorkloadAndGetResult<void>(workload, *GuildScheduledEvents::httpClientManager);
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
			DiscordCoreInternal::HttpClient<std::vector<GuildScheduledEventUserData>> httpClient{};
			co_return httpClient.submitWorkloadAndGetResult<std::vector<GuildScheduledEventUserData>>(workload, *GuildScheduledEvents::httpClientManager);
		}
		catch (...) {
			reportException("GuildScheduledEvents::getGuildScheduledEventUsersAsync()");
		}
	}
	DiscordCoreInternal::HttpClientManager* GuildScheduledEvents::httpClientManager{ nullptr };
}
