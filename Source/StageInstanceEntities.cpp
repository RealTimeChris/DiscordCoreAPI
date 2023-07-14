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
/// StageInstanceEntities.cpp - Source file for the Stage Instance related stuff.
/// Nov 29, 2021
/// https://discordcoreapi.com
/// \file StageInstanceEntities.cpp

#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::CreateStageInstanceData> {
		using ValueType = DiscordCoreAPI::CreateStageInstanceData;
		static constexpr auto parseValue = object("privacyLevel", &ValueType::privacyLevel, "channelId", &ValueType::channelId, "reason",
			&ValueType::reason, "topic", &ValueType::topic);
	};

	template<> struct Core<DiscordCoreAPI::ModifyStageInstanceData> {
		using ValueType = DiscordCoreAPI::ModifyStageInstanceData;
		static constexpr auto parseValue = object("privacyLevel", &ValueType::privacyLevel, "channelId", &ValueType::channelId, "reason",
			&ValueType::reason, "topic", &ValueType::topic);
	};

}

namespace DiscordCoreAPI {

	void StageInstances::initialize(DiscordCoreInternal::HttpsClient* client) {
		StageInstances::httpsClient = client;
	}

	CoRoutine<StageInstanceData> StageInstances::createStageInstanceAsync(CreateStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Stage_Instance };
		co_await NewThreadAwaitable<StageInstanceData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/stage-instances";
		workload.callStack = "StageInstances::createStageInstanceAsync()";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		StageInstanceData returnData{};
		StageInstances::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<StageInstanceData> StageInstances::getStageInstanceAsync(GetStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Stage_Instance };
		co_await NewThreadAwaitable<StageInstanceData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/stage-instances/" + dataPackage.channelId;
		workload.callStack = "StageInstances::getStageInstanceAsync()";
		StageInstanceData returnData{};
		StageInstances::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<StageInstanceData> StageInstances::modifyStageInstanceAsync(ModifyStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Stage_Instance };
		co_await NewThreadAwaitable<StageInstanceData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/stage-instances/" + dataPackage.channelId;
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "StageInstances::modifyStageInstanceAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		StageInstanceData returnData{};
		StageInstances::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> StageInstances::deleteStageInstanceAsync(DeleteStageInstanceData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Stage_Instance };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/stage-instances/" + dataPackage.channelId;
		workload.callStack = "StageInstances::deleteStageInstanceAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		StageInstances::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}
	DiscordCoreInternal::HttpsClient* StageInstances::httpsClient{};
}
