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
#include <discordcoreapi/Http.hpp>

namespace DiscordCoreAPI {

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

	VoiceConnection* Guild::connectToVoice(const uint64_t guildMemberId, const uint64_t channelId, bool selfDeaf, bool selfMute) {
		if (getVoiceConnectionMap()[this->id]->areWeConnected()) {
			this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
			return this->voiceConnectionPtr;
		} else if (guildMemberId != 0 || channelId != 0) {
			uint64_t theChannelId{};
			if (guildMemberId != 0) {
				if (this->voiceStates.contains(guildMemberId)) {
					theChannelId = this->voiceStates.at(guildMemberId).channelId;
				}
			} else {
				theChannelId = channelId;
			}
			std::string theShardId{ std::to_string((this->id >> 22) % this->discordCoreClient->shardingOptions.totalNumberOfShards) };
			auto theBaseSocketAgentIndex{ static_cast<int32_t>(
				floor(static_cast<float>(stod(theShardId)) / static_cast<float>(this->discordCoreClient->shardingOptions.totalNumberOfShards)) *
				static_cast<float>(std::thread::hardware_concurrency())) };
			getVoiceConnectionMap()[this->id] = std::make_unique<VoiceConnection>(this->discordCoreClient->baseSocketAgentMap[std::to_string(theBaseSocketAgentIndex)].get());
			this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
			DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
			voiceConnectInitData.currentShard = stoll(theShardId);
			voiceConnectInitData.channelId = theChannelId;
			voiceConnectInitData.guildId = this->id;
			voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
			voiceConnectInitData.selfDeaf = selfDeaf;
			voiceConnectInitData.selfMute = selfMute;
			this->voiceConnectionPtr->connect(voiceConnectInitData);
			return this->voiceConnectionPtr;
		} else {
			return nullptr;
		}
	}

	void Guild::disconnect() {
		if (getVoiceConnectionMap().contains(this->id) && getVoiceConnectionMap()[this->id]) {
			getVoiceConnectionMap()[this->id]->disconnect();
			SongAPI::stop(this->id);
			UpdateVoiceStateData updateVoiceData{};
			updateVoiceData.channelId = 0;
			updateVoiceData.selfDeaf = false;
			updateVoiceData.selfMute = false;
			updateVoiceData.guildId = this->id;
			this->discordCoreClient->getBotUser().updateVoiceStatus(updateVoiceData);
			getVoiceConnectionMap()[this->id].reset(nullptr);
			this->voiceConnectionPtr = nullptr;
		}
	}

	bool Guild::areWeConnected() {
		return getVoiceConnectionMap()[this->id]->areWeConnected();
	}

	void GuildData::initialize() {
		if (!getVoiceConnectionMap().contains(this->id)) {
			std::string theShardId{ std::to_string((this->id >> 22) % this->discordCoreClient->shardingOptions.totalNumberOfShards) };
			getVoiceConnectionMap()[this->id] = std::make_unique<VoiceConnection>(this->discordCoreClient->baseSocketAgentMap[theShardId].get());
		}
		this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
		if (!getYouTubeAPIMap().contains(this->id)) {
			getYouTubeAPIMap()[this->id] = std::make_unique<DiscordCoreInternal::YouTubeAPI>(this->id, this->discordCoreClient->httpClient.get());
		}
		if (!getSoundCloudAPIMap().contains(this->id)) {
			getSoundCloudAPIMap()[this->id] = std::make_unique<DiscordCoreInternal::SoundCloudAPI>(this->id, this->discordCoreClient->httpClient.get());
		}
		if (!getSongAPIMap().contains(this->id)) {
			getSongAPIMap()[this->id] = std::make_unique<SongAPI>(this->id);
		}
	}

	void Guilds::initialize(DiscordCoreInternal::HttpClient* theClient, DiscordCoreClient* discordCoreClientNew, bool doWeCacheNew) {
		Guilds::cache = std::make_unique<std::unordered_map<uint64_t, std::unique_ptr<GuildData>>>();
		Guilds::discordCoreClient = discordCoreClientNew;
		Guilds::doWeCache = doWeCacheNew;
		Guilds::httpClient = theClient;
	}

	CoRoutine<AuditLogData> Guilds::getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Audit_Logs);
		co_await NewThreadAwaitable<AuditLogData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Audit_Logs;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
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
		workload.callStack = "Guilds::getAuditLogDataAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<AuditLogData>(workload);
	}

	CoRoutine<Guild> Guilds::createGuildAsync(CreateGuildData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/guilds";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Guilds::createGuildAsync";
		auto guildNew = Guilds::httpClient->submitWorkloadAndGetResult<Guild>(workload);
		guildNew.discordCoreClient = Guilds::discordCoreClient;
		Guilds::insertGuild(guildNew);
		co_return guildNew;
	}

	CoRoutine<std::vector<GuildData>> Guilds::getAllGuildsAsync() {
		std::shared_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		co_await NewThreadAwaitable<std::vector<GuildData>>();
		std::vector<GuildData> guildVector{};
		for (auto& [key, value]: *Guilds::cache) {
			value->discordCoreClient = Guilds::discordCoreClient;
			guildVector.push_back(*value);
		}
		co_return guildVector;
	}

	CoRoutine<Guild> Guilds::getGuildAsync(GetGuildData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "?with_counts=true";
		workload.callStack = "Guilds::getGuildAsync";
		auto guildNew = Guilds::httpClient->submitWorkloadAndGetResult<Guild>(workload);
		guildNew = Guilds::getCachedGuildAsync({ .guildId = dataPackage.guildId }).get();
		guildNew.discordCoreClient = Guilds::discordCoreClient;
		co_return guildNew;
	}

	CoRoutine<GuildData> Guilds::getCachedGuildAsync(GetGuildData dataPackage) {
		std::shared_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		co_await NewThreadAwaitable<GuildData>();
		if (Guilds::cache->contains(dataPackage.guildId)) {
			co_return *(*Guilds::cache)[dataPackage.guildId];

		} else {
			auto guildNew = Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
			Guilds::insertGuild(guildNew);
			co_return guildNew;
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Preview);
		co_await NewThreadAwaitable<GuildPreviewData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Preview;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/preview";
		workload.callStack = "Guilds::getGuildPreviewAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildPreviewData>(workload);
	}

	CoRoutine<Guild> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId);
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Guilds::modifyGuildAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto guildNew = Guilds::httpClient->submitWorkloadAndGetResult<Guild>(workload);
		guildNew.discordCoreClient = Guilds::discordCoreClient;
		co_return guildNew;
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId);
		workload.callStack = "Guilds::deleteGuildAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<BanData>> Guilds::getGuildBansAsync(GetGuildBansData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Bans);
		co_await NewThreadAwaitable<std::vector<BanData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Bans;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
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
		workload.callStack = "Guilds::getGuildBansAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<std::vector<BanData>>(workload);
	}

	CoRoutine<BanData> Guilds::getGuildBanAsync(GetGuildBanData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Ban);
		co_await NewThreadAwaitable<BanData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Ban;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.userId);
		workload.callStack = "Guilds::getGuildBanAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<BanData>(workload);
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Guild_Ban);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Ban;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.guildMemberId);
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Guilds::createGuildBanAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Guilds::removeGuildBanAsync(RemoveGuildBanData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Ban);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Ban;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/bans/" + std::to_string(dataPackage.userId);
		workload.callStack = "Guilds::removeGuildBanAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildPruneCountData> Guilds::getGuildPruneCountAsync(GetGuildPruneCountData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Prune_Count);
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Prune_Count;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/prune";
		workload.callStack = "Guilds::getGuildPruneCountAsync";
		if (dataPackage.days != 0) {
			workload.relativePath += "?days=" + std::to_string(dataPackage.days);
			if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "&include_roles=";
				for (int32_t x = 0; x < dataPackage.includeRoles.size(); x += 1) {
					workload.relativePath += std::to_string(dataPackage.includeRoles[x]);
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
		} else if (dataPackage.includeRoles.size() > 0) {
			workload.relativePath += "?include_roles=";
			for (int32_t x = 0; x < dataPackage.includeRoles.size(); x += 1) {
				workload.relativePath += std::to_string(dataPackage.includeRoles[x]);
				if (x < dataPackage.includeRoles.size() - 1) {
					workload.relativePath += ",";
				}
			}
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload);
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Prune);
		co_await NewThreadAwaitable<GuildPruneCountData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Prune;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/prune";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Guilds::beginGuildPruneAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildPruneCountData>(workload);
	}

	CoRoutine<std::vector<VoiceRegionData>> Guilds::getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Voice_Regions);
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Voice_Regions;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/regions";
		workload.callStack = "Guilds::getGuildVoiceRegionsAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<std::vector<VoiceRegionData>>(workload);
	}

	CoRoutine<std::vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Invites);
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Invites;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/invites";
		workload.callStack = "Guilds::getGuildInvitesAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<std::vector<InviteData>>(workload);
	}

	CoRoutine<std::vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Integrations);
		co_await NewThreadAwaitable<std::vector<IntegrationData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Integrations;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/integrations";
		workload.callStack = "Guilds::getGuildIntegrationsAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<std::vector<IntegrationData>>(workload);
	}

	CoRoutine<void> Guilds::deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Integration);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Integration;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/integrations/" + std::to_string(dataPackage.integrationId);
		workload.callStack = "Guilds::deleteGuildIntegrationAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Settings);
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Settings;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget";
		workload.callStack = "Guilds::getGuildWidgetSettingsAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Widget);
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Widget;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget";
		nlohmann::json responseData = { { "channel_id", std::to_string(dataPackage.widgetData.channelId) }, { "enabled", dataPackage.widgetData.enabled } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::modifyGuildWidgetAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget);
		co_await NewThreadAwaitable<GuildWidgetData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/widget.nlohmann::json";
		workload.callStack = "Guilds::getGuildWidgetAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildWidgetData>(workload);
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Vanity_Invite);
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Vanity_Invite;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/vanity-url";
		workload.callStack = "Guilds::getGuildVanityInviteAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<InviteData>(workload);
	}

	CoRoutine<GuildWidgetImageData> Guilds::getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Image);
		co_await NewThreadAwaitable<GuildWidgetImageData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Image;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
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
		workload.callStack = "Guilds::getGuildWidgetImageAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildWidgetImageData>(workload);
	}

	CoRoutine<WelcomeScreenData> Guilds::getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Welcome_Screen);
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Welcome_Screen;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/welcome-screen";
		workload.callStack = "Guilds::getGuildWelcomeScreenAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload);
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Welcome_Screen);
		co_await NewThreadAwaitable<WelcomeScreenData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Welcome_Screen;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/welcome-screen";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Guilds::modifyGuildWelcomeScreenAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<WelcomeScreenData>(workload);
	};

	CoRoutine<GuildTemplateData> Guilds::getGuildTemplateAsync(GetGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::getGuildTemplateAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<Guild> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_From_Guild_Template);
		co_await NewThreadAwaitable<Guild>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_From_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
		nlohmann::json responseData = { { "name", dataPackage.name }, { "icon", dataPackage.imageData } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::createGuildFromGuildTemplateAsync";
		auto newGuild = Guilds::httpClient->submitWorkloadAndGetResult<Guild>(workload);
		newGuild.discordCoreClient = Guilds::discordCoreClient;
		co_return newGuild;
	}

	CoRoutine<std::vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Templates);
		co_await NewThreadAwaitable<std::vector<GuildTemplateData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Templates;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates";
		workload.callStack = "Guilds::getGuildTemplatesAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<std::vector<GuildTemplateData>>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates";
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::createGuildTemplateAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::syncGuildTemplateAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Template);
		co_await NewThreadAwaitable<GuildTemplateData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
		workload.content = responseData.dump();
		workload.callStack = "Guilds::modifyGuildTemplateAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<GuildTemplateData>(workload);
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Template);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Template;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/templates/" + dataPackage.templateCode;
		workload.callStack = "Guilds::deleteGuildTemplateAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<InviteData> Guilds::getInviteAsync(GetInviteData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Invite);
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Invite;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
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

		workload.callStack = "Guilds::getInviteAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<InviteData>(workload);
	}

	CoRoutine<void> Guilds::deleteInviteAsync(DeleteInviteData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Invite);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Invite;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/invites/" + dataPackage.inviteId;
		workload.callStack = "Guilds::deleteInviteAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Guilds::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Guild>> Guilds::getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Current_User_Guilds);
		co_await NewThreadAwaitable<std::vector<Guild>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Current_User_Guilds;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
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
		workload.callStack = "Users::getCurrentUserGuildsAsync";
		auto guildVector = Guilds::httpClient->submitWorkloadAndGetResult<std::vector<Guild>>(workload);
		for (auto& value: guildVector) {
			value.discordCoreClient = Guilds::discordCoreClient;
		}
		co_return guildVector;
	}

	CoRoutine<void> Guilds::leaveGuildAsync(LeaveGuildData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Leave_Guild);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Leave_Guild;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/users/@me/guilds/" + std::to_string(dataPackage.guildId);
		workload.callStack = "Users::leaveGuildAsync";
		co_return Guilds::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	void Guilds::insertGuild(GuildData guild) {
		std::unique_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		if (guild.id == 0) {
			return;
		}
		auto newCache = std::make_unique<std::unordered_map<uint64_t, std::unique_ptr<GuildData>>>();
		for (auto& [key, value]: *Guilds::cache) {
			(*newCache)[key] = std::move(value);
		}
		bool doWeShowIt{ false };
		if (!Guilds::cache->contains(guild.id) && Guilds::discordCoreClient->loggingOptions.logGeneralSuccessMessages) {
			doWeShowIt = true;
		}
		guild.initialize(doWeShowIt);
		if (Guilds::doWeCache) {
			(*newCache)[guild.id] = std::make_unique<GuildData>(guild);
		}
		Guilds::cache.reset(nullptr);
		Guilds::cache = std::move(newCache);
	}

	void Guilds::removeGuild(const uint64_t& guildId) {
		std::unique_lock<std::shared_mutex> theLock{ Guilds::theMutex };
		Guilds::cache->erase(guildId);
	};

	std::unique_ptr<std::unordered_map<uint64_t, std::unique_ptr<GuildData>>> Guilds::cache{};
	DiscordCoreInternal::HttpClient* Guilds::httpClient{ nullptr };
	DiscordCoreClient* Guilds::discordCoreClient{ nullptr };
	std::shared_mutex Guilds::theMutex{};
	bool Guilds::doWeCache{ false };

}