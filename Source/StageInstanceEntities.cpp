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
/// StageInstanceEntities.cpp - Source file for the Stage Instance related stuff.
/// Nov 29, 2021
/// https://discordcoreapi.com
/// \file StageInstanceEntities.cpp

#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	StageInstance::StageInstance(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->topic = getString(jsonObjectData, "topic");

		this->privacyLevel = static_cast<StageInstancePrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		this->discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	void StageInstances::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		StageInstances::httpsClient = theClient;
	}

	CoRoutine<StageInstance> StageInstances::createStageInstanceAsync(CreateStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Stage_Instance };
		co_await NewThreadAwaitable<StageInstance>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/stage-instances";
		workload.callStack = "StageInstances::createStageInstanceAsync()";
		JsonObject responseData{};
		responseData["privacy_level"] = dataPackage.privacyLevel;
		responseData["topic"] = dataPackage.topic;
		responseData["channel_id"] = std::to_string(dataPackage.channelId);
		workload.content = responseData;
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return StageInstances::httpsClient->submitWorkloadAndGetResult<StageInstance>(workload);
	}

	CoRoutine<StageInstance> StageInstances::getStageInstanceAsync(GetStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Stage_Instance };
		co_await NewThreadAwaitable<StageInstance>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/stage-instances/" + std::to_string(dataPackage.channelId);
		workload.callStack = "StageInstances::getStageInstanceAsync()";
		co_return StageInstances::httpsClient->submitWorkloadAndGetResult<StageInstance>(workload);
	}

	CoRoutine<StageInstance> StageInstances::modifyStageInstanceAsync(ModifyStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Stage_Instance };
		co_await NewThreadAwaitable<StageInstance>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/stage-instances/" + std::to_string(dataPackage.channelId);
		JsonObject responseData{};
		responseData["privacy_level"] = dataPackage.privacyLevel;
		responseData["topic"] = dataPackage.topic;
		workload.content = responseData;
		workload.callStack = "StageInstances::modifyStageInstanceAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return StageInstances::httpsClient->submitWorkloadAndGetResult<StageInstance>(workload);
	}

	CoRoutine<void> StageInstances::deleteStageInstanceAsync(DeleteStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Stage_Instance };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/stage-instances/" + std::to_string(dataPackage.channelId);
		workload.callStack = "StageInstances::deleteStageInstanceAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return StageInstances::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}
	DiscordCoreInternal::HttpsClient* StageInstances::httpsClient{ nullptr };
}