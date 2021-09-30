// RoleEntities.hpp - Header for the role related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROLE_ENTITIES_
#define _ROLE_ENTITIES_

#include "pch.h"
#include "JSONifier.hpp"
#include "GuildMemberEntities.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	struct UpdateRoleData {
		string hexColorValue{ "" };
		bool mentionable{ false };
		string permissions{ "" };
		string guildId{ "" };
		string roleId{ "" };
		bool hoist{ false };
		string name{ "" };
	};

	struct GetGuildRolesData {
		string guildId{ "" };
	};

	struct FetchRoleData {
		string guildId{ "" };
		string roleId{ "" };
	};

	struct UpdateRolePositionData {
		int newPosition{ 0 };
		string guildId{ "" };
		string roleId{ "" };
	};

	struct RemoveRoleFromGuildMemberData {
		string guildId{ "" };
		string userId{ "" };
		string roleId{ "" };
	};

	struct GetRoleData {
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
		string hexColorValue{ "" };
		bool mentionable{ false };
		string permissions{ "0" };
		string guildId{ "" };
		bool hoist{ false };
		string name{ "" };
		int position{ 0 };
	};

	struct GetGuildMemberRolesData {
		GuildMember guildMember{};
		string guildId{ "" };
	};

	class Role : public RoleData {
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
			this->permissions = dataNew.permissions;
			this->mentionable = dataNew.mentionable;
			this->position = dataNew.position;
			this->managed = dataNew.managed;
			this->color = dataNew.color;
			this->hoist = dataNew.hoist;
			this->name = dataNew.name;
			this->tags = dataNew.tags;
			this->id = dataNew.id;
		}
	};
};

namespace DiscordCoreInternal {

	class RoleManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class RoleManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<PatchRolePositionData> requestPatchGuildRolesBuffer{ nullptr };
		unbounded_buffer<DeleteGuildMemberRoleData> requestDeleteRoleBuffer{ nullptr };
		unbounded_buffer<DeleteGuildRoleData> requestDeleteGuildRoleBuffer{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Role>> outRolesBuffer{ nullptr };
		unbounded_buffer<PatchRoleData> requestPatchRoleBuffer{ nullptr };
		unbounded_buffer<PostRoleData> requestPostRoleBuffer{ nullptr };
		unbounded_buffer<GetRolesData> requestGetRolesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Role> outRoleBuffer{ nullptr };
		unbounded_buffer<GetRoleData> requestGetRoleBuffer{ nullptr };
		unbounded_buffer<PutRoleData> requestPutRoleBuffer{ nullptr };
		
		RoleManagerAgent()
			:agent(*RoleManagerAgent::threadContext->scheduler->scheduler) {}

		static void initialize() {
			RoleManagerAgent::threadContext = ThreadManager::getThreadContext().get();
		}

		static void cleanup() {
			RoleManagerAgent::threadContext->releaseContext();
		}

		vector<DiscordCoreAPI::Role> getObjectData(GetRolesData  dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::getObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<DiscordCoreAPI::Role> roleVector;
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				DiscordCoreAPI::RoleData roleData;
				DataParser::parseObject(returnData.data.at(x), &roleData);
				DiscordCoreAPI::Role newRole(roleData);
				roleVector.push_back(newRole);
			}
			return roleVector;
		}

		DiscordCoreAPI::Role getObjectData(GetRoleData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::getObjectData_01");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::getObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::getObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::Role role{};
			for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
				DiscordCoreAPI::Role newRole{};
				DataParser::parseObject(returnData.data.at(x), &newRole);
				if (newRole.id == dataPackage.roleId){
					role = newRole;
					break;
				}
			}
			return role;
		}

		DiscordCoreAPI::Role patchObjectData(PatchRoleData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			workload.content = getModifyRolePayload(dataPackage);
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::patchObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::RoleData roleData;
			DataParser::parseObject(returnData.data, &roleData);
			DiscordCoreAPI::Role newRole(roleData);
			return newRole;
		}

		vector<DiscordCoreAPI::Role> patchObjectData(PatchRolePositionData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_GUILD_ROLES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = getUpdateRolePositionsPayload(dataPackage);
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::patchObjectData_01");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::patchObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::patchObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<DiscordCoreAPI::Role> roleVector;
			for (auto value : returnData.data) {
				DiscordCoreAPI::RoleData newRoleData;
				DataParser::parseObject(value, &newRoleData);
				DiscordCoreAPI::Role newRole(newRoleData);
				roleVector.push_back(newRole);
			}
			return roleVector;
		}

		DiscordCoreAPI::Role postObjectData(PostRoleData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.workloadType = HttpWorkloadType::POST_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles";
			workload.content = getCreateRolePayload(dataPackage);
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::postObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::RoleData roleData;
			DataParser::parseObject(returnData.data, &roleData);
			DiscordCoreAPI::Role newRole(roleData);
			return newRole;
		}

		void putObjectData(PutRoleData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.workloadType = HttpWorkloadType::PUT_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::putObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void deleteObjectData(DeleteGuildMemberRoleData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.workloadType = HttpWorkloadType::DELETE_GUILD_MEMBER_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId + "/roles/" + dataPackage.roleId;
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::deleteObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void deleteObjectData(DeleteGuildRoleData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.workloadType = HttpWorkloadType::DELETE_GUILD_ROLE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/roles/" + dataPackage.roleId;
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "RoleManagerAgent::deleteObjectData_01");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "RoleManagerAgent::deleteObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "RoleManagerAgent::deleteObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void run() {
			try {
				GetRoleData dataPackage02;
				if (try_receive(this->requestGetRoleBuffer, dataPackage02)) {
					DiscordCoreAPI::Role role = getObjectData(dataPackage02);
					send(this->outRoleBuffer, role);
				}
				PatchRoleData dataPackage03;
				if (try_receive(this->requestPatchRoleBuffer, dataPackage03)) {
					DiscordCoreAPI::Role role = patchObjectData(dataPackage03);
					send(this->outRoleBuffer, role);
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
					send(this->outRoleBuffer, role);
				}
			}
			catch (...) {
				DiscordCoreAPI::rethrowException("RoleManagerAgent::run() Error: ");
			}
			this->done();
		}
	};

	class RoleManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClientBase;
		friend class DiscordCoreAPI::PermissionsConverter;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;
		friend class DiscordCoreAPI::Roles;

		RoleManager(RoleManager* pointer)
			: ThreadContext(*ThreadManager::getThreadContext().get()) {
			this->cache = new overwrite_buffer<map<string, DiscordCoreAPI::Role>>();
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

		~RoleManager(){
			delete this->cache;
			this->cache = nullptr;
		}

	protected:

		overwrite_buffer<map<string, DiscordCoreAPI::Role>>* cache{};

		task<DiscordCoreAPI::Role> fetchAsync(DiscordCoreAPI::FetchRoleData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::fetchAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			GetRoleData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestGetRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::RoleData roleData;
			DiscordCoreAPI::Role newRole(roleData);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		task<DiscordCoreAPI::Role> getRoleAsync(DiscordCoreAPI::GetRoleData dataPackage) {
			apartment_context mainThread{};
			co_await resume_background();
			map<string, DiscordCoreAPI::Role> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			DiscordCoreAPI::Role newRole{};
			if (cacheTemp.contains(dataPackage.roleId)) {
				newRole = cacheTemp.at(dataPackage.roleId);
			}
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return newRole;
		}

		task<DiscordCoreAPI::Role> updateRoleAsync(DiscordCoreAPI::UpdateRoleData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			PatchRoleData dataPackageNew;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = dataPackage.permissions;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestPatchRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::RoleData roleData;
			DiscordCoreAPI::Role newRole(roleData);
			try_receive(requestAgent.outRoleBuffer, newRole);
			co_await mainThread;
			co_return newRole;
		}

		vector<DiscordCoreAPI::Role> updateRolePositions(DiscordCoreAPI::UpdateRolePositionData dataPackage) {
			vector<DiscordCoreAPI::Role> currentRoles = this->getGuildRolesAsync({ .guildId = dataPackage.guildId }).get();
			DiscordCoreAPI::Role newRole = this->getRoleAsync({ .roleId = dataPackage.roleId }).get();
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
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestPatchGuildRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
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
			apartment_context mainThread{};
			co_await resume_background();
			if (dataPackage.guildId == "") {
				exception failError("RoleManager::getRoleAsync() Error: Sorry, but you forgot to set the guildId!");
				throw failError;
			}
			GetRolesData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestGetRolesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			vector<DiscordCoreAPI::Role> newRoles;
			try_receive(requestAgent.outRolesBuffer, newRoles);
			co_await mainThread;
			co_return newRoles;
		}

		task<DiscordCoreAPI::Role>createRoleAsync(DiscordCoreAPI::CreateRoleData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			PostRoleData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.hexColorValue = dataPackage.hexColorValue;
			dataPackageNew.hoist = dataPackage.hoist;
			dataPackageNew.mentionable = dataPackage.mentionable;
			dataPackageNew.name = dataPackage.name;
			dataPackageNew.permissions = stoll(dataPackage.permissions);
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestPostRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::RoleData roleData;
			DiscordCoreAPI::Role newRole(roleData);
			try_receive(requestAgent.outRoleBuffer, newRole);
			updateRolePositions({ .newPosition = dataPackage.position,.guildId = dataPackage.guildId,.roleId = roleData.id });
			co_await mainThread;
			co_return newRole;
		}
		
		task<void> addRoleToGuildMemberAsync(DiscordCoreAPI::AddRoleToGuildMemberData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			PutRoleData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestPutRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> removeRoleFromGuildMemberAsync(DiscordCoreAPI::RemoveRoleFromGuildMemberData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			DeleteGuildMemberRoleData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestDeleteRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> removeRoleFromGuildAsync(DiscordCoreAPI::RemoveRoleFromGuildData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			DeleteGuildRoleData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.roleId = dataPackage.roleId;
			RoleManagerAgent requestAgent{};
			send(requestAgent.requestDeleteGuildRoleBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> insertRoleAsync(DiscordCoreAPI::Role role) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Role> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			cacheTemp.insert_or_assign(role.id, role);
			send(this->cache, cacheTemp);
			co_return;
		}

		task<void> removeRoleAsync(string roleId) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Role> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			if (cacheTemp.contains(roleId)) {
				cacheTemp.erase(roleId);
			}
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return;
		}
	};
	shared_ptr<ThreadContext> RoleManagerAgent::threadContext{ nullptr };
}
#endif