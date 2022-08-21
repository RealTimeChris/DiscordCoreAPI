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

#include <discordcoreapi/FromJson.hpp>
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

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, WelcomeScreenChannelData& theObject) {
		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("emoji_id") && !(*jsonObjectData)["emoji_id"].is_null()) {
			theObject.emojiId = stoull((*jsonObjectData)["emoji_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emoji_name") && !(*jsonObjectData)["emoji_name"].is_null()) {
			theObject.emojiName = (*jsonObjectData)["emoji_name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, WelcomeScreenData& theObject) {
		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("welcome_channels") && !(*jsonObjectData)["welcome_channels"].is_null()) {
			theObject.welcomeChannels.clear();
			for (auto& value: (*jsonObjectData)["welcome_channels"]) {
				WelcomeScreenChannelData newData{};
				parseObject(&value, newData);
				theObject.welcomeChannels.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, MessageActivityData& theObject) {
		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<MessageActivityType>();
		}

		if (jsonObjectData->contains("party_id") && !(*jsonObjectData)["party_id"].is_null()) {
			theObject.partyId = (*jsonObjectData)["party_id"].get<std::string>();
		}
	}
	
	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ActionMetaData& theObject) {
		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("duration_seconds") && !(*jsonObjectData)["duration_seconds"].is_null()) {
			theObject.durationSeconds = (*jsonObjectData)["duration_seconds"].get<int64_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventMetadata& theObject) {
		if (jsonObjectData->contains("location") && !(*jsonObjectData)["location"].is_null()) {
			theObject.location = (*jsonObjectData)["location"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ActionData& theObject) {
		if (jsonObjectData->contains("metadata") && !(*jsonObjectData)["metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["metadata"], theObject.metadata);
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = static_cast<ActionType>((*jsonObjectData)["type"].get<int64_t>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, TriggerMetaData& theObject) {
		if (jsonObjectData->contains("keyword_filter") && !(*jsonObjectData)["keyword_filter"].is_null()) {
			for (auto& value: (*jsonObjectData)["keyword_filter"]) {
				theObject.keywordFilter.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("presets") && !(*jsonObjectData)["presets"].is_null()) {
			for (auto& value: (*jsonObjectData)["presets"]) {
				theObject.presets.emplace_back(static_cast<KeywordPresetType>(value.get<uint64_t>()));
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ThreadMemberData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theObject.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("join_timestamp") && !(*jsonObjectData)["join_timestamp"].is_null()) {
			theObject.joinTimestamp = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["join_timestamp"].get<std::string>());
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theObject.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandInteractionDataOption& theObject) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ApplicationCommandOptionType>();
		}

		if (jsonObjectData->contains("focused") && !(*jsonObjectData)["focused"].is_null()) {
			theObject.focused = (*jsonObjectData)["focused"].get<bool>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			if ((*jsonObjectData)["value"].is_boolean()) {
				theObject.valueBool = (*jsonObjectData)["value"].get<bool>();
			}
			if ((*jsonObjectData)["value"].is_string()) {
				theObject.valueString = (*jsonObjectData)["value"].get<std::string>();
			}
			if ((*jsonObjectData)["value"].is_number()) {
				theObject.valueInt = (*jsonObjectData)["value"].get<int32_t>();
			}
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			theObject.options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandInteractionDataOption newData{};
				parseObject(&value, newData);
				theObject.options.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ThreadMetadataData& theObject) {
		if (jsonObjectData->contains("archived") && !(*jsonObjectData)["archived"].is_null()) {
			theObject.archived = (*jsonObjectData)["archived"].get<bool>();
		}

		if (jsonObjectData->contains("invitable") && !(*jsonObjectData)["invitable"].is_null()) {
			theObject.invitable = (*jsonObjectData)["invitable"].get<bool>();
		}

		if (jsonObjectData->contains("auto_archive_duration") && !(*jsonObjectData)["auto_archive_duration"].is_null()) {
			theObject.autoArchiveDuration = (*jsonObjectData)["auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData->contains("archive_timestamp") && !(*jsonObjectData)["archive_timestamp"].is_null()) {
			theObject.archiveTimestamp = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["archive_timestamp"].get<std::string>());
		}

		if (jsonObjectData->contains("locked") && !(*jsonObjectData)["locked"].is_null()) {
			theObject.locked = (*jsonObjectData)["locked"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ChannelMentionData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, UserData& theObject) {
		theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::MFAEnabled, getBoolReal(jsonObjectData, "mfa_enabled"));

		theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::Verified, getBoolReal(jsonObjectData, "verified"));

		theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::System, getBoolReal(jsonObjectData, "system"));

		theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::Bot, getBoolReal(jsonObjectData, "bot"));

		theObject.discriminator = getString(jsonObjectData, "discriminator");

		theObject.flags = getUint32(jsonObjectData, "public_flags");

		theObject.userName = getString(jsonObjectData, "username");

		theObject.id = strtoull(getString(jsonObjectData, "id"));

		theObject.avatar = getString(jsonObjectData, "avatar");
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventData& theObject) {
		if (jsonObjectData->contains("privacy_level") && !(*jsonObjectData)["privacy_level"].is_null()) {
			theObject.privacyLevel = (*jsonObjectData)["privacy_level"].get<GuildScheduledEventPrivacyLevel>();
		}

		if (jsonObjectData->contains("entity_type") && !(*jsonObjectData)["entity_type"].is_null()) {
			theObject.entityType = (*jsonObjectData)["entity_type"].get<GuildScheduledEventEntityType>();
		}

		if (jsonObjectData->contains("status") && !(*jsonObjectData)["status"].is_null()) {
			theObject.status = (*jsonObjectData)["status"].get<GuildScheduledEventStatus>();
		}

		if (jsonObjectData->contains("entity_metadata") && !(*jsonObjectData)["entity_metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["entity_metadata"], theObject.entityMetadata);
		}

		if (jsonObjectData->contains("scheduled_start_time") && !(*jsonObjectData)["scheduled_start_time"].is_null()) {
			theObject.scheduledStartTime = (*jsonObjectData)["scheduled_start_time"].get<std::string>();
		}

		if (jsonObjectData->contains("scheduled_end_time") && !(*jsonObjectData)["scheduled_end_time"].is_null()) {
			theObject.scheduledEndTime = (*jsonObjectData)["scheduled_end_time"].get<std::string>();
		}

		if (jsonObjectData->contains("user_count") && !(*jsonObjectData)["user_count"].is_null()) {
			theObject.userCount = (*jsonObjectData)["user_count"].get<uint32_t>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theObject.creatorId = (*jsonObjectData)["creator_id"].get<std::string>();
		}

		if (jsonObjectData->contains("entity_id") && !(*jsonObjectData)["entity_id"].is_null()) {
			theObject.entityId = (*jsonObjectData)["entity_id"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("creator") && !(*jsonObjectData)["creator"].is_null()) {
			parseObject(&(*jsonObjectData)["creator"], theObject.creator);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, TeamMembersObjectData& theObject) {
		if (jsonObjectData->contains("membership_state") && !(*jsonObjectData)["membership_state"].is_null()) {
			theObject.membershipState = (*jsonObjectData)["membership_state"].get<int32_t>();
		}

		if (jsonObjectData->contains("team_id") && !(*jsonObjectData)["team_id"].is_null()) {
			theObject.teamId = (*jsonObjectData)["team_id"].get<std::string>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theObject.permissions.clear();
			for (auto& value: (*jsonObjectData)["permissions"]) {
				theObject.permissions.emplace_back(Permissions{ value.get<std::string>() });
			}
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, TeamObjectData& theObject) {
		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theObject.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				TeamMembersObjectData newData{};
				parseObject(&value, newData);
				theObject.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("owner_user_id") && !(*jsonObjectData)["owner_user_id"].is_null()) {
			theObject.ownerUserId = (*jsonObjectData)["owner_user_id"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, InstallParamsData& theObject) {
		if (jsonObjectData->contains("scopes") && !(*jsonObjectData)["scopes"].is_null()) {
			theObject.scopes = (*jsonObjectData)["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theObject.permissions = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationData& theObject) {
		if (jsonObjectData->contains("params") && !(*jsonObjectData)["params"].is_null()) {
			parseObject(&(*jsonObjectData)["params"], theObject.params);
		}

		if (jsonObjectData->contains("tags") && !(*jsonObjectData)["tags"].is_null()) {
			theObject.tags = (*jsonObjectData)["tags"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("rpc_origins") && !(*jsonObjectData)["rpc_origins"].is_null()) {
			theObject.rpcOrigins.clear();
			for (auto& value: (*jsonObjectData)["rpc_origins"]) {
				theObject.rpcOrigins.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("bot_public") && !(*jsonObjectData)["bot_public"].is_null()) {
			theObject.botPublic = (*jsonObjectData)["bot_public"].get<bool>();
		}

		if (jsonObjectData->contains("bot_require_code_grant") && !(*jsonObjectData)["bot_require_code_grant"].is_null()) {
			theObject.botRequireCodeGrant = (*jsonObjectData)["bot_require_code_grant"].get<bool>();
		}

		if (jsonObjectData->contains("terms_of_service_url") && !(*jsonObjectData)["terms_of_service_url"].is_null()) {
			theObject.termsOfServiceUrl = (*jsonObjectData)["terms_of_service_url"].get<std::string>();
		}

		if (jsonObjectData->contains("privacy_policy_url") && !(*jsonObjectData)["privacy_policy_url"].is_null()) {
			theObject.privacyPolicyUrl = (*jsonObjectData)["privacy_policy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("owner") && !(*jsonObjectData)["owner"].is_null()) {
			parseObject(&(*jsonObjectData)["owner"], theObject.owner);
		}

		if (jsonObjectData->contains("summary") && !(*jsonObjectData)["summary"].is_null()) {
			theObject.summary = (*jsonObjectData)["summary"].get<std::string>();
		}

		if (jsonObjectData->contains("verify_key") && !(*jsonObjectData)["verify_key"].is_null()) {
			theObject.verifyKey = (*jsonObjectData)["verify_key"].get<std::string>();
		}

		if (jsonObjectData->contains("team") && !(*jsonObjectData)["team"].is_null()) {
			parseObject(&(*jsonObjectData)["team"], theObject.team);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("primary_sku_id") && !(*jsonObjectData)["primary_sku_id"].is_null()) {
			theObject.primarySkuId = (*jsonObjectData)["primary_sku_id"].get<std::string>();
		}

		if (jsonObjectData->contains("slug") && !(*jsonObjectData)["slug"].is_null()) {
			theObject.slug = (*jsonObjectData)["slug"].get<std::string>();
		}

		if (jsonObjectData->contains("cover_image") && !(*jsonObjectData)["cover_image"].is_null()) {
			theObject.coverImage = (*jsonObjectData)["cover_image"].get<std::string>();
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theObject.flags = (*jsonObjectData)["flags"].get<ApplicationFlags>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildMemberData& theObject) {
		theObject.flags = setBool<int8_t, GuildMemberFlags>(theObject.flags, GuildMemberFlags::Pending, getBoolReal(jsonObjectData, "pending"));

		theObject.flags = setBool<int8_t, GuildMemberFlags>(theObject.flags, GuildMemberFlags::Mute, getBoolReal(jsonObjectData, "mute"));

		theObject.flags = setBool<int8_t, GuildMemberFlags>(theObject.flags, GuildMemberFlags::Deaf, getBoolReal(jsonObjectData, "deaf"));

		theObject.joinedAt = TimeStamp<std::chrono::milliseconds>(getString(jsonObjectData, "joined_at"));

		theObject.guildId = strtoull(getString(jsonObjectData, "guild_id"));
		std::vector<std::string> theVector{};
		getVector(jsonObjectData, "roles", theVector);
		for (auto& value: theVector) {
			theObject.roles.push_back(stoull(std::move(value)));
		}

		theObject.permissions = getUint64(jsonObjectData, "permissions");

		std::unique_ptr<UserData> theUser = std::make_unique<UserData>();
		parseObject(&(*jsonObjectData)["user"], *theUser);
		theObject.id = theUser->id;
		auto theUserNew = theUser.get();
		theUserNew->insertUser(std::move(theUser));

		theObject.avatar = getString(jsonObjectData, "avatar");

		theObject.flags = getUint8(jsonObjectData, "flags");

		theObject.nick = getString(jsonObjectData, "nick");
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildMemberVector& theVector) {
		theVector.theGuildMembers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildMemberData newData{};
			parseObject(&value, newData);
			theVector.theGuildMembers.push_back(newData);
		}
		theVector.theGuildMembers.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, MessageInteractionData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<InteractionType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, RoleData& theObject) {
		theObject.id = strtoull(getString(jsonObjectData, "id"));

		theObject.name = getString(jsonObjectData, "name");

		std::stringstream theStream{};
		theStream << getString(jsonObjectData, "unicode_emoji");
		for (auto& value: theStream.str()) {
			theObject.unicodeEmoji.push_back(value);
		}
		if (theObject.unicodeEmoji.size() > 3) {
			theObject.unicodeEmoji = static_cast<std::string>(theObject.unicodeEmoji).substr(1, theObject.unicodeEmoji.size() - 3);
		}

		theObject.color = getUint32(jsonObjectData, "color");

		theObject.flags = setBool<int8_t, RoleFlags>(theObject.flags, RoleFlags::Hoist, getBoolReal(jsonObjectData, "hoist"));

		theObject.flags = setBool<int8_t, RoleFlags>(theObject.flags, RoleFlags::Managed, getBoolReal(jsonObjectData, "managed"));

		theObject.flags = setBool<int8_t, RoleFlags>(theObject.flags, RoleFlags::Mentionable, getBoolReal(jsonObjectData, "mentionable"));

		theObject.position = getUint16(jsonObjectData, "position");

		theObject.permissions = Permissions{ getString(jsonObjectData, "permissions") };
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmojiData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theObject.roles.clear();
			for (auto& value: (*jsonObjectData)["roles"]) {
				RoleData newData{};
				parseObject(&value, newData);
				theObject.roles.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("require_colons") && !(*jsonObjectData)["require_colons"].is_null()) {
			theObject.requireColons = (*jsonObjectData)["require_colons"].get<bool>();
		}

		if (jsonObjectData->contains("managed") && !(*jsonObjectData)["managed"].is_null()) {
			theObject.managed = (*jsonObjectData)["managed"].get<bool>();
		}

		if (jsonObjectData->contains("animated") && !(*jsonObjectData)["animated"].is_null()) {
			theObject.animated = (*jsonObjectData)["animated"].get<bool>();
		}

		if (jsonObjectData->contains("available") && !(*jsonObjectData)["available"].is_null()) {
			theObject.available = (*jsonObjectData)["available"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmojiDataVector& theObject) {
		theObject.theEmojiDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			EmojiData newData{};
			parseObject(&value, newData);
			theObject.theEmojiDatas.push_back(newData);
		}
		theObject.theEmojiDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, OverWriteData& theObject) {
		theObject.id = strtoull(getString(jsonObjectData, "id"));

		theObject.allow = strtoull(getString(jsonObjectData, "allow"));

		theObject.deny = strtoull(getString(jsonObjectData, "deny"));

		theObject.type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ChannelData& theObject) {
		theObject.id = strtoull(getString(jsonObjectData, "id"));

		theObject.flags = getUint8(jsonObjectData, "flags");

		theObject.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theObject.parentId = strtoull(getString(jsonObjectData, "parent_id"));

		theObject.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theObject.position = getUint32(jsonObjectData, "position");

		if (jsonObjectData->contains("permission_overwrites") && !(*jsonObjectData)["permission_overwrites"].is_null()) {
			theObject.permissionOverwrites.clear();
			theObject.permissionOverwrites.reserve((*jsonObjectData)["permission_overwrites"].size());
			for (auto& value: (*jsonObjectData)["permission_overwrites"]) {
				OverWriteData theData{};
				parseObject(&value, theData);
				theObject.permissionOverwrites.push_back(theData);
			}
		}

		theObject.name = getString(jsonObjectData, "name");

		theObject.flags = setBool<int8_t, ChannelFlags>(theObject.flags, ChannelFlags::NSFW, getBoolReal(jsonObjectData, "nsfw"));

		theObject.ownerId = strtoull(getString(jsonObjectData, "owner_id"));

		theObject.memberCount = getUint32(jsonObjectData, "member_count");
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, VoiceStateData& theObject) {
		theObject.requestToSpeakTimestamp = getString(jsonObjectData, "request_to_speak_timestamp");

		theObject.channelId = strtoull(getString(jsonObjectData, "channel_id"));

		theObject.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theObject.selfStream = getBoolReal(jsonObjectData, "self_stream");

		theObject.userId = strtoull(getString(jsonObjectData, "user_id"));

		theObject.selfVideo = getBoolReal(jsonObjectData, "self_video");

		theObject.sessionId = getString(jsonObjectData, "session_id");

		theObject.selfDeaf = getBoolReal(jsonObjectData, "self_deaf");

		theObject.selfMute = getBoolReal(jsonObjectData, "self_mute");

		theObject.suppress = getBoolReal(jsonObjectData, "suppress");

		theObject.deaf = getBoolReal(jsonObjectData, "deaf");

		theObject.mute = getBoolReal(jsonObjectData, "mute");
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildData& theObject) {
		theObject.id = strtoull(getString(jsonObjectData, "id"));

		theObject.icon = getString(jsonObjectData, "icon");

		theObject.name = getString(jsonObjectData, "name");

		theObject.joinedAt = TimeStamp<std::chrono::milliseconds>(getString(jsonObjectData, "joined_at"));

		theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::Owner, getBoolReal(jsonObjectData, "owner"));

		theObject.ownerId = strtoull(getString(jsonObjectData, "owner_id"));

		theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::WidgetEnabled, getBoolReal(jsonObjectData, "widget_enabled"));

		theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::Large, getBoolReal(jsonObjectData, "large"));

		theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::Unavailable, getBoolReal(jsonObjectData, "unavailable"));

		theObject.memberCount = getUint32(jsonObjectData, "member_count");

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theObject.roles.clear();
			for (auto& value: (*jsonObjectData)["roles"]) {
				std::unique_ptr<RoleData> newData{ std::make_unique<RoleData>() };
				parseObject(&value, *newData);
				theObject.roles.push_back(newData->id);
				auto theObject = newData.get();
				auto theRole = newData.get();
				theRole->insertRole(std::move(newData));
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theObject.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				std::unique_ptr<GuildMemberData> newData{ std::make_unique<GuildMemberData>() };
				parseObject(&value, *newData);
				newData->guildId = theObject.id;
				theObject.members.push_back(newData.release());
			}
		}

		if (jsonObjectData->contains("voice_states") && !(*jsonObjectData)["voice_states"].is_null()) {
			for (auto& value: (*jsonObjectData)["voice_states"]) {
				auto userId = strtoull(value["user_id"].get<std::string>());
				for (auto& value02: theObject.members) {
					if (value02->id == userId) {
						value02->voiceChannelId = strtoull(value["channel_id"].get<std::string>());
					}
				}
			}
		}

		if (jsonObjectData->contains("channels") && !(*jsonObjectData)["channels"].is_null()) {
			theObject.channels.clear();
			for (auto& value: (*jsonObjectData)["channels"]) {
				std::unique_ptr<ChannelData> newData{ std::make_unique<ChannelData>() };
				parseObject(&value, *newData);
				newData->guildId = theObject.id;
				theObject.channels.push_back(newData->id);
				auto theChannel = newData.get();
				theChannel->insertChannel(std::move(newData));
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, PartyData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("Size") && !(*jsonObjectData)["Size"].is_null()) {
			theObject.size[0] = (*jsonObjectData)["Size"]["CurrentSize"].get<int32_t>();
			theObject.size[1] = (*jsonObjectData)["Size"]["MaxSize"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AssetsData& theObject) {
		if (jsonObjectData->contains("LargeImage") && !(*jsonObjectData)["LargeImage"].is_null()) {
			theObject.largeImage = (*jsonObjectData)["LargeImage"].get<std::string>();
		}

		if (jsonObjectData->contains("LargeText") && !(*jsonObjectData)["LargeText"].is_null()) {
			theObject.largeText = (*jsonObjectData)["LargeText"].get<std::string>();
		}

		if (jsonObjectData->contains("SmallImage") && !(*jsonObjectData)["SmallImage"].is_null()) {
			theObject.smallImage = (*jsonObjectData)["SmallImage"].get<std::string>();
		}

		if (jsonObjectData->contains("SmallText") && !(*jsonObjectData)["SmallText"].is_null()) {
			theObject.smallText = (*jsonObjectData)["SmallText"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, SecretsData& theObject) {
		if (jsonObjectData->contains("Join") && !(*jsonObjectData)["Join"].is_null()) {
			theObject.join = (*jsonObjectData)["Join"].get<std::string>();
		}

		if (jsonObjectData->contains("Spectate") && !(*jsonObjectData)["Spectate"].is_null()) {
			theObject.spectate = (*jsonObjectData)["Spectate"].get<std::string>();
		}

		if (jsonObjectData->contains("Match") && !(*jsonObjectData)["Match"].is_null()) {
			theObject.match = (*jsonObjectData)["Match"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, TimestampData& theObject) {
		if (jsonObjectData->contains("Start") && !(*jsonObjectData)["Start"].is_null()) {
			theObject.start = (*jsonObjectData)["Start"].get<int64_t>();
		}

		if (jsonObjectData->contains("End") && !(*jsonObjectData)["End"].is_null()) {
			theObject.end = (*jsonObjectData)["End"].get<int64_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ActivityData& theObject) {
		if (jsonObjectData->contains("Name") && !(*jsonObjectData)["Name"].is_null()) {
			theObject.name = (*jsonObjectData)["Name"].get<std::string>();
		}

		if (jsonObjectData->contains("Timestamps") && !(*jsonObjectData)["Timestamps"].is_null()) {
			parseObject(&(*jsonObjectData)["Timestamps"], theObject.timestamps);
		}

		if (jsonObjectData->contains("ApplicationId") && !(*jsonObjectData)["ApplicationId"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["ApplicationId"].get<std::string>());
		}

		if (jsonObjectData->contains("Details") && !(*jsonObjectData)["Details"].is_null()) {
			theObject.details = (*jsonObjectData)["Details"].get<std::string>();
		}

		if (jsonObjectData->contains("State") && !(*jsonObjectData)["State"].is_null()) {
			theObject.state = (*jsonObjectData)["State"].get<std::string>();
		}

		if (jsonObjectData->contains("ActivityParty") && !(*jsonObjectData)["ActivityParty"].is_null()) {
			parseObject(&(*jsonObjectData)["ActivityParty"], theObject.party);
		}

		if (jsonObjectData->contains("ActivityAssets") && !(*jsonObjectData)["ActivityAssets"].is_null()) {
			parseObject(&(*jsonObjectData)["ActivityAssets"], theObject.assets);
		}

		if (jsonObjectData->contains("ActivitySecrets") && !(*jsonObjectData)["ActivitySecrets"].is_null()) {
			parseObject(&(*jsonObjectData)["ActivitySecrets"], theObject.secrets);
		}

		if (jsonObjectData->contains("Instance") && !(*jsonObjectData)["Instance"].is_null()) {
			theObject.instance = (*jsonObjectData)["Instance"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ClientStatusData& theObject) {
		if (jsonObjectData->contains("desktop") && !(*jsonObjectData)["desktop"].is_null()) {
			theObject.desktop = (*jsonObjectData)["desktop"].get<std::string>();
		}

		if (jsonObjectData->contains("mobile") && !(*jsonObjectData)["mobile"].is_null()) {
			theObject.mobile = (*jsonObjectData)["mobile"].get<std::string>();
		}

		if (jsonObjectData->contains("web") && !(*jsonObjectData)["web"].is_null()) {
			theObject.web = (*jsonObjectData)["web"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, PresenceUpdateData& theObject) {
		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null() && (*jsonObjectData)["guild_id"].get<std::string>() != "") {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("status") && !(*jsonObjectData)["status"].is_null()) {
			theObject.status = (*jsonObjectData)["status"].get<std::string>();
		}

		if (jsonObjectData->contains("activities") && !(*jsonObjectData)["activities"].is_null()) {
			theObject.activities.clear();
			for (auto& value: (*jsonObjectData)["activities"]) {
				ActivityData newData{};
				parseObject(&value, newData);
				theObject.activities.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("client_status") && !(*jsonObjectData)["client_status"].is_null()) {
			parseObject(&(*jsonObjectData)["client_status"], theObject.clientStatus);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Guild& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("afk_channel_id") && !(*jsonObjectData)["afk_channel_id"].is_null()) {
			theObject.afkChannelId = stoull((*jsonObjectData)["afk_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("icon_hash") && !(*jsonObjectData)["icon_hash"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon_hash"].get<std::string>();
		}

		if (jsonObjectData->contains("splash") && !(*jsonObjectData)["splash"].is_null()) {
			theObject.splash = (*jsonObjectData)["splash"].get<std::string>();
		}

		if (jsonObjectData->contains("discovery_splash") && !(*jsonObjectData)["discovery_splash"].is_null()) {
			theObject.discoverySplash = (*jsonObjectData)["discovery_splash"].get<std::string>();
		}

		if (jsonObjectData->contains("owner") && !(*jsonObjectData)["owner"].is_null()) {
			theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::Owner, (*jsonObjectData)["owner"].get<bool>());
		}

		if (jsonObjectData->contains("owner_id") && !(*jsonObjectData)["owner_id"].is_null()) {
			theObject.ownerId = stoull((*jsonObjectData)["owner_id"].get<std::string>());
		}

		if (jsonObjectData->contains("preferred_locale") && !(*jsonObjectData)["preferred_locale"].is_null()) {
			theObject.preferredLocale = (*jsonObjectData)["preferred_locale"].get<std::string>();
		}

		if (jsonObjectData->contains("public_updates_channel_id") && !(*jsonObjectData)["public_updates_channel_id"].is_null()) {
			theObject.publicUpdatesChannelId = stoull((*jsonObjectData)["public_updates_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("vanity_url_code") && !(*jsonObjectData)["vanity_url_code"].is_null()) {
			theObject.vanityUrlCode = (*jsonObjectData)["vanity_url_code"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("banner") && !(*jsonObjectData)["banner"].is_null()) {
			theObject.banner = (*jsonObjectData)["banner"].get<std::string>();
		}

		if (jsonObjectData->contains("rule_Channel_id") && !(*jsonObjectData)["rule_Channel_id"].is_null()) {
			theObject.rulesChannelId = stoull((*jsonObjectData)["rule_Channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("joined_at") && !(*jsonObjectData)["joined_at"].is_null()) {
			theObject.joinedAt = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["joined_at"].get<std::string>());
		}

		if (jsonObjectData->contains("widget_channel_id") && !(*jsonObjectData)["widget_channel_id"].is_null()) {
			theObject.widgetChannelId = stoull((*jsonObjectData)["widget_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("system_channel_id") && !(*jsonObjectData)["system_channel_id"].is_null()) {
			theObject.systemChannelId = stoull((*jsonObjectData)["system_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("region") && !(*jsonObjectData)["region"].is_null()) {
			theObject.region = (*jsonObjectData)["region"].get<std::string>();
		}

		if (jsonObjectData->contains("afk_channel_id") && !(*jsonObjectData)["afk_channel_id"].is_null()) {
			theObject.afkChannelId = stoull((*jsonObjectData)["afk_channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("region") && !(*jsonObjectData)["region"].is_null()) {
			theObject.region = (*jsonObjectData)["region"].get<std::string>();
		}

		if (jsonObjectData->contains("features") && !(*jsonObjectData)["features"].is_null()) {
			for (auto& value: (*jsonObjectData)["features"].get<std::vector<std::string>>()) {
				theObject.features.emplace_back(StringWrapper{ value });
			}
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theObject.permissions = (*jsonObjectData)["permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theObject.roles.clear();
			theObject.roles.reserve((*jsonObjectData)["roles"].size());
			for (auto& value: (*jsonObjectData)["roles"]) {
				std::unique_ptr<RoleData> newData{ std::make_unique<RoleData>() };
				parseObject(&value, *newData);
				theObject.roles.push_back(newData->id);
				Roles::insertRole(std::move(newData));
			}
		}

		if (jsonObjectData->contains("afk_timeout") && !(*jsonObjectData)["afk_timeout"].is_null()) {
			theObject.afkTimeOut = (*jsonObjectData)["afk_timeout"].get<AfkTimeOutDurations>();
		}

		if (jsonObjectData->contains("owner") && !(*jsonObjectData)["owner"].is_null()) {
			theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::Owner, (*jsonObjectData)["owner"].get<bool>());
		}

		if (jsonObjectData->contains("widget_enabled") && !(*jsonObjectData)["widget_enabled"].is_null()) {
			theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::WidgetEnabled, (*jsonObjectData)["widget_enabled"].get<bool>());
		}

		if (jsonObjectData->contains("verification_level") && !(*jsonObjectData)["verification_level"].is_null()) {
			theObject.verificationLevel = (*jsonObjectData)["verification_level"].get<VerificationLevel>();
		}

		if (jsonObjectData->contains("default_message_notification_level") && !(*jsonObjectData)["default_message_notification_level"].is_null()) {
			theObject.defaultMessageNotifications = (*jsonObjectData)["default_message_notification_level"].get<DefaultMessageNotificationLevel>();
		}

		if (jsonObjectData->contains("explicit_content_filter_level") && !(*jsonObjectData)["explicit_content_filter_level"].is_null()) {
			theObject.explicitContentFilter = (*jsonObjectData)["explicit_content_filter_level"].get<ExplicitContentFilterLevel>();
		}

		if (jsonObjectData->contains("mfa_level") && !(*jsonObjectData)["mfa_level"].is_null()) {
			theObject.mfaLevel = (*jsonObjectData)["mfa_level"].get<MFALevel>();
		}

		if (jsonObjectData->contains("system_channel_flags") && !(*jsonObjectData)["system_channel_flags"].is_null()) {
			theObject.systemChannelFlags = (*jsonObjectData)["system_channel_flags"].get<SystemChannelFlags>();
		}

		if (jsonObjectData->contains("large") && !(*jsonObjectData)["large"].is_null()) {
			theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::Large, (*jsonObjectData)["large"].get<bool>());
		}

		if (jsonObjectData->contains("unavailable") && !(*jsonObjectData)["unavailable"].is_null()) {
			theObject.flags = setBool<int8_t, GuildFlags>(theObject.flags, GuildFlags::Unavailable, (*jsonObjectData)["unavailable"].get<bool>());
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			theObject.memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theObject.members.clear();
			theObject.members.reserve((*jsonObjectData)["members"].size());
			for (auto& value: (*jsonObjectData)["members"]) {
				std::unique_ptr<GuildMemberData> newData{ std::make_unique<GuildMemberData>() };
				parseObject(&value, *newData);
				newData->guildId = theObject.id;
				auto userId = newData->id;
				theObject.members.push_back(newData.release());
			}
		}

		if (jsonObjectData->contains("channels") && !(*jsonObjectData)["channels"].is_null()) {
			theObject.channels.clear();
			theObject.channels.reserve((*jsonObjectData)["channels"].size());
			for (auto& value: (*jsonObjectData)["channels"]) {
				std::unique_ptr<ChannelData> newData{ std::make_unique<ChannelData>() };
				parseObject(&value, *newData);
				newData->guildId = theObject.id;
				theObject.channels.push_back(newData->id);
				Channels::insertChannel(std::move(newData));
			}
		}

		if (jsonObjectData->contains("presences") && !(*jsonObjectData)["presences"].is_null()) {
			theObject.presences.clear();
			for (auto& value: (*jsonObjectData)["presences"]) {
				PresenceUpdateData newData{};
				parseObject(&value, newData);
				theObject.presences[newData.user.id] = newData;
			}
		}

		if (jsonObjectData->contains("max_presences") && !(*jsonObjectData)["max_presences"].is_null()) {
			theObject.maxPresences = (*jsonObjectData)["max_presences"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_members") && !(*jsonObjectData)["max_members"].is_null()) {
			theObject.maxMembers = (*jsonObjectData)["max_members"].get<int32_t>();
		}

		if (jsonObjectData->contains("premium_subscription_count") && !(*jsonObjectData)["premium_subscription_count"].is_null()) {
			theObject.premiumSubscriptionCount = (*jsonObjectData)["premium_subscription_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("premium_tier") && !(*jsonObjectData)["premium_tier"].is_null()) {
			theObject.premiumTier = (*jsonObjectData)["premium_tier"].get<PremiumTier>();
		}

		if (jsonObjectData->contains("max_video_channel_users") && !(*jsonObjectData)["max_video_channel_users"].is_null()) {
			theObject.maxVideoChannelUsers = (*jsonObjectData)["max_video_channel_users"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			theObject.approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			theObject.approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("welcome_screen") && !(*jsonObjectData)["welcome_screen"].is_null()) {
			parseObject(&(*jsonObjectData)["welcome_screen"], theObject.welcomeScreen);
		}

		if (jsonObjectData->contains("nsfw_level") && !(*jsonObjectData)["nsfw_level"].is_null()) {
			theObject.nsfwLevel = (*jsonObjectData)["nsfw_level"].get<GuildNSFWLevel>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandOptionChoiceData& theObject) {
		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null() && (*jsonObjectData)["value"].is_string()) {
			if ((*jsonObjectData)["value"].is_string()) {
				theObject.value = (*jsonObjectData)["value"].get<std::string>();
			} else if ((*jsonObjectData)["value"].is_number_float()) {
				theObject.value = (*jsonObjectData)["value"].get<float>();
			} else if ((*jsonObjectData)["value"].is_number()) {
				theObject.value = (*jsonObjectData)["value"].get<int32_t>();
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			theObject.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theObject.nameLocalizations[key] = newValue;
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandOptionData& theObject) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			theObject.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theObject.nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description_localizations") && !(*jsonObjectData)["description_localizations"].is_null()) {
			theObject.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["description_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theObject.descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_types") && !(*jsonObjectData)["channel_types"].is_null()) {
			theObject.channelTypes.clear();
			theObject.channelTypes.reserve((*jsonObjectData)["channel_types"].size());
			for (auto& value: (*jsonObjectData)["channel_types"]) {
				theObject.channelTypes.push_back(value);
			}
		}

		if (jsonObjectData->contains("min_value") && !(*jsonObjectData)["min_value"].is_null()) {
			theObject.minValue = (*jsonObjectData)["min_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_value") && !(*jsonObjectData)["max_value"].is_null()) {
			theObject.maxValue = (*jsonObjectData)["max_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("required") && !(*jsonObjectData)["required"].is_null()) {
			theObject.required = (*jsonObjectData)["required"].get<bool>();
		}

		if (jsonObjectData->contains("autocomplete") && !(*jsonObjectData)["autocomplete"].is_null()) {
			theObject.autocomplete = (*jsonObjectData)["autocomplete"].get<bool>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ApplicationCommandOptionType>();
		}

		if (theObject.type == ApplicationCommandOptionType::Sub_Command_Group || theObject.type == ApplicationCommandOptionType::Sub_Command) {
			if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
				theObject.options.clear();
				for (auto& value: (*jsonObjectData)["options"]) {
					ApplicationCommandOptionData newData{};
					parseObject(&value, newData);
					theObject.options.emplace_back(newData);
				}
			}
		} else {
			if (jsonObjectData->contains("choices") && !(*jsonObjectData)["choices"].is_null()) {
				theObject.choices.clear();
				for (auto& value: (*jsonObjectData)["choices"]) {
					ApplicationCommandOptionChoiceData newData{};
					parseObject(&value, newData);
					theObject.choices.emplace_back(newData);
				}
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommand& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("default_member_permissions") && !(*jsonObjectData)["default_member_permissions"].is_null()) {
			theObject.defaultMemberPermissions = (*jsonObjectData)["default_member_permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("dm_permission") && !(*jsonObjectData)["dm_permission"].is_null()) {
			theObject.dmPermission = (*jsonObjectData)["dm_permission"].get<bool>();
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theObject.version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			theObject.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theObject.nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description_localizations") && !(*jsonObjectData)["description_localizations"].is_null()) {
			theObject.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["description_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theObject.descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theObject.version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			theObject.options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandOptionData newData{};
				parseObject(&value, newData);
				theObject.options.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandVector& theObject) {
		theObject.theApplicationCommands.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ApplicationCommand newData{};
			parseObject(&value, newData);
			theObject.theApplicationCommands.push_back(newData);
		}
		theObject.theApplicationCommands.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AutoModerationActionExecutionEventData& theObject) {
		if (jsonObjectData->contains("alert_system_message_id") && !(*jsonObjectData)["alert_system_message_id"].is_null()) {
			theObject.alertSystemMessageId = stoull((*jsonObjectData)["alert_system_message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("rule_trigger_type") && !(*jsonObjectData)["rule_trigger_type"].is_null()) {
			theObject.ruleTriggerType = (*jsonObjectData)["rule_trigger_type"].get<TriggerType>();
		}

		if (jsonObjectData->contains("matched_keyword") && !(*jsonObjectData)["matched_keyword"].is_null()) {
			theObject.matchedKeyword = (*jsonObjectData)["matched_keyword"].get<std::string>();
		}

		if (jsonObjectData->contains("matched_content") && !(*jsonObjectData)["matched_content"].is_null()) {
			theObject.matchedContent = (*jsonObjectData)["matched_content"].get<std::string>();
		}

		if (jsonObjectData->contains("action") && !(*jsonObjectData)["action"].is_null()) {
			parseObject(&(*jsonObjectData)["action"], theObject.action);
		}

		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theObject.content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theObject.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("rule_id") && !(*jsonObjectData)["rule_id"].is_null()) {
			theObject.ruleId = stoull((*jsonObjectData)["rule_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theObject.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AutoModerationRule& theObject) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			theObject.enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("trigger_type") && !(*jsonObjectData)["trigger_type"].is_null()) {
			theObject.triggerType = static_cast<TriggerType>((*jsonObjectData)["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("event_type") && !(*jsonObjectData)["event_type"].is_null()) {
			theObject.eventType = static_cast<EventType>((*jsonObjectData)["event_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theObject.creatorId = (*jsonObjectData)["creator_id"].get<uint64_t>();
		}

		if (jsonObjectData->contains("actions") && !(*jsonObjectData)["actions"].is_null()) {
			theObject.actions.clear();
			for (auto& value: (*jsonObjectData)["actions"]) {
				ActionData newData{};
				parseObject(&value, newData);
				theObject.actions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("exempt_roles") && !(*jsonObjectData)["exempt_roles"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_roles"]) {
				theObject.exemptRoles.emplace_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData->contains("trigger_metadata") && !(*jsonObjectData)["trigger_metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["trigger_metadata"], theObject.triggerMetaData);
		}

		if (jsonObjectData->contains("exempt_channels") && !(*jsonObjectData)["exempt_channels"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_channels"]) {
				theObject.exemptChannels.emplace_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = (*jsonObjectData)["guild_id"].get<uint64_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AutoModerationRuleVector& theObject) {
		theObject.theAutoModerationRules.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			AutoModerationRule newData{};
			parseObject(&value, newData);
			theObject.theAutoModerationRules.push_back(newData);
		}
		theObject.theAutoModerationRules.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Channel& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				theObject.id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theObject.flags = (*jsonObjectData)["flags"].get<int8_t>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("default_auto_archive_duration") && !(*jsonObjectData)["default_auto_archive_duration"].is_null()) {
			theObject.defaultAutoArchiveDuration = (*jsonObjectData)["default_auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData->contains("position") && !(*jsonObjectData)["position"].is_null()) {
			theObject.position = (*jsonObjectData)["position"].get<int32_t>();
		}

		if (jsonObjectData->contains("permission_overwrites") && !(*jsonObjectData)["permission_overwrites"].is_null()) {
			theObject.permissionOverwrites.clear();
			for (auto& value: (*jsonObjectData)["permission_overwrites"]) {
				OverWriteData newData{};
				uint64_t overWriteId = newData.id;
				parseObject(&value, newData);
				theObject.permissionOverwrites[overWriteId] = newData;
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			theObject.topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theObject.permissions = (*jsonObjectData)["permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("nsfw") && !(*jsonObjectData)["nsfw"].is_null()) {
			theObject.flags = setBool<int8_t, ChannelFlags>(theObject.flags, ChannelFlags::NSFW, (*jsonObjectData)["nsfw"].get<bool>());
		}

		if (jsonObjectData->contains("last_message_id") && !(*jsonObjectData)["last_message_id"].is_null()) {
			theObject.lastMessageId = (*jsonObjectData)["last_message_id"].get<std::string>();
		}

		if (jsonObjectData->contains("bitrate") && !(*jsonObjectData)["bitrate"].is_null()) {
			theObject.bitrate = (*jsonObjectData)["bitrate"].get<int32_t>();
		}

		if (jsonObjectData->contains("user_limit") && !(*jsonObjectData)["user_limit"].is_null()) {
			theObject.userLimit = (*jsonObjectData)["user_limit"].get<int32_t>();
		}

		if (jsonObjectData->contains("rate_limit_per_user") && !(*jsonObjectData)["rate_limit_per_user"].is_null()) {
			theObject.rateLimitPerUser = (*jsonObjectData)["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData->contains("recipients") && !(*jsonObjectData)["recipients"].is_null()) {
			theObject.recipients.clear();
			for (auto& value: (*jsonObjectData)["recipients"]) {
				UserData newData{};
				parseObject(&value, newData);
				theObject.recipients[newData.id] = std::move(newData);
			}
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("owner_id") && !(*jsonObjectData)["owner_id"].is_null()) {
			theObject.ownerId = stoull((*jsonObjectData)["owner_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("parent_id") && !(*jsonObjectData)["parent_id"].is_null()) {
			if ((*jsonObjectData)["parent_id"].is_string()) {
				theObject.parentId = stoull((*jsonObjectData)["parent_id"].get<std::string>());
			} else {
				theObject.parentId = (*jsonObjectData)["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			theObject.lastPinTimestamp = (*jsonObjectData)["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("rtc_region") && !(*jsonObjectData)["rtc_region"].is_null()) {
			theObject.rtcRegion = (*jsonObjectData)["rtc_region"].get<std::string>();
		}

		if (jsonObjectData->contains("video_quality_mode") && !(*jsonObjectData)["video_quality_mode"].is_null()) {
			theObject.videoQualityMode = (*jsonObjectData)["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData->contains("message_count") && !(*jsonObjectData)["message_count"].is_null()) {
			theObject.messageCount = (*jsonObjectData)["message_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			theObject.memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("thread_metadata") && !(*jsonObjectData)["thread_metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["thread_metadata"], theObject.threadMetadata);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ChannelVector& theObject) {
		theObject.theChannels.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ChannelData newData{};
			parseObject(&value, newData);
			theObject.theChannels.push_back(newData);
		}
		theObject.theChannels.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildVector& theObject) {
		theObject.theGuilds.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildData newData{};
			parseObject(&value, newData);
			theObject.theGuilds.push_back(newData);
		}
		theObject.theGuilds.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEvent& theObject) {
		if (jsonObjectData->contains("privacy_level") && !(*jsonObjectData)["privacy_level"].is_null()) {
			theObject.privacyLevel = (*jsonObjectData)["privacy_level"].get<GuildScheduledEventPrivacyLevel>();
		}

		if (jsonObjectData->contains("entity_type") && !(*jsonObjectData)["entity_type"].is_null()) {
			theObject.entityType = (*jsonObjectData)["entity_type"].get<GuildScheduledEventEntityType>();
		}

		if (jsonObjectData->contains("status") && !(*jsonObjectData)["status"].is_null()) {
			theObject.status = (*jsonObjectData)["status"].get<GuildScheduledEventStatus>();
		}

		if (jsonObjectData->contains("entity_metadata") && !(*jsonObjectData)["entity_metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["entity_metadata"], theObject.entityMetadata);
		}

		if (jsonObjectData->contains("scheduled_start_time") && !(*jsonObjectData)["scheduled_start_time"].is_null()) {
			theObject.scheduledStartTime = (*jsonObjectData)["scheduled_start_time"].get<std::string>();
		}

		if (jsonObjectData->contains("scheduled_end_time") && !(*jsonObjectData)["scheduled_end_time"].is_null()) {
			theObject.scheduledEndTime = (*jsonObjectData)["scheduled_end_time"].get<std::string>();
		}

		if (jsonObjectData->contains("user_count") && !(*jsonObjectData)["user_count"].is_null()) {
			theObject.userCount = (*jsonObjectData)["user_count"].get<uint32_t>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theObject.creatorId = (*jsonObjectData)["creator_id"].get<std::string>();
		}

		if (jsonObjectData->contains("entity_id") && !(*jsonObjectData)["entity_id"].is_null()) {
			theObject.entityId = (*jsonObjectData)["entity_id"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("creator") && !(*jsonObjectData)["creator"].is_null()) {
			parseObject(&(*jsonObjectData)["creator"], theObject.creator);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Reaction& theObject) {
		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			theObject.count = (*jsonObjectData)["count"].get<int32_t>();
		}

		if (jsonObjectData->contains("me") && !(*jsonObjectData)["me"].is_null()) {
			theObject.count = (*jsonObjectData)["me"].get<bool>();
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			parseObject(&(*jsonObjectData)["emoji"], theObject.emoji);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theObject.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theObject.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ReactionVector& theObject) {
		theObject.theReactions.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Reaction newData{};
			parseObject(&value, newData);
			theObject.theReactions.push_back(newData);
		}
		theObject.theReactions.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, RoleVector& theObject) {
		theObject.theRoles.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			RoleData newData{};
			parseObject(&value, newData);
			theObject.theRoles.push_back(newData);
		}
		theObject.theRoles.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, StageInstance& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			theObject.topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("privacy_level") && !(*jsonObjectData)["privacy_level"].is_null()) {
			theObject.privacyLevel = (*jsonObjectData)["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		if (jsonObjectData->contains("discoverable_disabled") && !(*jsonObjectData)["discoverable_disabled"].is_null()) {
			theObject.discoverableDisabled = (*jsonObjectData)["discoverable_disabled"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Sticker& theObject) {
		if (jsonObjectData->contains("asset") && !(*jsonObjectData)["asset"].is_null()) {
			theObject.asset = (*jsonObjectData)["asset"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			theObject.formatType = (*jsonObjectData)["format_type"].get<StickerFormatType>();
		}

		if (jsonObjectData->contains("available") && !(*jsonObjectData)["available"].is_null()) {
			theObject.stickerFlags = setBool<int32_t, StickerFlags>(theObject.stickerFlags, StickerFlags::Available, (*jsonObjectData)["available"].get<bool>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("pack_id") && !(*jsonObjectData)["pack_id"].is_null()) {
			theObject.packId = (*jsonObjectData)["pack_id"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<StickerType>();
		}

		if (jsonObjectData->contains("sort_value") && !(*jsonObjectData)["sort_value"].is_null()) {
			theObject.sortValue = (*jsonObjectData)["sort_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, StickerVector& theObject) {
		theObject.theStickers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Sticker newData{};
			parseObject(&value, newData);
			theObject.theStickers.push_back(newData);
		}
		theObject.theStickers.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Thread& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				theObject.id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("position") && !(*jsonObjectData)["position"].is_null()) {
			theObject.position = (*jsonObjectData)["position"].get<int32_t>();
		}

		if (jsonObjectData->contains("permission_overwrites") && !(*jsonObjectData)["permission_overwrites"].is_null()) {
			theObject.permissionOverwrites.clear();
			for (auto& value: (*jsonObjectData)["permission_overwrites"]) {
				OverWriteData newData{};
				parseObject(&value, newData);
				theObject.permissionOverwrites.push_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			theObject.topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("nsfw") && !(*jsonObjectData)["nsfw"].is_null()) {
			theObject.flags = setBool<int8_t, ChannelFlags>(theObject.flags, ChannelFlags::NSFW, (*jsonObjectData)["nsfw"].get<bool>());
		}

		if (jsonObjectData->contains("last_message_id") && !(*jsonObjectData)["last_message_id"].is_null()) {
			theObject.lastMessageId = (*jsonObjectData)["last_message_id"].get<std::string>();
		}

		if (jsonObjectData->contains("bitrate") && !(*jsonObjectData)["bitrate"].is_null()) {
			theObject.bitrate = (*jsonObjectData)["bitrate"].get<int32_t>();
		}

		if (jsonObjectData->contains("user_limit") && !(*jsonObjectData)["user_limit"].is_null()) {
			theObject.userLimit = (*jsonObjectData)["user_limit"].get<int32_t>();
		}

		if (jsonObjectData->contains("rate_limit_per_user") && !(*jsonObjectData)["rate_limit_per_user"].is_null()) {
			theObject.rateLimitPerUser = (*jsonObjectData)["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData->contains("recipients") && !(*jsonObjectData)["recipients"].is_null()) {
			theObject.recipients.clear();
			for (auto& value: (*jsonObjectData)["recipients"]) {
				UserData newData{};
				parseObject(&value, newData);
				theObject.recipients[newData.id] = std::move(newData);
			}
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("owner_id") && !(*jsonObjectData)["owner_id"].is_null()) {
			theObject.ownerId = stoull((*jsonObjectData)["owner_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("parent_id") && !(*jsonObjectData)["parent_id"].is_null()) {
			if ((*jsonObjectData)["parent_id"].is_string()) {
				theObject.parentId = stoull((*jsonObjectData)["parent_id"].get<std::string>());
			} else {
				theObject.parentId = (*jsonObjectData)["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			theObject.lastPinTimestamp = (*jsonObjectData)["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("rtc_region") && !(*jsonObjectData)["rtc_region"].is_null()) {
			theObject.rtcRegion = (*jsonObjectData)["rtc_region"].get<std::string>();
		}

		if (jsonObjectData->contains("video_quality_mode") && !(*jsonObjectData)["video_quality_mode"].is_null()) {
			theObject.videoQualityMode = (*jsonObjectData)["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData->contains("message_count") && !(*jsonObjectData)["message_count"].is_null()) {
			theObject.messageCount = (*jsonObjectData)["message_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			theObject.memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("thread_metadata") && !(*jsonObjectData)["thread_metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["thread_metadata"], theObject.threadMetadata);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, User& theObject) {
		if (jsonObjectData->contains("username") && !(*jsonObjectData)["username"].is_null()) {
			theObject.userName = (*jsonObjectData)["username"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				theObject.id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("accent_color") && !(*jsonObjectData)["accent_color"].is_null()) {
			theObject.accentColor = (*jsonObjectData)["accent_color"].get<int32_t>();
		}

		if (jsonObjectData->contains("banner") && !(*jsonObjectData)["banner"].is_null()) {
			theObject.banner = (*jsonObjectData)["banner"].get<std::string>();
		}

		if (jsonObjectData->contains("discriminator") && !(*jsonObjectData)["discriminator"].is_null()) {
			theObject.discriminator = (*jsonObjectData)["discriminator"].get<std::string>();
		}

		if (jsonObjectData->contains("avatar") && !(*jsonObjectData)["avatar"].is_null()) {
			theObject.avatar = (*jsonObjectData)["avatar"].get<std::string>();
		}

		if (jsonObjectData->contains("bot") && !(*jsonObjectData)["bot"].is_null()) {
			theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::Bot, (*jsonObjectData)["bot"].get<bool>());
		}

		if (jsonObjectData->contains("system") && !(*jsonObjectData)["system"].is_null()) {
			theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::System, (*jsonObjectData)["system"].get<bool>());
		}

		if (jsonObjectData->contains("mfa_enabled") && !(*jsonObjectData)["mfa_enabled"].is_null()) {
			theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::MFAEnabled, (*jsonObjectData)["mfa_enabled"].get<bool>());
		}

		if (jsonObjectData->contains("verified") && !(*jsonObjectData)["verified"].is_null()) {
			theObject.flags = setBool<int32_t, UserFlags>(theObject.flags, UserFlags::Verified, (*jsonObjectData)["verified"].get<bool>());
		}

		if (jsonObjectData->contains("locale") && !(*jsonObjectData)["locale"].is_null()) {
			theObject.locale = (*jsonObjectData)["locale"].get<std::string>();
		}

		if (jsonObjectData->contains("email") && !(*jsonObjectData)["email"].is_null()) {
			theObject.email = (*jsonObjectData)["email"].get<std::string>();
		}

		if (jsonObjectData->contains("premium_type") && !(*jsonObjectData)["premium_type"].is_null()) {
			theObject.premiumType = (*jsonObjectData)["premium_type"].get<PremiumType>();
		}

		if (jsonObjectData->contains("public_flags") && !(*jsonObjectData)["public_flags"].is_null()) {
			theObject.flags = (*jsonObjectData)["public_flags"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, UserVector& theObject) {
		theObject.theUsers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			UserData newData{};
			parseObject(&value, newData);
			theObject.theUsers.push_back(newData);
		}
		theObject.theUsers.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildMember& theObject) {
		if (jsonObjectData->contains("communication_disabled_until") && !(*jsonObjectData)["communication_disabled_until"].is_null()) {
			theObject.communicationDisabledUntil = (*jsonObjectData)["communication_disabled_until"].get<std::string>();
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theObject.roles.clear();
			for (auto& value: (*jsonObjectData)["roles"].get<std::vector<std::string>>()) {
				theObject.roles.emplace_back(stoull(value));
			}
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theObject.flags = (*jsonObjectData)["flags"].get<int8_t>();
		}

		if (jsonObjectData->contains("premium_since") && !(*jsonObjectData)["premium_since"].is_null()) {
			theObject.premiumSince = (*jsonObjectData)["premium_since"].get<std::string>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theObject.permissions = Permissions{ (*jsonObjectData)["permissions"].get<std::string>() };
		}

		if (jsonObjectData->contains("joined_at") && !(*jsonObjectData)["joined_at"].is_null()) {
			theObject.joinedAt = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["joined_at"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("avatar") && !(*jsonObjectData)["avatar"].is_null()) {
			theObject.avatar = (*jsonObjectData)["avatar"].get<std::string>();
		}

		if (jsonObjectData->contains("nick") && !(*jsonObjectData)["nick"].is_null()) {
			theObject.nick = (*jsonObjectData)["nick"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			std::unique_ptr<User> theUser{ std::make_unique<User>() };
			parseObject(&(*jsonObjectData)["user"], *theUser);
			theObject.id = theUser->id;
			Users::insertUser(std::move(theUser));
		}

		if (jsonObjectData->contains("pending") && !(*jsonObjectData)["pending"].is_null()) {
			theObject.flags = setBool<int8_t, GuildMemberFlags>(theObject.flags, GuildMemberFlags::Pending, (*jsonObjectData)["pending"].get<bool>());
		}

		if (jsonObjectData->contains("mute") && !(*jsonObjectData)["mute"].is_null()) {
			theObject.flags = setBool<int8_t, GuildMemberFlags>(theObject.flags, GuildMemberFlags::Mute, (*jsonObjectData)["mute"].get<bool>());
		}

		if (jsonObjectData->contains("deaf") && !(*jsonObjectData)["deaf"].is_null()) {
			theObject.flags = setBool<int8_t, GuildMemberFlags>(theObject.flags, GuildMemberFlags::Deaf, (*jsonObjectData)["deaf"].get<bool>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, WebHook& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<WebHookType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("avatar") && !(*jsonObjectData)["avatar"].is_null()) {
			theObject.avatar = (*jsonObjectData)["avatar"].get<std::string>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			theObject.token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("source_guild") && !(*jsonObjectData)["source_guild"].is_null()) {
			parseObject(&(*jsonObjectData)["source_guild"], theObject.sourceGuild);
		}

		if (jsonObjectData->contains("source_channel") && !(*jsonObjectData)["source_channel"].is_null()) {
			parseObject(&(*jsonObjectData)["source_channel"], theObject.sourceChannel);
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, WebHookVector& theObject) {
		theObject.theWebHooks.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			WebHook newData{};
			parseObject(&value, newData);
			theObject.theWebHooks.push_back(newData);
		}
		theObject.theWebHooks.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, RoleTagsData& theObject) {
		if (jsonObjectData->contains("bot_id") && !(*jsonObjectData)["bot_id"].is_null()) {
			theObject.botId = (*jsonObjectData)["bot_id"].get<std::string>();
		}

		if (jsonObjectData->contains("integration_id") && !(*jsonObjectData)["integration_id"].is_null()) {
			theObject.integrationId = (*jsonObjectData)["integration_id"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Role& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				theObject.id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("unicode_emoji") && !(*jsonObjectData)["unicode_emoji"].is_null()) {
			std::stringstream theStream{};
			theStream << (*jsonObjectData)["unicode_emoji"] << endl;
			for (auto& value: theStream.str()) {
				theObject.unicodeEmoji.push_back(value);
			}
			theObject.unicodeEmoji = static_cast<std::string>(theObject.unicodeEmoji).substr(1, theObject.unicodeEmoji.size() - 3);
		}

		if (jsonObjectData->contains("color") && !(*jsonObjectData)["color"].is_null()) {
			theObject.color = (*jsonObjectData)["color"].get<int32_t>();
		}

		if (jsonObjectData->contains("hoist") && !(*jsonObjectData)["hoist"].is_null()) {
			theObject.flags = setBool<int8_t, RoleFlags>(theObject.flags, RoleFlags::Hoist, (*jsonObjectData)["hoist"].get<bool>());
		}

		if (jsonObjectData->contains("position") && !(*jsonObjectData)["position"].is_null()) {
			theObject.position = (*jsonObjectData)["position"].get<int32_t>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theObject.permissions = Permissions{ (*jsonObjectData)["permissions"].get<std::string>() };
		}

		if (jsonObjectData->contains("managed") && !(*jsonObjectData)["managed"].is_null()) {
			theObject.flags = setBool<int8_t, RoleFlags>(theObject.flags, RoleFlags::Managed, (*jsonObjectData)["managed"].get<bool>());
		}

		if (jsonObjectData->contains("mentionable") && !(*jsonObjectData)["mentionable"].is_null()) {
			theObject.flags = setBool<int8_t, RoleFlags>(theObject.flags, RoleFlags::Mentionable, (*jsonObjectData)["mentionable"].get<bool>());
		}

		if (jsonObjectData->contains("tags") && !(*jsonObjectData)["tags"].is_null()) {
			parseObject(&(*jsonObjectData)["tags"], theObject.tags);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AttachmentData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("filename") && !(*jsonObjectData)["filename"].is_null()) {
			theObject.filename = (*jsonObjectData)["filename"].get<std::string>();
		}

		if (jsonObjectData->contains("content_type") && !(*jsonObjectData)["content_type"].is_null()) {
			theObject.contentType = (*jsonObjectData)["content_type"].get<std::string>();
		}

		if (jsonObjectData->contains("ephemeral") && !(*jsonObjectData)["ephemeral"].is_null()) {
			theObject.ephemeral = (*jsonObjectData)["ephemeral"].get<bool>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("size") && !(*jsonObjectData)["size"].is_null()) {
			theObject.size = (*jsonObjectData)["size"].get<int32_t>();
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theObject.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theObject.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theObject.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ReactionData& theObject) {
		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			theObject.count = (*jsonObjectData)["count"].get<int32_t>();
		}

		if (jsonObjectData->contains("me") && !(*jsonObjectData)["me"].is_null()) {
			theObject.count = (*jsonObjectData)["me"].get<bool>();
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			parseObject(&(*jsonObjectData)["emoji"], theObject.emoji);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theObject.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theObject.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedFooterData& theObject) {
		if (jsonObjectData->contains("text") && !(*jsonObjectData)["text"].is_null()) {
			theObject.text = (*jsonObjectData)["text"].get<std::string>();
		}
		if (jsonObjectData->contains("icon_url") && !(*jsonObjectData)["icon_url"].is_null()) {
			theObject.iconUrl = (*jsonObjectData)["icon_url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_icon_url") && !(*jsonObjectData)["proxy_icon_url"].is_null()) {
			theObject.proxyIconUrl = (*jsonObjectData)["proxy_icon_url"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedImageData& theObject) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theObject.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theObject.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theObject.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedThumbnailData& theObject) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theObject.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theObject.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theObject.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedVideoData& theObject) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theObject.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theObject.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theObject.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedProviderData& theObject) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedAuthorData& theObject) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_icon_url") && !(*jsonObjectData)["proxy_icon_url"].is_null()) {
			theObject.proxyIconUrl = (*jsonObjectData)["proxy_icon_url"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("icon_url") && !(*jsonObjectData)["icon_url"].is_null()) {
			theObject.iconUrl = (*jsonObjectData)["icon_url"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedFieldData& theObject) {
		if (jsonObjectData->contains("inline") && !(*jsonObjectData)["inline"].is_null()) {
			theObject.Inline = (*jsonObjectData)["inline"].get<bool>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			theObject.value = (*jsonObjectData)["value"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, EmbedData& theObject) {
		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null()) {
			theObject.title = (*jsonObjectData)["title"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theObject.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("color") && !(*jsonObjectData)["color"].is_null()) {
			int32_t colorValInt = (*jsonObjectData)["color"].get<int32_t>();
			std::stringstream stream{};
			stream << std::setbase(16) << colorValInt;
			theObject.hexColorValue = stream.str();
		}

		if (jsonObjectData->contains("footer") && !(*jsonObjectData)["footer"].is_null()) {
			parseObject(&(*jsonObjectData)["footer"], theObject.footer);
		}

		if (jsonObjectData->contains("image") && !(*jsonObjectData)["image"].is_null()) {
			parseObject(&(*jsonObjectData)["image"], theObject.image);
		}

		if (jsonObjectData->contains("provider") && !(*jsonObjectData)["provider"].is_null()) {
			parseObject(&(*jsonObjectData)["provider"], theObject.provider);
		}

		if (jsonObjectData->contains("thumbnail") && !(*jsonObjectData)["thumbnail"].is_null()) {
			parseObject(&(*jsonObjectData)["thumbnail"], theObject.thumbnail);
		}

		if (jsonObjectData->contains("video") && !(*jsonObjectData)["video"].is_null()) {
			parseObject(&(*jsonObjectData)["video"], theObject.video);
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			parseObject(&(*jsonObjectData)["author"], theObject.author);
		}

		if (jsonObjectData->contains("fields") && !(*jsonObjectData)["fields"].is_null()) {
			theObject.fields.clear();
			for (auto& value: (*jsonObjectData)["fields"]) {
				EmbedFieldData newData{};
				parseObject(&value, newData);
				theObject.fields.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, MessageReferenceData& theObject) {
		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theObject.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("fail_if_not_exists") && !(*jsonObjectData)["fail_if_not_exists"].is_null()) {
			theObject.failIfNotExists = (*jsonObjectData)["fail_if_not_exists"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, StickerData& theObject) {
		if (jsonObjectData->contains("asset") && !(*jsonObjectData)["asset"].is_null()) {
			theObject.asset = (*jsonObjectData)["asset"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			theObject.formatType = (*jsonObjectData)["format_type"].get<StickerFormatType>();
		}

		if (jsonObjectData->contains("available") && !(*jsonObjectData)["available"].is_null()) {
			theObject.stickerFlags = setBool<int8_t, StickerFlags>(theObject.stickerFlags, StickerFlags::Available, (*jsonObjectData)["available"].get<bool>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("pack_id") && !(*jsonObjectData)["pack_id"].is_null()) {
			theObject.packId = (*jsonObjectData)["pack_id"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<StickerType>();
		}

		if (jsonObjectData->contains("sort_value") && !(*jsonObjectData)["sort_value"].is_null()) {
			theObject.sortValue = (*jsonObjectData)["sort_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, SelectOptionData& theObject) {
		if (jsonObjectData->contains("label") && !(*jsonObjectData)["label"].is_null()) {
			theObject.label = (*jsonObjectData)["label"].get<std::string>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			theObject.value = (*jsonObjectData)["value"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			parseObject(&(*jsonObjectData)["emoji"], theObject.emoji);
		}

		if (jsonObjectData->contains("default") && !(*jsonObjectData)["default"].is_null()) {
			theObject._default = (*jsonObjectData)["default"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ActionRowData& theObject) {
		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theObject.components.clear();
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
					parseObject(&value["emoji"], newData.emoji);
				}

				if (value.contains("url") && !value["url"].is_null()) {
					newData.url = value["url"].get<std::string>();
				}

				if (value.contains("options") && !value["options"].is_null()) {
					for (auto& value02: value["options"]) {
						SelectOptionData newerData{};
						parseObject(&value02, newerData);
						newData.options.emplace_back(newerData);
					}
				}

				if (value.contains("max_values") && !value["max_values"].is_null()) {
					newData.maxValues = value["max_values"].get<int32_t>();
				}

				if (value.contains("min_values") && !value["min_values"].is_null()) {
					newData.minValues = value["min_values"].get<int32_t>();
				}

				theObject.components.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, StickerItemData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			theObject.formatType = (*jsonObjectData)["format_type"].get<StickerItemType>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Message& theObject) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theObject.content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			parseObject(&(*jsonObjectData)["author"], theObject.author);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theObject.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("edited_timestamp") && !(*jsonObjectData)["edited_timestamp"].is_null()) {
			theObject.editedTimestamp = (*jsonObjectData)["edited_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("tts") && !(*jsonObjectData)["tts"].is_null()) {
			theObject.tts = (*jsonObjectData)["tts"].get<bool>();
		}

		if (jsonObjectData->contains("mention_everyone") && !(*jsonObjectData)["mention_everyone"].is_null()) {
			theObject.mentionEveryone = (*jsonObjectData)["mention_everyone"].get<bool>();
		}

		if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
			theObject.mentions.clear();
			for (auto& value: (*jsonObjectData)["mentions"]) {
				UserData newData{};
				parseObject(&value, newData);
				theObject.mentions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
			theObject.mentionRoles.clear();
			for (auto& value: (*jsonObjectData)["mention_roles"]) {
				theObject.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
			theObject.mentionChannels.clear();
			for (auto& value: (*jsonObjectData)["mention_channels"]) {
				ChannelMentionData newData{};
				parseObject(&value, newData);
				theObject.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			theObject.attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				parseObject(&value, newData);
				theObject.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			theObject.embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				parseObject(&value, newData);
				theObject.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			theObject.reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{};
				parseObject(&value, newData);
				theObject.reactions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			theObject.nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		if (jsonObjectData->contains("pinned") && !(*jsonObjectData)["pinned"].is_null()) {
			theObject.pinned = (*jsonObjectData)["pinned"].get<bool>();
		}

		if (jsonObjectData->contains("webhook_id") && !(*jsonObjectData)["webhook_id"].is_null()) {
			theObject.webHookId = stoull((*jsonObjectData)["webhook_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<MessageType>();
		}

		if (jsonObjectData->contains("activity") && !(*jsonObjectData)["activity"].is_null()) {
			parseObject(&(*jsonObjectData)["activity"], theObject.activity);
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			parseObject(&(*jsonObjectData)["application"], theObject.application);
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
			parseObject(&(*jsonObjectData)["message_reference"], theObject.messageReference);
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theObject.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			theObject.stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{};
				parseObject(&value, newData);
				theObject.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theObject.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theObject.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
			parseObject(&(*jsonObjectData)["interaction"], theObject.interaction);
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theObject.components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{};
				parseObject(&value, newData);
				theObject.components.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
			parseObject(&(*jsonObjectData)["thread"], theObject.thread);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, MessageVector& theObject) {
		theObject.theMessages.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Message newData{};
			parseObject(&value, newData);
			theObject.theMessages.push_back(newData);
		}
		theObject.theMessages.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ThreadMemberDataVector& theObject) {
		theObject.theThreadMemberDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ThreadMemberData newData{};
			parseObject(&value, newData);
			theObject.theThreadMemberDatas.push_back(newData);
		}
		theObject.theThreadMemberDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ActiveThreadsData& theObject) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theObject.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				parseObject(&value, newData);
				theObject.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theObject.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				theObject.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("has_more") && !(*jsonObjectData)["has_more"].is_null()) {
			theObject.hasMore = (*jsonObjectData)["has_more"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ArchivedThreadsData& theObject) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theObject.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				parseObject(&value, newData);
				theObject.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theObject.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				parseObject(&value, newData);
				theObject.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("has_more") && !(*jsonObjectData)["has_more"].is_null()) {
			theObject.hasMore = (*jsonObjectData)["has_more"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AutoModerationRuleData& theObject) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			theObject.enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("trigger_type") && !(*jsonObjectData)["trigger_type"].is_null()) {
			theObject.triggerType = static_cast<TriggerType>((*jsonObjectData)["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("event_type") && !(*jsonObjectData)["event_type"].is_null()) {
			theObject.eventType = static_cast<EventType>((*jsonObjectData)["event_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theObject.creatorId = (*jsonObjectData)["creator_id"].get<Snowflake>();
		}

		if (jsonObjectData->contains("actions") && !(*jsonObjectData)["actions"].is_null()) {
			for (auto& value: (*jsonObjectData)["actions"]) {
				ActionData newData{};
				parseObject(&value, newData);
				theObject.actions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("exempt_roles") && !(*jsonObjectData)["exempt_roles"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_roles"]) {
				theObject.exemptRoles.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData->contains("trigger_metadata") && !(*jsonObjectData)["trigger_metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["trigger_metadata"], theObject.triggerMetaData);
		}

		if (jsonObjectData->contains("exempt_channels") && !(*jsonObjectData)["exempt_channels"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_channels"]) {
				theObject.exemptChannels.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = (*jsonObjectData)["guild_id"].get<Snowflake>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandPermissionData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("permission") && !(*jsonObjectData)["permission"].is_null()) {
			theObject.permission = (*jsonObjectData)["permission"].get<bool>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ApplicationCommandPermissionType>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildApplicationCommandPermissionsData& theObject) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theObject.permissions.clear();
			for (auto& value: (*jsonObjectData)["permissions"]) {
				ApplicationCommandPermissionData newData{};
				parseObject(&value, newData);
				theObject.permissions.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildApplicationCommandPermissionsDataVector& theObject) {
		theObject.theGuildApplicationCommandPermissionsDatas.clear();
		for (auto& value: *jsonObjectData) {
			GuildApplicationCommandPermissionsData newData{};
			parseObject(&value, newData);
			theObject.theGuildApplicationCommandPermissionsDatas.emplace_back(newData);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, VoiceRegionData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("custom") && !(*jsonObjectData)["custom"].is_null()) {
			theObject.custom = (*jsonObjectData)["custom"].get<bool>();
		}

		if (jsonObjectData->contains("deprecated") && !(*jsonObjectData)["deprecated"].is_null()) {
			theObject.deprecated = (*jsonObjectData)["deprecated"].get<bool>();
		}

		if (jsonObjectData->contains("optimal") && !(*jsonObjectData)["optimal"].is_null()) {
			theObject.optimal = (*jsonObjectData)["optimal"].get<bool>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, VoiceRegionDataVector& theObject) {
		theObject.theVoiceRegionDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			VoiceRegionData newData{};
			parseObject(&value, newData);
			theObject.theVoiceRegionDatas.push_back(newData);
		}
		theObject.theVoiceRegionDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, BanData& theObject) {
		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("reason") && !(*jsonObjectData)["reason"].is_null()) {
			theObject.reason = (*jsonObjectData)["reason"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, BanDataVector& theObject) {
		theObject.theBanDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			BanData newData{};
			parseObject(&value, newData);
			theObject.theBanDatas.push_back(newData);
		}
		theObject.theBanDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AuthorizationInfoData& theObject) {
		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			parseObject(&(*jsonObjectData)["application"], theObject.application);
		}

		if (jsonObjectData->contains("scopes") && !(*jsonObjectData)["scopes"].is_null()) {
			theObject.scopes = (*jsonObjectData)["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("expires") && !(*jsonObjectData)["expires"].is_null()) {
			theObject.expires = (*jsonObjectData)["expires"];
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}
	};

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AccountData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildWidgetData& theObject) {
		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			theObject.enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}
	};

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildWidgetImageData& theObject) {
		if (jsonObjectData->contains("widget_image") && !(*jsonObjectData)["widget_image"].is_null()) {
			theObject.url = (*jsonObjectData)["widget_image"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, IntegrationData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("enabled") && !(*jsonObjectData)["enabled"].is_null()) {
			theObject.enabled = (*jsonObjectData)["enabled"].get<bool>();
		}

		if (jsonObjectData->contains("syncing") && !(*jsonObjectData)["syncing"].is_null()) {
			theObject.syncing = (*jsonObjectData)["syncing"].get<bool>();
		}

		if (jsonObjectData->contains("role_id") && !(*jsonObjectData)["role_id"].is_null()) {
			theObject.roleId = stoull((*jsonObjectData)["role_id"].get<std::string>());
		}

		if (jsonObjectData->contains("enable_emoticons") && !(*jsonObjectData)["enable_emoticons"].is_null()) {
			theObject.enableEmoticons = (*jsonObjectData)["enable_emoticons"].get<bool>();
		}

		if (jsonObjectData->contains("expire_behavior") && !(*jsonObjectData)["expire_behavior"].is_null()) {
			theObject.expireBehavior = (*jsonObjectData)["expire_behavior"].get<int32_t>();
		}

		if (jsonObjectData->contains("expire_grace_period") && !(*jsonObjectData)["expire_grace_period"].is_null()) {
			theObject.expireGracePeriod = (*jsonObjectData)["expire_grace_period"].get<int32_t>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("account") && !(*jsonObjectData)["account"].is_null()) {
			parseObject(&(*jsonObjectData)["account"], theObject.account);
		}

		if (jsonObjectData->contains("synced_at") && !(*jsonObjectData)["synced_at"].is_null()) {
			theObject.syncedAt = (*jsonObjectData)["synced_at"].get<std::string>();
		}

		if (jsonObjectData->contains("subscriber_count") && !(*jsonObjectData)["subscriber_count"].is_null()) {
			theObject.subscriberCount = (*jsonObjectData)["subscriber_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("revoked") && !(*jsonObjectData)["revoked"].is_null()) {
			theObject.revoked = (*jsonObjectData)["revoked"].get<bool>();
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			parseObject(&(*jsonObjectData)["application"], theObject.application);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, IntegrationDataVector& theObject) {
		theObject.theIntegrationDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			IntegrationData newData{};
			parseObject(&value, newData);
			theObject.theIntegrationDatas.push_back(newData);
		}
		theObject.theIntegrationDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, OptionalAuditEntryInfoData& theObject) {
		if (jsonObjectData->contains("delete_member_days") && !(*jsonObjectData)["delete_member_days"].is_null()) {
			theObject.deleteMemberDays = (*jsonObjectData)["delete_member_days"].get<std::string>();
		}

		if (jsonObjectData->contains("members_removed") && !(*jsonObjectData)["members_removed"].is_null()) {
			theObject.membersRemoved = (*jsonObjectData)["members_removed"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theObject.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			theObject.count = (*jsonObjectData)["count"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("role_name") && !(*jsonObjectData)["role_name"].is_null()) {
			theObject.roleName = (*jsonObjectData)["role_name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AuditLogChangeData& theObject) {
		if (jsonObjectData->contains("new_value") && !(*jsonObjectData)["new_value"].is_null()) {
			theObject.newValue = (*jsonObjectData)["new_value"];
		}

		if (jsonObjectData->contains("old_value") && !(*jsonObjectData)["old_value"].is_null()) {
			theObject.oldValue = (*jsonObjectData)["old_value"];
		}

		if (jsonObjectData->contains("key") && !(*jsonObjectData)["key"].is_null()) {
			theObject.key = (*jsonObjectData)["key"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildPruneCountData& theObject) {
		if (jsonObjectData->contains("pruned") && !(*jsonObjectData)["pruned"].is_null()) {
			theObject.count = (*jsonObjectData)["pruned"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AuditLogEntryData& theObject) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theObject.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}

		if (jsonObjectData->contains("changes") && !(*jsonObjectData)["changes"].is_null()) {
			theObject.changes.clear();
			for (auto& value: (*jsonObjectData)["changes"]) {
				AuditLogChangeData newData{};
				parseObject(&value, newData);
				theObject.changes.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theObject.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
			theObject.createdTimeStamp = theObject.getCreatedAtTimestamp(TimeFormat::LongDateTime);
		}

		if (jsonObjectData->contains("action_type") && !(*jsonObjectData)["action_type"].is_null()) {
			theObject.actionType = (*jsonObjectData)["action_type"].get<AuditLogEvent>();
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			parseObject(&(*jsonObjectData)["options"], theObject.options);
		}

		if (jsonObjectData->contains("reason") && !(*jsonObjectData)["reason"].is_null()) {
			theObject.reason = (*jsonObjectData)["reason"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, StageInstanceData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			theObject.topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("privacy_level") && !(*jsonObjectData)["privacy_level"].is_null()) {
			theObject.privacyLevel = (*jsonObjectData)["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		if (jsonObjectData->contains("discoverable_disabled") && !(*jsonObjectData)["discoverable_disabled"].is_null()) {
			theObject.discoverableDisabled = (*jsonObjectData)["discoverable_disabled"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildPreviewData& theObject) {
		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			theObject.approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			theObject.approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("discovery_splash") && !(*jsonObjectData)["discovery_splash"].is_null()) {
			theObject.discoverySplash = (*jsonObjectData)["discovery_splash"].get<std::string>();
		}

		if (jsonObjectData->contains("emojis") && !(*jsonObjectData)["emojis"].is_null()) {
			theObject.emojis.clear();
			for (auto& value: (*jsonObjectData)["emojis"]) {
				EmojiData newData{};
				parseObject(&value, newData);
				theObject.emojis.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theObject.emojis.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theObject.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("features") && !(*jsonObjectData)["features"].is_null()) {
			theObject.features = (*jsonObjectData)["features"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("splash") && !(*jsonObjectData)["splash"].is_null()) {
			theObject.splash = (*jsonObjectData)["splash"].get<std::string>();
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theObject.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildDataVector& theObject) {
		theObject.theGuildDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildData newData{};
			parseObject(&value, newData);
			theObject.theGuildDatas.push_back(newData);
		}
		theObject.theGuildDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventUserData& theObject) {
		if (jsonObjectData->contains("guild_scheduled_event_id") && !(*jsonObjectData)["guild_scheduled_event_id"].is_null()) {
			theObject.guildScheduledEventId = (*jsonObjectData)["guild_scheduled_event_id"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventUserDataVector& theObject) {
		theObject.theGuildScheduledEventUserDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildScheduledEventUserData newData{};
			parseObject(&value, newData);
			theObject.theGuildScheduledEventUserDatas.push_back(newData);
		}
		theObject.theGuildScheduledEventUserDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, InviteData& theObject) {
		if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null() && (*jsonObjectData)["code"].is_string()) {
			theObject.code = (*jsonObjectData)["code"].get<std::string>();
		} else if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null() && (*jsonObjectData)["code"].is_number_integer()) {
			theObject.code = std::to_string((*jsonObjectData)["code"].get<int32_t>());
		}

		if (jsonObjectData->contains("guild") && !(*jsonObjectData)["guild"].is_null()) {
			parseObject(&(*jsonObjectData)["guild"], theObject.guild);
		}

		if (jsonObjectData->contains("channel") && !(*jsonObjectData)["channel"].is_null()) {
			parseObject(&(*jsonObjectData)["channel"], theObject.channel);
		}

		if (jsonObjectData->contains("inviter") && !(*jsonObjectData)["inviter"].is_null()) {
			parseObject(&(*jsonObjectData)["inviter"], theObject.inviter);
		}

		if (jsonObjectData->contains("target_type") && !(*jsonObjectData)["target_type"].is_null()) {
			theObject.targetType = (*jsonObjectData)["target_type"].get<int32_t>();
		}

		if (jsonObjectData->contains("target_user") && !(*jsonObjectData)["target_user"].is_null()) {
			parseObject(&(*jsonObjectData)["target_user"], theObject.targetUser);
		}

		if (jsonObjectData->contains("target_application") && !(*jsonObjectData)["target_application"].is_null()) {
			parseObject(&(*jsonObjectData)["target_application"], theObject.targetApplication);
		}

		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			theObject.approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			theObject.approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("expires_at") && !(*jsonObjectData)["expires_at"].is_null()) {
			theObject.expiresAt = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["expires_at"].get<std::string>());
		}

		if (jsonObjectData->contains("stage_instance") && !(*jsonObjectData)["stage_instance"].is_null()) {
			parseObject(&(*jsonObjectData)["stage_instance"], theObject.stageInstance);
		}

		if (jsonObjectData->contains("guild_scheduled_event") && !(*jsonObjectData)["guild_scheduled_event"].is_null()) {
			parseObject(&(*jsonObjectData)["guild_scheduled_event"], theObject.guildScheduledEvent);
		}

		if (jsonObjectData->contains("uses") && !(*jsonObjectData)["uses"].is_null()) {
			theObject.uses = (*jsonObjectData)["uses"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_uses") && !(*jsonObjectData)["max_uses"].is_null()) {
			theObject.maxUses = (*jsonObjectData)["max_uses"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_age") && !(*jsonObjectData)["max_age"].is_null()) {
			theObject.maxAge = (*jsonObjectData)["max_age"].get<int32_t>();
		}

		if (jsonObjectData->contains("temporary") && !(*jsonObjectData)["temporary"].is_null()) {
			theObject.temporary = (*jsonObjectData)["temporary"].get<bool>();
		}

		if (jsonObjectData->contains("created_at") && !(*jsonObjectData)["created_at"].is_null()) {
			theObject.createdAt = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["created_at"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, InviteDataVector& theObject) {
		theObject.theInviteDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			InviteData newData{};
			parseObject(&value, newData);
			theObject.theInviteDatas.push_back(newData);
		}
		theObject.theInviteDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildTemplateData& theObject) {
		if (jsonObjectData->contains("serialized_source_guild") && !(*jsonObjectData)["serialized_source_guild"].is_null()) {
			parseObject(&(*jsonObjectData)["serialized_source_guild"], theObject.serializedSourceGuild);
		}

		if (jsonObjectData->contains("creator") && !(*jsonObjectData)["creator"].is_null()) {
			parseObject(&(*jsonObjectData)["creator"], theObject.creator);
		}

		if (jsonObjectData->contains("source_guild_id") && !(*jsonObjectData)["source_guild_id"].is_null()) {
			theObject.sourceGuildId = (*jsonObjectData)["source_guild_id"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("usage_count") && !(*jsonObjectData)["usage_count"].is_null()) {
			theObject.usageCount = (*jsonObjectData)["usage_count"].get<uint32_t>();
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theObject.creatorId = (*jsonObjectData)["creator_id"].get<std::string>();
		}

		if (jsonObjectData->contains("created_at") && !(*jsonObjectData)["created_at"].is_null()) {
			theObject.createdAt = (*jsonObjectData)["created_at"].get<std::string>();
		}

		if (jsonObjectData->contains("updated_at") && !(*jsonObjectData)["updated_at"].is_null()) {
			theObject.updatedAt = (*jsonObjectData)["updated_at"].get<std::string>();
		}

		if (jsonObjectData->contains("is_dirty") && !(*jsonObjectData)["is_dirty"].is_null()) {
			theObject.isDirty = (*jsonObjectData)["is_dirty"].get<bool>();
		}

		if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null()) {
			theObject.code = (*jsonObjectData)["code"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildTemplateDataVector& theObject) {
		theObject.theGuildTemplateDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildTemplateData newData{};
			parseObject(&value, newData);
			theObject.theGuildTemplateDatas.push_back(newData);
		}
		theObject.theGuildTemplateDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, WebHookData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<WebHookType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("avatar") && !(*jsonObjectData)["avatar"].is_null()) {
			theObject.avatar = (*jsonObjectData)["avatar"].get<std::string>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			theObject.token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("source_guild") && !(*jsonObjectData)["source_guild"].is_null()) {
			parseObject(&(*jsonObjectData)["source_guild"], theObject.sourceGuild);
		}

		if (jsonObjectData->contains("source_channel") && !(*jsonObjectData)["source_channel"].is_null()) {
			parseObject(&(*jsonObjectData)["source_channel"], theObject.sourceChannel);
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, WebHookDataVector& theObject) {
		theObject.theWebHookDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			WebHookData newData{};
			parseObject(&value, newData);
			theObject.theWebHookDatas.push_back(newData);
		}
		theObject.theWebHookDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AuditLogData& theObject) {
		if (jsonObjectData->contains("webhooks") && !(*jsonObjectData)["webhooks"].is_null()) {
			theObject.webhooks.clear();
			theObject.webhooks.reserve((*jsonObjectData)["webhooks"].size());
			for (auto& value: (*jsonObjectData)["webhooks"]) {
				WebHookData newData{};
				parseObject(&value, newData);
				theObject.webhooks.push_back(newData);
			}
		}

		if (jsonObjectData->contains("guild_scheduled_events") && !(*jsonObjectData)["guild_scheduled_events"].is_null()) {
			theObject.guildScheduledEvents.clear();
			theObject.guildScheduledEvents.reserve((*jsonObjectData)["guild_scheduled_events"].size());
			for (auto& value: (*jsonObjectData)["guild_scheduled_events"]) {
				GuildScheduledEventData newData{};
				parseObject(&value, newData);
				theObject.guildScheduledEvents.push_back(newData);
			}
		}

		if (jsonObjectData->contains("auto_moderation_rules") && !(*jsonObjectData)["auto_moderation_rules"].is_null()) {
			theObject.autoModerationRules.clear();
			theObject.autoModerationRules.reserve((*jsonObjectData)["auto_moderation_rules"].size());
			for (auto& value: (*jsonObjectData)["auto_moderation_rules"]) {
				AutoModerationRuleData newData{};
				parseObject(&value, newData);
				theObject.autoModerationRules.push_back(newData);
			}
		}

		if (jsonObjectData->contains("users") && !(*jsonObjectData)["users"].is_null()) {
			theObject.users.clear();
			theObject.users.reserve((*jsonObjectData)["users"].size());
			for (auto& value: (*jsonObjectData)["users"]) {
				UserData newData{};
				parseObject(&value, newData);
				theObject.users.push_back(newData);
			}
		}

		if (jsonObjectData->contains("audit_log_entries") && !(*jsonObjectData)["audit_log_entries"].is_null()) {
			theObject.auditLogEntries.clear();
			theObject.auditLogEntries.reserve((*jsonObjectData)["audit_log_entries"].size());
			for (auto& value: (*jsonObjectData)["audit_log_entries"]) {
				AuditLogEntryData newData{};
				parseObject(&value, newData);
				theObject.auditLogEntries.push_back(newData);
			}
		}

		if (jsonObjectData->contains("integrations") && !(*jsonObjectData)["integrations"].is_null()) {
			theObject.integrations.clear();
			theObject.integrations.reserve((*jsonObjectData)["integrations"].size());
			for (auto& value: (*jsonObjectData)["integrations"]) {
				IntegrationData newData{};
				parseObject(&value, newData);
				theObject.integrations.push_back(newData);
			}
		}

		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theObject.threads.clear();
			theObject.threads.reserve((*jsonObjectData)["threads"].size());
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				parseObject(&value, newData);
				theObject.threads.push_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ReactionRemoveData& theObject) {
		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theObject.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theObject.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			parseObject(&(*jsonObjectData)["emoji"], theObject.emoji);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, TypingStartData& theObject) {
		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theObject.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theObject.timestamp = (*jsonObjectData)["timestamp"].get<int32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, YouTubeFormatVector& theObject) {
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

					theObject.theFormats.emplace_back(newData);
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
					theObject.theFormats.emplace_back(newData);
				}
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, UserCommandInteractionData& theObject) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theObject.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, MessageCommandInteractionData& theObject) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theObject.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ComponentInteractionData& theObject) {
		if (jsonObjectData->contains("values") && !(*jsonObjectData)["values"].is_null()) {
			theObject.values.clear();
			for (auto& value: (*jsonObjectData)["values"]) {
				theObject.values.emplace_back(value);
			}
		}

		if (jsonObjectData->contains("custom_id") && !(*jsonObjectData)["custom_id"].is_null()) {
			theObject.customId = (*jsonObjectData)["custom_id"].get<std::string>();
		}

		if (jsonObjectData->contains("component_type") && !(*jsonObjectData)["component_type"].is_null()) {
			theObject.componentType = (*jsonObjectData)["component_type"].get<ComponentType>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ModalInteractionData& theObject) {
		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theObject.value = (*jsonObjectData)["components"][0]["components"][0]["value"].get<std::string>();
		}

		if (jsonObjectData->contains("custom_id") && !(*jsonObjectData)["custom_id"].is_null()) {
			theObject.customId = (*jsonObjectData)["custom_id"].get<std::string>();
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theObject.customIdSmall = (*jsonObjectData)["components"][0]["components"][0]["custom_id"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, AllowedMentionsData& theObject) {
		if (jsonObjectData->contains("parse") && !(*jsonObjectData)["parse"].is_null()) {
			theObject.parse.clear();
			for (auto& value: (*jsonObjectData)["parse"]) {
				theObject.parse.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theObject.roles.clear();
			for (auto& value: (*jsonObjectData)["roles"]) {
				theObject.roles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("users") && !(*jsonObjectData)["users"].is_null()) {
			theObject.users.clear();
			for (auto& value: (*jsonObjectData)["users"]) {
				theObject.users.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("replied_user") && !(*jsonObjectData)["replied_user"].is_null()) {
			theObject.repliedUser = (*jsonObjectData)["replied_user"].get<bool>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("default_member_permissions") && !(*jsonObjectData)["default_member_permissions"].is_null()) {
			theObject.defaultMemberPermissions = (*jsonObjectData)["default_member_permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("dm_permission") && !(*jsonObjectData)["dm_permission"].is_null()) {
			theObject.dmPermission = (*jsonObjectData)["dm_permission"].get<bool>();
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theObject.version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			theObject.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theObject.nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description_localizations") && !(*jsonObjectData)["description_localizations"].is_null()) {
			theObject.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["description_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theObject.descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theObject.version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			theObject.options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandOptionData newData{};
				parseObject(&value, newData);
				theObject.options.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ChannelPinsUpdateEventData& theObject) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			theObject.lastPinTimestamp = TimeStamp<std::chrono::milliseconds>((*jsonObjectData)["last_pin_timestamp"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ThreadListSyncData& theObject) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_ids") && !(*jsonObjectData)["channel_ids"].is_null()) {
			theObject.channelIds.clear();
			for (auto& value: (*jsonObjectData)["channel_ids"]) {
				theObject.channelIds.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theObject.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				parseObject(&value, newData);
				theObject.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theObject.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				parseObject(&value, newData);
				theObject.threads.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ThreadMembersUpdateData& theObject) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			theObject.memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("added_members") && !(*jsonObjectData)["added_members"].is_null()) {
			theObject.addedMembers.clear();
			for (auto& value: (*jsonObjectData)["added_members"]) {
				ThreadMemberData newData{};
				parseObject(&value, newData);
				theObject.addedMembers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("removed_member_ids") && !(*jsonObjectData)["removed_member_ids"].is_null()) {
			theObject.removedMemberIds.clear();
			for (auto& value: (*jsonObjectData)["removed_member_ids"]) {
				theObject.removedMemberIds.emplace_back(value);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, MessageDataOld& theObject) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theObject.content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			parseObject(&(*jsonObjectData)["author"], theObject.author);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theObject.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("edited_timestamp") && !(*jsonObjectData)["edited_timestamp"].is_null()) {
			theObject.editedTimestamp = (*jsonObjectData)["edited_timestamp"].get<std::string>();


			if (jsonObjectData->contains("tts") && !(*jsonObjectData)["tts"].is_null()) {
				theObject.tts = (*jsonObjectData)["tts"].get<bool>();
			}

			if (jsonObjectData->contains("mention_everyone") && !(*jsonObjectData)["mention_everyone"].is_null()) {
				theObject.mentionEveryone = (*jsonObjectData)["mention_everyone"].get<bool>();
			}

			if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
				theObject.mentions.clear();
				for (auto& value: (*jsonObjectData)["mentions"]) {
					UserData newData{};
					parseObject(&value, newData);
					theObject.mentions.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
				theObject.mentionRoles.clear();
				for (auto& value: (*jsonObjectData)["mention_roles"]) {
					theObject.mentionRoles.emplace_back(value.get<std::string>());
				}
			}

			if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
				theObject.mentionChannels.clear();
				for (auto& value: (*jsonObjectData)["mention_channels"]) {
					ChannelMentionData newData{};
					parseObject(&value, newData);
					theObject.mentionChannels.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
				theObject.attachments.clear();
				for (auto& value: (*jsonObjectData)["attachments"]) {
					AttachmentData newData{};
					parseObject(&value, newData);
					theObject.attachments.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
				theObject.embeds.clear();
				for (auto& value: (*jsonObjectData)["embeds"]) {
					EmbedData newData{};
					parseObject(&value, newData);
					theObject.embeds.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
				theObject.reactions.clear();
				for (auto& value: (*jsonObjectData)["reactions"]) {
					ReactionData newData{};
					parseObject(&value, newData);
					theObject.reactions.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
				theObject.nonce = (*jsonObjectData)["nonce"].get<std::string>();
			}

			if (jsonObjectData->contains("pinned") && !(*jsonObjectData)["pinned"].is_null()) {
				theObject.pinned = (*jsonObjectData)["pinned"].get<bool>();
			}

			if (jsonObjectData->contains("webhook_id") && !(*jsonObjectData)["webhook_id"].is_null()) {
				theObject.webHookId = stoull((*jsonObjectData)["webhook_id"].get<std::string>());
			}

			if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
				theObject.type = (*jsonObjectData)["type"].get<MessageType>();
			}

			if (jsonObjectData->contains("activity") && !(*jsonObjectData)["activity"].is_null()) {
				parseObject(&(*jsonObjectData)["activity"], theObject.activity);
			}

			if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
				parseObject(&(*jsonObjectData)["application"], theObject.application);
			}

			if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
				theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
			}

			if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
				parseObject(&(*jsonObjectData)["message_reference"], theObject.messageReference);
			}

			if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
				theObject.flags = (*jsonObjectData)["flags"].get<int32_t>();
			}

			if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
				theObject.stickerItems.clear();
				for (auto& value: (*jsonObjectData)["sticker_items"]) {
					StickerItemData newData{};
					parseObject(&value, newData);
					theObject.stickerItems.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
				theObject.stickers.clear();
				for (auto& value: (*jsonObjectData)["stickers"]) {
					StickerData newData{};
					parseObject(&value, newData);
					theObject.stickers.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
				parseObject(&(*jsonObjectData)["interaction"], theObject.interaction);
			}

			if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
				theObject.components.clear();
				for (auto& value: (*jsonObjectData)["components"]) {
					ActionRowData newData{};
					parseObject(&value, newData);
					theObject.components.emplace_back(newData);
				}
			}

			if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
				parseObject(&(*jsonObjectData)["thread"], theObject.thread);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, MessageData& theObject) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theObject.content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			parseObject(&(*jsonObjectData)["author"], theObject.author);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theObject.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("edited_timestamp") && !(*jsonObjectData)["edited_timestamp"].is_null()) {
			theObject.editedTimestamp = (*jsonObjectData)["edited_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("tts") && !(*jsonObjectData)["tts"].is_null()) {
			theObject.tts = (*jsonObjectData)["tts"].get<bool>();
		}

		if (jsonObjectData->contains("mention_everyone") && !(*jsonObjectData)["mention_everyone"].is_null()) {
			theObject.mentionEveryone = (*jsonObjectData)["mention_everyone"].get<bool>();
		}

		if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
			theObject.mentions.clear();
			for (auto& value: (*jsonObjectData)["mentions"]) {
				UserData newData{};
				parseObject(&value, newData);
				theObject.mentions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
			theObject.mentionRoles.clear();
			for (auto& value: (*jsonObjectData)["mention_roles"]) {
				theObject.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
			theObject.mentionChannels.clear();
			for (auto& value: (*jsonObjectData)["mention_channels"]) {
				ChannelMentionData newData{};
				parseObject(&value, newData);
				theObject.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			theObject.attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				parseObject(&value, newData);
				theObject.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			theObject.embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				parseObject(&value, newData);
				theObject.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			theObject.reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{};
				parseObject(&value, newData);
				theObject.reactions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			theObject.nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		if (jsonObjectData->contains("pinned") && !(*jsonObjectData)["pinned"].is_null()) {
			theObject.pinned = (*jsonObjectData)["pinned"].get<bool>();
		}

		if (jsonObjectData->contains("webhook_id") && !(*jsonObjectData)["webhook_id"].is_null()) {
			theObject.webHookId = stoull((*jsonObjectData)["webhook_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<MessageType>();
		}

		if (jsonObjectData->contains("activity") && !(*jsonObjectData)["activity"].is_null()) {
			parseObject(&(*jsonObjectData)["activity"], theObject.activity);
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			parseObject(&(*jsonObjectData)["application"], theObject.application);
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
			parseObject(&(*jsonObjectData)["message_reference"], theObject.messageReference);
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theObject.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			theObject.stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{};
				parseObject(&value, newData);
				theObject.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theObject.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theObject.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
			parseObject(&(*jsonObjectData)["interaction"], theObject.interaction);
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theObject.components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{};
				parseObject(&value, newData);
				theObject.components.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
			parseObject(&(*jsonObjectData)["thread"], theObject.thread);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ApplicationCommandInteractionData& theObject) {
		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			theObject.options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandInteractionDataOption newData{};
				parseObject(&value, newData);
				theObject.options.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("resolved") && !(*jsonObjectData)["resolved"].is_null()) {
			auto& value = (*jsonObjectData)["resolved"];

			if (value.contains("attachments") && !value["attachments"].is_null()) {
				theObject.resolved.attachments.clear();
				auto newMap = value["attachments"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					AttachmentData newData{};
					parseObject(&newValue, newData);
					theObject.resolved.attachments[stoull(key)] = newData;
				}
			}

			if (value.contains("users") && !value["users"].is_null()) {
				theObject.resolved.users.clear();
				auto newMap = value["users"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					UserData newData{};
					parseObject(&value, newData);
					theObject.resolved.users[stoull(key)] = newData;
				}
			}
			if (value.contains("channels") && !value["channels"].is_null()) {
				theObject.resolved.channels.clear();
				auto newMap = value["channels"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					ChannelData newData{};
					parseObject(&newValue, newData);
					theObject.resolved.channels[stoull(key)] = newData;
				}
			}
			if (value.contains("roles") && !value["roles"].is_null()) {
				theObject.resolved.roles.clear();
				auto newMap = value["roles"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					RoleData newData{};
					parseObject(&newValue, newData);
					theObject.resolved.roles[stoull(key)] = newData;
				}
			}
			if (value.contains("members") && !value["members"].is_null()) {
				theObject.resolved.members.clear();
				auto newMap = value["members"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					GuildMemberData newData{};
					parseObject(&newValue, newData);
					theObject.resolved.members[stoull(key)] = newData;
				}
			}
			if (value.contains("messages") && !value["messages"].is_null()) {
				theObject.resolved.messages.clear();
				auto newMap = value["messages"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					MessageData newData{};
					parseObject(&newValue, newData);
					theObject.resolved.messages[stoull(key)] = newData;
				}
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, StickerPackData& theObject) {
		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theObject.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theObject.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("cover_sticker_id") && !(*jsonObjectData)["cover_sticker_id"].is_null()) {
			theObject.coverStickerId = (*jsonObjectData)["cover_sticker_id"].get<std::string>();
		}

		if (jsonObjectData->contains("banner_asset_id") && !(*jsonObjectData)["banner_asset_id"].is_null()) {
			theObject.bannerAssetId = (*jsonObjectData)["banner_asset_id"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theObject.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.Id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("sku_id") && !(*jsonObjectData)["sku_id"].is_null()) {
			theObject.skuId = (*jsonObjectData)["sku_id"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, StickerPackDataVector& theObject) {
		theObject.theStickerPackDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			StickerPackData newData{};
			parseObject(&value, newData);
			theObject.theStickerPackDatas.push_back(newData);
		}
		theObject.theStickerPackDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ConnectionData& theObject) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("show_activity") && !(*jsonObjectData)["show_activity"].is_null()) {
			theObject.showActivity = (*jsonObjectData)["show_activity"].get<bool>();
		}

		if (jsonObjectData->contains("friend_sync") && !(*jsonObjectData)["friend_sync"].is_null()) {
			theObject.friendSync = (*jsonObjectData)["friend_sync"].get<bool>();
		}

		if (jsonObjectData->contains("revoked") && !(*jsonObjectData)["revoked"].is_null()) {
			theObject.revoked = (*jsonObjectData)["revoked"].get<bool>();
		}

		if (jsonObjectData->contains("verified") && !(*jsonObjectData)["verified"].is_null()) {
			theObject.verified = (*jsonObjectData)["verified"].get<bool>();
		}

		if (jsonObjectData->contains("visibility") && !(*jsonObjectData)["visibility"].is_null()) {
			theObject.visibility = (*jsonObjectData)["visibility"].get<ConnectionVisibilityTypes>();
		}

		if (jsonObjectData->contains("integrations") && !(*jsonObjectData)["integrations"].is_null()) {
			theObject.integrations.clear();
			for (auto& value: (*jsonObjectData)["integrations"]) {
				IntegrationData newData{};
				parseObject(&value, newData);
				theObject.integrations.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, ConnectionDataVector& theObject) {
		theObject.theConnectionDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ConnectionData newData{};
			parseObject(&value, newData);
			theObject.theConnectionDatas.push_back(newData);
		}
		theObject.theConnectionDatas.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, InteractionDataData& theObject) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			parseObject(jsonObjectData, theObject.applicationCommandData);
		}

		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			parseObject(jsonObjectData, theObject.messageInteractionData);
			parseObject(jsonObjectData, theObject.userInteractionData);
		}

		if (jsonObjectData->contains("component_type") && !(*jsonObjectData)["component_type"].is_null()) {
			parseObject(jsonObjectData, theObject.componentData);
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			parseObject(jsonObjectData, theObject.modalData);
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, InteractionData& theObject) {
		if (jsonObjectData->contains("data") && !(*jsonObjectData)["data"].is_null()) {
			parseObject(&(*jsonObjectData)["data"], theObject.data);
			theObject.rawData = (*jsonObjectData)["data"];
		}

		if (jsonObjectData->contains("app_permissions") && !(*jsonObjectData)["app_permissions"].is_null()) {
			theObject.appPermissions = (*jsonObjectData)["app_permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theObject.type = (*jsonObjectData)["type"].get<InteractionType>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			theObject.token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theObject.member);
			theObject.user.id = theObject.member.id;
			theObject.user.userName = theObject.member.getUserData().userName;
		} else if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theObject.user);
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theObject.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("locale") && !(*jsonObjectData)["locale"].is_null()) {
			theObject.locale = (*jsonObjectData)["locale"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_locale") && !(*jsonObjectData)["guild_locale"].is_null()) {
			theObject.guildLocale = (*jsonObjectData)["guild_locale"].get<std::string>();
		}

		if (jsonObjectData->contains("message") && !(*jsonObjectData)["message"].is_null()) {
			parseObject(&(*jsonObjectData)["message"], theObject.message);
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theObject.version = (*jsonObjectData)["version"].get<int32_t>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theObject.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theObject.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, SessionStartData& theObject) {
		if (jsonObjectData->contains("max_concurrency") && !(*jsonObjectData)["max_concurrency"].is_null()) {
			theObject.maxConcurrency = (*jsonObjectData)["max_concurrency"].get<uint32_t>();
		}

		if (jsonObjectData->contains("remaining") && !(*jsonObjectData)["remaining"].is_null()) {
			theObject.remaining = (*jsonObjectData)["remaining"].get<uint32_t>();
		}

		if (jsonObjectData->contains("reset_after") && !(*jsonObjectData)["reset_after"].is_null()) {
			theObject.resetAfter = (*jsonObjectData)["reset_after"].get<uint32_t>();
		}

		if (jsonObjectData->contains("total") && !(*jsonObjectData)["total"].is_null()) {
			theObject.total = (*jsonObjectData)["total"].get<uint32_t>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GatewayBotData& theObject) {
		if (jsonObjectData->contains("session_start_limit") && !(*jsonObjectData)["session_start_limit"].is_null()) {
			parseObject(&(*jsonObjectData)["session_start_limit"], theObject.sessionStartLimit);
		}

		if (jsonObjectData->contains("shards") && !(*jsonObjectData)["shards"].is_null()) {
			theObject.shards = (*jsonObjectData)["shards"].get<uint32_t>();
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theObject.url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildEmojisUpdateEventData& theObject) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emojis") && !(*jsonObjectData)["emojis"].is_null()) {
			theObject.emojis.clear();
			for (auto& value: (*jsonObjectData)["emojis"]) {
				EmojiData newData{};
				parseObject(&value, newData);
				theObject.emojis.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildStickersUpdateEventData& theObject) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theObject.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theObject.stickers.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildMembersChunkEventData& theObject) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theObject.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			theObject.nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		if (jsonObjectData->contains("chunk_index") && !(*jsonObjectData)["chunk_index"].is_null()) {
			theObject.chunkIndex = (*jsonObjectData)["chunk_index"].get<int32_t>();
		}

		if (jsonObjectData->contains("chunk_count") && !(*jsonObjectData)["chunk_count"].is_null()) {
			theObject.chunkCount = (*jsonObjectData)["chunk_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("presences") && !(*jsonObjectData)["presences"].is_null()) {
			theObject.presences.clear();
			for (auto& value: (*jsonObjectData)["presences"]) {
				PresenceUpdateData newData{};
				parseObject(&value, newData);
				theObject.presences.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("not_found") && !(*jsonObjectData)["not_found"].is_null()) {
			theObject.notFound.clear();
			for (auto& value: (*jsonObjectData)["not_found"]) {
				theObject.notFound.emplace_back(value);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theObject.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				GuildMemberData newData{};
				parseObject(&value, newData);
				theObject.members.emplace_back(newData);
			}
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, CommandData& theObject) {
		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			theObject.optionsArgs.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				if (value.contains("type") && value["type"] == 1) {
					if (value.contains("name")) {
						theObject.subCommandName = value["name"];
					}
				} else if (value.contains("type") && value["type"] == 2) {
					if (value.contains("name")) {
						theObject.subCommandGroupName = value["name"];
					}
				}
				if (value.contains("options")) {
					std::string theOption{};
					parseObject(&value, theOption);
				}
				if (value.contains("value") && !value["value"].is_null()) {
					auto& newValueNew = value["value"];
					if (newValueNew.is_string()) {
						theObject.optionsArgs.emplace_back(newValueNew);
					} else if (newValueNew.is_number()) {
						theObject.optionsArgs.emplace_back(std::to_string(newValueNew.get<int64_t>()));
					} else if (newValueNew.is_boolean()) {
						theObject.optionsArgs.emplace_back(std::to_string(newValueNew.get<bool>()));
					}
				}
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theObject.commandName = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, Song& theObject) {
		if (jsonObjectData->contains("lengthText") && !(*jsonObjectData)["lengthText"].is_null()) {
			theObject.duration = (*jsonObjectData)["lengthText"]["accessibility"]["accessibilityData"]["label"].get<std::string>();
		}

		if (jsonObjectData->contains("detailedMetadataSnippets") && !(*jsonObjectData)["detailedMetadataSnippets"].is_null()) {
			for (auto& value: (*jsonObjectData)["detailedMetadataSnippets"][0]["snippetText"]["runs"]) {
				std::string newString = value["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theObject.description = utf8MakeValid(newString);
			}
		}

		if (jsonObjectData->contains("thumbnail") && !(*jsonObjectData)["thumbnail"].is_null()) {
			theObject.thumbnailUrl = (*jsonObjectData)["thumbnail"]["thumbnails"][0]["url"].get<std::string>();
		}

		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null()) {
			if ((*jsonObjectData)["title"].contains("runs")) {
				std::string newString = (*jsonObjectData)["title"]["runs"][0]["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theObject.songTitle = utf8MakeValid(newString);
				;
			} else if ((*jsonObjectData)["title"].contains("simpleText")) {
				std::string newString = (*jsonObjectData)["title"]["simpleText"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theObject.songTitle = utf8MakeValid(newString);
			}
		}
		if (jsonObjectData->contains("videoId") && !(*jsonObjectData)["videoId"].is_null()) {
			theObject.songId = (*jsonObjectData)["videoId"].get<std::string>();
		}

		if (jsonObjectData->contains("track_authorization") && !(*jsonObjectData)["track_authorization"].is_null()) {
			theObject.trackAuthorization = (*jsonObjectData)["track_authorization"].get<std::string>();
		}

		if (jsonObjectData->contains("media") && !(*jsonObjectData)["media"].is_null()) {
			bool isItFound{ false };
			for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
				if (value["preset"] == "opus_0_0") {
					isItFound = true;
					theObject.firstDownloadUrl = value["url"].get<std::string>();
					theObject.songId = value["url"].get<std::string>();
					theObject.doWeGetSaved = true;
				}
			}
			bool isItFound2{ false };
			if (!isItFound) {
				for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
					if (value["preset"] == "mp3_0_0") {
						theObject.firstDownloadUrl = value["url"].get<std::string>();
						theObject.songId = value["url"].get<std::string>();
						isItFound2 = true;
					}
				}
			}
			if (!isItFound2 && !isItFound) {
				for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
					theObject.firstDownloadUrl = value["url"].get<std::string>();
					theObject.songId = value["url"].get<std::string>();
				}
			}
		}

		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null() && !(*jsonObjectData)["title"].is_object()) {
			std::string newString = (*jsonObjectData)["title"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			theObject.songTitle = utf8MakeValid(newString);
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			std::string newString = (*jsonObjectData)["description"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			theObject.description = utf8MakeValid(newString);
			theObject.description += "...";
		}

		if (jsonObjectData->contains("artwork_url") && !(*jsonObjectData)["artwork_url"].is_null()) {
			theObject.thumbnailUrl = (*jsonObjectData)["artwork_url"].get<std::string>();
		} else if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			if ((*jsonObjectData)["user"].contains("avatar_url") && !(*jsonObjectData)["user"]["avatar_url"].is_null()) {
				theObject.thumbnailUrl = (*jsonObjectData)["user"]["avatar_url"].get<std::string>();
			}
		}

		if (jsonObjectData->contains("duration") && !(*jsonObjectData)["duration"].is_null()) {
			TimeStamp<std::chrono::milliseconds> theTimeStamp{};
			theObject.duration = theTimeStamp.convertMsToDurationString((*jsonObjectData)["duration"].get<int32_t>());
		}

		if (jsonObjectData->contains("permalink_url") && !(*jsonObjectData)["permalink_url"].is_null()) {
			theObject.viewUrl = (*jsonObjectData)["permalink_url"].get<std::string>();
		}
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventVector& theObject) {
		theObject.theGuildScheduledEvents.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildScheduledEvent newData{};
			parseObject(&value, newData);
			theObject.theGuildScheduledEvents.push_back(newData);
		}
		theObject.theGuildScheduledEvents.shrink_to_fit();
	}

	template<> inline void parseObject(const nlohmann::json* jsonObjectData, GuildScheduledEventDataVector& theObject) {
		theObject.theGuildScheduledEventDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildScheduledEventData newData{};
			parseObject(&value, newData);
			theObject.theGuildScheduledEventDatas.push_back(newData);
		}
		theObject.theGuildScheduledEventDatas.shrink_to_fit();
	}
};
