// GuildEntities.hpp - Header for the guild classes and structs.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_ENTITIES_
#define _GUILD_ENTITIES_

#include "../pch.h"
#include "VoiceConnectionClass.hpp"
#include "DataParsingFunctions.hpp"
#include "HttpClass.hpp"
#include "ChannelEntities.hpp"
#include "MessageEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "UserEntities.hpp"
#include "RoleEntities.hpp"
#include "YouTubeAPI.hpp"
#include "SongAPI.hpp"
#include "DiscordCoreClientBase.hpp"

namespace DiscordCoreAPI {

	struct CreateGuildBanData {
		string guildMemberId{ "" };
		int deleteMessageDays{ 0 };
		string guildId{ "" };
		string reason{ "" };
	};

	struct GetGuildData {
		string guildId{ "" };
	};

	struct FetchVanityInviteData {
		string guildId{ "" };
	};

	struct FetchGuildData {
		string guildId{ "" };
	};

	struct FetchAuditLogData {
		AuditLogEvent actionType{};
		unsigned int limit{ 0 };
		string guildId{ "" };
		string userId{ "" };
	};

	struct FetchInviteData {
		string inviteId{ "" };
	};

	struct FetchInvitesData {
		string guildId{ "" };
	};

	class Guild : public GuildData {
	public:

		friend struct Concurrency::details::_ResultHolder<Guild>;
		friend class DiscordCoreInternal::GuildManagerAgent;
		friend class DiscordCoreInternal::GuildManager;
		friend struct OnGuildCreationData;
		friend struct OnGuildDeletionData;
		friend struct OnGuildUpdateData;
		friend class DiscordCoreClient;

		shared_ptr<VoiceConnection>* connectToVoice(string channelId) {
			shared_ptr<VoiceConnection>* voiceConnectionPtr{ nullptr };
			if (DiscordCoreClientBase::voiceConnectionMap->contains(this->id)) {
				return  &DiscordCoreClientBase::voiceConnectionMap->at(this->id);
			}
			else if (channelId != "") {
				DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData;
				voiceConnectInitData.channelId = channelId;
				voiceConnectInitData.guildId = this->id;
				voiceConnectInitData.userId = DiscordCoreClientBase::currentUser.id;
				DiscordCoreClientBase::voiceConnectionMap->insert_or_assign(this->id, make_shared<VoiceConnection>(voiceConnectInitData, DiscordCoreClientBase::audioBuffersMap, DiscordCoreClientBase::thisPointer->webSocketConnectionAgent, this->id, DiscordCoreClientBase::voiceConnectionMap));
				DiscordGuild* discordGuild = new DiscordGuild(*this);
				YouTubeAPI::voiceConnectionMap = DiscordCoreClientBase::voiceConnectionMap;
				SoundCloudAPI::voiceConnectionMap = DiscordCoreClientBase::voiceConnectionMap;
				SongAPI::voiceConnectionMap = DiscordCoreClientBase::voiceConnectionMap;
				YouTubeAPI::discordGuildMap->insert(make_pair(this->id, discordGuild));
				SoundCloudAPI::discordGuildMap->insert(make_pair(this->id, discordGuild));
				SongAPI::discordGuildMap->insert(make_pair(this->id, discordGuild));
				YouTubeAPI::voiceConnectionMap->insert_or_assign(this->id, DiscordCoreClientBase::voiceConnectionMap->at(this->id));
				SoundCloudAPI::voiceConnectionMap->insert_or_assign(this->id, DiscordCoreClientBase::voiceConnectionMap->at(this->id));
				SongAPI::voiceConnectionMap->insert_or_assign(this->id, DiscordCoreClientBase::voiceConnectionMap->at(this->id));
				auto youtubeAPI = make_shared<YouTubeAPI>(this->id);
				DiscordCoreClientBase::youtubeAPIMap->insert_or_assign(this->id, youtubeAPI);
				auto soundCloudAPI = make_shared<SoundCloudAPI>(this->id);
				DiscordCoreClientBase::soundCloudAPIMap->insert(make_pair(this->id, soundCloudAPI));
				auto songAPI = make_shared<SongAPI>(discordGuild);
				DiscordCoreClientBase::songAPIMap->insert(make_pair(this->id, songAPI));
				this->areWeConnectedBool = true;
				return &DiscordCoreClientBase::voiceConnectionMap->at(this->id);
			}
			return voiceConnectionPtr;
		}

		void disconnect() {
			if (DiscordCoreClientBase::voiceConnectionMap->contains(this->id)) {
				if (DiscordCoreClientBase::songAPIMap->contains(this->id)) {
					SongAPI::stop(this->id);
					DiscordGuild discordGuild(*this);
					discordGuild.data.playlist = SongAPI::getPlaylist(this->id);
					discordGuild.writeDataToDB();
					DiscordCoreClientBase::songAPIMap->erase(this->id);
				}
				shared_ptr<VoiceConnection>* voiceConnection = &DiscordCoreClientBase::voiceConnectionMap->at(this->id);
				(*voiceConnection)->stop();
				if ((*voiceConnection)->areWePlaying) {
					(*voiceConnection)->areWePlaying = false;
					(*voiceConnection)->areWeStopping = true;
				}
				(*voiceConnection)->doWeQuit = true;
				if ((*voiceConnection)->encoder != nullptr) {
					opus_encoder_destroy((*voiceConnection)->encoder);
					(*voiceConnection)->encoder = nullptr;
				}
				(*voiceConnection)->hasTerminateRun = true;
				DiscordCoreClientBase::voiceConnectionMap->at(this->id)->voiceChannelWebSocketAgent->~VoiceChannelWebSocketAgent();
				DiscordCoreClientBase::voiceConnectionMap->erase(this->id);
				if (DiscordCoreClientBase::audioBuffersMap->contains(this->id)) {
					send(DiscordCoreClientBase::audioBuffersMap->at(this->id).get(), AudioFrameData{ .type = AudioFrameType::Cancel });
					AudioFrameData frameData{};
					while (try_receive(DiscordCoreClientBase::audioBuffersMap->at(this->id).get(), frameData)) {};
					DiscordCoreClientBase::audioBuffersMap->erase(this->id);
					YouTubeAPI::sendAudioDataBufferMap->erase(this->id);
					SoundCloudAPI::sendAudioDataBufferMap->erase(this->id);
					SongAPI::sendAudioDataBufferMap->erase(this->id);
					SoundCloudAPI::voiceConnectionMap->erase(this->id);
					YouTubeAPI::voiceConnectionMap->erase(this->id);
					SongAPI::voiceConnectionMap->erase(this->id);
					SongAPI::songAPIMap->erase(this->id);
					SoundCloudAPI::soundCloudAPIMap->erase(this->id);
					YouTubeAPI::youtubeAPIMap->erase(this->id);
				}
				this->areWeConnectedBool = false;
			}
		}

		bool areWeConnected() {
			return this->areWeConnectedBool;
		}

	protected:

		bool areWeConnectedBool{ false };

		Guild() {};

		Guild(GuildData dataNew) {
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
			this->vanityURLCode = dataNew.vanityURLCode;
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
		}

		void initialize() {
			try {
				cout << "Caching guild: " << this->name << endl;
				cout << "Caching channels for guild: " << this->name << endl;
				for (auto value : channels) {
					value.guildId = this->id;
					ChannelData channelData = value;
					Channel channel(channelData);
					DiscordCoreClientBase::thisPointer->channels->insertChannelAsync(channel).get();
				}
				cout << "Caching guild members for guild: " << this->name << endl;
				for (unsigned int x = 0; x < this->members.size(); x += 1) {
					GuildMemberData guildMemberData = this->members.at(x);
					for (auto value : this->voiceStates) {
						if (value.userId == guildMemberData.user.id) {
							guildMemberData.voiceData = value;
						}
					}
					guildMemberData.guildId = this->id;
					DiscordGuildMember discordGuildMember(guildMemberData);
					discordGuildMember.writeDataToDB();
					GuildMember guildMember(guildMemberData, this->id);
					DiscordCoreClientBase::thisPointer->guildMembers->insertGuildMemberAsync(guildMember, this->id).get();
				}
				cout << "Caching roles for guild: " << this->name << endl;
				for (auto const& value : roles) {
					RoleData roleData = value;
					Role role(roleData);
					DiscordCoreClientBase::thisPointer->roles->insertRoleAsync(role).get();
				}
				cout << "Caching users for guild: " << this->name << endl << endl;
				for (auto value : members) {
					UserData userData = value.user;
					User user(userData);
					DiscordCoreClientBase::thisPointer->users->insertUserAsync(user).get();
				}
			}
			catch (...) {
				rethrowException("Guild::initialize() Error: ");
			}
		}

	};
};

namespace DiscordCoreInternal {

	class GuildManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class GuildManager;
		
		static overwrite_buffer<map<string, DiscordCoreAPI::Guild>>* cache;
		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<vector<DiscordCoreAPI::InviteData>> outInvitesBuffer{ nullptr };
		unbounded_buffer<GetVanityInviteData> requestGetVanityInviteBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::AuditLogData> outAuditLogBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::InviteData> outInviteBuffer{ nullptr };
		unbounded_buffer<CollectGuildData> requestCollectGuildBuffer{ nullptr };
		unbounded_buffer<PutGuildBanData> requestPutGuildBanBuffer{ nullptr };
		unbounded_buffer<GetAuditLogData> requestGetAuditLogBuffer{ nullptr };
		unbounded_buffer<GetInvitesData> requestGetInvitesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::BanData> outBanBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Guild> outGuildBuffer{ nullptr };
		unbounded_buffer<GetInviteData> requestGetInviteBuffer{ nullptr };
		unbounded_buffer<GetGuildData> requestGetGuildBuffer{ nullptr };
		concurrent_queue<DiscordCoreAPI::Guild> guildsToInsert{};

		GuildManagerAgent()
			: agent(*GuildManagerAgent::threadContext->scheduler->scheduler) {}

		static void initialize() {
			GuildManagerAgent::cache = new overwrite_buffer<map<string, DiscordCoreAPI::Guild>>();
			GuildManagerAgent::threadContext = ThreadManager::getThreadContext().get();
		}

		static void cleanup() {
			if (GuildManagerAgent::cache != nullptr) {
				auto cacheNew = receive(GuildManagerAgent::cache);
				for (auto [key, value] : cacheNew) {
					value.disconnect();
				}
				GuildManagerAgent::threadContext->releaseGroup();
				delete GuildManagerAgent::cache;
				GuildManagerAgent::cache = nullptr;
			}
		}

		DiscordCoreAPI::Guild getObjectData(GetGuildData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			HttpRequestAgent requestAgent{};
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildManagerAgent::getObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::GuildData guildData;
			DataParser::parseObject(returnData.data, &guildData);
			DiscordCoreAPI::Guild guildNew(guildData);
			return guildNew;
		}

		DiscordCoreAPI::InviteData getObjectData(GetInviteData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_INVITE;
			workload.relativePath = "/invites/" + dataPackage.inviteId + "?with_counts=true&with_expiration=true";
			HttpRequestAgent requestAgent{};
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildManagerAgent::getObjectData_01");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::InviteData inviteData;
			DataParser::parseObject(returnData.data, &inviteData);
			return inviteData;
		}

		vector<DiscordCoreAPI::InviteData> getObjectData(GetInvitesData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_INVITES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/invites";
			HttpRequestAgent requestAgent{};
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildManagerAgent::getObjectData_02");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<DiscordCoreAPI::InviteData> inviteData;
			for (auto value : returnData.data) {
				DiscordCoreAPI::InviteData inviteDataNew;
				DataParser::parseObject(value, &inviteDataNew);
				inviteData.push_back(inviteDataNew);
			}
			return inviteData;
		}

		DiscordCoreAPI::InviteData getObjectData(GetVanityInviteData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_VANITY_INVITE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/vanity-url";
			HttpRequestAgent requestAgent{};
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildManagerAgent::getObjectData_03");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_03 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_03 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::InviteData inviteData;
			DataParser::parseObject(returnData.data, &inviteData);
			return inviteData;
		}

		DiscordCoreAPI::AuditLogData getObjectData(GetAuditLogData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_AUDIT_LOG;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/audit-logs";
			if (dataPackage.userId != "") {
				workload.relativePath += "?user_id=" + dataPackage.userId;
				if (to_string((int)dataPackage.actionType) != "") {
					workload.relativePath += "&action_type=" + to_string((int)dataPackage.actionType);
				}
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (to_string((int)dataPackage.actionType) != "") {
				workload.relativePath += "?action_type=" + to_string((int)dataPackage.actionType);
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + to_string(dataPackage.limit);
			}
			HttpRequestAgent requestAgent{};
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildManagerAgent::getObjectData_04");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_04 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_04 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::AuditLogData auditLogData;
			DataParser::parseObject(returnData.data, &auditLogData);
			return auditLogData;
		}

		DiscordCoreAPI::BanData putObjectData(PutGuildBanData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.workloadType = HttpWorkloadType::PUT_GUILD_BAN;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
			workload.content = getAddBanPayload(dataPackage);
			HttpRequestAgent requestAgent{};
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildManagerAgent::putObjectData_00");
			DiscordCoreAPI::BanData banData;
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				banData.failedDueToPerms = true;
			}
			else {
				cout << "GuildManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DataParser::parseObject(returnData.data, &banData);
			return banData;
		}

		void run() {
			try {
				CollectGuildData dataPackage01;
				if (try_receive(this->requestCollectGuildBuffer, dataPackage01)) {
					map<string, DiscordCoreAPI::Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId)) {
							DiscordCoreAPI::Guild guild = cacheTemp.at(dataPackage01.guildId);
							send(this->outGuildBuffer, guild);
						}
					}
				}
				GetGuildData dataPackage02;
				if (try_receive(this->requestGetGuildBuffer, dataPackage02)) {
					map<string, DiscordCoreAPI::Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.guildId)) {
							cacheTemp.erase(dataPackage02.guildId);
						}
					}
					DiscordCoreAPI::Guild guild = getObjectData(dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.guildId, guild));
					send(this->outGuildBuffer, guild);
					send(GuildManagerAgent::cache, cacheTemp);
				}
				GetAuditLogData dataPackage03;
				if (try_receive(this->requestGetAuditLogBuffer, dataPackage03)) {
					DiscordCoreAPI::AuditLogData auditLog = getObjectData(dataPackage03);
					send(this->outAuditLogBuffer, auditLog);
				}
				GetInvitesData dataPackage04;
				if (try_receive(this->requestGetInvitesBuffer, dataPackage04)) {
					vector<DiscordCoreAPI::InviteData> inviteData = getObjectData(dataPackage04);
					send(this->outInvitesBuffer, inviteData);
				}
				GetInviteData dataPackage05;
				if (try_receive(this->requestGetInviteBuffer, dataPackage05)) {
					DiscordCoreAPI::InviteData inviteData = getObjectData(dataPackage05);
					send(this->outInviteBuffer, inviteData);
				}
				GetVanityInviteData dataPackage06;
				if (try_receive(this->requestGetVanityInviteBuffer, dataPackage06)) {
					DiscordCoreAPI::InviteData inviteData = getObjectData(dataPackage06);
					send(this->outInviteBuffer, inviteData);
				}
				PutGuildBanData dataPackage07;
				if (try_receive(this->requestPutGuildBanBuffer, dataPackage07)) {
					DiscordCoreAPI::BanData inviteData = putObjectData(dataPackage07);
					send(this->outBanBuffer, inviteData);
				}
				DiscordCoreAPI::Guild guildNew;
				while (this->guildsToInsert.try_pop(guildNew)) {
					map<string, DiscordCoreAPI::Guild> cacheTemp;
					try_receive(GuildManagerAgent::cache, cacheTemp);
					if (cacheTemp.contains(guildNew.id)) {
						cacheTemp.erase(guildNew.id);
					}
					else {
						guildNew.initialize();
					}					
					cacheTemp.insert(make_pair(guildNew.id, guildNew));
					send(GuildManagerAgent::cache, cacheTemp);
				}
			}
			catch (...) {
				DiscordCoreAPI::rethrowException("GuildManagerAgent::run() Error: ");
			}
			this->done();
		}

	};

	class GuildManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::PermissionsConverter;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::YouTubeAPI;
		friend class DiscordCoreAPI::Guilds;

		GuildManager(GuildManager* pointer) : ThreadContext(*ThreadManager::getThreadContext().get()) {
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		task<DiscordCoreAPI::Guild> fetchAsync(DiscordCoreAPI::FetchGuildData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetGuildData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent{};
			send(requestAgent.requestGetGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::GuildData guildData;
			DiscordCoreAPI::Guild guildNew(guildData);
			try_receive(requestAgent.outGuildBuffer, guildNew);
			co_await mainThread;
			co_return guildNew;
		}

		task<vector<DiscordCoreAPI::InviteData>> fetchInvitesAsync(DiscordCoreAPI::FetchInvitesData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetInvitesData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent{};
			send(requestAgent.requestGetInvitesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			vector<DiscordCoreAPI::InviteData> inviteData;
			try_receive(requestAgent.outInvitesBuffer, inviteData);
			co_await mainThread;
			co_return inviteData;
		}

		task<DiscordCoreAPI::BanData> createGuildBanAsync(DiscordCoreAPI::CreateGuildBanData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			PutGuildBanData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			dataPackageNew.deleteMessageDays = dataPackage.deleteMessageDays;
			dataPackageNew.reason = dataPackage.reason;
			GuildManagerAgent requestAgent{};
			send(requestAgent.requestPutGuildBanBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::BanData banData;
			try_receive(requestAgent.outBanBuffer, banData);
			co_await mainThread;
			co_return banData;
		}

		task<DiscordCoreAPI::InviteData> fetchVanityInviteAsync(DiscordCoreAPI::FetchVanityInviteData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetVanityInviteData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent{};
			send(requestAgent.requestGetVanityInviteBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::InviteData inviteData;
			try_receive(requestAgent.outInviteBuffer, inviteData);
			co_await mainThread;
			co_return inviteData;
		}

		task<DiscordCoreAPI::InviteData> fetchInviteAsync(DiscordCoreAPI::FetchInviteData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetInviteData dataPackageNew;
			dataPackageNew.inviteId = dataPackage.inviteId;
			GuildManagerAgent requestAgent{};
			send(requestAgent.requestGetInviteBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::InviteData inviteData;
			try_receive(requestAgent.outInviteBuffer, inviteData);
			co_await mainThread;
			co_return inviteData;
		}

		task<DiscordCoreAPI::AuditLogData> fetchAuditLogDataAsync(DiscordCoreAPI::FetchAuditLogData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetAuditLogData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.actionType = (AuditLogEvent)dataPackage.actionType;
			dataPackageNew.limit = dataPackage.limit;
			dataPackageNew.userId = dataPackage.userId;
			GuildManagerAgent requestAgent{};
			send(requestAgent.requestGetAuditLogBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::AuditLogData auditLog;
			try_receive(requestAgent.outAuditLogBuffer, auditLog);
			co_await mainThread;
			co_return auditLog;
		}

		task<DiscordCoreAPI::Guild> getGuildAsync(DiscordCoreAPI::GetGuildData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			CollectGuildData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent{};
			send(requestAgent.requestCollectGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::GuildData guildData;
			DiscordCoreAPI::Guild guildNew(guildData);
			try_receive(requestAgent.outGuildBuffer, guildNew);
			co_await mainThread;
			co_return guildNew;
		}

		task<vector<DiscordCoreAPI::Guild>> getAllGuildsAsync() {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Guild>cache;
			try_receive(GuildManagerAgent::cache, cache);
			vector<DiscordCoreAPI::Guild> guildVector;
			for (auto [key, value] : cache) {
				guildVector.push_back(value);
			}
			send(GuildManagerAgent::cache, cache);
			co_await mainThread;
			co_return guildVector;
		}

		task<void> insertGuildAsync(DiscordCoreAPI::Guild guild) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GuildManagerAgent requestAgent{};
			requestAgent.guildsToInsert.push(guild);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> removeGuildAsync(string guildId) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Guild> cache;
			try_receive(GuildManagerAgent::cache, cache);
			if (cache.contains(guildId)) {
				cache.erase(guildId);
			}
			send(GuildManagerAgent::cache, cache);
			co_await mainThread;
			co_return;
		}
	};
	overwrite_buffer<map<string, DiscordCoreAPI::Guild>>* GuildManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> GuildManagerAgent::threadContext{ nullptr };
}
#endif