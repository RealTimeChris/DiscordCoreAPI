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
/// AutoModerationEntities.cpp - Source file for the ApplicationCommand classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file AutoModerationEntities.cpp

#include <discordcoreapi/AutoModerationEntities.hpp>

namespace DiscordCoreAPI {

	CreateAutoModerationRuleData::operator std::string() {
		nlohmann::json data{};
		data["actions"];
		for (auto& value: this->actions) {
			nlohmann::json dataNew{};
			dataNew["metadata"]["channel_id"] = value.metadata.channelId;
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			data["actions"].push_back(dataNew);
		}
		data["enabled"] = this->enabled;
		data["event_type"] = this->eventType;
		data["exempt_channels"];
		for (auto& value: this->exemptChannels) {
			data["exempt_channels"].push_back(value);
		}
		data["exempt_roles"];
		for (auto& value: this->exemptRoles) {
			data["exempt_roles"].push_back(value);
		}
		data["name"] = this->name;
		data["trigger_metadata"]["keyword_filter"] = this->triggerMetadata.keywordFilter;
		data["trigger_metadata"]["presets"] = this->triggerMetadata.presets;
		data["trigger_type"] = this->triggerType;
		return data.dump();
	}

	AutoModerationActionExecutionEventData& AutoModerationActionExecutionEventData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AutoModerationActionExecutionEventData::AutoModerationActionExecutionEventData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ModifyAutoModerationRuleData::operator std::string() {
		nlohmann::json data{};
		data["actions"];
		for (auto& value: this->actions) {
			nlohmann::json dataNew{};
			dataNew["metadata"]["channel_id"] = value.metadata.channelId;
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			data["actions"].push_back(dataNew);
		}
		data["enabled"] = this->enabled;
		data["event_type"] = this->eventType;
		data["exempt_channels"];
		for (auto& value: this->exemptChannels) {
			data["exempt_channels"].push_back(value);
		}
		data["exempt_roles"];
		for (auto& value: this->exemptRoles) {
			data["exempt_roles"].push_back(value);
		}
		data["name"] = this->name;
		data["trigger_metadata"]["keyword_filter"] = this->triggerMetadata.keywordFilter;
		data["trigger_metadata"]["presets"] = this->triggerMetadata.presets;
		return data.dump();
	}

	AutoModerationRule& AutoModerationRule::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AutoModerationRule::AutoModerationRule(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AutoModerationRuleVector::operator std::vector<AutoModerationRule>() {
		return this->theAutoModerationRules;
	}

	AutoModerationRuleVector& AutoModerationRuleVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AutoModerationRuleVector::AutoModerationRuleVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	void AutoModerationRules::initialize(DiscordCoreInternal::HttpsClient* HttpsClientNew) {
		AutoModerationRules::httpsClient = HttpsClientNew;
	}

	CoRoutine<std::vector<AutoModerationRule>> AutoModerationRules::listAutoModerationRulesForGuildAsync(ListAutoModerationRulesForGuildData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rules };
		co_await NewThreadAwaitable<std::vector<AutoModerationRule>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules";
		workload.callStack = "AutoModerationRules::listAutoModerationRulesForGuildAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRuleVector>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::getAutoModerationRuleAsync(GetAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::getAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::createAutoModerationRuleAsync(CreateAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules";
		workload.content = dataPackage;
		workload.callStack = "AutoModerationRules::createAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.content = dataPackage;
		workload.callStack = "AutoModerationRules::modifyAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<void> AutoModerationRules::deleteAutoModerationRuleAsync(DeleteAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::deleteAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	DiscordCoreInternal::HttpsClient* AutoModerationRules::httpsClient{ nullptr };
}
