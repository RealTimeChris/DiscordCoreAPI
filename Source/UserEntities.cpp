/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// UserEntities.cpp - Source file for user related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file UserEntities.cpp

#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/JSONIfier.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Http.hpp>

namespace DiscordCoreAPI {

	User& User::operator=(UserData& other) {
		this->discriminator = other.discriminator;
		this->publicFlags = other.publicFlags;
		this->premiumType = other.premiumType;
		this->createdAt = other.createdAt;
		this->userFlags = other.userFlags;
		this->userName = other.userName;
		this->locale = other.locale;
		this->avatar = other.avatar;
		this->email = other.email;
		this->flags = other.flags;
		this->id = other.id;
		return *this;
	}

	User::User(UserData& dataNew) {
		*this = dataNew;
	}

	void BotUser::updateVoiceStatus(UpdateVoiceStateData dataPackage) {
		nlohmann::json payload = DiscordCoreInternal::JSONIFY(dataPackage);
		this->baseSocketAgent->sendMessage(payload);
	}

	void BotUser::updatePresence(UpdatePresenceData dataPackage) {
		dataPackage.since =
			static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		nlohmann::json payload = DiscordCoreInternal::JSONIFY(dataPackage);
		this->baseSocketAgent->sendMessage(payload);
	}

	BotUser::BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* baseSocketAgentNew) : User(dataPackage) {
		this->baseSocketAgent = baseSocketAgentNew;
	}

	void Users::initialize(DiscordCoreInternal::HttpClient* theClient) {
		Users::httpClient = theClient;
	}

	CoRoutine<void> Users::addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Put_Recipient_To_Group_Dm];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Put_Recipient_To_Group_Dm] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Recipient_To_Group_Dm;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Users::addRecipientToGroupDMAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::addRecipientToGroupDMAsync()");
		}
	}

	CoRoutine<void> Users::removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Recipient_From_Group_Dm];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Recipient_From_Group_Dm] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Recipient_From_Group_Dm;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
			workload.callStack = "Users::removeRecipientToGroupDMAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::removeRecipientFromGroupDMAsync()");
		}
	}

	CoRoutine<void> Users::modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Current_User_Voice_State];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Current_User_Voice_State] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Current_User_Voice_State;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/voice-states/@me";
			workload.callStack = "Users::modifyCurrentUserVoiceStateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::modifyCurrentUserVoiceStateAsync()");
		}
	}

	CoRoutine<void> Users::modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_User_Voice_State];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_User_Voice_State] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_User_Voice_State;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/voice-states/" + dataPackage.userId;
			workload.callStack = "Users::modifyUserVoiceStateAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::modifyUserVoiceStateAsync()");
		}
	}

	CoRoutine<UserData> Users::getCurrentUserAsync() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Current_User];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Current_User] += 1;
			co_await NewThreadAwaitable<UserData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Current_User;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/users/@me";
			workload.callStack = "Users::getCurrentUserAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<UserData>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::getCurrentUserAsync()");
		}
	}

	CoRoutine<User> Users::getCachedUserAsync(GetUserData dataPackage) {
		try {
			co_await NewThreadAwaitable<User>();
			if (Users::cache.contains(dataPackage.userId)) {
				co_return Users::cache[dataPackage.userId];
			} else {
				co_return getUserAsync(dataPackage).get();
			}
		} catch (...) {
			reportException("Users::getCachedUserAsync()");
		}
	}

	CoRoutine<User> Users::getUserAsync(GetUserData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_User];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_User] += 1;
			co_await NewThreadAwaitable<User>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_User;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/users/" + dataPackage.userId;
			workload.callStack = "Users::getUserAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<User>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::getUserAsync()");
		}
	}

	CoRoutine<User> Users::modifyCurrentUserAsync(ModifyCurrentUserData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Current_User];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Current_User] += 1;
			co_await NewThreadAwaitable<User>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Current_User;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/users/@me";
			workload.callStack = "Users::modifyCurrentUserAsync";
			if (dataPackage.avatar.size() > 0) {
				nlohmann::json responseData = { { "avatar", dataPackage.avatar }, { "username", dataPackage.username } };
				workload.content = responseData.dump();
			} else {
				nlohmann::json responseData = { { "username", dataPackage.username } };
				workload.content = responseData.dump();
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<User>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::modifyCurrentUserAsync()");
		}
	}

	CoRoutine<std::vector<ConnectionData>> Users::getUserConnectionsAsync() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_User_Connections];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_User_Connections] += 1;
			co_await NewThreadAwaitable<std::vector<ConnectionData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_User_Connections;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/users/@me/connections";
			workload.callStack = "Users::getUserConnectionsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<ConnectionData>>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::getUserConnectionsAsync()");
		}
	}

	CoRoutine<ApplicationData> Users::getCurrentUserApplicationInfoAsync() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Application_Info];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Application_Info] += 1;
			co_await NewThreadAwaitable<ApplicationData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Application_Info;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/oauth2/applications/@me";
			workload.callStack = "Users::getApplicationDataAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<ApplicationData>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::getApplicationDataAsync()");
		}
	}

	CoRoutine<AuthorizationInfoData> Users::getCurrentUserAuthorizationInfoAsync() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Authorization_Info];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Authorization_Info] += 1;
			co_await NewThreadAwaitable<AuthorizationInfoData>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Authorization_Info;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/oauth2/@me";
			workload.callStack = "Users::getCurrentUserAuthorizationInfoAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<AuthorizationInfoData>(*Users::httpClient, workload);
		} catch (...) {
			reportException("Users::getCurrentUserAuthorizationInfoAsync()");
		}
	}

	void Users::insertUser(User user) {
		try {
			if (user.id == "") {
				return;
			}
			Users::cache.insert_or_assign(user.id, user);
		} catch (...) {
			reportException("Users::insertUser()");
		}
	}

	DiscordCoreInternal::HttpClient* Users::httpClient{ nullptr };
	std::unordered_map<std::string, User> Users::cache{};

}
