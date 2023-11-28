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
/// AutoModerationEntities.cpp - Source file for the application_command_data classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file AutoModerationEntities.cpp

#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::create_auto_moderation_rule_data> {
		using value_type				 = discord_core_api::create_auto_moderation_rule_data;
		static constexpr auto parseValue = createValue("exemptChannels", &value_type::exemptChannels, "exemptRoles", &value_type::exemptRoles, "triggerMetadata",
			&value_type::triggerMetadata, "actions", &value_type::actions, "triggerType", &value_type::triggerType, "eventType", &value_type::eventType, "guildId",
			&value_type::guildId, "name", &value_type::name, "enabled", &value_type::enabled);
	};

	template<> struct core<discord_core_api::modify_auto_moderation_rule_data> {
		using value_type				 = discord_core_api::modify_auto_moderation_rule_data;
		static constexpr auto parseValue = createValue("exemptChannels", &value_type::exemptChannels, "exemptRoles", &value_type::exemptRoles, "triggerMetadata",
			&value_type::triggerMetadata, "actions", &value_type::actions, "autoModerationRuleId", &value_type::autoModerationRuleId, "eventType", &value_type::eventType,
			"guildId", &value_type::guildId, "name", &value_type::name, "enabled", &value_type::enabled);
	};

}

namespace discord_core_api {

	void auto_moderation_rules::initialize(discord_core_internal::https_client* https_client_new) {
		auto_moderation_rules::httpsClient = https_client_new;
	}

	co_routine<jsonifier::vector<auto_moderation_rule_data>> auto_moderation_rules::listAutoModerationRulesForGuildAsync(list_auto_moderation_rules_for_guild_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Auto_Moderation_Rules };
		co_await newThreadAwaitable<jsonifier::vector<auto_moderation_rule_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules";
		workload.callStack	   = "auto_moderation_rules::listAutoModerationRulesForGuildAsync()";
		jsonifier::vector<auto_moderation_rule_data> returnVector{};
		auto_moderation_rules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnVector);
		co_return std::move(returnVector);
	}

	co_routine<auto_moderation_rule_data> auto_moderation_rules::getAutoModerationRuleAsync(get_auto_moderation_rule_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Auto_Moderation_Rule };
		co_await newThreadAwaitable<auto_moderation_rule_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + dataPackage.autoModerationRuleId.operator jsonifier::string();
		workload.callStack	   = "auto_moderation_rules::getAutoModerationRuleAsync()";
		auto_moderation_rule_data returnData{};
		auto_moderation_rules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<auto_moderation_rule_data> auto_moderation_rules::createAutoModerationRuleAsync(create_auto_moderation_rule_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Auto_Moderation_Rule };
		co_await newThreadAwaitable<auto_moderation_rule_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "auto_moderation_rules::createAutoModerationRuleAsync()";
		auto_moderation_rule_data returnData{};
		auto_moderation_rules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<auto_moderation_rule_data> auto_moderation_rules::modifyAutoModerationRuleAsync(modify_auto_moderation_rule_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Auto_Moderation_Rule };
		co_await newThreadAwaitable<auto_moderation_rule_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + dataPackage.autoModerationRuleId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "auto_moderation_rules::modifyAutoModerationRuleAsync()";
		auto_moderation_rule_data returnData{};
		auto_moderation_rules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> auto_moderation_rules::deleteAutoModerationRuleAsync(delete_auto_moderation_rule_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Auto_Moderation_Rule };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + dataPackage.autoModerationRuleId;
		workload.callStack	   = "auto_moderation_rules::deleteAutoModerationRuleAsync()";
		auto_moderation_rules::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	discord_core_internal::https_client* auto_moderation_rules::httpsClient{};

}
