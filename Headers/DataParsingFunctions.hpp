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

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = (DiscordCoreAPI::PermissionOverwritesType)jsonObjectData["type"].get<int32_t>();
            }

            if (jsonObjectData.contains("allow") && !jsonObjectData["allow"].is_null()) {
                pDataStructure->allow = jsonObjectData["allow"].get<string>();
            }

            if (jsonObjectData.contains("deny") && !jsonObjectData["deny"].is_null()) {
                pDataStructure->deny = jsonObjectData["deny"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::UserData* pDataStructure) {

            if (jsonObjectData.contains("username") && !jsonObjectData["username"].is_null()) {
                pDataStructure->userName = jsonObjectData["username"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
                pDataStructure->createdAt = pDataStructure->getCreatedAtTimestamp();
            }

            if (jsonObjectData.contains("discriminator") && !jsonObjectData["discriminator"].is_null()) {
                pDataStructure->discriminator = jsonObjectData["discriminator"].get<string>();
            }

            if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
                string avatarString = "https://cdn.discordapp.com/avatars/" + pDataStructure->id + "/" + jsonObjectData["avatar"].get<string>();
                pDataStructure->avatar = avatarString;
            }

            if (jsonObjectData.contains("bot") && !jsonObjectData["bot"].is_null()) {
                pDataStructure->bot = jsonObjectData["bot"].get<bool>();
            }

            if (jsonObjectData.contains("system") && !jsonObjectData["system"].is_null()) {
                pDataStructure->system = jsonObjectData["system"].get<bool>();
            }

            if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData["mfa_enabled"].is_null()) {
                pDataStructure->mfaEnabled = jsonObjectData["mfa_enabled"].get<bool>();
            }

            if (jsonObjectData.contains("locale") && !jsonObjectData["locale"].is_null()) {
                pDataStructure->locale = jsonObjectData["locale"].get<string>();
            }

            if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
                pDataStructure->verified = jsonObjectData["verified"].get<bool>();
            }

            if (jsonObjectData.contains("email") && !jsonObjectData["email"].is_null()) {
                pDataStructure->email = jsonObjectData["email"].get<string>();
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
                pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
            }

            if (jsonObjectData.contains("premium_type") && !jsonObjectData["premium_type"].is_null()) {
                pDataStructure->premiumType = jsonObjectData["premium_type"].get<int32_t>();
            }

            if (jsonObjectData.contains("public_flags") && !jsonObjectData["public_flags"].is_null()) {
                pDataStructure->publicFlags = jsonObjectData["public_flags"].get<int32_t>();
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::UserData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::UserData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildWidgetImageData* pDataStructure) {

            if (jsonObjectData.contains("widget_image") && !jsonObjectData["widget_image"].is_null()) {
                pDataStructure->url = jsonObjectData["widget_image"].get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMetadataData* pDataStructure) {

            if (jsonObjectData.contains("archived") && !jsonObjectData["archived"].is_null()) {
                pDataStructure->archived = jsonObjectData["archived"].get<bool>();
            }

            if (jsonObjectData.contains("invitable") && !jsonObjectData["invitable"].is_null()) {
                pDataStructure->invitable = jsonObjectData["invitable"].get<bool>();
            }

            if (jsonObjectData.contains("auto_archive_duration") && !jsonObjectData["auto_archive_duration"].is_null()) {
                pDataStructure->autoArchiveDuration = jsonObjectData["auto_archive_duration"].get<int32_t>();
            }

            if (jsonObjectData.contains("archive_timestamp") && !jsonObjectData["archive_timestamp"].is_null()) {
                pDataStructure->archiveTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["archive_timestamp"].get<string>());
            }

            if (jsonObjectData.contains("locked") && !jsonObjectData["locked"].is_null()) {
                pDataStructure->locked = jsonObjectData["locked"].get<bool>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMemberData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
                pDataStructure->userId = jsonObjectData["user_id"].get<string>();
            }

            if (jsonObjectData.contains("join_timestamp") && !jsonObjectData["join_timestamp"].is_null()) {
                pDataStructure->joinTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["join_timestamp"].get<string>());
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
                pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::ThreadMemberData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::ThreadMemberData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = (DiscordCoreAPI::ChannelType)jsonObjectData["type"].get<int32_t>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
                pDataStructure->position = jsonObjectData["position"].get<int32_t>();
            }

            if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
                map<string, DiscordCoreAPI::OverWriteData> newMap{};
                for (auto& value : jsonObjectData["permission_overwrites"]) {
                    DiscordCoreAPI::OverWriteData newData{};
                    if (pDataStructure->permissionOverwrites.contains(value["id"])) {
                        newData = pDataStructure->permissionOverwrites.at(value["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.id, move(newData));
                }
                pDataStructure->permissionOverwrites = newMap;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
                pDataStructure->topic = jsonObjectData["topic"].get<string>();
            }

            if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
                pDataStructure->nsfw = jsonObjectData["nsfw"].get<bool>();
            }

            if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
                pDataStructure->lastMessageId = jsonObjectData["last_message_id"].get<string>();
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
                map<string, DiscordCoreAPI::UserData> newMap{};
                for (auto& value : jsonObjectData["recipients"]) {
                    DiscordCoreAPI::UserData newData{};
                    if (pDataStructure->recipients.contains(value["id"])) {
                        newData = pDataStructure->recipients.at(value["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.id, move(newData));
                }
                pDataStructure->recipients = newMap;
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
                pDataStructure->icon = jsonObjectData["icon"].get<string>();
            }

            if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
                pDataStructure->ownerId = jsonObjectData["owner_id"].get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["application_id"].get<string>();
            }

            if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
                pDataStructure->parentId = jsonObjectData["parent_id"].get<string>();
            }

            if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
                pDataStructure->lastPinTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["last_pin_timestamp"].get<string>());
            }

            if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
                pDataStructure->rtcRegion = jsonObjectData["rtc_region"].get<string>();
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
                parseObject(jsonObjectData["thread_metadata"], &pDataStructure->threadMetadata);
            }

            if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
                parseObject(jsonObjectData["member"], &pDataStructure->member);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::ChannelData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x+=1 ) {
                DiscordCoreAPI::ChannelData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::RoleTagsData* pDataStructure) {

            if (jsonObjectData.contains("bot_id") && !jsonObjectData["bot_id"].is_null()) {
                pDataStructure->botId = jsonObjectData["bot_id"].get<string>();
            }

            if (jsonObjectData.contains("integration_id") && !jsonObjectData["integration_id"].is_null()) {
                pDataStructure->integrationId = jsonObjectData["integration_id"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::RoleData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
                pDataStructure->color = jsonObjectData["color"].get<int32_t>();
            }

            if (jsonObjectData.contains("hoist") && !jsonObjectData["hoist"].is_null()) {
                pDataStructure->hoist = jsonObjectData["hoist"].get<bool>();
            }

            if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
                pDataStructure->position = jsonObjectData["position"].get<int32_t>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
                pDataStructure->permissions = jsonObjectData["permissions"].get<string>();
            }

            if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
                pDataStructure->managed = jsonObjectData["managed"].get<bool>();
            }

            if (jsonObjectData.contains("mentionable") && !jsonObjectData["mentionable"].is_null()) {
                pDataStructure->mentionable = jsonObjectData["mentionable"].get<bool>();
            }

            if (jsonObjectData.contains("tags") && !jsonObjectData["tags"].is_null()) {
                parseObject(jsonObjectData["tags"], &pDataStructure->tags);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::RoleData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::RoleData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildMemberData* pDataStructure) {

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

            if (jsonObjectData.contains("nick") && !jsonObjectData["nick"].is_null()) {
                pDataStructure->nick = jsonObjectData["nick"].get<string>();
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

            if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
                pDataStructure->roles.resize(jsonObjectData["roles"].size());
                for (uint32_t x = 0; x < jsonObjectData["roles"].size(); x += 1) {
                    pDataStructure->roles[x] = jsonObjectData["roles"][x];
                }
            }

            if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
                pDataStructure->joinedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["joined_at"].get<string>());
            }

            if (jsonObjectData.contains("premium_since") && !jsonObjectData["premium_since"].is_null()) {
                pDataStructure->premiumSince = jsonObjectData["premium_since"].get<string>();
            }

            if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
                pDataStructure->deaf = jsonObjectData["deaf"].get<bool>();
            }

            if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
                pDataStructure->mute = jsonObjectData["mute"].get<bool>();
            }

            if (jsonObjectData.contains("pending") && !jsonObjectData["pending"].is_null()) {
                pDataStructure->pending = jsonObjectData["pending"].get<bool>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
                pDataStructure->permissions = jsonObjectData["permissions"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::GuildMemberData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::GuildMemberData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmojiData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
                pDataStructure->roles.resize(jsonObjectData["roles"].size());
                for (uint32_t x = 0; x < jsonObjectData["roles"].size(); x += 1) {
                    DiscordCoreAPI::RoleData newData{};
                    parseObject(jsonObjectData["roles"][x], &newData);
                    pDataStructure->roles[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

            if (jsonObjectData.contains("require_colons") && !jsonObjectData["require_colons"].is_null()) {
                pDataStructure->requireColons = jsonObjectData["require_colons"].get<bool>();
            }

            if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
                pDataStructure->managed = jsonObjectData["managed"].get<bool>();
            }

            if (jsonObjectData.contains("animated") && !jsonObjectData["animated"].is_null()) {
                pDataStructure->animated = jsonObjectData["animated"].get<bool>();
            }

            if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
                pDataStructure->available = jsonObjectData["available"].get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::EmojiData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::EmojiData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildPreviewData* pDataStructure) {

            if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
                pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
            }

            if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
                pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
            }

            if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
                pDataStructure->discoverySplash = jsonObjectData["discovery_splash"].get<string>();
            }

            if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
                pDataStructure->emojis.resize(jsonObjectData["emojis"].size());
                for (uint32_t x = 0; x < jsonObjectData["emojis"].size(); x += 1) {
                    DiscordCoreAPI::EmojiData newData{};
                    parseObject(jsonObjectData["emojis"][x], &newData);
                    pDataStructure->emojis[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
                pDataStructure->features.resize(jsonObjectData["features"].size());
                for (uint32_t x = 0; x < jsonObjectData["features"].size(); x += 1) {
                    pDataStructure->features[x] = jsonObjectData["features"];
                }
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
                pDataStructure->splash = jsonObjectData["splash"].get<string>();
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
                pDataStructure->icon = jsonObjectData["icon"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::VoiceStateData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
                pDataStructure->userId = jsonObjectData["user_id"].get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
                parseObject(jsonObjectData["member"], &pDataStructure->member);
            }

            if (jsonObjectData.contains("session_id") && !jsonObjectData["session_id"].is_null()) {
                pDataStructure->sessionId = jsonObjectData["session_id"].get<string>();
            }

            if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
                pDataStructure->deaf = jsonObjectData["deaf"].get<bool>();
            }

            if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
                pDataStructure->mute = jsonObjectData["mute"].get<bool>();
            }

            if (jsonObjectData.contains("self_deaf") && !jsonObjectData["self_deaf"].is_null()) {
                pDataStructure->selfDeaf = jsonObjectData["self_deaf"].get<bool>();
            }

            if (jsonObjectData.contains("self_mute") && !jsonObjectData["self_mute"].is_null()) {
                pDataStructure->selfMute = jsonObjectData["self_mute"].get<bool>();
            }

            if (jsonObjectData.contains("self_stream") && !jsonObjectData["self_stream"].is_null()) {
                pDataStructure->selfStream = jsonObjectData["self_stream"].get<bool>();
            }

            if (jsonObjectData.contains("self_video") && !jsonObjectData["self_video"].is_null()) {
                pDataStructure->selfVideo = jsonObjectData["self_video"].get<bool>();
            }

            if (jsonObjectData.contains("suppress") && !jsonObjectData["suppress"].is_null()) {
                pDataStructure->suppress = jsonObjectData["suppress"].get<bool>();
            }

            if (jsonObjectData.contains("request_to_speak_timestamp") && !jsonObjectData["request_to_speak_timestamp"].is_null()) {
                pDataStructure->requestToSpeakTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["request_to_speak_timestamp"].get<string>());
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::PartyData* pDataStructure) {

            if (jsonObjectData.contains("Id") && !jsonObjectData["Id"].is_null()) {
                pDataStructure->id = jsonObjectData["Id"].get<string>();
            }

            if (jsonObjectData.contains("Size") && !jsonObjectData["Size"].is_null()) {
                pDataStructure->size[0] = jsonObjectData["Size"]["CurrentSize"].get<int32_t>();
                pDataStructure->size[1] = jsonObjectData["Size"]["MaxSize"].get<int32_t>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AssetsData* pDataStructure) {

            if (jsonObjectData.contains("LargeImage") && !jsonObjectData["LargeImage"].is_null()) {
                pDataStructure->largeImage = jsonObjectData["LargeImage"].get<string>();
            }

            if (jsonObjectData.contains("LargeText") && !jsonObjectData["LargeText"].is_null()) {
                pDataStructure->largeText = jsonObjectData["LargeText"].get<string>();
            }

            if (jsonObjectData.contains("SmallImage") && !jsonObjectData["SmallImage"].is_null()) {
                pDataStructure->smallImage = jsonObjectData["SmallImage"].get<string>();
            }

            if (jsonObjectData.contains("SmallText") && !jsonObjectData["SmallText"].is_null()) {
                pDataStructure->smallText = jsonObjectData["SmallText"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::SecretsData* pDataStructure) {

            if (jsonObjectData.contains("Join") && !jsonObjectData["Join"].is_null()) {
                pDataStructure->join = jsonObjectData["Join"].get<string>();
            }

            if (jsonObjectData.contains("Spectate") && !jsonObjectData["Spectate"].is_null()) {
                pDataStructure->spectate = jsonObjectData["Spectate"].get<string>();
            }

            if (jsonObjectData.contains("Match") && !jsonObjectData["Match"].is_null()) {
                pDataStructure->match = jsonObjectData["Match"].get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ClientStatusData* pDataStructure) {

            if (jsonObjectData.contains("desktop") && !jsonObjectData["desktop"].is_null()) {
                pDataStructure->desktop = jsonObjectData["desktop"].get<string>();
            }

            if (jsonObjectData.contains("mobile") && !jsonObjectData["mobile"].is_null()) {
                pDataStructure->mobile = jsonObjectData["mobile"].get<string>();
            }

            if (jsonObjectData.contains("web") && !jsonObjectData["web"].is_null()) {
                pDataStructure->web = jsonObjectData["web"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TimestampData* pDataStructure) {

            if (jsonObjectData.contains("Start") && !jsonObjectData["Start"].is_null()) {
                pDataStructure->start = jsonObjectData["Start"].get<int64_t>();
            }

            if (jsonObjectData.contains("End") && !jsonObjectData["End"].is_null()) {
                pDataStructure->end = jsonObjectData["End"].get<int64_t>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ActivityData* pDataStructure) {

            if (jsonObjectData.contains("Name") && !jsonObjectData["Name"].is_null()) {
                pDataStructure->name = jsonObjectData["Name"].get<string>();
            }

            if (jsonObjectData.contains("Timestamps") && !jsonObjectData["Timestamps"].is_null()) {
                parseObject(jsonObjectData["Timestamps"], &pDataStructure->timestamps);
            }

            if (jsonObjectData.contains("ApplicationId") && !jsonObjectData["ApplicationId"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["ApplicationId"].get<string>();
            }

            if (jsonObjectData.contains("Details") && !jsonObjectData["Details"].is_null()) {
                pDataStructure->details = jsonObjectData["Details"].get<string>();
            }

            if (jsonObjectData.contains("State") && !jsonObjectData["State"].is_null()) {
                pDataStructure->state = jsonObjectData["State"].get<string>();
            }

            if (jsonObjectData.contains("ActivityParty") && !jsonObjectData["ActivityParty"].is_null()) {
                parseObject(jsonObjectData["ActivityParty"], &pDataStructure->party);
            }

            if (jsonObjectData.contains("ActivityAssets") && !jsonObjectData["ActivityAssets"].is_null()) {
                parseObject(jsonObjectData["ActivityAssets"], &pDataStructure->assets);
            }

            if (jsonObjectData.contains("ActivitySecrets") && !jsonObjectData["ActivitySecrets"].is_null()) {
                parseObject(jsonObjectData["ActivitySecrets"], &pDataStructure->secrets);
            }

            if (jsonObjectData.contains("Instance") && !jsonObjectData["Instance"].is_null()) {
                pDataStructure->instance = jsonObjectData["Instance"].get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::PresenceUpdateData* pDataStructure) {

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
                pDataStructure->status = jsonObjectData["status"].get<string>();
            }

            if (jsonObjectData.contains("activities") && !jsonObjectData["activities"].is_null()) {
                pDataStructure->activities.resize(jsonObjectData["activities"].size());
                for (uint32_t x = 0; x < jsonObjectData["activities"].size(); x += 1) {
                    DiscordCoreAPI::ActivityData newData{};
                    parseObject(jsonObjectData["activities"][x], &newData);
                    pDataStructure->activities[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("client_status") && !jsonObjectData["client_status"].is_null()) {
                parseObject(jsonObjectData["client_status"], &pDataStructure->clientStatus);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenChannelData* pDataStructure) {

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("emoji_id") && !jsonObjectData["emoji_id"].is_null()) {
                pDataStructure->emojiId = jsonObjectData["emoji_id"].get<string>();
            }

            if (jsonObjectData.contains("emoji_name") && !jsonObjectData["emoji_name"].is_null()) {
                pDataStructure->emojiName = jsonObjectData["emoji_name"].get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenData* pDataStructure) {

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("welcome_channels") && !jsonObjectData["welcome_channels"].is_null()) {
                pDataStructure->welcomeChannels.resize(jsonObjectData["welcome_channels"].size());
                for (uint32_t x = 0; x < jsonObjectData["welcome_channels"].size(); x += 1) {
                    DiscordCoreAPI::WelcomeScreenChannelData newData{};
                    parseObject(jsonObjectData["welcome_channels"][x], &newData);
                    pDataStructure->welcomeChannels[x] = move(newData);
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::StageInstanceData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
                pDataStructure->topic = jsonObjectData["topic"].get<string>();
            }

            if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
                pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<int32_t>();
            }

            if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData["discoverable_disabled"].is_null()) {
                pDataStructure->discoverableDisabled = jsonObjectData["discoverable_disabled"].get<bool>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::StickerData* pDataStructure) {

            if (jsonObjectData.contains("asset") && !jsonObjectData["asset"].is_null()) {
                pDataStructure->asset = jsonObjectData["asset"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
                pDataStructure->formatType = jsonObjectData["format_type"].get<DiscordCoreAPI::StickerFormatType>();
            }

            if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
                pDataStructure->available = jsonObjectData["available"].get<bool>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("pack_id") && !jsonObjectData["pack_id"].is_null()) {
                pDataStructure->packId = jsonObjectData["pack_id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::StickerType>();
            }

            if (jsonObjectData.contains("sort_value") && !jsonObjectData["sort_value"].is_null()) {
                pDataStructure->sortValue = jsonObjectData["sort_value"].get<int32_t>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::VoiceRegionData* pDataStructure) {
            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("custom") && !jsonObjectData["custom"].is_null()) {
                pDataStructure->custom = jsonObjectData["custom"].get<bool>();
            }

            if (jsonObjectData.contains("deprecated") && !jsonObjectData["deprecated"].is_null()) {
                pDataStructure->deprecated= jsonObjectData["deprecated"].get<bool>();
            }

            if (jsonObjectData.contains("optimal") && !jsonObjectData["optimal"].is_null()) {
                pDataStructure->optimal = jsonObjectData["optimal"].get<bool>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::VoiceRegionData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x+=1 ) {
                DiscordCoreAPI::VoiceRegionData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildPruneCountData* pDataStructure) {

            if (jsonObjectData.contains("pruned") && !jsonObjectData["pruned"].is_null()) {
                pDataStructure->count = jsonObjectData["pruned"].get<int32_t>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildData* pDataStructure) {
            
            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
                pDataStructure->afkChannelId = jsonObjectData["afk_channel_id"].get<string>();
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
                string iconURLString = "https://cdn.discordapp.com/";
                iconURLString += "icons/" + pDataStructure->id + "/" + jsonObjectData["icon"].get<string>() + ".png";
                pDataStructure->icon = iconURLString;
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("icon_hash") && !jsonObjectData["icon_hash"].is_null()) {
                pDataStructure->iconHash = jsonObjectData["icon_hash"].get<string>();
            }

            if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
                string iconURLString = "https://cdn.discordapp.com/";
                iconURLString += "splashes/" + pDataStructure->id + "/" + jsonObjectData["splash"].get<string>() + ".png";
                pDataStructure->splash = iconURLString;
            }

            if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
                string discordSplashURLString = "https://cdn.discordapp.com/";
                discordSplashURLString += "discovery-splashes/" + pDataStructure->id + "/" + jsonObjectData["discovery_splash"].get<string>() + ".png";
                pDataStructure->discoverySplash = discordSplashURLString;
            }

            if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
                pDataStructure->owner = jsonObjectData["owner"].get<bool>();
            }

            if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
                pDataStructure->ownerId = jsonObjectData["owner_id"].get<string>();
            }

            if (jsonObjectData.contains("preferred_locale") && !jsonObjectData["preferred_locale"].is_null()) {
                pDataStructure->preferredLocale = jsonObjectData["preferred_locale"].get<string>();
            }

            if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData["public_updates_channel_id"].is_null()) {
                pDataStructure->publicUpdatesChannelId = jsonObjectData["public_updates_channel_id"].get<string>();
            }

            if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData["vanity_url_code"].is_null()) {
                pDataStructure->vanityURLCode = jsonObjectData["vanity_url_code"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("banner") && !jsonObjectData["banner"].is_null()) {
                string guildBannerURL = "https://cdn.discordapp.com/";
                guildBannerURL += "banners/" + pDataStructure->id + "/" + jsonObjectData["banner"].get<string>() + ".png";
                pDataStructure->banner = guildBannerURL;
            }

            if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData["rule_Channel_id"].is_null()) {
                pDataStructure->rulesChannelId = jsonObjectData["rule_Channel_id"].get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["application_id"].get<string>();
            }

            if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
                pDataStructure->joinedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["joined_at"].get<string>());
            }

            if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData["widget_channel_id"].is_null()) {
                pDataStructure->widgetChannelId = jsonObjectData["widget_channel_id"].get<string>();
            }

            if (jsonObjectData.contains("system_channel_id") && !jsonObjectData["system_channel_id"].is_null()) {
                pDataStructure->systemChannelId = jsonObjectData["system_channel_id"].get<string>();
            }

            if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
                pDataStructure->region = jsonObjectData["region"].get<string>();
            }

            if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
                pDataStructure->afkChannelId = jsonObjectData["afk_channel_id"].get<string>();
            }

            if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
                pDataStructure->region = jsonObjectData["region"].get<string>();
            }

            if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
                pDataStructure->features.resize(jsonObjectData["features"].size());
                for (uint32_t x = 0; x < jsonObjectData["features"].size(); x += 1) {
                    pDataStructure->features[x] = jsonObjectData["features"][x];
                }
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
                pDataStructure->permissions = jsonObjectData["permissions"].get<string>();
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
                map<string, DiscordCoreAPI::StickerData> newMap{};
                for (auto& value : jsonObjectData["stickers"]) {
                    DiscordCoreAPI::StickerData newData{};
                    if (pDataStructure->stickers.contains(value["id"])) {
                        newData = pDataStructure->stickers.at(value["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.id, move(newData));
                }
                pDataStructure->stickers = newMap;
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
                map<string, DiscordCoreAPI::RoleData> newMap{};
                for (auto& value: jsonObjectData["roles"]) {
                    DiscordCoreAPI::RoleData newData{};
                    if (pDataStructure->roles.contains(value["id"])) {
                        newData = pDataStructure->roles.at(value["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.id, move(newData));
                }
                pDataStructure->roles = newMap;
            }

            if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
                pDataStructure->owner = jsonObjectData["owner"].get<bool>();
            }

            if (jsonObjectData.contains("afk_timeout") && !jsonObjectData["afk_timeout"].is_null()) {
                pDataStructure->afkTimeOut = jsonObjectData["afk_timeout"].get<DiscordCoreAPI::AfkTimeOutDurations>();
            }

            if (jsonObjectData.contains("widget_enabled") && !jsonObjectData["widget_enabled"].is_null()) {
                pDataStructure->widgetEnabled = jsonObjectData["widget_enabled"].get<bool>();
            }

            if (jsonObjectData.contains("verification_level") && !jsonObjectData["verification_level"].is_null()) {
                pDataStructure->verificationLevel = jsonObjectData["verification_level"].get<DiscordCoreAPI::VerificationLevel>();
            }

            if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData["default_message_notification_level"].is_null()) {
                pDataStructure->defaultMessageNotifications = jsonObjectData["default_message_notification_level"].get<DiscordCoreAPI::DefaultMessageNotificationLevel >();
            }

            if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData["explicit_content_filter_level"].is_null()) {
                pDataStructure->explicitContentFilter = jsonObjectData["explicit_content_filter_level"].get< DiscordCoreAPI::ExplicitContentFilterLevel>();
            }

            if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
                map<string, DiscordCoreAPI::EmojiData> newMap{};
                for (auto& value : jsonObjectData["emojis"]) {
                    DiscordCoreAPI::EmojiData newData{};
                    if (pDataStructure->emoji.contains(value["name"])) {
                        newData = pDataStructure->emoji.at(value["name"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.name, move(newData));
                }
                pDataStructure->emoji = newMap;
            }

            if (jsonObjectData.contains("mfa_level") && !jsonObjectData["mfa_level"].is_null()) {
                pDataStructure->mfaLevel = jsonObjectData["mfa_level"].get< DiscordCoreAPI::MFALevel>();
            }

            if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData["system_channel_flags"].is_null()) {
                pDataStructure->systemChannelFlags = jsonObjectData["system_channel_flags"].get<int32_t>();
            }

            if (jsonObjectData.contains("large") && !jsonObjectData["large"].is_null()) {
                pDataStructure->large = jsonObjectData["large"].get<bool>();
            }

            if (jsonObjectData.contains("unavailable") && !jsonObjectData["unavailable"].is_null()) {
                pDataStructure->unavailable = jsonObjectData["unavailable"].get<bool>();
            }

            if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
                pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
            }

            if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
                map<string, DiscordCoreAPI::VoiceStateData> newMap{};
                for (auto& value : jsonObjectData["voice_states"]) {
                    DiscordCoreAPI::VoiceStateData newData{};
                    if (pDataStructure->voiceStates.contains(value["user_id"])) {
                        newData = pDataStructure->voiceStates.at(value["user_id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.userId, move(newData));
                }
                pDataStructure->voiceStates = newMap;
            }

            if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
                map<string, DiscordCoreAPI::GuildMemberData> newMap{};
                for (auto& value : jsonObjectData["members"]) {
                    DiscordCoreAPI::GuildMemberData newData{};
                    if (pDataStructure->members.contains(value["user"]["id"])) {
                        newData = pDataStructure->members.at(value["user"]["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.user.id, move(newData));
                }
                pDataStructure->members = newMap;
            }

            if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
                map<string, DiscordCoreAPI::ChannelData> newMap{};
                for (auto& value : jsonObjectData["channels"]) {
                    DiscordCoreAPI::ChannelData newData{};
                    if (pDataStructure->channels.contains(value["id"])) {
                        newData = pDataStructure->channels.at(value["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.id, move(newData));
                }
                pDataStructure->channels = newMap;
            }

            if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
                map<string, DiscordCoreAPI::ChannelData> newMap{};
                for (auto& value : jsonObjectData["threads"]) {
                    DiscordCoreAPI::ChannelData newData{};
                    if (pDataStructure->threads.contains(value["id"])) {
                        newData = pDataStructure->threads.at(value["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.id, move(newData));
                }
                pDataStructure->threads = newMap;
            }

            if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
                map<string, DiscordCoreAPI::PresenceUpdateData> newMap{};
                for (auto& value : jsonObjectData["presences"]) {
                    DiscordCoreAPI::PresenceUpdateData newData{};
                    if (pDataStructure->presences.contains(value["user"]["id"])) {
                        newData = pDataStructure->presences.at(value["user"]["id"]);
                    }
                    parseObject(value, &newData);
                    newMap.insert_or_assign(newData.user.id, move(newData));
                }
                pDataStructure->presences = newMap;
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
                pDataStructure->premiumTier = jsonObjectData["premium_tier"].get< DiscordCoreAPI::PremiumTier>();
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
                parseObject(jsonObjectData["welcome_screen"], &pDataStructure->welcomeScreen);
            }

            if (jsonObjectData.contains("nsfw_level") && !jsonObjectData["nsfw_level"].is_null()) {
                pDataStructure->nsfwLevel = jsonObjectData["nsfw_level"].get<int32_t>();
            }

            if (jsonObjectData.contains("stage_instances") && !jsonObjectData["stage_instances"].is_null()) {
                pDataStructure->stageInstances.resize(jsonObjectData["stage_instances"].size());
                for (uint32_t x = 0; x < jsonObjectData["stage_instances"].size(); x += 1) {
                    DiscordCoreAPI::StageInstanceData newData{};
                    parseObject(jsonObjectData["stage_instances"][x], &newData);
                    pDataStructure->stageInstances[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->createdAt = pDataStructure->getCreatedAtTimestamp();
            }

            cout << "TIME UNTIL PARSED: " << pDataStructure->stopWatch.totalTimePassed() << endl;
        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildWidgetData* pDataStructure) {

            if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
                pDataStructure->enabled = jsonObjectData["enabled"].get<bool>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }
        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelMentionData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::ChannelType>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }
        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AttachmentData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("filename") && !jsonObjectData["filename"].is_null()) {
                pDataStructure->filename = jsonObjectData["filename"].get<string>();
            }

            if (jsonObjectData.contains("content_type") && !jsonObjectData["content_type"].is_null()) {
                pDataStructure->contentType = jsonObjectData["content_type"].get<string>();
            }

            if (jsonObjectData.contains("size") && !jsonObjectData["size"].is_null()) {
                pDataStructure->size = jsonObjectData["size"].get<int32_t>();
            }

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
                pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
                pDataStructure->width = jsonObjectData["width"].get<int32_t>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
                pDataStructure->height = jsonObjectData["height"].get<int32_t>();
            }
        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFooterData* pDataStructure) {

            if (jsonObjectData.contains("text") && !jsonObjectData["text"].is_null()) {
                pDataStructure->text = jsonObjectData["text"].get<string>();
            }

            if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
                pDataStructure->iconUrl = jsonObjectData["icon_url"].get<string>();
            }

            if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
                pDataStructure->proxyIconUrl = jsonObjectData["proxy_icon_url"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedImageData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
                pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
                pDataStructure->width = jsonObjectData["width"].get<int32_t>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
                pDataStructure->height = jsonObjectData["height"].get<int32_t>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedThumbnailData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
                pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
                pDataStructure->width = jsonObjectData["width"].get<int32_t>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
                pDataStructure->height = jsonObjectData["height"].get<int32_t>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedVideoData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }

            if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
                pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<string>();
            }

            if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
                pDataStructure->width = jsonObjectData["width"].get<int32_t>();
            }

            if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
                pDataStructure->height = jsonObjectData["height"].get<int32_t>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedAuthorData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }

            if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
                pDataStructure->proxyIconUrl = jsonObjectData["proxy_icon_url"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
                pDataStructure->iconUrl = jsonObjectData["icon_url"].get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedProviderData* pDataStructure) {

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFieldData* pDataStructure) {

            if (jsonObjectData.contains("inline") && !jsonObjectData["inline"].is_null()) {
                pDataStructure->Inline = jsonObjectData["inline"].get<bool>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
                pDataStructure->value = jsonObjectData["value"].get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedData* pDataStructure) {

            if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
                pDataStructure->title = jsonObjectData["title"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
                pDataStructure->timestamp = jsonObjectData["timestamp"].get<string>();
            }

            if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
                int32_t colorValInt = jsonObjectData["color"].get<int32_t>();
                stringstream stream;
                stream << setbase(16) << colorValInt;
                pDataStructure->hexColorValue = stream.str();
            }

            if (jsonObjectData.contains("footer") && !jsonObjectData["footer"].is_null()) {
                parseObject(jsonObjectData["footer"], &pDataStructure->footer);
            }

            if (jsonObjectData.contains("image") && !jsonObjectData["image"].is_null()) {
                parseObject(jsonObjectData["image"], &pDataStructure->image);
            }

            if (jsonObjectData.contains("provider") && !jsonObjectData["provider"].is_null()) {
                parseObject(jsonObjectData["provider"], &pDataStructure->provider);
            }

            if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
                parseObject(jsonObjectData["thumbnail"], &pDataStructure->thumbnail);
            }

            if (jsonObjectData.contains("video") && !jsonObjectData["video"].is_null()) {
                parseObject(jsonObjectData["video"], &pDataStructure->video);
            }

            if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
                parseObject(jsonObjectData["author"], &pDataStructure->author);
            }

            if (jsonObjectData.contains("fields") && !jsonObjectData["fields"].is_null()) {
                pDataStructure->fields.resize(jsonObjectData["fields"].size());
                for (uint32_t x = 0; jsonObjectData["fields"].size(); x += 1) {
                    DiscordCoreAPI::EmbedFieldData newData{};
                    parseObject(jsonObjectData[x], &newData);
                    pDataStructure->fields[x] = move(newData);
                }
            }

        };

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionData* pDataStructure) {

            if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
                pDataStructure->count = jsonObjectData["count"].get<int32_t>();
            }

            if (jsonObjectData.contains("me") && !jsonObjectData["me"].is_null()) {
                pDataStructure->count = jsonObjectData["me"].get<bool>();
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
                parseObject(jsonObjectData["emoji"], &pDataStructure->emoji);
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
                pDataStructure->userId = jsonObjectData["user_id"].get<string>();
            }

            if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
                pDataStructure->messageId = jsonObjectData["message_id"].get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
                parseObject(jsonObjectData["member"], &pDataStructure->member);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::ReactionData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::ReactionData newData;
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageActivityData* pDataStructure) {

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::MessageActivityType>();
            }

            if (jsonObjectData.contains("party_id") && !jsonObjectData["party_id"].is_null()) {
                pDataStructure->partyId = jsonObjectData["party_id"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TeamMembersObjectData* pDataStructure) {

            if (jsonObjectData.contains("membership_state") && !jsonObjectData["membership_state"].is_null()) {
                pDataStructure->membershipState = jsonObjectData["membership_state"].get<int32_t>();
            }

            if (jsonObjectData.contains("team_id") && !jsonObjectData["team_id"].is_null()) {
                pDataStructure->teamId = jsonObjectData["team_id"].get<string>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
                pDataStructure->permissions.resize(jsonObjectData["permissions"].size());
                for (uint32_t x = 0; x < jsonObjectData["permissions"].size(); x += 1) {
                    pDataStructure->permissions[x] = jsonObjectData["permissions"][x];
                }
            }

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TeamObjectData* pDataStructure) {

            if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
                pDataStructure->icon = jsonObjectData["icon"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
                pDataStructure->members.resize(jsonObjectData["members"].size());
                for (uint32_t x = 0; x < jsonObjectData["members"].size(); x += 1) {
                    DiscordCoreAPI::TeamMembersObjectData newData{};
                    parseObject(jsonObjectData["members"][x], &newData);
                    pDataStructure->members[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("owner_user_id") && !jsonObjectData["owner_user_id"].is_null()) {
                pDataStructure->ownerUserId = jsonObjectData["owner_user_id"].get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
                pDataStructure->icon = jsonObjectData["icon"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("rpc_origins") && !jsonObjectData["rpc_origins"].is_null()) {
                pDataStructure->rpcOrigins.resize(jsonObjectData["rpc_origins"].size());
                for (uint32_t x = 0; x < jsonObjectData["rpc_origins"].size(); x += 1) {
                    pDataStructure->rpcOrigins[x] = jsonObjectData["rpc_origins"][x];
                }
            }

            if (jsonObjectData.contains("bot_public") && !jsonObjectData["bot_public"].is_null()) {
                pDataStructure->botPublic = jsonObjectData["bot_public"].get<bool>();
            }

            if (jsonObjectData.contains("bot_require_code_grant") && !jsonObjectData["bot_require_code_grant"].is_null()) {
                pDataStructure->botRequireCodeGrant = jsonObjectData["bot_require_code_grant"].get<bool>();
            }

            if (jsonObjectData.contains("terms_of_service_url") && !jsonObjectData["terms_of_service_url"].is_null()) {
                pDataStructure->termsOfServiceUrl = jsonObjectData["terms_of_service_url"].get<string>();
            }

            if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData["privacy_policy_url"].is_null()) {
                pDataStructure->privacyPolicyUrl = jsonObjectData["privacy_policy_url"].get<string>();
            }

            if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
                parseObject(jsonObjectData["owner"], &pDataStructure->owner);
            }

            if (jsonObjectData.contains("summary") && !jsonObjectData["summary"].is_null()) {
                pDataStructure->summary = jsonObjectData["summary"].get<string>();
            }

            if (jsonObjectData.contains("verify_key") && !jsonObjectData["verify_key"].is_null()) {
                pDataStructure->verifyKey = jsonObjectData["verify_key"].get<string>();
            }

            if (jsonObjectData.contains("team") && !jsonObjectData["team"].is_null()) {
                parseObject(jsonObjectData["team"], &pDataStructure->team);
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData["primary_sku_id"].is_null()) {
                pDataStructure->primarySkuId = jsonObjectData["primary_sku_id"].get<string>();
            }

            if (jsonObjectData.contains("slug") && !jsonObjectData["slug"].is_null()) {
                pDataStructure->slug = jsonObjectData["slug"].get<string>();
            }

            if (jsonObjectData.contains("cover_image") && !jsonObjectData["cover_image"].is_null()) {
                pDataStructure->coverImage = jsonObjectData["cover_image"].get<string>();
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
                pDataStructure->coverImage = jsonObjectData["flags"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildScheduledEventMetadata* pDataStructure) {

            if (jsonObjectData.contains("location") && !jsonObjectData["location"].is_null()) {
                pDataStructure->location = jsonObjectData["location"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildScheduledEventData* pDataStructure) {

            if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
                pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<DiscordCoreAPI::GuildScheduledEventPrivacyLevel>();
            }

            if (jsonObjectData.contains("entity_type") && !jsonObjectData["entity_type"].is_null()) {
                pDataStructure->entityType = jsonObjectData["entity_type"].get<DiscordCoreAPI::GuildScheduledEventEntityType>();
            }

            if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
                pDataStructure->status = jsonObjectData["status"].get<DiscordCoreAPI::GuildScheduledEventStatus>();
            }

            if (jsonObjectData.contains("entity_metadata") && !jsonObjectData["entity_metadata"].is_null()) {
                parseObject(jsonObjectData["entity_metadata"], &pDataStructure->entityMetadata);
            }

            if (jsonObjectData.contains("scheduled_start_time") && !jsonObjectData["scheduled_start_time"].is_null()) {
                pDataStructure->scheduledStartTime = jsonObjectData["scheduled_start_time"].get<string>();
            }

            if (jsonObjectData.contains("scheduled_end_time") && !jsonObjectData["scheduled_end_time"].is_null()) {
                pDataStructure->scheduledEndTime = jsonObjectData["scheduled_end_time"].get<string>();
            }

            if (jsonObjectData.contains("user_count") && !jsonObjectData["user_count"].is_null()) {
                pDataStructure->userCount = jsonObjectData["user_count"].get<uint32_t>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }
           
            if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
                pDataStructure->creatorId = jsonObjectData["creator_id"].get<string>();
            }

            if (jsonObjectData.contains("entity_id") && !jsonObjectData["entity_id"].is_null()) {
                pDataStructure->entityId = jsonObjectData["entity_id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
                parseObject(jsonObjectData["creator"], &pDataStructure->creator);
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::GuildScheduledEventData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::GuildScheduledEventData newData;
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InviteData* pDataStructure) {

            if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].type() == nlohmann::detail::value_t::string) {
                pDataStructure->code = jsonObjectData["code"].get<string>();
            }
            else if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].type() == nlohmann::detail::value_t::number_integer) {
                pDataStructure->code = to_string(jsonObjectData["code"].get<int32_t>());
            }

            if (jsonObjectData.contains("guild") && !jsonObjectData["guild"].is_null()) {
                parseObject(jsonObjectData["guild"], &pDataStructure->guild);
            }

            if (jsonObjectData.contains("channel") && !jsonObjectData["channel"].is_null()) {
                parseObject(jsonObjectData["channel"], &pDataStructure->channel);
            }

            if (jsonObjectData.contains("inviter") && !jsonObjectData["inviter"].is_null()) {
                parseObject(jsonObjectData["inviter"], &pDataStructure->inviter);
            }

            if (jsonObjectData.contains("target_type") && !jsonObjectData["target_type"].is_null()) {
                pDataStructure->targetType = jsonObjectData["target_type"].get<int32_t>();
            }

            if (jsonObjectData.contains("target_user") && !jsonObjectData["target_user"].is_null()) {
                parseObject(jsonObjectData["target_user"], &pDataStructure->targetUser);
            }

            if (jsonObjectData.contains("target_application") && !jsonObjectData["target_application"].is_null()) {
                parseObject(jsonObjectData["target_application"], &pDataStructure->targetApplication);
            }

            if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
                pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
            }

            if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
                pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
            }

            if (jsonObjectData.contains("expires_at") && !jsonObjectData["expires_at"].is_null()) {
                pDataStructure->expiresAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["expires_at"].get<string>());
            }

            if (jsonObjectData.contains("stage_instance") && !jsonObjectData["stage_instance"].is_null()) {
                parseObject(jsonObjectData["stage_instance"], &pDataStructure->stageInstance);
            }

            if (jsonObjectData.contains("guild_scheduled_event") && !jsonObjectData["guild_scheduled_event"].is_null()) {
                parseObject(jsonObjectData["guild_scheduled_event"], &pDataStructure->guildScheduledEvent);
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
                pDataStructure->createdAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["created_at"].get<string>());
            }
        };

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::InviteData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::InviteData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageReferenceData* pDataStructure) {

            if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
                pDataStructure->messageId = jsonObjectData["message_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("fail_if_not_exists") && !jsonObjectData["fail_if_not_exists"].is_null()) {
                pDataStructure->failIfNotExists = jsonObjectData["fail_if_not_exists"].get<bool>();
            }

        }
       
        static void parseObject(json jsonObjectData, DiscordCoreAPI::AllowedMentionsData* pDataStructure) {

            if (jsonObjectData.contains("parse") && !jsonObjectData["parse"].is_null()) {
                pDataStructure->parse.resize(jsonObjectData["parse"].size());
                for (uint32_t x = 0; x < jsonObjectData["parse"].size(); x += 1) {
                    pDataStructure->parse[x] = jsonObjectData["parse"][x];
                }
            }

            if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
                pDataStructure->roles.resize(jsonObjectData["roles"].size());
                for (uint32_t x = 0; x < jsonObjectData["roles"].size(); x += 1) {
                    pDataStructure->roles[x] = jsonObjectData["roles"][x];
                }
            }

            if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
                pDataStructure->users.resize(jsonObjectData["users"].size());
                for (uint32_t x = 0; x < jsonObjectData["users"].size(); x += 1) {
                    pDataStructure->users[x] = jsonObjectData["users"][x];
                }
            }

            if (jsonObjectData.contains("replied_user") && !jsonObjectData["replied_user"].is_null()) {
                pDataStructure->repliedUser = jsonObjectData["replied_user"].get<bool>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionApplicationCommandCallbackData* pDataStructure) {

            if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
                pDataStructure->tts = jsonObjectData["tts"].get<bool>();
            }

            if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
                pDataStructure->content = jsonObjectData["content"].get<string>();
            }

            if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
                pDataStructure->embeds.resize(jsonObjectData["embeds"].size());
                for (uint32_t x = 0; x < jsonObjectData["embeds"].size(); x += 1) {
                    DiscordCoreAPI::EmbedData newData{};
                    parseObject(jsonObjectData["embeds"][x], &newData);
                    pDataStructure->embeds[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
                pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
            }

            if (jsonObjectData.contains("allowed_mentions") && !jsonObjectData["allowed_mentions"].is_null()) {
                parseObject(jsonObjectData["allowed_mentions"], &pDataStructure->allowedMentions);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionResponseData* pDataStructure) {

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::InteractionCallbackType>();
            }

            if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
                parseObject(jsonObjectData["data"], &pDataStructure->data);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::SelectOptionData* pDataStructure) {

            if (jsonObjectData.contains("label") && !jsonObjectData["label"].is_null()) {
                pDataStructure->label = jsonObjectData["label"].get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
                pDataStructure->value = jsonObjectData["value"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
                parseObject(jsonObjectData["emoji"], &pDataStructure->emoji);
            }

            if (jsonObjectData.contains("default") && !jsonObjectData["default"].is_null()) {
                pDataStructure->_default = jsonObjectData["default"].get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ActionRowData* pDataStructure) {
            
            if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
                pDataStructure->components.resize(jsonObjectData["components"].size());
                for (uint32_t x = 0; x < jsonObjectData["components"].size(); x += 1) {
                    DiscordCoreAPI::ComponentData newData{};
                    if (jsonObjectData["components"][x].contains("type") && !jsonObjectData["components"]["type"].is_null()) {
                        newData.type = jsonObjectData["components"]["type"].get<DiscordCoreAPI::ComponentType>();
                    }

                    if (jsonObjectData["components"][x].contains("custom_id") && !jsonObjectData["components"][x]["custom_id"].is_null()) {
                        newData.customId = jsonObjectData["components"][x]["custom_id"].get<string>();
                    }

                    if (jsonObjectData["components"][x].contains("placeholder") && !jsonObjectData["components"][x]["placeholder"].is_null()) {
                        newData.placeholder = jsonObjectData["components"][x]["placeholder"].get<string>();
                    }

                    if (jsonObjectData["components"][x].contains("disabled") && !jsonObjectData["components"][x]["disabled"].is_null()) {
                        newData.disabled = jsonObjectData["components"][x]["disabled"].get<bool>();
                    }

                    if (jsonObjectData["components"][x].contains("style") && !jsonObjectData["components"][x]["style"].is_null()) {
                        newData.style = jsonObjectData["components"][x]["style"].get<DiscordCoreAPI::ButtonStyle>();
                    }

                    if (jsonObjectData["components"][x].contains("label") && !jsonObjectData["components"][x]["label"].is_null()) {
                        newData.label = jsonObjectData["components"][x]["label"].get<string>();
                    }

                    if (jsonObjectData["components"][x].contains("emoji") && !jsonObjectData["components"][x]["emoji"].is_null()) {
                        parseObject(jsonObjectData["components"][x]["emoji"], &newData.emoji);
                    }

                    if (jsonObjectData["components"][x].contains("url") && !jsonObjectData["components"][x]["url"].is_null()) {
                        newData.url = jsonObjectData["components"][x]["url"].get<string>();
                    }

                    if (jsonObjectData["components"][x].contains("options") && !jsonObjectData["components"][x]["options"].is_null()) {
                        newData.options.resize(jsonObjectData["components"][x]["options"].size());
                        for (uint32_t x = 0; x < jsonObjectData["components"][x]["options"].size(); x += 1) {
                            DiscordCoreAPI::SelectOptionData newerData{};
                            parseObject(jsonObjectData["components"][x]["options"][x], &newerData);
                            newData.options[x] = move(newerData);
                        }
                    }

                    if (jsonObjectData["components"][x].contains("max_values") && !jsonObjectData["components"][x]["max_values"].is_null()) {
                        newData.maxValues = jsonObjectData["components"][x]["max_values"].get<int32_t>();
                    }

                    if (jsonObjectData["components"][x].contains("min_values") && !jsonObjectData["components"][x]["min_values"].is_null()) {
                        newData.minValues = jsonObjectData["components"][x]["min_values"].get<int32_t>();
                    }

                    pDataStructure->components[x] = move(newData);
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageInteractionData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::InteractionType>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::StickerItemData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
                pDataStructure->formatType = jsonObjectData["format_type"].get<DiscordCoreAPI::StickerItemType>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageDataOld* pDataStructure) {

            if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
                pDataStructure->content = jsonObjectData["content"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
                parseObject(jsonObjectData["author"], &pDataStructure->author);
            }

            if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
                parseObject(jsonObjectData["member"], &pDataStructure->member);
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
                pDataStructure->timestampRaw = jsonObjectData["timestamp"].get<string>();
                pDataStructure->timestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["timestamp"].get<string>());
            }

            if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
                pDataStructure->editedTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["edited_timestamp"].get<string>());
            }

            if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
                pDataStructure->tts = jsonObjectData["tts"].get<bool>();
            }

            if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
                pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
            }

            if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
                pDataStructure->mentions.resize(jsonObjectData["mentions"].size());
                for (uint32_t x = 0; x < jsonObjectData["mentions"].size(); x += 1) {
                    DiscordCoreAPI::UserData newData{};
                    parseObject(jsonObjectData["mentions"][x], &newData);
                    pDataStructure->mentions[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
                pDataStructure->mentionRoles.resize(jsonObjectData["mention_roles"].size());
                for (uint32_t x = 0; x < jsonObjectData["mention_roles"].size(); x += 1) {
                    pDataStructure->mentionRoles[x] = jsonObjectData["mention_roles"][x];
                }
            }

            if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
                pDataStructure->mentionChannels.resize(jsonObjectData["mention_channels"].size());
                for (uint32_t x = 0; x < jsonObjectData["mention_channels"].size(); x += 1) {
                    DiscordCoreAPI::ChannelMentionData newData{};
                    parseObject(jsonObjectData["mention_channels"][x], &newData);
                    pDataStructure->mentionChannels[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
                pDataStructure->attachments.resize(jsonObjectData["attachments"].size());
                for (uint32_t x = 0; x < jsonObjectData["attachments"].size(); x += 1) {
                    DiscordCoreAPI::AttachmentData newData{};
                    parseObject(jsonObjectData["attachments"][x], &newData);
                    pDataStructure->attachments[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
                pDataStructure->embeds.resize(jsonObjectData["embeds"].size());
                for (uint32_t x = 0; x < jsonObjectData["embeds"].size(); x += 1) {
                    DiscordCoreAPI::EmbedData newData{};
                    parseObject(jsonObjectData["embeds"][x], &newData);
                    pDataStructure->embeds[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
                pDataStructure->reactions.resize(jsonObjectData["reactions"].size());
                for (uint32_t x = 0; x < jsonObjectData["reactions"].size(); x += 1) {
                    DiscordCoreAPI::ReactionData newData{};
                    parseObject(jsonObjectData["reactions"][x], &newData);
                    pDataStructure->reactions[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
                pDataStructure->nonce = jsonObjectData["nonce"].get<string>();
            }

            if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
                pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
            }

            if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
                pDataStructure->webhookId = jsonObjectData["webhook_id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = (DiscordCoreAPI::MessageType)jsonObjectData["type"].get<int32_t>();
            }

            if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
                parseObject(jsonObjectData["activity"], &pDataStructure->activity);
            }

            if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
                parseObject(jsonObjectData["application"], &pDataStructure->application);
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["application_id"].get<string>();
            }

            if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
                parseObject(jsonObjectData["message_reference"], &pDataStructure->messageReference);
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
                pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
            }

            if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
                pDataStructure->stickerItems.resize(jsonObjectData["sticker_items"].size());
                for (uint32_t x = 0; x < jsonObjectData["sticker_items"].size(); x += 1) {
                    DiscordCoreAPI::StickerItemData newData{};
                    parseObject(jsonObjectData["sticker_items"][x], &newData);
                    pDataStructure->stickerItems[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
                pDataStructure->stickers.resize(jsonObjectData["stickers"].size());
                for (uint32_t x = 0; x < jsonObjectData["stickers"].size(); x += 1) {
                    DiscordCoreAPI::StickerData newData{};
                    parseObject(jsonObjectData["stickers"][x], &newData);
                    pDataStructure->stickers[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
                parseObject(jsonObjectData["interaction"], &pDataStructure->interaction);
            }

            if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
                pDataStructure->components.resize(jsonObjectData["components"].size());
                for (uint32_t x = 0; x < jsonObjectData["components"].size(); x += 1) {
                    DiscordCoreAPI::ActionRowData newData{};
                    parseObject(jsonObjectData["components"][x], &newData);
                    pDataStructure->components[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
                parseObject(jsonObjectData["thread"], &pDataStructure->thread);
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageData* pDataStructure) {

            if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
                pDataStructure->content = jsonObjectData["content"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
                parseObject(jsonObjectData["author"], &pDataStructure->author);
                pDataStructure->requesterId = pDataStructure->author.id;
            }

            if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
                parseObject(jsonObjectData["member"], &pDataStructure->member);
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
                pDataStructure->timestampRaw = jsonObjectData["timestamp"].get<string>();
                pDataStructure->timestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["timestamp"].get<string>());
            }

            if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
                pDataStructure->editedTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["edited_timestamp"].get<string>());
            }

            if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
                pDataStructure->tts = jsonObjectData["tts"].get<bool>();
            }

            if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
                pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
            }

            if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
                pDataStructure->mentions.resize(jsonObjectData["mentions"].size());
                for (uint32_t x = 0; x < jsonObjectData["mentions"].size(); x += 1) {
                    DiscordCoreAPI::UserData newData{};
                    parseObject(jsonObjectData["mentions"][x], &newData);
                    pDataStructure->mentions[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
                pDataStructure->mentionRoles.resize(jsonObjectData["mention_roles"].size());
                for (uint32_t x = 0; x < jsonObjectData["mention_roles"].size(); x += 1) {
                    pDataStructure->mentionRoles[x] = jsonObjectData["mention_roles"][x];
                }
            }

            if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
                pDataStructure->mentionChannels.resize(jsonObjectData["mentions_channels"].size());
                for (uint32_t x = 0; x < jsonObjectData["mention_channels"].size(); x += 1) {
                    DiscordCoreAPI::ChannelMentionData newData{};
                    parseObject(jsonObjectData["mentions_channels"][x], &newData);
                    pDataStructure->mentionChannels[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
                pDataStructure->attachments.resize(jsonObjectData["attachments"].size());
                for (uint32_t x = 0; x < jsonObjectData["attachments"].size(); x += 1) {
                    DiscordCoreAPI::AttachmentData newData{};
                    parseObject(jsonObjectData["attachments"][x], &newData);
                    pDataStructure->attachments[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
                pDataStructure->embeds.resize(jsonObjectData["embeds"].size());
                for (uint32_t x = 0; x < jsonObjectData["embeds"].size(); x += 1) {
                    DiscordCoreAPI::EmbedData newData{};
                    parseObject(jsonObjectData["embeds"][x], &newData);
                    pDataStructure->embeds[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
                pDataStructure->reactions.resize(jsonObjectData["reactions"].size());
                for (uint32_t x = 0; x < jsonObjectData["reactions"].size(); x += 1) {
                    DiscordCoreAPI::ReactionData newData{};
                    parseObject(jsonObjectData["reactions"][x], &newData);
                    pDataStructure->reactions[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
                pDataStructure->nonce = jsonObjectData["nonce"].get<string>();
            }

            if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
                pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
            }

            if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
                pDataStructure->webhookId = jsonObjectData["webhook_id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = (DiscordCoreAPI::MessageType)jsonObjectData["type"].get<int32_t>();
            }

            if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
                parseObject(jsonObjectData["activity"], &pDataStructure->activity);
            }

            if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
                parseObject(jsonObjectData["application"], &pDataStructure->application);
            }

            if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
                parseObject(jsonObjectData["message_reference"], &pDataStructure->messageReference);
            }

            if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
                pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
                pDataStructure->stickers.resize(jsonObjectData["stickers"].size());
                for (uint32_t x = 0; x < jsonObjectData["stickers"].size(); x += 1) {
                    DiscordCoreAPI::StickerData newData{};
                    parseObject(jsonObjectData["stickers"][x], &newData);
                    pDataStructure->stickers[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
                parseObject(jsonObjectData["interaction"], &pDataStructure->interaction);
            }

            if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
                pDataStructure->components.resize(jsonObjectData["components"].size());
                for (uint32_t x = 0; x < jsonObjectData["components"].size(); x += 1) {
                    DiscordCoreAPI::ActionRowData newData{};
                    parseObject(jsonObjectData["components"][x], &newData);
                    pDataStructure->components[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("referenced_message") && !jsonObjectData["referenced_message"].is_null()) {
                parseObject(jsonObjectData["referenced_message"], &pDataStructure->referencedMessage);
            }

            if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
                parseObject(jsonObjectData["thread"], &pDataStructure->thread);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::MessageData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::MessageData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandOptionChoiceData* pDataStructure) {

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null() && jsonObjectData["value"].is_string()) {
                pDataStructure->valueString = jsonObjectData["value"].get<string>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null() && jsonObjectData["value"].is_number_integer()) {
                pDataStructure->valueInt = jsonObjectData["value"].get<int32_t>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandOptionData* pDataStructure) {

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("channel_types") && !jsonObjectData["channel_types"].is_null()) {
                pDataStructure->channelTypes.resize(jsonObjectData["channel_types"].size());
                for (uint32_t x = 0; x < jsonObjectData["channel_types"].size(); x += 1) {
                    pDataStructure->channelTypes[x] = jsonObjectData["channel_types"][x];
                }
            }

            if (jsonObjectData.contains("required") && !jsonObjectData["required"].is_null()) {
                pDataStructure->required = jsonObjectData["required"].get<bool>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandOptionType>();
            }

            if (pDataStructure->type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND_GROUP || pDataStructure->type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND) {
                if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
                    pDataStructure->options.resize(jsonObjectData["options"].size());
                    for (uint32_t x = 0; x < jsonObjectData["options"].size(); x += 1) {
                        DiscordCoreAPI::ApplicationCommandOptionData newData{};
                        parseObject(jsonObjectData["options"][x], &newData);
                        pDataStructure->options[x] = move(newData);
                    }
                }
            }
            else {
                if (jsonObjectData.contains("choices") && !jsonObjectData["choices"].is_null()) {
                    pDataStructure->choices.resize(jsonObjectData["choices"].size());
                    for (uint32_t x = 0; x < jsonObjectData["choices"].size(); x += 1) {
                        DiscordCoreAPI::ApplicationCommandOptionChoiceData newData{};
                        parseObject(jsonObjectData["choices"][x], &newData);
                        pDataStructure->choices[x] = move(newData);
                    }
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["application_id"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                pDataStructure->description = jsonObjectData["description"].get<string>();
            }

            if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
                pDataStructure->version = jsonObjectData["version"].get<string>();
            }

            if (jsonObjectData.contains("default_permission") && !jsonObjectData["default_permission"].is_null()) {
                pDataStructure->defaultPermission = jsonObjectData["default_permission"].get<bool>();
            }

            if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
                pDataStructure->options.resize(jsonObjectData["options"].size());
                for (uint32_t x = 0; x < jsonObjectData["options"].size(); x += 1) {
                    DiscordCoreAPI::ApplicationCommandOptionData newData{};
                    parseObject(jsonObjectData["options"][x], &newData);
                    pDataStructure->options[x] = move(newData);
                }
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::ApplicationCommandData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x+=1 ) {
                DiscordCoreAPI::ApplicationCommandData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionDataOption* pDataStructure) {

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandOptionType>();
            }

            if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
                if (jsonObjectData["value"].is_boolean()) {
                    pDataStructure->valueBool = jsonObjectData["value"].get<bool>();
                }
                if (jsonObjectData["value"].is_string()) {
                    pDataStructure->valueString = jsonObjectData["value"].get<string>();
                }
                if (jsonObjectData["value"].is_number()) {
                    pDataStructure->valueInt = jsonObjectData["value"].get<int32_t>();
                }
            }

            if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
                pDataStructure->options.resize(jsonObjectData["options"].size());
                for (uint32_t x = 0; x < jsonObjectData["options"].size(); x += 1) {
                    DiscordCoreAPI::ApplicationCommandInteractionDataOption newData{};
                    parseObject(jsonObjectData["options"][x], &newData);
                    pDataStructure->options[x] = move(newData);
                }
            }

        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::YouTubeFormat>* pDataStructure) {

            if (!jsonObjectData.is_null()) {
                if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("formats") && !jsonObjectData["streamingData"]["formats"].is_null()) {
                    for (uint32_t x = 0; x < jsonObjectData["streamingData"]["formats"].size(); x += 1) {
                        DiscordCoreAPI::YouTubeFormat newData{};
                        if (jsonObjectData["streamingData"]["formats"][x].contains("audioQuality") && !jsonObjectData["streamingData"]["formats"][x]["audioQuality"].is_null()) {
                            newData.audioQuality = jsonObjectData["streamingData"]["formats"][x]["audioQuality"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("averageBitrate") && !jsonObjectData["streamingData"]["formats"][x]["averageBitrate"].is_null()) {
                            newData.averageBitrate = jsonObjectData["streamingData"]["formats"][x]["averageBitrate"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("audioSampleRate") && !jsonObjectData["streamingData"]["formats"][x]["audioSampleRate"].is_null()) {
                            newData.audioSampleRate = jsonObjectData["streamingData"]["formats"][x]["audioSampleRate"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("bitrate") && !jsonObjectData["streamingData"]["formats"][x]["bitrate"].is_null()) {
                            newData.bitrate = jsonObjectData["streamingData"]["formats"][x]["bitrate"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("contentLength") && !jsonObjectData["streamingData"]["formats"][x]["contentLength"].is_null()) {
                            newData.contentLength = stoll(jsonObjectData["streamingData"]["formats"][x]["contentLength"].get<string>());
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("fps") && !jsonObjectData["streamingData"]["formats"][x]["fps"].is_null()) {
                            newData.fps = jsonObjectData["streamingData"]["formats"][x]["fps"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("height") && !jsonObjectData["streamingData"]["formats"][x]["height"].is_null()) {
                            newData.height = jsonObjectData["streamingData"]["formats"][x]["height"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("width") && !jsonObjectData["streamingData"]["formats"][x]["width"].is_null()) {
                            newData.height = jsonObjectData["streamingData"]["formats"][x]["width"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("aitags") && !jsonObjectData["streamingData"]["formats"][x]["aitags"].is_null()) {
                            newData.aitags = jsonObjectData["streamingData"]["formats"][x]["aitags"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("itag") && !jsonObjectData["streamingData"]["formats"][x]["itag"].is_null()) {
                            newData.itag = jsonObjectData["streamingData"]["formats"][x]["itag"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("mimeType") && !jsonObjectData["streamingData"]["formats"][x]["mimeType"].is_null()) {
                            newData.mimeType = jsonObjectData["streamingData"]["formats"][x]["mimeType"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("quality") && !jsonObjectData["streamingData"]["formats"][x]["quality"].is_null()) {
                            newData.quality = jsonObjectData["streamingData"]["formats"][x]["quality"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["formats"][x].contains("signatureCipher") && !jsonObjectData["streamingData"]["formats"][x]["signatureCipher"].is_null()) {
                            newData.signatureCipher = jsonObjectData["streamingData"]["formats"][x]["signatureCipher"].get<string>();
                        }
                        else if (jsonObjectData["streamingData"]["formats"][x].contains("cipher") && !jsonObjectData["streamingData"]["formats"][x]["cipher"].is_null()) {
                            newData.signatureCipher = jsonObjectData["streamingData"]["formats"][x]["cipher"].get<string>();
                        }
                        if (newData.signatureCipher.find("&sp") != string::npos) {
                            newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
                        }
                        if (newData.signatureCipher.find("url") != string::npos) {
                            newData.downloadURL = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
                        }
                        else if (jsonObjectData["streamingData"]["formats"][x].contains("url") && !jsonObjectData["streamingData"]["formats"][x]["url"].is_null()) {
                            newData.downloadURL = jsonObjectData["streamingData"]["formats"][x]["url"];
                        }
                        (*pDataStructure)[x] = move(newData);
                    }
                }

                if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("adaptiveFormats") && !jsonObjectData["streamingData"]["adaptiveFormats"].is_null()) {
                    for (uint32_t x = 0; x < jsonObjectData["streamingData"]["adaptiveFormats"].size(); x += 1) {
                        DiscordCoreAPI::YouTubeFormat newData{};
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("audioQuality") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["audioQuality"].is_null()) {
                            newData.audioQuality = jsonObjectData["streamingData"]["adaptiveFormats"][x]["audioQuality"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("averageBitrate") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["averageBitrate"].is_null()) {
                            newData.averageBitrate = jsonObjectData["streamingData"]["adaptiveFormats"][x]["averageBitrate"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("audioSampleRate") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["audioSampleRate"].is_null()) {
                            newData.audioSampleRate = jsonObjectData["streamingData"]["adaptiveFormats"][x]["audioSampleRate"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("bitrate") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["bitrate"].is_null()) {
                            newData.bitrate = jsonObjectData["streamingData"]["adaptiveFormats"][x]["bitrate"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("contentLength") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["contentLength"].is_null()) {
                            newData.contentLength = stoll(jsonObjectData["streamingData"]["adaptiveFormats"][x]["contentLength"].get<string>());
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("fps") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["fps"].is_null()) {
                            newData.fps = jsonObjectData["streamingData"]["adaptiveFormats"][x]["fps"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("height") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["height"].is_null()) {
                            newData.height = jsonObjectData["streamingData"]["adaptiveFormats"][x]["height"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("width") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["width"].is_null()) {
                            newData.height = jsonObjectData["streamingData"]["adaptiveFormats"][x]["width"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("aitags") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["aitags"].is_null()) {
                            newData.aitags = jsonObjectData["streamingData"]["adaptiveFormats"][x]["aitags"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("itag") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["itag"].is_null()) {
                            newData.itag = jsonObjectData["streamingData"]["adaptiveFormats"][x]["itag"].get<int32_t>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("mimeType") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["mimeType"].is_null()) {
                            newData.mimeType = jsonObjectData["streamingData"]["adaptiveFormats"][x]["mimeType"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("quality") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["quality"].is_null()) {
                            newData.quality = jsonObjectData["streamingData"]["adaptiveFormats"][x]["quality"].get<string>();
                        }
                        if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("signatureCipher") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["signatureCipher"].is_null()) {
                            newData.signatureCipher = jsonObjectData["streamingData"]["adaptiveFormats"][x]["signatureCipher"].get<string>();
                        }
                        else if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("cipher") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["cipher"].is_null()) {
                            newData.signatureCipher = jsonObjectData["streamingData"]["adaptiveFormats"][x]["cipher"].get<string>();
                        }
                        if (newData.signatureCipher.find("&sp") != string::npos) {
                            newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
                        }
                        if (newData.signatureCipher.find("url") != string::npos) {
                            newData.downloadURL = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
                        }
                        else if (jsonObjectData["streamingData"]["adaptiveFormats"][x].contains("url") && !jsonObjectData["streamingData"]["adaptiveFormats"][x]["url"].is_null()) {
                            newData.downloadURL = jsonObjectData["streamingData"]["adaptiveFormats"][x]["url"];
                        }
                        (*pDataStructure)[x] = move(newData);
                    }
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::CommandData* pDataStructure) {
            if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
                pDataStructure->optionsArgs.resize(jsonObjectData["options"].size());
                for (uint32_t x = 0; x < jsonObjectData["options"].size(); x += 1) {
                    if (jsonObjectData["options"][x].contains("type") && jsonObjectData["options"][x]["type"] == 1) {
                        if (jsonObjectData["options"][x].contains("name")) {
                            pDataStructure->optionsArgs.push_back(jsonObjectData["options"][x]["name"]);
                        }
                    }
                    if (jsonObjectData["options"][x].contains("options")) {
                        parseObject(jsonObjectData["options"][x], pDataStructure);
                    }
                    if (jsonObjectData["options"][x].contains("value") && !jsonObjectData["options"][x]["value"].is_null()) {
                        auto& newValueNew = jsonObjectData["options"][x]["value"];
                        if (newValueNew.is_string()) {
                            pDataStructure->optionsArgs[x] = move(newValueNew);
                        }
                        else {
                            pDataStructure->optionsArgs[x] = move(to_string(newValueNew));
                        }
                    }
                }
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->commandName = jsonObjectData["name"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionData* pDataStructure) {

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandType>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
                pDataStructure->options.resize(jsonObjectData["options"].size());
                for (uint32_t x = 0; x < jsonObjectData["options"].size(); x += 1) {
                    DiscordCoreAPI::ApplicationCommandInteractionDataOption newData{};
                    parseObject(jsonObjectData["options"][x], &newData);
                    pDataStructure->options[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("resolved") && !jsonObjectData["resolved"].is_null()) {
                auto& value = jsonObjectData["resolved"];
                if (value.contains("users") && !value["users"].is_null()) {
                    auto newMap = value["users"].get<map<string, json>>();
                    map<string, DiscordCoreAPI::UserData> newerMap;
                    for (const auto& [key, newValue] : newMap) {
                        DiscordCoreAPI::UserData newData{};
                        parseObject(newValue, &newData);
                        pDataStructure->resolved.users.insert(make_pair(key, newData));
                    }
                }
                if (value.contains("channels") && !value["channels"].is_null()) {
                    auto newMap = value["channels"].get<map<string, json>>();
                    map<string, DiscordCoreAPI::ChannelData> newerMap;
                    for (const auto& [key, newValue] : newMap) {
                        DiscordCoreAPI::ChannelData newData{};
                        parseObject(newValue, &newData);
                        pDataStructure->resolved.channels.insert(make_pair(key, newData));
                    }
                }
                if (value.contains("roles") && !value["roles"].is_null()) {
                    auto newMap = value["roles"].get<map<string, json>>();
                    map<string, DiscordCoreAPI::RoleData> newerMap;
                    for (const auto& [key, newValue] : newMap) {
                        DiscordCoreAPI::RoleData newData{};
                        parseObject(newValue, &newData);
                        pDataStructure->resolved.roles.insert(make_pair(key, newData));
                    }
                }
                if (value.contains("members") && !value["members"].is_null()) {
                    auto newMap = value["members"].get<map<string, json>>();
                    map<string, DiscordCoreAPI::GuildMemberData> newerMap;
                    for (const auto& [key, newValue] : newMap) {
                        DiscordCoreAPI::GuildMemberData newData{};
                        parseObject(newValue, &newData);
                        pDataStructure->resolved.members.insert(make_pair(key, newData));
                    }
                }
                if (value.contains("messages") && !value["messages"].is_null()) {
                    auto newMap = value["messages"].get<map<string, json>>();
                    map<string, DiscordCoreAPI::MessageData> newerMap;
                    for (const auto& [key, newValue] : newMap) {
                        DiscordCoreAPI::MessageData newData{};
                        parseObject(newValue, &newData);
                        pDataStructure->resolved.messages.insert(make_pair(key, newData));
                    }
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::MessageCommandInteractionData* pDataStructure) {

            if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
                pDataStructure->targetId = jsonObjectData["target_id"].get<string>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::UserCommandInteractionData* pDataStructure) {

            if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
                pDataStructure->targetId = jsonObjectData["target_id"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ComponentInteractionData* pDataStructure) {

            if (jsonObjectData.contains("values") && !jsonObjectData["values"].is_null()) {
                pDataStructure->values.resize(jsonObjectData["values"].size());
                for (uint32_t x = 0; x < jsonObjectData["values"].size(); x += 1) {
                    pDataStructure->values[x] = jsonObjectData["values"][x];
                }
            }

            if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
                pDataStructure->customId = jsonObjectData["custom_id"].get<string>();
            }

            if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
                pDataStructure->componentType = jsonObjectData["component_type"].get<DiscordCoreAPI::ComponentType>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionDataData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                parseObject(jsonObjectData, &pDataStructure->applicationCommanddata);
            }

            if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
                parseObject(jsonObjectData, &pDataStructure->messageInteractionData);
                parseObject(jsonObjectData, &pDataStructure->userInteractionData);
            }

            if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
                parseObject(jsonObjectData, &pDataStructure->componentData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionData* pDataStructure) {

            if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
                parseObject(jsonObjectData["data"], &pDataStructure->data);
                pDataStructure->rawData = jsonObjectData["data"];
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::InteractionType>();
            }

            if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
                pDataStructure->token = jsonObjectData["token"].get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
                parseObject(jsonObjectData["member"], &pDataStructure->member);
                pDataStructure->user = pDataStructure->member.user;
            }
            else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("message") && !jsonObjectData["message"].is_null()) {
                parseObject(jsonObjectData["message"], &pDataStructure->message);
            }

            if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
                pDataStructure->version = jsonObjectData["version"].get<int32_t>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["application_id"].get<string>();

            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionRemoveData* pDataStructure) {

            if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
                pDataStructure->userId = jsonObjectData["user_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
                pDataStructure->messageId = jsonObjectData["message_id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
                parseObject(jsonObjectData["emoji"], &pDataStructure->emoji);
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::WebhookData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["id"].get< int32_t>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
                pDataStructure->avatar = jsonObjectData["avatar"].get<string>();
            }

            if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
                pDataStructure->token = jsonObjectData["token"].get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["application_id"].get<string>();
            }

            if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
                parseObject(jsonObjectData["source_guild"], &pDataStructure->sourceGuild);
            }

            if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
                parseObject(jsonObjectData["source_channel"], &pDataStructure->sourceChannel);
            }

            if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
                pDataStructure->url = jsonObjectData["url"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogChangeData* pDataStructure) {

            if (jsonObjectData.contains("new_value") && !jsonObjectData["new_value"].is_null()) {
                if (jsonObjectData["new_value"].is_string()) {
                    pDataStructure->newValueString = jsonObjectData["new_value"].get<string>();
                }
                else if (jsonObjectData["new_value"].is_boolean()) {
                    pDataStructure->newValueBool = jsonObjectData["new_value"].get<bool>();
                }
                else if (jsonObjectData["new_value"].is_number_integer()) {
                    pDataStructure->newValueInt = jsonObjectData["new_value"].get<int32_t>();
                }
            }

            if (jsonObjectData.contains("old_value") && !jsonObjectData["old_value"].is_null()) {
                if (jsonObjectData["old_value"].is_string()) {
                    pDataStructure->oldValueString = jsonObjectData["old_value"].get<string>();
                }
                else if (jsonObjectData["old_value"].is_boolean()) {
                    pDataStructure->oldValueBool = jsonObjectData["old_value"].get<bool>();
                }
                else if (jsonObjectData["old_value"].is_number_integer()) {
                    pDataStructure->oldValueInt = jsonObjectData["old_value"].get<int32_t>();
                }
            }

            if (jsonObjectData.contains("key") && !jsonObjectData["key"].is_null()) {
                pDataStructure->key = jsonObjectData["key"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryInfoData* pDataStructure) {

            if (jsonObjectData.contains("delete_member_days") && !jsonObjectData["delete_member_days"].is_null()) {
                pDataStructure->deleteMemberDays = jsonObjectData["delete_member_days"].get<string>();
            }

            if (jsonObjectData.contains("members_removed") && !jsonObjectData["members_removed"].is_null()) {
                pDataStructure->membersRemoved = jsonObjectData["members_removed"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
                pDataStructure->messageId = jsonObjectData["message_id"].get<string>();
            }

            if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
                pDataStructure->count = jsonObjectData["count"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<string>();
            }

            if (jsonObjectData.contains("role_name") && !jsonObjectData["role_name"].is_null()) {
                pDataStructure->roleName = jsonObjectData["role_name"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryData* pDataStructure) {

            if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
                pDataStructure->targetId = jsonObjectData["target_id"].get<string>();
            }

            if (jsonObjectData.contains("changes") && !jsonObjectData["changes"].is_null()) {
                pDataStructure->changes.resize(jsonObjectData["changes"].size());
                for (uint32_t x = 0; x < jsonObjectData["changes"].size(); x += 1) {
                    DiscordCoreAPI::AuditLogChangeData newData{};
                    parseObject(jsonObjectData["changes"][x], &newData);
                    pDataStructure->changes[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
                pDataStructure->userId = jsonObjectData["user_id"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
                pDataStructure->createdTimeStamp = (DiscordCoreAPI::TimeStamp)pDataStructure->getCreatedAtTimestamp();

            }

            if (jsonObjectData.contains("action_type") && !jsonObjectData["action_type"].is_null()) {
                pDataStructure->actionType = jsonObjectData["action_type"].get<DiscordCoreAPI::AuditLogEvent>();
            }

            if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
                parseObject(jsonObjectData["options"], &pDataStructure->options);
            }

            if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
                pDataStructure->reason = jsonObjectData["reason"].get<string>();
            }
            
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AccountData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::IntegrationData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
                pDataStructure->name = jsonObjectData["name"].get<string>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<string>();
            }

            if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
                pDataStructure->enabled = jsonObjectData["enabled"].get<bool>();
            }

            if (jsonObjectData.contains("syncing") && !jsonObjectData["syncing"].is_null()) {
                pDataStructure->syncing = jsonObjectData["syncing"].get<bool>();
            }

            if (jsonObjectData.contains("role_id") && !jsonObjectData["role_id"].is_null()) {
                pDataStructure->roleId = jsonObjectData["role_id"].get<string>();
            }

            if (jsonObjectData.contains("enable_emoticons") && !jsonObjectData["enable_emoticons"].is_null()) {
                pDataStructure->enableEmoticons = jsonObjectData["enable_emoticons"].get<bool>();
            }

            if (jsonObjectData.contains("expire_behavior") && !jsonObjectData["expire_behavior"].is_null()) {
                pDataStructure->expireBehavior = jsonObjectData["expire_behavior"].get< int32_t>();
            }

            if (jsonObjectData.contains("expire_grace_period") && !jsonObjectData["expire_grace_period"].is_null()) {
                pDataStructure->expireGracePeriod = jsonObjectData["expire_grace_period"].get< int32_t>();
            }

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

            if (jsonObjectData.contains("account") && !jsonObjectData["account"].is_null()) {
                parseObject(jsonObjectData["account"], &pDataStructure->account);
            }

            if (jsonObjectData.contains("synced_at") && !jsonObjectData["synced_at"].is_null()) {
                pDataStructure->syncedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData["synced_at"].get<string>());
            }

            if (jsonObjectData.contains("subscriber_count") && !jsonObjectData["subscriber_count"].is_null()) {
                pDataStructure->subscriberCount = jsonObjectData["subscriber_count"].get< int32_t>();
            }

            if (jsonObjectData.contains("revoked") && !jsonObjectData["revoked"].is_null()) {
                pDataStructure->revoked = jsonObjectData["revoked"].get<bool>();
            }

            if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
                parseObject(jsonObjectData["application"], &pDataStructure->application);
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::IntegrationData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x += 1) {
                DiscordCoreAPI::IntegrationData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogData* pDataStructure) {

            if (jsonObjectData.contains("webhooks") && !jsonObjectData["webhooks"].is_null()) {
                pDataStructure->webhooks.resize(jsonObjectData["webhooks"].size());
                for (uint32_t x = 0; x < jsonObjectData["webhooks"].size(); x += 1) {
                    DiscordCoreAPI::WebhookData newData{};
                    parseObject(jsonObjectData["webhooks"][x], &newData);
                    pDataStructure->webhooks[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
                pDataStructure->users.resize(jsonObjectData["users"].size());
                for (uint32_t x = 0; x < jsonObjectData["users"].size(); x += 1) {
                    DiscordCoreAPI::UserData newData{};
                    parseObject(jsonObjectData["users"][x], &newData);
                    pDataStructure->users[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData["audit_log_entries"].is_null()) {
                pDataStructure->auditLogEntries.resize(jsonObjectData["audit_log_entries"].size());
                for (uint32_t x = 0; x < jsonObjectData["audit_log_entries"].size(); x += 1) {
                    DiscordCoreAPI::AuditLogEntryData newData{};
                    parseObject(jsonObjectData["audit_log_entries"][x], &newData);
                    pDataStructure->auditLogEntries[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
                pDataStructure->integrations.resize(jsonObjectData["integrations"].size());
                for (uint32_t x = 0; x < jsonObjectData["integrations"].size(); x += 1) {
                    DiscordCoreAPI::IntegrationData newData{};
                    parseObject(jsonObjectData["integrations"][x], &newData);
                    pDataStructure->integrations[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
                pDataStructure->threads.resize(jsonObjectData["threads"].size());
                for (uint32_t x = 0; x < jsonObjectData["threads"].size(); x += 1) {
                    DiscordCoreAPI::ChannelData newData{};
                    parseObject(jsonObjectData["threads"][x], &newData);
                    pDataStructure->threads[x] = move(newData);
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::TypingStartData* pDataStructure) {

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructure->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
                parseObject(jsonObjectData["member"], &pDataStructure->member);
            }

            if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
                pDataStructure->userId = jsonObjectData["user_id"].get<string>();
            }

            if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
                pDataStructure->timestamp = jsonObjectData["timestamp"].get<int32_t>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelPinsUpdateEventData* pDataStructre) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructre->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
                pDataStructre->channelId = jsonObjectData["channel_id"].get<string>();
            }

            if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
                pDataStructre->lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::BanData* pDataStructure) {

            if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                parseObject(jsonObjectData["user"], &pDataStructure->user);
            }

            if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
                pDataStructure->reason = jsonObjectData["reason"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::BanData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x+=1 ) {
                DiscordCoreAPI::BanData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadListSyncData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("channel_ids") && !jsonObjectData["channel_ids"].is_null()) {
                pDataStructure->channelIds.resize(jsonObjectData["channel_ids"].size());
                for (uint32_t x = 0; x < jsonObjectData["channel_ids"].size(); x += 1) {
                    pDataStructure->channelIds[x] = jsonObjectData["channel_ids"][x];
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
                pDataStructure->members.resize(jsonObjectData["members"].size());
                for (uint32_t x = 0; x < jsonObjectData["members"].size(); x += 1) {
                    DiscordCoreAPI::ThreadMemberData newData{};
                    parseObject(jsonObjectData["members"][x], &newData);
                    pDataStructure->members[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
                pDataStructure->threads.resize(jsonObjectData["threads"].size());
                for (uint32_t x = 0; x < jsonObjectData["threads"].size(); x += 1) {
                    DiscordCoreAPI::ChannelData newData{};
                    parseObject(jsonObjectData["threads"][x], &newData);
                    pDataStructure->threads[x] = move(newData);
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMembersUpdateData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
                pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
            }

            if (jsonObjectData.contains("added_members") && !jsonObjectData["added_members"].is_null()) {
                pDataStructure->addedMembers.resize(jsonObjectData["added_members"].size());
                for (uint32_t x = 0; x < jsonObjectData["added_members"].size(); x += 1) {
                    DiscordCoreAPI::ThreadMemberData newData{};
                    parseObject(jsonObjectData["members"][x], &newData);
                    pDataStructure->addedMembers[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("removed_member_ids") && !jsonObjectData["removed_member_ids"].is_null()) {
                pDataStructure->removedMemberIds.resize(jsonObjectData["removed_member_ids"].size());
                for (uint32_t x = 0; x < jsonObjectData["removed_member_ids"].size(); x += 1) {
                    pDataStructure->removedMemberIds[x] = jsonObjectData["removed_member_ids"][x];
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ActiveThreadsData* pDataStructure) {
            if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
                pDataStructure->threads.resize(jsonObjectData["threads"].size());
                for (uint32_t x = 0; x < jsonObjectData["threads"].size(); x += 1) {
                    DiscordCoreAPI::ChannelData newData{};
                    parseObject(jsonObjectData["threads"][x], &newData);
                    pDataStructure->threads[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
                pDataStructure->members.resize(jsonObjectData["members"].size());
                for (uint32_t x = 0; x < jsonObjectData["members"].size(); x += 1) {
                    DiscordCoreAPI::ThreadMemberData newData{};
                    parseObject(jsonObjectData["members"][x], &newData);
                    pDataStructure->members[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("has_more") && !jsonObjectData["has_more"].is_null()) {
                pDataStructure->hasMore = jsonObjectData["has_more"].get<bool>();
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildEmojisUpdateEventData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
                pDataStructure->emojis.resize(jsonObjectData["emojis"].size());
                for (uint32_t x = 0; x < jsonObjectData["emojis"].size(); x += 1) {
                    DiscordCoreAPI::EmojiData newData{};
                    parseObject(jsonObjectData["emojis"][x], &newData);
                    pDataStructure->emojis[x] = move(newData);
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildStickersUpdateEventData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
                pDataStructure->stickers.resize(jsonObjectData["stickers"].size());
                for (uint32_t x = 0; x < jsonObjectData["stickers"].size(); x += 1) {
                    DiscordCoreAPI::StickerData newData{};
                    parseObject(jsonObjectData["stickers"][x], &newData);
                    pDataStructure->stickers[x] = move(newData);
                }
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildMembersChunkEventData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
                pDataStructure->nonce = jsonObjectData["nonce"].get<string>();
            }

            if (jsonObjectData.contains("chunk_index") && !jsonObjectData["chunk_index"].is_null()) {
                pDataStructure->chunkIndex = jsonObjectData["chunk_index"].get<int32_t>();
            }

            if (jsonObjectData.contains("chunk_count") && !jsonObjectData["chunk_count"].is_null()) {
                pDataStructure->chunkCount = jsonObjectData["chunk_count"].get<int32_t>();
            }

            if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
                pDataStructure->presences.resize(jsonObjectData["presences"].size());
                for (uint32_t x = 0; x < jsonObjectData["presences"].size(); x += 1) {
                    DiscordCoreAPI::PresenceUpdateData newData{};
                    parseObject(jsonObjectData["presences"][x], &newData);
                    pDataStructure->presences[x] = move(newData);
                }
            }

            if (jsonObjectData.contains("not_found") && !jsonObjectData["not_found"].is_null()) {
                pDataStructure->notFound.resize(jsonObjectData["not_found"].size());
                for (uint32_t x = 0; x < jsonObjectData["not_found"].size(); x += 1) {
                    pDataStructure->notFound[x] = jsonObjectData["not_found"][x];
                }
            }

            if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
                pDataStructure->members.resize(jsonObjectData["members"].size());
                for (uint32_t x = 0; x < jsonObjectData["members"].size(); x += 1) {
                    DiscordCoreAPI::GuildMemberData newData{};
                    parseObject(jsonObjectData["members"][x], &newData);
                    pDataStructure->members[x] = move(newData);
                }
            }

        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandPermissionData* pDataStructure) {

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("permission") && !jsonObjectData["permission"].is_null()) {
                pDataStructure->permission = jsonObjectData["permission"].get<bool>();
            }

            if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
                pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandPermissionType>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::GuildApplicationCommandPermissionsData* pDataStructure) {

            if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
                pDataStructure->guildId = jsonObjectData["guild_id"].get<string>();
            }

            if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
                pDataStructure->applicationId = jsonObjectData["application_id"].get<string>();
            }

            if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
                pDataStructure->id = jsonObjectData["id"].get<string>();
            }

            if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
                pDataStructure->permissions.resize(jsonObjectData["permissions"].size());
                for (uint32_t x = 0; x < jsonObjectData["permissions"].size(); x += 1) {
                    DiscordCoreAPI::ApplicationCommandPermissionData newData{};
                    DataParser::parseObject(jsonObjectData["permissions"][x], &newData);
                    pDataStructure->permissions[x] = move(newData);
                }
            }
        }

        static void parseObject(json jsonObjectData, vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData>* pDataStructure) {
            pDataStructure->resize(jsonObjectData.size());
            for (uint32_t x = 0; x < jsonObjectData.size(); x+=1 ) {
                DiscordCoreAPI::GuildApplicationCommandPermissionsData newData{};
                parseObject(jsonObjectData[x], &newData);
                (*pDataStructure)[x] = move(newData);
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::YouTubeSong* pDataStructure) {

            if (jsonObjectData.contains("lengthText") && !jsonObjectData["lengthText"].is_null()) {
                pDataStructure->duration = jsonObjectData["lengthText"]["accessibility"]["accessibilityData"]["label"].get<string>();
            }

            if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData["detailedMetadataSnippets"].is_null()) {
                pDataStructure->description.resize(jsonObjectData["detailedMetadataSnippets"].size());
                for (uint32_t x = 0; x < jsonObjectData["detailedMetadataSnippets"].at(0)["snippetText"]["runs"].size(); x += 1) {
                    pDataStructure->description += jsonObjectData["detailedMetadataSnippets"]["text"].get<string>();
                }
            }

            if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
                pDataStructure->thumbnailURL = jsonObjectData["thumbnail"]["thumbnails"].at(0)["url"].get<string>();
            }

            if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
                pDataStructure->songId = "https://www.youtube.com/watch?v=" + jsonObjectData["videoId"].get<string>();
            }

            if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
                if (jsonObjectData["title"].contains("runs")) {
                    pDataStructure->songTitle = jsonObjectData["title"]["runs"].at(0)["text"].get<string>();
                }
                else if (jsonObjectData["title"].contains("simpleText")) {
                    pDataStructure->songTitle = jsonObjectData["title"]["simpleText"].get<string>();
                }
            }

            if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
                pDataStructure->songId = jsonObjectData["videoId"].get<string>();
            }
        }

        static void parseObject(json jsonObjectData, DiscordCoreAPI::SoundCloudSong* pDataStructure) {

            if (jsonObjectData.contains("track_authorization") && !jsonObjectData["track_authorization"].is_null()) {
                pDataStructure->trackAuthorization = jsonObjectData["track_authorization"].get<string>();
            }

            if (jsonObjectData.contains("media") && !jsonObjectData["media"].is_null()) {
                bool isItFound{ false };
                for (auto& value : jsonObjectData["media"]["transcodings"]) {
                    if (value["preset"] == "opus_0_0") {
                        isItFound = true;
                        pDataStructure->firstDownloadURL = to_string(to_hstring(value["url"].get<string>()));
                        pDataStructure->songId = to_string(to_hstring(value["url"].get<string>()));
                    }
                }
                bool isItFound2{ false };
                if (!isItFound) {
                    for (auto& value : jsonObjectData["media"]["transcodings"]) {
                        if (value["preset"] == "mp3_0_0") {
                            pDataStructure->firstDownloadURL = to_string(to_hstring(value["url"].get<string>()));
                            pDataStructure->songId = to_string(to_hstring(value["url"].get<string>()));
                            isItFound2 = true;
                        }

                    }
                }

                if (!isItFound2 && !isItFound) {
                    for (auto& value : jsonObjectData["media"]["transcodings"]) {
                        pDataStructure->firstDownloadURL = to_string(to_hstring(value["url"].get<string>()));
                        pDataStructure->songId = to_string(to_hstring(value["url"].get<string>()));
                    }
                }
            }

            if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null() && !jsonObjectData["title"].is_object()) {
                pDataStructure->songTitle = to_string(to_hstring(jsonObjectData["title"].get<string>()));
            }
            if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
                string newString = to_string(to_hstring(jsonObjectData["description"].get<string>()));
                if (newString.size() > 100) {
                    newString = newString.substr(0, 100);
                }
                char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
                char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);
                for (int32_t x = 0; x < newString.size(); x += 1) {
                    pDataStructure->description.push_back(newString02[x]);
                }
                pDataStructure->description += "...";
            }

            if (jsonObjectData.contains("artwork_url") && !jsonObjectData["artwork_url"].is_null()) {
                pDataStructure->thumbnailURL = to_string(to_hstring(jsonObjectData["artwork_url"].get<string>()));
            }
            else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
                if (jsonObjectData["user"].contains("avatar_url") && !jsonObjectData["user"]["avatar_url"].is_null()) {
                    pDataStructure->thumbnailURL = jsonObjectData["user"]["avatar_url"].get<string>();
                }
            }

            if (jsonObjectData.contains("duration") && !jsonObjectData["duration"].is_null()) {
                int32_t durationNew = jsonObjectData["duration"].get<int32_t>();
                pDataStructure->duration = DiscordCoreAPI::convertMsToDurationString(durationNew);
            }

            if (jsonObjectData.contains("permalink_url") && !jsonObjectData["permalink_url"].is_null()) {
                pDataStructure->viewURL = to_string(to_hstring(jsonObjectData["permalink_url"].get<string>()));
            }

        }

    };
};