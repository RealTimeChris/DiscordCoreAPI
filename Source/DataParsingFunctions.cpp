/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
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

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommand& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.defaultMemberPermissions = getString(jsonObjectData, "default_member_permissions");

		theData.dmPermission = getBool(jsonObjectData, "dm_permission");

		theData.version = getString(jsonObjectData, "version");

		theData.type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		for (auto& [key, value]: jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>()) {
			theData.nameLocalizations.emplace(key, value);
		}

		for (auto& [key, value]: jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>()) {
			theData.descriptionLocalizations.emplace(key, value);
		}

		theData.applicationId = strtoull(getString(jsonObjectData, "application_id"));

		theData.name = getString(jsonObjectData, "name");

		theData.description = getString(jsonObjectData, "description");

		theData.version = getString(jsonObjectData, "version");

		for (auto& value: jsonObjectData["options"]) {
			ApplicationCommandOptionData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.options.emplace_back(newData);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, UserData& theData) {
		theData.flags |= setBool(theData.flags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		theData.flags |= setBool(theData.flags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		theData.flags |= setBool(theData.flags, UserFlags::System, getBool(jsonObjectData, "system"));

		theData.flags |= setBool(theData.flags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		theData.discriminator = getString(jsonObjectData, "discriminator");

		theData.flags |= getUint32(jsonObjectData, "public_flags");

		theData.userName = getString(jsonObjectData, "username");

		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.avatar = getString(jsonObjectData, "avatar");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, RoleData& theData) {
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

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.color = getUint32(jsonObjectData, "color");

		theData.flags |= setBool(theData.flags, RoleFlags::Hoist, getBool(jsonObjectData, "hoist"));

		theData.flags |= setBool(theData.flags, RoleFlags::Managed, getBool(jsonObjectData, "managed"));

		theData.flags |= setBool(theData.flags, RoleFlags::Mentionable, getBool(jsonObjectData, "mentionable"));

		theData.position = getUint32(jsonObjectData, "position");

		theData.permissions = getString(jsonObjectData, "permissions");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, User& theData) {
		theData.userName = getString(jsonObjectData, "username");

		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.accentColor = getUint32(jsonObjectData, "accent_color");

		theData.banner = getString(jsonObjectData, "banner");

		theData.discriminator = getString(jsonObjectData, "discriminator");

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags |= setBool(theData.flags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		theData.flags |= setBool(theData.flags, UserFlags::System, getBool(jsonObjectData, "system"));

		theData.flags |= setBool(theData.flags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		theData.flags |= setBool(theData.flags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		theData.locale = getString(jsonObjectData, "locale");

		theData.email = getString(jsonObjectData, "email");

		theData.premiumType = static_cast<PremiumType>(getUint8(jsonObjectData, "premium_type"));

		theData.flags = getUint32(jsonObjectData, "public_flags");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Role& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.icon = getString(jsonObjectData, "icon");

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

		theData.flags |= setBool(theData.flags, RoleFlags::Hoist, getBool(jsonObjectData, "hoist"));

		theData.position = getUint32(jsonObjectData, "position");

		theData.permissions = getString(jsonObjectData, "permissions");

		theData.flags |= setBool(theData.flags, RoleFlags::Managed, getBool(jsonObjectData, "managed"));

		theData.flags |= setBool(theData.flags, RoleFlags::Mentionable, getBool(jsonObjectData, "mentionable"));

		if (jsonObjectData.contains("tags") && !jsonObjectData["tags"].is_null()) {
			parseObject(jsonObjectData["tags"], theData.tags);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildMemberData& theData) {
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.roles.clear();
		theData.roles.reserve(jsonObjectData["roles"].size());
		for (auto& value: jsonObjectData["roles"]) {
			theData.roles.push_back(stoull(value.get<std::string>()));
		}

		theData.permissions = getString(jsonObjectData, "permissions");

		std::unique_ptr<UserData> theUser = std::make_unique<UserData>();
		DiscordCoreAPI::parseObject(jsonObjectData["user"], *theUser);
		theData.id = theUser->id;
		Users::insertUser(std::move(theUser));

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.nick = getString(jsonObjectData, "nick");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildMember& theData) {
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.roles.clear();
		theData.roles.reserve(jsonObjectData["roles"].size());
		for (auto& value: jsonObjectData["roles"]) {
			theData.roles.push_back(stoull(value.get<std::string>()));
		}

		theData.permissions = getString(jsonObjectData, "permissions");

		std::unique_ptr<UserData> theUser = std::make_unique<UserData>();
		DiscordCoreAPI::parseObject(jsonObjectData["user"], *theUser);
		theData.id = theUser->id;
		Users::insertUser(std::move(theUser));

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.nick = getString(jsonObjectData, "nick");

		theData.communicationDisabledUntil = getString(jsonObjectData, "communication_disabled_until");

		theData.premiumSince = getString(jsonObjectData, "premium_since");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, OverWriteData& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.allow = strtoull(getString(jsonObjectData, "allow"));

		theData.deny = strtoull(getString(jsonObjectData, "deny"));

		theData.type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ChannelData& theData) {
		theData.flags |= setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.ownerId = strtoull(getString(jsonObjectData, "owner_id"));

		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.parentId = strtoull(getString(jsonObjectData, "parent_id"));

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.position = getUint32(jsonObjectData, "position");

		theData.permissionOverwrites = {};
		for (auto& value: jsonObjectData["permission_overwrites"]) {
			OverWriteData* theDataNew{ new OverWriteData{} };
			DiscordCoreAPI::parseObject(value, *theDataNew);
			theData.permissionOverwrites.emplace_back(std::move(theDataNew));
		}

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildData& theData) {
		theData.flags |= setBool(theData.flags, GuildFlags::WidgetEnabled, DiscordCoreAPI::getBool(jsonObjectData, "widget_enabled"));

		theData.flags |= setBool(theData.flags, GuildFlags::Unavailable, DiscordCoreAPI::getBool(jsonObjectData, "unavailable"));

		theData.flags |= setBool(theData.flags, GuildFlags::Owner, DiscordCoreAPI::getBool(jsonObjectData, "owner"));

		theData.flags |= setBool(theData.flags, GuildFlags::Large, DiscordCoreAPI::getBool(jsonObjectData, "large"));

		theData.ownerId = strtoull(getString(jsonObjectData, "owner_id"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.icon = getString(jsonObjectData, "icon");

		theData.name = getString(jsonObjectData, "name");

		theData.threads.clear();
		theData.threads.reserve(jsonObjectData["threads"].size());
		for (auto& value: jsonObjectData["threads"]) {
			theData.threads.push_back(strtoull(value["id"].get<std::string>()));
		}

		theData.stickers.clear();
		theData.stickers.reserve(jsonObjectData["stickers"].size());
		for (auto& value: jsonObjectData["stickers"]) {
			theData.stickers.push_back(strtoull(value["id"].get<std::string>()));
		}

		theData.guildScheduledEvents.clear();
		theData.guildScheduledEvents.reserve(jsonObjectData["guild_scheduled_events"].size());
		for (auto& value: jsonObjectData["guild_scheduled_events"]) {
			theData.guildScheduledEvents.push_back(strtoull(value["id"].get<std::string>()));
		}

		theData.stageInstances.clear();
		theData.stageInstances.reserve(jsonObjectData["stage_instances"].size());
		for (auto& value: jsonObjectData["stage_instances"]) {
			theData.stageInstances.push_back(strtoull(value["id"].get<std::string>()));
		}

		theData.emoji.clear();
		theData.emoji.reserve(jsonObjectData["emoji"].size());
		for (auto& value: jsonObjectData["emoji"]) {
			theData.emoji.push_back(strtoull(value["id"].get<std::string>()));
		}

		if (Roles::doWeCacheRoles) {
			theData.roles.clear();
			theData.roles.reserve(jsonObjectData["roles"].size());
			for (auto& value: jsonObjectData["roles"]) {
				std::unique_ptr<RoleData> newData{ std::make_unique<RoleData>() };
				DiscordCoreAPI::parseObject(value, *newData);
				theData.roles.emplace_back(newData->id);
				Roles::insertRole(std::move(newData));
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			theData.members.clear();
			theData.members.reserve(jsonObjectData["members"].size());
			for (auto& value: jsonObjectData["members"]) {
				std::unique_ptr<GuildMemberData> newData{ std::make_unique<GuildMemberData>() };
				DiscordCoreAPI::parseObject(value, *newData);
				newData->guildId = theData.id;
				theData.members.push_back(newData->id);
				GuildMembers::insertGuildMember(std::move(newData));
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			for (auto& value: jsonObjectData["voice_states"]) {
				auto userId = strtoull(value["user_id"].get<std::string>());
				std::unique_lock theLock{ GuildMembers::theMutex };
				if (!GuildMembers::cache.contains(theData.id)) {
					GuildMembers::cache[theData.id] = GuildMemberHolder{};
				}
				if (!GuildMembers::cache[theData.id].cache.contains(userId)) {
					GuildMembers::cache[theData.id].cache[userId] = std::make_unique<GuildMemberData>();
				}
				GuildMembers::cache[theData.id].cache[userId]->voiceChannelId = strtoull(value["channel_id"].get<std::string>());
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			for (auto& value: jsonObjectData["presences"]) {
				auto userId = strtoull(value["user"]["id"].get<std::string>());
				PresenceUpdateData* theDataNew{ new PresenceUpdateData{} };
				DiscordCoreAPI::parseObject(value, *theDataNew);
				theDataNew->guildId = theData.id;
				theData.presences[userId] = std::move(theDataNew);
			}
		}

		if (Channels::doWeCacheChannels) {
			theData.channels.clear();
			theData.channels.reserve(jsonObjectData["channels"].size());
			for (auto& value: jsonObjectData["channels"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				newData.guildId = theData.id;
				theData.channels.emplace_back(newData.id);
				Channels::insertChannel(std::make_unique<ChannelData>(std::move(newData)));
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Guild& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			theData.stickers.clear();
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.threads.push_back(newData);
			}
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: jsonObjectData["stickers"]) {
				Sticker newData{};
				parseObject(value, newData);
				theData.stickers.push_back(newData);
			}
		}

		if (jsonObjectData.contains("guild_scheduled_events") && !jsonObjectData["guild_scheduled_events"].is_null()) {
			theData.guildScheduledEvents.clear();
			for (auto& value: jsonObjectData["guild_scheduled_events"]) {
				GuildScheduledEvent newData{};
				parseObject(value, newData);
				theData.guildScheduledEvents.push_back(newData);
			}
		}

		if (jsonObjectData.contains("stage_instance") && !jsonObjectData["stage_instance"].is_null()) {
			theData.stageInstances.clear();
			for (auto& value: jsonObjectData["stage_instance"]) {
				StageInstance newData{};
				parseObject(value, newData);
				theData.stageInstances.push_back(newData);
			}
		}

		if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
			theData.emoji.clear();
			for (auto& value: jsonObjectData["emoji"]) {
				EmojiData newData{};
				parseObject(value, newData);
				theData.emoji.push_back(newData.id);
			}
		}

		if (Roles::doWeCacheRoles) {
			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				theData.roles.clear();

				for (auto& value: jsonObjectData["roles"]) {
					std::unique_ptr<RoleData> newData{ std::make_unique<RoleData>() };
					parseObject(value, *newData);
					theData.roles.push_back(newData->id);
					auto theRole = newData.get();
					Roles::insertRole(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				theData.members.clear();
				for (auto& value: jsonObjectData["members"]) {
					std::unique_ptr<GuildMemberData> newData{ std::make_unique<GuildMemberData>() };
					parseObject(value, *newData);
					newData->guildId = theData.id;
					auto theMember = newData.get();
					theData.members.push_back(newData->id);
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
				for (auto& value: jsonObjectData["voice_states"]) {
					auto userId = strtoull(value["user_id"].get<std::string>());
					if (!GuildMembers::cache.contains(theData.id)) {
						GuildMembers::cache[theData.id] = GuildMemberHolder{};
					}
					if (!GuildMembers::cache[theData.id].cache.contains(userId)) {
						GuildMembers::cache[theData.id].cache[userId] = std::make_unique<GuildMemberData>();
					}
					GuildMembers::cache[theData.id].cache[userId]->voiceChannelId = strtoull(value["channel_id"].get<std::string>());
				}
			}
		}

		if (Channels::doWeCacheChannels) {
			if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
				theData.channels.clear();

				for (auto& value: jsonObjectData["channels"]) {
					std::unique_ptr<ChannelData> newData{ std::make_unique<ChannelData>() };
					parseObject(value, *newData);
					newData->guildId = theData.id;
					theData.channels.push_back(newData->id);
					Channels::insertChannel(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
				theData.presences.clear();
				for (auto& value: jsonObjectData["presences"]) {
					PresenceUpdateData* newData{ new PresenceUpdateData{} };
					DiscordCoreAPI::parseObject(value, *newData);
					theData.presences[strtoull(value["user"]["id"].get<std::string>())] = std::move(newData);
				}
			}
		}

		if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
			theData.afkChannelId = stoull(jsonObjectData["afk_channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			theData.icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("icon_hash") && !jsonObjectData["icon_hash"].is_null()) {
			theData.icon = jsonObjectData["icon_hash"].get<std::string>();
		}

		if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
			theData.splash = jsonObjectData["splash"].get<std::string>();
		}

		if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
			theData.discoverySplash = jsonObjectData["discovery_splash"].get<std::string>();
		}

		if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
			theData.flags = setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));
		}

		if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
			theData.ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
		}

		if (jsonObjectData.contains("preferred_locale") && !jsonObjectData["preferred_locale"].is_null()) {
			theData.preferredLocale = jsonObjectData["preferred_locale"].get<std::string>();
		}

		if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData["public_updates_channel_id"].is_null()) {
			theData.publicUpdatesChannelId = stoull(jsonObjectData["public_updates_channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData["vanity_url_code"].is_null()) {
			theData.vanityUrlCode = jsonObjectData["vanity_url_code"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("banner") && !jsonObjectData["banner"].is_null()) {
			theData.banner = jsonObjectData["banner"].get<std::string>();
		}

		if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData["rule_Channel_id"].is_null()) {
			theData.rulesChannelId = stoull(jsonObjectData["rule_Channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
			theData.joinedAt = jsonObjectData["joined_at"].get<std::string>();
		}

		if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData["widget_channel_id"].is_null()) {
			theData.widgetChannelId = stoull(jsonObjectData["widget_channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("system_channel_id") && !jsonObjectData["system_channel_id"].is_null()) {
			theData.systemChannelId = stoull(jsonObjectData["system_channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
			theData.region = jsonObjectData["region"].get<std::string>();
		}

		if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
			theData.afkChannelId = stoull(jsonObjectData["afk_channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
			theData.region = jsonObjectData["region"].get<std::string>();
		}

		if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
			for (auto& value: jsonObjectData["features"].get<std::vector<std::string>>()) {
				theData.features.emplace_back(std::string{ value });
			}
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			theData.permissions = jsonObjectData["permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("afk_timeout") && !jsonObjectData["afk_timeout"].is_null()) {
			theData.afkTimeOut = jsonObjectData["afk_timeout"].get<AfkTimeOutDurations>();
		}

		if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
			theData.flags = setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));
		}

		if (jsonObjectData.contains("widget_enabled") && !jsonObjectData["widget_enabled"].is_null()) {
			theData.flags = setBool(theData.flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));
		}

		if (jsonObjectData.contains("verification_level") && !jsonObjectData["verification_level"].is_null()) {
			theData.verificationLevel = jsonObjectData["verification_level"].get<VerificationLevel>();
		}

		if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData["default_message_notification_level"].is_null()) {
			theData.defaultMessageNotifications = jsonObjectData["default_message_notification_level"].get<DefaultMessageNotificationLevel>();
		}

		if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData["explicit_content_filter_level"].is_null()) {
			theData.explicitContentFilter = jsonObjectData["explicit_content_filter_level"].get<ExplicitContentFilterLevel>();
		}

		if (jsonObjectData.contains("mfa_level") && !jsonObjectData["mfa_level"].is_null()) {
			theData.mfaLevel = jsonObjectData["mfa_level"].get<MFALevel>();
		}

		if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData["system_channel_flags"].is_null()) {
			theData.systemChannelFlags = jsonObjectData["system_channel_flags"].get<SystemChannelFlags>();
		}

		if (jsonObjectData.contains("large") && !jsonObjectData["large"].is_null()) {
			theData.flags = setBool(theData.flags, GuildFlags::Large, getBool(jsonObjectData, "large"));
		}

		if (jsonObjectData.contains("unavailable") && !jsonObjectData["unavailable"].is_null()) {
			theData.flags = setBool(theData.flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));
		}

		if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
			theData.memberCount = jsonObjectData["member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_presences") && !jsonObjectData["max_presences"].is_null()) {
			theData.maxPresences = jsonObjectData["max_presences"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_members") && !jsonObjectData["max_members"].is_null()) {
			theData.maxMembers = jsonObjectData["max_members"].get<int32_t>();
		}

		if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData["premium_subscription_count"].is_null()) {
			theData.premiumSubscriptionCount = jsonObjectData["premium_subscription_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("premium_tier") && !jsonObjectData["premium_tier"].is_null()) {
			theData.premiumTier = jsonObjectData["premium_tier"].get<PremiumTier>();
		}

		if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData["max_video_channel_users"].is_null()) {
			theData.maxVideoChannelUsers = jsonObjectData["max_video_channel_users"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
			theData.approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
			theData.approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("welcome_screen") && !jsonObjectData["welcome_screen"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["welcome_screen"], theData.welcomeScreen);
		}

		if (jsonObjectData.contains("nsfw_level") && !jsonObjectData["nsfw_level"].is_null()) {
			theData.nsfwLevel = jsonObjectData["nsfw_level"].get<GuildNSFWLevel>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildScheduledEventMetadata& theData) {
		theData.location = getString(jsonObjectData, "location");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AttachmentData& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.filename = getString(jsonObjectData, "filename");

		if (jsonObjectData.contains("content_type") && !jsonObjectData["content_type"].is_null()) {
			theData.contentType = getString(jsonObjectData, "content_type");
		}

		theData.ephemeral = getBool(jsonObjectData, "ephemeral");

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("size") && !jsonObjectData["size"].is_null()) {
			theData.size = jsonObjectData["size"].get<int32_t>();
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
			theData.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
			theData.width = jsonObjectData["width"].get<int32_t>();
		}

		if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
			theData.height = jsonObjectData["height"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommandVector& theData) {
		theData.theApplicationCommands.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			ApplicationCommand newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theApplicationCommands.push_back(newData);
		}
		theData.theApplicationCommands.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AutoModerationActionExecutionEventData& theData) {
		if (jsonObjectData.contains("alert_system_message_id") && !jsonObjectData["alert_system_message_id"].is_null()) {
			theData.alertSystemMessageId = stoull(jsonObjectData["alert_system_message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("rule_trigger_type") && !jsonObjectData["rule_trigger_type"].is_null()) {
			theData.ruleTriggerType = jsonObjectData["rule_trigger_type"].get<TriggerType>();
		}

		if (jsonObjectData.contains("matched_keyword") && !jsonObjectData["matched_keyword"].is_null()) {
			theData.matchedKeyword = jsonObjectData["matched_keyword"].get<std::string>();
		}

		if (jsonObjectData.contains("matched_content") && !jsonObjectData["matched_content"].is_null()) {
			theData.matchedContent = jsonObjectData["matched_content"].get<std::string>();
		}

		if (jsonObjectData.contains("action") && !jsonObjectData["action"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["action"], theData.action);
		}

		if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
			theData.content = jsonObjectData["content"].get<std::string>();
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			theData.messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("rule_id") && !jsonObjectData["rule_id"].is_null()) {
			theData.ruleId = stoull(jsonObjectData["rule_id"].get<std::string>());
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			theData.userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AutoModerationRule& theData) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		theData.enabled = getBool(jsonObjectData, "enabled");

		if (jsonObjectData.contains("trigger_type") && !jsonObjectData["trigger_type"].is_null()) {
			theData.triggerType = static_cast<TriggerType>(jsonObjectData["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData.contains("event_type") && !jsonObjectData["event_type"].is_null()) {
			theData.eventType = static_cast<EventType>(jsonObjectData["event_type"].get<uint64_t>());
		}

		if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
			theData.creatorId = jsonObjectData["creator_id"].get<uint64_t>();
		}

		if (jsonObjectData.contains("actions") && !jsonObjectData["actions"].is_null()) {
			theData.actions.clear();
			for (auto& value: jsonObjectData["actions"]) {
				ActionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.actions.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("exempt_roles") && !jsonObjectData["exempt_roles"].is_null()) {
			for (auto& value: jsonObjectData["exempt_roles"]) {
				theData.exemptRoles.emplace_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData.contains("trigger_metadata") && !jsonObjectData["trigger_metadata"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["trigger_metadata"], theData.triggerMetaData);
		}

		if (jsonObjectData.contains("exempt_channels") && !jsonObjectData["exempt_channels"].is_null()) {
			for (auto& value: jsonObjectData["exempt_channels"]) {
				theData.exemptChannels.emplace_back(value.get<uint64_t>());
			}
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = jsonObjectData["guild_id"].get<uint64_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AutoModerationRuleVector& theData) {
		theData.theAutoModerationRules.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			AutoModerationRule newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theAutoModerationRules.push_back(newData);
		}
		theData.theAutoModerationRules.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Channel& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			if (jsonObjectData["id"].is_string()) {
				theData.id = stoull(jsonObjectData["id"].get<std::string>());
			} else {
				theData.id = jsonObjectData["id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			theData.flags = jsonObjectData["flags"].get<int8_t>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ChannelType>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("default_auto_archive_duration") && !jsonObjectData["default_auto_archive_duration"].is_null()) {
			theData.defaultAutoArchiveDuration = jsonObjectData["default_auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
			theData.position = jsonObjectData["position"].get<int32_t>();
		}

		if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
			theData.permissionOverwrites.clear();
			for (auto& value: jsonObjectData["permission_overwrites"]) {
				OverWriteData* newData{ new OverWriteData{} };
				DiscordCoreAPI::parseObject(value, *newData);
				theData.permissionOverwrites.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
			theData.topic = jsonObjectData["topic"].get<std::string>();
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			theData.permissions = jsonObjectData["permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
			theData.flags = setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));
		}

		if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
			theData.lastMessageId = jsonObjectData["last_message_id"].get<std::string>();
		}

		if (jsonObjectData.contains("bitrate") && !jsonObjectData["bitrate"].is_null()) {
			theData.bitrate = jsonObjectData["bitrate"].get<int32_t>();
		}

		if (jsonObjectData.contains("user_limit") && !jsonObjectData["user_limit"].is_null()) {
			theData.userLimit = jsonObjectData["user_limit"].get<int32_t>();
		}

		if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData["rate_limit_per_user"].is_null()) {
			theData.rateLimitPerUser = jsonObjectData["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData.contains("recipients") && !jsonObjectData["recipients"].is_null()) {
			theData.recipients.clear();
			for (auto& value: jsonObjectData["recipients"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.recipients[newData.id] = std::move(newData);
			}
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			theData.icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
			theData.ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
			if (jsonObjectData["parent_id"].is_string()) {
				theData.parentId = stoull(jsonObjectData["parent_id"].get<std::string>());
			} else {
				theData.parentId = jsonObjectData["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
			theData.lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
			theData.rtcRegion = jsonObjectData["rtc_region"].get<std::string>();
		}

		if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData["video_quality_mode"].is_null()) {
			theData.videoQualityMode = jsonObjectData["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData.contains("message_count") && !jsonObjectData["message_count"].is_null()) {
			theData.messageCount = jsonObjectData["message_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
			theData.memberCount = jsonObjectData["member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("thread_metadata") && !jsonObjectData["thread_metadata"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["thread_metadata"], theData.threadMetadata);
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ChannelVector& theData) {
		theData.theChannels.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			Channel newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theChannels.push_back(newData);
		}
		theData.theChannels.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildVector& theData) {
		theData.theGuilds.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			Guild newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuilds.push_back(newData);
		}
		theData.theGuilds.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildMemberVector& theData) {
		theData.theGuildMembers.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			GuildMember newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuildMembers.push_back(newData);
		}
		theData.theGuildMembers.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildScheduledEvent& theData) {
		if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
			theData.privacyLevel = jsonObjectData["privacy_level"].get<GuildScheduledEventPrivacyLevel>();
		}

		if (jsonObjectData.contains("entity_type") && !jsonObjectData["entity_type"].is_null()) {
			theData.entityType = jsonObjectData["entity_type"].get<GuildScheduledEventEntityType>();
		}

		if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
			theData.status = jsonObjectData["status"].get<GuildScheduledEventStatus>();
		}

		parseObject(jsonObjectData["entity_metadata"], theData.entityMetadata);

		if (jsonObjectData.contains("scheduled_start_time") && !jsonObjectData["scheduled_start_time"].is_null()) {
			theData.scheduledStartTime = jsonObjectData["scheduled_start_time"].get<std::string>();
		}

		if (jsonObjectData.contains("scheduled_end_time") && !jsonObjectData["scheduled_end_time"].is_null()) {
			theData.scheduledEndTime = jsonObjectData["scheduled_end_time"].get<std::string>();
		}

		if (jsonObjectData.contains("user_count") && !jsonObjectData["user_count"].is_null()) {
			theData.userCount = jsonObjectData["user_count"].get<uint32_t>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
			theData.creatorId = jsonObjectData["creator_id"].get<std::string>();
		}

		if (jsonObjectData.contains("entity_id") && !jsonObjectData["entity_id"].is_null()) {
			theData.entityId = jsonObjectData["entity_id"].get<std::string>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["creator"], theData.creator);
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Message& theData) {
		if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
			theData.content = jsonObjectData["content"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["author"], theData.author);
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			theData.timestamp = jsonObjectData["timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
			theData.editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();
		}

		theData.tts = getBool(jsonObjectData, "tts");

		theData.mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
			theData.mentions.clear();
			for (auto& value: jsonObjectData["mentions"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.mentions.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
			theData.mentionRoles.clear();
			for (auto& value: jsonObjectData["mention_roles"]) {
				theData.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
			theData.mentionChannels.clear();
			for (auto& value: jsonObjectData["mention_channels"]) {
				ChannelMentionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: jsonObjectData["attachments"]) {
				AttachmentData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: jsonObjectData["embeds"]) {
				EmbedData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: jsonObjectData["reactions"]) {
				ReactionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.reactions.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
			theData.nonce = jsonObjectData["nonce"].get<std::string>();
		}

		theData.pinned = getBool(jsonObjectData, "pinned");

		if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
			theData.webHookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<MessageType>();
		}

		if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["activity"], theData.activity);
		}

		if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["application"], theData.application);
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["message_reference"], theData.messageReference);
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			theData.flags = jsonObjectData["flags"].get<int32_t>();
		}

		if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: jsonObjectData["sticker_items"]) {
				StickerItemData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["interaction"], theData.interaction);
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			theData.components.clear();
			for (auto& value: jsonObjectData["components"]) {
				ActionRowData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.components.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["thread"], theData.thread);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, MessageVector& theData) {
		theData.theMessages.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			Message newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theMessages.push_back(newData);
		}
		theData.theMessages.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Reaction& theData) {
		if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
			theData.count = jsonObjectData["count"].get<int32_t>();
		}

		theData.me = getBool(jsonObjectData, "me");

		if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["emoji"], theData.emoji);
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			theData.userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			theData.messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ReactionVector& theData) {
		theData.theReactions.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			Reaction newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theReactions.push_back(newData);
		}
		theData.theReactions.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, RoleVector& theData) {
		theData.theRoles.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			Role newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theRoles.push_back(newData);
		}
		theData.theRoles.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, StageInstance& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
			theData.topic = jsonObjectData["topic"].get<std::string>();
		}

		if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
			theData.privacyLevel = jsonObjectData["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		theData.discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Sticker& theData) {
		if (jsonObjectData.contains("asset") && !jsonObjectData["asset"].is_null()) {
			theData.asset = jsonObjectData["asset"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
			theData.formatType = jsonObjectData["format_type"].get<StickerFormatType>();
		}

		if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
			theData.stickerFlags = setBool(theData.stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("pack_id") && !jsonObjectData["pack_id"].is_null()) {
			theData.packId = jsonObjectData["pack_id"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<StickerType>();
		}

		if (jsonObjectData.contains("sort_value") && !jsonObjectData["sort_value"].is_null()) {
			theData.sortValue = jsonObjectData["sort_value"].get<int32_t>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, StickerVector& theData) {
		theData.theStickers.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			Sticker newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theStickers.push_back(newData);
		}
		theData.theStickers.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Thread& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			if (jsonObjectData["id"].is_string()) {
				theData.id = stoull(jsonObjectData["id"].get<std::string>());
			} else {
				theData.id = jsonObjectData["id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ChannelType>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
			theData.position = jsonObjectData["position"].get<int32_t>();
		}

		if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
			theData.permissionOverwrites.clear();
			for (auto& value: jsonObjectData["permission_overwrites"]) {
				OverWriteData* newData{ new OverWriteData{} };
				DiscordCoreAPI::parseObject(value, *newData);
				theData.permissionOverwrites.push_back(newData);
			}
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
			theData.topic = jsonObjectData["topic"].get<std::string>();
		}

		if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
			theData.flags = setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));
		}

		if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
			theData.lastMessageId = jsonObjectData["last_message_id"].get<std::string>();
		}

		if (jsonObjectData.contains("bitrate") && !jsonObjectData["bitrate"].is_null()) {
			theData.bitrate = jsonObjectData["bitrate"].get<int32_t>();
		}

		if (jsonObjectData.contains("user_limit") && !jsonObjectData["user_limit"].is_null()) {
			theData.userLimit = jsonObjectData["user_limit"].get<int32_t>();
		}

		if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData["rate_limit_per_user"].is_null()) {
			theData.rateLimitPerUser = jsonObjectData["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData.contains("recipients") && !jsonObjectData["recipients"].is_null()) {
			theData.recipients.clear();
			for (auto& value: jsonObjectData["recipients"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.recipients[newData.id] = std::move(newData);
			}
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			theData.icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
			theData.ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
			if (jsonObjectData["parent_id"].is_string()) {
				theData.parentId = stoull(jsonObjectData["parent_id"].get<std::string>());
			} else {
				theData.parentId = jsonObjectData["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
			theData.lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
			theData.rtcRegion = jsonObjectData["rtc_region"].get<std::string>();
		}

		if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData["video_quality_mode"].is_null()) {
			theData.videoQualityMode = jsonObjectData["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData.contains("message_count") && !jsonObjectData["message_count"].is_null()) {
			theData.messageCount = jsonObjectData["message_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
			theData.memberCount = jsonObjectData["member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("thread_metadata") && !jsonObjectData["thread_metadata"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["thread_metadata"], theData.threadMetadata);
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, UserVector& theData) {
		theData.theUsers.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			User newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theUsers.push_back(newData);
		}
		theData.theUsers.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, WebHook& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<WebHookType>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
			theData.avatar = jsonObjectData["avatar"].get<std::string>();
		}

		if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
			theData.token = jsonObjectData["token"].get<std::string>();
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["source_guild"], theData.sourceGuild);
		}

		if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["source_channel"], theData.sourceChannel);
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, WebHookVector& theData) {
		theData.theWebHooks.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			WebHook newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theWebHooks.push_back(newData);
		}
		theData.theWebHooks.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, RoleTagsData& theData) {
		theData.botId = getString(jsonObjectData, "bot_id");

		theData.integrationId = getString(jsonObjectData, "integration_id");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedFooterData& theData) {
		if (jsonObjectData.contains("text") && !jsonObjectData["text"].is_null()) {
			theData.text = jsonObjectData["text"].get<std::string>();
		}
		if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
			theData.iconUrl = jsonObjectData["icon_url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
			theData.proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedImageData& theData) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
			theData.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
			theData.width = jsonObjectData["width"].get<int32_t>();
		}

		if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
			theData.height = jsonObjectData["height"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedThumbnailData& theData) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
			theData.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
			theData.width = jsonObjectData["width"].get<int32_t>();
		}

		if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
			theData.height = jsonObjectData["height"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedVideoData& theData) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
			theData.proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
			theData.width = jsonObjectData["width"].get<int32_t>();
		}

		if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
			theData.height = jsonObjectData["height"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedProviderData& theData) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedAuthorData& theData) {
		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
			theData.proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
			theData.iconUrl = jsonObjectData["icon_url"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedFieldData& theData) {
		if (jsonObjectData.contains("inline") && !jsonObjectData["inline"].is_null()) {
			auto theString = jsonObjectData["inline"];
			if (jsonObjectData["inline"].is_string()) {
				if (theString == "true") {
					theData.Inline = true;
				} else {
					theData.Inline = false;
				}
			} else {
				theData.Inline = theString;
			}
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
			theData.value = jsonObjectData["value"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmbedData& theData) {
		if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
			theData.title = jsonObjectData["title"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			theData.timestamp = jsonObjectData["timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
			theData.hexColorValue = jsonObjectData["color"].get<int32_t>();
		}

		if (jsonObjectData.contains("footer") && !jsonObjectData["footer"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["footer"], theData.footer);
		}

		if (jsonObjectData.contains("image") && !jsonObjectData["image"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["image"], theData.image);
		}

		if (jsonObjectData.contains("provider") && !jsonObjectData["provider"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["provider"], theData.provider);
		}

		if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["thumbnail"], theData.thumbnail);
		}

		if (jsonObjectData.contains("video") && !jsonObjectData["video"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["video"], theData.video);
		}

		if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["author"], theData.author);
		}

		if (jsonObjectData.contains("fields") && !jsonObjectData["fields"].is_null()) {
			theData.fields.clear();
			for (auto& value: jsonObjectData["fields"]) {
				EmbedFieldData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.fields.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, MessageReferenceData& theData) {
		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			theData.messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		theData.failIfNotExists = getBool(jsonObjectData, "fail_if_not_exists");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ThreadMetadataData& theData) {
		theData.archived = getBool(jsonObjectData, "archived");

		theData.invitable = getBool(jsonObjectData, "invitable");

		if (jsonObjectData.contains("auto_archive_duration") && !jsonObjectData["auto_archive_duration"].is_null()) {
			theData.autoArchiveDuration = jsonObjectData["auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData.contains("archive_timestamp") && !jsonObjectData["archive_timestamp"].is_null()) {
			theData.archiveTimestamp = jsonObjectData["archive_timestamp"].get<std::string>();
		}

		theData.locked = getBool(jsonObjectData, "locked");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ThreadMemberData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			theData.userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("join_timestamp") && !jsonObjectData["join_timestamp"].is_null()) {
			theData.joinTimestamp = jsonObjectData["join_timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			theData.flags = jsonObjectData["flags"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ThreadMemberDataVector& theData) {
		theData.theThreadMemberDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			ThreadMemberData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theThreadMemberDatas.push_back(newData);
		}
		theData.theThreadMemberDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, VoiceStateData& theData) {
		theData.requestToSpeakTimestamp = getString(jsonObjectData, "request_to_speak_timestamp");

		theData.channelId = strtoull(getString(jsonObjectData, "channel_id"));

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.selfStream = getBool(jsonObjectData, "self_stream");

		theData.userId = strtoull(getString(jsonObjectData, "user_id"));

		theData.selfVideo = getBool(jsonObjectData, "self_video");

		theData.sessionId = getString(jsonObjectData, "session_id");

		theData.selfDeaf = getBool(jsonObjectData, "self_deaf");

		theData.selfMute = getBool(jsonObjectData, "self_mute");

		theData.suppress = getBool(jsonObjectData, "suppress");

		theData.deaf = getBool(jsonObjectData, "deaf");

		theData.mute = getBool(jsonObjectData, "mute");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ActiveThreadsData& theData) {
		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			theData.members.clear();
			for (auto& value: jsonObjectData["members"]) {
				ThreadMemberData newData{};
				theData.members.emplace_back(newData);
			}
		}

		theData.hasMore = getBool(jsonObjectData, "has_more");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ArchivedThreadsData& theData) {
		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			theData.members.clear();
			for (auto& value: jsonObjectData["members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.members.emplace_back(newData);
			}
		}

		theData.hasMore = getBool(jsonObjectData, "has_more");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ActionMetaData& theData) {
		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("duration_seconds") && !jsonObjectData["duration_seconds"].is_null()) {
			theData.durationSeconds = jsonObjectData["duration_seconds"].get<int64_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, TriggerMetaData& theData) {
		if (jsonObjectData.contains("keyword_filter") && !jsonObjectData["keyword_filter"].is_null()) {
			for (auto& value: jsonObjectData["keyword_filter"]) {
				theData.keywordFilter.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("presets") && !jsonObjectData["presets"].is_null()) {
			for (auto& value: jsonObjectData["presets"]) {
				theData.presets.emplace_back(static_cast<KeywordPresetType>(value.get<uint64_t>()));
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ActionData& theData) {
		if (jsonObjectData.contains("metadata") && !jsonObjectData["metadata"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["metadata"], theData.metadata);
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = static_cast<ActionType>(jsonObjectData["type"].get<int64_t>());
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AutoModerationRuleData& theData) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		theData.enabled = getBool(jsonObjectData, "enabled");

		if (jsonObjectData.contains("trigger_type") && !jsonObjectData["trigger_type"].is_null()) {
			theData.triggerType = static_cast<TriggerType>(jsonObjectData["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData.contains("event_type") && !jsonObjectData["event_type"].is_null()) {
			theData.eventType = static_cast<EventType>(jsonObjectData["event_type"].get<uint64_t>());
		}

		if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
			theData.creatorId = jsonObjectData["creator_id"].get<Snowflake>();
		}

		if (jsonObjectData.contains("actions") && !jsonObjectData["actions"].is_null()) {
			for (auto& value: jsonObjectData["actions"]) {
				ActionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.actions.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("exempt_roles") && !jsonObjectData["exempt_roles"].is_null()) {
			for (auto& value: jsonObjectData["exempt_roles"]) {
				theData.exemptRoles.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData.contains("trigger_metadata") && !jsonObjectData["trigger_metadata"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["trigger_metadata"], theData.triggerMetaData);
		}

		if (jsonObjectData.contains("exempt_channels") && !jsonObjectData["exempt_channels"].is_null()) {
			for (auto& value: jsonObjectData["exempt_channels"]) {
				theData.exemptChannels.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = jsonObjectData["guild_id"].get<Snowflake>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommandPermissionData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		theData.permission = getBool(jsonObjectData, "permission");

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ApplicationCommandPermissionType>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsData& theData) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			theData.permissions.clear();
			for (auto& value: jsonObjectData["permissions"]) {
				ApplicationCommandPermissionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.permissions.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsDataVector& theData) {
		theData.theGuildApplicationCommandPermissionsDatas.clear();
		for (auto& value: jsonObjectData) {
			GuildApplicationCommandPermissionsData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuildApplicationCommandPermissionsDatas.emplace_back(newData);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmojiData& theData) {
		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.name = getString(jsonObjectData, "name");

		for (auto& value: jsonObjectData["roles"]) {
			RoleData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.roles.emplace_back(newData);
		}

		DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);

		theData.requireColons = getBool(jsonObjectData, "require_colons");

		theData.managed = getBool(jsonObjectData, "managed");

		theData.animated = getBool(jsonObjectData, "animated");

		theData.available = getBool(jsonObjectData, "available");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, EmojiDataVector& theData) {
		theData.theEmojiDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			EmojiData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theEmojiDatas.push_back(newData);
		}
		theData.theEmojiDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ReactionData& theData) {
		if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
			theData.count = jsonObjectData["count"].get<int32_t>();
		}

		theData.me = getBool(jsonObjectData, "me");

		if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["emoji"], theData.emoji);
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			theData.userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			theData.messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, VoiceRegionData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		theData.custom = getBool(jsonObjectData, "custom");

		theData.deprecated = getBool(jsonObjectData, "deprecated");

		theData.optimal = getBool(jsonObjectData, "optimal");

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, VoiceRegionDataVector& theData) {
		theData.theVoiceRegionDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			VoiceRegionData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theVoiceRegionDatas.push_back(newData);
		}
		theData.theVoiceRegionDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, MessageActivityData& theData) {
		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<MessageActivityType>();
		}

		if (jsonObjectData.contains("party_id") && !jsonObjectData["party_id"].is_null()) {
			theData.partyId = jsonObjectData["party_id"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, BanData& theData) {
		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}

		if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
			theData.reason = jsonObjectData["reason"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, BanDataVector& theData) {
		theData.theBanDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			BanData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theBanDatas.push_back(newData);
		}
		theData.theBanDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, TeamMembersObjectData& theData) {
		if (jsonObjectData.contains("membership_state") && !jsonObjectData["membership_state"].is_null()) {
			theData.membershipState = jsonObjectData["membership_state"].get<int32_t>();
		}

		if (jsonObjectData.contains("team_id") && !jsonObjectData["team_id"].is_null()) {
			theData.teamId = jsonObjectData["team_id"].get<std::string>();
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			theData.permissions.clear();
			for (auto& value: jsonObjectData["permissions"]) {
				theData.permissions.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, TeamObjectData& theData) {
		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			theData.icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			theData.members.clear();
			for (auto& value: jsonObjectData["members"]) {
				TeamMembersObjectData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.members.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("owner_user_id") && !jsonObjectData["owner_user_id"].is_null()) {
			theData.ownerUserId = jsonObjectData["owner_user_id"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, InstallParamsData& theData) {
		if (jsonObjectData.contains("scopes") && !jsonObjectData["scopes"].is_null()) {
			theData.scopes = jsonObjectData["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
			theData.permissions = jsonObjectData["name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationData& theData) {
		if (jsonObjectData.contains("params") && !jsonObjectData["params"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["params"], theData.params);
		}

		if (jsonObjectData.contains("tags") && !jsonObjectData["tags"].is_null()) {
			theData.tags = jsonObjectData["tags"].get<std::vector<std::string>>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			theData.icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("rpc_origins") && !jsonObjectData["rpc_origins"].is_null()) {
			theData.rpcOrigins.clear();
			for (auto& value: jsonObjectData["rpc_origins"]) {
				theData.rpcOrigins.emplace_back(value.get<std::string>());
			}
		}

		theData.botPublic = getBool(jsonObjectData, "bot_public");

		theData.botRequireCodeGrant = getBool(jsonObjectData, "bot_require_code_grant");

		if (jsonObjectData.contains("terms_of_service_url") && !jsonObjectData["terms_of_service_url"].is_null()) {
			theData.termsOfServiceUrl = jsonObjectData["terms_of_service_url"].get<std::string>();
		}

		if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData["privacy_policy_url"].is_null()) {
			theData.privacyPolicyUrl = jsonObjectData["privacy_policy_url"].get<std::string>();
		}

		if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["owner"], theData.owner);
		}

		if (jsonObjectData.contains("summary") && !jsonObjectData["summary"].is_null()) {
			theData.summary = jsonObjectData["summary"].get<std::string>();
		}

		if (jsonObjectData.contains("verify_key") && !jsonObjectData["verify_key"].is_null()) {
			theData.verifyKey = jsonObjectData["verify_key"].get<std::string>();
		}

		if (jsonObjectData.contains("team") && !jsonObjectData["team"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["team"], theData.team);
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData["primary_sku_id"].is_null()) {
			theData.primarySkuId = jsonObjectData["primary_sku_id"].get<std::string>();
		}

		if (jsonObjectData.contains("slug") && !jsonObjectData["slug"].is_null()) {
			theData.slug = jsonObjectData["slug"].get<std::string>();
		}

		if (jsonObjectData.contains("cover_image") && !jsonObjectData["cover_image"].is_null()) {
			theData.coverImage = jsonObjectData["cover_image"].get<std::string>();
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			theData.flags = jsonObjectData["flags"].get<ApplicationFlags>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AuthorizationInfoData& theData) {
		if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["application"], theData.application);
		}

		if (jsonObjectData.contains("scopes") && !jsonObjectData["scopes"].is_null()) {
			theData.scopes = jsonObjectData["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData.contains("expires") && !jsonObjectData["expires"].is_null()) {
			theData.expires = jsonObjectData["expires"];
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}
	};

	template<> void parseObject(nlohmann::json& jsonObjectData, AccountData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildWidgetData& theData) {
		theData.enabled = getBool(jsonObjectData, "enabled");

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}
	};

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildWidgetImageData& theData) {
		if (jsonObjectData.contains("widget_image") && !jsonObjectData["widget_image"].is_null()) {
			theData.url = jsonObjectData["widget_image"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, IntegrationData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<std::string>();
		}

		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.syncing = getBool(jsonObjectData, "syncing");

		if (jsonObjectData.contains("role_id") && !jsonObjectData["role_id"].is_null()) {
			theData.roleId = stoull(jsonObjectData["role_id"].get<std::string>());
		}

		theData.enableEmoticons = getBool(jsonObjectData, "enable_emoticons");

		if (jsonObjectData.contains("expire_behavior") && !jsonObjectData["expire_behavior"].is_null()) {
			theData.expireBehavior = jsonObjectData["expire_behavior"].get<int32_t>();
		}

		if (jsonObjectData.contains("expire_grace_period") && !jsonObjectData["expire_grace_period"].is_null()) {
			theData.expireGracePeriod = jsonObjectData["expire_grace_period"].get<int32_t>();
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}

		if (jsonObjectData.contains("account") && !jsonObjectData["account"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["account"], theData.account);
		}

		if (jsonObjectData.contains("synced_at") && !jsonObjectData["synced_at"].is_null()) {
			theData.syncedAt = jsonObjectData["synced_at"].get<std::string>();
		}

		if (jsonObjectData.contains("subscriber_count") && !jsonObjectData["subscriber_count"].is_null()) {
			theData.subscriberCount = jsonObjectData["subscriber_count"].get<int32_t>();
		}

		theData.revoked = getBool(jsonObjectData, "revoked");

		if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["application"], theData.application);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, IntegrationDataVector& theData) {
		theData.theIntegrationDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			IntegrationData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theIntegrationDatas.push_back(newData);
		}
		theData.theIntegrationDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, OptionalAuditEntryInfoData& theData) {
		if (jsonObjectData.contains("delete_member_days") && !jsonObjectData["delete_member_days"].is_null()) {
			theData.deleteMemberDays = jsonObjectData["delete_member_days"].get<std::string>();
		}

		if (jsonObjectData.contains("members_removed") && !jsonObjectData["members_removed"].is_null()) {
			theData.membersRemoved = jsonObjectData["members_removed"].get<std::string>();
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			theData.messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
			theData.count = jsonObjectData["count"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<std::string>();
		}

		if (jsonObjectData.contains("role_name") && !jsonObjectData["role_name"].is_null()) {
			theData.roleName = jsonObjectData["role_name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AuditLogChangeData& theData) {
		if (jsonObjectData.contains("new_value") && !jsonObjectData["new_value"].is_null()) {
			theData.newValue = jsonObjectData["new_value"];
		}

		if (jsonObjectData.contains("old_value") && !jsonObjectData["old_value"].is_null()) {
			theData.oldValue = jsonObjectData["old_value"];
		}

		if (jsonObjectData.contains("key") && !jsonObjectData["key"].is_null()) {
			theData.key = jsonObjectData["key"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildPruneCountData& theData) {
		if (jsonObjectData.contains("pruned") && !jsonObjectData["pruned"].is_null()) {
			theData.count = jsonObjectData["pruned"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AuditLogEntryData& theData) {
		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			theData.targetId = stoull(jsonObjectData["target_id"].get<std::string>());
		}

		if (jsonObjectData.contains("changes") && !jsonObjectData["changes"].is_null()) {
			theData.changes.clear();
			for (auto& value: jsonObjectData["changes"]) {
				AuditLogChangeData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.changes.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			theData.userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
			theData.createdTimeStamp = theData.getCreatedAtTimestamp(TimeFormat::LongDateTime);
		}

		if (jsonObjectData.contains("action_type") && !jsonObjectData["action_type"].is_null()) {
			theData.actionType = jsonObjectData["action_type"].get<AuditLogEvent>();
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["options"], theData.options);
		}

		if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
			theData.reason = jsonObjectData["reason"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, PartyData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("Size") && !jsonObjectData["Size"].is_null()) {
			theData.size[0] = jsonObjectData["Size"]["CurrentSize"].get<int32_t>();
			theData.size[1] = jsonObjectData["Size"]["MaxSize"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AssetsData& theData) {
		if (jsonObjectData.contains("LargeImage") && !jsonObjectData["LargeImage"].is_null()) {
			theData.largeImage = jsonObjectData["LargeImage"].get<std::string>();
		}

		if (jsonObjectData.contains("LargeText") && !jsonObjectData["LargeText"].is_null()) {
			theData.largeText = jsonObjectData["LargeText"].get<std::string>();
		}

		if (jsonObjectData.contains("SmallImage") && !jsonObjectData["SmallImage"].is_null()) {
			theData.smallImage = jsonObjectData["SmallImage"].get<std::string>();
		}

		if (jsonObjectData.contains("SmallText") && !jsonObjectData["SmallText"].is_null()) {
			theData.smallText = jsonObjectData["SmallText"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, SecretsData& theData) {
		if (jsonObjectData.contains("Join") && !jsonObjectData["Join"].is_null()) {
			theData.join = jsonObjectData["Join"].get<std::string>();
		}

		if (jsonObjectData.contains("Spectate") && !jsonObjectData["Spectate"].is_null()) {
			theData.spectate = jsonObjectData["Spectate"].get<std::string>();
		}

		if (jsonObjectData.contains("Match") && !jsonObjectData["Match"].is_null()) {
			theData.match = jsonObjectData["Match"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, TimestampData& theData) {
		if (jsonObjectData.contains("Start") && !jsonObjectData["Start"].is_null()) {
			theData.start = jsonObjectData["Start"].get<int64_t>();
		}

		if (jsonObjectData.contains("End") && !jsonObjectData["End"].is_null()) {
			theData.end = jsonObjectData["End"].get<int64_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ActivityData& theData) {
		if (jsonObjectData.contains("Name") && !jsonObjectData["Name"].is_null()) {
			theData.name = jsonObjectData["Name"].get<std::string>();
		}

		if (jsonObjectData.contains("Timestamps") && !jsonObjectData["Timestamps"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["Timestamps"], theData.timestamps);
		}

		if (jsonObjectData.contains("ApplicationId") && !jsonObjectData["ApplicationId"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["ApplicationId"].get<std::string>());
		}

		if (jsonObjectData.contains("Details") && !jsonObjectData["Details"].is_null()) {
			theData.details = jsonObjectData["Details"].get<std::string>();
		}

		if (jsonObjectData.contains("State") && !jsonObjectData["State"].is_null()) {
			theData.state = jsonObjectData["State"].get<std::string>();
		}

		if (jsonObjectData.contains("ActivityParty") && !jsonObjectData["ActivityParty"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["ActivityParty"], theData.party);
		}

		if (jsonObjectData.contains("ActivityAssets") && !jsonObjectData["ActivityAssets"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["ActivityAssets"], theData.assets);
		}

		if (jsonObjectData.contains("ActivitySecrets") && !jsonObjectData["ActivitySecrets"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["ActivitySecrets"], theData.secrets);
		}

		theData.instance = getBool(jsonObjectData, "Instance");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, PresenceUpdateFlags& theData) {
		uint8_t theDataNew = static_cast<uint8_t>(theData);
		if (jsonObjectData.contains("desktop") && !jsonObjectData["desktop"].is_null()) {
			if (jsonObjectData["desktop"].get<std::string>() == "online") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Online, true));
			} else if (jsonObjectData["desktop"].get<std::string>() == "idle") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Idle, true));
			} else if (jsonObjectData["desktop"].get<std::string>() == "dnd") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Dnd, true));
			}
		}

		if (jsonObjectData.contains("mobile") && !jsonObjectData["mobile"].is_null()) {
			if (jsonObjectData["mobile"].get<std::string>() == "online") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Online, true));
			} else if (jsonObjectData["mobile"].get<std::string>() == "idle") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Idle, true));
			} else if (jsonObjectData["mobile"].get<std::string>() == "dnd") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Dnd, true));
			}
		}

		if (jsonObjectData.contains("web") && !jsonObjectData["web"].is_null()) {
			if (jsonObjectData["web"].get<std::string>() == "online") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Online, true));
			} else if (jsonObjectData["web"].get<std::string>() == "idle") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Idle, true));
			} else if (jsonObjectData["web"].get<std::string>() == "dnd") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Dnd, true));
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, WelcomeScreenChannelData& theData) {
		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("emoji_id") && !jsonObjectData["emoji_id"].is_null()) {
			theData.emojiId = stoull(jsonObjectData["emoji_id"].get<std::string>());
		}

		if (jsonObjectData.contains("emoji_name") && !jsonObjectData["emoji_name"].is_null()) {
			theData.emojiName = jsonObjectData["emoji_name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, WelcomeScreenData& theData) {
		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("welcome_channels") && !jsonObjectData["welcome_channels"].is_null()) {
			theData.welcomeChannels.clear();
			for (auto& value: jsonObjectData["welcome_channels"]) {
				WelcomeScreenChannelData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.welcomeChannels.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, PresenceUpdateData& theData) {

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
			if (jsonObjectData["status"].get<std::string>() == "online") {
				theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Online, true);
			} else if (jsonObjectData["status"].get<std::string>() == "idle") {
				theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Idle, true);
			}else if (jsonObjectData["status"].get<std::string>() == "dnd") {
				theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Dnd, true);
			}
		}

		if (jsonObjectData.contains("activities") && !jsonObjectData["activities"].is_null()) {
			theData.activities.clear();
			for (auto& value: jsonObjectData["activities"]) {
				ActivityData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.activities.emplace_back(newData);
			}
		}
		PresenceUpdateFlags theFlags{};
		if (jsonObjectData.contains("client_status") && !jsonObjectData["client_status"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["client_status"], theFlags);
		}
		theData.theStatus |= static_cast<uint8_t>(theFlags);
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, StageInstanceData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
			theData.topic = jsonObjectData["topic"].get<std::string>();
		}

		if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
			theData.privacyLevel = jsonObjectData["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		theData.discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, StickerData& theData) {
		theData.asset = getString(jsonObjectData, "asset");

		theData.description = getString(jsonObjectData, "description");

		theData.formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		theData.stickerFlags |= setBool(theData.stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.packId = getString(jsonObjectData, "pack_id");

		theData.type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		theData.sortValue = getUint32(jsonObjectData, "sort_value");

		theData.name = getString(jsonObjectData, "name");

		DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildPreviewData& theData) {
		if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
			theData.approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
			theData.approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
			theData.discoverySplash = jsonObjectData["discovery_splash"].get<std::string>();
		}

		if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
			theData.emojis.clear();
			for (auto& value: jsonObjectData["emojis"]) {
				EmojiData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.emojis.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			theData.emojis.clear();
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
			theData.features = jsonObjectData["features"].get<std::vector<std::string>>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
			theData.splash = jsonObjectData["splash"].get<std::string>();
		}

		if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
			theData.icon = jsonObjectData["icon"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildDataVector& theData) {
		theData.theGuildDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			GuildData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuildDatas.push_back(newData);
		}
		theData.theGuildDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildScheduledEventData& theData) {
		theData.privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		theData.status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		parseObject(jsonObjectData["entity_metadata"], theData.entityMetadata);

		theData.scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		theData.scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		theData.userCount = getUint32(jsonObjectData, "user_count");

		theData.channelId = strtoull(getString(jsonObjectData, "channel_id"));

		theData.creatorId = strtoull(getString(jsonObjectData, "creator_id"));

		theData.entityId = strtoull(getString(jsonObjectData, "entity_id"));

		theData.guildId = strtoull(getString(jsonObjectData, "guild_id"));

		theData.id = strtoull(getString(jsonObjectData, "id"));

		theData.description = getString(jsonObjectData, "description");

		DiscordCoreAPI::parseObject(jsonObjectData["creator"], theData.creator);

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildScheduledEventUserData& theData) {
		if (jsonObjectData.contains("guild_scheduled_event_id") && !jsonObjectData["guild_scheduled_event_id"].is_null()) {
			theData.guildScheduledEventId = jsonObjectData["guild_scheduled_event_id"].get<std::string>();
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildScheduledEventUserDataVector& theData) {
		theData.theGuildScheduledEventUserDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			GuildScheduledEventUserData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuildScheduledEventUserDatas.push_back(newData);
		}
		theData.theGuildScheduledEventUserDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, InviteData& theData) {
		if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_string()) {
			theData.code = jsonObjectData["code"].get<std::string>();
		} else if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_number_integer()) {
			theData.code = std::to_string(jsonObjectData["code"].get<int32_t>());
		}

		if (jsonObjectData.contains("guild") && !jsonObjectData["guild"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["guild"], theData.guild);
		}

		if (jsonObjectData.contains("channel") && !jsonObjectData["channel"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["channel"], theData.channel);
		}

		if (jsonObjectData.contains("inviter") && !jsonObjectData["inviter"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["inviter"], theData.inviter);
		}

		if (jsonObjectData.contains("target_type") && !jsonObjectData["target_type"].is_null()) {
			theData.targetType = jsonObjectData["target_type"].get<int32_t>();
		}

		if (jsonObjectData.contains("target_user") && !jsonObjectData["target_user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["target_user"], theData.targetUser);
		}

		if (jsonObjectData.contains("target_application") && !jsonObjectData["target_application"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["target_application"], theData.targetApplication);
		}

		if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
			theData.approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
			theData.approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("expires_at") && !jsonObjectData["expires_at"].is_null()) {
			theData.expiresAt = jsonObjectData["expires_at"].get<std::string>();
		}

		if (jsonObjectData.contains("stage_instance") && !jsonObjectData["stage_instance"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["stage_instance"], theData.stageInstance);
		}

		if (jsonObjectData.contains("guild_scheduled_event") && !jsonObjectData["guild_scheduled_event"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["guild_scheduled_event"], theData.guildScheduledEvent);
		}

		if (jsonObjectData.contains("uses") && !jsonObjectData["uses"].is_null()) {
			theData.uses = jsonObjectData["uses"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_uses") && !jsonObjectData["max_uses"].is_null()) {
			theData.maxUses = jsonObjectData["max_uses"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_age") && !jsonObjectData["max_age"].is_null()) {
			theData.maxAge = jsonObjectData["max_age"].get<int32_t>();
		}

		theData.temporary = getBool(jsonObjectData, "temporary");

		if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
			theData.createdAt = jsonObjectData["created_at"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, InviteDataVector& theData) {
		theData.theInviteDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			InviteData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theInviteDatas.push_back(newData);
		}
		theData.theInviteDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildTemplateData& theData) {
		if (jsonObjectData.contains("serialized_source_guild") && !jsonObjectData["serialized_source_guild"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["serialized_source_guild"], theData.serializedSourceGuild);
		}

		if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["creator"], theData.creator);
		}

		if (jsonObjectData.contains("source_guild_id") && !jsonObjectData["source_guild_id"].is_null()) {
			theData.sourceGuildId = jsonObjectData["source_guild_id"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("usage_count") && !jsonObjectData["usage_count"].is_null()) {
			theData.usageCount = jsonObjectData["usage_count"].get<uint32_t>();
		}

		if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
			theData.creatorId = jsonObjectData["creator_id"].get<std::string>();
		}

		if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
			theData.createdAt = jsonObjectData["created_at"].get<std::string>();
		}

		if (jsonObjectData.contains("updated_at") && !jsonObjectData["updated_at"].is_null()) {
			theData.updatedAt = jsonObjectData["updated_at"].get<std::string>();
		}

		theData.isDirty = getBool(jsonObjectData, "is_dirty");

		if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null()) {
			theData.code = jsonObjectData["code"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildTemplateDataVector& theData) {
		theData.theGuildTemplateDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			GuildTemplateData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuildTemplateDatas.push_back(newData);
		}
		theData.theGuildTemplateDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, WebHookData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<WebHookType>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
			theData.avatar = jsonObjectData["avatar"].get<std::string>();
		}

		if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
			theData.token = jsonObjectData["token"].get<std::string>();
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["source_guild"], theData.sourceGuild);
		}

		if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["source_channel"], theData.sourceChannel);
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, WebHookDataVector& theData) {
		theData.theWebHookDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			WebHookData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theWebHookDatas.push_back(newData);
		}
		theData.theWebHookDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AuditLogData& theData) {
		if (jsonObjectData.contains("webhooks") && !jsonObjectData["webhooks"].is_null()) {
			theData.webhooks.clear();
			theData.webhooks.reserve(jsonObjectData["webhooks"].size());
			for (auto& value: jsonObjectData["webhooks"]) {
				WebHookData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.webhooks.push_back(newData);
			}
		}

		if (jsonObjectData.contains("guild_scheduled_events") && !jsonObjectData["guild_scheduled_events"].is_null()) {
			theData.guildScheduledEvents.clear();
			theData.guildScheduledEvents.reserve(jsonObjectData["guild_scheduled_events"].size());
			for (auto& value: jsonObjectData["guild_scheduled_events"]) {
				GuildScheduledEventData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.guildScheduledEvents.push_back(newData);
			}
		}

		if (jsonObjectData.contains("auto_moderation_rules") && !jsonObjectData["auto_moderation_rules"].is_null()) {
			theData.autoModerationRules.clear();
			theData.autoModerationRules.reserve(jsonObjectData["auto_moderation_rules"].size());
			for (auto& value: jsonObjectData["auto_moderation_rules"]) {
				AutoModerationRuleData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.autoModerationRules.push_back(newData);
			}
		}

		if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
			theData.users.clear();
			theData.users.reserve(jsonObjectData["users"].size());
			for (auto& value: jsonObjectData["users"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.users.push_back(newData);
			}
		}

		if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData["audit_log_entries"].is_null()) {
			theData.auditLogEntries.clear();
			theData.auditLogEntries.reserve(jsonObjectData["audit_log_entries"].size());
			for (auto& value: jsonObjectData["audit_log_entries"]) {
				AuditLogEntryData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.auditLogEntries.push_back(newData);
			}
		}

		if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
			theData.integrations.clear();
			theData.integrations.reserve(jsonObjectData["integrations"].size());
			for (auto& value: jsonObjectData["integrations"]) {
				IntegrationData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.integrations.push_back(newData);
			}
		}

		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			theData.threads.clear();
			theData.threads.reserve(jsonObjectData["threads"].size());
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.threads.push_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ReactionRemoveData& theData) {
		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			theData.userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
			theData.messageId = stoull(jsonObjectData["message_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["emoji"], theData.emoji);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommandOptionChoiceData& theData) {
		if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null() && jsonObjectData["value"].is_string()) {
			if (jsonObjectData["value"].is_string()) {
				theData.value = jsonObjectData["value"].get<std::string>();
			} else if (jsonObjectData["value"].is_number_float()) {
				theData.value = jsonObjectData["value"].get<float>();
			} else if (jsonObjectData["value"].is_number()) {
				theData.value = jsonObjectData["value"].get<int32_t>();
			}
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.nameLocalizations[key] = newValue;
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommandOptionData& theData) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData.contains("description_localizations") && !jsonObjectData["description_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("channel_types") && !jsonObjectData["channel_types"].is_null()) {
			theData.channelTypes.clear();
			theData.channelTypes.reserve(jsonObjectData["channel_types"].size());
			for (auto& value: jsonObjectData["channel_types"]) {
				theData.channelTypes.push_back(value);
			}
		}

		if (jsonObjectData.contains("min_value") && !jsonObjectData["min_value"].is_null()) {
			theData.minValue = jsonObjectData["min_value"].get<int32_t>();
		}

		if (jsonObjectData.contains("max_value") && !jsonObjectData["max_value"].is_null()) {
			theData.maxValue = jsonObjectData["max_value"].get<int32_t>();
		}

		theData.required = getBool(jsonObjectData, "required");

		theData.autocomplete = getBool(jsonObjectData, "autocomplete");

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ApplicationCommandOptionType>();
		}

		if (theData.type == ApplicationCommandOptionType::Sub_Command_Group || theData.type == ApplicationCommandOptionType::Sub_Command) {
			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				theData.options.clear();
				for (auto& value: jsonObjectData["options"]) {
					ApplicationCommandOptionData newData{};
					DiscordCoreAPI::parseObject(value, newData);
					theData.options.emplace_back(newData);
				}
			}
		} else {
			if (jsonObjectData.contains("choices") && !jsonObjectData["choices"].is_null()) {
				theData.choices.clear();
				for (auto& value: jsonObjectData["choices"]) {
					ApplicationCommandOptionChoiceData newData{};
					DiscordCoreAPI::parseObject(value, newData);
					theData.choices.emplace_back(newData);
				}
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, TypingStartData& theData) {
		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}

		if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
			theData.userId = stoull(jsonObjectData["user_id"].get<std::string>());
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			theData.timestamp = jsonObjectData["timestamp"].get<int32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, YouTubeFormatVector& theData) {
		if (!jsonObjectData.is_null()) {
			if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("formats") &&
				!jsonObjectData["streamingData"]["formats"].is_null()) {
				for (auto& value: jsonObjectData["streamingData"]["formats"]) {
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

					theData.theFormats.emplace_back(newData);
				}
			}

			if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("adaptiveFormats") &&
				!jsonObjectData["streamingData"]["adaptiveFormats"].is_null()) {
				for (auto& value: jsonObjectData["streamingData"]["adaptiveFormats"]) {
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
					theData.theFormats.emplace_back(newData);
				}
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, UserCommandInteractionData& theData) {
		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			theData.targetId = stoull(jsonObjectData["target_id"].get<std::string>());
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, MessageCommandInteractionData& theData) {
		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			theData.targetId = stoull(jsonObjectData["target_id"].get<std::string>());
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ComponentInteractionData& theData) {
		if (jsonObjectData.contains("values") && !jsonObjectData["values"].is_null()) {
			theData.values.clear();
			for (auto& value: jsonObjectData["values"]) {
				theData.values.emplace_back(value);
			}
		}

		if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
			theData.customId = jsonObjectData["custom_id"].get<std::string>();
		}

		if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
			theData.componentType = jsonObjectData["component_type"].get<ComponentType>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ModalInteractionData& theData) {
		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			theData.value = jsonObjectData["components"][0]["components"][0]["value"].get<std::string>();
		}

		if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
			theData.customId = jsonObjectData["custom_id"].get<std::string>();
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			theData.customIdSmall = jsonObjectData["components"][0]["components"][0]["custom_id"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, AllowedMentionsData& theData) {
		if (jsonObjectData.contains("parse") && !jsonObjectData["parse"].is_null()) {
			theData.parse.clear();
			for (auto& value: jsonObjectData["parse"]) {
				theData.parse.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
			theData.roles.clear();
			for (auto& value: jsonObjectData["roles"]) {
				theData.roles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
			theData.users.clear();
			for (auto& value: jsonObjectData["users"]) {
				theData.users.emplace_back(value.get<std::string>());
			}
		}

		theData.repliedUser = getBool(jsonObjectData, "replied_user");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, SelectOptionData& theData) {
		if (jsonObjectData.contains("label") && !jsonObjectData["label"].is_null()) {
			theData.label = jsonObjectData["label"].get<std::string>();
		}

		if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
			theData.value = jsonObjectData["value"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["emoji"], theData.emoji);
		}

		theData._default = getBool(jsonObjectData, "default");
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ActionRowData& theData) {
		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			theData.components.clear();
			for (auto& value: jsonObjectData["components"]) {
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
					newData.disabled = getBool(value, "disabled");
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
					newData.required = getBool(value, "required");
				}

				if (value.contains("emoji") && !value["emoji"].is_null()) {
					DiscordCoreAPI::parseObject(value["emoji"], newData.emoji);
				}

				if (value.contains("url") && !value["url"].is_null()) {
					newData.url = value["url"].get<std::string>();
				}

				if (value.contains("options") && !value["options"].is_null()) {
					for (auto& value02: value["options"]) {
						SelectOptionData newerData{};
						DiscordCoreAPI::parseObject(value, newerData);
						newData.options.emplace_back(newerData);
					}
				}

				if (value.contains("max_values") && !value["max_values"].is_null()) {
					newData.maxValues = value["max_values"].get<int32_t>();
				}

				if (value.contains("min_values") && !value["min_values"].is_null()) {
					newData.minValues = value["min_values"].get<int32_t>();
				}

				theData.components.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommandData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("default_member_permissions") && !jsonObjectData["default_member_permissions"].is_null()) {
			theData.defaultMemberPermissions = jsonObjectData["default_member_permissions"].get<std::string>();
		}

		theData.dmPermission = getBool(jsonObjectData, "dm_permission");

		if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
			theData.version = jsonObjectData["version"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData.contains("description_localizations") && !jsonObjectData["description_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
			theData.version = jsonObjectData["version"].get<std::string>();
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			theData.options.clear();
			for (auto& value: jsonObjectData["options"]) {
				ApplicationCommandOptionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.options.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ChannelMentionData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ChannelType>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ChannelPinsUpdateEventData& theData) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
			theData.lastPinTimeStamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ThreadListSyncData& theData) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_ids") && !jsonObjectData["channel_ids"].is_null()) {
			theData.channelIds.clear();
			for (auto& value: jsonObjectData["channel_ids"]) {
				theData.channelIds.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			theData.members.clear();
			for (auto& value: jsonObjectData["members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.members.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: jsonObjectData["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.threads.push_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ThreadMembersUpdateData& theData) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
			theData.memberCount = jsonObjectData["member_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("added_members") && !jsonObjectData["added_members"].is_null()) {
			theData.addedMembers.clear();
			for (auto& value: jsonObjectData["added_members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.addedMembers.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("removed_member_ids") && !jsonObjectData["removed_member_ids"].is_null()) {
			theData.removedMemberIds.clear();
			for (auto& value: jsonObjectData["removed_member_ids"]) {
				theData.removedMemberIds.emplace_back(value);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, MessageInteractionData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<InteractionType>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, StickerItemData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
			theData.formatType = jsonObjectData["format_type"].get<StickerItemType>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, MessageDataOld& theData) {
		if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
			theData.content = jsonObjectData["content"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["author"], theData.author);
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			theData.timestamp = jsonObjectData["timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
			theData.editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();
		}

		theData.tts = getBool(jsonObjectData, "tts");

		theData.mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
			theData.mentions.clear();
			for (auto& value: jsonObjectData["mentions"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.mentions.push_back(newData);
			}
		}

		if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
			theData.mentionRoles.clear();
			for (auto& value: jsonObjectData["mention_roles"]) {
				theData.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
			theData.mentionChannels.clear();
			for (auto& value: jsonObjectData["mention_channels"]) {
				ChannelMentionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: jsonObjectData["attachments"]) {
				AttachmentData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: jsonObjectData["embeds"]) {
				EmbedData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: jsonObjectData["reactions"]) {
				ReactionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.reactions.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
			theData.nonce = jsonObjectData["nonce"].get<std::string>();
		}

		theData.pinned = getBool(jsonObjectData, "pinned");

		if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
			theData.webHookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<MessageType>();
		}

		if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["activity"], theData.activity);
		}

		if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["application"], theData.application);
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["message_reference"], theData.messageReference);
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			theData.flags = jsonObjectData["flags"].get<int32_t>();
		}

		if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: jsonObjectData["sticker_items"]) {
				StickerItemData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["interaction"], theData.interaction);
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			theData.components.clear();
			for (auto& value: jsonObjectData["components"]) {
				ActionRowData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.components.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["thread"], theData.thread);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, MessageData& theData) {
		if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
			theData.content = jsonObjectData["content"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["author"], theData.author);
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
		}

		if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
			theData.timestamp = jsonObjectData["timestamp"].get<std::string>();
		}

		if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
			theData.editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();
		}

		theData.tts = getBool(jsonObjectData, "tts");

		theData.mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
			theData.mentions.clear();
			for (auto& value: jsonObjectData["mentions"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.mentions.push_back(newData);
			}
		}

		if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
			theData.mentionRoles.clear();
			for (auto& value: jsonObjectData["mention_roles"]) {
				theData.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
			theData.mentionChannels.clear();
			for (auto& value: jsonObjectData["mention_channels"]) {
				ChannelMentionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: jsonObjectData["attachments"]) {
				AttachmentData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: jsonObjectData["embeds"]) {
				EmbedData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: jsonObjectData["reactions"]) {
				ReactionData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.reactions.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
			theData.nonce = jsonObjectData["nonce"].get<std::string>();
		}

		theData.pinned = getBool(jsonObjectData, "pinned");

		if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
			theData.webHookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<MessageType>();
		}

		if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["activity"], theData.activity);
		}

		if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["application"], theData.application);
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}

		if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["message_reference"], theData.messageReference);
		}

		if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
			theData.flags = jsonObjectData["flags"].get<int32_t>();
		}

		if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: jsonObjectData["sticker_items"]) {
				StickerItemData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["interaction"], theData.interaction);
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			theData.components.clear();
			for (auto& value: jsonObjectData["components"]) {
				ActionRowData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.components.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["thread"], theData.thread);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, StickerPackData& theData) {
		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("cover_sticker_id") && !jsonObjectData["cover_sticker_id"].is_null()) {
			theData.coverStickerId = jsonObjectData["cover_sticker_id"].get<std::string>();
		}

		if (jsonObjectData.contains("banner_asset_id") && !jsonObjectData["banner_asset_id"].is_null()) {
			theData.bannerAssetId = jsonObjectData["banner_asset_id"].get<std::string>();
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			theData.description = jsonObjectData["description"].get<std::string>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.Id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("sku_id") && !jsonObjectData["sku_id"].is_null()) {
			theData.skuId = jsonObjectData["sku_id"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, StickerPackDataVector& theData) {
		theData.theStickerPackDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			StickerPackData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theStickerPackDatas.push_back(newData);
		}
		theData.theStickerPackDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ConnectionData& theData) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<std::string>();
		}

		theData.showActivity = getBool(jsonObjectData, "show_activity");

		theData.friendSync = getBool(jsonObjectData, "friend_sync");

		theData.revoked = getBool(jsonObjectData, "revoked");

		theData.verified = getBool(jsonObjectData, "verified");

		if (jsonObjectData.contains("visibility") && !jsonObjectData["visibility"].is_null()) {
			theData.visibility = jsonObjectData["visibility"].get<ConnectionVisibilityTypes>();
		}

		if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
			theData.integrations.clear();
			for (auto& value: jsonObjectData["integrations"]) {
				IntegrationData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.integrations.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ConnectionDataVector& theData) {
		theData.theConnectionDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			ConnectionData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theConnectionDatas.push_back(newData);
		}
		theData.theConnectionDatas.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommandInteractionDataOption& theData) {
		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ApplicationCommandOptionType>();
		}

		theData.focused = getBool(jsonObjectData, "focused");

		if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
			if (jsonObjectData["value"].is_boolean()) {
				theData.valueBool = getBool(jsonObjectData, "value");
			}
			if (jsonObjectData["value"].is_string()) {
				theData.valueString = jsonObjectData["value"].get<std::string>();
			}
			if (jsonObjectData["value"].is_number()) {
				theData.valueInt = jsonObjectData["value"].get<int32_t>();
			}
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			theData.options.clear();
			for (auto& value: jsonObjectData["options"]) {
				ApplicationCommandInteractionDataOption newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.options.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, ApplicationCommandInteractionData& theData) {
		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.name = jsonObjectData["name"].get<std::string>();
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
			theData.options.clear();
			for (auto& value: jsonObjectData["options"]) {
				ApplicationCommandInteractionDataOption newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.options.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("resolved") && !jsonObjectData["resolved"].is_null()) {
			auto& value = jsonObjectData["resolved"];

			if (value.contains("attachments") && !value["attachments"].is_null()) {
				theData.resolved.attachments.clear();
				auto newMap = value["attachments"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					AttachmentData newData{};
					DiscordCoreAPI::parseObject(newValue, newData);
					theData.resolved.attachments[stoull(key)] = newData;
				}
			}

			if (value.contains("users") && !value["users"].is_null()) {
				theData.resolved.users.clear();
				auto newMap = value["users"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					UserData newData{};
					DiscordCoreAPI::parseObject(value, newData);
					theData.resolved.users[newData.id] = newData;
				}
			}
			if (value.contains("channels") && !value["channels"].is_null()) {
				theData.resolved.channels.clear();
				auto newMap = value["channels"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					ChannelData newData{};
					DiscordCoreAPI::parseObject(value, newData);
					theData.resolved.channels[newData.id] = newData;
				}
			}
			if (value.contains("roles") && !value["roles"].is_null()) {
				theData.resolved.roles.clear();
				auto newMap = value["roles"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					RoleData newData{};
					DiscordCoreAPI::parseObject(value, newData);
					theData.resolved.roles[newData.id] = newData;
				}
			}
			if (value.contains("members") && !value["members"].is_null()) {
				theData.resolved.members.clear();
				auto newMap = value["members"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					GuildMemberData theDataNew{};
					DiscordCoreAPI::parseObject(newValue, theDataNew);
					theData.resolved.members.emplace(stoull(key), theDataNew);
				}
			}
			if (value.contains("messages") && !value["messages"].is_null()) {
				theData.resolved.messages.clear();
				auto newMap = value["messages"].get<std::unordered_map<std::string, nlohmann::json>>();
				for (auto& [key, newValue]: newMap) {
					MessageData newData{};
					DiscordCoreAPI::parseObject(newValue, newData);
					theData.resolved.messages[stoull(key)] = newData;
				}
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, InteractionDataData& theData) {
		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData, theData.applicationCommandData);
		}

		if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData, theData.messageInteractionData);
			DiscordCoreAPI::parseObject(jsonObjectData, theData.userInteractionData);
		}

		if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData, theData.componentData);
		}

		if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData, theData.modalData);
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, InteractionData& theData) {
		if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["data"], theData.data);
			theData.rawData = jsonObjectData["data"];
		}

		if (jsonObjectData.contains("app_permissions") && !jsonObjectData["app_permissions"].is_null()) {
			theData.appPermissions = jsonObjectData["app_permissions"].get<std::string>();
		}

		if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
			theData.type = jsonObjectData["type"].get<InteractionType>();
		}

		if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
			theData.token = jsonObjectData["token"].get<std::string>();
		}

		if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["member"], theData.member);
			theData.user.id = theData.member.id;
			theData.user.userName = theData.member.getUserData().userName;
		} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["user"], theData.user);
		}

		if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
			theData.channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
		}

		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("locale") && !jsonObjectData["locale"].is_null()) {
			theData.locale = jsonObjectData["locale"].get<std::string>();
		}

		if (jsonObjectData.contains("guild_locale") && !jsonObjectData["guild_locale"].is_null()) {
			theData.guildLocale = jsonObjectData["guild_locale"].get<std::string>();
		}

		if (jsonObjectData.contains("message") && !jsonObjectData["message"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["message"], theData.message);
		}

		if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
			theData.version = jsonObjectData["version"].get<int32_t>();
		}

		if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
			theData.id = stoull(jsonObjectData["id"].get<std::string>());
		}

		if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
			theData.applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, SessionStartData& theData) {
		if (jsonObjectData.contains("max_concurrency") && !jsonObjectData["max_concurrency"].is_null()) {
			theData.maxConcurrency = jsonObjectData["max_concurrency"].get<uint32_t>();
		}

		if (jsonObjectData.contains("remaining") && !jsonObjectData["remaining"].is_null()) {
			theData.remaining = jsonObjectData["remaining"].get<uint32_t>();
		}

		if (jsonObjectData.contains("reset_after") && !jsonObjectData["reset_after"].is_null()) {
			theData.resetAfter = jsonObjectData["reset_after"].get<uint32_t>();
		}

		if (jsonObjectData.contains("total") && !jsonObjectData["total"].is_null()) {
			theData.total = jsonObjectData["total"].get<uint32_t>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GatewayBotData& theData) {
		if (jsonObjectData.contains("session_start_limit") && !jsonObjectData["session_start_limit"].is_null()) {
			DiscordCoreAPI::parseObject(jsonObjectData["session_start_limit"], theData.sessionStartLimit);
		}

		if (jsonObjectData.contains("shards") && !jsonObjectData["shards"].is_null()) {
			theData.shards = jsonObjectData["shards"].get<uint32_t>();
		}

		if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
			theData.url = jsonObjectData["url"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildEmojisUpdateEventData& theData) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
			theData.emojis.clear();
			for (auto& value: jsonObjectData["emojis"]) {
				EmojiData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.emojis.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildStickersUpdateEventData& theData) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: jsonObjectData["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.stickers.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildMembersChunkEventData& theData) {
		if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
			theData.guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
		}

		if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
			theData.nonce = jsonObjectData["nonce"].get<std::string>();
		}

		if (jsonObjectData.contains("chunk_index") && !jsonObjectData["chunk_index"].is_null()) {
			theData.chunkIndex = jsonObjectData["chunk_index"].get<int32_t>();
		}

		if (jsonObjectData.contains("chunk_count") && !jsonObjectData["chunk_count"].is_null()) {
			theData.chunkCount = jsonObjectData["chunk_count"].get<int32_t>();
		}

		if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
			theData.presences.clear();
			for (auto& value: jsonObjectData["presences"]) {
				PresenceUpdateData newData{};
				DiscordCoreAPI::parseObject(value, newData);
				theData.presences.emplace_back(newData);
			}
		}

		if (jsonObjectData.contains("not_found") && !jsonObjectData["not_found"].is_null()) {
			theData.notFound.clear();
			for (auto& value: jsonObjectData["not_found"]) {
				theData.notFound.emplace_back(value);
			}
		}

		if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
			theData.members.clear();
			for (auto& value: jsonObjectData["members"]) {
				GuildMemberData theDataNew{};
				DiscordCoreAPI::parseObject(value, theDataNew);
				theData.members.emplace_back(theDataNew);
			}
		}
	}

	void CommandData::parseOptions(nlohmann::json& jsonObjectData) {
		for (auto theOptionIterator = jsonObjectData.at("options").begin(); theOptionIterator != jsonObjectData.at("options").end(); ++theOptionIterator) {
			if (theOptionIterator->contains("type") && theOptionIterator->at("type") == 1) {
				if (theOptionIterator->contains("name")) {
					this->subCommandName = theOptionIterator->at("name").get<std::string>();
				}
			} else if (theOptionIterator->contains("type") && theOptionIterator->at("type") == 2) {
				if (theOptionIterator->contains("name")) {
					this->subCommandGroupName = theOptionIterator->at("name").get<std::string>();
				}
			}
			if (theOptionIterator->contains("value") && theOptionIterator->contains("name")) {
				nlohmann::json newValueNew = theOptionIterator->at("value");
				nlohmann::json newKeyNew = theOptionIterator->at("name");
				if (newValueNew.is_string()) {
					this->optionsArgs.emplace(newKeyNew.get<std::string>(), newValueNew.get<std::string>());
				} else if (newValueNew.is_number()) {
					this->optionsArgs.emplace(newKeyNew.get<std::string>(), std::to_string(newValueNew.get<uint64_t>()));
				} else if (newValueNew.is_boolean()) {
					this->optionsArgs.emplace(newKeyNew.get<std::string>(), newValueNew.get<bool>());
				}
			}
			if (theOptionIterator->contains("options")) {
				this->parseOptions(*theOptionIterator);
			}
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, CommandData& theData) {
		for (auto theIterator = jsonObjectData.begin(); theIterator != jsonObjectData.end(); ++theIterator) {
			if (theIterator->contains("type") && theIterator->at("type") == 1) {
				if (theIterator->contains("name")) {
					theData.subCommandName = theIterator->get<std::string>();
				}
			} else if (theIterator->contains("type") && theIterator->at("type") == 2) {
				if (theIterator->contains("name")) {
					theData.subCommandGroupName = theIterator->get<std::string>();
				}
			}
		}
		if (jsonObjectData.contains("options")) {
			theData.parseOptions(jsonObjectData);
		}

		if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
			theData.commandName = jsonObjectData["name"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, Song& theData) {
		if (jsonObjectData.contains("lengthText") && !jsonObjectData["lengthText"].is_null()) {
			theData.duration = jsonObjectData["lengthText"]["accessibility"]["accessibilityData"]["label"].get<std::string>();
		}

		if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData["detailedMetadataSnippets"].is_null()) {
			for (auto& value: jsonObjectData["detailedMetadataSnippets"][0]["snippetText"]["runs"]) {
				std::string newString = value["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theData.description = utf8MakeValid(newString);
			}
		}

		if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
			theData.thumbnailUrl = jsonObjectData["thumbnail"]["thumbnails"][0]["url"].get<std::string>();
		}

		if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
			if (jsonObjectData["title"].contains("runs")) {
				std::string newString = jsonObjectData["title"]["runs"][0]["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theData.songTitle = utf8MakeValid(newString);
			} else if (jsonObjectData["title"].contains("simpleText")) {
				std::string newString = jsonObjectData["title"]["simpleText"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theData.songTitle = utf8MakeValid(newString);
			}
		}
		if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
			theData.songId = jsonObjectData["videoId"].get<std::string>();
		}

		if (jsonObjectData.contains("track_authorization") && !jsonObjectData["track_authorization"].is_null()) {
			theData.trackAuthorization = jsonObjectData["track_authorization"].get<std::string>();
		}

		if (jsonObjectData.contains("media") && !jsonObjectData["media"].is_null()) {
			bool isItFound{ false };
			for (auto& value: jsonObjectData["media"]["transcodings"]) {
				if (value["preset"] == "opus_0_0") {
					isItFound = true;
					theData.firstDownloadUrl = value["url"].get<std::string>();
					theData.songId = value["url"].get<std::string>();
					theData.doWeGetSaved = true;
				}
			}
			bool isItFound2{ false };
			if (!isItFound) {
				for (auto& value: jsonObjectData["media"]["transcodings"]) {
					if (value["preset"] == "mp3_0_0") {
						theData.firstDownloadUrl = value["url"].get<std::string>();
						theData.songId = value["url"].get<std::string>();
						isItFound2 = true;
					}
				}
			}
			if (!isItFound2 && !isItFound) {
				for (auto& value: jsonObjectData["media"]["transcodings"]) {
					theData.firstDownloadUrl = value["url"].get<std::string>();
					theData.songId = value["url"].get<std::string>();
				}
			}
		}

		if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null() && !jsonObjectData["title"].is_object()) {
			std::string newString = jsonObjectData["title"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			theData.songTitle = utf8MakeValid(newString);
		}

		if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
			std::string newString = jsonObjectData["description"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			theData.description = utf8MakeValid(newString);
			theData.description += "...";
		}

		if (jsonObjectData.contains("artwork_url") && !jsonObjectData["artwork_url"].is_null()) {
			theData.thumbnailUrl = jsonObjectData["artwork_url"].get<std::string>();
		} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
			if (jsonObjectData["user"].contains("avatar_url") && !jsonObjectData["user"]["avatar_url"].is_null()) {
				theData.thumbnailUrl = jsonObjectData["user"]["avatar_url"].get<std::string>();
			}
		}

		if (jsonObjectData.contains("duration") && !jsonObjectData["duration"].is_null()) {
			theData.duration = TimeStamp<std::chrono::milliseconds>::convertMsToDurationString(jsonObjectData["duration"].get<int32_t>());
		}

		if (jsonObjectData.contains("permalink_url") && !jsonObjectData["permalink_url"].is_null()) {
			theData.viewUrl = jsonObjectData["permalink_url"].get<std::string>();
		}
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildScheduledEventVector& theData) {
		theData.theGuildScheduledEvents.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			GuildScheduledEvent newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuildScheduledEvents.push_back(newData);
		}
		theData.theGuildScheduledEvents.shrink_to_fit();
	}

	template<> void parseObject(nlohmann::json& jsonObjectData, GuildScheduledEventDataVector& theData) {
		theData.theGuildScheduledEventDatas.reserve(jsonObjectData.size());
		for (auto& value: jsonObjectData) {
			GuildScheduledEventData newData{};
			DiscordCoreAPI::parseObject(value, newData);
			theData.theGuildScheduledEventDatas.push_back(newData);
		}
		theData.theGuildScheduledEventDatas.shrink_to_fit();
	}
};
