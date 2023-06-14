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
/// AutoModerationEntities.cpp - Source file for the ApplicationCommand classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file AutoModerationEntities.cpp

#include <discordcoreapi/AutoModerationEntities.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::CreateAutoModerationRuleData> {
		using ValueType = DiscordCoreAPI::CreateAutoModerationRuleData;
		static constexpr auto parseValue = object("exemptChannels", &ValueType::exemptChannels, "exemptRoles", &ValueType::exemptRoles,
			"triggerMetadata", &ValueType::triggerMetadata, "actions", &ValueType::actions, "triggerType", &ValueType::triggerType, "eventType",
			&ValueType::eventType, "guildId", &ValueType::guildId, "name", &ValueType::name, "enabled", &ValueType::enabled);
	};

	template<> struct Core<DiscordCoreAPI::ModifyAutoModerationRuleData> {
		using ValueType = DiscordCoreAPI::ModifyAutoModerationRuleData;
		static constexpr auto parseValue = object("exemptChannels", &ValueType::exemptChannels, "exemptRoles", &ValueType::exemptRoles,
			"triggerMetadata", &ValueType::triggerMetadata, "actions", &ValueType::actions, "autoModerationRuleId", &ValueType::autoModerationRuleId,
			"eventType", &ValueType::eventType, "guildId", &ValueType::guildId, "name", &ValueType::name, "enabled", &ValueType::enabled);
	};

}

namespace DiscordCoreAPI {

	void AutoModerationRules::initialize(DiscordCoreInternal::HttpsClient* HttpsClientNew) {
		AutoModerationRules::httpsClient = HttpsClientNew;
	}

	CoRoutine<std::vector<AutoModerationRule>> AutoModerationRules::listAutoModerationRulesForGuildAsync(
		ListAutoModerationRulesForGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rules };
		co_await NewThreadAwaitable<std::vector<AutoModerationRule>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules";
		workload.callStack = "AutoModerationRules::listAutoModerationRulesForGuildAsync()";
		AutoModerationRuleVector returnVector{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRuleVector>(workload, returnVector);
		co_return returnVector;
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::getAutoModerationRuleAsync(GetAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::getAutoModerationRuleAsync()";
		AutoModerationRule returnData{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::createAutoModerationRuleAsync(CreateAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "AutoModerationRules::createAutoModerationRuleAsync()";
		AutoModerationRule returnData{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "AutoModerationRules::modifyAutoModerationRuleAsync()";
		AutoModerationRule returnData{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> AutoModerationRules::deleteAutoModerationRuleAsync(DeleteAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::deleteAutoModerationRuleAsync()";
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	DiscordCoreInternal::HttpsClient* AutoModerationRules::httpsClient{};

}
