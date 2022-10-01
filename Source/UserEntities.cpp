/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
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

	AddRecipientToGroupDMData::operator JsonObject() {
		JsonObject theData{};
		theData["access_token"] = this->token;
		theData["nick"] = this->nick;
		return theData;
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

	User& User::operator=(const UserData& other) noexcept {
		if (this != &other) {
			this->discriminator = other.discriminator;
			this->userName = other.userName;
			this->avatar = other.avatar;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	User::User(const UserData& dataNew) noexcept {
		*this = dataNew;
	}

	User::User(simdjson::ondemand::value jsonObjectData) {
		flags |= setBool(this->flags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		this->flags |= setBool(this->flags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		this->flags |= setBool(this->flags, UserFlags::System, getBool(jsonObjectData, "system"));

		this->flags |= setBool(this->flags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		this->discriminator = getString(jsonObjectData, "discriminator");

		this->flags |= getUint32(jsonObjectData, "public_flags");

		this->userName = getString(jsonObjectData, "username");

		this->id = getId(jsonObjectData, "id");

		this->avatar = getString(jsonObjectData, "avatar");

		this->locale = getString(jsonObjectData, "locale");

		this->email = getString(jsonObjectData, "email");

		this->premiumType = static_cast<PremiumType>(getUint8(jsonObjectData, "premium_type"));

		this->flags = getUint32(jsonObjectData, "public_flags");
	}

	UserVector::UserVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					User newData{ value.value() };
					this->theUsers.emplace_back(std::move(newData));
				}
			}
		}
	}

	UserVector::operator std::vector<User>() {
		return this->theUsers;
	}

	void BotUser::updateVoiceStatus(UpdateVoiceStateData& dataPackage) {
		if (this->baseSocketAgent) {
			std::string payload = dataPackage.operator DiscordCoreAPI::JsonObject();
			std::string theString{};
			uint32_t shardId = (dataPackage.guildId >> 22) % this->baseSocketAgent->configManager->getTotalShardCount();
			uint32_t basesocketAgentIndex{ shardId % this->baseSocketAgent->configManager->getTotalShardCount() };
			theString = this->baseSocketAgent->discordCoreClient->baseSocketAgentMap[basesocketAgentIndex]->theShardMap[shardId]->stringifyJsonData(payload,
				static_cast<DiscordCoreInternal::WebSocketSSLShard*>(this->baseSocketAgent->discordCoreClient->baseSocketAgentMap[basesocketAgentIndex]->theShardMap[shardId].get())
					->dataOpCode);
			this->baseSocketAgent->discordCoreClient->baseSocketAgentMap[basesocketAgentIndex]->theShardMap[shardId]->sendMessage(theString, true);
		}
	}

	void BotUser::updatePresence(DiscordCoreInternal::UpdatePresenceData& dataPackage) {
		if (this->baseSocketAgent) {
			std::string payload = dataPackage.operator DiscordCoreAPI::JsonObject();
			std::string theString{};
			uint32_t shardId = 0;
			uint32_t basesocketAgentIndex{ 0 };
			theString = this->baseSocketAgent->discordCoreClient->baseSocketAgentMap[basesocketAgentIndex]->theShardMap[shardId]->stringifyJsonData(payload,
				static_cast<DiscordCoreInternal::WebSocketSSLShard*>(this->baseSocketAgent->discordCoreClient->baseSocketAgentMap[basesocketAgentIndex]->theShardMap[shardId].get())
					->dataOpCode);
			this->baseSocketAgent->discordCoreClient->baseSocketAgentMap[basesocketAgentIndex]->theShardMap[shardId]->sendMessage(theString, true);
		}
	}

	BotUser::BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* baseSocketAgentNew) : User(dataPackage) {
		this->baseSocketAgent = baseSocketAgentNew;
	}

	void Users::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		Users::doWeCacheUsers = configManagerNew->doWeCacheUsers();
		Users::httpsClient = theClient;
	}

	CoRoutine<void> Users::addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Recipient_To_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/recipients/" + std::to_string(dataPackage.userId);
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		auto userData = Users::httpsClient->submitWorkloadAndGetResult<UserData>(workload);
		Users::insertUser(userData);
		co_return userData;
	}

	CoRoutine<UserData> Users::getCachedUserAsync(GetUserData dataPackage) {
		co_await NewThreadAwaitable<UserData>();
		DiscordEntity theData{};
		theData.id = dataPackage.userId;
		if (!Users::cache.contains(*static_cast<UserData*>(&theData))) {
			co_return getUserAsync(dataPackage).get();

		} else {
			co_return Users::cache[*static_cast<UserData*>(&theData)];
		}
	}

	CoRoutine<User> Users::getUserAsync(GetUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/" + std::to_string(dataPackage.userId);
		workload.callStack = "Users::getUserAsync()";
		User theData{};
		theData.id = dataPackage.userId;
		if (!Users::cache.contains(theData)) {
			Users::cache.emplace(theData);
		}
		theData = Users::cache.at(theData);
		theData = Users::httpsClient->submitWorkloadAndGetResult<User>(workload, &theData);
		Users::insertUser(theData);
		co_return theData;
	}

	CoRoutine<User> Users::modifyCurrentUserAsync(ModifyCurrentUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::modifyCurrentUserAsync()";
		if (dataPackage.avatar.size() > 0) {
			JsonObject responseData{};
			responseData["avatar"] = dataPackage.avatar;
			responseData["userName"] = dataPackage.userName;
			workload.content = responseData;
		} else {
			JsonObject responseData{};
			responseData["userName"] = dataPackage.userName;
			workload.content = responseData;
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

	void Users::insertUser(UserData user) {
		if (user.id.operator size_t() == 0) {
			return;
		}
		if (Users::doWeCacheUsers) {
			Users::cache.emplace(std::move(user));
			if (Users::cache.size() % 1000 == 0) {
				std::cout << "USERS COUNT: " << Users::cache.size() << std::endl;
			}
		}
	}

	DiscordCoreInternal::HttpsClient* Users::httpsClient{ nullptr };
	ObjectCache<UserData> Users::cache{};
	bool Users::doWeCacheUsers{ false };
}
