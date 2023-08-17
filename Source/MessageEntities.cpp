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
/// MessageEntities.cpp - Source file for the message related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file MessageEntities.cpp

#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::MessageResponseBase> {
		using ValueType					 = DiscordCoreAPI::MessageResponseBase;
		static constexpr auto parseValue = object("components", &ValueType::components, "allowed_mentions", &ValueType::allowedMentions, "embeds", &ValueType::embeds, "files",
			&ValueType::files, "custom_id", &ValueType::customId, "content", &ValueType::content, "title", &ValueType::title, "flags", &ValueType::flags, "tts", &ValueType::tts);
	};

	template<> struct Core<DiscordCoreAPI::CreateMessageData> {
		using ValueType					 = DiscordCoreAPI::CreateMessageData;
		static constexpr auto parseValue = object("components", &ValueType::components, "allowed_mentions", &ValueType::allowedMentions, "embeds", &ValueType::embeds, "files",
			&ValueType::files, "custom_id", &ValueType::customId, "content", &ValueType::content, "title", &ValueType::title, "flags", &ValueType::flags, "tts", &ValueType::tts);
	};

	template<> struct Core<DiscordCoreAPI::EditMessageData> {
		using ValueType					 = DiscordCoreAPI::EditMessageData;
		static constexpr auto parseValue = object("components", &ValueType::components, "allowed_mentions", &ValueType::allowedMentions, "embeds", &ValueType::embeds, "files",
			&ValueType::files, "custom_id", &ValueType::customId, "content", &ValueType::content, "title", &ValueType::title, "flags", &ValueType::flags, "tts", &ValueType::tts);
	};

	template<> struct Core<DiscordCoreAPI::DeleteMessagesBulkData> {
		using ValueType					 = DiscordCoreAPI::DeleteMessagesBulkData;
		static constexpr auto parseValue = object("messages", &ValueType::messageIds);
	};
}

namespace DiscordCoreAPI {

	template<> UnorderedMap<std::string, UnboundedMessageBlock<MessageData>*> ObjectCollector<MessageData>::objectsBuffersMap{};

	template<> ObjectCollector<MessageData>::ObjectCollector() {
		collectorId										= std::to_string(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
		ObjectCollector::objectsBuffersMap[collectorId] = &objectsBuffer;
	};

	template<> void ObjectCollector<MessageData>::run(
		std::coroutine_handle<typename DiscordCoreAPI::CoRoutine<DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::MessageData>::ObjectCollectorReturnData>::promise_type>&
			coroHandle) {
		int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
		int64_t elapsedTime{};
		while (elapsedTime < msToCollectFor && !coroHandle.promise().areWeStopped()) {
			MessageData message{};
			waitForTimeToPass<MessageData>(objectsBuffer, message, static_cast<uint64_t>(msToCollectFor - static_cast<uint64_t>(elapsedTime)));
			if (filteringFunction(message)) {
				objectReturnData.objects.emplace_back(message);
			}
			if (static_cast<int32_t>(objectReturnData.objects.size()) >= quantityOfObjectsToCollect) {
				break;
			}

			elapsedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count() - startingTime;
		}
	}

	template<> CoRoutine<ObjectCollector<MessageData>::ObjectCollectorReturnData> ObjectCollector<MessageData>::collectObjects(int32_t quantityToCollect, int32_t msToCollectForNew,
		ObjectFilter<MessageData> filteringFunctionNew) {
		auto coroHandle			   = co_await NewThreadAwaitable<ObjectCollectorReturnData>();
		quantityOfObjectsToCollect = quantityToCollect;
		filteringFunction		   = filteringFunctionNew;
		msToCollectFor			   = msToCollectForNew;

		run(coroHandle);
		co_return std::move(objectReturnData);
	}

	template<> ObjectCollector<MessageData>::~ObjectCollector() {
		if (ObjectCollector::objectsBuffersMap.contains(collectorId)) {
			ObjectCollector::objectsBuffersMap.erase(collectorId);
		}
	};

	CreateMessageData::CreateMessageData(const Snowflake channelIdNew) {
		channelId = channelIdNew;
	}

	CreateMessageData::CreateMessageData(RespondToInputEventData dataPackage) {
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

	CreateMessageData::CreateMessageData(MessageData dataPackage) {
		channelId				   = dataPackage.channelId;
		messageReference.channelId = dataPackage.channelId;
		messageReference.messageId = dataPackage.id;
		messageReference.guildId   = dataPackage.guildId;
		tts						   = dataPackage.tts;
	}

	CreateMessageData::CreateMessageData(InputEventData dataPackage) {
		channelId = dataPackage.getChannelData().id;
	}

	SendDMData::SendDMData(RespondToInputEventData dataPackage) {
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

	EditMessageData::EditMessageData(InputEventData dataPackage) {
		channelId = dataPackage.getChannelData().id;
		messageId = dataPackage.getMessageData().id;
	}

	EditMessageData::EditMessageData(RespondToInputEventData dataPackage) {
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

	DeleteMessageData::DeleteMessageData(const MessageData& messageToDelete) {
		channelId = messageToDelete.channelId;
		timeStamp = messageToDelete.timeStamp;
		messageId = messageToDelete.id;
	}

	void Messages::initialize(DiscordCoreInternal::HttpsClient* client) {
		Messages::httpsClient = client;
	}

	CoRoutine<Jsonifier::Vector<MessageData>> Messages::getMessagesAsync(GetMessagesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Messages };
		co_await NewThreadAwaitable<Jsonifier::Vector<MessageData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages";
		if (dataPackage.aroundThisId != 0) {
			workload.relativePath += "?around=" + std::to_string(dataPackage.aroundThisId);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else if (dataPackage.beforeThisId != 0) {
			workload.relativePath += "?before=" + dataPackage.beforeThisId;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else if (dataPackage.afterThisId != 0) {
			workload.relativePath += "?after=" + dataPackage.afterThisId;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else {
			if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		}
		workload.callStack = "Messages::getMessagesAsync()";
		Jsonifier::Vector<MessageData> returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> Messages::getMessageAsync(GetMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.id;
		workload.callStack	   = "Messages::getMessageAsync()";
		MessageData returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> Messages::createMessageAsync(CreateMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages";
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			parser.serializeJson(dataPackage, workload.content);
		} else {
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "Messages::createMessageAsync()";
		MessageData returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> Messages::crosspostMessageAsync(CrosspostMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Crosspost_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/crosspost";
		workload.callStack	   = "Messages::crosspostMessageAsync()";
		MessageData returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> Messages::editMessageAsync(EditMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			parser.serializeJson(dataPackage, workload.content);
		} else {
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "Messages::editMessageAsync()";
		MessageData returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Messages::deleteMessageAsync(DeleteMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		bool hasTimeElapsedNew = dataPackage.timeStamp.hasTimeElapsed(14, 0, 0);
		if (!hasTimeElapsedNew) {
			workload = DiscordCoreInternal::HttpsWorkloadType::Delete_Message;
		} else {
			workload = DiscordCoreInternal::HttpsWorkloadType::Delete_Message_Old;
		}
		co_await NewThreadAwaitable<void>();
		if (dataPackage.timeDelay > 0) {
			std::this_thread::sleep_for(Milliseconds{ dataPackage.timeDelay });
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
		workload.callStack	   = "Messages::deleteMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Messages::deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Delete_Messages };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
		parser.serializeJson(dataPackage, workload.content);
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<Jsonifier::Vector<MessageData>> Messages::getPinnedMessagesAsync(GetPinnedMessagesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Pinned_Messages };
		co_await NewThreadAwaitable<Jsonifier::Vector<MessageData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/pins";
		workload.callStack	   = "Messages::getPinnedMessagesAsync()";
		Jsonifier::Vector<MessageData> returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Messages::pinMessageAsync(PinMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Pin_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
		workload.callStack	   = "Messages::pinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Messages::unpinMessageAsync(UnpinMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Pin_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
		workload.callStack	   = "Messages::unpinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	DiscordCoreInternal::HttpsClient* Messages::httpsClient{};
}
