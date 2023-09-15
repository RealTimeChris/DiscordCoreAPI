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
/// AutoModerationEntities.cpp - Source file for the ApplicationCommandData classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file AutoModerationEntities.cpp

#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace jsonifier {

	template<> struct core<DiscordCoreAPI::CreateAutoModerationRuleData> {
		using ValueType					 = DiscordCoreAPI::CreateAutoModerationRuleData;
		static constexpr auto parseValue = createObject("exemptChannels", &ValueType::exemptChannels, "exemptRoles", &ValueType::exemptRoles, "triggerMetadata",
			&ValueType::triggerMetadata, "actions", &ValueType::actions, "triggerType", &ValueType::triggerType, "eventType", &ValueType::eventType, "guildId", &ValueType::guildId,
			"name", &ValueType::name, "enabled", &ValueType::enabled);
	};

	template<> struct core<DiscordCoreAPI::ModifyAutoModerationRuleData> {
		using ValueType					 = DiscordCoreAPI::ModifyAutoModerationRuleData;
		static constexpr auto parseValue = createObject("exemptChannels", &ValueType::exemptChannels, "exemptRoles", &ValueType::exemptRoles, "triggerMetadata",
			&ValueType::triggerMetadata, "actions", &ValueType::actions, "autoModerationRuleId", &ValueType::autoModerationRuleId, "eventType", &ValueType::eventType, "guildId",
			&ValueType::guildId, "name", &ValueType::name, "enabled", &ValueType::enabled);
	};

}

namespace DiscordCoreAPI {

	void AutoModerationRules::initialize(DiscordCoreInternal::HttpsClient* HttpsClientNew) {
		AutoModerationRules::httpsClient = HttpsClientNew;
	}

	CoRoutine<jsonifier::vector<AutoModerationRuleData>> AutoModerationRules::listAutoModerationRulesForGuildAsync(ListAutoModerationRulesForGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rules };
		co_await NewThreadAwaitable<jsonifier::vector<AutoModerationRuleData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules";
		workload.callStack	   = "AutoModerationRules::listAutoModerationRulesForGuildAsync()";
		jsonifier::vector<AutoModerationRuleData> returnVector{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnVector);
		co_return std::move(returnVector);
	}

	CoRoutine<AutoModerationRuleData> AutoModerationRules::getAutoModerationRuleAsync(GetAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRuleData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + jsonifier::toString(dataPackage.autoModerationRuleId);
		workload.callStack	   = "AutoModerationRules::getAutoModerationRuleAsync()";
		AutoModerationRuleData returnData{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<AutoModerationRuleData> AutoModerationRules::createAutoModerationRuleAsync(CreateAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRuleData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "AutoModerationRules::createAutoModerationRuleAsync()";
		AutoModerationRuleData returnData{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<AutoModerationRuleData> AutoModerationRules::modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRuleData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + jsonifier::toString(dataPackage.autoModerationRuleId);
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "AutoModerationRules::modifyAutoModerationRuleAsync()";
		AutoModerationRuleData returnData{};
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> AutoModerationRules::deleteAutoModerationRuleAsync(DeleteAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + jsonifier::toString(dataPackage.autoModerationRuleId);
		workload.callStack	   = "AutoModerationRules::deleteAutoModerationRuleAsync()";
		AutoModerationRules::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	DiscordCoreInternal::HttpsClient* AutoModerationRules::httpsClient{};

}
