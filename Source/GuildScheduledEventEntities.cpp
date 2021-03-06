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
/// GuildScheduledEventEntities.cpp - Source file for the Guild Scheduled Events stuff.
/// Nov 27, 2021
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.cpp

#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	CreateGuildScheduledEventData::operator std::string() {
		nlohmann::json data{};
		if (this->entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data["channel_id"] = nullptr;
		} else {
			data["channel_id"] = std::to_string(this->channelId);
		}
		data["entity_metadata"]["location"] = this->entityMetadata.location;
		data["entity_metadata"]["entity_type"] = this->entityType;
		data["scheduled_start_time"] = this->scheduledStartTime;
		data["scheduled_end_time"] = this->scheduledEndTime;
		data["privacy_level"] = this->privacyLevel;
		data["description"] = this->description;
		data["name"] = this->name;
		return data.dump();
	}

	ModifyGuildScheduledEventData::operator std::string() {
		nlohmann::json data{};
		if (this->entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data["channel_id"] = nullptr;
		} else {
			data["channel_id"] = std::to_string(this->channelId);
		}
		data["entity_metadata"]["location"] = this->entityMetadata.location;
		data["entity_metadata"]["entity_type"] = this->entityType;
		data["scheduled_start_time"] = this->scheduledStartTime;
		data["scheduled_end_time"] = this->scheduledEndTime;
		data["privacy_level"] = this->privacyLevel;
		data["description"] = this->description;
		data["name"] = this->name;
		return data.dump();
	}

	GuildScheduledEvent& GuildScheduledEvent::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildScheduledEvent::GuildScheduledEvent(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	void GuildScheduledEvents::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		GuildScheduledEvents::httpsClient = theClient;
	}

	CoRoutine<std::vector<GuildScheduledEvent>> GuildScheduledEvents::getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Events };
		co_await NewThreadAwaitable<std::vector<GuildScheduledEvent>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events";
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<std::vector<GuildScheduledEvent>>(workload);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events";
		workload.content = dataPackage;
		workload.callStack = "GuildScheduledEvents::createGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events/" + std::to_string(dataPackage.guildScheduledEventId);
		workload.relativePath += "?with_user_count=";
		std::stringstream stream{};
		stream << std::boolalpha << dataPackage.withUserCount;
		workload.relativePath += stream.str();
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events/" + std::to_string(dataPackage.guildScheduledEventId);
		workload.content = dataPackage;
		workload.callStack = "GuildScheduledEvents::modifyGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload);
	}

	CoRoutine<void> GuildScheduledEvents::deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events/" + std::to_string(dataPackage.guildScheduledEventId);
		workload.callStack = "GuildScheduledEvents::deleteGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<GuildScheduledEventUserData>> GuildScheduledEvents::getGuildScheduledEventUsersAsync(GetGuildScheduledEventUsersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Event_Users };
		co_await NewThreadAwaitable<std::vector<GuildScheduledEventUserData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events/" + std::to_string(dataPackage.guildScheduledEventId) + "/users";
		if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			if (dataPackage.after != 0) {
				workload.relativePath += "&after=" + std::to_string(dataPackage.after);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
			if (dataPackage.withMember) {
				workload.relativePath += "&with_member=true";
			}
		}
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
			if (dataPackage.withMember) {
				workload.relativePath += "&with_member=true";
			}
		}
		if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + std::to_string(dataPackage.before);
			if (dataPackage.withMember) {
				workload.relativePath += "&with_member=true";
			}
		}
		if (dataPackage.withMember) {
			workload.relativePath += "?with_member=true";
		}
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventUsersAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEventUserDataVector>(workload);
	}
	DiscordCoreInternal::HttpsClient* GuildScheduledEvents::httpsClient{ nullptr };
}
