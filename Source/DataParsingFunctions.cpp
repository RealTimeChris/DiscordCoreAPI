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

	uint64_t getId(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, const char* theKey) {
		return strtoull(getString(jsonObjectData, theKey));
	}

	bool getBool(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonData, const char* theKey) {
		try {
			bool theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint8_t>(theValue);
		} catch (...) {
			reportException("getBool()");
			return 0;
		}
	}

	bool getBool(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonData, const char* theKey) {
		try {
			bool theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint8_t>(theValue);
		} catch (...) {
			reportException("getBool()");
			return 0;
		}
	}

	uint8_t getUint8(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint8_t>(theValue);
		} catch (...) {
			reportException("getUint8()");
			return 0;
		}
	}

	uint8_t getUint8(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint8_t>(theValue);
		} catch (...) {
			reportException("getUint8()");
			return 0;
		}
	}

	uint16_t getUint16(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint16_t>(theValue);
		} catch (...) {
			reportException("getUint16()");
			return 0;
		}
	}

	uint16_t getUint16(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint16_t>(theValue);
		} catch (...) {
			reportException("getUint16()");
			return 0;
		}
	}

	uint32_t getUint32(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint32_t>(theValue);
		} catch (...) {
			reportException("getUint32()");
			return 0;
		}
	}

	uint32_t getUint32(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return static_cast<uint32_t>(theValue);
		} catch (...) {
			reportException("getUint32()");
			return 0;
		}
	}

	uint64_t getUint64(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return theValue;
		} catch (...) {
			reportException("getUint64()");
			return 0;
		}
	}

	uint64_t getUint64(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			auto theResult = jsonData[theKey].get(theValue);
			return theValue;
		} catch (...) {
			reportException("getUint64()");
			return 0;
		}
	}

	std::string getString(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonData, const char* theKey) {
		try {
			std::string_view theStringNew{};
			auto theValue = jsonData[theKey].get(theStringNew);
			if (theValue != simdjson::error_code::SUCCESS) {
				return "0";
			}
			std::string theStringNewer{};
			theStringNewer.insert(theStringNewer.begin(), theStringNew.begin(), theStringNew.end());
			return theStringNewer;
		} catch (...) {
			reportException("getString()");
			return "";
		}
	}

	uint64_t getId(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, const char* theKey) {
		return strtoull(getString(jsonObjectData, theKey));
	}

	std::string getString(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonData, const char* theKey) {
		try {
			std::string_view theStringNew{};
			auto theValue = jsonData[theKey].get(theStringNew);
			if (theValue != simdjson::error_code::SUCCESS) {
				return "0";
			}
			std::string theStringNewer{};
			theStringNewer.insert(theStringNewer.begin(), theStringNew.begin(), theStringNew.end());
			return theStringNewer;
		} catch (...) {
			reportException("getString()");
			return "";
		}
	}

	template<typename ReturnType>
	void getArray(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, std::vector<ReturnType>& theReturnVector, const char* theArrayName) {
		theReturnVector.clear();
		simdjson::fallback::ondemand::array theArray{};
		auto theResult = jsonObjectData[theArrayName].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ReturnType newData{};
			for (auto value: theArray) {
				DiscordCoreAPI::parseObject(value.get_object(), newData);
				theReturnVector.emplace_back(newData);
			}
		}
	}

	template<typename KeyType,typename ObjectType>
	void getMap(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, std::unordered_map<KeyType, ObjectType>& theReturnVector, const char* theMapName) {
		theReturnVector.clear();
		simdjson::fallback::ondemand::object theArray{};
		auto theResult = jsonObjectData[theMapName].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ObjectType newData{};
			for (auto [key,value]: theArray) {
				DiscordCoreAPI::parseObject(value.get_object(), newData);
				theReturnVector.emplace(strtoull(key.first.raw()), newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationCommand& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

		theData.defaultMemberPermissions = getString(jsonObjectData, "default_member_permissions");

		theData.dmPermission = getBool(jsonObjectData, "dm_permission");

		theData.version = getString(jsonObjectData, "version");

		theData.type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		getMap(jsonObjectData, theData.nameLocalizations, "name_localizations");

		getMap(jsonObjectData, theData.descriptionLocalizations, "description_localizations");

		theData.applicationId = strtoull(getString(jsonObjectData, "application_id"));

		theData.name = getString(jsonObjectData, "name");

		theData.description = getString(jsonObjectData, "description");

		theData.version = getString(jsonObjectData, "version");

		getArray(jsonObjectData, theData.options, "options");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, UserData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, UserData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, UserData& theData) {
		theData.flags |= setBool(theData.flags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		theData.flags |= setBool(theData.flags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		theData.flags |= setBool(theData.flags, UserFlags::System, getBool(jsonObjectData, "system"));

		theData.flags |= setBool(theData.flags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		theData.discriminator = getString(jsonObjectData, "discriminator");

		theData.flags |= getUint32(jsonObjectData, "public_flags");

		theData.userName = getString(jsonObjectData, "username");

		theData.id = getId(jsonObjectData, "owner_id");

		theData.avatar = getString(jsonObjectData, "avatar");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, RoleData& theData) {
		
		theData.id = getId(jsonObjectData, "owner_id");
		
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, RoleData& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, RoleData& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, User& theData) {
		theData.userName = getString(jsonObjectData, "username");

		theData.id = getId(jsonObjectData, "owner_id");

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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Role& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

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

		parseObject(jsonObjectData["tags"].get_object(), theData.tags);
	}

 	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, GuildMemberData& theData) {
		
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));
		
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));
		
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));
		
		theData.joinedAt = getString(jsonObjectData, "joined_at");
		
		theData.guildId = getId(jsonObjectData, "guild_id");
		theData.roles.clear();
		simdjson::fallback::ondemand::array theArray{};
		auto theValue = jsonObjectData["roles"].get(theArray);
		if (theValue == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				std::string theString{ value.get_string().take_value().data() };
				theData.roles.emplace_back(stoull(theString));
			}
		}
		
		theData.permissions = getString(jsonObjectData, "permissions");
		
		UserData theUser{};
		DiscordCoreAPI::parseObject(jsonObjectData["user"].get_object(), theUser);
		theData.id = theUser.id;
		Users::insertUser(std::move(theUser));
		
		theData.avatar = getString(jsonObjectData, "avatar");
		
		theData.flags |= getUint8(jsonObjectData, "flags");
		
		theData.nick = getString(jsonObjectData, "nick");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildMemberData& theData) {
		
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));
		
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));
		
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));
		
		theData.joinedAt = getString(jsonObjectData, "joined_at");
		
		theData.guildId = getId(jsonObjectData, "guild_id");
		theData.roles.clear();
		simdjson::fallback::ondemand::array theArray{};
		auto theValue = jsonObjectData["roles"].get(theArray);
		if (theValue == simdjson::error_code::SUCCESS) {
			for (auto value: theArray) {
				std::string theString{ value.get_string().take_value().data() };
				theData.roles.emplace_back(stoull(theString));
			}
		}
		
		theData.permissions = getString(jsonObjectData, "permissions");
		
		UserData theUser{};
		DiscordCoreAPI::parseObject(jsonObjectData["user"].get_object(), theUser);
		theData.id = theUser.id;
		Users::insertUser(std::move(theUser));

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.nick = getString(jsonObjectData, "nick");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildMember& theData) {
		theData.flags |= setBool(theData.flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		theData.flags |= setBool(theData.flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		theData.joinedAt = getString(jsonObjectData, "joined_at");

		theData.guildId = getId(jsonObjectData, "guild_id");

		getArray(jsonObjectData, theData.roles, "roles");

		theData.permissions = getString(jsonObjectData, "permissions");

		UserData theUser{};
		DiscordCoreAPI::parseObject(jsonObjectData["user"].get_object(), theUser);
		theData.id = theUser.id;
		Users::insertUser(std::move(theUser));

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.nick = getString(jsonObjectData, "nick");

		theData.communicationDisabledUntil = getString(jsonObjectData, "communication_disabled_until");

		theData.premiumSince = getString(jsonObjectData, "premium_since");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, OverWriteData& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

		theData.allow = getId(jsonObjectData, "allow");

		theData.deny = getId(jsonObjectData, "deny");

		theData.type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, OverWriteData& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

		theData.allow = getId(jsonObjectData, "allow");

		theData.deny = getId(jsonObjectData, "deny");

		theData.type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, ChannelData& theData) {
		theData.flags |= setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));
		
		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.ownerId = getId(jsonObjectData, "owner_id");

		theData.id = getId(jsonObjectData, "owner_id");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.parentId = getId(jsonObjectData, "parent_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.position = getUint32(jsonObjectData, "position");

		getArray(jsonObjectData, theData.permissionOverwrites, "permission_overwrites");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ChannelData& theData) {
		theData.flags |= setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.ownerId = getId(jsonObjectData, "owner_id");

		theData.id = getId(jsonObjectData, "owner_id");
		
		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.parentId = getId(jsonObjectData, "parent_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.position = getUint32(jsonObjectData, "position");

		getArray(jsonObjectData, theData.permissionOverwrites, "permission_overwrites");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ChannelData& theData) {
		theData.flags |= setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.ownerId = getId(jsonObjectData, "owner_id");

		theData.id = getId(jsonObjectData, "owner_id");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.parentId = getId(jsonObjectData, "parent_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.position = getUint32(jsonObjectData, "position");

		getArray(jsonObjectData, theData.permissionOverwrites, "permission_overwrites");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, GuildData& theData) {
		
		theData.flags |= setBool(theData.flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));
		
		theData.flags |= setBool(theData.flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));
		
		theData.flags |= setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));
		
		theData.flags |= setBool(theData.flags, GuildFlags::Large, getUint8(jsonObjectData, "large"));
		
		theData.ownerId = getId(jsonObjectData, "owner_id");
		
		theData.memberCount = getUint32(jsonObjectData, "member_count");
		
		theData.joinedAt = getString(jsonObjectData, "joined_at");
		
		theData.id = getId(jsonObjectData, "owner_id");
		
		theData.icon = getString(jsonObjectData, "icon");
		
		theData.name = getString(jsonObjectData, "name");
		
		theData.threads.clear();
		simdjson::ondemand::array theArray01{};
		auto theValue = jsonObjectData["threads"].get(theArray01);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray01) {
				theData.threads.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}
		
		simdjson::ondemand::array theArray02{};
		theValue = jsonObjectData["stickers"].get(theArray02);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray02) {
				theData.stickers.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}
		
		simdjson::ondemand::array theArray03{};
		theValue = jsonObjectData["guild_scheduled_events"].get(theArray03);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray03) {
				theData.guildScheduledEvents.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray04{};
		theValue = jsonObjectData["stage_instances"].get(theArray04);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray04) {
				theData.stageInstances.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray05{};
		theValue = jsonObjectData["emoji"].get(theArray05);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray05) {
				theData.emoji.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray06{};
		theValue = jsonObjectData["roles"].get(theArray06);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (Roles::doWeCacheRoles) {
				theData.roles.clear();
				RoleData newData{};
				for (auto iterator: theArray06) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.roles.emplace_back(newData.id);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray07{};
		theValue = jsonObjectData["members"].get(theArray07);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				theData.members.clear();
				GuildMemberData newData{};
				for (auto iterator: theArray07) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.members.emplace_back(newData.id);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray08{};
		theValue = jsonObjectData["voice_states"].get(theArray08);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				for (auto iterator: theArray08) {
					auto userId = strtoull(getString(iterator.get_object(), "user_id"));
					if (GuildMembers::cache.contains(GuildMemberKey{ theData.id, userId })) {
						GuildMembers::cache.at(GuildMemberKey{ theData.id, userId }).voiceChannelId = strtoull(getString(iterator.get_object(), "channel_id"));
					}
				}
			}
		}

		simdjson::ondemand::array theArray09{};
		theValue = jsonObjectData["presences"].get(theArray09);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				theData.presences.clear();
				PresenceUpdateData newData{};
				for (auto iterator: theArray09) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.presences.emplace_back(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray10{};
		theValue = jsonObjectData["channels"].get(theArray10);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (Channels::doWeCacheChannels) {
				theData.channels.clear();
				ChannelData newData{};
				for (auto iterator: theArray10) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.channels.emplace_back(newData.id);
					Channels::insertChannel(std::move(newData));
				}
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData,  GuildData& theData) {

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

		theData.threads.clear();
		simdjson::ondemand::array theArray01{};
		auto theValue = jsonObjectData["threads"].get(theArray01);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray01) {
				theData.threads.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray02{};
		theValue = jsonObjectData["stickers"].get(theArray02);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray02) {
				theData.stickers.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray03{};
		theValue = jsonObjectData["guild_scheduled_events"].get(theArray03);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray03) {
				theData.guildScheduledEvents.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray04{};
		theValue = jsonObjectData["stage_instances"].get(theArray04);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray04) {
				theData.stageInstances.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray05{};
		theValue = jsonObjectData["emoji"].get(theArray05);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			for (auto value: theArray05) {
				theData.emoji.emplace_back(strtoull(getString(value.get_object(), "id")));
			}
		}

		simdjson::ondemand::array theArray06{};
		theValue = jsonObjectData["roles"].get(theArray06);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (Roles::doWeCacheRoles) {
				theData.roles.clear();
				RoleData newData{};
				for (auto iterator: theArray06) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.roles.emplace_back(newData.id);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray07{};
		theValue = jsonObjectData["members"].get(theArray07);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				theData.members.clear();
				GuildMemberData newData{};
				for (auto iterator: theArray07) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.members.emplace_back(newData.id);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray08{};
		theValue = jsonObjectData["voice_states"].get(theArray08);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				for (auto iterator: theArray08) {
					auto userId = strtoull(getString(iterator.get_object(), "user_id"));
					if (GuildMembers::cache.contains(GuildMemberKey{ theData.id, userId })) {
						GuildMembers::cache.at(GuildMemberKey{ theData.id, userId }).voiceChannelId = strtoull(getString(iterator.get_object(), "channel_id"));
					}
				}
			}
		}

		simdjson::ondemand::array theArray09{};
		theValue = jsonObjectData["presences"].get(theArray09);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				theData.presences.clear();
				PresenceUpdateData newData{};
				for (auto iterator: theArray09) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.presences.emplace_back(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray10{};
		theValue = jsonObjectData["channels"].get(theArray10);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (Channels::doWeCacheChannels) {
				theData.channels.clear();
				ChannelData newData{};
				for (auto iterator: theArray10) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.channels.emplace_back(newData.id);
					Channels::insertChannel(std::move(newData));
				}
			}
		}
	}
		
	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Guild& theData) {
		theData.id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray01{};
		auto theValue = jsonObjectData["threads"].get(theArray01);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.threads.clear();
			Channel newData{};
			for (auto iterator: theArray01) {
				DiscordCoreAPI::parseObject(iterator.get_object(), newData);
				theData.threads.emplace_back(newData);
			}
		}

		simdjson::ondemand::array theArray02{};
		theValue = jsonObjectData["stickers"].get(theArray02);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.stickers.clear();
			Sticker newData{};
			for (auto iterator: theArray02) {
				DiscordCoreAPI::parseObject(iterator.get_object(), newData);
				theData.stickers.emplace_back(newData);
			}
		}

		simdjson::ondemand::array theArray03{};
		theValue = jsonObjectData["guild_scheduled_events"].get(theArray03);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.guildScheduledEvents.clear();
			GuildScheduledEvent newData{};
			for (auto iterator: theArray03) {
				DiscordCoreAPI::parseObject(iterator.get_object(), newData);
				theData.guildScheduledEvents.emplace_back(newData);
			}
		}

		simdjson::ondemand::array theArray04{};
		theValue = jsonObjectData["stage_instances"].get(theArray04);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.stageInstances.clear();
			StageInstance newData{};
			for (auto iterator: theArray04) {
				DiscordCoreAPI::parseObject(iterator.get_object(), newData);
				theData.stageInstances.emplace_back(newData);
			}
		}

		simdjson::ondemand::array theArray05{};
		theValue = jsonObjectData["emojis"].get(theArray05);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.emoji.clear();
			EmojiData newData{};
			for (auto iterator: theArray05) {
				DiscordCoreAPI::parseObject(iterator.get_object(), newData);
				theData.emoji.emplace_back(newData);
			}
		}

		simdjson::ondemand::array theArray06{};
		theValue = jsonObjectData["roles"].get(theArray06);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (Roles::doWeCacheRoles) {
				theData.roles.clear();
				RoleData newData{};
				for (auto iterator: theArray06) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.roles.emplace_back(newData);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray07{};
		theValue = jsonObjectData["members"].get(theArray07);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				theData.members.clear();
				GuildMemberData newData{};
				for (auto iterator: theArray07) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.members.emplace_back(newData);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray08{};
		theValue = jsonObjectData["voice_states"].get(theArray08);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				for (auto iterator: theArray08) {
					auto userId = strtoull(getString(iterator.get_object(), "user_id"));
					if (GuildMembers::cache.contains(GuildMemberKey{ theData.id, userId })) {
						GuildMembers::cache.at(GuildMemberKey{ theData.id, userId }).voiceChannelId = strtoull(getString(iterator.get_object(), "channel_id"));
					}
				}
			}
		}

		simdjson::ondemand::array theArray09{};
		theValue = jsonObjectData["presences"].get(theArray09);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (GuildMembers::doWeCacheGuildMembers) {
				theData.presences.clear();
				PresenceUpdateData newData{};
				for (auto iterator: theArray09) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
					newData.guildId = theData.id;
					theData.presences[newData.userId] = std::move(newData);
				}
			}
		}

		simdjson::ondemand::array theArray10{};
		theValue = jsonObjectData["channels"].get(theArray10);
		if (theValue == simdjson::error_code::SUCCESS) {
			if (Channels::doWeCacheChannels) {
				theData.channels.clear();
				ChannelData newData{};
				for (auto iterator: theArray10) {
					DiscordCoreAPI::parseObject(iterator.get_object(), newData);
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

		theData.flags |= setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

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

		theData.region = getString(jsonObjectData, "region");

		getArray(jsonObjectData, theData.features, "features");

		theData.permissions = getString(jsonObjectData, "permissions");

		theData.afkTimeOut = static_cast<AfkTimeOutDurations>(getUint8(jsonObjectData, "afk_timeout"));

		theData.flags |= setBool(theData.flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		theData.flags |= setBool(theData.flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));

		theData.verificationLevel = static_cast<VerificationLevel>(getUint8(jsonObjectData, "verification_level"));

		theData.defaultMessageNotifications = static_cast<DefaultMessageNotificationLevel>(getUint8(jsonObjectData, "default_message_notification_level"));

		theData.explicitContentFilter = static_cast<ExplicitContentFilterLevel>(getUint8(jsonObjectData, "explicit_content_filter_level"));

		theData.mfaLevel = static_cast<MFALevel>(getUint8(jsonObjectData, "mfa_level"));

		theData.systemChannelFlags = static_cast<SystemChannelFlags>(getUint8(jsonObjectData, "system_channel_flags"));

		theData.flags |= setBool(theData.flags, GuildFlags::Large, getBool(jsonObjectData, "large"));

		theData.flags |= setBool(theData.flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		theData.maxPresences = getUint32(jsonObjectData, "max_presences");

		theData.maxMembers = getUint32(jsonObjectData, "max_members");

		theData.premiumSubscriptionCount = getUint32(jsonObjectData, "premium_subscription_count");

		theData.premiumTier = static_cast<PremiumTier>(getUint8(jsonObjectData, "premium_tier"));

		theData.maxVideoChannelUsers = getUint32(jsonObjectData, "max_video_channel_users");

		theData.approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		theData.approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		DiscordCoreAPI::parseObject(jsonObjectData["welcome_screen"].get_object(), theData.welcomeScreen);

		theData.nsfwLevel = static_cast<GuildNSFWLevel>(getUint8(jsonObjectData, "nsfw_level"));
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildScheduledEventMetadata& theData) {
		theData.location = getString(jsonObjectData, "location");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AttachmentData& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationCommandVector& theData) {
		for (auto value: jsonObjectData) {
			ApplicationCommand newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theApplicationCommands.emplace_back(newData);
		}
		theData.theApplicationCommands.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AutoModerationActionExecutionEventData& theData) {
		theData.alertSystemMessageId = getId(jsonObjectData, "alert_system_message_id");

		theData.ruleTriggerType = static_cast<TriggerType>(getUint8(jsonObjectData, "rule_trigger_type"));

		theData.matchedKeyword = getString(jsonObjectData, "matched_keyword");

		theData.matchedContent = getString(jsonObjectData, "matched_content");

		DiscordCoreAPI::parseObject(jsonObjectData["action"].get_object(), theData.action);

		theData.content = getString(jsonObjectData, "content");

		theData.messageId = getId(jsonObjectData, "message_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.ruleId = getId(jsonObjectData, "rule_id");

		theData.userId = getId(jsonObjectData, "user_id");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AutoModerationRule& theData) {
		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");

		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.triggerType = static_cast<TriggerType>(getUint8(jsonObjectData, "trigger_type"));

		theData.eventType = static_cast<EventType>(getUint8(jsonObjectData, "event_type"));

		theData.creatorId = getId(jsonObjectData, "creator_id");

		getArray(jsonObjectData, theData.actions, "actions");

		getArray(jsonObjectData, theData.exemptRoles, "exempt_roles");

		DiscordCoreAPI::parseObject(jsonObjectData["trigger_metadata"].get_object(), theData.triggerMetaData);

		getArray(jsonObjectData, theData.exemptChannels, "exempt_channels");

		theData.guildId = getId(jsonObjectData, "guild_id");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AutoModerationRuleVector& theData) {
		for (auto value: jsonObjectData) {
			AutoModerationRule newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theAutoModerationRules.emplace_back(newData);
		}
		theData.theAutoModerationRules.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Channel& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.flags |= getUint8(jsonObjectData, "flags");

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.defaultAutoArchiveDuration = getUint32(jsonObjectData, "default_auto_archive_duration");

		theData.position = getUint32(jsonObjectData, "position");

		getArray(jsonObjectData, theData.permissionOverwrites, "permission_overwrites");

		theData.name = getString(jsonObjectData, "name");

		theData.topic = getString(jsonObjectData, "topic");

		theData.permissions = getString(jsonObjectData, "permissions");

		theData.flags |= setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		theData.lastMessageId = getId(jsonObjectData, "last_message_id");

		theData.bitrate = getUint32(jsonObjectData, "bitrate");

		theData.userLimit = getUint32(jsonObjectData, "user_limit");

		theData.rateLimitPerUser = getUint32(jsonObjectData, "rate_limit_per_user");

		simdjson::ondemand::array theArray09{};
		auto theValue = jsonObjectData["recipients"].get(theArray09);
		if (theValue == simdjson::error_code::SUCCESS) {
			theData.recipients.clear();
			User newData{};
			for (auto iterator: theArray09) {
				DiscordCoreAPI::parseObject(iterator.get_object(), newData);
				theData.recipients[newData.id] = std::move(newData);
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

		DiscordCoreAPI::parseObject(jsonObjectData["thread_metadata"].get_object(), theData.threadMetadata);

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ChannelVector& theData) {
		for (auto value: jsonObjectData) {
			Channel newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theChannels.emplace_back(newData);
		}
		theData.theChannels.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildVector& theData) {
		for (auto value: jsonObjectData) {
			Guild newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuilds.emplace_back(newData);
		}
		theData.theGuilds.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildMemberVector& theData) {
		
		for (auto value: jsonObjectData) {
			GuildMember newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuildMembers.emplace_back(newData);
		}
		theData.theGuildMembers.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildScheduledEvent& theData) {
		theData.privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		theData.status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		parseObject(jsonObjectData["entity_metadata"].get_object(), theData.entityMetadata);

		theData.scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		theData.scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		theData.userCount = getUint32(jsonObjectData, "user_count");

		theData.description = getString(jsonObjectData, "description");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.creatorId = getString(jsonObjectData, "creator_id");

		theData.entityId = getString(jsonObjectData, "entity_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		DiscordCoreAPI::parseObject(jsonObjectData["creator"].get_object(), theData.creator);

		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Message& theData) {
		theData.content = getString(jsonObjectData, "content");

		theData.id = getId(jsonObjectData, "id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		DiscordCoreAPI::parseObject(jsonObjectData["author"].get_object(), theData.author);

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);

		theData.timestamp = getString(jsonObjectData, "timestamp");

		theData.editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		theData.tts = getBool(jsonObjectData, "tts");

		theData.mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		getArray(jsonObjectData, theData.mentions, "mentions");

		getArray(jsonObjectData, theData.mentionRoles, "mention_roles");

		getArray(jsonObjectData, theData.mentionChannels, "mention_channels");

		getArray(jsonObjectData, theData.attachments, "attachments");

		getArray(jsonObjectData, theData.embeds, "embeds");

		getArray(jsonObjectData, theData.reactions, "reactions");

		theData.nonce = getString(jsonObjectData, "nonce");

		theData.pinned = getBool(jsonObjectData, "pinned");

		theData.webHookId = getId(jsonObjectData, "webhook_id");

		theData.type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		DiscordCoreAPI::parseObject(jsonObjectData["activity"].get_object(), theData.activity);

		DiscordCoreAPI::parseObject(jsonObjectData["application"].get_object(), theData.application);

		theData.applicationId = getId(jsonObjectData, "application_id");

		DiscordCoreAPI::parseObject(jsonObjectData["message_reference"].get_object(), theData.messageReference);

		theData.flags = getUint32(jsonObjectData, "flags");

		getArray(jsonObjectData, theData.stickerItems, "sticker_items");

		getArray(jsonObjectData, theData.stickers, "stickers");

		DiscordCoreAPI::parseObject(jsonObjectData["interaction"].get_object(), theData.interaction);

		getArray(jsonObjectData, theData.components, "components");

		DiscordCoreAPI::parseObject(jsonObjectData["thread"].get_object(), theData.thread);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, MessageVector& theData) {
		for (auto value: jsonObjectData) {
			Message newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theMessages.emplace_back(newData);
		}
		theData.theMessages.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Reaction& theData) {
		theData.count = getUint32(jsonObjectData, "count");

		theData.me = getBool(jsonObjectData, "me");

		DiscordCoreAPI::parseObject(jsonObjectData["emoji"].get_object(), theData.emoji);

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.userId = getId(jsonObjectData, "user_id");

		theData.messageId = getId(jsonObjectData, "message_id");

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ReactionVector& theData) {
		for (auto value: jsonObjectData) {
			Reaction newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theReactions.emplace_back(newData);
		}
		theData.theReactions.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, RoleVector& theData) {
		for (auto value: jsonObjectData) {
			Role newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theRoles.emplace_back(newData);
		}
		theData.theRoles.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, StageInstance& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.topic = getString(jsonObjectData, "topic");

		theData.privacyLevel = static_cast<StageInstancePrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Sticker& theData) {
		theData.asset = getString(jsonObjectData, "asset");

		theData.description = getString(jsonObjectData, "description");

		theData.formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		theData.stickerFlags |= setBool(theData.stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.packId = getString(jsonObjectData, "pack_id");

		theData.type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		theData.sortValue = getUint32(jsonObjectData, "sort_value");

		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");

		DiscordCoreAPI::parseObject(jsonObjectData["user"].get_object(), theData.user);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, StickerVector& theData) {
		for (auto value: jsonObjectData) {
			Sticker newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theStickers.emplace_back(newData);
		}
		theData.theStickers.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Thread& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.position = getUint32(jsonObjectData, "position");

		getArray(jsonObjectData, theData.permissionOverwrites, "permission_overwrites");

		theData.name = getString(jsonObjectData, "name");

		theData.topic = getString(jsonObjectData, "topic");

		theData.flags |= setBool(theData.flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		theData.lastMessageId = getId(jsonObjectData, "last_message_id");

		theData.bitrate = getUint32(jsonObjectData, "bitrate");

		theData.userLimit = getUint32(jsonObjectData, "user_limit");

		theData.rateLimitPerUser = getUint32(jsonObjectData, "rate_limit_per_user");

		getMap(jsonObjectData, theData.recipients, "recipients");

		theData.icon = getString(jsonObjectData, "icon");

		theData.ownerId = getId(jsonObjectData, "owner_id");

		theData.applicationId = getId(jsonObjectData, "application_id");

		theData.parentId = getId(jsonObjectData, "parent_id");

		theData.lastPinTimestamp = getString(jsonObjectData, "last_pin_timestamp");

		theData.rtcRegion = getString(jsonObjectData, "rtc_region");

		theData.videoQualityMode = getUint32(jsonObjectData, "video_quality_mode");

		theData.messageCount = getUint32(jsonObjectData, "message_count");

		theData.memberCount = getUint32(jsonObjectData, "member_count");

		DiscordCoreAPI::parseObject(jsonObjectData["thread_metadata"].get_object(), theData.threadMetadata);

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, UserVector& theData) {
		for (auto value: jsonObjectData) {
			User newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theUsers.emplace_back(newData);
		}
		theData.theUsers.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, WebHook& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.type = static_cast<WebHookType>(getUint8(jsonObjectData, "type"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		DiscordCoreAPI::parseObject(jsonObjectData["user"].get_object(), theData.user);

		theData.name = getString(jsonObjectData, "name");

		theData.avatar = getString(jsonObjectData, "avatar");

		theData.token = getString(jsonObjectData, "token");

		theData.applicationId = getId(jsonObjectData, "application_id");

		DiscordCoreAPI::parseObject(jsonObjectData["source_guild"].get_object(), theData.sourceGuild);

		DiscordCoreAPI::parseObject(jsonObjectData["source_channel"].get_object(), theData.sourceChannel);

		theData.url = getString(jsonObjectData, "url");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, WebHookVector& theData) {
		for (auto value: jsonObjectData) {
			WebHook newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theWebHooks.emplace_back(newData);
		}
		theData.theWebHooks.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, RoleTagsData& theData) {
		theData.botId = getString(jsonObjectData, "bot_id");

		theData.integrationId = getString(jsonObjectData, "integration_id");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedFooterData& theData) {
		theData.text = getString(jsonObjectData, "text");

		theData.iconUrl = getString(jsonObjectData, "icon_url");

		theData.proxyIconUrl = getString(jsonObjectData, "proxy_icon_url");
	}
	/*
	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedImageData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedThumbnailData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedVideoData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedProviderData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedAuthorData& theData) {
		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}

		if (jsonObjectData->contains("proxy_icon_url") && !(*jsonObjectData)["proxy_icon_url"].is_null()) {
			theData.proxyIconUrl = (*jsonObjectData)["proxy_icon_url"].get<std::string>();
		}

		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("icon_url") && !(*jsonObjectData)["icon_url"].is_null()) {
			theData.iconUrl = (*jsonObjectData)["icon_url"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedFieldData& theData) {
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

		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			theData.value = (*jsonObjectData)["value"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmbedData& theData) {
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
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["footer"], theData.footer);
		}

		if (jsonObjectData->contains("image") && !(*jsonObjectData)["image"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["image"], theData.image);
		}

		if (jsonObjectData->contains("provider") && !(*jsonObjectData)["provider"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["provider"], theData.provider);
		}

		if (jsonObjectData->contains("thumbnail") && !(*jsonObjectData)["thumbnail"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["thumbnail"], theData.thumbnail);
		}

		if (jsonObjectData->contains("video") && !(*jsonObjectData)["video"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["video"], theData.video);
		}

		DiscordCoreAPI::parseObject(jsonObjectData["author"].get_object(), theData.author);

		if (jsonObjectData->contains("fields") && !(*jsonObjectData)["fields"].is_null()) {
			theData.fields.clear();
			for (auto& value: (*jsonObjectData)["fields"]) {
				EmbedFieldData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.fields.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, MessageReferenceData& theData) {
		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.failIfNotExists = getBool(jsonObjectData, "fail_if_not_exists");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ThreadMetadataData& theData) {
		theData.archived = getBool(jsonObjectData, "archived");

		theData.invitable = getBool(jsonObjectData, "invitable");

		if (jsonObjectData->contains("auto_archive_duration") && !(*jsonObjectData)["auto_archive_duration"].is_null()) {
			theData.autoArchiveDuration = (*jsonObjectData)["auto_archive_duration"].get<int32_t>();
		}

		if (jsonObjectData->contains("archive_timestamp") && !(*jsonObjectData)["archive_timestamp"].is_null()) {
			theData.archiveTimestamp = (*jsonObjectData)["archive_timestamp"].get<std::string>();
		}

		theData.locked = getBool(jsonObjectData, "locked");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ThreadMemberData& theData) {
		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("join_timestamp") && !(*jsonObjectData)["join_timestamp"].is_null()) {
			theData.joinTimestamp = (*jsonObjectData)["join_timestamp"].get<std::string>();
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theData.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ThreadMemberDataVector& theData) {
		theData.theThreadMemberDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			ThreadMemberData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theThreadMemberDatas.emplace_back(newData);
		}
		theData.theThreadMemberDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, VoiceStateData& theData) {
		theData.requestToSpeakTimestamp = getString(jsonObjectData, "request_to_speak_timestamp");

		theData.channelId = strtoull(getString(jsonObjectData, "channel_id"));

		theData.guildId = getId(jsonObjectData, "guild_id");

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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, VoiceStateData& theData) {
		theData.requestToSpeakTimestamp = getString(jsonObjectData, "request_to_speak_timestamp");

		theData.channelId = strtoull(getString(jsonObjectData, "channel_id"));

		theData.guildId = getId(jsonObjectData, "guild_id");

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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, VoiceStateData& theData) {
		theData.requestToSpeakTimestamp = getString(jsonObjectData, "request_to_speak_timestamp");

		theData.channelId = strtoull(getString(jsonObjectData, "channel_id"));

		theData.guildId = getId(jsonObjectData, "guild_id");

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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ActiveThreadsData& theData) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				theData.members.emplace_back(newData);
			}
		}

		theData.hasMore = getBool(jsonObjectData, "has_more");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ArchivedThreadsData& theData) {
		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.threads.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.members.emplace_back(newData);
			}
		}

		theData.hasMore = getBool(jsonObjectData, "has_more");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ActionMetaData& theData) {
		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("duration_seconds") && !(*jsonObjectData)["duration_seconds"].is_null()) {
			theData.durationSeconds = (*jsonObjectData)["duration_seconds"].get<int64_t>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, TriggerMetaData& theData) {
		if (jsonObjectData->contains("keyword_filter") && !(*jsonObjectData)["keyword_filter"].is_null()) {
			for (auto& value: (*jsonObjectData)["keyword_filter"]) {
				theData.keywordFilter.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("presets") && !(*jsonObjectData)["presets"].is_null()) {
			for (auto& value: (*jsonObjectData)["presets"]) {
				theData.presets.emplace_back(static_cast<KeywordPresetType>(value.get<uint64_t>()));
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ActionData& theData) {
		if (jsonObjectData->contains("metadata") && !(*jsonObjectData)["metadata"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["metadata"], theData.metadata);
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = static_cast<ActionType>((*jsonObjectData)["type"].get<int64_t>());
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AutoModerationRuleData& theData) {
		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");

		theData.enabled = getBool(jsonObjectData, "enabled");

		if (jsonObjectData->contains("trigger_type") && !(*jsonObjectData)["trigger_type"].is_null()) {
			theData.triggerType = static_cast<TriggerType>((*jsonObjectData)["trigger_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("event_type") && !(*jsonObjectData)["event_type"].is_null()) {
			theData.eventType = static_cast<EventType>((*jsonObjectData)["event_type"].get<uint64_t>());
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theData.creatorId = (*jsonObjectData)["creator_id"].get<Snowflake>();
		}

		if (jsonObjectData->contains("actions") && !(*jsonObjectData)["actions"].is_null()) {
			for (auto& value: (*jsonObjectData)["actions"]) {
				ActionData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.actions.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("exempt_roles") && !(*jsonObjectData)["exempt_roles"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_roles"]) {
				theData.exemptRoles.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData->contains("trigger_metadata") && !(*jsonObjectData)["trigger_metadata"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["trigger_metadata"], theData.triggerMetaData);
		}

		if (jsonObjectData->contains("exempt_channels") && !(*jsonObjectData)["exempt_channels"].is_null()) {
			for (auto& value: (*jsonObjectData)["exempt_channels"]) {
				theData.exemptChannels.emplace_back(value.get<Snowflake>());
			}
		}

		if (jsonObjectData->contains("guild_id") && !(*jsonObjectData)["guild_id"].is_null()) {
			theData.guildId = (*jsonObjectData)["guild_id"].get<Snowflake>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationCommandPermissionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.permission = getBool(jsonObjectData, "permission");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<ApplicationCommandPermissionType>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildApplicationCommandPermissionsData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theData.permissions.clear();
			for (auto& value: (*jsonObjectData)["permissions"]) {
				ApplicationCommandPermissionData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.permissions.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildApplicationCommandPermissionsDataVector& theData) {
		theData.theGuildApplicationCommandPermissionsDatas.clear();
		for (auto value: jsonObjectData) {
			GuildApplicationCommandPermissionsData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuildApplicationCommandPermissionsDatas.emplace_back(newData);
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmojiData& theData) {
		theData.id = getId(jsonObjectData, "owner_id");

		theData.name = getString(jsonObjectData, "name");

		for (auto& value: (*jsonObjectData)["roles"]) {
			RoleData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.roles.emplace_back(newData);
		}

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);

		theData.requireColons = getBool(jsonObjectData, "require_colons");

		theData.managed = getBool(jsonObjectData, "managed");

		theData.animated = getBool(jsonObjectData, "animated");

		theData.available = getBool(jsonObjectData, "available");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, EmojiDataVector& theData) {
		theData.theEmojiDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			EmojiData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theEmojiDatas.emplace_back(newData);
		}
		theData.theEmojiDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ReactionData& theData) {
		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			theData.count = (*jsonObjectData)["count"].get<int32_t>();
		}

		theData.me = getBool(jsonObjectData, "me");

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["emoji"], theData.emoji);
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, VoiceRegionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.custom = getBool(jsonObjectData, "custom");

		theData.deprecated = getBool(jsonObjectData, "deprecated");

		theData.optimal = getBool(jsonObjectData, "optimal");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, VoiceRegionDataVector& theData) {
		theData.theVoiceRegionDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			VoiceRegionData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theVoiceRegionDatas.emplace_back(newData);
		}
		theData.theVoiceRegionDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, MessageActivityData& theData) {
		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<MessageActivityType>();
		}

		if (jsonObjectData->contains("party_id") && !(*jsonObjectData)["party_id"].is_null()) {
			theData.partyId = (*jsonObjectData)["party_id"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, BanData& theData) {
		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("reason") && !(*jsonObjectData)["reason"].is_null()) {
			theData.reason = (*jsonObjectData)["reason"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, BanDataVector& theData) {
		theData.theBanDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			BanData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theBanDatas.emplace_back(newData);
		}
		theData.theBanDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, TeamMembersObjectData& theData) {
		if (jsonObjectData->contains("membership_state") && !(*jsonObjectData)["membership_state"].is_null()) {
			theData.membershipState = (*jsonObjectData)["membership_state"].get<int32_t>();
		}

		if (jsonObjectData->contains("team_id") && !(*jsonObjectData)["team_id"].is_null()) {
			theData.teamId = (*jsonObjectData)["team_id"].get<std::string>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theData.permissions.clear();
			for (auto& value: (*jsonObjectData)["permissions"]) {
				theData.permissions.emplace_back(value.get<std::string>());
			}
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, TeamObjectData& theData) {
		theData.icon = getString(jsonObjectData, "icon");

		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("members") && !(*jsonObjectData)["members"].is_null()) {
			theData.members.clear();
			for (auto& value: (*jsonObjectData)["members"]) {
				TeamMembersObjectData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("owner_user_id") && !(*jsonObjectData)["owner_user_id"].is_null()) {
			theData.ownerUserId = (*jsonObjectData)["owner_user_id"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, InstallParamsData& theData) {
		if (jsonObjectData->contains("scopes") && !(*jsonObjectData)["scopes"].is_null()) {
			theData.scopes = (*jsonObjectData)["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("permissions") && !(*jsonObjectData)["permissions"].is_null()) {
			theData.permissions = (*jsonObjectData)["name"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationData& theData) {
		if (jsonObjectData->contains("params") && !(*jsonObjectData)["params"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["params"], theData.params);
		}

		if (jsonObjectData->contains("tags") && !(*jsonObjectData)["tags"].is_null()) {
			theData.tags = (*jsonObjectData)["tags"].get<std::vector<std::string>>();
		}

		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		theData.icon = getString(jsonObjectData, "icon");

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("rpc_origins") && !(*jsonObjectData)["rpc_origins"].is_null()) {
			theData.rpcOrigins.clear();
			for (auto& value: (*jsonObjectData)["rpc_origins"]) {
				theData.rpcOrigins.emplace_back(value.get<std::string>());
			}
		}

		theData.botPublic = getBool(jsonObjectData, "bot_public");

		theData.botRequireCodeGrant = getBool(jsonObjectData, "bot_require_code_grant");

		if (jsonObjectData->contains("terms_of_service_url") && !(*jsonObjectData)["terms_of_service_url"].is_null()) {
			theData.termsOfServiceUrl = (*jsonObjectData)["terms_of_service_url"].get<std::string>();
		}

		if (jsonObjectData->contains("privacy_policy_url") && !(*jsonObjectData)["privacy_policy_url"].is_null()) {
			theData.privacyPolicyUrl = (*jsonObjectData)["privacy_policy_url"].get<std::string>();
		}

		if (jsonObjectData->contains("owner") && !(*jsonObjectData)["owner"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["owner"], theData.owner);
		}

		if (jsonObjectData->contains("summary") && !(*jsonObjectData)["summary"].is_null()) {
			theData.summary = (*jsonObjectData)["summary"].get<std::string>();
		}

		if (jsonObjectData->contains("verify_key") && !(*jsonObjectData)["verify_key"].is_null()) {
			theData.verifyKey = (*jsonObjectData)["verify_key"].get<std::string>();
		}

		if (jsonObjectData->contains("team") && !(*jsonObjectData)["team"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["team"], theData.team);
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("primary_sku_id") && !(*jsonObjectData)["primary_sku_id"].is_null()) {
			theData.primarySkuId = (*jsonObjectData)["primary_sku_id"].get<std::string>();
		}

		if (jsonObjectData->contains("slug") && !(*jsonObjectData)["slug"].is_null()) {
			theData.slug = (*jsonObjectData)["slug"].get<std::string>();
		}

		if (jsonObjectData->contains("cover_image") && !(*jsonObjectData)["cover_image"].is_null()) {
			theData.coverImage = (*jsonObjectData)["cover_image"].get<std::string>();
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theData.flags = (*jsonObjectData)["flags"].get<ApplicationFlags>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AuthorizationInfoData& theData) {
		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], theData.application);
		}

		if (jsonObjectData->contains("scopes") && !(*jsonObjectData)["scopes"].is_null()) {
			theData.scopes = (*jsonObjectData)["scopes"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("expires") && !(*jsonObjectData)["expires"].is_null()) {
			theData.expires = (*jsonObjectData)["expires"];
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}
	};

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AccountData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildWidgetData& theData) {
		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.channelId = getId(jsonObjectData, "channel_id");
	};

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildWidgetImageData& theData) {
		if (jsonObjectData->contains("widget_image") && !(*jsonObjectData)["widget_image"].is_null()) {
			theData.url = (*jsonObjectData)["widget_image"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, IntegrationData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<std::string>();
		}

		theData.enabled = getBool(jsonObjectData, "enabled");

		theData.syncing = getBool(jsonObjectData, "syncing");

		if (jsonObjectData->contains("role_id") && !(*jsonObjectData)["role_id"].is_null()) {
			theData.roleId = stoull((*jsonObjectData)["role_id"].get<std::string>());
		}

		theData.enableEmoticons = getBool(jsonObjectData, "enable_emoticons");

		if (jsonObjectData->contains("expire_behavior") && !(*jsonObjectData)["expire_behavior"].is_null()) {
			theData.expireBehavior = (*jsonObjectData)["expire_behavior"].get<int32_t>();
		}

		if (jsonObjectData->contains("expire_grace_period") && !(*jsonObjectData)["expire_grace_period"].is_null()) {
			theData.expireGracePeriod = (*jsonObjectData)["expire_grace_period"].get<int32_t>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		if (jsonObjectData->contains("account") && !(*jsonObjectData)["account"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["account"], theData.account);
		}

		if (jsonObjectData->contains("synced_at") && !(*jsonObjectData)["synced_at"].is_null()) {
			theData.syncedAt = (*jsonObjectData)["synced_at"].get<std::string>();
		}

		if (jsonObjectData->contains("subscriber_count") && !(*jsonObjectData)["subscriber_count"].is_null()) {
			theData.subscriberCount = (*jsonObjectData)["subscriber_count"].get<int32_t>();
		}

		theData.revoked = getBool(jsonObjectData, "revoked");

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], theData.application);
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, IntegrationDataVector& theData) {
		theData.theIntegrationDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			IntegrationData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theIntegrationDatas.emplace_back(newData);
		}
		theData.theIntegrationDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, OptionalAuditEntryInfoData& theData) {
		if (jsonObjectData->contains("delete_member_days") && !(*jsonObjectData)["delete_member_days"].is_null()) {
			theData.deleteMemberDays = (*jsonObjectData)["delete_member_days"].get<std::string>();
		}

		if (jsonObjectData->contains("members_removed") && !(*jsonObjectData)["members_removed"].is_null()) {
			theData.membersRemoved = (*jsonObjectData)["members_removed"].get<std::string>();
		}

		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		if (jsonObjectData->contains("count") && !(*jsonObjectData)["count"].is_null()) {
			theData.count = (*jsonObjectData)["count"].get<std::string>();
		}

		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<std::string>();
		}

		if (jsonObjectData->contains("role_name") && !(*jsonObjectData)["role_name"].is_null()) {
			theData.roleName = (*jsonObjectData)["role_name"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AuditLogChangeData& theData) {
		if (jsonObjectData->contains("new_value") && !(*jsonObjectData)["new_value"].is_null()) {
			theData.newValue = (*jsonObjectData)["new_value"];
		}

		if (jsonObjectData->contains("old_value") && !(*jsonObjectData)["old_value"].is_null()) {
			theData.oldValue = (*jsonObjectData)["old_value"];
		}

		if (jsonObjectData->contains("key") && !(*jsonObjectData)["key"].is_null()) {
			theData.key = (*jsonObjectData)["key"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildPruneCountData& theData) {
		if (jsonObjectData->contains("pruned") && !(*jsonObjectData)["pruned"].is_null()) {
			theData.count = (*jsonObjectData)["pruned"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AuditLogEntryData& theData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theData.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}

		if (jsonObjectData->contains("changes") && !(*jsonObjectData)["changes"].is_null()) {
			theData.changes.clear();
			for (auto& value: (*jsonObjectData)["changes"]) {
				AuditLogChangeData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.changes.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.id = stoull((*jsonObjectData)["id"].get<std::string>());
			theData.createdTimeStamp = theData.getCreatedAtTimestamp(TimeFormat::LongDateTime);
		}

		if (jsonObjectData->contains("action_type") && !(*jsonObjectData)["action_type"].is_null()) {
			theData.actionType = (*jsonObjectData)["action_type"].get<AuditLogEvent>();
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["options"], theData.options);
		}

		if (jsonObjectData->contains("reason") && !(*jsonObjectData)["reason"].is_null()) {
			theData.reason = (*jsonObjectData)["reason"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, PartyData& theData) {
		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("Size") && !(*jsonObjectData)["Size"].is_null()) {
			theData.size[0] = (*jsonObjectData)["Size"]["CurrentSize"].get<int32_t>();
			theData.size[1] = (*jsonObjectData)["Size"]["MaxSize"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AssetsData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, SecretsData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, TimestampData& theData) {
		if (jsonObjectData->contains("Start") && !(*jsonObjectData)["Start"].is_null()) {
			theData.start = (*jsonObjectData)["Start"].get<int64_t>();
		}

		if (jsonObjectData->contains("End") && !(*jsonObjectData)["End"].is_null()) {
			theData.end = (*jsonObjectData)["End"].get<int64_t>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ActivityData& theData) {
		if (jsonObjectData->contains("Name") && !(*jsonObjectData)["Name"].is_null()) {
			theData.name = (*jsonObjectData)["Name"].get<std::string>();
		}

		if (jsonObjectData->contains("Timestamps") && !(*jsonObjectData)["Timestamps"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["Timestamps"], theData.timestamps);
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
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["ActivityParty"], theData.party);
		}

		if (jsonObjectData->contains("ActivityAssets") && !(*jsonObjectData)["ActivityAssets"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["ActivityAssets"], theData.assets);
		}

		if (jsonObjectData->contains("ActivitySecrets") && !(*jsonObjectData)["ActivitySecrets"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["ActivitySecrets"], theData.secrets);
		}

		theData.instance = getBool(jsonObjectData, "Instance");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, PresenceUpdateFlags& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, PresenceUpdateFlags& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, PresenceUpdateFlags& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, WelcomeScreenChannelData& theData) {
		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("emoji_id") && !(*jsonObjectData)["emoji_id"].is_null()) {
			theData.emojiId = stoull((*jsonObjectData)["emoji_id"].get<std::string>());
		}

		if (jsonObjectData->contains("emoji_name") && !(*jsonObjectData)["emoji_name"].is_null()) {
			theData.emojiName = (*jsonObjectData)["emoji_name"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, WelcomeScreenData& theData) {
		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("welcome_channels") && !(*jsonObjectData)["welcome_channels"].is_null()) {
			theData.welcomeChannels.clear();
			for (auto& value: (*jsonObjectData)["welcome_channels"]) {
				WelcomeScreenChannelData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.welcomeChannels.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, PresenceUpdateData& theData) {
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
		DiscordCoreAPI::parseObject(jsonObjectData, theFlags);

		theData.theStatus |= static_cast<uint8_t>(theFlags);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, PresenceUpdateData& theData) {
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
		DiscordCoreAPI::parseObject(jsonObjectData, theFlags);

		theData.theStatus |= static_cast<uint8_t>(theFlags);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, PresenceUpdateData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

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
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["client_status"], theFlags);
		}
		theData.theStatus |= static_cast<uint8_t>(theFlags);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, StageInstanceData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("topic") && !(*jsonObjectData)["topic"].is_null()) {
			theData.topic = (*jsonObjectData)["topic"].get<std::string>();
		}

		if (jsonObjectData->contains("privacy_level") && !(*jsonObjectData)["privacy_level"].is_null()) {
			theData.privacyLevel = (*jsonObjectData)["privacy_level"].get<StageInstancePrivacyLevel>();
		}

		theData.discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, StickerData& theData) {
		theData.asset = getString(jsonObjectData, "asset");

		theData.description = getString(jsonObjectData, "description");

		theData.formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		theData.stickerFlags |= setBool(theData.stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.id = getId(jsonObjectData, "owner_id");

		theData.packId = getString(jsonObjectData, "pack_id");

		theData.type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		theData.sortValue = getUint32(jsonObjectData, "sort_value");

		theData.name = getString(jsonObjectData, "name");

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildPreviewData& theData) {
		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			theData.approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			theData.approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

			theData.discoverySplash = getString(jsonObjectData, "discovery_splash");

		if (jsonObjectData->contains("emojis") && !(*jsonObjectData)["emojis"].is_null()) {
			theData.emojis.clear();
			for (auto& value: (*jsonObjectData)["emojis"]) {
				EmojiData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.emojis.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.emojis.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("features") && !(*jsonObjectData)["features"].is_null()) {
			theData.features = (*jsonObjectData)["features"].get<std::vector<std::string>>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

			theData.splash = getString(jsonObjectData, "splash");

		theData.icon = getString(jsonObjectData, "icon");

		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildDataVector& theData) {
		theData.theGuildDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			GuildData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuildDatas.emplace_back(newData);
		}
		theData.theGuildDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildScheduledEventData& theData) {
		theData.privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		theData.entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		theData.status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		parseObject(&(*jsonObjectData)["entity_metadata"], theData.entityMetadata);

		theData.scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		theData.scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		theData.userCount = getUint32(jsonObjectData, "user_count");

		theData.channelId = strtoull(getString(jsonObjectData, "channel_id"));

		theData.creatorId = strtoull(getString(jsonObjectData, "creator_id"));

		theData.entityId = strtoull(getString(jsonObjectData, "entity_id"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.id = getId(jsonObjectData, "owner_id");

		theData.description = getString(jsonObjectData, "description");

		DiscordCoreAPI::parseObject(&(*jsonObjectData)["creator"], theData.creator);

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildScheduledEventUserData& theData) {
		if (jsonObjectData->contains("guild_scheduled_event_id") && !(*jsonObjectData)["guild_scheduled_event_id"].is_null()) {
			theData.guildScheduledEventId = (*jsonObjectData)["guild_scheduled_event_id"].get<std::string>();
		}

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildScheduledEventUserDataVector& theData) {
		theData.theGuildScheduledEventUserDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			GuildScheduledEventUserData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuildScheduledEventUserDatas.emplace_back(newData);
		}
		theData.theGuildScheduledEventUserDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, InviteData& theData) {
		if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null() && (*jsonObjectData)["code"].is_string()) {
			theData.code = (*jsonObjectData)["code"].get<std::string>();
		} else if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null() && (*jsonObjectData)["code"].is_number_integer()) {
			theData.code = std::to_string((*jsonObjectData)["code"].get<int32_t>());
		}

		if (jsonObjectData->contains("guild") && !(*jsonObjectData)["guild"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["guild"], theData.guild);
		}

		if (jsonObjectData->contains("channel") && !(*jsonObjectData)["channel"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["channel"], theData.channel);
		}

		if (jsonObjectData->contains("inviter") && !(*jsonObjectData)["inviter"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["inviter"], theData.inviter);
		}

		if (jsonObjectData->contains("target_type") && !(*jsonObjectData)["target_type"].is_null()) {
			theData.targetType = (*jsonObjectData)["target_type"].get<int32_t>();
		}

		if (jsonObjectData->contains("target_user") && !(*jsonObjectData)["target_user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["target_user"], theData.targetUser);
		}

		if (jsonObjectData->contains("target_application") && !(*jsonObjectData)["target_application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["target_application"], theData.targetApplication);
		}

		if (jsonObjectData->contains("approximate_presence_count") && !(*jsonObjectData)["approximate_presence_count"].is_null()) {
			theData.approximatePresenceCount = (*jsonObjectData)["approximate_presence_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("approximate_member_count") && !(*jsonObjectData)["approximate_member_count"].is_null()) {
			theData.approximateMemberCount = (*jsonObjectData)["approximate_member_count"].get<int32_t>();
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("expires_at") && !(*jsonObjectData)["expires_at"].is_null()) {
			theData.expiresAt = (*jsonObjectData)["expires_at"].get<std::string>();
		}

		if (jsonObjectData->contains("stage_instance") && !(*jsonObjectData)["stage_instance"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["stage_instance"], theData.stageInstance);
		}

		if (jsonObjectData->contains("guild_scheduled_event") && !(*jsonObjectData)["guild_scheduled_event"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["guild_scheduled_event"], theData.guildScheduledEvent);
		}

		if (jsonObjectData->contains("uses") && !(*jsonObjectData)["uses"].is_null()) {
			theData.uses = (*jsonObjectData)["uses"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_uses") && !(*jsonObjectData)["max_uses"].is_null()) {
			theData.maxUses = (*jsonObjectData)["max_uses"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_age") && !(*jsonObjectData)["max_age"].is_null()) {
			theData.maxAge = (*jsonObjectData)["max_age"].get<int32_t>();
		}

		theData.temporary = getBool(jsonObjectData, "temporary");

		if (jsonObjectData->contains("created_at") && !(*jsonObjectData)["created_at"].is_null()) {
			theData.createdAt = (*jsonObjectData)["created_at"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, InviteDataVector& theData) {
		theData.theInviteDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			InviteData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theInviteDatas.emplace_back(newData);
		}
		theData.theInviteDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildTemplateData& theData) {
		if (jsonObjectData->contains("serialized_source_guild") && !(*jsonObjectData)["serialized_source_guild"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["serialized_source_guild"], theData.serializedSourceGuild);
		}

		if (jsonObjectData->contains("creator") && !(*jsonObjectData)["creator"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["creator"], theData.creator);
		}

		if (jsonObjectData->contains("source_guild_id") && !(*jsonObjectData)["source_guild_id"].is_null()) {
			theData.sourceGuildId = (*jsonObjectData)["source_guild_id"].get<std::string>();
		}

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("usage_count") && !(*jsonObjectData)["usage_count"].is_null()) {
			theData.usageCount = (*jsonObjectData)["usage_count"].get<uint32_t>();
		}

		if (jsonObjectData->contains("creator_id") && !(*jsonObjectData)["creator_id"].is_null()) {
			theData.creatorId = (*jsonObjectData)["creator_id"].get<std::string>();
		}

		if (jsonObjectData->contains("created_at") && !(*jsonObjectData)["created_at"].is_null()) {
			theData.createdAt = (*jsonObjectData)["created_at"].get<std::string>();
		}

		if (jsonObjectData->contains("updated_at") && !(*jsonObjectData)["updated_at"].is_null()) {
			theData.updatedAt = (*jsonObjectData)["updated_at"].get<std::string>();
		}

		theData.isDirty = getBool(jsonObjectData, "is_dirty");

		if (jsonObjectData->contains("code") && !(*jsonObjectData)["code"].is_null()) {
			theData.code = (*jsonObjectData)["code"].get<std::string>();
		}

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildTemplateDataVector& theData) {
		theData.theGuildTemplateDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			GuildTemplateData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuildTemplateDatas.emplace_back(newData);
		}
		theData.theGuildTemplateDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, WebHookData& theData) {
		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<WebHookType>();
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		theData.name = getString(jsonObjectData, "name");

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
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["source_guild"], theData.sourceGuild);
		}

		if (jsonObjectData->contains("source_channel") && !(*jsonObjectData)["source_channel"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["source_channel"], theData.sourceChannel);
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, WebHookDataVector& theData) {
		theData.theWebHookDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			WebHookData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theWebHookDatas.emplace_back(newData);
		}
		theData.theWebHookDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AuditLogData& theData) {
		if (jsonObjectData->contains("webhooks") && !(*jsonObjectData)["webhooks"].is_null()) {
			theData.webhooks.clear();
			theData.webhooks.reserve((*jsonObjectData)["webhooks"].size());
			for (auto& value: (*jsonObjectData)["webhooks"]) {
				WebHookData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.webhooks.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("guild_scheduled_events") && !(*jsonObjectData)["guild_scheduled_events"].is_null()) {
			theData.guildScheduledEvents.clear();
			theData.guildScheduledEvents.reserve((*jsonObjectData)["guild_scheduled_events"].size());
			for (auto& value: (*jsonObjectData)["guild_scheduled_events"]) {
				GuildScheduledEventData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.guildScheduledEvents.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("auto_moderation_rules") && !(*jsonObjectData)["auto_moderation_rules"].is_null()) {
			theData.autoModerationRules.clear();
			theData.autoModerationRules.reserve((*jsonObjectData)["auto_moderation_rules"].size());
			for (auto& value: (*jsonObjectData)["auto_moderation_rules"]) {
				AutoModerationRuleData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.autoModerationRules.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("users") && !(*jsonObjectData)["users"].is_null()) {
			theData.users.clear();
			theData.users.reserve((*jsonObjectData)["users"].size());
			for (auto& value: (*jsonObjectData)["users"]) {
				UserData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.users.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("audit_log_entries") && !(*jsonObjectData)["audit_log_entries"].is_null()) {
			theData.auditLogEntries.clear();
			theData.auditLogEntries.reserve((*jsonObjectData)["audit_log_entries"].size());
			for (auto& value: (*jsonObjectData)["audit_log_entries"]) {
				AuditLogEntryData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.auditLogEntries.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("integrations") && !(*jsonObjectData)["integrations"].is_null()) {
			theData.integrations.clear();
			theData.integrations.reserve((*jsonObjectData)["integrations"].size());
			for (auto& value: (*jsonObjectData)["integrations"]) {
				IntegrationData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.integrations.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			theData.threads.reserve((*jsonObjectData)["threads"].size());
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.threads.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ReactionRemoveData& theData) {
		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("message_id") && !(*jsonObjectData)["message_id"].is_null()) {
			theData.messageId = stoull((*jsonObjectData)["message_id"].get<std::string>());
		}

		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("emoji") && !(*jsonObjectData)["emoji"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["emoji"], theData.emoji);
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationCommandOptionChoiceData& theData) {
		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null() && (*jsonObjectData)["value"].is_string()) {
			if ((*jsonObjectData)["value"].is_string()) {
				theData.value = (*jsonObjectData)["value"].get<std::string>();
			} else if ((*jsonObjectData)["value"].is_number_float()) {
				theData.value = (*jsonObjectData)["value"].get<float>();
			} else if ((*jsonObjectData)["value"].is_number()) {
				theData.value = (*jsonObjectData)["value"].get<int32_t>();
			}
		}

		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("name_localizations") && !(*jsonObjectData)["name_localizations"].is_null()) {
			theData.nameLocalizations.clear();
			auto newMap = (*jsonObjectData)["name_localizations"].get<std::unordered_map<std::string, std::string>>();
			for (auto& [key, newValue]: newMap) {
				theData.nameLocalizations[key] = newValue;
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationCommandOptionData& theData) {
		theData.name = getString(jsonObjectData, "name");

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

		if (jsonObjectData->contains("description") && !(*jsonObjectData)["description"].is_null()) {
			theData.description = (*jsonObjectData)["description"].get<std::string>();
		}

		if (jsonObjectData->contains("channel_types") && !(*jsonObjectData)["channel_types"].is_null()) {
			theData.channelTypes.clear();
			theData.channelTypes.reserve((*jsonObjectData)["channel_types"].size());
			for (auto& value: (*jsonObjectData)["channel_types"]) {
				theData.channelTypes.push_back(value);
			}
		}

		if (jsonObjectData->contains("min_value") && !(*jsonObjectData)["min_value"].is_null()) {
			theData.minValue = (*jsonObjectData)["min_value"].get<int32_t>();
		}

		if (jsonObjectData->contains("max_value") && !(*jsonObjectData)["max_value"].is_null()) {
			theData.maxValue = (*jsonObjectData)["max_value"].get<int32_t>();
		}

		theData.required = getBool(jsonObjectData, "required");

		theData.autocomplete = getBool(jsonObjectData, "autocomplete");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<ApplicationCommandOptionType>();
		}

		if (theData.type == ApplicationCommandOptionType::Sub_Command_Group || theData.type == ApplicationCommandOptionType::Sub_Command) {
			if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
				theData.options.clear();
				for (auto& value: (*jsonObjectData)["options"]) {
					ApplicationCommandOptionData newData{};
					DiscordCoreAPI::parseObject(value.value().get_object(), newData);
					theData.options.emplace_back(newData);
				}
			}
		} else {
			if (jsonObjectData->contains("choices") && !(*jsonObjectData)["choices"].is_null()) {
				theData.choices.clear();
				for (auto& value: (*jsonObjectData)["choices"]) {
					ApplicationCommandOptionChoiceData newData{};
					DiscordCoreAPI::parseObject(value.value().get_object(), newData);
					theData.choices.emplace_back(newData);
				}
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, TypingStartData& theData) {
		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);

		if (jsonObjectData->contains("user_id") && !(*jsonObjectData)["user_id"].is_null()) {
			theData.userId = stoull((*jsonObjectData)["user_id"].get<std::string>());
		}

		if (jsonObjectData->contains("timestamp") && !(*jsonObjectData)["timestamp"].is_null()) {
			theData.timestamp = (*jsonObjectData)["timestamp"].get<int32_t>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, YouTubeFormatVector& theData) {
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

					theData.theFormats.emplace_back(newData);
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
					theData.theFormats.emplace_back(newData);
				}
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, UserCommandInteractionData& theData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theData.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, MessageCommandInteractionData& theData) {
		if (jsonObjectData->contains("target_id") && !(*jsonObjectData)["target_id"].is_null()) {
			theData.targetId = stoull((*jsonObjectData)["target_id"].get<std::string>());
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ComponentInteractionData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ModalInteractionData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, AllowedMentionsData& theData) {
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, SelectOptionData& theData) {
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
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["emoji"], theData.emoji);
		}

		theData._default = getBool(jsonObjectData, "default");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ActionRowData& theData) {
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
					DiscordCoreAPI::parseObject(&value["emoji"], newData.emoji);
				}

				if (value.contains("url") && !value["url"].is_null()) {
					newData.url = value["url"].get<std::string>();
				}

				if (value.contains("options") && !value["options"].is_null()) {
					for (auto& value02: value["options"]) {
						SelectOptionData newerData{};
						DiscordCoreAPI::parseObject(&value, newerData);
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationCommandData& theData) {
		theData.id = getId(jsonObjectData, "id");

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

		theData.name = getString(jsonObjectData, "name");

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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.options.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ChannelMentionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<ChannelType>();
		}

		theData.name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ChannelPinsUpdateEventData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		if (jsonObjectData->contains("last_pin_timestamp") && !(*jsonObjectData)["last_pin_timestamp"].is_null()) {
			theData.lastPinTimeStamp = (*jsonObjectData)["last_pin_timestamp"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ThreadListSyncData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.members.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("threads") && !(*jsonObjectData)["threads"].is_null()) {
			theData.threads.clear();
			for (auto& value: (*jsonObjectData)["threads"]) {
				ChannelData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.threads.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ThreadMembersUpdateData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("member_count") && !(*jsonObjectData)["member_count"].is_null()) {
			theData.memberCount = (*jsonObjectData)["member_count"].get<int32_t>();
		}

		if (jsonObjectData->contains("added_members") && !(*jsonObjectData)["added_members"].is_null()) {
			theData.addedMembers.clear();
			for (auto& value: (*jsonObjectData)["added_members"]) {
				ThreadMemberData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.addedMembers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("removed_member_ids") && !(*jsonObjectData)["removed_member_ids"].is_null()) {
			theData.removedMemberIds.clear();
			for (auto& value: (*jsonObjectData)["removed_member_ids"]) {
				theData.removedMemberIds.emplace_back(value);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, MessageInteractionData& theData) {
		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<InteractionType>();
		}

		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, StickerItemData& theData) {
		theData.id = getId(jsonObjectData, "id");

		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("format_type") && !(*jsonObjectData)["format_type"].is_null()) {
			theData.formatType = (*jsonObjectData)["format_type"].get<StickerItemType>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, MessageDataOld& theData) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theData.content = (*jsonObjectData)["content"].get<std::string>();
		}

		theData.id = getId(jsonObjectData, "id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		DiscordCoreAPI::parseObject(jsonObjectData["author"].get_object(), theData.author);

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);

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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.mentions.emplace_back(newData);
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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.reactions.emplace_back(newData);
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
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["activity"], theData.activity);
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], theData.application);
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["message_reference"], theData.messageReference);
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theData.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["interaction"], theData.interaction);
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.components.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["thread"], theData.thread);
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, MessageData& theData) {
		if (jsonObjectData->contains("content") && !(*jsonObjectData)["content"].is_null()) {
			theData.content = (*jsonObjectData)["content"].get<std::string>();
		}

		theData.id = getId(jsonObjectData, "id");

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		DiscordCoreAPI::parseObject(jsonObjectData["author"].get_object(), theData.author);

		DiscordCoreAPI::parseObject(jsonObjectData["member"].get_object(), theData.member);

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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.mentions.emplace_back(newData);
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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.mentionChannels.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("attachments") && !(*jsonObjectData)["attachments"].is_null()) {
			theData.attachments.clear();
			for (auto& value: (*jsonObjectData)["attachments"]) {
				AttachmentData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.attachments.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("embeds") && !(*jsonObjectData)["embeds"].is_null()) {
			theData.embeds.clear();
			for (auto& value: (*jsonObjectData)["embeds"]) {
				EmbedData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.embeds.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("reactions") && !(*jsonObjectData)["reactions"].is_null()) {
			theData.reactions.clear();
			for (auto& value: (*jsonObjectData)["reactions"]) {
				ReactionData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.reactions.emplace_back(newData);
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
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["activity"], theData.activity);
		}

		if (jsonObjectData->contains("application") && !(*jsonObjectData)["application"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["application"], theData.application);
		}

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}

		if (jsonObjectData->contains("message_reference") && !(*jsonObjectData)["message_reference"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["message_reference"], theData.messageReference);
		}

		if (jsonObjectData->contains("flags") && !(*jsonObjectData)["flags"].is_null()) {
			theData.flags = (*jsonObjectData)["flags"].get<int32_t>();
		}

		if (jsonObjectData->contains("sticker_items") && !(*jsonObjectData)["sticker_items"].is_null()) {
			theData.stickerItems.clear();
			for (auto& value: (*jsonObjectData)["sticker_items"]) {
				StickerItemData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.stickerItems.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.stickers.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("interaction") && !(*jsonObjectData)["interaction"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["interaction"], theData.interaction);
		}

		if (jsonObjectData->contains("components") && !(*jsonObjectData)["components"].is_null()) {
			theData.components.clear();
			for (auto& value: (*jsonObjectData)["components"]) {
				ActionRowData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.components.emplace_back(newData);
			}
		}

		if (jsonObjectData->contains("thread") && !(*jsonObjectData)["thread"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["thread"], theData.thread);
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, StickerPackData& theData) {
		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.stickers.emplace_back(newData);
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

		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("id") && !(*jsonObjectData)["id"].is_null()) {
			theData.Id = stoull((*jsonObjectData)["id"].get<std::string>());
		}

		if (jsonObjectData->contains("sku_id") && !(*jsonObjectData)["sku_id"].is_null()) {
			theData.skuId = (*jsonObjectData)["sku_id"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, StickerPackDataVector& theData) {
		theData.theStickerPackDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			StickerPackData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theStickerPackDatas.emplace_back(newData);
		}
		theData.theStickerPackDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ConnectionData& theData) {
		theData.name = getString(jsonObjectData, "name");

		theData.id = getId(jsonObjectData, "id");

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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.integrations.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ConnectionDataVector& theData) {
		theData.theConnectionDatas.reserve(jsonObjectData->size());
		for (auto value: jsonObjectData) {
			ConnectionData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theConnectionDatas.emplace_back(newData);
		}
		theData.theConnectionDatas.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ApplicationCommandInteractionDataOption& theData) {
		theData.name = getString(jsonObjectData, "name");

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<ApplicationCommandOptionType>();
		}

		theData.focused = getBool(jsonObjectData, "focused");

		if (jsonObjectData->contains("value") && !(*jsonObjectData)["value"].is_null()) {
			if ((*jsonObjectData)["value"].is_boolean()) {
				theData.valueBool = getBool(jsonObjectData, "value");
			}
			if ((*jsonObjectData)["value"].is_string()) {
				theData.valueString = (*jsonObjectData)["value"].get<std::string>();
			}
			if ((*jsonObjectData)["value"].is_number()) {
				theData.valueInt = (*jsonObjectData)["value"].get<int32_t>();
			}
		}

		if (jsonObjectData->contains("options") && !(*jsonObjectData)["options"].is_null()) {
			theData.options.clear();
			for (auto& value: (*jsonObjectData)["options"]) {
				ApplicationCommandInteractionDataOption newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.options.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, ResolvedData& theData) {
		getMap(jsonObjectData, theData.attachments, "attachments");

		getMap(jsonObjectData, theData.channels, "channels");

		getMap(jsonObjectData, theData.messages, "messages");

		getMap(jsonObjectData, theData.members, "members");

		getMap(jsonObjectData, theData.users, "users");

		getMap(jsonObjectData, theData.roles, "roles");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, ApplicationCommandInteractionData& theData) {
		theData.type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		theData.guildId = getId(jsonObjectData, "guild_id");

		getArray(jsonObjectData, theData.options, "options");

		theData.name = getString(jsonObjectData, "name");

		parseObject(jsonObjectData, theData.resolved);

		theData.id = getId(jsonObjectData, "id");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, InteractionDataData& theData) {
		DiscordCoreAPI::parseObject(jsonObjectData, theData.applicationCommandData);

		DiscordCoreAPI::parseObject(jsonObjectData, theData.messageInteractionData);

		DiscordCoreAPI::parseObject(jsonObjectData, theData.userInteractionData);

		DiscordCoreAPI::parseObject(jsonObjectData, theData.componentData);

		DiscordCoreAPI::parseObject(jsonObjectData, theData.modalData);
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, InteractionData& theData) {
		if (jsonObjectData->contains("data") && !(*jsonObjectData)["data"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["data"], theData.data);
			theData.rawData = (*jsonObjectData)["data"];
		}

		if (jsonObjectData->contains("app_permissions") && !(*jsonObjectData)["app_permissions"].is_null()) {
			theData.appPermissions = (*jsonObjectData)["app_permissions"].get<std::string>();
		}

		if (jsonObjectData->contains("type") && !(*jsonObjectData)["type"].is_null()) {
			theData.type = (*jsonObjectData)["type"].get<InteractionType>();
		}

		if (jsonObjectData->contains("token") && !(*jsonObjectData)["token"].is_null()) {
			theData.token = (*jsonObjectData)["token"].get<std::string>();
		}

		if (jsonObjectData->contains("member") && !(*jsonObjectData)["member"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["member"], theData.member);
			theData.user.id = theData.member.id;
			theData.user.userName = theData.member.getUserData().userName;
		} else if (jsonObjectData->contains("user") && !(*jsonObjectData)["user"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["user"], theData.user);
		}

		theData.channelId = getId(jsonObjectData, "channel_id");

		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("locale") && !(*jsonObjectData)["locale"].is_null()) {
			theData.locale = (*jsonObjectData)["locale"].get<std::string>();
		}

		if (jsonObjectData->contains("guild_locale") && !(*jsonObjectData)["guild_locale"].is_null()) {
			theData.guildLocale = (*jsonObjectData)["guild_locale"].get<std::string>();
		}

		if (jsonObjectData->contains("message") && !(*jsonObjectData)["message"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["message"], theData.message);
		}

		if (jsonObjectData->contains("version") && !(*jsonObjectData)["version"].is_null()) {
			theData.version = (*jsonObjectData)["version"].get<int32_t>();
		}

		theData.id = getId(jsonObjectData, "id");

		if (jsonObjectData->contains("application_id") && !(*jsonObjectData)["application_id"].is_null()) {
			theData.applicationId = stoull((*jsonObjectData)["application_id"].get<std::string>());
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, SessionStartData& theData) {
		if (jsonObjectData->contains("max_concurrency") && !(*jsonObjectData)["max_concurrency"].is_null()) {
			theData.maxConcurrency = (*jsonObjectData)["max_concurrency"].get<uint32_t>();
		}

		if (jsonObjectData->contains("remaining") && !(*jsonObjectData)["remaining"].is_null()) {
			theData.remaining = (*jsonObjectData)["remaining"].get<uint32_t>();
		}

		if (jsonObjectData->contains("reset_after") && !(*jsonObjectData)["reset_after"].is_null()) {
			theData.resetAfter = (*jsonObjectData)["reset_after"].get<uint32_t>();
		}

		if (jsonObjectData->contains("total") && !(*jsonObjectData)["total"].is_null()) {
			theData.total = (*jsonObjectData)["total"].get<uint32_t>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GatewayBotData& theData) {
		if (jsonObjectData->contains("session_start_limit") && !(*jsonObjectData)["session_start_limit"].is_null()) {
			DiscordCoreAPI::parseObject(&(*jsonObjectData)["session_start_limit"], theData.sessionStartLimit);
		}

		if (jsonObjectData->contains("shards") && !(*jsonObjectData)["shards"].is_null()) {
			theData.shards = (*jsonObjectData)["shards"].get<uint32_t>();
		}

		if (jsonObjectData->contains("url") && !(*jsonObjectData)["url"].is_null()) {
			theData.url = (*jsonObjectData)["url"].get<std::string>();
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildEmojisUpdateEventData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("emojis") && !(*jsonObjectData)["emojis"].is_null()) {
			theData.emojis.clear();
			for (auto& value: (*jsonObjectData)["emojis"]) {
				EmojiData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.emojis.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildStickersUpdateEventData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

		if (jsonObjectData->contains("stickers") && !(*jsonObjectData)["stickers"].is_null()) {
			theData.stickers.clear();
			for (auto& value: (*jsonObjectData)["stickers"]) {
				StickerData newData{};
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.stickers.emplace_back(newData);
			}
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildMembersChunkEventData& theData) {
		theData.guildId = getId(jsonObjectData, "guild_id");

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
				DiscordCoreAPI::parseObject(value.value().get_object(), newData);
				theData.presences.emplace_back(newData);
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
				DiscordCoreAPI::parseObject(&value, theDataNew);
				theData.members.emplace_back(theDataNew);
			}
		}
	}
	*/
	void CommandData::parseOptions(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData) {
		for (auto theOptionIterator = jsonObjectData["options"].begin(); theOptionIterator != jsonObjectData.at("options").end(); ++theOptionIterator) {
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
	
	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, CommandData& theData) {
		for (auto theIterator: jsonObjectData) {
			if (theIterator.value().find_field_unordered("type").get_int64() == 1) {
				if (!theIterator.value().find_field_unordered("name").is_null()) {
					theData.subCommandName = theIterator.value().get_string().take_value().data();
				}
			} else if (theIterator.value().find_field_unordered("type").get_int64() == 2) {
				if (!theIterator.value().find_field_unordered("name").is_null()) {
					theData.subCommandGroupName = theIterator.value().get_string().take_value().data();
				}
			}
		}
		simdjson::fallback::ondemand::array theArray{};
		auto theResult = jsonObjectData["options"].get(theArray);
		if (theResult==simdjson::error_code::SUCCESS) {
			theData.parseOptions(theArray);
		}

		theData.commandName = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, Song& theData) {
		theData.duration = getString(jsonObjectData["lengthText"]["accessibility"]["accessibilityData"]["label"].get_object(), "label");

		if (jsonObjectData->contains("detailedMetadataSnippets") && !(*jsonObjectData)["detailedMetadataSnippets"].is_null()) {
			for (auto& value: (*jsonObjectData)["detailedMetadataSnippets"][0]["snippetText"]["runs"]) {
				std::string newString = value["text"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				theData.description = utf8MakeValid(newString);
			}
		}

		if (jsonObjectData->contains("thumbnail") && !(*jsonObjectData)["thumbnail"].is_null()) {
			theData.thumbnailUrl = (*jsonObjectData)["thumbnail"]["thumbnails"][0]["url"].get<std::string>();
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

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildScheduledEventVector& theData) {
		for (auto value: jsonObjectData) {
			GuildScheduledEvent newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuildScheduledEvents.emplace_back(newData);
		}
		theData.theGuildScheduledEvents.shrink_to_fit();
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, GuildScheduledEventDataVector& theData) {
		for (auto value: jsonObjectData) {
			GuildScheduledEventData newData{};
			DiscordCoreAPI::parseObject(value.value().get_object(), newData);
			theData.theGuildScheduledEventDatas.emplace_back(newData);
		}
		theData.theGuildScheduledEventDatas.shrink_to_fit();
	}
};

namespace DiscordCoreInternal {

	JsonParseError::JsonParseError(int32_t theCode) : std::runtime_error(theErrors[theCode]){};

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& jsonObjectData, WebSocketMessage& theData) {
		try {
			theData.op = DiscordCoreAPI::getUint32(jsonObjectData, "op");

			theData.s = DiscordCoreAPI::getUint32(jsonObjectData, "s");

			theData.t = DiscordCoreAPI::getString(jsonObjectData, "t");
		} catch (...) {
			DiscordCoreAPI::reportException("parseObject()");
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, WebSocketMessage& theData) {
		try {
			theData.op = DiscordCoreAPI::getUint32(jsonObjectData, "op");

			theData.s = DiscordCoreAPI::getUint32(jsonObjectData, "s");

			theData.t = DiscordCoreAPI::getString(jsonObjectData, "t");
		} catch (...) {
			DiscordCoreAPI::reportException("parseObject()");
		}
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, HelloData& theData) {
		theData.heartbeatInterval = DiscordCoreAPI::getUint64(jsonObjectData["d"].get_object(), "heartbeat_interval");
	}

	template<> void parseObject(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& jsonObjectData, InvalidSessionData& theData) {
		theData.d = DiscordCoreAPI::getBool(jsonObjectData, "d");
	}
}