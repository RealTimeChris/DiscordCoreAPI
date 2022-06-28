/*
*
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
/// DataParsingFunctions.cpp - Source file for the data-parsing functions.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file DataParsingFunctions.cpp

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>

namespace DiscordCoreAPI {

	void ApplicationCommand::parseObject(const nlohmann::json& jsonObjectData, ApplicationCommand* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("default_member_permissions") && !jsonObjectData["default_member_permissions"].is_null()) {
			pDataStructure->defaultMemberPermissions = jsonObjectData["default_member_permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("dm_permission") && !jsonObjectData["dm_permission"].is_null()) {
			pDataStructure->dmPermission = jsonObjectData["dm_permission"].get<bool>();
		}

		if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
			pDataStructure->version = jsonObjectData["version"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
			pDataStructure->nameLocalizations.clear();
			auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				pDataStructure->nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData.contains("description_localizations") && !jsonObjectData["description_localizations"].is_null()) {
			pDataStructure->nameLocalizations.clear();
			auto newMap = jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				pDataStructure->descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
			pDataStructure->version = jsonObjectData["version"].get<std::string>();
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			pDataStructure->options.reserve(jsonObjectData["options"].size());
			for (auto& value: jsonObjectData["options"]) {
				ApplicationCommandOptionData newData{ value };
				pDataStructure->options.push_back(newData);
			}
			pDataStructure->options.shrink_to_fit();
		}
	}

	void AutoModerationRule::parseObject(const nlohmann::json& jsonObjectData, AutoModerationRule* pDataStructure) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
			pDataStructure->enabled = jsonObjectData["enabled"].get<bool>();
		}

		if (jsonObjectData.contains("trigger_type") && !jsonObjectData["trigger_type"].is_null()) {
			pDataStructure->triggerType = static_cast<TriggerType>(jsonObjectData["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData.contains("event_type") && !jsonObjectData["event_type"].is_null()) {
			pDataStructure->eventType = static_cast<EventType>(jsonObjectData["event_type"].get<uint64_t>());
		}

		if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
			pDataStructure->creatorId = jsonObjectData["creator_id"].get<uint64_t>();
		}

		if (jsonObjectData.contains("actions") && !jsonObjectData["actions"].is_null()) {
			for (auto& value: jsonObjectData["actions"]) {
				ActionData newData{ value };
				pDataStructure->actions.push_back(newData);
			}
		}

		if (jsonObjectData.contains("exempt_roles") && !jsonObjectData["exempt_roles"].is_null()) {
			for (auto& value: jsonObjectData["exempt_roles"]) {
				pDataStructure->exemptRoles.push_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData.contains("trigger_metadata") && !jsonObjectData["trigger_metadata"].is_null()) {
			pDataStructure->triggerMetaData = jsonObjectData["trigger_metadata"];
		}

		if (jsonObjectData.contains("exempt_channels") && !jsonObjectData["exempt_channels"].is_null()) {
			for (auto& value: jsonObjectData["exempt_channels"]) {
				pDataStructure->exemptChannels.push_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = jsonObjectData["guild_id"].get<uint64_t>();
		}
	}

	void AutoModerationRuleVector::parseObject(const nlohmann::json& jsonObjectData, AutoModerationRuleVector* pDataStructure) {
		pDataStructure->theAutoModerationRules.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::AutoModerationRule newData{ value };
			pDataStructure->theAutoModerationRules.push_back(newData);
		}
		pDataStructure->theAutoModerationRules.shrink_to_fit();
	}

	void Channel::parseObject(const nlohmann::json& jsonObjectData, Channel* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			if (jsonObjectData["id"].is_string()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			} else {
				pDataStructure->id = jsonObjectData["id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			pDataStructure->flags = jsonObjectData["flags"].get<int8_t>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::ChannelType>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("default_auto_archive_duration") && !jsonObjectData["default_auto_archive_duration"].is_null()) {
			pDataStructure->defaultAutoArchiveDuration = jsonObjectData["default_auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
			pDataStructure->position = jsonObjectData["position"].get<int32_t>();
		}

		if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
			pDataStructure->permissionOverwrites.clear();
			for (auto& value: jsonObjectData["permission_overwrites"]) {
				DiscordCoreAPI::OverWriteData newData{ value };
				uint64_t overWriteId = newData.id;
				pDataStructure->permissionOverwrites[overWriteId] = newData;
			}
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
			pDataStructure->topic = jsonObjectData["topic"].get<std::string>();
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
			pDataStructure->flags =
				DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::ChannelFlags>(pDataStructure->flags, DiscordCoreAPI::ChannelFlags::NSFW, jsonObjectData["nsfw"].get<bool>());
		}

		if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
			pDataStructure->lastMessageId = jsonObjectData["last_message_id"].get<std::string>();
		}

		if (jsonObjectData.contains("bitrate") && !jsonObjectData["bitrate"].is_null()) {
			pDataStructure->bitrate = jsonObjectData["bitrate"].get<int32_t>();
		}

		if (jsonObjectData.contains("user_limit") && !jsonObjectData["user_limit"].is_null()) {
			pDataStructure->userLimit = jsonObjectData["user_limit"].get<int32_t>();
		}

		if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData["rate_limit_per_user"].is_null()) {
			pDataStructure->rateLimitPerUser = jsonObjectData["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData.contains("recipients") && !jsonObjectData["recipients"].is_null()) {
			pDataStructure->recipients.clear();
			for (auto& value: jsonObjectData["recipients"]) {
				DiscordCoreAPI::UserData newData{ value };
				uint64_t userId = newData.id;
				pDataStructure->recipients[userId] = newData;
			}
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			pDataStructure->icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
			pDataStructure->ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
			if (jsonObjectData["parent_id"].is_string()) {
				pDataStructure->parentId = stoull(jsonObjectData["parent_id"].get<std::string>());
			} else {
				pDataStructure->parentId = jsonObjectData["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
			pDataStructure->lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
			pDataStructure->rtcRegion = jsonObjectData["rtc_region"].get<std::string>();
		}

		if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData["video_quality_mode"].is_null()) {
			pDataStructure->videoQualityMode = jsonObjectData["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData.contains("message_count") && !jsonObjectData["message_count"].is_null()) {
			pDataStructure->messageCount = jsonObjectData["message_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
			pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("thread_metadata") && !jsonObjectData["thread_metadata"].is_null()) {
			pDataStructure->threadMetadata = jsonObjectData["thread_metadata"];
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			pDataStructure->member = jsonObjectData["member"];
		}
	}

	void ChannelVector::parseObject(const nlohmann::json& jsonObjectData, ChannelVector* pDataStructure) {
		pDataStructure->theChannels.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::Channel newData{ value };
			pDataStructure->theChannels.push_back(newData);
		}
		pDataStructure->theChannels.shrink_to_fit();
	}
};
