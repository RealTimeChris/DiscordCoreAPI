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

namespace DiscordCoreInternal {
	class GuildMemberManagerAgent;
	class GuildMemberManager;
};

namespace DiscordCoreAPI {

	class EventHandler;
	class GuildMembers;
	class Guild;

	class GuildMember {
	public:
		friend class DiscordCoreInternal::GuildMemberManagerAgent;
		friend class DiscordCoreInternal::GuildMemberManager;
		friend class DiscordCoreClient;
		friend class Guild;

		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		GuildMemberData data{};

		GuildMember() {};

	protected:

		GuildMember(GuildMemberData guildMemberData, string guildIdNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->data.guildId = guildIdNew;
			this->data = guildMemberData;
		}
	};

	struct ModifyGuildMemberData {
		string guildMemberId{ "" };
		string guildId{ "" };
		string nick{ "" };
		vector<string> roleIds{};
		bool mute{ false };
		bool deaf{ false };
		string newVoiceChannelId{ "" };
		string currentChannelId{ "" };
	};

	struct FetchGuildMemberData {
		string guildId{ "" };
		string guildMemberId{ "" };
	};

	struct GetGuildMemberData {
		string guildId{ "" };
		string guildMemberId{ "" };
	};
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI::EventHandler;
	class DiscordCoreAPI::GuildMembers;

	class GuildMemberManagerAgent : agent {
	protected:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class GuildMemberManager;

		static overwrite_buffer<map<string, DiscordCoreAPI::GuildMember>> cache;
		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<CollectGuildMemberData> requestCollectGuildMemberBuffer{ nullptr };
		unbounded_buffer<PatchGuildMemberData> requestPatchGuildMemberBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::GuildMember> outGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberData> requestGetGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberRolesData> requestGetRolesBuffer{ nullptr };
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		concurrent_queue<DiscordCoreAPI::GuildMember> guildMembersToInsert{};
		unbounded_buffer<exception> errorBuffer{ nullptr };
		HttpAgentResources agentResources{};

		GuildMemberManagerAgent(HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew)
			:agent(*GuildMemberManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void intialize(shared_ptr<ThreadContext> threadContextNew) {
			GuildMemberManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			GuildMemberManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::GuildMemberManagerAgent Error: " << error.what() << endl << endl;
			}
			return;
		}
		
		DiscordCoreAPI::GuildMember getObjectData(GetGuildMemberData dataPackage){
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildMemberManagerAgent::getObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DataParser::parseObject(returnData.data, &guildMemberData);
			DiscordCoreAPI::GuildMember guildMemberNew(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			return guildMemberNew;
		}

		DiscordCoreAPI::GuildMember patchObjectData(PatchGuildMemberData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.content = getModifyGuildMemberPayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildMemberManagerAgent::patchObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DataParser::parseObject(returnData.data, &guildMemberData);
			DiscordCoreAPI::GuildMember guildMemberNew(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			return guildMemberNew;
		}

		void run() {
			try {
				CollectGuildMemberData dataPackage01;
				if (try_receive(this->requestCollectGuildMemberBuffer, dataPackage01)) {
					map<string, DiscordCoreAPI::GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId + " + " + dataPackage01.guildMemberId)) {
							DiscordCoreAPI::GuildMember GuildMember = cacheTemp.at(dataPackage01.guildId + " + " + dataPackage01.guildMemberId);
							send(this->outGuildMemberBuffer, GuildMember);
						}
					}
					send(GuildMemberManagerAgent::cache, cacheTemp);
				}
				GetGuildMemberData dataPackage02;
				if (try_receive(this->requestGetGuildMemberBuffer, dataPackage02)) {
					map<string, DiscordCoreAPI::GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.guildId + " + "+ dataPackage02.guildMemberId)) {
							cacheTemp.erase(dataPackage02.guildId + " + " + dataPackage02.guildMemberId);
						}
					}
					DiscordCoreAPI::GuildMember GuildMember = getObjectData(dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.guildId + " + " + dataPackage02.guildMemberId, GuildMember));
					send(this->outGuildMemberBuffer, GuildMember);
					send(GuildMemberManagerAgent::cache, cacheTemp);
				}
				PatchGuildMemberData dataPackage03;
				if (try_receive(this->requestPatchGuildMemberBuffer, dataPackage03)) {
					map<string, DiscordCoreAPI::GuildMember> cacheTemp;
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.guildId + " + " + dataPackage03.guildMemberId)) {
							cacheTemp.erase(dataPackage03.guildId + " + " + dataPackage03.guildMemberId);
						}
					}
					DiscordCoreAPI::GuildMember GuildMember = patchObjectData(dataPackage03);
					cacheTemp.insert(make_pair(dataPackage03.guildId + " + " + dataPackage03.guildMemberId, GuildMember));
					send(this->outGuildMemberBuffer, GuildMember);
					send(GuildMemberManagerAgent::cache, cacheTemp);
				}
				DiscordCoreAPI::GuildMember guildMember;
				while (this->guildMembersToInsert.try_pop(guildMember)) {
					map<string, DiscordCoreAPI::GuildMember> cacheTemp;
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

		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

		GuildMemberManager(HttpAgentResources agentResourcesNew, shared_ptr<ThreadContext> threadContextNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		task<DiscordCoreAPI::GuildMember> fetchAsync(DiscordCoreAPI::FetchGuildMemberData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GetGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(dataPackageNew.agentResources,  this->discordCoreClient);
			send(requestAgent.requestGetGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildMemberManager::fetchAsync");
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DiscordCoreAPI::GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			guildMember.data.guildId = dataPackage.guildId;
			co_return guildMember;
		}

		task<DiscordCoreAPI::GuildMember> modifyGuildMemberAsync(DiscordCoreAPI::ModifyGuildMemberData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PatchGuildMemberData dataPackageNew;
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
			requestAgent.getError("GuildMemberManager::modifyGuildMemberAsync");
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DiscordCoreAPI::GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			guildMember.data.guildId = dataPackage.guildId;
			co_return guildMember;
		}

		task<DiscordCoreAPI::GuildMember> getGuildMemberAsync(DiscordCoreAPI::GetGuildMemberData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			CollectGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestCollectGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildMemberManager::getGuildMemberAsync");
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DiscordCoreAPI::GuildMember guildMember(guildMemberData, dataPackage.guildId, this->discordCoreClient);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			guildMember.data.guildId = dataPackage.guildId;
			co_return guildMember;
		}

		~GuildMemberManager() {
			this->threadContext->releaseGroup();
		}

	protected:

		task<void> insertGuildMemberAsync(DiscordCoreAPI::GuildMember guildMember, string guildId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GuildMemberManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			requestAgent.guildMembersToInsert.push(guildMember);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildMemberManager::insertGuildMemberAsync");
			co_return;
		}

		task<void> removeGuildMemberAsync(string guildId, string guildMemberId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, DiscordCoreAPI::GuildMember> cache;
			try_receive(GuildMemberManagerAgent::cache, cache);
			if (cache.contains(guildId + guildMemberId)) {
				DiscordCoreAPI::GuildMember guildMember = cache.at(guildId + guildMemberId);
				cache.erase(guildId + guildMemberId);
			}
			send(GuildMemberManagerAgent::cache, cache);
			co_return;
		}

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<ThreadContext> threadContext{ nullptr };
		HttpAgentResources agentResources{};
	};
	overwrite_buffer<map<string, DiscordCoreAPI::GuildMember>> GuildMemberManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> GuildMemberManagerAgent::threadContext{ nullptr };
};
#endif