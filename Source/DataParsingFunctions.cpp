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
#include <discordcoreapi/DataParsingFunctions.hpp>

namespace DiscordCoreAPI {

	uint64_t getId(simdjson::ondemand::value&jsonObjectData, const char* theKey) {
		return strtoull(getString(jsonObjectData, theKey));
	}

	bool getBool(simdjson::ondemand::value& jsonData, const char* theKey) {
		try {
			bool theValue{};
			jsonData[theKey].get(theValue);
			return static_cast<uint8_t>(theValue);
		} catch (...) {
			reportException("getBool()");
			return 0;
		}
	}

	uint8_t getUint8(simdjson::ondemand::value& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			jsonData[theKey].get(theValue);
			return static_cast<uint8_t>(theValue);
		} catch (...) {
			reportException("getUint8()");
			return 0;
		}
	}

	uint16_t getUint16(simdjson::ondemand::value& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			jsonData[theKey].get(theValue);
			return static_cast<uint16_t>(theValue);
		} catch (...) {
			reportException("getUint16()");
			return 0;
		}
	}

	uint32_t getUint32(simdjson::ondemand::value& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			jsonData[theKey].get(theValue);
			return static_cast<uint32_t>(theValue);
		} catch (...) {
			reportException("getUint32()");
			return 0;
		}
	}

	uint64_t getUint64(simdjson::ondemand::value& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			jsonData[theKey].get(theValue);
			return theValue;
		} catch (...) {
			reportException("getUint64()");
			return 0;
		}
	}

	std::string getString(simdjson::ondemand::value& jsonData, const char* theKey) {
		try {
			std::string_view theStringNew{};
			auto theResult= jsonData[theKey].get(theStringNew);
			if (theResult != simdjson::error_code::SUCCESS) {
				return "0";
			}
			std::string theStringNewer{ theStringNew.data(), theStringNew.size() };
			return theStringNewer;
		} catch (...) {
			reportException("getString()");
			return "";
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommand& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.defaultMemberPermissions = getString(jsonObjectData, "default_member_permissions");

		theData.dmPermission = getBool(jsonObjectData, "dm_permission");

		theData.version = getString(jsonObjectData, "version");

		theData.type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		simdjson::ondemand::object theMap{};
		auto theResult= jsonObjectData["name_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.nameLocalizations.clear();
			for (auto value: theMap) {
				theData.nameLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}

		theResult = jsonObjectData["description_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.descriptionLocalizations.clear();
			for (auto value: theMap) {
				theData.descriptionLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}

		theData.applicationId = getId(jsonObjectData, "application_id");

		theData.name = getString(jsonObjectData, "name");

		theData.description = getString(jsonObjectData, "description");

		theData.version = getString(jsonObjectData, "version");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["options"].get(theArray);
		if (theResult== simdjson::error_code::SUCCESS) {
			theData.options.clear();
			for (auto value: theArray) {
				ApplicationCommandOptionData theDataNew{};
				auto theObject = value.value();
				parseObject(theObject, theDataNew);
				theData.options.emplace_back(std::move(theDataNew));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, UserData& theData) {
		theData.flags |= setBool(theData.flags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		theData.flags |= setBool(theData.flags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		theData.flags |= setBool(theData.flags, UserFlags::System, getBool(jsonObjectData, "system"));

		theData.flags |= setBool(theData.flags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		theData.discriminator = getString(jsonObjectData, "discriminator");

		theData.flags |= getUint32(jsonObjectData, "public_flags");

		theData.userName = getString(jsonObjectData, "username");

		theData.id = getId(jsonObjectData, "id");

		theData.avatar = getString(jsonObjectData, "avatar");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, RoleData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		std::stringstream theStream{};
		theStream << getString(jsonObjectData, "unicode_emoji");
		for (auto& value: theStream.str()) {
			theData.unicodeEmoji.push_back(value);
		}
		if (theData.unicodeEmoji.size() > 3) {
			theData.unicodeEmoji = static_cast<std::string>(theData.unicodeEmoji).substr(1, theData.unicodeEmoji.size() - 3);
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.color = getUint32(jsonObjectData, "color");

		theData.flags |= setBool(theData.flags, RoleFlags::Hoist, getBool(jsonObjectData, "hoist"));

		theData.flags |= setBool(theData.flags, RoleFlags::Managed, getBool(jsonObjectData, "managed"));

		theData.flags |= setBool(theData.flags, RoleFlags::Mentionable, getBool(jsonObjectData, "mentionable"));

		theData.position = getUint32(jsonObjectData, "position");

		theData.permissions = getString(jsonObjectData, "permissions");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, User& theData) {
		theData.userName = getString(jsonObjectData, "username");

		theData.id = getId(jsonObjectData, "id");

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

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Role& theData) {
		theData.id = getId(jsonObjectData, "id");

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

		simdjson::ondemand::value theRoleTagsNew{};
		auto theResult= jsonObjectData["tags"].get(theRoleTagsNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			RoleTagsData theRoleTags{};
			parseObject(theRoleTagsNew, theRoleTags);
			theData.tags = std::move(theRoleTags);
		}

	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildMemberData& theData) {
		
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.guildId = getId(jsonObjectData, "guild_id");
		
		simdjson::ondemand::array theArray{};
		auto theResult= jsonObjectData["roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.roles.clear();
			for (auto value: theArray) {
				std::string theString{ value.get_string().take_value().data() };
				theData.roles.emplace_back(stoull(theString));
			}
		}

		theData.permissions = getString(jsonObjectData, "permissions");

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			UserData theUser{};
			parseObject(theObject, theUser);
			theData.id = theUser.id;
			Users::insertUser(std::move(theUser));
		}

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.nick = getString(jsonObjectData, "nick");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildMember& theData) {
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array theArray{};
		auto theResult= jsonObjectData["roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.roles.clear();
			for (auto value: theArray) {
				std::string theString{ value.get_string().take_value().data() };
				theData.roles.emplace_back(stoull(theString));
			}
		}

		theData.permissions = getString(jsonObjectData, "permissions");

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			UserData theUser{};
			parseObject(theObject, theUser);
			theData.id = theUser.id;
			Users::insertUser(std::move(theUser));
		}

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.nick = getString(jsonObjectData, "nick");

		theData.communicationDisabledUntil = getString(jsonObjectData, "communication_disabled_until");

		theData.premiumSince = getString(jsonObjectData, "premium_since");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, OverWriteData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.allow = getId(jsonObjectData, "allow");

		theData.deny = getId(jsonObjectData, "deny");

		theData.type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ChannelData& theData) {
		theData.flags |= setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.ownerId = getId(jsonObjectData, "owner_id");

		theData.id = getId(jsonObjectData, "id");
		
		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.parentId = getId(jsonObjectData, "parent_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.position = getUint32(jsonObjectData, "position");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["permission_overwrites"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.permissionOverwrites.clear();
			for (auto value: theArray) {
				OverWriteData theDataNew{};
				auto theObject = value.value();
				parseObject(theObject, theDataNew);
				theData.permissionOverwrites.emplace_back(std::move(theDataNew));
			}
		}

		theData.name = getString(jsonObjectData, "name");
	}
	
	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildData& theData) {

		theData.flags |= setBool(theData.flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));

		theData.flags |= setBool(theData.flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		theData.flags |= setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		theData.flags |= setBool(theData.flags, GuildFlags::Large, getUint8(jsonObjectData, "large"));

		theData.ownerId = getId(jsonObjectData, "owner_id");
		
		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.id = getId(jsonObjectData, "id");

		theData.icon = getString(jsonObjectData, "icon");

		theData.name = getString(jsonObjectData, "name");

		theData.threads.clear();

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["threads"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray) {
				auto theObject = value.value();
				theData.threads.emplace_back(getId(theObject, "id"));
			}
		}

		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.stickers.clear();
			for (auto value: theArray) {
				auto theObject = value.value();
				theData.stickers.emplace_back(getId(theObject, "id"));
			}
		}

		theResult = jsonObjectData["guild_scheduled_events"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.guildScheduledEvents.clear();
			for (auto value: theArray) {
				auto theObject = value.value();
				theData.guildScheduledEvents.emplace_back(getId(theObject, "id"));
			}
		}
		
		theResult = jsonObjectData["stage_instances"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.stageInstances.clear();
			for (auto value: theArray) {
				auto theObject = value.value();
				theData.stageInstances.emplace_back(getId(theObject, "id"));
			}
		}

		theResult = jsonObjectData["emoji"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.emoji.clear();
			for (auto value: theArray) {
				auto theObject = value.value();
				theData.emoji.emplace_back(getId(theObject, "id"));
			}
		}
		
		if (Roles::doWeCacheRoles) {
			theData.roles.clear();
			RoleData newData{};
			theResult = jsonObjectData["roles"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = theData.id;
					theData.roles.emplace_back(newData.id);
					Roles::insertRole(std::move(newData));
				}
			}
		}
		
		if (GuildMembers::doWeCacheGuildMembers) {
			theData.members.clear();
			GuildMemberData newData{};
			theResult = jsonObjectData["members"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = theData.id;
					theData.members.emplace_back(newData.id);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}
		
		if (GuildMembers::doWeCacheGuildMembers) {
			theResult = jsonObjectData["voice_states"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					auto userId = getId(theObject, "user_id");
					if (GuildMembers::cache.contains(GuildMemberKey{ theData.id, userId })) {
						GuildMembers::cache.at(GuildMemberKey{ theData.id, userId }).voiceChannelId = getId(theObject, "channel_id");
					}
				}
			}
		}
		
		if (GuildMembers::doWeCacheGuildMembers) {
			theData.presences.clear();
			PresenceUpdateData newData{};
			theResult = jsonObjectData["presences"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = theData.id;
					theData.presences.emplace_back(std::move(newData));
				}
			}
		}
		
		if (Channels::doWeCacheChannels) {
			theData.channels.clear();
			ChannelData newData{};
			theResult = jsonObjectData["channels"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = theData.id;
					theData.channels.emplace_back(newData.id);
					Channels::insertChannel(std::move(newData));
				}
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AttachmentData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.filename = getString(jsonObjectData, "filename");

		theData.contentType = getString(jsonObjectData, "content_type");

		theData.ephemeral = getBool(jsonObjectData, "ephemeral");

		theData.description = getString(jsonObjectData, "description");

		theData.size = getUint32(jsonObjectData, "size");

		theData.url = getString(jsonObjectData, "url");

		theData.proxyUrl = getString(jsonObjectData, "proxy_url");

		theData.width = getUint32(jsonObjectData, "width");

		theData.height = getUint32(jsonObjectData, "height");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Guild& theData) {

		theData.id = getId(jsonObjectData, "id");

		theData.channels.clear();
		Channel newChannel{};
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["threads"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newChannel);
				newChannel.guildId = theData.id;
				theData.threads.emplace_back(newChannel);
			}
		}

		Sticker newSticker{};
		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newSticker);
				newSticker.guildId = theData.id;
				theData.stickers.emplace_back(newSticker);
			}
		}

		GuildScheduledEvent newGuildEvent{};
		theResult = jsonObjectData["guild_scheduled_events"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newGuildEvent);
				newGuildEvent.guildId = theData.id;
				theData.guildScheduledEvents.emplace_back(newGuildEvent);
			}
		}

		StageInstance newStageInstance{};
		theResult = jsonObjectData["stage_instances"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newStageInstance);
				newStageInstance.guildId = theData.id;
				theData.stageInstances.emplace_back(newStageInstance);
			}
		}

		EmojiData newEmojiData{};
		theResult = jsonObjectData["emoji"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newEmojiData);
				theData.emoji.emplace_back(newEmojiData);
			}
		}

		if (Roles::doWeCacheRoles) {
			theData.roles.clear();
			RoleData newData{};
			theResult = jsonObjectData["roles"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = theData.id;
					theData.roles.emplace_back(newData);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			theData.members.clear();
			GuildMemberData newData{};
			theResult = jsonObjectData["members"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = theData.id;
					theData.members.emplace_back(newData);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			theResult = jsonObjectData["voice_states"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					auto userId = getId(theObject, "user_id");
					if (GuildMembers::cache.contains(GuildMemberKey{ theData.id, userId })) {
						GuildMembers::cache.at(GuildMemberKey{ theData.id, userId }).voiceChannelId = getId(theObject, "channel_id");
					}
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			theData.presences.clear();
			PresenceUpdateData newData{};
			theResult = jsonObjectData["presences"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					auto userId = newData.userId;
					newData.guildId = theData.id;
					theData.presences.emplace(userId, std::move(newData));
				}
			}
		}

		if (Channels::doWeCacheChannels) {
			theData.channels.clear();
			ChannelData newData{};
			theResult = jsonObjectData["channels"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					auto theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = theData.id;
					theData.channels.emplace_back(newData);
					Channels::insertChannel(std::move(newData));
				}
			}
		}

		theData.afkChannelId = getId(jsonObjectData, "afk_channel_id");

		theData.icon = getString(jsonObjectData, "icon");

		theData.name = getString(jsonObjectData, "name");

		theData.icon = getString(jsonObjectData, "icon_hash");

		theData.splash = getString(jsonObjectData, "splash");

		theData.discoverySplash = getString(jsonObjectData, "discovery_splash");

		theData.flags = setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		theData.ownerId = getId(jsonObjectData, "owner_id");

		theData.preferredLocale = getString(jsonObjectData, "preferred_locale");

		theData.publicUpdatesChannelId = getId(jsonObjectData, "public_updates_channel_id");

		theData.vanityUrlCode = getString(jsonObjectData, "vanity_url_code");

		theData.description = getString(jsonObjectData, "description");

		theData.banner = getString(jsonObjectData, "banner");

		theData.rulesChannelId = getId(jsonObjectData, "rule_Channel_id");

		theData.applicationId = getId(jsonObjectData, "application_id");

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.widgetChannelId = getId(jsonObjectData, "widget_channel_id");

		theData.systemChannelId = getId(jsonObjectData, "system_channel_id");

		theData.region = getString(jsonObjectData, "region");

		theData.afkChannelId = getId(jsonObjectData, "afk_channel_id");

		theData.region = getString(jsonObjectData, "region");

		theResult= jsonObjectData["features"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.features.clear();
			for (auto value: theArray) {
				std::string theString{ value.get_string().take_value().data() };
				theData.features.emplace_back(theString);
			}
		}

		theData.permissions = getString(jsonObjectData, "permissions");

		theData.afkTimeOut = static_cast<AfkTimeOutDurations>(getUint8(jsonObjectData, "afk_timeout"));

		theData.flags = setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		theData.flags = setBool(theData.flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));

		theData.verificationLevel = static_cast<VerificationLevel>(getUint8(jsonObjectData, "verification_level"));

		theData.defaultMessageNotifications = static_cast<DefaultMessageNotificationLevel>(getUint8(jsonObjectData, "default_message_notification_level"));

		theData.explicitContentFilter = static_cast<ExplicitContentFilterLevel>(getUint8(jsonObjectData, "explicit_content_filter_level"));

		theData.mfaLevel = static_cast<MFALevel>(getUint8(jsonObjectData, "mfa_level"));

		theData.systemChannelFlags = static_cast<SystemChannelFlags>(getUint8(jsonObjectData, "system_channel_flags"));

		theData.flags = setBool(theData.flags, GuildFlags::Large, getBool(jsonObjectData, "large"));

		theData.flags = setBool(theData.flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.maxPresences = getUint32(jsonObjectData, "max_presences");

		theData.maxMembers = getUint32(jsonObjectData, "max_members");

		theData.premiumSubscriptionCount = getUint32(jsonObjectData, "premium_subscription_count");

		theData.premiumTier = static_cast<PremiumTier>(getUint8(jsonObjectData, "premium_tier"));

		theData.maxVideoChannelUsers = getUint32(jsonObjectData, "max_video_channel_users");

		theData.approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		theData.approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["welcome_screen"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.welcomeScreen);
		}

		theData.nsfwLevel = static_cast<GuildNSFWLevel>(getUint8(jsonObjectData, "nsfw_level"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildScheduledEventMetadata& theData) {
		theData.location = getString(jsonObjectData, "location");
	}
	
	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommandVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData[""].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theApplicationCommands.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				ApplicationCommand newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theApplicationCommands.push_back(newData);
			}
			theData.theApplicationCommands.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildApplicationCommandPermissionsData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.applicationId = getId(jsonObjectData, "application_id");

		theData.id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["permissions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.permissions.clear();
			for (auto value: theArray) {
				ApplicationCommandPermissionData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.permissions.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildApplicationCommandPermissionsDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theGuildApplicationCommandPermissionsDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				GuildApplicationCommandPermissionsData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theGuildApplicationCommandPermissionsDatas.push_back(newData);
			}
			theData.theGuildApplicationCommandPermissionsDatas.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AutoModerationRule& theData) {
		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");

		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.triggerType = static_cast<TriggerType>(getUint64(jsonObjectData, "trigger_type"));

		theData.eventType = static_cast<EventType>(getUint64(jsonObjectData, "event_type"));

		theData.creatorId = getId(jsonObjectData, "creator_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["actions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.actions.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				ActionData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.actions.push_back(newData);
			}
			theData.actions.shrink_to_fit();
		}

		theResult = jsonObjectData["exempt_roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.exemptRoles.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.exemptRoles.push_back(value.get_uint64().value());
			}
			theData.exemptRoles.shrink_to_fit();
		}

		theResult = jsonObjectData["exempt_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.exemptChannels.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.exemptChannels.push_back(value.get_uint64().value());
			}
			theData.exemptChannels.shrink_to_fit();
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["trigger_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			TriggerMetaData theDataNew{};
			parseObject(theObject, theDataNew);
			theData.triggerMetaData = theDataNew;
		}

		theData.guildId = getId(jsonObjectData, "guild_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, VoiceRegionDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theVoiceRegionDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				VoiceRegionData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theVoiceRegionDatas.push_back(newData);
			}
			theData.theVoiceRegionDatas.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, InviteData& theData) {
		theData.code = getUint32(jsonObjectData, "code");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["guild"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.guild);
		}

		theResult = jsonObjectData["channel"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.channel);
		}

		theResult = jsonObjectData["inviter"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.inviter);
		}

		theData.targetType = getUint32(jsonObjectData, "target_type");

		theResult = jsonObjectData["target_user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.targetUser);
		}

		theResult = jsonObjectData["target_application"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.targetApplication);
		}

		theResult = jsonObjectData["stage_instance"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.stageInstance);
		}

		theResult = jsonObjectData["guild_scheduled_event"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.guildScheduledEvent);
		}

		theData.approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		theData.approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.expiresAt = getString(jsonObjectData, "expires_at");

		theData.uses = getUint32(jsonObjectData, "uses");

		theData.maxUses = getUint32(jsonObjectData, "max_uses");

		theData.maxAge = getUint32(jsonObjectData, "max_age");

		theData.temporary = getBool(jsonObjectData, "temporary");

		theData.createdAt = getString(jsonObjectData, "created_at");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, InviteDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theInviteDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				InviteData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theInviteDatas.push_back(newData);
			}
			theData.theInviteDatas.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ModalInteractionData& theData) {
		simdjson::ondemand::value theComponent{};
		auto theResult = jsonObjectData["components"][0]["components"][0].get(theComponent);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.value = getString(theComponent, "value");
			theData.customIdSmall = getString(theComponent, "value");
		}

		theData.customId = getString(jsonObjectData, "custom_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ComponentInteractionData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["values"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.values.clear();
			for (auto iterator = theArray.begin(); iterator != theArray.end(); ++iterator) {
				theData.values.emplace_back(iterator.value().operator*().get_string().take_value().data());
			}
		}

		theData.customId = getString(jsonObjectData, "custom_id");

		theData.componentType = static_cast<ComponentType>(getUint8(jsonObjectData, "component_type"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, SelectOptionData& theData) {
		theData.label = getString(jsonObjectData, "label");

		theData.value = getString(jsonObjectData, "value");

		theData.description = getString(jsonObjectData, "description");

		simdjson::ondemand::value theEmoji{};
		auto theResult = jsonObjectData["emoji"].get(theEmoji);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theEmoji, theData.emoji);
		}

		theData._default = getBool(jsonObjectData, "default");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ComponentData& theData) {
		theData.type = static_cast<ComponentType>(getUint8(jsonObjectData, "type"));

		theData.customId = getString(jsonObjectData, "custom_id");

		theData.placeholder = getString(jsonObjectData, "placeholder");

		theData.disabled = getBool(jsonObjectData, "disabled");

		theData.style = getUint32(jsonObjectData, "style");

		theData.label = getString(jsonObjectData, "label");

		theData.minLength = getUint32(jsonObjectData, "min_length");

		theData.maxLength = getUint32(jsonObjectData, "max_length");

		theData.maxValues = getUint32(jsonObjectData, "max_values");

		theData.maxValues = getUint32(jsonObjectData, "min_values");

		theData.title = getString(jsonObjectData, "title");

		theData.required = getBool(jsonObjectData, "required");

		simdjson::ondemand::value theEmoji{};
		auto theResult = jsonObjectData["emoji"].get(theEmoji);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theEmoji, theData.emoji);
		}

		theData.url = getString(jsonObjectData, "url");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["select_option_data"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.options.clear();
			SelectOptionData newData{};
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.options.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ActionRowData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["components"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.components.clear();
			ComponentData newData{};
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.components.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, UserCommandInteractionData& theData) {
		theData.targetId = getId(jsonObjectData, "target_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageCommandInteractionData& theData) {
		theData.targetId = getId(jsonObjectData, "target_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageInteractionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		theData.name = getString(jsonObjectData, "name");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.user);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StickerData& theData) {
		theData.asset = getString(jsonObjectData, "asset");

		theData.description = getString(jsonObjectData, "description");

		theData.formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		theData.stickerFlags |= setBool(theData.stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.id = getId(jsonObjectData, "id");

		theData.packId = getString(jsonObjectData, "pack_id");

		theData.type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		theData.sortValue = getUint32(jsonObjectData, "sort_value");

		theData.name = getString(jsonObjectData, "name");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.user);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StickerItemData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		theData.formatType = static_cast<StickerItemType>(getUint8(jsonObjectData, "format_type"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ChannelMentionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageData& theData) {
		theData.content = getString(jsonObjectData, "content");

		theData.id = getId(jsonObjectData, "id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["author"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.author);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.member);
		}

		theData.timestamp = getString(jsonObjectData, "timestamp");

		theData.editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		theData.tts = getBool(jsonObjectData, "tts");

		theData.mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		UserData newData{};
		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["mentions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.mentions.clear();
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.mentions.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["mention_roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.mentionRoles.clear();
			for (auto value: theArray) {
				auto theObject = value.get_string().take_value().data();
				theData.mentionRoles.emplace_back(std::move(theObject));
			}
		}

		theResult = jsonObjectData["mention_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.mentionChannels.clear();
			ChannelMentionData newChannelData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newChannelData);
				theData.mentionChannels.emplace_back(std::move(newChannelData));
			}
		}

		theResult = jsonObjectData["attachments"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.attachments.clear();
			AttachmentData newAttachmentData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newAttachmentData);
				theData.attachments.emplace_back(std::move(newAttachmentData));
			}
		}

		theResult = jsonObjectData["embeds"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.embeds.clear();
			EmbedData newEmbedData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newEmbedData);
				theData.embeds.emplace_back(std::move(newEmbedData));
			}
		}

		theResult = jsonObjectData["reactions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.reactions.clear();
			ReactionData newReactionData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newReactionData);
				theData.reactions.emplace_back(std::move(newReactionData));
			}
		}

		theData.nonce = getString(jsonObjectData, "nonce");

		theData.pinned = getBool(jsonObjectData, "pinned");

		theData.webHookId = getId(jsonObjectData, "webhook_id");

		theData.type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		theResult = jsonObjectData["activity"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.activity);
		}

		theResult = jsonObjectData["application"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.application);
		}

		theData.applicationId = getId(jsonObjectData, "application_id");

		theResult = jsonObjectData["message_reference"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.messageReference);
		}

		theData.flags = getUint32(jsonObjectData, "flags");

		theResult = jsonObjectData["sticker_items"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.stickerItems.clear();
			StickerItemData newReactionData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newReactionData);
				theData.stickerItems.emplace_back(std::move(newReactionData));
			}
		}

		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.stickers.clear();
			StickerData newReactionData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newReactionData);
				theData.stickers.emplace_back(std::move(newReactionData));
			}
		}

		theResult = jsonObjectData["interaction"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.interaction);
		}

		theResult = jsonObjectData["components"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.components.clear();
			ActionRowData newActionRowData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newActionRowData);
				theData.components.emplace_back(std::move(newActionRowData));
			}
		}

		theResult = jsonObjectData["thread"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.thread);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AutoModerationRuleVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theAutoModerationRules.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				AutoModerationRule newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theAutoModerationRules.push_back(newData);
			}
			theData.theAutoModerationRules.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Channel& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.flags = getUint8(jsonObjectData, "flags");

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.defaultAutoArchiveDuration = getUint32(jsonObjectData, "default_auto_archive_duration");

		theData.position = getUint32(jsonObjectData, "position");

		simdjson::ondemand::object theArray{};
		auto theResult = jsonObjectData["permission_overwrites"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.permissionOverwrites.clear();
			for (auto value: theArray) {
				OverWriteData theDataNew{};
				auto theObject = value.value().value();
				parseObject(theObject, theDataNew);
				theData.permissionOverwrites.emplace_back(std::move(theDataNew));
			}
		}

		theData.name = getString(jsonObjectData, "name");

		theData.topic = getString(jsonObjectData, "topic");

		theData.permissions = getString(jsonObjectData, "permissions");

		theData.flags = setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		theData.lastMessageId = getString(jsonObjectData, "last_message_id");

		theData.bitrate = getUint32(jsonObjectData, "bitrate");

		theData.userLimit = getUint32(jsonObjectData, "user_limit");

		theData.rateLimitPerUser = getUint32(jsonObjectData, "rate_limit_per_user");

		theResult = jsonObjectData["recipients"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.recipients.clear();
			for (auto value: theArray) {
				UserData theDataNew{};
				auto theObject = value.value().value();
				parseObject(theObject, theDataNew);
				theData.recipients.emplace(theDataNew.id, std::move(theDataNew));
			}
		}

		theData.icon = getString(jsonObjectData, "icon");

		theData.ownerId = getId(jsonObjectData, "owner_id");

		theData.applicationId = getId(jsonObjectData, "application_id");

		theData.parentId = getId(jsonObjectData, "parent_id");

		theData.lastPinTimestamp = getString(jsonObjectData, "last_pin_timestamp");

		theData.rtcRegion = getString(jsonObjectData, "rtc_region");

		theData.videoQualityMode = getUint32(jsonObjectData, "video_quality_mode");

		theData.messageCount = getUint32(jsonObjectData, "message_count");

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["thread_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.threadMetadata);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ChannelVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theChannels.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				Channel newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theChannels.push_back(newData);
			}
			theData.theChannels.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, RoleTagsData& theData) {
		theData.botId = getString(jsonObjectData, "bot_id");

		theData.integrationId = getString(jsonObjectData, "integration_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageReferenceData& theData) {
		theData.messageId = getId(jsonObjectData, "message_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.failIfNotExists = getBool(jsonObjectData, "fail_if_not_exists");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedFooterData& theData) {
		theData.text = getString(jsonObjectData, "text");

		theData.iconUrl = getString(jsonObjectData, "icon_url");

		theData.proxyIconUrl = getString(jsonObjectData, "proxy_icon_url");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedImageData& theData) {
		theData.url = getString(jsonObjectData, "url");

		theData.proxyUrl = getString(jsonObjectData, "proxy_url");

		theData.width = getUint32(jsonObjectData, "width");

		theData.height = getUint32(jsonObjectData, "height");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedThumbnailData& theData) {
		theData.url = getString(jsonObjectData, "url");

		theData.proxyUrl = getString(jsonObjectData, "proxy_url");

		theData.width = getUint32(jsonObjectData, "width");

		theData.height = getUint32(jsonObjectData, "height");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedVideoData& theData) {
		theData.url = getString(jsonObjectData, "url");

		theData.proxyUrl = getString(jsonObjectData, "proxy_url");

		theData.width = getUint32(jsonObjectData, "width");

		theData.height = getUint32(jsonObjectData, "height");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedProviderData& theData) {
		theData.url = getString(jsonObjectData, "url");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedAuthorData& theData) {
		theData.url = getString(jsonObjectData, "url");

		theData.proxyIconUrl = getString(jsonObjectData, "proxy_icon_url");

		theData.name = getString(jsonObjectData, "name");

		theData.iconUrl = getString(jsonObjectData, "icon_url");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedFieldData& theData) {
		theData.Inline = getBool(jsonObjectData, "inline");

		theData.name = getString(jsonObjectData, "name");

		theData.value = getString(jsonObjectData, "value");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedData& theData) {
		theData.title = getString(jsonObjectData, "title");

		theData.type = getString(jsonObjectData, "type");

		theData.description = getString(jsonObjectData, "description");

		theData.url = getString(jsonObjectData, "url");

		theData.timestamp = getString(jsonObjectData, "timestamp");

		theData.hexColorValue = getUint32(jsonObjectData, "color");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["footer"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.footer);
		}

		theResult = jsonObjectData["image"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.image);
		}

		theResult = jsonObjectData["provider"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.provider);
		}

		theResult = jsonObjectData["thumbnail"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.thumbnail);
		}

		theResult = jsonObjectData["video"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.video);
		}

		theResult = jsonObjectData["author"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.author);
		}

		EmbedFieldData newData{};
		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["fields"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.fields.clear();
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.fields.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, VoiceStateData& theData) {
		theData.requestToSpeakTimestamp = getString(jsonObjectData, "request_to_speak_timestamp");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.selfStream = getBool(jsonObjectData, "self_stream");

		theData.userId = getId(jsonObjectData, "user_id");

		theData.selfVideo = getBool(jsonObjectData, "self_video");

		theData.sessionId = getString(jsonObjectData, "session_id");

		theData.selfDeaf = getBool(jsonObjectData, "self_deaf");

		theData.selfMute = getBool(jsonObjectData, "self_mute");

		theData.suppress = getBool(jsonObjectData, "suppress");

		theData.deaf = getBool(jsonObjectData, "deaf");

		theData.mute = getBool(jsonObjectData, "mute");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ThreadMemberData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.userId = getId(jsonObjectData, "user_id");

		theData.joinTimestamp = getString(jsonObjectData, "join_timestamp");

		theData.flags = getUint32(jsonObjectData, "flags");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmojiData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.roles.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				RoleData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.roles.push_back(newData);
			}
			theData.roles.shrink_to_fit();
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.user);
		}

		theData.requireColons = getBool(jsonObjectData, "require_colons");

		theData.managed = getBool(jsonObjectData, "managed");

		theData.animated = getBool(jsonObjectData, "animated");

		theData.available = getBool(jsonObjectData, "available");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmojiDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theEmojiDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				EmojiData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theEmojiDatas.push_back(newData);
			}
			theData.theEmojiDatas.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, PresenceUpdateFlags& theData) {
		uint8_t theDataNew = static_cast<uint8_t>(theData);

		auto theStringNew = getString(jsonObjectData, "dekstop");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Dnd, true));
		}

		theStringNew = getString(jsonObjectData, "mobile");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Dnd, true));
		}

		theStringNew = getString(jsonObjectData, "web");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Dnd, true));
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, PresenceUpdateData& theData) {
		theData.guildId = stoull(getString(jsonObjectData, "guild_id"));

		auto theStringNew = getString(jsonObjectData, "status");

		if (theStringNew == "online") {
			theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Online, true);
		} else if (theStringNew == "idle") {
			theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Idle, true);
		} else if (theStringNew == "dnd") {
			theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Dnd, true);
		}

		PresenceUpdateFlags theFlags{};
		parseObject(jsonObjectData, theFlags);

		theData.theStatus |= static_cast<uint8_t>(theFlags);
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, TriggerMetaData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["keyword_filter"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.keywordFilter.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.keywordFilter.push_back(value.get_string().value().data());
			}
			theData.keywordFilter.shrink_to_fit();
		}

		theResult = jsonObjectData["keyword_filter"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.presets.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.presets.push_back(static_cast<KeywordPresetType>(value.get_uint64().value()));
			}
			theData.presets.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ActionMetaData& theData) {
		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.durationSeconds = getUint64(jsonObjectData, "duration_seconds");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ActionData& theData) {

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.metadata);
		}

		theData.type = static_cast<ActionType>(getUint64(jsonObjectData, "type"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommandPermissionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.permission = getBool(jsonObjectData, "permission");

		theData.type = static_cast<ApplicationCommandPermissionType>(getUint8(jsonObjectData, "type"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ReactionData& theData) {
		theData.count = getUint32(jsonObjectData, "count");

		theData.me = getBool(jsonObjectData, "me");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["emoji"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.emoji);
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.userId = getId(jsonObjectData, "user_id");

		theData.messageId = getId(jsonObjectData, "message_id");

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, VoiceRegionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.custom = getBool(jsonObjectData, "custom");

		theData.deprecated = getBool(jsonObjectData, "deprecated");

		theData.optimal = getBool(jsonObjectData, "optimal");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageActivityData& theData) {
		theData.type = static_cast<MessageActivityType>(getUint8(jsonObjectData, "type"));

		theData.partyId = getString(jsonObjectData, "party_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationData& theData) {
		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["params"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.params);
		}

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["tags"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.tags.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.tags.push_back(value.get_string().value().data());
			}
			theData.tags.shrink_to_fit();
		}

		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		theData.icon = getString(jsonObjectData, "icon");

		theData.description = getString(jsonObjectData, "description");

		theResult = jsonObjectData["rpc_origins"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.rpcOrigins.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.rpcOrigins.push_back(value.get_string().value().data());
			}
			theData.rpcOrigins.shrink_to_fit();
		}

		theData.botPublic = getBool(jsonObjectData, "bot_public");

		theData.botRequireCodeGrant = getBool(jsonObjectData, "bot_require_code_grant");

		theData.termsOfServiceUrl = getString(jsonObjectData, "terms_of_service_url");

		theData.privacyPolicyUrl = getString(jsonObjectData, "privacy_policy_url");

		theResult = jsonObjectData["owner"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.owner);
		}

		theResult = jsonObjectData["team"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.team);
		}

		theData.summary = getString(jsonObjectData, "summary");

		theData.verifyKey = getString(jsonObjectData, "verify_key");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.primarySkuId = getString(jsonObjectData, "primary_sku_id");

		theData.slug = getString(jsonObjectData, "slug");

		theData.coverImage = getString(jsonObjectData, "cover_image");

		theData.flags = static_cast<ApplicationFlags>(getUint8(jsonObjectData, "flags"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, TeamObjectData& theData) {
		theData.icon = getString(jsonObjectData, "icon");

		theData.id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.members.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				TeamMembersObjectData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.members.push_back(newData);
			}
			theData.members.shrink_to_fit();
		}

		theData.ownerUserId = getId(jsonObjectData, "owner_user_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, TeamMembersObjectData& theData) {
		theData.membershipState = getUint32(jsonObjectData, "membership_state");

		theData.teamId = getId(jsonObjectData, "team_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["permissions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.permissions.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.permissions.push_back(Permissions{ value.get_string().value().data() });
			}
			theData.permissions.shrink_to_fit();
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.user);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, InstallParamsData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["scopes"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.scopes.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.scopes.push_back(Permissions{ value.get_string().value().data() });
			}
			theData.scopes.shrink_to_fit();
		}

		theData.permissions = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, WelcomeScreenData& theData) {
		theData.description = getString(jsonObjectData, "description");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["welcome_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.welcomeChannels.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				WelcomeScreenChannelData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.welcomeChannels.push_back(newData);
			}
			theData.welcomeChannels.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, WelcomeScreenChannelData& theData) {
		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.description = getString(jsonObjectData, "description");

		theData.emojiId = getId(jsonObjectData, "emoji_id");

		theData.emojiName = getString(jsonObjectData, "emoji_name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StageInstanceData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.topic = getString(jsonObjectData, "topic");

		theData.privacyLevel = static_cast<StageInstancePrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildScheduledEventData& theData) {
		theData.privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		theData.status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["entity_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.entityMetadata);
		}

		theData.scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		theData.scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		theData.userCount = getUint32(jsonObjectData, "user_count");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.creatorId = getId(jsonObjectData, "creator_id");

		theData.entityId = getId(jsonObjectData, "entity_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.id = getId(jsonObjectData, "id");

		theData.description = getString(jsonObjectData, "description");

		theResult = jsonObjectData["creator"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.creator);
		}

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommandOptionData& theData) {
		theData.name = getString(jsonObjectData, "name");

		simdjson::ondemand::object theMap{};
		auto theResult= jsonObjectData["name_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.nameLocalizations.clear();
			for (auto value: theMap) {
				theData.nameLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}

		theResult = jsonObjectData["description_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.descriptionLocalizations.clear();
			for (auto value: theMap) {
				theData.descriptionLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}

		theData.description = getString(jsonObjectData, "description");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["channel_types"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.channelTypes.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				theData.channelTypes.push_back(static_cast<ChannelType>(value.get_uint64().value()));
			}
			theData.channelTypes.shrink_to_fit();
		}

		theData.minValue = getUint32(jsonObjectData, "min_value");

		theData.maxValue = getUint32(jsonObjectData, "max_value");

		theData.required = getBool(jsonObjectData, "required");

		theData.autocomplete = getBool(jsonObjectData, "autocomplete");

		theData.type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		if (theData.type == ApplicationCommandOptionType::Sub_Command_Group || theData.type == ApplicationCommandOptionType::Sub_Command) {
			auto theResult01 = jsonObjectData["options"].get(theArray);
			auto theResult02 = jsonObjectData["choices"].get(theArray);
			if (theResult01 == simdjson::error_code::SUCCESS) {
				theData.options.reserve(theArray.count_elements().take_value());
				for (auto value: theArray) {
					ApplicationCommandOptionData newData{};
					auto theObject = value.value();
					parseObject(theObject, newData);
					theData.options.push_back(newData);
				}
				theData.options.shrink_to_fit();
			} else if (theResult02 == simdjson::error_code::SUCCESS) {
				theData.choices.reserve(theArray.count_elements().take_value());
				for (auto value: theArray) {
					ApplicationCommandOptionChoiceData newData{};
					auto theObject = value.value();
					parseObject(theObject, newData);
					theData.choices.push_back(newData);
				}
				theData.choices.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommandOptionChoiceData& theData) {
		std::string_view theString{};
		auto theResult01 = jsonObjectData["value"].get(theString);
		uint64_t theInt{};
		auto theResult02 = jsonObjectData["value"].get(theInt);
		bool theBool{};
		auto theResult03 = jsonObjectData["value"].get(theBool);
		double theDouble{};
		auto theResult04 = jsonObjectData["value"].get(theDouble);

		if (theResult01 == simdjson::error_code::SUCCESS) {
			theData.valueString = theString;
			theData.valueStringReal = theData.valueString.data();
			theData.type = JsonType::String;
		} else if (theResult02 == simdjson::error_code::SUCCESS) {
			theData.valueInt = theInt;
			theData.type = JsonType::Integer;
		} else if (theResult03 == simdjson::error_code::SUCCESS) {
			theData.type = JsonType::Boolean;
			theData.valueBool = theBool;
		} else if (theResult04 == simdjson::error_code::SUCCESS) {
			theData.type = JsonType::Float;
			theData.valueFloat = theDouble;
		}

		theData.name = getString(jsonObjectData, "name");

		simdjson::ondemand::object theMap{};
		auto theResult= jsonObjectData["name_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.nameLocalizations.clear();
			for (auto value: theMap) {
				theData.nameLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Sticker& theData) {
		theData.asset = getString(jsonObjectData, "asset");

		theData.description = getString(jsonObjectData, "description");

		theData.formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		theData.stickerFlags = setBool(theData.stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.packId = getString(jsonObjectData, "pack_id");

		theData.type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		theData.sortValue = getUint32(jsonObjectData, "sort_value");

		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.user);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildScheduledEvent& theData) {
		theData.privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		theData.status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["entity_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.entityMetadata);
		}

		theData.scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		theData.scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		theData.userCount = getUint32(jsonObjectData, "user_count");

		theData.description = getString(jsonObjectData, "description");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.creatorId = getId(jsonObjectData, "creator_id");

		theData.entityId = getString(jsonObjectData, "entity_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theResult = jsonObjectData["creator"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.creator);
		}

		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StageInstance& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.topic = getString(jsonObjectData, "topic");

		theData.privacyLevel = static_cast<StageInstancePrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ThreadMetadataData& theData) {
		theData.archived = getBool(jsonObjectData, "archived");

		theData.invitable = getBool(jsonObjectData, "invitable");

		theData.autoArchiveDuration = getUint32(jsonObjectData, "auto_archive_duration");

		theData.archiveTimestamp = getString(jsonObjectData, "archive_timestamp");

		theData.locked = getBool(jsonObjectData, "locked");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GatewayBotData& theData) {
		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["session_start_limit"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.sessionStartLimit);
		}

		theData.shards = getUint32(jsonObjectData, "shards");

		theData.url = getString(jsonObjectData, "url");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, SessionStartData& theData) {
		theData.maxConcurrency = getUint32(jsonObjectData, "max_concurrency");

		theData.remaining = getUint32(jsonObjectData, "remaining");

		theData.resetAfter = getUint32(jsonObjectData, "reset_after");

		theData.total = getUint32(jsonObjectData, "total");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, BanData& theData) {
		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.user);
		}

		theData.reason = getString(jsonObjectData, "reason");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, BanDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theBanDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				BanData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theBanDatas.push_back(newData);
			}
			theData.theBanDatas.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildWidgetData& theData) {
		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.channelId = getId(jsonObjectData, "channel_id");
	};

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildWidgetImageData& theData) {
		theData.url = getString(jsonObjectData, "widget_image");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, IntegrationDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theIntegrationDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				IntegrationData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theIntegrationDatas.push_back(newData);
			}
			theData.theIntegrationDatas.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, IntegrationData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		theData.type = getString(jsonObjectData, "type");

		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.syncing = getBool(jsonObjectData, "syncing");

		theData.roleId = getId(jsonObjectData, "role_id");

		theData.enableEmoticons = getBool(jsonObjectData, "enable_emoticons");

		theData.expireBehavior = getUint32(jsonObjectData, "expire_behavior");

		theData.expireGracePeriod = getUint32(jsonObjectData, "expire_grace_period");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.user);
		}

		theResult = jsonObjectData["account"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.account);
		}

		theResult = jsonObjectData["application"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.application);
		}

		theData.syncedAt = getString(jsonObjectData, "synced_at");

		theData.subscriberCount = getUint32(jsonObjectData, "subscriber_count");

		theData.revoked = getBool(jsonObjectData, "revoked");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AccountData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildPruneCountData& theData) {
		theData.count = getUint32(jsonObjectData, "pruned");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildPreviewData& theData) {
		theData.approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		theData.approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		theData.discoverySplash = getString(jsonObjectData, "discovery_splash");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["emojis"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.emojis.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				EmojiData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.emojis.push_back(newData);
			}
			theData.emojis.shrink_to_fit();
		}

		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.stickers.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				StickerData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.stickers.push_back(newData);
			}
			theData.stickers.shrink_to_fit();
		}

		theResult = jsonObjectData["features"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.features.clear();
			for (auto value: theArray) {
				theData.features.emplace_back(value.get_string().take_value().data());
			}
		}

		theData.description = getString(jsonObjectData, "description");

		theData.splash = getString(jsonObjectData, "splash");

		theData.icon = getString(jsonObjectData, "icon");

		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildTemplateData& theData) {
		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["serialized_source_guild"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.serializedSourceGuild);
		}

		theResult = jsonObjectData["creator"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.creator);
		}

		theData.sourceGuildId = getString(jsonObjectData, "source_guild_id");

		theData.description = getString(jsonObjectData, "description");

		theData.usageCount = getUint32(jsonObjectData, "usage_count");

		theData.creatorId = getString(jsonObjectData, "creator_id");

		theData.createdAt = getString(jsonObjectData, "created_at");

		theData.updatedAt = getString(jsonObjectData, "updated_at");

		theData.isDirty = getBool(jsonObjectData, "is_dirty");

		theData.code = getString(jsonObjectData, "code");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildTemplateDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theGuildTemplateDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				GuildTemplateData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theGuildTemplateDatas.push_back(newData);
			}
			theData.theGuildTemplateDatas.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AuditLogData& theData) {

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["webhooks"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.webhooks.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				WebHookData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.webhooks.push_back(newData);
			}
			theData.webhooks.shrink_to_fit();
		}

		theResult = jsonObjectData["guild_scheduled_events"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.guildScheduledEvents.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				GuildScheduledEventData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.guildScheduledEvents.push_back(newData);
			}
			theData.guildScheduledEvents.shrink_to_fit();
		}

		theResult = jsonObjectData["auto_moderation_rules"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.autoModerationRules.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				auto theObject = value.value();
				AutoModerationRuleData newData{};
				parseObject(theObject, newData);
				theData.autoModerationRules.push_back(newData);
			}
			theData.autoModerationRules.shrink_to_fit();
		}

		theResult = jsonObjectData["users"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.users.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				auto theObject = value.value();
				UserData newData{};
				parseObject(theObject, newData);
				theData.users.push_back(newData);
			}
			theData.users.shrink_to_fit();
		}

		theResult = jsonObjectData["audit_log_entries"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.auditLogEntries.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				auto theObject = value.value();
				AuditLogEntryData newData{};
				parseObject(theObject, newData);
				theData.auditLogEntries.push_back(newData);
			}
			theData.auditLogEntries.shrink_to_fit();
		}

		theResult = jsonObjectData["integrations"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.integrations.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				auto theObject = value.value();
				IntegrationData newData{};
				parseObject(theObject, newData);
				theData.integrations.push_back(newData);
			}
			theData.integrations.shrink_to_fit();
		}

		theResult = jsonObjectData["threads"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.threads.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				auto theObject = value.value();
				ChannelData newData{};
				parseObject(theObject, newData);
				theData.threads.push_back(newData);
			}
			theData.threads.shrink_to_fit();
		}
	}
	
	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AutoModerationRuleData& theData) {
		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");

		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.triggerType = static_cast<TriggerType>(getUint8(jsonObjectData, "trigger_type"));

		theData.eventType = static_cast<EventType>(getUint8(jsonObjectData, "event_type"));

		theData.creatorId = getId(jsonObjectData, "creator_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["actions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.actions.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				ActionData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.actions.push_back(newData);
			}
			theData.actions.shrink_to_fit();
		}

		theResult = jsonObjectData["exempt_roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.exemptRoles.clear();
			for (auto value: theArray) {
				std::string theString{ value.get_string().take_value().data() };
				theData.exemptRoles.emplace_back(strtoull(theString));
			}
		}

		theResult = jsonObjectData["exempt_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.exemptChannels.clear();
			for (auto value: theArray) {
				std::string theString{ value.get_string().take_value().data() };
				theData.exemptChannels.emplace_back(strtoull(theString));
			}
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["trigger_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.triggerMetaData);
		}

		theData.guildId = getId(jsonObjectData, "guild_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AuditLogEntryData& theData) {
		theData.targetId = getId(jsonObjectData, "target_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["changes"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.changes.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				AuditLogChangeData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.changes.push_back(newData);
			}
			theData.changes.shrink_to_fit();
		}

		theData.userId = getId(jsonObjectData, "user_id");

		theData.id = getId(jsonObjectData, "id");
		theData.createdTimeStamp = theData.getCreatedAtTimestamp(TimeFormat::LongDateTime);

		theData.actionType = static_cast<AuditLogEvent>(getUint16(jsonObjectData, "action_type"));

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["optinos"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.options);
		}

		theData.reason = getString(jsonObjectData, "reason");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, OptionalAuditEntryInfoData& theData) {
		theData.deleteMemberDays = getString(jsonObjectData, "delete_member_days");

		theData.membersRemoved = getString(jsonObjectData, "members_removed");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.messageId = getId(jsonObjectData, "message_id");

		theData.count = getString(jsonObjectData, "count");

		theData.id = getId(jsonObjectData, "id");

		theData.type = getString(jsonObjectData, "type");

		theData.roleName = getString(jsonObjectData, "role_name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AuditLogChangeData& theData) {
		theData.newValue = getString(jsonObjectData, "new_value");

		theData.oldValue = getString(jsonObjectData, "old_value");

		theData.key = getString(jsonObjectData, "key");
	}

	/*
	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AutoModerationActionExecutionEventData& theData) {
		if (jsonObjectData->contains("alert_system_message_id") && !(*jsonObjectData)["alert_system_message_id"].is_null()) {
			theData.alertSystemMessageId = stoull((*jsonObjectData)["alert_system_message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("rule_trigger_type") && !(*jsonObjectData)["rule_trigger_type"].is_null()) {
			theData.ruleTriggerType = (*jsonObjectData)["rule_trigger_type"].get<TriggerType>();
		}

		if (jsonObjectData->contains("matched_keyword") && !(*jsonObjectData)["matched_keyword"].is_null()) {
			theData.matchedKeyword = (*jsonObjectData)["matched_keyword"].get<std::string>();
		}

		if (jsonObjectData->contains("matched_content") && !(*jsonObjectData)["matched_content"].is_null()) {
			theData.matchedContent = (*jsonObjectData)["matched_content"].get<std::string>();
		}

		if (jsonObjectData->contains("action") && !(*jsonObjectData)["action"].is_null()) {
			parseObject(&(*jsonObjectData)["action"], theData.action);
		}

		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theData.content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("rule_id") && !(*jsonObjectData)["rule_id"].is_null()) {
			theData.ruleId = stoull((*jsonObjectData)["rule_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildVector& theData) {
		theData.theGuilds.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Guild newData{};
			parseObject(&value, newData);
			theData.theGuilds.push_back(newData);
		}
		theData.theGuilds.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildMemberVector& theData) {
		theData.theGuildMembers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildMember newData{};
			parseObject(&value, newData);
			theData.theGuildMembers.push_back(newData);
		}
		theData.theGuildMembers.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Message& theData) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theData.content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			parseObject(&(*jsonObjectData)["author"], theData.author);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theData.member);
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theData.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("edited_timestamp") && !(*jsonObjectData)["edited_timestamp"].is_null()) {
			theData.editedTimestamp = (*jsonObjectData)["edited_timestamp"].get<std::string>();
		}

		theData.tts = getBool(jsonObjectData, "tts");

		theData.mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
			theData.mentions.clear();
			for (auto& value: (*jsonObjectData)["mentions"]) {
				UserData newData{};
				parseObject(&value, newData);
				theData.mentions.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
			theData.mentionRoles.clear();
			for (auto& value: (*jsonObjectData)["mention_roles"]) {
				theData.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
			theData.mentionChannels.clear();
			for (auto& value: (*jsonObjectData)["mention_channels"]) {
				ChannelMentionData newData{};
				parseObject(&value, newData);
				theData.mentionChannels.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				parseObject(&value, newData);
				theData.attachments.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				parseObject(&value, newData);
				theData.embeds.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{};
				parseObject(&value, newData);
				theData.reactions.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			theData.nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		theData.pinned = getBool(jsonObjectData, "pinned");

		if (jsonObjectData->contains("webhook_id") && !(*jsonObjectData)["webhook_id"].is_null()) {
			theData.webHookId = stoull((*jsonObjectData)["webhook_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<MessageType>();
		}

		if (jsonObjectData->contains("activity") && !(*jsonObjectData)["activity"].is_null()) {
			parseObject(&(*jsonObjectData)["activity"], theData.activity);
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			parseObject(&(*jsonObjectData)["application"], theData.application);
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
			parseObject(&(*jsonObjectData)["message_reference"], theData.messageReference);
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theData.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{};
				parseObject(&value, newData);
				theData.stickerItems.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theData.stickers.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
			parseObject(&(*jsonObjectData)["interaction"], theData.interaction);
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{};
				parseObject(&value, newData);
				theData.components.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
			parseObject(&(*jsonObjectData)["thread"], theData.thread);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageVector& theData) {
		theData.theMessages.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Message newData{};
			parseObject(&value, newData);
			theData.theMessages.push_back(newData);
		}
		theData.theMessages.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Reaction& theData) {
		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			theData.count = (*jsonObjectData)["count"].get<int32_t>();
		}

		theData.me = getBool(jsonObjectData, "me");

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			parseObject(&(*jsonObjectData)["emoji"], theData.emoji);
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theData.member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ReactionVector& theData) {
		theData.theReactions.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Reaction newData{};
			parseObject(&value, newData);
			theData.theReactions.push_back(newData);
		}
		theData.theReactions.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, RoleVector& theData) {
		theData.theRoles.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Role newData{};
			parseObject(&value, newData);
			theData.theRoles.push_back(newData);
		}
		theData.theRoles.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StickerVector& theData) {
		theData.theStickers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			Sticker newData{};
			parseObject(&value, newData);
			theData.theStickers.push_back(newData);
		}
		theData.theStickers.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Thread& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			if ((*jsonObjectData)["id"].is_string()) {
				theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
			} else {
				theData.id = (*jsonObjectData)["id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("position") && !(*jsonObjectData)["position"].is_null()) {
			theData.position = (*jsonObjectData)["position"].get<int32_t>();
		}

		if (jsonObjectData->contains("permission_overwrites") && !(*jsonObjectData)["permission_overwrites"].is_null()) {
			theData.permissionOverwrites.clear();
			for (auto& value: (*jsonObjectData)["permission_overwrites"]) {
				theData.permissionOverwrites.emplace_back(value);
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			theData.topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("nsfw") && !(*jsonObjectData)["nsfw"].is_null()) {
			theData.flags = setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));
		}

		if (jsonObjectData->contains("last_message_id") && !(*jsonObjectData)["last_message_id"].is_null()) {
			theData.lastMessageId = (*jsonObjectData)["last_message_id"].get<std::string>();
		}

		if (jsonObjectData->contains("bitrate") && !(*jsonObjectData)["bitrate"].is_null()) {
			theData.bitrate = (*jsonObjectData)["bitrate"].get<int32_t>();
		}

		if (jsonObjectData->contains("user_limit") && !(*jsonObjectData)["user_limit"].is_null()) {
			theData.userLimit = (*jsonObjectData)["user_limit"].get<int32_t>();
		}

		if (jsonObjectData->contains("rate_limit_per_user") && !(*jsonObjectData)["rate_limit_per_user"].is_null()) {
			theData.rateLimitPerUser = (*jsonObjectData)["rate_limit_per_user"].get<int32_t>();
		}

		if (jsonObjectData->contains("recipients") && !(*jsonObjectData)["recipients"].is_null()) {
			theData.recipients.clear();
			for (auto& value: (*jsonObjectData)["recipients"]) {
				UserData newData{};
				parseObject(&value, newData);
				theData.recipients[newData.id] = std::move(newData);
			}
		}

		if (jsonObjectData->contains("icon") && !(*jsonObjectData)["icon"].is_null()) {
			theData.icon = (*jsonObjectData)["icon"].get<std::string>();
		}

		if (jsonObjectData->contains("owner_id") && !(*jsonObjectData)["owner_id"].is_null()) {
			theData.ownerId = stoull((*jsonObjectData)["owner_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("parent_id") && !(*jsonObjectData)["parent_id"].is_null()) {
			if ((*jsonObjectData)["parent_id"].is_string()) {
				theData.parentId = stoull((*jsonObjectData)["parent_id"].get<std::string>());
			} else {
				theData.parentId = (*jsonObjectData)["parent_id"].get<int64_t>();
			}
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			theData.lastPinTimestamp = (*jsonObjectData)["last_pin_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("rtc_region") && !(*jsonObjectData)["rtc_region"].is_null()) {
			theData.rtcRegion = (*jsonObjectData)["rtc_region"].get<std::string>();
		}

		if (jsonObjectData->contains("video_quality_mode") && !(*jsonObjectData)["video_quality_mode"].is_null()) {
			theData.videoQualityMode = (*jsonObjectData)["video_quality_mode"].get<int32_t>();
		}

		if (jsonObjectData->contains("message_count") && !(*jsonObjectData)["message_count"].is_null()) {
			theData.messageCount = (*jsonObjectData)["message_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			theData.memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("thread_metadata") && !(*jsonObjectData)["thread_metadata"].is_null()) {
			parseObject(&(*jsonObjectData)["thread_metadata"], theData.threadMetadata);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theData.member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, UserVector& theData) {
		theData.theUsers.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			User newData{};
			parseObject(&value, newData);
			theData.theUsers.push_back(newData);
		}
		theData.theUsers.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, WebHook& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<WebHookType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("avatar") && !(*jsonObjectData)["avatar"].is_null()) {
			theData.avatar = (*jsonObjectData)["avatar"].get<std::string>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			theData.token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("source_guild") && !(*jsonObjectData)["source_guild"].is_null()) {
			parseObject(&(*jsonObjectData)["source_guild"], theData.sourceGuild);
		}

		if (jsonObjectData->contains("source_channel") && !(*jsonObjectData)["source_channel"].is_null()) {
			parseObject(&(*jsonObjectData)["source_channel"], theData.sourceChannel);
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, WebHookVector& theData) {
		theData.theWebHooks.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			WebHook newData{};
			parseObject(&value, newData);
			theData.theWebHooks.push_back(newData);
		}
		theData.theWebHooks.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedFooterData& theData) {
		if (jsonObjectData->contains("text") && !(*jsonObjectData)["text"].is_null()) {
			theData.text = (*jsonObjectData)["text"].get<std::string>();
		}
		if (jsonObjectData->contains("icon_url") && !(*jsonObjectData)["icon_url"].is_null()) {
			theData.iconUrl = (*jsonObjectData)["icon_url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_icon_url") && !(*jsonObjectData)["proxy_icon_url"].is_null()) {
			theData.proxyIconUrl = (*jsonObjectData)["proxy_icon_url"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedImageData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theData.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theData.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theData.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedThumbnailData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theData.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theData.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theData.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedVideoData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_url") && !(*jsonObjectData)["proxy_url"].is_null()) {
			theData.proxyUrl = (*jsonObjectData)["proxy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("width") && !(*jsonObjectData)["width"].is_null()) {
			theData.width = (*jsonObjectData)["width"].get<int32_t>();
		}

		if (jsonObjectData->contains("height") && !(*jsonObjectData)["height"].is_null()) {
			theData.height = (*jsonObjectData)["height"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedProviderData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedAuthorData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_icon_url") && !(*jsonObjectData)["proxy_icon_url"].is_null()) {
			theData.proxyIconUrl = (*jsonObjectData)["proxy_icon_url"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("icon_url") && !(*jsonObjectData)["icon_url"].is_null()) {
			theData.iconUrl = (*jsonObjectData)["icon_url"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedFieldData& theData) {
		if (jsonObjectData->contains("inline") && !(*jsonObjectData)["inline"].is_null()) {
			auto& theString = (*jsonObjectData)["inline"];
			if ((*jsonObjectData)["inline"].is_string()) {
				if (theString == "true") {
					theData.Inline = true;
				} else {
					theData.Inline = false;
				}
			} else {
				theData.Inline = theString;
			}
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			theData.value = (*jsonObjectData)["value"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmbedData& theData) {
		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null()) {
			theData.title = (*jsonObjectData)["title"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theData.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("color") && !(*jsonObjectData)["color"].is_null()) {
			theData.hexColorValue = (*jsonObjectData)["color"].get<int32_t>();
		}

		if (jsonObjectData->contains("footer") && !(*jsonObjectData)["footer"].is_null()) {
			parseObject(&(*jsonObjectData)["footer"], theData.footer);
		}

		if (jsonObjectData->contains("image") && !(*jsonObjectData)["image"].is_null()) {
			parseObject(&(*jsonObjectData)["image"], theData.image);
		}

		if (jsonObjectData->contains("provider") && !(*jsonObjectData)["provider"].is_null()) {
			parseObject(&(*jsonObjectData)["provider"], theData.provider);
		}

		if (jsonObjectData->contains("thumbnail") && !(*jsonObjectData)["thumbnail"].is_null()) {
			parseObject(&(*jsonObjectData)["thumbnail"], theData.thumbnail);
		}

		if (jsonObjectData->contains("video") && !(*jsonObjectData)["video"].is_null()) {
			parseObject(&(*jsonObjectData)["video"], theData.video);
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			parseObject(&(*jsonObjectData)["author"], theData.author);
		}

		if (jsonObjectData->contains("fields") && !(*jsonObjectData)["fields"].is_null()) {
			theData.fields.clear();
			for (auto& value: (*jsonObjectData)["fields"]) {
				EmbedFieldData newData{};
				parseObject(&value, newData);
				theData.fields.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageReferenceData& theData) {
		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		theData.failIfNotExists = getBool(jsonObjectData, "fail_if_not_exists");
	}

	

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ThreadMemberDataVector& theData) {
		theData.theThreadMemberDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ThreadMemberData newData{};
			parseObject(&value, newData);
			theData.theThreadMemberDatas.push_back(newData);
		}
		theData.theThreadMemberDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ActiveThreadsData& theData) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				parseObject(&value, newData);
				theData.threads.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				theData.members.emplace_back(std::move(newData));
			}
		}

		theData.hasMore = getBool(jsonObjectData, "has_more");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ArchivedThreadsData& theData) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				parseObject(&value, newData);
				theData.threads.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				parseObject(&value, newData);
				theData.members.emplace_back(std::move(newData));
			}
		}

		theData.hasMore = getBool(jsonObjectData, "has_more");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildApplicationCommandPermissionsData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theData.permissions.clear();
			for (auto& value: (*jsonObjectData)["permissions"]) {
				ApplicationCommandPermissionData newData{};
				parseObject(&value, newData);
				theData.permissions.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AuthorizationInfoData& theData) {
		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			parseObject(&(*jsonObjectData)["application"], theData.application);
		}

		if (jsonObjectData->contains("scopes") && !(*jsonObjectData)["scopes"].is_null()) {
			theData.scopes = (*jsonObjectData)["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("expires") && !(*jsonObjectData)["expires"].is_null()) {
			theData.expires = (*jsonObjectData)["expires"];
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theData.user);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, PartyData& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("Size") && !(*jsonObjectData)["Size"].is_null()) {
			theData.size[0] = (*jsonObjectData)["Size"]["CurrentSize"].get<int32_t>();
			theData.size[1] = (*jsonObjectData)["Size"]["MaxSize"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AssetsData& theData) {
		if (jsonObjectData->contains("LargeImage") && !(*jsonObjectData)["LargeImage"].is_null()) {
			theData.largeImage = (*jsonObjectData)["LargeImage"].get<std::string>();
		}

		if (jsonObjectData->contains("LargeText") && !(*jsonObjectData)["LargeText"].is_null()) {
			theData.largeText = (*jsonObjectData)["LargeText"].get<std::string>();
		}

		if (jsonObjectData->contains("SmallImage") && !(*jsonObjectData)["SmallImage"].is_null()) {
			theData.smallImage = (*jsonObjectData)["SmallImage"].get<std::string>();
		}

		if (jsonObjectData->contains("SmallText") && !(*jsonObjectData)["SmallText"].is_null()) {
			theData.smallText = (*jsonObjectData)["SmallText"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, SecretsData& theData) {
		if (jsonObjectData->contains("Join") && !(*jsonObjectData)["Join"].is_null()) {
			theData.join = (*jsonObjectData)["Join"].get<std::string>();
		}

		if (jsonObjectData->contains("Spectate") && !(*jsonObjectData)["Spectate"].is_null()) {
			theData.spectate = (*jsonObjectData)["Spectate"].get<std::string>();
		}

		if (jsonObjectData->contains("Match") && !(*jsonObjectData)["Match"].is_null()) {
			theData.match = (*jsonObjectData)["Match"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, TimestampData& theData) {
		if (jsonObjectData->contains("Start") && !(*jsonObjectData)["Start"].is_null()) {
			theData.start = (*jsonObjectData)["Start"].get<int64_t>();
		}

		if (jsonObjectData->contains("End") && !(*jsonObjectData)["End"].is_null()) {
			theData.end = (*jsonObjectData)["End"].get<int64_t>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ActivityData& theData) {
		if (jsonObjectData->contains("Name") && !(*jsonObjectData)["Name"].is_null()) {
			theData.name = (*jsonObjectData)["Name"].get<std::string>();
		}

		if (jsonObjectData->contains("Timestamps") && !(*jsonObjectData)["Timestamps"].is_null()) {
			parseObject(&(*jsonObjectData)["Timestamps"], theData.timestamps);
		}

		if (jsonObjectData->contains("ApplicationId") && !(*jsonObjectData)["ApplicationId"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["ApplicationId"].get<std::string>());
		}

		if (jsonObjectData->contains("Details") && !(*jsonObjectData)["Details"].is_null()) {
			theData.details = (*jsonObjectData)["Details"].get<std::string>();
		}

		if (jsonObjectData->contains("State") && !(*jsonObjectData)["State"].is_null()) {
			theData.state = (*jsonObjectData)["State"].get<std::string>();
		}

		if (jsonObjectData->contains("ActivityParty") && !(*jsonObjectData)["ActivityParty"].is_null()) {
			parseObject(&(*jsonObjectData)["ActivityParty"], theData.party);
		}

		if (jsonObjectData->contains("ActivityAssets") && !(*jsonObjectData)["ActivityAssets"].is_null()) {
			parseObject(&(*jsonObjectData)["ActivityAssets"], theData.assets);
		}

		if (jsonObjectData->contains("ActivitySecrets") && !(*jsonObjectData)["ActivitySecrets"].is_null()) {
			parseObject(&(*jsonObjectData)["ActivitySecrets"], theData.secrets);
		}

		theData.instance = getBool(jsonObjectData, "Instance");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, PresenceUpdateFlags& theData) {
		uint8_t theDataNew = static_cast<uint8_t>(theData);
		if (jsonObjectData->contains("desktop") && !(*jsonObjectData)["desktop"].is_null()) {
			if ((*jsonObjectData)["desktop"].get<std::string>() == "online") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Online, true));
			} else if ((*jsonObjectData)["desktop"].get<std::string>() == "idle") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Idle, true));
			} else if ((*jsonObjectData)["desktop"].get<std::string>() == "dnd") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Dnd, true));
			}
		}

		if (jsonObjectData->contains("mobile") && !(*jsonObjectData)["mobile"].is_null()) {
			if ((*jsonObjectData)["mobile"].get<std::string>() == "online") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Online, true));
			} else if ((*jsonObjectData)["mobile"].get<std::string>() == "idle") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Idle, true));
			} else if ((*jsonObjectData)["mobile"].get<std::string>() == "dnd") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Dnd, true));
			}
		}

		if (jsonObjectData->contains("web") && !(*jsonObjectData)["web"].is_null()) {
			if ((*jsonObjectData)["web"].get<std::string>() == "online") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Online, true));
			} else if ((*jsonObjectData)["web"].get<std::string>() == "idle") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Idle, true));
			} else if ((*jsonObjectData)["web"].get<std::string>() == "dnd") {
				theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Dnd, true));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, TeamMembersObjectData& theData) {
		theData.membershipState = getUint32(jsonObjectData, "membership_state");

		theData.teamId = getString(jsonObjectData, "team_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["permissions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.permissions.clear();
			for (auto value: theArray) {
				theData.permissions.emplace_back(value.get_string().take_value().data());
			}
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			UserData theUser{};
			parseObject(theObject, theUser);
			Users::insertUser(std::move(theUser));
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, TeamObjectData& theData) {
		theData.icon = getString(jsonObjectData, "icon");

		theData.id = getId(jsonObjectData, "id");

		theData.members.clear();
		TeamMembersObjectData newData{};
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.members.emplace_back(std::move(newData));
			}
		}

		theData.ownerUserId = getId(jsonObjectData, "owner_user_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, InstallParamsData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["scopes"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.scopes.clear();
			for (auto value: theArray) {
				theData.scopes.emplace_back(value.get_string().take_value().data());
			}
		}

		theData.permissions = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationData& theData) {
		simdjson::ondemand::value theParams{};
		auto theResult = jsonObjectData["params"].get(theParams);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theParams, theData.params);
		}

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["tags"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.tags.clear();
			for (auto value: theArray) {
				theData.tags.emplace_back(value.get_string().take_value().data());
			}
		}

		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		theData.icon = getString(jsonObjectData, "icon");

		theData.description = getString(jsonObjectData, "description");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["rpc_origins"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.tags.clear();
			for (auto value: theArray) {
				theData.tags.emplace_back(value.get_string().take_value().data());
			}
		}

		theData.botPublic = getBool(jsonObjectData, "bot_public");

		theData.botRequireCodeGrant = getBool(jsonObjectData, "bot_require_code_grant");

		theData.termsOfServiceUrl = getString(jsonObjectData, "terms_of_service_url");

		theData.privacyPolicyUrl = getString(jsonObjectData, "privacy_policy_url");

		simdjson::ondemand::value theOwner{};
		theResult = jsonObjectData["owner"].get(theParams);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theOwner, theData.owner);
		}

		theData.summary = getString(jsonObjectData, "summary");

		theData.verifyKey = getString(jsonObjectData, "verify_key");

		simdjson::ondemand::value theTeam{};
		theResult = jsonObjectData["team"].get(theParams);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theTeam, theData.team);
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.primarySkuId = getString(jsonObjectData, "primary_sku_id");

		theData.slug = getString(jsonObjectData, "slug");

		theData.coverImage = getString(jsonObjectData, "cover_image");

		theData.flags = static_cast<ApplicationFlags>(getUint8(jsonObjectData, "flags"));
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageActivityData& theData) {
		theData.type = static_cast<MessageActivityType>(getUint8(jsonObjectData, "type"));

		theData.partyId = getString(jsonObjectData, "party_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, EmojiData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		theData.roles.clear();
		RoleData newData{};
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newData);
				newData.guildId = theData.id;
				theData.roles.emplace_back(newData);
				Roles::insertRole(std::move(newData));
			}
		}
		
		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			UserData theUser{};
			parseObject(theObject, theUser);
			theData.user = theUser;
			Users::insertUser(std::move(theUser));
		}

		theData.requireColons = getBool(jsonObjectData, "require_colons");

		theData.managed = getBool(jsonObjectData, "managed");

		theData.animated = getBool(jsonObjectData, "animated");

		theData.available = getBool(jsonObjectData, "available");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ReactionData& theData) {
		theData.count = getUint32(jsonObjectData, "count");

		theData.me = getBool(jsonObjectData, "me");

		parseObject(jsonObjectData, theData.emoji);

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.userId = getId(jsonObjectData, "user_id");

		theData.messageId = getId(jsonObjectData, "message_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			GuildMemberData theUser{};
			parseObject(theObject, theUser);
			theData.member = theUser;
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, PresenceUpdateData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("status") && !(*jsonObjectData)["status"].is_null()) {
			if ((*jsonObjectData)["status"].get<std::string>() == "online") {
				theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Online, true);
			} else if ((*jsonObjectData)["status"].get<std::string>() == "idle") {
				theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Idle, true);
			} else if ((*jsonObjectData)["status"].get<std::string>() == "dnd") {
				theData.theStatus = setBool(theData.theStatus, PresenceUpdateFlags::Status_Dnd, true);
			}
		}

		PresenceUpdateFlags theFlags{};
		if (jsonObjectData->contains("client_status") && !(*jsonObjectData)["client_status"].is_null()) {
			parseObject(&(*jsonObjectData)["client_status"], theFlags);
		}
		theData.theStatus |= static_cast<uint8_t>(theFlags);
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildDataVector& theData) {
		theData.theGuildDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildData newData{};
			parseObject(&value, newData);
			theData.theGuildDatas.push_back(newData);
		}
		theData.theGuildDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildScheduledEventUserData& theData) {
		if (jsonObjectData->contains("guild_scheduled_event_id") && !(*jsonObjectData)["guild_scheduled_event_id"].is_null()) {
			theData.guildScheduledEventId = (*jsonObjectData)["guild_scheduled_event_id"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theData.member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildScheduledEventUserDataVector& theData) {
		theData.theGuildScheduledEventUserDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			GuildScheduledEventUserData newData{};
			parseObject(&value, newData);
			theData.theGuildScheduledEventUserDatas.push_back(newData);
		}
		theData.theGuildScheduledEventUserDatas.shrink_to_fit();
	}

	

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, WebHookData& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<WebHookType>();
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("avatar") && !(*jsonObjectData)["avatar"].is_null()) {
			theData.avatar = (*jsonObjectData)["avatar"].get<std::string>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			theData.token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("source_guild") && !(*jsonObjectData)["source_guild"].is_null()) {
			parseObject(&(*jsonObjectData)["source_guild"], theData.sourceGuild);
		}

		if (jsonObjectData->contains("source_channel") && !(*jsonObjectData)["source_channel"].is_null()) {
			parseObject(&(*jsonObjectData)["source_channel"], theData.sourceChannel);
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, WebHookDataVector& theData) {
		theData.theWebHookDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			WebHookData newData{};
			parseObject(&value, newData);
			theData.theWebHookDatas.push_back(newData);
		}
		theData.theWebHookDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ReactionRemoveData& theData) {
		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			parseObject(&(*jsonObjectData)["emoji"], theData.emoji);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, TypingStartData& theData) {
		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theData.member);
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theData.timestamp = (*jsonObjectData)["timestamp"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, YouTubeFormatVector& theData) {
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

					theData.theFormats.emplace_back(std::move(newData));
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
					theData.theFormats.emplace_back(std::move(newData));
				}
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, UserCommandInteractionData& theData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theData.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageCommandInteractionData& theData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theData.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ComponentInteractionData& theData) {
		if (jsonObjectData->contains("values") && !(*jsonObjectData)["values"].is_null()) {
			theData.values.clear();
			for (auto& value: (*jsonObjectData)["values"]) {
				theData.values.emplace_back(value);
			}
		}

		if (jsonObjectData->contains("custom_id") && !(*jsonObjectData)["custom_id"].is_null()) {
			theData.customId = (*jsonObjectData)["custom_id"].get<std::string>();
		}

		if (jsonObjectData->contains("component_type") && !(*jsonObjectData)["component_type"].is_null()) {
			theData.componentType = (*jsonObjectData)["component_type"].get<ComponentType>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ModalInteractionData& theData) {
		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.value = (*jsonObjectData)["components"][0]["components"][0]["value"].get<std::string>();
		}

		if (jsonObjectData->contains("custom_id") && !(*jsonObjectData)["custom_id"].is_null()) {
			theData.customId = (*jsonObjectData)["custom_id"].get<std::string>();
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.customIdSmall = (*jsonObjectData)["components"][0]["components"][0]["custom_id"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, AllowedMentionsData& theData) {
		if (jsonObjectData->contains("parse") && !(*jsonObjectData)["parse"].is_null()) {
			theData.parse.clear();
			for (auto& value: (*jsonObjectData)["parse"]) {
				theData.parse.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("roles") && !(*jsonObjectData)["roles"].is_null()) {
			theData.roles.clear();
			for (auto& value: (*jsonObjectData)["roles"]) {
				theData.roles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("users") && !(*jsonObjectData)["users"].is_null()) {
			theData.users.clear();
			for (auto& value: (*jsonObjectData)["users"]) {
				theData.users.emplace_back(value.get<std::string>());
			}
		}

		theData.repliedUser = getBool(jsonObjectData, "replied_user");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, SelectOptionData& theData) {
		if (jsonObjectData->contains("label") && !(*jsonObjectData)["label"].is_null()) {
			theData.label = (*jsonObjectData)["label"].get<std::string>();
		}

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			theData.value = (*jsonObjectData)["value"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			parseObject(&(*jsonObjectData)["emoji"], theData.emoji);
		}

		theData._default = getBool(jsonObjectData, "default");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ActionRowData& theData) {
		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.components.clear();
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
					newData.disabled = getBool(&value, "disabled");
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
					newData.required = getBool(&value, "required");
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
						parseObject(&value, newerData);
						newData.options.emplace_back(newerData);
					}
				}

				if (value.contains("max_values") && !value["max_values"].is_null()) {
					newData.maxValues = value["max_values"].get<int32_t>();
				}

				if (value.contains("min_values") && !value["min_values"].is_null()) {
					newData.minValues = value["min_values"].get<int32_t>();
				}

				theData.components.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommandData& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("default_member_permissions") && !(*jsonObjectData)["default_member_permissions"].is_null()) {
			theData.defaultMemberPermissions = (*jsonObjectData)["default_member_permissions"].get<std::string>();
		}

		theData.dmPermission = getBool(jsonObjectData, "dm_permission");

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theData.version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<ApplicationCommandType>();
		}

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.nameLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("description_localizations") && !(*jsonObjectData)["description_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["description_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.descriptionLocalizations[key] = newValue;
			}
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theData.version = (*jsonObjectData)["version"].get<std::string>();
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			theData.options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandOptionData newData{};
				parseObject(&value, newData);
				theData.options.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ChannelMentionData& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ChannelPinsUpdateEventData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			theData.lastPinTimeStamp = (*jsonObjectData)["last_pin_timestamp"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ThreadListSyncData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_ids") && !(*jsonObjectData)["channel_ids"].is_null()) {
			theData.channelIds.clear();
			for (auto& value: (*jsonObjectData)["channel_ids"]) {
				theData.channelIds.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				parseObject(&value, newData);
				theData.members.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				parseObject(&value, newData);
				theData.threads.push_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ThreadMembersUpdateData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			theData.memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("added_members") && !(*jsonObjectData)["added_members"].is_null()) {
			theData.addedMembers.clear();
			for (auto& value: (*jsonObjectData)["added_members"]) {
				ThreadMemberData newData{};
				parseObject(&value, newData);
				theData.addedMembers.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("removed_member_ids") && !(*jsonObjectData)["removed_member_ids"].is_null()) {
			theData.removedMemberIds.clear();
			for (auto& value: (*jsonObjectData)["removed_member_ids"]) {
				theData.removedMemberIds.emplace_back(value);
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageInteractionData& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<InteractionType>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theData.member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StickerItemData& theData) {
		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			theData.formatType = (*jsonObjectData)["format_type"].get<StickerItemType>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, MessageDataOld& theData) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theData.content = (*jsonObjectData)["content"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("channel_id") && !(*jsonObjectData)["channel_id"].is_null()) {
			theData.channelId = stoull((*jsonObjectData)["channel_id"].get<std::string>());
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("author") && !(*jsonObjectData)["author"].is_null()) {
			parseObject(&(*jsonObjectData)["author"], theData.author);
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			parseObject(&(*jsonObjectData)["member"], theData.member);
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theData.timestamp = (*jsonObjectData)["timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("edited_timestamp") && !(*jsonObjectData)["edited_timestamp"].is_null()) {
			theData.editedTimestamp = (*jsonObjectData)["edited_timestamp"].get<std::string>();
		}

		theData.tts = getBool(jsonObjectData, "tts");

		theData.mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		if (jsonObjectData->contains("mentions") && !(*jsonObjectData)["mentions"].is_null()) {
			theData.mentions.clear();
			for (auto& value: (*jsonObjectData)["mentions"]) {
				UserData newData{};
				parseObject(&value, newData);
				theData.mentions.push_back(newData);
			}
		}

		if (jsonObjectData->contains("mention_roles") && !(*jsonObjectData)["mention_roles"].is_null()) {
			theData.mentionRoles.clear();
			for (auto& value: (*jsonObjectData)["mention_roles"]) {
				theData.mentionRoles.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("mention_channels") && !(*jsonObjectData)["mention_channels"].is_null()) {
			theData.mentionChannels.clear();
			for (auto& value: (*jsonObjectData)["mention_channels"]) {
				ChannelMentionData newData{};
				parseObject(&value, newData);
				theData.mentionChannels.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				parseObject(&value, newData);
				theData.attachments.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				parseObject(&value, newData);
				theData.embeds.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{};
				parseObject(&value, newData);
				theData.reactions.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			theData.nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		theData.pinned = getBool(jsonObjectData, "pinned");

		if (jsonObjectData->contains("webhook_id") && !(*jsonObjectData)["webhook_id"].is_null()) {
			theData.webHookId = stoull((*jsonObjectData)["webhook_id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<MessageType>();
		}

		if (jsonObjectData->contains("activity") && !(*jsonObjectData)["activity"].is_null()) {
			parseObject(&(*jsonObjectData)["activity"], theData.activity);
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			parseObject(&(*jsonObjectData)["application"], theData.application);
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
			parseObject(&(*jsonObjectData)["message_reference"], theData.messageReference);
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theData.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{};
				parseObject(&value, newData);
				theData.stickerItems.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theData.stickers.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
			parseObject(&(*jsonObjectData)["interaction"], theData.interaction);
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{};
				parseObject(&value, newData);
				theData.components.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
			parseObject(&(*jsonObjectData)["thread"], theData.thread);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StickerPackData& theData) {
		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theData.stickers.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("cover_sticker_id") && !(*jsonObjectData)["cover_sticker_id"].is_null()) {
			theData.coverStickerId = (*jsonObjectData)["cover_sticker_id"].get<std::string>();
		}

		if (jsonObjectData->contains("banner_asset_id") && !(*jsonObjectData)["banner_asset_id"].is_null()) {
			theData.bannerAssetId = (*jsonObjectData)["banner_asset_id"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.Id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("sku_id") && !(*jsonObjectData)["sku_id"].is_null()) {
			theData.skuId = (*jsonObjectData)["sku_id"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, StickerPackDataVector& theData) {
		theData.theStickerPackDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			StickerPackData newData{};
			parseObject(&value, newData);
			theData.theStickerPackDatas.push_back(newData);
		}
		theData.theStickerPackDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ConnectionData& theData) {
		if (jsonObjectData->contains("name") && !(*jsonObjectData)["name"].is_null()) {
			theData.name = (*jsonObjectData)["name"].get<std::string>();
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<std::string>();
		}

		theData.showActivity = getBool(jsonObjectData, "show_activity");

		theData.friendSync = getBool(jsonObjectData, "friend_sync");

		theData.revoked = getBool(jsonObjectData, "revoked");

		theData.verified = getBool(jsonObjectData, "verified");

		if (jsonObjectData->contains("visibility") && !(*jsonObjectData)["visibility"].is_null()) {
			theData.visibility = (*jsonObjectData)["visibility"].get<ConnectionVisibilityTypes>();
		}

		if (jsonObjectData->contains("integrations") && !(*jsonObjectData)["integrations"].is_null()) {
			theData.integrations.clear();
			for (auto& value: (*jsonObjectData)["integrations"]) {
				IntegrationData newData{};
				parseObject(&value, newData);
				theData.integrations.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ConnectionDataVector& theData) {
		theData.theConnectionDatas.reserve(jsonObjectData->size());
		for (auto& value: *jsonObjectData) {
			ConnectionData newData{};
			parseObject(&value, newData);
			theData.theConnectionDatas.push_back(newData);
		}
		theData.theConnectionDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommandInteractionDataOption& theData) {

		theData.type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		theData.focused = getBool(jsonObjectData, "focused");

		theData.name = getString(jsonObjectData, "name");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["options"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ApplicationCommandInteractionDataOption newData{};
			for (auto theValue: theArray) {
				auto theObject = theValue.value();
				parseObject(theObject, newData);
				theData.options.emplace_back(std::move(newData));
			}
		}

		theData.value.theType = ObjectType::Unset;
		bool theBool{};
		theResult = jsonObjectData["value"].get(theBool);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.value.theType = ObjectType::Boolean;
			theData.value.theValue = std::to_string(theBool);
			return;
		}

		uint64_t theInteger{};
		theResult = jsonObjectData["value"].get(theInteger);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.value.theType = ObjectType::Integer;
			theData.value.theValue = std::to_string(theInteger);
			return;
		} 

		std::string_view theString{};
		theResult = jsonObjectData["value"].get(theString);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.value.theType = ObjectType::String;
			theData.value.theValue = theString;
			return;
		} 
			
		double theFloat{};
		theResult = jsonObjectData["value"].get(theFloat);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.value.theType = ObjectType::Float;
			theData.value.theValue = std::to_string(theFloat);
			return;
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ResolvedData& theData) {
		simdjson::ondemand::value theArray{};
		auto theResult = jsonObjectData["attachments"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.attachments.clear();
			for (auto theValue: theArray) {
				AttachmentData newData{};
				auto theObject = theValue.value().get_object().value();
				parseObject(theObject, newData);
				theData.attachments[strtoull(theValue.key().take_value().raw())] = std::move(newData);
			}
		}

		simdjson::ondemand::value theArray{};
		theResult = jsonObjectData["users"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.users.clear();
			for (auto theValue: theArray) {			
				UserData newData{};
				auto theObject = theValue.value().get_object().value();
				parseObject(theObject, newData);
				theData.users[strtoull(theValue.key().take_value().raw())] = std::move(newData);
			}
		}

		simdjson::ondemand::value theArray{};
		theResult = jsonObjectData["channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.channels.clear();
			for (auto theValue: theArray) {
				ChannelData newData{};
				auto theObject = theValue.value().get_object().value();
				parseObject(theObject, newData);
				theData.channels[strtoull(theValue.key().take_value().raw())] = std::move(newData);
			}
		}

		simdjson::ondemand::value theArray{};
		theResult = jsonObjectData["roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.roles.clear();
			for (auto theValue: theArray) {
				RoleData newData{};
				auto theObject = theValue.value().get_object().value();
				parseObject(theObject, newData);
				theData.roles[strtoull(theValue.key().take_value().raw())] = std::move(newData);
			}
		}

		simdjson::ondemand::value theArray{};
		theResult = jsonObjectData["members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.members.clear();
			for (auto theValue: theArray) {
				GuildMemberData newData{};
				auto theObject = theValue.value().get_object().value();
				parseObject(theObject, newData);
				theData.members[strtoull(theValue.key().take_value().raw())] = std::move(newData);
			}
		}

		simdjson::ondemand::value theArray{};
		theResult = jsonObjectData["messages"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.messages.clear();
			for (auto theValue: theArray) {
				MessageData newData{};
				auto theObject = theValue.value().get_object().value();
				parseObject(theObject, newData);
				theData.messages[strtoull(theValue.key().take_value().raw())] = std::move(newData);
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, ApplicationCommandInteractionData& theData) {
		theData.type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		theData.name = getString(jsonObjectData, "name");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["options"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.options.clear();
			ApplicationCommandInteractionDataOption newData{};
			for (auto value: theArray) {
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.options.emplace_back(std::move(newData));
			}
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["resolved"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, theData.resolved);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, InteractionDataData& theData) {
		std::string_view idNew{};
		auto theResult = jsonObjectData["id"].get(idNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, theData.applicationCommandData);
		}

		std::string_view targetIdNew{};
		theResult = jsonObjectData["target_id"].get(targetIdNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, theData.messageInteractionData);
			parseObject(jsonObjectData, theData.userInteractionData);
		}

		uint64_t componentType{};
		theResult = jsonObjectData["component_type"].get(componentType);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, theData.componentData);
		}

		simdjson::ondemand::value components{};
		theResult = jsonObjectData["components"].get(components);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, theData.modalData);
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, InteractionData& theData) {
		simdjson::ondemand::value dataNew{};
		auto theResult = jsonObjectData["data"].get(dataNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(dataNew, theData.data);
		}

		theData.appPermissions = getString(jsonObjectData, "app_permissions");

		theData.type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		theData.token = getString(jsonObjectData, "token");

		simdjson::ondemand::value memberNew{};
		theResult = jsonObjectData["member"].get(memberNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(memberNew, theData.member);
			theData.user.id = theData.member.id;
			theData.user.userName = theData.member.getUserData().userName;
		}

		simdjson::ondemand::value userNew{};
		theResult = jsonObjectData["user"].get(userNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(userNew, theData.user);
		}

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.locale = getString(jsonObjectData, "locale");

		theData.guildLocale = getString(jsonObjectData, "guild_locale");

		simdjson::ondemand::value messageNew{};
		theResult = jsonObjectData["message"].get(messageNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(messageNew, theData.message);
		}

		theData.version = getUint32(jsonObjectData, "version");

		theData.id = getId(jsonObjectData, "id");

		theData.applicationId = getId(jsonObjectData, "application_id");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildEmojisUpdateEventData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emojis") && !(*jsonObjectData)["emojis"].is_null()) {
			theData.emojis.clear();
			for (auto& value: (*jsonObjectData)["emojis"]) {
				EmojiData newData{};
				parseObject(&value, newData);
				theData.emojis.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildStickersUpdateEventData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				parseObject(&value, newData);
				theData.stickers.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildMembersChunkEventData& theData) {
		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = stoull((*jsonObjectData)["guild_id"].get<std::string>());
		}

		if (jsonObjectData->contains("nonce") && !(*jsonObjectData)["nonce"].is_null()) {
			theData.nonce = (*jsonObjectData)["nonce"].get<std::string>();
		}

		if (jsonObjectData->contains("chunk_index") && !(*jsonObjectData)["chunk_index"].is_null()) {
			theData.chunkIndex = (*jsonObjectData)["chunk_index"].get<int32_t>();
		}

		if (jsonObjectData->contains("chunk_count") && !(*jsonObjectData)["chunk_count"].is_null()) {
			theData.chunkCount = (*jsonObjectData)["chunk_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("presences") && !(*jsonObjectData)["presences"].is_null()) {
			theData.presences.clear();
			for (auto& value: (*jsonObjectData)["presences"]) {
				PresenceUpdateData newData{};
				parseObject(&value, newData);
				theData.presences.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData->contains("not_found") && !(*jsonObjectData)["not_found"].is_null()) {
			theData.notFound.clear();
			for (auto& value: (*jsonObjectData)["not_found"]) {
				theData.notFound.emplace_back(value);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				GuildMemberData theDataNew{};
				parseObject(&value, theDataNew);
				theData.members.emplace_back(theDataNew);
			}
		}
	}

	/*
	

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, Song& theData) {
		simdjson::ondemand::value theValue{};
		auto theResult= jsonObjectData["lengthText"]["accessibility"]["accessibilityData"].get(theValue);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.duration = getString(theValue, "label");
		}

		theResult = jsonObjectData["detailedMetadataSnippets"][0]["snippetText"].get(theValue);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.duration = getString(theValue, "runs");
		}

		auto theResult = jsonObjectData["thumbnail"]["thumbnails"][0].get(theValue);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.thumbnailUrl = getString(theValue, "url");
		}

		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null()) {
			if ((*jsonObjectData)["title"].contains("runs")) {
				std::string newString = (*jsonObjectData)["title"]["runs"][0]["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theData.songTitle = utf8MakeValid(newString);
			} else if ((*jsonObjectData)["title"].contains("simpleText")) {
				std::string newString = (*jsonObjectData)["title"]["simpleText"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theData.songTitle = utf8MakeValid(newString);
			}
		}
		if (jsonObjectData->contains("videoId") && !(*jsonObjectData)["videoId"].is_null()) {
			theData.songId = (*jsonObjectData)["videoId"].get<std::string>();
		}

		if (jsonObjectData->contains("track_authorization") && !(*jsonObjectData)["track_authorization"].is_null()) {
			theData.trackAuthorization = (*jsonObjectData)["track_authorization"].get<std::string>();
		}

		if (jsonObjectData->contains("media") && !(*jsonObjectData)["media"].is_null()) {
			bool isItFound{ false };
			for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
				if (value["preset"] == "opus_0_0") {
					isItFound = true;
					theData.firstDownloadUrl = value["url"].get<std::string>();
					theData.songId = value["url"].get<std::string>();
					theData.doWeGetSaved = true;
				}
			}
			bool isItFound2{ false };
			if (!isItFound) {
				for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
					if (value["preset"] == "mp3_0_0") {
						theData.firstDownloadUrl = value["url"].get<std::string>();
						theData.songId = value["url"].get<std::string>();
						isItFound2 = true;
					}
				}
			}
			if (!isItFound2 && !isItFound) {
				for (auto& value: (*jsonObjectData)["media"]["transcodings"]) {
					theData.firstDownloadUrl = value["url"].get<std::string>();
					theData.songId = value["url"].get<std::string>();
				}
			}
		}

		if (jsonObjectData->contains("title") && !(*jsonObjectData)["title"].is_null() && !(*jsonObjectData)["title"].is_object()) {
			std::string newString = (*jsonObjectData)["title"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			theData.songTitle = utf8MakeValid(newString);
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			std::string newString = (*jsonObjectData)["description"].get<std::string>();
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			theData.description = utf8MakeValid(newString);
			theData.description += "...";
		}

		if (jsonObjectData->contains("artwork_url") && !(*jsonObjectData)["artwork_url"].is_null()) {
			theData.thumbnailUrl = (*jsonObjectData)["artwork_url"].get<std::string>();
		} else if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			if ((*jsonObjectData)["user"].contains("avatar_url") && !(*jsonObjectData)["user"]["avatar_url"].is_null()) {
				theData.thumbnailUrl = (*jsonObjectData)["user"]["avatar_url"].get<std::string>();
			}
		}

		if (jsonObjectData->contains("duration") && !(*jsonObjectData)["duration"].is_null()) {
			theData.duration = TimeStamp<std::chrono::milliseconds>::convertMsToDurationString((*jsonObjectData)["duration"].get<int32_t>());
		}

		if (jsonObjectData->contains("permalink_url") && !(*jsonObjectData)["permalink_url"].is_null()) {
			theData.viewUrl = (*jsonObjectData)["permalink_url"].get<std::string>();
		}
	}
	*/
	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildScheduledEventVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theGuildScheduledEvents.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				GuildScheduledEvent newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theGuildScheduledEvents.push_back(newData);
			}
			theData.theGuildScheduledEvents.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, GuildScheduledEventDataVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData.get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			theData.theGuildScheduledEventDatas.reserve(theArray.count_elements().take_value());
			for (auto value: theArray) {
				GuildScheduledEventData newData{};
				auto theObject = value.value();
				parseObject(theObject, newData);
				theData.theGuildScheduledEventDatas.push_back(newData);
			}
			theData.theGuildScheduledEventDatas.shrink_to_fit();
		}
	}
};

namespace DiscordCoreInternal {

	JsonParseError::JsonParseError(int32_t theCode) : std::runtime_error(theErrors[theCode]){};

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, WebSocketMessage& theData) {
		try {
			theData.op = DiscordCoreAPI::getUint32(jsonObjectData, "op");

			theData.s = DiscordCoreAPI::getUint32(jsonObjectData, "s");

			theData.t = DiscordCoreAPI::getString(jsonObjectData, "t");
		} catch (...) {
			DiscordCoreAPI::reportException("parseObject()");
		}
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, HelloData& theData) {
		theData.heartbeatInterval = DiscordCoreAPI::getUint64(jsonObjectData, "heartbeat_interval");
	}

	template<> void parseObject(simdjson::ondemand::value& jsonObjectData, InvalidSessionData& theData) {
		theData.d = DiscordCoreAPI::getBool(jsonObjectData, "d");
	}
}