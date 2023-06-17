/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::CreateGuildData> {
		using ValueType = DiscordCoreAPI::CreateGuildData;
		static constexpr auto parseValue = object("default_message_notifications", &ValueType::defaultMessageNotifications, "explicit_content_filter",
			&ValueType::explicitContentFilter, "system_channel_flags", &ValueType::systemChannelFlags, "verification_level",
			&ValueType::verificationLevel, "afk_timeout", &ValueType::afkTimeout, "region", &ValueType::region, "name", &ValueType::name, "icon",
			"channels", &ValueType::channels, "system_channel_id", &ValueType::systemChannelId, "roles", &ValueType::roles, "afk_channel_id",
			&ValueType::afkChannelId);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildData> {
		using ValueType = DiscordCoreAPI::ModifyGuildData;
		static constexpr auto parseValue = object("default_message_notifications", &ValueType::defaultMessageNotifications, "explicit_content_filter",
			&ValueType::explicitContentFilter, "system_channel_flags", &ValueType::systemChannelFlags, "verification_level",
			&ValueType::verificationLevel, "features", &ValueType::features, "public_updates_channel_id", &ValueType::publicUpdatesChannelId,
			"afk_timeout", &ValueType::afkTimeout, "preferred_locale", &ValueType::preferredLocale, "discovery_splash", &ValueType::discoverySplash,
			"system_channel_id", &ValueType::systemChannelId, "rules_channel_id", &ValueType::rulesChannelId, "description", &ValueType::description,
			"afk_channel_id", &ValueType::afkChannelId, "banner", &ValueType::banner, "splash", &ValueType::splash, "reason", &ValueType::reason,
			"owner_id", &ValueType::ownerId, "guild_id", &ValueType::guildId, "icon", &ValueType::icon, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildBanData> {
		using ValueType = DiscordCoreAPI::CreateGuildBanData;
		static constexpr auto parseValue = object("delete_message_days", &ValueType::deleteMessageDays, "member", &ValueType::guildMemberId, "reason",
			&ValueType::reason, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildWelcomeScreenData> {
		using ValueType = DiscordCoreAPI::ModifyGuildWelcomeScreenData;
		static constexpr auto parseValue = object("welcome_channels", &ValueType::welcomeChannels, "description", &ValueType::description, "guild_id",
			&ValueType::guildId, "enabled", &ValueType::enabled, "reason", &ValueType::reason);
	};

	template<> struct Core<DiscordCoreAPI::GetGuildPruneCountData> {
		using ValueType = DiscordCoreAPI::GetGuildPruneCountData;
		static constexpr auto parseValue =
			object("include_roles", &ValueType::includeRoles, "guild_id", &ValueType::guildId, "days", &ValueType::days);
	};

	template<> struct Core<DiscordCoreAPI::BeginGuildPruneData> {
		using ValueType = DiscordCoreAPI::BeginGuildPruneData;
		static constexpr auto parseValue = object("include_roles", &ValueType::includeRoles, "compute_prune_count", &ValueType::computePruneCount,
			"reason", &ValueType::reason, "guild_id", &ValueType::guildId, "days", &ValueType::days);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildTemplateData> {
		using ValueType = DiscordCoreAPI::ModifyGuildTemplateData;
		static constexpr auto parseValue = object("template_code", &ValueType::templateCode, "description", &ValueType::description, "guild_id",
			&ValueType::guildId, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildTemplateData> {
		using ValueType = DiscordCoreAPI::CreateGuildTemplateData;
		static constexpr auto parseValue = object("description", &ValueType::description, "guild_id", &ValueType::guildId, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildFromGuildTemplateData> {
		using ValueType = DiscordCoreAPI::CreateGuildFromGuildTemplateData;
		static constexpr auto parseValue =
			object("template_code", &ValueType::templateCode, "image_data", &ValueType::imageData, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildWidgetData> {
		using ValueType = DiscordCoreAPI::ModifyGuildWidgetData;
		static constexpr auto parseValue =
			object("widget_data", &ValueType::widgetData, "reason", &ValueType::reason, "guild_id", &ValueType::guildId);
	};


}

namespace DiscordCoreAPI {

	Guild& Guild::operator=(GuildData&& other) noexcept {
		if (this != &other) {
			voiceConnection = other.voiceConnection;
			discordCoreClient = other.discordCoreClient;
			for (auto& value: other.channels) {
				Channel data{};
				data.id = value;
				channels.emplace_back(std::move(data));
			}
			for (auto& value: other.members) {
				GuildMember data{};
				data.guildId = id;
				data.user.id = value;
				members.emplace_back(std::move(data));
			}
			for (auto& value: other.roles) {
				Role data{};
				data.id = value;
				roles.emplace_back(std::move(data));
			}
			for (auto& value: other.emoji) {
				EmojiData data{};
				data.id = value;
				emoji.emplace_back(std::move(data));
			}
			for (auto& value: other.guildScheduledEvents) {
				GuildScheduledEvent data{};
				data.id = value;
				guildScheduledEvents.emplace_back(std::move(data));
			}
			for (auto& value: other.presences) {
				PresenceUpdateData data{};
				data.user.id = value.user.id;
				presences.emplace(data.user.id, std::move(data));
			}
			for (auto& value: other.stageInstances) {
				StageInstance data{};
				data.id = value;
				stageInstances.emplace_back(std::move(data));
			}
			for (auto& value: other.stickers) {
				Sticker data{};
				data.id = value;
				stickers.emplace_back(std::move(data));
			}
			for (auto& value: other.threads) {
				Thread data{};
				data.id = value;
				threads.emplace_back(std::move(data));
			}
			joinedAt = std::move(other.joinedAt);
			ownerId = std::move(other.ownerId);
			memberCount = other.memberCount;
			icon = std::move(other.icon);
			name = std::move(other.name);
			flags = other.flags;
			id = other.id;
		}
		return *this;
	}

	Guild::Guild(GuildData&& other) noexcept {
		*this = std::move(other);
	}

	Guild& Guild::operator=(const GuildData& other) noexcept {
		if (this != &other) {
			voiceConnection = other.voiceConnection;
			discordCoreClient = other.discordCoreClient;
			for (auto& value: other.channels) {
				Channel data{};
				data.id = value;
				channels.emplace_back(data);
			}
			for (auto& value: other.members) {
				GuildMember data{};
				data.guildId = id;
				data.user.id = value;
				members.emplace_back(data);
			}
			for (auto& value: other.roles) {
				Role data{};
				data.id = value;
				roles.emplace_back(data);
			}
			for (auto& value: other.emoji) {
				EmojiData data{};
				data.id = value;
				emoji.emplace_back(data);
			}
			for (auto& value: other.guildScheduledEvents) {
				GuildScheduledEvent data{};
				data.id = value;
				guildScheduledEvents.emplace_back(data);
			}
			for (auto& value: other.presences) {
				PresenceUpdateData data{};
				data.user.id = value.user.id;
				presences.emplace(data.user.id, data);
			}
			for (auto& value: other.stageInstances) {
				StageInstance data{};
				data.id = value;
				stageInstances.emplace_back(data);
			}
			for (auto& value: other.stickers) {
				Sticker data{};
				data.id = value;
				stickers.emplace_back(data);
			}
			for (auto& value: other.threads) {
				Thread data{};
				data.id = value;
				threads.emplace_back(data);
			}
			memberCount = other.memberCount;
			joinedAt = other.joinedAt;
			ownerId = other.ownerId;
			flags = other.flags;
			icon = other.icon;
			name = other.name;
			id = other.id;
		}
		return *this;
	}

	Guild::Guild(const GuildData& other) noexcept {
		*this = other;
	}

	GuildData& GuildData::operator=(GuildCacheData&& other) noexcept {
		voiceConnection = other.voiceConnection;
		discordCoreClient = other.discordCoreClient;
		for (auto& value: other.voiceStates) {
			voiceStates.emplace_back(std::move(value.id));
		}
		for (auto& value: other.channels) {
			Snowflake data{};
			data = value.id;
			channels.emplace_back(std::move(data));
		}
		for (auto& value: other.members) {
			members.emplace_back(std::move(value.user.id));
		}
		for (auto& value: other.roles) {
			Snowflake data{};
			data = value.id;
			roles.emplace_back(std::move(data));
		}
		for (auto& value: other.emoji) {
			Snowflake data{};
			data = value.id;
			emoji.emplace_back(std::move(data));
		}
		for (auto& value: other.guildScheduledEvents) {
			Snowflake data{};
			data = value.id;
			guildScheduledEvents.emplace_back(std::move(data));
		}
		for (auto& value: other.presences) {
			PresenceUpdateData data{};
			data.user.id = value.user.id;
			presences.emplace_back(std::move(data));
		}
		for (auto& value: other.stageInstances) {
			Snowflake data{};
			data = value.id;
			stageInstances.emplace_back(std::move(data));
		}
		for (auto& value: other.stickers) {
			Snowflake data{};
			data = value.id;
			stickers.emplace_back(std::move(data));
		}
		for (auto& value: other.threads) {
			Snowflake data{};
			data = value.id;
			threads.emplace_back(std::move(data));
		}
		joinedAt = std::move(other.joinedAt);
		ownerId = std::move(other.ownerId);
		memberCount = other.memberCount;
		icon = std::move(other.icon);
		name = std::move(other.name);
		flags = other.flags;
		id = other.id;
		return *this;
	}

	GuildData::GuildData(GuildCacheData&& other) noexcept {
		*this = std::move(other);
	}

	GuildData& GuildData::operator=(const GuildCacheData& other) noexcept {
		voiceConnection = other.voiceConnection;
		discordCoreClient = other.discordCoreClient;
		for (auto& value: other.voiceStates) {
			voiceStates.emplace_back(std::move(value.id));
		}
		for (auto& value: other.channels) {
			Snowflake data{};
			data = value.id;
			channels.emplace_back(std::move(data));
		}
		for (auto& value: other.members) {
			members.emplace_back(std::move(value.user.id));
		}
		for (auto& value: other.roles) {
			Snowflake data{};
			data = value.id;
			roles.emplace_back(data);
		}
		for (auto& value: other.emoji) {
			Snowflake data{};
			data = value.id;
			emoji.emplace_back(data);
		}
		for (auto& value: other.guildScheduledEvents) {
			Snowflake data{};
			data = value.id;
			guildScheduledEvents.emplace_back(data);
		}
		for (auto& value: other.presences) {
			PresenceUpdateData data{};
			data.user.id = value.user.id;
			presences.emplace_back(data);
		}
		for (auto& value: other.stageInstances) {
			Snowflake data{};
			data = value.id;
			stageInstances.emplace_back(data);
		}
		for (auto& value: other.stickers) {
			Snowflake data{};
			data = value.id;
			stickers.emplace_back(data);
		}
		for (auto& value: other.threads) {
			Snowflake data{};
			data = value.id;
			threads.emplace_back(data);
		}
		joinedAt = other.joinedAt;
		ownerId = other.ownerId;
		memberCount = other.memberCount;
		icon = other.icon;
		name = other.name;
		flags = other.flags;
		id = other.id;
		return *this;
	}

	GuildData::GuildData(const GuildCacheData& other) noexcept {
		*this = other;
	}

	std::string Guild::getDiscoverySplashUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "discovery-splashes/" + id + "/" + discoverySplash.operator std::string() + ".png";
		return stringNew;
	}

	std::string Guild::getBannerUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "banners/" + id + "/" + banner.operator std::string() + ".png";
		return stringNew;
	}

	std::string Guild::getSplashUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "splashes/" + id + "/" + splash.operator std::string() + ".png";
		return stringNew;
	}

	ModifyGuildData::ModifyGuildData(Guild dataPackage) {
		premiumProgressBarEnabled = getBool<GuildFlags>(dataPackage.flags, GuildFlags::Premium_Progress_Bar_Enabled);
		defaultMessageNotifications = dataPackage.defaultMessageNotifications;
		publicUpdatesChannelId = dataPackage.publicUpdatesChannelId;
		explicitContentFilter = dataPackage.explicitContentFilter;
		systemChannelFlags = dataPackage.systemChannelFlags;
		verificationLevel = dataPackage.verificationLevel;
		preferredLocale = dataPackage.preferredLocale;
		systemChannelId = dataPackage.systemChannelId;
		rulesChannelId = dataPackage.rulesChannelId;
		afkChannelId = dataPackage.afkChannelId;
		description = dataPackage.description;
		afkTimeout = dataPackage.afkTimeOut;
		for (auto& value: dataPackage.features) {
			features.emplace_back(value);
		}
		ownerId = dataPackage.ownerId;
		guildId = dataPackage.id;
		name = dataPackage.name;
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
		AuditLogData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<AuditLogData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Guild> Guilds::createGuildAsync(CreateGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildAsync()";
		Guild returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildData>(workload, returnData);
		returnData.discordCoreClient = Guilds::discordCoreClient;
		co_return returnData;
	}

	CoRoutine<GuildDataVector> Guilds::getAllGuildsAsync() {
		co_await NewThreadAwaitable<GuildDataVector>();
		GuildDataVector guildVector{};
		for (auto& value: Guilds::cache) {
			value.discordCoreClient = Guilds::discordCoreClient;
			guildVector.emplace_back(value);
		}
		co_return guildVector;
	}

	CoRoutine<Guild> Guilds::getGuildAsync(GetGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "?with_counts=true";
		workload.callStack = "Guilds::getGuildAsync()";
		GuildData data{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildData>(workload, data);
		if (Guilds::cache.contains(dataPackage.guildId)) {
			data = Guilds::cache.find(dataPackage.guildId);
		} else {
			Guilds::insertGuild(std::move(data));
		}
		data.discordCoreClient = Guilds::discordCoreClient;
		co_return data;
	}

	GuildData Guilds::getCachedGuild(GetGuildData dataPackage) {
		Guild data{};
		data.id = dataPackage.guildId;
		if (!Guilds::cache.contains(dataPackage.guildId)) {
			auto guild = Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
			guild.discordCoreClient = Guilds::discordCoreClient;
			return guild;
		} else {
			return Guilds::cache.find(dataPackage.guildId);
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Preview };
		co_await NewThreadAwaitable<GuildPreviewData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/preview";
		workload.callStack = "Guilds::getGuildPreviewAsync()";
		GuildPreviewData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildPreviewData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Guild> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Guild data{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildData>(workload, data);
		data.discordCoreClient = Guilds::discordCoreClient;
		if (Guilds::cache.contains(dataPackage.guildId)) {
			data = Guilds::cache.find(dataPackage.guildId);
		} else {
			Guilds::insertGuild(std::move(data));
		}
		co_return data;
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId;
		workload.callStack = "Guilds::deleteGuildAsync()";
		Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
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
		BanDataVector returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<BanDataVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<BanData> Guilds::getGuildBanAsync(GetGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Ban };
		co_await NewThreadAwaitable<BanData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
		workload.callStack = "Guilds::getGuildBanAsync()";
		BanData data{};
		Guilds::httpsClient->submitWorkloadAndGetResult<BanData>(workload, data);
		co_return data;
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Ban };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
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
		Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
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
		GuildPruneCountData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Prune };
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/prune";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::beginGuildPruneAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildPruneCountData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<VoiceRegionData>> Guilds::getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Voice_Regions };
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/regions";
		workload.callStack = "Guilds::getGuildVoiceRegionsAsync()";
		VoiceRegionDataVector returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<VoiceRegionDataVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Invites };
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/invites";
		workload.callStack = "Guilds::getGuildInvitesAsync()";
		InviteDataVector returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<InviteDataVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Integrations };
		co_await NewThreadAwaitable<std::vector<IntegrationData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/integrations";
		workload.callStack = "Guilds::getGuildIntegrationsAsync()";
		IntegrationDataVector returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<IntegrationDataVector>(workload, returnData);
		co_return returnData;
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
		Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Settings };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
		workload.callStack = "Guilds::getGuildWidgetSettingsAsync()";
		GuildWidgetData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildWidgetAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildWidgetData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget.json";
		workload.callStack = "Guilds::getGuildWidgetAsync()";
		GuildWidgetData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Vanity_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/vanity-url";
		workload.callStack = "Guilds::getGuildVanityInviteAsync()";
		InviteData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<InviteData>(workload, returnData);
		co_return returnData;
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
		GuildWidgetImageData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetImageData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<WelcomeScreenData> Guilds::getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		workload.callStack = "Guilds::getGuildWelcomeScreenAsync()";
		WelcomeScreenData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildWelcomeScreenAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		WelcomeScreenData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload, returnData);
		co_return returnData;
	};

	CoRoutine<GuildTemplateData> Guilds::getGuildTemplateAsync(GetGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::getGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Guild> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_From_Guild_Template };
		co_await NewThreadAwaitable<Guild>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildFromGuildTemplateAsync()";
		Guild returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildData>(workload, returnData);
		returnData.discordCoreClient = Guilds::discordCoreClient;
		co_return std::move(returnData);
	}

	CoRoutine<std::vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Templates };
		co_await NewThreadAwaitable<std::vector<GuildTemplateData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
		workload.callStack = "Guilds::getGuildTemplatesAsync()";
		GuildTemplateDataVector returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateDataVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::syncGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Template };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::deleteGuildTemplateAsync()";
		Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
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
		InviteData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<InviteData>(workload, returnData);
		co_return returnData;
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
		Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<GuildDataVector> Guilds::getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Current_User_Guilds };
		co_await NewThreadAwaitable<GuildDataVector>();
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
		GuildDataVector returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult<GuildDataVector>(workload, returnData);
		for (auto& value: returnData) {
			value.discordCoreClient = Guilds::discordCoreClient;
		}
		co_return std::move(returnData);
	}

	CoRoutine<void> Guilds::leaveGuildAsync(LeaveGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Leave_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/users/@me/guilds/" + dataPackage.guildId;
		workload.callStack = "Guilds::leaveGuildAsync()";
		Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	StopWatch stopWatch{ 5s };

	GuildData& Guilds::insertGuild(GuildData&& guild) {
		if (guild.id == 0) {
			throw DCAException{ "Sorry, but there was no id set for that guild." };
		}
		auto id = guild.id;
		guild.discordCoreClient = Guilds::discordCoreClient;
		Guilds::cache.emplace(std::forward<GuildData>(guild));
		if (Guilds::cache.count() % 500 == 0) {
			std::cout << "THE GUILD COUNT: " << Guilds::cache.count() << ", TOTAL TIME: " << stopWatch.totalTimePassed().count() << std::endl;
		}
		return cache.find(id);
	}

	void Guilds::removeGuild(const Snowflake guildId) {
		Guilds::cache.erase(guildId);
	};

	ObjectCache<Snowflake, GuildData>& Guilds::getCache() {
		return Guilds::cache;
	}

	bool Guilds::doWeCacheGuilds() {
		return Guilds::doWeCacheGuildsBool;
	}

	DiscordCoreInternal::HttpsClient* Guilds::httpsClient{};
	ObjectCache<Snowflake, GuildData> Guilds::cache{};
	DiscordCoreClient* Guilds::discordCoreClient{};
	bool Guilds::doWeCacheGuildsBool{};

}