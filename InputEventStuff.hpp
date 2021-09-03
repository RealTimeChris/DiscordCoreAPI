// InputEventStuff.hpp - Header for the "input event" stuff.
// Aug 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INPUT_EVENT_STUFF_
#define _INPUT_EVENT_STUFF_

#include "../pch.h"
#include "MessageStuff.hpp"
#include "InteractionStuff.hpp"
#include "GuildStuff.hpp"

namespace DiscordCoreAPI {

	class InputEvents {
	public:

		static void initialize(shared_ptr<DiscordCoreClientBase> discordCoreClientBaseNew, shared_ptr<DiscordCoreClient> discordCoreClientNew, shared_ptr<DiscordCoreInternal::MessageManager> messagesNew, shared_ptr<DiscordCoreInternal::InteractionManager> interactionsNew) {
			InputEvents::discordCoreClientBase = discordCoreClientBaseNew;
			InputEvents::discordCoreClient = discordCoreClientNew;
			InputEvents::interactions = interactionsNew;
			InputEvents::messages = messagesNew;
		}

		static InputEventData respondToEvent(CreateFollowUpMessageData dataPackage) {
			MessageData messageData = InputEvents::interactions->createFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.messageData = messageData;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateEphemeralFollowUpMessageData dataPackage) {
			CreateFollowUpMessageData dataPackageNew;
			dataPackageNew.interactionPackage = dataPackage.interactionPackage;
			dataPackageNew.requesterId = dataPackage.requesterId;
			dataPackageNew.data = dataPackage.data;
			MessageData messageData = InputEvents::interactions->createFollowUpMessageAsync(dataPackageNew).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.messageData = messageData;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(EditFollowUpMessageData dataPackage) {
			MessageData messageData = InputEvents::interactions->editFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.messageData = messageData;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateInteractionResponseData dataPackage) {
			auto messageData = InputEvents::interactions->createInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE;
			dataPackageNewer.interactionData.message.components = dataPackage.data.data.components;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.messageData = messageData;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateDeferredInteractionResponseData dataPackage) {
			InputEvents::interactions->createDeferredInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_DEFERRED;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(EditInteractionResponseData dataPackage) {
			MessageData messageData = InputEvents::interactions->editInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.messageData = messageData;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateEphemeralInteractionResponseData dataPackage) {
			CreateInteractionResponseData newData;
			newData.interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			newData.interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			newData.interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			newData.data.data.components = dataPackage.data.data.components;
			newData.data.type = dataPackage.data.type;
			newData.data = dataPackage.data;
			newData.data.data.flags = 64;
			InputEvents::interactions->createInteractionResponseAsync(newData).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(ReplyMessageData dataPackage) {
			Message message = InputEvents::messages->replyAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.messageData = message;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateMessageData dataPackage) {
			Message message = InputEvents::messages->createMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.messageData = message;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(EditMessageData dataPackage) {
			Message message = InputEvents::messages->editMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.messageData = message;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(SendDMData dataPackage) {
			Channel dmChannel = InputEvents::discordCoreClientBase->channels->fetchDMChannelAsync({ .userId = dataPackage.userId }).get();
			dataPackage.channelId = dmChannel.id;
			Message message = InputEvents::messages->sendDMAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.messageData = message;
			return dataPackageNewer;
		}

		static void respondToEvent(DeferComponentResponseData dataPackage) {
			CreateInteractionResponseData dataPackageNew;
			dataPackageNew.interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			dataPackageNew.interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			dataPackageNew.interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNew.data.type = dataPackage.type;
			InputEvents::interactions->createInteractionResponseAsync(dataPackageNew).get();
			return;
		}

		static task<void> deleteInputEventResponseAsync(DeleteMessageData dataPackage, unsigned int timeDelayNew = 0) {
			apartment_context mainThread;
			co_await resume_background();
			DeleteMessageData deleteData;
			deleteData.channelId = dataPackage.channelId;
			deleteData.messageId = dataPackage.messageId;
			deleteData.timeDelay = dataPackage.timeDelay;
			InputEvents::messages->deleteMessageAsync(deleteData).get();
			co_await mainThread;
			co_return;
		}

		static task<void> deleteInputEventResponseAsync(InputEventData dataPackage, unsigned int timeDelayNew = 0) {
			apartment_context mainThread;
			co_await resume_background();
			if ((dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE) || (dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT)) {
				DeleteMessageData deleteData;
				deleteData.channelId = dataPackage.getChannelId();
				deleteData.messageId = dataPackage.getMessageId();
				deleteData.timeDelay = timeDelayNew;
				InputEvents::messages->deleteMessageAsync(deleteData).get();
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
				DeleteFollowUpMessageData dataPackageNewer(dataPackage);
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEvents::interactions->deleteFollowUpMessageAsync(dataPackageNewer).get();
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE || dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT
				|| dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED) {
				DeleteInteractionResponseData dataPackageNewer(dataPackage);
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEvents::interactions->deleteInteractionResponseAsync(dataPackageNewer).get();
			}
			co_await mainThread;
			co_return;
		}

	protected:
		inline static shared_ptr<DiscordCoreInternal::InteractionManager> interactions{ nullptr };
		inline static shared_ptr<DiscordCoreInternal::MessageManager> messages{ nullptr };
		static shared_ptr<DiscordCoreClientBase> discordCoreClientBase;
		static shared_ptr<DiscordCoreClient> discordCoreClient;

		InputEvents() {}
	};
	shared_ptr<DiscordCoreClientBase> InputEvents::discordCoreClientBase{ nullptr };
	shared_ptr<DiscordCoreClient> InputEvents::discordCoreClient{ nullptr };
}

#endif