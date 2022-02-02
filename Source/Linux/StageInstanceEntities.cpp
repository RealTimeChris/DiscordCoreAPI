// StageInstanceEntities.cpp - Source file for the Stage Instance related stuff.
// Nov 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "StageInstanceEntities.hpp"
#include "CoRoutine.hpp"
#include "Http.hpp"

namespace DiscordCoreAPI {

	StageInstance::StageInstance() {}

	StageInstance::StageInstance(StageInstanceData dataNew) {
		this->discoverableDisabled = dataNew.discoverableDisabled;
		this->privacyLevel = dataNew.privacyLevel;
		this->channelId = dataNew.channelId;
		this->guildId = dataNew.guildId;
		this->topic = dataNew.topic;
		this->id = dataNew.id;
	}

	void StageInstances::initialize(DiscordCoreInternal::HttpClient*theClient) {
		StageInstances::httpClient = theClient;
	}

	CoRoutine<StageInstance> StageInstances::createStageInstanceAsync(CreateStageInstanceData dataPackage) {
		try {
			co_await NewThreadAwaitable<StageInstance>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Stage_Instance;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/stage-instances";
			workload.callStack = "StageInstances::createStageInstanceAsync";
			nlohmann::json responseData = { {"topic",dataPackage.topic},{"privacy_level", dataPackage.privacyLevel},{"channel_id", dataPackage.channelId} };
			workload.content = responseData.dump();
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<StageInstance>(*StageInstances::httpClient, workload);
		}
		catch (...) {
			reportException("StageInstances::createStageInstanceAsync()");
		}
	}

	CoRoutine<StageInstance> StageInstances::getStageInstanceAsync(GetStageInstanceData dataPackage) {
		try {
			co_await NewThreadAwaitable<StageInstance>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Stage_Instance;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/stage-instances/" + dataPackage.channelId;
			workload.callStack = "StageInstances::getStageInstanceAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<StageInstance>(*StageInstances::httpClient, workload);
		}
		catch (...) {
			reportException("StageInstances::getStageInstanceAsync()");
		}
	}

	CoRoutine<StageInstance> StageInstances::modifyStageInstanceAsync(ModifyStageInstanceData dataPackage) {
		try {
			co_await NewThreadAwaitable<StageInstance>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Stage_Instance;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/stage-instances/" + dataPackage.channelId;
			nlohmann::json responseData = { {"privacy_level",dataPackage.privacyLevel},{"topic",dataPackage.topic} };
			workload.content = responseData.dump();
			workload.callStack = "StageInstances::modifyStageInstanceAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<StageInstance>(*StageInstances::httpClient, workload);
		}
		catch (...) {
			reportException("StageInstances::modifyStageInstanceAsync()");
		}
	}

	CoRoutine<void> StageInstances::deleteStageInstanceAsync(DeleteStageInstanceData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Stage_Instance;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/stage-instances/" + dataPackage.channelId;
			workload.callStack = "StageInstances::deleteStageInstanceAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*StageInstances::httpClient, workload);
		}
		catch (...) {
			reportException("StageInstances::deleteStageInstanceAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* StageInstances::httpClient{ nullptr };
}