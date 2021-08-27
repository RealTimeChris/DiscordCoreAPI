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

namespace DiscordCoreAPI {

	class Role {
	public:
		RoleData data{};
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		Role() {};

		Role(RoleData roleData, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->data = roleData;
			this->discordCoreClient = discordCoreClientNew;
		}

	protected:
		friend class DiscordCoreClient;
		friend class RoleManager;
		friend class RoleManagerAgent;
		friend class Guild;
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
		friend class Guild;
		friend class RoleManager;

		static overwrite_buffer<map<string, DiscordCoreAPI::Role>> cache;
		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;

		unbounded_buffer<DiscordCoreInternal::PatchRolePositionData> requestPatchGuildRolesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::DeleteGuildMemberRoleData> requestDeleteRoleBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::DeleteGuildRoleData> requestDeleteGuildRoleBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::CollectRoleData> requestCollectRoleBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PatchRoleData> requestPatchRoleBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetRolesData> requestGetRolesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PostRoleData> requestPostRoleBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetRoleData> requestGetRoleBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PutRoleData> requestPutRoleBuffer{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Role>> outRolesBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Role> outRoleBuffer{ nullptr };
		concurrent_queue<DiscordCoreAPI::Role> rolesToInsert{};

		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };

		RoleManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew)
			:agent(*RoleManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
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

		vector<DiscordCoreAPI::Role> getObjectData(DiscordCoreInternal::GetRolesData  dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::getObjectData_00");
			DiscordCoreInternal::HttpData returnData;
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
				DiscordCoreInternal::DataParser::parseObject(returnData.data.at(x), &roleData);
				DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
				roleVector.push_back(newRole);
			}
			return roleVector;
		}

		DiscordCoreAPI::Role getObjectData(DiscordCoreInternal::GetRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::getObjectData_01");
			DiscordCoreInternal::HttpData returnData;
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
				DiscordCoreInternal::DataParser::parseObject(returnData.data.at(x), &roleData);
				DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
				cacheTemp.insert(make_pair(newRole.data.id, newRole));
			}
			send(RoleManagerAgent::cache, cacheTemp);
			DiscordCoreAPI::RoleData roleData;
			if (cacheTemp.contains(dataPackage.roleId)) {
				roleData = cacheTemp.at(dataPackage.roleId).data;
				cacheTemp.erase(dataPackage.roleId);
			}
			DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
			return newRole;
		}

		DiscordCoreAPI::Role patchObjectData(DiscordCoreInternal::PatchRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.content = DiscordCoreInternal::getModifyRolePayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::patchObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::RoleData roleData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &roleData);
			DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
			return newRole;
		}

		vector<DiscordCoreAPI::Role> patchObjectData(DiscordCoreInternal::PatchRolePositionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_GUILD_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = DiscordCoreInternal::getUpdateRolePositionsPayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::patchObjectData_01");
			DiscordCoreInternal::HttpData returnData;
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
				DiscordCoreInternal::DataParser::parseObject(value, &newRoleData);
				DiscordCoreAPI::Role newRole(newRoleData, this->discordCoreClient);
				roleVector.push_back(newRole);
			}
			return roleVector;
		}

		DiscordCoreAPI::Role postObjectData(DiscordCoreInternal::PostRoleData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = DiscordCoreInternal::getCreateRolePayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManagerAgent::postObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::RoleData roleData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &roleData);
			DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
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
			requestAgent.getError("RoleManagerAgent::putObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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
			requestAgent.getError("RoleManagerAgent::deleteObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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
			requestAgent.getError("RoleManagerAgent::deleteObjectData_01");
			DiscordCoreInternal::HttpData returnData;
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
				DiscordCoreInternal::CollectRoleData dataPackage01;
				if (try_receive(this->requestCollectRoleBuffer, dataPackage01)) {
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.roleId)) {
							DiscordCoreAPI::Role role = cacheTemp.at(dataPackage01.roleId);
							send(this->outRoleBuffer, role);
						}
					}
				}
				DiscordCoreInternal::GetRoleData dataPackage02;
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
				DiscordCoreInternal::PatchRoleData dataPackage03;
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
				DiscordCoreInternal::GetRolesData dataPackage07;
				if (try_receive(this->requestGetRolesBuffer, dataPackage07)) {
					vector<DiscordCoreAPI::Role> roles = getObjectData(dataPackage07);
					send(this->outRolesBuffer, roles);
				}
				DiscordCoreInternal::PatchRolePositionData dataPackage08;
				if (try_receive(this->requestPatchGuildRolesBuffer, dataPackage08)) {
					vector<DiscordCoreAPI::Role> roles = patchObjectData(dataPackage08);
					send(this->outRolesBuffer, roles);
				}
				DiscordCoreInternal::PostRoleData dataPackage09;
				if (try_receive(this->requestPostRoleBuffer, dataPackage09)) {
					DiscordCoreAPI::Role role = postObjectData(dataPackage09);
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(role.data.id)) {
							cacheTemp.erase(role.data.id);
						}
					}
					cacheTemp.insert(make_pair(dataPackage09.roleId, role));
					send(this->outRoleBuffer, role);
					send(cache, cacheTemp);
				}
				DiscordCoreAPI::RoleData dataPackage10;
				DiscordCoreAPI::Role roleNew(dataPackage10, this->discordCoreClient);
				while (this->rolesToInsert.try_pop(roleNew)) {
					map<string, DiscordCoreAPI::Role> cacheTemp;
					if (try_receive(RoleManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(roleNew.data.id)) {
							cacheTemp.erase(roleNew.data.id);
						}
					}
					cacheTemp.insert(make_pair(roleNew.data.id, roleNew));
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

		RoleManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		task<DiscordCoreAPI::Role> fetchAsync(DiscordCoreAPI::FetchRoleData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::fetchAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::GetRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestGetRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::fetchAsync");
			DiscordCoreAPI::RoleData roleData;
			DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_return newRole;
		}

		task<DiscordCoreAPI::Role> getRoleAsync(DiscordCoreAPI::GetRoleData dataPackage) {
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::CollectRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestCollectRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::getRoleAsync");
			DiscordCoreAPI::RoleData roleData;
			DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_return newRole;
		}

		task<DiscordCoreAPI::Role> updateRoleAsync(DiscordCoreAPI::UpdateRoleData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PatchRoleData dataPackageNew;
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
			DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_return newRole;
		}

		vector<DiscordCoreAPI::Role> updateRolePositions(DiscordCoreAPI::UpdateRolePositionData dataPackage) {
			vector<DiscordCoreAPI::Role> currentRoles = this->getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			DiscordCoreAPI::Role newRole = this->getRoleAsync({ .guildId = dataPackage.guildId, .roleId = dataPackage.roleId }).get();
			DiscordCoreInternal::PatchRolePositionData dataPackageNew;
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
				for (auto value2 : dataPackage.guildMember.data.roles){
					if (value2 == value.data.id) {
						rolesVectorNew.push_back(value);
					}
				}
			}
			return rolesVectorNew;
		}

		task<vector<DiscordCoreAPI::Role>> getGuildRolesAsync(DiscordCoreAPI::GetGuildRolesData dataPackage) {
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			DiscordCoreInternal::GetRolesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestGetRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::getGuildRolesAsync");
			vector<DiscordCoreAPI::Role> newRoles;
			try_receive(requestAgent.outRolesBuffer, newRoles);
			co_return newRoles;
		}

		task<DiscordCoreAPI::Role>createRoleAsync(DiscordCoreAPI::CreateRoleData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PostRoleData dataPackageNew;
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
			DiscordCoreAPI::Role newRole(roleData, this->discordCoreClient);
			try_receive(requestAgent.outRoleBuffer, newRole);
			updateRolePositions({ .guildId = dataPackage.guildId, .roleId = roleData.id, .newPosition = dataPackage.position });
			co_return newRole;
		}
		
		task<void> addRoleToGuildMemberAsync(DiscordCoreAPI::AddRoleToGuildMemberData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PutRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestPutRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::addRoleToGuildMemberAsync");
			co_return;
		}

		task<void> removeRoleFromGuildMemberAsync(DiscordCoreAPI::RemoveRoleFromGuildMemberData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteGuildMemberRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::removeRoleFromGuildMemberAsync");
			co_return;
		}

		task<void> removeRoleFromGuildAsync(DiscordCoreAPI::RemoveRoleFromGuildData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteGuildRoleData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteGuildRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::removeRoleFromGuildAsync");
			co_return;
		}

		task<void> insertRoleAsync(DiscordCoreAPI::Role role) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			RoleManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			requestAgent.rolesToInsert.push(role);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("RoleManager::insertRoleAsync");
			co_return;
		}

		task<void> removeRoleAsync(string roleId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Role> cache;
			try_receive(RoleManagerAgent::cache, cache);
			if (cache.contains(roleId)) {
				DiscordCoreAPI::Role role = cache.at(roleId);
				cache.erase(roleId);
			}
			send(RoleManagerAgent::cache, cache);
			co_return;
		}

		~RoleManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class DiscordCoreClientBase;
		friend class DiscordCoreClient;
		friend class Roles;
		friend class EventHandler;
		friend class Guild;
		
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };

	};
	overwrite_buffer<map<string, DiscordCoreAPI::Role>> RoleManagerAgent::cache{ nullptr };
	shared_ptr<DiscordCoreInternal::ThreadContext> RoleManagerAgent::threadContext{ nullptr };
}
#endif