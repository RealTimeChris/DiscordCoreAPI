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