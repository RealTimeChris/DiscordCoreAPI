// GuildMemberStuff.hpp - Header for the guild member related stuff.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_STUFF_
#define _GUILD_MEMBER_STUFF_

#include "../pch.h"
#include "DatabaseStuff.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class GuildMemberManager;

	class RoleManager;

	class UserManager;

	class GuildMember {
	public:
		GuildMemberData data;
		shared_ptr<DiscordCoreClient> discordCoreClient;

		GuildMember() {};

	protected:
		friend class DiscordCoreClient;
		friend class GuildMemberManager;
		friend class GuildMemberManagerAgent;
		friend class Guild;

		GuildMember(GuildMemberData guildMemberData, string guildIdNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->data = guildMemberData;
			this->data.guildId = guildIdNew;
			this->discordCoreClient = discordCoreClientNew;
		}
	};

	struct ModifyGuildMemberData{
		string nick;
		vector<string> roleIds;
		bool mute;
		bool deaf;
		string guildMemberId;
		string newVoiceChannelId = "";
		string guildId;
	};

	struct FetchGuildMemberData {
		string guildId;
		string guildMemberId;
	};

	struct GetGuildMemberData {
		string guildId;
		string guildMemberId;
	};

	class GuildMemberManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class GuildMemberManager;
		friend class IndexHost;

		static overwrite_buffer<map<string, GuildMember>> cache;

		unbounded_buffer<DiscordCoreInternal::FetchGuildMemberData> requestFetchGuildMemberBuffer;
		unbounded_buffer<DiscordCoreInternal::GetGuildMemberData> requestGetGuildMemberBuffer;
		unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData> requestGetRolesBuffer;
		unbounded_buffer<DiscordCoreInternal::ModifyGuildMemberData> requestPatchGuildMemberBuffer;
		unbounded_buffer<GuildMember> outGuildMemberBuffer;
		concurrent_queue<GuildMember> guildMembersToInsert;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		GuildMemberManagerAgent(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreClient> discordCoreClientNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		bool getError(exception& error) {
			return try_receive(this->errorBuffer, error);
		}
		
		GuildMember getObjectData(DiscordCoreInternal::FetchGuildMemberData dataPackage){
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManagerAgent::getObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::getObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::getObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			GuildMemberData guildMemberData;
			DiscordCoreInternal::parseObject(returnData.data, &guildMemberData);
			GuildMember guildMemberNew(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			return guildMemberNew;
		}

		GuildMember patchObjectData(DiscordCoreInternal::ModifyGuildMemberData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.content = DiscordCoreInternal::getModifyGuildMemberPayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManagerAgent::patchObjectData() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::patchObjectData() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::patchObjectData() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			GuildMemberData guildMemberData;
			DiscordCoreInternal::parseObject(returnData.data, &guildMemberData);
			GuildMember guildMemberNew(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			return guildMemberNew;
		}

		void run() {
			try {
				DiscordCoreInternal::GetGuildMemberData dataPackage01;
				if (try_receive(this->requestGetGuildMemberBuffer, dataPackage01)) {
					map<string, GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId + " + " + dataPackage01.guildMemberId)) {
							GuildMember GuildMember = cacheTemp.at(dataPackage01.guildId + " + " + dataPackage01.guildMemberId);
							send(this->outGuildMemberBuffer, GuildMember);
						}
					}
					asend(GuildMemberManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::FetchGuildMemberData dataPackage02;
				if (try_receive(this->requestFetchGuildMemberBuffer, dataPackage02)) {
					map<string, GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.guildId + " + "+ dataPackage02.guildMemberId)) {
							cacheTemp.erase(dataPackage02.guildId + " + " + dataPackage02.guildMemberId);
						}
					}
					GuildMember GuildMember = getObjectData(dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.guildId + " + " + dataPackage02.guildMemberId, GuildMember));
					send(this->outGuildMemberBuffer, GuildMember);
					asend(GuildMemberManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::ModifyGuildMemberData dataPackage03;
				if (try_receive(this->requestPatchGuildMemberBuffer, dataPackage03)) {
					map<string, GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.guildId + " + " + dataPackage03.guildMemberId)) {
							cacheTemp.erase(dataPackage03.guildId + " + " + dataPackage03.guildMemberId);
						}
					}
					GuildMember GuildMember = patchObjectData(dataPackage03);
					cacheTemp.insert(make_pair(dataPackage03.guildId + " + " + dataPackage03.guildMemberId, GuildMember));
					send(this->outGuildMemberBuffer, GuildMember);
					asend(GuildMemberManagerAgent::cache, cacheTemp);
				}
				GuildMember guildMember;
				while (this->guildMembersToInsert.try_pop(guildMember)) {
					map<string, GuildMember> cacheTemp;
					try_receive(GuildMemberManagerAgent::cache, cacheTemp);
					if (cacheTemp.contains(guildMember.data.guildId +" + "+ guildMember.data.user.id)) {
						cacheTemp.erase(guildMember.data.guildId + " + " + guildMember.data.user.id);
					}
					cacheTemp.insert(make_pair(guildMember.data.guildId + " + " + guildMember.data.user.id, guildMember));
					asend(GuildMemberManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
		}
	};

	class GuildMemberManager {
	public:

		task<GuildMember> fetchAsync(FetchGuildMemberData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(this->threadContext, dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestFetchGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			this->threadContext->releaseGroup(groupIdNew);
			co_return guildMember;
		}

		task<GuildMember> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::ModifyGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.deaf = dataPackage.deaf;
			dataPackageNew.mute = dataPackage.mute;
			dataPackageNew.newVoiceChannelId = dataPackage.newVoiceChannelId;
			dataPackageNew.nick = dataPackage.nick;
			dataPackageNew.roleIds = dataPackage.roleIds;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(this->threadContext, dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPatchGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::modifyGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			this->threadContext->releaseGroup(groupIdNew);
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(this->threadContext, dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::getGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			this->threadContext->releaseGroup(groupIdNew);
			co_return guildMember;
		}

		task<void> insertGuildMemberAsync(GuildMember guildMember, string guildId) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GuildMemberManagerAgent requestAgent(this->threadContext, this->agentResources, this->discordCoreClient);
			requestAgent.guildMembersToInsert.push(guildMember);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::getGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeGuildMemberAsync(string guildId, string guildMemberId) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, GuildMember> cache;
			try_receive(GuildMemberManagerAgent::cache, cache);
			if (cache.contains(guildId + guildMemberId)) {
				GuildMember guildMember = cache.at(guildId + guildMemberId);
				cache.erase(guildId + guildMemberId);
			}
			asend(GuildMemberManagerAgent::cache, cache);
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		GuildMemberManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
			this->groupId = this->threadContext->createGroup();
		}

		~GuildMemberManager() {
			this->threadContext->releaseGroup(this->groupId);
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		unsigned int groupId;
	};
	overwrite_buffer<map<string, GuildMember>> GuildMemberManagerAgent::cache;
};
#endif