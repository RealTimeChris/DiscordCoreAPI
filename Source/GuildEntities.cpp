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
/// GuildEntities.cpp - Source file for the guild classes and structs.
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

namespace jsonifier {

	template<> struct core<discord_core_api::create_guild_data> {
		using value_type				 = discord_core_api::create_guild_data;
		static constexpr auto parseValue = createValue("default_message_notifications", &value_type::defaultMessageNotifications, "explicit_content_filter",
			&value_type::explicitContentFilter, "system_channel_flags", &value_type::systemChannelFlags, "verification_level", &value_type::verificationLevel, "afk_timeout",
			&value_type::afkTimeout, "region", &value_type::region, "name", &value_type::name, "icon", &value_type::icon, "channels", &value_type::channels, "system_channel_id",
			&value_type::systemChannelId, "roles", &value_type::roles, "afk_channel_id", &value_type::afkChannelId);
	};

	template<> struct core<discord_core_api::modify_guild_data> {
		using value_type = discord_core_api::modify_guild_data;
		static constexpr auto parseValue =
			createValue("default_message_notifications", &value_type::defaultMessageNotifications, "explicit_content_filter", &value_type::explicitContentFilter,
				"system_channel_flags", &value_type::systemChannelFlags, "verification_level", &value_type::verificationLevel, "features", &value_type::features,
				"public_updates_channel_id", &value_type::publicUpdatesChannelId, "afk_timeout", &value_type::afkTimeout, "preferred_locale", &value_type::preferredLocale,
				"discovery_splash", &value_type::discoverySplash, "system_channel_id", &value_type::systemChannelId, "rules_channel_id", &value_type::rulesChannelId, "description",
				&value_type::description, "afk_channel_id", &value_type::afkChannelId, "banner", &value_type::banner, "splash", &value_type::splash,
				"owner_id", &value_type::ownerId, "guild_id", &value_type::guildId, "icon", &value_type::icon, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::create_guild_ban_data> {
		using value_type				 = discord_core_api::create_guild_ban_data;
		static constexpr auto parseValue = createValue("delete_message_days", &value_type::deleteMessageDays, "member", &value_type::guildMemberId,
			"guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::modify_guild_welcome_screen_data> {
		using value_type				 = discord_core_api::modify_guild_welcome_screen_data;
		static constexpr auto parseValue = createValue("welcome_channels", &value_type::welcomeChannels, "description", &value_type::description, "guild_id", &value_type::guildId,
			"enabled", &value_type::enabled);
	};

	template<> struct core<discord_core_api::get_guild_prune_count_data> {
		using value_type				 = discord_core_api::get_guild_prune_count_data;
		static constexpr auto parseValue = createValue("include_roles", &value_type::includeRoles, "guild_id", &value_type::guildId, "days", &value_type::days);
	};

	template<> struct core<discord_core_api::begin_guild_prune_data> {
		using value_type				 = discord_core_api::begin_guild_prune_data;
		static constexpr auto parseValue = createValue("include_roles", &value_type::includeRoles, "compute_prune_count", &value_type::computePruneCount, "reason",
			&value_type::reason, "guild_id", &value_type::guildId, "days", &value_type::days);
	};

	template<> struct core<discord_core_api::modify_guild_template_data> {
		using value_type = discord_core_api::modify_guild_template_data;
		static constexpr auto parseValue =
			createValue("template_code", &value_type::templateCode, "description", &value_type::description, "guild_id", &value_type::guildId, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::create_guild_template_data> {
		using value_type				 = discord_core_api::create_guild_template_data;
		static constexpr auto parseValue = createValue("description", &value_type::description, "guild_id", &value_type::guildId, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::create_guild_from_guild_template_data> {
		using value_type				 = discord_core_api::create_guild_from_guild_template_data;
		static constexpr auto parseValue = createValue("template_code", &value_type::templateCode, "image_data", &value_type::imageData, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::modify_guild_widget_data> {
		using value_type				 = discord_core_api::modify_guild_widget_data;
		static constexpr auto parseValue = createValue("widget_data", &value_type::widgetData, "guild_id", &value_type::guildId);
	};


}

namespace discord_core_api {

	guild_data::guild_data(snowflake newId) {
		id = newId;
	}

	guild_cache_data::guild_cache_data(snowflake newId) {
		id = newId;
	}

	discord_core_client* guild_cache_data::getDiscordCoreClient() {
		return discord_core_client::getInstance();
	}

	discord_core_client* guild_data::getDiscordCoreClient() {
		return discord_core_client::getInstance();
	}

	guild_cache_data& guild_cache_data::operator=(const guild_data& other) {
		setFlagValue(guild_flags::Premium_Progress_Bar_Enabled, other.premiumProgressBarEnabled);
		setFlagValue(guild_flags::Widget_enabled, other.widgetEnabled);
		setFlagValue(guild_flags::Unavailable, other.unavailable);
		setFlagValue(guild_flags::Large, other.large);
		setFlagValue(guild_flags::Owner, other.owner);
		if (discord_core_client::getInstance()) {
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

	guild_cache_data::guild_cache_data(const guild_data& other) {
		*this = other;
	}

	guild_cache_data& guild_cache_data::operator=(guild_data&& other) noexcept {
		setFlagValue(guild_flags::Premium_Progress_Bar_Enabled, other.premiumProgressBarEnabled);
		setFlagValue(guild_flags::Widget_enabled, other.widgetEnabled);
		setFlagValue(guild_flags::Unavailable, other.unavailable);
		setFlagValue(guild_flags::Large, other.large);
		setFlagValue(guild_flags::Owner, other.owner);
		if (other.discoverySplash != "") {
			discoverySplash = std::move(other.discoverySplash);
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

	guild_cache_data::operator guild_data() {
		guild_data returnData{};
		returnData.voiceConnection = voiceConnection;
		for (auto& value: channels) {
			if (channels::doWeCacheChannels()) {
				returnData.channels.emplace_back(channels::getCachedChannel({ .channelId = value }));
			} else {
				channel_data newChannel{};
				newChannel.id = value;
				returnData.channels.emplace_back(newChannel);
			}
		}
		for (auto& value: members) {
			if (guild_members::doWeCacheGuildMembers()) {
				returnData.members.emplace_back(guild_members::getCachedGuildMember({ .guildMemberId = value, .guildId = id }));
			} else {
				guild_member_data newChannel{};
				newChannel.guildId = id;
				newChannel.user.id = value;
				returnData.members.emplace_back(newChannel);
			}
		}
		for (auto& value: roles) {
			if (roles::doWeCacheRoles()) {
				returnData.roles.emplace_back(roles::getCachedRole({ .guildId = id, .roleId = value }));
			} else {
				role_data newChannel{};
				newChannel.id = value;
				returnData.roles.emplace_back(newChannel);
			}
		}
		for (auto& value: emoji) {
			emoji_data newValue{};
			newValue.id = value;
			returnData.emoji.emplace_back(std::move(newValue));
		}
		returnData.premiumProgressBarEnabled = getFlagValue(guild_flags::Premium_Progress_Bar_Enabled);
		returnData.widgetEnabled			 = getFlagValue(guild_flags::Widget_enabled);
		returnData.unavailable				 = getFlagValue(guild_flags::Unavailable);
		returnData.large					 = getFlagValue(guild_flags::Large);
		returnData.owner					 = getFlagValue(guild_flags::Owner);
		returnData.discoverySplash			 = discoverySplash;
		returnData.memberCount				 = memberCount;
		returnData.discovery				 = discovery;
		returnData.ownerId					 = ownerId;
		returnData.banner					 = banner;
		returnData.splash					 = splash;
		returnData.flags					 = flags;
		returnData.icon						 = icon;
		returnData.name						 = name;
		returnData.id						 = id;
		return returnData;
	}

	guild_cache_data::guild_cache_data(guild_data&& other) noexcept {
		*this = std::move(other);
	}

	modify_guild_data::modify_guild_data(const guild_data dataPackage) {
		premiumProgressBarEnabled	= dataPackage.getFlagValue(guild_flags::Premium_Progress_Bar_Enabled);
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

	void guilds::initialize(discord_core_internal::https_client* client, config_manager* configManagerNew) {
		guilds::doWeCacheGuildsBool = configManagerNew->doWeCacheGuilds();
		guilds::httpsClient			= client;
		stopWatchNew.reset();
	}

	co_routine<audit_log_data> guilds::getGuildAuditLogsAsync(get_guild_audit_logs_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Audit_Logs };
		co_await newThreadAwaitable<audit_log_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/audit-logs";
		if (dataPackage.userId != 0) {
			workload.relativePath += "?user_id=" + dataPackage.userId;
			if (jsonifier::toString(static_cast<uint64_t>(dataPackage.actionType)) != "") {
				workload.relativePath += "&action_type=" + jsonifier::toString(static_cast<uint64_t>(dataPackage.actionType));
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
		} else if (jsonifier::toString(static_cast<uint64_t>(dataPackage.actionType)) != "") {
			workload.relativePath += "?action_type=" + jsonifier::toString(static_cast<uint64_t>(dataPackage.actionType));
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
		} else if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + dataPackage.before;
		}
		workload.callStack = "guilds::getAuditLogDataAsync()";
		audit_log_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_data> guilds::createGuildAsync(create_guild_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild };
		co_await newThreadAwaitable<guild_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::createGuildAsync()";
		guild_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	jsonifier::vector<guild_data> guilds::getAllGuildsAsync() {
		jsonifier::vector<guild_data> returnData{};
		for (auto& value: guilds::cache) {
			returnData.emplace_back(*value);
		}
		return returnData;
	}

	co_routine<guild_data> guilds::getGuildAsync(get_guild_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild };
		co_await newThreadAwaitable<guild_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "?with_counts=true";
		workload.callStack	   = "guilds::getGuildAsync()";
		guild_data data{ dataPackage.guildId };
		;
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheGuildsBool) {
			insertGuild(static_cast<guild_cache_data>(data));
		}
		co_return data;
	}

	guild_cache_data guilds::getCachedGuild(get_guild_data dataPackage) {
		if (guilds::cache.contains(dataPackage.guildId)) {
			return cache[dataPackage.guildId];
		} else {
			return getGuildAsync(dataPackage).get();
		}
	}

	co_routine<guild_preview_data> guilds::getGuildPreviewAsync(get_guild_preview_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Preview };
		co_await newThreadAwaitable<guild_preview_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/preview";
		workload.callStack	   = "guilds::getGuildPreviewAsync()";
		guild_preview_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_data> guilds::modifyGuildAsync(modify_guild_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild };
		co_await newThreadAwaitable<guild_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::modifyGuildAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guild_data data{ dataPackage.guildId };
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheGuildsBool) {
			insertGuild(static_cast<guild_cache_data>(data));
		}
		co_return data;
	}

	co_routine<void> guilds::deleteGuildAsync(delete_guild_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId;
		workload.callStack	   = "guilds::deleteGuildAsync()";
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<ban_data>> guilds::getGuildBansAsync(get_guild_bans_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Bans };
		co_await newThreadAwaitable<jsonifier::vector<ban_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "guilds::getGuildBansAsync()";
		jsonifier::vector<ban_data> returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<ban_data> guilds::getGuildBanAsync(get_guild_ban_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Ban };
		co_await newThreadAwaitable<ban_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
		workload.callStack	   = "guilds::getGuildBanAsync()";
		ban_data data{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		co_return std::move(data);
	}

	co_routine<void> guilds::createGuildBanAsync(create_guild_ban_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Guild_Ban };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::createGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> guilds::removeGuildBanAsync(remove_guild_ban_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Ban };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
		workload.callStack	   = "guilds::removeGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<guild_prune_count_data> guilds::getGuildPruneCountAsync(get_guild_prune_count_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Prune_Count };
		co_await newThreadAwaitable<guild_prune_count_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/prune";
		workload.callStack	   = "guilds::getGuildPruneCountAsync()";
		if (dataPackage.days != 0) {
			workload.relativePath += "?days=" + jsonifier::toString(dataPackage.days);
			if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "&include_roles=";
				for (uint64_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
					workload.relativePath += dataPackage.includeRoles.at(x).operator jsonifier::string();
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
		} else if (dataPackage.includeRoles.size() > 0) {
			workload.relativePath += "?include_roles=";
			for (uint64_t x = 0; x < dataPackage.includeRoles.size(); ++x) {
				workload.relativePath += dataPackage.includeRoles.at(x).operator jsonifier::string();
				if (x < dataPackage.includeRoles.size() - 1) {
					workload.relativePath += ",";
				}
			}
		}
		guild_prune_count_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_prune_count_data> guilds::beginGuildPruneAsync(begin_guild_prune_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Prune };
		co_await newThreadAwaitable<guild_prune_count_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/prune";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::beginGuildPruneAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guild_prune_count_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<voice_region_data>> guilds::getGuildVoiceRegionsAsync(get_guild_voice_regions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Voice_Regions };
		co_await newThreadAwaitable<jsonifier::vector<voice_region_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/regions";
		workload.callStack	   = "guilds::getGuildVoiceRegionsAsync()";
		jsonifier::vector<voice_region_data> returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<invite_data>> guilds::getGuildInvitesAsync(get_guild_invites_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Invites };
		co_await newThreadAwaitable<jsonifier::vector<invite_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/invites";
		workload.callStack	   = "guilds::getGuildInvitesAsync()";
		jsonifier::vector<invite_data> returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);

		co_return returnData;
	}

	co_routine<jsonifier::vector<integration_data>> guilds::getGuildIntegrationsAsync(get_guild_integrations_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Integrations };
		co_await newThreadAwaitable<jsonifier::vector<integration_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/integrations";
		workload.callStack	   = "guilds::getGuildIntegrationsAsync()";
		jsonifier::vector<integration_data> returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> guilds::deleteGuildIntegrationAsync(delete_guild_integration_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Integration };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/integrations/" + dataPackage.integrationId;
		workload.callStack	   = "guilds::deleteGuildIntegrationAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<guild_widget_data> guilds::getGuildWidgetSettingsAsync(get_guild_widget_settings_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Widget_Settings };
		co_await newThreadAwaitable<guild_widget_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget";
		workload.callStack	   = "guilds::getGuildWidgetSettingsAsync()";
		guild_widget_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_widget_data> guilds::modifyGuildWidgetAsync(modify_guild_widget_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Widget };
		co_await newThreadAwaitable<guild_widget_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::modifyGuildWidgetAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guild_widget_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_widget_data> guilds::getGuildWidgetAsync(get_guild_widget_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Widget };
		co_await newThreadAwaitable<guild_widget_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget.json";
		workload.callStack	   = "guilds::getGuildWidgetAsync()";
		guild_widget_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<invite_data> guilds::getGuildVanityInviteAsync(get_guild_vanity_invite_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Vanity_Invite };
		co_await newThreadAwaitable<invite_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/vanity-url";
		workload.callStack	   = "guilds::getGuildVanityInviteAsync()";
		invite_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_widget_image_data> guilds::getGuildWidgetImageAsync(get_guild_widget_image_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Widget_Image };
		co_await newThreadAwaitable<guild_widget_image_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/widget.png";
		switch (dataPackage.widgetStlye) {
			case widget_style_options::Shield: {
				workload.relativePath += "?style=shield";
				break;
			}
			case widget_style_options::Banner1: {
				workload.relativePath += "?style=banner1";
				break;
			}
			case widget_style_options::Banner2: {
				workload.relativePath += "?style=banner2";
				break;
			}
			case widget_style_options::Banner3: {
				workload.relativePath += "?style=banner3";
				break;
			}
			case widget_style_options::Banner4: {
				workload.relativePath += "?style=banner4";
				break;
			}
		}
		workload.callStack = "guilds::getGuildWidgetImageAsync()";
		guild_widget_image_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<welcome_screen_data> guilds::getGuildWelcomeScreenAsync(get_guild_welcome_screen_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Welcome_Screen };
		co_await newThreadAwaitable<welcome_screen_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		workload.callStack	   = "guilds::getGuildWelcomeScreenAsync()";
		welcome_screen_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<welcome_screen_data> guilds::modifyGuildWelcomeScreenAsync(modify_guild_welcome_screen_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Welcome_Screen };
		co_await newThreadAwaitable<welcome_screen_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/welcome-screen";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::modifyGuildWelcomeScreenAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		welcome_screen_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	};

	co_routine<guild_template_data> guilds::getGuildTemplateAsync(get_guild_template_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Template };
		co_await newThreadAwaitable<guild_template_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/templates/" + dataPackage.templateCode;
		workload.callStack	   = "guilds::getGuildTemplateAsync()";
		guild_template_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_data> guilds::createGuildFromGuildTemplateAsync(create_guild_from_guild_template_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_From_Guild_Template };
		co_await newThreadAwaitable<guild_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/templates/" + dataPackage.templateCode;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::createGuildFromGuildTemplateAsync()";
		guild_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<guild_template_data>> guilds::getGuildTemplatesAsync(get_guild_templates_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Templates };
		co_await newThreadAwaitable<jsonifier::vector<guild_template_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates";
		workload.callStack	   = "guilds::getGuildTemplatesAsync()";
		jsonifier::vector<guild_template_data> returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_template_data> guilds::createGuildTemplateAsync(create_guild_template_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Template };
		co_await newThreadAwaitable<guild_template_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::createGuildTemplateAsync()";
		guild_template_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_template_data> guilds::syncGuildTemplateAsync(sync_guild_template_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Guild_Template };
		co_await newThreadAwaitable<guild_template_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		workload.callStack	   = "guilds::syncGuildTemplateAsync()";
		guild_template_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_template_data> guilds::modifyGuildTemplateAsync(modify_guild_template_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Template };
		co_await newThreadAwaitable<guild_template_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guilds::modifyGuildTemplateAsync()";
		guild_template_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> guilds::deleteGuildTemplateAsync(delete_guild_template_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Template };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
		workload.callStack	   = "guilds::deleteGuildTemplateAsync()";
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<invite_data> guilds::getInviteAsync(get_invite_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Invite };
		co_await newThreadAwaitable<invite_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
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

		workload.callStack = "guilds::getInviteAsync()";
		invite_data returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> guilds::deleteInviteAsync(delete_invite_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Invite };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/invites/" + dataPackage.inviteId;
		workload.callStack	   = "guilds::deleteInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<guild_data>> guilds::getCurrentUserGuildsAsync(get_current_user_guilds_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Current_User_Guilds };
		co_await newThreadAwaitable<jsonifier::vector<guild_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/users/@me/guilds";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + dataPackage.before;
			}	
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.before != 0) {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "users::getCurrentUserGuildsAsync()";
		jsonifier::vector<guild_data> returnData{};
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> guilds::leaveGuildAsync(leave_guild_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Leave_Guild };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/users/@me/guilds/" + dataPackage.guildId;
		workload.callStack	   = "guilds::leaveGuildAsync()";
		guilds::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	object_cache<guild_cache_data>& guilds::getCache() {
		return guilds::cache;
	}

	void guilds::removeGuild(const guild_cache_data& guildId) {
		guilds::cache.erase(guildId);
	};

	bool guilds::doWeCacheGuilds() {
		return guilds::doWeCacheGuildsBool;
	}

	discord_core_internal::https_client* guilds::httpsClient{};
	object_cache<guild_cache_data> guilds::cache{};
	bool guilds::doWeCacheGuildsBool{};

}
