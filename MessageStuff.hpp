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

	class Message {
	public:
		MessageData data{};
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		Message() {};

		Message(MessageData dataNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = discordCoreClientNew;
		}

	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;
		friend class InteractionManager;
	};

	struct EditMessageData {
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
		string content{ "" };
		vector<EmbedData> embeds{};
		int flags{ 0 };
		vector<AttachmentData> attachments{};
		AllowedMentionsData allowedMentions{};
		MessageData originalMessageData{};
		vector<ActionRowData> components{};
		MessageReferenceData messageReference{};
		string requesterId{ "" };
	protected:
		friend class InteractionManagerAgent;
		friend class InteractionManager;
		friend class InputEvents;
		friend class MessageManager;
		friend class MessageManagerAgent;
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
		CreateMessageData() {}
		CreateMessageData(InputEventData dataPackage) {
			this->channelId = dataPackage.getChannelId();
			this->requesterId = dataPackage.getRequesterId();
		}
		AllowedMentionsData allowedMentions{};
		string content{ "" };
		vector<EmbedData> embeds{};
		MessageReferenceData messageReference{};
		vector<ActionRowData> components{};
		int nonce{ 0 };
		bool tts{ false };
		string channelId{ "" };
	protected:
		friend class MessageManager;
		string requesterId{ "" };
	};

	struct ReplyMessageData {
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
		string content{ "" };
		bool tts{ false };
		vector<EmbedData> embeds{};
		AllowedMentionsData allowedMentions{};
		MessageReferenceData messageReference{};
		vector<ActionRowData> components{};
		int nonce{ 0 };
		MessageData replyingToMessageData{};
		string requesterId{ "" };
	protected:
		friend class InteractionManagerAgent;
		friend class InteractionManager;
		friend class InputEvents;
		friend class MessageManager;
		friend class MessageManagerAgent;
	};

	struct SendDMData {
		SendDMData(InputEventData dataPackage) : messageData(dataPackage) {}
		string channelId{ "" };
		string userId{ "" };
		CreateMessageData messageData{};
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
		friend class InteractionManager;

		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;

		unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData> requestDeleteMultMessagesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetPinnedMessagesData> requestGetPinnedMessagesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::DeleteMessageData> requestDeleteMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PutPinMessageData> requestPutPinMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PatchMessageData> requestPatchMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PostMessageData> requestPostMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetMessagesData> requestGetMessagesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetMessageData> requestGetMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PostDMData> requestPostDMMessageBuffer{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Message>> outMultMessagesBuffer{nullptr};
		unbounded_buffer<DiscordCoreAPI::Message> outMessageBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		
		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };

		MessageManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew)
			:agent(*MessageManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
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

		DiscordCoreAPI::Message getObjectData(DiscordCoreInternal::GetMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::getObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObject_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObject_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
		}

		vector<DiscordCoreAPI::Message> getObjectData(DiscordCoreInternal::GetMessagesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGES;
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
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::getObjectData_01");
			DiscordCoreInternal::HttpData returnData;
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
				DiscordCoreInternal::DataParser::parseObject(value, &messageData);
				DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
				messagesVector.push_back(messageNew);
			}
			return messagesVector;
		}

		vector<DiscordCoreAPI::Message> getObjectData(DiscordCoreInternal::GetPinnedMessagesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_PINNED_MESSAGES;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::getObjectData_02");
			DiscordCoreInternal::HttpData returnData;
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
				DiscordCoreInternal::DataParser::parseObject(value, &messageData);
				DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
				messagesVector.push_back(messageNew);
			}
			return messagesVector;
		}

		DiscordCoreAPI::Message patchObjectData(DiscordCoreInternal::PatchMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = DiscordCoreInternal::getEditMessagePayload(dataPackage);;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::patchObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
		}

		DiscordCoreAPI::Message postObjectData(DiscordCoreInternal::PostMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.content;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = DiscordCoreInternal::getReplyMessagePayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::postObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
		}

		DiscordCoreAPI::Message postObjectData(DiscordCoreInternal::PostDMData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = DiscordCoreInternal::getCreateMessagePayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::postObjectData_01");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::MessageData messageData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
		}

		void postObjectData(DiscordCoreInternal::DeleteMessagesBulkData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::postObjectData_02");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void putObjectData(DiscordCoreInternal::PutPinMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_PIN_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::putObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent:putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void onDeleteData(DiscordCoreInternal::DeleteMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManagerAgent::deleteObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::onDeleteData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::onDeleteData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DiscordCoreInternal::DeleteMessageData dataPackage) {
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
				DiscordCoreInternal::PostMessageData dataPackage01;
				if (try_receive(this->requestPostMessageBuffer, dataPackage01)) {
					DiscordCoreAPI::Message message = this->postObjectData(dataPackage01);
					send(this->outMessageBuffer, message);
				};
				DiscordCoreInternal::PostDMData dataPackage03;
				if (try_receive(this->requestPostDMMessageBuffer, dataPackage03)) {
					DiscordCoreAPI::Message message = this->postObjectData(dataPackage03);
					send(this->outMessageBuffer, message);
				};
				DiscordCoreInternal::GetMessageData dataPackage04;
				if (try_receive(this->requestGetMessageBuffer, dataPackage04)) {
					DiscordCoreAPI::Message message = getObjectData(dataPackage04);
					send(this->outMessageBuffer, message);
				}
				DiscordCoreInternal::PatchMessageData dataPackage05;
				if (try_receive(this->requestPatchMessageBuffer, dataPackage05)) {
					DiscordCoreAPI::Message message = patchObjectData(dataPackage05);
					send(this->outMessageBuffer, message);
				}
				DiscordCoreInternal::GetMessagesData dataPackage06;
				if (try_receive(this->requestGetMessagesBuffer, dataPackage06)) {
					vector<DiscordCoreAPI::Message> messages = getObjectData(dataPackage06);
					send(this->outMultMessagesBuffer, messages);
				}
				DiscordCoreInternal::DeleteMessageData dataPackage07;
				if (try_receive(this->requestDeleteMessageBuffer, dataPackage07)) {
					deleteObjectData(dataPackage07);
				}
				DiscordCoreInternal::DeleteMessagesBulkData dataPackage08;
				if (try_receive(this->requestDeleteMultMessagesBuffer, dataPackage08)) {
					postObjectData(dataPackage08);
				}
				DiscordCoreInternal::GetPinnedMessagesData dataPackage09;
				if (try_receive(this->requestGetPinnedMessagesBuffer, dataPackage09)) {
					vector<DiscordCoreAPI::Message> messageVector = getObjectData(dataPackage09);
					send(this->outMultMessagesBuffer, messageVector);
				}
				DiscordCoreInternal::PutPinMessageData dataPackage10;
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

		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		task<DiscordCoreAPI::Message> replyAsync(DiscordCoreAPI::ReplyMessageData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PostMessageData dataPackageNew;
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
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.data.requesterId = dataPackage.requesterId;
			co_return messageNew;
		}

		task<DiscordCoreAPI::Message> sendDMAsync(DiscordCoreAPI::SendDMData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PostDMData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageData = dataPackage.messageData;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPostDMMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::sendDMAsync");
			DiscordCoreAPI::MessageData messageData;
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			co_return messageNew;
		}

		task<DiscordCoreAPI::Message> createMessageAsync(CreateMessageData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PostMessageData dataPackageNew;
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
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.data.requesterId = dataPackage.requesterId;
			co_return messageNew;
		}

		task<DiscordCoreAPI::Message> editMessageAsync(DiscordCoreAPI::EditMessageData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PatchMessageData dataPackageNew;
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
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			messageNew.data.requesterId = dataPackage.requesterId;
			co_return messageNew;
		}

		task<void> deleteMessageAsync(DiscordCoreAPI::DeleteMessageData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.timeDelay = dataPackage.timeDelay;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::deleteMessageAsync");
			co_return;
		}

		task<void> deleteMessagesBulkAsync(DiscordCoreAPI::DeleteMessagesBulkData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteMessagesBulkData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageIds = dataPackage.messageIds;
			dataPackageNew.content = DiscordCoreInternal::getDeleteMessagesBulkPayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteMultMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::deleteMessageBulkAsync");
			co_return;
		}

		task<vector<DiscordCoreAPI::Message>> fetchPinnedMessagesAsync(DiscordCoreAPI::FetchPinnedMessagesData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetPinnedMessagesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetPinnedMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::fetchPinnedMessagesAsync");
			vector<DiscordCoreAPI::Message> messageVector{};
			try_receive(requestAgent.outMultMessagesBuffer, messageVector);
			co_return messageVector;
		}

		task<std::optional<vector<DiscordCoreAPI::Message>>> fetchMessagesAsync(DiscordCoreAPI::FetchMessagesData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetMessagesData dataPackageNew;
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
			co_return messageVector;
		}

		task<void> pinMessageAsync(DiscordCoreAPI::PinMessageData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PutPinMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPutPinMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			requestAgent.getError("MessageManager::pinMessageAsync");
			co_return;

		}

		task<DiscordCoreAPI::Message> fetchAsync(DiscordCoreAPI::FetchMessageData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetMessageData dataPackageNew;
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
			DiscordCoreAPI::Message messageNew(messageData, this->discordCoreClient);
			try_receive(requestAgent.outMessageBuffer, messageNew);
			co_return messageNew;
		}

		~MessageManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class Channel;
		friend class Guild;
		friend class DiscordCoreClient;

		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
	};

	shared_ptr<DiscordCoreInternal::ThreadContext> MessageManagerAgent::threadContext{ nullptr };
}
#endif