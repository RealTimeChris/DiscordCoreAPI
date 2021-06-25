// RoleStuff.hpp - Header for the Role-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_STUFF_
#define _ROLE_STUFF_

#include "pch.h"
#include "JSONifier.hpp"
#include "FoundationEntities.hpp"
#include "GuildMemberStuff.hpp"

namespace DiscordCoreAPI {

	class Role {
	public:
		RoleData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		Role() {};

	protected:
		friend class DiscordCoreClient;
		friend class RoleManager;
		friend class RoleManagerAgent;
		friend class Guild;

		Role(RoleData roleData,  DiscordCoreClient* discordCoreClientNew) {
			this->data = roleData;
			this->discordCoreClient = discordCoreClientNew;
		}
	};

	struct UpdateRoleData {
		string guildId;
		string roleId;
		string name;
		string permissions;
		string hexColorValue;
		bool hoist;
		bool mentionable;
	};

	struct GetRolesData{
		string guildId;
	};

	struct FetchRoleData {
		string guildId;
		string roleId;
	};

	struct RemoveRoleFromGuildMemberData {
		string guildId;
		string userId;
		string roleId;
	};

	struct GetRoleData {
		string guildId;
		string roleId;
	};

	struct DeleteGuildRoleData {
		string guildId;
		string roleId;
	};

	struct GetGuildMemberRolesData {
		string guildId;
		GuildMember guildMember;
	};

	struct AddRoleToGuildMemberData {
		string guildId;
		string userId;
		string roleId;
	};

	struct CreateRoleData {
		string name;
		string permissions;
		string hexColorValue;
		bool hoist;
		bool mentionable;
		string guildId;
	};

	class RoleManagerAgent :agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class RoleManager;

		static unbounded_buffer<DiscordCoreInternal::FetchRolesData>* requestFetchRolesBuffer;
		static unbounded_buffer<DiscordCoreInternal::FetchRoleData>* requestFetchRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetRoleData>* requestGetRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchRoleData>* requestPatchRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::PutRoleData>* requestPutRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostRoleData>* requestPostRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteGuildMemberRoleData>* requestDeleteRoleBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteGuildRoleData>* requestDeleteGuildRoleBuffer;
		static unbounded_buffer<Role>* outRoleBuffer;
		static unbounded_buffer<vector<Role>>* outRolesBuffer;
		static concurrent_queue<Role>* rolesToInsert;
		static overwrite_buffer<map<string, Role>>* cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient{ nullptr };

		RoleManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* discordCoreClientNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize() {
			RoleManagerAgent::requestGetRoleBuffer = new unbounded_buffer<DiscordCoreInternal::GetRoleData>;
			RoleManagerAgent::requestPatchRoleBuffer = new unbounded_buffer<DiscordCoreInternal::PatchRoleData>;
			RoleManagerAgent::requestPostRoleBuffer = new unbounded_buffer<DiscordCoreInternal::PostRoleData>;
			RoleManagerAgent::requestDeleteRoleBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteGuildMemberRoleData>;
			RoleManagerAgent::requestFetchRolesBuffer = new unbounded_buffer<DiscordCoreInternal::FetchRolesData>;
			RoleManagerAgent::requestFetchRoleBuffer = new unbounded_buffer<DiscordCoreInternal::FetchRoleData>;
			RoleManagerAgent::requestPutRoleBuffer = new unbounded_buffer<DiscordCoreInternal::PutRoleData>;
			RoleManagerAgent::requestDeleteGuildRoleBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteGuildRoleData>;
			RoleManagerAgent::rolesToInsert = new concurrent_queue<Role>;
			RoleManagerAgent::outRolesBuffer = new unbounded_buffer<vector<Role>>;
			RoleManagerAgent::outRoleBuffer = new unbounded_buffer<Role>;
			RoleManagerAgent::cache = new overwrite_buffer<map<string, Role>>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		vector<Role> getObjectData(DiscordCoreInternal::FetchRolesData  dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::getObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::getObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::getObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<Role> roleVector;
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				RoleData roleData;
				DiscordCoreInternal::parseObject(returnData.data.at(x), &roleData);
				Role newRole(roleData, this->discordCoreClient);
				roleVector.push_back(newRole);
			}
			return roleVector;
		}

		Role getObjectData(DiscordCoreInternal::FetchRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::getObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::getObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::getObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			map<string, Role> cacheTemp = receive(RoleManagerAgent::cache, 1U);
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				RoleData roleData;
				DiscordCoreInternal::parseObject(returnData.data.at(x), &roleData);
				Role newRole(roleData, this->discordCoreClient);
				cacheTemp.insert(make_pair(newRole.data.id, newRole));
			}
			asend(RoleManagerAgent::cache, cacheTemp);
			RoleData roleData;
			if (cacheTemp.contains(dataPackage.roleId)) {
				roleData = cacheTemp.at(dataPackage.roleId).data;
				cacheTemp.erase(dataPackage.roleId);
			}
			Role newRole(roleData, this->discordCoreClient);
			return newRole;
		}

		Role patchObjectData(DiscordCoreInternal::PatchRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::paatchObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			RoleData roleData;
			DiscordCoreInternal::parseObject(returnData.data, &roleData);
			Role newRole(roleData, this->discordCoreClient);
			return newRole;
		}

		Role postObjectData(DiscordCoreInternal::PostRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::postObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::postObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::postObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			RoleData roleData;
			DiscordCoreInternal::parseObject(returnData.data, &roleData);
			Role newRole(roleData, this->discordCoreClient);
			return newRole;
		}

		void putObjectData(DiscordCoreInternal::PutRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::putObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::putObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::putObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DiscordCoreInternal::DeleteGuildMemberRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::deleteObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DiscordCoreInternal::DeleteGuildRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_GUILD_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::deleteObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void run() {
			try {
				DiscordCoreInternal::GetRoleData dataPackage01;
				if (try_receive(RoleManagerAgent::requestGetRoleBuffer, dataPackage01)) {
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.roleId)) {
							Role role = cacheTemp.at(dataPackage01.roleId);
							send(RoleManagerAgent::outRoleBuffer, role);
						}
					}
				}
				DiscordCoreInternal::FetchRoleData dataPackage02;
				if (try_receive(RoleManagerAgent::requestFetchRoleBuffer, dataPackage02)) {
					Role role = getObjectData(dataPackage02);
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.roleId)) {
							cacheTemp.erase(dataPackage02.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage02.roleId, role));
					send(RoleManagerAgent::outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::PatchRoleData dataPackage03;
				if (try_receive(RoleManagerAgent::requestPatchRoleBuffer, dataPackage03)) {
					Role role = patchObjectData(dataPackage03);
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.roleId)) {
							cacheTemp.erase(dataPackage03.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage03.roleId, role));
					send(RoleManagerAgent::outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::PutRoleData dataPackage04;
				if (try_receive(RoleManagerAgent::requestPutRoleBuffer, dataPackage04)) {
					putObjectData(dataPackage04);
				}
				DiscordCoreInternal::DeleteGuildMemberRoleData dataPackage05;
				if (try_receive(RoleManagerAgent::requestDeleteRoleBuffer, dataPackage05)) {
					deleteObjectData(dataPackage05);
				}
				DiscordCoreInternal::DeleteGuildRoleData dataPackage06;
				if (try_receive(RoleManagerAgent::requestDeleteGuildRoleBuffer, dataPackage06)) {
					deleteObjectData(dataPackage06);
				}
				DiscordCoreInternal::FetchRolesData dataPackage07;
				if (try_receive(RoleManagerAgent::requestFetchRolesBuffer, dataPackage07)) {
					vector<Role> roles = getObjectData(dataPackage07);
					send(RoleManagerAgent::outRolesBuffer, roles);
				}
				DiscordCoreInternal::PostRoleData dataPacakge08;
				if (try_receive(RoleManagerAgent::requestPostRoleBuffer, dataPacakge08)) {
					Role role = postObjectData(dataPacakge08);
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(role.data.id)) {
							cacheTemp.erase(role.data.id);
						}
					}
					cacheTemp.insert(make_pair(dataPacakge08.roleId, role));
					send(RoleManagerAgent::outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				RoleData dataPackage09;
				Role roleNew(dataPackage09, this->discordCoreClient);
				while (RoleManagerAgent::rolesToInsert->try_pop(roleNew)) {
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(roleNew.data.id)) {
							cacheTemp.erase(roleNew.data.id);
						}
					}
					cacheTemp.insert(make_pair(roleNew.data.id, roleNew));
					asend(RoleManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class RoleManager {
	public:

		task<Role> fetchAsync(FetchRoleData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::fetchAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::FetchRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestFetchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			this->threadContext->releaseGroup(groupIdNew);
			co_return newRole;
		}

		task<Role> getRoleAsync(GetRoleData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::GetRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestGetRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::getRoleAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			this->threadContext->releaseGroup(groupIdNew);
			co_return newRole;
		}

		task<Role> updateRoleAsync(UpdateRoleData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PatchRoleData dataPackageNew;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = dataPackage.permissions;
			dataPackageNew.content = DiscordCoreInternal::getModifyRolePayload(dataPackageNew);
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestPatchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::updateRoleAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			this->threadContext->releaseGroup(groupIdNew);
			co_return newRole;
		}

		vector<Role> getGuildMemberRolesAsync(GetGuildMemberRolesData dataPackage) {
			vector<Role> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			vector<Role> rolesVectorNew;
			for (auto value : rolesVector) {
				for (auto value2 : dataPackage.guildMember.data.roles) {
					if (value2 == value.data.id) {
						rolesVectorNew.push_back(value);
					}
				}
			}
			return rolesVector;
		}

		task<vector<Role>> getGuildRolesAsync(GetRolesData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::FetchRolesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestFetchRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::getGuildRolesAsync() Error: " << error.what() << endl << endl;
			}
			vector<Role> newRoles;
			try_receive(RoleManagerAgent::outRolesBuffer, newRoles);
			this->threadContext->releaseGroup(groupIdNew);
			co_return newRoles;
		}

		task<Role>createRoleAsync(CreateRoleData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PostRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = stoll(dataPackage.permissions);
			dataPackageNew.content = DiscordCoreInternal::getCreateRolePayload(dataPackageNew);
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestPostRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(RoleManagerAgent::outRoleBuffer, newRole);
			this->threadContext->releaseGroup(groupIdNew);
			co_return newRole;
		}
		
		task<void> addRoleToGuildMemberAsync(AddRoleToGuildMemberData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PutRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestPutRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::addRoleToGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeRoleFromGuildMemberAsync(RemoveRoleFromGuildMemberData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteGuildMemberRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestDeleteRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::removeRoleFromGuildMemberAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeRoleFromGuildAsync(DeleteGuildRoleData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteGuildRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(RoleManagerAgent::requestDeleteGuildRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::removeRoleFromGuildAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> insertRoleAsync(Role role) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			RoleManagerAgent::rolesToInsert->push(role);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::inserRoleAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeRoleAsync(string roleId) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Role> cache;
			try_receive(RoleManagerAgent::cache, cache);
			if (cache.contains(roleId)) {
				Role role = cache.at(roleId);
				cache.erase(roleId);
			}
			asend(RoleManagerAgent::cache, cache);
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		~RoleManager() {
			this->threadContext->releaseGroup(this->groupId);
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreClient* discordCoreClient{ nullptr };
		unsigned int groupId;

		RoleManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
			this->groupId = this->threadContext->createGroup();
		}

	};
	unbounded_buffer<DiscordCoreInternal::FetchRolesData>* RoleManagerAgent::requestFetchRolesBuffer;
	unbounded_buffer<DiscordCoreInternal::GetRoleData>* RoleManagerAgent::requestGetRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchRoleData>* RoleManagerAgent::requestPatchRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::PutRoleData>* RoleManagerAgent::requestPutRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::PostRoleData>* RoleManagerAgent::requestPostRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteGuildMemberRoleData>* RoleManagerAgent::requestDeleteRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::FetchRoleData>* RoleManagerAgent::requestFetchRoleBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteGuildRoleData>* RoleManagerAgent::requestDeleteGuildRoleBuffer;
	unbounded_buffer<vector<Role>>* RoleManagerAgent::outRolesBuffer;
	unbounded_buffer<Role>* RoleManagerAgent::outRoleBuffer;
	concurrent_queue<Role>* RoleManagerAgent::rolesToInsert;
	overwrite_buffer<map<string, Role>>* RoleManagerAgent::cache;
}
#endif