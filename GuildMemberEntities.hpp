// GuildMemberEntities.hpp - Header for the guild member related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_ENTITIES
#define _GUILD_MEMBER_ENTITIES

#include "../pch.h"
#include "DatabaseEntities.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

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
};

namespace DiscordCoreInternal {

	class GuildMemberManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class GuildMemberManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<PatchGuildMemberData> requestPatchGuildMemberBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::GuildMember> outGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberData> requestGetGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberRolesData> requestGetRolesBuffer{ nullptr };

		GuildMemberManagerAgent()
			:agent(*GuildMemberManagerAgent::threadContext->scheduler->scheduler) {
			
		}

		static void intialize() {
			GuildMemberManagerAgent::threadContext = ThreadManager::getThreadContext().get();
		}

		static void cleanup() {
			GuildMemberManagerAgent::threadContext->releaseContext();
		}

		DiscordCoreAPI::GuildMember getObjectData(GetGuildMemberData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_GUILD_MEMBER;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
			HttpRequestAgent requestAgent{};
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
			HttpRequestAgent requestAgent{};
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
				GetGuildMemberData dataPackage02;
				if (try_receive(this->requestGetGuildMemberBuffer, dataPackage02)) {
					DiscordCoreAPI::GuildMember GuildMember = getObjectData(dataPackage02);
					send(this->outGuildMemberBuffer, GuildMember);
				}
				PatchGuildMemberData dataPackage03;
				if (try_receive(this->requestPatchGuildMemberBuffer, dataPackage03)) {
					DiscordCoreAPI::GuildMember GuildMember = patchObjectData(dataPackage03);
					send(this->outGuildMemberBuffer, GuildMember);
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
			this->cache = new overwrite_buffer<map<string, DiscordCoreAPI::GuildMember>>();
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		overwrite_buffer<map<string, DiscordCoreAPI::GuildMember>>* cache{};

		task<DiscordCoreAPI::GuildMember> fetchAsync(DiscordCoreAPI::FetchGuildMemberData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetGuildMemberData dataPackageNew;
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
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			PatchGuildMemberData dataPackageNew;
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
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			DiscordCoreAPI::GuildMember guildMember{};
			map<string, DiscordCoreAPI::GuildMember> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			if (cacheTemp.contains(dataPackage.guildId + " + " + dataPackage.guildMemberId)) {
				guildMember = cacheTemp.at(dataPackage.guildId + " + " + dataPackage.guildMemberId);
			};
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return guildMember;
		}

		task<void> insertGuildMemberAsync(DiscordCoreAPI::GuildMember guildMember, string guildId) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::GuildMember> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			cacheTemp.insert_or_assign(guildId + " + " + guildMember.user.id, guildMember);
			send(this->cache, cacheTemp);
			co_return;
		}

		task<void> removeGuildMemberAsync(string guildId, string guildMemberId) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::GuildMember> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			if (cacheTemp.contains(guildId + " + " + guildMemberId)) {
				cacheTemp.erase(guildId + " + " + guildMemberId);
			};
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return;
		}
	};
	shared_ptr<ThreadContext> GuildMemberManagerAgent::threadContext{ nullptr };
};
#endif