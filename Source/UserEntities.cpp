/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::AddRecipientToGroupDMData> {
		using ValueType = DiscordCoreAPI::AddRecipientToGroupDMData;
		static constexpr auto parseValue =
			object("channel_id", &ValueType::channelId, "access_token", &ValueType::token, "nick", &ValueType::nick, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::ModifyCurrentUserData> {
		using ValueType = DiscordCoreAPI::ModifyCurrentUserData;
		static constexpr auto parseValue = object("username", &ValueType::userName, "avatar", &ValueType::avatar);
	};
}

namespace DiscordCoreInternal {

	WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData>::WebSocketMessageData(const DiscordCoreAPI::UpdateVoiceStateData& other) noexcept {
		d.channelId = other.channelId;
		d.guildId = other.guildId;
		d.selfDeaf = other.selfDeaf;
		d.selfMute = other.selfMute;
		excludedKeys.emplace("t");
		excludedKeys.emplace("s");
		op = 4;
	}

	WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData>::operator EtfSerializer() noexcept {
		EtfSerializer data{};
		data["op"] = 4;
		if (d.channelId == 0) {
			data["d"]["channel_id"] = DiscordCoreInternal::JsonType::Null;
		} else {
			data["d"]["channel_id"] = d.channelId.operator std::string();
		}
		data["d"]["self_deaf"] = d.selfDeaf;
		data["d"]["self_mute"] = d.selfMute;
		data["d"]["guild_id"] = d.guildId.operator std::string();
		return data;
	}

	WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateDataDC>::WebSocketMessageData(const DiscordCoreAPI::UpdateVoiceStateData& other) noexcept {
		d.channelId = nullptr;
		d.guildId = other.guildId;
		d.selfDeaf = other.selfDeaf;
		d.selfMute = other.selfMute;
		excludedKeys.emplace("t");
		excludedKeys.emplace("s");
		op = 4;
	}

	WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateDataDC>::operator EtfSerializer() noexcept {
		EtfSerializer data{};
		data["op"] = 4;
		data["d"]["channel_id"] = DiscordCoreInternal::JsonType::Null;
		data["d"]["self_deaf"] = d.selfDeaf;
		data["d"]["self_mute"] = d.selfMute;
		data["d"]["guild_id"] = d.guildId.operator std::string();
		return data;
	}
}

namespace DiscordCoreAPI {


	BotUser::BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* baseSocketAgentNew) : User(dataPackage) {
		baseSocketAgent = baseSocketAgentNew;
	}

	User& User::operator=(const UserData& other) noexcept {
		if (this != &other) {
			discriminator = other.discriminator;
			userName = other.userName;
			avatar = other.avatar;
			flags = other.flags;
			id = other.id;
		}
		return *this;
	}

	User::User(const UserData& dataNew) noexcept {
		*this = dataNew;
	}

	User& User::operator=(UserData&& other) noexcept {
		if (this != &other) {
			discriminator = std::move(other.discriminator);
			userName = std::move(other.userName);
			avatar = std::move(other.avatar);
			flags = other.flags;
			id = other.id;
		}
		return *this;
	}

	User::User(UserData&& other) noexcept {
		*this = std::move(other);
	}

	void BotUser::updateVoiceStatus(UpdateVoiceStateData& dataPackage) {
		if (baseSocketAgent) {
			DiscordCoreInternal::EtfSerializer serializer{};
			std::string string{};
			uint32_t shardId = (dataPackage.guildId.operator uint64_t() >> 22) % baseSocketAgent->configManager->getTotalShardCount();
			uint32_t basesocketAgentIndex{ shardId % baseSocketAgent->configManager->getTotalShardCount() };
			if (dataPackage.channelId == 0) {
				DiscordCoreInternal::WebSocketMessageData<UpdateVoiceStateDataDC> data{ dataPackage };
				if (static_cast<DiscordCoreInternal::WebSocketOpCode>(
						baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).dataOpCode) ==
					DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
					serializer = data.operator DiscordCoreInternal::EtfSerializer();
					serializer.refreshString();
					string = serializer.operator std::string();
				} else {
					Jsonifier::JsonifierCore parser{};
					parser.serializeJson(data, string);
				}
			} else {
				DiscordCoreInternal::WebSocketMessageData<UpdateVoiceStateData> data{ dataPackage };
				if (static_cast<DiscordCoreInternal::WebSocketOpCode>(
						baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).dataOpCode) ==
					DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
					serializer = data.operator DiscordCoreInternal::EtfSerializer();
					serializer.refreshString();
					string = serializer.operator std::string();
				} else {
					Jsonifier::JsonifierCore parser{};
					parser.serializeJson(data, string);
				}
			}
			baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).createHeader(string,
				baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).dataOpCode);
			baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).sendMessage(string, false);
		}
	}

	void BotUser::updatePresence(UpdatePresenceData& dataPackage) {
		if (baseSocketAgent) {
			std::string string{};
			uint32_t shardId = 0;
			uint32_t basesocketAgentIndex{};
			if (static_cast<DiscordCoreInternal::WebSocketOpCode>(
					baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).dataOpCode) ==
				DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
				auto serializer = dataPackage.operator DiscordCoreInternal::EtfSerializer();
				serializer.refreshString();
				string = serializer.operator std::string();
			} else {
				Jsonifier::JsonifierCore parser{};
				parser.serializeJson(dataPackage, string);
			}
			baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).createHeader(string,
				baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).dataOpCode);
			baseSocketAgent->discordCoreClient->baseSocketAgentsMap[basesocketAgentIndex]->getClient(shardId).sendMessage(string, false);
		}
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
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Users::addRecipientToGroupDMAsync()";
		Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<void> Users::removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Recipient_From_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
		workload.callStack = "Users::removeRecipientToGroupDMAsync()";
		Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<void> Users::modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/voice-states/@me";
		workload.callStack = "Users::modifyCurrentUserVoiceStateAsync()";
		Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<void> Users::modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/voice-states/" + dataPackage.userId;
		workload.callStack = "Users::modifyUserVoiceStateAsync()";
		Users::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<User> Users::getCurrentUserAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Current_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::getCurrentUserAsync()";
		User returnData{};
		Users::httpsClient->submitWorkloadAndGetResult<UserData>(workload, returnData);
		Users::insertUser(returnData);
		co_return returnData;
	}

	UserData Users::getCachedUser(GetUserData dataPackage) {
		UserData data{};
		data.id = dataPackage.userId;
		if (!Users::cache.contains(dataPackage.userId)) {
			return getUserAsync(dataPackage).get();
		} else {
			return Users::cache.find(dataPackage.userId);
		}
	}

	CoRoutine<User> Users::getUserAsync(GetUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/" + dataPackage.userId;
		workload.callStack = "Users::getUserAsync()";
		User data{};
		Users::httpsClient->submitWorkloadAndGetResult<User>(workload, data);
		if (Users::cache.contains(dataPackage.userId)) {
			data = Users::cache.find(dataPackage.userId);
		} else {
			Users::insertUser(std::move(data));
		}
		co_return data;
	}

	CoRoutine<User> Users::modifyCurrentUserAsync(ModifyCurrentUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User };
		co_await NewThreadAwaitable<User>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/users/@me";
		workload.callStack = "Users::modifyCurrentUserAsync()";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		User returnData{};
		Users::httpsClient->submitWorkloadAndGetResult<User>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ConnectionDataVector> Users::getUserConnectionsAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User_Connections };
		co_await NewThreadAwaitable<ConnectionDataVector>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/users/@me/connections";
		workload.callStack = "Users::getUserConnectionsAsync()";
		ConnectionDataVector returnData{};
		Users::httpsClient->submitWorkloadAndGetResult<ConnectionDataVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationData> Users::getCurrentUserApplicationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Application_Info };
		co_await NewThreadAwaitable<ApplicationData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/oauth2/applications/@me";
		workload.callStack = "Users::getApplicationDataAsync()";
		ApplicationData returnData{};
		Users::httpsClient->submitWorkloadAndGetResult<ApplicationData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<AuthorizationInfoData> Users::getCurrentUserAuthorizationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Authorization_Info };
		co_await NewThreadAwaitable<AuthorizationInfoData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/oauth2/@me";
		workload.callStack = "Users::getCurrentUserAuthorizationInfoAsync()";
		AuthorizationInfoData returnData{};
		Users::httpsClient->submitWorkloadAndGetResult<AuthorizationInfoData>(workload, returnData);
		co_return returnData;
	}

	bool Users::doWeCacheUsers() {
		return Users::doWeCacheUsersBool;
	}

	void Users::insertUser(const UserData& user) {
		if (user.id == 0) {
			return;
		}
		if (Users::doWeCacheUsers()) {
			Users::cache.emplace(UserData{ user });
			if (Users::cache.count() % 10 == 0) {
				std::cout << "USERS COUNT: " << Users::cache.count() << std::endl;
			}
		}
	}

	void Users::insertUser(UserData&& user) {
		if (user.id == 0) {
			return;
		}
		if (Users::doWeCacheUsers()) {
			Users::cache.emplace(UserData{ std::move(user) });
			if (Users::cache.count() % 10 == 0) {
				std::cout << "USERS COUNT: " << Users::cache.count() << std::endl;
			}
		}
	}

	DiscordCoreInternal::HttpsClient* Users::httpsClient{};
	ObjectCache<Snowflake, UserData> Users::cache{};
	bool Users::doWeCacheUsersBool{};
}
