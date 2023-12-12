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
/// GuildScheduledEventEntities.cpp - Source file for the guild scheduled events stuff.
/// Nov 27, 2021
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.cpp

#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::modify_guild_scheduled_event_data> {
		using value_type				 = discord_core_api::modify_guild_scheduled_event_data;
		static constexpr auto parseValue = createValue("privacy_level", &value_type::privacyLevel, "status", &value_type::status, "entity_metadata", &value_type::entityMetadata,
			"entity_type", &value_type::entityType, "guild_scheduled_event_id", &value_type::guildScheduledEventId, "scheduled_start_time", &value_type::scheduledStartTime,
			"scheduled_end_time", &value_type::scheduledEndTime, "description", &value_type::description, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId,
			"name", &value_type::name);
	};

	template<> struct core<discord_core_api::create_guild_scheduled_event_data> {
		using value_type				 = discord_core_api::create_guild_scheduled_event_data;
		static constexpr auto parseValue = createValue("privacy_level", &value_type::privacyLevel, "entity_metadata", &value_type::entityMetadata, "entity_type",
			&value_type::entityType, "scheduled_start_time", &value_type::scheduledStartTime, "scheduled_end_time", &value_type::scheduledEndTime, "description",
			&value_type::description, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "name", &value_type::name);
	};

}

namespace discord_core_api {

	void guild_scheduled_events::initialize(discord_core_internal::https_client* client) {
		guild_scheduled_events::httpsClient = client;
	}

	co_routine<jsonifier::vector<guild_scheduled_event_data>> guild_scheduled_events::getGuildScheduledEventsAsync(get_guild_scheduled_events_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Scheduled_Events };
		co_await newThreadAwaitable<jsonifier::vector<guild_scheduled_event_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		workload.callStack	   = "guild_scheduled_events::getGuildScheduledEventAsync()";
		jsonifier::vector<guild_scheduled_event_data> returnData{};
		guild_scheduled_events::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_scheduled_event_data> guild_scheduled_events::createGuildScheduledEventAsync(create_guild_scheduled_event_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Scheduled_Event };
		co_await newThreadAwaitable<guild_scheduled_event_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/scheduled-events";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guild_scheduled_events::createGuildScheduledEventAsync()";
		guild_scheduled_event_data returnData{};
		guild_scheduled_events::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_scheduled_event_data> guild_scheduled_events::getGuildScheduledEventAsync(get_guild_scheduled_event_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Scheduled_Event };
		co_await newThreadAwaitable<guild_scheduled_event_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		workload.relativePath += "?with_user_count=";
		std::stringstream stream{};
		stream << std::boolalpha << dataPackage.withUserCount;
		workload.relativePath += stream.str();
		workload.callStack = "guild_scheduled_events::getGuildScheduledEventAsync()";
		guild_scheduled_event_data returnData{};
		guild_scheduled_events::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_scheduled_event_data> guild_scheduled_events::modifyGuildScheduledEventAsync(modify_guild_scheduled_event_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Scheduled_Event };
		co_await newThreadAwaitable<guild_scheduled_event_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guild_scheduled_events::modifyGuildScheduledEventAsync()";
		guild_scheduled_event_data returnData{};
		guild_scheduled_events::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> guild_scheduled_events::deleteGuildScheduledEventAsync(delete_guild_scheduled_event_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Scheduled_Event };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId;
		workload.callStack	   = "guild_scheduled_events::deleteGuildScheduledEventAsync()";
		guild_scheduled_events::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<guild_scheduled_event_user_data>> guild_scheduled_events::getGuildScheduledEventUsersAsync(get_guild_scheduled_event_users_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Scheduled_Event_Users };
		co_await newThreadAwaitable<jsonifier::vector<guild_scheduled_event_user_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/scheduled-events/" + dataPackage.guildScheduledEventId + "/users";
		if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
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
		workload.callStack = "guild_scheduled_events::getGuildScheduledEventUsersAsync()";
		jsonifier::vector<guild_scheduled_event_user_data> returnData{};
		guild_scheduled_events::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	discord_core_internal::https_client* guild_scheduled_events::httpsClient{};

}
