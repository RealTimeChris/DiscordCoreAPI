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
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>

namespace DiscordCoreAPI {

	void ApplicationCommand::parseObject(const nlohmann::json* jsonObjectData) {
		DiscordCoreAPI::parseObject(jsonObjectData, *this);
	}

	void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommand& theData) {
		theData.id = strtoull(getString(&(*jsonObjectData), "id"));

		theData.defaultMemberPermissions = getString(&(*jsonObjectData), "default_member_permissions");

		theData.dmPermission = getBoolReal(&(*jsonObjectData), "dm_permission");

		theData.version = getString(&(*jsonObjectData), "version");

		theData.type = static_cast<ApplicationCommandType>(getUint8(&(*jsonObjectData), "type"));
		
		theData.nameLocalizations = getMap<std::string, std::string>(&(*jsonObjectData), "name_localizations");

		theData.descriptionLocalizations = getMap<std::string, std::string>(&(*jsonObjectData), "description_localizations");

		theData.applicationId = strtoull(getString(&(*jsonObjectData), "application_id"));

		theData.name = getString(&(*jsonObjectData), "name");

		theData.description = getString(&(*jsonObjectData), "description");

		theData.version = getString(&(*jsonObjectData), "version");

		std::vector<nlohmann::json> theVector = getVector<nlohmann::json>(&(*jsonObjectData), "options");
		for (auto& value: theVector) {
			ApplicationCommandOptionData newData{ &value };
			theData.options.emplace_back(newData);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, RoleData& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.name = getString(jsonObjectData, "name");

		std::stringstream theStream{};
		theStream << getString(jsonObjectData, "unicode_emoji");
		for (auto& value: theStream.str()) {
			theData.unicodeEmoji.push_back(value);
		}
		if (theData.unicodeEmoji.size() > 3) {
			theData.unicodeEmoji = static_cast<std::string>(theData.unicodeEmoji).substr(1, theData.unicodeEmoji.size() - 3);
		}

		theData.color = getUint32(jsonObjectData, "color");

		theData.flags = setBool<int8_t, RoleFlags>(theData.flags, RoleFlags::Hoist, getBoolReal(jsonObjectData, "hoist"));

		theData.flags = setBool<int8_t, RoleFlags>(theData.flags, RoleFlags::Managed, getBoolReal(jsonObjectData, "managed"));

		theData.flags = setBool<int8_t, RoleFlags>(theData.flags, RoleFlags::Mentionable, getBoolReal(jsonObjectData, "mentionable"));

		theData.position = getUint32(jsonObjectData, "position");

		theData.permissions = strtoull(getString(jsonObjectData, "permissions"));
	}

	void parseObject(const nlohmann::json* jsonObjectData, UserData& theData) {
		theData.flags = setBool<int32_t, UserFlags>(theData.flags, UserFlags::MFAEnabled, getBoolReal(jsonObjectData, "mfa_enabled"));

		theData.flags = setBool<int32_t, UserFlags>(theData.flags, UserFlags::Verified, getBoolReal(jsonObjectData, "verified"));

		theData.flags = setBool<int32_t, UserFlags>(theData.flags, UserFlags::System, getBoolReal(jsonObjectData, "system"));

		theData.flags = setBool<int32_t, UserFlags>(theData.flags, UserFlags::Bot, getBoolReal(jsonObjectData, "bot"));

		theData.discriminator = getString(jsonObjectData, "discriminator");

		theData.flags = getUint32(jsonObjectData, "public_flags");

		theData.userName = getString(jsonObjectData, "username");

		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.avatar = getString(jsonObjectData, "avatar");
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildMemberData& theData) {
		theData.flags = setBool<int8_t, GuildMemberFlags>(theData.flags, GuildMemberFlags::Pending, getBoolReal(jsonObjectData, "pending"));

		theData.flags = setBool<int8_t, GuildMemberFlags>(theData.flags, GuildMemberFlags::Mute, getBoolReal(jsonObjectData, "mute"));

		theData.flags = setBool<int8_t, GuildMemberFlags>(theData.flags, GuildMemberFlags::Deaf, getBoolReal(jsonObjectData, "deaf"));

		theData.joinedAt = TimeStamp<std::chrono::milliseconds>(getString(jsonObjectData, "joined_at"));

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));
		auto roles = getVector<std::string>(jsonObjectData, "roles");

		for (auto& value: roles) {
			theData.roles.push_back(stoull(value));
		}

		theData.permissions = strtoull(getString(jsonObjectData, "permissions"));

		std::unique_ptr<UserData> theUser = std::make_unique<UserData>();
		auto theUserObject = getObject(&(*jsonObjectData), "user");
		DiscordCoreAPI::parseObject(&theUserObject, *theUser);
		theData.id = theUser->id;
		theData.userAvatar = theUser->avatar;
		auto theUserNew = theUser.get();
		theUserNew->insertUser(std::move(theUser));

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags = getUint8(jsonObjectData, "flags");

		theData.nick = getString(jsonObjectData, "nick");
	}

	void parseObject(const nlohmann::json* jsonObjectData, OverWriteData& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.allow = strtoull(getString(jsonObjectData, "allow"));

		theData.deny = strtoull(getString(jsonObjectData, "deny"));

		theData.type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	void parseObject(const nlohmann::json* jsonObjectData, ChannelData& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.flags = getUint8(jsonObjectData, "flags");

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.parentId = strtoull(getString(jsonObjectData, "parent_id"));

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.position = getUint32(jsonObjectData, "position");

		if (jsonObjectData->contains("permission_overwrites") && !(*jsonObjectData)["permission_overwrites"].is_null()) {
			theData.permissionOverwrites.clear();
			theData.permissionOverwrites.reserve((*jsonObjectData)["permission_overwrites"].size());
			for (auto& value: (*jsonObjectData)["permission_overwrites"]) {
				OverWriteData theDataNew{};
				DiscordCoreAPI::parseObject(&value, theDataNew);
				theData.permissionOverwrites.push_back(std::move(theDataNew));
			}
		}

		theData.name = getString(jsonObjectData, "name");

		theData.flags = setBool<int8_t, ChannelFlags>(theData.flags, ChannelFlags::NSFW, getBoolReal(jsonObjectData, "nsfw"));

		theData.ownerId = strtoull(getString(jsonObjectData, "owner_id"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildData& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.icon = getString(jsonObjectData, "icon");

		theData.name = getString(jsonObjectData, "name");

		theData.joinedAt = TimeStamp<std::chrono::milliseconds>(getString(jsonObjectData, "joined_at"));

		theData.flags = setBool<int8_t, GuildFlags>(theData.flags, GuildFlags::Owner, getBoolReal(jsonObjectData, "owner"));

		theData.ownerId = strtoull(getString(jsonObjectData, "owner_id"));

		theData.flags = setBool<int8_t, GuildFlags>(theData.flags, GuildFlags::WidgetEnabled, getBoolReal(jsonObjectData, "widget_enabled"));

		theData.flags = setBool<int8_t, GuildFlags>(theData.flags, GuildFlags::Large, getBoolReal(jsonObjectData, "large"));

		theData.flags = setBool<int8_t, GuildFlags>(theData.flags, GuildFlags::Unavailable, getBoolReal(jsonObjectData, "unavailable"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theData.roles.clear();
			for (auto& value: (*jsonObjectData)["roles"]) {
				std::unique_ptr<RoleData> newData{ std::make_unique<RoleData>() };
				DiscordCoreAPI::parseObject(&value, *newData);
				theData.roles.push_back(newData->id);
				auto theRole = newData.get();
				Roles::insertRole(std::move(newData));
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				std::unique_ptr<GuildMemberData> newData{ std::make_unique<GuildMemberData>() };
				DiscordCoreAPI::parseObject(&value, *newData);
				newData->guildId = theData.id;
				theData.members.push_back(newData.release());
			}
		}

		if (jsonObjectData->contains("voice_states") && !(*jsonObjectData)["voice_states"].is_null()) {
			for (auto& value: (*jsonObjectData)["voice_states"]) {
				auto userId = strtoull(value["user_id"].get<std::string>());
				for (auto& value02: theData.members) {
					if (value02->id == userId) {
						value02->voiceChannelId = strtoull(value["channel_id"].get<std::string>());
					}
				}
			}
		}

		if (jsonObjectData->contains("channels") && !(*jsonObjectData)["channels"].is_null()) {
			theData.channels.clear();
			for (auto& value: (*jsonObjectData)["channels"]) {
				std::unique_ptr<ChannelData> newData{ std::make_unique<ChannelData>() };
				DiscordCoreAPI::parseObject(&value, *newData);
				newData->guildId = theData.id;
				theData.channels.push_back(newData->id);
				auto theChannel = newData.get();
				theChannel->insertChannel(std::move(newData));
			}
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, AttachmentData& theData) {
		theData.id = strtoull(getString(&(*jsonObjectData), "id"));

		theData.filename = getString(&(*jsonObjectData), "filename");

		if (jsonObjectData->contains("content_type") && !(*jsonObjectData)["content_type"].is_null()) {
			theData.contentType = getString(&(*jsonObjectData),"content_type");
		}

		if (jsonObjectData->contains("ephemeral") && !(*jsonObjectData)["ephemeral"].is_null()) {
			theData.ephemeral = (*jsonObjectData)["ephemeral"].get<bool>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("size") && !(*jsonObjectData)["size"].is_null()) {
			theData.size = (*jsonObjectData)["size"].get<int32_t>();
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theData.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theData.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theData.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandVector& theData) {
		theData.theApplicationCommands.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ApplicationCommand newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theApplicationCommands.push_back(newData);
		}
		theData.theApplicationCommands.shrink_to_fit();
	}

	void AutoModerationActionExecutionEventData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("alert_system_message_id") && !(*jsonObjectData)["alert_system_message_id"].is_null()) {
			this->alertSystemMessageId = stoull((*jsonObjectData)["alert_system_message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("rule_trigger_type") && !(*jsonObjectData)["rule_trigger_type"].is_null()) {
			this->ruleTriggerType = (*jsonObjectData)["rule_trigger_type"].get<TriggerType>();
		}

		if (jsonObjectData->contains("matched_keyword") && !(*jsonObjectData)["matched_keyword"].is_null()) {
			this->matchedKeyword = (*jsonObjectData)["matched_keyword"].get<std::string>();
		}

		if (jsonObjectData->contains("matched_content") && !(*jsonObjectData)["matched_content"].is_null()) {
			this->matchedContent = (*jsonObjectData)["matched_content"].get<std::string>();
		}

		if (jsonObjectData->contains("action") && !(*jsonObjectData)["action"].is_null()) {
			this->action.parseObject(&(*jsonObjectData)["action"]);
		}

		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			this->content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			this->messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("rule_id") && !(*jsonObjectData)["rule_id"].is_null()) {
			this->ruleId = stoull((*jsonObjectData)["rule_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			this->userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, AutoModerationRule& theData) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			theData.enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("trigger_type") && !(*jsonObjectData)["trigger_type"].is_null()) {
			theData.triggerType = static_cast<TriggerType>((*jsonObjectData)["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("event_type") && !(*jsonObjectData)["event_type"].is_null()) {
			theData.eventType = static_cast<EventType>((*jsonObjectData)["event_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theData.creatorId = (*jsonObjectData)["creator_id"].get<uint64_t>();
		}

		if (jsonObjectData->contains("actions") && !(*jsonObjectData)["actions"].is_null()) {
			theData.actions.clear();
			for (auto& value: (*jsonObjectData)["actions"]) {
				ActionData newData{ &value };
				theData.actions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("exempt_roles") && !(*jsonObjectData)["exempt_roles"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_roles"]) {
				theData.exemptRoles.emplace_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData->contains("trigger_metadata") && !(*jsonObjectData)["trigger_metadata"].is_null()) {
			theData.triggerMetaData = &(*jsonObjectData)["trigger_metadata"];
		}

		if (jsonObjectData->contains("exempt_channels") && !(*jsonObjectData)["exempt_channels"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_channels"]) {
				theData.exemptChannels.emplace_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = (*jsonObjectData)["guild_id"].get<uint64_t>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, AutoModerationRuleVector& theData) {
		theData.theAutoModerationRules.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			AutoModerationRule newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theAutoModerationRules.push_back(newData);
		}
		theData.theAutoModerationRules.shrink_to_fit();
	}

	void Channel::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				this->id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				this->id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			this->flags = (*jsonObjectData)["flags"].get<int8_t>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("default_auto_archive_duration") && !(*jsonObjectData)["default_auto_archive_duration"].is_null()) {
			this->defaultAutoArchiveDuration = (*jsonObjectData)["default_auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData->contains("position") && !(*jsonObjectData)["position"].is_null()) {
			this->position = (*jsonObjectData)["position"].get<int32_t>();
		}

		if (jsonObjectData->contains("permission_overwrites") && !(*jsonObjectData)["permission_overwrites"].is_null()) {
			this->permissionOverwrites.clear();
			for (auto& value: (*jsonObjectData)["permission_overwrites"]) {
				OverWriteData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->permissionOverwrites[newData.id] = newData;
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			this->topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			this->permissions = (*jsonObjectData)["permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("nsfw") && !(*jsonObjectData)["nsfw"].is_null()) {
			this->flags = setBool<int8_t, ChannelFlags>(this->flags, ChannelFlags::NSFW, (*jsonObjectData)["nsfw"].get<bool>());
		}

		if (jsonObjectData->contains("last_message_id") && !(*jsonObjectData)["last_message_id"].is_null()) {
			this->lastMessageId = (*jsonObjectData)["last_message_id"].get<std::string>();
		}

		if (jsonObjectData->contains("bitrate") && !(*jsonObjectData)["bitrate"].is_null()) {
			this->bitrate = (*jsonObjectData)["bitrate"].get<int32_t>();
		}

		if (jsonObjectData->contains("user_limit") && !(*jsonObjectData)["user_limit"].is_null()) {
			this->userLimit = (*jsonObjectData)["user_limit"].get<int32_t>();
		}

		if (jsonObjectData->contains("rate_limit_per_user") && !(*jsonObjectData)["rate_limit_per_user"].is_null()) {
			this->rateLimitPerUser = (*jsonObjectData)["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData->contains("recipients") && !(*jsonObjectData)["recipients"].is_null()) {
			this->recipients.clear();
			for (auto& value: (*jsonObjectData)["recipients"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->recipients[newData.id] = std::move(newData);
			}
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			this->icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("owner_id") && !(*jsonObjectData)["owner_id"].is_null()) {
			this->ownerId = stoull((*jsonObjectData)["owner_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("parent_id") && !(*jsonObjectData)["parent_id"].is_null()) {
			if ((*jsonObjectData)["parent_id"].is_string()) {
				this->parentId = stoull((*jsonObjectData)["parent_id"].get<std::string>());
			} else {
				this->parentId = (*jsonObjectData)["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			this->lastPinTimestamp = (*jsonObjectData)["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("rtc_region") && !(*jsonObjectData)["rtc_region"].is_null()) {
			this->rtcRegion = (*jsonObjectData)["rtc_region"].get<std::string>();
		}

		if (jsonObjectData->contains("video_quality_mode") && !(*jsonObjectData)["video_quality_mode"].is_null()) {
			this->videoQualityMode = (*jsonObjectData)["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData->contains("message_count") && !(*jsonObjectData)["message_count"].is_null()) {
			this->messageCount = (*jsonObjectData)["message_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			this->memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("thread_metadata") && !(*jsonObjectData)["thread_metadata"].is_null()) {
			this->threadMetadata = &(*jsonObjectData)["thread_metadata"];
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, ChannelVector& theData) {
		theData.theChannels.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ChannelData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theChannels.push_back(newData);
		}
		theData.theChannels.shrink_to_fit();
	}

	void Guild::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("afk_channel_id") && !(*jsonObjectData)["afk_channel_id"].is_null()) {
			this->afkChannelId = stoull((*jsonObjectData)["afk_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			this->icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("icon_hash") && !(*jsonObjectData)["icon_hash"].is_null()) {
			this->icon = (*jsonObjectData)["icon_hash"].get<std::string>();
		}

		if (jsonObjectData->contains("splash") && !(*jsonObjectData)["splash"].is_null()) {
			this->splash = (*jsonObjectData)["splash"].get<std::string>();
		}

		if (jsonObjectData->contains("discovery_splash") && !(*jsonObjectData)["discovery_splash"].is_null()) {
			this->discoverySplash = (*jsonObjectData)["discovery_splash"].get<std::string>();
		}

		if (jsonObjectData->contains("owner") && !(*jsonObjectData)["owner"].is_null()) {
			this->flags = setBool<int8_t, GuildFlags>(this->flags, GuildFlags::Owner, (*jsonObjectData)["owner"].get<bool>());
		}

		if (jsonObjectData->contains("owner_id") && !(*jsonObjectData)["owner_id"].is_null()) {
			this->ownerId = stoull((*jsonObjectData)["owner_id"].get<std::string>());
		}

		if (jsonObjectData->contains("preferred_locale") && !(*jsonObjectData)["preferred_locale"].is_null()) {
			this->preferredLocale = (*jsonObjectData)["preferred_locale"].get<std::string>();
		}

		if (jsonObjectData->contains("public_updates_channel_id") && !(*jsonObjectData)["public_updates_channel_id"].is_null()) {
			this->publicUpdatesChannelId = stoull((*jsonObjectData)["public_updates_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("vanity_url_code") && !(*jsonObjectData)["vanity_url_code"].is_null()) {
			this->vanityUrlCode = (*jsonObjectData)["vanity_url_code"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("banner") && !(*jsonObjectData)["banner"].is_null()) {
			this->banner = (*jsonObjectData)["banner"].get<std::string>();
		}

		if (jsonObjectData->contains("rule_Channel_id") && !(*jsonObjectData)["rule_Channel_id"].is_null()) {
			this->rulesChannelId = stoull((*jsonObjectData)["rule_Channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("joined_at") && !(*jsonObjectData)["joined_at"].is_null()) {
			this->joinedAt = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["joined_at"].get<std::string>());
		}

		if (jsonObjectData->contains("widget_channel_id") && !(*jsonObjectData)["widget_channel_id"].is_null()) {
			this->widgetChannelId = stoull((*jsonObjectData)["widget_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("system_channel_id") && !(*jsonObjectData)["system_channel_id"].is_null()) {
			this->systemChannelId = stoull((*jsonObjectData)["system_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("region") && !(*jsonObjectData)["region"].is_null()) {
			this->region = (*jsonObjectData)["region"].get<std::string>();
		}

		if (jsonObjectData->contains("afk_channel_id") && !(*jsonObjectData)["afk_channel_id"].is_null()) {
			this->afkChannelId = stoull((*jsonObjectData)["afk_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("region") && !(*jsonObjectData)["region"].is_null()) {
			this->region = (*jsonObjectData)["region"].get<std::string>();
		}

		if (jsonObjectData->contains("features") && !(*jsonObjectData)["features"].is_null()) {
			for (auto& value: (*jsonObjectData)["features"].get<std::vector<std::string>>()) {
				this->features.emplace_back(StringWrapper{ value });
			}
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			this->permissions = (*jsonObjectData)["permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			this->roles.clear();
			this->roles.reserve((*jsonObjectData)["roles"].size());
			for (auto& value: (*jsonObjectData)["roles"]) {
				std::unique_ptr<RoleData> newData{};
				DiscordCoreAPI::parseObject(&value, *newData);
				this->roles.push_back(newData->id);
				Roles::insertRole(std::move(newData));
			}
		}

		if (jsonObjectData->contains("afk_timeout") && !(*jsonObjectData)["afk_timeout"].is_null()) {
			this->afkTimeOut = (*jsonObjectData)["afk_timeout"].get<AfkTimeOutDurations>();
		}

		if (jsonObjectData->contains("owner") && !(*jsonObjectData)["owner"].is_null()) {
			this->flags = setBool<int8_t, GuildFlags>(this->flags, GuildFlags::Owner, (*jsonObjectData)["owner"].get<bool>());
		}

		if (jsonObjectData->contains("widget_enabled") && !(*jsonObjectData)["widget_enabled"].is_null()) {
			this->flags = setBool<int8_t, GuildFlags>(this->flags, GuildFlags::WidgetEnabled,
				(*jsonObjectData)["widget_enabled"].get<bool>());
		}

		if (jsonObjectData->contains("verification_level") && !(*jsonObjectData)["verification_level"].is_null()) {
			this->verificationLevel = (*jsonObjectData)["verification_level"].get<VerificationLevel>();
		}

		if (jsonObjectData->contains("default_message_notification_level") && !(*jsonObjectData)["default_message_notification_level"].is_null()) {
			this->defaultMessageNotifications = (*jsonObjectData)["default_message_notification_level"].get<DefaultMessageNotificationLevel>();
		}

		if (jsonObjectData->contains("explicit_content_filter_level") && !(*jsonObjectData)["explicit_content_filter_level"].is_null()) {
			this->explicitContentFilter = (*jsonObjectData)["explicit_content_filter_level"].get<ExplicitContentFilterLevel>();
		}

		if (jsonObjectData->contains("mfa_level") && !(*jsonObjectData)["mfa_level"].is_null()) {
			this->mfaLevel = (*jsonObjectData)["mfa_level"].get<MFALevel>();
		}

		if (jsonObjectData->contains("system_channel_flags") && !(*jsonObjectData)["system_channel_flags"].is_null()) {
			this->systemChannelFlags = (*jsonObjectData)["system_channel_flags"].get<SystemChannelFlags>();
		}

		if (jsonObjectData->contains("large") && !(*jsonObjectData)["large"].is_null()) {
			this->flags = setBool<int8_t, GuildFlags>(this->flags, GuildFlags::Large, (*jsonObjectData)["large"].get<bool>());
		}

		if (jsonObjectData->contains("unavailable") && !(*jsonObjectData)["unavailable"].is_null()) {
			this->flags =
				setBool<int8_t, GuildFlags>(this->flags, GuildFlags::Unavailable, (*jsonObjectData)["unavailable"].get<bool>());
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			this->memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			this->members.clear();
			this->members.reserve((*jsonObjectData)["members"].size());
			for (auto& value: (*jsonObjectData)["members"]) {
				std::unique_ptr<GuildMemberData> newData{ std::make_unique<GuildMemberData>() };
				DiscordCoreAPI::parseObject(&value, *newData);
				newData->guildId = this->id;
				auto userId = newData->id;
				this->members.push_back(newData.release());
			}
		}

		if (jsonObjectData->contains("voice_states") && !(*jsonObjectData)["voice_states"].is_null()) {
			for (auto& value: (*jsonObjectData)["voice_states"]) {
				auto userId = stoull(value["user_id"].get<std::string>());
				for (auto& value02: this->members) {
					if (value02->id == userId) {
						value02->voiceChannelId = stoull(value["channel_id"].get<std::string>());
					}
				}
			}
		}

		if (jsonObjectData->contains("channels") && !(*jsonObjectData)["channels"].is_null()) {
			this->channels.clear();
			this->channels.reserve((*jsonObjectData)["channels"].size());
			for (auto& value: (*jsonObjectData)["channels"]) {
				std::unique_ptr<ChannelData> newData{};
				DiscordCoreAPI::parseObject(&value, *newData);
				newData->guildId = this->id;
				this->channels.push_back(newData->id);
				Channels::insertChannel(std::move(newData));
			}
		}

		if (jsonObjectData->contains("presences") && !(*jsonObjectData)["presences"].is_null()) {
			this->presences.clear();
			for (auto& value: (*jsonObjectData)["presences"]) {
				PresenceUpdateData newData{ &value };
				this->presences[newData.user.id] = newData;
			}
		}

		if (jsonObjectData->contains("max_presences") && !(*jsonObjectData)["max_presences"].is_null()) {
			this->maxPresences = (*jsonObjectData)["max_presences"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_members") && !(*jsonObjectData)["max_members"].is_null()) {
			this->maxMembers = (*jsonObjectData)["max_members"].get<int32_t>();
		}

		if (jsonObjectData->contains("premium_subscription_count") && !(*jsonObjectData)["premium_subscription_count"].is_null()) {
			this->premiumSubscriptionCount = (*jsonObjectData)["premium_subscription_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("premium_tier") && !(*jsonObjectData)["premium_tier"].is_null()) {
			this->premiumTier = (*jsonObjectData)["premium_tier"].get<PremiumTier>();
		}

		if (jsonObjectData->contains("max_video_channel_users") && !(*jsonObjectData)["max_video_channel_users"].is_null()) {
			this->maxVideoChannelUsers = (*jsonObjectData)["max_video_channel_users"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			this->approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			this->approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("welcome_screen") && !(*jsonObjectData)["welcome_screen"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["welcome_screen"], this->welcomeScreen);
		}

		if (jsonObjectData->contains("nsfw_level") && !(*jsonObjectData)["nsfw_level"].is_null()) {
			this->nsfwLevel = (*jsonObjectData)["nsfw_level"].get<GuildNSFWLevel>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildVector& theData) {
		theData.theGuilds.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theGuilds.push_back(newData);
		}
		theData.theGuilds.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildMemberVector& theData) {
		theData.theGuildMembers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildMemberData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theGuildMembers.push_back(newData);
		}
		theData.theGuildMembers.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEvent& theData) {
		theData.privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(&(*jsonObjectData), "privacy_level"));

		theData.entityType = static_cast<GuildScheduledEventEntityType>(getUint8(&(*jsonObjectData), "entity_type"));

		theData.status = static_cast<GuildScheduledEventStatus>(getUint8(&(*jsonObjectData), "status"));

		auto entityMetadataData = getObject(&(*jsonObjectData), "entity_metadata");
		theData.entityMetadata = &entityMetadataData;

		theData.scheduledStartTime = getString(&(*jsonObjectData), "scheduled_start_time");

		theData.scheduledEndTime = getString(&(*jsonObjectData), "scheduled_end_time");

		theData.userCount = getUint32(&(*jsonObjectData), "user_count");

		theData.description = getString(&(*jsonObjectData), "description");

		theData.channelId = strtoull(getString(&(*jsonObjectData), "channel_id"));

		theData.creatorId = getString(&(*jsonObjectData), "creator_id");

		theData.entityId = getString(&(*jsonObjectData), "entity_id");

		theData.guildId = strtoull(getString(&(*jsonObjectData), "guild_id"));

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["creator"], theData.creator);

		theData.name = getString(&(*jsonObjectData), "name");

		theData.id = strtoull(getString(&(*jsonObjectData), "id"));
	}

	void parseObject(const nlohmann::json* jsonObjectData, Message& theData) {
		theData.content = getString(&(*jsonObjectData), "content");

		theData.id = strtoull(getString(&(*jsonObjectData), "id"));

		theData.channelId = strtoull(getString(&(*jsonObjectData), "channel_id"));

		theData.guildId = strtoull(getString(&(*jsonObjectData), "guild_id"));

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["author"], theData.author);

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], theData.member);

		theData.timestamp = getString(&(*jsonObjectData), "timestamp");

		theData.editedTimestamp = getString(&(*jsonObjectData), "edited_timestamp");

		theData.tts = getBoolReal(&(*jsonObjectData), "tts");

		theData.mentionEveryone = getBoolReal(&(*jsonObjectData), "mention_everyone");

		if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
			theData.mentions.clear();
			for (auto& value: (*jsonObjectData)["mentions"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.mentions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
			theData.mentionRoles.clear();
			for (auto& value: (*jsonObjectData)["mention_roles"]) {
				theData.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
			theData.mentionChannels.clear();
			for (auto& value: (*jsonObjectData)["mention_channels"]) {
				ChannelMentionData newData{ &value };
				theData.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{ &value };
				theData.reactions.emplace_back(newData);
			}
		}

		theData.nonce = getString(&(*jsonObjectData), "nonce");

		theData.pinned = getBoolReal(&(*jsonObjectData), "pinned");

		theData.webHookId = strtoull(getString(&(*jsonObjectData), "webhook_id"));

		theData.type = static_cast<MessageType>(getUint8(&(*jsonObjectData), "type"));

		auto activityData = getObject(&(*jsonObjectData), "activity");
		theData.activity = &activityData;

		auto applicationData = getObject(&(*jsonObjectData), "application");
		DiscordCoreAPI::parseObject(&applicationData, theData.application);

		theData.applicationId = strtoull(getString(&(*jsonObjectData), "application_id"));

		auto messageReferenceData= getObject(&(*jsonObjectData), "message_reference");
		theData.messageReference = &messageReferenceData;

		theData.flags = getUint32(&(*jsonObjectData), "flags");

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{ &value };
				theData.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{ &value };
				theData.stickers.emplace_back(newData);
			}
		}

		auto interactionData= getObject(&(*jsonObjectData), "interaction");
		theData.interaction = &interactionData;

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{ &value };
				theData.components.emplace_back(newData);
			}
		}

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["thread"], theData.thread);
	}

	void parseObject(const nlohmann::json* jsonObjectData, MessageVector& theData) {
		theData.theMessages.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Message newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theMessages.push_back(newData);
		}
		theData.theMessages.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, Reaction& theData) {
		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			theData.count = (*jsonObjectData)["count"].get<int32_t>();
		}

		if (jsonObjectData->contains("me") && !(*jsonObjectData)["me"].is_null()) {
			theData.count = (*jsonObjectData)["me"].get<bool>();
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["emoji"], theData.emoji);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], theData.member);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, ReactionVector& theData) {
		theData.theReactions.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Reaction newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theReactions.push_back(newData);
		}
		theData.theReactions.shrink_to_fit();
	}

	void Role::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				this->id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				this->id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			this->icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("unicode_emoji") && !(*jsonObjectData)["unicode_emoji"].is_null()) {
			std::stringstream theStream{};
			theStream << (*jsonObjectData)["unicode_emoji"] << endl;
			for (auto& value: theStream.str()) {
				this->unicodeEmoji.push_back(value);
			}
			this->unicodeEmoji = static_cast<std::string>(this->unicodeEmoji).substr(1, this->unicodeEmoji.size() - 3);
		}

		if (jsonObjectData->contains("color") && !(*jsonObjectData)["color"].is_null()) {
			this->color = (*jsonObjectData)["color"].get<int32_t>();
		}

		if (jsonObjectData->contains("hoist") && !(*jsonObjectData)["hoist"].is_null()) {
			this->flags = setBool<int8_t, RoleFlags>(this->flags, RoleFlags::Hoist, (*jsonObjectData)["hoist"].get<bool>());
		}

		if (jsonObjectData->contains("position") && !(*jsonObjectData)["position"].is_null()) {
			this->position = (*jsonObjectData)["position"].get<int32_t>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			this->permissions = Permissions{ (*jsonObjectData)["permissions"].get<std::string>() };
		}

		if (jsonObjectData->contains("managed") && !(*jsonObjectData)["managed"].is_null()) {
			this->flags = setBool<int8_t, RoleFlags>(this->flags, RoleFlags::Managed, (*jsonObjectData)["managed"].get<bool>());
		}

		if (jsonObjectData->contains("mentionable") && !(*jsonObjectData)["mentionable"].is_null()) {
			this->flags = setBool<int8_t, RoleFlags>(this->flags, RoleFlags::Mentionable, (*jsonObjectData)["mentionable"].get<bool>());
		}

		if (jsonObjectData->contains("tags") && !(*jsonObjectData)["tags"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["tags"], this->tags);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, RoleVector&theData) {
		theData.theRoles.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			RoleData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theRoles.push_back(newData);
		}
		theData.theRoles.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, StageInstance& theData) {
		theData.id = strtoull(getString(&(*jsonObjectData), "id"));

		theData.guildId = strtoull(getString(&(*jsonObjectData), "guild_id"));

		theData.channelId = strtoull(getString(&(*jsonObjectData), "channel_id"));

		theData.topic = getString(&(*jsonObjectData), "topic");

		theData.privacyLevel = static_cast<StageInstancePrivacyLevel>(getUint8(&(*jsonObjectData), "privacy_level"));

		theData.discoverableDisabled = getBoolReal(&(*jsonObjectData), "discoverable_disabled");
	}

	void parseObject(const nlohmann::json* jsonObjectData, Sticker& theData) {
		if (jsonObjectData->contains("asset") && !(*jsonObjectData)["asset"].is_null()) {
			theData.asset = (*jsonObjectData)["asset"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			theData.formatType = (*jsonObjectData)["format_type"].get<StickerFormatType>();
		}

		if (jsonObjectData->contains("available") && !(*jsonObjectData)["available"].is_null()) {
			theData.stickerFlags = setBool<int32_t, StickerFlags>(theData.stickerFlags, StickerFlags::Available, (*jsonObjectData)["available"].get<bool>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("pack_id") && !(*jsonObjectData)["pack_id"].is_null()) {
			theData.packId = (*jsonObjectData)["pack_id"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<StickerType>();
		}

		if (jsonObjectData->contains("sort_value") && !(*jsonObjectData)["sort_value"].is_null()) {
			theData.sortValue = (*jsonObjectData)["sort_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, StickerVector&theData) {
		theData.theStickers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Sticker newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theStickers.push_back(newData);
		}
		theData.theStickers.shrink_to_fit();
	}

	void Thread::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				this->id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				this->id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("position") && !(*jsonObjectData)["position"].is_null()) {
			this->position = (*jsonObjectData)["position"].get<int32_t>();
		}

		if (jsonObjectData->contains("permission_overwrites") && !(*jsonObjectData)["permission_overwrites"].is_null()) {
			this->permissionOverwrites.clear();
			for (auto& value: (*jsonObjectData)["permission_overwrites"]) {
				OverWriteData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->permissionOverwrites.push_back(newData);
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			this->topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("nsfw") && !(*jsonObjectData)["nsfw"].is_null()) {
			this->flags = setBool<int8_t, ChannelFlags>(this->flags, ChannelFlags::NSFW, (*jsonObjectData)["nsfw"].get<bool>());
		}

		if (jsonObjectData->contains("last_message_id") && !(*jsonObjectData)["last_message_id"].is_null()) {
			this->lastMessageId = (*jsonObjectData)["last_message_id"].get<std::string>();
		}

		if (jsonObjectData->contains("bitrate") && !(*jsonObjectData)["bitrate"].is_null()) {
			this->bitrate = (*jsonObjectData)["bitrate"].get<int32_t>();
		}

		if (jsonObjectData->contains("user_limit") && !(*jsonObjectData)["user_limit"].is_null()) {
			this->userLimit = (*jsonObjectData)["user_limit"].get<int32_t>();
		}

		if (jsonObjectData->contains("rate_limit_per_user") && !(*jsonObjectData)["rate_limit_per_user"].is_null()) {
			this->rateLimitPerUser = (*jsonObjectData)["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData->contains("recipients") && !(*jsonObjectData)["recipients"].is_null()) {
			this->recipients.clear();
			for (auto& value: (*jsonObjectData)["recipients"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->recipients[newData.id] = std::move(newData);
			}
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			this->icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("owner_id") && !(*jsonObjectData)["owner_id"].is_null()) {
			this->ownerId = stoull((*jsonObjectData)["owner_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("parent_id") && !(*jsonObjectData)["parent_id"].is_null()) {
			if ((*jsonObjectData)["parent_id"].is_string()) {
				this->parentId = stoull((*jsonObjectData)["parent_id"].get<std::string>());
			} else {
				this->parentId = (*jsonObjectData)["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			this->lastPinTimestamp = (*jsonObjectData)["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("rtc_region") && !(*jsonObjectData)["rtc_region"].is_null()) {
			this->rtcRegion = (*jsonObjectData)["rtc_region"].get<std::string>();
		}

		if (jsonObjectData->contains("video_quality_mode") && !(*jsonObjectData)["video_quality_mode"].is_null()) {
			this->videoQualityMode = (*jsonObjectData)["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData->contains("message_count") && !(*jsonObjectData)["message_count"].is_null()) {
			this->messageCount = (*jsonObjectData)["message_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			this->memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("thread_metadata") && !(*jsonObjectData)["thread_metadata"].is_null()) {
			this->threadMetadata = &(*jsonObjectData)["thread_metadata"];
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, UserVector& theData) {
		theData.theUsers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			UserData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theUsers.push_back(newData);
		}
		theData.theUsers.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, WebHook& theData) {
		theData.id = strtoull(getString(&(*jsonObjectData), "id"));

		theData.type = static_cast<WebHookType>(getUint8(&(*jsonObjectData), "type"));

		theData.guildId = strtoull(getString(&(*jsonObjectData), "guild_id"));

		theData.channelId = strtoull(getString(&(*jsonObjectData), "channel_id"));

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);

		theData.name = getString(&(*jsonObjectData), "name");

		theData.avatar = getString(&(*jsonObjectData), "avatar");

		theData.token = getString(&(*jsonObjectData), "token");

		theData.applicationId = strtoull(getString(&(*jsonObjectData), "application_id"));

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["source_guild"], theData.sourceGuild);

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["source_channel"], theData.sourceChannel);

		theData.url = getString(&(*jsonObjectData), "url");
	}

	void parseObject(const nlohmann::json* jsonObjectData, WebHookVector& theData) {
		theData.theWebHooks.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			WebHook newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theWebHooks.push_back(newData);
		}
		theData.theWebHooks.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, RoleTagsData& theData) {
		theData.botId = getString(&(*jsonObjectData), "bot_id");

		theData.integrationId = getString(&(*jsonObjectData), "integration_id");
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedFooterData& theData) {
		if (jsonObjectData->contains("text") && !(*jsonObjectData)["text"].is_null()) {
			theData.text = (*jsonObjectData)["text"].get<std::string>();
		}
		if (jsonObjectData->contains("icon_url") && !(*jsonObjectData)["icon_url"].is_null()) {
			theData.iconUrl = (*jsonObjectData)["icon_url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_icon_url") && !(*jsonObjectData)["proxy_icon_url"].is_null()) {
			theData.proxyIconUrl = (*jsonObjectData)["proxy_icon_url"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedImageData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theData.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theData.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theData.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedThumbnailData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theData.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theData.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theData.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedVideoData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theData.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theData.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theData.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedProviderData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedAuthorData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_icon_url") && !(*jsonObjectData)["proxy_icon_url"].is_null()) {
			theData.proxyIconUrl = (*jsonObjectData)["proxy_icon_url"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("icon_url") && !(*jsonObjectData)["icon_url"].is_null()) {
			theData.iconUrl = (*jsonObjectData)["icon_url"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedFieldData& theData) {
		if (jsonObjectData->contains("inline") && !(*jsonObjectData)["inline"].is_null()) {
			theData.Inline = (*jsonObjectData)["inline"].get<bool>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			theData.value = (*jsonObjectData)["value"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmbedData& theData) {
		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null()) {
			theData.title = (*jsonObjectData)["title"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theData.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("color") && !(*jsonObjectData)["color"].is_null()) {
			int32_t colorValInt = (*jsonObjectData)["color"].get<int32_t>();
			std::stringstream stream{};
			stream << std::setbase(16) << colorValInt;
			theData.hexColorValue = stream.str();
		}

		if (jsonObjectData->contains("footer") && !(*jsonObjectData)["footer"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["footer"], theData.footer);
		}

		if (jsonObjectData->contains("image") && !(*jsonObjectData)["image"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["image"], theData.image);
		}

		if (jsonObjectData->contains("provider") && !(*jsonObjectData)["provider"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["provider"], theData.provider);
		}

		if (jsonObjectData->contains("thumbnail") && !(*jsonObjectData)["thumbnail"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["thumbnail"], theData.thumbnail);
		}

		if (jsonObjectData->contains("video") && !(*jsonObjectData)["video"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["video"], theData.video);
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["author"], theData.author);
		}

		if (jsonObjectData->contains("fields") && !(*jsonObjectData)["fields"].is_null()) {
			theData.fields.clear();
			for (auto& value: (*jsonObjectData)["fields"]) {
				EmbedFieldData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.fields.emplace_back(newData);
			}
		}
	}

	void MessageReferenceData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			this->messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("fail_if_not_exists") && !(*jsonObjectData)["fail_if_not_exists"].is_null()) {
			this->failIfNotExists = (*jsonObjectData)["fail_if_not_exists"].get<bool>();
		}
	}

	void ThreadMetadataData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("archived") && !(*jsonObjectData)["archived"].is_null()) {
			this->archived = (*jsonObjectData)["archived"].get<bool>();
		}

		if (jsonObjectData->contains("invitable") && !(*jsonObjectData)["invitable"].is_null()) {
			this->invitable = (*jsonObjectData)["invitable"].get<bool>();
		}

		if (jsonObjectData->contains("auto_archive_duration") && !(*jsonObjectData)["auto_archive_duration"].is_null()) {
			this->autoArchiveDuration = (*jsonObjectData)["auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData->contains("archive_timestamp") && !(*jsonObjectData)["archive_timestamp"].is_null()) {
			this->archiveTimestamp = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["archive_timestamp"].get<std::string>());
		}

		if (jsonObjectData->contains("locked") && !(*jsonObjectData)["locked"].is_null()) {
			this->locked = (*jsonObjectData)["locked"].get<bool>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, ThreadMemberData& theData) {
		theData.id = strtoull(getString(&(*jsonObjectData), "id"));

		theData.userId = strtoull(getString(&(*jsonObjectData), "user_id"));

		theData.joinTimestamp = TimeStamp<std::chrono::milliseconds>(getString(&(*jsonObjectData), "join_timestamp"));

		theData.flags = getUint32(&(*jsonObjectData), "flags");
	}

	void parseObject(const nlohmann::json* jsonObjectData, ThreadMemberDataVector&theData) {
		theData.theThreadMemberDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ThreadMemberData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theThreadMemberDatas.push_back(newData);
		}
		theData.theThreadMemberDatas.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, VoiceStateData& theData) {
		theData.requestToSpeakTimestamp = DiscordCoreAPI::getString(jsonObjectData, "request_to_speak_timestamp");

		theData.channelId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(jsonObjectData, "channel_id"));

		theData.guildId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(jsonObjectData, "guild_id"));

		theData.selfStream = DiscordCoreAPI::getBoolReal(jsonObjectData, "self_stream");

		theData.userId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(jsonObjectData, "user_id"));

		theData.selfVideo = DiscordCoreAPI::getBoolReal(jsonObjectData, "self_video");

		theData.sessionId = DiscordCoreAPI::getString(jsonObjectData, "session_id");

		theData.selfDeaf = DiscordCoreAPI::getBoolReal(jsonObjectData, "self_deaf");

		theData.selfMute = DiscordCoreAPI::getBoolReal(jsonObjectData, "self_mute");

		theData.suppress = DiscordCoreAPI::getBoolReal(jsonObjectData, "suppress");

		theData.deaf = DiscordCoreAPI::getBoolReal(jsonObjectData, "deaf");

		theData.mute = DiscordCoreAPI::getBoolReal(jsonObjectData, "mute");
	}

	void parseObject(const nlohmann::json* jsonObjectData, ActiveThreadsData& theData) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("has_more") && !(*jsonObjectData)["has_more"].is_null()) {
			theData.hasMore = (*jsonObjectData)["has_more"].get<bool>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, ArchivedThreadsData& theData) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("has_more") && !(*jsonObjectData)["has_more"].is_null()) {
			theData.hasMore = (*jsonObjectData)["has_more"].get<bool>();
		}
	}

	void ActionMetaData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("duration_seconds") && !(*jsonObjectData)["duration_seconds"].is_null()) {
			this->durationSeconds = (*jsonObjectData)["duration_seconds"].get<int64_t>();
		}
	}

	void TriggerMetaData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("keyword_filter") && !(*jsonObjectData)["keyword_filter"].is_null()) {
			for (auto& value: (*jsonObjectData)["keyword_filter"]) {
				this->keywordFilter.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("presets") && !(*jsonObjectData)["presets"].is_null()) {
			for (auto& value: (*jsonObjectData)["presets"]) {
				this->presets.emplace_back(static_cast<KeywordPresetType>(value.get<uint64_t>()));
			}
		}
	}

	void ActionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("metadata") && !(*jsonObjectData)["metadata"].is_null()) {
			this->metadata = &(*jsonObjectData)["metadata"];
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = static_cast<ActionType>((*jsonObjectData)["type"].get<int64_t>());
		}
	}

	void AutoModerationRuleData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			this->enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("trigger_type") && !(*jsonObjectData)["trigger_type"].is_null()) {
			this->triggerType = static_cast<TriggerType>((*jsonObjectData)["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("event_type") && !(*jsonObjectData)["event_type"].is_null()) {
			this->eventType = static_cast<EventType>((*jsonObjectData)["event_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			this->creatorId = (*jsonObjectData)["creator_id"].get<Snowflake>();
		}

		if (jsonObjectData->contains("actions") && !(*jsonObjectData)["actions"].is_null()) {
			for (auto& value: (*jsonObjectData)["actions"]) {
				ActionData newData{ &value };
				this->actions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("exempt_roles") && !(*jsonObjectData)["exempt_roles"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_roles"]) {
				this->exemptRoles.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData->contains("trigger_metadata") && !(*jsonObjectData)["trigger_metadata"].is_null()) {
			this->triggerMetaData = &(*jsonObjectData)["trigger_metadata"];
		}

		if (jsonObjectData->contains("exempt_channels") && !(*jsonObjectData)["exempt_channels"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_channels"]) {
				this->exemptChannels.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = (*jsonObjectData)["guild_id"].get<Snowflake>();
		}
	}

	void ApplicationCommandPermissionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("permission") && !(*jsonObjectData)["permission"].is_null()) {
			this->permission = (*jsonObjectData)["permission"].get<bool>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ApplicationCommandPermissionType>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildApplicationCommandPermissionsData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theData.permissions.clear();
			for (auto& value: (*jsonObjectData)["permissions"]) {
				ApplicationCommandPermissionData newData{ &value };
				theData.permissions.emplace_back(newData);
			}
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildApplicationCommandPermissionsDataVector& theData) {
		theData.theGuildApplicationCommandPermissionsDatas.clear();
		for (auto& value: *jsonObjectData) {
			GuildApplicationCommandPermissionsData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theGuildApplicationCommandPermissionsDatas.emplace_back(newData);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmojiData& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theData.roles.clear();
			for (auto& value: (*jsonObjectData)["roles"]) {
				RoleData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.roles.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("require_colons") && !(*jsonObjectData)["require_colons"].is_null()) {
			theData.requireColons = (*jsonObjectData)["require_colons"].get<bool>();
		}

		if (jsonObjectData->contains("managed") && !(*jsonObjectData)["managed"].is_null()) {
			theData.managed = (*jsonObjectData)["managed"].get<bool>();
		}

		if (jsonObjectData->contains("animated") && !(*jsonObjectData)["animated"].is_null()) {
			theData.animated = (*jsonObjectData)["animated"].get<bool>();
		}

		if (jsonObjectData->contains("available") && !(*jsonObjectData)["available"].is_null()) {
			theData.available = (*jsonObjectData)["available"].get<bool>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, EmojiDataVector& theData) {
		theData.theEmojiDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			EmojiData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theEmojiDatas.push_back(newData);
		}
		theData.theEmojiDatas.shrink_to_fit();
	}

	void ReactionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			this->count = (*jsonObjectData)["count"].get<int32_t>();
		}

		if (jsonObjectData->contains("me") && !(*jsonObjectData)["me"].is_null()) {
			this->count = (*jsonObjectData)["me"].get<bool>();
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["emoji"], this->emoji);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			this->userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			this->messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}
	}

	void VoiceRegionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("custom") && !(*jsonObjectData)["custom"].is_null()) {
			this->custom = (*jsonObjectData)["custom"].get<bool>();
		}

		if (jsonObjectData->contains("deprecated") && !(*jsonObjectData)["deprecated"].is_null()) {
			this->deprecated = (*jsonObjectData)["deprecated"].get<bool>();
		}

		if (jsonObjectData->contains("optimal") && !(*jsonObjectData)["optimal"].is_null()) {
			this->optimal = (*jsonObjectData)["optimal"].get<bool>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, VoiceRegionDataVector&theData) {
		theData.theVoiceRegionDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			VoiceRegionData newData{ &value };
			theData.theVoiceRegionDatas.push_back(newData);
		}
		theData.theVoiceRegionDatas.shrink_to_fit();
	}

	void MessageActivityData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<MessageActivityType>();
		}

		if (jsonObjectData->contains("party_id") && !(*jsonObjectData)["party_id"].is_null()) {
			this->partyId = (*jsonObjectData)["party_id"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, BanData& theData) {
		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("reason") && !(*jsonObjectData)["reason"].is_null()) {
			theData.reason = (*jsonObjectData)["reason"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, BanDataVector& theData) {
		theData.theBanDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			BanData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theBanDatas.push_back(newData);
		}
		theData.theBanDatas.shrink_to_fit();
	}

	void TeamMembersObjectData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("membership_state") && !(*jsonObjectData)["membership_state"].is_null()) {
			this->membershipState = (*jsonObjectData)["membership_state"].get<int32_t>();
		}

		if (jsonObjectData->contains("team_id") && !(*jsonObjectData)["team_id"].is_null()) {
			this->teamId = (*jsonObjectData)["team_id"].get<std::string>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			this->permissions.clear();
			for (auto& value: (*jsonObjectData)["permissions"]) {
				this->permissions.emplace_back(Permissions{ value.get<std::string>() });
			}
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}
	}

	void TeamObjectData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			this->icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			this->members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				TeamMembersObjectData newData{ &value };
				this->members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("owner_user_id") && !(*jsonObjectData)["owner_user_id"].is_null()) {
			this->ownerUserId = (*jsonObjectData)["owner_user_id"].get<std::string>();
		}
	}

	void InstallParamsData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("scopes") && !(*jsonObjectData)["scopes"].is_null()) {
			this->scopes = (*jsonObjectData)["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			this->permissions = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, ApplicationData& theData) {
		if (jsonObjectData->contains("params") && !(*jsonObjectData)["params"].is_null()) {
			theData.params = &(*jsonObjectData)["params"];
		}

		if (jsonObjectData->contains("tags") && !(*jsonObjectData)["tags"].is_null()) {
			theData.tags = (*jsonObjectData)["tags"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theData.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("rpc_origins") && !(*jsonObjectData)["rpc_origins"].is_null()) {
			theData.rpcOrigins.clear();
			for (auto& value: (*jsonObjectData)["rpc_origins"]) {
				theData.rpcOrigins.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("bot_public") && !(*jsonObjectData)["bot_public"].is_null()) {
			theData.botPublic = (*jsonObjectData)["bot_public"].get<bool>();
		}

		if (jsonObjectData->contains("bot_require_code_grant") && !(*jsonObjectData)["bot_require_code_grant"].is_null()) {
			theData.botRequireCodeGrant = (*jsonObjectData)["bot_require_code_grant"].get<bool>();
		}

		if (jsonObjectData->contains("terms_of_service_url") && !(*jsonObjectData)["terms_of_service_url"].is_null()) {
			theData.termsOfServiceUrl = (*jsonObjectData)["terms_of_service_url"].get<std::string>();
		}

		if (jsonObjectData->contains("privacy_policy_url") && !(*jsonObjectData)["privacy_policy_url"].is_null()) {
			theData.privacyPolicyUrl = (*jsonObjectData)["privacy_policy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("owner") && !(*jsonObjectData)["owner"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["owner"], theData.owner);
		}

		if (jsonObjectData->contains("summary") && !(*jsonObjectData)["summary"].is_null()) {
			theData.summary = (*jsonObjectData)["summary"].get<std::string>();
		}

		if (jsonObjectData->contains("verify_key") && !(*jsonObjectData)["verify_key"].is_null()) {
			theData.verifyKey = (*jsonObjectData)["verify_key"].get<std::string>();
		}

		if (jsonObjectData->contains("team") && !(*jsonObjectData)["team"].is_null()) {
			theData.team = &(*jsonObjectData)["team"];
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("primary_sku_id") && !(*jsonObjectData)["primary_sku_id"].is_null()) {
			theData.primarySkuId = (*jsonObjectData)["primary_sku_id"].get<std::string>();
		}

		if (jsonObjectData->contains("slug") && !(*jsonObjectData)["slug"].is_null()) {
			theData.slug = (*jsonObjectData)["slug"].get<std::string>();
		}

		if (jsonObjectData->contains("cover_image") && !(*jsonObjectData)["cover_image"].is_null()) {
			theData.coverImage = (*jsonObjectData)["cover_image"].get<std::string>();
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theData.flags = (*jsonObjectData)["flags"].get<ApplicationFlags>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, AuthorizationInfoData& theData) {
		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], theData.application);
		}

		if (jsonObjectData->contains("scopes") && !(*jsonObjectData)["scopes"].is_null()) {
			theData.scopes = (*jsonObjectData)["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("expires") && !(*jsonObjectData)["expires"].is_null()) {
			theData.expires = (*jsonObjectData)["expires"];
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}
	};

	void AccountData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildWidgetData& theData) {
		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			theData.enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}
	};

	void parseObject(const nlohmann::json* jsonObjectData, GuildWidgetImageData& theData) {
		if (jsonObjectData->contains("widget_image") && !(*jsonObjectData)["widget_image"].is_null()) {
			theData.url = (*jsonObjectData)["widget_image"].get<bool>();
		}
	}

	void IntegrationData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			this->enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("syncing") && !(*jsonObjectData)["syncing"].is_null()) {
			this->syncing = (*jsonObjectData)["syncing"].get<bool>();
		}

		if (jsonObjectData->contains("role_id") && !(*jsonObjectData)["role_id"].is_null()) {
			this->roleId = stoull((*jsonObjectData)["role_id"].get<std::string>());
		}

		if (jsonObjectData->contains("enable_emoticons") && !(*jsonObjectData)["enable_emoticons"].is_null()) {
			this->enableEmoticons = (*jsonObjectData)["enable_emoticons"].get<bool>();
		}

		if (jsonObjectData->contains("expire_behavior") && !(*jsonObjectData)["expire_behavior"].is_null()) {
			this->expireBehavior = (*jsonObjectData)["expire_behavior"].get<int32_t>();
		}

		if (jsonObjectData->contains("expire_grace_period") && !(*jsonObjectData)["expire_grace_period"].is_null()) {
			this->expireGracePeriod = (*jsonObjectData)["expire_grace_period"].get<int32_t>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}

		if (jsonObjectData->contains("account") && !(*jsonObjectData)["account"].is_null()) {
			this->account = &(*jsonObjectData)["account"];
		}

		if (jsonObjectData->contains("synced_at") && !(*jsonObjectData)["synced_at"].is_null()) {
			this->syncedAt = (*jsonObjectData)["synced_at"].get<std::string>();
		}

		if (jsonObjectData->contains("subscriber_count") && !(*jsonObjectData)["subscriber_count"].is_null()) {
			this->subscriberCount = (*jsonObjectData)["subscriber_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("revoked") && !(*jsonObjectData)["revoked"].is_null()) {
			this->revoked = (*jsonObjectData)["revoked"].get<bool>();
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], this->application);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, IntegrationDataVector& theData) {
		theData.theIntegrationDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			IntegrationData newData{ &value };
			theData.theIntegrationDatas.push_back(newData);
		}
		theData.theIntegrationDatas.shrink_to_fit();
	}

	void OptionalAuditEntryInfoData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("delete_member_days") && !(*jsonObjectData)["delete_member_days"].is_null()) {
			this->deleteMemberDays = (*jsonObjectData)["delete_member_days"].get<std::string>();
		}

		if (jsonObjectData->contains("members_removed") && !(*jsonObjectData)["members_removed"].is_null()) {
			this->membersRemoved = (*jsonObjectData)["members_removed"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			this->messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			this->count = (*jsonObjectData)["count"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("role_name") && !(*jsonObjectData)["role_name"].is_null()) {
			this->roleName = (*jsonObjectData)["role_name"].get<std::string>();
		}
	}

	void AuditLogChangeData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("new_value") && !(*jsonObjectData)["new_value"].is_null()) {
			this->newValue = (*jsonObjectData)["new_value"];
		}

		if (jsonObjectData->contains("old_value") && !(*jsonObjectData)["old_value"].is_null()) {
			this->oldValue = (*jsonObjectData)["old_value"];
		}

		if (jsonObjectData->contains("key") && !(*jsonObjectData)["key"].is_null()) {
			this->key = (*jsonObjectData)["key"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildPruneCountData& theData) {
		if (jsonObjectData->contains("pruned") && !(*jsonObjectData)["pruned"].is_null()) {
			theData.count = (*jsonObjectData)["pruned"].get<int32_t>();
		}
	}

	void AuditLogEntryData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			this->targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}

		if (jsonObjectData->contains("changes") && !(*jsonObjectData)["changes"].is_null()) {
			this->changes.clear();
			for (auto& value: (*jsonObjectData)["changes"]) {
				AuditLogChangeData newData{ &value };
				this->changes.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			this->userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
			this->createdTimeStamp = this->getCreatedAtTimestamp(TimeFormat::LongDateTime);
		}

		if (jsonObjectData->contains("action_type") && !(*jsonObjectData)["action_type"].is_null()) {
			this->actionType = (*jsonObjectData)["action_type"].get<AuditLogEvent>();
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			this->options = &(*jsonObjectData)["options"];
		}

		if (jsonObjectData->contains("reason") && !(*jsonObjectData)["reason"].is_null()) {
			this->reason = (*jsonObjectData)["reason"].get<std::string>();
		}
	}

	void PartyData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("Size") && !(*jsonObjectData)["Size"].is_null()) {
			this->size[0] = (*jsonObjectData)["Size"]["CurrentSize"].get<int32_t>();
			this->size[1] = (*jsonObjectData)["Size"]["MaxSize"].get<int32_t>();
		}
	}

	void AssetsData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("LargeImage") && !(*jsonObjectData)["LargeImage"].is_null()) {
			this->largeImage = (*jsonObjectData)["LargeImage"].get<std::string>();
		}

		if (jsonObjectData->contains("LargeText") && !(*jsonObjectData)["LargeText"].is_null()) {
			this->largeText = (*jsonObjectData)["LargeText"].get<std::string>();
		}

		if (jsonObjectData->contains("SmallImage") && !(*jsonObjectData)["SmallImage"].is_null()) {
			this->smallImage = (*jsonObjectData)["SmallImage"].get<std::string>();
		}

		if (jsonObjectData->contains("SmallText") && !(*jsonObjectData)["SmallText"].is_null()) {
			this->smallText = (*jsonObjectData)["SmallText"].get<std::string>();
		}
	}

	void SecretsData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("Join") && !(*jsonObjectData)["Join"].is_null()) {
			this->join = (*jsonObjectData)["Join"].get<std::string>();
		}

		if (jsonObjectData->contains("Spectate") && !(*jsonObjectData)["Spectate"].is_null()) {
			this->spectate = (*jsonObjectData)["Spectate"].get<std::string>();
		}

		if (jsonObjectData->contains("Match") && !(*jsonObjectData)["Match"].is_null()) {
			this->match = (*jsonObjectData)["Match"].get<std::string>();
		}
	}

	void TimestampData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("Start") && !(*jsonObjectData)["Start"].is_null()) {
			this->start = (*jsonObjectData)["Start"].get<int64_t>();
		}

		if (jsonObjectData->contains("End") && !(*jsonObjectData)["End"].is_null()) {
			this->end = (*jsonObjectData)["End"].get<int64_t>();
		}
	}

	void ActivityData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("Name") && !(*jsonObjectData)["Name"].is_null()) {
			this->name = (*jsonObjectData)["Name"].get<std::string>();
		}

		if (jsonObjectData->contains("Timestamps") && !(*jsonObjectData)["Timestamps"].is_null()) {
			this->timestamps = &(*jsonObjectData)["Timestamps"];
		}

		if (jsonObjectData->contains("ApplicationId") && !(*jsonObjectData)["ApplicationId"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["ApplicationId"].get<std::string>());
		}

		if (jsonObjectData->contains("Details") && !(*jsonObjectData)["Details"].is_null()) {
			this->details = (*jsonObjectData)["Details"].get<std::string>();
		}

		if (jsonObjectData->contains("State") && !(*jsonObjectData)["State"].is_null()) {
			this->state = (*jsonObjectData)["State"].get<std::string>();
		}

		if (jsonObjectData->contains("ActivityParty") && !(*jsonObjectData)["ActivityParty"].is_null()) {
			this->party = &(*jsonObjectData)["ActivityParty"];
		}

		if (jsonObjectData->contains("ActivityAssets") && !(*jsonObjectData)["ActivityAssets"].is_null()) {
			this->assets = &(*jsonObjectData)["ActivityAssets"];
		}

		if (jsonObjectData->contains("ActivitySecrets") && !(*jsonObjectData)["ActivitySecrets"].is_null()) {
			this->secrets = &(*jsonObjectData)["ActivitySecrets"];
		}

		if (jsonObjectData->contains("Instance") && !(*jsonObjectData)["Instance"].is_null()) {
			this->instance = (*jsonObjectData)["Instance"].get<bool>();
		}
	}

	void ClientStatusData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("desktop") && !(*jsonObjectData)["desktop"].is_null()) {
			this->desktop = (*jsonObjectData)["desktop"].get<std::string>();
		}

		if (jsonObjectData->contains("mobile") && !(*jsonObjectData)["mobile"].is_null()) {
			this->mobile = (*jsonObjectData)["mobile"].get<std::string>();
		}

		if (jsonObjectData->contains("web") && !(*jsonObjectData)["web"].is_null()) {
			this->web = (*jsonObjectData)["web"].get<std::string>();
		}
	}

	void WelcomeScreenChannelData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("emoji_id") && !(*jsonObjectData)["emoji_id"].is_null()) {
			this->emojiId = stoull((*jsonObjectData)["emoji_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emoji_name") && !(*jsonObjectData)["emoji_name"].is_null()) {
			this->emojiName = (*jsonObjectData)["emoji_name"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, WelcomeScreenData& theData) {
		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("welcome_channels") && !(*jsonObjectData)["welcome_channels"].is_null()) {
			theData.welcomeChannels.clear();
			for (auto& value: (*jsonObjectData)["welcome_channels"]) {
				WelcomeScreenChannelData newData{ &value };
				theData.welcomeChannels.emplace_back(newData);
			}
		}
	}

	void PresenceUpdateData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null() && (*jsonObjectData)["guild_id"].get<std::string>() != "") {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("status") && !(*jsonObjectData)["status"].is_null()) {
			this->status = (*jsonObjectData)["status"].get<std::string>();
		}

		if (jsonObjectData->contains("activities") && !(*jsonObjectData)["activities"].is_null()) {
			this->activities.clear();
			for (auto& value: (*jsonObjectData)["activities"]) {
				ActivityData newData{ &value };
				this->activities.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("client_status") && !(*jsonObjectData)["client_status"].is_null()) {
			this->clientStatus = &(*jsonObjectData)["client_status"];
		}
	}

	void StageInstanceData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			this->topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("privacy_level") && !(*jsonObjectData)["privacy_level"].is_null()) {
			this->privacyLevel = (*jsonObjectData)["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		if (jsonObjectData->contains("discoverable_disabled") && !(*jsonObjectData)["discoverable_disabled"].is_null()) {
			this->discoverableDisabled = (*jsonObjectData)["discoverable_disabled"].get<bool>();
		}
	}

	void StickerData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("asset") && !(*jsonObjectData)["asset"].is_null()) {
			this->asset = (*jsonObjectData)["asset"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			this->formatType = (*jsonObjectData)["format_type"].get<StickerFormatType>();
		}

		if (jsonObjectData->contains("available") && !(*jsonObjectData)["available"].is_null()) {
			this->stickerFlags = setBool<int8_t, StickerFlags>(this->stickerFlags, StickerFlags::Available, (*jsonObjectData)["available"].get<bool>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("pack_id") && !(*jsonObjectData)["pack_id"].is_null()) {
			this->packId = (*jsonObjectData)["pack_id"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<StickerType>();
		}

		if (jsonObjectData->contains("sort_value") && !(*jsonObjectData)["sort_value"].is_null()) {
			this->sortValue = (*jsonObjectData)["sort_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildPreviewData& theData) {
		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			theData.approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			theData.approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("discovery_splash") && !(*jsonObjectData)["discovery_splash"].is_null()) {
			theData.discoverySplash = (*jsonObjectData)["discovery_splash"].get<std::string>();
		}

		if (jsonObjectData->contains("emojis") && !(*jsonObjectData)["emojis"].is_null()) {
			theData.emojis.clear();
			for (auto& value: (*jsonObjectData)["emojis"]) {
				EmojiData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.emojis.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.emojis.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{ &value };
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("features") && !(*jsonObjectData)["features"].is_null()) {
			theData.features = (*jsonObjectData)["features"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("splash") && !(*jsonObjectData)["splash"].is_null()) {
			theData.splash = (*jsonObjectData)["splash"].get<std::string>();
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theData.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}
	}

	void GuildDataVector::parseObject(const nlohmann::json* jsonObjectData) {
		this->theGuildDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			this->theGuildDatas.push_back(newData);
		}
		this->theGuildDatas.shrink_to_fit();
	}

	void GuildScheduledEventMetadata::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("location") && !(*jsonObjectData)["location"].is_null()) {
			this->location = (*jsonObjectData)["location"].get<std::string>();
		}
	}

	void GuildScheduledEventData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("privacy_level") && !(*jsonObjectData)["privacy_level"].is_null()) {
			this->privacyLevel = (*jsonObjectData)["privacy_level"].get<GuildScheduledEventPrivacyLevel>();
		}

		if (jsonObjectData->contains("entity_type") && !(*jsonObjectData)["entity_type"].is_null()) {
			this->entityType = (*jsonObjectData)["entity_type"].get<GuildScheduledEventEntityType>();
		}

		if (jsonObjectData->contains("status") && !(*jsonObjectData)["status"].is_null()) {
			this->status = (*jsonObjectData)["status"].get<GuildScheduledEventStatus>();
		}

		if (jsonObjectData->contains("entity_metadata") && !(*jsonObjectData)["entity_metadata"].is_null()) {
			this->entityMetadata = &(*jsonObjectData)["entity_metadata"];
		}

		if (jsonObjectData->contains("scheduled_start_time") && !(*jsonObjectData)["scheduled_start_time"].is_null()) {
			this->scheduledStartTime = (*jsonObjectData)["scheduled_start_time"].get<std::string>();
		}

		if (jsonObjectData->contains("scheduled_end_time") && !(*jsonObjectData)["scheduled_end_time"].is_null()) {
			this->scheduledEndTime = (*jsonObjectData)["scheduled_end_time"].get<std::string>();
		}

		if (jsonObjectData->contains("user_count") && !(*jsonObjectData)["user_count"].is_null()) {
			this->userCount = (*jsonObjectData)["user_count"].get<uint32_t>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			this->creatorId = (*jsonObjectData)["creator_id"].get<std::string>();
		}

		if (jsonObjectData->contains("entity_id") && !(*jsonObjectData)["entity_id"].is_null()) {
			this->entityId = (*jsonObjectData)["entity_id"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("creator") && !(*jsonObjectData)["creator"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["creator"], this->creator);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}
	}

	void GuildScheduledEventUserData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("guild_scheduled_event_id") && !(*jsonObjectData)["guild_scheduled_event_id"].is_null()) {
			this->guildScheduledEventId = (*jsonObjectData)["guild_scheduled_event_id"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventUserDataVector& theData) {
		theData.theGuildScheduledEventUserDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildScheduledEventUserData newData{ &value };
			theData.theGuildScheduledEventUserDatas.push_back(newData);
		}
		theData.theGuildScheduledEventUserDatas.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, InviteData& theData) {
		if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null() && (*jsonObjectData)["code"].is_string()) {
			theData.code = (*jsonObjectData)["code"].get<std::string>();
		} else if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null() && (*jsonObjectData)["code"].is_number_integer()) {
			theData.code = std::to_string((*jsonObjectData)["code"].get<int32_t>());
		}

		if (jsonObjectData->contains("guild") && !(*jsonObjectData)["guild"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["guild"], theData.guild);
		}

		if (jsonObjectData->contains("channel") && !(*jsonObjectData)["channel"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["channel"], theData.channel);
		}

		if (jsonObjectData->contains("inviter") && !(*jsonObjectData)["inviter"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["inviter"], theData.inviter);
		}

		if (jsonObjectData->contains("target_type") && !(*jsonObjectData)["target_type"].is_null()) {
			theData.targetType = (*jsonObjectData)["target_type"].get<int32_t>();
		}

		if (jsonObjectData->contains("target_user") && !(*jsonObjectData)["target_user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["target_user"], theData.targetUser);
		}

		if (jsonObjectData->contains("target_application") && !(*jsonObjectData)["target_application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["target_application"], theData.targetApplication);
		}

		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			theData.approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			theData.approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("expires_at") && !(*jsonObjectData)["expires_at"].is_null()) {
			theData.expiresAt = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["expires_at"].get<std::string>());
		}

		if (jsonObjectData->contains("stage_instance") && !(*jsonObjectData)["stage_instance"].is_null()) {
			theData.stageInstance = &(*jsonObjectData)["stage_instance"];
		}

		if (jsonObjectData->contains("guild_scheduled_event") && !(*jsonObjectData)["guild_scheduled_event"].is_null()) {
			theData.guildScheduledEvent = &(*jsonObjectData)["guild_scheduled_event"];
		}

		if (jsonObjectData->contains("uses") && !(*jsonObjectData)["uses"].is_null()) {
			theData.uses = (*jsonObjectData)["uses"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_uses") && !(*jsonObjectData)["max_uses"].is_null()) {
			theData.maxUses = (*jsonObjectData)["max_uses"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_age") && !(*jsonObjectData)["max_age"].is_null()) {
			theData.maxAge = (*jsonObjectData)["max_age"].get<int32_t>();
		}

		if (jsonObjectData->contains("temporary") && !(*jsonObjectData)["temporary"].is_null()) {
			theData.temporary = (*jsonObjectData)["temporary"].get<bool>();
		}

		if (jsonObjectData->contains("created_at") && !(*jsonObjectData)["created_at"].is_null()) {
			theData.createdAt = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["created_at"].get<std::string>());
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, InviteDataVector&theData) {
		theData.theInviteDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			InviteData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theInviteDatas.push_back(newData);
		}
		theData.theInviteDatas.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildTemplateData& theData) {
		if (jsonObjectData->contains("serialized_source_guild") && !(*jsonObjectData)["serialized_source_guild"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["serialized_source_guild"], theData.serializedSourceGuild);
		}

		if (jsonObjectData->contains("creator") && !(*jsonObjectData)["creator"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["creator"], theData.creator);
		}

		if (jsonObjectData->contains("source_guild_id") && !(*jsonObjectData)["source_guild_id"].is_null()) {
			theData.sourceGuildId = (*jsonObjectData)["source_guild_id"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("usage_count") && !(*jsonObjectData)["usage_count"].is_null()) {
			theData.usageCount = (*jsonObjectData)["usage_count"].get<uint32_t>();
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theData.creatorId = (*jsonObjectData)["creator_id"].get<std::string>();
		}

		if (jsonObjectData->contains("created_at") && !(*jsonObjectData)["created_at"].is_null()) {
			theData.createdAt = (*jsonObjectData)["created_at"].get<std::string>();
		}

		if (jsonObjectData->contains("updated_at") && !(*jsonObjectData)["updated_at"].is_null()) {
			theData.updatedAt = (*jsonObjectData)["updated_at"].get<std::string>();
		}

		if (jsonObjectData->contains("is_dirty") && !(*jsonObjectData)["is_dirty"].is_null()) {
			theData.isDirty = (*jsonObjectData)["is_dirty"].get<bool>();
		}

		if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null()) {
			theData.code = (*jsonObjectData)["code"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildTemplateDataVector& theData) {
		theData.theGuildTemplateDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildTemplateData newData{};
			DiscordCoreAPI::parseObject(&value, newData);
			theData.theGuildTemplateDatas.push_back(newData);
		}
		theData.theGuildTemplateDatas.shrink_to_fit();
	}

	void WebHookData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<WebHookType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("avatar") && !(*jsonObjectData)["avatar"].is_null()) {
			this->avatar = (*jsonObjectData)["avatar"].get<std::string>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			this->token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("source_guild") && !(*jsonObjectData)["source_guild"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["source_guild"], this->sourceGuild);
		}

		if (jsonObjectData->contains("source_channel") && !(*jsonObjectData)["source_channel"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["source_channel"], this->sourceChannel);
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			this->url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	void WebHookDataVector::parseObject(const nlohmann::json* jsonObjectData) {
		this->theWebHookDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			WebHookData newData{ &value };
			this->theWebHookDatas.push_back(newData);
		}
		this->theWebHookDatas.shrink_to_fit();
	}

	void parseObject(const nlohmann::json* jsonObjectData, AuditLogData& theData) {
		if (jsonObjectData->contains("webhooks") && !(*jsonObjectData)["webhooks"].is_null()) {
			theData.webhooks.clear();
			theData.webhooks.reserve((*jsonObjectData)["webhooks"].size());
			for (auto& value: (*jsonObjectData)["webhooks"]) {
				WebHookData newData{ &value };
				theData.webhooks.push_back(newData);
			}
		}

		if (jsonObjectData->contains("guild_scheduled_events") && !(*jsonObjectData)["guild_scheduled_events"].is_null()) {
			theData.guildScheduledEvents.clear();
			theData.guildScheduledEvents.reserve((*jsonObjectData)["guild_scheduled_events"].size());
			for (auto& value: (*jsonObjectData)["guild_scheduled_events"]) {
				GuildScheduledEventData newData{ &value };
				theData.guildScheduledEvents.push_back(newData);
			}
		}

		if (jsonObjectData->contains("auto_moderation_rules") && !(*jsonObjectData)["auto_moderation_rules"].is_null()) {
			theData.autoModerationRules.clear();
			theData.autoModerationRules.reserve((*jsonObjectData)["auto_moderation_rules"].size());
			for (auto& value: (*jsonObjectData)["auto_moderation_rules"]) {
				AutoModerationRuleData newData{ &value };
				theData.autoModerationRules.push_back(newData);
			}
		}

		if (jsonObjectData->contains("users") && !(*jsonObjectData)["users"].is_null()) {
			theData.users.clear();
			theData.users.reserve((*jsonObjectData)["users"].size());
			for (auto& value: (*jsonObjectData)["users"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.users.push_back(newData);
			}
		}

		if (jsonObjectData->contains("audit_log_entries") && !(*jsonObjectData)["audit_log_entries"].is_null()) {
			theData.auditLogEntries.clear();
			theData.auditLogEntries.reserve((*jsonObjectData)["audit_log_entries"].size());
			for (auto& value: (*jsonObjectData)["audit_log_entries"]) {
				AuditLogEntryData newData{ &value };
				theData.auditLogEntries.push_back(newData);
			}
		}

		if (jsonObjectData->contains("integrations") && !(*jsonObjectData)["integrations"].is_null()) {
			theData.integrations.clear();
			theData.integrations.reserve((*jsonObjectData)["integrations"].size());
			for (auto& value: (*jsonObjectData)["integrations"]) {
				IntegrationData newData{ &value };
				theData.integrations.push_back(newData);
			}
		}

		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			theData.threads.reserve((*jsonObjectData)["threads"].size());
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				theData.threads.push_back(newData);
			}
		}
	}

	void ReactionRemoveData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			this->userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			this->messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["emoji"], this->emoji);
		}
	}

	void ApplicationCommandOptionChoiceData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null() && (*jsonObjectData)["value"].is_string()) {
			if ((*jsonObjectData)["value"].is_string()) {
				this->value = (*jsonObjectData)["value"].get<std::string>();
			} else if ((*jsonObjectData)["value"].is_number_float()) {
				this->value = (*jsonObjectData)["value"].get<float>();
			} else if ((*jsonObjectData)["value"].is_number()) {
				this->value = (*jsonObjectData)["value"].get<int32_t>();
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			this->nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				this->nameLocalizations[key] = newValue;
			}
		}
	}

	void ApplicationCommandOptionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			this->nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				this->nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description_localizations") && !(*jsonObjectData)["description_localizations"].is_null()) {
			this->nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["description_localizations"].get<std::map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				this->descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_types") && !(*jsonObjectData)["channel_types"].is_null()) {
			this->channelTypes.clear();
			this->channelTypes.reserve((*jsonObjectData)["channel_types"].size());
			for (auto& value: (*jsonObjectData)["channel_types"]) {
				this->channelTypes.push_back(value);
			}
		}

		if (jsonObjectData->contains("min_value") && !(*jsonObjectData)["min_value"].is_null()) {
			this->minValue = (*jsonObjectData)["min_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_value") && !(*jsonObjectData)["max_value"].is_null()) {
			this->maxValue = (*jsonObjectData)["max_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("required") && !(*jsonObjectData)["required"].is_null()) {
			this->required = (*jsonObjectData)["required"].get<bool>();
		}

		if (jsonObjectData->contains("autocomplete") && !(*jsonObjectData)["autocomplete"].is_null()) {
			this->autocomplete = (*jsonObjectData)["autocomplete"].get<bool>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ApplicationCommandOptionType>();
		}

		if (this->type == ApplicationCommandOptionType::Sub_Command_Group || this->type == ApplicationCommandOptionType::Sub_Command) {
			if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
				this->options.clear();
				for (auto& value: (*jsonObjectData)["options"]) {
					ApplicationCommandOptionData newData{ &value };
					this->options.emplace_back(newData);
				}
			}
		} else {
			if (jsonObjectData->contains("choices") && !(*jsonObjectData)["choices"].is_null()) {
				this->choices.clear();
				for (auto& value: (*jsonObjectData)["choices"]) {
					ApplicationCommandOptionChoiceData newData{ &value };
					this->choices.emplace_back(newData);
				}
			}
		}
	}

	void TypingStartData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			this->userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			this->timestamp = (*jsonObjectData)["timestamp"].get<int32_t>();
		}
	}

	void YouTubeFormatVector::parseObject(const nlohmann::json* jsonObjectData) {
		if (!jsonObjectData->is_null()) {
			if (jsonObjectData->contains("streamingData") && !(*jsonObjectData)["streamingData"].is_null() && (*jsonObjectData)["streamingData"].contains("formats") &&
				!(*jsonObjectData)["streamingData"]["formats"].is_null()) {
				for (auto& value: (*jsonObjectData)["streamingData"]["formats"]) {
					YouTubeFormat newData{};

					if (value.contains("audioQuality") && !value["audioQuality"].is_null()) {
						newData.audioQuality = value["audioQuality"].get<std::string>();
					}

					if (value.contains("averageBitrate") && !value["averageBitrate"].is_null()) {
						newData.averageBitrate = value["averageBitrate"].get<int32_t>();
					}

					if (value.contains("audioSampleRate") && !value["audioSampleRate"].is_null()) {
						newData.audioSampleRate = value["audioSampleRate"].get<std::string>();
					}

					if (value.contains("bitrate") && !value["bitrate"].is_null()) {
						newData.bitrate = value["bitrate"].get<int32_t>();
					}

					if (value.contains("contentLength") && !value["contentLength"].is_null()) {
						newData.contentLength = stoll(value["contentLength"].get<std::string>());
					}

					if (value.contains("fps") && !value["fps"].is_null()) {
						newData.fps = value["fps"].get<int32_t>();
					}

					if (value.contains("height") && !value["height"].is_null()) {
						newData.height = value["height"].get<int32_t>();
					}

					if (value.contains("width") && !value["width"].is_null()) {
						newData.height = value["width"].get<int32_t>();
					}

					if (value.contains("aitags") && !value["aitags"].is_null()) {
						newData.aitags = value["aitags"].get<std::string>();
					}

					if (value.contains("itag") && !value["itag"].is_null()) {
						newData.itag = value["itag"].get<int32_t>();
					}

					if (value.contains("url") && !value["url"].is_null()) {
						newData.downloadUrl = value["url"].get<std::string>();
					}

					if (value.contains("mimeType") && !value["mimeType"].is_null()) {
						newData.mimeType = value["mimeType"].get<std::string>();
					}

					if (value.contains("quality") && !value["quality"].is_null()) {
						newData.quality = value["quality"].get<std::string>();
					}

					if (value.contains("signatureCipher") && !value["signatureCipher"].is_null()) {
						newData.signatureCipher = value["signatureCipher"].get<std::string>();
					} else if (value.contains("cipher") && !value["cipher"].is_null()) {
						newData.signatureCipher = value["cipher"].get<std::string>();
					}

					auto ampersandSpFind = newData.signatureCipher.find("&sp");
					if (ampersandSpFind != std::string::npos) {
						newData.signature = newData.signatureCipher.substr(0, ampersandSpFind);
					}

					auto urlFind = newData.signatureCipher.find("url");
					if (urlFind != std::string::npos) {
						newData.downloadUrl = newData.signatureCipher.substr(urlFind + 4);
					} else if (value.contains("url") && !value["url"].is_null()) {
						newData.downloadUrl = value["url"];
					}

					this->theFormats.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("streamingData") && !(*jsonObjectData)["streamingData"].is_null() && (*jsonObjectData)["streamingData"].contains("adaptiveFormats") &&
				!(*jsonObjectData)["streamingData"]["adaptiveFormats"].is_null()) {
				for (auto& value: (*jsonObjectData)["streamingData"]["adaptiveFormats"]) {
					YouTubeFormat newData{};

					if (value.contains("audioQuality") && !value["audioQuality"].is_null()) {
						newData.audioQuality = value["audioQuality"].get<std::string>();
					}

					if (value.contains("averageBitrate") && !value["averageBitrate"].is_null()) {
						newData.averageBitrate = value["averageBitrate"].get<int32_t>();
					}

					if (value.contains("audioSampleRate") && !value["audioSampleRate"].is_null()) {
						newData.audioSampleRate = value["audioSampleRate"].get<std::string>();
					}

					if (value.contains("bitrate") && !value["bitrate"].is_null()) {
						newData.bitrate = value["bitrate"].get<int32_t>();
					}

					if (value.contains("contentLength") && !value["contentLength"].is_null()) {
						newData.contentLength = stoll(value["contentLength"].get<std::string>());
					}

					if (value.contains("fps") && !value["fps"].is_null()) {
						newData.fps = value["fps"].get<int32_t>();
					}

					if (value.contains("height") && !value["height"].is_null()) {
						newData.height = value["height"].get<int32_t>();
					}

					if (value.contains("width") && !value["width"].is_null()) {
						newData.height = value["width"].get<int32_t>();
					}

					if (value.contains("aitags") && !value["aitags"].is_null()) {
						newData.aitags = value["aitags"].get<std::string>();
					}

					if (value.contains("itag") && !value["itag"].is_null()) {
						newData.itag = value["itag"].get<int32_t>();
					}

					if (value.contains("mimeType") && !value["mimeType"].is_null()) {
						newData.mimeType = value["mimeType"].get<std::string>();
					}

					if (value.contains("quality") && !value["quality"].is_null()) {
						newData.quality = value["quality"].get<std::string>();
					}

					if (value.contains("signatureCipher") && !value["signatureCipher"].is_null()) {
						newData.signatureCipher = value["signatureCipher"].get<std::string>();
					} else if (value.contains("cipher") && !value["cipher"].is_null()) {
						newData.signatureCipher = value["cipher"].get<std::string>();
					}

					auto ampersandSpFind = newData.signatureCipher.find("&sp");
					if (ampersandSpFind != std::string::npos) {
						newData.signature = newData.signatureCipher.substr(0, ampersandSpFind);
					}

					auto urlFind = newData.signatureCipher.find("url");
					if (urlFind != std::string::npos) {
						newData.downloadUrl = newData.signatureCipher.substr(urlFind + 4);
					} else if (value.contains("url") && !value["url"].is_null()) {
						newData.downloadUrl = value["url"];
					}
					this->theFormats.emplace_back(newData);
				}
			}
		}
	}

	void UserCommandInteractionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			this->targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	void MessageCommandInteractionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			this->targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	void ComponentInteractionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("values") && !(*jsonObjectData)["values"].is_null()) {
			this->values.clear();
			for (auto& value: (*jsonObjectData)["values"]) {
				this->values.emplace_back(value);
			}
		}

		if (jsonObjectData->contains("custom_id") && !(*jsonObjectData)["custom_id"].is_null()) {
			this->customId = (*jsonObjectData)["custom_id"].get<std::string>();
		}

		if (jsonObjectData->contains("component_type") && !(*jsonObjectData)["component_type"].is_null()) {
			this->componentType = (*jsonObjectData)["component_type"].get<ComponentType>();
		}
	}

	void ModalInteractionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			this->value = (*jsonObjectData)["components"][0]["components"][0]["value"].get<std::string>();
		}

		if (jsonObjectData->contains("custom_id") && !(*jsonObjectData)["custom_id"].is_null()) {
			this->customId = (*jsonObjectData)["custom_id"].get<std::string>();
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			this->customIdSmall = (*jsonObjectData)["components"][0]["components"][0]["custom_id"].get<std::string>();
		}
	}

	void AllowedMentionsData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("parse") && !(*jsonObjectData)["parse"].is_null()) {
			this->parse.clear();
			for (auto& value: (*jsonObjectData)["parse"]) {
				this->parse.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			this->roles.clear();
			for (auto& value: (*jsonObjectData)["roles"]) {
				this->roles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("users") && !(*jsonObjectData)["users"].is_null()) {
			this->users.clear();
			for (auto& value: (*jsonObjectData)["users"]) {
				this->users.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("replied_user") && !(*jsonObjectData)["replied_user"].is_null()) {
			this->repliedUser = (*jsonObjectData)["replied_user"].get<bool>();
		}
	}

	void SelectOptionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("label") && !(*jsonObjectData)["label"].is_null()) {
			this->label = (*jsonObjectData)["label"].get<std::string>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			this->value = (*jsonObjectData)["value"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["emoji"], this->emoji);
		}

		if (jsonObjectData->contains("default") && !(*jsonObjectData)["default"].is_null()) {
			this->_default = (*jsonObjectData)["default"].get<bool>();
		}
	}

	void ActionRowData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			this->components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ComponentData newData{};
				if (value.contains("type") && !value["type"].is_null()) {
					newData.type = value["type"].get<ComponentType>();
				}

				if (value.contains("custom_id") && !value["custom_id"].is_null()) {
					newData.customId = value["custom_id"].get<std::string>();
				}

				if (value.contains("placeholder") && !value["placeholder"].is_null()) {
					newData.placeholder = value["placeholder"].get<std::string>();
				}

				if (value.contains("disabled") && !value["disabled"].is_null()) {
					newData.disabled = value["disabled"].get<bool>();
				}

				if (value.contains("style") && !value["style"].is_null()) {
					newData.style = value["style"].get<int32_t>();
				}

				if (value.contains("label") && !value["label"].is_null()) {
					newData.label = value["label"].get<std::string>();
				}

				if (value.contains("min_length") && !value["min_length"].is_null()) {
					newData.minLength = value["min_length"].get<int32_t>();
				}

				if (value.contains("max_length") && !value["max_length"].is_null()) {
					newData.maxLength = value["max_length"].get<int32_t>();
				}

				if (value.contains("value") && !value["value"].is_null()) {
					newData.value = value["value"].get<std::string>();
				}

				if (value.contains("required") && !value["required"].is_null()) {
					newData.required = value["required"].get<bool>();
				}

				if (value.contains("emoji") && !value["emoji"].is_null()) {
					DiscordCoreAPI::parseObject(&value["emoji"], newData.emoji);
				}

				if (value.contains("url") && !value["url"].is_null()) {
					newData.url = value["url"].get<std::string>();
				}

				if (value.contains("options") && !value["options"].is_null()) {
					for (auto& value02: value["options"]) {
						SelectOptionData newerData{ &value };
						newData.options.emplace_back(newerData);
					}
				}

				if (value.contains("max_values") && !value["max_values"].is_null()) {
					newData.maxValues = value["max_values"].get<int32_t>();
				}

				if (value.contains("min_values") && !value["min_values"].is_null()) {
					newData.minValues = value["min_values"].get<int32_t>();
				}

				this->components.emplace_back(newData);
			}
		}
	}

	void ApplicationCommandData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("default_member_permissions") && !(*jsonObjectData)["default_member_permissions"].is_null()) {
			this->defaultMemberPermissions = (*jsonObjectData)["default_member_permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("dm_permission") && !(*jsonObjectData)["dm_permission"].is_null()) {
			this->dmPermission = (*jsonObjectData)["dm_permission"].get<bool>();
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			this->version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			this->nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				this->nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description_localizations") && !(*jsonObjectData)["description_localizations"].is_null()) {
			this->nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["description_localizations"].get<std::map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				this->descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			this->version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			this->options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandOptionData newData{ &value };
				this->options.emplace_back(newData);
			}
		}
	}

	void ChannelMentionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	void ChannelPinsUpdateEventData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			this->lastPinTimestamp = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["last_pin_timestamp"].get<std::string>());
		}
	}

	void ThreadListSyncData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_ids") && !(*jsonObjectData)["channel_ids"].is_null()) {
			this->channelIds.clear();
			for (auto& value: (*jsonObjectData)["channel_ids"]) {
				this->channelIds.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			this->members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			this->threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->threads.push_back(newData);
			}
		}
	}

	void ThreadMembersUpdateData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			this->memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("added_members") && !(*jsonObjectData)["added_members"].is_null()) {
			this->addedMembers.clear();
			for (auto& value: (*jsonObjectData)["added_members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->addedMembers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("removed_member_ids") && !(*jsonObjectData)["removed_member_ids"].is_null()) {
			this->removedMemberIds.clear();
			for (auto& value: (*jsonObjectData)["removed_member_ids"]) {
				this->removedMemberIds.emplace_back(value);
			}
		}
	}

	void MessageInteractionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<InteractionType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}
	}

	void StickerItemData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			this->formatType = (*jsonObjectData)["format_type"].get<StickerItemType>();
		}
	}

	void MessageDataOld::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			this->content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["author"], this->author);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			this->timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("edited_timestamp") && !(*jsonObjectData)["edited_timestamp"].is_null()) {
			this->editedTimestamp = (*jsonObjectData)["edited_timestamp"].get<std::string>();


			if (jsonObjectData->contains("tts") && !(*jsonObjectData)["tts"].is_null()) {
				this->tts = (*jsonObjectData)["tts"].get<bool>();
			}

			if (jsonObjectData->contains("mention_everyone") && !(*jsonObjectData)["mention_everyone"].is_null()) {
				this->mentionEveryone = (*jsonObjectData)["mention_everyone"].get<bool>();
			}

			if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
				this->mentions.clear();
				for (auto& value: (*jsonObjectData)["mentions"]) {
					UserData newData{};
					DiscordCoreAPI::parseObject(&value, newData);
					this->mentions.push_back(newData);
				}
			}

			if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
				this->mentionRoles.clear();
				for (auto& value: (*jsonObjectData)["mention_roles"]) {
					this->mentionRoles.emplace_back(value.get<std::string>());
				}
			}

			if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
				this->mentionChannels.clear();
				for (auto& value: (*jsonObjectData)["mention_channels"]) {
					ChannelMentionData newData{ &value };
					this->mentionChannels.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
				this->attachments.clear();
				for (auto& value: (*jsonObjectData)["attachments"]) {
					AttachmentData newData{};
					DiscordCoreAPI::parseObject(&value, newData);
					this->attachments.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
				this->embeds.clear();
				for (auto& value: (*jsonObjectData)["embeds"]) {
					EmbedData newData{};
					DiscordCoreAPI::parseObject(&value, newData);
					this->embeds.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
				this->reactions.clear();
				for (auto& value: (*jsonObjectData)["reactions"]) {
					ReactionData newData{ &value };
					this->reactions.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
				this->nonce = (*jsonObjectData)["nonce"].get<std::string>();
			}

			if (jsonObjectData->contains("pinned") && !(*jsonObjectData)["pinned"].is_null()) {
				this->pinned = (*jsonObjectData)["pinned"].get<bool>();
			}

			if (jsonObjectData->contains("webhook_id") && !(*jsonObjectData)["webhook_id"].is_null()) {
				this->webHookId = stoull((*jsonObjectData)["webhook_id"].get<std::string>());
			}

			if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
				this->type = (*jsonObjectData)["type"].get<MessageType>();
			}

			if (jsonObjectData->contains("activity") && !(*jsonObjectData)["activity"].is_null()) {
				this->activity = &(*jsonObjectData)["activity"];
			}

			if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
				DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], this->application);
			}

			if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
				this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
			}

			if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
				this->messageReference = &(*jsonObjectData)["message_reference"];
			}

			if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
				this->flags = (*jsonObjectData)["flags"].get<int32_t>();
			}

			if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
				this->stickerItems.clear();
				for (auto& value: (*jsonObjectData)["sticker_items"]) {
					StickerItemData newData{ &value };
					this->stickerItems.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
				this->stickers.clear();
				for (auto& value: (*jsonObjectData)["stickers"]) {
					StickerData newData{ &value };
					this->stickers.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
				this->interaction = &(*jsonObjectData)["interaction"];
			}

			if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
				this->components.clear();
				for (auto& value: (*jsonObjectData)["components"]) {
					ActionRowData newData{ &value };
					this->components.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
				DiscordCoreAPI::parseObject(&(*jsonObjectData)["thread"], this->thread);
			}
		}
	}

	void MessageData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			this->content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["author"], this->author);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			this->timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("edited_timestamp") && !(*jsonObjectData)["edited_timestamp"].is_null()) {
			this->editedTimestamp = (*jsonObjectData)["edited_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("tts") && !(*jsonObjectData)["tts"].is_null()) {
			this->tts = (*jsonObjectData)["tts"].get<bool>();
		}

		if (jsonObjectData->contains("mention_everyone") && !(*jsonObjectData)["mention_everyone"].is_null()) {
			this->mentionEveryone = (*jsonObjectData)["mention_everyone"].get<bool>();
		}

		if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
			this->mentions.clear();
			for (auto& value: (*jsonObjectData)["mentions"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->mentions.push_back(newData);
			}
		}

		if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
			this->mentionRoles.clear();
			for (auto& value: (*jsonObjectData)["mention_roles"]) {
				this->mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
			this->mentionChannels.clear();
			for (auto& value: (*jsonObjectData)["mention_channels"]) {
				ChannelMentionData newData{ &value };
				this->mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			this->attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			this->embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			this->reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{ &value };
				this->reactions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			this->nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		if (jsonObjectData->contains("pinned") && !(*jsonObjectData)["pinned"].is_null()) {
			this->pinned = (*jsonObjectData)["pinned"].get<bool>();
		}

		if (jsonObjectData->contains("webhook_id") && !(*jsonObjectData)["webhook_id"].is_null()) {
			this->webHookId = stoull((*jsonObjectData)["webhook_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<MessageType>();
		}

		if (jsonObjectData->contains("activity") && !(*jsonObjectData)["activity"].is_null()) {
			this->activity = &(*jsonObjectData)["activity"];
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], this->application);
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
			this->messageReference = &(*jsonObjectData)["message_reference"];
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			this->flags = (*jsonObjectData)["flags"].get<int32_t>();
		}

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			this->stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{ &value };
				this->stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			this->stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{ &value };
				this->stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
			this->interaction = &(*jsonObjectData)["interaction"];
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			this->components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{ &value };
				this->components.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["thread"], this->thread);
		}
	}

	void StickerPackData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			this->stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{ &value };
				this->stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("cover_sticker_id") && !(*jsonObjectData)["cover_sticker_id"].is_null()) {
			this->coverStickerId = (*jsonObjectData)["cover_sticker_id"].get<std::string>();
		}

		if (jsonObjectData->contains("banner_asset_id") && !(*jsonObjectData)["banner_asset_id"].is_null()) {
			this->bannerAssetId = (*jsonObjectData)["banner_asset_id"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			this->description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->Id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("sku_id") && !(*jsonObjectData)["sku_id"].is_null()) {
			this->skuId = (*jsonObjectData)["sku_id"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, StickerPackDataVector& theData) {
		theData.theStickerPackDatas.reserve(jsonObjectData->size());		
		for (auto& value: *jsonObjectData) {
			StickerPackData newData{ &value };
			theData.theStickerPackDatas.push_back(newData);
		}
		theData.theStickerPackDatas.shrink_to_fit();
	}

	void ConnectionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("show_activity") && !(*jsonObjectData)["show_activity"].is_null()) {
			this->showActivity = (*jsonObjectData)["show_activity"].get<bool>();
		}

		if (jsonObjectData->contains("friend_sync") && !(*jsonObjectData)["friend_sync"].is_null()) {
			this->friendSync = (*jsonObjectData)["friend_sync"].get<bool>();
		}

		if (jsonObjectData->contains("revoked") && !(*jsonObjectData)["revoked"].is_null()) {
			this->revoked = (*jsonObjectData)["revoked"].get<bool>();
		}

		if (jsonObjectData->contains("verified") && !(*jsonObjectData)["verified"].is_null()) {
			this->verified = (*jsonObjectData)["verified"].get<bool>();
		}

		if (jsonObjectData->contains("visibility") && !(*jsonObjectData)["visibility"].is_null()) {
			this->visibility = (*jsonObjectData)["visibility"].get<ConnectionVisibilityTypes>();
		}

		if (jsonObjectData->contains("integrations") && !(*jsonObjectData)["integrations"].is_null()) {
			this->integrations.clear();
			for (auto& value: (*jsonObjectData)["integrations"]) {
				IntegrationData newData{ &value };
				this->integrations.emplace_back(newData);
			}
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, ConnectionDataVector&theData) {
		theData.theConnectionDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ConnectionData newData{ &value };
			theData.theConnectionDatas.push_back(newData);
		}
		theData.theConnectionDatas.shrink_to_fit();
	}

	void ApplicationCommandInteractionDataOption::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ApplicationCommandOptionType>();
		}

		if (jsonObjectData->contains("focused") && !(*jsonObjectData)["focused"].is_null()) {
			this->focused = (*jsonObjectData)["focused"].get<bool>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			if ((*jsonObjectData)["value"].is_boolean()) {
				this->valueBool = (*jsonObjectData)["value"].get<bool>();
			}
			if ((*jsonObjectData)["value"].is_string()) {
				this->valueString = (*jsonObjectData)["value"].get<std::string>();
			}
			if ((*jsonObjectData)["value"].is_number()) {
				this->valueInt = (*jsonObjectData)["value"].get<int32_t>();
			}
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			this->options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandInteractionDataOption newData{ &value };
				this->options.emplace_back(newData);
			}
		}
	}

	void ApplicationCommandInteractionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			this->options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandInteractionDataOption newData{ &value };
				this->options.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("resolved") && !(*jsonObjectData)["resolved"].is_null()) {
			auto& value = (*jsonObjectData)["resolved"];

			if (value.contains("attachments") && !value["attachments"].is_null()) {
				this->resolved.attachments.clear();
				auto newMap = value["attachments"].get<std::map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					AttachmentData newData{};
					DiscordCoreAPI::parseObject(&newValue, newData);
					this->resolved.attachments[stoull(key)] = newData;
				}
			}

			if (value.contains("users") && !value["users"].is_null()) {
				this->resolved.users.clear();
				auto newMap = value["users"].get<std::map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					UserData newData{};
					DiscordCoreAPI::parseObject(&value, newData);
					this->resolved.users[newData.id] = newData;
				}
			}
			if (value.contains("channels") && !value["channels"].is_null()) {
				this->resolved.channels.clear();
				auto newMap = value["channels"].get<std::map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					ChannelData newData{};
					DiscordCoreAPI::parseObject(&value, newData);
					this->resolved.channels[newData.id] = newData;
				}
			}
			if (value.contains("roles") && !value["roles"].is_null()) {
				this->resolved.roles.clear();
				auto newMap = value["roles"].get<std::map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					RoleData newData{};
					DiscordCoreAPI::parseObject(&value, newData);
					this->resolved.roles[newData.id] = newData;
				}
			}
			if (value.contains("members") && !value["members"].is_null()) {
				this->resolved.members.clear();
				auto newMap = value["members"].get<std::map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					GuildMemberData theData{};
					DiscordCoreAPI::parseObject(&newValue, theData);
					this->resolved.members[stoull(key)] = theData;
				}
			}
			if (value.contains("messages") && !value["messages"].is_null()) {
				this->resolved.messages.clear();
				auto newMap = value["messages"].get<std::map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					MessageData newData{ &newValue };
					this->resolved.messages[stoull(key)] = newData;
				}
			}
		}
	}

	void InteractionDataData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->applicationCommandData = jsonObjectData;
		}

		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			this->messageInteractionData = jsonObjectData;
			this->userInteractionData = jsonObjectData;
		}

		if (jsonObjectData->contains("component_type") && !(*jsonObjectData)["component_type"].is_null()) {
			this->componentData = jsonObjectData;
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			this->modalData = jsonObjectData;
		}
	}

	void InteractionData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("data") && !(*jsonObjectData)["data"].is_null()) {
			this->data = &(*jsonObjectData)["data"];
			this->rawData = (*jsonObjectData)["data"];
		}

		if (jsonObjectData->contains("app_permissions") && !(*jsonObjectData)["app_permissions"].is_null()) {
			this->appPermissions = (*jsonObjectData)["app_permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			this->type = (*jsonObjectData)["type"].get<InteractionType>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			this->token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], this->member);
			this->user.id = this->member.id;
			this->user.userName = this->member.getUserData().userName;
		} else if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], this->user);
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			this->channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("locale") && !(*jsonObjectData)["locale"].is_null()) {
			this->locale = (*jsonObjectData)["locale"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_locale") && !(*jsonObjectData)["guild_locale"].is_null()) {
			this->guildLocale = (*jsonObjectData)["guild_locale"].get<std::string>();
		}

		if (jsonObjectData->contains("message") && !(*jsonObjectData)["message"].is_null()) {
			this->message = &(*jsonObjectData)["message"];
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			this->version = (*jsonObjectData)["version"].get<int32_t>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			this->id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			this->applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, SessionStartData& theData) {
		theData.maxConcurrency = getUint32(&(*jsonObjectData), "max_concurrency");

		theData.remaining = getUint32(&(*jsonObjectData), "remaining");

		theData.resetAfter = getUint32(&(*jsonObjectData), "reset_after");

		theData.total = getUint32(&(*jsonObjectData), "total");
	}

	void parseObject(const nlohmann::json* jsonObjectData, GatewayBotData& theData) {
		DiscordCoreAPI::parseObject(&(*jsonObjectData)["session_start_limit"], theData.sessionStartLimit);

		theData.shards = getUint32(&(*jsonObjectData), "shards");

		theData.url = getString(&(*jsonObjectData), "url");
	}

	void GuildEmojisUpdateEventData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emojis") && !(*jsonObjectData)["emojis"].is_null()) {
			this->emojis.clear();
			for (auto& value: (*jsonObjectData)["emojis"]) {
				EmojiData newData{};
				DiscordCoreAPI::parseObject(&value, newData);
				this->emojis.emplace_back(newData);
			}
		}
	}

	void GuildStickersUpdateEventData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			this->stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{ &value };
				this->stickers.emplace_back(newData);
			}
		}
	}

	void GuildMembersChunkEventData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			this->guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			this->nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		if (jsonObjectData->contains("chunk_index") && !(*jsonObjectData)["chunk_index"].is_null()) {
			this->chunkIndex = (*jsonObjectData)["chunk_index"].get<int32_t>();
		}

		if (jsonObjectData->contains("chunk_count") && !(*jsonObjectData)["chunk_count"].is_null()) {
			this->chunkCount = (*jsonObjectData)["chunk_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("presences") && !(*jsonObjectData)["presences"].is_null()) {
			this->presences.clear();
			for (auto& value: (*jsonObjectData)["presences"]) {
				PresenceUpdateData newData{ &value };
				this->presences.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("not_found") && !(*jsonObjectData)["not_found"].is_null()) {
			this->notFound.clear();
			for (auto& value: (*jsonObjectData)["not_found"]) {
				this->notFound.emplace_back(value);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			this->members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				GuildMemberData theData{};
				DiscordCoreAPI::parseObject(&value, theData);
				this->members.emplace_back(theData);
			}
		}
	}

	void CommandData::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			this->optionsArgs.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				if (value.contains("type") && value["type"] == 1) {
					if (value.contains("name")) {
						this->subCommandName = value["name"];
					}
				} else if (value.contains("type") && value["type"] == 2) {
					if (value.contains("name")) {
						this->subCommandGroupName = value["name"];
					}
				}
				if (value.contains("options")) {
					parseObject(&value);
				}
				if (value.contains("value") && !value["value"].is_null()) {
					auto& newValueNew = value["value"];
					if (newValueNew.is_string()) {
						this->optionsArgs.emplace_back(newValueNew);
					} else if (newValueNew.is_number()) {
						this->optionsArgs.emplace_back(std::to_string(newValueNew.get<int64_t>()));
					} else if (newValueNew.is_boolean()) {
						this->optionsArgs.emplace_back(std::to_string(newValueNew.get<bool>()));
					}
				}
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			this->commandName = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	void Song::parseObject(const nlohmann::json* jsonObjectData) {
		if (jsonObjectData->contains("lengthText") && !(*jsonObjectData)["lengthText"].is_null()) {
			this->duration = (*jsonObjectData)["lengthText"]["accessibility"]["accessibilityData"]["label"].get<std::string>();
		}

		if (jsonObjectData->contains("detailedMetadataSnippets") && !(*jsonObjectData)["detailedMetadataSnippets"].is_null()) {
			for (auto& value: (*jsonObjectData)["detailedMetadataSnippets"][0]["snippetText"]["runs"]) {
				std::string newString = value["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				this->description = utf8MakeValid(newString);
			}
		}

		if (jsonObjectData->contains("thumbnail") && !(*jsonObjectData)["thumbnail"].is_null()) {
			this->thumbnailUrl = (*jsonObjectData)["thumbnail"]["thumbnails"][0]["url"].get<std::string>();
		}

		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null()) {
			if ((*jsonObjectData)["title"].contains("runs")) {
				std::string newString = (*jsonObjectData)["title"]["runs"][0]["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				this->songTitle = utf8MakeValid(newString);
				;
			} else if ((*jsonObjectData)["title"].contains("simpleText")) {
				std::string newString = (*jsonObjectData)["title"]["simpleText"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				this->songTitle = utf8MakeValid(newString);
			}
		}
		if (jsonObjectData->contains("videoId") && !(*jsonObjectData)["videoId"].is_null()) {
			this->songId = (*jsonObjectData)["videoId"].get<std::string>();
		}

		if (jsonObjectData->contains("track_authorization") && !(*jsonObjectData)["track_authorization"].is_null()) {
			this->trackAuthorization = (*jsonObjectData)["track_authorization"].get<std::string>();
		}

		if (jsonObjectData->contains("media") && !(*jsonObjectData)["media"].is_null()) {
			bool isItFound{ false };
			for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
				if (value["preset"] == "opus_0_0") {
					isItFound = true;
					this->firstDownloadUrl = value["url"].get<std::string>();
					this->songId = value["url"].get<std::string>();
					this->doWeGetSaved = true;
				}
			}
			bool isItFound2{ false };
			if (!isItFound) {
				for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
					if (value["preset"] == "mp3_0_0") {
						this->firstDownloadUrl = value["url"].get<std::string>();
						this->songId = value["url"].get<std::string>();
						isItFound2 = true;
					}
				}
			}
			if (!isItFound2 && !isItFound) {
				for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
					this->firstDownloadUrl = value["url"].get<std::string>();
					this->songId = value["url"].get<std::string>();
				}
			}
		}

		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null() && !(*jsonObjectData)["title"].is_object()) {
			std::string newString = (*jsonObjectData)["title"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->songTitle = utf8MakeValid(newString);
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			std::string newString = (*jsonObjectData)["description"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->description = utf8MakeValid(newString);
			this->description += "...";
		}

		if (jsonObjectData->contains("artwork_url") && !(*jsonObjectData)["artwork_url"].is_null()) {
			this->thumbnailUrl = (*jsonObjectData)["artwork_url"].get<std::string>();
		} else if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			if ((*jsonObjectData)["user"].contains("avatar_url") && !(*jsonObjectData)["user"]["avatar_url"].is_null()) {
				this->thumbnailUrl = (*jsonObjectData)["user"]["avatar_url"].get<std::string>();
			}
		}

		if (jsonObjectData->contains("duration") && !(*jsonObjectData)["duration"].is_null()) {
			TimeStamp<std::chrono::milliseconds> theTimeStamp{};
			this->duration = theTimeStamp.convertMsToDurationString((*jsonObjectData)["duration"].get<int32_t>());
		}

		if (jsonObjectData->contains("permalink_url") && !(*jsonObjectData)["permalink_url"].is_null()) {
			this->viewUrl = (*jsonObjectData)["permalink_url"].get<std::string>();
		}
	}

	void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventVector& theData) {
		theData.theGuildScheduledEvents.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildScheduledEvent newData{};
			newData.parseObject(&value);
			theData.theGuildScheduledEvents.push_back(newData);
		}
		theData.theGuildScheduledEvents.shrink_to_fit();
	}

	void GuildScheduledEventDataVector::parseObject(const nlohmann::json* jsonObjectData) {
		this->theGuildScheduledEventDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildScheduledEventData newData{};
			newData.parseObject(&value);
			this->theGuildScheduledEventDatas.push_back(newData);
		}
		this->theGuildScheduledEventDatas.shrink_to_fit();
	}
};
