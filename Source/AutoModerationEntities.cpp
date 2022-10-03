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

	AutoModerationRule::AutoModerationRule(simdjson::ondemand::value jsonObjectData) {
		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		this->enabled = getBool(jsonObjectData, "enabled");

		this->triggerType = static_cast<TriggerType>(getUint64(jsonObjectData, "trigger_type"));

		this->eventType = static_cast<EventType>(getUint64(jsonObjectData, "event_type"));

		this->creatorId = getId(jsonObjectData, "creator_id");

		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["actions"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ActionData newData{ value.value() };
				this->actions.emplace_back(std::move(newData));
			}
		}

		theResult = jsonObjectData["exempt_roles"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->exemptRoles.emplace_back(Snowflake{ value.get_uint64().value() });
			}
		}

		theResult = jsonObjectData["exempt_channels"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->exemptChannels.emplace_back(Snowflake{ value.get_uint64().value() });
			}
		}

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["trigger_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			TriggerMetaData theDataNew{ theObject };
			this->triggerMetaData = theDataNew;
		}

		this->guildId = getId(jsonObjectData, "guild_id");
	}

	AutoModerationRuleVector::AutoModerationRuleVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					AutoModerationRule newData{ value.value() };
					this->theAutoModerationRules.emplace_back(std::move(newData));
				}
			}
		}
	}

	AutoModerationActionExecutionEventData::AutoModerationActionExecutionEventData(simdjson::ondemand::value jsonObjectData) {
		this->alertSystemMessageId = getId(jsonObjectData, "alert_system_message_id");

		this->ruleTriggerType = static_cast<TriggerType>(getUint8(jsonObjectData, "rule_trigger_type"));

		this->matchedKeyword = getString(jsonObjectData, "matched_keyword");

		this->matchedContent = getString(jsonObjectData, "matched_content");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["action"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->action = ActionData{ theObject };
		}

		this->content = getString(jsonObjectData, "content");

		this->messageId = getId(jsonObjectData, "message_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->ruleId = getId(jsonObjectData, "rule_id");

		this->userId = getId(jsonObjectData, "user_id");
	}

	CreateAutoModerationRuleData::operator String() {
		JsonObject theData{};
		for (auto& value: this->actions) {
			JsonObject dataNew{};
			dataNew["metadata"]["channel_id"] = std::to_string(value.metadata.channelId);
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			theData["actions"].pushBack(dataNew);
		}
		theData["enabled"] = this->enabled;
		theData["event_type"] = this->eventType;
		for (auto& value: this->exemptChannels) {
			theData["exempt_channels"].pushBack(value);
		}
		for (auto& value: this->exemptRoles) {
			theData["exempt_roles"].pushBack(value);
		}
		theData["name"] = this->name;
		for (auto& value: this->triggerMetadata.keywordFilter) {
			theData["trigger_metadata"]["keyword_filter"].pushBack(value);
		}
		theData["presets"] = this->triggerMetadata.presets;
		theData["trigger_type"] = this->triggerType;
		return theData;
	}

	ModifyAutoModerationRuleData::operator String() {
		JsonObject theData{};
		for (auto& value: this->actions) {
			JsonObject dataNew{};
			dataNew["metadata"]["channel_id"] = std::to_string(value.metadata.channelId);
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			theData["actions"].pushBack(dataNew);
		}
		theData["enabled"] = this->enabled;
		theData["event_type"] = this->eventType;
		for (auto& value: this->exemptChannels) {
			theData["exempt_channels"].pushBack(value);
		}
		for (auto& value: this->exemptRoles) {
			theData["exempt_roles"].pushBack(value);
		}
		theData["name"] = this->name;
		for (auto& value: this->triggerMetadata.keywordFilter) {
			theData["trigger_metadata"]["keyword_filter"].pushBack(value);
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
		workload.content = dataPackage.operator String();
		workload.callStack = "AutoModerationRules::createAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.content = dataPackage.operator String();
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
