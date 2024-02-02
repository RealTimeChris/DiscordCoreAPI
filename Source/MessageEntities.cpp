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
/// MessageEntities.cpp - Source file for the message_data related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file MessageEntities.cpp

#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::message_response_base> {
		using value_type = discord_core_api::message_response_base;
		static constexpr auto parseValue =
			createValue("components", &value_type::components, "allowed_mentions", &value_type::allowedMentions, "embeds", &value_type::embeds, "files", &value_type::files,
				"custom_id", &value_type::customId, "content", &value_type::content, "title", &value_type::title, "flags", &value_type::flags, "tts", &value_type::tts);
	};

	template<> struct core<discord_core_api::create_message_data> {
		using value_type = discord_core_api::create_message_data;
		static constexpr auto parseValue =
			createValue("components", &value_type::components, "allowed_mentions", &value_type::allowedMentions, "embeds", &value_type::embeds, "files", &value_type::files,
				"custom_id", &value_type::customId, "content", &value_type::content, "title", &value_type::title, "flags", &value_type::flags, "tts", &value_type::tts);
	};

	template<> struct core<discord_core_api::edit_message_data> {
		using value_type = discord_core_api::edit_message_data;
		static constexpr auto parseValue =
			createValue("components", &value_type::components, "allowed_mentions", &value_type::allowedMentions, "embeds", &value_type::embeds, "files", &value_type::files,
				"custom_id", &value_type::customId, "content", &value_type::content, "title", &value_type::title, "flags", &value_type::flags, "tts", &value_type::tts);
	};

	template<> struct core<discord_core_api::delete_messages_bulk_data> {
		using value_type				 = discord_core_api::delete_messages_bulk_data;
		static constexpr auto parseValue = createValue("messages", &value_type::messageIds);
	};
}

namespace discord_core_api {

	template<> unordered_map<jsonifier::string, unbounded_message_block<message_data>*> object_collector<message_data>::objectsBuffersMap{};

	template<> object_collector<message_data>::object_collector() {
		collectorId										 = jsonifier::toString(std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count());
		object_collector::objectsBuffersMap[collectorId] = &objectsBuffer;
	};

	template<> void object_collector<message_data>::run(std::coroutine_handle<
		typename discord_core_api::co_routine<discord_core_api::object_collector<discord_core_api::message_data>::object_collector_return_data, false>::promise_type>& coroHandle) {
		int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count());
		int64_t elapsedTime{};
		while (elapsedTime < msToCollectFor && !coroHandle.promise().stopRequested()) {
			message_data message{};
			waitForTimeToPass<message_data>(objectsBuffer, message, static_cast<uint64_t>(msToCollectFor - static_cast<uint64_t>(elapsedTime)));
			if (filteringFunction(message)) {
				objectReturnData.objects.emplace_back(message);
			}
			if (static_cast<int32_t>(objectReturnData.objects.size()) >= quantityOfObjectsToCollect) {
				break;
			}

			elapsedTime = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count() - startingTime;
		}
	}

	template<> co_routine<object_collector<message_data>::object_collector_return_data, false> object_collector<message_data>::collectObjects(int32_t quantityToCollect,
		int32_t msToCollectForNew, object_filter<message_data> filteringFunctionNew) {
		auto coroHandle			   = co_await newThreadAwaitable<object_collector_return_data, false>();
		quantityOfObjectsToCollect = quantityToCollect;
		filteringFunction		   = filteringFunctionNew;
		msToCollectFor			   = msToCollectForNew;

		run(coroHandle);
		co_return objectReturnData;
	}

	template<> object_collector<message_data>::~object_collector() {
		if (object_collector::objectsBuffersMap.contains(collectorId)) {
			object_collector::objectsBuffersMap.erase(collectorId);
		}
	};

	create_message_data::create_message_data(const snowflake channelIdNew) {
		channelId = channelIdNew;
	}

	create_message_data::create_message_data(respond_to_input_event_data dataPackage) {
		channelId = dataPackage.channelId;
		addAllowedMentions(dataPackage.allowedMentions);
		for (auto& value: dataPackage.components) {
			components.emplace_back(value);
		}
		addContent(dataPackage.content);
		for (auto& value: dataPackage.embeds) {
			embeds.emplace_back(value);
		}
		tts = dataPackage.tts;
	}

	create_message_data::create_message_data(message_data dataPackage) {
		channelId				   = dataPackage.channelId;
		messageReference.channelId = dataPackage.channelId;
		messageReference.messageId = dataPackage.id;
		messageReference.guildId   = dataPackage.guildId;
		tts						   = dataPackage.tts;
	}

	create_message_data::create_message_data(input_event_data dataPackage) {
		channelId = dataPackage.getChannelData().id;
	}

	send_dmdata::send_dmdata(respond_to_input_event_data dataPackage) {
		targetUserId = dataPackage.targetUserId;
		addAllowedMentions(dataPackage.allowedMentions);
		for (auto& value: dataPackage.components) {
			components.emplace_back(value);
		}
		addContent(dataPackage.content);
		for (auto& value: dataPackage.embeds) {
			embeds.emplace_back(value);
		}
		channelId = dataPackage.targetUserId;
		tts		  = dataPackage.tts;
	}

	edit_message_data::edit_message_data(input_event_data dataPackage) {
		channelId = dataPackage.getChannelData().id;
		messageId = dataPackage.getMessageData().id;
	}

	edit_message_data::edit_message_data(respond_to_input_event_data dataPackage) {
		allowedMentions = dataPackage.allowedMentions;
		channelId		= dataPackage.channelId;
		messageId		= dataPackage.messageId;
		for (auto& value: dataPackage.components) {
			components.emplace_back(value);
		}
		content = dataPackage.content;
		for (auto& value: dataPackage.embeds) {
			embeds.emplace_back(value);
		}
	}

	delete_message_data::delete_message_data(const message_data& messageToDelete) {
		channelId = messageToDelete.channelId;
		timeStamp = messageToDelete.timeStamp;
		messageId = messageToDelete.id;
	}

	void messages::initialize(discord_core_internal::https_client* client) {
		messages::httpsClient = client;
	}

	co_routine<jsonifier::vector<message_data>> messages::getMessagesAsync(get_messages_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Messages };
		co_await newThreadAwaitable<jsonifier::vector<message_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages";
		if (dataPackage.aroundThisId != 0) {
			workload.relativePath += "?around=" + dataPackage.aroundThisId;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else if (dataPackage.beforeThisId != 0) {
			workload.relativePath += "?before=" + dataPackage.beforeThisId;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else if (dataPackage.afterThisId != 0) {
			workload.relativePath += "?after=" + dataPackage.afterThisId;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else {
			if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		}
		workload.callStack = "messages::getMessagesAsync()";
		jsonifier::vector<message_data> returnData{};
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		std::cout << "CURRENT RESULTS: " << returnData.at(0).timeStamp << std::endl;
		co_return returnData;
	}

	co_routine<message_data> messages::getMessageAsync(get_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Message };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.id;
		workload.callStack	   = "messages::getMessageAsync()";
		message_data returnData{};
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> messages::createMessageAsync(create_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Message };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages";
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
			parser.serializeJson(dataPackage, workload.content);
		} else {
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "messages::createMessageAsync()";
		message_data returnData{};
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> messages::crosspostMessageAsync(crosspost_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Crosspost_Message };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/crosspost";
		workload.callStack	   = "messages::crosspostMessageAsync()";
		message_data returnData{};
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> messages::editMessageAsync(edit_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Message };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
			parser.serializeJson(dataPackage, workload.content);
		} else {
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "messages::editMessageAsync()";
		message_data returnData{};
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> messages::deleteMessageAsync(delete_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{};
		bool hasTimeElapsedNew = dataPackage.timeStamp.hasTimeElapsed(14, 0, 0);
		if (!hasTimeElapsedNew) {
			workload = discord_core_internal::https_workload_type::Delete_Message;
		} else {
			workload = discord_core_internal::https_workload_type::Delete_Message_Old;
		}
		co_await newThreadAwaitable<void>();
		if (dataPackage.timeDelay > 0) {
			std::this_thread::sleep_for(milliseconds{ dataPackage.timeDelay });
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
		workload.callStack	   = "messages::deleteMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> messages::deleteMessagesBulkAsync(delete_messages_bulk_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Bulk_Delete_Messages };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
		parser.serializeJson(dataPackage, workload.content);
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<message_data>> messages::getPinnedMessagesAsync(get_pinned_messages_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Pinned_Messages };
		co_await newThreadAwaitable<jsonifier::vector<message_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/pins";
		workload.callStack	   = "messages::getPinnedMessagesAsync()";
		jsonifier::vector<message_data> returnData{};
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> messages::pinMessageAsync(pin_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Pin_Message };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
		workload.callStack	   = "messages::pinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> messages::unpinMessageAsync(unpin_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Pin_Message };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
		workload.callStack	   = "messages::unpinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	discord_core_internal::https_client* messages::httpsClient{};
}
