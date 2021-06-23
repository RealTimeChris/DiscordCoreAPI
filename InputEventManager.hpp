// InputEventManager.hpp - Header for the "input event handlermanager" class.
// June 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INPUT_EVENT_HANDLER_
#define _INPUT_EVENT_HANDLER_

#include "../pch.h"
#include "MessageStuff.hpp"
#include "InteractionManager.hpp"

namespace DiscordCoreAPI {

	class InputEventManager {
	public:
		InputEventManager() {}

		static void initialize(MessageManager* messagesNew, DiscordCoreClient* discordCoreClientNew, DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::ThreadContext threadContextNew) {
			InputEventManager::messages = messagesNew;
			InputEventManager::agentResources = agentResourcesNew;
			InputEventManager::discordCoreClient = discordCoreClientNew;
			InputEventManager::threadContext = threadContextNew;
		}

		static task<InputEventData> respondToEvent(CreateFollowUpMessageData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			MessageData messageData = InputEventManager::interactions->createFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(EditFollowUpMessageData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			MessageData messageData = InputEventManager::interactions->editFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(CreateInteractionResponseData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			InputEventManager::interactions->createInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE;
			dataPackageNewer.interactionData.message.components = dataPackage.data.components;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(CreateDeferredInteractionResponseData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			InputEventManager::interactions->createDeferredInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_DEFERRED;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(EditInteractionResponseData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			MessageData messageData = InputEventManager::interactions->editInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(CreateEphemeralInteractionResponseData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			InteractionPackageData interactionPackage;
			interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			CreateInteractionResponseData newData(interactionPackage);
			newData.data = dataPackage.data;
			newData.data.flags = 64;
			newData.type = dataPackage.type;
			InputEventManager::interactions->createInteractionResponseAsync(newData).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(ReplyMessageData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			Message message = InputEventManager::messages->replyAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<InputEventData> respondToEvent(EditMessageData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			Message message = InputEventManager::messages->editMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventManager::discordCoreClient;
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return dataPackageNewer;
		}

		static task<void> respondToEvent(DeferButtonResponseData dataPackage) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			ButtonInteractionData newData;
			newData.token = dataPackage.interactionPackage.interactionToken;
			newData.id = dataPackage.interactionPackage.interactionId;
			newData.applicationId = dataPackage.interactionPackage.applicationId;
			newData.type = InteractionType::MessageComponent;
			CreateInteractionResponseData dataPackageNew(newData);
			dataPackageNew.interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			dataPackageNew.interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNew.interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			dataPackageNew.type = InteractionCallbackType::DeferredUpdateMessage;
			InputEventManager::interactions->createInteractionResponseAsync(dataPackageNew).get();
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return;
		}

		static task<void> deleteInputEventResponse(InputEventData dataPackage, unsigned int timeDelayNew = 0) {
			unsigned int groupId = InputEventManager::threadContext.createGroup();
			co_await resume_foreground(*InputEventManager::threadContext.dispatcherQueue.get());
			if ((dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE)|| (dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT)) {
				DeleteMessageData deleteData(dataPackage);
				deleteData.timeDelay = timeDelayNew;
				InputEventManager::messages->deleteMessageAsync(deleteData).get();
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT){
				DeleteFollowUpMessageData dataPackageNewer(dataPackage);
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEventManager::interactions->deleteFollowUpMessageAsync(dataPackageNewer).get();
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE|| dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT ||
				dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL|| dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED){
				DeleteInteractionResponseData dataPackageNewer(dataPackage);
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEventManager::interactions->deleteInteractionResponseAsync(dataPackageNewer).get();
			}
			InputEventManager::threadContext.releaseGroup(groupId);
			co_return;
		}

	protected:
		static MessageManager* messages;
		static InteractionManager* interactions;
		static DiscordCoreClient* discordCoreClient;
		static DiscordCoreInternal::HttpAgentResources agentResources;
		static DiscordCoreInternal::ThreadContext threadContext;
	};
	MessageManager* InputEventManager::messages;
	InteractionManager* InputEventManager::interactions;
	DiscordCoreClient* InputEventManager::discordCoreClient;
	DiscordCoreInternal::HttpAgentResources InputEventManager::agentResources;
	DiscordCoreInternal::ThreadContext InputEventManager::threadContext;


}
#endif
