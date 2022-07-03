/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

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

	VoiceConnection* GuildData::connectToVoice(const Snowflake guildMemberId, const Snowflake channelId, bool selfDeaf, bool selfMute) {
		if (getVoiceConnectionMap().contains(this->id) && getVoiceConnectionMap()[this->id] && getVoiceConnectionMap()[this->id]->areWeConnected()) {
			this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
			return this->voiceConnectionPtr;
		} else if (guildMemberId != 0 || channelId != 0) {
			Snowflake theChannelId{};
			if (guildMemberId != 0) {
				if (this->voiceStates.contains(guildMemberId)) {
					theChannelId = this->voiceStates[guildMemberId].channelId;
				}
			} else {
				theChannelId = channelId;
			}
			uint64_t theShardId{ (this->id >> 22) % this->discordCoreClient->configManager.getTotalShardCount() };
			auto theBaseSocketAgentIndex{ static_cast<int32_t>(
				floor(static_cast<float>(theShardId) / static_cast<float>(this->discordCoreClient->configManager.getTotalShardCount())) *
				static_cast<float>(std::thread::hardware_concurrency())) };
			DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
			voiceConnectInitData.currentShard = theShardId;
			voiceConnectInitData.channelId = theChannelId;
			voiceConnectInitData.guildId = this->id;
			voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
			voiceConnectInitData.selfDeaf = selfDeaf;
			voiceConnectInitData.selfMute = selfMute;
			getVoiceConnectionMap()[this->id] = std::make_unique<VoiceConnection>(this->discordCoreClient->baseSocketAgentMap[std::to_string(theBaseSocketAgentIndex)].get(),
				voiceConnectInitData, &this->discordCoreClient->configManager);
			this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
			this->voiceConnectionPtr->connect();
			return this->voiceConnectionPtr;
		} else {
			return nullptr;
		}
	}

	bool GuildData::areWeConnected() {
		return getVoiceConnectionMap()[this->id]->areWeConnected();
	}

	void GuildData::disconnect() {
		if (getVoiceConnectionMap().contains(this->id) && getVoiceConnectionMap()[this->id].get()) {
			UpdateVoiceStateData updateVoiceData{};
			updateVoiceData.channelId = 0;
			updateVoiceData.selfDeaf = false;
			updateVoiceData.selfMute = false;
			updateVoiceData.guildId = this->id;
			this->discordCoreClient->getBotUser().updateVoiceStatus(updateVoiceData);
			getVoiceConnectionMap()[this->id]->activeState.store(VoiceActiveState::Exiting);
			getVoiceConnectionMap()[this->id]->disconnect();
			this->voiceConnectionPtr = nullptr;
		}
	}

	void GuildData::insertChannel(ChannelData theData) {
		Channels::insertChannel(theData);
	}

	void GuildData::insertRole(RoleData theData) {
		Roles::insertRole(theData);
	}

	void GuildData::insertGuildMember(GuildMemberData theData) {
		GuildMembers::insertGuildMember(theData);
	}

	void GuildData::initialize() {
		if (!getVoiceConnectionMap().contains(this->id)) {
			std::string theShardId{ std::to_string((this->id >> 22) % this->discordCoreClient->configManager.getTotalShardCount()) };
			getVoiceConnectionMap()[this->id] = std::make_unique<VoiceConnection>();
		}
		this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
		if (!getYouTubeAPIMap().contains(this->id)) {
			getYouTubeAPIMap()[this->id] =
				std::make_unique<DiscordCoreInternal::YouTubeAPI>(this->id, this->discordCoreClient->httpsClient.get(), &this->discordCoreClient->configManager);
		}
		if (!getSoundCloudAPIMap().contains(this->id)) {
			getSoundCloudAPIMap()[this->id] =
				std::make_unique<DiscordCoreInternal::SoundCloudAPI>(this->id, this->discordCoreClient->httpsClient.get(), &this->discordCoreClient->configManager);
		}
		if (!getSongAPIMap().contains(this->id)) {
			getSongAPIMap()[this->id] = std::make_unique<SongAPI>(this->id);
		}
	}

	Guild& Guild::operator=(GuildData&& other) {
		if (this != &other) {
			this->voiceConnectionPtr = other.voiceConnectionPtr;
			this->discordCoreClient = other.discordCoreClient;
			this->voiceStates = std::move(other.voiceStates);
			this->presences = std::move(other.presences);
			this->features = std::move(other.features);
			this->channels = std::move(other.channels);
			this->joinedAt = std::move(other.joinedAt);
			this->ownerId = std::move(other.ownerId);
			this->members = std::move(other.members);
			this->memberCount = other.memberCount;
			this->roles = std::move(other.roles);
			this->icon = std::move(other.icon);
			this->name = std::move(other.name);
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	Guild::Guild(GuildData&& other) {
		*this = std::move(other);
	}

	Guild& Guild::operator=(GuildData& other) {
		if (this != &other) {
			this->voiceConnectionPtr = other.voiceConnectionPtr;
			this->discordCoreClient = other.discordCoreClient;
			this->voiceStates = other.voiceStates;
			this->memberCount = other.memberCount;
			this->presences = other.presences;
			this->features = other.features;
			this->channels = other.channels;
			this->joinedAt = other.joinedAt;
			this->ownerId = other.ownerId;
			this->members = other.members;
			this->flags = other.flags;
			this->roles = other.roles;
			this->icon = other.icon;
			this->name = other.name;
			this->id = other.id;
		}
		return *this;
	}

	Guild::Guild(GuildData& other) {
		*this = other;
	}

	Guild& Guild::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	Guild::Guild(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildVector::operator std::vector<Guild>() {
		return this->theGuilds;
	}

	GuildVector& GuildVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildVector::GuildVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	void Guilds::initialize(DiscordCoreInternal::HttpsClient* theClient, DiscordCoreClient* discordCoreClientNew, ConfigManager* configManagerNew) {
		Guilds::discordCoreClient = discordCoreClientNew;
		Guilds::configManager = configManagerNew;
		Guilds::httpsClient = theClient;
	}

	CoRoutine<AuditLogData> Guilds::getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Audit_Logs);
		co_await NewThreadAwaitable<AuditLogData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Audit_Logs;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Guild);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds";
		workload.content = dataPackage;
		workload.callStack = "Guilds::createGuildAsync()";
		auto guildNew = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload);
		guildNew.discordCoreClient = Guilds::discordCoreClient;
		Guilds::insertGuild(guildNew);
		co_return guildNew;
	}

	CoRoutine<std::vector<GuildData>> Guilds::getAllGuildsAsync() {
		co_await NewThreadAwaitable<std::vector<GuildData>>();
		std::shared_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		GuildDataVector guildVector{};
		for (auto& [key, value]: *Guilds::cache) {
			value->discordCoreClient = Guilds::discordCoreClient;
			guildVector.theGuildDatas.push_back(*value);
		}
		co_return guildVector;
	}

	CoRoutine<Guild> Guilds::getGuildAsync(GetGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "?with_counts=true";
		workload.callStack = "Guilds::getGuildAsync()";
		auto guildNew = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload);
		guildNew.discordCoreClient = Guilds::discordCoreClient;
		co_return guildNew;
	}

	CoRoutine<GuildData> Guilds::getCachedGuildAsync(GetGuildData dataPackage) {
		co_await NewThreadAwaitable<GuildData>();
		std::shared_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		if (!Guilds::cache->contains(dataPackage.guildId)) {
			theLock.unlock();
			auto guildNew = Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
			Guilds::insertGuild(guildNew);
			co_return guildNew;
		} else {
			co_return *(*Guilds::cache)[dataPackage.guildId];
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Preview);
		co_await NewThreadAwaitable<GuildPreviewData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Preview;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/preview";
		workload.callStack = "Guilds::getGuildPreviewAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPreviewData>(workload);
	}

	CoRoutine<Guild> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Guild);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId);
		workload.content = dataPackage;
		workload.callStack = "Guilds::modifyGuildAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto guildNew = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload);
		guildNew.discordCoreClient = Guilds::discordCoreClient;
		co_return guildNew;
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Guild);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId);
		workload.callStack = "Guilds::deleteGuildAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<BanData>> Guilds::getGuildBansAsync(GetGuildBansData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Bans);
		co_await NewThreadAwaitable<std::vector<BanData>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Bans;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Ban);
		co_await NewThreadAwaitable<BanData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Ban;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.userId);
		workload.callStack = "Guilds::getGuildBanAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<BanData>(workload);
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Ban);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Ban;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Ban);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Ban;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.userId);
		workload.callStack = "Guilds::removeGuildBanAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildPruneCountData> Guilds::getGuildPruneCountAsync(GetGuildPruneCountData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Prune_Count);
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Prune_Count;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/prune";
		workload.callStack = "Guilds::getGuildPruneCountAsync()";
		if (dataPackage.days != 0) {
			workload.relativePath += "?days=" + std::to_string(dataPackage.days);
			if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "&include_roles=";
				for (int32_t x = 0; x < dataPackage.includeRoles.size(); x++) {
					workload.relativePath += std::to_string(dataPackage.includeRoles[x]);
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
		} else if (dataPackage.includeRoles.size() > 0) {
			workload.relativePath += "?include_roles=";
			for (int32_t x = 0; x < dataPackage.includeRoles.size(); x++) {
				workload.relativePath += std::to_string(dataPackage.includeRoles[x]);
				if (x < dataPackage.includeRoles.size() - 1) {
					workload.relativePath += ",";
				}
			}
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload);
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Prune);
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Prune;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Voice_Regions);
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Voice_Regions;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/regions";
		workload.callStack = "Guilds::getGuildVoiceRegionsAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<VoiceRegionDataVector>(workload);
	}

	CoRoutine<std::vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Invites);
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Invites;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/invites";
		workload.callStack = "Guilds::getGuildInvitesAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteDataVector>(workload);
	}

	CoRoutine<std::vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Integrations);
		co_await NewThreadAwaitable<std::vector<IntegrationData>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Integrations;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/integrations";
		workload.callStack = "Guilds::getGuildIntegrationsAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<IntegrationDataVector>(workload);
	}

	CoRoutine<void> Guilds::deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Integration);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Integration;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/integrations/" + std::to_string(dataPackage.integrationId);
		workload.callStack = "Guilds::deleteGuildIntegrationAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Settings);
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Settings;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget";
		workload.callStack = "Guilds::getGuildWidgetSettingsAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Widget);
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Widget;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget";
		nlohmann::json responseData = { { "channel_id", std::to_string(dataPackage.widgetData.channelId) }, { "enabled", dataPackage.widgetData.enabled } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::modifyGuildWidgetAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget);
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget.nlohmann::json";
		workload.callStack = "Guilds::getGuildWidgetAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Vanity_Invite);
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Vanity_Invite;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/vanity-url";
		workload.callStack = "Guilds::getGuildVanityInviteAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<InviteData>(workload);
	}

	CoRoutine<GuildWidgetImageData> Guilds::getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Image);
		co_await NewThreadAwaitable<GuildWidgetImageData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Widget_Image;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Welcome_Screen);
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Welcome_Screen;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/welcome-screen";
		workload.callStack = "Guilds::getGuildWelcomeScreenAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload);
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Welcome_Screen);
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Welcome_Screen;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::getGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<Guild> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Guild_From_Guild_Template);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Guild_From_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		nlohmann::json responseData = { { "name", dataPackage.name }, { "icon", dataPackage.imageData } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::createGuildFromGuildTemplateAsync()";
		auto newGuild = Guilds::httpsClient->submitWorkloadAndGetResult<Guild>(workload);
		newGuild.discordCoreClient = Guilds::discordCoreClient;
		co_return newGuild;
	}

	CoRoutine<std::vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Templates);
		co_await NewThreadAwaitable<std::vector<GuildTemplateData>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Templates;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates";
		workload.callStack = "Guilds::getGuildTemplatesAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateDataVector>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates";
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::createGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::syncGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::modifyGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Template);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::deleteGuildTemplateAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<InviteData> Guilds::getInviteAsync(GetInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Invite);
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Invite;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Invite);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Invite;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/invites/" + dataPackage.inviteId;
		workload.callStack = "Guilds::deleteInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Guild>> Guilds::getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Current_User_Guilds);
		co_await NewThreadAwaitable<std::vector<Guild>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Current_User_Guilds;
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Leave_Guild);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Leave_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/users/@me/guilds/" + std::to_string(dataPackage.guildId);
		workload.callStack = "Users::leaveGuildAsync()";
		co_return Guilds::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	void Guilds::insertGuild(GuildData guild) {
		std::unique_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		if (guild.id == 0) {
			return;
		}
		auto newCache = std::make_unique<std::unordered_map<Snowflake, std::unique_ptr<GuildData>>>();
		for (auto& [key, value]: *Guilds::cache) {
			(*newCache)[key] = std::move(value);
		}
		guild.initialize();
		if (Guilds::configManager->doWeCacheGuilds()) {
			(*newCache)[guild.id] = std::make_unique<GuildData>(guild);
		}
		Guilds::cache.reset(nullptr);
		Guilds::cache = std::move(newCache);
	}

	void Guilds::removeGuild(const Snowflake& guildId) {
		std::unique_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		Guilds::cache->erase(guildId);
	};

	std::unique_ptr<std::unordered_map<Snowflake, std::unique_ptr<GuildData>>> Guilds::cache{ std::make_unique<std::unordered_map<Snowflake, std::unique_ptr<GuildData>>>() };
	DiscordCoreInternal::HttpsClient* Guilds::httpsClient{ nullptr };
	DiscordCoreClient* Guilds::discordCoreClient{ nullptr };
	ConfigManager* Guilds::configManager{ nullptr };
	std::shared_mutex Guilds::theMutex{};

}