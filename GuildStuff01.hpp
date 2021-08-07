// GuildStuff01.hpp - First header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_01_
#define _GUILD_STUFF_01_

#include "../pch.h"
#include "DataParsingFunctions.hpp"
#include "HttpStuff.hpp"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"
#include "VoiceConnectionStuff.hpp"
#include "DiscordCoreClientBase.hpp"

namespace DiscordCoreAPI {

	class Guild {
	public:
		GuildData data;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		shared_ptr<VoiceConnection> connectToVoice(string channelId, shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> websocketAgent, shared_ptr<unbounded_buffer<AudioDataChunk>> bufferMessageBlockNew);

		void disconnectFromVoice();

	protected:
		friend class GuildManagerAgent;
		friend class DiscordCoreClient;
		friend class GuildManager;
		friend class DiscordCoreClientBase;
		friend struct concurrency::details::_ResultHolder<Guild>;
		friend struct OnGuildCreationData;
		friend struct OnGuildUpdateData;
		friend struct OnGuildDeletionData;

		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };

		Guild() {};

		Guild(DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildData dataNew, shared_ptr<DiscordCoreClient> discordCoreClientNew, shared_ptr<DiscordCoreClientBase> discordCoreClientBaseNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->discordCoreClientBase = discordCoreClientBaseNew;
			this->data = dataNew;
			return;
		}

		void initialize() {
			try {
				cout << "Caching guild: " << this->data.name << endl;
				cout << "Caching channels for guild: " << this->data.name << endl;
				for (auto value : data.channels) {
					value.guildId = this->data.id;
					ChannelData channelData = value;
					Channel channel(channelData, this->discordCoreClient);
					this->discordCoreClientBase->channels->insertChannelAsync(channel).get();
				}
				cout << "Caching guild members for guild: " << this->data.name << endl;
				for (unsigned int x = 0; x < this->data.members.size(); x += 1) {
					GuildMemberData guildMemberData = this->data.members.at(x);
					for (auto value : this->data.voiceStates) {
						if (value.userId == guildMemberData.user.id) {
							guildMemberData.voiceData = value;
						}
					}					
					guildMemberData.guildId = this->data.id;
					DiscordGuildMember discordGuildMember(guildMemberData);
					discordGuildMember.writeDataToDB();
					GuildMember guildMember(guildMemberData, this->data.id, this->discordCoreClient);
					this->discordCoreClientBase->guildMembers->insertGuildMemberAsync(guildMember, this->data.id).get();
				}
				cout << "Caching roles for guild: " << this->data.name << endl;
				for (auto const& value : data.roles) {
					RoleData roleData = value;
					Role role(roleData, this->discordCoreClient);
					this->discordCoreClientBase->roles->insertRoleAsync(role).get();
				}
				cout << "Caching users for guild: " << this->data.name << endl << endl;
				for (auto value : data.members) {
					DiscordCoreInternal::UserData userData = value.user;
					User user(userData, this->discordCoreClient);
					this->discordCoreClientBase->users->insertUserAsync(user).get();
				}
			}
			catch (exception error) {
				cout << "Error: " << error.what() << endl;
			}
			return;
		}
	};

	struct GetGuildData {
		string guildId;
	};

	struct GetVanityInviteData {
		string guildId;
	};

	struct FetchGuildData {
		string guildId;
	};

	struct GetAuditLogData {
		AuditLogEvent actionType;
		string guildId;
		unsigned int limit;
		string userId;
	};

	struct GetInviteData {
		string inviteId;
	};

	struct GetInvitesData {
		string guildId;
	};

	class GuildManagerAgent : agent {
	public:
		static overwrite_buffer<map<string, Guild>> cache;
	protected:
		friend class DiscordCoreClient;
		friend class GuildManager;

		unbounded_buffer<DiscordCoreInternal::GetVanityInviteData> requestGetVanityInviteBuffer;
		unbounded_buffer<DiscordCoreInternal::PutGuildBanData> requestPutGuildBanBuffer;
		unbounded_buffer<DiscordCoreInternal::GetAuditLogData> requestGetAuditLogBuffer;
		unbounded_buffer<DiscordCoreInternal::FetchGuildData> requestFetchGuildBuffer;
		unbounded_buffer<DiscordCoreInternal::GetInvitesData> requestGetInvitesBuffer;
		unbounded_buffer<DiscordCoreInternal::GetInviteData> requestGetInviteBuffer;
		unbounded_buffer<DiscordCoreInternal::GetGuildData> requestGetGuildBuffer;
		unbounded_buffer<vector<InviteData>> outInvitesBuffer;
		unbounded_buffer<AuditLogData> outAuditLogBuffer;
		unbounded_buffer<InviteData> outInviteBuffer;
		unbounded_buffer<exception> errorBuffer;
		unbounded_buffer<BanData> outBanBuffer;
		unbounded_buffer<Guild> outGuildBuffer;
		concurrent_queue<Guild> guildsToInsert;

		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		GuildManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreClient> coreClientNew, shared_ptr<DiscordCoreClientBase> coreClientBaseNew)
			:agent(*GuildManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
			return;
		}

		static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
			GuildManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			GuildManagerAgent::threadContext->releaseGroup();
		}

		bool getError(exception error) {
			return try_receive(this->errorBuffer, error);
		}

		Guild getObjectData(DiscordCoreInternal::FetchGuildData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData 01");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData() Error 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData() Success 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			GuildData guildData;
			DiscordCoreInternal::parseObject(returnData.data, &guildData);
			Guild guildNew(this->agentResources, guildData, this->discordCoreClient, this->discordCoreClientBase);
			return guildNew;
		}

		InviteData getObjectData(DiscordCoreInternal::GetInviteData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_INVITE;
			workload.relativePath = "/invites/" + dataPackage.inviteId + "?with_counts=true&with_expiration=true";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData 02");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			InviteData inviteData;
			DiscordCoreInternal::parseObject(returnData.data, &inviteData);
			return inviteData;
		}

		vector<InviteData> getObjectData(DiscordCoreInternal::GetInvitesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_INVITES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/invites";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData 03");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<InviteData> inviteData;
			for (auto value : returnData.data) {
				InviteData inviteDataNew;
				DiscordCoreInternal::parseObject(value, &inviteDataNew);
				inviteData.push_back(inviteDataNew);
			}
			return inviteData;
		}

		InviteData getObjectData(DiscordCoreInternal::GetVanityInviteData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_VANITY_INVITE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/vanity-url";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData 04");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			InviteData inviteData;
			DiscordCoreInternal::parseObject(returnData.data, &inviteData);
			return inviteData;
		}

		AuditLogData getObjectData(DiscordCoreInternal::GetAuditLogData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_AUDIT_LOG;
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
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData 05");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			AuditLogData auditLogData;
			DiscordCoreInternal::parseObject(returnData.data, &auditLogData);
			return auditLogData;
		}

		BanData putObjectData(DiscordCoreInternal::PutGuildBanData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_GUILD_BAN;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
			workload.content = DiscordCoreInternal::getAddBanPayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::putObjectData 00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			BanData banData;
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::putObjectData() Error 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				banData.failedDueToPerms = true;
			}
			else {
				cout << "GuildManagerAgent::putObjectData() Success 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreInternal::parseObject(returnData.data, &banData);
			return banData;
		}

		void run() {
			try {
				DiscordCoreInternal::GetGuildData dataPackage01;
				if (try_receive(this->requestGetGuildBuffer, dataPackage01)) {
					map<string, Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId)) {
							Guild guild = cacheTemp.at(dataPackage01.guildId);
							send(this->outGuildBuffer, guild);
						}
					}
				}
				DiscordCoreInternal::FetchGuildData dataPackage02;
				if (try_receive(this->requestFetchGuildBuffer, dataPackage02)) {
					map<string, Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.guildId)) {
							cacheTemp.erase(dataPackage02.guildId);
						}
					}
					Guild guild = getObjectData(dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.guildId, guild));
					send(this->outGuildBuffer, guild);
					send(GuildManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::GetAuditLogData dataPackage03;
				if (try_receive(this->requestGetAuditLogBuffer, dataPackage03)) {
					AuditLogData auditLog = getObjectData(dataPackage03);
					send(this->outAuditLogBuffer, auditLog);
				}
				DiscordCoreInternal::GetInvitesData dataPackage04;
				if (try_receive(this->requestGetInvitesBuffer, dataPackage04)) {
					vector<InviteData> inviteData = getObjectData(dataPackage04);
					send(this->outInvitesBuffer, inviteData);
				}
				DiscordCoreInternal::GetInviteData dataPackage05;
				if (try_receive(this->requestGetInviteBuffer, dataPackage05)) {
					InviteData inviteData = getObjectData(dataPackage05);
					send(this->outInviteBuffer, inviteData);
				}
				DiscordCoreInternal::GetVanityInviteData dataPackage06;
				if (try_receive(this->requestGetVanityInviteBuffer, dataPackage06)) {
					InviteData inviteData = getObjectData(dataPackage06);
					send(this->outInviteBuffer, inviteData);
				}
				DiscordCoreInternal::PutGuildBanData dataPackage07;
				if (try_receive(this->requestPutGuildBanBuffer, dataPackage07)) {
					BanData inviteData = putObjectData(dataPackage07);
					send(this->outBanBuffer, inviteData);
				}
				Guild guildNew;
				while (this->guildsToInsert.try_pop(guildNew)) {
					map<string, Guild> cacheTemp;
					try_receive(GuildManagerAgent::cache, cacheTemp);
					if (cacheTemp.contains(guildNew.data.id)) {
						cacheTemp.erase(guildNew.data.id);
					}
					guildNew.initialize();
					cacheTemp.insert(make_pair(guildNew.data.id, guildNew));
					send(GuildManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
			return;
		}
	};

	class GuildManager {
	public:

		task<Guild> fetchAsync(FetchGuildData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestFetchGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			GuildData guildData;
			Guild guild(this->agentResources, guildData, this->discordCoreClient, this->discordCoreClientBase);
			try_receive(requestAgent.outGuildBuffer, guild);
			co_return guild;
		}

		task<vector<InviteData>> getInvitesAsync(GetInvitesData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetInvitesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetInvitesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getInviteAsync() Error: " << error.what() << endl << endl;
			}
			vector<InviteData> inviteData;
			try_receive(requestAgent.outInvitesBuffer, inviteData);
			co_return inviteData;
		}

		task<BanData> createGuildBanAsync(CreateGuildBanData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PutGuildBanData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			dataPackageNew.deleteMessageDays = dataPackage.deleteMessageDays;
			dataPackageNew.reason = dataPackage.reason;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestPutGuildBanBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getInviteAsync() Error: " << error.what() << endl << endl;
			}
			BanData banData;
			try_receive(requestAgent.outBanBuffer, banData);
			co_return banData;
		}

		task<InviteData> getVanityInviteAsync(GetVanityInviteData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetVanityInviteData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetVanityInviteBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getVanityInviteAsync() Error: " << error.what() << endl << endl;
			}
			InviteData inviteData;
			try_receive(requestAgent.outInviteBuffer, inviteData);
			co_return inviteData;
		}

		task<InviteData> getInviteAsync(GetInviteData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetInviteData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.inviteId = dataPackage.inviteId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetInviteBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getAuditLogDataAsync() Error: " << error.what() << endl << endl;
			}
			InviteData inviteData;
			try_receive(requestAgent.outInviteBuffer, inviteData);
			co_return inviteData;
		}

		task<AuditLogData> getAuditLogDataAsync(GetAuditLogData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetAuditLogData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.actionType = (DiscordCoreInternal::AuditLogEvent)dataPackage.actionType;
			dataPackageNew.limit = dataPackage.limit;
			dataPackageNew.userId = dataPackage.userId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetAuditLogBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getAuditLogDataAsync() Error: " << error.what() << endl << endl;
			}
			AuditLogData auditLog;
			try_receive(requestAgent.outAuditLogBuffer, auditLog);
			co_return auditLog;
		}

		task<Guild> getGuildAsync(GetGuildData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getGuildAsync() Error: " << error.what() << endl << endl;
			}
			GuildData guildData;
			Guild guild(this->agentResources, guildData, this->discordCoreClient, this->discordCoreClientBase);
			try_receive(requestAgent.outGuildBuffer, guild);
			co_return guild;
		}

		task<void> insertGuildAsync(Guild guild) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			requestAgent.guildsToInsert.push(guild);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::inserGuildAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> removeGuildAsync(string guildId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Guild> cache;
			try_receive(GuildManagerAgent::cache, cache);
			if (cache.contains(guildId)) {
				cache.erase(guildId);
			}
			send(GuildManagerAgent::cache, cache);
			co_return;
		}

		task<vector<Guild>> getAllGuildsAsync() {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Guild>cache;
			try_receive(GuildManagerAgent::cache, cache);
			vector<Guild> guildVector;
			for (auto [key, value] : cache) {
				guildVector.push_back(value);
			}
			send(GuildManagerAgent::cache, cache);
			co_return guildVector;
		}

		GuildManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> coreClientNew, shared_ptr<DiscordCoreClientBase> coreClientBaseNew) {
			this->threadContext = threadContextNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
		}

		~GuildManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class DiscordCoreClient;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
	};
	overwrite_buffer<map<string, Guild>> GuildManagerAgent::cache;
	shared_ptr<DiscordCoreInternal::ThreadContext> GuildManagerAgent::threadContext;
}
#endif
