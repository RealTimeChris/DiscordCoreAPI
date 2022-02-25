/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// MessageEntities.cpp - Source file for the message related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file MessageEntities.cpp

#include <MessageEntities.hpp>
#include <DiscordCoreClient.hpp>
#include <CoRoutine.hpp>
#include <Http.hpp>
#include <EventEntities.hpp>

namespace DiscordCoreAPI {

	Message::Message(MessageData dataNew) {
		this->referencedMessage = dataNew.referencedMessage;
		this->messageReference = dataNew.messageReference;
		this->mentionChannels = dataNew.mentionChannels;
		this->mentionEveryone = dataNew.mentionEveryone;
		this->editedTimestamp = dataNew.editedTimestamp;
		this->applicationId = dataNew.applicationId;
		this->mentionRoles = dataNew.mentionRoles;
		this->stickerItems = dataNew.stickerItems;
		this->attachments = dataNew.attachments;
		this->application = dataNew.application;
		this->interaction = dataNew.interaction;
		this->requesterId = dataNew.requesterId;
		this->components = dataNew.components;
		this->channelId = dataNew.channelId;
		this->timestamp = dataNew.timestamp;
		this->reactions = dataNew.reactions;
		this->webhookId = dataNew.webhookId;
		this->mentions = dataNew.mentions;
		this->activity = dataNew.activity;
		this->stickers = dataNew.stickers;
		this->guildId = dataNew.guildId;
		this->content = dataNew.content;
		this->author = dataNew.author;
		this->member = dataNew.member;
		this->embeds = dataNew.embeds;
		this->pinned = dataNew.pinned;
		this->thread = dataNew.thread;
		this->nonce = dataNew.nonce;
		this->flags = dataNew.flags;
		this->type = dataNew.type;
		this->tts = dataNew.tts;
		this->id = dataNew.id;
	};

	CoRoutine<MessageCollectorReturnData> MessageCollector::collectMessages(int32_t quantityToCollect, int32_t msToCollectForNew, std::string userIdNew, std::function<bool(Message)> filteringFunctionNew) {
		co_await NewThreadAwaitable<MessageCollectorReturnData>();
		this->quantityOfMessageToCollect = quantityToCollect;
		this->filteringFunction = filteringFunctionNew;
		this->msToCollectFor = msToCollectForNew;
		this->userId = userIdNew;
		this->messagesBuffer = std::make_unique<UnboundedMessageBlock<Message>>();
		MessageCollector::messagesBufferMap.insert_or_assign(this->userId, this->messagesBuffer.get());
		this->run();
		co_return this->messageReturnData;
	}

	void MessageCollector::run() {
		int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		int64_t elapsedTime{ 0 };
		while (elapsedTime < this->msToCollectFor) {
			Message message{};
			waitForTimeToPass<Message>(*this->messagesBuffer.get(), message, static_cast<int32_t>(this->msToCollectFor - elapsedTime));
			if (this->filteringFunction(message)) {
				this->messageReturnData.messages.push_back(message);
			}
			if (static_cast<int32_t>(this->messageReturnData.messages.size()) >= this->quantityOfMessageToCollect) {
				break;
			}

			elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingTime;
		}
	}

	MessageCollector::~MessageCollector() {
		if (MessageCollector::messagesBufferMap.contains(this->userId)) {
			MessageCollector::messagesBufferMap.erase(this->userId);
		}
	}

	void Messages::initialize(DiscordCoreInternal::HttpClient*theClient) {
		Messages::httpClient = theClient;
	}

	CoRoutine<std::vector<Message>> Messages::getMessagesAsync(GetMessagesData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<Message>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Messages;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			if (dataPackage.aroundThisId != "") {
				workload.relativePath += "?around=" + dataPackage.aroundThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.beforeThisId != "") {
				workload.relativePath += "?before=" + dataPackage.beforeThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.afterThisId != "") {
				workload.relativePath += "?after=" + dataPackage.afterThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else {
				if (dataPackage.limit != 0) {
					workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
				}
			}
			workload.callStack = "Messages::getMessagesAsync";
			co_return  DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Message>>(*Messages::httpClient, workload);
		}
		catch (...) {
			reportException("Messages::getMessagesAsync()");
		}
	}
	
	CoRoutine<Message> Messages::getMessageAsync(GetMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<Message>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.id;
			workload.callStack = "Messages::getMessageAsync";
			auto result = DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
			result.requesterId = dataPackage.requesterId;
			co_return result;
		}
		catch (...) {
			reportException("Messages::getMessageAsync()");
		}
	}

	CoRoutine<Message> Messages::createMessageAsync(CreateMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<Message>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Messages::createMessageAsync";
			auto result = DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
			result.requesterId = dataPackage.requesterId;
			co_return result;
		}
		catch (...) {
			reportException("Messages::createMessageAsync()");
		}
	}

	CoRoutine<Message> Messages::crosspostMessageAsync(CrosspostMessageData dataPackage) {
		try{
			co_await NewThreadAwaitable<Message>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Crosspost_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/crosspost";
			workload.callStack = "Messages::crosspostMessageAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
		}
		catch (...) {
			reportException("Messages::crosspostMessageAsync()");
		}
	}
	
	CoRoutine<Message> Messages::editMessageAsync(EditMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<Message>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Messages::editMessageAsync";
			auto result = DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
			result.requesterId = dataPackage.requesterId;
			co_return result;
		}
		catch (...) {
			reportException("Messages::editMessageAsync()");
		}
	}
	
	void Messages::deleteMessageToBeWrapped(DeleteMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			bool hasTimeElapsedNew = hasTimeElapsed(dataPackage.messageData.timestamp.getOriginalTimeStamp(), 14);
			if (hasTimeElapsedNew) {
				workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Message_Old;
			}
			else {
				workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Message;
			}
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.messageData.channelId + "/messages/" + dataPackage.messageData.id;
			workload.callStack = "Messages::deleteMessageToBeWrapped";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		}
		catch (...) {
			reportException("Messages::deleteMessageToBeWrapped()");
		}
	}
	 
	CoRoutine<void> Messages::deleteMessageAsync(DeleteMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			if (dataPackage.timeDelay > 0) {
				TimeElapsedHandler onSend = [=]() {
					Messages::deleteMessageToBeWrapped(dataPackage);
				};
				ThreadPoolTimer::executeFunctionAfterTimePeriod(onSend, dataPackage.timeDelay);
			}
			else {
				Messages::deleteMessageToBeWrapped(dataPackage);
			}
			co_return;
		}
		catch (...) {
			reportException("Messages::deleteMessageAsync()");
		}
	}
	
	CoRoutine<void> Messages::deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Bulk_Delete_Messages;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Messages::deleteMessagesBulkAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		}
		catch (...) {
			reportException("Messages::deleteMessagesBulkAsync()");
		}
	}
	
	CoRoutine<std::vector<Message>> Messages::getPinnedMessagesAsync(GetPinnedMessagesData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<Message>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Pinned_Messages;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins";
			workload.callStack = "Messages::getPinnedMessagesAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Message>>(*Messages::httpClient, workload);
		}
		catch (...) {
			reportException("Messages::getPinnedMessagesAsync()");
		}
	}

	CoRoutine<void> Messages::pinMessageAsync(PinMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Pin_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
			workload.callStack = "Messages::pinMessageAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		}
		catch (...) {
			reportException("Messages::pinMessageAsync()");
		}
	}

	CoRoutine<void> Messages::unpinMessageAsync(UnpinMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Pin_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
			workload.callStack = "Messages::unpinMessageAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		}
		catch (...) {
			reportException("Messages::unpinMessageAsync()");
		}
	}
	
	std::unordered_map<std::string, UnboundedMessageBlock<Message>*> MessageCollector::messagesBufferMap{};
	DiscordCoreInternal::HttpClient* Messages::httpClient{ nullptr };
}
