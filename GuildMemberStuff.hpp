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

		GuildMember(GuildMemberData guildMemberData, string guildIdNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->data = guildMemberData;
			this->data.guildId = guildIdNew;
			this->discordCoreClient = discordCoreClientNew;
		}

	protected:
		friend class DiscordCoreClient;
		friend class GuildMemberManager;
		friend class GuildMemberManagerAgent;
		friend class Guild;
	};

	struct ModifyGuildMemberData{
		string guildMemberId;
		string guildId;
		string nick;
		vector<string> roleIds;
		bool mute;
		bool deaf;
		string newVoiceChannelId = "";
		string currentChannelId = "";
	};

	struct FetchGuildMemberData {
		string guildId;
		string guildMemberId;
	};

	struct GetGuildMemberData {
		string guildId;
		string guildMemberId;
	};

	class GuildMemberManagerAgent : agent {
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

		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		GuildMemberManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreClient> discordCoreClientNew)
			:agent(*GuildMemberManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void intialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
			GuildMemberManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			GuildMemberManagerAgent::threadContext->releaseGroup();
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
			requestAgent.getError("GuildMemberManagerAgent::getObjectData");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::getObjectData() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::getObjectData() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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
			requestAgent.getError("GuildMemberManagerAgent::patchObjectData");
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
					send(GuildMemberManagerAgent::cache, cacheTemp);
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
					send(GuildMemberManagerAgent::cache, cacheTemp);
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
					send(GuildMemberManagerAgent::cache, cacheTemp);
				}
				GuildMember guildMember;
				while (this->guildMembersToInsert.try_pop(guildMember)) {
					map<string, GuildMember> cacheTemp;
					try_receive(GuildMemberManagerAgent::cache, cacheTemp);
					if (cacheTemp.contains(guildMember.data.guildId +" + "+ guildMember.data.user.id)) {
						cacheTemp.erase(guildMember.data.guildId + " + " + guildMember.data.user.id);
					}
					cacheTemp.insert(make_pair(guildMember.data.guildId + " + " + guildMember.data.user.id, guildMember));
					send(GuildMemberManagerAgent::cache, cacheTemp);
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
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(dataPackageNew.agentResources,  this->discordCoreClient);
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
			co_return guildMember;
		}

		task<GuildMember> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::ModifyGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.deaf = dataPackage.deaf;
			dataPackageNew.mute = dataPackage.mute;
			dataPackageNew.newVoiceChannelId = dataPackage.newVoiceChannelId;
			dataPackageNew.currentChannelId = dataPackage.currentChannelId;
			dataPackageNew.nick = dataPackage.nick;
			dataPackageNew.roleIds = dataPackage.roleIds;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
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
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
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
			co_return guildMember;
		}

		task<void> insertGuildMemberAsync(GuildMember guildMember, string guildId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GuildMemberManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			requestAgent.guildMembersToInsert.push(guildMember);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "GuildMemberManager::getGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			co_return;
		}

		task<void> removeGuildMemberAsync(string guildId, string guildMemberId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, GuildMember> cache;
			try_receive(GuildMemberManagerAgent::cache, cache);
			if (cache.contains(guildId + guildMemberId)) {
				GuildMember guildMember = cache.at(guildId + guildMemberId);
				cache.erase(guildId + guildMemberId);
			}
			send(GuildMemberManagerAgent::cache, cache);
			co_return;
		}

		GuildMemberManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		~GuildMemberManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
	};
	overwrite_buffer<map<string, GuildMember>> GuildMemberManagerAgent::cache;
	shared_ptr<DiscordCoreInternal::ThreadContext> GuildMemberManagerAgent::threadContext{ nullptr };
};
#endif