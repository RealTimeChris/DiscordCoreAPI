/*
*
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
/// MessageEntities.cpp - Source file for the message related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file MessageEntities.cpp

#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/EventEntities.hpp>

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

	CoRoutine<MessageCollectorReturnData> MessageCollector::collectMessages(
		int32_t quantityToCollect, int32_t msToCollectForNew, MessageFilter filteringFunctionNew) {
		co_await NewThreadAwaitable<MessageCollectorReturnData>();
		this->quantityOfMessageToCollect = quantityToCollect;
		this->filteringFunction = filteringFunctionNew;
		this->msToCollectFor = msToCollectForNew;
		this->collectorId = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		MessageCollector::messagesBufferMap.insert_or_assign(this->collectorId, &this->messagesBuffer);
		this->run();
		co_return this->messageReturnData;
	}

	void MessageCollector::run() {
		int64_t startingTime =
			static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		int64_t elapsedTime{ 0 };
		while (elapsedTime < this->msToCollectFor) {
			Message message{};
			waitForTimeToPass<Message>(this->messagesBuffer, message, static_cast<int32_t>(this->msToCollectFor - elapsedTime));
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
		if (MessageCollector::messagesBufferMap.contains(this->collectorId)) {
			MessageCollector::messagesBufferMap.erase(this->collectorId);
		}
	}

	void Messages::initialize(DiscordCoreInternal::HttpClient* theClient) {
		Messages::httpClient = theClient;
	}

	CoRoutine<std::vector<Message>> Messages::getMessagesAsync(GetMessagesData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Messages];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Messages] += 1;
			co_await NewThreadAwaitable<std::vector<Message>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Messages;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			if (dataPackage.aroundThisId != "") {
				workload.relativePath += "?around=" + dataPackage.aroundThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				} else if (dataPackage.limit == 0) {
					workload.relativePath += "&limit=1";
				}
			} else if (dataPackage.beforeThisId != "") {
				workload.relativePath += "?before=" + dataPackage.beforeThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				} else if (dataPackage.limit == 0) {
					workload.relativePath += "&limit=1";
				}
			} else if (dataPackage.afterThisId != "") {
				workload.relativePath += "?after=" + dataPackage.afterThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				} else if (dataPackage.limit == 0) {
					workload.relativePath += "&limit=1";
				}
			} else {
				if (dataPackage.limit != 0) {
					workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
				} else if (dataPackage.limit == 0) {
					workload.relativePath += "&limit=1";
				}
			}
			workload.callStack = "Messages::getMessagesAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Message>>(*Messages::httpClient, workload);
		} catch (...) {
			reportException("Messages::getMessagesAsync()");
		}
	}

	CoRoutine<Message> Messages::getMessageAsync(GetMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Message];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Message] += 1;
			co_await NewThreadAwaitable<Message>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.id;
			workload.callStack = "Messages::getMessageAsync";
			auto result = DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
			result.requesterId = dataPackage.requesterId;
			co_return result;
		} catch (...) {
			reportException("Messages::getMessageAsync()");
		}
	}

	CoRoutine<Message> Messages::createMessageAsync(CreateMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Message];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Message] += 1;
			co_await NewThreadAwaitable<Message>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Messages::createMessageAsync";
			auto result = DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
			result.requesterId = dataPackage.requesterId;
			co_return result;
		} catch (...) {
			reportException("Messages::createMessageAsync()");
		}
	}

	CoRoutine<Message> Messages::crosspostMessageAsync(CrosspostMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Crosspost_Message];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Crosspost_Message] += 1;
			co_await NewThreadAwaitable<Message>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Crosspost_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/crosspost";
			workload.callStack = "Messages::crosspostMessageAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
		} catch (...) {
			reportException("Messages::crosspostMessageAsync()");
		}
	}

	CoRoutine<Message> Messages::editMessageAsync(EditMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Message];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Message] += 1;
			co_await NewThreadAwaitable<Message>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Messages::editMessageAsync";
			auto result = DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Messages::httpClient, workload);
			result.requesterId = dataPackage.requesterId;
			co_return result;
		} catch (...) {
			reportException("Messages::editMessageAsync()");
		}
	}

	CoRoutine<void> Messages::deleteMessageAsync(DeleteMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			bool hasTimeElapsedNew = hasTimeElapsed(dataPackage.timeStamp.getOriginalTimeStamp(), 14);
			if (hasTimeElapsedNew) {
				workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Message_Old];
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Message_Old] += 1;
				workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Message_Old;
			} else {
				workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Message];
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Message] += 1;
				workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Message;
			}
			co_await NewThreadAwaitable<void>();
			std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.callStack = "Messages::deleteMessageAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		} catch (...) {
			reportException("Messages::deleteMessageAsync()");
		}
	}

	CoRoutine<void> Messages::deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Bulk_Delete_Messages];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Bulk_Delete_Messages] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Bulk_Delete_Messages;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Messages::deleteMessagesBulkAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		} catch (...) {
			reportException("Messages::deleteMessagesBulkAsync()");
		}
	}

	CoRoutine<std::vector<Message>> Messages::getPinnedMessagesAsync(GetPinnedMessagesData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Pinned_Messages];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Pinned_Messages] += 1;
			co_await NewThreadAwaitable<std::vector<Message>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Pinned_Messages;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins";
			workload.callStack = "Messages::getPinnedMessagesAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Message>>(*Messages::httpClient, workload);
		} catch (...) {
			reportException("Messages::getPinnedMessagesAsync()");
		}
	}

	CoRoutine<void> Messages::pinMessageAsync(PinMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Put_Pin_Message];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Put_Pin_Message] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Pin_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
			workload.callStack = "Messages::pinMessageAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		} catch (...) {
			reportException("Messages::pinMessageAsync()");
		}
	}

	CoRoutine<void> Messages::unpinMessageAsync(UnpinMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Pin_Message];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Pin_Message] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Pin_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
			workload.callStack = "Messages::unpinMessageAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Messages::httpClient, workload);
		} catch (...) {
			reportException("Messages::unpinMessageAsync()");
		}
	}

	std::unordered_map<std::string, UnboundedMessageBlock<Message>*> MessageCollector::messagesBufferMap{};
	DiscordCoreInternal::HttpClient* Messages::httpClient{ nullptr };
}
