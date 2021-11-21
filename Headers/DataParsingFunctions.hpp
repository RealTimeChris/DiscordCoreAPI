// DataParsingFunctions.hpp - Header for the functions that will parse the Discord data into C++ types.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"

namespace DiscordCoreInternal {

    class DiscordCoreAPI_Dll DataParser {
    public:
        static void parseObject(json jsonObjectData, DiscordCoreAPI::OverWriteData* pDataStructure) {

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = (DiscordCoreAPI::EditChannelPermissionOverwritesType)jsonObjectData.at("type").get<__int32>();
            }

            if (jsonObjectData.contains("allow") && !jsonObjectData.at("allow").is_null()) {
                pDataStructure->allow = jsonObjectData.at("allow").get<string>();
            }

            if (jsonObjectData.contains("deny") && !jsonObjectData.at("deny").is_null()) {
                pDataStructure->deny = jsonObjectData.at("deny").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::UserData* pDataStructure) {

            if (jsonObjectData.contains("username") && !jsonObjectData.at("username").is_null()) {
                pDataStructure->userName = jsonObjectData.at("username").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
                pDataStructure->createdAt = pDataStructure->getCreatedAtTimestamp();
            }

            if (jsonObjectData.contains("discriminator") && !jsonObjectData.at("discriminator").is_null()) {
                pDataStructure->discriminator = jsonObjectData.at("discriminator").get<string>();
            }

            if (jsonObjectData.contains("avatar") && !jsonObjectData.at("avatar").is_null()) {
                string avatarString = "https://cdn.discordapp.com/avatars/" + pDataStructure->id + "/" + jsonObjectData.at("avatar").get<string>();
                pDataStructure->avatar = avatarString;
            }

            if (jsonObjectData.contains("bot") && !jsonObjectData.at("bot").is_null()) {
                pDataStructure->bot = jsonObjectData.at("bot").get<bool>();
            }

            if (jsonObjectData.contains("system") && !jsonObjectData.at("system").is_null()) {
                pDataStructure->system = jsonObjectData.at("system").get<bool>();
            }

            if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData.at("mfa_enabled").is_null()) {
                pDataStructure->mfaEnabled = jsonObjectData.at("mfa_enabled").get<bool>();
            }

            if (jsonObjectData.contains("locale") && !jsonObjectData.at("locale").is_null()) {
                pDataStructure->locale = jsonObjectData.at("locale").get<string>();
            }

            if (jsonObjectData.contains("verified") && !jsonObjectData.at("verified").is_null()) {
                pDataStructure->verified = jsonObjectData.at("verified").get<bool>();
            }

            if (jsonObjectData.contains("email") && !jsonObjectData.at("email").is_null()) {
                pDataStructure->email = jsonObjectData.at("email").get<string>();
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                pDataStructure->flags = jsonObjectData.at("flags").get<__int32>();
            }

            if (jsonObjectData.contains("premium_type") && !jsonObjectData.at("premium_type").is_null()) {
                pDataStructure->premiumType = jsonObjectData.at("premium_type").get<__int32>();
            }

            if (jsonObjectData.contains("public_flags") && !jsonObjectData.at("public_flags").is_null()) {
                pDataStructure->publicFlags = jsonObjectData.at("public_flags").get<__int32>();
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::User>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::User newData{};
                parseObject(value, &newData);
                pDataStructure->push_back(move(newData));
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMetadataData* pDataStructure) {

            if (jsonObjectData.contains("archived") && !jsonObjectData.at("archived").is_null()) {
                pDataStructure->archived = jsonObjectData.at("archived").get<bool>();
            }

            if (jsonObjectData.contains("invitable") && !jsonObjectData.at("invitable").is_null()) {
                pDataStructure->invitable = jsonObjectData.at("invitable").get<bool>();
            }

            if (jsonObjectData.contains("auto_archive_duration") && !jsonObjectData.at("auto_archive_duration").is_null()) {
                pDataStructure->autoArchiveDuration = jsonObjectData.at("auto_archive_duration").get<__int32>();
            }

            if (jsonObjectData.contains("archive_timestamp") && !jsonObjectData.at("archive_timestamp").is_null()) {
                pDataStructure->archiveTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("archive_timestamp").get<string>());
            }

            if (jsonObjectData.contains("locked") && !jsonObjectData.at("locked").is_null()) {
                pDataStructure->locked = jsonObjectData.at("locked").get<bool>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMemberData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
                pDataStructure->userId = jsonObjectData.at("user_id").get<string>();
            }

            if (jsonObjectData.contains("join_timestamp") && !jsonObjectData.at("join_timestamp").is_null()) {
                pDataStructure->joinTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("join_timestamp").get<string>());
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                pDataStructure->flags = jsonObjectData.at("flags").get<__int32>();
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::ThreadMemberData>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::ThreadMemberData newData{};
                parseObject(value, &newData);
                pDataStructure->push_back(move(newData));
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = (DiscordCoreAPI::ChannelType)jsonObjectData.at("type").get<__int32>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
                pDataStructure->position = jsonObjectData.at("position").get<__int32>();
            }

            if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData.at("permission_overwrites").is_null()) {
                for (auto newValue : jsonObjectData.at("permission_overwrites")) {
                    DiscordCoreAPI::OverWriteData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->permissionOverwrites.insert(make_pair(pDataStructure->id, newData));
                }
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("topic") && !jsonObjectData.at("topic").is_null()) {
                pDataStructure->topic = jsonObjectData.at("topic").get<string>();
            }

            if (jsonObjectData.contains("nsfw") && !jsonObjectData.at("nsfw").is_null()) {
                pDataStructure->nsfw = jsonObjectData.at("nsfw").get<bool>();
            }

            if (jsonObjectData.contains("last_message_id") && !jsonObjectData.at("last_message_id").is_null()) {
                pDataStructure->lastMessageId = jsonObjectData.at("last_message_id").get<string>();
            }

            if (jsonObjectData.contains("bitrate") && !jsonObjectData.at("bitrate").is_null()) {
                pDataStructure->bitrate = jsonObjectData.at("bitrate").get<__int32>();
            }

            if (jsonObjectData.contains("user_limit") && !jsonObjectData.at("user_limit").is_null()) {
                pDataStructure->userLimit = jsonObjectData.at("user_limit").get<__int32>();
            }

            if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData.at("rate_limit_per_user").is_null()) {
                pDataStructure->rateLimitPerUser = jsonObjectData.at("rate_limit_per_user").get<__int32>();
            }

            if (jsonObjectData.contains("recipients") && !jsonObjectData.at("recipients").is_null()) {
                for (auto newValue : jsonObjectData.at("recipients")) {
                    DiscordCoreAPI::UserData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->recipients.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                pDataStructure->icon = jsonObjectData.at("icon").get<string>();
            }

            if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
                pDataStructure->ownerId = jsonObjectData.at("owner_id").get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("application_id").get<string>();
            }

            if (jsonObjectData.contains("parent_id") && !jsonObjectData.at("parent_id").is_null()) {
                pDataStructure->parentId = jsonObjectData.at("parent_id").get<string>();
            }

            if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData.at("last_pin_timestamp").is_null()) {
                pDataStructure->lastPinTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("last_pin_timestamp").get<string>());
            }

            if (jsonObjectData.contains("rtc_region") && !jsonObjectData.at("rtc_region").is_null()) {
                pDataStructure->rtcRegion = jsonObjectData.at("rtc_region").get<string>();
            }

            if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData.at("video_quality_mode").is_null()) {
                pDataStructure->videoQualityMode = jsonObjectData.at("video_quality_mode").get<__int32>();
            }

            if (jsonObjectData.contains("message_count") && !jsonObjectData.at("message_count").is_null()) {
                pDataStructure->messageCount = jsonObjectData.at("message_count").get<__int32>();
            }

            if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
                pDataStructure->memberCount = jsonObjectData.at("member_count").get<__int32>();
            }

            if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
                pDataStructure->memberCount = jsonObjectData.at("member_count").get<__int32>();
            }

            if (jsonObjectData.contains("thread_metadata") && !jsonObjectData.at("thread_metadata").is_null()) {
                parseObject(jsonObjectData.at("thread_metadata"), &pDataStructure->threadMetadata);
            }

            if (jsonObjectData.contains("thread_metadata") && !jsonObjectData.at("thread_metadata").is_null()) {
                parseObject(jsonObjectData.at("thread_metadata"), &pDataStructure->member);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::ChannelData>* pDataStructure) {

            for (auto value : jsonObjectData) {
                DiscordCoreAPI::ChannelData newData{};
                parseObject(value, &newData);
                pDataStructure->push_back(move(newData));
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::RoleTagsData* pDataStructure) {

            if (jsonObjectData.contains("bot_id") && !jsonObjectData.at("bot_id").is_null()) {
                pDataStructure->botId = jsonObjectData.at("bot_id").get<string>();
            }

            if (jsonObjectData.contains("integration_id") && !jsonObjectData.at("integration_id").is_null()) {
                pDataStructure->integrationId = jsonObjectData.at("integration_id").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::RoleData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
                pDataStructure->color = jsonObjectData.at("color").get<__int32>();
            }

            if (jsonObjectData.contains("hoist") && !jsonObjectData.at("hoist").is_null()) {
                pDataStructure->hoist = jsonObjectData.at("hoist").get<bool>();
            }

            if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
                pDataStructure->position = jsonObjectData.at("position").get<__int32>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                pDataStructure->permissions = jsonObjectData.at("permissions").get<string>();
            }

            if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
                pDataStructure->managed = jsonObjectData.at("managed").get<bool>();
            }

            if (jsonObjectData.contains("mentionable") && !jsonObjectData.at("mentionable").is_null()) {
                pDataStructure->mentionable = jsonObjectData.at("mentionable").get<bool>();
            }

            if (jsonObjectData.contains("tags") && !jsonObjectData.at("tags").is_null()) {
                parseObject(jsonObjectData.at("tags"), &pDataStructure->tags);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::Role>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::Role newData{};
                parseObject(value, &newData);
                pDataStructure->push_back(move(newData));
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildMemberData* pDataStructure) {

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

            if (jsonObjectData.contains("nick") && !jsonObjectData.at("nick").is_null()) {
                pDataStructure->nick = jsonObjectData.at("nick").get<string>();
            }
            else {
                pDataStructure->nick = pDataStructure->user.userName;
            }

            if (pDataStructure->nick == "") {
                pDataStructure->userMention = "<@" + pDataStructure->user.id + ">";
            }
            else {
                pDataStructure->userMention = "<@!" + pDataStructure->user.id + ">";
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
                for (auto newValue : jsonObjectData.at("roles")) {
                    pDataStructure->roles.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
                pDataStructure->joinedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("joined_at").get<string>());
            }

            if (jsonObjectData.contains("premium_since") && !jsonObjectData.at("premium_since").is_null()) {
                pDataStructure->premiumSince = jsonObjectData.at("premium_since").get<string>();
            }

            if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
                pDataStructure->deaf = jsonObjectData.at("deaf").get<bool>();
            }

            if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
                pDataStructure->mute = jsonObjectData.at("mute").get<bool>();
            }

            if (jsonObjectData.contains("pending") && !jsonObjectData.at("pending").is_null()) {
                pDataStructure->pending = jsonObjectData.at("pending").get<bool>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                pDataStructure->permissions = jsonObjectData.at("permissions").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            pDataStructure->voiceData = make_shared<DiscordCoreAPI::VoiceStateData>();
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::GuildMemberData>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::GuildMemberData newData{};
                parseObject(value, &newData);
                pDataStructure->push_back(move(newData));
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmojiData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
                for (auto newValue : jsonObjectData.at("roles")) {
                    DiscordCoreAPI::RoleData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->roles.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

            if (jsonObjectData.contains("require_colons") && !jsonObjectData.at("require_colons").is_null()) {
                pDataStructure->requireColons = jsonObjectData.at("require_colons").get<bool>();
            }

            if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
                pDataStructure->managed = jsonObjectData.at("managed").get<bool>();
            }

            if (jsonObjectData.contains("animated") && !jsonObjectData.at("animated").is_null()) {
                pDataStructure->animated = jsonObjectData.at("animated").get<bool>();
            }

            if (jsonObjectData.contains("available") && !jsonObjectData.at("available").is_null()) {
                pDataStructure->available = jsonObjectData.at("available").get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::EmojiData>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::EmojiData newData{};
                parseObject(value, &newData);
                pDataStructure->push_back(move(newData));
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildPreviewData* pDataStructure) {

            if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData.at("approximate_presence_count").is_null()) {
                pDataStructure->approximatePresenceCount = jsonObjectData.at("approximate_presence_count").get<__int32>();
            }

            if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData.at("approximate_member_count").is_null()) {
                pDataStructure->approximateMemberCount = jsonObjectData.at("approximate_member_count").get<__int32>();
            }

            if (jsonObjectData.contains("discovery_splash") && !jsonObjectData.at("discovery_splash").is_null()) {
                pDataStructure->discoverySplash = jsonObjectData.at("discovery_splash").get<string>();
            }

            if (jsonObjectData.contains("emojis") && !jsonObjectData.at("emojis").is_null()) {
                for (auto value : jsonObjectData.at("emojis")) {
                    DiscordCoreAPI::EmojiData newDataValue{};
                    parseObject(value, &newDataValue);
                    pDataStructure->emojis.push_back(newDataValue);
                }
            }

            if (jsonObjectData.contains("features") && !jsonObjectData.at("features").is_null()) {
                for (auto value : jsonObjectData.at("features")) {
                    pDataStructure->features.push_back(value.get<string>());
                }
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("splash") && !jsonObjectData.at("splash").is_null()) {
                pDataStructure->splash = jsonObjectData.at("splash").get<string>();
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                pDataStructure->icon = jsonObjectData.at("icon").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::VoiceStateData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
                pDataStructure->userId = jsonObjectData.at("user_id").get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
                DiscordCoreAPI::GuildMemberData guildMemberData;
                parseObject(jsonObjectData.at("member"), &guildMemberData);
                pDataStructure->userId = guildMemberData.user.id;
            }

            if (jsonObjectData.contains("session_id") && !jsonObjectData.at("session_id").is_null()) {
                pDataStructure->sessionId = jsonObjectData.at("session_id").get<string>();
            }

            if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
                pDataStructure->deaf = jsonObjectData.at("deaf").get<bool>();
            }

            if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
                pDataStructure->mute = jsonObjectData.at("mute").get<bool>();
            }

            if (jsonObjectData.contains("self_deaf") && !jsonObjectData.at("self_deaf").is_null()) {
                pDataStructure->selfDeaf = jsonObjectData.at("self_deaf").get<bool>();
            }

            if (jsonObjectData.contains("self_mute") && !jsonObjectData.at("self_mute").is_null()) {
                pDataStructure->selfMute = jsonObjectData.at("self_mute").get<bool>();
            }

            if (jsonObjectData.contains("self_stream") && !jsonObjectData.at("self_stream").is_null()) {
                pDataStructure->selfStream = jsonObjectData.at("self_stream").get<bool>();
            }

            if (jsonObjectData.contains("self_video") && !jsonObjectData.at("self_video").is_null()) {
                pDataStructure->selfVideo = jsonObjectData.at("self_video").get<bool>();
            }

            if (jsonObjectData.contains("suppress") && !jsonObjectData.at("suppress").is_null()) {
                pDataStructure->suppress = jsonObjectData.at("suppress").get<bool>();
            }

            if (jsonObjectData.contains("request_to_speak_timestamp") && !jsonObjectData.at("request_to_speak_timestamp").is_null()) {
                pDataStructure->requestToSpeakTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("request_to_speak_timestamp").get<string>());
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::PartyData* pDataStructure) {

            if (jsonObjectData.contains("Id") && !jsonObjectData.at("Id").is_null()) {
                pDataStructure->id = jsonObjectData.at("Id").get<string>();
            }

            if (jsonObjectData.contains("Size") && !jsonObjectData.at("Size").is_null()) {
                pDataStructure->size[0] = jsonObjectData.at("Size").at("CurrentSize").get<__int32>();
                pDataStructure->size[1] = jsonObjectData.at("Size").at("MaxSize").get<__int32>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AssetsData* pDataStructure) {

            if (jsonObjectData.contains("LargeImage") && !jsonObjectData.at("LargeImage").is_null()) {
                pDataStructure->largeImage = jsonObjectData.at("LargeImage").get<string>();
            }

            if (jsonObjectData.contains("LargeText") && !jsonObjectData.at("LargeText").is_null()) {
                pDataStructure->largeText = jsonObjectData.at("LargeText").get<string>();
            }

            if (jsonObjectData.contains("SmallImage") && !jsonObjectData.at("SmallImage").is_null()) {
                pDataStructure->smallImage = jsonObjectData.at("SmallImage").get<string>();
            }

            if (jsonObjectData.contains("SmallText") && !jsonObjectData.at("SmallText").is_null()) {
                pDataStructure->smallText = jsonObjectData.at("SmallText").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::SecretsData* pDataStructure) {

            if (jsonObjectData.contains("Join") && !jsonObjectData.at("Join").is_null()) {
                pDataStructure->join = jsonObjectData.at("Join").get<string>();
            }

            if (jsonObjectData.contains("Spectate") && !jsonObjectData.at("Spectate").is_null()) {
                pDataStructure->spectate = jsonObjectData.at("Spectate").get<string>();
            }

            if (jsonObjectData.contains("Match") && !jsonObjectData.at("Match").is_null()) {
                pDataStructure->match = jsonObjectData.at("Match").get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ClientStatusData* pDataStructure) {

            if (jsonObjectData.contains("desktop") && !jsonObjectData.at("desktop").is_null()) {
                pDataStructure->desktop = jsonObjectData.at("desktop").get<string>();
            }

            if (jsonObjectData.contains("mobile") && !jsonObjectData.at("mobile").is_null()) {
                pDataStructure->mobile = jsonObjectData.at("mobile").get<string>();
            }

            if (jsonObjectData.contains("web") && !jsonObjectData.at("web").is_null()) {
                pDataStructure->web = jsonObjectData.at("web").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TimestampData* pDataStructure) {

            if (jsonObjectData.contains("Start") && !jsonObjectData.at("Start").is_null()) {
                pDataStructure->start = jsonObjectData.at("Start").get<__int64>();
            }

            if (jsonObjectData.contains("End") && !jsonObjectData.at("End").is_null()) {
                pDataStructure->end = jsonObjectData.at("End").get<__int64>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ActivityData* pDataStructure) {

            if (jsonObjectData.contains("Name") && !jsonObjectData.at("Name").is_null()) {
                pDataStructure->name = jsonObjectData.at("Name").get<string>();
            }

            if (jsonObjectData.contains("Timestamps") && !jsonObjectData.at("Timestamps").is_null()) {
                parseObject(jsonObjectData.at("Timestamps"), &pDataStructure->timestamps);
            }

            if (jsonObjectData.contains("ApplicationId") && !jsonObjectData.at("ApplicationId").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("ApplicationId").get<string>();
            }

            if (jsonObjectData.contains("Details") && !jsonObjectData.at("Details").is_null()) {
                pDataStructure->details = jsonObjectData.at("Details").get<string>();
            }

            if (jsonObjectData.contains("State") && !jsonObjectData.at("State").is_null()) {
                pDataStructure->state = jsonObjectData.at("State").get<string>();
            }

            if (jsonObjectData.contains("ActivityParty") && !jsonObjectData.at("ActivityParty").is_null()) {
                parseObject(jsonObjectData.at("ActivityParty"), &pDataStructure->party);
            }

            if (jsonObjectData.contains("ActivityAssets") && !jsonObjectData.at("ActivityAssets").is_null()) {
                parseObject(jsonObjectData.at("ActivityAssets"), &pDataStructure->assets);
            }

            if (jsonObjectData.contains("ActivitySecrets") && !jsonObjectData.at("ActivitySecrets").is_null()) {
                parseObject(jsonObjectData.at("ActivitySecrets"), &pDataStructure->secrets);
            }

            if (jsonObjectData.contains("Instance") && !jsonObjectData.at("Instance").is_null()) {
                pDataStructure->instance = jsonObjectData.at("Instance").get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::PresenceUpdateData* pDataStructure) {

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("status") && !jsonObjectData.at("status").is_null()) {
                pDataStructure->status = jsonObjectData.at("status").get<string>();
            }

            if (jsonObjectData.contains("activities") && !jsonObjectData.at("activities").is_null()) {
                for (auto newValue : jsonObjectData.at("activities")) {
                    DiscordCoreAPI::ActivityData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->activities.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("client_status") && !jsonObjectData.at("client_status").is_null()) {
                parseObject(jsonObjectData.at("client_status"), &pDataStructure->clientStatus);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenChannelData* pDataStructure) {

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("emoji_id") && !jsonObjectData.at("emoji_id").is_null()) {
                pDataStructure->emojiId = jsonObjectData.at("emoji_id").get<string>();
            }

            if (jsonObjectData.contains("emoji_name") && !jsonObjectData.at("emoji_name").is_null()) {
                pDataStructure->emojiName = jsonObjectData.at("emoji_name").get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenData* pDataStructure) {

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("welcome_channels") && !jsonObjectData.at("welcome_channels").is_null()) {
                for (auto newValue : jsonObjectData.at("welcome_channels")) {
                    DiscordCoreAPI::WelcomeScreenChannelData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->welcomeChannels.push_back(move(newData));
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::StageInstanceData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("topic") && !jsonObjectData.at("topic").is_null()) {
                pDataStructure->topic = jsonObjectData.at("topic").get<string>();
            }

            if (jsonObjectData.contains("privacy_level") && !jsonObjectData.at("privacy_level").is_null()) {
                pDataStructure->privacyLevel = jsonObjectData.at("privacy_level").get<__int32>();
            }

            if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData.at("discoverable_disabled").is_null()) {
                pDataStructure->discoverableDisabled = jsonObjectData.at("discoverable_disabled").get<bool>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::StickerData* pDataStructure) {

            if (jsonObjectData.contains("asset") && !jsonObjectData.at("asset").is_null()) {
                pDataStructure->asset = jsonObjectData.at("asset").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("format_type") && !jsonObjectData.at("format_type").is_null()) {
                pDataStructure->formatType = jsonObjectData.at("format_type").get<DiscordCoreAPI::StickerFormatType>();
            }

            if (jsonObjectData.contains("available") && !jsonObjectData.at("available").is_null()) {
                pDataStructure->available = jsonObjectData.at("available").get<bool>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("pack_id") && !jsonObjectData.at("pack_id").is_null()) {
                pDataStructure->packId = jsonObjectData.at("pack_id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::StickerType>();
            }

            if (jsonObjectData.contains("sort_value") && !jsonObjectData.at("sort_value").is_null()) {
                pDataStructure->sortValue = jsonObjectData.at("sort_value").get<__int32>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildData* pDataStructure) {
            
            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
                pDataStructure->afkChannelId = jsonObjectData.at("afk_channel_id").get<string>();
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                string iconURLString = "https://cdn.discordapp.com/";
                iconURLString += "icons/" + pDataStructure->id + "/" + jsonObjectData.at("icon").get<string>() + ".png";
                pDataStructure->icon = iconURLString;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("icon_hash") && !jsonObjectData.at("icon_hash").is_null()) {
                pDataStructure->iconHash = jsonObjectData.at("icon_hash").get<string>();
            }

            if (jsonObjectData.contains("splash") && !jsonObjectData.at("splash").is_null()) {
                string iconURLString = "https://cdn.discordapp.com/";
                iconURLString += "splashes/" + pDataStructure->id + "/" + jsonObjectData.at("splash").get<string>() + ".png";
                pDataStructure->splash = iconURLString;
            }

            if (jsonObjectData.contains("discovery_splash") && !jsonObjectData.at("discovery_splash").is_null()) {
                string discordSplashURLString = "https://cdn.discordapp.com/";
                discordSplashURLString += "discovery-splashes/" + pDataStructure->id + "/" + jsonObjectData.at("discovery_splash").get<string>() + ".png";
                pDataStructure->discoverySplash = discordSplashURLString;
            }

            if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
                pDataStructure->owner = jsonObjectData.at("owner").get<bool>();
            }

            if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
                pDataStructure->ownerId = jsonObjectData.at("owner_id").get<string>();
            }

            if (jsonObjectData.contains("preferred_locale") && !jsonObjectData.at("preferred_locale").is_null()) {
                pDataStructure->preferredLocale = jsonObjectData.at("preferred_locale").get<string>();
            }

            if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData.at("public_updates_channel_id").is_null()) {
                pDataStructure->publicUpdatesChannelId = jsonObjectData.at("public_updates_channel_id").get<string>();
            }

            if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData.at("vanity_url_code").is_null()) {
                pDataStructure->vanityURLCode = jsonObjectData.at("vanity_url_code").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("banner") && !jsonObjectData.at("banner").is_null()) {
                string guildBannerURL = "https://cdn.discordapp.com/";
                guildBannerURL += "banners/" + pDataStructure->id + "/" + jsonObjectData.at("banner").get<string>() + ".png";
                pDataStructure->banner = guildBannerURL;
            }

            if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData.at("rule_Channel_id").is_null()) {
                pDataStructure->rulesChannelId = jsonObjectData.at("rule_Channel_id").get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("application_id").get<string>();
            }

            if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
                pDataStructure->joinedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("joined_at").get<string>());
            }

            if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData.at("widget_channel_id").is_null()) {
                pDataStructure->widgetChannelId = jsonObjectData.at("widget_channel_id").get<string>();
            }

            if (jsonObjectData.contains("system_channel_id") && !jsonObjectData.at("system_channel_id").is_null()) {
                pDataStructure->systemChannelId = jsonObjectData.at("system_channel_id").get<string>();
            }

            if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
                pDataStructure->region = jsonObjectData.at("region").get<string>();
            }

            if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
                pDataStructure->afkChannelId = jsonObjectData.at("afk_channel_id").get<string>();
            }

            if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
                pDataStructure->region = jsonObjectData.at("region").get<string>();
            }

            if (jsonObjectData.contains("features") && !jsonObjectData.at("features").is_null()) {
                for (auto newValue : jsonObjectData.at("features")) {
                    pDataStructure->features.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                pDataStructure->permissions = jsonObjectData.at("permissions").get<string>();
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
                for (auto value : jsonObjectData.at("stickers")) {
                    DiscordCoreAPI::StickerData newData{};
                    parseObject(value, &newData);
                    pDataStructure->stickers.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
                for (auto newValue : jsonObjectData.at("roles")) {
                    DiscordCoreAPI::RoleData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->roles.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
                pDataStructure->owner = jsonObjectData.at("owner").get<bool>();
            }

            if (jsonObjectData.contains("afk_timeout") && !jsonObjectData.at("afk_timeout").is_null()) {
                pDataStructure->afkTimeOut = jsonObjectData.at("afk_timeout").get<DiscordCoreAPI::AfkTimeOutDurations>();
            }

            if (jsonObjectData.contains("widget_enabled") && !jsonObjectData.at("widget_enabled").is_null()) {
                pDataStructure->widgetEnabled = jsonObjectData.at("widget_enabled").get<bool>();
            }

            if (jsonObjectData.contains("verification_level") && !jsonObjectData.at("verification_level").is_null()) {
                pDataStructure->verificationLevel = jsonObjectData.at("verification_level").get<DiscordCoreAPI::VerificationLevel>();
            }

            if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData.at("default_message_notification_level").is_null()) {
                pDataStructure->defaultMessageNotifications = jsonObjectData.at("default_message_notification_level").get<DiscordCoreAPI::DefaultMessageNotificationLevel >();
            }

            if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData.at("explicit_content_filter_level").is_null()) {
                pDataStructure->explicitContentFilter = jsonObjectData.at("explicit_content_filter_level").get< DiscordCoreAPI::ExplicitContentFilterLevel>();
            }

            if (jsonObjectData.contains("emojis") && !jsonObjectData.at("emojis").is_null()) {
                for (auto newValue : jsonObjectData.at("emojis")) {
                    DiscordCoreAPI::EmojiData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->emoji.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("mfa_level") && !jsonObjectData.at("mfa_level").is_null()) {
                pDataStructure->mfaLevel = jsonObjectData.at("mfa_level").get< DiscordCoreAPI::MFALevel>();
            }

            if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData.at("system_channel_flags").is_null()) {
                pDataStructure->systemChannelFlags = jsonObjectData.at("system_channel_flags").get<__int32>();
            }

            if (jsonObjectData.contains("large") && !jsonObjectData.at("large").is_null()) {
                pDataStructure->large = jsonObjectData.at("large").get<bool>();
            }

            if (jsonObjectData.contains("unavailable") && !jsonObjectData.at("unavailable").is_null()) {
                pDataStructure->unavailable = jsonObjectData.at("unavailable").get<bool>();
            }

            if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
                pDataStructure->memberCount = jsonObjectData.at("member_count").get<__int32>();
            }

            if (jsonObjectData.contains("voice_states") && !jsonObjectData.at("voice_states").is_null()) {
                for (auto newValue : jsonObjectData.at("voice_states")) {
                    shared_ptr<DiscordCoreAPI::VoiceStateData> newData{ make_shared<DiscordCoreAPI::VoiceStateData>() };
                    parseObject(newValue, newData.get());
                    pDataStructure->voiceStates.insert_or_assign(newData->userId, newData);
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
                for (auto newValue : jsonObjectData.at("members")) {
                    DiscordCoreAPI::GuildMemberData newData{};
                    parseObject(newValue, &newData);
                    newData.guildId = pDataStructure->id;
                    pDataStructure->members.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("channels") && !jsonObjectData.at("channels").is_null()) {
                for (auto newValue : jsonObjectData.at("channels")) {
                    DiscordCoreAPI::ChannelData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->channels.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("threads") && !jsonObjectData.at("threads").is_null()) {
                for (auto newValue : jsonObjectData.at("threads")) {
                    DiscordCoreAPI::ChannelData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->threads.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("presences") && !jsonObjectData.at("presences").is_null()) {
                for (auto newValue : jsonObjectData.at("presences")) {
                    DiscordCoreAPI::PresenceUpdateData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->presences.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("max_presences") && !jsonObjectData.at("max_presences").is_null()) {
                pDataStructure->maxPresences = jsonObjectData.at("max_presences").get<__int32>();
            }

            if (jsonObjectData.contains("max_members") && !jsonObjectData.at("max_members").is_null()) {
                pDataStructure->maxMembers = jsonObjectData.at("max_members").get<__int32>();
            }

            if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData.at("premium_subscription_count").is_null()) {
                pDataStructure->premiumSubscriptionCount = jsonObjectData.at("premium_subscription_count").get<__int32>();
            }

            if (jsonObjectData.contains("premium_tier") && !jsonObjectData.at("premium_tier").is_null()) {
                pDataStructure->premiumTier = jsonObjectData.at("premium_tier").get< DiscordCoreAPI::PremiumTier>();
            }

            if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData.at("max_video_channel_users").is_null()) {
                pDataStructure->maxVideoChannelUsers = jsonObjectData.at("max_video_channel_users").get<__int32>();
            }

            if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData.at("approximate_member_count").is_null()) {
                pDataStructure->approximateMemberCount = jsonObjectData.at("approximate_member_count").get<__int32>();
            }

            if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData.at("approximate_presence_count").is_null()) {
                pDataStructure->approximatePresenceCount = jsonObjectData.at("approximate_presence_count").get<__int32>();
            }

            if (jsonObjectData.contains("welcome_screen") && !jsonObjectData.at("welcome_screen").is_null()) {
                parseObject(jsonObjectData.at("welcome_screen"), &pDataStructure->welcomeScreen);
            }

            if (jsonObjectData.contains("nsfw_level") && !jsonObjectData.at("nsfw_level").is_null()) {
                pDataStructure->nsfwLevel = jsonObjectData.at("nsfw_level").get<__int32>();
            }

            if (jsonObjectData.contains("stage_instances") && !jsonObjectData.at("stage_instances").is_null()) {
                for (auto newValue : jsonObjectData.at("stage_instances")) {
                    DiscordCoreAPI::StageInstanceData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->stageInstances.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->createdAt = pDataStructure->getCreatedAtTimestamp();
            }
        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelMentionData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::ChannelType>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }
        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AttachmentData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("filename") && !jsonObjectData.at("filename").is_null()) {
                pDataStructure->filename = jsonObjectData.at("filename").get<string>();
            }

            if (jsonObjectData.contains("content_type") && !jsonObjectData.at("content_type").is_null()) {
                pDataStructure->contentType = jsonObjectData.at("content_type").get<string>();
            }

            if (jsonObjectData.contains("size") && !jsonObjectData.at("size").is_null()) {
                pDataStructure->size = jsonObjectData.at("size").get<__int32>();
            }

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                pDataStructure->proxyUrl = jsonObjectData.at("proxy_url").get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                pDataStructure->width = jsonObjectData.at("width").get<__int32>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                pDataStructure->height = jsonObjectData.at("height").get<__int32>();
            }
        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFooterData* pDataStructure) {

            if (jsonObjectData.contains("text") && !jsonObjectData.at("text").is_null()) {
                pDataStructure->text = jsonObjectData.at("text").get<string>();
            }

            if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
                pDataStructure->iconUrl = jsonObjectData.at("icon_url").get<string>();
            }

            if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
                pDataStructure->proxyIconUrl = jsonObjectData.at("proxy_icon_url").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedImageData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                pDataStructure->proxyUrl = jsonObjectData.at("proxy_url").get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                pDataStructure->width = jsonObjectData.at("width").get<__int32>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                pDataStructure->height = jsonObjectData.at("height").get<__int32>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedThumbnailData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                pDataStructure->proxyUrl = jsonObjectData.at("proxy_url").get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                pDataStructure->width = jsonObjectData.at("width").get<__int32>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                pDataStructure->height = jsonObjectData.at("height").get<__int32>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedVideoData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
                pDataStructure->proxyUrl = jsonObjectData.at("proxy_url").get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
                pDataStructure->width = jsonObjectData.at("width").get<__int32>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
                pDataStructure->height = jsonObjectData.at("height").get<__int32>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedAuthorData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }

            if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
                pDataStructure->proxyIconUrl = jsonObjectData.at("proxy_icon_url").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
                pDataStructure->iconUrl = jsonObjectData.at("icon_url").get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedProviderData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFieldData* pDataStructure) {

            if (jsonObjectData.contains("inline") && !jsonObjectData.at("inline").is_null()) {
                pDataStructure->Inline = jsonObjectData.at("inline").get<bool>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
                pDataStructure->value = jsonObjectData.at("value").get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedData* pDataStructure) {

            if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null()) {
                pDataStructure->title = jsonObjectData.at("title").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
                pDataStructure->timestamp = jsonObjectData.at("timestamp").get<string>();
            }

            if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
                __int32 colorValInt = (__int32)jsonObjectData.at("color").get<__int32>();
                stringstream stream;
                stream << setbase(16) << colorValInt;
                pDataStructure->hexColorValue = stream.str();
            }

            if (jsonObjectData.contains("footer") && !jsonObjectData.at("footer").is_null()) {
                parseObject(jsonObjectData.at("footer"), &pDataStructure->footer);
            }

            if (jsonObjectData.contains("image") && !jsonObjectData.at("image").is_null()) {
                parseObject(jsonObjectData.at("image"), &pDataStructure->image);
            }

            if (jsonObjectData.contains("provider") && !jsonObjectData.at("provider").is_null()) {
                parseObject(jsonObjectData.at("provider"), &pDataStructure->provider);
            }

            if (jsonObjectData.contains("thumbnail") && !jsonObjectData.at("thumbnail").is_null()) {
                parseObject(jsonObjectData.at("thumbnail"), &pDataStructure->thumbnail);
            }

            if (jsonObjectData.contains("video") && !jsonObjectData.at("video").is_null()) {
                parseObject(jsonObjectData.at("video"), &pDataStructure->video);
            }

            if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
                parseObject(jsonObjectData.at("author"), &pDataStructure->author);
            }

            if (jsonObjectData.contains("fields") && !jsonObjectData.at("fields").is_null()) {
                for (auto newValue : jsonObjectData.at("fields")) {
                    DiscordCoreAPI::EmbedFieldData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->fields.push_back(move(newData));
                }
            }

        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionData* pDataStructure) {

            if (jsonObjectData.contains("count") && !jsonObjectData.at("count").is_null()) {
                pDataStructure->count = jsonObjectData.at("count").get<__int32>();
            }

            if (jsonObjectData.contains("me") && !jsonObjectData.at("me").is_null()) {
                pDataStructure->count = jsonObjectData.at("me").get<bool>();
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
                parseObject(jsonObjectData.at("emoji"), &pDataStructure->emoji);
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
                pDataStructure->userId = jsonObjectData.at("user_id").get<string>();
            }

            if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
                pDataStructure->messageId = jsonObjectData.at("message_id").get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
                parseObject(jsonObjectData.at("member"), &pDataStructure->member);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::Reaction>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::Reaction newInvite;
                parseObject(value, &newInvite);
                pDataStructure->push_back(newInvite);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageActivityData* pDataStructure) {

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::MessageActivityType>();
            }

            if (jsonObjectData.contains("party_id") && !jsonObjectData.at("party_id").is_null()) {
                pDataStructure->partyId = jsonObjectData.at("party_id").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TeamMembersObjectData* pDataStructure) {

            if (jsonObjectData.contains("membership_state") && !jsonObjectData.at("membership_state").is_null()) {
                pDataStructure->membershipState = jsonObjectData.at("membership_state").get<__int32>();
            }

            if (jsonObjectData.contains("team_id") && !jsonObjectData.at("team_id").is_null()) {
                pDataStructure->teamId = jsonObjectData.at("team_id").get<string>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                for (auto newValue : jsonObjectData.at("permissions")) {
                    pDataStructure->permissions.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TeamObjectData* pDataStructure) {

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                pDataStructure->icon = jsonObjectData.at("icon").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
                for (auto newValue : jsonObjectData.at("members")) {
                    DiscordCoreAPI::TeamMembersObjectData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->members.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("owner_user_id") && !jsonObjectData.at("owner_user_id").is_null()) {
                pDataStructure->ownerUserId = jsonObjectData.at("owner_user_id").get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
                pDataStructure->icon = jsonObjectData.at("icon").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("rpc_origins") && !jsonObjectData.at("rpc_origins").is_null()) {
                for (auto newValue : jsonObjectData.at("rpc_origins")) {
                    pDataStructure->rpcOrigins.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("bot_public") && !jsonObjectData.at("bot_public").is_null()) {
                pDataStructure->botPublic = jsonObjectData.at("bot_public").get<bool>();
            }

            if (jsonObjectData.contains("bot_require_code_grant") && !jsonObjectData.at("bot_require_code_grant").is_null()) {
                pDataStructure->botRequireCodeGrant = jsonObjectData.at("bot_require_code_grant").get<bool>();
            }

            if (jsonObjectData.contains("terms_of_service_url") && !jsonObjectData.at("terms_of_service_url").is_null()) {
                pDataStructure->termsOfServiceUrl = jsonObjectData.at("terms_of_service_url").get<string>();
            }

            if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData.at("privacy_policy_url").is_null()) {
                pDataStructure->privacyPolicyUrl = jsonObjectData.at("privacy_policy_url").get<string>();
            }

            if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
                parseObject(jsonObjectData.at("owner"), &pDataStructure->owner);
            }

            if (jsonObjectData.contains("summary") && !jsonObjectData.at("summary").is_null()) {
                pDataStructure->summary = jsonObjectData.at("summary").get<string>();
            }

            if (jsonObjectData.contains("verify_key") && !jsonObjectData.at("verify_key").is_null()) {
                pDataStructure->verifyKey = jsonObjectData.at("verify_key").get<string>();
            }

            if (jsonObjectData.contains("team") && !jsonObjectData.at("team").is_null()) {
                parseObject(jsonObjectData.at("team"), &pDataStructure->team);
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData.at("primary_sku_id").is_null()) {
                pDataStructure->primarySkuId = jsonObjectData.at("primary_sku_id").get<string>();
            }

            if (jsonObjectData.contains("slug") && !jsonObjectData.at("slug").is_null()) {
                pDataStructure->slug = jsonObjectData.at("slug").get<string>();
            }

            if (jsonObjectData.contains("cover_image") && !jsonObjectData.at("cover_image").is_null()) {
                pDataStructure->coverImage = jsonObjectData.at("cover_image").get<string>();
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                pDataStructure->coverImage = jsonObjectData.at("flags").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InviteData* pDataStructure) {

            if (jsonObjectData.contains("code") && !jsonObjectData.at("code").is_null() && jsonObjectData.at("code").type() == nlohmann::detail::value_t::string) {
                pDataStructure->code = jsonObjectData.at("code").get<string>();
            }
            else if (jsonObjectData.contains("code") && !jsonObjectData.at("code").is_null() && jsonObjectData.at("code").type() == nlohmann::detail::value_t::number_integer) {
                pDataStructure->code = to_string(jsonObjectData.at("code").get<__int32>());
            }

            if (jsonObjectData.contains("guild") && !jsonObjectData.at("guild").is_null()) {
                parseObject(jsonObjectData.at("guild"), &pDataStructure->guild);
            }

            if (jsonObjectData.contains("channel") && !jsonObjectData.at("channel").is_null()) {
                parseObject(jsonObjectData.at("channel"), &pDataStructure->channel);
            }

            if (jsonObjectData.contains("inviter") && !jsonObjectData.at("inviter").is_null()) {
                parseObject(jsonObjectData.at("inviter"), &pDataStructure->inviter);
            }

            if (jsonObjectData.contains("target_type") && !jsonObjectData.at("target_type").is_null()) {
                pDataStructure->targetType = jsonObjectData.at("target_type").get<__int32>();
            }

            if (jsonObjectData.contains("target_user") && !jsonObjectData.at("target_user").is_null()) {
                parseObject(jsonObjectData.at("target_user"), &pDataStructure->targetUser);
            }

            if (jsonObjectData.contains("target_application") && !jsonObjectData.at("target_application").is_null()) {
                parseObject(jsonObjectData.at("target_application"), &pDataStructure->targetApplication);
            }

            if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData.at("approximate_presence_count").is_null()) {
                pDataStructure->approximatePresenceCount = jsonObjectData.at("approximate_presence_count").get<__int32>();
            }

            if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData.at("approximate_member_count").is_null()) {
                pDataStructure->approximateMemberCount = jsonObjectData.at("approximate_member_count").get<__int32>();
            }

            if (jsonObjectData.contains("expires_at") && !jsonObjectData.at("expires_at").is_null()) {
                pDataStructure->expiresAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("expires_at").get<string>());
            }

            if (jsonObjectData.contains("stage_instance") && !jsonObjectData.at("stage_instance").is_null()) {
                parseObject(jsonObjectData.at("stage_instance"), &pDataStructure->stageInstance);
            }

            if (jsonObjectData.contains("uses") && !jsonObjectData.at("uses").is_null()) {
                pDataStructure->uses = jsonObjectData.at("uses").get<__int32>();
            }

            if (jsonObjectData.contains("max_uses") && !jsonObjectData.at("max_uses").is_null()) {
                pDataStructure->maxUses = jsonObjectData.at("max_uses").get<__int32>();
            }

            if (jsonObjectData.contains("max_age") && !jsonObjectData.at("max_age").is_null()) {
                pDataStructure->maxAge = jsonObjectData.at("max_age").get<__int32>();
            }

            if (jsonObjectData.contains("temporary") && !jsonObjectData.at("temporary").is_null()) {
                pDataStructure->temporary = jsonObjectData.at("temporary").get<bool>();
            }

            if (jsonObjectData.contains("created_at") && !jsonObjectData.at("created_at").is_null()) {
                pDataStructure->createdAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("created_at").get<string>());
            }
        };

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::InviteData>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::InviteData newInvite{};
                parseObject(value, &newInvite);
                pDataStructure->push_back(newInvite);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageReferenceData* pDataStructure) {

            if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
                pDataStructure->messageId = jsonObjectData.at("message_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("fail_if_not_exists") && !jsonObjectData.at("fail_if_not_exists").is_null()) {
                pDataStructure->failIfNotExists = jsonObjectData.at("fail_if_not_exists").get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AllowedMentionsData* pDataStructure) {

            if (jsonObjectData.contains("parse") && !jsonObjectData.at("parse").is_null()) {
                for (auto newValue : jsonObjectData.at("parse")) {
                    pDataStructure->parse.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
                for (auto newValue : jsonObjectData.at("roles")) {
                    pDataStructure->roles.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("users") && !jsonObjectData.at("users").is_null()) {
                for (auto newValue : jsonObjectData.at("users")) {
                    pDataStructure->users.push_back(newValue.get<string>());
                }

            }

            if (jsonObjectData.contains("replied_user") && !jsonObjectData.at("replied_user").is_null()) {
                pDataStructure->repliedUser = jsonObjectData.at("replied_user").get<bool>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionApplicationCommandCallbackData* pDataStructure) {

            if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
                pDataStructure->tts = jsonObjectData.at("tts").get<bool>();
            }

            if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
                pDataStructure->content = jsonObjectData.at("content").get<string>();
            }

            if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
                for (auto newValue : jsonObjectData.at("embeds")) {
                    DiscordCoreAPI::EmbedData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->embeds.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                pDataStructure->flags = jsonObjectData.at("flags").get<__int32>();
            }

            if (jsonObjectData.contains("allowed_mentions") && !jsonObjectData.at("allowed_mentions").is_null()) {
                parseObject(jsonObjectData.at("allowed_mentions"), &pDataStructure->allowedMentions);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionResponseData* pDataStructure) {

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::InteractionCallbackType>();
            }

            if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
                parseObject(jsonObjectData.at("data"), &pDataStructure->data);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::SelectOptionData* pDataStructure) {

            if (jsonObjectData.contains("label") && !jsonObjectData.at("label").is_null()) {
                pDataStructure->label = jsonObjectData.at("label").get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
                pDataStructure->value = jsonObjectData.at("value").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
                parseObject(jsonObjectData.at("emoji"), &pDataStructure->emoji);
            }

            if (jsonObjectData.contains("default") && !jsonObjectData.at("default").is_null()) {
                pDataStructure->_default = jsonObjectData.at("default").get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ActionRowData* pDataStructure) {

            if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
                for (auto newValue : jsonObjectData.at("components")) {
                    DiscordCoreAPI::ComponentData newData{};
                    if (newValue.contains("type") && !newValue.at("type").is_null()) {
                        newData.type = newValue.at("type").get<DiscordCoreAPI::ComponentType>();
                    }

                    if (newValue.contains("custom_id") && !newValue.at("custom_id").is_null()) {
                        newData.customId = newValue.at("custom_id").get<string>();
                    }

                    if (newValue.contains("placeholder") && !newValue.at("placeholder").is_null()) {
                        newData.placeholder = newValue.at("placeholder").get<string>();
                    }

                    if (newValue.contains("disabled") && !newValue.at("disabled").is_null()) {
                        newData.disabled = newValue.at("disabled").get<bool>();
                    }

                    if (newValue.contains("style") && !newValue.at("style").is_null()) {
                        newData.style = newValue.at("style").get<DiscordCoreAPI::ButtonStyle>();
                    }

                    if (newValue.contains("label") && !newValue.at("label").is_null()) {
                        newData.label = newValue.at("label").get<string>();
                    }

                    if (newValue.contains("emoji") && !newValue.at("emoji").is_null()) {
                        parseObject(newValue.at("emoji"), &newData.emoji);
                    }

                    if (newValue.contains("url") && !newValue.at("url").is_null()) {
                        newData.url = newValue.at("url").get<string>();
                    }

                    if (newValue.contains("options") && !newValue.at("options").is_null()) {
                        for (auto value : newValue.at("options")) {
                            DiscordCoreAPI::SelectOptionData newData02;
                            parseObject(value, &newData02);
                            newData.options.push_back(newData02);
                        }
                    }

                    if (newValue.contains("max_values") && !newValue.at("max_values").is_null()) {
                        newData.maxValues = newValue.at("max_values").get<__int32>();
                    }

                    if (newValue.contains("min_values") && !newValue.at("min_values").is_null()) {
                        newData.minValues = newValue.at("min_values").get<__int32>();
                    }

                    pDataStructure->components.push_back(move(newData));
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageInteractionData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::InteractionType>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::StickerItemData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("format_type") && !jsonObjectData.at("format_type").is_null()) {
                pDataStructure->formatType = jsonObjectData.at("format_type").get<DiscordCoreAPI::StickerItemType>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageDataOld* pDataStructure) {

            if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
                pDataStructure->content = jsonObjectData.at("content").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
                parseObject(jsonObjectData.at("author"), &pDataStructure->author);
            }

            if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
                parseObject(jsonObjectData.at("member"), &pDataStructure->member);
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
                pDataStructure->timestampRaw = jsonObjectData.at("timestamp").get<string>();
                pDataStructure->timestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("timestamp").get<string>());
            }

            if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData.at("edited_timestamp").is_null()) {
                pDataStructure->editedTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("edited_timestamp").get<string>());
            }

            if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
                pDataStructure->tts = jsonObjectData.at("tts").get<bool>();
            }

            if (jsonObjectData.contains("mention_everyone") && !jsonObjectData.at("mention_everyone").is_null()) {
                pDataStructure->mentionEveryone = jsonObjectData.at("mention_everyone").get<bool>();
            }

            if (jsonObjectData.contains("mentions") && !jsonObjectData.at("mentions").is_null()) {
                for (auto newValue : jsonObjectData.at("mentions")) {
                    DiscordCoreAPI::UserData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->mentions.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("mention_roles") && !jsonObjectData.at("mention_roles").is_null()) {
                for (auto newValue : jsonObjectData.at("mention_roles")) {
                    pDataStructure->mentionRoles.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("mention_channels") && !jsonObjectData.at("mention_channels").is_null()) {
                for (auto newValue : jsonObjectData.at("mention_channels")) {
                    DiscordCoreAPI::ChannelMentionData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->mentionChannels.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("attachments") && !jsonObjectData.at("attachments").is_null()) {
                for (auto newValue : jsonObjectData.at("attachments")) {
                    DiscordCoreAPI::AttachmentData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->attachments.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
                for (auto newValue : jsonObjectData.at("embeds")) {
                    DiscordCoreAPI::EmbedData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->embeds.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("reactions") && !jsonObjectData.at("reactions").is_null()) {
                for (auto newValue : jsonObjectData.at("reactions")) {
                    DiscordCoreAPI::ReactionData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->reactions.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("nonce") && !jsonObjectData.at("nonce").is_null()) {
                pDataStructure->nonce = jsonObjectData.at("nonce").get<string>();
            }

            if (jsonObjectData.contains("pinned") && !jsonObjectData.at("pinned").is_null()) {
                pDataStructure->pinned = jsonObjectData.at("pinned").get<bool>();
            }

            if (jsonObjectData.contains("webhook_id") && !jsonObjectData.at("webhook_id").is_null()) {
                pDataStructure->webhookId = jsonObjectData.at("webhook_id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = (DiscordCoreAPI::MessageType)jsonObjectData.at("type").get<__int32>();
            }

            if (jsonObjectData.contains("activity") && !jsonObjectData.at("activity").is_null()) {
                parseObject(jsonObjectData.at("activity"), &pDataStructure->activity);
            }

            if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
                parseObject(jsonObjectData.at("application"), &pDataStructure->application);
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("application_id").get<string>();
            }

            if (jsonObjectData.contains("message_reference") && !jsonObjectData.at("message_reference").is_null()) {
                parseObject(jsonObjectData.at("message_reference"), &pDataStructure->messageReference);
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                pDataStructure->flags = jsonObjectData.at("flags").get<__int32>();
            }

            if (jsonObjectData.contains("sticker_items") && !jsonObjectData.at("sticker_items").is_null()) {
                for (auto newValue : jsonObjectData.at("sticker_items")) {
                    DiscordCoreAPI::StickerItemData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->stickerItems.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
                for (auto newValue : jsonObjectData.at("stickers")) {
                    DiscordCoreAPI::StickerData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->stickers.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("interaction") && !jsonObjectData.at("interaction").is_null()) {
                parseObject(jsonObjectData.at("interaction"), &pDataStructure->interaction);
            }

            if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
                for (auto& newValue : jsonObjectData.at("components")) {
                    DiscordCoreAPI::ActionRowData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->components.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("thread") && !jsonObjectData.at("thread").is_null()) {
                parseObject(jsonObjectData.at("thread"), &pDataStructure->thread);
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageData* pDataStructure) {

            if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
                pDataStructure->content = jsonObjectData.at("content").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
                parseObject(jsonObjectData.at("author"), &pDataStructure->author);
            }

            if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
                parseObject(jsonObjectData.at("member"), &pDataStructure->member);
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
                pDataStructure->timestampRaw = jsonObjectData.at("timestamp").get<string>();
                pDataStructure->timestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("timestamp").get<string>());
            }

            if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData.at("edited_timestamp").is_null()) {
                pDataStructure->editedTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("edited_timestamp").get<string>());
            }

            if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
                pDataStructure->tts = jsonObjectData.at("tts").get<bool>();
            }

            if (jsonObjectData.contains("mention_everyone") && !jsonObjectData.at("mention_everyone").is_null()) {
                pDataStructure->mentionEveryone = jsonObjectData.at("mention_everyone").get<bool>();
            }

            if (jsonObjectData.contains("mentions") && !jsonObjectData.at("mentions").is_null()) {
                for (auto newValue : jsonObjectData.at("mentions")) {
                    DiscordCoreAPI::UserData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->mentions.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("mention_roles") && !jsonObjectData.at("mention_roles").is_null()) {
                for (auto newValue : jsonObjectData.at("mention_roles")) {
                    pDataStructure->mentionRoles.push_back(newValue.get<string>());
                }
            }

            if (jsonObjectData.contains("mention_channels") && !jsonObjectData.at("mention_channels").is_null()) {
                for (auto newValue : jsonObjectData.at("mention_channels")) {
                    DiscordCoreAPI::ChannelMentionData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->mentionChannels.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("attachments") && !jsonObjectData.at("attachments").is_null()) {
                for (auto newValue : jsonObjectData.at("attachments")) {
                    DiscordCoreAPI::AttachmentData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->attachments.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
                for (auto newValue : jsonObjectData.at("embeds")) {
                    DiscordCoreAPI::EmbedData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->embeds.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("reactions") && !jsonObjectData.at("reactions").is_null()) {
                for (auto newValue : jsonObjectData.at("reactions")) {
                    DiscordCoreAPI::ReactionData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->reactions.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("nonce") && !jsonObjectData.at("nonce").is_null()) {
                pDataStructure->nonce = jsonObjectData.at("nonce").get<string>();
            }

            if (jsonObjectData.contains("pinned") && !jsonObjectData.at("pinned").is_null()) {
                pDataStructure->pinned = jsonObjectData.at("pinned").get<bool>();
            }

            if (jsonObjectData.contains("webhook_id") && !jsonObjectData.at("webhook_id").is_null()) {
                pDataStructure->webhookId = jsonObjectData.at("webhook_id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = (DiscordCoreAPI::MessageType)jsonObjectData.at("type").get<__int32>();
            }

            if (jsonObjectData.contains("activity") && !jsonObjectData.at("activity").is_null()) {
                parseObject(jsonObjectData.at("activity"), &pDataStructure->activity);
            }

            if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
                parseObject(jsonObjectData.at("application"), &pDataStructure->application);
            }

            if (jsonObjectData.contains("message_reference") && !jsonObjectData.at("message_reference").is_null()) {
                parseObject(jsonObjectData.at("message_reference"), &pDataStructure->messageReference);
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
                pDataStructure->flags = jsonObjectData.at("flags").get<__int32>();
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
                for (auto newValue : jsonObjectData.at("stickers")) {
                    DiscordCoreAPI::StickerData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->stickers.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("interaction") && !jsonObjectData.at("interaction").is_null()) {
                parseObject(jsonObjectData.at("interaction"), &pDataStructure->interaction);
            }

            if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
                for (auto& newValue : jsonObjectData.at("components")) {
                    DiscordCoreAPI::ActionRowData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->components.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("referenced_message") && !jsonObjectData.at("referenced_message").is_null()) {
                parseObject(jsonObjectData.at("referenced_message"), &pDataStructure->referencedMessage);
            }

            if (jsonObjectData.contains("thread") && !jsonObjectData.at("thread").is_null()) {
                parseObject(jsonObjectData.at("thread"), &pDataStructure->thread);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::Message>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::Message newMessage{};
                parseObject(value, &newMessage);
                pDataStructure->push_back(newMessage);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandOptionChoiceData* pDataStructure) {

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null() && jsonObjectData.at("value").is_string()) {
                pDataStructure->valueString = jsonObjectData.at("value").get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null() && jsonObjectData.at("value").is_number_integer()) {
                pDataStructure->valueInt = jsonObjectData.at("value").get<__int32>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandOptionData* pDataStructure) {

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("channel_types") && !jsonObjectData.at("channel_types").is_null()) {
                for (auto value : jsonObjectData.at("channel_types")) {
                    pDataStructure->channelTypes.push_back(value);
                }
            }

            if (jsonObjectData.contains("required") && !jsonObjectData.at("required").is_null()) {
                pDataStructure->required = jsonObjectData.at("required").get<bool>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::ApplicationCommandOptionType>();
            }

            if (pDataStructure->type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND_GROUP || pDataStructure->type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND) {
                if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                    for (auto newValue : jsonObjectData.at("options")) {
                        DiscordCoreAPI::ApplicationCommandOptionData appCommandOptionDataNew;
                        parseObject(newValue, &appCommandOptionDataNew);
                        pDataStructure->options.push_back(appCommandOptionDataNew);
                    }
                }
            }
            else {
                if (jsonObjectData.contains("choices") && !jsonObjectData.at("choices").is_null()) {
                    for (auto newValue : jsonObjectData.at("choices")) {
                        DiscordCoreAPI::ApplicationCommandOptionChoiceData appCommandChoiceData;
                        parseObject(newValue, &appCommandChoiceData);
                        pDataStructure->choices.push_back(appCommandChoiceData);
                    }
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("application_id").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                pDataStructure->description = jsonObjectData.at("description").get<string>();
            }

            if (jsonObjectData.contains("version") && !jsonObjectData.at("version").is_null()) {
                pDataStructure->version = jsonObjectData.at("version").get<string>();
            }

            if (jsonObjectData.contains("default_permission") && !jsonObjectData.at("default_permission").is_null()) {
                pDataStructure->defaultPermission = jsonObjectData.at("default_permission").get<bool>();
            }

            if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                for (auto newValue : jsonObjectData.at("options")) {
                    DiscordCoreAPI::ApplicationCommandOptionData appCommandOptionData;
                    parseObject(newValue, &appCommandOptionData);
                    pDataStructure->options.push_back(appCommandOptionData);
                }
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::ApplicationCommand>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::ApplicationCommand newInvite{};
                parseObject(value, &newInvite);
                pDataStructure->push_back(newInvite);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionDataOption* pDataStructure) {

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::ApplicationCommandOptionType>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
                if (jsonObjectData.at("value").is_boolean()) {
                    pDataStructure->valueBool = jsonObjectData.at("value").get<bool>();
                }
                if (jsonObjectData.at("value").is_string()) {
                    pDataStructure->valueString = jsonObjectData.at("value").get<string>();
                }
                if (jsonObjectData.at("value").is_number()) {
                    pDataStructure->valueInt = jsonObjectData.at("value").get<__int32>();
                }
            }

            if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                for (auto newValue : jsonObjectData.at("options")) {
                    DiscordCoreAPI::ApplicationCommandInteractionDataOption newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->options.push_back(move(newData));
                }
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::YouTubeFormat>* pDataStructure) {

            if (!jsonObjectData.is_null()) {
                if (jsonObjectData.contains("streamingData") && !jsonObjectData.at("streamingData").is_null() && jsonObjectData.at("streamingData").contains("formats") && !jsonObjectData.at("streamingData").at("formats").is_null()) {
                    for (auto value : jsonObjectData.at("streamingData").at("formats")) {
                        DiscordCoreAPI::YouTubeFormat newData{};
                        if (value.contains("audioQuality") && !value.at("audioQuality").is_null()) {
                            newData.audioQuality = value.at("audioQuality").get<string>();
                        }
                        if (value.contains("averageBitrate") && !value.at("averageBitrate").is_null()) {
                            newData.averageBitrate = value.at("averageBitrate").get<__int32>();
                        }
                        if (value.contains("audioSampleRate") && !value.at("audioSampleRate").is_null()) {
                            newData.audioSampleRate = value.at("audioSampleRate").get<string>();
                        }
                        if (value.contains("bitrate") && !value.at("bitrate").is_null()) {
                            newData.bitrate = value.at("bitrate").get<__int32>();
                        }
                        if (value.contains("contentLength") && !value.at("contentLength").is_null()) {
                            newData.contentLength = stoll(value.at("contentLength").get<string>());
                        }
                        if (value.contains("fps") && !value.at("fps").is_null()) {
                            newData.fps = value.at("fps").get<__int32>();
                        }
                        if (value.contains("height") && !value.at("height").is_null()) {
                            newData.height = value.at("height").get<__int32>();
                        }
                        if (value.contains("width") && !value.at("width").is_null()) {
                            newData.height = value.at("width").get<__int32>();
                        }
                        if (value.contains("aitags") && !value.at("aitags").is_null()) {
                            newData.aitags = value.at("aitags").get<string>();
                        }
                        if (value.contains("itag") && !value.at("itag").is_null()) {
                            newData.itag = value.at("itag").get<__int32>();
                        }
                        if (value.contains("mimeType") && !value.at("mimeType").is_null()) {
                            newData.mimeType = value.at("mimeType").get<string>();
                        }
                        if (value.contains("quality") && !value.at("quality").is_null()) {
                            newData.quality = value.at("quality").get<string>();
                        }
                        if (value.contains("signatureCipher") && !value.at("signatureCipher").is_null()) {
                            newData.signatureCipher = value.at("signatureCipher").get<string>();
                        }
                        else if (value.contains("cipher") && !value.at("cipher").is_null()) {
                            newData.signatureCipher = value.at("cipher").get<string>();
                        }
                        if (newData.signatureCipher.find("&sp") != string::npos) {
                            newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
                        }
                        if (newData.signatureCipher.find("url") != string::npos) {
                            newData.downloadURL = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
                        }
                        else if (value.contains("url") && !value.at("url").is_null()) {
                            newData.downloadURL = value.at("url");
                        }
                        pDataStructure->push_back(move(newData));
                    }
                }

                if (jsonObjectData.contains("streamingData") && !jsonObjectData.at("streamingData").is_null() && jsonObjectData.at("streamingData").contains("adaptiveFormats") && !jsonObjectData.at("streamingData").at("adaptiveFormats").is_null()) {
                    for (auto value : jsonObjectData.at("streamingData").at("adaptiveFormats")) {
                        DiscordCoreAPI::YouTubeFormat newData{};
                        if (value.contains("audioQuality") && !value.at("audioQuality").is_null()) {
                            newData.audioQuality = value.at("audioQuality").get<string>();
                        }
                        if (value.contains("averageBitrate") && !value.at("averageBitrate").is_null()) {
                            newData.averageBitrate = value.at("averageBitrate").get<__int32>();
                        }
                        if (value.contains("audioSampleRate") && !value.at("audioSampleRate").is_null()) {
                            newData.audioSampleRate = value.at("audioSampleRate").get<string>();
                        }
                        if (value.contains("bitrate") && !value.at("bitrate").is_null()) {
                            newData.bitrate = value.at("bitrate").get<__int32>();
                        }
                        if (value.contains("contentLength") && !value.at("contentLength").is_null()) {
                            newData.contentLength = stoll(value.at("contentLength").get<string>());
                        }
                        if (value.contains("fps") && !value.at("fps").is_null()) {
                            newData.fps = value.at("fps").get<__int32>();
                        }
                        if (value.contains("height") && !value.at("height").is_null()) {
                            newData.height = value.at("height").get<__int32>();
                        }
                        if (value.contains("width") && !value.at("width").is_null()) {
                            newData.height = value.at("width").get<__int32>();
                        }
                        if (value.contains("aitags") && !value.at("aitags").is_null()) {
                            newData.aitags = value.at("aitags").get<string>();
                        }
                        if (value.contains("itag") && !value.at("itag").is_null()) {
                            newData.itag = value.at("itag").get<__int32>();
                        }
                        if (value.contains("mimeType") && !value.at("mimeType").is_null()) {
                            newData.mimeType = value.at("mimeType").get<string>();
                        }
                        if (value.contains("quality") && !value.at("quality").is_null()) {
                            newData.quality = value.at("quality").get<string>();
                        }
                        if (value.contains("signatureCipher") && !value.at("signatureCipher").is_null()) {
                            newData.signatureCipher = value.at("signatureCipher").get<string>();
                        }
                        else if (value.contains("cipher") && !value.at("cipher").is_null()) {
                            newData.signatureCipher = value.at("cipher").get<string>();
                        }
                        if (newData.signatureCipher.find("&sp") != string::npos) {
                            newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
                        }
                        if (newData.signatureCipher.find("url") != string::npos) {
                            newData.downloadURL = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
                        }
                        else if (value.contains("url") && !value.at("url").is_null()) {
                            newData.downloadURL = value.at("url");
                        }
                        pDataStructure->push_back(move(newData));
                    }
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::CommandData* pDataStructure) {
            if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                for (auto newValue : jsonObjectData.at("options")) {
                    if (newValue.contains("type") && newValue.at("type") == 1) {
                        if (newValue.contains("name")) {
                            pDataStructure->optionsArgs.push_back(newValue.at("name"));
                        }
                    }
                    if (newValue.contains("options")) {
                        parseObject(newValue, pDataStructure);
                    }
                    if (newValue.contains("value") && !newValue.at("value").is_null()) {
                        auto newValueNew = newValue.at("value");
                        if (newValueNew.is_string()) {
                            pDataStructure->optionsArgs.push_back(newValueNew);
                        }
                        else {
                            pDataStructure->optionsArgs.push_back(to_string(newValueNew));
                        }
                    }
                }
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->commandName = jsonObjectData.at("name").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionData* pDataStructure) {

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::ApplicationCommandType>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                for (auto value : jsonObjectData.at("options")) {
                    DiscordCoreAPI::ApplicationCommandInteractionDataOption newData02{};
                    parseObject(value, &newData02);
                    pDataStructure->options.push_back(newData02);
                }
            }

            if (jsonObjectData.contains("resolved") && !jsonObjectData.at("resolved").is_null()) {
                auto newValue = jsonObjectData.at("resolved");
                if (newValue.contains("users") && !newValue.at("users").is_null()) {
                    auto newMap = newValue.at("users").get<map<string, json>>();
                    map<string, DiscordCoreAPI::UserData> newerMap;
                    for (const auto& [key, newValue01] : newMap) {
                        DiscordCoreAPI::UserData newData02{};
                        parseObject(newValue01, &newData02);
                        pDataStructure->resolved.users.insert(make_pair(key, newData02));
                    }
                }
                if (newValue.contains("channels") && !newValue.at("channels").is_null()) {
                    auto newMap = newValue.at("channels").get<map<string, json>>();
                    map<string, DiscordCoreAPI::ChannelData> newerMap;
                    for (const auto& [key, newValue02] : newMap) {
                        DiscordCoreAPI::ChannelData newData02{};
                        parseObject(newValue02, &newData02);
                        pDataStructure->resolved.channels.insert(make_pair(key, newData02));
                    }
                }
                if (newValue.contains("roles") && !newValue.at("roles").is_null()) {
                    auto newMap = newValue.at("roles").get<map<string, json>>();
                    map<string, DiscordCoreAPI::RoleData> newerMap;
                    for (const auto& [key, newValue03] : newMap) {
                        DiscordCoreAPI::RoleData newData02{};
                        parseObject(newValue03, &newData02);
                        pDataStructure->resolved.roles.insert(make_pair(key, newData02));
                    }
                }
                if (newValue.contains("members") && !newValue.at("members").is_null()) {
                    auto newMap = newValue.at("members").get<map<string, json>>();
                    map<string, DiscordCoreAPI::GuildMemberData> newerMap;
                    for (const auto& [key, newValue04] : newMap) {
                        DiscordCoreAPI::GuildMemberData newData02{};
                        parseObject(newValue04, &newData02);
                        pDataStructure->resolved.members.insert(make_pair(key, newData02));
                    }
                }
                if (newValue.contains("messages") && !newValue.at("messages").is_null()) {
                    auto newMap = newValue.at("messages").get<map<string, json>>();
                    map<string, DiscordCoreAPI::MessageData> newerMap;
                    for (const auto& [key, newValue04] : newMap) {
                        DiscordCoreAPI::MessageData newData02{};
                        parseObject(newValue04, &newData02);
                        pDataStructure->resolved.messages.insert(make_pair(key, newData02));
                    }
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageCommandInteractionData* pDataStructure) {

            if (jsonObjectData.contains("target_id") && !jsonObjectData.at("target_id").is_null()) {
                pDataStructure->targetId = jsonObjectData.at("target_id").get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::UserCommandInteractionData* pDataStructure) {

            if (jsonObjectData.contains("target_id") && !jsonObjectData.at("target_id").is_null()) {
                pDataStructure->targetId = jsonObjectData.at("target_id").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ComponentInteractionData* pDataStructure) {

            if (jsonObjectData.contains("values") && !jsonObjectData.at("values").is_null()) {
                for (auto value : jsonObjectData.at("values")) {
                    pDataStructure->values.push_back(value);
                }
            }

            if (jsonObjectData.contains("custom_id") && !jsonObjectData.at("custom_id").is_null()) {
                pDataStructure->customId = jsonObjectData.at("custom_id").get<string>();
            }

            if (jsonObjectData.contains("component_type") && !jsonObjectData.at("component_type").is_null()) {
                pDataStructure->componentType = jsonObjectData.at("component_type").get<DiscordCoreAPI::ComponentType>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionDataData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                parseObject(jsonObjectData, &pDataStructure->applicationCommanddata);
            }

            if (jsonObjectData.contains("target_id") && !jsonObjectData.at("target_id").is_null()) {
                parseObject(jsonObjectData, &pDataStructure->messageInteractionData);
                parseObject(jsonObjectData, &pDataStructure->userInteractionData);
            }

            if (jsonObjectData.contains("component_type") && !jsonObjectData.at("component_type").is_null()) {
                parseObject(jsonObjectData, &pDataStructure->componentData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionData* pDataStructure) {

            if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
                parseObject(jsonObjectData.at("data"), &pDataStructure->data);
                pDataStructure->rawData = jsonObjectData.at("data");
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::InteractionType>();
            }

            if (jsonObjectData.contains("token") && !jsonObjectData.at("token").is_null()) {
                pDataStructure->token = jsonObjectData.at("token").get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
                parseObject(jsonObjectData.at("member"), &pDataStructure->member);
                pDataStructure->user = pDataStructure->member.user;
            }
            else if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("message") && !jsonObjectData.at("message").is_null()) {
                parseObject(jsonObjectData.at("message"), &pDataStructure->message);
            }

            if (jsonObjectData.contains("version") && !jsonObjectData.at("version").is_null()) {
                pDataStructure->version = jsonObjectData.at("version").get<__int32>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("application_id").get<string>();

            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionRemoveData* pDataStructure) {

            if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
                pDataStructure->userId = jsonObjectData.at("user_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
                pDataStructure->messageId = jsonObjectData.at("message_id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
                parseObject(jsonObjectData.at("emoji"), &pDataStructure->emoji);
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::WebhookData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("id").get< __int32>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("avatar") && !jsonObjectData.at("avatar").is_null()) {
                pDataStructure->avatar = jsonObjectData.at("avatar").get<string>();
            }

            if (jsonObjectData.contains("token") && !jsonObjectData.at("token").is_null()) {
                pDataStructure->token = jsonObjectData.at("token").get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("application_id").get<string>();
            }

            if (jsonObjectData.contains("source_guild") && !jsonObjectData.at("source_guild").is_null()) {
                parseObject(jsonObjectData.at("source_guild"), &pDataStructure->sourceGuild);
            }

            if (jsonObjectData.contains("source_channel") && !jsonObjectData.at("source_channel").is_null()) {
                parseObject(jsonObjectData.at("source_channel"), &pDataStructure->sourceChannel);
            }

            if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
                pDataStructure->url = jsonObjectData.at("url").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogChangeData* pDataStructure) {

            if (jsonObjectData.contains("new_value") && !jsonObjectData.at("new_value").is_null()) {
                if (jsonObjectData.at("new_value").is_string()) {
                    pDataStructure->newValueString = jsonObjectData.at("new_value").get<string>();
                }
                else if (jsonObjectData.at("new_value").is_boolean()) {
                    pDataStructure->newValueBool = jsonObjectData.at("new_value").get<bool>();
                }
                else if (jsonObjectData.at("new_value").is_number_integer()) {
                    pDataStructure->newValueInt = jsonObjectData.at("new_value").get<__int32>();
                }
            }

            if (jsonObjectData.contains("old_value") && !jsonObjectData.at("old_value").is_null()) {
                if (jsonObjectData.at("old_value").is_string()) {
                    pDataStructure->oldValueString = jsonObjectData.at("old_value").get<string>();
                }
                else if (jsonObjectData.at("old_value").is_boolean()) {
                    pDataStructure->oldValueBool = jsonObjectData.at("old_value").get<bool>();
                }
                else if (jsonObjectData.at("old_value").is_number_integer()) {
                    pDataStructure->oldValueInt = jsonObjectData.at("old_value").get<__int32>();
                }
            }

            if (jsonObjectData.contains("key") && !jsonObjectData.at("key").is_null()) {
                pDataStructure->key = jsonObjectData.at("key").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryInfoData* pDataStructure) {

            if (jsonObjectData.contains("delete_member_days") && !jsonObjectData.at("delete_member_days").is_null()) {
                pDataStructure->deleteMemberDays = jsonObjectData.at("delete_member_days").get<string>();
            }

            if (jsonObjectData.contains("members_removed") && !jsonObjectData.at("members_removed").is_null()) {
                pDataStructure->membersRemoved = jsonObjectData.at("members_removed").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
                pDataStructure->messageId = jsonObjectData.at("message_id").get<string>();
            }

            if (jsonObjectData.contains("count") && !jsonObjectData.at("count").is_null()) {
                pDataStructure->count = jsonObjectData.at("count").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<string>();
            }

            if (jsonObjectData.contains("role_name") && !jsonObjectData.at("role_name").is_null()) {
                pDataStructure->roleName = jsonObjectData.at("role_name").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryData* pDataStructure) {

            if (jsonObjectData.contains("target_id") && !jsonObjectData.at("target_id").is_null()) {
                pDataStructure->targetId = jsonObjectData.at("target_id").get<string>();
            }

            if (jsonObjectData.contains("changes") && !jsonObjectData.at("changes").is_null()) {
                for (auto newValue : jsonObjectData.at("changes")) {
                    DiscordCoreAPI::AuditLogChangeData changesData;
                    parseObject(newValue, &changesData);
                    pDataStructure->changes.push_back(move(changesData));
                }
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
                pDataStructure->userId = jsonObjectData.at("user_id").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
                pDataStructure->createdTimeStamp = (DiscordCoreAPI::TimeStamp)pDataStructure->getCreatedAtTimestamp();

            }

            if (jsonObjectData.contains("action_type") && !jsonObjectData.at("action_type").is_null()) {
                pDataStructure->actionType = jsonObjectData.at("action_type").get<DiscordCoreAPI::AuditLogEvent>();
            }

            if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                parseObject(jsonObjectData.at("options"), &pDataStructure->options);
            }

            if (jsonObjectData.contains("reason") && !jsonObjectData.at("reason").is_null()) {
                pDataStructure->reason = jsonObjectData.at("reason").get<string>();
            }
            
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AccountData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::IntegrationData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
                pDataStructure->name = jsonObjectData.at("name").get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<string>();
            }

            if (jsonObjectData.contains("enabled") && !jsonObjectData.at("enabled").is_null()) {
                pDataStructure->enabled = jsonObjectData.at("enabled").get<bool>();
            }

            if (jsonObjectData.contains("syncing") && !jsonObjectData.at("syncing").is_null()) {
                pDataStructure->syncing = jsonObjectData.at("syncing").get<bool>();
            }

            if (jsonObjectData.contains("role_id") && !jsonObjectData.at("role_id").is_null()) {
                pDataStructure->roleId = jsonObjectData.at("role_id").get<string>();
            }

            if (jsonObjectData.contains("enable_emoticons") && !jsonObjectData.at("enable_emoticons").is_null()) {
                pDataStructure->enableEmoticons = jsonObjectData.at("enable_emoticons").get<bool>();
            }

            if (jsonObjectData.contains("expire_behavior") && !jsonObjectData.at("expire_behavior").is_null()) {
                pDataStructure->expireBehavior = jsonObjectData.at("expire_behavior").get< __int32>();
            }

            if (jsonObjectData.contains("expire_grace_period") && !jsonObjectData.at("expire_grace_period").is_null()) {
                pDataStructure->expireGracePeriod = jsonObjectData.at("expire_grace_period").get< __int32>();
            }

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

            if (jsonObjectData.contains("account") && !jsonObjectData.at("account").is_null()) {
                parseObject(jsonObjectData.at("account"), &pDataStructure->account);
            }

            if (jsonObjectData.contains("synced_at") && !jsonObjectData.at("synced_at").is_null()) {
                pDataStructure->syncedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("synced_at").get<string>());
            }

            if (jsonObjectData.contains("subscriber_count") && !jsonObjectData.at("subscriber_count").is_null()) {
                pDataStructure->subscriberCount = jsonObjectData.at("subscriber_count").get< __int32>();
            }

            if (jsonObjectData.contains("revoked") && !jsonObjectData.at("revoked").is_null()) {
                pDataStructure->revoked = jsonObjectData.at("revoked").get<bool>();
            }

            if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
                parseObject(jsonObjectData.at("application"), &pDataStructure->application);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogData* pDataStructure) {

            if (jsonObjectData.contains("webhooks") && !jsonObjectData.at("webhooks").is_null()) {
                for (auto newValue : jsonObjectData.at("webhooks")) {
                    DiscordCoreAPI::WebhookData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->webhooks.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("users") && !jsonObjectData.at("users").is_null()) {
                for (auto newValue : jsonObjectData.at("users")) {
                    DiscordCoreAPI::UserData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->users.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData.at("audit_log_entries").is_null()) {
                for (auto newValue : jsonObjectData.at("audit_log_entries")) {
                    DiscordCoreAPI::AuditLogEntryData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->auditLogEntries.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("integrations") && !jsonObjectData.at("integrations").is_null()) {
                for (auto newValue : jsonObjectData.at("integrations")) {
                    DiscordCoreAPI::IntegrationData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->integrations.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("threads") && !jsonObjectData.at("threads").is_null()) {
                for (auto newValue : jsonObjectData.at("threads")) {
                    DiscordCoreAPI::ChannelData newData{};
                    parseObject(newValue, &newData);
                    pDataStructure->threads.push_back(move(newData));
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TypingStartData* pDataStructure) {

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructure->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
                parseObject(jsonObjectData.at("member"), &pDataStructure->member);
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
                pDataStructure->userId = jsonObjectData.at("user_id").get<string>();
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
                pDataStructure->timestamp = jsonObjectData.at("timestamp").get<__int32>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelPinsUpdateEventData* pDataStructre) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructre->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
                pDataStructre->channelId = jsonObjectData.at("channel_id").get<string>();
            }

            if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData.at("last_pin_timestamp").is_null()) {
                pDataStructre->lastPinTimestamp = jsonObjectData.at("last_pin_timestamp").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::BanData* pDataStructure) {

            if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                parseObject(jsonObjectData.at("user"), &pDataStructure->user);
            }

            if (jsonObjectData.contains("reason") && !jsonObjectData.at("reason").is_null()) {
                pDataStructure->reason = jsonObjectData.at("reason").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::BanData>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::BanData newData{};
                parseObject(value, &newData);
                pDataStructure->push_back(move(newData));
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadListSyncData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("channel_ids") && !jsonObjectData.at("channel_ids").is_null()) {
                for (auto value : jsonObjectData.at("channel_ids")) {
                    pDataStructure->channelIds.push_back(value);
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
                for (auto value : jsonObjectData.at("members")) {
                    DiscordCoreAPI::ThreadMemberData newData01;
                    parseObject(value, &newData01);
                    pDataStructure->members.push_back(newData01);
                }
            }

            if (jsonObjectData.contains("threads") && !jsonObjectData.at("threads").is_null()) {
                for (auto value : jsonObjectData.at("threads")) {
                    DiscordCoreAPI::ChannelData newData01;
                    parseObject(value, &newData01);
                    pDataStructure->threads.push_back(newData01);
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMembersUpdateData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
                pDataStructure->memberCount = jsonObjectData.at("member_count").get<__int32>();
            }

            if (jsonObjectData.contains("added_members") && !jsonObjectData.at("added_members").is_null()) {
                for (auto value : jsonObjectData.at("added_members")) {
                    DiscordCoreAPI::ThreadMemberData newData01;
                    parseObject(value, &newData01);
                    pDataStructure->addedMembers.push_back(newData01);
                }
            }

            if (jsonObjectData.contains("removed_member_ids") && !jsonObjectData.at("removed_member_ids").is_null()) {
                for (auto value : jsonObjectData.at("removed_member_ids")) {
                    pDataStructure->removedMemberIds.push_back(value);
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ActiveThreadsData* pDataStructure) {
            if (jsonObjectData.contains("threads") && !jsonObjectData.at("threads").is_null()) {
                for (auto value : jsonObjectData.at("threads")) {
                    DiscordCoreAPI::ChannelData newData{};
                    parseObject(value, &newData);
                    pDataStructure->threads.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
                for (auto value : jsonObjectData.at("members")) {
                    DiscordCoreAPI::ThreadMemberData newData{};
                    parseObject(value, &newData);
                    pDataStructure->members.push_back(move(newData));
                }
            }

            if (jsonObjectData.contains("has_more") && !jsonObjectData.at("has_more").is_null()) {
                pDataStructure->hasMore = jsonObjectData.at("has_more").get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildEmojisUpdateEventData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("emojis") && !jsonObjectData.at("emojis").is_null()) {
                for (auto value : jsonObjectData.at("emojis")) {
                    DiscordCoreAPI::EmojiData newData01{};
                    parseObject(value, &newData01);
                    pDataStructure->emojis.push_back(newData01);
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildStickersUpdateEventData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
                for (auto value : jsonObjectData.at("stickers")) {
                    DiscordCoreAPI::StickerData newData01{};
                    parseObject(value, &newData01);
                    pDataStructure->stickers.push_back(newData01);
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildMembersChunkEventData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("nonce") && !jsonObjectData.at("nonce").is_null()) {
                pDataStructure->nonce = jsonObjectData.at("nonce").get<string>();
            }

            if (jsonObjectData.contains("chunk_index") && !jsonObjectData.at("chunk_index").is_null()) {
                pDataStructure->chunkIndex = jsonObjectData.at("chunk_index").get<__int32>();
            }

            if (jsonObjectData.contains("chunk_count") && !jsonObjectData.at("chunk_count").is_null()) {
                pDataStructure->chunkCount = jsonObjectData.at("chunk_count").get<__int32>();
            }

            if (jsonObjectData.contains("presences") && !jsonObjectData.at("presences").is_null()) {
                for (auto value : jsonObjectData.at("presences")) {
                    DiscordCoreAPI::PresenceUpdateData newData01{};
                    parseObject(value, &newData01);
                    pDataStructure->presences.push_back(newData01);
                }
            }

            if (jsonObjectData.contains("not_found") && !jsonObjectData.at("not_found").is_null()) {
                for (auto value : jsonObjectData.at("not_found")) {
                    pDataStructure->notFound.push_back(value);
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
                for (auto value : jsonObjectData.at("members")) {
                    DiscordCoreAPI::GuildMemberData newData01{};
                    parseObject(value, &newData01);
                    pDataStructure->members.push_back(newData01);
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandPermissionData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("permission") && !jsonObjectData.at("permission").is_null()) {
                pDataStructure->permission = jsonObjectData.at("permission").get<bool>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
                pDataStructure->type = jsonObjectData.at("type").get<DiscordCoreAPI::ApplicationCommandPermissionType>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildApplicationCommandPermissionsData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
                pDataStructure->guildId = jsonObjectData.at("guild_id").get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
                pDataStructure->applicationId = jsonObjectData.at("application_id").get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
                pDataStructure->id = jsonObjectData.at("id").get<string>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
                for (auto value : jsonObjectData.at("permissions")) {
                    DiscordCoreAPI::ApplicationCommandPermissionData newData02{};
                    DataParser::parseObject(value, &newData02);
                    pDataStructure->permissions.push_back(newData02);
                }
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData>* pDataStructure) {
            for (auto value : jsonObjectData) {
                DiscordCoreAPI::GuildApplicationCommandPermissionsData newInvite{};
                parseObject(value, &newInvite);
                pDataStructure->push_back(newInvite);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::YouTubeSong* pDataStructure) {

            if (jsonObjectData.contains("lengthText") && !jsonObjectData.at("lengthText").is_null()) {
                pDataStructure->duration = jsonObjectData.at("lengthText").at("accessibility").at("accessibilityData").at("label").get<string>();
            }

            if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData.at("detailedMetadataSnippets").is_null()) {
                for (auto value : jsonObjectData.at("detailedMetadataSnippets").at(0).at("snippetText").at("runs")) {
                    pDataStructure->description += value.at("text").get<string>();
                }
            }

            if (jsonObjectData.contains("thumbnail") && !jsonObjectData.at("thumbnail").is_null()) {
                pDataStructure->thumbnailURL = jsonObjectData.at("thumbnail").at("thumbnails").at(0).at("url").get<string>();
            }

            if (jsonObjectData.contains("videoId") && !jsonObjectData.at("videoId").is_null()) {
                pDataStructure->songId = "https://www.youtube.com/watch?v=" + jsonObjectData.at("videoId").get<string>();
            }

            if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null()) {
                if (jsonObjectData.at("title").contains("runs")) {
                    pDataStructure->songTitle = jsonObjectData.at("title").at("runs").at(0).at("text").get<string>();
                }
                else if (jsonObjectData.at("title").contains("simpleText")) {
                    pDataStructure->songTitle = jsonObjectData.at("title").at("simpleText").get<string>();
                }
            }

            if (jsonObjectData.contains("videoId") && !jsonObjectData.at("videoId").is_null()) {
                pDataStructure->songId = jsonObjectData.at("videoId").get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::SoundCloudSong* pDataStructure) {

            if (jsonObjectData.contains("track_authorization") && !jsonObjectData.at("track_authorization").is_null()) {
                pDataStructure->trackAuthorization = jsonObjectData.at("track_authorization").get<string>();
            }

            if (jsonObjectData.contains("media") && !jsonObjectData.at("media").is_null()) {
                bool isItFound{ false };
                for (auto value : jsonObjectData.at("media").at("transcodings")) {
                    if (value.at("preset") == "opus_0_0") {
                        isItFound = true;
                        pDataStructure->firstDownloadURL = to_string(to_hstring(value.at("url").get<string>()));
                        pDataStructure->songId = to_string(to_hstring(value.at("url").get<string>()));
                    }
                }
                bool isItFound2{ false };
                if (!isItFound) {
                    for (auto value : jsonObjectData.at("media").at("transcodings")) {
                        if (value.at("preset") == "mp3_0_0") {
                            pDataStructure->firstDownloadURL = to_string(to_hstring(value.at("url").get<string>()));
                            pDataStructure->songId = to_string(to_hstring(value.at("url").get<string>()));
                            isItFound2 = true;
                        }

                    }
                }

                if (!isItFound2 && !isItFound) {
                    for (auto value : jsonObjectData.at("media").at("transcodings")) {
                        pDataStructure->firstDownloadURL = to_string(to_hstring(value.at("url").get<string>()));
                        pDataStructure->songId = to_string(to_hstring(value.at("url").get<string>()));
                    }
                }
            }

            if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null() && !jsonObjectData.at("title").is_object()) {
                pDataStructure->songTitle = to_string(to_hstring(jsonObjectData.at("title").get<string>()));
            }
            if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
                string newString = to_string(to_hstring(jsonObjectData.at("description").get<string>()));
                if (newString.size() > 100) {
                    newString = newString.substr(0, 100);
                }
                char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
                char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);
                for (__int32 x = 0; x < newString.size(); x += 1) {
                    pDataStructure->description.push_back(newString02[x]);
                }
                pDataStructure->description += "...";
            }

            if (jsonObjectData.contains("artwork_url") && !jsonObjectData.at("artwork_url").is_null()) {
                pDataStructure->thumbnailURL = to_string(to_hstring(jsonObjectData.at("artwork_url").get<string>()));
            }
            else if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
                if (jsonObjectData.at("user").contains("avatar_url") && !jsonObjectData.at("user").at("avatar_url").is_null()) {
                    pDataStructure->thumbnailURL = jsonObjectData.at("user").at("avatar_url").get<string>();
                }
            }

            if (jsonObjectData.contains("duration") && !jsonObjectData.at("duration").is_null()) {
                __int32 durationNew = jsonObjectData.at("duration").get<__int32>();
                pDataStructure->duration = DiscordCoreAPI::convertMsToDurationString(durationNew);
            }

            if (jsonObjectData.contains("permalink_url") && !jsonObjectData.at("permalink_url").is_null()) {
                pDataStructure->viewURL = to_string(to_hstring(jsonObjectData.at("permalink_url").get<string>()));
            }

        }

    };
};
