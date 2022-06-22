/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
/// https://discordcoreapi.com
/// \file UserEntities.cpp

#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/JSONIfier.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	User& User::operator=(UserData&& other) {
		if (this != &other) {
			this->discriminator = std::move(other.discriminator);
			this->userName = std::move(other.userName);
			this->avatar = std::move(other.avatar);
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	User::User(UserData&& other) {
		*this = std::move(other);
	}

	User& User::operator=(UserData& other) {
		if (this != &other) {
			this->discriminator = other.discriminator;
			this->userName = other.userName;
			this->avatar = other.avatar;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	User::User(UserData& dataNew) {
		*this = dataNew;
	}

	void BotUser::updateVoiceStatus(const UpdateVoiceStateData& dataPackage) {
		nlohmann::json payload = DiscordCoreInternal::JSONIFY(dataPackage);
		this->baseSocketAgent->sendMessage(payload, this->baseSocketAgent->theClients.begin().operator*().second.get());
	}

	void BotUser::updatePresence(UpdatePresenceData& dataPackage) {
		dataPackage.since = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		nlohmann::json payload = DiscordCoreInternal::JSONIFY(dataPackage);
		if (this->baseSocketAgent) {
			this->baseSocketAgent->sendMessage(payload, this->baseSocketAgent->theClients.begin().operator*().second.get());
		}
	}

	BotUser::BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* baseSocketAgentNew) : User(dataPackage) {
		this->baseSocketAgent = baseSocketAgentNew;
	}

	void Users::initialize(DiscordCoreInternal::HttpClient* theClient, bool doWeCacheNew) {
		Users::cache = std::make_unique<std::unordered_map<uint64_t, std::unique_ptr<UserData>>>();
		Users::doWeCache = doWeCacheNew;
		Users::httpClient = theClient;
	}

	CoRoutine<void> Users::addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Recipient_To_Group_Dm);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Recipient_To_Group_Dm;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/recipients/" + std::to_string(dataPackage.userId);
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Users::addRecipientToGroupDMAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Recipient_From_Group_Dm);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Recipient_From_Group_Dm;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/recipients/" + std::to_string(dataPackage.userId);
		workload.callStack = "Users::removeRecipientToGroupDMAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Current_User_Voice_State);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Current_User_Voice_State;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/voice-states/@me";
		workload.callStack = "Users::modifyCurrentUserVoiceStateAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_User_Voice_State);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_User_Voice_State;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/voice-states/" + std::to_string(dataPackage.userId);
		workload.callStack = "Users::modifyUserVoiceStateAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<UserData> Users::getCurrentUserAsync() {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Current_User);
		co_await NewThreadAwaitable<UserData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Current_User;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::getCurrentUserAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<UserData>(workload);
	}

	CoRoutine<UserData> Users::getCachedUserAsync(GetUserData dataPackage) {
		std::shared_lock<std::shared_mutex> theLock{ Users::theMutex };
		co_await NewThreadAwaitable<UserData>();
		if (Users::cache->contains(dataPackage.userId)) {
			co_return *(*Users::cache)[dataPackage.userId];
		} else {
			co_return getUserAsync(dataPackage).get();
		}
	}

	CoRoutine<User> Users::getUserAsync(GetUserData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_User);
		co_await NewThreadAwaitable<User>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_User;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/users/" + std::to_string(dataPackage.userId);
		workload.callStack = "Users::getUserAsync";
		auto userNew = Users::httpClient->submitWorkloadAndGetResult<User>(workload);
		Users::insertUser(userNew);
		co_return userNew;
	}

	CoRoutine<User> Users::modifyCurrentUserAsync(ModifyCurrentUserData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Current_User);
		co_await NewThreadAwaitable<User>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Current_User;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::modifyCurrentUserAsync";
		if (dataPackage.avatar.size() > 0) {
			nlohmann::json responseData = { { "avatar", dataPackage.avatar }, { "userName", dataPackage.userName } };
			workload.content = responseData.dump();
		} else {
			nlohmann::json responseData = { { "userName", dataPackage.userName } };
			workload.content = responseData.dump();
		}
		co_return Users::httpClient->submitWorkloadAndGetResult<User>(workload);
	}

	CoRoutine<std::vector<ConnectionData>> Users::getUserConnectionsAsync() {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_User_Connections);
		co_await NewThreadAwaitable<std::vector<ConnectionData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_User_Connections;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/users/@me/connections";
		workload.callStack = "Users::getUserConnectionsAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<std::vector<ConnectionData>>(workload);
	}

	CoRoutine<ApplicationData> Users::getCurrentUserApplicationInfoAsync() {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Application_Info);
		co_await NewThreadAwaitable<ApplicationData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Application_Info;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/oauth2/applications/@me";
		workload.callStack = "Users::getApplicationDataAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<ApplicationData>(workload);
	}

	CoRoutine<AuthorizationInfoData> Users::getCurrentUserAuthorizationInfoAsync() {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Authorization_Info);
		co_await NewThreadAwaitable<AuthorizationInfoData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Authorization_Info;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/oauth2/@me";
		workload.callStack = "Users::getCurrentUserAuthorizationInfoAsync";
		co_return Users::httpClient->submitWorkloadAndGetResult<AuthorizationInfoData>(workload);
	}

	void Users::insertUser(UserData user) {
		std::unique_lock<std::shared_mutex> theLock{ Users::theMutex };
		if (user.id == 0) {
			return;
		}
		auto newCache = std::make_unique<std::unordered_map<uint64_t, std::unique_ptr<UserData>>>();
		for (auto& [key, value]: *Users::cache) {
			(*newCache)[key] = std::move(value);
		}
		if (Users::doWeCache) {
			(*newCache)[user.id] = std::make_unique<UserData>(user);
		}
		Users::cache.reset(nullptr);
		Users::cache = std::move(newCache);
	}

	std::unique_ptr<std::unordered_map<uint64_t, std::unique_ptr<UserData>>> Users::cache{};
	DiscordCoreInternal::HttpClient* Users::httpClient{ nullptr };
	std::shared_mutex Users::theMutex{};
	bool Users::doWeCache{ false };

}
