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
/// GuildEntities.hpp - Header for the Guild classes and structs.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file GuildEntities.cpp

#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/SongAPI.hpp>
#include <discordcoreapi/YouTubeAPI.hpp>
#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	CreateGuildData::operator JsonObject() {
		JsonObject theData{};
		theData["default_message_notifications"] = this->defaultMessageNotifications;
		theData["explicit_content_filter"] = this->explicitContentFilter;
		theData["system_channel_flags"] = this->systemChannelFlags;
		theData["verification_level"] = this->verificationLevel;
		theData["afk_timeout"] = this->afkTimeout;
		theData["region"] = this->region;
		theData["name"] = this->name;
		theData["icon"] = this->icon;
		for (auto& value: this->channels) {
			JsonObject newData{};
			newData["parent_id"] = std::to_string(value.parentId);
			newData["name"] = std::string{ value.name };
			newData["id"] = std::to_string(value.id);
			newData["type"] = value.type;
			theData.pushBack("channels", newData);
		}
		for (auto& value: this->roles) {
			JsonObject newData{};
			newData["permissions"] = static_cast<uint64_t>(value.permissions);
			newData["tags"]["premium_subscriber"] = value.tags.premiumSubscriber;
			newData["tags"]["integration_id"] = value.tags.integrationId;
			newData["mentionable"] = DiscordCoreAPI::getBool<int8_t, DiscordCoreAPI::RoleFlags>(value.flags, DiscordCoreAPI::RoleFlags::Mentionable);
			newData["tags"]["bot_id"] = value.tags.botId;
			newData["managed"] = DiscordCoreAPI::getBool<int8_t, DiscordCoreAPI::RoleFlags>(value.flags, DiscordCoreAPI::RoleFlags::Managed);
			newData["position"] = value.position;
			newData["hoist"] = DiscordCoreAPI::getBool<int8_t, DiscordCoreAPI::RoleFlags>(value.flags, DiscordCoreAPI::RoleFlags::Hoist);
			newData["color"] = value.color.getIntColorValue();
			newData["name"] = std::string{ value.name };
			theData.pushBack("roles", newData);
		}
		if (this->systemChannelId.operator size_t() != 0) {
			theData["system_channel_id"] = std::to_string(this->systemChannelId);
		}
		if (this->afkChannelId.operator size_t() != 0) {
			theData["afk_channel_id"] = std::to_string(this->afkChannelId);
		}
		return theData;
	}

	CreateGuildBanData::operator JsonObject() {
		JsonObject theData{};
		if (this->deleteMessageDays != 0) {
			theData["delete_message_days"] = this->deleteMessageDays;
		}
		return theData;
	}

	BeginGuildPruneData::operator JsonObject() {
		JsonObject theData{};
		theData["compute_prune_count"] = this->computePruneCount;
		for (auto& value: this->includeRoles) {
			theData.pushBack("include_roles", std::to_string(value));
		}
		theData["days"] = this->days;
		return theData;
	}

	ModifyGuildWelcomeScreenData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->welcomeChannels) {
			JsonObject newData{};
			newData["description"] = value.description;
			newData["channel_id"] = std::to_string(value.channelId);
			newData["emoji_name"] = value.emojiName;
			if (value.emojiId.operator size_t() != 0) {
				newData["emoji_id"] = std::to_string(value.emojiId);
			}
			theData.pushBack("welcome_channels", newData);
		}
		theData["description"] = this->description;
		theData["enabled"] = this->enabled;
		return theData;
	}

	GuildVector::GuildVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theGuilds.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Guild newData{ value.value() };
					this->theGuilds.push_back(std::move(newData));
				}
				this->theGuilds.shrink_to_fit();
			}
		}
	}

	Guild& Guild::operator=(GuildData&& other) noexcept {
		if (this != &other) {
			this->voiceConnectionPtr = other.voiceConnectionPtr;
			this->discordCoreClient = other.discordCoreClient;
			for (auto& value: other.channels) {
				Channel theData{};
				theData.id = value;
				this->channels.emplace_back(theData);
			}
			for (auto& value: other.members) {
				GuildMember theData{};
				theData.id = value;
				this->members.emplace_back(theData);
			}
			for (auto& value: other.roles) {
				Role theData{};
				theData.id = value;
				this->roles.emplace_back(theData);
			}
			for (auto& value: other.emoji) {
				EmojiData theData{};
				theData.id = value;
				this->emoji.emplace_back(theData);
			}
			for (auto& value: other.guildScheduledEvents) {
				GuildScheduledEvent theData{};
				theData.id = value;
				this->guildScheduledEvents.emplace_back(theData);
			}
			for (auto& value: other.presences) {
				PresenceUpdateData theData{};
				theData.userId = value.userId;
				this->presences.emplace(theData.userId, theData);
			}
			for (auto& value: other.stageInstances) {
				StageInstance theData{};
				theData.id = value;
				this->stageInstances.emplace_back(theData);
			}
			for (auto& value: other.stickers) {
				Sticker theData{};
				theData.id = value;
				this->stickers.emplace_back(theData);
			}
			for (auto& value: other.threads) {
				Thread theData{};
				theData.id = value;
				this->threads.emplace_back(theData);
			}
			this->joinedAt = std::move(other.joinedAt);
			this->ownerId = std::move(other.ownerId);
			this->memberCount = other.memberCount;
			this->icon = std::move(other.icon);
			this->name = std::move(other.name);
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	Guild::Guild(GuildData&& other) noexcept {
		*this = std::move(other);
	}

	Guild& Guild::operator=(const GuildData& other) noexcept {
		if (this != &other) {
			this->voiceConnectionPtr = other.voiceConnectionPtr;
			this->discordCoreClient = other.discordCoreClient;
			for (auto& value: other.channels) {
				Channel theData{};
				theData.id = value;
				this->channels.emplace_back(theData);
			}
			for (auto& value: other.members) {
				GuildMember theData{};
				theData.id = value;
				this->members.emplace_back(theData);
			}
			for (auto& value: other.roles) {
				Role theData{};
				theData.id = value;
				this->roles.emplace_back(theData);
			}
			for (auto& value: other.emoji) {
				EmojiData theData{};
				theData.id = value;
				this->emoji.emplace_back(theData);
			}
			for (auto& value: other.guildScheduledEvents) {
				GuildScheduledEvent theData{};
				theData.id = value;
				this->guildScheduledEvents.emplace_back(theData);
			}
			for (auto& value: other.presences) {
				PresenceUpdateData theData{};
				theData.userId = value.userId;
				this->presences.emplace(theData.userId, theData);
			}
			for (auto& value: other.stageInstances) {
				StageInstance theData{};
				theData.id = value;
				this->stageInstances.emplace_back(theData);
			}
			for (auto& value: other.stickers) {
				Sticker theData{};
				theData.id = value;
				this->stickers.emplace_back(theData);
			}
			for (auto& value: other.threads) {
				Thread theData{};
				theData.id = value;
				this->threads.emplace_back(theData);
			}
			this->memberCount = other.memberCount;
			this->joinedAt = other.joinedAt;
			this->ownerId = other.ownerId;
			this->flags = other.flags;
			this->icon = other.icon;
			this->name = other.name;
			this->id = other.id;
		}
		return *this;
	}

	Guild::Guild(const GuildData& other) noexcept {
		*this = other;
	}

	Guild::Guild(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->channels.clear();
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["threads"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				Channel newChannel{ value.value() };
				newChannel.guildId = this->id;
				this->threads.emplace_back(newChannel);
			}
		}


		theResult = jsonObjectData["stickers"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				Sticker newSticker{ value.value() };
				newSticker.guildId = this->id;
				this->stickers.emplace_back(newSticker);
			}
		}


		theResult = jsonObjectData["guild_scheduled_events"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				GuildScheduledEvent newGuildEvent{ value.value() };
				newGuildEvent.guildId = this->id;
				this->guildScheduledEvents.emplace_back(newGuildEvent);
			}
		}

		theResult = jsonObjectData["stage_instances"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StageInstance newStageInstance{ value.value() };
				newStageInstance.guildId = this->id;
				this->stageInstances.emplace_back(newStageInstance);
			}
		}


		theResult = jsonObjectData["emoji"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				EmojiData newEmojiData{ value.value() };
				this->emoji.emplace_back(newEmojiData);
			}
		}

		if (Roles::doWeCacheRoles) {
			this->roles.clear();
			theResult = jsonObjectData["roles"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					RoleData newData{ value.value() };
					newData.guildId = this->id;
					this->roles.emplace_back(newData);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			this->members.clear();
			theResult = jsonObjectData["members"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildMemberData newData{ value.value() };
					newData.guildId = this->id;
					this->members.emplace_back(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			theResult = jsonObjectData["voice_states"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					VoiceStateData theData{ value.value() };
					GuildMemberData theDataNew{};
					theDataNew.id = theData.userId;
					theDataNew.guildId = this->id;
					if (GuildMembers::cache.contains(theDataNew)) {
						GuildMembers::cache.at(theDataNew).voiceChannelId = theData.channelId;
					}
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers) {
			this->presences.clear();
			theResult = jsonObjectData["presences"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					PresenceUpdateData newData{ value.value() };
					auto userId = newData.userId;
					newData.guildId = this->id;
					this->presences.emplace(userId, std::move(newData));
				}
			}
		}

		if (Channels::doWeCacheChannels) {
			this->channels.clear();
			theResult = jsonObjectData["channels"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ChannelData newData{ value.value() };
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
			this->welcomeScreen = WelcomeScreenData{ theObject };
		}

		this->nsfwLevel = static_cast<GuildNSFWLevel>(getUint8(jsonObjectData, "nsfw_level"));
	}

	std::string Guild::getDiscoverySplashUrl() noexcept {
		std::string theStringNew{ "https://cdn.discordapp.com/" };
		theStringNew += "discovery-splashes/" + std::to_string(this->id) + "/" + this->discoverySplash.getIconHash() + ".png";
		return theStringNew;
	}

	std::string Guild::getBannerUrl() noexcept {
		std::string theStringNew{ "https://cdn.discordapp.com/" };
		theStringNew += "banners/" + std::to_string(this->id) + "/" + this->banner.getIconHash() + ".png";
		return theStringNew;
	}

	std::string Guild::getSplashUrl() noexcept {
		std::string theStringNew{ "https://cdn.discordapp.com/" };
		theStringNew += "splashes/" + std::to_string(this->id) + "/" + this->splash.getIconHash() + ".png";
		return theStringNew;
	}

	GuildVector::operator std::vector<Guild>() {
		return this->theGuilds;
	}

	ModifyGuildData::ModifyGuildData(Guild dataPackage) {
		this->premiumProgressBarEnabled = getBool<int8_t, GuildFlags>(dataPackage.flags, GuildFlags::Premium_Progress_Bar_Enabled);
		this->defaultMessageNotifications = dataPackage.defaultMessageNotifications;
		this->publicUpdatesChannelId = dataPackage.publicUpdatesChannelId;
		this->explicitContentFilter = dataPackage.explicitContentFilter;
		this->systemChannelFlags = dataPackage.systemChannelFlags;
		this->verificationLevel = dataPackage.verificationLevel;
		this->preferredLocale = dataPackage.preferredLocale;
		this->systemChannelId = dataPackage.systemChannelId;
		this->rulesChannelId = dataPackage.rulesChannelId;
		this->afkChannelId = dataPackage.afkChannelId;
		this->description = dataPackage.description;
		this->afkTimeout = dataPackage.afkTimeOut;
		for (auto& value: dataPackage.features) {
			this->features.emplace_back(value);
		}
		this->ownerId = dataPackage.ownerId;
		this->guildId = dataPackage.id;
		this->name = dataPackage.name;
	}

	ModifyGuildData::operator JsonObject() {
		JsonObject theData{};
		theData["premium_progress_bar_enabled"] = this->premiumProgressBarEnabled;
		theData["default_message_notifications"] = this->defaultMessageNotifications;
		theData["explicit_content_filter"] = this->explicitContentFilter;
		theData["system_channel_flags"] = this->systemChannelFlags;
		theData["verification_level"] = this->verificationLevel;
		theData["discovery_splash"] = this->discoverySplash;
		theData["preferred_locale"] = std::string{ this->preferredLocale };
		theData["rules_channel_id"] = std::to_string(this->rulesChannelId);
		theData["description"] = std::string{ this->description };
		theData["afk_timeout"] = this->afkTimeout;
		theData["features"] = this->features;
		theData["splash"] = this->splash;
		theData["banner"] = this->banner;
		theData["name"] = std::string{ this->name };
		theData["icon"] = this->icon;
		if (this->publicUpdatesChannelId.operator size_t() != 0) {
			theData["public_updates_channel_id"] = std::to_string(this->publicUpdatesChannelId);
		}
		if (this->systemChannelId.operator size_t() != 0) {
			theData["system_channel_id"] = std::to_string(this->systemChannelId);
		}
		if (this->afkChannelId.operator size_t() != 0) {
			theData["afk_channel_id"] = std::to_string(this->afkChannelId);
		}
		if (this->ownerId.operator size_t() != 0) {
			theData["owner_id"] = std::to_string(this->ownerId);
		}
		return theData;
	}

	void Guilds::initialize(DiscordCoreInternal::HttpsClient* theClient, DiscordCoreClient* discordCoreClientNew, ConfigManager* configManagerNew) {
		Guilds::doWeCacheGuilds = configManagerNew->doWeCacheGuilds();
		Guilds::discordCoreClient = discordCoreClientNew;
		Guilds::httpsClient = theClient;
	}

	CoRoutine<AuditLogData> Guilds::getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Audit_Logs };
		co_await NewThreadAwaitable<AuditLogData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/audit-logs";
		if (dataPackage.userId.operator size_t() != 0) {
			workload.relativePath += "?user_id=" + std::to_string(dataPackage.userId);
			if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
				workload.relativePath += "&action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
			if (dataPackage.before.operator size_t() != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
		} else if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
			workload.relativePath += "?action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
			if (dataPackage.before.operator size_t() != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			if (dataPackage.before.operator size_t() != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
		} else if (dataPackage.before.operator size_t() != 0) {
			workload.relativePath += "?before=" + std::to_string(dataPackage.before);
		}
		workload.callStack = "Guilds::getAuditLogDataAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<AuditLogData>(workload);
	}

	CoRoutine<Guild> Guilds::createGuildAsync(CreateGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds";
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Guilds::createGuildAsync()";
		auto theData = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload);
		theData.discordCoreClient = Guilds::discordCoreClient;
		co_return theData;
	}

	CoRoutine<std::vector<GuildData>> Guilds::getAllGuildsAsync() {
		co_await NewThreadAwaitable<std::vector<GuildData>>();
		GuildDataVector guildVector{};
		for (auto value: Guilds::cache) {
			value.discordCoreClient = Guilds::discordCoreClient;
			guildVector.theGuildDatas.emplace_back(std::move(value));
		}
		co_return guildVector;
	}

	CoRoutine<Guild> Guilds::getGuildAsync(GetGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "?with_counts=true";
		workload.callStack = "Guilds::getGuildAsync()";
		GuildData theData{};
		theData.id = dataPackage.guildId;
		if (!Guilds::cache.contains(theData)) {
			Guilds::cache.emplace(theData);
		}
		Guild theDataNew{};
		theDataNew = Guilds::cache.at(theData);
		theDataNew = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload, &theDataNew);
		theDataNew.discordCoreClient = Guilds::discordCoreClient;
		Guilds::insertGuild(theDataNew);
		co_return theDataNew;
	}

	CoRoutine<GuildData> Guilds::getCachedGuildAsync(GetGuildData dataPackage) {
		co_await NewThreadAwaitable<GuildData>();
		Guild theData{};
		theData.id = dataPackage.guildId;
		if (!Guilds::cache.contains(theData)) {
			auto theGuild = Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
			theGuild.discordCoreClient = Guilds::discordCoreClient;
			co_return theGuild;
		} else {
			GuildData theGuild = Guilds::cache.at(theData);
			theGuild.discordCoreClient = Guilds::discordCoreClient;
			co_return theGuild;
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Preview };
		co_await NewThreadAwaitable<GuildPreviewData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/preview";
		workload.callStack = "Guilds::getGuildPreviewAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPreviewData>(workload);
	}

	CoRoutine<Guild> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId);
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Guilds::modifyGuildAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildData theData{};
		theData.id = dataPackage.guildId;
		if (!Guilds::cache.contains(theData)) {
			Guilds::cache.emplace(theData);
		}
		Guild theDataNew{};
		theDataNew = Guilds::cache.at(theData);
		theDataNew = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload, &theDataNew);
		theDataNew.discordCoreClient = Guilds::discordCoreClient;
		Guilds::insertGuild(theDataNew);
		co_return theDataNew;
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId);
		workload.callStack = "Guilds::deleteGuildAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<BanData>> Guilds::getGuildBansAsync(GetGuildBansData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Bans };
		co_await NewThreadAwaitable<std::vector<BanData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans";
		if (dataPackage.after.operator size_t() != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
			if (dataPackage.before.operator size_t() != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.before.operator size_t() != 0) {
			workload.relativePath += "?before=" + std::to_string(dataPackage.before);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Guilds::getGuildBansAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<BanDataVector>(workload);
	}

	CoRoutine<BanData> Guilds::getGuildBanAsync(GetGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Ban };
		co_await NewThreadAwaitable<BanData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.userId);
		workload.callStack = "Guilds::getGuildBanAsync()";
		BanData theData{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<BanData>(workload, &theData);
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Ban };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.guildMemberId);
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Guilds::createGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Guilds::removeGuildBanAsync(RemoveGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Ban };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.userId);
		workload.callStack = "Guilds::removeGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildPruneCountData> Guilds::getGuildPruneCountAsync(GetGuildPruneCountData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Prune_Count };
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/prune";
		workload.callStack = "Guilds::getGuildPruneCountAsync()";
		if (dataPackage.days != 0) {
			workload.relativePath += "?days=" + std::to_string(dataPackage.days);
			if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "&include_roles=";
				for (int32_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
					workload.relativePath += std::to_string(dataPackage.includeRoles[x]);
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
		} else if (dataPackage.includeRoles.size() > 0) {
			workload.relativePath += "?include_roles=";
			for (int32_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
				workload.relativePath += std::to_string(dataPackage.includeRoles[x]);
				if (x < dataPackage.includeRoles.size() - 1) {
					workload.relativePath += ",";
				}
			}
		}
		GuildPruneCountData theData{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload);
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Prune };
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/prune";
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Guilds::beginGuildPruneAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload);
	}

	CoRoutine<std::vector<VoiceRegionData>> Guilds::getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Voice_Regions };
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/regions";
		workload.callStack = "Guilds::getGuildVoiceRegionsAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<VoiceRegionDataVector>(workload);
	}

	CoRoutine<std::vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Invites };
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/invites";
		workload.callStack = "Guilds::getGuildInvitesAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteDataVector>(workload);
	}

	CoRoutine<std::vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Integrations };
		co_await NewThreadAwaitable<std::vector<IntegrationData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/integrations";
		workload.callStack = "Guilds::getGuildIntegrationsAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<IntegrationDataVector>(workload);
	}

	CoRoutine<void> Guilds::deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Integration };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/integrations/" + std::to_string(dataPackage.integrationId);
		workload.callStack = "Guilds::deleteGuildIntegrationAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Settings };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget";
		workload.callStack = "Guilds::getGuildWidgetSettingsAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget";
		JsonObject responseData{};
		responseData["channel_id"] = std::to_string(dataPackage.widgetData.channelId);
		responseData["enabled"] = dataPackage.widgetData.enabled;
		workload.content = responseData;
		workload.callStack = "Guilds::modifyGuildWidgetAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget.json";
		workload.callStack = "Guilds::getGuildWidgetAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Vanity_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/vanity-url";
		workload.callStack = "Guilds::getGuildVanityInviteAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteData>(workload);
	}

	CoRoutine<GuildWidgetImageData> Guilds::getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Image };
		co_await NewThreadAwaitable<GuildWidgetImageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget.png";
		switch (dataPackage.widgetStlye) {
			case WidgetStyleOptions::Shield: {
				workload.relativePath += "?style=shield";
				break;
			}
			case WidgetStyleOptions::Banner1: {
				workload.relativePath += "?style=banner1";
				break;
			}
			case WidgetStyleOptions::Banner2: {
				workload.relativePath += "?style=banner2";
				break;
			}
			case WidgetStyleOptions::Banner3: {
				workload.relativePath += "?style=banner3";
				break;
			}
			case WidgetStyleOptions::Banner4: {
				workload.relativePath += "?style=banner4";
				break;
			}
		}
		workload.callStack = "Guilds::getGuildWidgetImageAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetImageData>(workload);
	}

	CoRoutine<WelcomeScreenData> Guilds::getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/welcome-screen";
		workload.callStack = "Guilds::getGuildWelcomeScreenAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload);
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/welcome-screen";
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Guilds::modifyGuildWelcomeScreenAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload);
	};

	CoRoutine<GuildTemplateData> Guilds::getGuildTemplateAsync(GetGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::getGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<Guild> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_From_Guild_Template };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		JsonObject responseData{};
		responseData["name"] = dataPackage.name;
		responseData["icon"] = dataPackage.imageData;
		workload.content = responseData;
		workload.callStack = "Guilds::createGuildFromGuildTemplateAsync()";
		auto newGuild = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload);
		newGuild.discordCoreClient = Guilds::discordCoreClient;
		co_return newGuild;
	}

	CoRoutine<std::vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Templates };
		co_await NewThreadAwaitable<std::vector<GuildTemplateData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates";
		workload.callStack = "Guilds::getGuildTemplatesAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateDataVector>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates";
		JsonObject responseData{};
		responseData["description"] = dataPackage.description;
		responseData["name"] = dataPackage.name;
		workload.content = responseData;
		workload.callStack = "Guilds::createGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::syncGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		JsonObject responseData{};
		responseData["description"] = dataPackage.description;
		responseData["name"] = dataPackage.name;
		workload.content = responseData;
		workload.callStack = "Guilds::modifyGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Template };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::deleteGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<InviteData> Guilds::getInviteAsync(GetInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/invites/" + dataPackage.inviteId;
		if (dataPackage.withCount) {
			workload.relativePath += "?with_counts=true";
			if (dataPackage.withExpiration) {
				workload.relativePath += "&with_expiration=true";
			}
			if (dataPackage.guildScheduledEventId.operator size_t() != 0) {
				workload.relativePath += "&guild_scheduled_event_id=" + std::to_string(dataPackage.guildScheduledEventId);
			}
		} else if (dataPackage.withExpiration) {
			workload.relativePath += "?with_expiration=true";
			if (dataPackage.guildScheduledEventId.operator size_t() != 0) {
				workload.relativePath += "&guild_scheduled_event_id=" + std::to_string(dataPackage.guildScheduledEventId);
			}
		} else if (dataPackage.guildScheduledEventId.operator size_t() != 0) {
			workload.relativePath += "?guild_scheduled_event_id=" + std::to_string(dataPackage.guildScheduledEventId);
		}

		workload.callStack = "Guilds::getInviteAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteData>(workload);
	}

	CoRoutine<void> Guilds::deleteInviteAsync(DeleteInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Invite };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/invites/" + dataPackage.inviteId;
		workload.callStack = "Guilds::deleteInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Guild>> Guilds::getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Current_User_Guilds };
		co_await NewThreadAwaitable<std::vector<Guild>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/@me/guilds";
		if (dataPackage.after.operator size_t() != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
			if (dataPackage.before.operator size_t() != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.before.operator size_t() != 0) {
			workload.relativePath += "?before=" + std::to_string(dataPackage.before);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Users::getCurrentUserGuildsAsync()";
		auto guildVector = Guilds::httpsClient->submitWorkloadAndGetResult<GuildVector>(workload);
		for (auto& value: guildVector.theGuilds) {
			value.discordCoreClient = Guilds::discordCoreClient;
		}
		co_return guildVector;
	}

	CoRoutine<void> Guilds::leaveGuildAsync(LeaveGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Leave_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/users/@me/guilds/" + std::to_string(dataPackage.guildId);
		workload.callStack = "Guilds::leaveGuildAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	StopWatch theStopWatch{ 5s };
	void Guilds::insertGuild(GuildData guild) {
		if (guild.id.operator size_t() == 0) {
			return;
		}
		if (Guilds::doWeCacheGuilds) {
			guild.discordCoreClient = Guilds::discordCoreClient;
			Guilds::cache.emplace(std::move(guild));
			if (Guilds::cache.size() % 100 == 0) {
				std::cout << "THE GUILD COUNT: " << Guilds::cache.size() << ", TOTAL TIME: " << theStopWatch.totalTimePassed() << std::endl;
			}
		}
	}

	void Guilds::removeGuild(const Snowflake guildId) {
		GuildData theData{};
		theData.id = guildId;
		Guilds::cache.erase(theData);
	};

	DiscordCoreInternal::HttpsClient* Guilds::httpsClient{ nullptr };
	DiscordCoreClient* Guilds::discordCoreClient{ nullptr };
	TSUnorderedSet<GuildData> Guilds::cache{};
	bool Guilds::doWeCacheGuilds{ false };

}