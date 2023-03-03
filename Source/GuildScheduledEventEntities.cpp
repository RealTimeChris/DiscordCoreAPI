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
/// GuildScheduledEventEntities.cpp - Source file for the Guild Scheduled Events stuff.
/// Nov 27, 2021
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.cpp

#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	CreateGuildScheduledEventData::operator Jsonifier() {
		Jsonifier data{};
		if (this->entityType == GuildScheduledEventEntityType::External) {
			data["channel_id"] = JsonType::Null;
		} else {
			data["channel_id"] = this->channelId;
		}
		data["entity_metadata"]["location"] = this->entityMetadata.location;
		data["entity_metadata"]["entity_type"] = this->entityType;
		data["scheduled_start_time"] = this->scheduledStartTime;
		data["scheduled_end_time"] = this->scheduledEndTime;
		data["privacy_level"] = this->privacyLevel;
		data["description"] = this->description;
		data["name"] = this->name;
		return data;
	}

	ModifyGuildScheduledEventData::operator Jsonifier() {
		Jsonifier data{};
		if (this->entityType == GuildScheduledEventEntityType::External) {
			data["channel_id"] = JsonType::Null;
		} else {
			data["channel_id"] = this->channelId;
		}
		data["entity_metadata"]["location"] = this->entityMetadata.location;
		data["entity_metadata"]["entity_type"] = this->entityType;
		data["scheduled_start_time"] = this->scheduledStartTime;
		data["scheduled_end_time"] = this->scheduledEndTime;
		data["privacy_level"] = this->privacyLevel;
		data["description"] = this->description;
		data["name"] = this->name;
		return data;
	}

	GuildScheduledEvent::GuildScheduledEvent(simdjson::ondemand::value jsonObjectData) {
		this->privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		this->entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		this->status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		simdjson::ondemand::value object{};
		if (getObject(object, "entity_metadata", jsonObjectData)) {
			this->entityMetadata = GuildScheduledEventMetadata{ object };
		}

		this->scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		this->scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		this->userCount = getUint32(jsonObjectData, "user_count");

		this->description = getString(jsonObjectData, "description");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->creatorId = getId(jsonObjectData, "creator_id");

		this->entityId = getString(jsonObjectData, "entity_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData["creator"].get(object) == simdjson::error_code::SUCCESS) {
			this->creator = UserData{ object };
		}

		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");
	}

	GuildScheduledEventVector::GuildScheduledEventVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildScheduledEvent newData{ value.value() };
					this->guildScheduledEvents.emplace_back(std::move(newData));
				}
			}
		}
	}


	GuildScheduledEventVector::operator std::vector<GuildScheduledEvent>() {
		return this->guildScheduledEvents;
	}

	void GuildScheduledEvents::initialize(DiscordCoreInternal::HttpsClient* client) {
		GuildScheduledEvents::httpsClient = client;
	}

	CoRoutine<std::vector<GuildScheduledEvent>> GuildScheduledEvents::getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Events };
		co_await NewThreadAwaitable<std::vector<GuildScheduledEvent>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync()";
		GuildScheduledEventVector returnValue{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEventVector>(workload, &returnValue);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "GuildScheduledEvents::createGuildScheduledEventAsync()";
		GuildScheduledEvent returnValue{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload, &returnValue);
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
		GuildScheduledEvent returnValue{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload, &returnValue);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "GuildScheduledEvents::modifyGuildScheduledEventAsync()";
		GuildScheduledEvent returnValue{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload, &returnValue);
	}

	CoRoutine<void> GuildScheduledEvents::deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		workload.callStack = "GuildScheduledEvents::deleteGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<void>(workload);
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
		GuildScheduledEventUserDataVector returnValue{};
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEventUserDataVector>(workload, &returnValue);
	}
	DiscordCoreInternal::HttpsClient* GuildScheduledEvents::httpsClient{ nullptr };
}
