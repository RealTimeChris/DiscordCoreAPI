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
		using OTy2 = DiscordCoreAPI::CreateGuildData;
		static constexpr auto parseValue = object("default_message_notifications", &OTy2::defaultMessageNotifications, "explicit_content_filter",
			&OTy2::explicitContentFilter, "system_channel_flags", &OTy2::systemChannelFlags, "verification_level", &OTy2::verificationLevel,
			"afk_timeout", &OTy2::afkTimeout, "region", &OTy2::region, "name", &OTy2::name, "icon", "channels", &OTy2::channels, "system_channel_id",
			&OTy2::systemChannelId, "roles", &OTy2::roles, "afk_channel_id", &OTy2::afkChannelId);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildData> {
		using OTy2 = DiscordCoreAPI::ModifyGuildData;
		static constexpr auto parseValue = object("default_message_notifications", &OTy2::defaultMessageNotifications, "explicit_content_filter",
			&OTy2::explicitContentFilter, "system_channel_flags", &OTy2::systemChannelFlags, "verification_level", &OTy2::verificationLevel,
			"features", &OTy2::features, "public_updates_channel_id", &OTy2::publicUpdatesChannelId, "afk_timeout", &OTy2::afkTimeout,
			"preferred_locale", &OTy2::preferredLocale, "discovery_splash", &OTy2::discoverySplash, "system_channel_id", &OTy2::systemChannelId,
			"rules_channel_id", &OTy2::rulesChannelId, "description", &OTy2::description, "afk_channel_id", &OTy2::afkChannelId, "banner",
			&OTy2::banner, "splash", &OTy2::splash, "reason", &OTy2::reason, "owner_id", &OTy2::ownerId, "guild_id", &OTy2::guildId, "icon",
			&OTy2::icon, "name", &OTy2::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildBanData> {
		using OTy2 = DiscordCoreAPI::CreateGuildBanData;
		static constexpr auto parseValue = object("delete_message_days", &OTy2::deleteMessageDays, "member", &OTy2::guildMemberId, "reason",
			&OTy2::reason, "guild_id", &OTy2::guildId);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildWelcomeScreenData> {
		using OTy2 = DiscordCoreAPI::ModifyGuildWelcomeScreenData;
		static constexpr auto parseValue = object("welcome_channels", &OTy2::welcomeChannels, "description", &OTy2::description, "guild_id",
			&OTy2::guildId, "enabled", &OTy2::enabled, "reason", &OTy2::reason);
	};

	template<> struct Core<DiscordCoreAPI::GetGuildPruneCountData> {
		using OTy2 = DiscordCoreAPI::GetGuildPruneCountData;
		static constexpr auto parseValue = object("include_roles", &OTy2::includeRoles, "guild_id", &OTy2::guildId, "days", &OTy2::days);
	};

	template<> struct Core<DiscordCoreAPI::BeginGuildPruneData> {
		using OTy2 = DiscordCoreAPI::BeginGuildPruneData;
		static constexpr auto parseValue = object("include_roles", &OTy2::includeRoles, "compute_prune_count", &OTy2::computePruneCount, "reason",
			&OTy2::reason, "guild_id", &OTy2::guildId, "days", &OTy2::days);
	};


}

namespace DiscordCoreAPI {

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
				data.guildId = this->id;
				data.user.id = value;
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
				data.user.id = value.user.id;
				this->presences.emplace(data.user.id, std::move(data));
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
				data.guildId = this->id;
				data.user.id = value;
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
				data.user.id = value.user.id;
				this->presences.emplace(data.user.id, data);
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

	GuildData& GuildData::operator=(GuildCacheData&& other) noexcept {
		this->voiceConnectionPtr = other.voiceConnectionPtr;
		this->discordCoreClient = other.discordCoreClient;
		for (auto& value: other.voiceStates) {
			this->voiceStates.emplace_back(std::move(value.id));
		}
		for (auto& value: other.channels) {
			Id data{};
			data = value.id;
			this->channels.emplace_back(std::move(data));
		}
		for (auto& value: other.members) {
			this->members.emplace_back(std::move(value.user.id));
		}
		for (auto& value: other.roles) {
			Id data{};
			data = value.id;
			this->roles.emplace_back(std::move(data));
		}
		for (auto& value: other.emoji) {
			Id data{};
			data = value.id;
			this->emoji.emplace_back(std::move(data));
		}
		for (auto& value: other.guildScheduledEvents) {
			Id data{};
			data = value.id;
			this->guildScheduledEvents.emplace_back(std::move(data));
		}
		for (auto& value: other.presences) {
			PresenceUpdateData data{};
			data.user.id = value.user.id;
			this->presences.emplace_back(std::move(data));
		}
		for (auto& value: other.stageInstances) {
			Id data{};
			data = value.id;
			this->stageInstances.emplace_back(std::move(data));
		}
		for (auto& value: other.stickers) {
			Id data{};
			data = value.id;
			this->stickers.emplace_back(std::move(data));
		}
		for (auto& value: other.threads) {
			Id data{};
			data = value.id;
			this->threads.emplace_back(std::move(data));
		}
		this->joinedAt = std::move(other.joinedAt);
		this->ownerId = std::move(other.ownerId);
		this->memberCount = other.memberCount;
		this->icon = std::move(other.icon);
		this->name = std::move(other.name);
		this->flags = other.flags;
		this->id = other.id;
		return *this;
	}

	GuildData::GuildData(GuildCacheData&& other) noexcept {
		*this = std::move(other);
	}

	GuildData& GuildData::operator=(const GuildCacheData& other) noexcept {
		this->voiceConnectionPtr = other.voiceConnectionPtr;
		this->discordCoreClient = other.discordCoreClient;
		for (auto& value: other.voiceStates) {
			this->voiceStates.emplace_back(std::move(value.id));
		}
		for (auto& value: other.channels) {
			Id data{};
			data = value.id;
			this->channels.emplace_back(std::move(data));
		}
		for (auto& value: other.members) {
			this->members.emplace_back(std::move(value.user.id));
		}
		for (auto& value: other.roles) {
			Id data{};
			data = value.id;
			this->roles.emplace_back(data);
		}
		for (auto& value: other.emoji) {
			Id data{};
			data = value.id;
			this->emoji.emplace_back(data);
		}
		for (auto& value: other.guildScheduledEvents) {
			Id data{};
			data = value.id;
			this->guildScheduledEvents.emplace_back(data);
		}
		for (auto& value: other.presences) {
			PresenceUpdateData data{};
			data.user.id = value.user.id;
			this->presences.emplace_back(data);
		}
		for (auto& value: other.stageInstances) {
			Id data{};
			data = value.id;
			this->stageInstances.emplace_back(data);
		}
		for (auto& value: other.stickers) {
			Id data{};
			data = value.id;
			this->stickers.emplace_back(data);
		}
		for (auto& value: other.threads) {
			Id data{};
			data = value.id;
			this->threads.emplace_back(data);
		}
		this->joinedAt = other.joinedAt;
		this->ownerId = other.ownerId;
		this->memberCount = other.memberCount;
		this->icon = other.icon;
		this->name = other.name;
		this->flags = other.flags;
		this->id = other.id;
		return *this;
	}

	GuildData::GuildData(const GuildCacheData& other) noexcept {
		*this = other;
	}

	std::string Guild::getDiscoverySplashUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "discovery-splashes/" + this->id + "/" + this->discoverySplash.operator std::string() + ".png";
		return stringNew;
	}

	std::string Guild::getBannerUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "banners/" + this->id + "/" + this->banner.operator std::string() + ".png";
		return stringNew;
	}

	std::string Guild::getSplashUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "splashes/" + this->id + "/" + this->splash.operator std::string() + ".png";
		return stringNew;
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
		for (auto& value: Guilds::getCache()) {
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
		if (Guilds::getCache().contains(dataPackage.guildId)) {
			data = Guilds::getCache().find(dataPackage.guildId);
		} else {
			Guilds::insertGuild(std::move(data));
		}
		data.discordCoreClient = Guilds::discordCoreClient;
		co_return data;
	}

	GuildData Guilds::getCachedGuild(GetGuildData dataPackage) {
		Guild data{};
		data.id = dataPackage.guildId;
		if (!Guilds::getCache().contains(dataPackage.guildId)) {
			auto guild = Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
			guild.discordCoreClient = Guilds::discordCoreClient;
			return guild;
		} else {
			GuildData guild = Guilds::getCache().find(dataPackage.guildId);
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
		if (Guilds::getCache().contains(dataPackage.guildId)) {
			data = Guilds::getCache().find(dataPackage.guildId);
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
		Guilds::httpsClient->submitWorkloadAndGetResult(workload);
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
		Guilds::httpsClient->submitWorkloadAndGetResult(workload);
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
		Guilds::httpsClient->submitWorkloadAndGetResult(workload);
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
		Guilds::httpsClient->submitWorkloadAndGetResult(workload);
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
		EtfSerializer responseData{};
		responseData["channel_id"] = dataPackage.widgetData.channelId;
		responseData["enabled"] = dataPackage.widgetData.enabled;
		responseData.refreshString(SerializerSerializeType::Etf);
		workload.content = responseData.operator std::string();
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
		EtfSerializer responseData{};
		responseData["name"] = dataPackage.name;
		responseData["icon"] = dataPackage.imageData;
		responseData.refreshString(SerializerSerializeType::Etf);
		workload.content = responseData.operator std::string();
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
		EtfSerializer responseData{};
		responseData["description"] = dataPackage.description;
		responseData["name"] = dataPackage.name;
		responseData.refreshString(SerializerSerializeType::Etf);
		workload.content = responseData.operator std::string();
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
		EtfSerializer responseData{};
		responseData["description"] = dataPackage.description;
		responseData["name"] = dataPackage.name;
		responseData.refreshString(SerializerSerializeType::Etf);
		workload.content = responseData.operator std::string();
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
		Guilds::httpsClient->submitWorkloadAndGetResult(workload);
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
		Guilds::httpsClient->submitWorkloadAndGetResult(workload);
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
		Guilds::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	StopWatch stopWatch{ 5s };
	void Guilds::insertGuild(const GuildData& guild) {
		if (guild.id == 0) {
			return;
		}
		if (Guilds::doWeCacheGuilds()) {
			GuildData newGuild{ guild };
			newGuild.discordCoreClient = Guilds::discordCoreClient;
			Guilds::getCache().emplace(std::forward<GuildData>(newGuild));

			if (Guilds::getCache().count() % 1 == 0) {
				std::cout << "THE GUILD COUNT: " << Guilds::getCache().count() << ", TOTAL TIME: " << stopWatch.totalTimePassed().count()
						  << std::endl;
			}
		}
	}

	void Guilds::insertGuild(GuildData&& guild) {
		if (guild.id == 0) {
			return;
		}
		if (Guilds::doWeCacheGuilds()) {
			guild.discordCoreClient = Guilds::discordCoreClient;
			Guilds::getCache().emplace(std::forward<GuildData>(guild));

			if (Guilds::getCache().count() % 1 == 0) {
				std::cout << "THE GUILD COUNT: " << Guilds::getCache().count() << ", TOTAL TIME: " << stopWatch.totalTimePassed().count()
						  << std::endl;
			}
		}
	}

	void Guilds::removeGuild(const Snowflake guildId) {
		Guilds::getCache().erase(guildId);
	};

	ObjectCache<Snowflake, GuildData>& Guilds::getCache() {
		return Guilds::cache;
	}

	bool Guilds::doWeCacheGuilds() {
		return Guilds::doWeCacheGuildsBool;
	}

	DiscordCoreInternal::HttpsClient* Guilds::httpsClient{ nullptr };
	DiscordCoreClient* Guilds::discordCoreClient{ nullptr };
	ObjectCache<Snowflake, GuildData> Guilds::cache{};
	bool Guilds::doWeCacheGuildsBool{};

}