// InputEventStuff.hpp - Header for the "input event" stuff.
// Aug 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INPUT_EVENT_STUFF_
#define _INPUT_EVENT_STUFF_

#include "../pch.h"
#include "MessageStuff.hpp"
#include "InteractionManager.hpp"
#include "GuildStuff01.hpp"

namespace DiscordCoreAPI {

	class InputEventStuff {
	public:
		InputEventStuff() {}

		static void initialize(shared_ptr<DiscordCoreClientBase> discordCoreClientBaseNew, shared_ptr<DiscordCoreClient> discordCoreClientNew, shared_ptr<MessageManager> messagesNew, shared_ptr<InteractionManager> interactionsNew) {
			InputEventStuff::messages = messagesNew;
			InputEventStuff::discordCoreClient = discordCoreClientNew;
			InputEventStuff::discordCoreClientBase = discordCoreClientBaseNew;
			InputEventStuff::interactions = interactionsNew;
		}

		static InputEventData respondToEvent(CreateFollowUpMessageData dataPackage) {
			MessageData messageData = InputEventStuff::interactions->createFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(EditFollowUpMessageData dataPackage) {
			MessageData messageData = InputEventStuff::interactions->editFollowUpMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateInteractionResponseData dataPackage) {
			auto messageData = InputEventStuff::interactions->createInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE;
			dataPackageNewer.interactionData.message.components = dataPackage.data.components;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateDeferredInteractionResponseData dataPackage) {
			InputEventStuff::interactions->createDeferredInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_DEFERRED;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.channelId = dataPackage.channelId;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.guildId = dataPackage.guildId;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(EditInteractionResponseData dataPackage) {
			MessageData messageData = InputEventStuff::interactions->editInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.messageData = messageData;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EDIT;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateEphemeralInteractionResponseData dataPackage) {
			CreateInteractionResponseData newData;
			newData.interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			newData.interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			newData.interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			newData.data = dataPackage.data;
			newData.data.flags = 64;
			newData.type = dataPackage.type;
			InputEventStuff::interactions->createInteractionResponseAsync(newData).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(ReplyMessageData dataPackage) {
			Message message = InputEventStuff::messages->replyAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(CreateMessageData dataPackage) {
			Message message = InputEventStuff::messages->createMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(EditMessageData dataPackage) {
			Message message = InputEventStuff::messages->editMessageAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_EDIT;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static InputEventData respondToEvent(SendDMData dataPackage) {
			Channel dmChannel = InputEventStuff::discordCoreClientBase->channels->fetchDMChannelAsync({ .userId = dataPackage.userId }).get();
			dataPackage.channelId = dmChannel.data.id;
			Message message = InputEventStuff::messages->sendDMAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.eventType = InputEventType::REGULAR_MESSAGE;
			dataPackageNewer.messageData = message.data;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
			dataPackageNewer.discordCoreClient = InputEventStuff::discordCoreClient;
			return dataPackageNewer;
		}

		static void respondToEvent(DeferComponentResponseData dataPackage) {
			CreateInteractionResponseData dataPackageNew;
			dataPackageNew.interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			dataPackageNew.interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNew.interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			dataPackageNew.type = dataPackage.type;
			InputEventStuff::interactions->createInteractionResponseAsync(dataPackageNew).get();
			return;
		}

		static task<void> deleteInputEventResponseAsync(InputEventData dataPackage, unsigned int timeDelayNew = 0) {
			apartment_context mainThread;
			co_await resume_background();
			if ((dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE) || (dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT)) {
				DeleteMessageData deleteData;
				deleteData.channelId = dataPackage.getChannelId();
				deleteData.messageId = dataPackage.getMessageId();
				deleteData.timeDelay = timeDelayNew;
				InputEventStuff::messages->deleteMessageAsync(deleteData).get();
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
				DeleteFollowUpMessageData dataPackageNewer(dataPackage);
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEventStuff::interactions->deleteFollowUpMessageAsync(dataPackageNewer).get();
			}
			else if (dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE || dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT
				|| dataPackage.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED) {
				DeleteInteractionResponseData dataPackageNewer(dataPackage);
				dataPackageNewer.timeDelay = timeDelayNew;
				InputEventStuff::interactions->deleteInteractionResponseAsync(dataPackageNewer).get();
			}
			co_await mainThread;
			co_return;
		}

	protected:
		static shared_ptr<MessageManager> messages;
		static shared_ptr<InteractionManager> interactions;
		static shared_ptr<DiscordCoreClientBase> discordCoreClientBase;
		static shared_ptr<DiscordCoreClient> discordCoreClient;
	};
	shared_ptr<MessageManager> InputEventStuff::messages;
	shared_ptr<InteractionManager> InputEventStuff::interactions;
	shared_ptr<DiscordCoreClientBase> InputEventStuff::discordCoreClientBase;
	shared_ptr< DiscordCoreClient> InputEventStuff::discordCoreClient;
}

#endif