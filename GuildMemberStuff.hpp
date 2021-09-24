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

	class GuildMember : public GuildMemberData {
	protected:

		friend struct Concurrency::details::_ResultHolder<GuildMember>;
		friend class DiscordCoreInternal::GuildMemberManagerAgent;
		friend class DiscordCoreInternal::GuildMemberManager;
		friend struct GetGuildMemberRolesData;
		friend struct OnGuildMemberUpdateData;
		friend struct OnGuildMemberAddData;
		friend class DiscordCoreClient;
		friend class Guild;

		GuildMember() {};

		GuildMember(GuildMemberData dataNew, string guildIdNew) {
			this->premiumSince = dataNew.premiumSince;
			this->permissions = dataNew.permissions;
			this->userMention = dataNew.userMention;
			this->voiceData = dataNew.voiceData;
			this->joinedAt = dataNew.joinedAt;
			this->pending = dataNew.pending;
			this->roles = dataNew.roles;
			this->guildId = guildIdNew;
			this->user = dataNew.user;
			this->nick = dataNew.nick;
			if (this->nick == "") {
				this->nick = this->user.userName;
			}
			this->deaf = dataNew.deaf;
			this->mute = dataNew.mute;
		}
	};

	struct ModifyGuildMemberData {
		string newVoiceChannelId{ "" };
		string currentChannelId{ "" };
		string guildMemberId{ "" };
		vector<string> roleIds{};
		string guildId{ "" };
		bool mute{ false };
		bool deaf{ false };
		string nick{ "" };
	};

	struct FetchGuildMemberData {
		string guildMemberId{ "" };
		string guildId{ "" };
	};

	struct GetGuildMemberData {
		string guildMemberId{ "" };
		string guildId{ "" };
	};
};

namespace DiscordCoreInternal {

	class GuildMemberManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class GuildMemberManager;

		static overwrite_buffer<map<string, DiscordCoreAPI::GuildMember>> cache;
		static shared_ptr<ThreadContext> threadContext;
		static HttpAgentResources agentResources;

		unbounded_buffer<CollectGuildMemberData> requestCollectGuildMemberBuffer{ nullptr };
		unbounded_buffer<PatchGuildMemberData> requestPatchGuildMemberBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::GuildMember> outGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberData> requestGetGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberRolesData> requestGetRolesBuffer{ nullptr };
		concurrent_queue<DiscordCoreAPI::GuildMember> guildMembersToInsert{};

		GuildMemberManagerAgent()
			:agent(*GuildMemberManagerAgent::threadContext->scheduler->scheduler) {
			
		}

		static void intialize(HttpAgentResources agentResourcesNew) {
			GuildMemberManagerAgent::threadContext = ThreadManager::getThreadContext().get();
			GuildMemberManagerAgent::agentResources = agentResourcesNew;
		}

		static void cleanup() {
			GuildMemberManagerAgent::threadContext->releaseGroup();
		}

		DiscordCoreAPI::GuildMember getObjectData(GetGuildMemberData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildMemberManagerAgent::getObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DataParser::parseObject(returnData.data, &guildMemberData);
			DiscordCoreAPI::GuildMember guildMemberNew(guildMemberData, dataPackage.guildId);
			return guildMemberNew;
		}

		DiscordCoreAPI::GuildMember patchObjectData(PatchGuildMemberData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			workload.content = getModifyGuildMemberPayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "GuildMemberManagerAgent::patchObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildMemberManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildMemberManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DataParser::parseObject(returnData.data, &guildMemberData);
			DiscordCoreAPI::GuildMember guildMemberNew(guildMemberData, dataPackage.guildId);
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
						if (cacheTemp.contains(dataPackage02.guildId + " + " + dataPackage02.guildMemberId)) {
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
					if (try_receive(GuildMemberManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(guildMember.guildId + " + " + guildMember.user.id)) {
							cacheTemp.erase(guildMember.guildId + " + " + guildMember.user.id);
						}
						cacheTemp.insert(make_pair(guildMember.guildId + " + " + guildMember.user.id, guildMember));
					};
					send(GuildMemberManagerAgent::cache, cacheTemp);
				}
			}
			catch (...) {
				DiscordCoreAPI::rethrowException("GuildMemberManagerAgent::run() Error: ");
			}
			this->done();
		}
	};

	class GuildMemberManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClientBase;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

		GuildMemberManager(GuildMemberManager* pointer) 
			: ThreadContext(*ThreadManager::getThreadContext().get()) {
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		HttpAgentResources agentResources{};

		GuildMemberManager initialize(HttpAgentResources agentResourcesNew) {
			this->agentResources = agentResourcesNew;
			return *this;
		}

		task<DiscordCoreAPI::GuildMember> fetchAsync(DiscordCoreAPI::FetchGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent{};
			send(requestAgent.requestGetGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DiscordCoreAPI::GuildMember guildMember(guildMemberData, dataPackage.guildId);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			guildMember.guildId = dataPackage.guildId;
			co_await mainThread;
			co_return guildMember;
		}

		task<DiscordCoreAPI::GuildMember> modifyGuildMemberAsync(DiscordCoreAPI::ModifyGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
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
			GuildMemberManagerAgent requestAgent{};
			send(requestAgent.requestPatchGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DiscordCoreAPI::GuildMember guildMember(guildMemberData, dataPackage.guildId);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			guildMember.guildId = dataPackage.guildId;
			co_await mainThread;
			co_return guildMember;
		}

		task<DiscordCoreAPI::GuildMember> getGuildMemberAsync(DiscordCoreAPI::GetGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			CollectGuildMemberData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			GuildMemberManagerAgent requestAgent{};
			send(requestAgent.requestCollectGuildMemberBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::GuildMemberData guildMemberData;
			DiscordCoreAPI::GuildMember guildMember(guildMemberData, dataPackage.guildId);
			try_receive(requestAgent.outGuildMemberBuffer, guildMember);
			guildMember.guildId = dataPackage.guildId;
			co_await mainThread;
			co_return guildMember;
		}

		task<void> insertGuildMemberAsync(DiscordCoreAPI::GuildMember guildMember, string guildId) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GuildMemberManagerAgent requestAgent{};
			requestAgent.guildMembersToInsert.push(guildMember);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> removeGuildMemberAsync(string guildId, string guildMemberId) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::GuildMember> cache;
			try_receive(GuildMemberManagerAgent::cache, cache);
			if (cache.contains(guildId + guildMemberId)) {
				DiscordCoreAPI::GuildMember guildMember = cache.at(guildId + guildMemberId);
				cache.erase(guildId + guildMemberId);
			}
			send(GuildMemberManagerAgent::cache, cache);
			co_await mainThread;
			co_return;
		}

		~GuildMemberManager() {}
	};
	overwrite_buffer<map<string, DiscordCoreAPI::GuildMember>> GuildMemberManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> GuildMemberManagerAgent::threadContext{ nullptr };
	HttpAgentResources GuildMemberManagerAgent::agentResources{};
};
#endif