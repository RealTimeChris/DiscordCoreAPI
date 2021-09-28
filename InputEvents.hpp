// InputEvents.hpp - Header for the input event handler class.
// Aug 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INPUT_EVENTS_
#define _INPUT_EVENTS_

#include "../pch.h"
#include "MessageEntities.hpp"
#include "InteractionEntities.hpp"
#include "GuildEntities.hpp"

namespace DiscordCoreAPI {

	class InputEvents {
	public:

		static void initialize(shared_ptr<DiscordCoreInternal::MessageManager> messagesNew, shared_ptr<DiscordCoreInternal::InteractionManager> interactionsNew) {
			InputEvents::interactions = interactionsNew;
			InputEvents::messages = messagesNew;
		}

		static InputEventData respondToEvent(RespondToInputEventData dataPackage) {
			switch (dataPackage.type) {
			case DesiredInputEventResponseType::DeferredResponse: {
				CreateDeferredInteractionResponseData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::InteractionResponse: {
				CreateInteractionResponseData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::InteractionResponseEdit: {
				EditInteractionResponseData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::EphemeralInteractionResponse: {
				CreateEphemeralInteractionResponseData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::RegularMessage: {
				ReplyMessageData dataPackage02{};
				dataPackage02.messageReference.channelId = dataPackage.channelId;
				dataPackage02.messageReference.messageId = dataPackage.messageId;
				dataPackage02.addAllowedMentions(dataPackage.allowedMentions);
				dataPackage02.requesterId = dataPackage.requesterId;
				for (auto value : dataPackage.components) {
					dataPackage02.components.push_back(value);
				}
				dataPackage02.addContent(dataPackage.content);
				for (auto value : dataPackage.embeds) {
					dataPackage02.embeds.push_back(value);
				}
				dataPackage02.tts = dataPackage.tts;
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::RegularMessageEdit: {
				EditMessageData dataPackage02{};
				dataPackage02.channelId = dataPackage.channelId;
				dataPackage02.messageId = dataPackage.messageId;
				dataPackage02.addAllowedMentions(dataPackage.allowedMentions);
				dataPackage02.requesterId = dataPackage.requesterId;
				for (auto value : dataPackage.components) {
					dataPackage02.components.push_back(value);
				}
				dataPackage02.addContent(dataPackage.content);
				for (auto value : dataPackage.embeds) {
					dataPackage02.embeds.push_back(value);
				}
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::FollowUpMessage: {
				CreateFollowUpMessageData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::FollowUpMessageEdit: {
				EditFollowUpMessageData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::EphemeralFollowUpMessage: {
				CreateEphemeralFollowUpMessageData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			case DesiredInputEventResponseType::SendDM: {
				SendDMData dataPackage02{ dataPackage };
				return respondToEvent(dataPackage02);
			}
			default: {
				return InputEventData();
			}

			}
		}

		static task<void> deleteInputEventResponseAsync(InputEventData dataPackage, unsigned int timeDelayNew = 0) {
			apartment_context mainThread;
			co_await resume_background();
			if ((dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE) || (dataPackage.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT)) {
				DeleteMessageData deleteData;
				deleteData.messageData = dataPackage.getMessageData();
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

		InputEvents() {};

		static InputEventData respondToEvent(CreateDeferredInteractionResponseData dataPackage) {
			InputEvents::interactions->createDeferredInteractionResponseAsync(dataPackage).get();
			InputEventData dataPackageNewer;
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_RESPONSE_DEFERRED;
			dataPackageNewer.interactionData.applicationId = dataPackage.interactionPackage.applicationId;
			dataPackageNewer.interactionData.token = dataPackage.interactionPackage.interactionToken;
			dataPackageNewer.interactionData.id = dataPackage.interactionPackage.interactionId;
			dataPackageNewer.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
			dataPackageNewer.discordCoreClient = InputEvents::discordCoreClient;
			dataPackageNewer.interactionData.channelId = dataPackage.channelId;
			dataPackageNewer.requesterId = dataPackage.requesterId;
			return dataPackageNewer;
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
			dataPackageNewer.inputEventResponseType = InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EPHEMERAL;
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
		}

		static shared_ptr<DiscordCoreInternal::InteractionManager> interactions;
		static shared_ptr<DiscordCoreInternal::MessageManager> messages;
		static DiscordCoreClientBase* discordCoreClientBase;
		static DiscordCoreClient* discordCoreClient;

	};
	shared_ptr<DiscordCoreInternal::InteractionManager> InputEvents::interactions{ nullptr };
	shared_ptr<DiscordCoreInternal::MessageManager> InputEvents::messages{ nullptr };
	DiscordCoreClientBase* InputEvents::discordCoreClientBase{ nullptr };
	DiscordCoreClient* InputEvents::discordCoreClient{ nullptr };
}

#endif