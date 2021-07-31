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
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		Role() {};

	protected:
		friend class DiscordCoreClient;
		friend class RoleManager;
		friend class RoleManagerAgent;
		friend class Guild;

		Role(RoleData roleData,  shared_ptr<DiscordCoreClient> discordCoreClientNew) {
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

	struct UpdateRolePositionData {
		string guildId;
		string roleId;
		int newPosition;
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
		string permissions = "0";
		string hexColorValue;
		bool hoist;
		bool mentionable;
		string guildId;
		int position;
	};

	class RoleManagerAgent : agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class RoleManager;

		static overwrite_buffer<map<string, Role>> cache;

		unbounded_buffer<DiscordCoreInternal::FetchRolesData> requestFetchRolesBuffer;
		unbounded_buffer<DiscordCoreInternal::FetchRoleData> requestFetchRoleBuffer;
		unbounded_buffer<DiscordCoreInternal::GetRoleData> requestGetRoleBuffer;
		unbounded_buffer<DiscordCoreInternal::PatchRoleData> requestPatchRoleBuffer;
		unbounded_buffer<DiscordCoreInternal::PutRoleData> requestPutRoleBuffer;
		unbounded_buffer<DiscordCoreInternal::PostRoleData> requestPostRoleBuffer;
		unbounded_buffer<DiscordCoreInternal::DeleteGuildMemberRoleData> requestDeleteRoleBuffer;
		unbounded_buffer<DiscordCoreInternal::DeleteGuildRoleData> requestDeleteGuildRoleBuffer;
		unbounded_buffer<DiscordCoreInternal::UpdateRolePositionData> requestPatchGuildRolesBuffer;
		unbounded_buffer<Role> outRoleBuffer;
		unbounded_buffer<vector<Role>> outRolesBuffer;
		concurrent_queue<Role> rolesToInsert;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		RoleManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> discordCoreClientNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		bool getError(exception& error) {
			return try_receive(this->errorBuffer, error);
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
				cout << "RoleManagerAgent::paatchObjectData() Error 00: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectData() Error 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectData() Success 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			RoleData roleData;
			DiscordCoreInternal::parseObject(returnData.data, &roleData);
			Role newRole(roleData, this->discordCoreClient);
			return newRole;
		}

		vector<Role> patchObjectData(DiscordCoreInternal::UpdateRolePositionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_GUILD_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManagerAgent::paatchObjectData() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectData() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectData() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<Role> roleVector;
			for (auto value : returnData.data) {
				RoleData newRoleData;
				DiscordCoreInternal::parseObject(value, &newRoleData);
				Role newRole(newRoleData, this->discordCoreClient);
				roleVector.push_back(newRole);
			}
			return roleVector;
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
				if (try_receive(this->requestGetRoleBuffer, dataPackage01)) {
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.roleId)) {
							Role role = cacheTemp.at(dataPackage01.roleId);
							send(this->outRoleBuffer, role);
						}
					}
				}
				DiscordCoreInternal::FetchRoleData dataPackage02;
				if (try_receive(this->requestFetchRoleBuffer, dataPackage02)) {
					Role role = getObjectData(dataPackage02);
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.roleId)) {
							cacheTemp.erase(dataPackage02.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage02.roleId, role));
					send(this->outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::PatchRoleData dataPackage03;
				if (try_receive(this->requestPatchRoleBuffer, dataPackage03)) {
					Role role = patchObjectData(dataPackage03);
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.roleId)) {
							cacheTemp.erase(dataPackage03.roleId);
						}
					}
					cacheTemp.insert(make_pair(dataPackage03.roleId, role));
					send(this->outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::PutRoleData dataPackage04;
				if (try_receive(this->requestPutRoleBuffer, dataPackage04)) {
					putObjectData(dataPackage04);
				}
				DiscordCoreInternal::DeleteGuildMemberRoleData dataPackage05;
				if (try_receive(this->requestDeleteRoleBuffer, dataPackage05)) {
					deleteObjectData(dataPackage05);
				}
				DiscordCoreInternal::DeleteGuildRoleData dataPackage06;
				if (try_receive(this->requestDeleteGuildRoleBuffer, dataPackage06)) {
					deleteObjectData(dataPackage06);
				}
				DiscordCoreInternal::FetchRolesData dataPackage07;
				if (try_receive(this->requestFetchRolesBuffer, dataPackage07)) {
					vector<Role> roles = getObjectData(dataPackage07);
					send(this->outRolesBuffer, roles);
				}
				DiscordCoreInternal::UpdateRolePositionData dataPackage08;
				if (try_receive(this->requestPatchGuildRolesBuffer, dataPackage08)) {
					vector<Role> roles = patchObjectData(dataPackage08);
					send(this->outRolesBuffer, roles);
				}
				DiscordCoreInternal::PostRoleData dataPackage09;
				if (try_receive(this->requestPostRoleBuffer, dataPackage09)) {
					Role role = postObjectData(dataPackage09);
					map<string, Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(role.data.id)) {
							cacheTemp.erase(role.data.id);
						}
					}
					cacheTemp.insert(make_pair(dataPackage09.roleId, role));
					send(this->outRoleBuffer, role);
					asend(cache, cacheTemp);
				}
				RoleData dataPackage10;
				Role roleNew(dataPackage10, this->discordCoreClient);
				while (this->rolesToInsert.try_pop(roleNew)) {
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
				send(this->errorBuffer, e);
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
			send(requestAgent.requestFetchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
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
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::GetRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(requestAgent.requestGetRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::getRoleAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
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
			send(requestAgent.requestPatchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::updateRoleAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
			this->threadContext->releaseGroup(groupIdNew);
			co_return newRole;
		}

		vector<Role> updateRolePositions(UpdateRolePositionData dataPackage) {
			vector<Role> currentRoles = this->getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			Role newRole = this->getRoleAsync({ .guildId = dataPackage.guildId, .roleId = dataPackage.roleId }).get();
			DiscordCoreInternal::UpdateRolePositionData dataPackageNew;
			int positionBeforeMoving = newRole.data.position;
			for (auto value : currentRoles) {
				DiscordCoreInternal::RolePositionData newData;
				if (value.data.position < dataPackage.newPosition && value.data.position >(int)positionBeforeMoving) {
					newData.roleId = value.data.id;
					newData.rolePosition = value.data.position - 1;
					dataPackageNew.rolePositions.push_back(newData);
				}
				else if (value.data.position != positionBeforeMoving) {
					newData.roleId = value.data.id;
					newData.rolePosition = value.data.position;
					dataPackageNew.rolePositions.push_back(newData);
				}
			}
			DiscordCoreInternal::RolePositionData newData;
			newData.roleId = newRole.data.id;
			newData.rolePosition = dataPackage.newPosition;
			dataPackageNew.rolePositions.push_back(newData);
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.content = DiscordCoreInternal::getUpdateRolePositionsPayload(dataPackageNew);
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(requestAgent.requestPatchGuildRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::updateRolePositionsAsync() Error: " << error.what() << endl << endl;
			}
			vector<Role> roleData;
			try_receive(requestAgent.outRolesBuffer, roleData);
			return roleData;
		}

		vector<Role> getGuildMemberRoles(GetGuildMemberRolesData dataPackage) {
			vector<Role> rolesVector = getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			vector<Role> rolesVectorNew;
			for (auto value : rolesVector) {
				for (auto value2 : dataPackage.guildMember.data.roles) {
					if (value2 == value.data.id) {
						rolesVectorNew.push_back(value);
					}
				}
			}
			return rolesVectorNew;
		}

		task<vector<Role>> getGuildRolesAsync(GetRolesData dataPackage) {
			unsigned int groupIdNew;
			groupIdNew = this->threadContext->createGroup();
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::FetchRolesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(requestAgent.requestFetchRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::getGuildRolesAsync() Error: " << error.what() << endl << endl;
			}
			vector<Role> newRoles;
			try_receive(requestAgent.outRolesBuffer, newRoles);
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
			send(requestAgent.requestPostRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "RoleManager::createRoleAsync() Error: " << error.what() << endl << endl;
			}
			RoleData roleData;
			Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
			updateRolePositions({ .guildId = dataPackage.guildId, .roleId = roleData.id, .newPosition = dataPackage.position });
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
			send(requestAgent.requestPutRoleBuffer, dataPackageNew);
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
			send(requestAgent.requestDeleteRoleBuffer, dataPackageNew);
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
			send(requestAgent.requestDeleteGuildRoleBuffer, dataPackageNew);
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
			requestAgent.rolesToInsert.push(role);
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

		RoleManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
			this->groupId = this->threadContext->createGroup();
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
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		unsigned int groupId;

	};
	overwrite_buffer<map<string, Role>> RoleManagerAgent::cache;
	
}
#endif