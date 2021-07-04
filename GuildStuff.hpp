// GuildStuff.hpp - Header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_
#define _GUILD_STUFF_

#include "../pch.h"
#include "DataParsingFunctions.hpp"
#include "HttpStuff.hpp"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase {
	public:
		shared_ptr<ChannelManager> channels{ nullptr };
		shared_ptr<GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<RoleManager> roles{ nullptr };
		shared_ptr<UserManager> users{ nullptr };
		shared_ptr<DiscordCoreClientBase> thisPointer{ this };
	protected:
		friend class Guild;
		friend class WebSocketConnectionAgent;
		friend class HttpRequestAgent;
		static map<string, DiscordGuild> guildMap;
		static map<string, DiscordGuildMember> guildMemberMap;
	};

	class Guild {
	public:
		GuildData data;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

	protected:
		friend class GuildManagerAgent;
		friend class DiscordCoreClient;
		friend class GuildManager;
		friend class DiscordCoreClientBase;
		friend class DiscordCoreClientNew;
		friend struct concurrency::details::_ResultHolder<Guild>;
		friend struct OnGuildCreationData;
		friend struct OnGuildUpdateData;
		friend struct OnGuildDeletionData;

		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };

		Guild() {};

		Guild(DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildData dataNew,  shared_ptr<DiscordCoreClient> discordCoreClientNew, shared_ptr<DiscordCoreClientBase> discordCoreClientBaseNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->discordCoreClientBase = discordCoreClientBaseNew;
			this->data = dataNew;
		}

		void initialize() {
			try {
				cout << "Caching guild: " << this->data.name << endl;
				cout << "Caching channels for guild: " << this->data.name << endl;
				for (auto value:data.channels) {
					value.guildId = this->data.id;
					ChannelData channelData = value;
					Channel channel(channelData, this->discordCoreClient);
					this->discordCoreClientBase->channels->insertChannelAsync(channel).get();
				}
				cout << "Caching guild members for guild: " << this->data.name << endl;
				for (unsigned int x = 0;x< this->data.members.size(); x+=1) {
					GuildMemberData guildMemberData = data.members.at(x);
					if (x >= 1) {
						bool doWeContinue = false;
						map<string, GuildMember> guildMemberMap = receive(GuildMemberManagerAgent::cache);
						for (auto [key, value] : guildMemberMap) {
							if (this->data.id + " + " +guildMemberData.user.id == value.data.guildId + " + " + value.data.user.id) {
								doWeContinue = true;
								this->data.members.erase(this->data.members.begin() + x);
								x -= 1;
								break;
							}
						}
						if (doWeContinue == true) {
							continue;
						}
						asend(GuildMemberManagerAgent::cache, guildMemberMap);
					}
					guildMemberData.guildId = this->data.id;
					DiscordGuildMember discordGuildMember(guildMemberData);
					discordGuildMember.writeDataToDB();
					DiscordCoreClientBase::guildMemberMap.insert(make_pair(guildMemberData.guildId + " + " + guildMemberData.user.id, discordGuildMember));
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
		string guildId;
		string inviteId;
	};

	struct GetInvitesData {
		string guildId;
	};

	class GuildManagerAgent : public agent {
	public:
		static overwrite_buffer<map<string, Guild>> cache;
	protected:
		friend class DiscordCoreClientExt;
		friend class DiscordCoreClient;
		friend class GuildManager;

		unbounded_buffer<DiscordCoreInternal::GetAuditLogData> requestGetAuditLogBuffer;
		unbounded_buffer<DiscordCoreInternal::FetchGuildData> requestFetchGuildBuffer;
		unbounded_buffer<DiscordCoreInternal::GetInvitesData> requestGetInvitesBuffer;
		unbounded_buffer<DiscordCoreInternal::GetGuildData> requestGetGuildBuffer;
		unbounded_buffer<vector<InviteData>> outInvitesBuffer;
		unbounded_buffer<AuditLogData> outAuditLogBuffer;
		unbounded_buffer<exception> errorBuffer;
		unbounded_buffer<Guild> outGuildBuffer;
		concurrent_queue<Guild> guildsToInsert;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		GuildManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> coreClientNew, shared_ptr<DiscordCoreClientBase> coreClientBaseNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
		}

		bool getError(exception error) {
			if (try_receive(this->errorBuffer, error)) {
				return true;
			}
			return false;
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "requestAgent.getObject() Error 00: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "requestAgent.getObject() Error 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "requestAgent.getObject() Success 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			GuildData guildData;
			DiscordCoreInternal::parseObject(returnData.data, &guildData);
			Guild guildNew(this->agentResources, guildData, this->discordCoreClient, this->discordCoreClientBase);
			return guildNew;
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "requestAgent.getObject() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "requestAgent.getObject() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "requestAgent.getObject() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<InviteData> inviteData;
			for (auto value : returnData.data) {
				InviteData inviteDataNew;
				DiscordCoreInternal::parseObject(value, &inviteDataNew);
				inviteData.push_back(inviteDataNew);
			}			
			return inviteData;
		}

		AuditLogData getObjectData(DiscordCoreInternal::GetAuditLogData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_AUDIT_LOG;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/audit-logs";
			if (dataPackage.userId != "") {
				workload.relativePath += "?user_id=" + dataPackage.userId;
				if (to_string((int)dataPackage.actionType)!= "") {
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "requestAgent.getObject() Error 02: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "requestAgent.getObject() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "requestAgent.getObject() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			AuditLogData auditLogData;
			DiscordCoreInternal::parseObject(returnData.data, &auditLogData);
			return auditLogData;
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
					asend(GuildManagerAgent::cache, cacheTemp);
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
				GuildData dataPackage05;
				Guild guildNew(this->agentResources, dataPackage05, this->discordCoreClient, this->discordCoreClientBase);
				while (this->guildsToInsert.try_pop(guildNew)) {
					map<string, Guild> cacheTemp;
					try_receive(GuildManagerAgent::cache, cacheTemp);
					if (cacheTemp.contains(guildNew.data.id)) {
						cacheTemp.erase(guildNew.data.id);
					}
					guildNew.initialize();
					cacheTemp.insert(make_pair(guildNew.data.id, guildNew));
					asend(GuildManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
		}
	};

	class GuildManager {
	public:

		task<Guild> fetchAsync(FetchGuildData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient, this->discordCoreClientBase);
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
			this->threadContext->releaseGroup(groupIdNew);
			co_return guild;
		}

		task<vector<InviteData>> getInvitesAsync(GetInvitesData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetInvitesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetInvitesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getAuditLogDataAsync() Error: " << error.what() << endl << endl;
			}
			vector<InviteData> inviteData;
			try_receive(requestAgent.outInvitesBuffer, inviteData);
			this->threadContext->releaseGroup(groupIdNew);
			co_return inviteData;
		}

		InviteData getInvite(GetInviteData dataPackage) {
			vector<InviteData>invites = getInvitesAsync({ dataPackage.guildId }).get();
			for (auto value : invites) {
				if (dataPackage.inviteId == value.code) {
					return value;
				}
			}
		}

		task<AuditLogData> getAuditLogDataAsync(GetAuditLogData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetAuditLogData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.actionType = (DiscordCoreInternal::AuditLogEvent)dataPackage.actionType;
			dataPackageNew.limit = dataPackage.limit;
			dataPackageNew.userId = dataPackage.userId;
			GuildManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetAuditLogBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::getAuditLogDataAsync() Error: " << error.what() << endl << endl;
			}
			AuditLogData auditLog;
			try_receive(requestAgent.outAuditLogBuffer, auditLog);
			this->threadContext->releaseGroup(groupIdNew);
			co_return auditLog;
		}

		task<Guild> getGuildAsync(GetGuildData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient, this->discordCoreClientBase);
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
			this->threadContext->releaseGroup(groupIdNew);
			co_return guild;
		}

		task<void> insertGuildAsync(Guild guild) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GuildManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient, this->discordCoreClientBase);
			requestAgent.guildsToInsert.push(guild);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildManager::inserGuildAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeGuildAsync(string guildId) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Guild> cache;
			try_receive(GuildManagerAgent::cache, cache);
			if (cache.contains(guildId)) {
				cache.erase(guildId);
			}
			asend(GuildManagerAgent::cache, cache);
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		GuildManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> coreClientNew, shared_ptr<DiscordCoreClientBase> coreClientBaseNew) {
			this->threadContext = threadContextNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
			this->groupId = this->threadContext->createGroup();
		}

		~GuildManager() {
			this->threadContext->releaseGroup(this->groupId);
		}

	protected:
		friend class DiscordCoreClient;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
		unsigned int groupId;
	};
	overwrite_buffer<map<string, Guild>> GuildManagerAgent::cache;
}
#endif
