/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
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
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::CreateGuildData> {
		using ValueType					 = DiscordCoreAPI::CreateGuildData;
		static constexpr auto parseValue = object("default_message_notifications", &ValueType::defaultMessageNotifications, "explicit_content_filter",
			&ValueType::explicitContentFilter, "system_channel_flags", &ValueType::systemChannelFlags, "verification_level", &ValueType::verificationLevel, "afk_timeout",
			&ValueType::afkTimeout, "region", &ValueType::region, "name", &ValueType::name, "icon", "channels", &ValueType::channels, "system_channel_id",
			&ValueType::systemChannelId, "roles", &ValueType::roles, "afk_channel_id", &ValueType::afkChannelId);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildData> {
		using ValueType					 = DiscordCoreAPI::ModifyGuildData;
		static constexpr auto parseValue = object("default_message_notifications", &ValueType::defaultMessageNotifications, "explicit_content_filter",
			&ValueType::explicitContentFilter, "system_channel_flags", &ValueType::systemChannelFlags, "verification_level", &ValueType::verificationLevel, "features",
			&ValueType::features, "public_updates_channel_id", &ValueType::publicUpdatesChannelId, "afk_timeout", &ValueType::afkTimeout, "preferred_locale",
			&ValueType::preferredLocale, "discovery_splash", &ValueType::discoverySplash, "system_channel_id", &ValueType::systemChannelId, "rules_channel_id",
			&ValueType::rulesChannelId, "description", &ValueType::description, "afk_channel_id", &ValueType::afkChannelId, "banner", &ValueType::banner, "splash",
			&ValueType::splash, "reason", &ValueType::reason, "owner_id", &ValueType::ownerId, "guild_id", &ValueType::guildId, "icon", &ValueType::icon, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildBanData> {
		using ValueType = DiscordCoreAPI::CreateGuildBanData;
		static constexpr auto parseValue =
			object("delete_message_days", &ValueType::deleteMessageDays, "member", &ValueType::guildMemberId, "reason", &ValueType::reason, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildWelcomeScreenData> {
		using ValueType					 = DiscordCoreAPI::ModifyGuildWelcomeScreenData;
		static constexpr auto parseValue = object("welcome_channels", &ValueType::welcomeChannels, "description", &ValueType::description, "guild_id", &ValueType::guildId,
			"enabled", &ValueType::enabled, "reason", &ValueType::reason);
	};

	template<> struct Core<DiscordCoreAPI::GetGuildPruneCountData> {
		using ValueType					 = DiscordCoreAPI::GetGuildPruneCountData;
		static constexpr auto parseValue = object("include_roles", &ValueType::includeRoles, "guild_id", &ValueType::guildId, "days", &ValueType::days);
	};

	template<> struct Core<DiscordCoreAPI::BeginGuildPruneData> {
		using ValueType					 = DiscordCoreAPI::BeginGuildPruneData;
		static constexpr auto parseValue = object("include_roles", &ValueType::includeRoles, "compute_prune_count", &ValueType::computePruneCount, "reason", &ValueType::reason,
			"guild_id", &ValueType::guildId, "days", &ValueType::days);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildTemplateData> {
		using ValueType = DiscordCoreAPI::ModifyGuildTemplateData;
		static constexpr auto parseValue =
			object("template_code", &ValueType::templateCode, "description", &ValueType::description, "guild_id", &ValueType::guildId, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildTemplateData> {
		using ValueType					 = DiscordCoreAPI::CreateGuildTemplateData;
		static constexpr auto parseValue = object("description", &ValueType::description, "guild_id", &ValueType::guildId, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildFromGuildTemplateData> {
		using ValueType					 = DiscordCoreAPI::CreateGuildFromGuildTemplateData;
		static constexpr auto parseValue = object("template_code", &ValueType::templateCode, "image_data", &ValueType::imageData, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildWidgetData> {
		using ValueType					 = DiscordCoreAPI::ModifyGuildWidgetData;
		static constexpr auto parseValue = object("widget_data", &ValueType::widgetData, "reason", &ValueType::reason, "guild_id", &ValueType::guildId);
	};


}

namespace DiscordCoreAPI {

	GuildData::GuildData(Snowflake newId) {
		id = newId;
	}

	GuildCacheData& GuildCacheData::operator=(const GuildData& other) {
		setFlagValue(GuildFlags::Premium_Progress_Bar_Enabled, other.premiumProgressBarEnabled);
		setFlagValue(GuildFlags::WidgetEnabled, other.widgetEnabled);
		setFlagValue(GuildFlags::Unavailable, other.unavailable);
		setFlagValue(GuildFlags::Large, other.large);
		setFlagValue(GuildFlags::Owner, other.owner);
		if (other.discordCoreClient) {
			discordCoreClient = other.discordCoreClient;
		}
		if (other.voiceConnection) {
			voiceConnection = other.voiceConnection;
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		for (auto& value: other.channels) {
			channels.emplace_back(value.id);
		}
		for (auto& value: other.members) {
			members.emplace_back(value.user.id);
		}
		for (auto& value: other.roles) {
			roles.emplace_back(value.id);
		}
		for (auto& value: other.emoji) {
			emoji.emplace_back(value.id);
		}
		if (other.discoverySplash != "") {
			discoverySplash = other.discoverySplash;
		}
		if (other.memberCount != 0) {
			memberCount = other.memberCount;
		}
		if (other.discovery != "") {
			discovery = other.discovery;
		}
		if (other.ownerId != 0) {
			ownerId = other.ownerId;
		}
		if (other.banner != "") {
			banner = other.banner;
		}
		if (other.splash != "") {
			splash = other.splash;
		}
		if (other.icon != "") {
			icon = other.icon;
		}
		if (other.name != "") {
			name = other.name;
		}
		if (other.id != 0) {
			id = other.id;
		}
		return *this;
	}

	GuildCacheData::GuildCacheData(const GuildData& other) {
		*this = other;
	}

	GuildCacheData& GuildCacheData::operator=(GuildData&& other) {
		setFlagValue(GuildFlags::Premium_Progress_Bar_Enabled, other.premiumProgressBarEnabled);
		setFlagValue(GuildFlags::WidgetEnabled, other.widgetEnabled);
		setFlagValue(GuildFlags::Unavailable, other.unavailable);
		setFlagValue(GuildFlags::Large, other.large);
		setFlagValue(GuildFlags::Owner, other.owner);
		if (other.discoverySplash != "") {
			discoverySplash = std::move(other.discoverySplash);
		}
		if (other.discordCoreClient) {
			discordCoreClient = other.discordCoreClient;
		}
		if (other.voiceConnection) {
			voiceConnection = other.voiceConnection;
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		if (other.discovery != "") {
			discovery = std::move(other.discovery);
		}
		for (auto& value: other.channels) {
			channels.emplace_back(value.id);
		}
		for (auto& value: other.members) {
			members.emplace_back(value.user.id);
		}
		for (auto& value: other.roles) {
			roles.emplace_back(value.id);
		}
		for (auto& value: other.emoji) {
			emoji.emplace_back(value.id);
		}
		if (other.memberCount != 0) {
			memberCount = other.memberCount;
		}
		if (other.banner != "") {
			banner = std::move(other.banner);
		}
		if (other.splash != "") {
			splash = std::move(other.splash);
		}
		if (other.icon != "") {
			icon = std::move(other.icon);
		}
		if (other.name != "") {
			name = std::move(other.name);
		}
		if (other.ownerId != 0) {
			ownerId = other.ownerId;
		}
		if (other.id != 0) {
			id = other.id;
		}
		return *this;
	}

	GuildCacheData::operator GuildData() {
		GuildData returnData{};
		returnData.discordCoreClient = discordCoreClient;
		returnData.voiceConnection	 = voiceConnection;
		for (auto& value: channels) {
			returnData.channels.emplace_back(Channels::getCachedChannel({ .channelId = value }));
		}
		for (auto& value: members) {
			returnData.members.emplace_back(GuildMembers::getCachedGuildMember({ .guildMemberId = value, .guildId = id }));
		}
		for (auto& value: roles) {
			returnData.roles.emplace_back(Roles::getCachedRole({ .guildId = id, .roleId = value }));
		}
		for (auto& value: emoji) {
			returnData.emoji.emplace_back(std::move(value));
		}
		returnData.premiumProgressBarEnabled = getFlagValue(GuildFlags::Premium_Progress_Bar_Enabled);
		returnData.widgetEnabled			 = getFlagValue(GuildFlags::WidgetEnabled);
		returnData.unavailable				 = getFlagValue(GuildFlags::Unavailable);
		returnData.name						 = std::move(name.operator std::string());
		returnData.large					 = getFlagValue(GuildFlags::Large);
		returnData.owner					 = getFlagValue(GuildFlags::Owner);
		returnData.discoverySplash			 = discoverySplash;
		returnData.discovery				 = std::move(discovery);
		returnData.ownerId					 = std::move(ownerId);
		returnData.banner					 = std::move(banner);
		returnData.splash					 = std::move(splash);
		returnData.memberCount				 = memberCount;
		returnData.icon						 = std::move(icon);
		returnData.flags					 = flags;
		returnData.id						 = id;
		return returnData;
	}

	GuildCacheData::GuildCacheData(GuildData&& other) {
		*this = std::move(other);
	}

	ModifyGuildData::ModifyGuildData(const GuildData& dataPackage) {
		premiumProgressBarEnabled	= dataPackage.getFlagValue(GuildFlags::Premium_Progress_Bar_Enabled);
		defaultMessageNotifications = dataPackage.defaultMessageNotifications;
		publicUpdatesChannelId		= dataPackage.publicUpdatesChannelId;
		explicitContentFilter		= dataPackage.explicitContentFilter;
		systemChannelFlags			= dataPackage.systemChannelFlags;
		verificationLevel			= dataPackage.verificationLevel;
		preferredLocale				= dataPackage.preferredLocale;
		systemChannelId				= dataPackage.systemChannelId;
		rulesChannelId				= dataPackage.rulesChannelId;
		afkChannelId				= dataPackage.afkChannelId;
		description					= dataPackage.description;
		for (auto& value: dataPackage.features) {
			features.emplace_back(value);
		}
		ownerId = dataPackage.ownerId;
		guildId = dataPackage.id;
		name	= dataPackage.name;
	}

	void Guilds::initialize(DiscordCoreInternal::HttpsClient* client, DiscordCoreClient* discordCoreClientNew, ConfigManager* configManagerNew) {
		Guilds::doWeCacheGuildsBool = configManagerNew->doWeCacheGuilds();
		Guilds::discordCoreClient	= discordCoreClientNew;
		Guilds::httpsClient			= client;
		stopWatchNew.resetTimer();
	}

	CoRoutine<AuditLogData> Guilds::getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Audit_Logs };
		co_await NewThreadAwaitable<AuditLogData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/audit-logs";
		if (dataPackage.userId != 0) {
			workload.relativePath += "?user_id=" + dataPackage.userId;
			if (std::to_string(static_cast<uint64_t>(dataPackage.actionType)) != "") {
				workload.relativePath += "&action_type=" + std::to_string(static_cast<uint64_t>(dataPackage.actionType));
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
		} else if (std::to_string(static_cast<uint64_t>(dataPackage.actionType)) != "") {
			workload.relativePath += "?action_type=" + std::to_string(static_cast<uint64_t>(dataPackage.actionType));
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
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildData> Guilds::createGuildAsync(CreateGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild };
		co_await NewThreadAwaitable<GuildData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/guilds";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildAsync()";
		GuildData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		returnData.discordCoreClient = Guilds::discordCoreClient;
		co_return returnData;
	}

	Jsonifier::Vector<GuildData> Guilds::getAllGuildsAsync() {
		Jsonifier::Vector<GuildData> returnData{};
		for (auto& value: Guilds::cache) {
			value->discordCoreClient = Guilds::discordCoreClient;
			returnData.emplace_back(*value);
		}
		return returnData;
	}

	CoRoutine<GuildData> Guilds::getGuildAsync(GetGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild };
		co_await NewThreadAwaitable<GuildData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "?with_counts=true";
		workload.callStack	   = "Guilds::getGuildAsync()";
		GuildData data{ dataPackage.guildId };
		data.discordCoreClient = Guilds::discordCoreClient;
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheGuildsBool) {
			insertGuild(static_cast<GuildCacheData>(data));
		}
		co_return data;
	}

	GuildCacheData Guilds::getCachedGuild(GetGuildData dataPackage) {
		GuildCacheData data{};
		data.id = dataPackage.guildId;
		if (!Guilds::cache.contains(data.id)) {
			auto guild				= Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
			guild.discordCoreClient = Guilds::discordCoreClient;
			return guild;
		} else {
			return cache[dataPackage.guildId];
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Preview };
		co_await NewThreadAwaitable<GuildPreviewData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/preview";
		workload.callStack	   = "Guilds::getGuildPreviewAsync()";
		GuildPreviewData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildData> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild };
		co_await NewThreadAwaitable<GuildData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildData data{ dataPackage.guildId };
		data.discordCoreClient = Guilds::discordCoreClient;
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheGuildsBool) {
			insertGuild(static_cast<GuildCacheData>(data));
		}
		co_return data;
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId;
		workload.callStack	   = "Guilds::deleteGuildAsync()";
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<Jsonifier::Vector<BanData>> Guilds::getGuildBansAsync(GetGuildBansData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Bans };
		co_await NewThreadAwaitable<Jsonifier::Vector<BanData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans";
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
		Jsonifier::Vector<BanData> returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<BanData> Guilds::getGuildBanAsync(GetGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Ban };
		co_await NewThreadAwaitable<BanData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
		workload.callStack	   = "Guilds::getGuildBanAsync()";
		BanData data{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		co_return std::move(data);
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Ban };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Guilds::removeGuildBanAsync(RemoveGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Ban };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
		workload.callStack	   = "Guilds::removeGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<GuildPruneCountData> Guilds::getGuildPruneCountAsync(GetGuildPruneCountData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Prune_Count };
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/prune";
		workload.callStack	   = "Guilds::getGuildPruneCountAsync()";
		if (dataPackage.days != 0) {
			workload.relativePath += "?days=" + std::to_string(dataPackage.days);
			if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "&include_roles=";
				for (uint64_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
					workload.relativePath += dataPackage.includeRoles[x];
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
		} else if (dataPackage.includeRoles.size() > 0) {
			workload.relativePath += "?include_roles=";
			for (uint64_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
				workload.relativePath += dataPackage.includeRoles[x];
				if (x < dataPackage.includeRoles.size() - 1) {
					workload.relativePath += ",";
				}
			}
		}
		GuildPruneCountData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Prune };
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/prune";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::beginGuildPruneAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildPruneCountData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<Jsonifier::Vector<VoiceRegionData>> Guilds::getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Voice_Regions };
		co_await NewThreadAwaitable<Jsonifier::Vector<VoiceRegionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/regions";
		workload.callStack	   = "Guilds::getGuildVoiceRegionsAsync()";
		Jsonifier::Vector<VoiceRegionData> returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<Jsonifier::Vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Invites };
		co_await NewThreadAwaitable<Jsonifier::Vector<InviteData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/invites";
		workload.callStack	   = "Guilds::getGuildInvitesAsync()";
		Jsonifier::Vector<InviteData> returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<Jsonifier::Vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Integrations };
		co_await NewThreadAwaitable<Jsonifier::Vector<IntegrationData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/integrations";
		workload.callStack	   = "Guilds::getGuildIntegrationsAsync()";
		Jsonifier::Vector<IntegrationData> returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Guilds::deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Integration };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/integrations/" + dataPackage.integrationId;
		workload.callStack	   = "Guilds::deleteGuildIntegrationAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Settings };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget";
		workload.callStack	   = "Guilds::getGuildWidgetSettingsAsync()";
		GuildWidgetData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildWidgetAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildWidgetData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget };
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget.json";
		workload.callStack	   = "Guilds::getGuildWidgetAsync()";
		GuildWidgetData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Vanity_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/vanity-url";
		workload.callStack	   = "Guilds::getGuildVanityInviteAsync()";
		InviteData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildWidgetImageData> Guilds::getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Image };
		co_await NewThreadAwaitable<GuildWidgetImageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget.png";
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
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<WelcomeScreenData> Guilds::getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		workload.callStack	   = "Guilds::getGuildWelcomeScreenAsync()";
		WelcomeScreenData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Welcome_Screen };
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildWelcomeScreenAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		WelcomeScreenData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	};

	CoRoutine<GuildTemplateData> Guilds::getGuildTemplateAsync(GetGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/templates/" + dataPackage.templateCode;
		workload.callStack	   = "Guilds::getGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildData> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_From_Guild_Template };
		co_await NewThreadAwaitable<GuildData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/guilds/templates/" + dataPackage.templateCode;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildFromGuildTemplateAsync()";
		GuildData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		returnData.discordCoreClient = Guilds::discordCoreClient;
		co_return returnData;
	}

	CoRoutine<Jsonifier::Vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Templates };
		co_await NewThreadAwaitable<Jsonifier::Vector<GuildTemplateData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates";
		workload.callStack	   = "Guilds::getGuildTemplatesAsync()";
		Jsonifier::Vector<GuildTemplateData> returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::createGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		workload.callStack	   = "Guilds::syncGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Template };
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Guilds::modifyGuildTemplateAsync()";
		GuildTemplateData returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Template };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		workload.callStack	   = "Guilds::deleteGuildTemplateAsync()";
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<InviteData> Guilds::getInviteAsync(GetInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/invites/" + dataPackage.inviteId;
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
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Guilds::deleteInviteAsync(DeleteInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Invite };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/invites/" + dataPackage.inviteId;
		workload.callStack	   = "Guilds::deleteInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<Jsonifier::Vector<GuildData>> Guilds::getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Current_User_Guilds };
		co_await NewThreadAwaitable<Jsonifier::Vector<GuildData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/users/@me/guilds";
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
		Jsonifier::Vector<GuildData> returnData{};
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		for (auto& value: returnData) {
			value.discordCoreClient = Guilds::discordCoreClient;
		}
		co_return returnData;
	}

	CoRoutine<void> Guilds::leaveGuildAsync(LeaveGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Leave_Guild };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/users/@me/guilds/" + dataPackage.guildId;
		workload.callStack	   = "Guilds::leaveGuildAsync()";
		Guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	ObjectCache<GuildCacheData>& Guilds::getCache() {
		return Guilds::cache;
	}

	void Guilds::removeGuild(const GuildCacheData& guildId) {
		Guilds::cache.erase(guildId);
	};

	bool Guilds::doWeCacheGuilds() {
		return Guilds::doWeCacheGuildsBool;
	}

	DiscordCoreInternal::HttpsClient* Guilds::httpsClient{};
	ObjectCache<GuildCacheData> Guilds::cache{};
	DiscordCoreClient* Guilds::discordCoreClient{};
	bool Guilds::doWeCacheGuildsBool{};

}
