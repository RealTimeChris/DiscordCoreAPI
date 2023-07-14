/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// GuildScheduledEventEntities.cpp - Source file for the Guild Scheduled Events stuff.
/// Nov 27, 2021
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.cpp

#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::ModifyGuildScheduledEventData> {
		using ValueType = DiscordCoreAPI::ModifyGuildScheduledEventData;
		static constexpr auto parseValue = object("privacy_level", &ValueType::privacyLevel, "status", &ValueType::status, "entity_metadata",
			&ValueType::entityMetadata, "entity_type", &ValueType::entityType, "guild_scheduled_event_id", &ValueType::guildScheduledEventId,
			"scheduled_start_time", &ValueType::scheduledStartTime, "scheduled_end_time", &ValueType::scheduledEndTime, "description",
			&ValueType::description, "channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildScheduledEventData> {
		using ValueType = DiscordCoreAPI::CreateGuildScheduledEventData;
		static constexpr auto parseValue =
			object("privacy_level", &ValueType::privacyLevel, "entity_metadata", &ValueType::entityMetadata, "entity_type", &ValueType::entityType,
				"scheduled_start_time", &ValueType::scheduledStartTime, "scheduled_end_time", &ValueType::scheduledEndTime, "description",
				&ValueType::description, "channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId, "name", &ValueType::name);
	};

}

namespace DiscordCoreAPI {

	void GuildScheduledEvents::initialize(DiscordCoreInternal::HttpsClient* client) {
		GuildScheduledEvents::httpsClient = client;
	}

	CoRoutine<std::vector<GuildScheduledEventData>> GuildScheduledEvents::getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Events };
		co_await NewThreadAwaitable<std::vector<GuildScheduledEventData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync()";
		std::vector<GuildScheduledEventData> returnData{};
		GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildScheduledEventData> GuildScheduledEvents::createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEventData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildScheduledEvents::createGuildScheduledEventAsync()";
		GuildScheduledEventData returnData{};
		GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildScheduledEventData> GuildScheduledEvents::getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEventData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		workload.relativePath += "?with_user_count=";
		std::stringstream stream{};
		stream << std::boolalpha << dataPackage.withUserCount;
		workload.relativePath += stream.str();
		workload.callStack = "GuildScheduledEvents::getGuildScheduledEventAsync()";
		GuildScheduledEventData returnData{};
		GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildScheduledEventData> GuildScheduledEvents::modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<GuildScheduledEventData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildScheduledEvents::modifyGuildScheduledEventAsync()";
		GuildScheduledEventData returnData{};
		GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> GuildScheduledEvents::deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Scheduled_Event };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		workload.callStack = "GuildScheduledEvents::deleteGuildScheduledEventAsync()";
		GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
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
		std::vector<GuildScheduledEventUserData> returnData{};
		GuildScheduledEvents::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	DiscordCoreInternal::HttpsClient* GuildScheduledEvents::httpsClient{};

}
