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
/// FromJson.hpp - Header file for the from-json functions.
/// Aug 08, 2022
/// https://discordcoreapi.com
/// \file FromJson.hpp

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/InputEvents.hpp>

namespace nlohmann {

	template<> struct adl_serializer<DiscordCoreAPI::RoleData> {
		static DiscordCoreAPI::RoleData from_json(const json& jsonObjectData) {
			DiscordCoreAPI::RoleData theRole{};

			theRole.id = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "id"));

			theRole.name = DiscordCoreAPI::getString(&jsonObjectData, "name");

			std::stringstream theStream{};
			theStream << DiscordCoreAPI::getString(&jsonObjectData, "unicode_emoji");
			for (auto& value: theStream.str()) {
				theRole.unicodeEmoji.push_back(value);
			}
			if (theRole.unicodeEmoji.size() > 3) {
				theRole.unicodeEmoji = static_cast<std::string>(theRole.unicodeEmoji).substr(1, theRole.unicodeEmoji.size() - 3);
			}

			theRole.color = DiscordCoreAPI::getUint32(&jsonObjectData, "color");

			theRole.flags = DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::RoleFlags>(theRole.flags, DiscordCoreAPI::RoleFlags::Hoist, DiscordCoreAPI::getBoolReal(&jsonObjectData, "hoist"));

			theRole.flags =
				DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::RoleFlags>(theRole.flags, DiscordCoreAPI::RoleFlags::Managed, DiscordCoreAPI::getBoolReal(&jsonObjectData, "managed"));

			theRole.flags =
				DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::RoleFlags>(theRole.flags, DiscordCoreAPI::RoleFlags::Mentionable, DiscordCoreAPI::getBoolReal(&jsonObjectData, "mentionable"));

			theRole.position = DiscordCoreAPI::getUint16(&jsonObjectData, "position");

			theRole.permissions = DiscordCoreAPI::Permissions{ DiscordCoreAPI::getString(&jsonObjectData, "permissions") };
			return theRole;
		}
	};

	template<> struct adl_serializer<DiscordCoreAPI::OverWriteData> {
		static DiscordCoreAPI::OverWriteData from_json(const json& jsonObjectData) {
			DiscordCoreAPI::OverWriteData theOverWrite{};

			theOverWrite.id = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "id"));

			theOverWrite.allow = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "allow"));

			theOverWrite.deny = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "deny"));

			theOverWrite.type = static_cast<DiscordCoreAPI::PermissionOverwritesType>(DiscordCoreAPI::getUint8(&jsonObjectData, "type"));

			return theOverWrite;
		}
	};

	template<> struct adl_serializer<DiscordCoreAPI::ChannelData> {
		static DiscordCoreAPI::ChannelData from_json(const json& jsonObjectData) {
			DiscordCoreAPI::ChannelData theChannel{};

			theChannel.id = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "id"));

			theChannel.flags = DiscordCoreAPI::getUint8(&jsonObjectData, "flags");

			theChannel.type = static_cast<DiscordCoreAPI::ChannelType>(DiscordCoreAPI::getUint8(&jsonObjectData, "type"));

			theChannel.parentId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "parent_id"));

			theChannel.guildId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "guild_id"));

			theChannel.position = DiscordCoreAPI::getUint32(&jsonObjectData, "position");

			if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
				theChannel.permissionOverwrites.clear();
				theChannel.permissionOverwrites.reserve(jsonObjectData["permission_overwrites"].size());
				for (auto& value: jsonObjectData["permission_overwrites"]) {
					theChannel.permissionOverwrites.push_back(value.get<DiscordCoreAPI::OverWriteData>());
				}
			}

			theChannel.name = DiscordCoreAPI::getString(&jsonObjectData, "name");

			theChannel.flags =
				DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::ChannelFlags>(theChannel.flags, DiscordCoreAPI::ChannelFlags::NSFW, DiscordCoreAPI::getBoolReal(&jsonObjectData, "nsfw"));

			theChannel.ownerId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "owner_id"));

			theChannel.memberCount = DiscordCoreAPI::getUint32(&jsonObjectData, "member_count");
			return theChannel;
		}
	};

	template<> struct adl_serializer<DiscordCoreAPI::UserData> {
		static DiscordCoreAPI::UserData from_json(const json& jsonObjectData) {
			DiscordCoreAPI::UserData theUser{};
			theUser.flags =
				DiscordCoreAPI::setBool<int32_t, DiscordCoreAPI::UserFlags>(theUser.flags, DiscordCoreAPI::UserFlags::MFAEnabled, DiscordCoreAPI::getBoolReal(&jsonObjectData, "mfa_enabled"));

			theUser.flags =
				DiscordCoreAPI::setBool<int32_t, DiscordCoreAPI::UserFlags>(theUser.flags, DiscordCoreAPI::UserFlags::Verified, DiscordCoreAPI::getBoolReal(&jsonObjectData, "verified"));

			theUser.flags =
				DiscordCoreAPI::setBool<int32_t, DiscordCoreAPI::UserFlags>(theUser.flags, DiscordCoreAPI::UserFlags::System, DiscordCoreAPI::getBoolReal(&jsonObjectData, "system"));

			theUser.flags = DiscordCoreAPI::setBool<int32_t, DiscordCoreAPI::UserFlags>(theUser.flags, DiscordCoreAPI::UserFlags::Bot, DiscordCoreAPI::getBoolReal(&jsonObjectData, "bot"));

			theUser.discriminator = DiscordCoreAPI::getString(&jsonObjectData, "discriminator");

			theUser.flags = DiscordCoreAPI::getUint32(&jsonObjectData, "public_flags");

			theUser.userName = DiscordCoreAPI::getString(&jsonObjectData, "username");

			theUser.id = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "id"));

			theUser.avatar = DiscordCoreAPI::getString(&jsonObjectData, "avatar");
			return theUser;
		}
	};

	template<> struct adl_serializer<DiscordCoreAPI::GuildMemberData> {
		static DiscordCoreAPI::GuildMemberData from_json(const json& jsonObjectData) {
			DiscordCoreAPI::GuildMemberData guildMember{};
			guildMember.flags = DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildMemberFlags>(guildMember.flags, DiscordCoreAPI::GuildMemberFlags::Pending,
				DiscordCoreAPI::getBoolReal(&jsonObjectData, "pending"));

			guildMember.flags =
				setBool<int8_t, DiscordCoreAPI::GuildMemberFlags>(guildMember.flags, DiscordCoreAPI::GuildMemberFlags::Mute, DiscordCoreAPI::getBoolReal(&jsonObjectData, "mute"));

			guildMember.flags =
				setBool<int8_t, DiscordCoreAPI::GuildMemberFlags>(guildMember.flags, DiscordCoreAPI::GuildMemberFlags::Deaf, DiscordCoreAPI::getBoolReal(&jsonObjectData, "deaf"));

			guildMember.joinedAt = DiscordCoreAPI::TimeStamp<std::chrono::milliseconds>(DiscordCoreAPI::getString(&jsonObjectData, "joined_at"));

			guildMember.guildId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "guild_id"));
			std::vector<std::string> theVector{};
			DiscordCoreAPI::getVector(&jsonObjectData, "roles", theVector);
			for (auto& value: theVector) {
				guildMember.roles.push_back(stoull(std::move(value)));
			}

			guildMember.permissions = DiscordCoreAPI::getUint64(&jsonObjectData, "permissions");

			std::unique_ptr<DiscordCoreAPI::UserData> theUser = std::make_unique<DiscordCoreAPI::UserData>(jsonObjectData["user"].get<DiscordCoreAPI::UserData>());
			guildMember.id = theUser->id;
			auto theUserNew = theUser.get();
			theUserNew->insertUser(std::move(theUser));

			guildMember.avatar = DiscordCoreAPI::getString(&jsonObjectData, "avatar");

			guildMember.flags = DiscordCoreAPI::getUint8(&jsonObjectData, "flags");

			guildMember.nick = DiscordCoreAPI::getString(&jsonObjectData, "nick");
			return guildMember;
		}
	};

	template<> struct adl_serializer<DiscordCoreAPI::VoiceStateData> {
		static DiscordCoreAPI::VoiceStateData from_json(const json& jsonObjectData) {
			DiscordCoreAPI::VoiceStateData theVoiceState{};

			theVoiceState.requestToSpeakTimestamp = DiscordCoreAPI::getString(&jsonObjectData, "request_to_speak_timestamp");

			theVoiceState.channelId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "channel_id"));

			theVoiceState.guildId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "guild_id"));

			theVoiceState.selfStream = DiscordCoreAPI::getBoolReal(&jsonObjectData, "self_stream");

			theVoiceState.userId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "user_id"));

			theVoiceState.selfVideo = DiscordCoreAPI::getBoolReal(&jsonObjectData, "self_video");

			theVoiceState.sessionId = DiscordCoreAPI::getString(&jsonObjectData, "session_id");

			theVoiceState.selfDeaf = DiscordCoreAPI::getBoolReal(&jsonObjectData, "self_deaf");

			theVoiceState.selfMute = DiscordCoreAPI::getBoolReal(&jsonObjectData, "self_mute");

			theVoiceState.suppress = DiscordCoreAPI::getBoolReal(&jsonObjectData, "suppress");

			theVoiceState.deaf = DiscordCoreAPI::getBoolReal(&jsonObjectData, "deaf");

			theVoiceState.mute = DiscordCoreAPI::getBoolReal(&jsonObjectData, "mute");
			return theVoiceState;
		}
	};

	template<> struct adl_serializer<DiscordCoreAPI::GuildData> {
		static DiscordCoreAPI::GuildData from_json(const json& jsonObjectData) {
			DiscordCoreAPI::GuildData theGuild{};
			theGuild.id = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "id"));

			theGuild.icon = DiscordCoreAPI::getString(&jsonObjectData, "icon");

			theGuild.name = DiscordCoreAPI::getString(&jsonObjectData, "name");

			theGuild.joinedAt = DiscordCoreAPI::TimeStamp<std::chrono::milliseconds>(DiscordCoreAPI::getString(&jsonObjectData, "joined_at"));

			theGuild.flags = setBool<int8_t, DiscordCoreAPI::GuildFlags>(theGuild.flags, DiscordCoreAPI::GuildFlags::Owner, DiscordCoreAPI::getBoolReal(&jsonObjectData, "owner"));

			theGuild.ownerId = DiscordCoreAPI::strtoull(DiscordCoreAPI::getString(&jsonObjectData, "owner_id"));

			theGuild.flags =
				setBool<int8_t, DiscordCoreAPI::GuildFlags>(theGuild.flags, DiscordCoreAPI::GuildFlags::WidgetEnabled, DiscordCoreAPI::getBoolReal(&jsonObjectData, "widget_enabled"));

			theGuild.flags = setBool<int8_t, DiscordCoreAPI::GuildFlags>(theGuild.flags, DiscordCoreAPI::GuildFlags::Large, DiscordCoreAPI::getBoolReal(&jsonObjectData, "large"));

			theGuild.flags = setBool<int8_t, DiscordCoreAPI::GuildFlags>(theGuild.flags, DiscordCoreAPI::GuildFlags::Unavailable, DiscordCoreAPI::getBoolReal(&jsonObjectData, "unavailable"));

			theGuild.memberCount = DiscordCoreAPI::getUint32(&jsonObjectData, "member_count");

			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				theGuild.roles.clear();
				for (auto& value: jsonObjectData["roles"]) {
					std::unique_ptr<DiscordCoreAPI::RoleData> newData{ std::make_unique<DiscordCoreAPI::RoleData>(value.get<DiscordCoreAPI::RoleData>()) };
					theGuild.roles.push_back(newData->id);
					auto theRole = newData.get();
					theRole->insertRole(std::move(newData));
				}
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				theGuild.members.clear();
				for (auto& value: jsonObjectData["members"]) {
					std::unique_ptr<DiscordCoreAPI::GuildMemberData> newData{ std::make_unique<DiscordCoreAPI::GuildMemberData>(value.get<DiscordCoreAPI::GuildMemberData>()) };
					newData->guildId = theGuild.id;
					theGuild.members.push_back(newData.release());
				}
			}

			if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
				for (auto& value: jsonObjectData["voice_states"]) {
					auto userId = DiscordCoreAPI::strtoull(value["user_id"].get<std::string>());
					for (auto& value02: theGuild.members) {
						if (value02->id == userId) {
							value02->voiceChannelId = DiscordCoreAPI::strtoull(value["channel_id"].get<std::string>());
						}
					}
				}
			}

			if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
				theGuild.channels.clear();
				for (auto& value: jsonObjectData["channels"]) {
					std::unique_ptr<DiscordCoreAPI::ChannelData> newData{ std::make_unique<DiscordCoreAPI::ChannelData>(value.get<DiscordCoreAPI::ChannelData>()) };
					newData->guildId = theGuild.id;
					theGuild.channels.push_back(newData->id);
					auto theChannel = newData.get();
					theChannel->insertChannel(std::move(newData));
				}
			}
			return theGuild;
		}
	};
}// namespace nlohmann