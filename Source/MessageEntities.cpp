/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// MessageEntities.cpp - Source file for the message related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file MessageEntities.cpp

#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/EventEntities.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::MessageResponseBase> {
		using OTy2 = DiscordCoreAPI::MessageResponseBase;
		static constexpr auto parseValue =
			object("components", &OTy2::components, "allowed_mentions", &OTy2::allowedMentions, "embeds", &OTy2::embeds, "files", &OTy2::files,
				"custom_id", &OTy2::customId, "content", &OTy2::content, "title", &OTy2::title, "flags", &OTy2::flags, "tts", &OTy2::tts);
	};

	template<> struct Core<DiscordCoreAPI::CreateMessageData> {
		using OTy2 = DiscordCoreAPI::CreateMessageData;
		static constexpr auto parseValue =
			object("components", &OTy2::components, "allowed_mentions", &OTy2::allowedMentions, "embeds", &OTy2::embeds, "files", &OTy2::files,
				"custom_id", &OTy2::customId, "content", &OTy2::content, "title", &OTy2::title, "flags", &OTy2::flags, "tts", &OTy2::tts);
	};

	template<> struct Core<DiscordCoreAPI::EditMessageData> {
		using OTy2 = DiscordCoreAPI::EditMessageData;
		static constexpr auto parseValue =
			object("components", &OTy2::components, "allowed_mentions", &OTy2::allowedMentions, "embeds", &OTy2::embeds, "files", &OTy2::files,
				"custom_id", &OTy2::customId, "content", &OTy2::content, "title", &OTy2::title, "flags", &OTy2::flags, "tts", &OTy2::tts);
	};

	template<> struct Core<DiscordCoreAPI::DeleteMessagesBulkData> {
		using OTy2 = DiscordCoreAPI::DeleteMessagesBulkData;
		static constexpr auto parseValue = object("messages", &OTy2::messageIds);
	};
}

namespace DiscordCoreAPI {

	CoRoutine<MessageCollectorReturnData> MessageCollector::collectMessages(int32_t quantityToCollect, int32_t msToCollectForNew,
		ObjectFilter<MessageData> filteringFunctionNew) {
		co_await NewThreadAwaitable<MessageCollectorReturnData>();
		this->quantityOfMessageToCollect = quantityToCollect;
		this->filteringFunction = filteringFunctionNew;
		this->msToCollectFor = msToCollectForNew;
		this->collectorId = std::to_string(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
		MessageCollector::objectsBuffersMap[this->collectorId] = &this->objectsBuffer;
		this->run();
		co_return std::move(this->objectReturnData);
	}

	void MessageCollector::run() {
		int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
		int64_t elapsedTime{};
		while (elapsedTime < this->msToCollectFor) {
			Message message{};
			waitForTimeToPass<MessageData>(this->objectsBuffer, message, static_cast<int32_t>(this->msToCollectFor - elapsedTime));
			if (this->filteringFunction(message)) {
				this->objectReturnData.messages.emplace_back(message);
			}
			if (static_cast<int32_t>(this->objectReturnData.messages.size()) >= this->quantityOfMessageToCollect) {
				break;
			}

			elapsedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count() - startingTime;
		}
	}

	MessageCollector::~MessageCollector() {
		if (MessageCollector::objectsBuffersMap.contains(this->collectorId)) {
			MessageCollector::objectsBuffersMap.erase(this->collectorId);
		}
	};

	CreateMessageData::CreateMessageData(const Snowflake channelIdNew) {
		this->channelId = channelIdNew;
	}

	CreateMessageData::CreateMessageData(RespondToInputEventData dataPackage) {
		this->channelId = dataPackage.channelId;
		this->addAllowedMentions(dataPackage.allowedMentions);
		for (auto& value: dataPackage.components) {
			this->components.emplace_back(value);
		}
		this->addContent(dataPackage.content);
		for (auto& value: dataPackage.embeds) {
			this->embeds.emplace_back(value);
		}
		this->tts = dataPackage.tts;
	}

	CreateMessageData::CreateMessageData(MessageData dataPackage) {
		this->channelId = dataPackage.channelId;
		this->messageReference.channelId = dataPackage.channelId;
		this->messageReference.messageId = dataPackage.id;
		this->messageReference.guildId = dataPackage.guildId;
		this->tts = dataPackage.tts;
	}

	CreateMessageData::CreateMessageData(InputEventData dataPackage) {
		this->channelId = dataPackage.getChannelData().id;
	}

	SendDMData::SendDMData(RespondToInputEventData dataPackage) {
		this->targetUserId = dataPackage.targetUserId;
		this->addAllowedMentions(dataPackage.allowedMentions);
		for (auto& value: dataPackage.components) {
			this->components.emplace_back(value);
		}
		this->addContent(dataPackage.content);
		for (auto& value: dataPackage.embeds) {
			this->embeds.emplace_back(value);
		}
		this->channelId = dataPackage.targetUserId;
		this->tts = dataPackage.tts;
	}

	EditMessageData::EditMessageData(InputEventData dataPackage) {
		this->channelId = dataPackage.getChannelData().id;
		this->messageId = dataPackage.getMessageData().id;
	}

	EditMessageData::EditMessageData(RespondToInputEventData dataPackage) {
		this->allowedMentions = dataPackage.allowedMentions;
		this->channelId = dataPackage.channelId;
		this->messageId = dataPackage.messageId;
		for (auto& value: dataPackage.components) {
			this->components.emplace_back(value);
		}
		this->content = dataPackage.content;
		for (auto& value: dataPackage.embeds) {
			this->embeds.emplace_back(value);
		}
	}

	void Messages::initialize(DiscordCoreInternal::HttpsClient* client) {
		Messages::httpsClient = client;
	}

	CoRoutine<std::vector<Message>> Messages::getMessagesAsync(GetMessagesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Messages };
		co_await NewThreadAwaitable<std::vector<Message>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
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
		MessageVector returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult<MessageVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> Messages::getMessageAsync(GetMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.id;
		workload.callStack = "Messages::getMessageAsync()";
		Message returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> Messages::createMessageAsync(CreateMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "Messages::createMessageAsync()";
		Message returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> Messages::crosspostMessageAsync(CrosspostMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Crosspost_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/crosspost";
		workload.callStack = "Messages::crosspostMessageAsync()";
		Message returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> Messages::editMessageAsync(EditMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "Messages::editMessageAsync()";
		Message returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
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
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
		workload.callStack = "Messages::deleteMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	CoRoutine<void> Messages::deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Delete_Messages };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	CoRoutine<std::vector<Message>> Messages::getPinnedMessagesAsync(GetPinnedMessagesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Pinned_Messages };
		co_await NewThreadAwaitable<std::vector<Message>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/pins";
		workload.callStack = "Messages::getPinnedMessagesAsync()";
		MessageVector returnData{};
		Messages::httpsClient->submitWorkloadAndGetResult<MessageVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> Messages::pinMessageAsync(PinMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Pin_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
		workload.callStack = "Messages::pinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	CoRoutine<void> Messages::unpinMessageAsync(UnpinMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Pin_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
		workload.callStack = "Messages::unpinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Messages::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	DiscordCoreInternal::HttpsClient* Messages::httpsClient{ nullptr };
}
