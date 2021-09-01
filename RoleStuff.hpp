// RoleStuff.hpp - Header for the Role-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_STUFF_
#define _ROLE_STUFF_

#include "pch.h"
#include "JSONifier.hpp"
#include "GuildMemberStuff.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	class RoleManagerAgent;
	class RoleManager;

}

namespace DiscordCoreAPI {

	class PermissionsConverter;
	class Roles;

	class Role :public RoleData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Role>;
		friend class DiscordCoreInternal::RoleManagerAgent;
		friend class DiscordCoreInternal::RoleManager;
		friend struct OnRoleDeletionData;
		friend struct OnRoleCreationData;
		friend class DiscordCoreClient;
		friend struct OnRoleUpdateData;
		friend class Guild;

		Role() {};

		Role(RoleData dataNew) {
			this->tags = dataNew.tags;
			this->permissions = dataNew.permissions;
			this->mentionable = dataNew.mentionable;
			this->position = dataNew.position;
			this->name = dataNew.name;
			this->managed = dataNew.managed;
			this->hoist = dataNew.hoist;
			this->id = dataNew.id;
			this->color = dataNew.color;
			this->discordCoreClient = dataNew.discordCoreClient;
		}
	};

	struct UpdateRoleData {
		string guildId{ "" };
		string roleId{ "" };
		string name{ "" };
		string permissions{ "" };
		string hexColorValue{ "" };
		bool hoist{ false };
		bool mentionable{ false };
	};

	struct GetGuildRolesData {
		string guildId{ "" };
	};

	struct FetchRoleData {
		string guildId{ "" };
		string roleId{ "" };
	};

	struct UpdateRolePositionData {
		string guildId{ "" };
		string roleId{ "" };
		int newPosition{ 0 };
	};

	struct RemoveRoleFromGuildMemberData {
		string guildId{ "" };
		string userId{ "" };
		string roleId{ "" };
	};

	struct GetRoleData {
		string guildId{ "" };
		string roleId{ "" };
	};

	struct RemoveRoleFromGuildData {
		string guildId{ "" };
		string roleId{ "" };
	};

	struct AddRoleToGuildMemberData {
		string guildId{ "" };
		string userId{ "" };
		string roleId{ "" };
	};

	struct CreateRoleData {
		string name{ "" };
		string permissions{ "0" };
		string hexColorValue{ "" };
		bool hoist{ false };
		bool mentionable{ false };
		string guildId{ "" };
		int position{ 0 };
	};

	struct GetGuildMemberRolesData {
		string guildId{ "" };
		GuildMember guildMember{};
	};
};

namespace DiscordCoreInternal {

	class RoleManagerAgent : agent {
	protected:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class RoleManager;
		
		static overwrite_buffer<map<string, DiscordCoreAPI::Role>> cache;
		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<PatchRolePositionData> requestPatchGuildRolesBuffer{ nullptr };
		unbounded_buffer<DeleteGuildMemberRoleData> requestDeleteRoleBuffer{ nullptr };
		unbounded_buffer<DeleteGuildRoleData> requestDeleteGuildRoleBuffer{ nullptr };
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Role>> outRolesBuffer{ nullptr };
		unbounded_buffer<CollectRoleData> requestCollectRoleBuffer{ nullptr };
		unbounded_buffer<PatchRoleData> requestPatchRoleBuffer{ nullptr };
		unbounded_buffer<PostRoleData> requestPostRoleBuffer{ nullptr };
		unbounded_buffer<GetRolesData> requestGetRolesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Role> outRoleBuffer{ nullptr };
		unbounded_buffer<GetRoleData> requestGetRoleBuffer{ nullptr };
		unbounded_buffer<PutRoleData> requestPutRoleBuffer{ nullptr };
		concurrent_queue<DiscordCoreAPI::Role> rolesToInsert{};
		unbounded_buffer<exception> errorBuffer{ nullptr };
		HttpAgentResources agentResources{};

		RoleManagerAgent(HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew)
			:agent(*RoleManagerAgent::threadContext->scheduler->ptrScheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize(shared_ptr<ThreadContext> threadContextNew) {
			RoleManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			RoleManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::RoleManagerAgent Error: " << error.what() << endl << endl;
			}
			return;
		}

		vector<DiscordCoreAPI::Role> getObjectData(GetRolesData  dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::getObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<DiscordCoreAPI::Role> roleVector;
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				DiscordCoreAPI::RoleData roleData;
				roleData.discordCoreClient = this->discordCoreClient;
				DataParser::parseObject(returnData.data.at(x), &roleData);
				DiscordCoreAPI::Role newRole(roleData);
				roleVector.push_back(newRole);
			}
			return roleVector;
		}

		DiscordCoreAPI::Role getObjectData(GetRoleData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::getObjectData_01");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::getObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::getObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			map<string, DiscordCoreAPI::Role> cacheTemp = receive(RoleManagerAgent::cache, 1U);
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				DiscordCoreAPI::RoleData roleData;
				DataParser::parseObject(returnData.data.at(x), &roleData);
				roleData.discordCoreClient = this->discordCoreClient;
				DiscordCoreAPI::Role newRole(roleData);
				cacheTemp.insert(make_pair(newRole.id, newRole));
			}
			send(RoleManagerAgent::cache, cacheTemp);
			DiscordCoreAPI::RoleData roleData;
			if (cacheTemp.contains(dataPackage.roleId)) {
				roleData = cacheTemp.at(dataPackage.roleId);
				cacheTemp.erase(dataPackage.roleId);
			}
			roleData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Role newRole(roleData);
			return newRole;
		}

		DiscordCoreAPI::Role patchObjectData(PatchRoleData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.content = getModifyRolePayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::patchObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::RoleData roleData;
			roleData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &roleData);
			DiscordCoreAPI::Role newRole(roleData);
			return newRole;
		}

		vector<DiscordCoreAPI::Role> patchObjectData(PatchRolePositionData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_GUILD_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = getUpdateRolePositionsPayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::patchObjectData_01");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<DiscordCoreAPI::Role> roleVector;
			for (auto value : returnData.data) {
				DiscordCoreAPI::RoleData newRoleData;
				newRoleData.discordCoreClient = this->discordCoreClient;
				DataParser::parseObject(value, &newRoleData);
				DiscordCoreAPI::Role newRole(newRoleData);
				roleVector.push_back(newRole);
			}
			return roleVector;
		}

		DiscordCoreAPI::Role postObjectData(PostRoleData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.workloadType = HttpWorkloadType::POST_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = getCreateRolePayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::postObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::RoleData roleData;
			roleData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &roleData);
			DiscordCoreAPI::Role newRole(roleData);
			return newRole;
		}

		void putObjectData(PutRoleData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.workloadType = HttpWorkloadType::PUT_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::putObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DeleteGuildMemberRoleData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.workloadType = HttpWorkloadType::DELETE_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::deleteObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DeleteGuildRoleData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.workloadType = HttpWorkloadType::DELETE_GUILD_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::deleteObjectData_01");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void run() {
			try {
				CollectRoleData dataPackage01;
				if (try_receive(this->requestCollectRoleBuffer, dataPackage01)) {
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.roleId)) {
							DiscordCoreAPI::Role role = cacheTemp.at(dataPackage01.roleId);
							send(this->outRoleBuffer, role);
						}
					}
				}
				GetRoleData dataPackage02;
				if (try_receive(this->requestGetRoleBuffer, dataPackage02)) {
					DiscordCoreAPI::Role role = getObjectData(dataPackage02);
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.roleId)) {
							cacheTemp.erase(dataPackage02.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage02.roleId, role));
					send(this->outRoleBuffer, role);
					send(cache, cacheTemp);
				}
				PatchRoleData dataPackage03;
				if (try_receive(this->requestPatchRoleBuffer, dataPackage03)) {
					DiscordCoreAPI::Role role = patchObjectData(dataPackage03);
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.roleId)) {
							cacheTemp.erase(dataPackage03.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage03.roleId, role));
					send(this->outRoleBuffer, role);
					send(cache, cacheTemp);
				}
				PutRoleData dataPackage04;
				if (try_receive(this->requestPutRoleBuffer, dataPackage04)) {
					putObjectData(dataPackage04);
				}
				DeleteGuildMemberRoleData dataPackage05;
				if (try_receive(this->requestDeleteRoleBuffer, dataPackage05)) {
					deleteObjectData(dataPackage05);
				}
				DeleteGuildRoleData dataPackage06;
				if (try_receive(this->requestDeleteGuildRoleBuffer, dataPackage06)) {
					deleteObjectData(dataPackage06);
				}
				GetRolesData dataPackage07;
				if (try_receive(this->requestGetRolesBuffer, dataPackage07)) {
					vector<DiscordCoreAPI::Role> roles = getObjectData(dataPackage07);
					send(this->outRolesBuffer, roles);
				}
				PatchRolePositionData dataPackage08;
				if (try_receive(this->requestPatchGuildRolesBuffer, dataPackage08)) {
					vector<DiscordCoreAPI::Role> roles = patchObjectData(dataPackage08);
					send(this->outRolesBuffer, roles);
				}
				PostRoleData dataPackage09;
				if (try_receive(this->requestPostRoleBuffer, dataPackage09)) {
					DiscordCoreAPI::Role role = postObjectData(dataPackage09);
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(role.id)) {
							cacheTemp.erase(role.id);
						}
					}
					cacheTemp.insert(make_pair(dataPackage09.roleId, role));
					send(this->outRoleBuffer, role);
					send(cache, cacheTemp);
				}
				DiscordCoreAPI::RoleData dataPackage10;
				dataPackage10.discordCoreClient = this->discordCoreClient;
				DiscordCoreAPI::Role roleNew(dataPackage10);
				while (this->rolesToInsert.try_pop(roleNew)) {
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(roleNew.id)) {
							cacheTemp.erase(roleNew.id);
						}
					}
					cacheTemp.insert(make_pair(roleNew.id, roleNew));
					send(RoleManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
		}
	};

	class RoleManager {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClientBase;
		friend class DiscordCoreAPI::PermissionsConverter;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;
		friend class DiscordCoreAPI::Roles;

		RoleManager(RoleManager* pointer) {
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<ThreadContext> threadContext{ nullptr };
		HttpAgentResources agentResources{};

		RoleManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<ThreadContext> threadContextNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
			this->threadContext = threadContextNew;
			return *this;
		}

		task<DiscordCoreAPI::Role> fetchAsync(DiscordCoreAPI::FetchRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::fetchAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			GetRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestGetRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::fetchAsync");
			DiscordCoreAPI::RoleData roleData;
			roleData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Role newRole(roleData);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<DiscordCoreAPI::Role> getRoleAsync(DiscordCoreAPI::GetRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			CollectRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestCollectRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::getRoleAsync");
			DiscordCoreAPI::RoleData roleData;
			roleData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Role newRole(roleData);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<DiscordCoreAPI::Role> updateRoleAsync(DiscordCoreAPI::UpdateRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PatchRoleData dataPackageNew;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = dataPackage.permissions;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestPatchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::updateRoleAsync");
			DiscordCoreAPI::RoleData roleData;
			roleData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Role newRole(roleData);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		vector<DiscordCoreAPI::Role> updateRolePositions(DiscordCoreAPI::UpdateRolePositionData dataPackage) {
			vector<DiscordCoreAPI::Role> currentRoles = this->getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			DiscordCoreAPI::Role newRole = this->getRoleAsync({ .guildId = dataPackage.guildId, .roleId = dataPackage.roleId }).get();
			PatchRolePositionData dataPackageNew;
			int positionBeforeMoving = newRole.position;
			for (auto value : currentRoles) {
				RolePositionData newData;
				if (value.position < dataPackage.newPosition && value.position >(int)positionBeforeMoving) {
					newData.roleId = value.id;
					newData.rolePosition = value.position - 1;
					dataPackageNew.rolePositions.push_back(newData);
				}
				else if (value.position != positionBeforeMoving) {
					newData.roleId = value.id;
					newData.rolePosition = value.position;
					dataPackageNew.rolePositions.push_back(newData);
				}
			}
			RolePositionData newData;
			newData.roleId = newRole.id;
			newData.rolePosition = dataPackage.newPosition;
			dataPackageNew.rolePositions.push_back(newData);
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestPatchGuildRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::updateRolePositions");
			vector<DiscordCoreAPI::Role> roleData;
			try_receive(requestAgent.outRolesBuffer, roleData);
			return roleData;
		}

		vector<DiscordCoreAPI::Role> getGuildMemberRoles(DiscordCoreAPI::GetGuildMemberRolesData dataPackage) {
			vector<DiscordCoreAPI::Role> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			vector<DiscordCoreAPI::Role> rolesVectorNew;
			for (auto value : rolesVector) {
				for (auto value2 : dataPackage.guildMember.roles){
					if (value2 == value.id) {
						rolesVectorNew.push_back(value);
					}
				}
			}
			return rolesVectorNew;
		}

		task<vector<DiscordCoreAPI::Role>> getGuildRolesAsync(DiscordCoreAPI::GetGuildRolesData dataPackage) {
			apartment_context mainThread;
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			GetRolesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestGetRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::getGuildRolesAsync");
			vector<DiscordCoreAPI::Role> newRoles;
			try_receive(requestAgent.outRolesBuffer, newRoles);
			co_await mainThread;
			co_return newRoles;
		}

		task<DiscordCoreAPI::Role>createRoleAsync(DiscordCoreAPI::CreateRoleData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PostRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = stoll(dataPackage.permissions);
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestPostRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::createRoleAsync");
			DiscordCoreAPI::RoleData roleData;
			roleData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Role newRole(roleData);
			try_receive(requestAgent.outRoleBuffer, newRole);
			updateRolePositions({ .guildId = dataPackage.guildId, .roleId = roleData.id, .newPosition = dataPackage.position });
			co_await mainThread;
			co_return newRole;
		}
		
		task<void> addRoleToGuildMemberAsync(DiscordCoreAPI::AddRoleToGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PutRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestPutRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::addRoleToGuildMemberAsync");
			co_await mainThread;
			co_return;
		}

		task<void> removeRoleFromGuildMemberAsync(DiscordCoreAPI::RemoveRoleFromGuildMemberData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteGuildMemberRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::removeRoleFromGuildMemberAsync");
			co_await mainThread;
			co_return;
		}

		task<void> removeRoleFromGuildAsync(DiscordCoreAPI::RemoveRoleFromGuildData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteGuildRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteGuildRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::removeRoleFromGuildAsync");
			co_await mainThread;
			co_return;
		}

		task<void> insertRoleAsync(DiscordCoreAPI::Role role) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			requestAgent.rolesToInsert.push(role);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::insertRoleAsync");
			co_await mainThread;
			co_return;
		}

		task<void> removeRoleAsync(string roleId) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Role> cache;
			try_receive(RoleManagerAgent::cache, cache);
			if (cache.contains(roleId)) {
				DiscordCoreAPI::Role role = cache.at(roleId);
				cache.erase(roleId);
			}
			send(RoleManagerAgent::cache, cache);
			co_await mainThread;
			co_return;
		}

		~RoleManager() {}
	};
	overwrite_buffer<map<string, DiscordCoreAPI::Role>> RoleManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> RoleManagerAgent::threadContext{ nullptr };
}
#endif