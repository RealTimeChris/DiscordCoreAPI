/*
*
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
/// StageInstanceEntities.cpp - Source file for the Stage Instance related stuff.
/// Nov 29, 2021
/// https://discordcoreapi.com
/// \file StageInstanceEntities.cpp

#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	void StageInstances::initialize(DiscordCoreInternal::HttpClient* theClient) {
		StageInstances::httpClient = theClient;
	}

	CoRoutine<StageInstance> StageInstances::createStageInstanceAsync(CreateStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Stage_Instance);
		co_await NewThreadAwaitable<StageInstance>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Stage_Instance;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/stage-instances";
		workload.callStack = "StageInstances::createStageInstanceAsync";
		nlohmann::json responseData = { { "topic", dataPackage.topic }, { "privacy_level", dataPackage.privacyLevel }, { "channel_id", dataPackage.channelId } };
		workload.content = responseData.dump();
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return StageInstances::httpClient->submitWorkloadAndGetResult<StageInstance>(workload);
	}

	CoRoutine<StageInstance> StageInstances::getStageInstanceAsync(GetStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Stage_Instance);
		co_await NewThreadAwaitable<StageInstance>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Stage_Instance;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/stage-instances/" + std::to_string(dataPackage.channelId);
		workload.callStack = "StageInstances::getStageInstanceAsync";
		co_return StageInstances::httpClient->submitWorkloadAndGetResult<StageInstance>(workload);
	}

	CoRoutine<StageInstance> StageInstances::modifyStageInstanceAsync(ModifyStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Stage_Instance);
		co_await NewThreadAwaitable<StageInstance>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Stage_Instance;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/stage-instances/" + std::to_string(dataPackage.channelId);
		nlohmann::json responseData = { { "privacy_level", dataPackage.privacyLevel }, { "topic", dataPackage.topic } };
		workload.content = responseData.dump();
		workload.callStack = "StageInstances::modifyStageInstanceAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return StageInstances::httpClient->submitWorkloadAndGetResult<StageInstance>(workload);
	}

	CoRoutine<void> StageInstances::deleteStageInstanceAsync(DeleteStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Stage_Instance);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Stage_Instance;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/stage-instances/" + std::to_string(dataPackage.channelId);
		workload.callStack = "StageInstances::deleteStageInstanceAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return StageInstances::httpClient->submitWorkloadAndGetResult<void>(workload);
	}
	DiscordCoreInternal::HttpClient* StageInstances::httpClient{ nullptr };
}