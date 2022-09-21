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
/// MessageEntities.cpp - Source file for the message related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file MessageEntities.cpp

#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/EventEntities.hpp>

namespace DiscordCoreAPI {

	/// ObjectCollectorReturnData responseData. \brief ObjectCollectorReturnData responseData.
	template<> struct DiscordCoreAPI_Dll ObjectCollectorReturnData<Message> {
		std::vector<Message> messages{};///< A vector of collected Objects.
	};

	/// ObjectCollector, for collecting Objects from a Channel. \brief Object collector, for collecting Objects from a Channel.
	template<> class DiscordCoreAPI_Dll ObjectCollector<Message> {
	  public:
		static std::unordered_map<std::string, UnboundedMessageBlock<Message>*> objectsBufferMap;

		ObjectCollector() noexcept = default;

		/// Begin waiting for Objects. \brief Begin waiting for Objects.
		/// \param quantityToCollect Maximum quantity of Objects to collect before returning the results.
		/// \param msToCollectForNew Maximum number of std::chrono::milliseconds to wait for Objects before returning the results.
		/// \param messageFilter A filter function to apply to new Objects, where returning "true" from the function results in a Object being stored.
		/// \returns A ObjectCollectorReturnData structure.
		CoRoutine<ObjectCollectorReturnData<Message>> collectObjects(int32_t quantityToCollect, int32_t msToCollectForNew, ObjectFilter<Message> filteringFunctionNew) {
			co_await NewThreadAwaitable<ObjectCollectorReturnData<Message>>();
			this->quantityOfObjectToCollect = quantityToCollect;
			this->filteringFunction = filteringFunctionNew;
			this->msToCollectFor = msToCollectForNew;
			this->collectorId = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			std::cout << "WAITING FOR TIME TO PASS!" << this->collectorId << std::endl;
			ObjectCollector::objectsBufferMap[this->collectorId] = &this->messagesBuffer;
			this->run();
			co_return this->messageReturnData;
		}

		void run() {
			int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			int64_t elapsedTime{ 0 };
			while (elapsedTime < this->msToCollectFor) {
				std::cout << "WAITING FOR TIME TO PASS!" << std::endl;
				Message message{};
				waitForTimeToPass<Message>(this->messagesBuffer, message, static_cast<int32_t>(this->msToCollectFor - elapsedTime));
				if (this->filteringFunction(message)) {
					this->messageReturnData.messages.emplace_back(message);
				}
				if (static_cast<int32_t>(this->messageReturnData.messages.size()) >= this->quantityOfObjectToCollect) {
					break;
				}

				elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingTime;
			}
		}

		~ObjectCollector() {
			if (ObjectCollector::objectsBufferMap.contains(this->collectorId)) {
				ObjectCollector::objectsBufferMap.erase(this->collectorId);
			}
		}

	  protected:		
		ObjectCollectorReturnData<Message> messageReturnData{};
		ObjectFilter<Message> filteringFunction{ nullptr };
		UnboundedMessageBlock<Message> messagesBuffer{};
		int32_t quantityOfObjectToCollect{ 0 };
		int32_t msToCollectFor{ 0 };
		std::string collectorId{};
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

	CreateMessageData::CreateMessageData(InputEventData dataPackage) {
		this->channelId = dataPackage.getChannelId();
	}

	CreateMessageData::operator JsonSerializer() {
		JsonSerializer theData{};
		/*
		for (auto& value: this->attachments) {
			theData.pushBack("attachments", value);
		}
		theData[""] = JsonParseEvent::Array_End;
		if (this->messageReference.messageId != 0) {
			theData["message_reference"] = this->messageReference;
		}
		if (this->components.size() == 0) {
			theData["components"] = JsonParseEvent::Null_Value;
		} else {
			for (auto& value: this->components) {
				theData.pushBack("components", value);
			}
			theData[""] = JsonParseEvent::Array_End;
		}
		theData["allowed_mentions"] = this->allowedMentions;
		for (auto& value: this->stickerIds) {
			theData.pushBack("sticker_ids", value);
		}

		theData[""] = JsonParseEvent::Array_End;
		if (this->embeds.size() == 0) {
			theData["embeds"] = JsonParseEvent::Null_Value;
		} else {
			for (auto& value: this->embeds) {
				theData.pushBack("embeds", value);
			}
			theData[""] = JsonParseEvent::Array_End;
		}
		if (this->content != "") {
			theData["content"] = this->content;
		}
		theData["flags"] = this->flags;
		theData["tts"] = this->tts;
		*/
		return theData;
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
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
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

	EditMessageData::operator JsonSerializer() {
		JsonSerializer theData{};
		/*
		for (auto& value: this->attachments) {
			theData.pushBack("attachments", value);
		}
		theData[""] = JsonParseEvent::Array_End;
		if (this->components.size() == 0) {
			theData["components"] = JsonParseEvent::Null_Value;
		} else {
			for (auto& value: this->components) {
				theData.pushBack("components", value);
			}
			theData[""] = JsonParseEvent::Array_End;
		}
		theData["allowed_mentions"] = this->allowedMentions;
		if (this->embeds.size() == 0) {
			theData["embeds"] = JsonParseEvent::Null_Value;
		} else {
			for (auto& value: this->embeds) {
				theData.pushBack("embeds", value);
			}
			theData[""] = JsonParseEvent::Array_End;
		}
		if (this->content != "") {
			theData["content"] = this->content;
		}
		theData["flags"] = this->flags;
		*/
		return theData;
	}

	DeleteMessagesBulkData::operator JsonSerializer() {
		JsonSerializer theData{};
		/*
		theData["messages"] = this->messageIds;
		*/
		return theData;
	}

	MessageVector::operator std::vector<Message>() {
		return this->theMessages;
	}

	void Messages::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		Messages::httpsClient = theClient;
	}

	CoRoutine<std::vector<Message>> Messages::getMessagesAsync(GetMessagesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Messages };
		co_await NewThreadAwaitable<std::vector<Message>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages";
		if (dataPackage.aroundThisId != 0) {
			workload.relativePath += "?around=" + std::to_string(dataPackage.aroundThisId);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else if (dataPackage.beforeThisId != 0) {
			workload.relativePath += "?before=" + std::to_string(dataPackage.beforeThisId);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			} else {
				workload.relativePath += "&limit=1";
			}
		} else if (dataPackage.afterThisId != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.afterThisId);
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
		co_return Messages::httpsClient->submitWorkloadAndGetResult<MessageVector>(workload);
	}

	CoRoutine<Message> Messages::getMessageAsync(GetMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.id);
		workload.callStack = "Messages::getMessageAsync()";
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Messages::createMessageAsync(CreateMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages";
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(static_cast<JsonSerializer>(dataPackage).getString(), dataPackage.files);
		} else {
			workload.content = static_cast<JsonSerializer>(dataPackage).getString();
		}
		workload.callStack = "Messages::createMessageAsync()";
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Messages::crosspostMessageAsync(CrosspostMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Crosspost_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/crosspost";
		workload.callStack = "Messages::crosspostMessageAsync()";
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Messages::editMessageAsync(EditMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId);
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(static_cast<JsonSerializer>(dataPackage).getString(), dataPackage.files);
		} else {
			workload.content = static_cast<JsonSerializer>(dataPackage).getString();
		}
		workload.callStack = "Messages::editMessageAsync()";
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<void> Messages::deleteMessageAsync(DeleteMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Message_Old };
		bool hasTimeElapsedNew = dataPackage.timeStamp.hasTimeElapsed(14, 0, 0);
		if (!hasTimeElapsedNew) {
			workload = DiscordCoreInternal::HttpsWorkloadType::Delete_Message;
		}
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId);
		workload.callStack = "Messages::deleteMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Messages::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Messages::deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Delete_Messages };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/bulk-delete";
		workload.content = static_cast<JsonSerializer>(dataPackage).getString();
		workload.callStack = "Messages::deleteMessagesBulkAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Messages::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Message>> Messages::getPinnedMessagesAsync(GetPinnedMessagesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Pinned_Messages };
		co_await NewThreadAwaitable<std::vector<Message>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/pins";
		workload.callStack = "Messages::getPinnedMessagesAsync()";
		co_return Messages::httpsClient->submitWorkloadAndGetResult<MessageVector>(workload);
	}

	CoRoutine<void> Messages::pinMessageAsync(PinMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Pin_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/pins/" + std::to_string(dataPackage.messageId);
		workload.callStack = "Messages::pinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Messages::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Messages::unpinMessageAsync(UnpinMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Pin_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/pins/" + std::to_string(dataPackage.messageId);
		workload.callStack = "Messages::unpinMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Messages::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	template<> std::unordered_map<std::string, UnboundedMessageBlock<Message>*> ObjectCollector<Message>::objectsBufferMap{};
	DiscordCoreInternal::HttpsClient* Messages::httpsClient{ nullptr };
}
