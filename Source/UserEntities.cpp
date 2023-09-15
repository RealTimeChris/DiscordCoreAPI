/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// UserEntities.cpp - Source file for user related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file UserEntities.cpp

#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace jsonifier {

	template<> struct core<DiscordCoreAPI::AddRecipientToGroupDMData> {
		using ValueType = DiscordCoreAPI::AddRecipientToGroupDMData;
		static constexpr auto parseValue =
			createObject("channel_id", &ValueType::channelId, "access_token", &ValueType::token, "nick", &ValueType::nick, "user_id", &ValueType::userId);
	};

	template<> struct core<DiscordCoreAPI::ModifyCurrentUserData> {
		using ValueType					 = DiscordCoreAPI::ModifyCurrentUserData;
		static constexpr auto parseValue = createObject("username", &ValueType::userName, "avatar", &ValueType::avatar);
	};
}
namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		WebSocketMessageData<UpdateVoiceStateData>::WebSocketMessageData(const UpdateVoiceStateData& other) {
			d.channelId = other.channelId;
			d.guildId	= other.guildId;
			d.selfDeaf	= other.selfDeaf;
			d.selfMute	= other.selfMute;
			excludedKeys.emplace("t");
			excludedKeys.emplace("s");
			op = 4;
		}

		WebSocketMessageData<UpdateVoiceStateData>::operator EtfSerializer() {
			EtfSerializer data{};
			data["op"] = 4;
			if (d.channelId == 0) {
				data["d"]["channel_id"] = DiscordCoreInternal::JsonType::Null;
			} else {
				data["d"]["channel_id"] = d.channelId.operator std::string();
			}
			data["d"]["self_deaf"] = d.selfDeaf;
			data["d"]["self_mute"] = d.selfMute;
			data["d"]["guild_id"]  = d.guildId.operator std::string();
			return data;
		}

		WebSocketMessageData<UpdateVoiceStateDataDC>::WebSocketMessageData(const UpdateVoiceStateData& other) {
			d.channelId = nullptr;
			d.guildId	= other.guildId;
			d.selfDeaf	= other.selfDeaf;
			d.selfMute	= other.selfMute;
			excludedKeys.emplace("t");
			excludedKeys.emplace("s");
			op = 4;
		}

		WebSocketMessageData<UpdateVoiceStateDataDC>::operator EtfSerializer() {
			EtfSerializer data{};
			data["op"]				= 4;
			data["d"]["channel_id"] = DiscordCoreInternal::JsonType::Null;
			data["d"]["self_deaf"]	= d.selfDeaf;
			data["d"]["self_mute"]	= d.selfMute;
			data["d"]["guild_id"]	= d.guildId.operator std::string();
			return data;
		}
	}

	UserCacheData& UserCacheData::operator=(const UserData& other) {
		premiumType = static_cast<PremiumType>(other.premiumType);
		setFlagValue(UserFlags::Verified, other.verified);
		if (other.avatarDecoration != "") {
			avatarDecoration = other.avatarDecoration;
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(UserFlags::System, other.system);
		setFlagValue(UserFlags::Bot, other.bot);
		if (other.discriminator != "") {
			discriminator = other.discriminator;
		}
		if (other.accentColor != 0) {
			accentColor = other.accentColor;
		}
		if (other.globalName != "") {
			globalName = other.globalName;
		}
		if (other.userName != "") {
			userName = other.userName;
		}
		if (other.banner != "") {
			banner = other.banner;
		}
		if (other.avatar != "") {
			avatar = other.avatar;
		}
		if (other.id != 0) {
			id = other.id;
		}
		return *this;
	}

	UserCacheData::UserCacheData(const UserData& dataNew) {
		*this = dataNew;
	}

	UserCacheData& UserCacheData::operator=(UserData&& other) noexcept {
		premiumType = static_cast<PremiumType>(other.premiumType);
		if (other.avatarDecoration != "") {
			avatarDecoration = std::move(other.avatarDecoration);
		}
		if (other.discriminator != "") {
			discriminator = std::move(other.discriminator);
		}
		setFlagValue(UserFlags::Verified, other.verified);
		setFlagValue(UserFlags::System, other.system);
		if (other.globalName != "") {
			globalName = std::move(other.globalName);
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(UserFlags::Bot, other.bot);
		if (other.userName != "") {
			userName = std::move(other.userName);
		}
		if (other.banner != "") {
			banner = std::move(other.banner);
		}
		if (other.avatar != "") {
			avatar = std::move(other.avatar);
		}
		if (other.accentColor != 0) {
			accentColor = other.accentColor;
		}
		if (other.id != 0) {
			id = other.id;
		}
		return *this;
	}

	UserCacheData::operator UserData() {
		DiscordCoreAPI::UserData returnData{};
		returnData.verified			= getFlagValue<UserFlags>(UserFlags::Verified);
		returnData.system			= getFlagValue<UserFlags>(UserFlags::System);
		returnData.bot				= getFlagValue<UserFlags>(UserFlags::Bot);
		returnData.discriminator	= discriminator.operator std::string();
		returnData.globalName		= globalName.operator std::string();
		returnData.userName			= userName.operator std::string();
		returnData.avatarDecoration = avatarDecoration;
		returnData.accentColor		= accentColor;
		returnData.avatar			= avatar;
		returnData.banner			= banner;
		returnData.flags			= flags;
		returnData.id				= id;
		return returnData;
	}

	UserCacheData::UserCacheData(UserData&& other) noexcept {
		*this = std::move(other);
	}

	UserData::UserData(Snowflake newId) {
		id = newId;
	}

	BotUser::BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* baseSocketAgentNew) : UserData(dataPackage) {
		baseSocketAgent = baseSocketAgentNew;
	}

	void BotUser::updateVoiceStatus(UpdateVoiceStateData& dataPackage) {
		if (baseSocketAgent) {
			std::basic_string<uint8_t> string{};
			uint32_t shardId = (dataPackage.guildId.operator const uint64_t&() >> 22) % DiscordCoreClient::getInstance()->configManager.getTotalShardCount();
			uint32_t basesocketAgentIndex{ shardId % DiscordCoreClient::getInstance()->configManager.getTotalShardCount() };
			if (dataPackage.channelId == 0) {
				DiscordCoreInternal::WebSocketMessageData<UpdateVoiceStateDataDC> data{};
				UpdateVoiceStateDataDC dcData{};
				dcData.guildId	= dataPackage.guildId;
				dcData.selfDeaf = dataPackage.selfDeaf;
				dcData.selfMute = dataPackage.selfMute;
				data.d			= dcData;
				data.op			= 4;
				if (static_cast<DiscordCoreInternal::WebSocketOpCode>(DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode) ==
					DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
					auto serializer = data.operator DiscordCoreInternal::EtfSerializer();
					string			= serializer.operator std::basic_string<uint8_t, std::char_traits<unsigned char>, std::allocator<unsigned char>>();
				} else {
					parser.serializeJson(data, string);
				}
			} else {
				DiscordCoreInternal::WebSocketMessageData<UpdateVoiceStateData> data{};
				data.d	= dataPackage;
				data.op = 4;
				if (static_cast<DiscordCoreInternal::WebSocketOpCode>(DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode) ==
					DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
					auto serializer = data.operator DiscordCoreInternal::EtfSerializer();
					string			= serializer.operator std::basic_string<uint8_t, std::char_traits<unsigned char>, std::allocator<unsigned char>>();
				} else {
					parser.serializeJson(data, string);
				}
			}
			DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].createHeader(string,
				DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode);
			DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].sendMessage(string, false);
		}
	}

	void BotUser::updatePresence(UpdatePresenceData& dataPackage) {
		if (baseSocketAgent) {
			std::basic_string<uint8_t> string{};
			uint32_t shardId = 0;
			uint32_t basesocketAgentIndex{};
			DiscordCoreInternal::WebSocketMessageData<UpdatePresenceData> data{};
			data.d = dataPackage;
			data.excludedKeys.emplace("s");
			for (auto& value: data.d.activities) {
				if (value.url == "") {
					value.excludedKeys.emplace("url");
				}
			}
			data.op = 3;
			if (static_cast<DiscordCoreInternal::WebSocketOpCode>(DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode) ==
				DiscordCoreInternal::WebSocketOpCode::Op_Binary) {
				auto serializer = data.operator DiscordCoreInternal::EtfSerializer();
				string			= serializer.operator std::basic_string<uint8_t, std::char_traits<unsigned char>, std::allocator<unsigned char>>();
			} else {
				parser.serializeJson<true>(data, string);
			}
			DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].createHeader(string,
				DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode);
			DiscordCoreClient::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].sendMessage(string, true);
		}
	}

	void Users::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		Users::doWeCacheUsersBool = configManagerNew->doWeCacheUsers();
		Users::httpsClient		  = client;
	}

	CoRoutine<void> Users::addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Recipient_To_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Users::addRecipientToGroupDMAsync()";
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Users::removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Recipient_From_Group_Dm };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
		workload.callStack	   = "Users::removeRecipientToGroupDMAsync()";
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Users::modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/voice-states/@me";
		workload.callStack	   = "Users::modifyCurrentUserVoiceStateAsync()";
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Users::modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_User_Voice_State };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/voice-states/" + dataPackage.userId;
		workload.callStack	   = "Users::modifyUserVoiceStateAsync()";
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<UserData> Users::getCurrentUserAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Current_User };
		co_await NewThreadAwaitable<UserData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/users/@me";
		workload.callStack	   = "Users::getCurrentUserAsync()";
		UserData returnData{};
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		auto newId = returnData.id;
		insertUser(static_cast<UserCacheData>(returnData));
		co_return cache[newId];
	}

	UserCacheData Users::getCachedUser(GetUserData dataPackage) {
		if (cache.contains(dataPackage.userId)) {
			return cache[dataPackage.userId];
		} else {
			return {};
		}
	}

	CoRoutine<UserData> Users::getUserAsync(GetUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User };
		co_await NewThreadAwaitable<UserData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/users/" + dataPackage.userId;
		workload.callStack	   = "Users::getUserAsync()";
		UserData data{ dataPackage.userId };
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheUsersBool) {
			insertUser(static_cast<UserCacheData>(data));
		}
		co_return data;
	}

	CoRoutine<UserData> Users::modifyCurrentUserAsync(ModifyCurrentUserData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_User };
		co_await NewThreadAwaitable<UserData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/users/@me";
		workload.callStack	   = "Users::modifyCurrentUserAsync()";
		parser.serializeJson(dataPackage, workload.content);
		UserData returnData{};
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<jsonifier::vector<ConnectionData>> Users::getUserConnectionsAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_User_Connections };
		co_await NewThreadAwaitable<jsonifier::vector<ConnectionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/users/@me/connections";
		workload.callStack	   = "Users::getUserConnectionsAsync()";
		jsonifier::vector<ConnectionData> returnData{};
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationData> Users::getCurrentUserApplicationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Application_Info };
		co_await NewThreadAwaitable<ApplicationData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/oauth2/applications/@me";
		workload.callStack	   = "Users::getApplicationDataAsync()";
		ApplicationData returnData{};
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<AuthorizationInfoData> Users::getCurrentUserAuthorizationInfoAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Authorization_Info };
		co_await NewThreadAwaitable<AuthorizationInfoData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/oauth2/@me";
		workload.callStack	   = "Users::getCurrentUserAuthorizationInfoAsync()";
		AuthorizationInfoData returnData{};
		Users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	bool Users::doWeCacheUsers() {
		return Users::doWeCacheUsersBool;
	}

	DiscordCoreInternal::HttpsClient* Users::httpsClient{};
	ObjectCache<UserCacheData> Users::cache{};
	bool Users::doWeCacheUsersBool{};
}
