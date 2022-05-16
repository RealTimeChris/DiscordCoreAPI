/*
*
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
/// Chris M.
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

	VoiceConnection* Guild::connectToVoice(std::string channelId, bool selfDeaf, bool selfMute) {
		try {
			if (getVoiceConnectionMap()[this->id]->areWeConnected()) {
				this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
				return this->voiceConnectionPtr;
			} else if (channelId != "") {
				std::string theShardId{ std::to_string((stoll(this->id) >> 22) % this->discordCoreClient->shardingOptions.totalNumberOfShards) };
				getVoiceConnectionMap()[this->id] = std::make_unique<VoiceConnection>(this->discordCoreClient->webSocketMap[theShardId].get());
				this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
				DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
				voiceConnectInitData.channelId = channelId;
				voiceConnectInitData.guildId = this->id;
				voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
				voiceConnectInitData.selfDeaf = selfDeaf;
				voiceConnectInitData.selfMute = selfMute;
				this->voiceConnectionPtr->connect(voiceConnectInitData);
				return this->voiceConnectionPtr;
			} else {
				return nullptr;
			}
		} catch (...) {
			reportException("Guild::connectToVoice()");
		}
		return nullptr;
	}

	void Guild::disconnect() {
		try {
			if (getVoiceConnectionMap().contains(this->id) && getVoiceConnectionMap()[this->id]) {
				getVoiceConnectionMap()[this->id]->disconnect();
				SongAPI::stop(this->id);
				UpdateVoiceStateData updateVoiceData{};
				updateVoiceData.channelId = "";
				updateVoiceData.selfDeaf = false;
				updateVoiceData.selfMute = false;
				updateVoiceData.guildId = this->id;
				this->discordCoreClient->getBotUser().updateVoiceStatus(updateVoiceData);
				getVoiceConnectionMap()[this->id].reset(nullptr);
				this->voiceConnectionPtr = nullptr;
			}
		} catch (...) {
			reportException("Guild::disconnect()");
		}
	}

	bool Guild::areWeConnected() {
		try {
			this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
			return this->voiceConnectionPtr->areWeConnected();
		} catch (...) {
			reportException("Guild::areWeConnected()");
		}
		return false;
	}

	void Guild::initialize(bool doWeShowIt) {
		try {
			if (this->discordCoreClient->cacheOptions.cacheGuilds && doWeShowIt) {
				std::cout << shiftToBrightBlue() << "Caching Guild: " << this->id << reset() << std::endl;
			}
			if (!getVoiceConnectionMap().contains(this->id)) {
				std::string theShardId{ std::to_string((stoll(this->id) >> 22) % this->discordCoreClient->shardingOptions.totalNumberOfShards) };
				getVoiceConnectionMap().insert(std::make_pair(this->id, std::make_unique<VoiceConnection>(this->discordCoreClient->webSocketMap[theShardId].get())));
			}
			this->voiceConnectionPtr = getVoiceConnectionMap()[this->id].get();
			if (!getYouTubeAPIMap().contains(this->id)) {
				getYouTubeAPIMap().insert(std::make_pair(this->id, std::make_unique<DiscordCoreInternal::YouTubeAPI>(this->id, this->discordCoreClient->httpClient.get())));
			}
			if (!getSoundCloudAPIMap().contains(this->id)) {
				getSoundCloudAPIMap().insert(std::make_pair(this->id, std::make_unique<DiscordCoreInternal::SoundCloudAPI>(this->id, this->discordCoreClient->httpClient.get())));
			}
			if (!getSongAPIMap().contains(this->id)) {
				getSongAPIMap().insert(std::make_pair(this->id, std::make_unique<SongAPI>(this->id)));
			}
		} catch (...) {
			reportException("Guild::initialize()");
		}
	}

	void Guilds::initialize(DiscordCoreInternal::HttpClient* theClient, DiscordCoreClient* discordCoreClientNew) {
		Guilds::discordCoreClient = discordCoreClientNew;
		Guilds::httpClient = theClient;
	}

	CoRoutine<AuditLogData> Guilds::getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Audit_Logs);
			co_await NewThreadAwaitable<AuditLogData>();
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
				if (dataPackage.before != "") {
					workload.relativePath += "&before=" + dataPackage.before;
				}
			} else if (std::to_string(static_cast<int32_t>(dataPackage.actionType)) != "") {
				workload.relativePath += "?action_type=" + std::to_string(static_cast<int32_t>(dataPackage.actionType));
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
				if (dataPackage.before != "") {
					workload.relativePath += "&before=" + dataPackage.before;
				}
			} else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
				if (dataPackage.before != "") {
					workload.relativePath += "&before=" + dataPackage.before;
				}
			} else if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
			}
			workload.callStack = "Guilds::getAuditLogDataAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<AuditLogData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildAuditLogsAsync()");
		}
	}

	CoRoutine<Guild> Guilds::createGuildAsync(CreateGuildData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild);
			co_await NewThreadAwaitable<Guild>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::createGuildAsync";
			auto guildNew = DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(*Guilds::httpClient, workload);
			guildNew.discordCoreClient = Guilds::discordCoreClient;
			Guilds::insertGuild(guildNew);
			co_return guildNew;
		} catch (...) {
			reportException("Guilds::createGuildAsync()");
		}
	}

	CoRoutine<std::vector<Guild>> Guilds::getAllGuildsAsync() {
		try {
			co_await NewThreadAwaitable<std::vector<Guild>>();
			std::vector<Guild> guildVector{};
			for (auto& [key, value]: Guilds::cache) {
				value.discordCoreClient = Guilds::discordCoreClient;
				guildVector.push_back(value);
			}
			co_return guildVector;
		} catch (...) {
			reportException("Guilds::getAllGuildsAsync()");
		}
	}

	CoRoutine<Guild> Guilds::getGuildAsync(GetGuildData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild);
			co_await NewThreadAwaitable<Guild>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "?with_counts=true";
			workload.callStack = "Guilds::getGuildAsync";
			auto guildNew = DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(*Guilds::httpClient, workload);
			guildNew.discordCoreClient = Guilds::discordCoreClient;
			co_return guildNew;
		} catch (...) {
			reportException("Guilds::getGuildAsync()");
		}
	}

	CoRoutine<Guild> Guilds::getCachedGuildAsync(GetGuildData dataPackage) {
		try {
			co_await NewThreadAwaitable<Guild>();
			if (Guilds::cache.contains(dataPackage.guildId)) {
				co_return Guilds::cache[dataPackage.guildId];

			} else {
				auto guildNew = Guilds::getGuildAsync({ .guildId = dataPackage.guildId }).get();
				Guilds::insertGuild(guildNew);
				co_return guildNew;
			}
		} catch (...) {
			reportException("Guilds::getCachedGuildAsync()");
		}
	}

	CoRoutine<GuildPreviewData> Guilds::getGuildPreviewAsync(GetGuildPreviewData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Preview);
			co_await NewThreadAwaitable<GuildPreviewData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Preview;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/preview";
			workload.callStack = "Guilds::getGuildPreviewAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildPreviewData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildPreviewAsync()");
		}
	}

	CoRoutine<Guild> Guilds::modifyGuildAsync(ModifyGuildData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild);
			co_await NewThreadAwaitable<Guild>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::modifyGuildAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			auto guildNew = DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(*Guilds::httpClient, workload);
			guildNew.discordCoreClient = Guilds::discordCoreClient;
			co_return guildNew;
		} catch (...) {
			reportException("Guilds::modifyGuildAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteGuildAsync(DeleteGuildData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			workload.callStack = "Guilds::deleteGuildAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::deleteGuildAsync()");
		}
	}

	CoRoutine<std::vector<BanData>> Guilds::getGuildBansAsync(GetGuildBansData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Bans);
			co_await NewThreadAwaitable<std::vector<BanData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Bans;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans";
			if (dataPackage.after != "") {
				workload.relativePath += "?after=" + dataPackage.after;
				if (dataPackage.before != "") {
					workload.relativePath += "&before=" + dataPackage.before;
				}
				if (dataPackage.limit != "") {
					workload.relativePath += "&limit=" + dataPackage.limit;
				}
			} else if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
				if (dataPackage.limit != "") {
					workload.relativePath += "&limit=" + dataPackage.limit;
				}
			} else if (dataPackage.limit != "") {
				workload.relativePath += "?limit=" + dataPackage.limit;
			}
			workload.callStack = "Guilds::getGuildBansAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<BanData>>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildBansAsync()");
		}
	}

	CoRoutine<BanData> Guilds::getGuildBanAsync(GetGuildBanData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Ban);
			co_await NewThreadAwaitable<BanData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Ban;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
			workload.callStack = "Guilds::getGuildBanAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<BanData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildBanAsync()");
		}
	}

	CoRoutine<void> Guilds::createGuildBanAsync(CreateGuildBanData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Guild_Ban);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Ban;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::createGuildBanAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::createGuildBanAsync()");
		}
	}

	CoRoutine<void> Guilds::removeGuildBanAsync(RemoveGuildBanData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Ban);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Ban;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.userId;
			workload.callStack = "Guilds::removeGuildBanAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::removeGuildBanAsync()");
		}
	}

	CoRoutine<GuildPruneCountData> Guilds::getGuildPruneCountAsync(GetGuildPruneCountData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Prune_Count);
			co_await NewThreadAwaitable<GuildPruneCountData>();
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
			} else if (dataPackage.includeRoles.size() > 0) {
				workload.relativePath += "?include_roles=";
				for (int32_t x = 0; x < dataPackage.includeRoles.size(); x += 1) {
					workload.relativePath += dataPackage.includeRoles[x];
					if (x < dataPackage.includeRoles.size() - 1) {
						workload.relativePath += ",";
					}
				}
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildPruneCountData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildPruneCountAsync()");
		}
	}

	CoRoutine<GuildPruneCountData> Guilds::beginGuildPruneAsync(BeginGuildPruneData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Prune);
			co_await NewThreadAwaitable<GuildPruneCountData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Prune;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/prune";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::beginGuildPruneAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildPruneCountData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::beginGuildPruneAsync()");
		}
	}

	CoRoutine<std::vector<VoiceRegionData>> Guilds::getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Voice_Regions);
			co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Voice_Regions;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/regions";
			workload.callStack = "Guilds::getGuildVoiceRegionsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<VoiceRegionData>>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildVoiceRegionsAsync()");
		}
	}

	CoRoutine<std::vector<InviteData>> Guilds::getGuildInvitesAsync(GetGuildInvitesData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Invites);
			co_await NewThreadAwaitable<std::vector<InviteData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Invites;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/invites";
			workload.callStack = "Guilds::getGuildInvitesAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<InviteData>>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildInvitesAsync()");
		}
	}

	CoRoutine<std::vector<IntegrationData>> Guilds::getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Integrations);
			co_await NewThreadAwaitable<std::vector<IntegrationData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Integrations;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/integrations";
			workload.callStack = "Guilds::getGuildIntegrationsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<IntegrationData>>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildIntegrationsAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Integration);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Integration;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/integrations/" + dataPackage.integrationId;
			workload.callStack = "Guilds::deleteGuildIntegrationAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::deleteGuildIntegrationAsync()");
		}
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Settings);
			co_await NewThreadAwaitable<GuildWidgetData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Settings;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
			workload.callStack = "Guilds::getGuildWidgetSettingsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildWidgetSettingsAsync()");
		}
	}

	CoRoutine<GuildWidgetData> Guilds::modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Widget);
			co_await NewThreadAwaitable<GuildWidgetData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Widget;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget";
			nlohmann::json responseData = { { "channel_id", dataPackage.widgetData.channelId }, { "enabled", dataPackage.widgetData.enabled } };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::modifyGuildWidgetAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::modifyGuildWidgetAsync()");
		}
	}

	CoRoutine<GuildWidgetData> Guilds::getGuildWidgetAsync(GetGuildWidgetData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget);
			co_await NewThreadAwaitable<GuildWidgetData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/widget.nlohmann::json";
			workload.callStack = "Guilds::getGuildWidgetAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildWidgetAsync()");
		}
	}

	CoRoutine<InviteData> Guilds::getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Vanity_Invite);
			co_await NewThreadAwaitable<InviteData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Vanity_Invite;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/vanity-url";
			workload.callStack = "Guilds::getGuildVanityInviteAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<InviteData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildVanityInviteAsync()");
		}
	}

	CoRoutine<GuildWidgetImageData> Guilds::getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Widget_Image);
			co_await NewThreadAwaitable<GuildWidgetImageData>();
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
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildWidgetImageData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildWidgetImageAsync()");
		}
	}

	CoRoutine<WelcomeScreenData> Guilds::getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Welcome_Screen);
			co_await NewThreadAwaitable<WelcomeScreenData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Welcome_Screen;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
			workload.callStack = "Guilds::getGuildWelcomeScreenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WelcomeScreenData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildWelcomeScreenAsync()");
		}
	}

	CoRoutine<WelcomeScreenData> Guilds::modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Welcome_Screen);
			co_await NewThreadAwaitable<WelcomeScreenData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Welcome_Screen;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/welcome-screen";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Guilds::modifyGuildWelcomeScreenAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WelcomeScreenData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::modifyGuildWelcomeScreenAsync()");
		}
	};

	CoRoutine<GuildTemplateData> Guilds::getGuildTemplateAsync(GetGuildTemplateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Template);
			co_await NewThreadAwaitable<GuildTemplateData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
			workload.callStack = "Guilds::getGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildTemplateAsync()");
		}
	}

	CoRoutine<Guild> Guilds::createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_From_Guild_Template);
			co_await NewThreadAwaitable<Guild>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_From_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/templates/" + dataPackage.templateCode;
			nlohmann::json responseData = { { "name", dataPackage.name }, { "icon", dataPackage.imageData } };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::createGuildFromGuildTemplateAsync";
			auto newGuild = DiscordCoreInternal::submitWorkloadAndGetResult<Guild>(*Guilds::httpClient, workload);
			newGuild.discordCoreClient = Guilds::discordCoreClient;
			co_return newGuild;
		} catch (...) {
			reportException("Guilds::createGuildFromGuildTemplateAsync()");
		}
	}

	CoRoutine<std::vector<GuildTemplateData>> Guilds::getGuildTemplatesAsync(GetGuildTemplatesData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Templates);
			co_await NewThreadAwaitable<std::vector<GuildTemplateData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Templates;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
			workload.callStack = "Guilds::getGuildTemplatesAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildTemplateData>>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getGuildTemplatesAsync()");
		}
	}

	CoRoutine<GuildTemplateData> Guilds::createGuildTemplateAsync(CreateGuildTemplateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Template);
			co_await NewThreadAwaitable<GuildTemplateData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates";
			nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::createGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::createGuildTemplateAsync()");
		}
	}

	CoRoutine<GuildTemplateData> Guilds::syncGuildTemplateAsync(SyncGuildTemplateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Guild_Template);
			co_await NewThreadAwaitable<GuildTemplateData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
			workload.callStack = "Guilds::syncGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::syncGuildTemplateAsync()");
		}
	}

	CoRoutine<GuildTemplateData> Guilds::modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Template);
			co_await NewThreadAwaitable<GuildTemplateData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
			nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name } };
			workload.content = responseData.dump();
			workload.callStack = "Guilds::modifyGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildTemplateData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::modifyGuildTemplateAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Template);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Template;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/templates/" + dataPackage.templateCode;
			workload.callStack = "Guilds::deleteGuildTemplateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::deleteGuildTemplateAsync()");
		}
	}

	CoRoutine<InviteData> Guilds::getInviteAsync(GetInviteData dataPackage) {
		try {
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
				if (dataPackage.guildScheduledEventId != "") {
					workload.relativePath += "&guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
				}
			} else if (dataPackage.withExpiration) {
				workload.relativePath += "?with_expiration=true";
				if (dataPackage.guildScheduledEventId != "") {
					workload.relativePath += "&guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
				}
			} else if (dataPackage.guildScheduledEventId != "") {
				workload.relativePath += "?guild_scheduled_event_id=" + dataPackage.guildScheduledEventId;
			}

			workload.callStack = "Guilds::getInviteAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<InviteData>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::getInviteAsync()");
		}
	}

	CoRoutine<void> Guilds::deleteInviteAsync(DeleteInviteData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Invite);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Invite;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/invites/" + dataPackage.inviteId;
			workload.callStack = "Guilds::deleteInviteAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Guilds::deleteInviteAsync()");
		}
	}

	CoRoutine<std::vector<Guild>> Guilds::getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Current_User_Guilds);
			co_await NewThreadAwaitable<std::vector<Guild>>();
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
			} else if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + dataPackage.limit;
				}
			} else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + dataPackage.limit;
			}
			workload.callStack = "Users::getCurrentUserGuildsAsync";
			auto guildVector = DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Guild>>(*Guilds::httpClient, workload);
			for (auto& value: guildVector) {
				value.discordCoreClient = Guilds::discordCoreClient;
			}
			co_return guildVector;
		} catch (...) {
			reportException("Users::getCurrentUserGuildsAsync()");
		}
	}

	CoRoutine<void> Guilds::leaveGuildAsync(LeaveGuildData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Leave_Guild);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Leave_Guild;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/users/@me/guilds/" + dataPackage.guildId;
			workload.callStack = "Users::leaveGuildAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Guilds::httpClient, workload);
		} catch (...) {
			reportException("Users::leaveGuildAsync()");
		}
	}

	void Guilds::insertGuild(Guild guild) {
		try {
			if (guild.id == "") {
				return;
			}
			bool doWeShowIt{ false };
			if (!Guilds::cache.contains(guild.id)) {
				doWeShowIt = true;
			}
			guild.initialize(doWeShowIt);
			Guilds::cache.insert_or_assign(guild.id, guild);
		} catch (...) {
			reportException("Guilds::insertGuild()");
		}
	}

	void Guilds::removeGuild(std::string guildId) {
		try {
			Guilds::cache.erase(guildId);
		} catch (...) {
			reportException("Guilds::removeGuild()");
		}
	};

	DiscordCoreInternal::HttpClient* Guilds::httpClient{ nullptr };
	DiscordCoreClient* Guilds::discordCoreClient{ nullptr };
	std::unordered_map<std::string, Guild> Guilds::cache{};

}