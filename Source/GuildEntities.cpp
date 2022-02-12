// GuildEntities.hpp - Header for the Guild classes and structs.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "GuildEntities.hpp"
#include "DiscordCoreClient.hpp"
#include "UserEntities.hpp"
#include "VoiceConnection.hpp"
#include "SongAPI.hpp"
#include "YouTubeAPI.hpp"
#include "SoundCloudAPI.hpp"
#include "RoleEntities.hpp"
#include "Http.hpp"

namespace DiscordCoreAPI {

	VoiceConnection* Guild::connectToVoice(std::string channelId) {
		try {
			if (this->voiceConnectionPtr->areWeConnected()) {
				return this->voiceConnectionPtr;
			}
			else if (channelId != "") {
				DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
				voiceConnectInitData.channelId = channelId;
				voiceConnectInitData.guildId = this->id;
				voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
				this->voiceConnectionPtr->connect(voiceConnectInitData);
				this->areWeConnectedBool = true;
				return this->voiceConnectionPtr;
			}
			return this->voiceConnectionPtr;
		}
		catch (...) {
			reportException("Guild::connectToVoice()");
		}
		return this->voiceConnectionPtr;
	}

	void Guild::disconnect() {
		try {
			if (getVoiceConnectionMap()->contains(this->id) && getVoiceConnectionMap()->at(this->id) != nullptr) {
				if (getSongAPIMap()->contains(this->id)) {
					SongAPI::stop(this->id);
				}
				getSoundCloudAPIMap()->at(this->id)->stop();
				getYouTubeAPIMap()->at(this->id)->stop();				
				UpdateVoiceStateData updateVoiceData{};
				updateVoiceData.channelId = "";
				updateVoiceData.selfDeaf = false;
				updateVoiceData.selfMute = false;
				updateVoiceData.guildId = this->id;
				this->discordCoreClient->getBotUser().updateVoiceStatus(updateVoiceData);
				this->areWeConnectedBool = false;
				this->voiceConnectionPtr = nullptr;
				getVoiceConnectionMap()->at(this->id)->disconnect();
			}
		}
		catch (...) {
			reportException("Guild::disconnect()");
		}
	}

	bool Guild::areWeConnected() {
		try {
			if (getVoiceConnectionMap()->contains(this->id) && getVoiceConnectionMap()->at(this->id) != nullptr) {
				return true;
			}
			else {
				return false;
			}
		}
		catch (...) {
			reportException("Guild::areWeConnected()");
		}
		return false;
	}

	Guild::Guild() {};

	Guild::Guild(GuildData dataNew, DiscordCoreClient* discordCoreClientNew) {
		this->defaultMessageNotifications = dataNew.defaultMessageNotifications;
		this->premiumSubscriptionCount = dataNew.premiumSubscriptionCount;
		this->approximatePresenceCount = dataNew.approximatePresenceCount;
		this->publicUpdatesChannelId = dataNew.publicUpdatesChannelId;
		this->approximateMemberCount = dataNew.approximateMemberCount;
		this->explicitContentFilter = dataNew.explicitContentFilter;
		this->maxVideoChannelUsers = dataNew.maxVideoChannelUsers;
		this->systemChannelFlags = dataNew.systemChannelFlags;
		this->verificationLevel = dataNew.verificationLevel;
		this->discoverySplash = dataNew.discoverySplash;
		this->preferredLocale = dataNew.preferredLocale;
		this->widgetChannelId = dataNew.widgetChannelId;
		this->systemChannelId = dataNew.systemChannelId;
		this->rulesChannelId = dataNew.rulesChannelId;
		this->stageInstances = dataNew.stageInstances;
		this->welcomeScreen = dataNew.welcomeScreen;
		this->widgetEnabled = dataNew.widgetEnabled;
		this->vanityUrlCode = dataNew.vanityUrlCode;
		this->applicationId = dataNew.applicationId;
		this->maxPresences = dataNew.maxPresences;
		this->afkChannelId = dataNew.afkChannelId;
		this->description = dataNew.description;
		this->voiceStates = dataNew.voiceStates;
		this->premiumTier = dataNew.premiumTier;
		this->unavailable = dataNew.unavailable;
		this->memberCount = dataNew.memberCount;
		this->permissions = dataNew.permissions;
		this->afkTimeOut = dataNew.afkTimeOut;
		this->maxMembers = dataNew.maxMembers;
		this->nsfwLevel = dataNew.nsfwLevel;
		this->createdAt = dataNew.createdAt;
		this->presences = dataNew.presences;
		this->features = dataNew.features;
		this->mfaLevel = dataNew.mfaLevel;
		this->joinedAt = dataNew.joinedAt;
		this->iconHash = dataNew.iconHash;
		this->channels = dataNew.channels;
		this->ownerId = dataNew.ownerId;
		this->threads = dataNew.threads;
		this->members = dataNew.members;
		this->splash = dataNew.splash;
		this->banner = dataNew.banner;
		this->region = dataNew.region;
		this->large = dataNew.large;
		this->owner = dataNew.owner;
		this->emoji = dataNew.emoji;
		this->roles = dataNew.roles;
		this->icon = dataNew.icon;
		this->name = dataNew.name;
		this->id = dataNew.id;
		this->discordCoreClient = discordCoreClientNew;
		if (!getVoiceConnectionMap()->contains(this->id)) {
			getVoiceConnectionMap()->insert_or_assign(this->id, std::make_unique<VoiceConnection>(this->discordCoreClient->baseSocketAgent.get()));
		}
		this->voiceConnectionPtr = getVoiceConnectionMap()->at(this->id).get();
		if (!getYouTubeAPIMap()->contains(this->id)) {
			getYouTubeAPIMap()->insert_or_assign(this->id, std::make_unique<YouTubeAPI>(this->id, this->discordCoreClient->httpClient.get()));
		}
		if (!getSoundCloudAPIMap()->contains(this->id)) {
			getSoundCloudAPIMap()->insert_or_assign(this->id, std::make_unique<SoundCloudAPI>(this->id, this->discordCoreClient->httpClient.get()));
		}
		if (!getSongAPIMap()->contains(this->id)) {
			getSongAPIMap()->insert_or_assign(this->id, std::make_unique<SongAPI>(this->id));
		}
		if (!getAudioBufferMap()->contains(this->id)) {
			getAudioBufferMap()->insert_or_assign(this->id, &getVoiceConnectionMap()->at(this->id)->audioBuffer);
		}
	}

	void Guild::initialize() {
		try {

			if (this->discordCoreClient->cacheOptions.cacheGuilds) {
				std::cout << "Caching Guild: " << this->name << std::endl;
			}
			if (this->discordCoreClient->cacheOptions.cacheChannels) {
				std::cout << "Caching channels for Guild: " << this->name << std::endl;
				for (auto& [key, value] : channels) {
					value.guildId = this->id;
					Channels::insertChannel(std::move(value));
				}
			}
			if (this->discordCoreClient->cacheOptions.cacheGuildMembers) {
				std::cout << "Caching GuildMembers for Guild: " << this->name << std::endl;
				for (auto& [key00, value00] : this->members) {
					value00.guildId = this->id;
					value00.voiceData.guildId = this->id;
					for (auto& [key01, value01] : this->voiceStates) {
						if (key01 == value00.user.id) {
							value01.guildId = this->id;
							value00.voiceData = value01;
						}
					}
					GuildMembers::insertGuildMember(std::move(value00));
				}
			}
			if (this->discordCoreClient->cacheOptions.cacheRoles) {
				std::cout << "Caching roles for Guild: " << this->name << std::endl;
				for (auto& [key, value] : roles) {
					Roles::insertRole(std::move(value));
				}
			}
			if (this->discordCoreClient->cacheOptions.cacheUsers) {
				std::cout << "Caching users for Guild: " << this->name << std::endl << std::endl;
				for (auto& [key, value] : members) {
					Users::insertUser(std::move(value.user));
				}
			}

		}
		catch (...) {
			reportException("Guild::initialize()");
		}
	}

	void Guilds::initialize(DiscordCoreInternal::HttpClient* theClient, DiscordCoreClient* discordCoreClientNew) {
		Guilds::discordCoreClient = discordCoreClientNew;
		Guilds::httpClientManager = theClient;
	}

	CoRoutine<AuditLogData> Guilds::getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage) {
		try {
			co_await NewThreadAwaitable<AuditLogData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Audit_Logs;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/audit-logs";
			if (dataPackage.userId != "") {
				workload.relativePath += "?user_id=" + dataPackage.userId;
				if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
					workload.relativePath += "&action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
				}
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
				workload.relativePath += "?action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "Guilds::getAuditLogDataAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<AuditLogData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getAuditLogDataAsync()");
		}
	}

	CoRoutine<Guild> Guilds::createGuildAsync(CreateGuildData dataPackage) {
		try {
			co_await NewThreadAwaitable<Guild>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::createGuildAsync";
			auto guildNew = DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(Guilds::httpClient, workload);
			guildNew.discordCoreClient = Guilds::discordCoreClient;
			Guilds::insertGuild(guildNew);
			co_return guildNew;
		}
		catch (...) {
			reportException("Guilds::createGuildAsync()");
		}
	}

	CoRoutine<std::vector<Guild>> Guilds::getAllGuildsAsync() {
		try {
			co_await NewThreadAwaitable<std::vector<Guild>>();
			std::vector<Guild> guildVector;
			for (auto& [key, value] : Guilds::cache) {
				value.discordCoreClient = Guilds::discordCoreClient;
				guildVector.push_back(value);
			}
			co_return guildVector;
		}
		catch (...) {
			reportException("Guilds::getAllGuildsAsync()");
		}
	}

	CoRoutine<Guild> Guilds::getGuildAsync(GetGuildData dataPackage) {
		try {
			co_await NewThreadAwaitable<Guild>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			workload.callStack = "Guilds::getGuildAsync";
			auto guild = DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(Guilds::httpClient, workload);
			guild.discordCoreClient = Guilds::discordCoreClient;
			Guilds::insertGuild(guild);
			co_return guild;
		}
		catch (...) {
			reportException("Guilds::getGuildAsync()");
		}
	}

	CoRoutine<Guild> Guilds::getCachedGuildAsync(GetGuildData dataPackage) {
		try {
			co_await NewThreadAwaitable<Guild>();

			if (Guilds::cache.contains(dataPackage.guildId)) {
				Guild guild = Guilds::cache.at(dataPackage.guildId);
				Guild guildNew{ guild };
				co_return guildNew;
			}
			else {
				Guild guild = Guilds::getGuildAsync(dataPackage).get();
				Guild guildNew{ guild };
				co_return guildNew;
			}
		}
		catch (...) {
			reportException("Guilds::getCachedGuildAsync()");
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildPreviewData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Preview;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/preview";
			workload.callStack = "Guilds::getGuildPreviewAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildPreviewData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildPreviewAsync()");
		}
	}

	CoRoutine<Guild> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		try {
			co_await NewThreadAwaitable<Guild>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::modifyGuildAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			auto guildNew = DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(Guilds::httpClient, workload);
			Guilds::insertGuild(guildNew);
			co_return guildNew;
		}
		catch (...) {
			reportException("Guilds::modifyGuildAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			workload.callStack = "Guilds::deleteGuildAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::deleteGuildAsync()");
		}
	}

	CoRoutine<std::vector<BanData>> Guilds::getGuildBansAsync(GetGuildBansData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<BanData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Bans;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans";
			workload.callStack = "Guilds::getGuildBansAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<BanData>>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildBansAsync()");
		}
	}

	CoRoutine<BanData> Guilds::getGuildBanAsync(GetGuildBanData dataPackage) {
		try {
			co_await NewThreadAwaitable<BanData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Ban;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
			workload.callStack = "Guilds::getGuildBanAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<BanData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildBanAsync()");
		}
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Ban;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::createGuildBanAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::createGuildBanAsync()");
		}
	}

	CoRoutine<void> Guilds::removeGuildBanAsync(RemoveGuildBanData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Ban;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
			workload.callStack = "Guilds::removeGuildBanAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::removeGuildBanAsync()");
		}
	}

	CoRoutine<GuildPruneCountData> Guilds::getGuildPruneCountAsync(GetGuildPruneCountData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildPruneCountData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Prune_Count;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/prune";
			workload.callStack = "Guilds::getGuildPruneCountAsync";
			if (dataPackage.days != 0) {
				workload.relativePath += "?days=" + std::to_string(dataPackage.days);
				if (dataPackage.includeRoles.size() > 0) {
					workload.relativePath += "&include_roles=";
					for (int32_t x = 0; x < dataPackage.includeRoles.size(); x += 1) {
						workload.relativePath += dataPackage.includeRoles[x];
						if (x < dataPackage.includeRoles.size() - 1) {
							workload.relativePath += ",";
						}
					}
				}
			}
			else if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "?include_roles=";
				for (int32_t x = 0; x < dataPackage.includeRoles.size(); x += 1) {
					workload.relativePath += dataPackage.includeRoles[x];
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildPruneCountData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildPruneCountAsync()");
		}
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildPruneCountData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Prune;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/prune";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::beginGuildPruneAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<GuildPruneCountData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::beginGuildPruneAsync()");
		}
	}

	CoRoutine<std::vector<VoiceRegionData>> Guilds::getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Voice_Regions;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/regions";
			workload.callStack = "Guilds::getGuildVoiceRegionsAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<VoiceRegionData>>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildVoiceRegionsAsync()");
		}
	}

	CoRoutine<std::vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<InviteData >>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Invites;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/invites";
			workload.callStack = "Guilds::getGuildInvitesAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<InviteData>>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildInvitesAsync()");
		}
	}

	CoRoutine<std::vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<IntegrationData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Integrations;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/integrations";
			workload.callStack = "Guilds::getGuildIntegrationsAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<IntegrationData>>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildIntegrationsAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Integration;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/integrations/" + dataPackage.integrationId;
			workload.callStack = "Guilds::deleteGuildIntegrationAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<void>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::deleteGuildIntegrationAsync()");
		}
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildWidgetData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Settings;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
			workload.callStack = "Guilds::getGuildWidgetSettingsAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildWidgetSettingsAsync()");
		}
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildWidgetData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Widget;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
			nlohmann::json responseData = { {"channel_id",dataPackage.widgetData.channelId},{"enabled",dataPackage.widgetData.enabled} };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::modifyGuildWidgetAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::modifyGuildWidgetAsync()");
		}
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildWidgetData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget.nlohmann::json";
			workload.callStack = "Guilds::getGuildWidgetAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildWidgetAsync()");
		}
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		try {
			co_await NewThreadAwaitable<InviteData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Vanity_Invite;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/vanity-url";
			workload.callStack = "Guilds::getGuildVanityInviteAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<InviteData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildVanityInviteAsync()");
		}
	}

	CoRoutine<GuildWidgetImageData> Guilds::getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildWidgetImageData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Image;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
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
			workload.callStack = "Guilds::getGuildWidgetImageAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetImageData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildWidgetImageAsync()");
		}
	}

	CoRoutine<WelcomeScreenData> Guilds::getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage) {
		try {
			co_await NewThreadAwaitable<WelcomeScreenData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Welcome_Screen;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
			workload.callStack = "Guilds::getGuildWelcomeScreenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WelcomeScreenData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildWelcomeScreenAsync()");
		}
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		try {
			co_await NewThreadAwaitable<WelcomeScreenData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Welcome_Screen;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::modifyGuildWelcomeScreenAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WelcomeScreenData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::modifyGuildWelcomeScreenAsync()");
		}
	};

	CoRoutine<GuildTemplateData> Guilds::getGuildTemplateAsync(GetGuildTemplateData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildTemplateData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
			workload.callStack = "Guilds::getGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildTemplateAsync()");
		}
	}

	CoRoutine<Guild> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		try {
			co_await NewThreadAwaitable<Guild>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_From_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
			nlohmann::json responseData = { {"name", dataPackage.name},{"icon", dataPackage.imageData} };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::createGuildFromGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::createGuildFromGuildTemplateAsync()");
		}
	}

	CoRoutine<std::vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<GuildTemplateData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Templates;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
			workload.callStack = "Guilds::getGuildTemplatesAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildTemplateData>>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getGuildTemplatesAsync()");
		}
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildTemplateData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
			nlohmann::json responseData = { {"description",dataPackage.description},{"name",dataPackage.name} };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::createGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::createGuildTemplateAsync()");
		}
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildTemplateData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
			workload.callStack = "Guilds::syncGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::syncGuildTemplateAsync()");
		}
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		try {
			co_await NewThreadAwaitable<GuildTemplateData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
			nlohmann::json responseData = { {"description",dataPackage.description},{"name",dataPackage.name} };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::modifyGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::modifyGuildTemplateAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
			workload.callStack = "Guilds::deleteGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::deleteGuildTemplateAsync()");
		}
	}

	CoRoutine<InviteData> Guilds::getInviteAsync(GetInviteData dataPackage) {
		try {
			co_await NewThreadAwaitable<InviteData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Invite;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/invites/" + dataPackage.inviteId;
			if (dataPackage.withCount) {
				workload.relativePath += "?with_counts=true";
				if (dataPackage.withExpiration) {
					workload.relativePath += "&with_expiration=true";
				}
				if (dataPackage.guildScheduledEventId != "") {
					workload.relativePath += "&guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
				}
			}
			else if (dataPackage.withExpiration) {
				workload.relativePath += "?with_expiration=true";
				if (dataPackage.guildScheduledEventId != "") {
					workload.relativePath += "&guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
				}
			}
			else if (dataPackage.guildScheduledEventId != "") {
				workload.relativePath += "?guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
			}

			workload.callStack = "Guilds::getInviteAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<InviteData>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::getInviteAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteInviteAsync(DeleteInviteData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Invite;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/invites/" + dataPackage.inviteId;
			workload.callStack = "Guilds::deleteInviteAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Guilds::deleteInviteAsync()");
		}
	}

	CoRoutine<std::vector<Guild>> Guilds::getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<Guild>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Current_User_Guilds;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/users/@me/guilds";
			if (dataPackage.after != "") {
				workload.relativePath += "?after=" + dataPackage.after;
				if (dataPackage.before != "") {
					workload.relativePath += "&before=" + dataPackage.before;
				}
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + dataPackage.limit;
				}
			}
			else if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + dataPackage.limit;
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + dataPackage.limit;
			}
			workload.callStack = "Users::getCurrentUserGuildsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Guild>>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Users::getCurrentUserGuildsAsync()");
		}
	}

	CoRoutine<void> Guilds::leaveGuildAsync(LeaveGuildData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Leave_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/users/@me/guilds/" + dataPackage.guildId;
			workload.callStack = "Users::leaveGuildAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(Guilds::httpClient, workload);
		}
		catch (...) {
			reportException("Users::leaveGuildAsync()");
		}
	}

	void Guilds::insertGuild(Guild guild) {
		try {
			Guild guildNew(guild);
			if (guild.id == "") {
				return;
			}
			if (!Guilds::cache.contains(guild.id)) {
				guildNew.initialize();
			}
			Guilds::cache.insert_or_assign(guildNew.id, guildNew);
		}
		catch (...) {
			reportException("Guilds::insertGuild()");
		}
	}

	void Guilds::removeGuild(std::string guildId) {
		try {
			Guilds::cache.erase(guildId);
		}
		catch (...) {
			reportException("Guilds::removeGuild()");
		}
	};

	DiscordCoreInternal::HttpClient* Guilds::httpClientManager{ nullptr };
	DiscordCoreClient* Guilds::discordCoreClient{ nullptr };
	std::unordered_map<std::string, Guild> Guilds::cache{};

}