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
/// AutoModerationEntities.cpp - Source file for the ApplicationCommand classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file AutoModerationEntities.cpp

#include <discordcoreapi/AutoModerationEntities.hpp>

namespace DiscordCoreAPI {

	CreateAutoModerationRuleData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->actions) {
			JsonObject dataNew{};
			dataNew["metadata"]["channel_id"] = std::to_string(value.metadata.channelId);
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			theData.pushBack("actions", dataNew);
		}
		theData["enabled"] = this->enabled;
		theData["event_type"] = this->eventType;
		for (auto& value: this->exemptChannels) {
			theData.pushBack("exempt_channels", value);
		}
		for (auto& value: this->exemptRoles) {
			theData.pushBack("exempt_roles", value);
		}
		theData["name"] = this->name;
		for (auto& value: this->triggerMetadata.keywordFilter) {
			theData["trigger_metadata"].pushBack("keyword_filter", value);
		}
		theData["presets"] = this->triggerMetadata.presets;
		theData["trigger_type"] = this->triggerType;
		return theData;
	}

	ModifyAutoModerationRuleData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->actions) {
			JsonObject dataNew{};
			dataNew["metadata"]["channel_id"] = std::to_string(value.metadata.channelId);
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			theData.pushBack("actions", dataNew);
		}
		theData["enabled"] = this->enabled;
		theData["event_type"] = this->eventType;
		for (auto& value: this->exemptChannels) {
			theData.pushBack("exempt_channels", value);
		}
		for (auto& value: this->exemptRoles) {
			theData.pushBack("exempt_roles", value);
		}
		theData["name"] = this->name;
		for (auto& value: this->triggerMetadata.keywordFilter) {
			theData["trigger_metadata"].pushBack("keyword_filter", value);
		}
		theData["presets"] = this->triggerMetadata.presets;
		return theData;
	}

	AutoModerationRuleVector::operator std::vector<AutoModerationRule>() {
		return this->theAutoModerationRules;
	}

	void AutoModerationRules::initialize(DiscordCoreInternal::HttpsClient* HttpsClientNew) {
		AutoModerationRules::httpsClient = HttpsClientNew;
	}

	CoRoutine<std::vector<AutoModerationRule>> AutoModerationRules::listAutoModerationRulesForGuildAsync(ListAutoModerationRulesForGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rules };
		co_await NewThreadAwaitable<std::vector<AutoModerationRule>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/api/v10/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules";
		workload.callStack = "AutoModerationRules::listAutoModerationRulesForGuildAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRuleVector>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::getAutoModerationRuleAsync(GetAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/api/v10/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::getAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::createAutoModerationRuleAsync(CreateAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/api/v10/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules";
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "AutoModerationRules::createAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/api/v10/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "AutoModerationRules::modifyAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<void> AutoModerationRules::deleteAutoModerationRuleAsync(DeleteAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/api/v10/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::deleteAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	DiscordCoreInternal::HttpsClient* AutoModerationRules::httpsClient{ nullptr };
}
