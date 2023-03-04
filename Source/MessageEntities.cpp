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

	Message::Message(simdjson::ondemand::value jsonObjectData) {
		this->referencedMessage = std::make_unique<MessageDataOld>();

		this->content = getString(jsonObjectData, "content");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::value object{};
		if (getObject(object, "author", jsonObjectData)) {
			this->author = UserData{ object };
		}

		if (jsonObjectData["member"].get(object) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ object };
		}

		this->timeStamp = getString(jsonObjectData, "timestamp");

		this->editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		this->tts = getBool(jsonObjectData, "tts");

		this->mentionEveryone = getBool(jsonObjectData, "mention_everyone");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "mentions", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				UserData newData{ value.value() };
				this->mentions.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "mention_roles", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->mentionRoles.emplace_back(value.get_string().take_value());
			}
		}

		if (getArray(arrayValue, "mention_channels", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelMentionData newData{ value.value() };
				this->mentionChannels.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "attachments", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				AttachmentData newData{ value.value() };
				this->attachments.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "embeds", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				EmbedData newData{ value.value() };
				this->embeds.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "reactions", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ReactionData newData{ value.value() };
				this->reactions.emplace_back(std::move(newData));
			}
		}

		this->nonce = getString(jsonObjectData, "nonce");

		this->pinned = getBool(jsonObjectData, "pinned");

		this->webHookId = getId(jsonObjectData, "webhook_id");

		this->type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		if (getObject(object, "activity", jsonObjectData)) {
			this->activity = MessageActivityData{ object };
		}

		if (getObject(object, "application", jsonObjectData)) {
			this->application = ApplicationData{ object };
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		if (getObject(object, "message_reference", jsonObjectData)) {
			this->messageReference = MessageReferenceData{ object };
		}

		this->flags = getUint32(jsonObjectData, "flags");

		if (getArray(arrayValue, "sticker_items", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerItemData newData{ value.value() };
				this->stickerItems.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerData newData{ value.value() };
				this->stickers.emplace_back(std::move(newData));
			}
		}

		if (getObject(object, "interaction", jsonObjectData)) {
			this->interaction = MessageInteractionData{ object };
		}

		if (getArray(arrayValue, "components", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ActionRowData newData{ value.value() };
				this->components.emplace_back(std::move(newData));
			}
		}

		if (getObject(object, "thread", jsonObjectData)) {
			this->thread = ChannelData{ object };
		}
	}

	MessageVector::MessageVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					Message newData{ value.value() };
					this->messages.emplace_back(std::move(newData));
				}
			}
		}
	}

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

	CreateMessageData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->attachments) {
			data["attachments"].emplaceBack(value.operator Jsonifier());
		}
		if (this->messageReference.messageId != 0) {
			data["message_reference"] = this->messageReference.operator Jsonifier();
		}
		if (this->components.size() == 0) {
			data["components"].emplaceBack(ActionRowData{});
			data["components"].getValue<Jsonifier::ArrayType>().clear();
		} else {
			for (auto& value: this->components) {
				data["components"].emplaceBack(value.operator Jsonifier());
			}
		}
		if (this->allowedMentions.parse.size() > 0 || this->allowedMentions.roles.size() > 0 || this->allowedMentions.users.size() > 0) {
			data["allowed_mentions"] = this->allowedMentions.operator Jsonifier();
		}
		for (auto& value: this->stickerIds) {
			data["sticker_ids"].emplaceBack(value);
		}
		if (this->embeds.size() == 0) {
			data["embeds"].emplaceBack(EmbedData{});
			data["embeds"].getValue<Jsonifier::ArrayType>().clear();
		} else {
			for (auto& value: this->embeds) {
				data["embeds"].emplaceBack(value.operator Jsonifier());
			}
		}
		if (this->content != "") {
			data["content"] = this->content;
		}
		data["flags"] = this->flags;
		data["tts"] = this->tts;
		return data;
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

	EditMessageData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->attachments) {
			data["attachments"].emplaceBack(value.operator Jsonifier());
		}
		if (this->components.size() == 0) {
			data["components"].emplaceBack(ActionRowData{});
			data["components"].getValue<Jsonifier::ArrayType>().clear();
		} else {
			for (auto& value: this->components) {
				data["components"].emplaceBack(value.operator Jsonifier());
			}
		}
		if (this->allowedMentions.parse.size() > 0 || this->allowedMentions.roles.size() > 0 || this->allowedMentions.users.size() > 0) {
			data["allowed_mentions"] = this->allowedMentions.operator Jsonifier();
		}
		if (this->embeds.size() == 0) {
			data["embeds"].emplaceBack(EmbedData{});
			data["embeds"].getValue<Jsonifier::ArrayType>().clear();
		} else {
			for (auto& value: this->embeds) {
				data["embeds"].emplaceBack(value.operator Jsonifier());
			}
		}
		if (this->content != "") {
			data["content"] = this->content;
		}
		data["flags"] = this->flags;
		data["tts"] = this->tts;
		return data;
	}

	DeleteMessagesBulkData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->messageIds) {
			data["messages"].emplaceBack(value.operator std::string());
		}
		return data;
	}

	MessageVector::operator std::vector<Message>() {
		return this->messages;
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
		MessageVector returnValue{};
		co_return Messages::httpsClient->submitWorkloadAndGetResult<MessageVector>(workload, &returnValue);
	}

	CoRoutine<Message> Messages::getMessageAsync(GetMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.id;
		workload.callStack = "Messages::getMessageAsync()";
		Message returnValue{};
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, &returnValue);
	}

	CoRoutine<Message> Messages::createMessageAsync(CreateMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			auto serializer = dataPackage.operator Jsonifier();
			serializer.refreshString(JsonifierSerializeType::Json);
			workload.content = constructMultiPartData(serializer.operator std::string(), dataPackage.files);
		} else {
			auto serializer = dataPackage.operator Jsonifier();
			serializer.refreshString(JsonifierSerializeType::Json);
			workload.content = serializer.operator std::string();
		}
		workload.callStack = "Messages::createMessageAsync()";
		Message returnValue{};
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, &returnValue);
	}

	CoRoutine<Message> Messages::crosspostMessageAsync(CrosspostMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Crosspost_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/crosspost";
		workload.callStack = "Messages::crosspostMessageAsync()";
		Message returnValue{};
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, &returnValue);
	}

	CoRoutine<Message> Messages::editMessageAsync(EditMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			auto serializer = dataPackage.operator Jsonifier();
			serializer.refreshString(JsonifierSerializeType::Json);
			workload.content = constructMultiPartData(serializer.operator std::string(), dataPackage.files);
		} else {
			auto serializer = dataPackage.operator Jsonifier();
			serializer.refreshString(JsonifierSerializeType::Json);
			workload.content = serializer.operator std::string();
		}
		workload.callStack = "Messages::editMessageAsync()";
		Message returnValue{};
		co_return Messages::httpsClient->submitWorkloadAndGetResult<Message>(workload, &returnValue);
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
		Messages::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<void> Messages::deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Delete_Messages };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
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
		workload.relativePath = "/channels/" + dataPackage.channelId + "/pins";
		workload.callStack = "Messages::getPinnedMessagesAsync()";
		MessageVector returnValue{};
		co_return Messages::httpsClient->submitWorkloadAndGetResult<MessageVector>(workload, &returnValue);
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
		co_return Messages::httpsClient->submitWorkloadAndGetResult<void>(workload);
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
		co_return Messages::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	DiscordCoreInternal::HttpsClient* Messages::httpsClient{ nullptr };
}
