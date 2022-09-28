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
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreAPI {

	JsonParseError::JsonParseError(int32_t theCode) : std::runtime_error(theErrors[theCode]){};

	Snowflake getId(simdjson::ondemand::value jsonObjectData, const char* theKey) {
		return Snowflake{ DiscordCoreAPI::strtoull(getString(jsonObjectData, theKey)) };
	}

	float getFloat(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			double theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<float>(theValue);
			} else {
				return 0.0f;
			}
		} catch (...) {
			reportException("getFloat()");
			return 0.0f;
		}
	}

	bool getBool(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			bool theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return theValue;
			} else {
				return false;
			}
		} catch (...) {
			reportException("getBool()");
			return 0;
		}
	}

	uint8_t getUint8(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<uint8_t>(theValue);
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint8()");
			return 0;
		}
	}

	uint16_t getUint16(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<uint16_t>(theValue);
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint16()");
			return 0;
		}
	}

	uint32_t getUint32(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<uint32_t>(theValue);
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint32()");
			return 0;
		}
	}

	uint64_t getUint64(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return theValue;
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint64()");
			return 0;
		}
	}

	std::string getString(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			std::string_view theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return std::string{ theValue.data(), theValue.size() };
			} else {
				return "";
			}
		} catch (...) {
			reportException("getString()");
			return "";
		}
	}

	ObjectReturnData getObject(ArrayReturnData jsonObjectData, size_t objectIndex, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (jsonObjectData.didItSucceed) {
				auto theResult = jsonObjectData.theArray.at(objectIndex).get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ObjectReturnData getObject(ObjectReturnData jsonObjectData, const char* objectName, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (jsonObjectData.didItSucceed) {
				auto theResult = jsonObjectData.theObject[objectName].get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ArrayReturnData getArray(ObjectReturnData jsonObjectData, const char* arrayName) {
		ArrayReturnData theValue{};
		try {
			if (jsonObjectData.didItSucceed) {
				auto theResult = jsonObjectData.theObject[arrayName].get(theValue.theArray);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getArray()");
			return theValue;
		}
		return theValue;
	}

	ObjectReturnData getObject(simdjson::ondemand::array jsonObjectData, size_t objectIndex, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (!jsonObjectData.at(objectIndex).is_null()) {
				auto theResult = jsonObjectData.at(objectIndex).get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (!jsonObjectData.is_null()) {
				auto theResult = jsonObjectData[objectName].get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName) {
		ArrayReturnData theValue{};
		try {
			auto theResult = jsonObjectData[arrayName].get(theValue.theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				theValue.didItSucceed = true;
				return theValue;
			} else {
				return theValue;
			}
		} catch (...) {
			reportException("getArray()");
			return theValue;
		}
	}

	std::string getString(ObjectReturnData jsonData, const char* theKey) {
		try {
			std::string_view theValue{};
			if (jsonData.didItSucceed) {
				jsonData.theObject[theKey].get(theValue);
				return std::string{ theValue.data(), theValue.size() };
			} else {
				return "";
			}
		} catch (...) {
			reportException("getString()");
			return "";
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildData& theData) {
		this->flags |= setBool(this->flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));
		this->flags |= setBool(this->flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		this->flags |= setBool(this->flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		this->flags |= setBool(this->flags, GuildFlags::Large, getUint8(jsonObjectData, "large"));

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->id = getId(jsonObjectData, "id");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		this->threads.clear();

		simdjson::ondemand::array theArray01{};
		auto theResult = jsonObjectData["threads"].get(theArray01);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->threads.clear();
			for (auto value: theArray01) {
				auto& theObject = value.value();
				this->threads.emplace_back(getId(theObject, "id"));
			}
		}

		simdjson::ondemand::array theArray02{};
		theResult = jsonObjectData["stickers"].get(theArray02);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->stickers.clear();
			for (auto value: theArray02) {
				this->stickers.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray03{};
		theResult = jsonObjectData["guild_scheduled_events"].get(theArray03);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->guildScheduledEvents.clear();
			for (auto value: theArray03) {
				this->guildScheduledEvents.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray04{};
		theResult = jsonObjectData["stage_instances"].get(theArray04);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->stageInstances.clear();
			for (auto value: theArray04) {
				this->stageInstances.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray05{};
		theResult = jsonObjectData["emoji"].get(theArray05);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->emoji.clear();
			for (auto value: theArray05) {
				this->emoji.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray06{};
		if (Roles::doWeCacheRoles) {
			this->roles.clear();
			RoleData newData{};
			theResult = jsonObjectData["roles"].get(theArray06);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray06) {
					parseObject(value.value(), newData);
					newData.guildId = this->id;
					this->roles.emplace_back(newData.id);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray08{};
		if (GuildMembers::doWeCacheGuildMembers) {
			this->members.clear();
			GuildMemberData newData{};
			theResult = jsonObjectData["members"].get(theArray08);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray08) {
					parseObject(value.value(), newData);
					newData.guildId = this->id;
					this->members.emplace_back(newData.id);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray07{};
		if (GuildMembers::doWeCacheGuildMembers) {
			theResult = jsonObjectData["voice_states"].get(theArray07);
			GuildMemberData theDataNew{};
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray07) {
					auto userId = getId(value.value(), "user_id");
					theDataNew.id = userId;
					theDataNew.guildId = this->id;
					GuildMembers::cache.at(theDataNew).voiceChannelId = getId(value.value(), "channel_id");
				}
			}
		}

		simdjson::ondemand::array theArray09{};
		if (GuildMembers::doWeCacheGuildMembers) {
			this->presences.clear();
			PresenceUpdateData newData{};
			theResult = jsonObjectData["presences"].get(theArray09);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray09) {
					parseObject(value.value(), newData);
					newData.guildId = this->id;
					this->presences.emplace_back(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray10{};
		if (Channels::doWeCacheChannels) {
			this->channels.clear();
			ChannelData newData{};
			theResult = jsonObjectData["channels"].get(theArray10);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray10) {
					parseObject(value.value(), newData);
					newData.guildId = this->id;
					this->channels.emplace_back(newData.id);
					Channels::insertChannel(std::move(newData));
				}
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, Guild& theData) {
		this->id = getId(jsonObjectData, "id");

		this->channels.clear();
		Channel newChannel{};
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["threads"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newChannel);
				newChannel.guildId = this->id;
				this->threads.emplace_back(newChannel);
			}
		}

		Sticker newSticker{};
		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newSticker);
				newSticker.guildId = this->id;
				this->stickers.emplace_back(newSticker);
			}
		}

		GuildScheduledEvent newGuildEvent{};
		theResult = jsonObjectData["guild_scheduled_events"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newGuildEvent);
				newGuildEvent.guildId = this->id;
				this->guildScheduledEvents.emplace_back(newGuildEvent);
			}
		}

		StageInstance newStageInstance{};
		theResult = jsonObjectData["stage_instances"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newStageInstance);
				newStageInstance.guildId = this->id;
				this->stageInstances.emplace_back(newStageInstance);
			}
		}

		EmojiData newEmojiData{};
		theResult = jsonObjectData["emoji"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newEmojiData);
				this->emoji.emplace_back(newEmojiData);
			}
		}

		if (Roles::doWeCacheRoles) {
			this->roles.clear();
			RoleData newData{};
			theResult = jsonObjectData["roles"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					auto& theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = this->id;
					this->roles.emplace_back(newData);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			this->members.clear();
			GuildMemberData newData{};
			theResult = jsonObjectData["members"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					auto& theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = this->id;
					this->members.emplace_back(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			theResult = jsonObjectData["voice_states"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					auto& theObject = value.value();
					auto userId = getId(theObject, "user_id");
					GuildMemberData theDataNew{};
					theDataNew.id = userId;
					theDataNew.guildId = this->id;
					if (GuildMembers::cache.contains(theDataNew)) {
						GuildMembers::cache.at(theDataNew).voiceChannelId = getId(theObject, "channel_id");
					}
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			this->presences.clear();
			PresenceUpdateData newData{};
			theResult = jsonObjectData["presences"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					auto& theObject = value.value();
					parseObject(theObject, newData);
					auto userId = newData.userId;
					newData.guildId = this->id;
					this->presences.emplace(userId, std::move(newData));
				}
			}
		}

		if (Channels::doWeCacheChannels) {
			this->channels.clear();
			ChannelData newData{};
			theResult = jsonObjectData["channels"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					auto& theObject = value.value();
					parseObject(theObject, newData);
					newData.guildId = this->id;
					this->channels.emplace_back(newData);
					Channels::insertChannel(std::move(newData));
				}
			}
		}

		this->afkChannelId = getId(jsonObjectData, "afk_channel_id");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		this->icon = getString(jsonObjectData, "icon_hash");

		this->splash = getString(jsonObjectData, "splash");

		this->discoverySplash = getString(jsonObjectData, "discovery_splash");

		this->flags = setBool(this->flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->preferredLocale = getString(jsonObjectData, "preferred_locale");

		this->publicUpdatesChannelId = getId(jsonObjectData, "public_updates_channel_id");

		this->vanityUrlCode = getString(jsonObjectData, "vanity_url_code");

		this->description = getString(jsonObjectData, "description");

		this->banner = getString(jsonObjectData, "banner");

		this->rulesChannelId = getId(jsonObjectData, "rule_Channel_id");

		this->applicationId = getId(jsonObjectData, "application_id");

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->widgetChannelId = getId(jsonObjectData, "widget_channel_id");

		this->systemChannelId = getId(jsonObjectData, "system_channel_id");

		this->region = getString(jsonObjectData, "region");

		this->afkChannelId = getId(jsonObjectData, "afk_channel_id");

		this->region = getString(jsonObjectData, "region");

		theResult = jsonObjectData["features"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->features.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->features.emplace_back(std::string{ value.get_string().take_value().data() });
			}
		}

		this->permissions = getString(jsonObjectData, "permissions");

		this->afkTimeOut = static_cast<AfkTimeOutDurations>(getUint8(jsonObjectData, "afk_timeout"));

		this->flags = setBool(this->flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		this->flags = setBool(this->flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));

		this->verificationLevel = static_cast<VerificationLevel>(getUint8(jsonObjectData, "verification_level"));

		this->defaultMessageNotifications = static_cast<DefaultMessageNotificationLevel>(getUint8(jsonObjectData, "default_message_notification_level"));

		this->explicitContentFilter = static_cast<ExplicitContentFilterLevel>(getUint8(jsonObjectData, "explicit_content_filter_level"));

		this->mfaLevel = static_cast<MFALevel>(getUint8(jsonObjectData, "mfa_level"));

		this->systemChannelFlags = static_cast<SystemChannelFlags>(getUint8(jsonObjectData, "system_channel_flags"));

		this->flags = setBool(this->flags, GuildFlags::Large, getBool(jsonObjectData, "large"));

		this->flags = setBool(this->flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		this->memberCount = getUint32(jsonObjectData, "member_count");

		this->maxPresences = getUint32(jsonObjectData, "max_presences");

		this->maxMembers = getUint32(jsonObjectData, "max_members");

		this->premiumSubscriptionCount = getUint32(jsonObjectData, "premium_subscription_count");

		this->premiumTier = static_cast<PremiumTier>(getUint8(jsonObjectData, "premium_tier"));

		this->maxVideoChannelUsers = getUint32(jsonObjectData, "max_video_channel_users");

		this->approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		this->approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["welcome_screen"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->welcomeScreen);
		}

		this->nsfwLevel = static_cast<GuildNSFWLevel>(getUint8(jsonObjectData, "nsfw_level"));
	}

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ApplicationCommandVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theApplicationCommands.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ApplicationCommand newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theApplicationCommands.push_back(newData);
				}
				this->theApplicationCommands.shrink_to_fit();
			}
		}
	}




	template<> void parseObject(simdjson::ondemand::value jsonObjectData, AutoModerationRule& theData) {
		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		this->enabled = getBool(jsonObjectData, "enabled");

		this->triggerType = static_cast<TriggerType>(getUint64(jsonObjectData, "trigger_type"));

		this->eventType = static_cast<EventType>(getUint64(jsonObjectData, "event_type"));

		this->creatorId = getId(jsonObjectData, "creator_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["actions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->actions.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ActionData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->actions.push_back(newData);
			}
			this->actions.shrink_to_fit();
		}

		theResult = jsonObjectData["exempt_roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->exemptRoles.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->exemptRoles.push_back(Snowflake{ value.get_uint64().value() });
			}
			this->exemptRoles.shrink_to_fit();
		}

		theResult = jsonObjectData["exempt_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->exemptChannels.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->exemptChannels.push_back(Snowflake{ value.get_uint64().value() });
			}
			this->exemptChannels.shrink_to_fit();
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["trigger_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			TriggerMetaData theDataNew{};
			parseObject(theObject, theDataNew);
			this->triggerMetaData = theDataNew;
		}

		this->guildId = getId(jsonObjectData, "guild_id");
	}



	

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ModalInteractionData& theData) {
		simdjson::ondemand::value theComponent{};
		auto theResult = jsonObjectData["components"][0]["components"][0].get(theComponent);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->value = getString(theComponent, "value");
			this->customIdSmall = getString(theComponent, "value");
		}

		this->customId = getString(jsonObjectData, "custom_id");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ComponentInteractionData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["values"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->values.clear();
			for (auto iterator = theArray.begin(); iterator != theArray.end(); ++iterator) {
				this->values.emplace_back(iterator.value().operator*().get_string().take_value().data());
			}
		}

		this->customId = getString(jsonObjectData, "custom_id");

		this->componentType = static_cast<ComponentType>(getUint8(jsonObjectData, "component_type"));
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, SelectOptionData& theData) {
		this->label = getString(jsonObjectData, "label");

		this->value = getString(jsonObjectData, "value");

		this->description = getString(jsonObjectData, "description");

		simdjson::ondemand::value theEmoji{};
		auto theResult = jsonObjectData["emoji"].get(theEmoji);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theEmoji, this->emoji);
		}

		this->_default = getBool(jsonObjectData, "default");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ComponentData& theData) {
		this->type = static_cast<ComponentType>(getUint8(jsonObjectData, "type"));

		this->customId = getString(jsonObjectData, "custom_id");

		this->placeholder = getString(jsonObjectData, "placeholder");

		this->disabled = getBool(jsonObjectData, "disabled");

		this->style = getUint32(jsonObjectData, "style");

		this->label = getString(jsonObjectData, "label");

		this->minLength = getUint32(jsonObjectData, "min_length");

		this->maxLength = getUint32(jsonObjectData, "max_length");

		this->maxValues = getUint32(jsonObjectData, "max_values");

		this->maxValues = getUint32(jsonObjectData, "min_values");

		this->title = getString(jsonObjectData, "title");

		this->required = getBool(jsonObjectData, "required");

		simdjson::ondemand::value theEmoji{};
		auto theResult = jsonObjectData["emoji"].get(theEmoji);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theEmoji, this->emoji);
		}

		this->url = getString(jsonObjectData, "url");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["select_option_data"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->options.clear();
			SelectOptionData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->options.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ActionRowData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["components"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->components.clear();
			ComponentData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->components.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, UserCommandInteractionData& theData) {
		this->targetId = getId(jsonObjectData, "target_id");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, MessageCommandInteractionData& theData) {
		this->targetId = getId(jsonObjectData, "target_id");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, MessageInteractionData& theData) {
		this->id = getId(jsonObjectData, "id");

		this->type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->user);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
		}
	}

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, StickerItemData& theData) {
		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		this->formatType = static_cast<StickerItemType>(getUint8(jsonObjectData, "format_type"));
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ChannelMentionData& theData) {
		this->id = getId(jsonObjectData, "id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, MessageData& theData) {
		this->content = getString(jsonObjectData, "content");

		this->id = getId(jsonObjectData, "id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["author"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->author);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
		}

		this->timestamp = getString(jsonObjectData, "timestamp");

		this->editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		this->tts = getBool(jsonObjectData, "tts");

		this->mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		UserData newData{};
		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["mentions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->mentions.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->mentions.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["mention_roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->mentionRoles.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto theObject = value.get_string().take_value().data();
				this->mentionRoles.emplace_back(std::move(theObject));
			}
		}

		theResult = jsonObjectData["mention_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->mentionChannels.clear();
			ChannelMentionData newChannelData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newChannelData);
				this->mentionChannels.emplace_back(std::move(newChannelData));
			}
		}

		theResult = jsonObjectData["attachments"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->attachments.clear();
			AttachmentData newAttachmentData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newAttachmentData);
				this->attachments.emplace_back(std::move(newAttachmentData));
			}
		}

		theResult = jsonObjectData["embeds"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->embeds.clear();
			EmbedData newEmbedData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newEmbedData);
				this->embeds.emplace_back(std::move(newEmbedData));
			}
		}

		theResult = jsonObjectData["reactions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->reactions.clear();
			ReactionData newReactionData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newReactionData);
				this->reactions.emplace_back(std::move(newReactionData));
			}
		}

		this->nonce = getString(jsonObjectData, "nonce");

		this->pinned = getBool(jsonObjectData, "pinned");

		this->webHookId = getId(jsonObjectData, "webhook_id");

		this->type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		theResult = jsonObjectData["activity"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->activity);
		}

		theResult = jsonObjectData["application"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->application);
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		theResult = jsonObjectData["message_reference"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->messageReference);
		}

		this->flags = getUint32(jsonObjectData, "flags");

		theResult = jsonObjectData["sticker_items"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->stickerItems.clear();
			StickerItemData newReactionData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newReactionData);
				this->stickerItems.emplace_back(std::move(newReactionData));
			}
		}

		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->stickers.clear();
			StickerData newReactionData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newReactionData);
				this->stickers.emplace_back(std::move(newReactionData));
			}
		}

		theResult = jsonObjectData["interaction"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->interaction);
		}

		theResult = jsonObjectData["components"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->components.clear();
			ActionRowData newActionRowData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newActionRowData);
				this->components.emplace_back(std::move(newActionRowData));
			}
		}

		theResult = jsonObjectData["thread"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->thread);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, AutoModerationRuleVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theAutoModerationRules.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					AutoModerationRule newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theAutoModerationRules.push_back(newData);
				}
				this->theAutoModerationRules.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, Channel& theData) {
		this->id = getId(jsonObjectData, "id");

		this->flags = getUint8(jsonObjectData, "flags");

		this->type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->defaultAutoArchiveDuration = getUint32(jsonObjectData, "default_auto_archive_duration");

		this->position = getUint32(jsonObjectData, "position");

		simdjson::ondemand::value theArray{};
		auto theResult = jsonObjectData["permission_overwrites"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->permissionOverwrites.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				OverWriteData theDataNew{};
				auto& theObject = value.value();
				parseObject(theObject, theDataNew);
				this->permissionOverwrites.emplace_back(std::move(theDataNew));
			}
		}

		this->name = getString(jsonObjectData, "name");

		this->topic = getString(jsonObjectData, "topic");

		this->permissions = getString(jsonObjectData, "permissions");

		this->flags = setBool(this->flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		this->lastMessageId = getString(jsonObjectData, "last_message_id");

		this->bitrate = getUint32(jsonObjectData, "bitrate");

		this->userLimit = getUint32(jsonObjectData, "user_limit");

		this->rateLimitPerUser = getUint32(jsonObjectData, "rate_limit_per_user");

		theResult = jsonObjectData["recipients"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->recipients.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				UserData theDataNew{};
				auto& theObject = value.value();
				parseObject(theObject, theDataNew);
				this->recipients.emplace(theDataNew.id, std::move(theDataNew));
			}
		}

		this->icon = getString(jsonObjectData, "icon");

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->applicationId = getId(jsonObjectData, "application_id");

		this->parentId = getId(jsonObjectData, "parent_id");

		this->lastPinTimestamp = getString(jsonObjectData, "last_pin_timestamp");

		this->rtcRegion = getString(jsonObjectData, "rtc_region");

		this->videoQualityMode = getUint32(jsonObjectData, "video_quality_mode");

		this->messageCount = getUint32(jsonObjectData, "message_count");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["thread_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->threadMetadata);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ChannelVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theChannels.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Channel newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theChannels.push_back(newData);
				}
				this->theChannels.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, RoleTagsData& theData) {
		this->botId = getString(jsonObjectData, "bot_id");

		this->integrationId = getString(jsonObjectData, "integration_id");
	}




	template<> void parseObject(simdjson::ondemand::value jsonObjectData, PresenceUpdateFlags& theData) {
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



	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ActionMetaData& theData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->durationSeconds = getUint64(jsonObjectData, "duration_seconds");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, TeamObjectData& theData) {
		this->icon = getString(jsonObjectData, "icon");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->members.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				TeamMembersObjectData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->members.push_back(newData);
			}
			this->members.shrink_to_fit();
		}

		this->ownerUserId = getId(jsonObjectData, "owner_user_id");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, TeamMembersObjectData& theData) {
		this->membershipState = getUint32(jsonObjectData, "membership_state");

		this->teamId = std::to_string(getId(jsonObjectData, "team_id"));

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["permissions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->permissions.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->permissions.push_back(Permissions{ value.get_string().value().data() });
			}
			this->permissions.shrink_to_fit();
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->user);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, InstallParamsData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["scopes"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->scopes.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->scopes.push_back(Permissions{ value.get_string().value().data() });
			}
			this->scopes.shrink_to_fit();
		}

		this->permissions = getString(jsonObjectData, "name");
	}

	

	

	

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ApplicationCommandOptionData& theData) {
		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::object theMap{};
		auto theResult = jsonObjectData["name_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: theMap) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}

		theResult = jsonObjectData["description_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->descriptionLocalizations.clear();
			for (auto value: theMap) {
				this->descriptionLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}

		this->description = getString(jsonObjectData, "description");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["channel_types"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->channelTypes.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->channelTypes.push_back(static_cast<ChannelType>(value.get_uint64().value()));
			}
			this->channelTypes.shrink_to_fit();
		}

		this->minValue = getUint32(jsonObjectData, "min_value");

		this->maxValue = getUint32(jsonObjectData, "max_value");

		this->required = getBool(jsonObjectData, "required");

		this->autocomplete = getBool(jsonObjectData, "autocomplete");

		this->type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		if (this->type == ApplicationCommandOptionType::Sub_Command_Group || this->type == ApplicationCommandOptionType::Sub_Command) {
			auto theResult01 = jsonObjectData["options"].get(theArray);
			auto theResult02 = jsonObjectData["choices"].get(theArray);
			if (theResult01 == simdjson::error_code::SUCCESS) {
				this->options.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ApplicationCommandOptionData newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->options.push_back(newData);
				}
				this->options.shrink_to_fit();
			} else if (theResult02 == simdjson::error_code::SUCCESS) {
				this->choices.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ApplicationCommandOptionChoiceData newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->choices.push_back(newData);
				}
				this->choices.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ApplicationCommandOptionChoiceData& theData) {
		std::string_view theString{};
		auto theResult01 = jsonObjectData["value"].get(theString);
		uint64_t theInt{};
		auto theResult02 = jsonObjectData["value"].get(theInt);
		bool theBool{};
		auto theResult03 = jsonObjectData["value"].get(theBool);
		double theDouble{};
		auto theResult04 = jsonObjectData["value"].get(theDouble);

		if (theResult01 == simdjson::error_code::SUCCESS) {
			this->valueString = theString;
			this->valueStringReal = this->valueString.data();
			this->type = JsonType::String;
		} else if (theResult02 == simdjson::error_code::SUCCESS) {
			this->valueInt = theInt;
			this->type = JsonType::Integer;
		} else if (theResult03 == simdjson::error_code::SUCCESS) {
			this->type = JsonType::Boolean;
			this->valueBool = theBool;
		} else if (theResult04 == simdjson::error_code::SUCCESS) {
			this->type = JsonType::Float;
			this->valueFloat = theDouble;
		}

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::object theMap{};
		auto theResult = jsonObjectData["name_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: theMap) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, Sticker& theData) {
		this->asset = getString(jsonObjectData, "asset");

		this->description = getString(jsonObjectData, "description");

		this->formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		this->stickerFlags = setBool(this->stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->packId = getString(jsonObjectData, "pack_id");

		this->type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		this->sortValue = getUint32(jsonObjectData, "sort_value");

		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->user);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildScheduledEvent& theData) {
		this->privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		this->entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		this->status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["entity_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->entityMetadata);
		}

		this->scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		this->scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		this->userCount = getUint32(jsonObjectData, "user_count");

		this->description = getString(jsonObjectData, "description");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->creatorId = std::to_string(getId(jsonObjectData, "creator_id"));

		this->entityId = getString(jsonObjectData, "entity_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		theResult = jsonObjectData["creator"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->creator);
		}

		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, StageInstance& theData) {
		this->id = getId(jsonObjectData, "id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->topic = getString(jsonObjectData, "topic");

		this->privacyLevel = static_cast<StageInstancePrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		this->discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}



	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GatewayBotData& theData) {
		this->url = getString(jsonObjectData, "url");

		this->shards = getUint32(jsonObjectData, "shards");

		simdjson::ondemand::value theObject{};

		auto theResult = jsonObjectData["session_start_limit"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->sessionStartLimit);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, SessionStartData& theData) {
		this->maxConcurrency = getUint32(jsonObjectData, "max_concurrency");

		this->remaining = getUint32(jsonObjectData, "remaining");

		this->resetAfter = getUint32(jsonObjectData, "reset_after");

		this->total = getUint32(jsonObjectData, "total");
	}




	

	

	

	

	

	

	

	

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, AuditLogData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["webhooks"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->webhooks.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				WebHookData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->webhooks.push_back(newData);
			}
			this->webhooks.shrink_to_fit();
		}

		theResult = jsonObjectData["guild_scheduled_events"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->guildScheduledEvents.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				GuildScheduledEventData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->guildScheduledEvents.push_back(newData);
			}
			this->guildScheduledEvents.shrink_to_fit();
		}

		theResult = jsonObjectData["auto_moderation_rules"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->autoModerationRules.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				AutoModerationRuleData newData{};
				parseObject(theObject, newData);
				this->autoModerationRules.push_back(newData);
			}
			this->autoModerationRules.shrink_to_fit();
		}

		theResult = jsonObjectData["users"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->users.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				UserData newData{};
				parseObject(theObject, newData);
				this->users.push_back(newData);
			}
			this->users.shrink_to_fit();
		}

		theResult = jsonObjectData["audit_log_entries"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->auditLogEntries.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				AuditLogEntryData newData{};
				parseObject(theObject, newData);
				this->auditLogEntries.push_back(newData);
			}
			this->auditLogEntries.shrink_to_fit();
		}

		theResult = jsonObjectData["integrations"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->integrations.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				IntegrationData newData{};
				parseObject(theObject, newData);
				this->integrations.push_back(newData);
			}
			this->integrations.shrink_to_fit();
		}

		theResult = jsonObjectData["threads"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->threads.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				ChannelData newData{};
				parseObject(theObject, newData);
				this->threads.push_back(newData);
			}
			this->threads.shrink_to_fit();
		}
	}



	

	

	

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theGuilds.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Guild newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theGuilds.push_back(newData);
				}
				this->theGuilds.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildMemberVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theGuildMembers.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildMember newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theGuildMembers.push_back(newData);
				}
				this->theGuildMembers.shrink_to_fit();
			}
		}
	}

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildScheduledEventUserData& theData) {
		this->guildScheduledEventId = getString(jsonObjectData, "guild_scheduled_event_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->user);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, Message& theData) {
		this->content = getString(jsonObjectData, "content");

		this->id = getId(jsonObjectData, "id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["author"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->author);
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
		}

		this->timestamp = getString(jsonObjectData, "timestamp");

		this->editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		this->tts = getBool(jsonObjectData, "tts");

		this->mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["mentions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->mentions.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				UserData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->mentions.push_back(newData);
			}
			this->mentions.shrink_to_fit();
		}

		theResult = jsonObjectData["mention_roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->mentionRoles.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->mentionRoles.push_back(value.get_string().take_value().data());
			}
			this->mentionRoles.shrink_to_fit();
		}

		theResult = jsonObjectData["mention_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->mentionChannels.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ChannelMentionData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->mentionChannels.push_back(newData);
			}
			this->mentionChannels.shrink_to_fit();
		}

		theResult = jsonObjectData["attachments"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->attachments.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				AttachmentData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->attachments.push_back(newData);
			}
			this->attachments.shrink_to_fit();
		}

		theResult = jsonObjectData["embeds"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->embeds.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				EmbedData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->embeds.push_back(newData);
			}
			this->embeds.shrink_to_fit();
		}

		theResult = jsonObjectData["reactions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->reactions.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ReactionData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->reactions.push_back(newData);
			}
			this->reactions.shrink_to_fit();
		}

		this->nonce = getString(jsonObjectData, "nonce");

		this->pinned = getBool(jsonObjectData, "pinned");

		this->webHookId = getId(jsonObjectData, "webhook_id");

		this->type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		theResult = jsonObjectData["activity"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->activity);
		}

		theResult = jsonObjectData["application"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->application);
		}

		theResult = jsonObjectData["application"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->application);
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		theResult = jsonObjectData["message_reference"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->messageReference);
		}

		this->flags = getUint32(jsonObjectData, "flags");

		theResult = jsonObjectData["sticker_items"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->stickerItems.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerItemData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->stickerItems.push_back(newData);
			}
			this->stickerItems.shrink_to_fit();
		}

		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->stickers.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->stickers.push_back(newData);
			}
			this->stickers.shrink_to_fit();
		}

		theResult = jsonObjectData["interaction"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->interaction);
		}

		theResult = jsonObjectData["components"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->components.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ActionRowData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->components.push_back(newData);
			}
			this->components.shrink_to_fit();
		}

		theResult = jsonObjectData["thread"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->thread);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, MessageVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theMessages.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Message newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theMessages.push_back(newData);
				}
				this->theMessages.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, UserVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theUsers.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					User newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theUsers.push_back(newData);
				}
				this->theUsers.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, Reaction& theData) {
		this->count = getUint32(jsonObjectData, "count");

		this->me = getBool(jsonObjectData, "me");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["emoji"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->emoji);
		}

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->userId = getId(jsonObjectData, "user_id");

		this->messageId = getId(jsonObjectData, "message_id");

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ReactionVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theReactions.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Reaction newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theReactions.push_back(newData);
				}
				this->theReactions.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, RoleVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theRoles.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Role newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theRoles.push_back(newData);
				}
				this->theRoles.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, StickerVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theStickers.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Sticker newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theStickers.push_back(newData);
				}
				this->theStickers.shrink_to_fit();
			}
		}
	}

	struct DiscordCoreAPI_Dll MediaTranscoding {
		std::string thePreset{};
		std::string theUrl{};
	};

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, MediaTranscoding& theData) {
		this->thePreset = getString(jsonObjectData, "preset");

		this->theUrl = getString(jsonObjectData, "url");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, Song& theData) {
		try {
			this->duration = getString(getObject(getObject(getObject(jsonObjectData, "lengthText"), "accessibility"), "accessibilityData"), "label");
			std::string newString = getString(getObject(getArray(getObject(getObject(getArray(jsonObjectData, "detailedMetadataSnippets"), 0), "snippetText"), "runs"), 0), "text");
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->description = utf8MakeValid(newString);

			this->thumbnailUrl = getString(getObject(getArray(getObject(jsonObjectData, "thumbnail"), "thumbnails"), 0), "url");
			std::string newTitle01 = getString(getObject(getArray(getObject(jsonObjectData, "title"), "runs"), 0), "text");
			if (newTitle01.size() > 256) {
				newTitle01 = newTitle01.substr(0, 256);
			}
			if (newTitle01.size() > 0) {
				this->songTitle = utf8MakeValid(newTitle01);
			}
			std::string newTitle02 = getString(getObject(jsonObjectData, "title"), "simpleText");
			if (newTitle02.size() > 256) {
				newTitle02 = newTitle02.substr(0, 256);
			}
			if (newTitle02.size() > 0) {
				this->songTitle = utf8MakeValid(newTitle02);
			}

			if (newTitle01 != "") {
				this->songTitle = newTitle01;
			} else {
				this->songTitle = newTitle02;
			}

			this->songId = getString(jsonObjectData, "videoId");

			this->trackAuthorization = getString(jsonObjectData, "track_authorization");

			std::vector<MediaTranscoding> theMedia{};
			auto theArrayNew = getArray(getObject(jsonObjectData, "media"), "transcodings");
			if (theArrayNew.didItSucceed) {
				for (auto value: theArrayNew.theArray) {
					MediaTranscoding theDataNew{};
					parseObject(value.value(), theDataNew);
					theMedia.push_back(theDataNew);
				}
			}

			bool isItFound{ false };
			for (auto& value: theMedia) {
				if (value.thePreset == "opus_0_0") {
					isItFound = true;
					this->firstDownloadUrl = value.theUrl;
					this->songId = value.theUrl;
					this->doWeGetSaved = true;
				}
			}
			bool isItFound2{ false };
			if (!isItFound) {
				for (auto& value: theMedia) {
					if (value.thePreset == "mp3_0_0") {
						this->firstDownloadUrl = value.theUrl;
						this->songId = value.theUrl;
						isItFound2 = true;
					}
				}
			}
			if (!isItFound2 && !isItFound) {
				for (auto& value: theMedia) {
					this->firstDownloadUrl = value.theUrl;
					this->songId = value.theUrl;
				}
			}

			newString = getString(jsonObjectData, "title");
			if (newString.size() > 0) {
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				this->songTitle = utf8MakeValid(newString);
			}

			newString = getString(jsonObjectData, "description");
			if (newString.size() > 0) {
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				this->description = utf8MakeValid(newString);
				this->description += "...";
			}

			newString = getString(jsonObjectData, "artwork_url");
			if (newString.size() > 0) {
				this->thumbnailUrl = newString;
			}

			newString = getString(getObject(jsonObjectData, "user"), "avatar_url");
			if (newString.size() > 0) {
				this->thumbnailUrl = newString;
			}

			uint32_t theDuration = getUint32(jsonObjectData, "duration");
			if (theDuration != 0) {
				this->duration = TimeStamp<std::chrono::milliseconds>::convertMsToDurationString(theDuration);
			}

			newString = getString(jsonObjectData, "permalink_url");
			if (newString.size() > 0) {
				this->viewUrl = newString;
			}

		} catch (...) {
			reportException("parseObject()");
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, StickerPackDataVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theStickerPackDatas.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					StickerPackData newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theStickerPackDatas.push_back(newData);
				}
				this->theStickerPackDatas.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, StickerPackData& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->stickers.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->stickers.push_back(newData);
			}
			this->stickers.shrink_to_fit();
		}

		this->coverStickerId = getString(jsonObjectData, "cover_sticker_id");

		this->bannerAssetId = getString(jsonObjectData, "banner_asset_id");

		this->description = getString(jsonObjectData, "description");

		this->name = getString(jsonObjectData, "name");

		this->Id = getId(jsonObjectData, "id");

		this->skuId = getString(jsonObjectData, "sku_id");
	}



	template<> void parseObject(simdjson::ondemand::value jsonObjectData, Thread& theData) {
		std::unique_ptr<Channel> theChannel{ std::make_unique<Channel>() };
		parseObject(jsonObjectData, *theChannel);
		theData = *static_cast<Thread*>(theChannel.get());
	}

	

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ConnectionData& theData) {
		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		this->type = getString(jsonObjectData, "type");

		this->showActivity = getBool(jsonObjectData, "show_activity");

		this->friendSync = getBool(jsonObjectData, "friend_sync");

		this->revoked = getBool(jsonObjectData, "revoked");

		this->verified = getBool(jsonObjectData, "verified");

		this->visibility = static_cast<ConnectionVisibilityTypes>(getUint8(jsonObjectData, "visibility"));

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["integrations"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->integrations.reserve(theArray.count_elements().take_value());
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				IntegrationData newData{};
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->integrations.push_back(newData);
			}
			this->integrations.shrink_to_fit();
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, WebHook& theData) {
		this->id = getId(jsonObjectData, "id");

		this->type = static_cast<WebHookType>(getUint8(jsonObjectData, "type"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->user);
		}

		this->name = getString(jsonObjectData, "name");

		this->avatar = getString(jsonObjectData, "avatar");

		this->token = getString(jsonObjectData, "token");

		this->applicationId = getId(jsonObjectData, "application_id");

		theResult = jsonObjectData["source_guild"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->sourceGuild);
		}

		theResult = jsonObjectData["source_channel"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->sourceChannel);
		}

		this->url = getString(jsonObjectData, "url");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, WebHookVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theWebHooks.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					WebHook newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theWebHooks.push_back(newData);
				}
				this->theWebHooks.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, InteractionData& theData) {
		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["data"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->data);
		}

		this->appPermissions = getString(jsonObjectData, "app_permissions");

		this->type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		this->token = getString(jsonObjectData, "token");

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
			this->user.id = this->member.id;
			this->user.userName = this->member.getUserData().userName;
		}

		theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->user);
		}

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->locale = getString(jsonObjectData, "locale");

		this->guildLocale = getString(jsonObjectData, "guild_locale");

		theResult = jsonObjectData["message"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->message);
		}

		this->version = getUint32(jsonObjectData, "version");

		this->id = getId(jsonObjectData, "id");

		this->applicationId = getId(jsonObjectData, "application_id");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, InteractionDataData& theData) {
		std::string_view theObject{};
		auto theResult = jsonObjectData["id"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, this->applicationCommandData);
		}

		theResult = jsonObjectData["target_id"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, this->messageInteractionData);
			parseObject(jsonObjectData, this->userInteractionData);
		}

		uint64_t componentType{};
		theResult = jsonObjectData["component_type"].get(componentType);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, this->componentData);
		}

		simdjson::ondemand::value theObjectNew{};
		theResult = jsonObjectData["components"].get(theObjectNew);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(jsonObjectData, this->modalData);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ApplicationCommandInteractionData& theData) {
		this->type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["options"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ApplicationCommandInteractionDataOption newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);

				this->options.emplace_back(std::move(newData));
			}
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["resolved"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->resolved);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ApplicationCommandInteractionDataOption& theData) {
		this->type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		this->focused = getBool(jsonObjectData, "focused");

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["options"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ApplicationCommandInteractionDataOption newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);

				this->options.emplace_back(std::move(newData));
			}
		}

		this->value.theType = ObjectType::Null;
		bool theBool{};
		theResult = jsonObjectData["value"].get(theBool);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->value.theType = ObjectType::Boolean;
			this->value.theValue = std::to_string(theBool);
			return;
		}

		uint64_t theInteger{};
		theResult = jsonObjectData["value"].get(theInteger);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->value.theType = ObjectType::Number_Unsigned;
			this->value.theValue = std::to_string(theInteger);
			return;
		}

		std::string_view theString{};
		theResult = jsonObjectData["value"].get(theString);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->value.theType = ObjectType::String;
			this->value.theValue = theString;
			return;
		}

		double theFloat{};
		theResult = jsonObjectData["value"].get(theFloat);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->value.theType = ObjectType::Number_Float;
			this->value.theValue = std::to_string(theFloat);
			return;
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ResolvedData& theData) {
		simdjson::ondemand::object theArray{};
		auto theResult = jsonObjectData["attachments"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->attachments.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				AttachmentData newData{};
				auto theObject = value.value().value();
				parseObject(theObject, newData);
				this->attachments[DiscordCoreAPI::strtoull(value.key().take_value().raw())] = std::move(newData);
			}
		}

		theResult = jsonObjectData["users"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->users.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				UserData newData{};
				auto theObject = value.value().value();
				parseObject(theObject, newData);
				this->users[DiscordCoreAPI::strtoull(value.key().take_value().raw())] = std::move(newData);
			}
		}

		theResult = jsonObjectData["channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->channels.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				ChannelData newData{};
				auto theObject = value.value().value();
				parseObject(theObject, newData);
				this->channels[DiscordCoreAPI::strtoull(value.key().take_value().raw())] = std::move(newData);
			}
		}

		theResult = jsonObjectData["roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->roles.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				RoleData newData{};
				auto theObject = value.value().value();
				parseObject(theObject, newData);
				this->roles[DiscordCoreAPI::strtoull(value.key().take_value().raw())] = std::move(newData);
			}
		}

		theResult = jsonObjectData["members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->members.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				GuildMemberData newData{};
				auto theObject = value.value().value();
				parseObject(theObject, newData);
				this->members[DiscordCoreAPI::strtoull(value.key().take_value().raw())] = std::move(newData);
			}
		}

		theResult = jsonObjectData["messages"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->messages.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				MessageData newData{};
				auto theObject = value.value().value();
				parseObject(theObject, newData);
				this->messages[DiscordCoreAPI::strtoull(value.key().take_value().raw())] = std::move(newData);
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, AutoModerationActionExecutionEventData& theData) {
		this->alertSystemMessageId = getId(jsonObjectData, "alert_system_message_id");

		this->ruleTriggerType = static_cast<TriggerType>(getUint8(jsonObjectData, "rule_trigger_type"));

		this->matchedKeyword = getString(jsonObjectData, "matched_keyword");

		this->matchedContent = getString(jsonObjectData, "matched_content");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["action"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->action);
		}

		this->content = getString(jsonObjectData, "content");

		this->messageId = getId(jsonObjectData, "message_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->ruleId = getId(jsonObjectData, "rule_id");

		this->userId = getId(jsonObjectData, "user_id");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, YouTubeFormat& theData) {
		this->audioQuality = getString(jsonObjectData, "audioQuality");

		this->averageBitrate = getUint32(jsonObjectData, "averageBitrate");

		this->audioSampleRate = getString(jsonObjectData, "audioSampleRate");

		this->bitrate = getUint32(jsonObjectData, "bitrate");

		this->contentLength = strtoull(getString(jsonObjectData, "contentLength"));

		this->fps = getUint32(jsonObjectData, "fps");

		this->height = getUint32(jsonObjectData, "height");

		this->height = getUint32(jsonObjectData, "width");

		this->aitags = getString(jsonObjectData, "aitags");

		this->itag = getUint32(jsonObjectData, "itag");

		this->downloadUrl = getString(jsonObjectData, "url");

		this->mimeType = getString(jsonObjectData, "mimeType");

		this->quality = getString(jsonObjectData, "quality");

		std::string theString = getString(jsonObjectData, "signatureCipher");
		if (theString == "") {
			theString = getString(jsonObjectData, "cipher");
		}

		this->signatureCipher = theString;

		auto ampersandSpFind = this->signatureCipher.find("&sp");
		if (ampersandSpFind != std::string::npos) {
			this->signature = this->signatureCipher.substr(0, ampersandSpFind);
		}

		auto urlFind = this->signatureCipher.find("url");
		if (urlFind != std::string::npos) {
			this->downloadUrl = this->signatureCipher.substr(urlFind + 4);
		} else {
			this->downloadUrl = getString(jsonObjectData, "url");
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, YouTubeFormatVector& theData) {
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["streamingData"]["formats"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			YouTubeFormat newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->theFormats.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["streamingData"]["adaptiveFormats"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			YouTubeFormat newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->theFormats.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ReactionRemoveData& theData) {
		this->userId = getId(jsonObjectData, "user_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->messageId = getId(jsonObjectData, "message_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["emoji"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->emoji);
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ChannelPinsUpdateEventData& theData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->lastPinTimeStamp = getString(jsonObjectData, "last_pin_timestamp");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, TypingStartData& theData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			parseObject(theObject, this->member);
		}

		this->userId = getId(jsonObjectData, "user_id");

		this->timestamp = getUint32(jsonObjectData, "timestamp");
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ThreadListSyncData& theData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["channel_ids"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->channelIds.emplace_back(value.get_string().take_value().data());
			}
		}

		theResult = jsonObjectData["members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ThreadMemberData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->members.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["threads"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ChannelData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->threads.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, ThreadMembersUpdateData& theData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["added_members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ThreadMemberData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->addedMembers.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["added_members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			ThreadMemberData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->addedMembers.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["removed_member_ids"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->removedMemberIds.emplace_back(value.get_string().take_value().data());
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildEmojisUpdateEventData& theData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["emojis"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			EmojiData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->emojis.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildStickersUpdateEventData& theData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			StickerData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->stickers.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildMembersChunkEventData& theData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->nonce = getString(jsonObjectData, "nonce");

		this->chunkIndex = getUint32(jsonObjectData, "chunk_index");

		this->chunkCount = getUint32(jsonObjectData, "chunk_count");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["presences"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			PresenceUpdateData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->presences.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["not_found"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->notFound.emplace_back(value.get_string().take_value().data());
			}
		}

		theResult = jsonObjectData["members"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			GuildMemberData newData{};
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto& theObject = value.value();
				parseObject(theObject, newData);
				this->members.emplace_back(std::move(newData));
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildScheduledEventVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theGuildScheduledEvents.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildScheduledEvent newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theGuildScheduledEvents.push_back(newData);
				}
				this->theGuildScheduledEvents.shrink_to_fit();
			}
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, GuildScheduledEventDataVector& theData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			simdjson::error_code theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theGuildScheduledEventDatas.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildScheduledEventData newData{};
					auto& theObject = value.value();
					parseObject(theObject, newData);
					this->theGuildScheduledEventDatas.push_back(newData);
				}
				this->theGuildScheduledEventDatas.shrink_to_fit();
			}
		}
	}
};
