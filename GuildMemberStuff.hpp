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

	class DiscordCoreClientBase {
	public:
		ChannelManager* channels{ nullptr };
		GuildMemberManager* guildMembers{ nullptr };
		RoleManager* roles{ nullptr };
		UserManager* users{ nullptr };
	protected:
		friend class Guild;
		friend class WebSocketConnectionAgent;
		friend class HttpRequestAgent;
		static map<string, DiscordGuild> guildMap;
		static map<string, DiscordGuildMember> guildMemberMap;
	};

	class GuildMember {
	public:
		GuildMemberData data;
		DiscordCoreClient* discordCoreClient;

		GuildMember() {};

	protected:
		friend class DiscordCoreClient;
		friend class GuildMemberManager;
		friend class GuildMemberManagerAgent;
		friend class Guild;

		GuildMember(GuildMemberData guildMemberData, string guildIdNew, DiscordCoreClient* discordCoreClientNew) {
			this->data = guildMemberData;
			this->data.guildId = guildIdNew;
			this->discordCoreClient = discordCoreClientNew;
		}

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

		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestFetchGuildMemberBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* requestGetGuildMemberBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>* requestGetRolesBuffer;
		static unbounded_buffer<GuildMember>* outGuildMemberBuffer;
		static concurrent_queue<GuildMember> guildMembersToInsert;
		static overwrite_buffer<map<string, GuildMember>> cache2;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient{ nullptr };
		string guildId;

		GuildMemberManagerAgent(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreClient* discordCoreClientNew, string guildIdNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
			this->guildId = guildIdNew;
		}

		static void initialize() {
			GuildMemberManagerAgent::requestFetchGuildMemberBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::requestGetGuildMemberBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>;
			GuildMemberManagerAgent::requestGetRolesBuffer = new unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>;
			GuildMemberManagerAgent::outGuildMemberBuffer = new unbounded_buffer<GuildMember>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		GuildMember getObjectData(DiscordCoreInternal::GetGuildMemberData dataPackage){
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

		void run() {
			try {
				DiscordCoreInternal::GetGuildMemberData dataPackage01;
				if (try_receive(GuildMemberManagerAgent::requestGetGuildMemberBuffer, dataPackage01)) {
					map<string, GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId + dataPackage01.guildMemberId)) {
							GuildMember GuildMember = cacheTemp.at(dataPackage01.guildId + dataPackage01.guildMemberId);
							send(GuildMemberManagerAgent::outGuildMemberBuffer, GuildMember);
						}
					}
					asend(GuildMemberManagerAgent::cache2, cacheTemp);
				}
				if (try_receive(GuildMemberManagerAgent::requestFetchGuildMemberBuffer, dataPackage01)) {
					map<string, GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId + dataPackage01.guildMemberId)) {
							cacheTemp.erase(dataPackage01.guildId + dataPackage01.guildMemberId);
						}
					}
					GuildMember GuildMember = getObjectData(dataPackage01);
					cacheTemp.insert(make_pair(dataPackage01.guildId + dataPackage01.guildMemberId, GuildMember));
					send(GuildMemberManagerAgent::outGuildMemberBuffer, GuildMember);
					asend(GuildMemberManagerAgent::cache2, cacheTemp);
				}
				GuildMemberData dataPackage02;
				GuildMember guildMember(dataPackage02, this->guildId, this->discordCoreClient);
				while (GuildMemberManagerAgent::guildMembersToInsert.try_pop(guildMember)) {
					map<string, GuildMember> cacheTemp;
					try_receive(GuildMemberManagerAgent::cache2, cacheTemp);
					if (cacheTemp.contains(guildMember.data.guildId + guildMember.data.user.id)) {
						cacheTemp.erase(guildMember.data.guildId + guildMember.data.user.id);
					}
					cacheTemp.insert(make_pair(guildMember.data.guildId + guildMember.data.user.id, guildMember));
					asend(GuildMemberManagerAgent::cache2, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
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
			DiscordCoreInternal::GetGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(this->threadContext, dataPackageNew.agentResources, this->discordCoreClient, dataPackage.guildId);
			send(GuildMemberManagerAgent::requestFetchGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(GuildMemberManagerAgent::outGuildMemberBuffer, guildMember);
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
			GuildMemberManagerAgent requestAgent(this->threadContext, dataPackageNew.agentResources, this->discordCoreClient, dataPackage.guildId);
			send(GuildMemberManagerAgent::requestGetGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::getGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			GuildMemberData guildMemberData;
			GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(GuildMemberManagerAgent::outGuildMemberBuffer, guildMember);
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
			GuildMemberManagerAgent requestAgent(this->threadContext, this->agentResources, this->discordCoreClient, guildId);
			GuildMemberManagerAgent::guildMembersToInsert.push(guildMember);
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
			map<string, GuildMember> cache2;
			try_receive(GuildMemberManagerAgent::cache2, cache2);
			if (cache2.contains(guildId + guildMemberId)) {
				GuildMember guildMember = cache2.at(guildId + guildMemberId);
				cache2.erase(guildId + guildMemberId);
			}
			asend(GuildMemberManagerAgent::cache2, cache2);
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient{ nullptr };
		unsigned int groupId;

		GuildMemberManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
			this->groupId = this->threadContext->createGroup();
		}

		~GuildMemberManager() {
			this->threadContext->releaseGroup(this->groupId);
		}
	};
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestFetchGuildMemberBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberData>* GuildMemberManagerAgent::requestGetGuildMemberBuffer;
	unbounded_buffer<DiscordCoreInternal::GetGuildMemberRolesData>* GuildMemberManagerAgent::requestGetRolesBuffer;
	unbounded_buffer<GuildMember>* GuildMemberManagerAgent::outGuildMemberBuffer;
	concurrent_queue<GuildMember> GuildMemberManagerAgent::guildMembersToInsert;
	overwrite_buffer<map<string, GuildMember>> GuildMemberManagerAgent::cache2;
};
#endif