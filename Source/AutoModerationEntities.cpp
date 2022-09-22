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

	CreateAutoModerationRuleData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData.addNewArray("actions");
		for (auto& value: this->actions) {
			JsonSerializer dataNew{};
			dataNew.addNewStructure("metadata");
			dataNew.appendStructElement("channel_id", std::to_string(value.metadata.channelId));
			dataNew.appendStructElement("duration_seconds", value.metadata.durationSeconds);
			dataNew.endStructure();
			dataNew.appendStructElement("type", value.type);
			theData.appendArrayElement(dataNew);
		}
		theData.endArray();
		theData.appendStructElement("enabled", this->enabled);
		theData.appendStructElement("event_type", this->eventType);
		theData.addNewArray("exempt_channels");
		for (auto& value: this->exemptChannels) {
			theData.appendArrayElement(value);
		}
		theData.endArray();
		theData.addNewArray("exempt_roles");
		for (auto& value: this->exemptRoles) {
			theData.appendArrayElement(value);
		}
		theData.endArray();
		theData.appendStructElement("name", this->name);
		theData.addNewStructure("trigger_metadata");
		theData.appendStructElement("keyword_filter", this->triggerMetadata.keywordFilter);
		theData.appendStructElement("presets", this->triggerMetadata.presets);
		theData.endStructure();
		theData.appendStructElement("trigger_type", this->triggerType);
		return theData;
	}

	ModifyAutoModerationRuleData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData.addNewArray("actions");
		for (auto& value: this->actions) {
			JsonSerializer dataNew{};
			dataNew.addNewStructure("metadata");
			dataNew.appendStructElement("channel_id", std::to_string(value.metadata.channelId));
			dataNew.appendStructElement("duration_seconds", value.metadata.durationSeconds);
			dataNew.endStructure();
			dataNew.appendStructElement("type", value.type);
			theData.appendArrayElement(dataNew);
		}
		theData.endArray();
		theData.appendStructElement("enabled", this->enabled);
		theData.appendStructElement("event_type", this->eventType);
		theData.addNewArray("exempt_channels");
		for (auto& value: this->exemptChannels) {
			theData.appendArrayElement(value);
		}
		theData.endArray();
		theData.addNewArray("exempt_roles");
		for (auto& value: this->exemptRoles) {
			theData.appendArrayElement(value);
		}
		theData.endArray();
		theData.appendStructElement("name", this->name);
		theData.addNewStructure("trigger_metadata");
		theData.appendStructElement("keyword_filter", this->triggerMetadata.keywordFilter);
		theData.appendStructElement("presets", this->triggerMetadata.presets);
		theData.endStructure();
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
		workload.content = static_cast<JsonSerializer>(dataPackage).getString();
		workload.callStack = "AutoModerationRules::createAutoModerationRuleAsync()";
		co_return AutoModerationRules::httpsClient->submitWorkloadAndGetResult<AutoModerationRule>(workload);
	}

	CoRoutine<AutoModerationRule> AutoModerationRules::modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Auto_Moderation_Rule };
		co_await NewThreadAwaitable<AutoModerationRule>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/auto-moderation/rules/" + std::to_string(dataPackage.autoModerationRuleId);
		workload.content = static_cast<JsonSerializer>(dataPackage).getString();
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
