// DataParsingFunctions.hpp - Header for the functions that will parse the Discord data into C++ types.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATA_PARSING_FUNCTIONS_
#define _DATA_PARSING_FUNCTIONS_

#include "../pch.h"

namespace DiscordCoreInternal {

    void parseObject(json jsonObjectData, DiscordCoreAPI::OverWriteData* pDataStructure) {
        DiscordCoreAPI::OverWriteData overWriteData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            overWriteData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            overWriteData.type = jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("allow") && !jsonObjectData.at("allow").is_null()) {
            overWriteData.allow = jsonObjectData.at("allow").get<string>();
        }

        if (jsonObjectData.contains("deny") && !jsonObjectData.at("deny").is_null()) {
            overWriteData.deny = jsonObjectData.at("deny").get<string>();
        }

        *pDataStructure = overWriteData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::YouTubeSearchResult* pDataStructure) {
        DiscordCoreAPI::YouTubeSearchResult partialSearchResult = *pDataStructure;

        if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null()) {
            if (jsonObjectData.at("title").contains("runs")) {
                partialSearchResult.videoTitle = jsonObjectData.at("title").at("runs").at(0).at("text").get<string>();
            }
            else {
                partialSearchResult.videoTitle = jsonObjectData.at("title").at("simpleText").get<string>();
            }

        }

        if (jsonObjectData.contains("videoId") && !jsonObjectData.at("videoId").is_null()) {
            partialSearchResult.videoId = jsonObjectData.at("videoId").get<string>();
        }

        if (jsonObjectData.contains("videoId") && !jsonObjectData.at("videoId").is_null()) {
            partialSearchResult.videoURL = "https://www.youtube.com/watch?v=" + jsonObjectData.at("videoId").get<string>();
        }

        if (jsonObjectData.contains("thumbnail") && !jsonObjectData.at("thumbnail").is_null()) {
            partialSearchResult.thumbNailURL = jsonObjectData.at("thumbnail").at("thumbnails").at(0).at("url").get<string>();
        }

        if (jsonObjectData.contains("lengthText") && !jsonObjectData.at("lengthText").is_null()) {
            partialSearchResult.duration = jsonObjectData.at("lengthText").at("accessibility").at("accessibilityData").at("label").get<string>();
        }

        if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData.at("detailedMetadataSnippets").is_null()) {
            for (auto value : jsonObjectData.at("detailedMetadataSnippets").at(0).at("snippetText").at("runs")) {
                partialSearchResult.description += value.at("text").get<string>();
            }
        }

        *pDataStructure = partialSearchResult;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::UserData* pDataStructure) {
        DiscordCoreAPI::UserData userData = *pDataStructure;

        if (jsonObjectData.contains("username") && !jsonObjectData.at("username").is_null()) {
            userData.username = jsonObjectData.at("username").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            userData.id = jsonObjectData.at("id").get<string>();
            userData.createdAt = DiscordCoreAPI::convertSnowFlakeToDateTimeString(jsonObjectData.at("id").get<string>());
        }

        if (jsonObjectData.contains("discriminator") && !jsonObjectData.at("discriminator").is_null()) {
            userData.discriminator = jsonObjectData.at("discriminator").get<string>();
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData.at("avatar").is_null()) {
            string avatarString = "https://cdn.discordapp.com/avatars/" + userData.id + "/" + jsonObjectData.at("avatar").get<string>();
            userData.avatar = avatarString;
        }

        if (jsonObjectData.contains("bot") && !jsonObjectData.at("bot").is_null()) {
            userData.bot = jsonObjectData.at("bot").get<bool>();
        }

        if (jsonObjectData.contains("system") && !jsonObjectData.at("system").is_null()) {
            userData.system = jsonObjectData.at("system").get<bool>();
        }

        if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData.at("mfa_enabled").is_null()) {
            userData.mfaEnabled = jsonObjectData.at("mfa_enabled").get<bool>();
        }

        if (jsonObjectData.contains("locale") && !jsonObjectData.at("locale").is_null()) {
            userData.locale = jsonObjectData.at("locale").get<string>();
        }

        if (jsonObjectData.contains("verified") && !jsonObjectData.at("verified").is_null()) {
            userData.verified = jsonObjectData.at("verified").get<bool>();
        }

        if (jsonObjectData.contains("email") && !jsonObjectData.at("email").is_null()) {
            userData.email = jsonObjectData.at("email").get<string>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            userData.flags = jsonObjectData.at("flags").get<int>();
        }

        if (jsonObjectData.contains("premium_type") && !jsonObjectData.at("premium_type").is_null()) {
            userData.premiumType = jsonObjectData.at("premium_type").get<int>();
        }

        if (jsonObjectData.contains("public_flags") && !jsonObjectData.at("public_flags").is_null()) {
            userData.publicFlags = jsonObjectData.at("public_flags").get<int>();
        }

        *pDataStructure = userData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMetadataData* pDataStructure) {
        DiscordCoreAPI::ThreadMetadataData threadMetadata = *pDataStructure;

        if (jsonObjectData.contains("archived") && !jsonObjectData.at("archived").is_null()) {
            threadMetadata.archived = jsonObjectData.at("archived").get<bool>();
        }

        if (jsonObjectData.contains("archiver_id") && !jsonObjectData.at("archiver_id").is_null()) {
            threadMetadata.archiverId = jsonObjectData.at("archiver_id").get<string>();
        }

        if (jsonObjectData.contains("auto_archive_duration") && !jsonObjectData.at("auto_archive_duration").is_null()) {
            threadMetadata.autoArchiveDuration = jsonObjectData.at("auto_archive_duration").get<int>();
        }

        if (jsonObjectData.contains("archive_timestamp") && !jsonObjectData.at("archive_timestamp").is_null()) {
            threadMetadata.archiveTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("archive_timestamp").get<string>());
        }

        if (jsonObjectData.contains("locked") && !jsonObjectData.at("locked").is_null()) {
            threadMetadata.locked = jsonObjectData.at("locked").get<bool>();
        }

        *pDataStructure = threadMetadata;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ThreadMember* pDataStructure) {
        DiscordCoreAPI::ThreadMember threadMember = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            threadMember.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            threadMember.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("join_timestamp") && !jsonObjectData.at("join_timestamp").is_null()) {
            threadMember.joinTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("join_timestamp").get<string>());
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            threadMember.flags = jsonObjectData.at("falgs").get<int>();
        }

        *pDataStructure = threadMember;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelData* pDataStructure) {
        DiscordCoreAPI::ChannelData channelData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            channelData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            channelData.type = (DiscordCoreAPI::ChannelType)jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            channelData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
            channelData.position = jsonObjectData.at("position").get<int>();
        }

        if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData.at("permission_overwrites").is_null()) {
            map<string, DiscordCoreAPI::OverWriteData> newMap;
            for (auto newValue : jsonObjectData.at("permission_overwrites")) {
                DiscordCoreAPI::OverWriteData newData;
                parseObject(newValue, &newData);
                newMap.insert(make_pair(newData.id, newData));
            }
            channelData.permissionOverwrites = newMap;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            channelData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData.at("topic").is_null()) {
            channelData.topic = jsonObjectData.at("topic").get<string>();
        }

        if (jsonObjectData.contains("nsfw") && !jsonObjectData.at("nsfw").is_null()) {
            channelData.nsfw = jsonObjectData.at("nsfw").get<bool>();
        }

        if (jsonObjectData.contains("last_message_id") && !jsonObjectData.at("last_message_id").is_null()) {
            channelData.lastMessageId = jsonObjectData.at("last_message_id").get<string>();
        }

        if (jsonObjectData.contains("bitrate") && !jsonObjectData.at("bitrate").is_null()) {
            channelData.bitrate = jsonObjectData.at("bitrate").get<int>();
        }

        if (jsonObjectData.contains("user_limit") && !jsonObjectData.at("user_limit").is_null()) {
            channelData.userLimit = jsonObjectData.at("user_limit").get<int>();
        }

        if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData.at("rate_limit_per_user").is_null()) {
            channelData.rateLimitPerUser = jsonObjectData.at("rate_limit_per_user").get<int>();
        }

        if (jsonObjectData.contains("recipients") && !jsonObjectData.at("recipients").is_null()) {
            vector<DiscordCoreAPI::UserData> newArray;
            for (auto newValue : jsonObjectData.at("recipients")) {
                DiscordCoreAPI::UserData newData;
                parseObject(newValue, &newData);
                newArray.push_back(newData);
            }
            channelData.recipients = newArray;
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            channelData.icon = jsonObjectData.at("icon").get<string>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
            channelData.ownerId = jsonObjectData.at("owner_id").get<string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            channelData.applicationId = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("parent_id") && !jsonObjectData.at("parent_id").is_null()) {
            channelData.parentId = jsonObjectData.at("parent_id").get<string>();
        }

        if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData.at("last_pin_timestamp").is_null()) {
            channelData.lastPinTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("last_pin_timestamp").get<string>());
        }

        if (jsonObjectData.contains("rtc_region") && !jsonObjectData.at("rtc_region").is_null()) {
            channelData.rtcRegion = jsonObjectData.at("rtc_region").get<string>();
        }

        if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData.at("video_quality_mode").is_null()) {
            channelData.videoQualityMode = jsonObjectData.at("video_quality_mode").get<int>();
        }

        if (jsonObjectData.contains("message_count") && !jsonObjectData.at("message_count").is_null()) {
            channelData.messageCount = jsonObjectData.at("message_count").get<int>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
            channelData.memberCount = jsonObjectData.at("member_count").get<int>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
            channelData.memberCount = jsonObjectData.at("member_count").get<int>();
        }

        if (jsonObjectData.contains("thread_metadata") && !jsonObjectData.at("thread_metadata").is_null()) {
            parseObject(jsonObjectData.at("thread_metadata"), &channelData.threadMetadata);
        }

        if (jsonObjectData.contains("thread_metadata") && !jsonObjectData.at("thread_metadata").is_null()) {
            parseObject(jsonObjectData.at("thread_metadata"), &channelData.member);
        }

        *pDataStructure = channelData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::RoleTagsData* pDataStructure) {
        DiscordCoreAPI::RoleTagsData roleTagsData = *pDataStructure;

        if (jsonObjectData.contains("bot_id") && !jsonObjectData.at("bot_id").is_null()) {
            roleTagsData.botId = jsonObjectData.at("bot_id").get<string>();
        }

        if (jsonObjectData.contains("integration_id") && !jsonObjectData.at("integration_id").is_null()) {
            roleTagsData.integrationId = jsonObjectData.at("integration_id").get<string>();
        }

        *pDataStructure = roleTagsData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::RoleData* pDataStructure) {
        DiscordCoreAPI::RoleData roleData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            roleData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            roleData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
            roleData.color = jsonObjectData.at("color").get<int>();
        }

        if (jsonObjectData.contains("hoist") && !jsonObjectData.at("hoist").is_null()) {
            roleData.hoist = jsonObjectData.at("hoist").get<bool>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData.at("position").is_null()) {
            roleData.position = jsonObjectData.at("position").get<int>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            roleData.permissions = jsonObjectData.at("permissions").get<string>();
        }

        if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
            roleData.managed = jsonObjectData.at("managed").get<bool>();
        }

        if (jsonObjectData.contains("mentionable") && !jsonObjectData.at("mentionable").is_null()) {
            roleData.mentionable = jsonObjectData.at("mentionable").get<bool>();
        }

        if (jsonObjectData.contains("tags") && !jsonObjectData.at("tags").is_null()) {
            parseObject(jsonObjectData.at("tags"), &roleData.tags);
        }

        *pDataStructure = roleData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::GuildMemberData* pDataStructure) {
        DiscordCoreAPI::GuildMemberData guildMemberData = *pDataStructure;

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &guildMemberData.user);
        }

        if (jsonObjectData.contains("nick") && !jsonObjectData.at("nick").is_null()) {
            guildMemberData.nick = jsonObjectData.at("nick").get<string>();
        }
        else {
            guildMemberData.nick = "";
        }

        if (guildMemberData.nick == "") {
            guildMemberData.userMention = "<@" + guildMemberData.user.id + ">";
        }
        else {
            guildMemberData.userMention = "<@!" + guildMemberData.user.id + ">";
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
            vector<string> newArray;
            for (auto newValue : jsonObjectData.at("roles")) {
                newArray.push_back(newValue.get<string>());
            }
            guildMemberData.roles = newArray;
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
            guildMemberData.joinedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("joined_at").get<string>());
        }

        if (jsonObjectData.contains("premium_since") && !jsonObjectData.at("premium_since").is_null()) {
            guildMemberData.premiumSince = jsonObjectData.at("premium_since").get<string>();
        }

        if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
            guildMemberData.deaf = jsonObjectData.at("deaf").get<bool>();
        }

        if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
            guildMemberData.mute = jsonObjectData.at("mute").get<bool>();
        }

        if (jsonObjectData.contains("pending") && !jsonObjectData.at("pending").is_null()) {
            guildMemberData.pending = jsonObjectData.at("pending").get<bool>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            guildMemberData.permissions = jsonObjectData.at("permissions").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            guildMemberData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        *pDataStructure = guildMemberData;
    }

    void parseObject(json jsonObjectData, DiscordCoreInternal::VoiceReadyPayload* pDatastructure) {
        DiscordCoreInternal::VoiceReadyPayload readyPayload;

        if (jsonObjectData.contains("ip") && !jsonObjectData.at("ip").is_null()) {
            readyPayload.ip = jsonObjectData.at("ip").get<string>();
        }

        if (jsonObjectData.contains("port") && !jsonObjectData.at("port").is_null()) {
            readyPayload.port = jsonObjectData.at("port").get<int>();
        }

        if (jsonObjectData.contains("ssrc") && !jsonObjectData.at("ssrc").is_null()) {
            readyPayload.ssrc = jsonObjectData.at("ssrc").get<int>();
        }

        if (jsonObjectData.contains("modes") && !jsonObjectData.at("modes").is_null()) {
            for (auto value : jsonObjectData.at("modes")) {
                readyPayload.modes.push_back(value.get<string>());
            };
        }

        *pDatastructure = readyPayload;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmojiData* pDataStructure) {
        DiscordCoreAPI::EmojiData emojiData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            emojiData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            emojiData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
            vector<DiscordCoreAPI::RoleData> newVector;
            for (auto newValue : jsonObjectData.at("roles")) {
                DiscordCoreAPI::RoleData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            emojiData.roles = newVector;
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &emojiData.user);
        }

        if (jsonObjectData.contains("require_colons") && !jsonObjectData.at("require_colons").is_null()) {
            emojiData.requireColons = jsonObjectData.at("require_colons").get<bool>();
        }

        if (jsonObjectData.contains("managed") && !jsonObjectData.at("managed").is_null()) {
            emojiData.managed = jsonObjectData.at("managed").get<bool>();
        }

        if (jsonObjectData.contains("animated") && !jsonObjectData.at("animated").is_null()) {
            emojiData.animated = jsonObjectData.at("animated").get<bool>();
        }

        if (jsonObjectData.contains("available") && !jsonObjectData.at("available").is_null()) {
            emojiData.available = jsonObjectData.at("available").get<bool>();
        }

        *pDataStructure = emojiData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::VoiceStateData* pDataStructure) {
        DiscordCoreAPI::VoiceStateData voiceStateData = *pDataStructure;

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            voiceStateData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            voiceStateData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            voiceStateData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            DiscordCoreAPI::GuildMemberData guildMemberData;
            parseObject(jsonObjectData.at("member"), &guildMemberData);
            voiceStateData.userId = guildMemberData.user.id;
        }

        if (jsonObjectData.contains("session_id") && !jsonObjectData.at("session_id").is_null()) {
            voiceStateData.sessionId = jsonObjectData.at("session_id").get<string>();
        }

        if (jsonObjectData.contains("deaf") && !jsonObjectData.at("deaf").is_null()) {
            voiceStateData.deaf = jsonObjectData.at("deaf").get<bool>();
        }

        if (jsonObjectData.contains("mute") && !jsonObjectData.at("mute").is_null()) {
            voiceStateData.mute = jsonObjectData.at("mute").get<bool>();
        }

        if (jsonObjectData.contains("self_deaf") && !jsonObjectData.at("self_deaf").is_null()) {
            voiceStateData.selfDeaf = jsonObjectData.at("self_deaf").get<bool>();
        }

        if (jsonObjectData.contains("self_mute") && !jsonObjectData.at("self_mute").is_null()) {
            voiceStateData.selfMute = jsonObjectData.at("self_mute").get<bool>();
        }

        if (jsonObjectData.contains("self_stream") && !jsonObjectData.at("self_stream").is_null()) {
            voiceStateData.selfStream = jsonObjectData.at("self_stream").get<bool>();
        }

        if (jsonObjectData.contains("self_video") && !jsonObjectData.at("self_video").is_null()) {
            voiceStateData.selfVideo = jsonObjectData.at("self_video").get<bool>();
        }

        if (jsonObjectData.contains("suppress") && !jsonObjectData.at("suppress").is_null()) {
            voiceStateData.suppress = jsonObjectData.at("suppress").get<bool>();
        }

        if (jsonObjectData.contains("request_to_speak_timestamp") && !jsonObjectData.at("request_to_speak_timestamp").is_null()) {
            voiceStateData.requestToSpeakTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("request_to_speak_timestamp").get<string>());
        }

        *pDataStructure = voiceStateData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::PartyData* pDataStructure) {
        DiscordCoreAPI::PartyData partyData = *pDataStructure;

        if (jsonObjectData.contains("Id") && !jsonObjectData.at("Id").is_null()) {
            partyData.id = jsonObjectData.at("Id").get<string>();
        }

        if (jsonObjectData.contains("Size") && !jsonObjectData.at("Size").is_null()) {
            partyData.size[0] = jsonObjectData.at("Size").at("CurrentSize").get<int>();
            partyData.size[1] = jsonObjectData.at("Size").at("MaxSize").get<int>();
        }

        *pDataStructure = partyData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AssetsData* pDataStructure) {
        DiscordCoreAPI::AssetsData assetData = *pDataStructure;

        if (jsonObjectData.contains("LargeImage") && !jsonObjectData.at("LargeImage").is_null()) {
            assetData.largeImage = jsonObjectData.at("LargeImage").get<string>();
        }

        if (jsonObjectData.contains("LargeText") && !jsonObjectData.at("LargeText").is_null()) {
            assetData.largeText = jsonObjectData.at("LargeText").get<string>();
        }

        if (jsonObjectData.contains("SmallImage") && !jsonObjectData.at("SmallImage").is_null()) {
            assetData.smallImage = jsonObjectData.at("SmallImage").get<string>();
        }

        if (jsonObjectData.contains("SmallText") && !jsonObjectData.at("SmallText").is_null()) {
            assetData.smallText = jsonObjectData.at("SmallText").get<string>();
        }

        *pDataStructure = assetData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::SecretsData* pDataStructure) {
        DiscordCoreAPI::SecretsData secretData = *pDataStructure;

        if (jsonObjectData.contains("Join") && !jsonObjectData.at("Join").is_null()) {
            secretData.join = jsonObjectData.at("Join").get<string>();
        }

        if (jsonObjectData.contains("Spectate") && !jsonObjectData.at("Spectate").is_null()) {
            secretData.spectate = jsonObjectData.at("Spectate").get<string>();
        }

        if (jsonObjectData.contains("Match") && !jsonObjectData.at("Match").is_null()) {
            secretData.match = jsonObjectData.at("Match").get<string>();
        }

        *pDataStructure = secretData;

    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ClientStatusData* pDataStructure) {
        DiscordCoreAPI::ClientStatusData clientStatusData = *pDataStructure;

        if (jsonObjectData.contains("desktop") && !jsonObjectData.at("desktop").is_null()) {
            clientStatusData.desktop = jsonObjectData.at("desktop").get<string>();
        }

        if (jsonObjectData.contains("mobile") && !jsonObjectData.at("mobile").is_null()) {
            clientStatusData.mobile = jsonObjectData.at("mobile").get<string>();
        }

        if (jsonObjectData.contains("web") && !jsonObjectData.at("web").is_null()) {
            clientStatusData.web = jsonObjectData.at("web").get<string>();
        }

        *pDataStructure = clientStatusData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::TimestampData* pDataStructure) {
        DiscordCoreAPI::TimestampData timestampData = *pDataStructure;

        if (jsonObjectData.contains("Start") && !jsonObjectData.at("Start").is_null()) {
            timestampData.start = jsonObjectData.at("Start").get<__int64>();
        }

        if (jsonObjectData.contains("End") && !jsonObjectData.at("End").is_null()) {
            timestampData.end = jsonObjectData.at("End").get<__int64>();
        }

        *pDataStructure = timestampData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ActivityData* pDataStructure) {
        DiscordCoreAPI::ActivityData activityData = *pDataStructure;

        if (jsonObjectData.contains("Name") && !jsonObjectData.at("Name").is_null()) {
            activityData.name = jsonObjectData.at("Name").get<string>();
        }

        if (jsonObjectData.contains("Timestamps") && !jsonObjectData.at("Timestamps").is_null()) {
            parseObject(jsonObjectData.at("Timestamps"), &activityData.timestamps);
        }

        if (jsonObjectData.contains("ApplicationId") && !jsonObjectData.at("ApplicationId").is_null()) {
            activityData.applicationId = jsonObjectData.at("ApplicationId").get<string>();
        }

        if (jsonObjectData.contains("Details") && !jsonObjectData.at("Details").is_null()) {
            activityData.details = jsonObjectData.at("Details").get<string>();
        }

        if (jsonObjectData.contains("State") && !jsonObjectData.at("State").is_null()) {
            activityData.state = jsonObjectData.at("State").get<string>();
        }

        if (jsonObjectData.contains("ActivityParty") && !jsonObjectData.at("ActivityParty").is_null()) {
            parseObject(jsonObjectData.at("ActivityParty"), &activityData.party);
        }

        if (jsonObjectData.contains("ActivityAssets") && !jsonObjectData.at("ActivityAssets").is_null()) {
            parseObject(jsonObjectData.at("ActivityAssets"), &activityData.assets);
        }

        if (jsonObjectData.contains("ActivitySecrets") && !jsonObjectData.at("ActivitySecrets").is_null()) {
            parseObject(jsonObjectData.at("ActivitySecrets"), &activityData.secrets);
        }

        if (jsonObjectData.contains("Instance") && !jsonObjectData.at("Instance").is_null()) {
            activityData.instance = jsonObjectData.at("Instance").get<bool>();
        }

        *pDataStructure = activityData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::PresenceUpdateData* pDataStructure) {
        DiscordCoreAPI::PresenceUpdateData presenceUpdateData = *pDataStructure;

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &presenceUpdateData.user);
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            presenceUpdateData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("status") && !jsonObjectData.at("status").is_null()) {
            presenceUpdateData.status = jsonObjectData.at("status").get<string>();
        }

        if (jsonObjectData.contains("activities") && !jsonObjectData.at("activities").is_null()) {
            vector<DiscordCoreAPI::ActivityData> newVector;
            for (auto newValue : jsonObjectData.at("activities")) {
                DiscordCoreAPI::ActivityData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            presenceUpdateData.activities = newVector;
        }

        if (jsonObjectData.contains("client_status") && !jsonObjectData.at("client_status").is_null()) {
            parseObject(jsonObjectData.at("client_status"), &presenceUpdateData.clientStatus);
        }

        *pDataStructure = presenceUpdateData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenChannelData* pDataStructure) {
        DiscordCoreAPI::WelcomeScreenChannelData welcomeSceenChannelData = *pDataStructure;

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            welcomeSceenChannelData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            welcomeSceenChannelData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("emoji_id") && !jsonObjectData.at("emoji_id").is_null()) {
            welcomeSceenChannelData.emojiId = jsonObjectData.at("emoji_id").get<string>();
        }

        if (jsonObjectData.contains("emoji_name") && !jsonObjectData.at("emoji_name").is_null()) {
            welcomeSceenChannelData.emojiName = jsonObjectData.at("emoji_name").get<string>();
        }

        *pDataStructure = welcomeSceenChannelData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::WelcomeScreenData* pDataStructure) {
        DiscordCoreAPI::WelcomeScreenData welcomeScreenData = *pDataStructure;

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            welcomeScreenData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("welcome_channels") && !jsonObjectData.at("welcome_channels").is_null()) {
            vector<DiscordCoreAPI::WelcomeScreenChannelData> newVector;
            for (auto newValue : jsonObjectData.at("welcome_channels")) {
                DiscordCoreAPI::WelcomeScreenChannelData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            welcomeScreenData.welcomeChannels = newVector;
        }

        *pDataStructure = welcomeScreenData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::StageInstanceData* pDataStructure) {
        DiscordCoreAPI::StageInstanceData stageInstanceData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            stageInstanceData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            stageInstanceData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            stageInstanceData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData.at("topic").is_null()) {
            stageInstanceData.topic = jsonObjectData.at("topic").get<string>();
        }

        if (jsonObjectData.contains("privacy_level") && !jsonObjectData.at("privacy_level").is_null()) {
            stageInstanceData.privacyLevel = jsonObjectData.at("privacy_level").get<int>();
        }

        if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData.at("discoverable_disabled").is_null()) {
            stageInstanceData.discoverableDisabled = jsonObjectData.at("discoverable_disabled").get<bool>();
        }

        *pDataStructure = stageInstanceData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::GuildData* pDataStructure) {
        DiscordCoreAPI::GuildData guildData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            guildData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
            guildData.afkChannelID = jsonObjectData.at("afk_channel_id").get<string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            string iconURLString = "https://cdn.discordapp.com/";
            iconURLString += "icons/" + guildData.id + "/" + jsonObjectData.at("icon").get<string>() + ".png";
            guildData.icon = iconURLString;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            guildData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("icon_hash") && !jsonObjectData.at("icon_hash").is_null()) {
            guildData.iconHash = jsonObjectData.at("icon_hash").get<string>();
        }

        if (jsonObjectData.contains("splash") && !jsonObjectData.at("splash").is_null()) {
            string iconURLString = "https://cdn.discordapp.com/";
            iconURLString += "splashes/" + guildData.id + "/" + jsonObjectData.at("splash").get<string>() + ".png";
            guildData.splash = iconURLString;
        }

        if (jsonObjectData.contains("discovery_splash") && !jsonObjectData.at("discovery_splash").is_null()) {
            string discordSplashURLString = "https://cdn.discordapp.com/";
            discordSplashURLString += "discovery-splashes/" + guildData.id + "/" + jsonObjectData.at("discovery_splash").get<string>() + ".png";
            guildData.discoverySplash = discordSplashURLString;
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
            guildData.owner = jsonObjectData.at("owner").get<bool>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
            guildData.ownerID = jsonObjectData.at("owner_id").get<string>();
        }

        if (jsonObjectData.contains("preferred_locale") && !jsonObjectData.at("preferred_locale").is_null()) {
            guildData.preferredLocale = jsonObjectData.at("preferred_locale").get<string>();
        }

        if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData.at("public_updates_channel_id").is_null()) {
            guildData.publicUpdatesChannelID = jsonObjectData.at("public_updates_channel_id").get<string>();
        }

        if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData.at("vanity_url_code").is_null()) {
            guildData.vanityURLCode = jsonObjectData.at("vanity_url_code").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            guildData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("banner") && !jsonObjectData.at("banner").is_null()) {
            string guildBannerURL = "https://cdn.discordapp.com/";
            guildBannerURL += "banners/" + guildData.id + "/" + jsonObjectData.at("banner").get<string>() + ".png";
            guildData.banner = guildBannerURL;
        }

        if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData.at("rule_Channel_id").is_null()) {
            guildData.ruleChannelID = jsonObjectData.at("rule_Channel_id").get<string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            guildData.applicationID = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData.at("joined_at").is_null()) {
            guildData.joinedAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("joined_at").get<string>());
        }

        if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData.at("widget_channel_id").is_null()) {
            guildData.widgetChannelID = jsonObjectData.at("widget_channel_id").get<string>();
        }

        if (jsonObjectData.contains("system_channel_id") && !jsonObjectData.at("system_channel_id").is_null()) {
            guildData.systemChannelID = jsonObjectData.at("system_channel_id").get<string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
            guildData.region = jsonObjectData.at("region").get<string>();
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData.at("afk_channel_id").is_null()) {
            guildData.afkChannelID = jsonObjectData.at("afk_channel_id").get<string>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData.at("owner_id").is_null()) {
            guildData.ownerID = jsonObjectData.at("owner_id").get<string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData.at("region").is_null()) {
            guildData.region = jsonObjectData.at("region").get<string>();
        }

        if (jsonObjectData.contains("features") && !jsonObjectData.at("features").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("features")) {
                newVector.push_back(newValue.get<string>());
            }
            guildData.features = newVector;
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            guildData.permissions = jsonObjectData.at("permissions").get<string>();
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
            vector<DiscordCoreAPI::RoleData> newVector;
            for (auto newValue : jsonObjectData.at("roles")) {
                DiscordCoreAPI::RoleData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            guildData.roles = newVector;
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
            guildData.owner = jsonObjectData.at("owner").get<bool>();
        }

        if (jsonObjectData.contains("afk_timeout") && !jsonObjectData.at("afk_timeout").is_null()) {
            guildData.afkTimeOut = jsonObjectData.at("afk_timeout").get<int>();
        }

        if (jsonObjectData.contains("widget_enabled") && !jsonObjectData.at("widget_enabled").is_null()) {
            guildData.widgetEnabled = jsonObjectData.at("widget_enabled").get<bool>();
        }

        if (jsonObjectData.contains("verification_level") && !jsonObjectData.at("verification_level").is_null()) {
            guildData.verificationLevel = jsonObjectData.at("verification_level").get<DiscordCoreAPI::VerificationLevel>();
        }

        if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData.at("default_message_notification_level").is_null()) {
            guildData.defaultMessageNotifications = jsonObjectData.at("default_message_notification_level").get<DiscordCoreAPI::DefaultMessageNotificationLevel >();
        }

        if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData.at("explicit_content_filter_level").is_null()) {
            guildData.explicitContentFilter = jsonObjectData.at("explicit_content_filter_level").get< DiscordCoreAPI::ExplicitContentFilterLevel>();
        }

        if (jsonObjectData.contains("emojis") && !jsonObjectData.at("emojis").is_null()) {
            vector<DiscordCoreAPI::EmojiData> newVector;
            for (auto newValue : jsonObjectData.at("emojis")) {
                DiscordCoreAPI::EmojiData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            guildData.emoji = newVector;
        }

        if (jsonObjectData.contains("mfa_level") && !jsonObjectData.at("mfa_level").is_null()) {
            guildData.mfaLevel = jsonObjectData.at("mfa_level").get< DiscordCoreAPI::MFALevel>();
        }

        if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData.at("system_channel_flags").is_null()) {
            guildData.systemChannelFlags = jsonObjectData.at("system_channel_flags").get<int>();
        }

        if (jsonObjectData.contains("large") && !jsonObjectData.at("large").is_null()) {
            guildData.large = jsonObjectData.at("large").get<bool>();
        }

        if (jsonObjectData.contains("unavailable") && !jsonObjectData.at("unavailable").is_null()) {
            guildData.unavailable = jsonObjectData.at("unavailable").get<bool>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData.at("member_count").is_null()) {
            guildData.memberCount = jsonObjectData.at("member_count").get<int>();
        }

        if (jsonObjectData.contains("voice_states") && !jsonObjectData.at("voice_states").is_null()) {
            vector<DiscordCoreAPI::VoiceStateData> newVector;
            for (auto newValue : jsonObjectData.at("voice_states")) {
                DiscordCoreAPI::VoiceStateData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            guildData.voiceStates = newVector;
        }

        if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
            vector<DiscordCoreAPI::GuildMemberData> newVector;
            for (auto newValue : jsonObjectData.at("members")) {
                DiscordCoreAPI::GuildMemberData newData;
                parseObject(newValue, &newData);
                newData.guildId = guildData.id;
                newVector.push_back(newData);
            }
            guildData.members = newVector;
        }

        if (jsonObjectData.contains("channels") && !jsonObjectData.at("channels").is_null()) {
            vector<DiscordCoreAPI::ChannelData> newVector;
            for (auto newValue : jsonObjectData.at("channels")) {
                DiscordCoreAPI::ChannelData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            guildData.channels = newVector;
        }

        if (jsonObjectData.contains("threads") && !jsonObjectData.at("threads").is_null()) {
            vector<DiscordCoreAPI::ChannelData> newVector;
            for (auto newValue : jsonObjectData.at("threads")) {
                DiscordCoreAPI::ChannelData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            guildData.threads = newVector;
        }

        if (jsonObjectData.contains("presences") && !jsonObjectData.at("presences").is_null()) {
            vector<DiscordCoreAPI::PresenceUpdateData> newVector;
            for (auto newValue : jsonObjectData.at("presences")) {
                DiscordCoreAPI::PresenceUpdateData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            guildData.presences = newVector;
        }

        if (jsonObjectData.contains("max_presences") && !jsonObjectData.at("max_presences").is_null()) {
            guildData.maxPresences = jsonObjectData.at("max_presences").get<int>();
        }

        if (jsonObjectData.contains("max_members") && !jsonObjectData.at("max_members").is_null()) {
            guildData.maxMembers = jsonObjectData.at("max_members").get<int>();
        }

        if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData.at("premium_subscription_count").is_null()) {
            guildData.premiumSubscriptionCount = jsonObjectData.at("premium_subscription_count").get<int>();
        }

        if (jsonObjectData.contains("premium_tier") && !jsonObjectData.at("premium_tier").is_null()) {
            guildData.premiumTier = jsonObjectData.at("premium_tier").get< DiscordCoreAPI::PremiumTier>();
        }

        if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData.at("max_video_channel_users").is_null()) {
            guildData.maxVideoChannelUsers = jsonObjectData.at("max_video_channel_users").get<int>();
        }

        if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData.at("approximate_member_count").is_null()) {
            guildData.approximateMemberCount = jsonObjectData.at("approximate_member_count").get<int>();
        }

        if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData.at("approximate_presence_count").is_null()) {
            guildData.approximatePresenceCount = jsonObjectData.at("approximate_presence_count").get<int>();
        }

        if (jsonObjectData.contains("welcome_screen") && !jsonObjectData.at("welcome_screen").is_null()) {
            parseObject(jsonObjectData.at("welcome_screen"), &guildData.welcomeScreen);
        }

        if (jsonObjectData.contains("nsfw_level") && !jsonObjectData.at("nsfw_level").is_null()) {
            guildData.nsfwLevel = jsonObjectData.at("nsfw_level").get<int>();
        }

        if (jsonObjectData.contains("stage_instances") && !jsonObjectData.at("stage_instances").is_null()) {
            vector<DiscordCoreAPI::StageInstanceData> newVector;
            for (auto newValue : jsonObjectData.at("stage_instances")) {
                DiscordCoreAPI::StageInstanceData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            guildData.stageInstances = newVector;
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            guildData.createdAt = DiscordCoreAPI::convertSnowFlakeToDateTimeString(jsonObjectData.at("id"));
        }

        *pDataStructure = guildData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::ChannelMentionData* pDataStructure) {
        DiscordCoreAPI::ChannelMentionData channelMentionData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            channelMentionData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            channelMentionData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            channelMentionData.type = jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            channelMentionData.name = jsonObjectData.at("name").get<string>();
        }

        *pDataStructure = channelMentionData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::AttachmentData* pDataStructure) {
        DiscordCoreAPI::AttachmentData attachmentData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            attachmentData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("filename") && !jsonObjectData.at("filename").is_null()) {
            attachmentData.filename = jsonObjectData.at("filename").get<string>();
        }

        if (jsonObjectData.contains("content_type") && !jsonObjectData.at("content_type").is_null()) {
            attachmentData.contentType = jsonObjectData.at("content_type").get<string>();
        }

        if (jsonObjectData.contains("size") && !jsonObjectData.at("size").is_null()) {
            attachmentData.size = jsonObjectData.at("size").get<int>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            attachmentData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            attachmentData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            attachmentData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            attachmentData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = attachmentData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFooterData* pDataStructure) {
        DiscordCoreAPI::EmbedFooterData footerData = *pDataStructure;

        if (jsonObjectData.contains("text") && !jsonObjectData.at("text").is_null()) {
            footerData.text = jsonObjectData.at("text").get<string>();
        }

        if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
            footerData.iconUrl = jsonObjectData.at("icon_url").get<string>();
        }

        if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
            footerData.proxyIconUrl = jsonObjectData.at("proxy_icon_url").get<string>();
        }

        *pDataStructure = footerData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedImageData* pDataStructure) {
        DiscordCoreAPI::EmbedImageData imageData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            imageData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            imageData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            imageData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            imageData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = imageData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedThumbnailData* pDataStructure) {
        DiscordCoreAPI::EmbedThumbnailData thumbnailData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            thumbnailData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            thumbnailData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            thumbnailData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            thumbnailData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = thumbnailData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedVideoData* pDataStructure) {
        DiscordCoreAPI::EmbedVideoData videoData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            videoData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData.at("proxy_url").is_null()) {
            videoData.proxyUrl = jsonObjectData.at("proxy_url").get<string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData.at("width").is_null()) {
            videoData.width = jsonObjectData.at("width").get<int>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData.at("height").is_null()) {
            videoData.height = jsonObjectData.at("height").get<int>();
        }

        *pDataStructure = videoData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedAuthorData* pDataStructure) {
        DiscordCoreAPI::EmbedAuthorData authorData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            authorData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData.at("proxy_icon_url").is_null()) {
            authorData.proxyIconUrl = jsonObjectData.at("proxy_icon_url").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            authorData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("icon_url") && !jsonObjectData.at("icon_url").is_null()) {
            authorData.iconUrl = jsonObjectData.at("icon_url").get<string>();
        }

        *pDataStructure = authorData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedProviderData* pDataStructure) {
        DiscordCoreAPI::EmbedProviderData providerData = *pDataStructure;

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            providerData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            providerData.name = jsonObjectData.at("name").get<string>();
        }

        *pDataStructure = providerData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedFieldData* pDataStructure) {
        DiscordCoreAPI::EmbedFieldData fieldData = *pDataStructure;

        if (jsonObjectData.contains("inline") && !jsonObjectData.at("inline").is_null()) {
            fieldData.Inline = jsonObjectData.at("inline").get<bool>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            fieldData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
            fieldData.value = jsonObjectData.at("value").get<string>();
        }

        *pDataStructure = fieldData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::EmbedData* pDataStructure) {
        DiscordCoreAPI::EmbedData embedData = *pDataStructure;

        if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null()) {
            embedData.title = jsonObjectData.at("title").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            embedData.type = jsonObjectData.at("type").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            embedData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            embedData.url = jsonObjectData.at("url").get<string>();
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
            embedData.timestamp = jsonObjectData.at("timestamp").get<string>();
        }

        if (jsonObjectData.contains("color") && !jsonObjectData.at("color").is_null()) {
            unsigned int colorValInt = (unsigned int)jsonObjectData.at("color").get<int>();
            stringstream stream;
            stream << setbase(16) << colorValInt;
            embedData.hexColorValue = stream.str();
        }

        if (jsonObjectData.contains("footer") && !jsonObjectData.at("footer").is_null()) {
            parseObject(jsonObjectData.at("footer"), &embedData.footer);
        }

        if (jsonObjectData.contains("image") && !jsonObjectData.at("image").is_null()) {
            parseObject(jsonObjectData.at("image"), &embedData.image);
        }

        if (jsonObjectData.contains("provider") && !jsonObjectData.at("provider").is_null()) {
            parseObject(jsonObjectData.at("provider"), &embedData.provider);
        }

        if (jsonObjectData.contains("thumbnail") && !jsonObjectData.at("thumbnail").is_null()) {
            parseObject(jsonObjectData.at("thumbnail"), &embedData.thumbnail);
        }

        if (jsonObjectData.contains("video") && !jsonObjectData.at("video").is_null()) {
            parseObject(jsonObjectData.at("video"), &embedData.video);
        }

        if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
            parseObject(jsonObjectData.at("author"), &embedData.author);
        }

        if (jsonObjectData.contains("fields") && !jsonObjectData.at("fields").is_null()) {
            vector<DiscordCoreAPI::EmbedFieldData> newVector;
            for (auto newValue : jsonObjectData.at("fields")) {
                DiscordCoreAPI::EmbedFieldData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            embedData.fields = newVector;
        }

        *pDataStructure = embedData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionData* pDataStructure) {
        DiscordCoreAPI::ReactionData reactionData = *pDataStructure;

        if (jsonObjectData.contains("count") && !jsonObjectData.at("count").is_null()) {
            reactionData.count = jsonObjectData.at("count").get<int>();
        }

        if (jsonObjectData.contains("me") && !jsonObjectData.at("me").is_null()) {
            reactionData.count = jsonObjectData.at("me").get<bool>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
            parseObject(jsonObjectData.at("emoji"), &reactionData.emoji);
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            reactionData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            reactionData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            reactionData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            reactionData.messageId = jsonObjectData.at("message_id").get<string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            parseObject(jsonObjectData.at("member"), &reactionData.member);
        }

        *pDataStructure = reactionData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageActivityData* pDataStructure) {
        DiscordCoreAPI::MessageActivityData messageActivityData = *pDataStructure;

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            messageActivityData.type = jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("party_id") && !jsonObjectData.at("party_id").is_null()) {
            messageActivityData.partyId = jsonObjectData.at("party_id").get<string>();
        }

        *pDataStructure = messageActivityData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::TeamMembersObjectData* pDataStructure) {
        DiscordCoreAPI::TeamMembersObjectData teamMemberObjectData = *pDataStructure;

        if (jsonObjectData.contains("membership_state") && !jsonObjectData.at("membership_state").is_null()) {
            teamMemberObjectData.membershipState = jsonObjectData.at("membership_state").get<int>();
        }

        if (jsonObjectData.contains("team_id") && !jsonObjectData.at("team_id").is_null()) {
            teamMemberObjectData.teamId = jsonObjectData.at("team_id").get<string>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData.at("permissions").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("permissions")) {
                newVector.push_back(newValue.get<string>());
            }
            teamMemberObjectData.permissions = newVector;
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &teamMemberObjectData.user);
        }

        *pDataStructure = teamMemberObjectData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::TeamObjectData* pDataStructure) {
        DiscordCoreAPI::TeamObjectData teamObjectData = *pDataStructure;

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            teamObjectData.icon = jsonObjectData.at("icon").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            teamObjectData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("members") && !jsonObjectData.at("members").is_null()) {
            vector<DiscordCoreAPI::TeamMembersObjectData> newVector;
            for (auto newValue : jsonObjectData.at("members")) {
                DiscordCoreAPI::TeamMembersObjectData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            teamObjectData.members = newVector;
        }

        if (jsonObjectData.contains("owner_user_id") && !jsonObjectData.at("owner_user_id").is_null()) {
            teamObjectData.ownerUserId = jsonObjectData.at("owner_user_id").get<string>();
        }

        *pDataStructure = teamObjectData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationData* pDataStructure) {
        DiscordCoreAPI::ApplicationData applicationData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            applicationData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            applicationData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData.at("icon").is_null()) {
            applicationData.icon = jsonObjectData.at("icon").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            applicationData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("rpc_origins") && !jsonObjectData.at("rpc_origins").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("rpc_origins")) {
                newVector.push_back(newValue.get<string>());
            }
            applicationData.rpcOrigins = newVector;
        }

        if (jsonObjectData.contains("bot_public") && !jsonObjectData.at("bot_public").is_null()) {
            applicationData.botPublic = jsonObjectData.at("bot_public").get<bool>();
        }

        if (jsonObjectData.contains("bot_require_code_grant") && !jsonObjectData.at("bot_require_code_grant").is_null()) {
            applicationData.botRequireCodeGrant = jsonObjectData.at("bot_require_code_grant").get<bool>();
        }

        if (jsonObjectData.contains("terms_of_service_url") && !jsonObjectData.at("terms_of_service_url").is_null()) {
            applicationData.termsOfServiceUrl = jsonObjectData.at("terms_of_service_url").get<string>();
        }

        if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData.at("privacy_policy_url").is_null()) {
            applicationData.privacyPolicyUrl = jsonObjectData.at("privacy_policy_url").get<string>();
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData.at("owner").is_null()) {
            parseObject(jsonObjectData.at("owner"), &applicationData.owner);
        }

        if (jsonObjectData.contains("summary") && !jsonObjectData.at("summary").is_null()) {
            applicationData.summary = jsonObjectData.at("summary").get<string>();
        }

        if (jsonObjectData.contains("verify_key") && !jsonObjectData.at("verify_key").is_null()) {
            applicationData.verifyKey = jsonObjectData.at("verify_key").get<string>();
        }

        if (jsonObjectData.contains("team") && !jsonObjectData.at("team").is_null()) {
            parseObject(jsonObjectData.at("team"), &applicationData.team);
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            applicationData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData.at("primary_sku_id").is_null()) {
            applicationData.primarySkuId = jsonObjectData.at("primary_sku_id").get<string>();
        }

        if (jsonObjectData.contains("slug") && !jsonObjectData.at("slug").is_null()) {
            applicationData.slug = jsonObjectData.at("slug").get<string>();
        }

        if (jsonObjectData.contains("cover_image") && !jsonObjectData.at("cover_image").is_null()) {
            applicationData.coverImage = jsonObjectData.at("cover_image").get<string>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            applicationData.coverImage = jsonObjectData.at("flags").get<string>();
        }

        *pDataStructure = applicationData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InviteData* pDataStructure) {
        DiscordCoreAPI::InviteData inviteData = *pDataStructure;
        
        if (jsonObjectData.contains("code") && !jsonObjectData.at("code").is_null()&&jsonObjectData.at("code").type() == nlohmann::detail::value_t::string) {
            inviteData.code = jsonObjectData.at("code").get<string>();
        }
        else if (jsonObjectData.contains("code") && !jsonObjectData.at("code").is_null()) {
            inviteData.code = to_string(jsonObjectData.at("code").get<int>());
        }
       
        if (jsonObjectData.contains("guild") && !jsonObjectData.at("guild").is_null()) {
            parseObject(jsonObjectData.at("guild"), &inviteData.guild);
        }

        if (jsonObjectData.contains("channel") && !jsonObjectData.at("channel").is_null()) {
            parseObject(jsonObjectData.at("channel"), &inviteData.channel);
        }
       
        if (jsonObjectData.contains("inviter") && !jsonObjectData.at("inviter").is_null()) {
            parseObject(jsonObjectData.at("inviter"), &inviteData.inviter);
        }

        if (jsonObjectData.contains("target_type") && !jsonObjectData.at("target_type").is_null()) {
            inviteData.targetType = jsonObjectData.at("target_type").get<int>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            inviteData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            inviteData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("target_user") && !jsonObjectData.at("target_user").is_null()) {
            parseObject(jsonObjectData.at("target_user"), &inviteData.targetUser);
        }

        if (jsonObjectData.contains("target_application") && !jsonObjectData.at("target_application").is_null()) {
            parseObject(jsonObjectData.at("target_application"), &inviteData.targetApplication);
        }

        if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData.at("approximate_presence_count").is_null()) {
            inviteData.approximatePresenceCount = jsonObjectData.at("approximate_presence_count").get<int>();
        }

        if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData.at("approximate_member_count").is_null()) {
            inviteData.approximateMemberCount = jsonObjectData.at("approximate_member_count").get<int>();
        }

        if (jsonObjectData.contains("expires_at") && !jsonObjectData.at("expires_at").is_null()) {
            inviteData.expiresAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("expires_at").get<string>());
        }

        if (jsonObjectData.contains("stage_instance") && !jsonObjectData.at("stage_instance").is_null()) {
            parseObject(jsonObjectData.at("stage_instance"), &inviteData.stageInstance);
        }

        if (jsonObjectData.contains("uses") && !jsonObjectData.at("uses").is_null()) {
            inviteData.uses = jsonObjectData.at("uses").get<int>();
        }

        if (jsonObjectData.contains("max_uses") && !jsonObjectData.at("max_uses").is_null()) {
            inviteData.maxUses = jsonObjectData.at("max_uses").get<int>();
        }

        if (jsonObjectData.contains("max_age") && !jsonObjectData.at("max_age").is_null()) {
            inviteData.maxAge = jsonObjectData.at("max_age").get<int>();
        }

        if (jsonObjectData.contains("temporary") && !jsonObjectData.at("temporary").is_null()) {
            inviteData.temporary = jsonObjectData.at("temporary").get<bool>();
        }

        if (jsonObjectData.contains("created_at") && !jsonObjectData.at("created_at").is_null()) {
            inviteData.createdAt = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("created_at").get<string>());
        }
        
        *pDataStructure = inviteData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageReferenceData* pDataStructure) {
        DiscordCoreAPI::MessageReferenceData messageReferenceData = *pDataStructure;

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            messageReferenceData.messageId = jsonObjectData.at("message_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            messageReferenceData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            messageReferenceData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("fail_if_not_exists") && !jsonObjectData.at("fail_if_not_exists").is_null()) {
            messageReferenceData.failIfNotExists = jsonObjectData.at("fail_if_not_exists").get<bool>();
        }

        *pDataStructure = messageReferenceData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageStickerData* pDataStructure) {
        DiscordCoreAPI::MessageStickerData messageStickerData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            messageStickerData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("pack_id") && !jsonObjectData.at("pack_id").is_null()) {
            messageStickerData.packId = jsonObjectData.at("pack_id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            messageStickerData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            messageStickerData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("tags") && !jsonObjectData.at("tags").is_null()) {
            messageStickerData.tags = jsonObjectData.at("tags").get<string>();
        }

        if (jsonObjectData.contains("asset") && !jsonObjectData.at("asset").is_null()) {
            messageStickerData.asset = jsonObjectData.at("asset").get<string>();
        }

        if (jsonObjectData.contains("format_type") && !jsonObjectData.at("format_type").is_null()) {
            messageStickerData.formatType = jsonObjectData.at("format_type").get<int>();
        }

        if (jsonObjectData.contains("available") && !jsonObjectData.at("available").is_null()) {
            messageStickerData.available = jsonObjectData.at("available").get<bool>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            messageStickerData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("sort_value") && !jsonObjectData.at("sort_value").is_null()) {
            messageStickerData.sortValue = jsonObjectData.at("sort_value").get<int>();
        }

        if (jsonObjectData.contains("sort_value") && !jsonObjectData.at("sort_value").is_null()) {
            messageStickerData.sortValue = jsonObjectData.at("sort_value").get<int>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &messageStickerData.user);
        }

        *pDataStructure = messageStickerData;
    };

    void parseObject(json jsonObjectData, DiscordCoreAPI::AllowedMentionsData* pDataStructure) {
        DiscordCoreAPI::AllowedMentionsData allowedMentionsData = *pDataStructure;

        if (jsonObjectData.contains("parse") && !jsonObjectData.at("parse").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("parse")) {
                newVector.push_back(newValue.get<string>());
            }
            allowedMentionsData.parse = newVector;
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData.at("roles").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("roles")) {
                newVector.push_back(newValue.get<string>());
            }
            allowedMentionsData.roles = newVector;
        }

        if (jsonObjectData.contains("users") && !jsonObjectData.at("users").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("users")) {
                newVector.push_back(newValue.get<string>());
            }
            allowedMentionsData.users = newVector;
        }

        if (jsonObjectData.contains("replied_user") && !jsonObjectData.at("replied_user").is_null()) {
            allowedMentionsData.repliedUser = jsonObjectData.at("replied_user").get<bool>();
        }

        *pDataStructure = allowedMentionsData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionApplicationCommandCallbackData* pDataStructure) {
        DiscordCoreAPI::InteractionApplicationCommandCallbackData interactionAppCommandCallbackData = *pDataStructure;

        if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
            interactionAppCommandCallbackData.tts = jsonObjectData.at("tts").get<bool>();
        }

        if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
            interactionAppCommandCallbackData.content = jsonObjectData.at("content").get<string>();
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
            vector<DiscordCoreAPI::EmbedData> newVector;
            for (auto newValue : jsonObjectData.at("embeds")) {
                DiscordCoreAPI::EmbedData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            interactionAppCommandCallbackData.embeds = newVector;
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            interactionAppCommandCallbackData.flags = jsonObjectData.at("flags").get<int>();
        }

        if (jsonObjectData.contains("allowed_mentions") && !jsonObjectData.at("allowed_mentions").is_null()) {
            parseObject(jsonObjectData.at("allowed_mentions"), &interactionAppCommandCallbackData.allowedMentions);
        }

        *pDataStructure = interactionAppCommandCallbackData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionResponseData* pDataStructure) {
        DiscordCoreAPI::InteractionResponseData interactionData = *pDataStructure;

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            interactionData.type = jsonObjectData.at("type").get<DiscordCoreAPI::InteractionCallbackType>();
        }

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            parseObject(jsonObjectData.at("data"), &interactionData.data);
        }

        *pDataStructure = interactionData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionAddData* pDataStructure) {
        DiscordCoreAPI::ReactionAddData reactionAddData = *pDataStructure;

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            reactionAddData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            reactionAddData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            reactionAddData.messageId = jsonObjectData.at("message_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            reactionAddData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            parseObject(jsonObjectData.at("member"), &reactionAddData.member);
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
            parseObject(jsonObjectData.at("emoji"), &reactionAddData.emoji);
        }

        *pDataStructure = reactionAddData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::SelectOptionData* pDataStructure) {
        DiscordCoreAPI::SelectOptionData newData = *pDataStructure;

        if (jsonObjectData.contains("label") && !jsonObjectData.at("label").is_null()) {
            newData.label = jsonObjectData.at("label").get<string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
            newData.value = jsonObjectData.at("value").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            newData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
            parseObject(jsonObjectData.at("emoji"), &newData.emoji);
        }

        if (jsonObjectData.contains("default") && !jsonObjectData.at("default").is_null()) {
            newData._default = jsonObjectData.at("default").get<bool>();
        }

        *pDataStructure = newData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ActionRowData* pDataStructure) {
        DiscordCoreAPI::ActionRowData actionRowData = *pDataStructure;

        if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
            vector<DiscordCoreAPI::ComponentData> newVector;
            for (auto newValue : jsonObjectData.at("components")) {
                DiscordCoreAPI::ComponentData newData;
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
                    vector<DiscordCoreAPI::SelectOptionData> newVector02;
                    for (auto value : newValue.at("options")) {
                        DiscordCoreAPI::SelectOptionData newData02;
                        parseObject(value, &newData02);
                        newVector02.push_back(newData02);
                    }
                    newData.options = newVector02;
                }

                if (newValue.contains("max_values") && !newValue.at("max_values").is_null()) {
                    newData.maxValues = newValue.at("max_values").get<int>();
                }

                if (newValue.contains("min_values") && !newValue.at("min_values").is_null()) {
                    newData.minValues = newValue.at("min_values").get<int>();
                }

                newVector.push_back(newData);
            }
            actionRowData.components = newVector;
        }
        *pDataStructure = actionRowData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageInteractionData* pDataStructure) {
        DiscordCoreAPI::MessageInteractionData messageInteractionData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            messageInteractionData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            messageInteractionData.type = jsonObjectData.at("type").get<DiscordCoreAPI::InteractionType>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            messageInteractionData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &messageInteractionData.user);
        }

        *pDataStructure = messageInteractionData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageStickerItemData* pDataStructure) {
        DiscordCoreAPI::MessageStickerItemData stickerItemData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            stickerItemData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            stickerItemData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("format_type") && !jsonObjectData.at("format_type").is_null()) {
            stickerItemData.formatType = jsonObjectData.at("format_type").get<DiscordCoreAPI::MessageStickerItemType>();
        }

        *pDataStructure = stickerItemData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageDataOld* pDataStructure) {
        DiscordCoreAPI::MessageDataOld messageData = *pDataStructure;

        if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
            messageData.content = jsonObjectData.at("content").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            messageData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            messageData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            messageData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
            parseObject(jsonObjectData.at("author"), &messageData.author);
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            parseObject(jsonObjectData.at("member"), &messageData.member);
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
            messageData.timestampRaw = jsonObjectData.at("timestamp").get<string>();
            messageData.timestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("timestamp").get<string>());
        }

        if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData.at("edited_timestamp").is_null()) {
            messageData.editedTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("edited_timestamp").get<string>());
        }

        if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
            messageData.tts = jsonObjectData.at("tts").get<bool>();
        }

        if (jsonObjectData.contains("mention_everyone") && !jsonObjectData.at("mention_everyone").is_null()) {
            messageData.mentionEveryone = jsonObjectData.at("mention_everyone").get<bool>();
        }

        if (jsonObjectData.contains("mentions") && !jsonObjectData.at("mentions").is_null()) {
            vector<DiscordCoreAPI::UserData> newVector;
            for (auto newValue : jsonObjectData.at("mentions")) {
                DiscordCoreAPI::UserData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.mentions = newVector;
        }

        if (jsonObjectData.contains("mention_roles") && !jsonObjectData.at("mention_roles").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("mention_roles")) {
                newVector.push_back(newValue.get<string>());
            }
            messageData.mentionRoles = newVector;
        }

        if (jsonObjectData.contains("mention_channels") && !jsonObjectData.at("mention_channels").is_null()) {
            vector<DiscordCoreAPI::ChannelMentionData> newVector;
            for (auto newValue : jsonObjectData.at("mention_channels")) {
                DiscordCoreAPI::ChannelMentionData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.mentionChannels = newVector;
        }

        if (jsonObjectData.contains("attachments") && !jsonObjectData.at("attachments").is_null()) {
            vector<DiscordCoreAPI::AttachmentData> newVector;
            for (auto newValue : jsonObjectData.at("attachments")) {
                DiscordCoreAPI::AttachmentData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.attachments = newVector;
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
            vector<DiscordCoreAPI::EmbedData> newVector;
            for (auto newValue : jsonObjectData.at("embeds")) {
                DiscordCoreAPI::EmbedData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.embeds = newVector;
        }

        if (jsonObjectData.contains("reactions") && !jsonObjectData.at("reactions").is_null()) {
            vector<DiscordCoreAPI::ReactionData> newVector;
            for (auto newValue : jsonObjectData.at("reactions")) {
                DiscordCoreAPI::ReactionData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.reactions = newVector;
        }

        if (jsonObjectData.contains("nonce") && !jsonObjectData.at("nonce").is_null()) {
            messageData.nonce = jsonObjectData.at("nonce").get<string>();
        }

        if (jsonObjectData.contains("pinned") && !jsonObjectData.at("pinned").is_null()) {
            messageData.pinned = jsonObjectData.at("pinned").get<bool>();
        }

        if (jsonObjectData.contains("webhook_id") && !jsonObjectData.at("webhook_id").is_null()) {
            messageData.webhookId = jsonObjectData.at("webhook_id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            messageData.type = (DiscordCoreAPI::MessageType)jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("activity") && !jsonObjectData.at("activity").is_null()) {
            parseObject(jsonObjectData.at("activity"), &messageData.activity);
        }

        if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
            parseObject(jsonObjectData.at("application"), &messageData.application);
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            messageData.applicationId = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("message_reference") && !jsonObjectData.at("message_reference").is_null()) {
            parseObject(jsonObjectData.at("message_reference"), &messageData.messageReference);
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            messageData.flags = jsonObjectData.at("flags").get<int>();
        }

        if (jsonObjectData.contains("sticker_items") && !jsonObjectData.at("sticker_items").is_null()) {
            vector<DiscordCoreAPI::MessageStickerItemData> newVector;
            for (auto newValue : jsonObjectData.at("sticker_items")) {
                DiscordCoreAPI::MessageStickerItemData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.stickerItems = newVector;
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
            vector<DiscordCoreAPI::MessageStickerData> newVector;
            for (auto newValue : jsonObjectData.at("stickers")) {
                DiscordCoreAPI::MessageStickerData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.stickers = newVector;
        }

        if (jsonObjectData.contains("interaction") && !jsonObjectData.at("interaction").is_null()) {
            parseObject(jsonObjectData.at("interaction"), &messageData.interaction);
        }

        if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
            vector<DiscordCoreAPI::ActionRowData> newVector;
            for (auto& newValue : jsonObjectData.at("components")) {
                DiscordCoreAPI::ActionRowData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.components = newVector;
        }

        if (jsonObjectData.contains("thread") && !jsonObjectData.at("thread").is_null()) {
            parseObject(jsonObjectData.at("thread"), &messageData.thread);
        }


        *pDataStructure = messageData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::MessageData* pDataStructure) {
        DiscordCoreAPI::MessageData messageData = *pDataStructure;

        if (jsonObjectData.contains("content") && !jsonObjectData.at("content").is_null()) {
            messageData.content = jsonObjectData.at("content").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            messageData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            messageData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            messageData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("author") && !jsonObjectData.at("author").is_null()) {
            parseObject(jsonObjectData.at("author"), &messageData.author);
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            parseObject(jsonObjectData.at("member"), &messageData.member);
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
            messageData.timestampRaw = jsonObjectData.at("timestamp").get<string>();
            messageData.timestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("timestamp").get<string>());
        }

        if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData.at("edited_timestamp").is_null()) {
            messageData.editedTimestamp = DiscordCoreAPI::convertTimeStampToNewOne(jsonObjectData.at("edited_timestamp").get<string>());
        }

        if (jsonObjectData.contains("tts") && !jsonObjectData.at("tts").is_null()) {
            messageData.tts = jsonObjectData.at("tts").get<bool>();
        }

        if (jsonObjectData.contains("mention_everyone") && !jsonObjectData.at("mention_everyone").is_null()) {
            messageData.mentionEveryone = jsonObjectData.at("mention_everyone").get<bool>();
        }

        if (jsonObjectData.contains("mentions") && !jsonObjectData.at("mentions").is_null()) {
            vector<DiscordCoreAPI::UserData> newVector;
            for (auto newValue : jsonObjectData.at("mentions")) {
                DiscordCoreAPI::UserData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.mentions = newVector;
        }

        if (jsonObjectData.contains("mention_roles") && !jsonObjectData.at("mention_roles").is_null()) {
            vector<string> newVector;
            for (auto newValue : jsonObjectData.at("mention_roles")) {
                newVector.push_back(newValue.get<string>());
            }
            messageData.mentionRoles = newVector;
        }

        if (jsonObjectData.contains("mention_channels") && !jsonObjectData.at("mention_channels").is_null()) {
            vector<DiscordCoreAPI::ChannelMentionData> newVector;
            for (auto newValue : jsonObjectData.at("mention_channels")) {
                DiscordCoreAPI::ChannelMentionData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.mentionChannels = newVector;
        }

        if (jsonObjectData.contains("attachments") && !jsonObjectData.at("attachments").is_null()) {
            vector<DiscordCoreAPI::AttachmentData> newVector;
            for (auto newValue : jsonObjectData.at("attachments")) {
                DiscordCoreAPI::AttachmentData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.attachments = newVector;
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData.at("embeds").is_null()) {
            vector<DiscordCoreAPI::EmbedData> newVector;
            for (auto newValue : jsonObjectData.at("embeds")) {
                DiscordCoreAPI::EmbedData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.embeds = newVector;
        }

        if (jsonObjectData.contains("reactions") && !jsonObjectData.at("reactions").is_null()) {
            vector<DiscordCoreAPI::ReactionData> newVector;
            for (auto newValue : jsonObjectData.at("reactions")) {
                DiscordCoreAPI::ReactionData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.reactions = newVector;
        }

        if (jsonObjectData.contains("nonce") && !jsonObjectData.at("nonce").is_null()) {
            messageData.nonce = jsonObjectData.at("nonce").get<string>();
        }

        if (jsonObjectData.contains("pinned") && !jsonObjectData.at("pinned").is_null()) {
            messageData.pinned = jsonObjectData.at("pinned").get<bool>();
        }

        if (jsonObjectData.contains("webhook_id") && !jsonObjectData.at("webhook_id").is_null()) {
            messageData.webhookId = jsonObjectData.at("webhook_id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            messageData.type = (DiscordCoreAPI::MessageType)jsonObjectData.at("type").get<int>();
        }

        if (jsonObjectData.contains("activity") && !jsonObjectData.at("activity").is_null()) {
            parseObject(jsonObjectData.at("activity"), &messageData.activity);
        }

        if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
            parseObject(jsonObjectData.at("application"), &messageData.application);
        }

        if (jsonObjectData.contains("message_reference") && !jsonObjectData.at("message_reference").is_null()) {
            parseObject(jsonObjectData.at("message_reference"), &messageData.messageReference);
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData.at("flags").is_null()) {
            messageData.flags = jsonObjectData.at("flags").get<int>();
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
            vector<DiscordCoreAPI::MessageStickerData> newVector;
            for (auto newValue : jsonObjectData.at("stickers")) {
                DiscordCoreAPI::MessageStickerData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.stickers = newVector;
        }

        if (jsonObjectData.contains("interaction") && !jsonObjectData.at("interaction").is_null()) {
            parseObject(jsonObjectData.at("interaction"), &messageData.interaction);
        }

        if (jsonObjectData.contains("components") && !jsonObjectData.at("components").is_null()) {
            vector<DiscordCoreAPI::ActionRowData> newVector;
            for (auto& newValue : jsonObjectData.at("components")) {
                DiscordCoreAPI::ActionRowData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            messageData.components = newVector;
        }

        if (jsonObjectData.contains("referenced_message") && !jsonObjectData.at("referenced_message").is_null()) {
            parseObject(jsonObjectData.at("referenced_message"), &messageData.referencedMessage);
        }

        if (jsonObjectData.contains("thread") && !jsonObjectData.at("thread").is_null()) {
            parseObject(jsonObjectData.at("thread"), &messageData.thread);
        }

        *pDataStructure = messageData;
    }

    void parseObject(json jsonObjectData, ApplicationCommandOptionChoiceData* pDataStructure) {
        ApplicationCommandOptionChoiceData appCommandOptionChoiceData = *pDataStructure;

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            appCommandOptionChoiceData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null() && jsonObjectData.at("value").is_string()) {
            appCommandOptionChoiceData.valueString = jsonObjectData.at("value").get<string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null() && jsonObjectData.at("value").is_number_integer()) {
            appCommandOptionChoiceData.valueInt = jsonObjectData.at("value").get<int>();
        }

        *pDataStructure = appCommandOptionChoiceData;
    }

    void parseObject(json jsonObjectData, ApplicationCommandOptionData* pDataStructure) {
        ApplicationCommandOptionData appCommandOptionData = *pDataStructure;

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            appCommandOptionData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            appCommandOptionData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("required") && !jsonObjectData.at("required").is_null()) {
            appCommandOptionData.required = jsonObjectData.at("required").get<bool>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            appCommandOptionData.type = jsonObjectData.at("type").get<ApplicationCommandOptionType>();
        }

        if (appCommandOptionData.type == ApplicationCommandOptionType::SUB_COMMAND_GROUP || appCommandOptionData.type == ApplicationCommandOptionType::SUB_COMMAND) {
            if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
                vector<ApplicationCommandOptionData> newVector;
                for (auto newValue : jsonObjectData.at("options")) {
                    ApplicationCommandOptionData appCommandOptionDataNew;
                    parseObject(newValue, &appCommandOptionDataNew);
                    newVector.push_back(appCommandOptionDataNew);
                }
                appCommandOptionData.options = newVector;
            }
        }
        else {
            if (jsonObjectData.contains("choices") && !jsonObjectData.at("choices").is_null()) {
                vector<ApplicationCommandOptionChoiceData> newVector;
                for (auto newValue : jsonObjectData.at("choices")) {
                    ApplicationCommandOptionChoiceData appCommandChoiceData;
                    parseObject(newValue, &appCommandChoiceData);
                    newVector.push_back(appCommandChoiceData);
                }
                appCommandOptionData.choices = newVector;
            }
        }

        *pDataStructure = appCommandOptionData;
    }

    void parseObject(json jsonObjectData, ApplicationCommandData* pDataStructure) {
        ApplicationCommandData appCommandData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            appCommandData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            appCommandData.applicationId = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            appCommandData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
            appCommandData.description = jsonObjectData.at("description").get<string>();
        }

        if (jsonObjectData.contains("default_permission") && !jsonObjectData.at("default_permission").is_null()) {
            appCommandData.defaultPermission = jsonObjectData.at("default_permission").get<bool>();
        }

        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            vector<ApplicationCommandOptionData> newVector;
            for (auto newValue : jsonObjectData.at("options")) {
                ApplicationCommandOptionData appCommandOptionData;
                parseObject(newValue, &appCommandOptionData);
                newVector.push_back(appCommandOptionData);
            }
            appCommandData.options = newVector;
        }

        *pDataStructure = appCommandData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionDataOption* pDataStructure) {
        DiscordCoreAPI::ApplicationCommandInteractionDataOption appCommandInteractionDataOption = *pDataStructure;

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            appCommandInteractionDataOption.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            appCommandInteractionDataOption.type = jsonObjectData.at("type").get<DiscordCoreAPI::ApplicationCommandOptionType>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData.at("value").is_null()) {
            if (jsonObjectData.at("value").is_boolean()) {
                appCommandInteractionDataOption.valueBool = jsonObjectData.at("value").get<bool>();
            }
            if (jsonObjectData.at("value").is_string()) {
                appCommandInteractionDataOption.valueString = jsonObjectData.at("value").get<string>();
            }
            if (jsonObjectData.at("value").is_number()) {
                appCommandInteractionDataOption.valueInt = jsonObjectData.at("value").get<int>();
            }
        }

        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            vector<DiscordCoreAPI::ApplicationCommandInteractionDataOption> newVector;
            for (auto newValue : jsonObjectData.at("options")) {
                DiscordCoreAPI::ApplicationCommandInteractionDataOption newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            appCommandInteractionDataOption.options = newVector;
        }

        *pDataStructure = appCommandInteractionDataOption;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionData* pDataStructure) {
        DiscordCoreAPI::ApplicationCommandInteractionData appCommandInteractionData = *pDataStructure;

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            appCommandInteractionData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            appCommandInteractionData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("custom_id") && !jsonObjectData.at("custom_id").is_null()) {
            appCommandInteractionData.customId = jsonObjectData.at("custom_id").get<string>();
        }

        if (jsonObjectData.contains("component_type") && !jsonObjectData.at("component_type").is_null()) {
            appCommandInteractionData.componentType = jsonObjectData.at("component_type").get<int>();
        }

        if (jsonObjectData.contains("resolved") && !jsonObjectData.at("resolved").is_null()) {
            auto newValue = jsonObjectData.at("resolved");
            if (newValue.contains("users") && !newValue.at("users").is_null()) {
                auto newMap = newValue.at("users").get<map<string, json>>();
                map<string, DiscordCoreAPI::UserData> newerMap;
                for (const auto& [key, newValue01] : newMap) {
                    DiscordCoreAPI::UserData newData;
                    parseObject(newValue01, &newData);
                    newerMap.insert(make_pair(key, newData));
                }
                appCommandInteractionData.resolved.users = newerMap;
            }
            if (newValue.contains("channels") && !newValue.at("channels").is_null()) {
                auto newMap = newValue.at("channels").get<map<string, json>>();
                map<string, DiscordCoreAPI::ChannelData> newerMap;
                for (const auto& [key, newValue02] : newMap) {
                    DiscordCoreAPI::ChannelData newData;
                    parseObject(newValue02, &newData);
                    newerMap.insert(make_pair(key, newData));
                }
                appCommandInteractionData.resolved.channels = newerMap;
            }
            if (newValue.contains("roles") && !newValue.at("roles").is_null()) {
                auto newMap = newValue.at("roles").get<map<string, json>>();
                map<string, DiscordCoreAPI::RoleData> newerMap;
                for (const auto& [key, newValue03] : newMap) {
                    DiscordCoreAPI::RoleData newData;
                    parseObject(newValue03, &newData);
                    newerMap.insert(make_pair(key, newData));
                }
                appCommandInteractionData.resolved.roles = newerMap;
            }
            if (newValue.contains("members") && !newValue.at("members").is_null()) {
                auto newMap = newValue.at("members").get<map<string, json>>();
                map<string, DiscordCoreAPI::GuildMemberData> newerMap;
                for (const auto& [key, newValue04] : newMap) {
                    DiscordCoreAPI::GuildMemberData newData;
                    parseObject(newValue04, &newData);
                    newerMap.insert(make_pair(key, newData));
                }
                appCommandInteractionData.resolved.members = newerMap;
            }
        }

        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            vector<DiscordCoreAPI::ApplicationCommandInteractionDataOption> newVector;
            for (auto newValue : jsonObjectData.at("options")) {
                DiscordCoreAPI::ApplicationCommandInteractionDataOption newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            };
            appCommandInteractionData.options = newVector;
        }

        *pDataStructure = appCommandInteractionData;
    }

    void parseObject(json jsonObjectData, vector<DiscordCoreAPI::YouTubeFormat>* pDataStructure) {
        vector<DiscordCoreAPI::YouTubeFormat> youtubeFormats;

        if (!jsonObjectData.is_null()) {
            if (jsonObjectData.contains("streamingData") && !jsonObjectData.at("streamingData").is_null() && jsonObjectData.at("streamingData").contains("formats") && !jsonObjectData.at("streamingData").at("formats").is_null()) {
                for (auto value : jsonObjectData.at("streamingData").at("formats")) {
                    DiscordCoreAPI::YouTubeFormat newData;
                    if (value.contains("audioQuality")) {
                        newData.audioQuality = value.at("audioQuality").get<string>();
                    }
                    if (value.contains("averageBitrate")) {
                        newData.averageBitrate = value.at("averageBitrate").get<int>();
                    }
                    if (value.contains("audioSampleRate")) {
                        newData.audioSampleRate = value.at("audioSampleRate").get<string>();
                    }
                    if (value.contains("bitrate")) {
                        newData.bitrate = value.at("bitrate").get<int>();
                    }
                    if (value.contains("contentLength")) {
                        newData.contentLength = stoll(value.at("contentLength").get<string>());
                    }
                    if (value.contains("fps")) {
                        newData.fps = value.at("fps").get<int>();
                    }
                    if (value.contains("height")) {
                        newData.height = value.at("height").get<int>();
                    }
                    if (value.contains("width")) {
                        newData.height = value.at("width").get<int>();
                    }
                    if (value.contains("aitags")) {
                        newData.aitags = value.at("aitags").get<string>();
                    }
                    if (value.contains("itag")) {
                        newData.itag = value.at("itag").get<int>();
                    }
                    if (value.contains("mimeType")) {
                        newData.mimeType = value.at("mimeType").get<string>();
                    }
                    if (value.contains("quality")) {
                        newData.quality = value.at("quality").get<string>();
                    }
                    if (value.contains("signatureCipher")) {
                        newData.signatureCipher = value.at("signatureCipher").get<string>();
                    }
                    else if (value.contains("cipher")) {
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
                    youtubeFormats.push_back(newData);
                }
            }

            if (jsonObjectData.contains("streamingData") && !jsonObjectData.at("streamingData").is_null() && jsonObjectData.at("streamingData").contains("adaptiveFormats") && !jsonObjectData.at("streamingData").at("adaptiveFormats").is_null()) {
                for (auto value : jsonObjectData.at("streamingData").at("adaptiveFormats")) {
                    DiscordCoreAPI::YouTubeFormat newData;
                    if (value.contains("audioQuality")) {
                        newData.audioQuality = value.at("audioQuality").get<string>();
                    }
                    if (value.contains("averageBitrate")) {
                        newData.averageBitrate = value.at("averageBitrate").get<int>();
                    }
                    if (value.contains("audioSampleRate")) {
                        newData.audioSampleRate = value.at("audioSampleRate").get<string>();
                    }
                    if (value.contains("bitrate")) {
                        newData.bitrate = value.at("bitrate").get<int>();
                    }
                    if (value.contains("contentLength")) {
                        newData.contentLength = stoll(value.at("contentLength").get<string>());
                    }
                    if (value.contains("fps")) {
                        newData.fps = value.at("fps").get<int>();
                    }
                    if (value.contains("height")) {
                        newData.height = value.at("height").get<int>();
                    }
                    if (value.contains("width")) {
                        newData.height = value.at("width").get<int>();
                    }
                    if (value.contains("aitags")) {
                        newData.aitags = value.at("aitags").get<string>();
                    }
                    if (value.contains("itag")) {
                        newData.itag = value.at("itag").get<int>();
                    }
                    if (value.contains("mimeType")) {
                        newData.mimeType = value.at("mimeType").get<string>();
                    }
                    if (value.contains("quality")) {
                        newData.quality = value.at("quality").get<string>();
                    }
                    if (value.contains("signatureCipher")) {
                        newData.signatureCipher = value.at("signatureCipher").get<string>();
                    }
                    else if (value.contains("cipher")) {
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
                    youtubeFormats.push_back(newData);
                }
            }
        }

        *pDataStructure = youtubeFormats;
    }

    void parseObject(json jsonObjectData, CommandData* pDataStructure) {
        CommandData commandData = *pDataStructure;
        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            for (auto newValue : jsonObjectData.at("options")) {
                if (newValue.contains("options")) {
                    parseObject(newValue, &commandData);
                }
                if (newValue.contains("value") && !newValue.at("value").is_null()) {
                    auto newValueNew = newValue.at("value");
                    if (newValueNew.is_string()) {
                        commandData.optionsArgs.push_back(newValueNew);
                    }
                    else if (newValueNew.is_number()) {
                        commandData.optionsArgs.push_back(to_string(newValueNew));
                    }
                    else if (newValueNew.is_boolean()) {
                        commandData.optionsArgs.push_back(to_string(newValueNew));
                    }

                }
            }
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            commandData.commandName = jsonObjectData.at("name").get<string>();
        }

        *pDataStructure = commandData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::InteractionData* pDataStructure) {
        DiscordCoreAPI::InteractionData interactionData = *pDataStructure;

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            parseObject(jsonObjectData.at("data"), &interactionData.data);
            if (jsonObjectData.at("data").contains("custom_id") && !jsonObjectData.at("data").at("custom_id").is_null()) {
                interactionData.customId = jsonObjectData.at("data").at("custom_id").get<string>();
            }
        }

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            interactionData.dataRaw = jsonObjectData.at("data").get<json>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            interactionData.type = jsonObjectData.at("type").get<DiscordCoreAPI::InteractionType>();
        }

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            if (jsonObjectData.at("data").contains("component_type") && !jsonObjectData.at("data").at("component_type").is_null()) {
                interactionData.componentType = jsonObjectData.at("data").at("component_type").get<DiscordCoreAPI::ComponentType>();
            }
        }

        if (jsonObjectData.contains("data") && !jsonObjectData.at("data").is_null()) {
            if (jsonObjectData.at("data").contains("values") && !jsonObjectData.at("data").at("values").is_null()) {
                for (auto value : jsonObjectData.at("data").at("values")) {
                    interactionData.values.push_back(value.get<string>());
                }
            }
        }

        if (jsonObjectData.contains("token") && !jsonObjectData.at("token").is_null()) {
            interactionData.token = jsonObjectData.at("token").get<string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            parseObject(jsonObjectData.at("member"), &interactionData.member);
            interactionData.user = interactionData.member.user;
        }
        else if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &interactionData.user);
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            interactionData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            interactionData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("message") && !jsonObjectData.at("message").is_null()) {
            parseObject(jsonObjectData.at("message"), &interactionData.message);
        }

        if (jsonObjectData.contains("version") && !jsonObjectData.at("version").is_null()) {
            interactionData.version = jsonObjectData.at("version").get<int>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            interactionData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            interactionData.applicationId = jsonObjectData.at("application_id").get<string>();

        }

        *pDataStructure = interactionData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::ReactionRemoveData* pDataStructure) {
        DiscordCoreAPI::ReactionRemoveData reactionRemoveData = *pDataStructure;

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            reactionRemoveData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            reactionRemoveData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            reactionRemoveData.messageId = jsonObjectData.at("message_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            reactionRemoveData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData.at("emoji").is_null()) {
            parseObject(jsonObjectData.at("emoji"), &reactionRemoveData.emoji);
        }

        *pDataStructure = reactionRemoveData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::WebhookData* pDataStructure) {
        DiscordCoreAPI::WebhookData webhookData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            webhookData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            webhookData.type = jsonObjectData.at("id").get<unsigned int>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            webhookData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            webhookData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            parseObject(jsonObjectData.at("user"), &webhookData.user);
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            webhookData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData.at("avatar").is_null()) {
            webhookData.avatar = jsonObjectData.at("avatar").get<string>();
        }

        if (jsonObjectData.contains("token") && !jsonObjectData.at("token").is_null()) {
            webhookData.token = jsonObjectData.at("token").get<string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData.at("application_id").is_null()) {
            webhookData.applicationId = jsonObjectData.at("application_id").get<string>();
        }

        if (jsonObjectData.contains("source_guild") && !jsonObjectData.at("source_guild").is_null()) {
            parseObject(jsonObjectData.at("source_guild"), &webhookData.sourceGuild);
        }

        if (jsonObjectData.contains("source_channel") && !jsonObjectData.at("source_channel").is_null()) {
            parseObject(jsonObjectData.at("source_channel"), &webhookData.sourceChannel);
        }

        if (jsonObjectData.contains("url") && !jsonObjectData.at("url").is_null()) {
            webhookData.url = jsonObjectData.at("url").get<string>();
        }

        *pDataStructure = webhookData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogChangeData* pDataStructure) {
        DiscordCoreAPI::AuditLogChangeData changeData = *pDataStructure;

        if (jsonObjectData.contains("new_value") && !jsonObjectData.at("new_value").is_null()) {
            changeData.newValueString = jsonObjectData.at("new_value").get<string>();
        }

        if (jsonObjectData.contains("old_value") && !jsonObjectData.at("old_value").is_null()) {
            changeData.oldValueString = jsonObjectData.at("old_value").get<string>();
        }

        if (jsonObjectData.contains("key") && !jsonObjectData.at("key").is_null()) {
            changeData.key = jsonObjectData.at("key").get<string>();
        }

        *pDataStructure = changeData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryInfoData* pDataStructure) {
        DiscordCoreAPI::AuditLogEntryInfoData entryInfoData = *pDataStructure;

        if (jsonObjectData.contains("delete_member_days") && !jsonObjectData.at("delete_member_days").is_null()) {
            entryInfoData.deleteMemberDays = jsonObjectData.at("delete_member_days").get<string>();
        }

        if (jsonObjectData.contains("members_removed") && !jsonObjectData.at("members_removed").is_null()) {
            entryInfoData.membersRemoved = jsonObjectData.at("members_removed").get<string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            entryInfoData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData.at("message_id").is_null()) {
            entryInfoData.messageId = jsonObjectData.at("message_id").get<string>();
        }

        if (jsonObjectData.contains("count") && !jsonObjectData.at("count").is_null()) {
            entryInfoData.count = jsonObjectData.at("count").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            entryInfoData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            entryInfoData.type = jsonObjectData.at("type").get<string>();
        }

        if (jsonObjectData.contains("role_name") && !jsonObjectData.at("role_name").is_null()) {
            entryInfoData.roleName = jsonObjectData.at("role_name").get<string>();
        }

        *pDataStructure = entryInfoData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogEntryData* pDataStructure) {
        DiscordCoreAPI::AuditLogEntryData entryData = *pDataStructure;

        if (jsonObjectData.contains("target_id") && !jsonObjectData.at("target_id").is_null()) {
            entryData.targetId = jsonObjectData.at("target_id").get<string>();
        }

        if (jsonObjectData.contains("changes") && !jsonObjectData.at("changes").is_null()) {
            vector<DiscordCoreAPI::AuditLogChangeData> newVector;
            for (auto newValue : jsonObjectData.at("changes")) {
                DiscordCoreAPI::AuditLogChangeData changesData;
                parseObject(newValue, &changesData);
                newVector.push_back(changesData);
            }
            entryData.changes = newVector;
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            entryData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            entryData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("action_type") && !jsonObjectData.at("action_type").is_null()) {
            entryData.actionType = jsonObjectData.at("action_type").get<DiscordCoreAPI::AuditLogEvent>();
        }

        if (jsonObjectData.contains("options") && !jsonObjectData.at("options").is_null()) {
            parseObject(jsonObjectData.at("options"), &entryData.options);
        }

        if (jsonObjectData.contains("reason") && !jsonObjectData.at("reason").is_null()) {
            entryData.reason = jsonObjectData.at("reason").get<string>();
        }

        *pDataStructure = entryData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AccountData* pDataStructure) {
        DiscordCoreAPI::AccountData accountData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            accountData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            accountData.name = jsonObjectData.at("name").get<string>();
        }

        *pDataStructure = accountData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::IntegrationData* pDataStructure) {
        DiscordCoreAPI::IntegrationData integrationData = *pDataStructure;

        if (jsonObjectData.contains("id") && !jsonObjectData.at("id").is_null()) {
            integrationData.id = jsonObjectData.at("id").get<string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData.at("name").is_null()) {
            integrationData.name = jsonObjectData.at("name").get<string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData.at("type").is_null()) {
            integrationData.type = jsonObjectData.at("type").get<string>();
        }

        if (jsonObjectData.contains("enabled") && !jsonObjectData.at("enabled").is_null()) {
            integrationData.enabled = jsonObjectData.at("enabled").get<bool>();
        }

        if (jsonObjectData.contains("syncing") && !jsonObjectData.at("syncing").is_null()) {
            integrationData.syncing = jsonObjectData.at("syncing").get<bool>();
        }

        if (jsonObjectData.contains("role_id") && !jsonObjectData.at("role_id").is_null()) {
            integrationData.roleId = jsonObjectData.at("role_id").get<string>();
        }

        if (jsonObjectData.contains("enable_emoticons") && !jsonObjectData.at("enable_emoticons").is_null()) {
            integrationData.enableEmoticons = jsonObjectData.at("enable_emoticons").get<bool>();
        }

        if (jsonObjectData.contains("expire_behavior") && !jsonObjectData.at("expire_behavior").is_null()) {
            integrationData.expireBehavior = jsonObjectData.at("expire_behavior").get<unsigned int>();
        }

        if (jsonObjectData.contains("expire_grace_period") && !jsonObjectData.at("expire_grace_period").is_null()) {
            integrationData.expireGracePeriod = jsonObjectData.at("expire_grace_period").get<unsigned int>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &integrationData.user);
        }

        if (jsonObjectData.contains("account") && !jsonObjectData.at("account").is_null()) {
            parseObject(jsonObjectData.at("account"), &integrationData.account);
        }

        if (jsonObjectData.contains("synced_at") && !jsonObjectData.at("synced_at").is_null()) {
            integrationData.syncedAt = jsonObjectData.at("synced_at").get<string>();
        }

        if (jsonObjectData.contains("subscriber_count") && !jsonObjectData.at("subscriber_count").is_null()) {
            integrationData.subscriberCount = jsonObjectData.at("subscriber_count").get<unsigned int>();
        }

        if (jsonObjectData.contains("revoked") && !jsonObjectData.at("revoked").is_null()) {
            integrationData.revoked = jsonObjectData.at("revoked").get<bool>();
        }

        if (jsonObjectData.contains("application") && !jsonObjectData.at("application").is_null()) {
            parseObject(jsonObjectData.at("application"), &integrationData.application);
        }

        *pDataStructure = integrationData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::AuditLogData* pDataStructure) {
        DiscordCoreAPI::AuditLogData auditLogData = *pDataStructure;

        if (jsonObjectData.contains("webhooks") && !jsonObjectData.at("webhooks").is_null()) {
            vector<DiscordCoreAPI::WebhookData> newVector;
            for (auto newValue : jsonObjectData.at("webhooks")) {
                DiscordCoreAPI::WebhookData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            auditLogData.webhooks = newVector;
        }

        if (jsonObjectData.contains("users") && !jsonObjectData.at("users").is_null()) {
            vector<DiscordCoreAPI::UserData> newVector;
            for (auto newValue : jsonObjectData.at("users")) {
                DiscordCoreAPI::UserData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            auditLogData.users = newVector;
        }

        if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData.at("audit_log_entries").is_null()) {
            vector<DiscordCoreAPI::AuditLogEntryData> newVector;
            for (auto newValue : jsonObjectData.at("audit_log_entries")) {
                DiscordCoreAPI::AuditLogEntryData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            auditLogData.auditLogEntries = newVector;
        }

        if (jsonObjectData.contains("integrations") && !jsonObjectData.at("integrations").is_null()) {
            vector<DiscordCoreAPI::IntegrationData> newVector;
            for (auto newValue : jsonObjectData.at("integrations")) {
                DiscordCoreAPI::IntegrationData newData;
                parseObject(newValue, &newData);
                newVector.push_back(newData);
            }
            auditLogData.integrations = newVector;
        }

        *pDataStructure = auditLogData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::TypingStartData* pDataStructure) {
        DiscordCoreAPI::TypingStartData typingStartData = *pDataStructure;

        if (jsonObjectData.contains("channel_id") && !jsonObjectData.at("channel_id").is_null()) {
            typingStartData.channelId = jsonObjectData.at("channel_id").get<string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData.at("guild_id").is_null()) {
            typingStartData.guildId = jsonObjectData.at("guild_id").get<string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData.at("member").is_null()) {
            parseObject(jsonObjectData.at("member"), &typingStartData.member);
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData.at("user_id").is_null()) {
            typingStartData.userId = jsonObjectData.at("user_id").get<string>();
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData.at("timestamp").is_null()) {
            typingStartData.timestamp = jsonObjectData.at("timestamp").get<int>();
        }

        *pDataStructure = typingStartData;
    }

    void parseObject(json jsonObjectData, DiscordCoreAPI::BanData* pDataStructure) {
        DiscordCoreAPI::BanData newData;

        if (jsonObjectData.contains("user") && !jsonObjectData.at("user").is_null()) {
            parseObject(jsonObjectData.at("user"), &newData.user);
        }

        if (jsonObjectData.contains("reason") && !jsonObjectData.at("reason").is_null()) {
            newData.reason = jsonObjectData.at("reason").get<string>();
        }

        *pDataStructure = newData;
    }
};
#endif
