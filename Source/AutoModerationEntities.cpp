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

	CreateAutoModerationRuleData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->actions) {
			Jsonifier dataNew{};
			dataNew["metadata"]["channel_id"] = value.metadata.channelId;
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			data["actions"].emplaceBack(dataNew);
		}
		data["enabled"] = this->enabled;
		data["event_type"] = this->eventType;
		for (auto& value: this->exemptChannels) {
			data["exempt_channels"].emplaceBack(value);
		}
		for (auto& value: this->exemptRoles) {
			data["exempt_roles"].emplaceBack(value);
		}
		data["name"] = this->name;
		for (auto& value: this->triggerMetadata.keywordFilter) {
			data["trigger_metadata"]["keyword_filter"].emplaceBack(value);
		}
		data["trigger_metadata"]["presets"] = this->triggerMetadata.presets;
		data["trigger_metadata"]["mention_total_limit"] = this->triggerMetadata.mentionTotalLimit;
		data["trigger_metadata"]["keyword_filter"] = this->triggerMetadata.keywordFilter;
		data["trigger_metadata"]["allow_list"] = this->triggerMetadata.allowList;
		data["trigger_type"] = this->triggerType;
		return data;
	}

	ModifyAutoModerationRuleData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->actions) {
			Jsonifier dataNew{};
			dataNew["metadata"]["channel_id"] = value.metadata.channelId;
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			data["actions"].emplaceBack(dataNew);
		}
		data["enabled"] = this->enabled;
		data["event_type"] = this->eventType;
		for (auto& value: this->exemptChannels) {
			data["exempt_channels"].emplaceBack(value);
		}
		for (auto& value: this->exemptRoles) {
			data["exempt_roles"].emplaceBack(value);
		}
		data["name"] = this->name;
		for (auto& value: this->triggerMetadata.keywordFilter) {
			data["trigger_metadata"]["keyword_filter"].emplaceBack(value);
		}
		data["trigger_metadata"]["presets"] = this->triggerMetadata.presets;
		data["trigger_metadata"]["mention_total_limit"] = this->triggerMetadata.mentionTotalLimit;
		data["trigger_metadata"]["keyword_filter"] = this->triggerMetadata.keywordFilter;
		data["trigger_metadata"]["allow_list"] = this->triggerMetadata.allowList;
		return data;
	}

	AutoModerationActionExecutionEventData::AutoModerationActionExecutionEventData(simdjson::ondemand::value jsonObjectData) {
		this->alertSystemMessageId = getId(jsonObjectData, "alert_system_message_id");

		this->ruleTriggerType = static_cast<TriggerType>(getUint8(jsonObjectData, "rule_trigger_type"));

		this->matchedKeyword = getString(jsonObjectData, "matched_keyword");

		this->matchedContent = getString(jsonObjectData, "matched_content");

		simdjson::ondemand::value object{};
		if (getObject(object, "action", jsonObjectData)) {
			this->action = ActionData{ object };
		}

		this->content = getString(jsonObjectData, "content");

		this->messageId = getId(jsonObjectData, "message_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->ruleId = getId(jsonObjectData, "rule_id");

		this->userId = getId(jsonObjectData, "user_id");
	}

	AutoModerationRule::AutoModerationRule(simdjson::ondemand::value jsonObjectData) {
		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		this->enabled = getBool(jsonObjectData, "enabled");

		this->triggerType = static_cast<TriggerType>(getUint64(jsonObjectData, "trigger_type"));

		this->eventType = static_cast<EventType>(getUint64(jsonObjectData, "event_type"));

		this->creatorId = getId(jsonObjectData, "creator_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "actions", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ActionData newData{ value.value() };
				this->actions.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "exempt_roles", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->exemptRoles.emplace_back(Snowflake{ value.get_uint64().value() });
			}
		}

		if (getArray(arrayValue, "exempt_channels", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->exemptChannels.emplace_back(Snowflake{ value.get_uint64().value() });
			}
		}

		simdjson::ondemand::value object{};
		if (getObject(object, "trigger_metadata", jsonObjectData)) {
			TriggerMetaData dataNew{ object };
			this->triggerMetaData = dataNew;
		}

		this->guildId = getId(jsonObjectData, "guild_id");
	}

	AutoModerationRuleVector::AutoModerationRuleVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					AutoModerationRule newData{ value.value() };
					this->theAutoModerationRules.emplace_back(std::move(newData));
				}
			}
		}
	}

	AutoModerationRuleVector::operator std::vector<AutoModerationRule>() {
		return this->theAutoModerationRules;
	}

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
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRuleVector>(workload, &returnVector);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::getAutoModerationRuleAsync(GetAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::getAutoModerationRuleAsync()";
		AutoModerationRule returnValue{};
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload, &returnValue);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::createAutoModerationRuleAsync(CreateAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "AutoModerationRules::createAutoModerationRuleAsync()";
		AutoModerationRule returnValue{};
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload, &returnValue);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "AutoModerationRules::modifyAutoModerationRuleAsync()";
		AutoModerationRule returnValue{};
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload, &returnValue);
	}

	CoRoutine<void> AutoModerationRules::deleteAutoModerationRuleAsync(DeleteAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.callStack = "AutoModerationRules::deleteAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	DiscordCoreInternal::HttpsClient* AutoModerationRules::httpsClient{ nullptr };
}
