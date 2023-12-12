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

	template<> struct core<discord_core_api::add_recipient_to_group_dmdata> {
		using value_type = discord_core_api::add_recipient_to_group_dmdata;
		static constexpr auto parseValue =
			createValue("channel_id", &value_type::channelId, "access_token", &value_type::token, "nick", &value_type::nick, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::modify_current_user_data> {
		using value_type				 = discord_core_api::modify_current_user_data;
		static constexpr auto parseValue = createValue("username", &value_type::userName, "avatar", &value_type::avatar);
	};
}
namespace discord_core_api {

	namespace discord_core_internal {

		websocket_message_data<update_voice_state_data>::websocket_message_data(const update_voice_state_data& other) {
			d.channelId = other.channelId;
			d.guildId	= other.guildId;
			d.selfDeaf	= other.selfDeaf;
			d.selfMute	= other.selfMute;
			jsonifierExcludedKeys.emplace("t");
			jsonifierExcludedKeys.emplace("s");
			op = 4;
		}

		websocket_message_data<update_voice_state_data>::operator etf_serializer() {
			etf_serializer data{};
			data["op"] = 4;
			if (d.channelId == 0) {
				data["d"]["channel_id"] = discord_core_internal::json_type::null_t;
			} else {
				data["d"]["channel_id"] = d.channelId.operator jsonifier::string();
			}
			data["d"]["self_deaf"] = d.selfDeaf;
			data["d"]["self_mute"] = d.selfMute;
			data["d"]["guild_id"]  = d.guildId.operator jsonifier::string();
			return data;
		}

		websocket_message_data<update_voice_state_data_dc>::websocket_message_data(const update_voice_state_data& other) {
			d.channelId = nullptr;
			d.guildId	= other.guildId;
			d.selfDeaf	= other.selfDeaf;
			d.selfMute	= other.selfMute;
			jsonifierExcludedKeys.emplace("t");
			jsonifierExcludedKeys.emplace("s");
			op = 4;
		}

		websocket_message_data<update_voice_state_data_dc>::operator etf_serializer() {
			etf_serializer data{};
			data["op"]				= 4;
			data["d"]["channel_id"] = discord_core_internal::json_type::null_t;
			data["d"]["self_deaf"]	= d.selfDeaf;
			data["d"]["self_mute"]	= d.selfMute;
			data["d"]["guild_id"]	= d.guildId.operator jsonifier::string();
			return data;
		}
	}

	user_cache_data& user_cache_data::operator=(const user_data& other) {
		premiumType = static_cast<premium_type>(other.premiumType);
		setFlagValue(user_flags::Verified, other.verified);
		if (other.avatarDecoration != "") {
			avatarDecoration = other.avatarDecoration;
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(user_flags::System, other.system);
		setFlagValue(user_flags::Bot, other.bot);
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

	user_cache_data::user_cache_data(const user_data& dataNew) {
		*this = dataNew;
	}

	user_cache_data& user_cache_data::operator=(user_data&& other) noexcept {
		premiumType = static_cast<premium_type>(other.premiumType);
		if (other.avatarDecoration != "") {
			avatarDecoration = std::move(other.avatarDecoration);
		}
		if (other.discriminator != "") {
			discriminator = std::move(other.discriminator);
		}
		setFlagValue(user_flags::Verified, other.verified);
		setFlagValue(user_flags::System, other.system);
		if (other.globalName != "") {
			globalName = std::move(other.globalName);
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(user_flags::Bot, other.bot);
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

	user_cache_data::operator user_data() {
		discord_core_api::user_data returnData{};
		returnData.verified			= getFlagValue<user_flags>(user_flags::Verified);
		returnData.system			= getFlagValue<user_flags>(user_flags::System);
		returnData.bot				= getFlagValue<user_flags>(user_flags::Bot);
		returnData.discriminator	= discriminator;
		returnData.globalName		= globalName;
		returnData.userName			= userName;
		returnData.avatarDecoration = avatarDecoration;
		returnData.accentColor		= accentColor;
		returnData.avatar			= avatar;
		returnData.banner			= banner;
		returnData.flags			= flags;
		returnData.id				= id;
		return returnData;
	}

	user_cache_data::user_cache_data(user_data&& other) noexcept {
		*this = std::move(other);
	}

	user_data::user_data(snowflake newId) {
		id = newId;
	}

	bot_user::bot_user(user_data& dataPackage, discord_core_internal::base_socket_agent* baseSocketAgentNew) : user_data(dataPackage) {
		baseSocketAgent = baseSocketAgentNew;
	}

	void bot_user::updateVoiceStatus(update_voice_state_data dataPackage) {
		if (baseSocketAgent) {
			jsonifier::string_base<uint8_t> string{};
			uint64_t shardId = (dataPackage.guildId.operator const uint64_t&() >> 22) % discord_core_client::getInstance()->configManager.getTotalShardCount();
			uint64_t basesocketAgentIndex{ shardId % discord_core_client::getInstance()->configManager.getTotalShardCount() };
			if (dataPackage.channelId == 0) {
				discord_core_internal::websocket_message_data<update_voice_state_data_dc> data{};
				update_voice_state_data_dc dcData{};
				dcData.guildId	= dataPackage.guildId;
				dcData.selfDeaf = dataPackage.selfDeaf;
				dcData.selfMute = dataPackage.selfMute;
				data.d			= dcData;
				data.op			= 4;
				if (static_cast<discord_core_internal::websocket_op_code>(
						discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode) ==
					discord_core_internal::websocket_op_code::Op_Binary) {
					auto serializer = data.operator discord_core_internal::etf_serializer();
					string			= serializer.operator jsonifier::string_base<uint8_t>();
				} else {
					parser.serializeJson(data, string);
				}
			} else {
				discord_core_internal::websocket_message_data<update_voice_state_data> data{};
				data.d	= dataPackage;
				data.op = 4;
				if (static_cast<discord_core_internal::websocket_op_code>(
						discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode) ==
					discord_core_internal::websocket_op_code::Op_Binary) {
					auto serializer = data.operator discord_core_internal::etf_serializer();
					string			= serializer.operator jsonifier::string_base<uint8_t>();
				} else {
					parser.serializeJson(data, string);
				}
			}
			discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].createHeader(string,
				discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode);
			discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].sendMessage(string, false);
		}
	}

	void bot_user::updatePresence(update_presence_data dataPackage) {
		if (baseSocketAgent) {
			jsonifier::string_base<uint8_t> string{};
			uint64_t shardId = 0;
			uint64_t basesocketAgentIndex{};
			discord_core_internal::websocket_message_data<update_presence_data> data{};
			data.d = dataPackage;
			data.jsonifierExcludedKeys.emplace("s");
			for (auto& value: data.d.activities) {
				if (value.url == "") {
					value.jsonifierExcludedKeys.emplace("url");
				}
			}
			data.op = 3;
			if (static_cast<discord_core_internal::websocket_op_code>(
					discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode) ==
				discord_core_internal::websocket_op_code::Op_Binary) {
				auto serializer = data.operator discord_core_internal::etf_serializer();
				string			= serializer.operator jsonifier::string_base<uint8_t>();
			} else {
				parser.serializeJson(data, string);
			}
			discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].createHeader(string,
				discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].dataOpCode);
			discord_core_client::getInstance()->baseSocketAgentsMap[basesocketAgentIndex]->shardMap[shardId].sendMessage(string, true);
		}
	}

	void users::initialize(discord_core_internal::https_client* client, config_manager* configManagerNew) {
		users::doWeCacheUsersBool = configManagerNew->doWeCacheUsers();
		users::httpsClient		  = client;
	}

	co_routine<void> users::addRecipientToGroupDMAsync(add_recipient_to_group_dmdata dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Recipient_To_Group_Dm };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "users::addRecipientToGroupDMAsync()";
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> users::removeRecipientFromGroupDMAsync(remove_recipient_from_group_dmdata dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Recipient_From_Group_Dm };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/recipients/" + dataPackage.userId;
		workload.callStack	   = "users::removeRecipientToGroupDMAsync()";
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> users::modifyCurrentUserVoiceStateAsync(modify_current_user_voice_state_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Current_User_Voice_State };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/voice-states/@me";
		workload.callStack	   = "users::modifyCurrentUserVoiceStateAsync()";
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> users::modifyUserVoiceStateAsync(modify_user_voice_state_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_User_Voice_State };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/voice-states/" + dataPackage.userId;
		workload.callStack	   = "users::modifyUserVoiceStateAsync()";
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<user_data> users::getCurrentUserAsync() {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Current_User };
		co_await newThreadAwaitable<user_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/users/@me";
		workload.callStack	   = "users::getCurrentUserAsync()";
		user_data returnData{};
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		auto newId = returnData.id;
		insertUser(static_cast<user_cache_data>(returnData));
		co_return cache[newId];
	}

	user_cache_data users::getCachedUser(const get_user_data dataPackage) {
		if (cache.contains(dataPackage.userId)) {
			return cache[dataPackage.userId];
		} else {
			return getUserAsync({ .userId = dataPackage.userId }).get();
		}
	}

	co_routine<user_data> users::getUserAsync(get_user_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_User };
		co_await newThreadAwaitable<user_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/users/" + dataPackage.userId;
		workload.callStack	   = "users::getUserAsync()";
		user_data data{ dataPackage.userId };
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheUsersBool) {
			insertUser(static_cast<user_cache_data>(data));
		}
		co_return data;
	}

	co_routine<user_data> users::modifyCurrentUserAsync(modify_current_user_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Current_User };
		co_await newThreadAwaitable<user_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/users/@me";
		workload.callStack	   = "users::modifyCurrentUserAsync()";
		parser.serializeJson(dataPackage, workload.content);
		user_data returnData{};
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<connection_data>> users::getUserConnectionsAsync() {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_User_Connections };
		co_await newThreadAwaitable<jsonifier::vector<connection_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/users/@me/connections";
		workload.callStack	   = "users::getUserConnectionsAsync()";
		jsonifier::vector<connection_data> returnData{};
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<application_data> users::getCurrentUserApplicationInfoAsync() {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Application_Info };
		co_await newThreadAwaitable<application_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/oauth2/applications/@me";
		workload.callStack	   = "users::getApplicationDataAsync()";
		application_data returnData{};
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<authorization_info_data> users::getCurrentUserAuthorizationInfoAsync() {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Authorization_Info };
		co_await newThreadAwaitable<authorization_info_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/oauth2/@me";
		workload.callStack	   = "users::getCurrentUserAuthorizationInfoAsync()";
		authorization_info_data returnData{};
		users::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	bool users::doWeCacheUsers() {
		return users::doWeCacheUsersBool;
	}

	discord_core_internal::https_client* users::httpsClient{};
	object_cache<user_cache_data> users::cache{};
	bool users::doWeCacheUsersBool{};
}
