// MessageStuff.hpp - Header for Message classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _MESSAGE_STUFF_
#define _MESSAGE_STUFF_

#include "../pch.h"
#include "ReactionStuff.hpp"
#include "JSONifier.hpp"
#include "HttpStuff.hpp"

namespace DiscordCoreAPI {

	class InputEvents;
	class Messages;
	
	class Message : public MessageData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Message>;
		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend struct OnMessageCreationData;
		friend struct OnMessageUpdateData;
		friend class DiscordCoreClient;

		Message() {};

		Message(MessageData dataNew) {
			this->id = dataNew.id;
			this->channelId = dataNew.channelId;
			this->guildId = dataNew.guildId;
			this->author = dataNew.author;
			this->member = dataNew.member;
			this->content = dataNew.content;
			this->timestamp = dataNew.timestamp;
			this->timestampRaw = dataNew.timestampRaw;
			this->editedTimestamp = dataNew.editedTimestamp;
			this->tts = dataNew.tts;
			this->mentionEveryone = dataNew.mentionEveryone;
			this->mentions = dataNew.mentions;
			this->mentionRoles = dataNew.mentionRoles;
			this->mentionChannels = dataNew.mentionChannels;
			this->attachments = dataNew.attachments;
			this->embeds = dataNew.embeds;
			this->reactions = dataNew.reactions;
			this->nonce = dataNew.nonce;
			this->pinned = dataNew.pinned;
			this->webhookId = dataNew.webhookId;
			this->type = dataNew.type;
			this->activity = dataNew.activity;
			this->application = dataNew.application;
			this->applicationId = dataNew.applicationId;
			this->messageReference = dataNew.messageReference;
			this->flags = dataNew.flags;
			this->referencedMessage = dataNew.referencedMessage;
			this->stickers = dataNew.stickers;
			this->interaction = dataNew.interaction;
			this->components = dataNew.components;
			this->thread = dataNew.thread;
			this->stickerItems = dataNew.stickerItems;
			this->discordCoreClient = dataNew.discordCoreClient;
		}
	};

	struct EditMessageData {

		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;
		
		EditMessageData(InputEventData dataPackage) {
			this->channelId = dataPackage.getChannelId();
			this->messageId = dataPackage.getMessageId();
			this->originalMessageData = dataPackage.getMessageData();
			this->requesterId = dataPackage.getRequesterId();
		}
		void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.name = emojiName;
					component.emoji.id = emojiId;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.url = url;
					component.type = ComponentType::Button;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:
		string content{ "" };
		bool tts{ false };
		vector<EmbedData> embeds{};
		AllowedMentionsData allowedMentions{};
		MessageReferenceData messageReference{};
		vector<ActionRowData> components{};
		vector<AttachmentData> attachments{};
		int flags{ 0 };
		int nonce{ 0 };
		MessageData originalMessageData{};
		MessageData replyingToMessageData{};
		string requesterId{ "" };
		string channelId{ "" };
		string messageId{ "" };
	};

	struct CreateMessageData {
		operator DiscordCoreInternal::CreateMessageData() {
			DiscordCoreInternal::CreateMessageData newData;
			newData.allowedMentions = this->allowedMentions;
			newData.channelId = this->channelId;
			for (auto value : this->components) {
				newData.components.push_back(value);
			}
			newData.content = this->content;
			for (auto value : this->embeds) {
				newData.embeds.push_back(value);
			}
			newData.messageReference = this->messageReference;
			newData.nonce = this->nonce;
			newData.tts = this->tts;
			return newData;
		}
		CreateMessageData(string channelId) {
			this->channelId = channelId;
		}
		CreateMessageData(InputEventData dataPackage) {
			this->channelId = dataPackage.getChannelId();
			this->requesterId = dataPackage.getRequesterId();
		}
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}
	protected:
		MessageReferenceData messageReference{};
		AllowedMentionsData allowedMentions{};
		vector<ActionRowData> components{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string content{ "" };
		bool tts{ false };
		int nonce{ 0 };
	};

	struct ReplyMessageData {

		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;
		
		ReplyMessageData(InputEventData dataPackage) {
			this->replyingToMessageData = dataPackage.getMessageData();
			this->messageReference.channelId = dataPackage.getChannelId();
			this->messageReference.failIfNotExists = false;
			this->messageReference.guildId = dataPackage.getGuildId();
			this->messageReference.messageId = dataPackage.getMessageId();
			this->requesterId = dataPackage.getRequesterId();
		}

		void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.name = emojiName;
					component.emoji.id = emojiId;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.url = url;
					component.type = ComponentType::Button;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}
		
	protected:
		string content{ "" };
		bool tts{ false };
		vector<EmbedData> embeds{};
		AllowedMentionsData allowedMentions{};
		MessageReferenceData messageReference{};
		vector<ActionRowData> components{};
		int nonce{ 0 };
		MessageData replyingToMessageData{};
		string requesterId{ "" };
	};

	struct SendDMData {

		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;

		SendDMData(InputEventData dataPackage, string targetUserId) {
			this->channelId = dataPackage.getChannelId();
			this->requesterId = dataPackage.getRequesterId();
			this->userId = targetUserId;
		}
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}
	protected:
		MessageReferenceData messageReference{};
		AllowedMentionsData allowedMentions{};
		vector<ActionRowData> components{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string content{ "" };
		string userId{ "" };		
		bool tts{ false };
		int nonce{ 0 };
	};

	struct FetchPinnedMessagesData {
		string channelId{ "" };
	};

	struct FetchMessagesData {
		string channelId{ "" };
		unsigned int limit{ 0 };
		string beforeThisId{ "" };
		string afterThisId{ "" };
		string aroundThisId{ "" };
	};

	struct FetchMessageData {
		string requesterId{ "" };
		string channelId{ "" };
		string id{ "" };
	};

	struct PinMessageData {
		string channelId{ "" };
		string messageId{ "" };
	};

	struct DeleteMessageData {
		unsigned int timeDelay{ 0 };
		string channelId{ "" };
		string messageId{ "" };
	};

	struct DeleteMessagesBulkData {
		string channelId{ "" };
		vector<string> messageIds{};
	};
};

namespace DiscordCoreInternal {

	class MessageManagerAgent : agent {
	protected:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class MessageManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<DeleteMessagesBulkData> requestDeleteMultMessagesBuffer{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Message>> outMultMessagesBuffer{ nullptr };
		unbounded_buffer<GetPinnedMessagesData> requestGetPinnedMessagesBuffer{ nullptr };
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		unbounded_buffer<DeleteMessageData> requestDeleteMessageBuffer{ nullptr };
		unbounded_buffer<PutPinMessageData> requestPutPinMessageBuffer{ nullptr };
		unbounded_buffer<PatchMessageData> requestPatchMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Message> outMessageBuffer{ nullptr };
		unbounded_buffer<PostMessageData> requestPostMessageBuffer{ nullptr };
		unbounded_buffer<GetMessagesData> requestGetMessagesBuffer{ nullptr };
		unbounded_buffer<GetMessageData> requestGetMessageBuffer{ nullptr };
		unbounded_buffer<PostDMData> requestPostDMMessageBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		HttpAgentResources agentResources{};
		
		MessageManagerAgent(HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew)
			:agent(*MessageManagerAgent::threadContext->scheduler->ptrScheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize(shared_ptr<ThreadContext> threadContextNew) {
			MessageManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			MessageManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::MessageManagerAgent Error: " << error.what() << endl << endl;
			}
			return;
		}

		DiscordCoreAPI::Message getObjectData(GetMessageData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::getObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObject_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObject_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &messageData);
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreAPI::Message messageNew(messageData);
			return messageNew;
		}

		vector<DiscordCoreAPI::Message> getObjectData(GetMessagesData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_MESSAGES;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			if (dataPackage.aroundThisId != ""){
				workload.relativePath += "?around=" + dataPackage.aroundThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.beforeThisId!= "") {
				workload.relativePath += "?before=" + dataPackage.beforeThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.afterThisId!= "") {
				workload.relativePath += "?after=" + dataPackage.afterThisId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else {
				if (dataPackage.limit != 0) {
					workload.relativePath += "?limit=" + to_string(dataPackage.limit);
				}
			}
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::getObjectData_01");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<DiscordCoreAPI::Message> messagesVector{};
			for (auto value : returnData.data) {
				DiscordCoreAPI::MessageData messageData;
				messageData.discordCoreClient = this->discordCoreClient;
				DataParser::parseObject(value, &messageData);
				DiscordCoreAPI::Message messageNew(messageData);
				messagesVector.push_back(messageNew);
			}
			return messagesVector;
		}

		vector<DiscordCoreAPI::Message> getObjectData(GetPinnedMessagesData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_PINNED_MESSAGES;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins";
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::getObjectData_02");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<DiscordCoreAPI::Message> messagesVector{};
			for (auto value : returnData.data) {
				DiscordCoreAPI::MessageData messageData;
				messageData.discordCoreClient = this->discordCoreClient;
				DataParser::parseObject(value, &messageData);
				DiscordCoreAPI::Message messageNew(messageData);
				messagesVector.push_back(messageNew);
			}
			return messagesVector;
		}

		DiscordCoreAPI::Message patchObjectData(PatchMessageData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = getEditMessagePayload(dataPackage);;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::patchObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &messageData);
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreAPI::Message messageNew(messageData);
			return messageNew;
		}

		DiscordCoreAPI::Message postObjectData(PostMessageData dataPackage) {
			HttpWorkload workload;
			workload.content = dataPackage.content;
			workload.workloadType = HttpWorkloadType::POST_MESSAGE;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = getReplyMessagePayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::postObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &messageData);
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreAPI::Message messageNew(messageData);
			return messageNew;
		}

		DiscordCoreAPI::Message postObjectData(PostDMData dataPackage) {
			HttpWorkload workload;
			workload.workloadType = HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = getCreateMessagePayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::postObjectData_01");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &messageData);
			DiscordCoreAPI::Message messageNew(messageData);
			return messageNew;
		}

		void postObjectData(DeleteMessagesBulkData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.workloadType = HttpWorkloadType::DELETE_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
			workload.content = dataPackage.content;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::postObjectData_02");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void putObjectData(PutPinMessageData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.workloadType = HttpWorkloadType::PUT_PIN_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::putObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent:putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void onDeleteData(DeleteMessageData dataPackage) {
			HttpWorkload workload;
			workload.workloadType = HttpWorkloadType::DELETE_MESSAGE;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::deleteObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::onDeleteData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::onDeleteData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DeleteMessageData dataPackage) {
			if (dataPackage.timeDelay > 0) {
				ThreadPoolTimer threadPoolTimer = ThreadPoolTimer(nullptr);
				TimerElapsedHandler onSend = [=](ThreadPoolTimer threadPoolTimer) {
					onDeleteData(dataPackage);
				};
				threadPoolTimer.CreateTimer(onSend, winrt::Windows::Foundation::TimeSpan(dataPackage.timeDelay * 10000));
			}
			else {
				onDeleteData(dataPackage);
			}
			return;
		}

		void run() {
			try {
				PostMessageData dataPackage01;
				if (try_receive(this->requestPostMessageBuffer, dataPackage01)) {
					DiscordCoreAPI::Message message = this->postObjectData(dataPackage01);
					send(this->outMessageBuffer, message);
				};
				PostDMData dataPackage03;
				if (try_receive(this->requestPostDMMessageBuffer, dataPackage03)) {
					DiscordCoreAPI::Message message = this->postObjectData(dataPackage03);
					send(this->outMessageBuffer, message);
				};
				GetMessageData dataPackage04;
				if (try_receive(this->requestGetMessageBuffer, dataPackage04)) {
					DiscordCoreAPI::Message message = getObjectData(dataPackage04);
					send(this->outMessageBuffer, message);
				}
				PatchMessageData dataPackage05;
				if (try_receive(this->requestPatchMessageBuffer, dataPackage05)) {
					DiscordCoreAPI::Message message = patchObjectData(dataPackage05);
					send(this->outMessageBuffer, message);
				}
				GetMessagesData dataPackage06;
				if (try_receive(this->requestGetMessagesBuffer, dataPackage06)) {
					vector<DiscordCoreAPI::Message> messages = getObjectData(dataPackage06);
					send(this->outMultMessagesBuffer, messages);
				}
				DeleteMessageData dataPackage07;
				if (try_receive(this->requestDeleteMessageBuffer, dataPackage07)) {
					deleteObjectData(dataPackage07);
				}
				DeleteMessagesBulkData dataPackage08;
				if (try_receive(this->requestDeleteMultMessagesBuffer, dataPackage08)) {
					postObjectData(dataPackage08);
				}
				GetPinnedMessagesData dataPackage09;
				if (try_receive(this->requestGetPinnedMessagesBuffer, dataPackage09)) {
					vector<DiscordCoreAPI::Message> messageVector = getObjectData(dataPackage09);
					send(this->outMultMessagesBuffer, messageVector);
				}
				PutPinMessageData dataPackage10;
				if (try_receive(this->requestPutPinMessageBuffer, dataPackage10)) {
					putObjectData(dataPackage10);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
		}
	};

	class MessageManager {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::InputEvents;
		friend class DiscordCoreAPI::Messages;

		MessageManager(MessageManager* pointer) {
			if (pointer != nullptr){
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<ThreadContext> threadContext{ nullptr };
		HttpAgentResources agentResources{};

		MessageManager operator=(const MessageManager& dataPackage) {
			MessageManager pointerToManager{ dataPackage };
			return pointerToManager;
		}

		MessageManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<ThreadContext> threadContextNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
			this->threadContext = threadContextNew;
			return *this;
		}

		task<DiscordCoreAPI::Message> replyAsync(DiscordCoreAPI::ReplyMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PostMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.content = dataPackage.content;
			for (auto value : dataPackage.embeds) {
				dataPackageNew.embeds.push_back(value);
			}
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.nonce = dataPackage.nonce;
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.channelId = dataPackage.replyingToMessageData.channelId;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPostMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::replyAsync");
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Message messageNew(messageData);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.requesterId = dataPackage.requesterId;
			co_await mainThread;
			co_return messageNew;
		}

		task<DiscordCoreAPI::Message> sendDMAsync(DiscordCoreAPI::SendDMData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PostDMData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageData.allowedMentions = dataPackage.allowedMentions;
			for (auto value : dataPackage.components) {
				dataPackageNew.messageData.components.push_back(value);
			}
			dataPackageNew.messageData.content= dataPackage.content;
			for (auto value : dataPackage.embeds) {
				dataPackageNew.messageData.embeds.push_back(value);
			}
			dataPackageNew.messageData.messageReference = dataPackage.messageReference;
			dataPackageNew.messageData.nonce = dataPackage.nonce;
			dataPackageNew.messageData.requesterId = dataPackage.requesterId;
			dataPackageNew.messageData.tts = dataPackage.tts;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPostDMMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::sendDMAsync");
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Message messageNew(messageData);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.requesterId = dataPackage.requesterId;
			co_await mainThread;
			co_return messageNew;
		}

		task<DiscordCoreAPI::Message> createMessageAsync(CreateMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PostMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.content = dataPackage.content;
			for (auto value : dataPackage.embeds) {
				dataPackageNew.embeds.push_back(value);
			}
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.nonce = dataPackage.nonce;
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.channelId = dataPackage.channelId;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPostMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::createMessageAsync");
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Message messageNew(messageData);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.requesterId = dataPackage.requesterId;
			co_await mainThread;
			co_return messageNew;
		}

		task<DiscordCoreAPI::Message> editMessageAsync(DiscordCoreAPI::EditMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PatchMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.originalMessageData.channelId;
			dataPackageNew.messageId = dataPackage.originalMessageData.id;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			for (auto value : dataPackage.attachments) {
				dataPackageNew.attachments.push_back(value);
			}
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.content = dataPackage.content;
			for (auto value : dataPackage.embeds) {
				dataPackageNew.embeds.push_back(value);
			}
			dataPackageNew.flags = dataPackage.flags;
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.originalMessageData = dataPackage.originalMessageData;
			dataPackageNew.requesterId = dataPackage.requesterId;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPatchMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::editMessageAsync");
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Message messageNew(messageData);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.requesterId = dataPackage.requesterId;
			co_await mainThread;
			co_return messageNew;
		}

		task<void> deleteMessageAsync(DiscordCoreAPI::DeleteMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.timeDelay = dataPackage.timeDelay;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::deleteMessageAsync");
			co_await mainThread;
			co_return;
		}

		task<void> deleteMessagesBulkAsync(DiscordCoreAPI::DeleteMessagesBulkData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteMessagesBulkData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageIds = dataPackage.messageIds;
			dataPackageNew.content = getDeleteMessagesBulkPayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteMultMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::deleteMessageBulkAsync");
			co_await mainThread;
			co_return;
		}

		task<vector<DiscordCoreAPI::Message>> fetchPinnedMessagesAsync(DiscordCoreAPI::FetchPinnedMessagesData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GetPinnedMessagesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetPinnedMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::fetchPinnedMessagesAsync");
			vector<DiscordCoreAPI::Message> messageVector{};
			try_receive(requestAgent.outMultMessagesBuffer, messageVector);
			co_await mainThread;
			co_return messageVector;
		}

		task<std::optional<vector<DiscordCoreAPI::Message>>> fetchMessagesAsync(DiscordCoreAPI::FetchMessagesData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GetMessagesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.afterThisId = dataPackage.afterThisId;
			dataPackageNew.aroundThisId = dataPackage.aroundThisId;
			dataPackageNew.beforeThisId = dataPackage.beforeThisId;
			dataPackageNew.limit = dataPackage.limit;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::fetchMessagesAsync");
			std::optional<vector<DiscordCoreAPI::Message>> messageVector;
			messageVector.emplace();
			try_receive(requestAgent.outMultMessagesBuffer, *messageVector);
			if (messageVector->size()==0){
				messageVector.reset();
			}
			co_await mainThread;
			co_return messageVector;
		}

		task<void> pinMessageAsync(DiscordCoreAPI::PinMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PutPinMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPutPinMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			requestAgent.getError("MessageManager::pinMessageAsync");
			co_await mainThread;
			co_return;

		}

		task<DiscordCoreAPI::Message> fetchAsync(DiscordCoreAPI::FetchMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GetMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.id;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			requestAgent.getError("MessageManager::fetchAsync");
			DiscordCoreAPI::MessageData messageData;
			messageData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Message messageNew(messageData);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.requesterId = dataPackage.requesterId;
			co_await mainThread;
			co_return messageNew;
		}

		~MessageManager() {}
	};

	shared_ptr<ThreadContext> MessageManagerAgent::threadContext{ nullptr };
}
#endif