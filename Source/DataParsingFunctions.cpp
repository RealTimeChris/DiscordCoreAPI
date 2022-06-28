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



}