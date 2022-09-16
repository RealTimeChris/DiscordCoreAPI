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

	CreateGuildData::operator std::string() {
		nlohmann::json data{};
		data["default_message_notifications"] = this->defaultMessageNotifications;
		data["explicit_content_filter"] = this->explicitContentFilter;
		data["system_channel_flags"] = this->systemChannelFlags;
		data["verification_level"] = this->verificationLevel;
		data["afk_timeout"] = this->afkTimeout;
		data["region"] = this->region;
		data["name"] = this->name;
		data["icon"] = this->icon;
		for (auto& value: this->channels) {
			nlohmann::json newData{};
			newData["parent_id"] = std::to_string(value.parentId);
			newData["name"] = std::string{ value.name };
			newData["id"] = value.id;
			newData["type"] = value.type;
			data["channels"].emplace_back(newData);
		}
		for (auto& value: this->roles) {
			nlohmann::json newData{};
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
			data["roles"].emplace_back(newData);
		}
		if (this->systemChannelId != 0) {
			data["system_channel_id"] = this->systemChannelId;
		}
		if (this->afkChannelId != 0) {
			data["afk_channel_id"] = this->afkChannelId;
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	CreateGuildBanData::operator std::string() {
		nlohmann::json data{};
		if (this->deleteMessageDays != 0) {
			data["delete_message_days"] = this->deleteMessageDays;
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	BeginGuildPruneData::operator std::string() {
		nlohmann::json data{};
		data["compute_prune_count"] = this->computePruneCount;
		data["include_roles"] = this->includeRoles;
		data["days"] = this->days;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	ModifyGuildWelcomeScreenData::operator std::string() {
		nlohmann::json channelsArray{};
		for (auto& value: this->welcomeChannels) {
			nlohmann::json newData{};
			newData["description"] = value.description;
			newData["channel_id"] = value.channelId;
			newData["emoji_name"] = value.emojiName;
			if (value.emojiId != 0) {
				newData["emoji_id"] = std::to_string(value.emojiId);
			}
			channelsArray.emplace_back(newData);
		}
		nlohmann::json data{};
		data["description"] = this->description;
		data["welcome_channels"] = channelsArray;
		data["enabled"] = this->enabled;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	VoiceConnection* GuildData::connectToVoice(const Snowflake guildMemberId, const Snowflake channelId, bool selfDeaf, bool selfMute, StreamType streamTypeNew,
		StreamInfo streamInfoNew) {
		if (DiscordCoreClient::getVoiceConnection(this->id) && DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected()) {
			this->voiceConnectionPtr = DiscordCoreClient::getVoiceConnection(this->id);
			return this->voiceConnectionPtr;
		} else if (guildMemberId != 0 || channelId != 0) {
			Snowflake theChannelId{};
			if (guildMemberId != 0) {
				auto guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = guildMemberId, .guildId = this->id }).get();
				if (guildMember.voiceChannelId != 0) {
					theChannelId = guildMember.voiceChannelId;
				}
			} else {
				theChannelId = channelId;
			}
			uint64_t theShardId{ (this->id >> 22) % this->discordCoreClient->configManager.getTotalShardCount() };
			auto theBaseSocketAgentIndex{ static_cast<int32_t>(floor(static_cast<float>(theShardId) /
				static_cast<float>(this->discordCoreClient->configManager.getTotalShardCount()) * this->discordCoreClient->baseSocketAgentMap.size())) };
			DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
			voiceConnectInitData.currentShard = theShardId;
			voiceConnectInitData.streamType = streamTypeNew;
			voiceConnectInitData.streamInfo = streamInfoNew;
			voiceConnectInitData.channelId = theChannelId;
			voiceConnectInitData.guildId = this->id;
			voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
			voiceConnectInitData.selfDeaf = selfDeaf;
			voiceConnectInitData.selfMute = selfMute;
			StopWatch theStopWatch{ 10000ms };
			this->discordCoreClient->baseSocketAgentMap[theBaseSocketAgentIndex]->connectVoiceChannel(voiceConnectInitData);
			while (!DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected()) {
				std::this_thread::sleep_for(1ms);
				if (theStopWatch.hasTimePassed()) {
					break;
				}
			}
			this->voiceConnectionPtr = DiscordCoreClient::getVoiceConnection(this->id);
			return this->voiceConnectionPtr;
		} else {
			return nullptr;
		}
	}

	std::string GuildData::getIconUrl() noexcept {
		std::string theStringNew{ "https://cdn.discordapp.com/" };
		theStringNew += "icons/" + std::to_string(this->id) + "/" + this->icon.getIconHash() + ".png";
		return theStringNew;
	}

	bool GuildData::areWeConnected() {
		return DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected();
	}

	void GuildData::disconnect() {
		if (DiscordCoreClient::getVoiceConnection(this->id)) {
			UpdateVoiceStateData updateVoiceData{};
			updateVoiceData.channelId = 0;
			updateVoiceData.selfDeaf = false;
			updateVoiceData.selfMute = false;
			updateVoiceData.guildId = this->id;
			this->discordCoreClient->getBotUser().updateVoiceStatus(updateVoiceData);
			DiscordCoreClient::getVoiceConnection(this->id)->disconnect();
			StopWatch theStopWatch{ 10000ms };
			while (DiscordCoreClient::getVoiceConnection(this->id)->areWeConnectedBool.load()) {
				std::this_thread::sleep_for(1ms);
				if (theStopWatch.hasTimePassed()) {
					break;
				}
			}
			this->voiceConnectionPtr = nullptr;
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

	ModifyGuildData::operator std::string() {
		nlohmann::json data{};
		data["premium_progress_bar_enabled"] = this->premiumProgressBarEnabled;
		data["default_message_notifications"] = this->defaultMessageNotifications;
		data["explicit_content_filter"] = this->explicitContentFilter;
		data["system_channel_flags"] = this->systemChannelFlags;
		data["verification_level"] = this->verificationLevel;
		data["discovery_splash"] = this->discoverySplash;
		data["preferred_locale"] = std::string{ this->preferredLocale };
		data["rules_channel_id"] = std::to_string(this->rulesChannelId);
		data["description"] = std::string{ this->description };
		data["afk_timeout"] = this->afkTimeout;
		data["features"] = this->features;
		data["splash"] = this->splash;
		data["banner"] = this->banner;
		data["name"] = std::string{ this->name };
		data["icon"] = this->icon;
		if (this->publicUpdatesChannelId != 0) {
			data["public_updates_channel_id"] = std::to_string(this->publicUpdatesChannelId);
		}
		if (this->systemChannelId != 0) {
			data["system_channel_id"] = std::to_string(this->systemChannelId);
		}
		if (this->afkChannelId != 0) {
			data["afk_channel_id"] = std::to_string(this->afkChannelId);
		}
		if (this->ownerId != 0) {
			data["owner_id"] = std::to_string(this->ownerId);
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
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
		if (dataPackage.userId != 0) {
			workload.relativePath += "?user_id=" + std::to_string(dataPackage.userId);
			if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
				workload.relativePath += "&action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
		} else if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
			workload.relativePath += "?action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
		} else if (dataPackage.before != 0) {
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
		workload.content = dataPackage;
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
		theDataNew = Guilds::cache.readOnly(theData);
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
			GuildData theGuild = Guilds::cache.readOnly(theData);
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
		workload.content = dataPackage;
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
		theDataNew = Guilds::cache.readOnly(theData);
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
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.before != 0) {
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
		workload.content = dataPackage;
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
		workload.content = dataPackage;
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
		nlohmann::json responseData = { { "channel_id", std::to_string(dataPackage.widgetData.channelId) }, { "enabled", dataPackage.widgetData.enabled } };
		workload.content = responseData.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
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
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget.nlohmann::json";
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
		workload.content = dataPackage;
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
		nlohmann::json responseData = { { "name", dataPackage.name }, { "icon", dataPackage.imageData } };
		workload.content = responseData.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
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
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
		workload.content = responseData.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
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
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
		workload.content = responseData.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
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
			if (dataPackage.guildScheduledEventId != 0) {
				workload.relativePath += "&guild_scheduled_event_id=" + std::to_string(dataPackage.guildScheduledEventId);
			}
		} else if (dataPackage.withExpiration) {
			workload.relativePath += "?with_expiration=true";
			if (dataPackage.guildScheduledEventId != 0) {
				workload.relativePath += "&guild_scheduled_event_id=" + std::to_string(dataPackage.guildScheduledEventId);
			}
		} else if (dataPackage.guildScheduledEventId != 0) {
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
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
			if (dataPackage.before != 0) {
				workload.relativePath += "&before=" + std::to_string(dataPackage.before);
			}
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.before != 0) {
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
		if (guild.id == 0) {
			return;
		}
		if (Guilds::doWeCacheGuilds) {
			guild.discordCoreClient = Guilds::discordCoreClient;
			if (!Guilds::cache.contains(guild)) {
				Guilds::cache.emplace(std::move(guild));
			} else {
				Guilds::cache.at(guild) = std::move(guild);
			}
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
	ObjectCache<GuildData> Guilds::cache{};
	bool Guilds::doWeCacheGuilds{ false };

}