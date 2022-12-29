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

	AddRecipientToGroupDMData::operator Jsonifier() {
		Jsonifier data{};
		data["access_token"] = this->token;
		data["nick"] = this->nick;
		return data;
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
		uint32_t newFlags{};
		newFlags = setBool(newFlags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		newFlags = setBool(newFlags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		newFlags = setBool(newFlags, UserFlags::System, getBool(jsonObjectData, "system"));

		newFlags = setBool(newFlags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		this->discriminator = getString(jsonObjectData, "discriminator");

		newFlags = setBool(newFlags, static_cast<UserFlags>(getUint32(jsonObjectData, "public_flags")), true);

		this->flags = static_cast<UserFlags>(newFlags);

		this->userName = getString(jsonObjectData, "username");

		this->id = getId(jsonObjectData, "id");

		this->avatar = getString(jsonObjectData, "avatar");

		this->locale = getString(jsonObjectData, "locale");

		this->email = getString(jsonObjectData, "email");

		this->premiumType = static_cast<PremiumType>(getUint8(jsonObjectData, "premium_type"));
	}

	UserVector::UserVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					User newData{ value.value() };
					this->users.emplace_back(std::move(newData));
				}
			}
		}
	}

	UserVector::operator std::vector<User>() {
		return this->users;
	}

	void BotUser::updateVoiceStatus(UpdateVoiceStateData& dataPackage) {
		if (this->baseSocketAgent) {
			std::string string{};
			uint32_t shardId = (dataPackage.guildId.operator size_t() >> 22) % this->baseSocketAgent->configManager->getTotalShardCount();
			uint32_t basesocketAgentIndex{ shardId % this->baseSocketAgent->configManager->getTotalShardCount() };
			auto serializer = dataPackage.operator Jsonifier();
			if (static_cast<DiscordCoreInternal::WebSocketOpCode>(
					this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId]->dataOpCode) ==
				DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
				serializer.refreshString(JsonifierSerializeType::Etf);
			} else {
				serializer.refreshString(JsonifierSerializeType::Json);
			}
			string = serializer.operator std::string();
			this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId]->createHeader(string,
				this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].get()->dataOpCode);
			this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId]->sendMessage(string,
				false);
		}
	}

	void BotUser::updatePresence(UpdatePresenceData& dataPackage) {
		if (this->baseSocketAgent) {
			std::string string{};
			uint32_t shardId = 0;
			uint32_t basesocketAgentIndex{};
			auto serializer = dataPackage.operator Jsonifier();
			if (static_cast<DiscordCoreInternal::WebSocketOpCode>(
					this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId]->dataOpCode) ==
				DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
				serializer.refreshString(JsonifierSerializeType::Etf);
			} else {
				serializer.refreshString(JsonifierSerializeType::Json);
			}
			string = serializer.operator std::string();
			this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId]->createHeader(string,
				this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].get()->dataOpCode);
			this->baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId]->sendMessage(string,
				false);
		}
	}

	BotUser::BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* baseSocketAgentNew) : User(dataPackage) {
		this->baseSocketAgent = baseSocketAgentNew;
	}

	void Users::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		Users::doWeCacheUsersBool = configManagerNew->doWeCacheUsers();
		Users::httpsClient = client;
	}

	CoRoutine<void> Users::addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Recipient_To_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Users::addRecipientToGroupDMAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Recipient_From_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
		workload.callStack = "Users::removeRecipientToGroupDMAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/voice-states/@me";
		workload.callStack = "Users::modifyCurrentUserVoiceStateAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Users::modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/voice-states/" + dataPackage.userId;
		workload.callStack = "Users::modifyUserVoiceStateAsync()";
		co_return Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<User> Users::getCurrentUserAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Current_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::getCurrentUserAsync()";
		User returnData{};
		auto userData = Users::httpsClient->submitWorkloadAndGetResult<UserData>(workload, &returnData);
		Users::insertUser(userData);
		co_return userData;
	}

	UserData Users::getCachedUser(GetUserData dataPackage) {
		UserData data{};
		data.id = dataPackage.userId;
		if (!Users::cache.contains(*static_cast<UserData*>(&data))) {
			return getUserAsync(dataPackage).get();

		} else {
			data = Users::cache[*static_cast<UserData*>(&data)];
			return data;
		}
	}

	CoRoutine<User> Users::getUserAsync(GetUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/" + dataPackage.userId;
		workload.callStack = "Users::getUserAsync()";
		User data{};
		data = Users::httpsClient->submitWorkloadAndGetResult<User>(workload, &data);
		if (Users::cache.contains(data)) {
			data = Users::cache.at(data);
		} else {
			Users::insertUser(data);
		}
		co_return std::move(data);
	}

	CoRoutine<User> Users::modifyCurrentUserAsync(ModifyCurrentUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::modifyCurrentUserAsync()";
		if (dataPackage.avatar.size() > 0) {
			Jsonifier responseData{};
			responseData["avatar"] = dataPackage.avatar;
			responseData["userName"] = dataPackage.userName;
			responseData.refreshString(JsonifierSerializeType::Json);
			workload.content = responseData.operator std::string();
		} else {
			Jsonifier responseData{};
			responseData["userName"] = dataPackage.userName;
			responseData.refreshString(JsonifierSerializeType::Json);
			workload.content = responseData.operator std::string();
		}
		User returnData{};
		co_return Users::httpsClient->submitWorkloadAndGetResult<User>(workload, &returnData);
	}

	CoRoutine<std::vector<ConnectionData>> Users::getUserConnectionsAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User_Connections };
		co_await NewThreadAwaitable<std::vector<ConnectionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/@me/connections";
		workload.callStack = "Users::getUserConnectionsAsync()";
		ConnectionDataVector returnData{};
		co_return Users::httpsClient->submitWorkloadAndGetResult<ConnectionDataVector>(workload, &returnData);
	}

	CoRoutine<ApplicationData> Users::getCurrentUserApplicationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Application_Info };
		co_await NewThreadAwaitable<ApplicationData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/oauth2/applications/@me";
		workload.callStack = "Users::getApplicationDataAsync()";
		ApplicationData returnData{};
		co_return Users::httpsClient->submitWorkloadAndGetResult<ApplicationData>(workload, &returnData);
	}

	CoRoutine<AuthorizationInfoData> Users::getCurrentUserAuthorizationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Authorization_Info };
		co_await NewThreadAwaitable<AuthorizationInfoData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/oauth2/@me";
		workload.callStack = "Users::getCurrentUserAuthorizationInfoAsync()";
		AuthorizationInfoData returnData{};
		co_return Users::httpsClient->submitWorkloadAndGetResult<AuthorizationInfoData>(workload, &returnData);
	}

	bool Users::doWeCacheUsers() {
		return Users::doWeCacheUsersBool;
	}

	void Users::insertUser(UserData user) {
		if (user.id == 0) {
			return;
		}
		if (Users::doWeCacheUsers()) {
			if (!Users::cache.contains(user)) {
				Users::cache.emplace(std::move(user));
			} else {
				Users::cache[user] = std::move(user);
			}
			if (Users::cache.size() % 10000 == 0) {
				std::cout << "USERS COUNT: " << Users::cache.size() << std::endl;
			}
		}
	}

	DiscordCoreInternal::HttpsClient* Users::httpsClient{ nullptr };
	bool Users::doWeCacheUsersBool{};
	ObjectCache<UserData> Users::cache{};
}
