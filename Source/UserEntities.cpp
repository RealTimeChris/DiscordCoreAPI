/*
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
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	AddRecipientToGroupDMData::operator std::string() {
		nlohmann::json data{};
		data["access_token"] = this->token;
		data["nick"] = this->nick;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	void UserData::insertUser(std::unique_ptr<UserData> other) {
		Users::insertUser(std::move(other));
	}

	std::string UserData::getAvatarUrl() {
		return this->avatar.getHashUrl(this->id, 0, HashType::User_Avatar);
	}

	User& User::operator=(UserData&& other) noexcept {
		if (this != &other) {
			this->discriminator = std::move(other.discriminator);
			this->userName = std::move(other.userName);
			this->avatar = std::move(other.avatar);
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	User::User(UserData&& other) noexcept {
		*this = std::move(other);
	}

	User& User::operator=(UserData& other) noexcept {
		if (this != &other) {
			this->discriminator = other.discriminator;
			this->userName = other.userName;
			this->avatar = other.avatar;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	User::User(UserData& dataNew) noexcept {
		*this = dataNew;
	}

	void User::parseObject(const nlohmann::json* theData) {
		DiscordCoreAPI::parseObject(theData, *this);
	}

	UserVector::operator std::vector<User>() {
		return this->theUsers;
	}

	UserVector& UserVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	UserVector::UserVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	void BotUser::updateVoiceStatus(UpdateVoiceStateData& dataPackage) {
		nlohmann::json payload = dataPackage;
		std::string theString{};
		uint32_t shardId = (dataPackage.guildId >> 22) % this->baseSocketAgent->configManager->getTotalShardCount();
		if (this->baseSocketAgent) {
			std::string theString{};
			this->baseSocketAgent->theShardMap[shardId]->stringifyJsonData(&payload, theString,
				static_cast<DiscordCoreInternal::WebSocketSSLShard*>(this->baseSocketAgent->theShardMap[shardId].get())->dataOpCode);
			this->baseSocketAgent->theShardMap[shardId]->sendMessage(theString, true);
		}
	}

	void BotUser::updatePresence(DiscordCoreInternal::UpdatePresenceData& dataPackage) {
		nlohmann::json payload = dataPackage;
		uint32_t shardId = 0;
		if (this->baseSocketAgent) {
			std::string theString{};
			this->baseSocketAgent->theShardMap[shardId]->stringifyJsonData(&payload, theString,
				static_cast<DiscordCoreInternal::WebSocketSSLShard*>(this->baseSocketAgent->theShardMap[shardId].get())->dataOpCode);
			this->baseSocketAgent->theShardMap[shardId]->sendMessage(theString, true);
		}
	}

	BotUser::BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* baseSocketAgentNew) : User(dataPackage) {
		this->baseSocketAgent = baseSocketAgentNew;
	}

	void Users::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		Users::configManager = configManagerNew;
		Users::httpsClient = theClient;
	}

	CoRoutine<void> Users::addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Recipient_To_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/recipients/" + std::to_string(dataPackage.userId);
		workload.content = dataPackage;
		workload.callStack = "Users::addRecipientToGroupDMAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Recipient_From_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/recipients/" + std::to_string(dataPackage.userId);
		workload.callStack = "Users::removeRecipientToGroupDMAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/voice-states/@me";
		workload.callStack = "Users::modifyCurrentUserVoiceStateAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/voice-states/" + std::to_string(dataPackage.userId);
		workload.callStack = "Users::modifyUserVoiceStateAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<User> Users::getCurrentUserAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Current_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::getCurrentUserAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<User>(workload);
	}

	CoRoutine<UserData> Users::getCachedUserAsync(GetUserData dataPackage) {
		co_await NewThreadAwaitable<UserData>();
		std::shared_lock theLock{ Users::theMutex };
		if (!Users::cache.contains(dataPackage.userId)) {
			theLock.unlock();
			auto theUser = getUserAsync(dataPackage).get();
			co_return theUser;

		} else {
			co_return *Users::cache[dataPackage.userId];
		}
	}

	CoRoutine<User> Users::getUserAsync(GetUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/" + std::to_string(dataPackage.userId);
		workload.callStack = "Users::getUserAsync()";
		User theData{};
		if (Users::cache.contains(dataPackage.userId)) {
			theData = Users::getCachedUserAsync({ .userId = dataPackage.userId }).get();
		}
		theData = Users::httpsClient->submitWorkloadAndGetResult<User>(workload, &theData);
		Users::insertUser(std::make_unique<UserData>(theData));
		co_return theData;
	}

	CoRoutine<User> Users::modifyCurrentUserAsync(ModifyCurrentUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::modifyCurrentUserAsync()";
		if (dataPackage.avatar.size() > 0) {
			nlohmann::json responseData = { { "avatar", dataPackage.avatar }, { "userName", dataPackage.userName } };
			workload.content = responseData.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
			;
		} else {
			nlohmann::json responseData = { { "userName", dataPackage.userName } };
			workload.content = responseData.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		}
		co_return Users::httpsClient->submitWorkloadAndGetResult<User>(workload);
	}

	CoRoutine<std::vector<ConnectionData>> Users::getUserConnectionsAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User_Connections };
		co_await NewThreadAwaitable<std::vector<ConnectionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/@me/connections";
		workload.callStack = "Users::getUserConnectionsAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<ConnectionDataVector>(workload);
	}

	CoRoutine<ApplicationData> Users::getCurrentUserApplicationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Application_Info };
		co_await NewThreadAwaitable<ApplicationData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/oauth2/applications/@me";
		workload.callStack = "Users::getApplicationDataAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<ApplicationData>(workload);
	}

	CoRoutine<AuthorizationInfoData> Users::getCurrentUserAuthorizationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Authorization_Info };
		co_await NewThreadAwaitable<AuthorizationInfoData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/oauth2/@me";
		workload.callStack = "Users::getCurrentUserAuthorizationInfoAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<AuthorizationInfoData>(workload);
	}

	void Users::insertUser(std::unique_ptr<UserData> user) {
		std::unique_lock theLock{ Users::theMutex };
		if (user->id == 0) {
			return;
		}
		if (Users::configManager->doWeCacheUsers()) {
			auto userId = user->id;
			Users::cache.insert_or_assign(userId, std::move(user));
		}
	}

	std::map<Snowflake, std::unique_ptr<UserData>> Users::cache{};
	DiscordCoreInternal::HttpsClient* Users::httpsClient{ nullptr };
	ConfigManager* Users::configManager{ nullptr };
	std::shared_mutex Users::theMutex{};
}
