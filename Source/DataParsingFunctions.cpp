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
/// DataParsingFunctions.cpp - Source for the functions that will parse the Discord data into C++ types.
/// Apr 21, 2021
/// https://discordcoreapi.com
/// \file DataParsingFunctions.cpp

#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>

namespace DiscordCoreAPI {

	void AutoModerationRule::parseObjectReal(const nlohmann::json& jsonObjectData, AutoModerationRule* pDataStructure) {
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

	void AutoModerationRuleVector::parseObjectReal(const nlohmann::json& jsonObjectData, AutoModerationRuleVector* pDataStructure) {
		pDataStructure->theAutoModerationRules.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::AutoModerationRule newData{ value };
			pDataStructure->theAutoModerationRules.push_back(newData);
		}
		pDataStructure->theAutoModerationRules.shrink_to_fit();
	}

	void Guild::parseObjectReal(const nlohmann::json& jsonObjectData, Guild* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
			pDataStructure->afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			std::string iconUrlString = "https://cdn.discordapp.com/";
			iconUrlString += "icons/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["icon"].get<std::string>() + ".png";
			pDataStructure->icon = iconUrlString;
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("icon_hash") && !jsonObjectData["icon_hash"].is_null()) {
			pDataStructure->iconHash = jsonObjectData["icon_hash"].get<std::string>();
		}

		if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
			std::string iconUrlString = "https://cdn.discordapp.com/";
			iconUrlString += "splashes/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["splash"].get<std::string>() + ".png";
			pDataStructure->splash = iconUrlString;
		}

		if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
			std::string discordSplashUrlString = "https://cdn.discordapp.com/";
			discordSplashUrlString += "discovery-splashes/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["discovery_splash"].get<std::string>() + ".png";
			pDataStructure->discoverySplash = discordSplashUrlString;
		}

		if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
			pDataStructure->flags =
				DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Owner, jsonObjectData["owner"].get<bool>());
		}

		if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
			pDataStructure->ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
		}

		if (jsonObjectData.contains("preferred_locale") && !jsonObjectData["preferred_locale"].is_null()) {
			pDataStructure->preferredLocale = jsonObjectData["preferred_locale"].get<std::string>();
		}

		if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData["public_updates_channel_id"].is_null()) {
			pDataStructure->publicUpdatesChannelId = jsonObjectData["public_updates_channel_id"].get<std::string>();
		}

		if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData["vanity_url_code"].is_null()) {
			pDataStructure->vanityUrlCode = jsonObjectData["vanity_url_code"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("banner") && !jsonObjectData["banner"].is_null()) {
			std::string guildBannerUrl = "https://cdn.discordapp.com/";
			guildBannerUrl += "banners/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["banner"].get<std::string>() + ".png";
			pDataStructure->banner = guildBannerUrl;
		}

		if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData["rule_Channel_id"].is_null()) {
			pDataStructure->rulesChannelId = jsonObjectData["rule_Channel_id"].get<std::string>();
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
			pDataStructure->joinedAt = jsonObjectData["joined_at"].get<std::string>();
		}

		if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData["widget_channel_id"].is_null()) {
			pDataStructure->widgetChannelId = jsonObjectData["widget_channel_id"].get<std::string>();
		}

		if (jsonObjectData.contains("system_channel_id") && !jsonObjectData["system_channel_id"].is_null()) {
			pDataStructure->systemChannelId = jsonObjectData["system_channel_id"].get<std::string>();
		}

		if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
			pDataStructure->region = jsonObjectData["region"].get<std::string>();
		}

		if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
			pDataStructure->afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
		}

		if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
			pDataStructure->region = jsonObjectData["region"].get<std::string>();
		}

		if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
			for (auto& value: jsonObjectData["features"].get<std::vector<std::string>>()) {
				pDataStructure->features.push_back(value);
			}
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
			for (auto& value: jsonObjectData["roles"]) {
				DiscordCoreAPI::RoleData newData{ value };
				pDataStructure->roles.push_back(newData.id);
				DiscordCoreAPI::Roles::insertRole(newData);
			}
		}

		if (jsonObjectData.contains("afk_timeout") && !jsonObjectData["afk_timeout"].is_null()) {
			pDataStructure->afkTimeOut = jsonObjectData["afk_timeout"].get<DiscordCoreAPI::AfkTimeOutDurations>();
		}

		if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
			pDataStructure->flags =
				DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Owner, jsonObjectData["owner"].get<bool>());
		}

		if (jsonObjectData.contains("widget_enabled") && !jsonObjectData["widget_enabled"].is_null()) {
			pDataStructure->flags = DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::WidgetEnabled,
				jsonObjectData["widget_enabled"].get<bool>());
		}

		if (jsonObjectData.contains("verification_level") && !jsonObjectData["verification_level"].is_null()) {
			pDataStructure->verificationLevel = jsonObjectData["verification_level"].get<DiscordCoreAPI::VerificationLevel>();
		}

		if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData["default_message_notification_level"].is_null()) {
			pDataStructure->defaultMessageNotifications = jsonObjectData["default_message_notification_level"].get<DiscordCoreAPI::DefaultMessageNotificationLevel>();
		}

		if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData["explicit_content_filter_level"].is_null()) {
			pDataStructure->explicitContentFilter = jsonObjectData["explicit_content_filter_level"].get<DiscordCoreAPI::ExplicitContentFilterLevel>();
		}

		if (jsonObjectData.contains("mfa_level") && !jsonObjectData["mfa_level"].is_null()) {
			pDataStructure->mfaLevel = jsonObjectData["mfa_level"].get<DiscordCoreAPI::MFALevel>();
		}

		if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData["system_channel_flags"].is_null()) {
			pDataStructure->systemChannelFlags = jsonObjectData["system_channel_flags"].get<DiscordCoreAPI::SystemChannelFlags>();
		}

		if (jsonObjectData.contains("large") && !jsonObjectData["large"].is_null()) {
			pDataStructure->flags =
				DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Large, jsonObjectData["large"].get<bool>());
		}

		if (jsonObjectData.contains("unavailable") && !jsonObjectData["unavailable"].is_null()) {
			pDataStructure->flags = DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Unavailable,
				jsonObjectData["unavailable"].get<bool>());
		}

		if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
			pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
			pDataStructure->voiceStates.clear();
			for (auto& value: jsonObjectData["voice_states"]) {
				DiscordCoreAPI::VoiceStateData newData{ value };
				uint64_t userId = newData.userId;
				pDataStructure->voiceStates[userId] = newData;
			}
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			for (auto& value: jsonObjectData["members"]) {
				DiscordCoreAPI::GuildMemberData newData{ value };
				newData.guildId = pDataStructure->id;
				pDataStructure->members.push_back(newData.id);
				DiscordCoreAPI::GuildMembers::insertGuildMember(newData);
			}
		}

		if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
			for (auto& value: jsonObjectData["channels"]) {
				DiscordCoreAPI::ChannelData newData{ value };
				newData.guildId = pDataStructure->id;
				pDataStructure->channels.push_back(newData.id);
				DiscordCoreAPI::Channels::insertChannel(newData);
			}
		}

		if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
			pDataStructure->presences.clear();
			for (auto& value: jsonObjectData["presences"]) {
				DiscordCoreAPI::PresenceUpdateData newData{ value };
				uint64_t presenceId = newData.user.id;
				pDataStructure->presences[presenceId] = newData;
			}
		}

		if (jsonObjectData.contains("max_presences") && !jsonObjectData["max_presences"].is_null()) {
			pDataStructure->maxPresences = jsonObjectData["max_presences"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_members") && !jsonObjectData["max_members"].is_null()) {
			pDataStructure->maxMembers = jsonObjectData["max_members"].get<int32_t>();
		}

		if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData["premium_subscription_count"].is_null()) {
			pDataStructure->premiumSubscriptionCount = jsonObjectData["premium_subscription_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("premium_tier") && !jsonObjectData["premium_tier"].is_null()) {
			pDataStructure->premiumTier = jsonObjectData["premium_tier"].get<DiscordCoreAPI::PremiumTier>();
		}

		if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData["max_video_channel_users"].is_null()) {
			pDataStructure->maxVideoChannelUsers = jsonObjectData["max_video_channel_users"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
			pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
			pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("welcome_screen") && !jsonObjectData["welcome_screen"].is_null()) {
			pDataStructure->welcomeScreen = jsonObjectData["welcome_screen"];
		}

		if (jsonObjectData.contains("nsfw_level") && !jsonObjectData["nsfw_level"].is_null()) {
			pDataStructure->nsfwLevel = jsonObjectData["nsfw_level"].get<DiscordCoreAPI::GuildNSFWLevel>();
		}
	}

	void GuildWidgetImageData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildWidgetImageData* pDataStructure) {
		if (jsonObjectData.contains("widget_image") && !jsonObjectData["widget_image"].is_null()) {
			pDataStructure->url = jsonObjectData["widget_image"].get<bool>();
		}
	}

	void GuildMember::parseObjectReal(const nlohmann::json& jsonObjectData, GuildMember* pDataStructure) {
		if (jsonObjectData.contains("communication_disabled_until") && !jsonObjectData["communication_disabled_until"].is_null()) {
			pDataStructure->communicationDisabledUntil = jsonObjectData["communication_disabled_until"];
		}

		if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
			for (auto& value: jsonObjectData["roles"].get<std::vector<std::string>>()) {
				pDataStructure->roles.push_back(stoull(value));
			}
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			pDataStructure->flags = jsonObjectData["flags"].get<int8_t>();
		}

		if (jsonObjectData.contains("premium_since") && !jsonObjectData["premium_since"].is_null()) {
			pDataStructure->premiumSince = jsonObjectData["premium_since"].get<std::string>();
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
			pDataStructure->joinedAt = jsonObjectData["joined_at"].get<std::string>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
			pDataStructure->avatar = jsonObjectData["avatar"].get<std::string>();
		}

		if (jsonObjectData.contains("nick") && !jsonObjectData["nick"].is_null()) {
			pDataStructure->nick = jsonObjectData["nick"].get<std::string>();
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			User theUser{ jsonObjectData["user"] };
			Users::insertUser(theUser);
			pDataStructure->id = theUser.id;
			pDataStructure->userAvatar = theUser.avatar;
			pDataStructure->userName = theUser.userName;
		}

		if (jsonObjectData.contains("pending") && !jsonObjectData["pending"].is_null()) {
			pDataStructure->flags = setBool<int8_t, GuildMemberFlags>(pDataStructure->flags, GuildMemberFlags::Pending, jsonObjectData["pending"].get<bool>());
		}

		if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
			pDataStructure->flags = setBool<int8_t, GuildMemberFlags>(pDataStructure->flags, GuildMemberFlags::Mute, jsonObjectData["mute"].get<bool>());
		}

		if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
			pDataStructure->flags = setBool<int8_t, GuildMemberFlags>(pDataStructure->flags, GuildMemberFlags::Deaf, jsonObjectData["deaf"].get<bool>());
		}
	}

	void Thread::parseObjectReal(const nlohmann::json& jsonObjectData, Thread* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			if (jsonObjectData["id"].is_string()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			} else {
				pDataStructure->id = jsonObjectData["id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<ChannelType>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
			pDataStructure->position = jsonObjectData["position"].get<int32_t>();
		}

		if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
			pDataStructure->permissionOverwrites.clear();
			for (auto& value: jsonObjectData["permission_overwrites"]) {
				OverWriteData newData{ value };
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

		if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
			pDataStructure->flags =
				setBool<int8_t, ChannelFlags>(pDataStructure->flags, ChannelFlags::NSFW, jsonObjectData["nsfw"].get<bool>());
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
				UserData newData{ value };
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

	void Role::parseObjectReal(const nlohmann::json& jsonObjectData, Role* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			if (jsonObjectData["id"].is_string()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			} else {
				pDataStructure->id = jsonObjectData["id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			pDataStructure->icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("unicode_emoji") && !jsonObjectData["unicode_emoji"].is_null()) {
			std::stringstream theStream{};
			theStream << jsonObjectData["unicode_emoji"] << std::endl;
			for (auto& value: theStream.str()) {
				pDataStructure->unicodeEmoji.push_back(value);
			}
			pDataStructure->unicodeEmoji = static_cast<std::string>(pDataStructure->unicodeEmoji).substr(1, pDataStructure->unicodeEmoji.size() - 3);
		}

		if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
			pDataStructure->color = jsonObjectData["color"].get<int32_t>();
		}

		if (jsonObjectData.contains("hoist") && !jsonObjectData["hoist"].is_null()) {
			pDataStructure->flags =
				setBool<int8_t, RoleFlags>(pDataStructure->flags, RoleFlags::Hoist, jsonObjectData["hoist"].get<bool>());
		}

		if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
			pDataStructure->position = jsonObjectData["position"].get<int32_t>();
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
			pDataStructure->flags =
				setBool<int8_t, RoleFlags>(pDataStructure->flags, RoleFlags::Managed, jsonObjectData["managed"].get<bool>());
		}

		if (jsonObjectData.contains("mentionable") && !jsonObjectData["mentionable"].is_null()) {
			pDataStructure->flags =
				setBool<int8_t, RoleFlags>(pDataStructure->flags, RoleFlags::Mentionable, jsonObjectData["mentionable"].get<bool>());
		}

		if (jsonObjectData.contains("tags") && !jsonObjectData["tags"].is_null()) {
			pDataStructure->tags = jsonObjectData["tags"];
		}
	}

	void Sticker::parseObjectReal(const nlohmann::json& jsonObjectData, Sticker* pDataStructure) {
		if (jsonObjectData.contains("asset") && !jsonObjectData["asset"].is_null()) {
			pDataStructure->asset = jsonObjectData["asset"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
			pDataStructure->formatType = jsonObjectData["format_type"].get<StickerFormatType>();
		}

		if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
			pDataStructure->stickerFlags = setBool<int32_t, StickerFlags>(pDataStructure->stickerFlags, StickerFlags::Available,
				jsonObjectData["available"].get<bool>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("pack_id") && !jsonObjectData["pack_id"].is_null()) {
			pDataStructure->packId = jsonObjectData["pack_id"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<StickerType>();
		}

		if (jsonObjectData.contains("sort_value") && !jsonObjectData["sort_value"].is_null()) {
			pDataStructure->sortValue = jsonObjectData["sort_value"].get<int32_t>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			pDataStructure->user = jsonObjectData["user"];
		}
	}

	void GuildPreviewData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildPreviewData* pDataStructure) {
		if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
			pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
			pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
			pDataStructure->discoverySplash = jsonObjectData["discovery_splash"].get<std::string>();
		}

		if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
			pDataStructure->emojis.clear();
			pDataStructure->emojis.reserve(jsonObjectData["emojis"].size());
			for (auto& value: jsonObjectData["emojis"]) {
				EmojiData newData{ value };
				pDataStructure->emojis.push_back(newData);
			}
			pDataStructure->emojis.shrink_to_fit();
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			pDataStructure->emojis.clear();
			pDataStructure->emojis.reserve(jsonObjectData["stickers"].size());
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{ value };
				pDataStructure->stickers.push_back(newData);
			}
			pDataStructure->stickers.shrink_to_fit();
		}

		if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
			pDataStructure->features = jsonObjectData["features"].get<std::vector<std::string>>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
			pDataStructure->splash = jsonObjectData["splash"].get<std::string>();
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			pDataStructure->icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}
	}

	void ClientStatusData::parseObjectReal(const nlohmann::json& jsonObjectData, ClientStatusData* pDataStructure) {
		if (jsonObjectData.contains("desktop") && !jsonObjectData["desktop"].is_null()) {
			pDataStructure->desktop = jsonObjectData["desktop"].get<std::string>();
		}

		if (jsonObjectData.contains("mobile") && !jsonObjectData["mobile"].is_null()) {
			pDataStructure->mobile = jsonObjectData["mobile"].get<std::string>();
		}

		if (jsonObjectData.contains("web") && !jsonObjectData["web"].is_null()) {
			pDataStructure->web = jsonObjectData["web"].get<std::string>();
		}
	}

	void StageInstance::parseObjectReal(const nlohmann::json& jsonObjectData, StageInstance* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
			pDataStructure->topic = jsonObjectData["topic"].get<std::string>();
		}

		if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
			pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData["discoverable_disabled"].is_null()) {
			pDataStructure->discoverableDisabled = jsonObjectData["discoverable_disabled"].get<bool>();
		}
	}

	void StageInstanceData::parseObjectReal(const nlohmann::json& jsonObjectData, StageInstanceData* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
			pDataStructure->topic = jsonObjectData["topic"].get<std::string>();
		}

		if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
			pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData["discoverable_disabled"].is_null()) {
			pDataStructure->discoverableDisabled = jsonObjectData["discoverable_disabled"].get<bool>();
		}
	}

	void VoiceRegionData::parseObjectReal(const nlohmann::json& jsonObjectData, VoiceRegionData* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("custom") && !jsonObjectData["custom"].is_null()) {
			pDataStructure->custom = jsonObjectData["custom"].get<bool>();
		}

		if (jsonObjectData.contains("deprecated") && !jsonObjectData["deprecated"].is_null()) {
			pDataStructure->deprecated = jsonObjectData["deprecated"].get<bool>();
		}

		if (jsonObjectData.contains("optimal") && !jsonObjectData["optimal"].is_null()) {
			pDataStructure->optimal = jsonObjectData["optimal"].get<bool>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}
	}

	void GuildPruneCountData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildPruneCountData* pDataStructure) {
		if (jsonObjectData.contains("pruned") && !jsonObjectData["pruned"].is_null()) {
			pDataStructure->count = jsonObjectData["pruned"].get<int32_t>();
		}
	}

	void SessionStartData::parseObjectReal(const nlohmann::json& jsonObjectData, SessionStartData* pDataStructure) {
		if (jsonObjectData.contains("max_concurrency") && !jsonObjectData["max_concurrency"].is_null()) {
			pDataStructure->maxConcurrency = jsonObjectData["max_concurrency"].get<uint32_t>();
		}

		if (jsonObjectData.contains("remaining") && !jsonObjectData["remaining"].is_null()) {
			pDataStructure->remaining = jsonObjectData["remaining"].get<uint32_t>();
		}

		if (jsonObjectData.contains("reset_after") && !jsonObjectData["reset_after"].is_null()) {
			pDataStructure->resetAfter = jsonObjectData["reset_after"].get<uint32_t>();
		}

		if (jsonObjectData.contains("total") && !jsonObjectData["total"].is_null()) {
			pDataStructure->total = jsonObjectData["total"].get<uint32_t>();
		}
	}

	void GatewayBotData::parseObjectReal(const nlohmann::json& jsonObjectData, GatewayBotData* pDataStructure) {
		if (jsonObjectData.contains("session_start_limit") && !jsonObjectData["session_start_limit"].is_null()) {
			pDataStructure->sessionStartLimit = jsonObjectData["session_start_limit"];
		}

		if (jsonObjectData.contains("shards") && !jsonObjectData["shards"].is_null()) {
			pDataStructure->shards = jsonObjectData["shards"].get<uint32_t>();
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}
	}

	void GuildWidgetData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildWidgetData* pDataStructure) {
		if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
			pDataStructure->enabled = jsonObjectData["enabled"].get<bool>();
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}
	};

	void EmbedFooterData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedFooterData* pDataStructure) {
		if (jsonObjectData.contains("text") && !jsonObjectData["text"].is_null()) {
			pDataStructure->text = jsonObjectData["text"].get<std::string>();
		}

		if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
			pDataStructure->iconUrl = jsonObjectData["icon_url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
			pDataStructure->proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
		}
	}

	void EmbedImageData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedImageData* pDataStructure) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
			pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
			pDataStructure->width = jsonObjectData["width"].get<int32_t>();
		}

		if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
			pDataStructure->height = jsonObjectData["height"].get<int32_t>();
		}
	}

	void EmbedThumbnailData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedThumbnailData* pDataStructure) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
			pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
			pDataStructure->width = jsonObjectData["width"].get<int32_t>();
		}

		if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
			pDataStructure->height = jsonObjectData["height"].get<int32_t>();
		}
	}

	void EmbedVideoData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedVideoData* pDataStructure) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
			pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
			pDataStructure->width = jsonObjectData["width"].get<int32_t>();
		}

		if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
			pDataStructure->height = jsonObjectData["height"].get<int32_t>();
		}
	}

	void EmbedAuthorData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedAuthorData* pDataStructure) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
			pDataStructure->proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
			pDataStructure->iconUrl = jsonObjectData["icon_url"].get<std::string>();
		}
	}

	void EmbedProviderData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedProviderData* pDataStructure) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}
	}

	void EmbedFieldData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedFieldData* pDataStructure) {
		if (jsonObjectData.contains("inline") && !jsonObjectData["inline"].is_null()) {
			pDataStructure->Inline = jsonObjectData["inline"].get<bool>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
			pDataStructure->value = jsonObjectData["value"].get<std::string>();
		}
	}

	void EmbedData::parseObjectReal(const nlohmann::json& jsonObjectData, EmbedData* pDataStructure) {
		if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
			pDataStructure->title = jsonObjectData["title"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
			int32_t colorValInt = jsonObjectData["color"].get<int32_t>();
			std::stringstream stream{};
			stream << std::setbase(16) << colorValInt;
			pDataStructure->hexColorValue = stream.str();
		}

		if (jsonObjectData.contains("footer") && !jsonObjectData["footer"].is_null()) {
			pDataStructure->footer = jsonObjectData["footer"];
		}

		if (jsonObjectData.contains("image") && !jsonObjectData["image"].is_null()) {
			pDataStructure->image = jsonObjectData["image"];
		}

		if (jsonObjectData.contains("provider") && !jsonObjectData["provider"].is_null()) {
			pDataStructure->provider = jsonObjectData["provider"];
		}

		if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
			pDataStructure->thumbnail = jsonObjectData["thumbnail"];
		}

		if (jsonObjectData.contains("video") && !jsonObjectData["video"].is_null()) {
			pDataStructure->video = jsonObjectData["video"];
		}

		if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
			pDataStructure->author = jsonObjectData["author"];
		}

		if (jsonObjectData.contains("fields") && !jsonObjectData["fields"].is_null()) {
			pDataStructure->fields.clear();
			pDataStructure->fields.reserve(jsonObjectData["fields"].size());
			for (auto& value: jsonObjectData["fields"]) {
				EmbedFieldData newData{ value };
				pDataStructure->fields.push_back(newData);
			}
			pDataStructure->fields.shrink_to_fit();
		}
	};

	void Reaction::parseObjectReal(const nlohmann::json& jsonObjectData, Reaction* pDataStructure) {
		if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
			pDataStructure->count = jsonObjectData["count"].get<int32_t>();
		}

		if (jsonObjectData.contains("me") && !jsonObjectData["me"].is_null()) {
			pDataStructure->count = jsonObjectData["me"].get<bool>();
		}

		if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
			pDataStructure->emoji = jsonObjectData["emoji"];
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			pDataStructure->member = jsonObjectData["member"];
		}
	}

	void ReactionVector::parseObjectReal(const nlohmann::json& jsonObjectData, ReactionVector* pDataStructure) {
		pDataStructure->theReactions.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::Reaction newData{ value };
			pDataStructure->theReactions.push_back(newData);
		}
		pDataStructure->theReactions.shrink_to_fit();
	}

	void GuildScheduledEvent::parseObjectReal(const nlohmann::json& jsonObjectData, GuildScheduledEvent* pDataStructure) {
		if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
			pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<GuildScheduledEventPrivacyLevel>();
		}

		if (jsonObjectData.contains("entity_type") && !jsonObjectData["entity_type"].is_null()) {
			pDataStructure->entityType = jsonObjectData["entity_type"].get<GuildScheduledEventEntityType>();
		}

		if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
			pDataStructure->status = jsonObjectData["status"].get<GuildScheduledEventStatus>();
		}

		if (jsonObjectData.contains("entity_metadata") && !jsonObjectData["entity_metadata"].is_null()) {
			pDataStructure->entityMetadata = jsonObjectData["entity_metadata"];
		}

		if (jsonObjectData.contains("scheduled_start_time") && !jsonObjectData["scheduled_start_time"].is_null()) {
			pDataStructure->scheduledStartTime = jsonObjectData["scheduled_start_time"].get<std::string>();
		}

		if (jsonObjectData.contains("scheduled_end_time") && !jsonObjectData["scheduled_end_time"].is_null()) {
			pDataStructure->scheduledEndTime = jsonObjectData["scheduled_end_time"].get<std::string>();
		}

		if (jsonObjectData.contains("user_count") && !jsonObjectData["user_count"].is_null()) {
			pDataStructure->userCount = jsonObjectData["user_count"].get<uint32_t>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
			pDataStructure->creatorId = jsonObjectData["creator_id"].get<std::string>();
		}

		if (jsonObjectData.contains("entity_id") && !jsonObjectData["entity_id"].is_null()) {
			pDataStructure->entityId = jsonObjectData["entity_id"].get<std::string>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
			pDataStructure->creator = jsonObjectData["creator"];
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}
	}

	void GuildTemplateData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildTemplateData* pDataStructure) {
		if (jsonObjectData.contains("serialized_source_guild") && !jsonObjectData["serialized_source_guild"].is_null()) {
			pDataStructure->serializedSourceGuild = jsonObjectData["serialized_source_guild"];
		}

		if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
			pDataStructure->creator = jsonObjectData["creator"];
		}

		if (jsonObjectData.contains("source_guild_id") && !jsonObjectData["source_guild_id"].is_null()) {
			pDataStructure->sourceGuildId = jsonObjectData["source_guild_id"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("usage_count") && !jsonObjectData["usage_count"].is_null()) {
			pDataStructure->usageCount = jsonObjectData["usage_count"].get<uint32_t>();
		}

		if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
			pDataStructure->creatorId = jsonObjectData["creator_id"].get<std::string>();
		}

		if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
			pDataStructure->createdAt = jsonObjectData["created_at"].get<std::string>();
		}

		if (jsonObjectData.contains("updated_at") && !jsonObjectData["updated_at"].is_null()) {
			pDataStructure->updatedAt = jsonObjectData["updated_at"].get<std::string>();
		}

		if (jsonObjectData.contains("is_dirty") && !jsonObjectData["is_dirty"].is_null()) {
			pDataStructure->isDirty = jsonObjectData["is_dirty"].get<bool>();
		}

		if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null()) {
			pDataStructure->code = jsonObjectData["code"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}
	}

	void GuildScheduledEventUserData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildScheduledEventUserData* pDataStructure) {
		if (jsonObjectData.contains("guild_scheduled_event_id") && !jsonObjectData["guild_scheduled_event_id"].is_null()) {
			pDataStructure->guildScheduledEventId = jsonObjectData["guild_scheduled_event_id"].get<std::string>();
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			pDataStructure->user = jsonObjectData["user"];
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			pDataStructure->member = jsonObjectData["member"];
		}
	}

	void GuildScheduledEventUserData::parseObjectReal(const nlohmann::json& jsonObjectData, std::vector<GuildScheduledEventUserData>* pDataStructure) {
		pDataStructure->reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			GuildScheduledEventUserData newData{ value };
			pDataStructure->push_back(newData);
		}
		pDataStructure->shrink_to_fit();
	}

	void AllowedMentionsData::parseObjectReal(const nlohmann::json& jsonObjectData, AllowedMentionsData* pDataStructure) {
		if (jsonObjectData.contains("parse") && !jsonObjectData["parse"].is_null()) {
			pDataStructure->parse.clear();
			pDataStructure->parse.reserve(jsonObjectData["parse"].size());
			for (auto& value: jsonObjectData["parse"]) {
				pDataStructure->parse.push_back(value.get<std::string>());
			}
			pDataStructure->parse.shrink_to_fit();
		}

		if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
			pDataStructure->roles.clear();
			pDataStructure->roles.reserve(jsonObjectData["roles"].size());
			for (auto& value: jsonObjectData["roles"]) {
				pDataStructure->roles.push_back(value.get<std::string>());
			}
			pDataStructure->roles.shrink_to_fit();
		}

		if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
			pDataStructure->users.clear();
			pDataStructure->users.reserve(jsonObjectData["users"].size());
			for (auto& value: jsonObjectData["users"]) {
				pDataStructure->users.push_back(value.get<std::string>());
			}
			pDataStructure->users.shrink_to_fit();
		}

		if (jsonObjectData.contains("replied_user") && !jsonObjectData["replied_user"].is_null()) {
			pDataStructure->repliedUser = jsonObjectData["replied_user"].get<bool>();
		}
	}

	void MessageDataOld::parseObjectReal(const nlohmann::json& jsonObjectData, MessageDataOld* pDataStructure) {
		if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
			pDataStructure->content = jsonObjectData["content"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
			pDataStructure->author = jsonObjectData["author"];
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			pDataStructure->member = jsonObjectData["member"];
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
			pDataStructure->editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();


			if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
				pDataStructure->tts = jsonObjectData["tts"].get<bool>();
			}

			if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
				pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
			}

			if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
				pDataStructure->mentions.clear();
				pDataStructure->mentions.reserve(jsonObjectData["mentions"].size());
				for (auto& value: jsonObjectData["mentions"]) {
					UserData newData{ value };
					pDataStructure->mentions.push_back(newData);
				}
				pDataStructure->mentions.shrink_to_fit();
			}

			if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
				pDataStructure->mentionRoles.clear();
				pDataStructure->mentionRoles.reserve(jsonObjectData["mention_roles"].size());
				for (auto& value: jsonObjectData["mention_roles"]) {
					pDataStructure->mentionRoles.push_back(value.get<std::string>());
				}
				pDataStructure->mentionRoles.shrink_to_fit();
			}

			if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
				pDataStructure->mentionChannels.clear();
				pDataStructure->mentionChannels.reserve(jsonObjectData["mention_channels"].size());
				for (auto& value: jsonObjectData["mention_channels"]) {
					ChannelMentionData newData{ value };
					pDataStructure->mentionChannels.push_back(newData);
				}
				pDataStructure->mentionChannels.shrink_to_fit();
			}

			if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
				pDataStructure->attachments.clear();
				pDataStructure->attachments.reserve(jsonObjectData["attachments"].size());
				for (auto& value: jsonObjectData["attachments"]) {
					AttachmentData newData{ value };
					pDataStructure->attachments.push_back(newData);
				}
				pDataStructure->attachments.shrink_to_fit();
			}

			if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
				pDataStructure->embeds.clear();
				pDataStructure->embeds.reserve(jsonObjectData["embeds"].size());
				for (auto& value: jsonObjectData["embeds"]) {
					EmbedData newData{ value };
					pDataStructure->embeds.push_back(newData);
				}
				pDataStructure->embeds.shrink_to_fit();
			}

			if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
				pDataStructure->reactions.clear();
				pDataStructure->reactions.reserve(jsonObjectData["reactions"].size());
				for (auto& value: jsonObjectData["reactions"]) {
					ReactionData newData{ value };
					pDataStructure->reactions.push_back(newData);
				}
				pDataStructure->reactions.shrink_to_fit();
			}

			if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
				pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
			}

			if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
				pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
			}

			if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
				pDataStructure->webhookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<MessageType>();
			}

			if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
				pDataStructure->activity = jsonObjectData["activity"];
			}

			if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
				pDataStructure->application = jsonObjectData["application"];
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
				pDataStructure->messageReference = jsonObjectData["message_reference"];
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
			}

			if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
				pDataStructure->stickerItems.clear();
				pDataStructure->stickerItems.reserve(jsonObjectData["sticker_items"].size());
				for (auto& value: jsonObjectData["sticker_items"]) {
					StickerItemData newData{ value };
					pDataStructure->stickerItems.push_back(newData);
				}
				pDataStructure->stickerItems.shrink_to_fit();
			}

			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
				pDataStructure->stickers.shrink_to_fit();
			}

			if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
				pDataStructure->interaction = jsonObjectData["interaction"];
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->components.clear();
				pDataStructure->components.reserve(jsonObjectData["components"].size());
				for (auto& value: jsonObjectData["components"]) {
					ActionRowData newData{ value };
					pDataStructure->components.push_back(newData);
				}
				pDataStructure->components.shrink_to_fit();
			}

			if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
				pDataStructure->thread = jsonObjectData["thread"];
			}
		}
	}

	void Message::parseObjectReal(const nlohmann::json& jsonObjectData, Message* pDataStructure) {
		if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
			pDataStructure->content = jsonObjectData["content"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
			pDataStructure->author = jsonObjectData["author"];
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			pDataStructure->member = jsonObjectData["member"];
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
			pDataStructure->editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
			pDataStructure->tts = jsonObjectData["tts"].get<bool>();
		}

		if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
			pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
		}

		if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
			pDataStructure->mentions.clear();
			pDataStructure->mentions.reserve(jsonObjectData["mentions"].size());
			for (auto& value: jsonObjectData["mentions"]) {
				UserData newData{ value };
				pDataStructure->mentions.push_back(newData);
			}
			pDataStructure->mentions.shrink_to_fit();
		}

		if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
			pDataStructure->mentionRoles.clear();
			pDataStructure->mentionRoles.reserve(jsonObjectData["mention_roles"].size());
			for (auto& value: jsonObjectData["mention_roles"]) {
				pDataStructure->mentionRoles.push_back(value.get<std::string>());
			}
			pDataStructure->mentionRoles.shrink_to_fit();
		}

		if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
			pDataStructure->mentionChannels.clear();
			pDataStructure->mentionChannels.reserve(jsonObjectData["mention_channels"].size());
			for (auto& value: jsonObjectData["mention_channels"]) {
				ChannelMentionData newData{ value };
				pDataStructure->mentionChannels.push_back(newData);
			}
			pDataStructure->mentionChannels.shrink_to_fit();
		}

		if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
			pDataStructure->attachments.clear();
			pDataStructure->attachments.reserve(jsonObjectData["attachments"].size());
			for (auto& value: jsonObjectData["attachments"]) {
				AttachmentData newData{ value };
				pDataStructure->attachments.push_back(newData);
			}
			pDataStructure->attachments.shrink_to_fit();
		}

		if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
			pDataStructure->embeds.clear();
			pDataStructure->embeds.reserve(jsonObjectData["embeds"].size());
			for (auto& value: jsonObjectData["embeds"]) {
				EmbedData newData{ value };
				pDataStructure->embeds.push_back(newData);
			}
			pDataStructure->embeds.shrink_to_fit();
		}

		if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
			pDataStructure->reactions.clear();
			pDataStructure->reactions.reserve(jsonObjectData["reactions"].size());
			for (auto& value: jsonObjectData["reactions"]) {
				ReactionData newData{ value };
				pDataStructure->reactions.push_back(newData);
			}
			pDataStructure->reactions.shrink_to_fit();
		}

		if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
			pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
		}

		if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
			pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
		}

		if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
			pDataStructure->webhookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<MessageType>();
		}

		if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
			pDataStructure->activity = jsonObjectData["activity"];
		}

		if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
			pDataStructure->application = jsonObjectData["application"];
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
			pDataStructure->messageReference = jsonObjectData["message_reference"];
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
		}

		if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
			pDataStructure->stickerItems.clear();
			pDataStructure->stickerItems.reserve(jsonObjectData["sticker_items"].size());
			for (auto& value: jsonObjectData["sticker_items"]) {
				StickerItemData newData{ value };
				pDataStructure->stickerItems.push_back(newData);
			}
			pDataStructure->stickerItems.shrink_to_fit();
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			pDataStructure->stickers.clear();
			pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{ value };
				pDataStructure->stickers.push_back(newData);
			}
			pDataStructure->stickers.shrink_to_fit();
		}

		if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
			pDataStructure->interaction = jsonObjectData["interaction"];
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			pDataStructure->components.clear();
			pDataStructure->components.reserve(jsonObjectData["components"].size());
			for (auto& value: jsonObjectData["components"]) {
				ActionRowData newData{ value };
				pDataStructure->components.push_back(newData);
			}
			pDataStructure->components.shrink_to_fit();
		}

		if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
			pDataStructure->thread = jsonObjectData["thread"];
		}
	}

	void ApplicationCommand::parseObjectReal(const nlohmann::json& jsonObjectData, ApplicationCommand* pDataStructure) {
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

	void ApplicationCommandInteractionDataOption::parseObjectReal(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionDataOption* pDataStructure) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandOptionType>();
		}

		if (jsonObjectData.contains("focused") && !jsonObjectData["focused"].is_null()) {
			pDataStructure->focused = jsonObjectData["focused"].get<bool>();
		}

		if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
			if (jsonObjectData["value"].is_boolean()) {
				pDataStructure->valueBool = jsonObjectData["value"].get<bool>();
			}
			if (jsonObjectData["value"].is_string()) {
				pDataStructure->valueString = jsonObjectData["value"].get<std::string>();
			}
			if (jsonObjectData["value"].is_number()) {
				pDataStructure->valueInt = jsonObjectData["value"].get<int32_t>();
			}
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			pDataStructure->options.reserve(jsonObjectData["options"].size());
			for (auto& value: jsonObjectData["options"]) {
				ApplicationCommandInteractionDataOption newData{ value };
				pDataStructure->options.push_back(newData);
			}
			pDataStructure->options.shrink_to_fit();
		}
	}

	void CommandData::parseObjectReal(const nlohmann::json& jsonObjectData, CommandData* pDataStructure) {
		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			pDataStructure->optionsArgs.reserve(jsonObjectData["options"].size());
			for (auto& value: jsonObjectData["options"]) {
				if (value.contains("type") && value["type"] == 1) {
					if (value.contains("name")) {
						pDataStructure->subCommandName = value["name"];
					}
				} else if (value.contains("type") && value["type"] == 2) {
					if (value.contains("name")) {
						pDataStructure->subCommandGroupName = value["name"];
					}
				}
				if (value.contains("options")) {
					parseObjectReal(value, pDataStructure);
				}
				if (value.contains("value") && !value["value"].is_null()) {
					auto& newValueNew = value["value"];
					if (newValueNew.is_string()) {
						pDataStructure->optionsArgs.push_back(newValueNew);
					} else if (newValueNew.is_number()) {
						pDataStructure->optionsArgs.push_back(std::to_string(newValueNew.get<int64_t>()));
					} else if (newValueNew.is_boolean()) {
						pDataStructure->optionsArgs.push_back(std::to_string(newValueNew.get<bool>()));
					}
				}
			}
			pDataStructure->optionsArgs.shrink_to_fit();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->commandName = jsonObjectData["name"].get<std::string>();
		}
	}

	void StickerPackData::parseObjectReal(const nlohmann::json& jsonObjectData, StickerPackData* pDataStructure) {
		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			pDataStructure->stickers.clear();
			pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{ value };
				pDataStructure->stickers.push_back(newData);
			}
			pDataStructure->stickers.shrink_to_fit();
		}

		if (jsonObjectData.contains("cover_sticker_id") && !jsonObjectData["cover_sticker_id"].is_null()) {
			pDataStructure->coverStickerId = jsonObjectData["cover_sticker_id"].get<std::string>();
		}

		if (jsonObjectData.contains("banner_asset_id") && !jsonObjectData["banner_asset_id"].is_null()) {
			pDataStructure->bannerAssetId = jsonObjectData["banner_asset_id"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			pDataStructure->description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->Id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("sku_id") && !jsonObjectData["sku_id"].is_null()) {
			pDataStructure->skuId = jsonObjectData["sku_id"].get<std::string>();
		}
	}

	void StickerPackData::parseObjectReal(const nlohmann::json& jsonObjectData, std::vector<StickerPackData>* pDataStructure) {
		if (jsonObjectData.contains("sticker_packs") && !jsonObjectData["sticker_packs"].is_null()) {
			pDataStructure->reserve(jsonObjectData["sticker_packs"].size());
			for (auto& value: jsonObjectData["sticker_packs"]) {
				StickerPackData newData{ value };
				pDataStructure->push_back(newData);
			}
			pDataStructure->shrink_to_fit();
		}
	}

	void ApplicationCommandInteractionData::parseObjectReal(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionData* pDataStructure) {
		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			pDataStructure->options.reserve(jsonObjectData["options"].size());
			for (auto& value: jsonObjectData["options"]) {
				ApplicationCommandInteractionDataOption newData{ value };
				pDataStructure->options.push_back(newData);
			}
			pDataStructure->options.shrink_to_fit();
		}

		if (jsonObjectData.contains("resolved") && !jsonObjectData["resolved"].is_null()) {
			auto& value = jsonObjectData["resolved"];

			if (value.contains("attachments") && !value["attachments"].is_null()) {
				pDataStructure->resolved.attachments.clear();
				auto newMap = value["attachments"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					AttachmentData newData{ newValue };
					pDataStructure->resolved.attachments[stoull(key)] = newData;
				}
			}

			if (value.contains("users") && !value["users"].is_null()) {
				pDataStructure->resolved.users.clear();
				auto newMap = value["users"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					UserData newData{ value };
					pDataStructure->resolved.users[stoull(key)] = newData;
				}
			}
			if (value.contains("channels") && !value["channels"].is_null()) {
				pDataStructure->resolved.channels.clear();
				auto newMap = value["channels"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					ChannelData newData{newValue};
					pDataStructure->resolved.channels[stoull(key)] = newData;
				}
			}
			if (value.contains("roles") && !value["roles"].is_null()) {
				pDataStructure->resolved.roles.clear();
				auto newMap = value["roles"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					RoleData newData{ newValue };
					pDataStructure->resolved.roles[stoull(key)] = newData;
				}
			}
			if (value.contains("members") && !value["members"].is_null()) {
				pDataStructure->resolved.members.clear();
				auto newMap = value["members"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					GuildMemberData newData{ newValue };
					pDataStructure->resolved.members[stoull(key)] = newData;
				}
			}
			if (value.contains("messages") && !value["messages"].is_null()) {
				pDataStructure->resolved.messages.clear();
				auto newMap = value["messages"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					MessageData newData{ newValue };
					pDataStructure->resolved.messages[stoull(key)] = newData;
				}
			}
		}
	}

	void MessageCommandInteractionData::parseObjectReal(const nlohmann::json& jsonObjectData, MessageCommandInteractionData* pDataStructure) {
		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			pDataStructure->targetId = jsonObjectData["target_id"].get<std::string>();
		}
	}

	void UserCommandInteractionData::parseObjectReal(const nlohmann::json& jsonObjectData, UserCommandInteractionData* pDataStructure) {
		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			pDataStructure->targetId = jsonObjectData["target_id"].get<std::string>();
		}
	}

	void ComponentInteractionData::parseObjectReal(const nlohmann::json& jsonObjectData, ComponentInteractionData* pDataStructure) {
		if (jsonObjectData.contains("values") && !jsonObjectData["values"].is_null()) {
			pDataStructure->values.clear();
			pDataStructure->values.reserve(jsonObjectData["values"].size());
			for (auto& value: jsonObjectData["values"]) {
				pDataStructure->values.push_back(value);
			}
			pDataStructure->values.shrink_to_fit();
		}

		if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
			pDataStructure->customId = jsonObjectData["custom_id"].get<std::string>();
		}

		if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
			pDataStructure->componentType = jsonObjectData["component_type"].get<ComponentType>();
		}
	}

	void ModalInteractionData::parseObjectReal(const nlohmann::json& jsonObjectData, ModalInteractionData* pDataStructure) {
		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			pDataStructure->value = jsonObjectData["components"][0]["components"][0]["value"].get<std::string>();
		}

		if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
			pDataStructure->customId = jsonObjectData["custom_id"].get<std::string>();
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			pDataStructure->customIdSmall = jsonObjectData["components"][0]["components"][0]["custom_id"].get<std::string>();
		}
	}

	void InteractionDataData::parseObjectReal(const nlohmann::json& jsonObjectData, InteractionDataData* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->applicationCommandData = jsonObjectData;
		}

		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			pDataStructure->messageInteractionData = jsonObjectData;
			pDataStructure->userInteractionData = jsonObjectData;
		}

		if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
			pDataStructure->componentData = jsonObjectData;
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			pDataStructure->modalData = jsonObjectData;
		}
	}

	void InteractionData::parseObjectReal(const nlohmann::json& jsonObjectData, InteractionData* pDataStructure) {
		if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
			pDataStructure->data = jsonObjectData["data"];
			pDataStructure->rawData = jsonObjectData["data"];
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<InteractionType>();
		}

		if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
			pDataStructure->token = jsonObjectData["token"].get<std::string>();
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			pDataStructure->member = jsonObjectData["member"];
			pDataStructure->user.avatar = pDataStructure->member.userAvatar;
			pDataStructure->user.id = pDataStructure->member.id;
			pDataStructure->user.userName = pDataStructure->member.userName;
		} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			pDataStructure->user = jsonObjectData["user"];
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("locale") && !jsonObjectData["locale"].is_null()) {
			pDataStructure->locale = jsonObjectData["locale"].get<std::string>();
		}

		if (jsonObjectData.contains("guild_locale") && !jsonObjectData["guild_locale"].is_null()) {
			pDataStructure->guildLocale = jsonObjectData["guild_locale"].get<std::string>();
		}

		if (jsonObjectData.contains("message") && !jsonObjectData["message"].is_null()) {
			pDataStructure->message = jsonObjectData["message"];
		}

		if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
			pDataStructure->version = jsonObjectData["version"].get<int32_t>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}
	}

	void ReactionRemoveData::parseObjectReal(const nlohmann::json& jsonObjectData, ReactionRemoveData* pDataStructure) {
		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
			pDataStructure->emoji = jsonObjectData["emoji"];
		}
	}

	void WebHook::parseObjectReal(const nlohmann::json& jsonObjectData, WebHook* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<WebHookType>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			pDataStructure->user = jsonObjectData["user"];
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
			pDataStructure->avatar = jsonObjectData["avatar"].get<std::string>();
		}

		if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
			pDataStructure->token = jsonObjectData["token"].get<std::string>();
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
			pDataStructure->sourceGuild = jsonObjectData["source_guild"];
		}

		if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
			pDataStructure->sourceChannel = jsonObjectData["source_channel"];
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			pDataStructure->url = jsonObjectData["url"].get<std::string>();
		}
	}

	void RoleTagsData::parseObjectReal(const nlohmann::json& jsonObjectData, RoleTagsData* pDataStructure) {
		if (jsonObjectData.contains("bot_id") && !jsonObjectData["bot_id"].is_null()) {
			pDataStructure->botId = jsonObjectData["bot_id"].get<std::string>();
		}

		if (jsonObjectData.contains("integration_id") && !jsonObjectData["integration_id"].is_null()) {
			pDataStructure->integrationId = jsonObjectData["integration_id"].get<std::string>();
		}
	}

	void GuildApplicationCommandPermissionsData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsData* pDataStructure) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			pDataStructure->permissions.clear();
			pDataStructure->permissions.reserve(jsonObjectData["permissions"].size());
			for (auto& value: jsonObjectData["permissions"]) {
				ApplicationCommandPermissionData newData{ value };
				pDataStructure->permissions.push_back(newData);
			}
			pDataStructure->permissions.shrink_to_fit();
		}
	}

	void GuildApplicationCommandPermissionsDataVector::parseObjectReal(const nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsDataVector* pDataStructure) {
		pDataStructure->theGuildApplicationCommandPermissionsDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::GuildApplicationCommandPermissionsData newData{ value };
			pDataStructure->theGuildApplicationCommandPermissionsDatas.push_back(newData);
		}
		pDataStructure->theGuildApplicationCommandPermissionsDatas.shrink_to_fit();
	}

	void ApplicationCommandPermissionData::parseObjectReal(const nlohmann::json& jsonObjectData, ApplicationCommandPermissionData* pDataStructure) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("permission") && !jsonObjectData["permission"].is_null()) {
			pDataStructure->permission = jsonObjectData["permission"].get<bool>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandPermissionType>();
		}
	}

	void AuditLogChangeData::parseObjectReal(const nlohmann::json& jsonObjectData, AuditLogChangeData* pDataStructure) {
		if (jsonObjectData.contains("new_value") && !jsonObjectData["new_value"].is_null()) {
			pDataStructure->newValue = jsonObjectData["new_value"];
		}

		if (jsonObjectData.contains("old_value") && !jsonObjectData["old_value"].is_null()) {
			pDataStructure->oldValue = jsonObjectData["old_value"];
		}

		if (jsonObjectData.contains("key") && !jsonObjectData["key"].is_null()) {
			pDataStructure->key = jsonObjectData["key"].get<std::string>();
		}
	}


	void OptionalAuditEntryInfoData::parseObjectReal(const nlohmann::json& jsonObjectData, OptionalAuditEntryInfoData* pDataStructure) {
		if (jsonObjectData.contains("delete_member_days") && !jsonObjectData["delete_member_days"].is_null()) {
			pDataStructure->deleteMemberDays = jsonObjectData["delete_member_days"].get<std::string>();
		}

		if (jsonObjectData.contains("members_removed") && !jsonObjectData["members_removed"].is_null()) {
			pDataStructure->membersRemoved = jsonObjectData["members_removed"].get<std::string>();
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
			pDataStructure->count = jsonObjectData["count"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<std::string>();
		}

		if (jsonObjectData.contains("role_name") && !jsonObjectData["role_name"].is_null()) {
			pDataStructure->roleName = jsonObjectData["role_name"].get<std::string>();
		}
	}

	void AuditLogEntryData::parseObjectReal(const nlohmann::json& jsonObjectData, AuditLogEntryData* pDataStructure) {
		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			pDataStructure->targetId = stoull(jsonObjectData["target_id"].get<std::string>());
		}

		if (jsonObjectData.contains("changes") && !jsonObjectData["changes"].is_null()) {
			pDataStructure->changes.clear();
			pDataStructure->changes.reserve(jsonObjectData["changes"].size());
			for (auto& value: jsonObjectData["changes"]) {
				AuditLogChangeData newData{ value };
				pDataStructure->changes.push_back(newData);
			}
			pDataStructure->changes.shrink_to_fit();
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			pDataStructure->createdTimeStamp = pDataStructure->getCreatedAtTimestamp(TimeFormat::LongDateTime);
		}

		if (jsonObjectData.contains("action_type") && !jsonObjectData["action_type"].is_null()) {
			pDataStructure->actionType = jsonObjectData["action_type"].get<AuditLogEvent>();
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			pDataStructure->options = jsonObjectData["options"];
		}

		if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
			pDataStructure->reason = jsonObjectData["reason"].get<std::string>();
		}
	}

	void IntegrationDataVector::parseObjectReal(const nlohmann::json& jsonObjectData, IntegrationDataVector* pDataStructure) {
		pDataStructure->theIntegrationDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::IntegrationData newData{ value };
			pDataStructure->theIntegrationDatas.push_back(newData);
		}
		pDataStructure->theIntegrationDatas.shrink_to_fit();
	}

	void ConnectionData::parseObjectReal(const nlohmann::json& jsonObjectData, ConnectionData* pDataStructure) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			pDataStructure->name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			pDataStructure->type = jsonObjectData["type"].get<std::string>();
		}

		if (jsonObjectData.contains("show_activity") && !jsonObjectData["show_activity"].is_null()) {
			pDataStructure->showActivity = jsonObjectData["show_activity"].get<bool>();
		}

		if (jsonObjectData.contains("friend_sync") && !jsonObjectData["friend_sync"].is_null()) {
			pDataStructure->friendSync = jsonObjectData["friend_sync"].get<bool>();
		}

		if (jsonObjectData.contains("revoked") && !jsonObjectData["revoked"].is_null()) {
			pDataStructure->revoked = jsonObjectData["revoked"].get<bool>();
		}

		if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
			pDataStructure->verified = jsonObjectData["verified"].get<bool>();
		}

		if (jsonObjectData.contains("visibility") && !jsonObjectData["visibility"].is_null()) {
			pDataStructure->visibility = jsonObjectData["visibility"].get<ConnectionVisibilityTypes>();
		}

		if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
			pDataStructure->integrations.clear();
			pDataStructure->integrations.reserve(jsonObjectData["integrations"].size());
			for (auto& value: jsonObjectData["integrations"]) {
				IntegrationData newData{ value };
				pDataStructure->integrations.push_back(newData);
			}
			pDataStructure->integrations.shrink_to_fit();
		}
	}

	void AuditLogData::parseObjectReal(const nlohmann::json& jsonObjectData, AuditLogData* pDataStructure) {
		if (jsonObjectData.contains("webhooks") && !jsonObjectData["webhooks"].is_null()) {
			pDataStructure->webhooks.clear();
			pDataStructure->webhooks.reserve(jsonObjectData["webhooks"].size());
			for (auto& value: jsonObjectData["webhooks"]) {
				WebHookData newData{ value };
				pDataStructure->webhooks.push_back(newData);
			}
			pDataStructure->webhooks.shrink_to_fit();
		}

		if (jsonObjectData.contains("guild_scheduled_events") && !jsonObjectData["guild_scheduled_events"].is_null()) {
			pDataStructure->webhooks.clear();
			pDataStructure->webhooks.reserve(jsonObjectData["guild_scheduled_events"].size());
			for (auto& value: jsonObjectData["guild_scheduled_events"]) {
				GuildScheduledEventData newData{ value };
				pDataStructure->guildScheduledEvents.push_back(newData);
			}
			pDataStructure->webhooks.shrink_to_fit();
		}

		if (jsonObjectData.contains("auto_moderation_rules") && !jsonObjectData["auto_moderation_rules"].is_null()) {
			pDataStructure->users.clear();
			pDataStructure->users.reserve(jsonObjectData["auto_moderation_rules"].size());
			for (auto& value: jsonObjectData["auto_moderation_rules"]) {
				AutoModerationRule newData{ value };
				pDataStructure->autoModerationRules.push_back(newData);
			}
			pDataStructure->autoModerationRules.shrink_to_fit();
		}

		if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
			pDataStructure->users.clear();
			pDataStructure->users.reserve(jsonObjectData["users"].size());
			for (auto& value: jsonObjectData["users"]) {
				UserData newData{ value };
				pDataStructure->users.push_back(newData);
			}
			pDataStructure->users.shrink_to_fit();
		}

		if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData["audit_log_entries"].is_null()) {
			pDataStructure->auditLogEntries.clear();
			pDataStructure->auditLogEntries.reserve(jsonObjectData["audit_log_entries"].size());
			for (auto& value: jsonObjectData["audit_log_entries"]) {
				AuditLogEntryData newData{ value };
				pDataStructure->auditLogEntries.push_back(newData);
			}
			pDataStructure->auditLogEntries.shrink_to_fit();
		}

		if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
			pDataStructure->integrations.clear();
			pDataStructure->integrations.reserve(jsonObjectData["integrations"].size());
			for (auto& value: jsonObjectData["integrations"]) {
				IntegrationData newData{ value };
				pDataStructure->integrations.push_back(newData);
			}
			pDataStructure->integrations.shrink_to_fit();
		}

		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			pDataStructure->threads.clear();
			pDataStructure->threads.reserve(jsonObjectData["threads"].size());
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{ value };
				pDataStructure->threads.push_back(newData);
			}
			pDataStructure->threads.shrink_to_fit();
		}
	}

	void TypingStartData::parseObjectReal(const nlohmann::json& jsonObjectData, TypingStartData* pDataStructure) {
		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			pDataStructure->member = jsonObjectData["member"];
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			pDataStructure->timestamp = jsonObjectData["timestamp"].get<int32_t>();
		}
	}

	void ChannelPinsUpdateEventData::parseObjectReal(const nlohmann::json& jsonObjectData, ChannelPinsUpdateEventData* pDataStructre) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructre->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			pDataStructre->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
			pDataStructre->lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
		}
	}

	void BanData::parseObjectReal(const nlohmann::json& jsonObjectData, BanData* pDataStructure) {
		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			pDataStructure->user = jsonObjectData["user"];
		}

		if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
			pDataStructure->reason = jsonObjectData["reason"].get<std::string>();
		}
	}

	void BanDataVector::parseObjectReal(const nlohmann::json& jsonObjectData, BanDataVector* pDataStructure) {
		pDataStructure->theBanDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::BanData newData{ value };
			pDataStructure->theBanDatas.push_back(newData);
		}
		pDataStructure->theBanDatas.shrink_to_fit();
	}

	void ThreadListSyncData::parseObjectReal(const nlohmann::json& jsonObjectData, ThreadListSyncData* pDataStructure) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_ids") && !jsonObjectData["channel_ids"].is_null()) {
			pDataStructure->channelIds.clear();
			pDataStructure->channelIds.reserve(jsonObjectData["channel_ids"].size());
			for (auto& value: jsonObjectData["channel_ids"]) {
				pDataStructure->channelIds.push_back(value.get<std::string>());
			}
			pDataStructure->channelIds.shrink_to_fit();
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			pDataStructure->members.clear();
			pDataStructure->members.reserve(jsonObjectData["members"].size());
			for (auto& value: jsonObjectData["members"]) {
				ThreadMemberData newData{ value };
				pDataStructure->members.push_back(newData);
			}
			pDataStructure->members.shrink_to_fit();
		}

		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			pDataStructure->threads.clear();
			pDataStructure->threads.reserve(jsonObjectData["threads"].size());
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{ value };
				pDataStructure->threads.push_back(newData);
			}
			pDataStructure->threads.shrink_to_fit();
		}
	}

	void ThreadMembersUpdateData::parseObjectReal(const nlohmann::json& jsonObjectData, ThreadMembersUpdateData* pDataStructure) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
			pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("added_members") && !jsonObjectData["added_members"].is_null()) {
			pDataStructure->addedMembers.clear();
			pDataStructure->addedMembers.reserve(jsonObjectData["added_members"].size());
			for (auto& value: jsonObjectData["added_members"]) {
				ThreadMemberData newData{ value };
				pDataStructure->addedMembers.push_back(newData);
			}
			pDataStructure->addedMembers.shrink_to_fit();
		}

		if (jsonObjectData.contains("removed_member_ids") && !jsonObjectData["removed_member_ids"].is_null()) {
			pDataStructure->removedMemberIds.clear();
			pDataStructure->removedMemberIds.reserve(jsonObjectData["removed_member_ids"].size());
			for (auto& value: jsonObjectData["removed_member_ids"]) {
				pDataStructure->removedMemberIds.push_back(value);
			}
			pDataStructure->removedMemberIds.shrink_to_fit();
		}
	}

	void GuildScheduledEventUserDataVector::parseObjectReal(const nlohmann::json& jsonObjectData, GuildScheduledEventUserDataVector* pDataStructure) {
		pDataStructure->theGuildScheduledEventUserDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::GuildScheduledEventUserData newData{ value };
			pDataStructure->theGuildScheduledEventUserDatas.push_back(newData);
		}
		pDataStructure->theGuildScheduledEventUserDatas.shrink_to_fit();
	}

	void InviteData::parseObjectReal(const nlohmann::json& jsonObjectData, InviteData* pDataStructure) {
		if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_string()) {
			pDataStructure->code = jsonObjectData["code"].get<std::string>();
		} else if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_number_integer()) {
			pDataStructure->code = std::to_string(jsonObjectData["code"].get<int32_t>());
		}

		if (jsonObjectData.contains("guild") && !jsonObjectData["guild"].is_null()) {
			pDataStructure->guild = jsonObjectData["guild"];
		}

		if (jsonObjectData.contains("channel") && !jsonObjectData["channel"].is_null()) {
			pDataStructure->channel = jsonObjectData["channel"];
		}

		if (jsonObjectData.contains("inviter") && !jsonObjectData["inviter"].is_null()) {
			pDataStructure->inviter = jsonObjectData["inviter"];
		}

		if (jsonObjectData.contains("target_type") && !jsonObjectData["target_type"].is_null()) {
			pDataStructure->targetType = jsonObjectData["target_type"].get<int32_t>();
		}

		if (jsonObjectData.contains("target_user") && !jsonObjectData["target_user"].is_null()) {
			pDataStructure->targetUser = jsonObjectData["target_user"];
		}

		if (jsonObjectData.contains("target_application") && !jsonObjectData["target_application"].is_null()) {
			pDataStructure->targetApplication = jsonObjectData["target_application"];
		}

		if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
			pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
			pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("expires_at") && !jsonObjectData["expires_at"].is_null()) {
			pDataStructure->expiresAt = jsonObjectData["expires_at"].get<std::string>();
		}

		if (jsonObjectData.contains("stage_instance") && !jsonObjectData["stage_instance"].is_null()) {
			pDataStructure->stageInstance = jsonObjectData["stage_instance"];
		}

		if (jsonObjectData.contains("guild_scheduled_event") && !jsonObjectData["guild_scheduled_event"].is_null()) {
			pDataStructure->guildScheduledEvent = jsonObjectData["guild_scheduled_event"];
		}

		if (jsonObjectData.contains("uses") && !jsonObjectData["uses"].is_null()) {
			pDataStructure->uses = jsonObjectData["uses"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_uses") && !jsonObjectData["max_uses"].is_null()) {
			pDataStructure->maxUses = jsonObjectData["max_uses"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_age") && !jsonObjectData["max_age"].is_null()) {
			pDataStructure->maxAge = jsonObjectData["max_age"].get<int32_t>();
		}

		if (jsonObjectData.contains("temporary") && !jsonObjectData["temporary"].is_null()) {
			pDataStructure->temporary = jsonObjectData["temporary"].get<bool>();
		}

		if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
			pDataStructure->createdAt = jsonObjectData["created_at"].get<std::string>();
		}
	}

	void ArchivedThreadsData::parseObjectReal(const nlohmann::json& jsonObjectData, ArchivedThreadsData* pDataStructure) {
		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			pDataStructure->threads.clear();
			pDataStructure->threads.reserve(jsonObjectData["threads"].size());
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{ value };
				pDataStructure->threads.push_back(newData);
			}
			pDataStructure->threads.shrink_to_fit();
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			pDataStructure->members.clear();
			pDataStructure->members.reserve(jsonObjectData["members"].size());
			for (auto& value: jsonObjectData["members"]) {
				ThreadMemberData newData{ value };
				pDataStructure->members.push_back(newData);
			}
			pDataStructure->members.shrink_to_fit();
		}

		if (jsonObjectData.contains("has_more") && !jsonObjectData["has_more"].is_null()) {
			pDataStructure->hasMore = jsonObjectData["has_more"].get<bool>();
		}
	}

	void ActiveThreadsData::parseObjectReal(const nlohmann::json& jsonObjectData, ActiveThreadsData* pDataStructure) {
		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			pDataStructure->threads.clear();
			pDataStructure->threads.reserve(jsonObjectData["threads"].size());
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{ value };
				pDataStructure->threads.push_back(newData);
			}
			pDataStructure->threads.shrink_to_fit();
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			pDataStructure->members.clear();
			pDataStructure->members.reserve(jsonObjectData["members"].size());
			for (auto& value: jsonObjectData["members"]) {
				ThreadMemberData newData{};
				pDataStructure->members.push_back(newData);
			}
			pDataStructure->members.shrink_to_fit();
		}

		if (jsonObjectData.contains("has_more") && !jsonObjectData["has_more"].is_null()) {
			pDataStructure->hasMore = jsonObjectData["has_more"].get<bool>();
		}
	}

	void GuildEmojisUpdateEventData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildEmojisUpdateEventData* pDataStructure) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
			pDataStructure->emojis.clear();
			pDataStructure->emojis.reserve(jsonObjectData["emojis"].size());
			for (auto& value: jsonObjectData["emojis"]) {
				EmojiData newData{ value };
				pDataStructure->emojis.push_back(newData);
			}
			pDataStructure->emojis.shrink_to_fit();
		}
	}

	void AuthorizationInfoData::parseObjectReal(const nlohmann::json& jsonObjectData, AuthorizationInfoData* pDataStructure) {
		if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
			pDataStructure->application = jsonObjectData["application"];
		}

		if (jsonObjectData.contains("scopes") && !jsonObjectData["scopes"].is_null()) {
			pDataStructure->scopes = jsonObjectData["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData.contains("expires") && !jsonObjectData["expires"].is_null()) {
			pDataStructure->expires = jsonObjectData["expires"];
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			pDataStructure->user = jsonObjectData["user"];
		}
	};

	void GuildStickersUpdateEventData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildStickersUpdateEventData* pDataStructure) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			pDataStructure->stickers.clear();
			pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{ value };
				pDataStructure->stickers.push_back(newData);
			}
			pDataStructure->stickers.shrink_to_fit();
		}
	}

	void Song::parseObjectReal(const nlohmann::json& jsonObjectData, Song* pDataStructure) {
		if (jsonObjectData.contains("lengthText") && !jsonObjectData["lengthText"].is_null()) {
			pDataStructure->duration = jsonObjectData["lengthText"]["accessibility"]["accessibilityData"]["label"].get<std::string>();
		}

		if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData["detailedMetadataSnippets"].is_null()) {
			for (auto& value: jsonObjectData["detailedMetadataSnippets"][0]["snippetText"]["runs"]) {
				std::string newString = value["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				pDataStructure->description = utf8MakeValid(newString);
			}
		}

		if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
			pDataStructure->thumbnailUrl = jsonObjectData["thumbnail"]["thumbnails"][0]["url"].get<std::string>();
		}

		if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
			if (jsonObjectData["title"].contains("runs")) {
				std::string newString = jsonObjectData["title"]["runs"][0]["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				pDataStructure->songTitle = utf8MakeValid(newString);
				;
			} else if (jsonObjectData["title"].contains("simpleText")) {
				std::string newString = jsonObjectData["title"]["simpleText"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				pDataStructure->songTitle = utf8MakeValid(newString);
			}
		}
		if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
			pDataStructure->songId = jsonObjectData["videoId"].get<std::string>();
		}

		if (jsonObjectData.contains("track_authorization") && !jsonObjectData["track_authorization"].is_null()) {
			pDataStructure->trackAuthorization = jsonObjectData["track_authorization"].get<std::string>();
		}

		if (jsonObjectData.contains("media") && !jsonObjectData["media"].is_null()) {
			bool isItFound{ false };
			for (auto& value: jsonObjectData["media"]["transcodings"]) {
				if (value["preset"] == "opus_0_0") {
					isItFound = true;
					pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
					pDataStructure->songId = value["url"].get<std::string>();
					pDataStructure->doWeGetSaved = true;
				}
			}
			bool isItFound2{ false };
			if (!isItFound) {
				for (auto& value: jsonObjectData["media"]["transcodings"]) {
					if (value["preset"] == "mp3_0_0") {
						pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
						pDataStructure->songId = value["url"].get<std::string>();
						isItFound2 = true;
					}
				}
			}
			if (!isItFound2 && !isItFound) {
				for (auto& value: jsonObjectData["media"]["transcodings"]) {
					pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
					pDataStructure->songId = value["url"].get<std::string>();
				}
			}
		}

		if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null() && !jsonObjectData["title"].is_object()) {
			std::string newString = jsonObjectData["title"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			pDataStructure->songTitle = utf8MakeValid(newString);
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			std::string newString = jsonObjectData["description"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			pDataStructure->description = utf8MakeValid(newString);
			pDataStructure->description += "...";
		}

		if (jsonObjectData.contains("artwork_url") && !jsonObjectData["artwork_url"].is_null()) {
			pDataStructure->thumbnailUrl = jsonObjectData["artwork_url"].get<std::string>();
		} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			if (jsonObjectData["user"].contains("avatar_url") && !jsonObjectData["user"]["avatar_url"].is_null()) {
				pDataStructure->thumbnailUrl = jsonObjectData["user"]["avatar_url"].get<std::string>();
			}
		}

		if (jsonObjectData.contains("duration") && !jsonObjectData["duration"].is_null()) {
			pDataStructure->duration = TimeStamp::convertMsToDurationString(jsonObjectData["duration"].get<int32_t>());
		}

		if (jsonObjectData.contains("permalink_url") && !jsonObjectData["permalink_url"].is_null()) {
			pDataStructure->viewUrl = jsonObjectData["permalink_url"].get<std::string>();
		}
	}	

	void GuildMembersChunkEventData::parseObjectReal(const nlohmann::json& jsonObjectData, GuildMembersChunkEventData* pDataStructure) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
			pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
		}

		if (jsonObjectData.contains("chunk_index") && !jsonObjectData["chunk_index"].is_null()) {
			pDataStructure->chunkIndex = jsonObjectData["chunk_index"].get<int32_t>();
		}

		if (jsonObjectData.contains("chunk_count") && !jsonObjectData["chunk_count"].is_null()) {
			pDataStructure->chunkCount = jsonObjectData["chunk_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
			pDataStructure->presences.clear();
			pDataStructure->presences.reserve(jsonObjectData["presences"].size());
			for (auto& value: jsonObjectData["presences"]) {
				PresenceUpdateData newData{ value };
				pDataStructure->presences.push_back(newData);
			}
			pDataStructure->presences.shrink_to_fit();
		}

		if (jsonObjectData.contains("not_found") && !jsonObjectData["not_found"].is_null()) {
			pDataStructure->notFound.clear();
			pDataStructure->notFound.reserve(jsonObjectData["not_found"].size());
			for (auto& value: jsonObjectData["not_found"]) {
				pDataStructure->notFound.push_back(value);
			}
			pDataStructure->notFound.shrink_to_fit();
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			pDataStructure->members.clear();
			pDataStructure->members.reserve(jsonObjectData["members"].size());
			for (auto& value: jsonObjectData["members"]) {
				GuildMemberData newData{ value };
				pDataStructure->members.push_back(newData);
			}
			pDataStructure->members.shrink_to_fit();
		}
	}

	void ConnectionDataVector::parseObjectReal(const nlohmann::json& jsonObjectData, ConnectionDataVector* pDataStructure) {
		pDataStructure->theConnectionDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::ConnectionData newData{ value };
			pDataStructure->theConnectionDatas.push_back(newData);
		}
		pDataStructure->theConnectionDatas.shrink_to_fit();
	}

	void InviteDataVector::parseObjectReal(const nlohmann::json& jsonObjectData, InviteDataVector* pDataStructure) {
		pDataStructure->theInviteDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			DiscordCoreAPI::InviteData newData{ value };
			pDataStructure->theInviteDatas.push_back(newData);
		}
		pDataStructure->theInviteDatas.shrink_to_fit();
	}

}