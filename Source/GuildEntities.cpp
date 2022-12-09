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

	CreateGuildData::operator Jsonifier() {
		Jsonifier data{};
		data["default_message_notifications"] = this->defaultMessageNotifications;
		data["explicit_content_filter"] = this->explicitContentFilter;
		data["system_channel_flags"] = this->systemChannelFlags;
		data["verification_level"] = this->verificationLevel;
		data["afk_timeout"] = this->afkTimeout;
		data["region"] = this->region;
		data["name"] = this->name;
		data["icon"] = this->icon;
		for (auto& value: this->channels) {
			Jsonifier newData{};
			newData["parent_id"] = value.parentId;
			newData["name"] = std::string{ value.name };
			newData["id"] = value.id;
			newData["type"] = value.type;
			data["channels"].emplaceBack(newData);
		}
		for (auto& value: this->roles) {
			Jsonifier newData{};
			newData["permissions"] = static_cast<uint64_t>(value.permissions);
			newData["tags"]["premium_subscriber"] = value.tags.premiumSubscriber;
			newData["tags"]["integration_id"] = value.tags.integrationId;
			newData["mentionable"] = getBool<RoleFlags>(value.flags, RoleFlags::Mentionable);
			newData["tags"]["bot_id"] = value.tags.botId;
			newData["managed"] = getBool<RoleFlags>(value.flags, RoleFlags::Managed);
			newData["position"] = value.position;
			newData["hoist"] = getBool<RoleFlags>(value.flags, RoleFlags::Hoist);
			newData["color"] = value.color.getIntColorValue();
			newData["name"] = std::string{ value.name };
			data["roles"].emplaceBack(newData);
		}
		if (this->systemChannelId != 0) {
			data["system_channel_id"] = this->systemChannelId;
		}
		if (this->afkChannelId != 0) {
			data["afk_channel_id"] = this->afkChannelId;
		}
		return data;
	}

	CreateGuildBanData::operator Jsonifier() {
		Jsonifier data{};
		if (this->deleteMessageDays != 0) {
			data["delete_message_days"] = this->deleteMessageDays;
		}
		return data;
	}

	BeginGuildPruneData::operator Jsonifier() {
		Jsonifier data{};
		data["compute_prune_count"] = this->computePruneCount;
		for (auto& value: this->includeRoles) {
			data["include_roles"].emplaceBack(value.operator std::string());
		}
		data["days"] = this->days;
		return data;
	}

	ModifyGuildWelcomeScreenData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->welcomeChannels) {
			Jsonifier newData{};
			newData["description"] = value.description;
			newData["channel_id"] = value.channelId;
			newData["emoji_name"] = value.emojiName;
			if (value.emojiId != 0) {
				newData["emoji_id"] = value.emojiId;
			}
			data["welcome_channels"].emplaceBack(newData);
		}
		data["description"] = this->description;
		data["enabled"] = this->enabled;
		return data;
	}

	GuildVector::GuildVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					Guild newData{ value.value() };
					this->guilds.emplace_back(std::move(newData));
				}
			}
		}
	}

	Guild& Guild::operator=(GuildData&& other) noexcept {
		if (this != &other) {
			this->voiceConnectionPtr = other.voiceConnectionPtr;
			this->discordCoreClient = other.discordCoreClient;
			for (auto& value: other.channels) {
				Channel data{};
				data.id = value;
				this->channels.emplace_back(std::move(data));
			}
			for (auto& value: other.members) {
				GuildMember data{};
				data.id = value;
				this->members.emplace_back(std::move(data));
			}
			for (auto& value: other.roles) {
				Role data{};
				data.id = value;
				this->roles.emplace_back(std::move(data));
			}
			for (auto& value: other.emoji) {
				EmojiData data{};
				data.id = value;
				this->emoji.emplace_back(std::move(data));
			}
			for (auto& value: other.guildScheduledEvents) {
				GuildScheduledEvent data{};
				data.id = value;
				this->guildScheduledEvents.emplace_back(std::move(data));
			}
			for (auto& value: other.presences) {
				PresenceUpdateData data{};
				data.userId = value.userId;
				this->presences.emplace(data.userId, std::move(data));
			}
			for (auto& value: other.stageInstances) {
				StageInstance data{};
				data.id = value;
				this->stageInstances.emplace_back(std::move(data));
			}
			for (auto& value: other.stickers) {
				Sticker data{};
				data.id = value;
				this->stickers.emplace_back(std::move(data));
			}
			for (auto& value: other.threads) {
				Thread data{};
				data.id = value;
				this->threads.emplace_back(std::move(data));
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
				Channel data{};
				data.id = value;
				this->channels.emplace_back(data);
			}
			for (auto& value: other.members) {
				GuildMember data{};
				data.id = value;
				this->members.emplace_back(data);
			}
			for (auto& value: other.roles) {
				Role data{};
				data.id = value;
				this->roles.emplace_back(data);
			}
			for (auto& value: other.emoji) {
				EmojiData data{};
				data.id = value;
				this->emoji.emplace_back(data);
			}
			for (auto& value: other.guildScheduledEvents) {
				GuildScheduledEvent data{};
				data.id = value;
				this->guildScheduledEvents.emplace_back(data);
			}
			for (auto& value: other.presences) {
				PresenceUpdateData data{};
				data.userId = value.userId;
				this->presences.emplace(data.userId, data);
			}
			for (auto& value: other.stageInstances) {
				StageInstance data{};
				data.id = value;
				this->stageInstances.emplace_back(data);
			}
			for (auto& value: other.stickers) {
				Sticker data{};
				data.id = value;
				this->stickers.emplace_back(data);
			}
			for (auto& value: other.threads) {
				Thread data{};
				data.id = value;
				this->threads.emplace_back(data);
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
		simdjson::ondemand::array arrayValue{};

		if (getArray(arrayValue, "threads", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				Channel newChannel{ value.value() };
				newChannel.guildId = this->id;
				this->threads.emplace_back(newChannel);
			}
		}

		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				Sticker newSticker{ value.value() };
				newSticker.guildId = this->id;
				this->stickers.emplace_back(newSticker);
			}
		}

		if (getArray(arrayValue, "guild_scheduled_events", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				GuildScheduledEvent newGuildEvent{ value.value() };
				newGuildEvent.guildId = this->id;
				this->guildScheduledEvents.emplace_back(newGuildEvent);
			}
		}

		if (getArray(arrayValue, "stage_instances", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StageInstance newStageInstance{ value.value() };
				newStageInstance.guildId = this->id;
				this->stageInstances.emplace_back(newStageInstance);
			}
		}

		if (getArray(arrayValue, "emoji", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				EmojiData newEmojiData{ value.value() };
				this->emoji.emplace_back(newEmojiData);
			}
		}

		if (Roles::doWeCacheRoles()) {
			this->roles.clear();
			if (getArray(arrayValue, "roles", jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					RoleData newData{ value.value() };
					newData.guildId = this->id;
					this->roles.emplace_back(newData);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers()) {
			this->members.clear();
			if (getArray(arrayValue, "members", jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildMemberData newData{ value.value() };
					newData.guildId = this->id;
					this->members.emplace_back(std::move(newData));
				}
			}
		}

		if (getArray(arrayValue, "voice_states", jsonObjectData)) {
			for (auto value: arrayValue) {
				VoiceStateDataLight dataNew{ value.value() };
				dataNew.guildId = this->id;
				GuildMembers::vsCache[dataNew] = dataNew;
			}
		}

		if (GuildMembers::doWeCacheGuildMembers()) {
			this->presences.clear();
			if (getArray(arrayValue, "presences", jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					PresenceUpdateData newData{ value.value() };
					auto userId = newData.userId;
					newData.guildId = this->id;
					this->presences.emplace(userId, std::move(newData));
				}
			}
		}

		if (Channels::doWeCacheChannels()) {
			this->channels.clear();
			if (getArray(arrayValue, "channels", jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
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

		uint8_t newFlags{};
		newFlags = setBool(newFlags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

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

		if (getArray(arrayValue, "features", jsonObjectData)) {
			this->features.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->features.emplace_back(std::string{ value.get_string().take_value() });
			}
		}

		this->permissions = getString(jsonObjectData, "permissions");

		this->afkTimeOut = static_cast<AfkTimeOutDurations>(getUint8(jsonObjectData, "afk_timeout"));

		newFlags = setBool(newFlags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		newFlags = setBool(newFlags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));

		this->verificationLevel = static_cast<VerificationLevel>(getUint8(jsonObjectData, "verification_level"));

		this->defaultMessageNotifications =
			static_cast<DefaultMessageNotificationLevel>(getUint8(jsonObjectData, "default_message_notification_level"));

		this->explicitContentFilter = static_cast<ExplicitContentFilterLevel>(getUint8(jsonObjectData, "explicit_content_filter_level"));

		this->mfaLevel = static_cast<MFALevel>(getUint8(jsonObjectData, "mfa_level"));

		this->systemChannelFlags = static_cast<SystemChannelFlags>(getUint8(jsonObjectData, "system_channel_flags"));

		newFlags = setBool(newFlags, GuildFlags::Large, getBool(jsonObjectData, "large"));

		newFlags = setBool(newFlags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		this->flags = static_cast<GuildFlags>(newFlags);

		this->memberCount = getUint32(jsonObjectData, "member_count");

		this->maxPresences = getUint32(jsonObjectData, "max_presences");

		this->maxMembers = getUint32(jsonObjectData, "max_members");

		this->premiumSubscriptionCount = getUint32(jsonObjectData, "premium_subscription_count");

		this->premiumTier = static_cast<PremiumTier>(getUint8(jsonObjectData, "premium_tier"));

		this->maxVideoChannelUsers = getUint32(jsonObjectData, "max_video_channel_users");

		this->approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		this->approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		simdjson::ondemand::value object{};
		if (getObject(object, "welcome_screen", jsonObjectData)) {
			this->welcomeScreen = WelcomeScreenData{ object };
		}

		this->nsfwLevel = static_cast<GuildNSFWLevel>(getUint8(jsonObjectData, "nsfw_level"));
	}

	std::string Guild::getDiscoverySplashUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "discovery-splashes/" + this->id + "/" + this->discoverySplash.getIconHash() + ".png";
		return stringNew;
	}

	std::string Guild::getBannerUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "banners/" + this->id + "/" + this->banner.getIconHash() + ".png";
		return stringNew;
	}

	std::string Guild::getSplashUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "splashes/" + this->id + "/" + this->splash.getIconHash() + ".png";
		return stringNew;
	}

	GuildVector::operator std::vector<Guild>() {
		return this->guilds;
	}

	ModifyGuildData::ModifyGuildData(Guild dataPackage) {
		this->premiumProgressBarEnabled = getBool<GuildFlags>(dataPackage.flags, GuildFlags::Premium_Progress_Bar_Enabled);
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

	ModifyGuildData::operator Jsonifier() {
		Jsonifier data{};
		data["premium_progress_bar_enabled"] = this->premiumProgressBarEnabled;
		data["default_message_notifications"] = this->defaultMessageNotifications;
		data["explicit_content_filter"] = this->explicitContentFilter;
		data["system_channel_flags"] = this->systemChannelFlags;
		data["verification_level"] = this->verificationLevel;
		data["discovery_splash"] = this->discoverySplash;
		data["preferred_locale"] = std::string{ this->preferredLocale };
		data["rules_channel_id"] = this->rulesChannelId;
		data["description"] = std::string{ this->description };
		data["afk_timeout"] = this->afkTimeout;
		data["features"] = this->features;
		data["splash"] = this->splash;
		data["banner"] = this->banner;
		data["name"] = std::string{ this->name };
		data["icon"] = this->icon;
		if (this->publicUpdatesChannelId != 0) {
			data["public_updates_channel_id"] = this->publicUpdatesChannelId;
		}
		if (this->systemChannelId != 0) {
			data["system_channel_id"] = this->systemChannelId;
		}
		if (this->afkChannelId != 0) {
			data["afk_channel_id"] = this->afkChannelId;
		}
		if (this->ownerId != 0) {
			data["owner_id"] = this->ownerId;
		}
		return data;
	}

	void Guilds::initialize(DiscordCoreInternal::HttpsClient* client, DiscordCoreClient* discordCoreClientNew, ConfigManager* configManagerNew) {
		Guilds::doWeCacheGuildsBool = configManagerNew->doWeCacheGuilds();
		Guilds::discordCoreClient = discordCoreClientNew;
		Guilds::httpsClient = client;
	}

	CoRoutine<AuditLogData> Guilds::getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Audit_Logs };
		co_await NewThreadAwaitable<AuditLogData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/audit-logs";
		if (dataPackage.userId != 0) {
			workload.relativePath += "?user_id=" + dataPackage.userId;
			if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
				workload.relativePath += "&action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
		} else if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
			workload.relativePath += "?action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
		} else if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + dataPackage.before;
		}
		workload.callStack = "Guilds::getAuditLogDataAsync()";
		AuditLogData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<AuditLogData>(workload, &returnValue);
	}

	CoRoutine<Guild> Guilds::createGuildAsync(CreateGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Guilds::createGuildAsync()";
		Guild returnValue{};
		auto data = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload, &returnValue);
		data.discordCoreClient = Guilds::discordCoreClient;
		co_return std::move(data);
	}

	CoRoutine<std::vector<GuildData>> Guilds::getAllGuildsAsync() {
		co_await NewThreadAwaitable<std::vector<GuildData>>();
		GuildDataVector guildVector{};
		for (auto value: Guilds::getCache()) {
			value.discordCoreClient = Guilds::discordCoreClient;
			guildVector.guilds.emplace_back(std::move(value));
		}
		co_return guildVector;
	}

	CoRoutine<Guild> Guilds::getGuildAsync(GetGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "?with_counts=true";
		workload.callStack = "Guilds::getGuildAsync()";
		Guild data{};
		data = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload, &data);
		if (Guilds::getCache().contains(data)) {
			data = Guilds::getCache().at(data);
		} else {
			Guilds::insertGuild(data);
		}
		data.discordCoreClient = Guilds::discordCoreClient;
		co_return std::move(data);
	}

	GuildData Guilds::getCachedGuild(GetGuildData dataPackage) {
		Guild data{};
		data.id = dataPackage.guildId;
		if (!Guilds::getCache().contains(data)) {
			auto guild = Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
			guild.discordCoreClient = Guilds::discordCoreClient;
			return guild;
		} else {
			GuildData guild = Guilds::getCache().at(data);
			guild.discordCoreClient = Guilds::discordCoreClient;
			return guild;
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Preview };
		co_await NewThreadAwaitable<GuildPreviewData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/preview";
		workload.callStack = "Guilds::getGuildPreviewAsync()";
		GuildPreviewData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPreviewData>(workload, &returnValue);
	}

	CoRoutine<Guild> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Guilds::modifyGuildAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Guild data{};
		data = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload, &data);
		data.discordCoreClient = Guilds::discordCoreClient;
		if (Guilds::getCache().contains(data)) {
			data = Guilds::getCache().at(data);
		} else {
			Guilds::insertGuild(data);
		}
		co_return std::move(data);
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId;
		workload.callStack = "Guilds::deleteGuildAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<BanData>> Guilds::getGuildBansAsync(GetGuildBansData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Bans };
		co_await NewThreadAwaitable<std::vector<BanData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Guilds::getGuildBansAsync()";
		BanDataVector returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<BanDataVector>(workload, &returnValue);
	}

	CoRoutine<BanData> Guilds::getGuildBanAsync(GetGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Ban };
		co_await NewThreadAwaitable<BanData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
		workload.callStack = "Guilds::getGuildBanAsync()";
		BanData data{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<BanData>(workload, &data);
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Ban };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
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
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
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
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/prune";
		workload.callStack = "Guilds::getGuildPruneCountAsync()";
		if (dataPackage.days != 0) {
			workload.relativePath += "?days=" + std::to_string(dataPackage.days);
			if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "&include_roles=";
				for (int32_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
					workload.relativePath += dataPackage.includeRoles[x];
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
		} else if (dataPackage.includeRoles.size() > 0) {
			workload.relativePath += "?include_roles=";
			for (int32_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
				workload.relativePath += dataPackage.includeRoles[x];
				if (x < dataPackage.includeRoles.size() - 1) {
					workload.relativePath += ",";
				}
			}
		}
		GuildPruneCountData data{};
		GuildPruneCountData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload, &returnValue);
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Prune };
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/prune";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Guilds::beginGuildPruneAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildPruneCountData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload, &returnValue);
	}

	CoRoutine<std::vector<VoiceRegionData>> Guilds::getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Voice_Regions };
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/regions";
		workload.callStack = "Guilds::getGuildVoiceRegionsAsync()";
		VoiceRegionDataVector returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<VoiceRegionDataVector>(workload, &returnValue);
	}

	CoRoutine<std::vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Invites };
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/invites";
		workload.callStack = "Guilds::getGuildInvitesAsync()";
		InviteDataVector returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteDataVector>(workload, &returnValue);
	}

	CoRoutine<std::vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Integrations };
		co_await NewThreadAwaitable<std::vector<IntegrationData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/integrations";
		workload.callStack = "Guilds::getGuildIntegrationsAsync()";
		IntegrationDataVector returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<IntegrationDataVector>(workload, &returnValue);
	}

	CoRoutine<void> Guilds::deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Integration };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/integrations/" + dataPackage.integrationId;
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
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
		workload.callStack = "Guilds::getGuildWidgetSettingsAsync()";
		GuildWidgetData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload, &returnValue);
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
		Jsonifier responseData{};
		responseData["channel_id"] = dataPackage.widgetData.channelId;
		responseData["enabled"] = dataPackage.widgetData.enabled;
		responseData.refreshString(JsonifierSerializeType::Json);
		workload.content = responseData.operator std::string();
		workload.callStack = "Guilds::modifyGuildWidgetAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildWidgetData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload, &returnValue);
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget.json";
		workload.callStack = "Guilds::getGuildWidgetAsync()";
		GuildWidgetData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload, &returnValue);
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Vanity_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/vanity-url";
		workload.callStack = "Guilds::getGuildVanityInviteAsync()";
		InviteData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteData>(workload, &returnValue);
	}

	CoRoutine<GuildWidgetImageData> Guilds::getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Image };
		co_await NewThreadAwaitable<GuildWidgetImageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget.png";
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
		GuildWidgetImageData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetImageData>(workload, &returnValue);
	}

	CoRoutine<WelcomeScreenData> Guilds::getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		workload.callStack = "Guilds::getGuildWelcomeScreenAsync()";
		WelcomeScreenData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload, &returnValue);
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Guilds::modifyGuildWelcomeScreenAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		WelcomeScreenData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload, &returnValue);
	};

	CoRoutine<GuildTemplateData> Guilds::getGuildTemplateAsync(GetGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::getGuildTemplateAsync()";
		GuildTemplateData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, &returnValue);
	}

	CoRoutine<Guild> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_From_Guild_Template };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		Jsonifier responseData{};
		responseData["name"] = dataPackage.name;
		responseData["icon"] = dataPackage.imageData;
		responseData.refreshString(JsonifierSerializeType::Json);
		workload.content = responseData.operator std::string();
		workload.callStack = "Guilds::createGuildFromGuildTemplateAsync()";
		Guild returnValue{};
		auto newGuild = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload, &returnValue);
		newGuild.discordCoreClient = Guilds::discordCoreClient;
		co_return std::move(newGuild);
	}

	CoRoutine<std::vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Templates };
		co_await NewThreadAwaitable<std::vector<GuildTemplateData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
		workload.callStack = "Guilds::getGuildTemplatesAsync()";
		GuildTemplateDataVector returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateDataVector>(workload, &returnValue);
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
		Jsonifier responseData{};
		responseData["description"] = dataPackage.description;
		responseData["name"] = dataPackage.name;
		responseData.refreshString(JsonifierSerializeType::Json);
		workload.content = responseData.operator std::string();
		workload.callStack = "Guilds::createGuildTemplateAsync()";
		GuildTemplateData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, &returnValue);
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::syncGuildTemplateAsync()";
		GuildTemplateData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, &returnValue);
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		Jsonifier responseData{};
		responseData["description"] = dataPackage.description;
		responseData["name"] = dataPackage.name;
		responseData.refreshString(JsonifierSerializeType::Json);
		workload.content = responseData.operator std::string();
		workload.callStack = "Guilds::modifyGuildTemplateAsync()";
		GuildTemplateData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, &returnValue);
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Template };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
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
			if (dataPackage.guildScheduledEventId != 0) {
				workload.relativePath += "&guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
			}
		} else if (dataPackage.withExpiration) {
			workload.relativePath += "?with_expiration=true";
			if (dataPackage.guildScheduledEventId != 0) {
				workload.relativePath += "&guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
			}
		} else if (dataPackage.guildScheduledEventId != 0) {
			workload.relativePath += "?guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
		}

		workload.callStack = "Guilds::getInviteAsync()";
		InviteData returnValue{};
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteData>(workload, &returnValue);
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
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Users::getCurrentUserGuildsAsync()";
		GuildVector returnValue{};
		auto guildVector = Guilds::httpsClient->submitWorkloadAndGetResult<GuildVector>(workload, &returnValue);
		for (auto& value: guildVector.guilds) {
			value.discordCoreClient = Guilds::discordCoreClient;
		}
		co_return guildVector;
	}

	CoRoutine<void> Guilds::leaveGuildAsync(LeaveGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Leave_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/users/@me/guilds/" + dataPackage.guildId;
		workload.callStack = "Guilds::leaveGuildAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	StopWatch stopWatch{ 5s };
	void Guilds::insertGuild(GuildData guild) {
		if (guild.id == 0) {
			return;
		}
		if (Guilds::doWeCacheGuilds()) {
			guild.discordCoreClient = Guilds::discordCoreClient;
			if (!Guilds::getCache().contains(guild)) {
				Guilds::getCache().emplace(std::move(guild));
			} else {
				Guilds::getCache()[guild] = std::move(guild);
			}
			if (Guilds::getCache().size() % 100 == 0) {
				std::cout << "THE GUILD COUNT: " << Guilds::getCache().size() << ", TOTAL TIME: " << stopWatch.totalTimePassed().count()
						  << "\n,THREAD ID: " << std::this_thread::get_id() << std::endl;
			}
		}
	}

	void Guilds::removeGuild(const Snowflake guildId) {
		GuildData data{};
		data.id = guildId;
		Guilds::getCache().erase(data);
	};

	ObjectCache<GuildData>& Guilds::getCache() {
		return Guilds::cache;
	}

	bool Guilds::doWeCacheGuilds() {
		return Guilds::doWeCacheGuildsBool;
	}

	DiscordCoreInternal::HttpsClient* Guilds::httpsClient{ nullptr };
	DiscordCoreClient* Guilds::discordCoreClient{ nullptr };
	bool Guilds::doWeCacheGuildsBool{};
	ObjectCache<GuildData> Guilds::cache{};
}