/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// DataParsingFunctions.cpp - Source for the functions that will parse the Discord data into C++ types.
/// Apr 21, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <DataParsingFunctions.hpp>
#include <ApplicationCommandEntities.hpp>
#include <Http.hpp>
#include <StickerEntities.hpp>
#include <GuildEntities.hpp>
#include <GuildMemberEntities.hpp>
#include <ChannelEntities.hpp>
#include <MessageEntities.hpp>
#include <GuildScheduledEventEntities.hpp>
#include <StageInstanceEntities.hpp>
#include <RoleEntities.hpp>
#include <UserEntities.hpp>
#include <WebHookEntities.hpp>
#include <ThreadEntities.hpp>
#include <ReactionEntities.hpp>
#include <glib-2.0/glib.h>

namespace DiscordCoreInternal {

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::OverWriteData& pDataStructure) {
        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = (DiscordCoreAPI::PermissionOverwritesType)jsonObjectData["type"].get<int32_t>();
        }

        if (jsonObjectData.contains("allow") && !jsonObjectData["allow"].is_null()) {
            pDataStructure.allow = jsonObjectData["allow"].get<std::string>();
        }

        if (jsonObjectData.contains("deny") && !jsonObjectData["deny"].is_null()) {
            pDataStructure.deny = jsonObjectData["deny"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }

        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::User& pDataStructure) {
        if (jsonObjectData.contains("username") && !jsonObjectData["username"].is_null()) {
            pDataStructure.userName = jsonObjectData["username"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }
            pDataStructure.createdAt = pDataStructure.getCreatedAtTimestamp(DiscordCoreAPI::TimeFormat::LongDateTime);
        }

        if (jsonObjectData.contains("discriminator") && !jsonObjectData["discriminator"].is_null()) {
            pDataStructure.discriminator = jsonObjectData["discriminator"].get<std::string>();
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
            std::string avatarString = "https://cdn.discordapp.com/avatars/" + pDataStructure.id + "/" + jsonObjectData["avatar"].get<std::string>();
            pDataStructure.avatar = avatarString;
        }

        if (jsonObjectData.contains("bot") && !jsonObjectData["bot"].is_null()) {
            pDataStructure.bot = jsonObjectData["bot"].get<bool>();
        }

        if (jsonObjectData.contains("system") && !jsonObjectData["system"].is_null()) {
            pDataStructure.system = jsonObjectData["system"].get<bool>();
        }

        if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData["mfa_enabled"].is_null()) {
            pDataStructure.mfaEnabled = jsonObjectData["mfa_enabled"].get<bool>();
        }

        if (jsonObjectData.contains("locale") && !jsonObjectData["locale"].is_null()) {
            pDataStructure.locale = jsonObjectData["locale"].get<std::string>();
        }

        if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
            pDataStructure.verified = jsonObjectData["verified"].get<bool>();
        }

        if (jsonObjectData.contains("email") && !jsonObjectData["email"].is_null()) {
            pDataStructure.email = jsonObjectData["email"].get<std::string>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.flags = jsonObjectData["flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("premium_type") && !jsonObjectData["premium_type"].is_null()) {
            pDataStructure.premiumType = jsonObjectData["premium_type"].get<int32_t>();
        }

        if (jsonObjectData.contains("public_flags") && !jsonObjectData["public_flags"].is_null()) {
            pDataStructure.publicFlags = jsonObjectData["public_flags"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::UserData& pDataStructure) {
        if (jsonObjectData.contains("username") && !jsonObjectData["username"].is_null()) {
            pDataStructure.userName = jsonObjectData["username"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }
            pDataStructure.createdAt = pDataStructure.getCreatedAtTimestamp(DiscordCoreAPI::TimeFormat::LongDateTime);
        }

        if (jsonObjectData.contains("discriminator") && !jsonObjectData["discriminator"].is_null()) {
            pDataStructure.discriminator = jsonObjectData["discriminator"].get<std::string>();
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
            std::string avatarString = "https://cdn.discordapp.com/avatars/" + pDataStructure.id + "/" + jsonObjectData["avatar"].get<std::string>();
            pDataStructure.avatar = avatarString;
        }

        if (jsonObjectData.contains("bot") && !jsonObjectData["bot"].is_null()) {
            pDataStructure.bot = jsonObjectData["bot"].get<bool>();
        }

        if (jsonObjectData.contains("system") && !jsonObjectData["system"].is_null()) {
            pDataStructure.system = jsonObjectData["system"].get<bool>();
        }

        if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData["mfa_enabled"].is_null()) {
            pDataStructure.mfaEnabled = jsonObjectData["mfa_enabled"].get<bool>();
        }

        if (jsonObjectData.contains("locale") && !jsonObjectData["locale"].is_null()) {
            pDataStructure.locale = jsonObjectData["locale"].get<std::string>();
        }

        if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
            pDataStructure.verified = jsonObjectData["verified"].get<bool>();
        }

        if (jsonObjectData.contains("email") && !jsonObjectData["email"].is_null()) {
            pDataStructure.email = jsonObjectData["email"].get<std::string>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.flags = jsonObjectData["flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("premium_type") && !jsonObjectData["premium_type"].is_null()) {
            pDataStructure.premiumType = jsonObjectData["premium_type"].get<int32_t>();
        }

        if (jsonObjectData.contains("public_flags") && !jsonObjectData["public_flags"].is_null()) {
            pDataStructure.publicFlags = jsonObjectData["public_flags"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::UserData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::UserData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::User>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::UserData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildWidgetImageData& pDataStructure) {
        if (jsonObjectData.contains("widget_image") && !jsonObjectData["widget_image"].is_null()) {
            pDataStructure.url = jsonObjectData["widget_image"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ThreadMetadataData& pDataStructure) {
        if (jsonObjectData.contains("archived") && !jsonObjectData["archived"].is_null()) {
            pDataStructure.archived = jsonObjectData["archived"].get<bool>();
        }

        if (jsonObjectData.contains("invitable") && !jsonObjectData["invitable"].is_null()) {
            pDataStructure.invitable = jsonObjectData["invitable"].get<bool>();
        }

        if (jsonObjectData.contains("auto_archive_duration") && !jsonObjectData["auto_archive_duration"].is_null()) {
            pDataStructure.autoArchiveDuration = jsonObjectData["auto_archive_duration"].get<int32_t>();
        }

        if (jsonObjectData.contains("archive_timestamp") && !jsonObjectData["archive_timestamp"].is_null()) {
            pDataStructure.archiveTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["archive_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("locked") && !jsonObjectData["locked"].is_null()) {
            pDataStructure.locked = jsonObjectData["locked"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ThreadMemberData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
            pDataStructure.userId = jsonObjectData["user_id"].get<std::string>();
        }

        if (jsonObjectData.contains("join_timestamp") && !jsonObjectData["join_timestamp"].is_null()) {
            pDataStructure.joinTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["join_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.flags = jsonObjectData["flags"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::ThreadMemberData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::ThreadMemberData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::Thread& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = (DiscordCoreAPI::ChannelType)jsonObjectData["type"].get<int32_t>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
            pDataStructure.position = jsonObjectData["position"].get<int32_t>();
        }

        if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
            pDataStructure.permissionOverwrites.clear();
            for (auto& value : jsonObjectData["permission_overwrites"]) {
                DiscordCoreAPI::OverWriteData newData{};
                DataParser::parseObject(value, newData);
                std::string overWriteId = newData.id;
                pDataStructure.permissionOverwrites.insert_or_assign(overWriteId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
            pDataStructure.topic = jsonObjectData["topic"].get<std::string>();
        }

        if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
            pDataStructure.nsfw = jsonObjectData["nsfw"].get<bool>();
        }

        if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
            pDataStructure.lastMessageId = jsonObjectData["last_message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("bitrate") && !jsonObjectData["bitrate"].is_null()) {
            pDataStructure.bitrate = jsonObjectData["bitrate"].get<int32_t>();
        }

        if (jsonObjectData.contains("user_limit") && !jsonObjectData["user_limit"].is_null()) {
            pDataStructure.userLimit = jsonObjectData["user_limit"].get<int32_t>();
        }

        if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData["rate_limit_per_user"].is_null()) {
            pDataStructure.rateLimitPerUser = jsonObjectData["rate_limit_per_user"].get<int32_t>();
        }

        if (jsonObjectData.contains("recipients") && !jsonObjectData["recipients"].is_null()) {
            pDataStructure.recipients.clear();
            for (auto& value : jsonObjectData["recipients"]) {
                DiscordCoreAPI::UserData newData{};
                DataParser::parseObject(value, newData);
                std::string userId = newData.id;
                pDataStructure.recipients.insert_or_assign(userId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
            pDataStructure.ownerId = jsonObjectData["owner_id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
            if (jsonObjectData["parent_id"].is_string()) {
                pDataStructure.parentId = jsonObjectData["parent_id"].get<std::string>();
            }
            else {
                pDataStructure.parentId = std::to_string(jsonObjectData["parent_id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
            pDataStructure.lastPinTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["last_pin_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
            pDataStructure.rtcRegion = jsonObjectData["rtc_region"].get<std::string>();
        }

        if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData["video_quality_mode"].is_null()) {
            pDataStructure.videoQualityMode = jsonObjectData["video_quality_mode"].get<int32_t>();
        }

        if (jsonObjectData.contains("message_count") && !jsonObjectData["message_count"].is_null()) {
            pDataStructure.messageCount = jsonObjectData["message_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
            pDataStructure.memberCount = jsonObjectData["member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("thread_metadata") && !jsonObjectData["thread_metadata"].is_null()) {
            DataParser::parseObject(jsonObjectData["thread_metadata"], pDataStructure.threadMetadata);
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ChannelData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = (DiscordCoreAPI::ChannelType)jsonObjectData["type"].get<int32_t>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
            pDataStructure.position = jsonObjectData["position"].get<int32_t>();
        }

        if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
            pDataStructure.permissionOverwrites.clear();
            for (auto& value : jsonObjectData["permission_overwrites"]) {
                DiscordCoreAPI::OverWriteData newData{};
                DataParser::parseObject(value, newData);
                std::string overWriteId = newData.id;
                pDataStructure.permissionOverwrites.insert_or_assign(overWriteId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
            pDataStructure.topic = jsonObjectData["topic"].get<std::string>();
        }

        if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
            pDataStructure.nsfw = jsonObjectData["nsfw"].get<bool>();
        }

        if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
            pDataStructure.lastMessageId = jsonObjectData["last_message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("bitrate") && !jsonObjectData["bitrate"].is_null()) {
            pDataStructure.bitrate = jsonObjectData["bitrate"].get<int32_t>();
        }

        if (jsonObjectData.contains("user_limit") && !jsonObjectData["user_limit"].is_null()) {
            pDataStructure.userLimit = jsonObjectData["user_limit"].get<int32_t>();
        }

        if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData["rate_limit_per_user"].is_null()) {
            pDataStructure.rateLimitPerUser = jsonObjectData["rate_limit_per_user"].get<int32_t>();
        }

        if (jsonObjectData.contains("recipients") && !jsonObjectData["recipients"].is_null()) {
            pDataStructure.recipients.clear();
            for (auto& value : jsonObjectData["recipients"]) {
                DiscordCoreAPI::UserData newData{};
                DataParser::parseObject(value, newData);
                std::string userId = newData.id;
                pDataStructure.recipients.insert_or_assign(userId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
            pDataStructure.ownerId = jsonObjectData["owner_id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
            if (jsonObjectData["parent_id"].is_string()) {
                pDataStructure.parentId = jsonObjectData["parent_id"].get<std::string>();
            }
            else {
                pDataStructure.parentId = std::to_string(jsonObjectData["parent_id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
            pDataStructure.lastPinTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["last_pin_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
            pDataStructure.rtcRegion = jsonObjectData["rtc_region"].get<std::string>();
        }

        if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData["video_quality_mode"].is_null()) {
            pDataStructure.videoQualityMode = jsonObjectData["video_quality_mode"].get<int32_t>();
        }

        if (jsonObjectData.contains("message_count") && !jsonObjectData["message_count"].is_null()) {
            pDataStructure.messageCount = jsonObjectData["message_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
            pDataStructure.memberCount = jsonObjectData["member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("thread_metadata") && !jsonObjectData["thread_metadata"].is_null()) {
            DataParser::parseObject(jsonObjectData["thread_metadata"], pDataStructure.threadMetadata);
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }
    }
    
    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::Channel& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = (DiscordCoreAPI::ChannelType)jsonObjectData["type"].get<int32_t>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
            pDataStructure.position = jsonObjectData["position"].get<int32_t>();
        }

        if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
            pDataStructure.permissionOverwrites.clear();
            for (auto& value : jsonObjectData["permission_overwrites"]) {
                DiscordCoreAPI::OverWriteData newData{};
                DataParser::parseObject(value, newData);
                std::string overWriteId = newData.id;
                pDataStructure.permissionOverwrites.insert_or_assign(overWriteId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
            pDataStructure.topic = jsonObjectData["topic"].get<std::string>();
        }

        if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
            pDataStructure.nsfw = jsonObjectData["nsfw"].get<bool>();
        }

        if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
            pDataStructure.lastMessageId = jsonObjectData["last_message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("bitrate") && !jsonObjectData["bitrate"].is_null()) {
            pDataStructure.bitrate = jsonObjectData["bitrate"].get<int32_t>();
        }

        if (jsonObjectData.contains("user_limit") && !jsonObjectData["user_limit"].is_null()) {
            pDataStructure.userLimit = jsonObjectData["user_limit"].get<int32_t>();
        }

        if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData["rate_limit_per_user"].is_null()) {
            pDataStructure.rateLimitPerUser = jsonObjectData["rate_limit_per_user"].get<int32_t>();
        }

        if (jsonObjectData.contains("recipients") && !jsonObjectData["recipients"].is_null()) {
            pDataStructure.recipients.clear();
            for (auto& value : jsonObjectData["recipients"]) {
                DiscordCoreAPI::UserData newData{};
                DataParser::parseObject(value, newData);
                std::string userId = newData.id;
                pDataStructure.recipients.insert_or_assign(userId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
            pDataStructure.ownerId = jsonObjectData["owner_id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
            if (jsonObjectData["parent_id"].is_string()) {
                pDataStructure.parentId = jsonObjectData["parent_id"].get<std::string>();
            }
            else {
                pDataStructure.parentId = std::to_string(jsonObjectData["parent_id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
            pDataStructure.lastPinTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["last_pin_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
            pDataStructure.rtcRegion = jsonObjectData["rtc_region"].get<std::string>();
        }

        if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData["video_quality_mode"].is_null()) {
            pDataStructure.videoQualityMode = jsonObjectData["video_quality_mode"].get<int32_t>();
        }

        if (jsonObjectData.contains("message_count") && !jsonObjectData["message_count"].is_null()) {
            pDataStructure.messageCount = jsonObjectData["message_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
            pDataStructure.memberCount = jsonObjectData["member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("thread_metadata") && !jsonObjectData["thread_metadata"].is_null()) {
            DataParser::parseObject(jsonObjectData["thread_metadata"], pDataStructure.threadMetadata);
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::ChannelData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::ChannelData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::Channel>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::Channel newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::RoleTagsData& pDataStructure) {
        if (jsonObjectData.contains("bot_id") && !jsonObjectData["bot_id"].is_null()) {
            pDataStructure.botId = jsonObjectData["bot_id"].get<std::string>();
        }

        if (jsonObjectData.contains("integration_id") && !jsonObjectData["integration_id"].is_null()) {
            pDataStructure.integrationId = jsonObjectData["integration_id"].get<std::string>();
        }
    }
    
    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::Role& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("unicode_emoji") && !jsonObjectData["unicode_emoji"].is_null()) {
            pDataStructure.unicodeEmoji = jsonObjectData["unicode_emoji"].get<std::string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
            pDataStructure.color = jsonObjectData["color"].get<int32_t>();
        }

        if (jsonObjectData.contains("hoist") && !jsonObjectData["hoist"].is_null()) {
            pDataStructure.hoist = jsonObjectData["hoist"].get<bool>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
            pDataStructure.position = jsonObjectData["position"].get<int32_t>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions = jsonObjectData["permissions"].get<std::string>();
        }

        if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
            pDataStructure.managed = jsonObjectData["managed"].get<bool>();
        }

        if (jsonObjectData.contains("mentionable") && !jsonObjectData["mentionable"].is_null()) {
            pDataStructure.mentionable = jsonObjectData["mentionable"].get<bool>();
        }

        if (jsonObjectData.contains("tags") && !jsonObjectData["tags"].is_null()) {
            DataParser::parseObject(jsonObjectData["tags"], pDataStructure.tags);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::RoleData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            if (jsonObjectData["id"].is_string()) {
                pDataStructure.id = jsonObjectData["id"].get<std::string>();
            }
            else {
                pDataStructure.id = std::to_string(jsonObjectData["id"].get<int64_t>());
            }
        }

        if (jsonObjectData.contains("unicode_emoji") && !jsonObjectData["unicode_emoji"].is_null()) {
            pDataStructure.unicodeEmoji = jsonObjectData["unicode_emoji"].get<std::string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
            pDataStructure.color = jsonObjectData["color"].get<int32_t>();
        }

        if (jsonObjectData.contains("hoist") && !jsonObjectData["hoist"].is_null()) {
            pDataStructure.hoist = jsonObjectData["hoist"].get<bool>();
        }

        if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
            pDataStructure.position = jsonObjectData["position"].get<int32_t>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions = jsonObjectData["permissions"].get<std::string>();
        }

        if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
            pDataStructure.managed = jsonObjectData["managed"].get<bool>();
        }

        if (jsonObjectData.contains("mentionable") && !jsonObjectData["mentionable"].is_null()) {
            pDataStructure.mentionable = jsonObjectData["mentionable"].get<bool>();
        }

        if (jsonObjectData.contains("tags") && !jsonObjectData["tags"].is_null()) {
            DataParser::parseObject(jsonObjectData["tags"], pDataStructure.tags);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::RoleData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::RoleData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::Role>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::Role newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<std::string>& pDataStructure) {
        pDataStructure.clear();
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            pDataStructure.push_back(std::move(value));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildMember& pDataStructure) {
        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("nick") && !jsonObjectData["nick"].is_null() && jsonObjectData["nick"] != "") {
            pDataStructure.nick = jsonObjectData["nick"].get<std::string>();
        }
        else {
            pDataStructure.nick = "";
        }

        if (pDataStructure.nick == "") {
            pDataStructure.userMention = "<@" + pDataStructure.user.id + ">";
        }
        else {
            pDataStructure.userMention = "<@!" + pDataStructure.user.id + ">";
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
            DataParser::parseObject(jsonObjectData["roles"], pDataStructure.roles);
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
            pDataStructure.joinedAt = DiscordCoreAPI::TimeStamp(jsonObjectData["joined_at"].get<std::string>());
        }

        if (jsonObjectData.contains("premium_since") && !jsonObjectData["premium_since"].is_null()) {
            pDataStructure.premiumSince = jsonObjectData["premium_since"].get<std::string>();
        }

        if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
            pDataStructure.deaf = jsonObjectData["deaf"].get<bool>();
        }

        if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
            pDataStructure.mute = jsonObjectData["mute"].get<bool>();
        }

        if (jsonObjectData.contains("pending") && !jsonObjectData["pending"].is_null()) {
            pDataStructure.pending = jsonObjectData["pending"].get<bool>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions = jsonObjectData["permissions"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildMemberData& pDataStructure) {
        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("nick") && !jsonObjectData["nick"].is_null() && jsonObjectData["nick"] != "") {
            pDataStructure.nick = jsonObjectData["nick"].get<std::string>();
        }
        else {
            pDataStructure.nick = "";
        }

        if (pDataStructure.nick == "") {
            pDataStructure.userMention = "<@" + pDataStructure.user.id + ">";
        }
        else {
            pDataStructure.userMention = "<@!" + pDataStructure.user.id + ">";
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
            DataParser::parseObject(jsonObjectData["roles"], pDataStructure.roles);
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
            pDataStructure.joinedAt = DiscordCoreAPI::TimeStamp(jsonObjectData["joined_at"].get<std::string>());
        }

        if (jsonObjectData.contains("premium_since") && !jsonObjectData["premium_since"].is_null()) {
            pDataStructure.premiumSince = jsonObjectData["premium_since"].get<std::string>();
        }

        if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
            pDataStructure.deaf = jsonObjectData["deaf"].get<bool>();
        }

        if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
            pDataStructure.mute = jsonObjectData["mute"].get<bool>();
        }

        if (jsonObjectData.contains("pending") && !jsonObjectData["pending"].is_null()) {
            pDataStructure.pending = jsonObjectData["pending"].get<bool>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions = jsonObjectData["permissions"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildMemberData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildMemberData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildMember>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildMember newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmojiData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
            pDataStructure.roles.clear();
            pDataStructure.roles.reserve(jsonObjectData["roles"].size());
            for (auto& value : jsonObjectData["roles"]) {
                DiscordCoreAPI::RoleData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.roles.push_back(std::move(newData));
            }
            pDataStructure.roles.shrink_to_fit();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("require_colons") && !jsonObjectData["require_colons"].is_null()) {
            pDataStructure.requireColons = jsonObjectData["require_colons"].get<bool>();
        }

        if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
            pDataStructure.managed = jsonObjectData["managed"].get<bool>();
        }

        if (jsonObjectData.contains("animated") && !jsonObjectData["animated"].is_null()) {
            pDataStructure.animated = jsonObjectData["animated"].get<bool>();
        }

        if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
            pDataStructure.available = jsonObjectData["available"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::EmojiData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::EmojiData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildPreviewData& pDataStructure) {
        if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
            pDataStructure.approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
            pDataStructure.approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
            pDataStructure.discoverySplash = jsonObjectData["discovery_splash"].get<std::string>();
        }

        if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
            pDataStructure.emojis.clear();
            pDataStructure.emojis.reserve(jsonObjectData["emojis"].size());
            for (auto& value : jsonObjectData["emojis"]) {
                DiscordCoreAPI::EmojiData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.emojis.push_back(std::move(newData));
            }
            pDataStructure.emojis.shrink_to_fit();
        }

        if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
            pDataStructure.features.clear();
            pDataStructure.features.reserve(jsonObjectData["features"].size());
            for (auto& value : jsonObjectData["features"]) {
                pDataStructure.features.push_back(std::move(value));
            }
            pDataStructure.features.shrink_to_fit();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
            pDataStructure.splash = jsonObjectData["splash"].get<std::string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::VoiceStateData& pDataStructure) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
            pDataStructure.userId = jsonObjectData["user_id"].get<std::string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }

        if (jsonObjectData.contains("session_id") && !jsonObjectData["session_id"].is_null()) {
            pDataStructure.sessionId = jsonObjectData["session_id"].get<std::string>();
        }

        if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
            pDataStructure.deaf = jsonObjectData["deaf"].get<bool>();
        }

        if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
            pDataStructure.mute = jsonObjectData["mute"].get<bool>();
        }

        if (jsonObjectData.contains("self_deaf") && !jsonObjectData["self_deaf"].is_null()) {
            pDataStructure.selfDeaf = jsonObjectData["self_deaf"].get<bool>();
        }

        if (jsonObjectData.contains("self_mute") && !jsonObjectData["self_mute"].is_null()) {
            pDataStructure.selfMute = jsonObjectData["self_mute"].get<bool>();
        }

        if (jsonObjectData.contains("self_stream") && !jsonObjectData["self_stream"].is_null()) {
            pDataStructure.selfStream = jsonObjectData["self_stream"].get<bool>();
        }

        if (jsonObjectData.contains("self_video") && !jsonObjectData["self_video"].is_null()) {
            pDataStructure.selfVideo = jsonObjectData["self_video"].get<bool>();
        }

        if (jsonObjectData.contains("suppress") && !jsonObjectData["suppress"].is_null()) {
            pDataStructure.suppress = jsonObjectData["suppress"].get<bool>();
        }

        if (jsonObjectData.contains("request_to_speak_timestamp") && !jsonObjectData["request_to_speak_timestamp"].is_null()) {
            pDataStructure.requestToSpeakTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["request_to_speak_timestamp"].get<std::string>());
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::PartyData& pDataStructure) {
        if (jsonObjectData.contains("Id") && !jsonObjectData["Id"].is_null()) {
            pDataStructure.id = jsonObjectData["Id"].get<std::string>();
        }

        if (jsonObjectData.contains("Size") && !jsonObjectData["Size"].is_null()) {
            pDataStructure.size[0] = jsonObjectData["Size"]["CurrentSize"].get<int32_t>();
            pDataStructure.size[1] = jsonObjectData["Size"]["MaxSize"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AssetsData& pDataStructure) {
        if (jsonObjectData.contains("LargeImage") && !jsonObjectData["LargeImage"].is_null()) {
            pDataStructure.largeImage = jsonObjectData["LargeImage"].get<std::string>();
        }

        if (jsonObjectData.contains("LargeText") && !jsonObjectData["LargeText"].is_null()) {
            pDataStructure.largeText = jsonObjectData["LargeText"].get<std::string>();
        }

        if (jsonObjectData.contains("SmallImage") && !jsonObjectData["SmallImage"].is_null()) {
            pDataStructure.smallImage = jsonObjectData["SmallImage"].get<std::string>();
        }

        if (jsonObjectData.contains("SmallText") && !jsonObjectData["SmallText"].is_null()) {
            pDataStructure.smallText = jsonObjectData["SmallText"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::SecretsData& pDataStructure) {
        if (jsonObjectData.contains("Join") && !jsonObjectData["Join"].is_null()) {
            pDataStructure.join = jsonObjectData["Join"].get<std::string>();
        }

        if (jsonObjectData.contains("Spectate") && !jsonObjectData["Spectate"].is_null()) {
            pDataStructure.spectate = jsonObjectData["Spectate"].get<std::string>();
        }

        if (jsonObjectData.contains("Match") && !jsonObjectData["Match"].is_null()) {
            pDataStructure.match = jsonObjectData["Match"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ClientStatusData& pDataStructure) {
        if (jsonObjectData.contains("desktop") && !jsonObjectData["desktop"].is_null()) {
            pDataStructure.desktop = jsonObjectData["desktop"].get<std::string>();
        }

        if (jsonObjectData.contains("mobile") && !jsonObjectData["mobile"].is_null()) {
            pDataStructure.mobile = jsonObjectData["mobile"].get<std::string>();
        }

        if (jsonObjectData.contains("web") && !jsonObjectData["web"].is_null()) {
            pDataStructure.web = jsonObjectData["web"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::TimestampData& pDataStructure) {
        if (jsonObjectData.contains("Start") && !jsonObjectData["Start"].is_null()) {
            pDataStructure.start = jsonObjectData["Start"].get<int64_t>();
        }

        if (jsonObjectData.contains("End") && !jsonObjectData["End"].is_null()) {
            pDataStructure.end = jsonObjectData["End"].get<int64_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ActivityData& pDataStructure) {
        if (jsonObjectData.contains("Name") && !jsonObjectData["Name"].is_null()) {
            pDataStructure.name = jsonObjectData["Name"].get<std::string>();
        }

        if (jsonObjectData.contains("Timestamps") && !jsonObjectData["Timestamps"].is_null()) {
            DataParser::parseObject(jsonObjectData["Timestamps"], pDataStructure.timestamps);
        }

        if (jsonObjectData.contains("ApplicationId") && !jsonObjectData["ApplicationId"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["ApplicationId"].get<std::string>();
        }

        if (jsonObjectData.contains("Details") && !jsonObjectData["Details"].is_null()) {
            pDataStructure.details = jsonObjectData["Details"].get<std::string>();
        }

        if (jsonObjectData.contains("State") && !jsonObjectData["State"].is_null()) {
            pDataStructure.state = jsonObjectData["State"].get<std::string>();
        }

        if (jsonObjectData.contains("ActivityParty") && !jsonObjectData["ActivityParty"].is_null()) {
            DataParser::parseObject(jsonObjectData["ActivityParty"], pDataStructure.party);
        }

        if (jsonObjectData.contains("ActivityAssets") && !jsonObjectData["ActivityAssets"].is_null()) {
            DataParser::parseObject(jsonObjectData["ActivityAssets"], pDataStructure.assets);
        }

        if (jsonObjectData.contains("ActivitySecrets") && !jsonObjectData["ActivitySecrets"].is_null()) {
            DataParser::parseObject(jsonObjectData["ActivitySecrets"], pDataStructure.secrets);
        }

        if (jsonObjectData.contains("Instance") && !jsonObjectData["Instance"].is_null()) {
            pDataStructure.instance = jsonObjectData["Instance"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::PresenceUpdateData& pDataStructure) {
        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
            pDataStructure.status = jsonObjectData["status"].get<std::string>();
        }

        if (jsonObjectData.contains("activities") && !jsonObjectData["activities"].is_null()) {
            pDataStructure.activities.clear();
            pDataStructure.activities.reserve(jsonObjectData["activities"].size());
            for (auto& value : jsonObjectData["activities"]) {
                DiscordCoreAPI::ActivityData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.activities.push_back(std::move(newData));
            }
            pDataStructure.activities.shrink_to_fit();
        }

        if (jsonObjectData.contains("client_status") && !jsonObjectData["client_status"].is_null()) {
            DataParser::parseObject(jsonObjectData["client_status"], pDataStructure.clientStatus);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::WelcomeScreenChannelData& pDataStructure) {
        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("emoji_id") && !jsonObjectData["emoji_id"].is_null()) {
            pDataStructure.emojiId = jsonObjectData["emoji_id"].get<std::string>();
        }

        if (jsonObjectData.contains("emoji_name") && !jsonObjectData["emoji_name"].is_null()) {
            pDataStructure.emojiName = jsonObjectData["emoji_name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::WelcomeScreenData& pDataStructure) {
        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("welcome_channels") && !jsonObjectData["welcome_channels"].is_null()) {
            pDataStructure.welcomeChannels.clear();
            pDataStructure.welcomeChannels.reserve(jsonObjectData["welcome_channels"].size());
            for (auto& value : jsonObjectData["welcome_channels"]) {
                DiscordCoreAPI::WelcomeScreenChannelData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.welcomeChannels.push_back(std::move(newData));
            }
            pDataStructure.welcomeChannels.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::StageInstance& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
            pDataStructure.topic = jsonObjectData["topic"].get<std::string>();
        }

        if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
            pDataStructure.privacyLevel = jsonObjectData["privacy_level"].get<DiscordCoreAPI::StageInstancePrivacyLevel>();
        }

        if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData["discoverable_disabled"].is_null()) {
            pDataStructure.discoverableDisabled = jsonObjectData["discoverable_disabled"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::StageInstanceData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
            pDataStructure.topic = jsonObjectData["topic"].get<std::string>();
        }

        if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
            pDataStructure.privacyLevel = jsonObjectData["privacy_level"].get<DiscordCoreAPI::StageInstancePrivacyLevel>();
        }

        if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData["discoverable_disabled"].is_null()) {
            pDataStructure.discoverableDisabled = jsonObjectData["discoverable_disabled"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::Sticker& pDataStructure) {
        if (jsonObjectData.contains("asset") && !jsonObjectData["asset"].is_null()) {
            pDataStructure.asset = jsonObjectData["asset"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
            pDataStructure.formatType = jsonObjectData["format_type"].get<DiscordCoreAPI::StickerFormatType>();
        }

        if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
            pDataStructure.available = jsonObjectData["available"].get<bool>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("pack_id") && !jsonObjectData["pack_id"].is_null()) {
            pDataStructure.packId = jsonObjectData["pack_id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::StickerType>();
        }

        if (jsonObjectData.contains("sort_value") && !jsonObjectData["sort_value"].is_null()) {
            pDataStructure.sortValue = jsonObjectData["sort_value"].get<int32_t>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::StickerData& pDataStructure) {
        if (jsonObjectData.contains("asset") && !jsonObjectData["asset"].is_null()) {
            pDataStructure.asset = jsonObjectData["asset"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
            pDataStructure.formatType = jsonObjectData["format_type"].get<DiscordCoreAPI::StickerFormatType>();
        }

        if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
            pDataStructure.available = jsonObjectData["available"].get<bool>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("pack_id") && !jsonObjectData["pack_id"].is_null()) {
            pDataStructure.packId = jsonObjectData["pack_id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::StickerType>();
        }

        if (jsonObjectData.contains("sort_value") && !jsonObjectData["sort_value"].is_null()) {
            pDataStructure.sortValue = jsonObjectData["sort_value"].get<int32_t>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::StickerData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::StickerData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::Sticker>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::Sticker newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::VoiceRegionData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("custom") && !jsonObjectData["custom"].is_null()) {
            pDataStructure.custom = jsonObjectData["custom"].get<bool>();
        }

        if (jsonObjectData.contains("deprecated") && !jsonObjectData["deprecated"].is_null()) {
            pDataStructure.deprecated = jsonObjectData["deprecated"].get<bool>();
        }

        if (jsonObjectData.contains("optimal") && !jsonObjectData["optimal"].is_null()) {
            pDataStructure.optimal = jsonObjectData["optimal"].get<bool>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::VoiceRegionData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::VoiceRegionData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildPruneCountData& pDataStructure) {
        if (jsonObjectData.contains("pruned") && !jsonObjectData.at("pruned").is_null()) {
            pDataStructure.count = jsonObjectData.at("pruned").get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::Guild& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
            pDataStructure.createdAt = pDataStructure.getCreatedAtTimestamp(DiscordCoreAPI::TimeFormat::LongDateTime);
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
            pDataStructure.afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            std::string iconUrlString = "https://cdn.discordapp.com/";
            iconUrlString += "icons/" + pDataStructure.id + "/" + jsonObjectData["icon"].get<std::string>() + ".png";
            pDataStructure.icon = iconUrlString;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("icon_hash") && !jsonObjectData["icon_hash"].is_null()) {
            pDataStructure.iconHash = jsonObjectData["icon_hash"].get<std::string>();
        }

        if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
            std::string iconUrlString = "https://cdn.discordapp.com/";
            iconUrlString += "splashes/" + pDataStructure.id + "/" + jsonObjectData["splash"].get<std::string>() + ".png";
            pDataStructure.splash = iconUrlString;
        }

        if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
            std::string discordSplashUrlString = "https://cdn.discordapp.com/";
            discordSplashUrlString += "discovery-splashes/" + pDataStructure.id + "/" + jsonObjectData["discovery_splash"].get<std::string>() + ".png";
            pDataStructure.discoverySplash = discordSplashUrlString;
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
            pDataStructure.owner = jsonObjectData["owner"].get<bool>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
            pDataStructure.ownerId = jsonObjectData["owner_id"].get<std::string>();
        }

        if (jsonObjectData.contains("preferred_locale") && !jsonObjectData["preferred_locale"].is_null()) {
            pDataStructure.preferredLocale = jsonObjectData["preferred_locale"].get<std::string>();
        }

        if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData["public_updates_channel_id"].is_null()) {
            pDataStructure.publicUpdatesChannelId = jsonObjectData["public_updates_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData["vanity_url_code"].is_null()) {
            pDataStructure.vanityUrlCode = jsonObjectData["vanity_url_code"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("banner") && !jsonObjectData["banner"].is_null()) {
            std::string guildBannerUrl = "https://cdn.discordapp.com/";
            guildBannerUrl += "banners/" + pDataStructure.id + "/" + jsonObjectData["banner"].get<std::string>() + ".png";
            pDataStructure.banner = guildBannerUrl;
        }

        if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData["rule_Channel_id"].is_null()) {
            pDataStructure.rulesChannelId = jsonObjectData["rule_Channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
            pDataStructure.joinedAt = DiscordCoreAPI::TimeStamp(jsonObjectData["joined_at"].get<std::string>());
        }

        if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData["widget_channel_id"].is_null()) {
            pDataStructure.widgetChannelId = jsonObjectData["widget_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("system_channel_id") && !jsonObjectData["system_channel_id"].is_null()) {
            pDataStructure.systemChannelId = jsonObjectData["system_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
            pDataStructure.region = jsonObjectData["region"].get<std::string>();
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
            pDataStructure.afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
            pDataStructure.region = jsonObjectData["region"].get<std::string>();
        }

        if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
            DataParser::parseObject(jsonObjectData["features"], pDataStructure.features);
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions = jsonObjectData["permissions"].get<std::string>();
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
            pDataStructure.stickers.clear();
            for (auto& value : jsonObjectData["stickers"]) {
                DiscordCoreAPI::StickerData newData{};
                DataParser::parseObject(value, newData);
                std::string stickerId = newData.id;
                pDataStructure.stickers.insert_or_assign(stickerId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
            pDataStructure.roles.clear();
            for (auto& value : jsonObjectData["roles"]) {
                DiscordCoreAPI::RoleData newData{};
                DataParser::parseObject(value, newData);
                std::string roleId = newData.id;
                pDataStructure.roles.insert_or_assign(roleId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
            pDataStructure.owner = jsonObjectData["owner"].get<bool>();
        }

        if (jsonObjectData.contains("afk_timeout") && !jsonObjectData["afk_timeout"].is_null()) {
            pDataStructure.afkTimeOut = jsonObjectData["afk_timeout"].get<DiscordCoreAPI::AfkTimeOutDurations>();
        }

        if (jsonObjectData.contains("widget_enabled") && !jsonObjectData["widget_enabled"].is_null()) {
            pDataStructure.widgetEnabled = jsonObjectData["widget_enabled"].get<bool>();
        }

        if (jsonObjectData.contains("verification_level") && !jsonObjectData["verification_level"].is_null()) {
            pDataStructure.verificationLevel = jsonObjectData["verification_level"].get<DiscordCoreAPI::VerificationLevel>();
        }

        if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData["default_message_notification_level"].is_null()) {
            pDataStructure.defaultMessageNotifications = jsonObjectData["default_message_notification_level"].get<DiscordCoreAPI::DefaultMessageNotificationLevel >();
        }

        if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData["explicit_content_filter_level"].is_null()) {
            pDataStructure.explicitContentFilter = jsonObjectData["explicit_content_filter_level"].get< DiscordCoreAPI::ExplicitContentFilterLevel>();
        }

        if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
            pDataStructure.emoji.clear();
            for (auto& value : jsonObjectData["emojis"]) {
                DiscordCoreAPI::EmojiData newData{};
                DataParser::parseObject(value, newData);
                std::string emojiId = newData.name;
                pDataStructure.emoji.insert_or_assign(emojiId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("mfa_level") && !jsonObjectData["mfa_level"].is_null()) {
            pDataStructure.mfaLevel = jsonObjectData["mfa_level"].get< DiscordCoreAPI::MFALevel>();
        }

        if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData["system_channel_flags"].is_null()) {
            pDataStructure.systemChannelFlags = jsonObjectData["system_channel_flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("large") && !jsonObjectData["large"].is_null()) {
            pDataStructure.large = jsonObjectData["large"].get<bool>();
        }

        if (jsonObjectData.contains("unavailable") && !jsonObjectData["unavailable"].is_null()) {
            pDataStructure.unavailable = jsonObjectData["unavailable"].get<bool>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
            pDataStructure.memberCount = jsonObjectData["member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
            pDataStructure.voiceStates.clear();
            for (auto& value : jsonObjectData["voice_states"]) {
                DiscordCoreAPI::VoiceStateData newData{};
                DataParser::parseObject(value, newData);
                std::string userId = newData.userId;
                pDataStructure.voiceStates.insert_or_assign(userId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
            pDataStructure.members.clear();
            for (auto& value : jsonObjectData["members"]) {
                DiscordCoreAPI::GuildMemberData newData{};
                DataParser::parseObject(value, newData);
                newData.guildId = pDataStructure.id;
                std::string userId = newData.user.id;
                pDataStructure.members.insert_or_assign(userId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
            pDataStructure.channels.clear();
            for (auto& value : jsonObjectData["channels"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                newData.guildId = pDataStructure.id;
                std::string channelId = newData.id;
                pDataStructure.channels.insert_or_assign(channelId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
            pDataStructure.threads.clear();
            for (auto& value : jsonObjectData["threads"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                std::string threadId = newData.id;
                pDataStructure.threads.insert_or_assign(threadId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
            pDataStructure.presences.clear();
            for (auto& value : jsonObjectData["presences"]) {
                DiscordCoreAPI::PresenceUpdateData newData{};
                DataParser::parseObject(value, newData);
                std::string presenceId = newData.user.id;
                pDataStructure.presences.insert_or_assign(presenceId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("max_presences") && !jsonObjectData["max_presences"].is_null()) {
            pDataStructure.maxPresences = jsonObjectData["max_presences"].get<int32_t>();
        }

        if (jsonObjectData.contains("max_members") && !jsonObjectData["max_members"].is_null()) {
            pDataStructure.maxMembers = jsonObjectData["max_members"].get<int32_t>();
        }

        if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData["premium_subscription_count"].is_null()) {
            pDataStructure.premiumSubscriptionCount = jsonObjectData["premium_subscription_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("premium_tier") && !jsonObjectData["premium_tier"].is_null()) {
            pDataStructure.premiumTier = jsonObjectData["premium_tier"].get< DiscordCoreAPI::PremiumTier>();
        }

        if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData["max_video_channel_users"].is_null()) {
            pDataStructure.maxVideoChannelUsers = jsonObjectData["max_video_channel_users"].get<int32_t>();
        }

        if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
            pDataStructure.approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
            pDataStructure.approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("welcome_screen") && !jsonObjectData["welcome_screen"].is_null()) {
            DataParser::parseObject(jsonObjectData["welcome_screen"], pDataStructure.welcomeScreen);
        }

        if (jsonObjectData.contains("nsfw_level") && !jsonObjectData["nsfw_level"].is_null()) {
            pDataStructure.nsfwLevel = jsonObjectData["nsfw_level"].get<int32_t>();
        }

        if (jsonObjectData.contains("stage_instances") && !jsonObjectData["stage_instances"].is_null()) {
            pDataStructure.stageInstances.clear();
            pDataStructure.stageInstances.reserve(jsonObjectData["stage_instances"].size());
            for (auto& value : jsonObjectData["stage_instances"]) {
                DiscordCoreAPI::StageInstanceData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stageInstances.push_back(std::move(newData));
            }
            pDataStructure.stageInstances.shrink_to_fit();
        }
    };

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
            pDataStructure.createdAt = pDataStructure.getCreatedAtTimestamp(DiscordCoreAPI::TimeFormat::LongDateTime);
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
            pDataStructure.afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            std::string iconUrlString = "https://cdn.discordapp.com/";
            iconUrlString += "icons/" + pDataStructure.id + "/" + jsonObjectData["icon"].get<std::string>() + ".png";
            pDataStructure.icon = iconUrlString;
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("icon_hash") && !jsonObjectData["icon_hash"].is_null()) {
            pDataStructure.iconHash = jsonObjectData["icon_hash"].get<std::string>();
        }

        if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
            std::string iconUrlString = "https://cdn.discordapp.com/";
            iconUrlString += "splashes/" + pDataStructure.id + "/" + jsonObjectData["splash"].get<std::string>() + ".png";
            pDataStructure.splash = iconUrlString;
        }

        if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
            std::string discordSplashUrlString = "https://cdn.discordapp.com/";
            discordSplashUrlString += "discovery-splashes/" + pDataStructure.id + "/" + jsonObjectData["discovery_splash"].get<std::string>() + ".png";
            pDataStructure.discoverySplash = discordSplashUrlString;
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
            pDataStructure.owner = jsonObjectData["owner"].get<bool>();
        }

        if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
            pDataStructure.ownerId = jsonObjectData["owner_id"].get<std::string>();
        }

        if (jsonObjectData.contains("preferred_locale") && !jsonObjectData["preferred_locale"].is_null()) {
            pDataStructure.preferredLocale = jsonObjectData["preferred_locale"].get<std::string>();
        }

        if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData["public_updates_channel_id"].is_null()) {
            pDataStructure.publicUpdatesChannelId = jsonObjectData["public_updates_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData["vanity_url_code"].is_null()) {
            pDataStructure.vanityUrlCode = jsonObjectData["vanity_url_code"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("banner") && !jsonObjectData["banner"].is_null()) {
            std::string guildBannerUrl = "https://cdn.discordapp.com/";
            guildBannerUrl += "banners/" + pDataStructure.id + "/" + jsonObjectData["banner"].get<std::string>() + ".png";
            pDataStructure.banner = guildBannerUrl;
        }

        if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData["rule_Channel_id"].is_null()) {
            pDataStructure.rulesChannelId = jsonObjectData["rule_Channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
            pDataStructure.joinedAt = DiscordCoreAPI::TimeStamp(jsonObjectData["joined_at"].get<std::string>());
        }

        if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData["widget_channel_id"].is_null()) {
            pDataStructure.widgetChannelId = jsonObjectData["widget_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("system_channel_id") && !jsonObjectData["system_channel_id"].is_null()) {
            pDataStructure.systemChannelId = jsonObjectData["system_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
            pDataStructure.region = jsonObjectData["region"].get<std::string>();
        }

        if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
            pDataStructure.afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
            pDataStructure.region = jsonObjectData["region"].get<std::string>();
        }

        if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
            DataParser::parseObject(jsonObjectData["features"], pDataStructure.features);
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions = jsonObjectData["permissions"].get<std::string>();
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
            pDataStructure.stickers.clear();
            for (auto& value : jsonObjectData["stickers"]) {
                DiscordCoreAPI::StickerData newData{};
                DataParser::parseObject(value, newData);
                std::string stickerId = newData.id;
                pDataStructure.stickers.insert_or_assign(stickerId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
            pDataStructure.roles.clear();
            for (auto& value : jsonObjectData["roles"]) {
                DiscordCoreAPI::RoleData newData{};
                DataParser::parseObject(value, newData);
                std::string roleId = newData.id;
                pDataStructure.roles.insert_or_assign(roleId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
            pDataStructure.owner = jsonObjectData["owner"].get<bool>();
        }

        if (jsonObjectData.contains("afk_timeout") && !jsonObjectData["afk_timeout"].is_null()) {
            pDataStructure.afkTimeOut = jsonObjectData["afk_timeout"].get<DiscordCoreAPI::AfkTimeOutDurations>();
        }

        if (jsonObjectData.contains("widget_enabled") && !jsonObjectData["widget_enabled"].is_null()) {
            pDataStructure.widgetEnabled = jsonObjectData["widget_enabled"].get<bool>();
        }

        if (jsonObjectData.contains("verification_level") && !jsonObjectData["verification_level"].is_null()) {
            pDataStructure.verificationLevel = jsonObjectData["verification_level"].get<DiscordCoreAPI::VerificationLevel>();
        }

        if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData["default_message_notification_level"].is_null()) {
            pDataStructure.defaultMessageNotifications = jsonObjectData["default_message_notification_level"].get<DiscordCoreAPI::DefaultMessageNotificationLevel >();
        }

        if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData["explicit_content_filter_level"].is_null()) {
            pDataStructure.explicitContentFilter = jsonObjectData["explicit_content_filter_level"].get< DiscordCoreAPI::ExplicitContentFilterLevel>();
        }

        if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
            pDataStructure.emoji.clear();
            for (auto& value : jsonObjectData["emojis"]) {
                DiscordCoreAPI::EmojiData newData{};
                DataParser::parseObject(value, newData);
                std::string emojiId = newData.name;
                pDataStructure.emoji.insert_or_assign(emojiId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("mfa_level") && !jsonObjectData["mfa_level"].is_null()) {
            pDataStructure.mfaLevel = jsonObjectData["mfa_level"].get< DiscordCoreAPI::MFALevel>();
        }

        if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData["system_channel_flags"].is_null()) {
            pDataStructure.systemChannelFlags = jsonObjectData["system_channel_flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("large") && !jsonObjectData["large"].is_null()) {
            pDataStructure.large = jsonObjectData["large"].get<bool>();
        }

        if (jsonObjectData.contains("unavailable") && !jsonObjectData["unavailable"].is_null()) {
            pDataStructure.unavailable = jsonObjectData["unavailable"].get<bool>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
            pDataStructure.memberCount = jsonObjectData["member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
            pDataStructure.voiceStates.clear();
            for (auto& value : jsonObjectData["voice_states"]) {
                DiscordCoreAPI::VoiceStateData newData{};
                DataParser::parseObject(value, newData);
                std::string userId = newData.userId;
                pDataStructure.voiceStates.insert_or_assign(userId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
            pDataStructure.members.clear();
            for (auto& value : jsonObjectData["members"]) {
                DiscordCoreAPI::GuildMemberData newData{};
                DataParser::parseObject(value, newData);
                newData.guildId = pDataStructure.id;
                std::string userId = newData.user.id;
                pDataStructure.members.insert_or_assign(userId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
            pDataStructure.channels.clear();
            for (auto& value : jsonObjectData["channels"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                newData.guildId = pDataStructure.id;
                std::string channelId = newData.id;
                pDataStructure.channels.insert_or_assign(channelId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
            pDataStructure.threads.clear();
            for (auto& value : jsonObjectData["threads"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                std::string threadId = newData.id;
                pDataStructure.threads.insert_or_assign(threadId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
            pDataStructure.presences.clear();
            for (auto& value : jsonObjectData["presences"]) {
                DiscordCoreAPI::PresenceUpdateData newData{};
                DataParser::parseObject(value, newData);
                std::string presenceId = newData.user.id;
                pDataStructure.presences.insert_or_assign(presenceId, std::move(newData));
            }
        }

        if (jsonObjectData.contains("max_presences") && !jsonObjectData["max_presences"].is_null()) {
            pDataStructure.maxPresences = jsonObjectData["max_presences"].get<int32_t>();
        }

        if (jsonObjectData.contains("max_members") && !jsonObjectData["max_members"].is_null()) {
            pDataStructure.maxMembers = jsonObjectData["max_members"].get<int32_t>();
        }

        if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData["premium_subscription_count"].is_null()) {
            pDataStructure.premiumSubscriptionCount = jsonObjectData["premium_subscription_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("premium_tier") && !jsonObjectData["premium_tier"].is_null()) {
            pDataStructure.premiumTier = jsonObjectData["premium_tier"].get< DiscordCoreAPI::PremiumTier>();
        }

        if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData["max_video_channel_users"].is_null()) {
            pDataStructure.maxVideoChannelUsers = jsonObjectData["max_video_channel_users"].get<int32_t>();
        }

        if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
            pDataStructure.approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
            pDataStructure.approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("welcome_screen") && !jsonObjectData["welcome_screen"].is_null()) {
            DataParser::parseObject(jsonObjectData["welcome_screen"], pDataStructure.welcomeScreen);
        }

        if (jsonObjectData.contains("nsfw_level") && !jsonObjectData["nsfw_level"].is_null()) {
            pDataStructure.nsfwLevel = jsonObjectData["nsfw_level"].get<int32_t>();
        }

        if (jsonObjectData.contains("stage_instances") && !jsonObjectData["stage_instances"].is_null()) {
            pDataStructure.stageInstances.clear();
            pDataStructure.stageInstances.reserve(jsonObjectData["stage_instances"].size());
            for (auto& value : jsonObjectData["stage_instances"]) {
                DiscordCoreAPI::StageInstanceData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stageInstances.push_back(std::move(newData));
            }
            pDataStructure.stageInstances.shrink_to_fit();
        }
    };

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::Guild>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::Guild newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::SessionStartData& pDataStructure) {
        if (jsonObjectData.contains("max_concurrency") && !jsonObjectData["max_concurrency"].is_null()) {
            pDataStructure.maxConcurrency = jsonObjectData["max_concurrency"].get<uint32_t>();
        }

        if (jsonObjectData.contains("remaining") && !jsonObjectData["remaining"].is_null()) {
            pDataStructure.remaining = jsonObjectData["remaining"].get<uint32_t>();
        }

        if (jsonObjectData.contains("reset_after") && !jsonObjectData["reset_after"].is_null()) {
            pDataStructure.resetAfter = jsonObjectData["reset_after"].get<uint32_t>();
        }

        if (jsonObjectData.contains("total") && !jsonObjectData["total"].is_null()) {
            pDataStructure.total = jsonObjectData["total"].get<uint32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GatewayBotData& pDataStructure) {
        if (jsonObjectData.contains("session_start_limit") && !jsonObjectData["session_start_limit"].is_null()) {
            DataParser::parseObject(jsonObjectData["session_start_limit"], pDataStructure.sessionStartLimit);
        }

        if (jsonObjectData.contains("shards") && !jsonObjectData["shards"].is_null()) {
            pDataStructure.shards = jsonObjectData["shards"].get<uint32_t>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildWidgetData& pDataStructure) {
        if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
            pDataStructure.enabled = jsonObjectData["enabled"].get<bool>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }
    };

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ChannelMentionData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::ChannelType>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AttachmentData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("filename") && !jsonObjectData["filename"].is_null()) {
            pDataStructure.filename = jsonObjectData["filename"].get<std::string>();
        }

        if (jsonObjectData.contains("content_type") && !jsonObjectData["content_type"].is_null()) {
            pDataStructure.contentType = jsonObjectData["content_type"].get<std::string>();
        }

        if (jsonObjectData.contains("size") && !jsonObjectData["size"].is_null()) {
            pDataStructure.size = jsonObjectData["size"].get<int32_t>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
            pDataStructure.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
            pDataStructure.width = jsonObjectData["width"].get<int32_t>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
            pDataStructure.height = jsonObjectData["height"].get<int32_t>();
        }
    };

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedFooterData& pDataStructure) {
        if (jsonObjectData.contains("text") && !jsonObjectData["text"].is_null()) {
            pDataStructure.text = jsonObjectData["text"].get<std::string>();
        }

        if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
            pDataStructure.iconUrl = jsonObjectData["icon_url"].get<std::string>();
        }

        if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
            pDataStructure.proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedImageData& pDataStructure) {
        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
            pDataStructure.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
            pDataStructure.width = jsonObjectData["width"].get<int32_t>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
            pDataStructure.height = jsonObjectData["height"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedThumbnailData& pDataStructure) {
        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
            pDataStructure.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
            pDataStructure.width = jsonObjectData["width"].get<int32_t>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
            pDataStructure.height = jsonObjectData["height"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedVideoData& pDataStructure) {
        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }

        if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
            pDataStructure.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
        }

        if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
            pDataStructure.width = jsonObjectData["width"].get<int32_t>();
        }

        if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
            pDataStructure.height = jsonObjectData["height"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedAuthorData& pDataStructure) {
        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }

        if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
            pDataStructure.proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
            pDataStructure.iconUrl = jsonObjectData["icon_url"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedProviderData& pDataStructure) {
        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedFieldData& pDataStructure) {
        if (jsonObjectData.contains("inline") && !jsonObjectData["inline"].is_null()) {
            pDataStructure.Inline = jsonObjectData["inline"].get<bool>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
            pDataStructure.value = jsonObjectData["value"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::EmbedData& pDataStructure) {
        if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
            pDataStructure.title = jsonObjectData["title"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
            pDataStructure.timestamp = jsonObjectData["timestamp"].get<std::string>();
        }

        if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
            int32_t colorValInt = jsonObjectData["color"].get<int32_t>();
            std::stringstream stream;
            stream << std::setbase(16) << colorValInt;
            pDataStructure.hexColorValue = stream.str();
        }

        if (jsonObjectData.contains("footer") && !jsonObjectData["footer"].is_null()) {
            DataParser::parseObject(jsonObjectData["footer"], pDataStructure.footer);
        }

        if (jsonObjectData.contains("image") && !jsonObjectData["image"].is_null()) {
            DataParser::parseObject(jsonObjectData["image"], pDataStructure.image);
        }

        if (jsonObjectData.contains("provider") && !jsonObjectData["provider"].is_null()) {
            DataParser::parseObject(jsonObjectData["provider"], pDataStructure.provider);
        }

        if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
            DataParser::parseObject(jsonObjectData["thumbnail"], pDataStructure.thumbnail);
        }

        if (jsonObjectData.contains("video") && !jsonObjectData["video"].is_null()) {
            DataParser::parseObject(jsonObjectData["video"], pDataStructure.video);
        }

        if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
            DataParser::parseObject(jsonObjectData["author"], pDataStructure.author);
        }

        if (jsonObjectData.contains("fields") && !jsonObjectData["fields"].is_null()) {
            pDataStructure.fields.clear();
            pDataStructure.fields.reserve(jsonObjectData["fields"].size());
            for (auto& value : jsonObjectData["fields"]) {
                DiscordCoreAPI::EmbedFieldData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.fields.push_back(std::move(newData));
            }
            pDataStructure.fields.shrink_to_fit();
        }
    };

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::Reaction& pDataStructure) {
        if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
            pDataStructure.count = jsonObjectData["count"].get<int32_t>();
        }

        if (jsonObjectData.contains("me") && !jsonObjectData["me"].is_null()) {
            pDataStructure.count = jsonObjectData["me"].get<bool>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
            DataParser::parseObject(jsonObjectData["emoji"], pDataStructure.emoji);
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
            pDataStructure.userId = jsonObjectData["user_id"].get<std::string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
            pDataStructure.messageId = jsonObjectData["message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ReactionData& pDataStructure) {
        if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
            pDataStructure.count = jsonObjectData["count"].get<int32_t>();
        }

        if (jsonObjectData.contains("me") && !jsonObjectData["me"].is_null()) {
            pDataStructure.count = jsonObjectData["me"].get<bool>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
            DataParser::parseObject(jsonObjectData["emoji"], pDataStructure.emoji);
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
            pDataStructure.userId = jsonObjectData["user_id"].get<std::string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
            pDataStructure.messageId = jsonObjectData["message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::ReactionData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::ReactionData newData;
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::MessageActivityData& pDataStructure) {
        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::MessageActivityType>();
        }

        if (jsonObjectData.contains("party_id") && !jsonObjectData["party_id"].is_null()) {
            pDataStructure.partyId = jsonObjectData["party_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::TeamMembersObjectData& pDataStructure) {
        if (jsonObjectData.contains("membership_state") && !jsonObjectData["membership_state"].is_null()) {
            pDataStructure.membershipState = jsonObjectData["membership_state"].get<int32_t>();
        }

        if (jsonObjectData.contains("team_id") && !jsonObjectData["team_id"].is_null()) {
            pDataStructure.teamId = jsonObjectData["team_id"].get<std::string>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions.clear();
            pDataStructure.permissions.reserve(jsonObjectData["permissions"].size());
            for (auto& value : jsonObjectData["permissions"]) {
                pDataStructure.permissions.push_back(value.get<std::string>());
            }
            pDataStructure.permissions.shrink_to_fit();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::TeamObjectData& pDataStructure) {
        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
            pDataStructure.members.clear();
            pDataStructure.members.reserve(jsonObjectData["members"].size());
            for (auto& value : jsonObjectData["members"]) {
                DiscordCoreAPI::TeamMembersObjectData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.members.push_back(std::move(newData));
            }
            pDataStructure.members.shrink_to_fit();
        }

        if (jsonObjectData.contains("owner_user_id") && !jsonObjectData["owner_user_id"].is_null()) {
            pDataStructure.ownerUserId = jsonObjectData["owner_user_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
            pDataStructure.icon = jsonObjectData["icon"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }


        if (jsonObjectData.contains("rpc_origins") && !jsonObjectData["rpc_origins"].is_null()) {
            pDataStructure.rpcOrigins.clear();
            pDataStructure.rpcOrigins.reserve(jsonObjectData["rpc_origins"].size());
            for (auto& value : jsonObjectData["rpc_origins"]) {
                pDataStructure.rpcOrigins.push_back(value.get<std::string>());
            }
            pDataStructure.rpcOrigins.shrink_to_fit();
        }

        if (jsonObjectData.contains("bot_public") && !jsonObjectData["bot_public"].is_null()) {
            pDataStructure.botPublic = jsonObjectData["bot_public"].get<bool>();
        }

        if (jsonObjectData.contains("bot_require_code_grant") && !jsonObjectData["bot_require_code_grant"].is_null()) {
            pDataStructure.botRequireCodeGrant = jsonObjectData["bot_require_code_grant"].get<bool>();
        }

        if (jsonObjectData.contains("terms_of_service_url") && !jsonObjectData["terms_of_service_url"].is_null()) {
            pDataStructure.termsOfServiceUrl = jsonObjectData["terms_of_service_url"].get<std::string>();
        }

        if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData["privacy_policy_url"].is_null()) {
            pDataStructure.privacyPolicyUrl = jsonObjectData["privacy_policy_url"].get<std::string>();
        }

        if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
            DataParser::parseObject(jsonObjectData["owner"], pDataStructure.owner);
        }

        if (jsonObjectData.contains("summary") && !jsonObjectData["summary"].is_null()) {
            pDataStructure.summary = jsonObjectData["summary"].get<std::string>();
        }

        if (jsonObjectData.contains("verify_key") && !jsonObjectData["verify_key"].is_null()) {
            pDataStructure.verifyKey = jsonObjectData["verify_key"].get<std::string>();
        }

        if (jsonObjectData.contains("team") && !jsonObjectData["team"].is_null()) {
            DataParser::parseObject(jsonObjectData["team"], pDataStructure.team);
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData["primary_sku_id"].is_null()) {
            pDataStructure.primarySkuId = jsonObjectData["primary_sku_id"].get<std::string>();
        }

        if (jsonObjectData.contains("slug") && !jsonObjectData["slug"].is_null()) {
            pDataStructure.slug = jsonObjectData["slug"].get<std::string>();
        }

        if (jsonObjectData.contains("cover_image") && !jsonObjectData["cover_image"].is_null()) {
            pDataStructure.coverImage = jsonObjectData["cover_image"].get<std::string>();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.coverImage = jsonObjectData["flags"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildScheduledEventMetadata& pDataStructure) {
        if (jsonObjectData.contains("location") && !jsonObjectData["location"].is_null()) {
            pDataStructure.location = jsonObjectData.at("location").get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildScheduledEvent& pDataStructure) {
        if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
            pDataStructure.privacyLevel = jsonObjectData.at("privacy_level").get<DiscordCoreAPI::GuildScheduledEventPrivacyLevel>();
        }

        if (jsonObjectData.contains("entity_type") && !jsonObjectData["entity_type"].is_null()) {
            pDataStructure.entityType = jsonObjectData.at("entity_type").get<DiscordCoreAPI::GuildScheduledEventEntityType>();
        }

        if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
            pDataStructure.status = jsonObjectData.at("status").get<DiscordCoreAPI::GuildScheduledEventStatus>();
        }

        if (jsonObjectData.contains("entity_metadata") && !jsonObjectData["entity_metadata"].is_null()) {
            DataParser::parseObject(jsonObjectData.at("entity_metadata"), pDataStructure.entityMetadata);
        }

        if (jsonObjectData.contains("scheduled_start_time") && !jsonObjectData["scheduled_start_time"].is_null()) {
            pDataStructure.scheduledStartTime = jsonObjectData.at("scheduled_start_time").get<std::string>();
        }

        if (jsonObjectData.contains("scheduled_end_time") && !jsonObjectData["scheduled_end_time"].is_null()) {
            pDataStructure.scheduledEndTime = jsonObjectData.at("scheduled_end_time").get<std::string>();
        }

        if (jsonObjectData.contains("user_count") && !jsonObjectData["user_count"].is_null()) {
            pDataStructure.userCount = jsonObjectData.at("user_count").get<uint32_t>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData.at("description").get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData.at("channel_id").get<std::string>();
        }

        if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
            pDataStructure.creatorId = jsonObjectData.at("creator_id").get<std::string>();
        }

        if (jsonObjectData.contains("entity_id") && !jsonObjectData["entity_id"].is_null()) {
            pDataStructure.entityId = jsonObjectData.at("entity_id").get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData.at("guild_id").get<std::string>();
        }

        if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
            DataParser::parseObject(jsonObjectData.at("creator"), pDataStructure.creator);
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData.at("name").get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData.at("id").get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildScheduledEventData& pDataStructure) {
        if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
            pDataStructure.privacyLevel = jsonObjectData.at("privacy_level").get<DiscordCoreAPI::GuildScheduledEventPrivacyLevel>();
        }

        if (jsonObjectData.contains("entity_type") && !jsonObjectData["entity_type"].is_null()) {
            pDataStructure.entityType = jsonObjectData.at("entity_type").get<DiscordCoreAPI::GuildScheduledEventEntityType>();
        }

        if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
            pDataStructure.status = jsonObjectData.at("status").get<DiscordCoreAPI::GuildScheduledEventStatus>();
        }

        if (jsonObjectData.contains("entity_metadata") && !jsonObjectData["entity_metadata"].is_null()) {
            DataParser::parseObject(jsonObjectData.at("entity_metadata"), pDataStructure.entityMetadata);
        }

        if (jsonObjectData.contains("scheduled_start_time") && !jsonObjectData["scheduled_start_time"].is_null()) {
            pDataStructure.scheduledStartTime = jsonObjectData.at("scheduled_start_time").get<std::string>();
        }

        if (jsonObjectData.contains("scheduled_end_time") && !jsonObjectData["scheduled_end_time"].is_null()) {
            pDataStructure.scheduledEndTime = jsonObjectData.at("scheduled_end_time").get<std::string>();
        }

        if (jsonObjectData.contains("user_count") && !jsonObjectData["user_count"].is_null()) {
            pDataStructure.userCount = jsonObjectData.at("user_count").get<uint32_t>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData.at("description").get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData.at("channel_id").get<std::string>();
        }

        if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
            pDataStructure.creatorId = jsonObjectData.at("creator_id").get<std::string>();
        }

        if (jsonObjectData.contains("entity_id") && !jsonObjectData["entity_id"].is_null()) {
            pDataStructure.entityId = jsonObjectData.at("entity_id").get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData.at("guild_id").get<std::string>();
        }

        if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
            DataParser::parseObject(jsonObjectData.at("creator"), pDataStructure.creator);
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData.at("name").get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData.at("id").get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildScheduledEventData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildScheduledEventData newData;
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildScheduledEvent>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildScheduledEvent newData;
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildTemplateData& pDataStructure) {

        if (jsonObjectData.contains("serialized_source_guild") && !jsonObjectData["serialized_source_guild"].is_null()) {
            DataParser::parseObject(jsonObjectData["serialized_source_guild"], pDataStructure.serializedSourceGuild);
        }

        if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
            DataParser::parseObject(jsonObjectData["creator"], pDataStructure.creator);
        }

        if (jsonObjectData.contains("source_guild_id") && !jsonObjectData["source_guild_id"].is_null()) {
            pDataStructure.sourceGuildId = jsonObjectData["source_guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("usage_count") && !jsonObjectData["usage_count"].is_null()) {
            pDataStructure.usageCount = jsonObjectData["usage_count"].get<uint32_t>();
        }

        if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
            pDataStructure.creatorId = jsonObjectData["creator_id"].get<std::string>();
        }

        if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
            pDataStructure.createdAt = jsonObjectData["created_at"].get<std::string>();
        }

        if (jsonObjectData.contains("updated_at") && !jsonObjectData["updated_at"].is_null()) {
            pDataStructure.updatedAt = jsonObjectData["updated_at"].get<std::string>();
        }

        if (jsonObjectData.contains("is_dirty") && !jsonObjectData["is_dirty"].is_null()) {
            pDataStructure.isDirty = jsonObjectData["is_dirty"].get<bool>();
        }

        if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null()) {
            pDataStructure.code = jsonObjectData["code"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildTemplateData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildTemplateData newData;
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildScheduledEventUserData& pDataStructure) {

        if (jsonObjectData.contains("guild_scheduled_event_id") && !jsonObjectData["guild_scheduled_event_id"].is_null()) {
            pDataStructure.guildScheduledEventId = jsonObjectData["guild_scheduled_event_id"].get<std::string>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildScheduledEventUserData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildScheduledEventUserData newData;
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::MessageReferenceData& pDataStructure) {
        if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
            pDataStructure.messageId = jsonObjectData["message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("fail_if_not_exists") && !jsonObjectData["fail_if_not_exists"].is_null()) {
            pDataStructure.failIfNotExists = jsonObjectData["fail_if_not_exists"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AllowedMentionsData& pDataStructure) {
        if (jsonObjectData.contains("parse") && !jsonObjectData["parse"].is_null()) {
            pDataStructure.parse.clear();
            pDataStructure.parse.reserve(jsonObjectData["parse"].size());
            for (auto& value : jsonObjectData["parse"]) {
                pDataStructure.parse.push_back(value.get<std::string>());
            }
            pDataStructure.parse.shrink_to_fit();
        }

        if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
            pDataStructure.roles.clear();
            pDataStructure.roles.reserve(jsonObjectData["roles"].size());
            for (auto& value : jsonObjectData["roles"]) {
                pDataStructure.roles.push_back(value.get<std::string>());
            }
            pDataStructure.roles.shrink_to_fit();
        }

        if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
            pDataStructure.users.clear();
            pDataStructure.users.reserve(jsonObjectData["users"].size());
            for (auto& value : jsonObjectData["users"]) {
                pDataStructure.users.push_back(value.get<std::string>());
            }
            pDataStructure.users.shrink_to_fit();
        }

        if (jsonObjectData.contains("replied_user") && !jsonObjectData["replied_user"].is_null()) {
            pDataStructure.repliedUser = jsonObjectData["replied_user"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::InteractionApplicationCommandCallbackData& pDataStructure) {
        if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
            pDataStructure.tts = jsonObjectData["tts"].get<bool>();
        }

        if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
            pDataStructure.content = jsonObjectData["content"].get<std::string>();
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
            pDataStructure.embeds.clear();
            pDataStructure.embeds.reserve(jsonObjectData["embeds"].size());
            for (auto& value : jsonObjectData["embeds"]) {
                DiscordCoreAPI::EmbedData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.embeds.push_back(std::move(newData));
            }
            pDataStructure.embeds.shrink_to_fit();
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.flags = jsonObjectData["flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("allowed_mentions") && !jsonObjectData["allowed_mentions"].is_null()) {
            DataParser::parseObject(jsonObjectData["allowed_mentions"], pDataStructure.allowedMentions);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::InteractionResponseData& pDataStructure) {
        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::InteractionCallbackType>();
        }

        if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
            DataParser::parseObject(jsonObjectData["data"], pDataStructure.data);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::SelectOptionData& pDataStructure) {
        if (jsonObjectData.contains("label") && !jsonObjectData["label"].is_null()) {
            pDataStructure.label = jsonObjectData["label"].get<std::string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
            pDataStructure.value = jsonObjectData["value"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
            DataParser::parseObject(jsonObjectData["emoji"], pDataStructure.emoji);
        }

        if (jsonObjectData.contains("default") && !jsonObjectData["default"].is_null()) {
            pDataStructure._default = jsonObjectData["default"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ActionRowData& pDataStructure) {
        if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
            pDataStructure.components.clear();
            pDataStructure.components.reserve(jsonObjectData.at("components").size());
            for (auto& value : jsonObjectData["components"]) {
                DiscordCoreAPI::ComponentData newData{};
                if (value.contains("type") && !value["type"].is_null()) {
                    newData.type = value["type"].get<DiscordCoreAPI::ComponentType>();
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
                    DataParser::parseObject(value["emoji"], newData.emoji);
                }

                if (value.contains("url") && !value["url"].is_null()) {
                    newData.url = value["url"].get<std::string>();
                }

                if (value.contains("options") && !value["options"].is_null()) {
                    newData.options.reserve(value["options"].size());
                    for (auto& value02 : value["options"]) {
                        DiscordCoreAPI::SelectOptionData newerData{};
                        DataParser::parseObject(value02, newerData);
                        newData.options.push_back(newerData);
                    }
                    newData.options.shrink_to_fit();
                }

                if (value.contains("max_values") && !value["max_values"].is_null()) {
                    newData.maxValues = value["max_values"].get<int32_t>();
                }

                if (value.contains("min_values") && !value["min_values"].is_null()) {
                    newData.minValues = value["min_values"].get<int32_t>();
                }

                pDataStructure.components.push_back(std::move(newData));
            }
        }
        pDataStructure.components.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::MessageInteractionData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::InteractionType>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::StickerItemData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
            pDataStructure.formatType = jsonObjectData["format_type"].get<DiscordCoreAPI::StickerItemType>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::MessageDataOld& pDataStructure) {
        if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
            pDataStructure.content = jsonObjectData["content"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
            DataParser::parseObject(jsonObjectData["author"], pDataStructure.author);
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
            pDataStructure.timestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
            pDataStructure.editedTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["edited_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
            pDataStructure.tts = jsonObjectData["tts"].get<bool>();
        }

        if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
            pDataStructure.mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
        }

        if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
            pDataStructure.mentions.clear();
            pDataStructure.mentions.reserve(jsonObjectData.at("mentions").size());
            for (auto& value : jsonObjectData["mentions"]) {
                DiscordCoreAPI::UserData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.mentions.push_back(std::move(newData));
            }
            pDataStructure.mentions.shrink_to_fit();
        }

        if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
            pDataStructure.mentionRoles.clear();
            pDataStructure.mentionRoles.reserve(jsonObjectData.at("mention_roles").size());
            for (auto& value : jsonObjectData["mention_roles"]) {
                pDataStructure.mentionRoles.push_back(value.get<std::string>());
            }
            pDataStructure.mentionRoles.shrink_to_fit();
        }

        if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
            pDataStructure.mentionChannels.clear();
            pDataStructure.mentionChannels.reserve(jsonObjectData.at("mention_channels").size());
            for (auto& value : jsonObjectData["mention_channels"]) {
                DiscordCoreAPI::ChannelMentionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.mentionChannels.push_back(std::move(newData));
            }
            pDataStructure.mentionChannels.shrink_to_fit();
        }

        if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
            pDataStructure.attachments.clear();
            pDataStructure.attachments.reserve(jsonObjectData.at("attachments").size());
            for (auto& value : jsonObjectData["attachments"]) {
                DiscordCoreAPI::AttachmentData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.attachments.push_back(std::move(newData));
            }
            pDataStructure.attachments.shrink_to_fit();
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
            pDataStructure.embeds.clear();
            pDataStructure.embeds.reserve(jsonObjectData.at("embeds").size());
            for (auto& value : jsonObjectData["embeds"]) {
                DiscordCoreAPI::EmbedData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.embeds.push_back(std::move(newData));
            }
            pDataStructure.embeds.shrink_to_fit();
        }

        if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
            pDataStructure.reactions.clear();
            pDataStructure.reactions.reserve(jsonObjectData.at("reactions").size());
            for (auto& value : jsonObjectData["reactions"]) {
                DiscordCoreAPI::ReactionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.reactions.push_back(std::move(newData));
            }
            pDataStructure.reactions.shrink_to_fit();
        }

        if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
            pDataStructure.nonce = jsonObjectData["nonce"].get<std::string>();
        }

        if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
            pDataStructure.pinned = jsonObjectData["pinned"].get<bool>();
        }

        if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
            pDataStructure.webhookId = jsonObjectData["webhook_id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = (DiscordCoreAPI::MessageType)jsonObjectData["type"].get<int32_t>();
        }

        if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
            DataParser::parseObject(jsonObjectData["activity"], pDataStructure.activity);
        }

        if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
            DataParser::parseObject(jsonObjectData["application"], pDataStructure.application);
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
            DataParser::parseObject(jsonObjectData["message_reference"], pDataStructure.messageReference);
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.flags = jsonObjectData["flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
            pDataStructure.stickerItems.clear();
            pDataStructure.stickerItems.reserve(jsonObjectData.at("sticker_items").size());
            for (auto& value : jsonObjectData["sticker_items"]) {
                DiscordCoreAPI::StickerItemData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stickerItems.push_back(std::move(newData));
            }
            pDataStructure.stickerItems.shrink_to_fit();
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
            pDataStructure.stickers.clear();
            pDataStructure.stickers.reserve(jsonObjectData.at("stickers").size());
            for (auto& value : jsonObjectData["stickers"]) {
                DiscordCoreAPI::StickerData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stickers.push_back(std::move(newData));
            }
            pDataStructure.stickers.shrink_to_fit();
        }

        if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
            DataParser::parseObject(jsonObjectData["interaction"], pDataStructure.interaction);
        }

        if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
            pDataStructure.components.clear();
            pDataStructure.components.reserve(jsonObjectData.at("components").size());
            for (auto& value : jsonObjectData["components"]) {
                DiscordCoreAPI::ActionRowData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.components.push_back(std::move(newData));
            }
            pDataStructure.components.shrink_to_fit();
        }

        if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
            DataParser::parseObject(jsonObjectData["thread"], pDataStructure.thread);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::Message& pDataStructure) {
        if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
            pDataStructure.content = jsonObjectData["content"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
            DataParser::parseObject(jsonObjectData["author"], pDataStructure.author);
            pDataStructure.requesterId = pDataStructure.author.id;
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
            pDataStructure.timestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
            pDataStructure.editedTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["edited_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
            pDataStructure.tts = jsonObjectData["tts"].get<bool>();
        }

        if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
            pDataStructure.mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
        }

        if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
            pDataStructure.mentions.clear();
            pDataStructure.mentions.reserve(jsonObjectData.at("mentions").size());
            for (auto& value : jsonObjectData["mentions"]) {
                DiscordCoreAPI::UserData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.mentions.push_back(std::move(newData));
            }
            pDataStructure.mentions.shrink_to_fit();
        }

        if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
            pDataStructure.mentionRoles.clear();
            pDataStructure.mentionRoles.reserve(jsonObjectData.at("mention_roles").size());
            for (auto& value : jsonObjectData["mention_roles"]) {
                pDataStructure.mentionRoles.push_back(value.get<std::string>());
            }
            pDataStructure.mentionRoles.shrink_to_fit();
        }

        if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
            pDataStructure.mentionChannels.clear();
            pDataStructure.mentionChannels.reserve(jsonObjectData.at("mentions_channels").size());
            for (auto& value : jsonObjectData["mention_channels"]) {
                DiscordCoreAPI::ChannelMentionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.mentionChannels.push_back(std::move(newData));
            }
            pDataStructure.mentionChannels.shrink_to_fit();
        }

        if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
            pDataStructure.attachments.clear();
            pDataStructure.attachments.reserve(jsonObjectData.at("attachments").size());
            for (auto& value : jsonObjectData["attachments"]) {
                DiscordCoreAPI::AttachmentData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.attachments.push_back(std::move(newData));
            }
            pDataStructure.attachments.shrink_to_fit();
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
            pDataStructure.embeds.clear();
            pDataStructure.embeds.reserve(jsonObjectData.at("embeds").size());
            for (auto& value : jsonObjectData["embeds"]) {
                DiscordCoreAPI::EmbedData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.embeds.push_back(std::move(newData));
            }
            pDataStructure.embeds.shrink_to_fit();
        }

        if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
            pDataStructure.reactions.clear();
            pDataStructure.reactions.reserve(jsonObjectData.at("reactions").size());
            for (auto& value : jsonObjectData["reactions"]) {
                DiscordCoreAPI::ReactionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.reactions.push_back(std::move(newData));
            }
            pDataStructure.reactions.shrink_to_fit();
        }

        if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
            pDataStructure.nonce = jsonObjectData["nonce"].get<std::string>();
        }

        if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
            pDataStructure.pinned = jsonObjectData["pinned"].get<bool>();
        }

        if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
            pDataStructure.webhookId = jsonObjectData["webhook_id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = (DiscordCoreAPI::MessageType)jsonObjectData["type"].get<int32_t>();
        }

        if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
            DataParser::parseObject(jsonObjectData["activity"], pDataStructure.activity);
        }

        if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
            DataParser::parseObject(jsonObjectData["application"], pDataStructure.application);
        }

        if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
            DataParser::parseObject(jsonObjectData["message_reference"], pDataStructure.messageReference);
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.flags = jsonObjectData["flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
            pDataStructure.stickers.clear();
            pDataStructure.stickers.reserve(jsonObjectData.at("stickers").size());
            for (auto& value : jsonObjectData["stickers"]) {
                DiscordCoreAPI::StickerData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stickers.push_back(std::move(newData));
            }
            pDataStructure.stickers.shrink_to_fit();
        }

        if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
            DataParser::parseObject(jsonObjectData["interaction"], pDataStructure.interaction);
        }

        if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
            pDataStructure.components.clear();
            pDataStructure.components.reserve(jsonObjectData.at("components").size());
            for (auto& value : jsonObjectData["components"]) {
                DiscordCoreAPI::ActionRowData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.components.push_back(std::move(newData));
            }
            pDataStructure.components.shrink_to_fit();
        }

        if (jsonObjectData.contains("referenced_message") && !jsonObjectData["referenced_message"].is_null()) {
            DataParser::parseObject(jsonObjectData["referenced_message"], pDataStructure.referencedMessage);
        }

        if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
            DataParser::parseObject(jsonObjectData["thread"], pDataStructure.thread);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::MessageData& pDataStructure) {
        if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
            pDataStructure.content = jsonObjectData["content"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
            DataParser::parseObject(jsonObjectData["author"], pDataStructure.author);
            pDataStructure.requesterId = pDataStructure.author.id;
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
            pDataStructure.timestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
            pDataStructure.editedTimestamp = DiscordCoreAPI::TimeStamp(jsonObjectData["edited_timestamp"].get<std::string>());
        }

        if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
            pDataStructure.tts = jsonObjectData["tts"].get<bool>();
        }

        if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
            pDataStructure.mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
        }

        if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
            pDataStructure.mentions.clear();
            pDataStructure.mentions.reserve(jsonObjectData.at("mentions").size());
            for (auto& value : jsonObjectData["mentions"]) {
                DiscordCoreAPI::UserData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.mentions.push_back(std::move(newData));
            }
            pDataStructure.mentions.shrink_to_fit();
        }

        if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
            pDataStructure.mentionRoles.clear();
            pDataStructure.mentionRoles.reserve(jsonObjectData.at("mention_roles").size());
            for (auto& value : jsonObjectData["mention_roles"]) {
                pDataStructure.mentionRoles.push_back(value.get<std::string>());
            }
            pDataStructure.mentionRoles.shrink_to_fit();
        }

        if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
            pDataStructure.mentionChannels.clear();
            pDataStructure.mentionChannels.reserve(jsonObjectData.at("mentions_channels").size());
            for (auto& value : jsonObjectData["mention_channels"]) {
                DiscordCoreAPI::ChannelMentionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.mentionChannels.push_back(std::move(newData));
            }
            pDataStructure.mentionChannels.shrink_to_fit();
        }

        if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
            pDataStructure.attachments.clear();
            pDataStructure.attachments.reserve(jsonObjectData.at("attachments").size());
            for (auto& value : jsonObjectData["attachments"]) {
                DiscordCoreAPI::AttachmentData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.attachments.push_back(std::move(newData));
            }
            pDataStructure.attachments.shrink_to_fit();
        }

        if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
            pDataStructure.embeds.clear();
            pDataStructure.embeds.reserve(jsonObjectData.at("embeds").size());
            for (auto& value : jsonObjectData["embeds"]) {
                DiscordCoreAPI::EmbedData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.embeds.push_back(std::move(newData));
            }
            pDataStructure.embeds.shrink_to_fit();
        }

        if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
            pDataStructure.reactions.clear();
            pDataStructure.reactions.reserve(jsonObjectData.at("reactions").size());
            for (auto& value : jsonObjectData["reactions"]) {
                DiscordCoreAPI::ReactionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.reactions.push_back(std::move(newData));
            }
            pDataStructure.reactions.shrink_to_fit();
        }

        if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
            pDataStructure.nonce = jsonObjectData["nonce"].get<std::string>();
        }

        if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
            pDataStructure.pinned = jsonObjectData["pinned"].get<bool>();
        }

        if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
            pDataStructure.webhookId = jsonObjectData["webhook_id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = (DiscordCoreAPI::MessageType)jsonObjectData["type"].get<int32_t>();
        }

        if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
            DataParser::parseObject(jsonObjectData["activity"], pDataStructure.activity);
        }

        if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
            DataParser::parseObject(jsonObjectData["application"], pDataStructure.application);
        }

        if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
            DataParser::parseObject(jsonObjectData["message_reference"], pDataStructure.messageReference);
        }

        if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
            pDataStructure.flags = jsonObjectData["flags"].get<int32_t>();
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
            pDataStructure.stickers.clear();
            pDataStructure.stickers.reserve(jsonObjectData.at("stickers").size());
            for (auto& value : jsonObjectData["stickers"]) {
                DiscordCoreAPI::StickerData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stickers.push_back(std::move(newData));
            }
            pDataStructure.stickers.shrink_to_fit();
        }

        if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
            DataParser::parseObject(jsonObjectData["interaction"], pDataStructure.interaction);
        }

        if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
            pDataStructure.components.clear();
            pDataStructure.components.reserve(jsonObjectData.at("components").size());
            for (auto& value : jsonObjectData["components"]) {
                DiscordCoreAPI::ActionRowData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.components.push_back(std::move(newData));
            }
            pDataStructure.components.shrink_to_fit();
        }

        if (jsonObjectData.contains("referenced_message") && !jsonObjectData["referenced_message"].is_null()) {
            DataParser::parseObject(jsonObjectData["referenced_message"], pDataStructure.referencedMessage);
        }

        if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
            DataParser::parseObject(jsonObjectData["thread"], pDataStructure.thread);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::MessageData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::MessageData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::Message>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::Message newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationCommandOptionChoiceData& pDataStructure) {
        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null() && jsonObjectData["value"].is_string()) {
            pDataStructure.valueString = jsonObjectData["value"].get<std::string>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null() && jsonObjectData["value"].is_number_integer()) {
            pDataStructure.valueInt = jsonObjectData["value"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationCommandOptionData& pDataStructure) {
        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_types") && !jsonObjectData["channel_types"].is_null()) {
            pDataStructure.channelTypes.clear();
            pDataStructure.channelTypes.reserve(jsonObjectData.at("channel_types").size());
            for (auto& value : jsonObjectData["channel_types"]) {
                pDataStructure.channelTypes.push_back(std::move(value));
            }
            pDataStructure.channelTypes.shrink_to_fit();
        }

        if (jsonObjectData.contains("required") && !jsonObjectData["required"].is_null()) {
            pDataStructure.required = jsonObjectData["required"].get<bool>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandOptionType>();
        }

        if (pDataStructure.type == DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group || pDataStructure.type == DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command) {
            if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
                pDataStructure.options.reserve(jsonObjectData.at("options").size());
                for (auto& value : jsonObjectData["options"]) {
                    DiscordCoreAPI::ApplicationCommandOptionData newData{};
                    DataParser::parseObject(value, newData);
                    pDataStructure.options.push_back(std::move(newData));
                }
                pDataStructure.channelTypes.shrink_to_fit();
            }
        }
        else {
            if (jsonObjectData.contains("choices") && !jsonObjectData["choices"].is_null()) {
                pDataStructure.choices.clear();
                pDataStructure.choices.reserve(jsonObjectData.at("choices").size());
                for (auto& value : jsonObjectData["choices"]) {
                    DiscordCoreAPI::ApplicationCommandOptionChoiceData newData{};
                    DataParser::parseObject(value, newData);
                    pDataStructure.choices.push_back(std::move(newData));
                }
                pDataStructure.choices.shrink_to_fit();
            }
        }

    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationCommand& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
            pDataStructure.version = jsonObjectData["version"].get<std::string>();
        }

        if (jsonObjectData.contains("default_permission") && !jsonObjectData["default_permission"].is_null()) {
            pDataStructure.defaultPermission = jsonObjectData["default_permission"].get<bool>();
        }

        if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
            pDataStructure.options.reserve(jsonObjectData.at("options").size());
            for (auto& value : jsonObjectData["options"]) {
                DiscordCoreAPI::ApplicationCommandOptionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.options.push_back(std::move(newData));
            }
            pDataStructure.options.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationCommandData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
            pDataStructure.version = jsonObjectData["version"].get<std::string>();
        }

        if (jsonObjectData.contains("default_permission") && !jsonObjectData["default_permission"].is_null()) {
            pDataStructure.defaultPermission = jsonObjectData["default_permission"].get<bool>();
        }

        if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
            pDataStructure.options.reserve(jsonObjectData.at("options").size());
            for (auto& value : jsonObjectData["options"]) {
                DiscordCoreAPI::ApplicationCommandOptionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.options.push_back(std::move(newData));
            }
            pDataStructure.options.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::ApplicationCommandData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::ApplicationCommandData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::ApplicationCommand>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::ApplicationCommand newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionDataOption& pDataStructure) {
        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandOptionType>();
        }

        if (jsonObjectData.contains("focused") && !jsonObjectData["focused"].is_null()) {
            pDataStructure.focused = jsonObjectData["focused"].get<bool>();
        }

        if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
            if (jsonObjectData["value"].is_boolean()) {
                pDataStructure.valueBool = jsonObjectData["value"].get<bool>();
            }
            if (jsonObjectData["value"].is_string()) {
                pDataStructure.valueString = jsonObjectData["value"].get<std::string>();
            }
            if (jsonObjectData["value"].is_number()) {
                pDataStructure.valueInt = jsonObjectData["value"].get<int32_t>();
            }
        }

        if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
            pDataStructure.options.reserve(jsonObjectData.at("options").size());
            for (auto& value : jsonObjectData["options"]) {
                DiscordCoreAPI::ApplicationCommandInteractionDataOption newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.options.push_back(std::move(newData));
            }
            pDataStructure.options.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::YouTubeFormat>& pDataStructure) {

        if (!jsonObjectData.is_null()) {
            if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("formats") && !jsonObjectData["streamingData"]["formats"].is_null()) {
                for (auto& value : jsonObjectData["streamingData"]["formats"]) {
                    DiscordCoreAPI::YouTubeFormat newData{};

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
                    }
                    else if (value.contains("cipher") && !value["cipher"].is_null()) {
                        newData.signatureCipher = value["cipher"].get<std::string>();
                    }

                    if (newData.signatureCipher.find("&sp") != std::string::npos) {
                        newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
                    }

                    if (newData.signatureCipher.find("url") != std::string::npos) {
                        newData.downloadUrl = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
                    }
                    else if (value.contains("url") && !value["url"].is_null()) {
                        newData.downloadUrl = value["url"];
                    }

                    pDataStructure.push_back(std::move(newData));
                }
            }

            if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("adaptiveFormats") && !jsonObjectData["streamingData"]["adaptiveFormats"].is_null()) {
                for (auto& value : jsonObjectData["streamingData"]["adaptiveFormats"]) {
                    DiscordCoreAPI::YouTubeFormat newData{};

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
                    }
                    else if (value.contains("cipher") && !value["cipher"].is_null()) {
                        newData.signatureCipher = value["cipher"].get<std::string>();
                    }

                    if (newData.signatureCipher.find("&sp") != std::string::npos) {
                        newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
                    }

                    if (newData.signatureCipher.find("url") != std::string::npos) {
                        newData.downloadUrl = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
                    }
                    else if (value.contains("url") && !value["url"].is_null()) {
                        newData.downloadUrl = value["url"];
                    }

                    pDataStructure.push_back(std::move(newData));
                }
            }
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::CommandData& pDataStructure) {
        if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
            pDataStructure.optionsArgs.reserve(jsonObjectData.at("options").size());
            for (auto& value : jsonObjectData["options"]) {
                if (value.contains("type") && value["type"] == 1) {
                    if (value.contains("name")) {
                        pDataStructure.optionsArgs.push_back(std::move(value["name"]));
                    }
                }
                if (value.contains("options")) {
                    DataParser::parseObject(value, pDataStructure);
                }
                if (value.contains("value") && !value["value"].is_null()) {
                    auto& newValueNew = value["value"];
                    if (newValueNew.is_string()) {
                        pDataStructure.optionsArgs.push_back(std::move(newValueNew));
                    }
                    else {
                        pDataStructure.optionsArgs.push_back(std::move(std::to_string(newValueNew.get<int64_t>())));
                    }
                }
            }
            pDataStructure.optionsArgs.shrink_to_fit();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.commandName = jsonObjectData["name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationCommandInteractionData& pDataStructure) {
        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandType>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
            pDataStructure.options.reserve(jsonObjectData.at("options").size());
            for (auto& value : jsonObjectData["options"]) {
                DiscordCoreAPI::ApplicationCommandInteractionDataOption newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.options.push_back(std::move(newData));
            }
            pDataStructure.options.shrink_to_fit();
        }

        if (jsonObjectData.contains("resolved") && !jsonObjectData["resolved"].is_null()) {
            auto& value = jsonObjectData["resolved"];

            if (value.contains("attachments") && !value["attachments"].is_null()) {
                pDataStructure.resolved.attachments.clear();
                auto newMap = value["attachments"].get<std::unordered_map<std::string, nlohmann::json>>();
                for (auto& [key, newValue] : newMap) {
                    DiscordCoreAPI::AttachmentData newData{};
                    DataParser::parseObject(newValue, newData);
                    pDataStructure.resolved.attachments.insert(std::make_pair(key, std::move(newData)));
                }
            }

            if (value.contains("users") && !value["users"].is_null()) {
                pDataStructure.resolved.users.clear();
                auto newMap = value["users"].get<std::unordered_map<std::string, nlohmann::json>>();
                for (auto& [key, newValue] : newMap) {
                    DiscordCoreAPI::UserData newData{};
                    DataParser::parseObject(newValue, newData);
                    pDataStructure.resolved.users.insert(std::make_pair(key, std::move(newData)));
                }
            }
            if (value.contains("channels") && !value["channels"].is_null()) {
                pDataStructure.resolved.channels.clear();
                auto newMap = value["channels"].get<std::unordered_map<std::string, nlohmann::json>>();
                for (auto& [key, newValue] : newMap) {
                    DiscordCoreAPI::ChannelData newData{};
                    DataParser::parseObject(newValue, newData);
                    pDataStructure.resolved.channels.insert(std::make_pair(key, std::move(newData)));
                }
            }
            if (value.contains("roles") && !value["roles"].is_null()) {
                pDataStructure.resolved.roles.clear();
                auto newMap = value["roles"].get<std::unordered_map<std::string, nlohmann::json>>();
                for (auto& [key, newValue] : newMap) {
                    DiscordCoreAPI::RoleData newData{};
                    DataParser::parseObject(newValue, newData);
                    pDataStructure.resolved.roles.insert(std::make_pair(key, std::move(newData)));
                }
            }
            if (value.contains("members") && !value["members"].is_null()) {
                pDataStructure.resolved.members.clear();
                auto newMap = value["members"].get<std::unordered_map<std::string, nlohmann::json>>();
                for (auto& [key, newValue] : newMap) {
                    DiscordCoreAPI::GuildMemberData newData{};
                    DataParser::parseObject(newValue, newData);
                    pDataStructure.resolved.members.insert(std::make_pair(key, std::move(newData)));
                }
            }
            if (value.contains("messages") && !value["messages"].is_null()) {
                pDataStructure.resolved.messages.clear();
                auto newMap = value["messages"].get<std::unordered_map<std::string, nlohmann::json>>();
                for (auto& [key, newValue] : newMap) {
                    DiscordCoreAPI::MessageData newData{};
                    DataParser::parseObject(newValue, newData);
                    pDataStructure.resolved.messages.insert(std::make_pair(key, std::move(newData)));
                }
            }
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::MessageCommandInteractionData& pDataStructure) {
        if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
            pDataStructure.targetId = jsonObjectData["target_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::UserCommandInteractionData& pDataStructure) {
        if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
            pDataStructure.targetId = jsonObjectData["target_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::StickerPackData& pDataStructure) {
        if (jsonObjectData.contains("stickers") && !jsonObjectData.at("stickers").is_null()) {
            pDataStructure.stickers.clear();
            pDataStructure.stickers.reserve(jsonObjectData["stickers"].size());
            for (auto& value : jsonObjectData["stickers"]) {
                DiscordCoreAPI::StickerData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stickers.push_back(std::move(newData));
            }
            pDataStructure.stickers.shrink_to_fit();
        }

        if (jsonObjectData.contains("cover_sticker_id") && !jsonObjectData["cover_sticker_id"].is_null()) {
            pDataStructure.coverStickerId = jsonObjectData["cover_sticker_id"].get<std::string>();
        }

        if (jsonObjectData.contains("banner_asset_id") && !jsonObjectData["banner_asset_id"].is_null()) {
            pDataStructure.bannerAssetId = jsonObjectData["banner_asset_id"].get<std::string>();
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            pDataStructure.description = jsonObjectData["description"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.Id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("sku_id") && !jsonObjectData["sku_id"].is_null()) {
            pDataStructure.skuId = jsonObjectData["sku_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::StickerPackData>& pDataStructure) {
        if (jsonObjectData.contains("sticker_packs") && !jsonObjectData["sticker_packs"].is_null()) {
            pDataStructure.reserve(jsonObjectData["sticker_packs"].size());
            for (auto& value : jsonObjectData["sticker_packs"]) {
                DiscordCoreAPI::StickerPackData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.push_back(std::move(newData));
            }
            pDataStructure.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ComponentInteractionData& pDataStructure) {
        if (jsonObjectData.contains("values") && !jsonObjectData["values"].is_null()) {
            pDataStructure.values.clear();
            pDataStructure.values.reserve(jsonObjectData.at("values").size());
            for (auto& value : jsonObjectData["values"]) {
                pDataStructure.values.push_back(std::move(value));
            }
            pDataStructure.values.shrink_to_fit();
        }

        if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
            pDataStructure.customId = jsonObjectData["custom_id"].get<std::string>();
        }

        if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
            pDataStructure.componentType = jsonObjectData["component_type"].get<DiscordCoreAPI::ComponentType>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ModalInteractionData& pDataStructure) {
        if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
            pDataStructure.value = jsonObjectData.at("components").at(0).at("components").at(0).at("value").get<std::string>();
        }

        if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
            pDataStructure.customId = jsonObjectData.at("custom_id").get<std::string>();
        }

        if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
            pDataStructure.customIdSmall = jsonObjectData.at("components").at(0).at("components").at(0).at("custom_id").get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::InteractionDataData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            DataParser::parseObject(jsonObjectData, pDataStructure.applicationCommanddata);
        }

        if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
            DataParser::parseObject(jsonObjectData, pDataStructure.messageInteractionData);
            DataParser::parseObject(jsonObjectData, pDataStructure.userInteractionData);
        }

        if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
            DataParser::parseObject(jsonObjectData, pDataStructure.componentData);
        }

        if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
            DataParser::parseObject(jsonObjectData, pDataStructure.modalData);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::InteractionData& pDataStructure) {
        if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
            DataParser::parseObject(jsonObjectData["data"], pDataStructure.data);
            pDataStructure.rawData = jsonObjectData["data"];
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::InteractionType>();
        }

        if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
            pDataStructure.token = jsonObjectData["token"].get<std::string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
            pDataStructure.user = pDataStructure.member.user;
        }
        else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("message") && !jsonObjectData["message"].is_null()) {
            DataParser::parseObject(jsonObjectData["message"], pDataStructure.message);
        }

        if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
            pDataStructure.version = jsonObjectData["version"].get<int32_t>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ReactionRemoveData& pDataStructure) {
        if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
            pDataStructure.userId = jsonObjectData["user_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
            pDataStructure.messageId = jsonObjectData["message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
            DataParser::parseObject(jsonObjectData["emoji"], pDataStructure.emoji);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::WebHook& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::WebHookType>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
            pDataStructure.avatar = jsonObjectData["avatar"].get<std::string>();
        }

        if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
            pDataStructure.token = jsonObjectData["token"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
            DataParser::parseObject(jsonObjectData["source_guild"], pDataStructure.sourceGuild);
        }

        if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
            DataParser::parseObject(jsonObjectData["source_channel"], pDataStructure.sourceChannel);
        }

        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::WebHookData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::WebHookType>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
            pDataStructure.avatar = jsonObjectData["avatar"].get<std::string>();
        }

        if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
            pDataStructure.token = jsonObjectData["token"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
            DataParser::parseObject(jsonObjectData["source_guild"], pDataStructure.sourceGuild);
        }

        if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
            DataParser::parseObject(jsonObjectData["source_channel"], pDataStructure.sourceChannel);
        }

        if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
            pDataStructure.url = jsonObjectData["url"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::WebHookData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::WebHookData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::WebHook>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::WebHook newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AuditLogChangeData& pDataStructure) {
        if (jsonObjectData.contains("new_value") && !jsonObjectData["new_value"].is_null()) {
            if (jsonObjectData["new_value"].is_string()) {
                pDataStructure.newValueString = jsonObjectData["new_value"].get<std::string>();
            }
            else if (jsonObjectData["new_value"].is_boolean()) {
                pDataStructure.newValueBool = jsonObjectData["new_value"].get<bool>();
            }
            else if (jsonObjectData["new_value"].is_number_integer()) {
                pDataStructure.newValueInt = jsonObjectData["new_value"].get<int32_t>();
            }
        }

        if (jsonObjectData.contains("old_value") && !jsonObjectData["old_value"].is_null()) {
            if (jsonObjectData["old_value"].is_string()) {
                pDataStructure.oldValueString = jsonObjectData["old_value"].get<std::string>();
            }
            else if (jsonObjectData["old_value"].is_boolean()) {
                pDataStructure.oldValueBool = jsonObjectData["old_value"].get<bool>();
            }
            else if (jsonObjectData["old_value"].is_number_integer()) {
                pDataStructure.oldValueInt = jsonObjectData["old_value"].get<int32_t>();
            }
        }

        if (jsonObjectData.contains("key") && !jsonObjectData["key"].is_null()) {
            pDataStructure.key = jsonObjectData["key"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AuditLogEntryInfoData& pDataStructure) {
        if (jsonObjectData.contains("delete_member_days") && !jsonObjectData["delete_member_days"].is_null()) {
            pDataStructure.deleteMemberDays = jsonObjectData["delete_member_days"].get<std::string>();
        }

        if (jsonObjectData.contains("members_removed") && !jsonObjectData["members_removed"].is_null()) {
            pDataStructure.membersRemoved = jsonObjectData["members_removed"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
            pDataStructure.messageId = jsonObjectData["message_id"].get<std::string>();
        }

        if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
            pDataStructure.count = jsonObjectData["count"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<std::string>();
        }

        if (jsonObjectData.contains("role_name") && !jsonObjectData["role_name"].is_null()) {
            pDataStructure.roleName = jsonObjectData["role_name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AuditLogEntryData& pDataStructure) {
        if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
            pDataStructure.targetId = jsonObjectData["target_id"].get<std::string>();
        }

        if (jsonObjectData.contains("changes") && !jsonObjectData["changes"].is_null()) {
            pDataStructure.changes.clear();
            pDataStructure.changes.reserve(jsonObjectData.at("changes").size());
            for (auto& value : jsonObjectData["changes"]) {
                DiscordCoreAPI::AuditLogChangeData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.changes.push_back(std::move(newData));
            }
            pDataStructure.changes.shrink_to_fit();
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
            pDataStructure.userId = jsonObjectData["user_id"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
            pDataStructure.createdTimeStamp = pDataStructure.getCreatedAtTimestamp(DiscordCoreAPI::TimeFormat::LongDateTime);
        }

        if (jsonObjectData.contains("action_type") && !jsonObjectData["action_type"].is_null()) {
            pDataStructure.actionType = jsonObjectData["action_type"].get<DiscordCoreAPI::AuditLogEvent>();
        }

        if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
            DataParser::parseObject(jsonObjectData["options"], pDataStructure.options);
        }

        if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
            pDataStructure.reason = jsonObjectData["reason"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AccountData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::IntegrationData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<std::string>();
        }

        if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
            pDataStructure.enabled = jsonObjectData["enabled"].get<bool>();
        }

        if (jsonObjectData.contains("syncing") && !jsonObjectData["syncing"].is_null()) {
            pDataStructure.syncing = jsonObjectData["syncing"].get<bool>();
        }

        if (jsonObjectData.contains("role_id") && !jsonObjectData["role_id"].is_null()) {
            pDataStructure.roleId = jsonObjectData["role_id"].get<std::string>();
        }

        if (jsonObjectData.contains("enable_emoticons") && !jsonObjectData["enable_emoticons"].is_null()) {
            pDataStructure.enableEmoticons = jsonObjectData["enable_emoticons"].get<bool>();
        }

        if (jsonObjectData.contains("expire_behavior") && !jsonObjectData["expire_behavior"].is_null()) {
            pDataStructure.expireBehavior = jsonObjectData["expire_behavior"].get< int32_t>();
        }

        if (jsonObjectData.contains("expire_grace_period") && !jsonObjectData["expire_grace_period"].is_null()) {
            pDataStructure.expireGracePeriod = jsonObjectData["expire_grace_period"].get< int32_t>();
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("account") && !jsonObjectData["account"].is_null()) {
            DataParser::parseObject(jsonObjectData["account"], pDataStructure.account);
        }

        if (jsonObjectData.contains("synced_at") && !jsonObjectData["synced_at"].is_null()) {
            pDataStructure.syncedAt = DiscordCoreAPI::TimeStamp(jsonObjectData["synced_at"].get<std::string>());
        }

        if (jsonObjectData.contains("subscriber_count") && !jsonObjectData["subscriber_count"].is_null()) {
            pDataStructure.subscriberCount = jsonObjectData["subscriber_count"].get< int32_t>();
        }

        if (jsonObjectData.contains("revoked") && !jsonObjectData["revoked"].is_null()) {
            pDataStructure.revoked = jsonObjectData["revoked"].get<bool>();
        }

        if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
            DataParser::parseObject(jsonObjectData["application"], pDataStructure.application);
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::IntegrationData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::IntegrationData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ConnectionData& pDataStructure) {
        if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
            pDataStructure.name = jsonObjectData["name"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<std::string>();
        }

        if (jsonObjectData.contains("show_activity") && !jsonObjectData["show_activity"].is_null()) {
            pDataStructure.showActivity = jsonObjectData["show_activity"].get<bool>();
        }

        if (jsonObjectData.contains("friend_sync") && !jsonObjectData["friend_sync"].is_null()) {
            pDataStructure.friendSync = jsonObjectData["friend_sync"].get<bool>();
        }

        if (jsonObjectData.contains("revoked") && !jsonObjectData["revoked"].is_null()) {
            pDataStructure.revoked = jsonObjectData["revoked"].get<bool>();
        }

        if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
            pDataStructure.verified = jsonObjectData["verified"].get<bool>();
        }

        if (jsonObjectData.contains("visibility") && !jsonObjectData["visibility"].is_null()) {
            pDataStructure.visibility = jsonObjectData["visibility"].get<DiscordCoreAPI::ConnectionVisibilityTypes>();
        }

        if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
            pDataStructure.integrations.clear();
            pDataStructure.integrations.reserve(jsonObjectData.at("integrations").size());
            for (auto& value : jsonObjectData["integrations"]) {
                DiscordCoreAPI::IntegrationData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.integrations.push_back(std::move(newData));
            }
            pDataStructure.integrations.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AuditLogData& pDataStructure) {
        if (jsonObjectData.contains("webhooks") && !jsonObjectData["webhooks"].is_null()) {
            pDataStructure.webhooks.clear();
            pDataStructure.webhooks.reserve(jsonObjectData.at("webhooks").size());
            for (auto& value : jsonObjectData["webhooks"]) {
                DiscordCoreAPI::WebHookData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.webhooks.push_back(std::move(newData));
            }
            pDataStructure.webhooks.shrink_to_fit();
        }

        if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
            pDataStructure.users.clear();
            pDataStructure.users.reserve(jsonObjectData.at("users").size());
            for (auto& value : jsonObjectData["users"]) {
                DiscordCoreAPI::UserData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.users.push_back(std::move(newData));
            }
            pDataStructure.users.shrink_to_fit();
        }

        if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData["audit_log_entries"].is_null()) {
            pDataStructure.auditLogEntries.clear();
            pDataStructure.auditLogEntries.reserve(jsonObjectData.at("audit_log_entries").size());
            for (auto& value : jsonObjectData["audit_log_entries"]) {
                DiscordCoreAPI::AuditLogEntryData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.auditLogEntries.push_back(std::move(newData));
            }
            pDataStructure.auditLogEntries.shrink_to_fit();
        }

        if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
            pDataStructure.integrations.clear();
            pDataStructure.integrations.reserve(jsonObjectData.at("integrations").size());
            for (auto& value : jsonObjectData["integrations"]) {
                DiscordCoreAPI::IntegrationData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.integrations.push_back(std::move(newData));
            }
            pDataStructure.integrations.shrink_to_fit();
        }

        if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
            pDataStructure.threads.clear();
            pDataStructure.threads.reserve(jsonObjectData.at("threads").size());
            for (auto& value : jsonObjectData["threads"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.threads.push_back(std::move(newData));
            }
            pDataStructure.threads.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::TypingStartData& pDataStructure) {
        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructure.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
            DataParser::parseObject(jsonObjectData["member"], pDataStructure.member);
        }

        if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
            pDataStructure.userId = jsonObjectData["user_id"].get<std::string>();
        }

        if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
            pDataStructure.timestamp = jsonObjectData["timestamp"].get<int32_t>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ChannelPinsUpdateEventData& pDataStructre) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructre.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
            pDataStructre.channelId = jsonObjectData["channel_id"].get<std::string>();
        }

        if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
            pDataStructre.lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::BanData& pDataStructure) {
        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }

        if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
            pDataStructure.reason = jsonObjectData["reason"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::BanData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::BanData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ThreadListSyncData& pDataStructure) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("channel_ids") && !jsonObjectData["channel_ids"].is_null()) {
            pDataStructure.channelIds.clear();
            pDataStructure.channelIds.reserve(jsonObjectData.at("channel_ids").size());
            for (auto& value : jsonObjectData["channel_ids"]) {
                pDataStructure.channelIds.push_back(value.get<std::string>());
            }
            pDataStructure.channelIds.shrink_to_fit();
        }

        if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
            pDataStructure.members.clear();
            pDataStructure.members.reserve(jsonObjectData.at("members").size());
            for (auto& value : jsonObjectData["members"]) {
                DiscordCoreAPI::ThreadMemberData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.members.push_back(std::move(newData));
            }
            pDataStructure.members.shrink_to_fit();
        }

        if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
            pDataStructure.threads.clear();
            pDataStructure.threads.reserve(jsonObjectData.at("threads").size());
            for (auto& value : jsonObjectData["threads"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.threads.push_back(std::move(newData));
            }
            pDataStructure.threads.shrink_to_fit();
        }

    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ThreadMembersUpdateData& pDataStructure) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
            pDataStructure.memberCount = jsonObjectData["member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("added_members") && !jsonObjectData["added_members"].is_null()) {
            pDataStructure.addedMembers.clear();
            pDataStructure.addedMembers.reserve(jsonObjectData.at("added_members").size());
            for (auto& value : jsonObjectData["added_members"]) {
                DiscordCoreAPI::ThreadMemberData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.addedMembers.push_back(std::move(newData));
            }
            pDataStructure.addedMembers.shrink_to_fit();
        }

        if (jsonObjectData.contains("removed_member_ids") && !jsonObjectData["removed_member_ids"].is_null()) {
            pDataStructure.removedMemberIds.clear();
            pDataStructure.removedMemberIds.reserve(jsonObjectData.at("removed_member_ids").size());
            for (auto& value : jsonObjectData["removed_member_ids"]) {
                pDataStructure.removedMemberIds.push_back(std::move(value));
            }
            pDataStructure.removedMemberIds.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ArchivedThreadsData& pDataStructure) {
        if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
            pDataStructure.threads.clear();
            pDataStructure.threads.reserve(jsonObjectData.at("threads").size());
            for (auto& value : jsonObjectData["threads"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.threads.push_back(std::move(newData));
            }
            pDataStructure.threads.shrink_to_fit();
        }

        if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
            pDataStructure.members.clear();
            pDataStructure.members.reserve(jsonObjectData.at("members").size());
            for (auto& value : jsonObjectData["members"]) {
                DiscordCoreAPI::ThreadMemberData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.members.push_back(std::move(newData));
            }
            pDataStructure.members.shrink_to_fit();
        }

        if (jsonObjectData.contains("has_more") && !jsonObjectData["has_more"].is_null()) {
            pDataStructure.hasMore = jsonObjectData["has_more"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ActiveThreadsData& pDataStructure) {
        if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
            pDataStructure.threads.clear();
            pDataStructure.threads.reserve(jsonObjectData.at("threads").size());
            for (auto& value : jsonObjectData["threads"]) {
                DiscordCoreAPI::ChannelData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.threads.push_back(std::move(newData));
            }
            pDataStructure.threads.shrink_to_fit();
        }

        if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
            pDataStructure.members.clear();
            pDataStructure.members.reserve(jsonObjectData.at("members").size());
            for (auto& value : jsonObjectData["members"]) {
                DiscordCoreAPI::ThreadMemberData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.members.push_back(std::move(newData));
            }
            pDataStructure.members.shrink_to_fit();
        }

        if (jsonObjectData.contains("has_more") && !jsonObjectData["has_more"].is_null()) {
            pDataStructure.hasMore = jsonObjectData["has_more"].get<bool>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildEmojisUpdateEventData& pDataStructure) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
            pDataStructure.emojis.clear();
            pDataStructure.emojis.reserve(jsonObjectData.at("emojis").size());
            for (auto& value : jsonObjectData["emojis"]) {
                DiscordCoreAPI::EmojiData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.emojis.push_back(std::move(newData));
            }
            pDataStructure.emojis.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildStickersUpdateEventData& pDataStructure) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
            pDataStructure.stickers.clear();
            pDataStructure.stickers.reserve(jsonObjectData.at("stickers").size());
            for (auto& value : jsonObjectData["stickers"]) {
                DiscordCoreAPI::StickerData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.stickers.push_back(std::move(newData));
            }
            pDataStructure.stickers.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildMembersChunkEventData& pDataStructure) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
            pDataStructure.nonce = jsonObjectData["nonce"].get<std::string>();
        }

        if (jsonObjectData.contains("chunk_index") && !jsonObjectData["chunk_index"].is_null()) {
            pDataStructure.chunkIndex = jsonObjectData["chunk_index"].get<int32_t>();
        }

        if (jsonObjectData.contains("chunk_count") && !jsonObjectData["chunk_count"].is_null()) {
            pDataStructure.chunkCount = jsonObjectData["chunk_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
            pDataStructure.presences.clear();
            pDataStructure.presences.reserve(jsonObjectData.at("presences").size());
            for (auto& value : jsonObjectData["presences"]) {
                DiscordCoreAPI::PresenceUpdateData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.presences.push_back(std::move(newData));
            }
            pDataStructure.presences.shrink_to_fit();
        }

        if (jsonObjectData.contains("not_found") && !jsonObjectData["not_found"].is_null()) {
            pDataStructure.notFound.clear();
            pDataStructure.notFound.reserve(jsonObjectData.at("not_found").size());
            for (auto& value : jsonObjectData["not_found"]) {
                pDataStructure.notFound.push_back(std::move(value));
            }
            pDataStructure.notFound.shrink_to_fit();
        }

        if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
            pDataStructure.members.clear();
            pDataStructure.members.reserve(jsonObjectData.at("members").size());
            for (auto& value : jsonObjectData["members"]) {
                DiscordCoreAPI::GuildMemberData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.members.push_back(std::move(newData));
            }
            pDataStructure.members.shrink_to_fit();
        }

    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::ApplicationCommandPermissionData& pDataStructure) {
        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("permission") && !jsonObjectData["permission"].is_null()) {
            pDataStructure.permission = jsonObjectData["permission"].get<bool>();
        }

        if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
            pDataStructure.type = jsonObjectData["type"].get<DiscordCoreAPI::ApplicationCommandPermissionType>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::GuildApplicationCommandPermissionsData& pDataStructure) {
        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
            pDataStructure.applicationId = jsonObjectData["application_id"].get<std::string>();
        }

        if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
            pDataStructure.id = jsonObjectData["id"].get<std::string>();
        }

        if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
            pDataStructure.permissions.clear();
            pDataStructure.permissions.reserve(jsonObjectData.at("permissions").size());
            for (auto& value : jsonObjectData["permissions"]) {
                DiscordCoreAPI::ApplicationCommandPermissionData newData{};
                DataParser::parseObject(value, newData);
                pDataStructure.permissions.push_back(std::move(newData));
            }
            pDataStructure.permissions.shrink_to_fit();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::GuildApplicationCommandPermissionsData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::YouTubeSong& pDataStructure) {
        if (jsonObjectData.contains("lengthText") && !jsonObjectData["lengthText"].is_null()) {
            pDataStructure.duration = jsonObjectData["lengthText"]["accessibility"]["accessibilityData"]["label"].get<std::string>();
        }

        if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData["detailedMetadataSnippets"].is_null()) {
            for (auto& value : jsonObjectData["detailedMetadataSnippets"].at(0)["snippetText"]["runs"]) {
                std::string newString = value["text"].get<std::string>();
                if (newString.size() > 256) {
                    newString = newString.substr(0, 256);
                }
                char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
                char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);

                pDataStructure.description += newString02;
            }
        }

        if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
            pDataStructure.thumbnailUrl = jsonObjectData["thumbnail"]["thumbnails"].at(0)["url"].get<std::string>();
        }

        if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
            pDataStructure.songId = "https://www.youtube.com/watch?v=" + jsonObjectData["videoId"].get<std::string>();
        }

        if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
            if (jsonObjectData["title"].contains("runs")) {

                std::string newString = jsonObjectData["title"]["runs"].at(0)["text"].get<std::string>();
                if (newString.size() > 256) {
                    newString = newString.substr(0, 256);
                }
                char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
                char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);

                pDataStructure.songTitle = newString02;
            }
            else if (jsonObjectData["title"].contains("simpleText")) {
                std::string newString = jsonObjectData["title"]["simpleText"].get<std::string>();
                if (newString.size() > 256) {
                    newString = newString.substr(0, 256);
                }
                char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
                char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);

                pDataStructure.songTitle = newString02;
            }
        }
        if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
            pDataStructure.songId = jsonObjectData["videoId"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::SoundCloudSong& pDataStructure) {
        if (jsonObjectData.contains("track_authorization") && !jsonObjectData["track_authorization"].is_null()) {
            pDataStructure.trackAuthorization = jsonObjectData["track_authorization"].get<std::string>();
        }

        if (jsonObjectData.contains("media") && !jsonObjectData["media"].is_null()) {
            bool isItFound{ false };
            for (auto& value : jsonObjectData["media"]["transcodings"]) {
                if (value["preset"] == "opus_0_0") {
                    isItFound = true;
                    pDataStructure.firstDownloadUrl = value["url"].get<std::string>();
                    pDataStructure.songId = value["url"].get<std::string>();
                    pDataStructure.doWeGetSaved = true;
                }
            }
            bool isItFound2{ false };
            if (!isItFound) {
                for (auto& value : jsonObjectData["media"]["transcodings"]) {
                    if (value["preset"] == "mp3_0_0") {
                        pDataStructure.firstDownloadUrl = value["url"].get<std::string>();
                        pDataStructure.songId = value["url"].get<std::string>();
                        isItFound2 = true;
                    }
                }
            }
            if (!isItFound2 && !isItFound) {
                for (auto& value : jsonObjectData["media"]["transcodings"]) {
                    pDataStructure.firstDownloadUrl = value["url"].get<std::string>();
                    pDataStructure.songId = value["url"].get<std::string>();
                }
            }
        }

        if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null() && !jsonObjectData["title"].is_object()) {
            std::string newString = jsonObjectData["title"].get<std::string>();
            if (newString.size() > 256) {
                newString = newString.substr(0, 256);
            }
            char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
            char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);

            pDataStructure.songTitle = newString02;
        }

        if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
            std::string newString = jsonObjectData["description"].get<std::string>();
            if (newString.size() > 256) {
                newString = newString.substr(0, 256);
            }
            char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
            char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);
            for (int32_t x = 0; x < newString.size(); x += 1) {
                pDataStructure.description.push_back(newString02[x]);
            }
            pDataStructure.description += "...";
        }

        if (jsonObjectData.contains("artwork_url") && !jsonObjectData["artwork_url"].is_null()) {
            pDataStructure.thumbnailUrl = jsonObjectData["artwork_url"].get<std::string>();
        }
        else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            if (jsonObjectData["user"].contains("avatar_url") && !jsonObjectData["user"]["avatar_url"].is_null()) {
                pDataStructure.thumbnailUrl = jsonObjectData["user"]["avatar_url"].get<std::string>();
            }
        }

        if (jsonObjectData.contains("duration") && !jsonObjectData["duration"].is_null()) {
            int32_t durationNew = jsonObjectData["duration"].get<int32_t>();
            pDataStructure.duration = DiscordCoreAPI::convertMsToDurationString(durationNew);
        }

        if (jsonObjectData.contains("permalink_url") && !jsonObjectData["permalink_url"].is_null()) {
            pDataStructure.viewUrl = jsonObjectData["permalink_url"].get<std::string>();
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::AuthorizationInfoData& pDataStructure) {

        if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
            DataParser::parseObject(jsonObjectData["application"], pDataStructure.application);
        }

        if (jsonObjectData.contains("scopes") && !jsonObjectData["scopes"].is_null()) {
            DataParser::parseObject(jsonObjectData["scopes"], pDataStructure.scopes);
        }

        if (jsonObjectData.contains("expires") && !jsonObjectData["expires"].is_null()) {
            pDataStructure.expires = jsonObjectData["expires"];
        }

        if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
            DataParser::parseObject(jsonObjectData["user"], pDataStructure.user);
        }
    };

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, DiscordCoreAPI::InviteData& pDataStructure) {
        if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_string()) {
            pDataStructure.code = jsonObjectData["code"].get<std::string>();
        }
        else if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_number_integer()) {
            pDataStructure.code = std::to_string(jsonObjectData["code"].get<int32_t>());
        }

        if (jsonObjectData.contains("guild") && !jsonObjectData["guild"].is_null()) {
            DataParser::parseObject(jsonObjectData["guild"], pDataStructure.guild);
        }

        if (jsonObjectData.contains("channel") && !jsonObjectData["channel"].is_null()) {
            DataParser::parseObject(jsonObjectData["channel"], pDataStructure.channel);
        }

        if (jsonObjectData.contains("inviter") && !jsonObjectData["inviter"].is_null()) {
            DataParser::parseObject(jsonObjectData["inviter"], pDataStructure.inviter);
        }

        if (jsonObjectData.contains("target_type") && !jsonObjectData["target_type"].is_null()) {
            pDataStructure.targetType = jsonObjectData["target_type"].get<int32_t>();
        }

        if (jsonObjectData.contains("target_user") && !jsonObjectData["target_user"].is_null()) {
            DataParser::parseObject(jsonObjectData["target_user"], pDataStructure.targetUser);
        }

        if (jsonObjectData.contains("target_application") && !jsonObjectData["target_application"].is_null()) {
            DataParser::parseObject(jsonObjectData["target_application"], pDataStructure.targetApplication);
        }

        if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
            pDataStructure.approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
            pDataStructure.approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
        }

        if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
            pDataStructure.guildId = jsonObjectData["guild_id"].get<std::string>();
        }

        if (jsonObjectData.contains("expires_at") && !jsonObjectData["expires_at"].is_null()) {
            pDataStructure.expiresAt = DiscordCoreAPI::TimeStamp(jsonObjectData["expires_at"].get<std::string>());
        }

        if (jsonObjectData.contains("stage_instance") && !jsonObjectData["stage_instance"].is_null()) {
            DataParser::parseObject(jsonObjectData["stage_instance"], pDataStructure.stageInstance);
        }

        if (jsonObjectData.contains("guild_scheduled_event") && !jsonObjectData["guild_scheduled_event"].is_null()) {
            DataParser::parseObject(jsonObjectData["guild_scheduled_event"], pDataStructure.guildScheduledEvent);
        }

        if (jsonObjectData.contains("uses") && !jsonObjectData["uses"].is_null()) {
            pDataStructure.uses = jsonObjectData["uses"].get<int32_t>();
        }

        if (jsonObjectData.contains("max_uses") && !jsonObjectData["max_uses"].is_null()) {
            pDataStructure.maxUses = jsonObjectData["max_uses"].get<int32_t>();
        }

        if (jsonObjectData.contains("max_age") && !jsonObjectData["max_age"].is_null()) {
            pDataStructure.maxAge = jsonObjectData["max_age"].get<int32_t>();
        }

        if (jsonObjectData.contains("temporary") && !jsonObjectData["temporary"].is_null()) {
            pDataStructure.temporary = jsonObjectData["temporary"].get<bool>();
        }

        if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
            pDataStructure.createdAt = DiscordCoreAPI::TimeStamp(jsonObjectData["created_at"].get<std::string>());
        }
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::ConnectionData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::ConnectionData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

    template<>
    void DataParser::parseObject(nlohmann::json const& jsonObjectData, std::vector<DiscordCoreAPI::InviteData>& pDataStructure) {
        pDataStructure.reserve(jsonObjectData.size());
        for (auto& value : jsonObjectData) {
            DiscordCoreAPI::InviteData newData{};
            DataParser::parseObject(value, newData);
            pDataStructure.push_back(std::move(newData));
        }
        pDataStructure.shrink_to_fit();
    }

};