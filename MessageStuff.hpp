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
#include "InteractionManager.hpp"

namespace DiscordCoreAPI {

	class Message {
	public:
		MessageData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		Message() {};

	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class MessageManagerAgent;
		friend class InteractionManager;

		Message(MessageData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->initialize(dataNew, discordCoreClientNew).get();
		}

		task<void> initialize(MessageData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = discordCoreClientNew;
			co_return;
		}

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
		string content = "";
		vector<EmbedData> embeds;
		int flags = 0;
		vector<AttachmentData> attachments;
		AllowedMentionsData allowedMentions;
		MessageData originalMessageData;
		vector<ActionRowData> components;
	protected:
		friend class InteractionManagerAgent;
		friend class InteractionManager;
		friend class InputEventManager;
		friend class MessageManager;
		friend class MessageManagerAgent;
		string requesterId;
		string channelId;
		string messageId;
	};

	struct CreateMessageData {
		CreateMessageData(InputEventData dataPackage) {
			this->channelId = dataPackage.getChannelId();
			this->requesterId = dataPackage.getRequesterId();
		}
		AllowedMentionsData allowedMentions;
		string content = "";
		vector<EmbedData> embeds;
		MessageReferenceData messageReference;
		vector<ActionRowData> components;
		int nonce;
		bool tts = false;
	protected:
		friend class MessageManager;
		string channelId;
		string requesterId;
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
		string content = "";
		bool tts = false;
		vector<EmbedData> embeds;
		AllowedMentionsData allowedMentions;
		MessageReferenceData messageReference;
		int nonce;
	protected:
		friend class InteractionManagerAgent;
		friend class InteractionManager;
		friend class InputEventManager;
		friend class MessageManager;
		friend class MessageManagerAgent;
		vector<ActionRowData> components;
		MessageData replyingToMessageData;
		string requesterId;
	};

	struct SendDMData {
		string channelId;
		string userId;
		CreateMessageData messageData;
	};

	struct GetMessageData {
		string channelId;
		string id;
		string requesterId;
	};

	struct FetchMessagesData {
		string channelId;
		unsigned int limit;
		string beforeThisId;
		string afterThisId;
		string aroundThisId;
	};

	struct FetchMessageData {
		string requesterId;
		string channelId;
		string id;
	};

	struct DeleteMessageData {
		DeleteMessageData(InputEventData dataPackage) {
			this->channelId = dataPackage.getChannelId();
			this->messageId = dataPackage.getMessageId();
		}
		unsigned int timeDelay = 0;
	protected:
		friend class MessageManager;
		string channelId;
		string messageId;
	};

	struct DeleteMessagesBulkData {
		bool deletePinned;
		string channelId;
		unsigned int limit;
		string beforeThisId;
		string afterThisId;
		string aroundThisId;
	};
	
	class MessageManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class MessageManager;
		friend class InteractionManager;

		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestFetchMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetMessageData>* requestGetMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::FetchMessagesData>* requestGetMultMessagesBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>* requestDeleteMultMessagesBuffer;
		static unbounded_buffer<DiscordCoreInternal::PostMessageData>* requestPostMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::SendDMData>* requestPostDMMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::PatchMessageData>* requestPatchMessageBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* requestDeleteMessageBuffer;
		static unbounded_buffer<vector<Message>>* outMultMessagesBuffer;
		static unbounded_buffer<Message>* outMessageBuffer;
		static concurrent_queue<Message>* messagesToInsert;
		static overwrite_buffer<map<string, Message>> cache2;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient{ nullptr };

		MessageManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* coreClientNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = coreClientNew;
		}

		static void initialize() {
			MessageManagerAgent::requestGetMultMessagesBuffer = new unbounded_buffer<DiscordCoreInternal::FetchMessagesData>;
			MessageManagerAgent::requestFetchMessageBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestGetMessageBuffer = new unbounded_buffer<DiscordCoreInternal::GetMessageData>;
			MessageManagerAgent::requestPostMessageBuffer = new unbounded_buffer<DiscordCoreInternal::PostMessageData>;
			MessageManagerAgent::requestPostDMMessageBuffer = new unbounded_buffer<DiscordCoreInternal::SendDMData>;
			MessageManagerAgent::requestDeleteMultMessagesBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>;
			MessageManagerAgent::requestPatchMessageBuffer = new unbounded_buffer<DiscordCoreInternal::PatchMessageData>;
			MessageManagerAgent::requestDeleteMessageBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteMessageData>;
			MessageManagerAgent::messagesToInsert = new concurrent_queue<Message>;
			MessageManagerAgent::outMultMessagesBuffer = new unbounded_buffer<vector<Message>>;
			MessageManagerAgent::outMessageBuffer = new unbounded_buffer<Message>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		Message getObjectData(DiscordCoreInternal::GetMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::getObject() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObject() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObject() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			messageData.requesterId = dataPackage.requesterId;
			Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
		}

		vector<Message> getObjectData(DiscordCoreInternal::FetchMessagesData dataPackage) {
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::getObject() Error 02: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::getObject() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::getObject() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<Message> messagesVector;
			for (auto value : returnData.data) {
				MessageData messageData;
				DiscordCoreInternal::parseObject(value, &messageData);
				Message messageNew(messageData, this->discordCoreClient);
				messagesVector.push_back(messageNew);
			}
			return messagesVector;
		}

		Message patchObjectData(DiscordCoreInternal::PatchMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId;
			workload.content = dataPackage.finalContent;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::patchObject() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::patchObject() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::patchObject() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
		}

		Message postObjectData(DiscordCoreInternal::PostMessageData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.finalContent;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_MESSAGE;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::postObject() Error 01: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObject() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			messageData.requesterId = dataPackage.requesterId;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
		}

		Message postObjectData(DiscordCoreInternal::SendDMData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.content = dataPackage.finalContent;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::postObject() Error 03: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObject() Error 04: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			MessageData messageData;
			DiscordCoreInternal::parseObject(returnData.data, &messageData);
			Message messageNew(messageData, this->discordCoreClient);
			return messageNew;
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::onDelete() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::onDelete() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::onDelete() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DiscordCoreInternal::DeleteMessageData dataPackage) {
			if (dataPackage.timeDelay > 0) {
				DispatcherQueueTimer timer = this->threadContext->dispatcherQueue.get()->CreateTimer();
				timer.Interval(chrono::milliseconds(dataPackage.timeDelay));
				timer.Tick([this, dataPackage, timer](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
					onDeleteData(dataPackage);
					timer.Stop();
					});
				timer.Start();
			}
			else {
				onDeleteData(dataPackage);
			}
			return;
		}

		void postObject(DiscordCoreInternal::DeleteMessagesBulkData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_MESSAGE;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/bulk-delete";
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManagerAgent::postObjec() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "MessageManagerAgent::postObjec() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "MessageManagerAgent::postObjec() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void run() {
			try {
				DiscordCoreInternal::PostMessageData dataPackage01;
				if (try_receive(MessageManagerAgent::requestPostMessageBuffer, dataPackage01)) {
					Message message = this->postObjectData(dataPackage01);
					map<string, Message> cacheTemp;
					try_receive(MessageManagerAgent::cache2, cacheTemp);
					cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache2, cacheTemp);
				};
				DiscordCoreInternal::GetMessageData dataPackage02;
				if (try_receive(MessageManagerAgent::requestGetMessageBuffer, dataPackage02)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.channelId + dataPackage02.messageId)) {
							Message message = cacheTemp.at(dataPackage02.channelId + dataPackage02.messageId);
							send(MessageManagerAgent::outMessageBuffer, message);
						}
					}
				};
				DiscordCoreInternal::SendDMData dataPackage03;
				if (try_receive(MessageManagerAgent::requestPostDMMessageBuffer, dataPackage03)) {
					Message message = this->postObjectData(dataPackage03);
					map<string, Message> cacheTemp;
					try_receive(MessageManagerAgent::cache2, cacheTemp);
					cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache2, cacheTemp);
				};
				DiscordCoreInternal::GetMessageData dataPackage04;
				if (try_receive(MessageManagerAgent::requestFetchMessageBuffer, dataPackage04)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage04.channelId + dataPackage04.messageId)) {
							cacheTemp.erase(dataPackage04.channelId + dataPackage04.messageId);
						}
					}
					Message message = getObjectData(dataPackage04);
					cacheTemp.insert(make_pair(dataPackage04.channelId + dataPackage04.messageId, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache2, cacheTemp);
				}
				DiscordCoreInternal::PatchMessageData dataPackage05;
				if (try_receive(MessageManagerAgent::requestPatchMessageBuffer, dataPackage05)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage05.channelId + dataPackage05.messageId)) {
							cacheTemp.erase(dataPackage05.channelId + dataPackage05.messageId);
						}
					}
					Message message = patchObjectData(dataPackage05);
					cacheTemp.insert(make_pair(dataPackage05.channelId + dataPackage05.messageId, message));
					send(MessageManagerAgent::outMessageBuffer, message);
					asend(MessageManagerAgent::cache2, cacheTemp);
				}
				DiscordCoreInternal::FetchMessagesData dataPackage06;
				if (try_receive(MessageManagerAgent::requestGetMultMessagesBuffer, dataPackage06)) {
					vector<Message> messages = getObjectData(dataPackage06);
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache2, cacheTemp)) {
						for (auto value : messages) {
							if (cacheTemp.contains(value.data.channelId + value.data.id)) {
								cacheTemp.erase(value.data.channelId + value.data.id);
							}
							cacheTemp.insert(make_pair(value.data.channelId + value.data.id, value));
						}
					}
					send(MessageManagerAgent::outMultMessagesBuffer, messages);
					asend(MessageManagerAgent::cache2, cacheTemp);
				}
				DiscordCoreInternal::DeleteMessageData dataPackage07;
				if (try_receive(MessageManagerAgent::requestDeleteMessageBuffer, dataPackage07)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.contains(dataPackage07.channelId + dataPackage07.messageId)) {
							cacheTemp.erase(dataPackage07.channelId + dataPackage07.messageId);
						}
					}
					deleteObjectData(dataPackage07);
					asend(MessageManagerAgent::cache2, cacheTemp);
				}
				DiscordCoreInternal::DeleteMessagesBulkData dataPackage08;
				if (try_receive(MessageManagerAgent::requestDeleteMultMessagesBuffer, dataPackage08)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache2, cacheTemp)) {
						for (auto value : dataPackage08.messageIds) {
							if (cacheTemp.contains(dataPackage08.channelId + value)) {
								cacheTemp.erase(dataPackage08.channelId + value);
							}
						}
					}
					postObject(dataPackage08);
					asend(MessageManagerAgent::cache2, cacheTemp);
				}
				MessageData messageData;
				Message message(messageData, this->discordCoreClient);
				while (MessageManagerAgent::messagesToInsert->try_pop(message)) {
					map<string, Message> cacheTemp;
					if (try_receive(MessageManagerAgent::cache2, cacheTemp)) {
						if (cacheTemp.size() >= 1000) {
							cacheTemp.erase(cacheTemp.end());
						}
						if (cacheTemp.contains(message.data.channelId + message.data.id)) {
							cacheTemp.erase(message.data.channelId + message.data.id);
						}
					}
					cacheTemp.insert(make_pair(message.data.channelId + message.data.id, message));
					asend(MessageManagerAgent::cache2, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class MessageManager {
	public:

		task<Message> replyAsync(ReplyMessageData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PostMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			dataPackageNew.channelId = dataPackage.replyingToMessageData.channelId;
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			dataPackageNew.content = dataPackage.content;
			for (auto value : dataPackage.embeds) {
				dataPackageNew.embeds.push_back(value);
			}
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.nonce = dataPackage.nonce;
			dataPackageNew.replyingToMessageData = dataPackage.replyingToMessageData;
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.content = dataPackage.content;
			dataPackageNew.finalContent = DiscordCoreInternal::getReplyMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestPostMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::replyAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			messageNew.data.requesterId = dataPackage.requesterId;
			this->threadContext->releaseGroup(groupIdNew);
			co_return messageNew;
		}

		task<Message> sendDMAsync(SendDMData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::SendDMData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allowedMentions = dataPackage.messageData.allowedMentions;
			dataPackageNew.content = dataPackage.messageData.content;
			for (auto value : dataPackage.messageData.components) {
				dataPackageNew.components.push_back(value);
			}
			for (auto value : dataPackage.messageData.embeds) {
				dataPackageNew.embeds.push_back(value);
			}
			dataPackageNew.messageReference = dataPackage.messageData.messageReference;
			dataPackageNew.nonce = dataPackage.messageData.nonce;
			dataPackageNew.tts = dataPackage.messageData.tts;
			dataPackageNew.finalContent = DiscordCoreInternal::getCreateMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestPostDMMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::sendDMAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			this->threadContext->releaseGroup(groupIdNew);
			co_return messageNew;
		}

		task<Message> createMessageAsync(CreateMessageData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PostMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allowedMentions = dataPackage.allowedMentions;
			dataPackageNew.content = dataPackage.content;
			for (auto value : dataPackage.components) {
				dataPackageNew.components.push_back(value);
			}
			for (auto value : dataPackage.embeds) {
				dataPackageNew.embeds.push_back(value);
			}
			dataPackageNew.messageReference = dataPackage.messageReference;
			dataPackageNew.nonce = dataPackage.nonce;
			dataPackageNew.tts = dataPackage.tts;
			dataPackageNew.finalContent = DiscordCoreInternal::getCreateMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestPostMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::createMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			messageNew.data.requesterId = dataPackage.requesterId;
			this->threadContext->releaseGroup(groupIdNew);
			co_return messageNew;
		}

		task<Message> editMessageAsync(EditMessageData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
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
			dataPackageNew.finalContent = DiscordCoreInternal::getEditMessagePayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestPatchMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::editMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			messageNew.data.requesterId = dataPackage.requesterId;
			this->threadContext->releaseGroup(groupIdNew);
			co_return messageNew;
		}

		task<void> deleteMessageAsync(DeleteMessageData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.timeDelay = dataPackage.timeDelay;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestDeleteMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::deleteMessageAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> deleteMessasgeBulkAsync(DeleteMessagesBulkData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			vector<Message> messages = this->fetchMessagesAsync({ .channelId = dataPackage.channelId, .limit = dataPackage.limit, .beforeThisId = dataPackage.beforeThisId, .afterThisId = dataPackage.afterThisId, .aroundThisId = dataPackage.aroundThisId, }).get();
			vector<string> messageIds;
			for (auto value : messages) {
				if (hasTimeElapsed(value.data.timestamp, 14)) {
					continue;
				}
				if (dataPackage.deletePinned) {
					messageIds.push_back(value.data.id);
				}
				else {
					if (!value.data.pinned) {
						messageIds.push_back(value.data.id);
					}
				}
			}
			DiscordCoreInternal::DeleteMessagesBulkData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageIds = messageIds;
			dataPackageNew.content = DiscordCoreInternal::getDeleteMessagesBulkPayload(dataPackageNew);
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestDeleteMultMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::deleteMessagesBulkAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<Message> getMessageAsync(GetMessageData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.id;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestGetMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::getMessageAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			this->threadContext->releaseGroup(groupIdNew);
			co_return messageNew;
		}

		task<vector<Message>> fetchMessagesAsync(FetchMessagesData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchMessagesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.afterThisId = dataPackage.afterThisId;
			dataPackageNew.aroundThisId = dataPackage.aroundThisId;
			dataPackageNew.beforeThisId = dataPackage.beforeThisId;
			dataPackageNew.limit = dataPackage.limit;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestGetMultMessagesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::fetchMessagesAsync() Error: " << error.what() << endl << endl;
			}
			vector<Message> messageVector;
			try_receive(MessageManagerAgent::outMultMessagesBuffer, messageVector);
			this->threadContext->releaseGroup(groupIdNew);
			co_return messageVector;
		}

		task<Message> fetchAsync(FetchMessageData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetMessageData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.id;
			MessageManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(MessageManagerAgent::requestFetchMessageBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::fetchAsync() Error: " << error.what() << endl << endl;
			}
			MessageData messageData;
			Message messageNew(messageData, this->discordCoreClient);
			try_receive(MessageManagerAgent::outMessageBuffer, messageNew);
			this->threadContext->releaseGroup(groupIdNew);
			co_return messageNew;
		}

		task<void> insertMessageAsync(Message message) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			MessageManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			MessageManagerAgent::messagesToInsert->push(message);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "MessageManager::insertMessageAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeMessageAsync(string channelId, string messageId) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Message> cache2;
			try_receive(MessageManagerAgent::cache2, cache2);
			if (cache2.contains(channelId + messageId)) {
				cache2.erase(channelId + messageId);
			}
			asend(MessageManagerAgent::cache2, cache2);
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

	protected:
		friend class Channel;
		friend class Guild;
		friend class DiscordCoreClient;
		unsigned int groupId;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient{ nullptr };

		MessageManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
			this->groupId = this->threadContext->createGroup();
		}

		~MessageManager() {
			this->threadContext->releaseGroup(this->groupId);
		}
	};
	overwrite_buffer<map<string, Message>> MessageManagerAgent::cache2;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestFetchMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::PostMessageData>* MessageManagerAgent::requestPostMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::SendDMData>* MessageManagerAgent::requestPostDMMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessageData>* MessageManagerAgent::requestDeleteMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteMessagesBulkData>* MessageManagerAgent::requestDeleteMultMessagesBuffer;
	unbounded_buffer<DiscordCoreInternal::GetMessageData>* MessageManagerAgent::requestGetMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::PatchMessageData>* MessageManagerAgent::requestPatchMessageBuffer;
	unbounded_buffer<DiscordCoreInternal::FetchMessagesData>* MessageManagerAgent::requestGetMultMessagesBuffer;
	unbounded_buffer<vector<Message>>* MessageManagerAgent::outMultMessagesBuffer;
	unbounded_buffer<Message>* MessageManagerAgent::outMessageBuffer;
	concurrent_queue<Message>* MessageManagerAgent::messagesToInsert;
}
#endif