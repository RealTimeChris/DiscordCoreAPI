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
/// StageInstanceEntities.cpp - Source file for the stage instance related stuff.
/// Nov 29, 2021
/// https://discordcoreapi.com
/// \file StageInstanceEntities.cpp

#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::create_stage_instance_data> {
		using value_type = discord_core_api::create_stage_instance_data;
		static constexpr auto parseValue =
			createValue("privacyLevel", &value_type::privacyLevel, "channelId", &value_type::channelId, "topic", &value_type::topic);
	};

	template<> struct core<discord_core_api::modify_stage_instance_data> {
		using value_type = discord_core_api::modify_stage_instance_data;
		static constexpr auto parseValue =
			createValue("privacyLevel", &value_type::privacyLevel, "channelId", &value_type::channelId, "topic", &value_type::topic);
	};

}

namespace discord_core_api {

	void stage_instances::initialize(discord_core_internal::https_client* client) {
		stage_instances::httpsClient = client;
	}

	co_routine<stage_instance_data> stage_instances::createStageInstanceAsync(create_stage_instance_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Stage_Instance };
		co_await newThreadAwaitable<stage_instance_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/stage-instances";
		workload.callStack	   = "stage_instances::createStageInstanceAsync()";
		parser.serializeJson(dataPackage, workload.content);
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		stage_instance_data returnData{};
		stage_instances::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<stage_instance_data> stage_instances::getStageInstanceAsync(get_stage_instance_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Stage_Instance };
		co_await newThreadAwaitable<stage_instance_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/stage-instances/" + dataPackage.channelId;
		workload.callStack	   = "stage_instances::getStageInstanceAsync()";
		stage_instance_data returnData{};
		stage_instances::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<stage_instance_data> stage_instances::modifyStageInstanceAsync(modify_stage_instance_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Stage_Instance };
		co_await newThreadAwaitable<stage_instance_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/stage-instances/" + dataPackage.channelId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "stage_instances::modifyStageInstanceAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		stage_instance_data returnData{};
		stage_instances::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> stage_instances::deleteStageInstanceAsync(delete_stage_instance_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Stage_Instance };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/stage-instances/" + dataPackage.channelId;
		workload.callStack	   = "stage_instances::deleteStageInstanceAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		stage_instances::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}
	discord_core_internal::https_client* stage_instances::httpsClient{};
}
