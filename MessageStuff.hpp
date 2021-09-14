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
			this->discordCoreClient = dataNew.discordCoreClient;
			this->referencedMessage = dataNew.referencedMessage;
			this->messageReference = dataNew.messageReference;
			this->mentionChannels = dataNew.mentionChannels;
			this->mentionEveryone = dataNew.mentionEveryone;
			this->editedTimestamp = dataNew.editedTimestamp;
			this->applicationId = dataNew.applicationId;
			this->timestampRaw = dataNew.timestampRaw;
			this->mentionRoles = dataNew.mentionRoles;
			this->stickerItems = dataNew.stickerItems;
			this->attachments = dataNew.attachments;
			this->application = dataNew.application;
			this->interaction = dataNew.interaction;
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
		}
	};

	struct MessageCollectorReturnData {
		vector<Message> messages;
	};

	class MessageCollector :DiscordCoreInternal::ThreadContext,  agent {
	public:

		friend class DiscordCoreClient;

		MessageCollector(int quantityToCollect,  int msToCollectForNew, string userIdNew, function<bool(Message)> filteringFunctionNew) :
			ThreadContext(*DiscordCoreInternal::ThreadManager::getThreadContext().get()),
			agent(*DiscordCoreInternal::ThreadManager::getThreadContext().get()->scheduler->scheduler) {
			this->messagesBuffer = new unbounded_buffer<Message>();
			MessageCollector::messagesBufferMap.insert_or_assign(userId, this->messagesBuffer);
			this->quantityOfMessageToCollect = quantityToCollect;
			this->filteringFunction = filteringFunctionNew;
			this->msToCollectFor = msToCollectForNew;
			this->userId = userIdNew;
		}

		task<MessageCollectorReturnData> collectMessages() {
			apartment_context mainThread;
			co_await resume_background();
			this->start();
			wait(this);
			co_await mainThread;
			co_return this->messageReturnData;
		}

		~MessageCollector() {
			MessageCollector::messagesBufferMap.erase(this->userId);
		}

	protected:
		static map<string, unbounded_buffer<Message>*> messagesBufferMap;
		unbounded_buffer<Message>* messagesBuffer{ nullptr };
		function<bool(Message)> filteringFunction{ nullptr };
		MessageCollectorReturnData messageReturnData{};
		int quantityOfMessageToCollect{ 0 };
		int msToCollectFor{ 0 };
		int startingTime{ 0 };
		int elapsedTime{ 0 };
		string userId{ "" };

		void run() {
			this->startingTime = (int)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
			while (this->elapsedTime < this->msToCollectFor) {
				try {
					Message message = receive(this->messagesBuffer, this->msToCollectFor - this->elapsedTime);
					if (this->filteringFunction(message)) {
						this->messageReturnData.messages.push_back(message);
					}
					if (this->messageReturnData.messages.size() >= this->quantityOfMessageToCollect) {
						break;
					}
				}
				catch (operation_timed_out&) {};

				this->elapsedTime = (int)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - this->startingTime;
			}
			done();
		}

	};

	map<string, unbounded_buffer<Message>*> MessageCollector::messagesBufferMap;

	struct EditMessageData {

		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;
		
		EditMessageData(InputEventData dataPackage) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
			this->messageId = dataPackage.getMessageId();
		}

		void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
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
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		void addAttachment(AttachmentData dataPackage) {
			this->attachments.push_back(dataPackage);
		}

		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

	protected:

		AllowedMentionsData allowedMentions{};
		vector<AttachmentData> attachments{};
		vector<ActionRowData> components{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string messageId{ "" };
		string content{ "" };
		int flags{ 0 };
		
		EditMessageData() {};

	};

	struct CreateMessageData {

		friend class DiscordCoreInternal::MessageManager;

		CreateMessageData(string channelIdNew) {
			this->channelId = channelIdNew;
		}

		CreateMessageData(InputEventData dataPackage) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
		}

		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:

		MessageReferenceData messageReference{};
		AllowedMentionsData allowedMentions{};
		vector<ActionRowData> components{};
		vector<string> stickerIds{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string content{ "" };
		bool tts{ false };

	};

	struct ReplyMessageData {

		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;
		
		ReplyMessageData(InputEventData dataPackage) {
			this->messageReference.messageId = dataPackage.getMessageId();
			this->messageReference.channelId = dataPackage.getChannelId();
			this->messageReference.guildId = dataPackage.getGuildId();
			this->requesterId = dataPackage.getRequesterId();
			this->messageReference.failIfNotExists = false;
		}

		void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
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
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		void addContent(string dataPackage) {
			this->content = dataPackage;
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
		string content{ "" };
		bool tts{ false };
		int nonce{ 0 };

		ReplyMessageData() {};

	};

	struct SendDMData {

		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;

		SendDMData(InputEventData dataPackage, string targetUserId) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
			this->userId = targetUserId;
		}

		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		void addContent(string dataPackage) {
			this->content = dataPackage;
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
		string aroundThisId{ "" };
		string beforeThisId{ "" };
		string afterThisId{ "" };
		unsigned int limit{ 0 };
		string channelId{ "" };
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
	public:
		unsigned int timeDelay{ 0 };
		MessageData messageData;
	};

	struct DeleteMessagesBulkData {
		vector<string> messageIds{};
		string channelId{ "" };
	};
};

namespace DiscordCoreInternal {

	class MessageManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class MessageManager;

		static shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient;
		static shared_ptr<ThreadContext> threadContext;
		static HttpAgentResources agentResources;

		unbounded_buffer<DeleteMessagesBulkData> requestDeleteMultMessagesBuffer{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Message>> outMultMessagesBuffer{ nullptr };
		unbounded_buffer<GetPinnedMessagesData> requestGetPinnedMessagesBuffer{ nullptr };
		unbounded_buffer<DeleteMessageData> requestDeleteMessageBuffer{ nullptr };
		unbounded_buffer<PutPinMessageData> requestPutPinMessageBuffer{ nullptr };
		unbounded_buffer<PatchMessageData> requestPatchMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Message> outMessageBuffer{ nullptr };
		unbounded_buffer<PostMessageData> requestPostMessageBuffer{ nullptr };
		unbounded_buffer<GetMessagesData> requestGetMessagesBuffer{ nullptr };
		unbounded_buffer<GetMessageData> requestGetMessageBuffer{ nullptr };
		unbounded_buffer<PostDMData> requestPostDMMessageBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		
		
		MessageManagerAgent()
			:agent(*MessageManagerAgent::threadContext->scheduler->scheduler) {}

		static void initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			MessageManagerAgent::threadContext = ThreadManager::getThreadContext().get();
			MessageManagerAgent::discordCoreClient = discordCoreClientNew;
			MessageManagerAgent::agentResources = agentResourcesNew;
		}

		static void cleanup() {
			MessageManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::MessageManagerAgent Error: " << error.what() << endl << endl;
			}
		}

		DiscordCoreAPI::Message getObjectData(GetMessageData dataPackage) {
			HttpWorkload workload{};
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::getObjectData_00");
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
			cout << "MESSAGE ID: " << messageNew.id << endl;
			return messageNew;
		}

		vector<DiscordCoreAPI::Message> getObjectData(GetMessagesData dataPackage) {
			HttpWorkload workload{};
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
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::getObjectData_01");
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
			HttpWorkload workload{};
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_PINNED_MESSAGES;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins";
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::getObjectData_02");
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
			HttpWorkload workload{};
			workload.workloadClass = HttpWorkloadClass::PATCH;
			workload.workloadType = HttpWorkloadType::PATCH_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = getEditMessagePayload(dataPackage);;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::patchObjectData_00");
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
			HttpWorkload workload{};
			workload.content = dataPackage.content;
			workload.workloadType = HttpWorkloadType::POST_MESSAGE;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = getCreateMessagePayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::postObjectData_00");
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
			HttpWorkload workload{};
			workload.workloadType = HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			workload.content = getCreateMessagePayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::postObjectData_01");
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
			HttpWorkload workload{};
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.workloadType = HttpWorkloadType::DELETE_MESSAGES_BULK;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
			workload.content = dataPackage.content;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::postObjectData_02");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void putObjectData(PutPinMessageData dataPackage) {
			HttpWorkload workload{};
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.workloadType = HttpWorkloadType::PUT_PIN_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/pins/" + dataPackage.messageId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::putObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent:putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void onDeleteData(DeleteMessageData dataPackage) {
			HttpWorkload workload{};
			bool hasTimeElapsed = DiscordCoreAPI::hasTimeElapsed(dataPackage.messageTimeStamp, 14);
			if (hasTimeElapsed) {
				workload.workloadType = HttpWorkloadType::DELETE_MESSAGE_OLD;
			}
			else {
				workload.workloadType = HttpWorkloadType::DELETE_MESSAGE;
			}
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "MessageManagerAgent::deleteObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::onDeleteData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::onDeleteData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
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

	class MessageManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::InputEvents;
		friend class DiscordCoreAPI::Messages;

		MessageManager(MessageManager* pointer)
			: ThreadContext(*ThreadManager::getThreadContext().get()) {
			if (pointer != nullptr){
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		HttpAgentResources agentResources{};

		MessageManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->agentResources = agentResourcesNew;
			return *this;
		}

		task<DiscordCoreAPI::Message> replyAsync(DiscordCoreAPI::ReplyMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
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
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.channelId = dataPackage.messageReference.channelId;
			MessageManagerAgent requestAgent{};
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
			co_await resume_foreground(*this->dispatcherQueue.get());
			PostDMData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allowedMentions= dataPackage.allowedMentions;
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.content= dataPackage.content;
			for (auto value : dataPackage.embeds) {
				dataPackageNew.embeds.push_back(value);
			}
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.nonce = dataPackage.nonce;
			dataPackageNew.requesterId = dataPackage.requesterId;
			dataPackageNew.tts = dataPackage.tts;
			MessageManagerAgent requestAgent{};
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

		task<DiscordCoreAPI::Message> createMessageAsync(DiscordCoreAPI::CreateMessageData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
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
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.channelId = dataPackage.channelId;
			MessageManagerAgent requestAgent{};
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
			co_await resume_foreground(*this->dispatcherQueue.get());
			PatchMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
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
			dataPackageNew.requesterId = dataPackage.requesterId;
			MessageManagerAgent requestAgent{};
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
			co_await resume_foreground(*this->dispatcherQueue.get());
			DeleteMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.messageData.channelId;
			dataPackageNew.messageId = dataPackage.messageData.id;
			dataPackageNew.messageTimeStamp = dataPackage.messageData.timestampRaw;
			dataPackageNew.timeDelay = dataPackage.timeDelay;
			MessageManagerAgent requestAgent{};
			send(requestAgent.requestDeleteMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::deleteMessageAsync");
			co_await mainThread;
			co_return;
		}

		task<void> deleteMessagesBulkAsync(DiscordCoreAPI::DeleteMessagesBulkData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			DeleteMessagesBulkData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageIds = dataPackage.messageIds;
			dataPackageNew.content = getDeleteMessagesBulkPayload(dataPackageNew);
			MessageManagerAgent requestAgent{};
			send(requestAgent.requestDeleteMultMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("MessageManager::deleteMessageBulkAsync");
			co_await mainThread;
			co_return;
		}

		task<vector<DiscordCoreAPI::Message>> fetchPinnedMessagesAsync(DiscordCoreAPI::FetchPinnedMessagesData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetPinnedMessagesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			MessageManagerAgent requestAgent{};
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
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetMessagesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.afterThisId = dataPackage.afterThisId;
			dataPackageNew.aroundThisId = dataPackage.aroundThisId;
			dataPackageNew.beforeThisId = dataPackage.beforeThisId;
			dataPackageNew.limit = dataPackage.limit;
			MessageManagerAgent requestAgent{};
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
			co_await resume_foreground(*this->dispatcherQueue.get());
			PutPinMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			MessageManagerAgent requestAgent{};
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
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.id;
			MessageManagerAgent requestAgent{};
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
	shared_ptr<DiscordCoreAPI::DiscordCoreClient> MessageManagerAgent::discordCoreClient{ nullptr };
	shared_ptr<ThreadContext> MessageManagerAgent::threadContext{ nullptr };
	HttpAgentResources MessageManagerAgent::agentResources{};
}
#endif