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

	GuildScheduledEvent::GuildScheduledEvent(simdjson::ondemand::value jsonObjectData) {
		this->privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		this->entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		this->status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["entity_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->entityMetadata = GuildScheduledEventMetadata{ theObject };
		}

		this->scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		this->scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		this->userCount = getUint32(jsonObjectData, "user_count");

		this->description = getString(jsonObjectData, "description");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->creatorId = std::to_string(getId(jsonObjectData, "creator_id"));

		this->entityId = getString(jsonObjectData, "entity_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		theResult = jsonObjectData["creator"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->creator = UserData{ theObject };
		}

		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");
	}

	GuildScheduledEventVector::GuildScheduledEventVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildScheduledEvent newData{ value.value() };
					this->theGuildScheduledEvents.emplace_back(std::move(newData));
				}
			}
		}
	}

	CreateGuildScheduledEventData::operator String() {
		JsonObject theData{};
		if (this->entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			theData["channel_id"] = ValueType::Null;
		} else {
			theData["channel_id"] = std::to_string(this->channelId);
		}
		theData["entity_metadata"]["location"] = this->entityMetadata.location;
		theData["entity_metadata"]["entity_type"] = this->entityType;
		theData["scheduled_start_time"] = this->scheduledStartTime;
		theData["scheduled_end_time"] = this->scheduledEndTime;
		theData["privacy_level"] = this->privacyLevel;
		theData["description"] = this->description;
		theData["name"] = this->name;
		return theData;
	}

	ModifyGuildScheduledEventData::operator String() {
		JsonObject theData{};
		if (this->entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			theData["channel_id"] = ValueType::Null;
		} else {
			theData["channel_id"] = std::to_string(this->channelId);
		}
		theData["entity_metadata"]["location"] = this->entityMetadata.location;
		theData["entity_metadata"]["entity_type"] = this->entityType;
		theData["scheduled_start_time"] = this->scheduledStartTime;
		theData["scheduled_end_time"] = this->scheduledEndTime;
		theData["privacy_level"] = this->privacyLevel;
		theData["description"] = this->description;
		theData["name"] = this->name;
		return theData;
	}

	GuildScheduledEventVector::operator std::vector<GuildScheduledEvent>() {
		return this->theGuildScheduledEvents;
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
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEventVector>(workload);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events";
		workload.content = dataPackage.operator String();
		workload.callStack = "GuildScheduledEvents::createGuildScheduledEventAsync()";
		co_return GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult<GuildScheduledEvent>(workload);
	}

	CoRoutine<GuildScheduledEvent> GuildScheduledEvents::getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEvent>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/scheduled-events/" + std::to_string(dataPackage.guildScheduledEventId);
		workload.relativePath += "?with_user_count=";
		StringStream stream{};
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
		workload.content = dataPackage.operator String();
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
