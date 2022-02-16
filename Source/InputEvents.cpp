// InputEvents.cpp - Source file for the input-event handler class DiscordCoreAPI_Dll.
// Aug 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#include <InputEvents.hpp>
#include <InteractionEntities.hpp>
#include <ChannelEntities.hpp>
#include <CoRoutine.hpp>

namespace DiscordCoreAPI {

	InputEventData InputEvents::respondToEvent(RespondToInputEventData dataPackage) {
		if (dataPackage.eventType == InteractionType::Message_Component){
			std::unique_ptr<CreateInteractionResponseData> dataPackage02 = std::make_unique<CreateInteractionResponseData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		switch (dataPackage.type) {
		case InputEventResponseType::Deferred_Response: {
			std::unique_ptr<CreateDeferredInteractionResponseData>dataPackage02 = std::make_unique<CreateDeferredInteractionResponseData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Interaction_Response: {
			std::unique_ptr<CreateInteractionResponseData> dataPackage02 = std::make_unique<CreateInteractionResponseData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Interaction_Response_Edit: {
			std::unique_ptr<EditInteractionResponseData> dataPackage02 = std::make_unique<EditInteractionResponseData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Ephemeral_Interaction_Response: {
			std::unique_ptr<CreateEphemeralInteractionResponseData> dataPackage02 = std::make_unique<CreateEphemeralInteractionResponseData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Regular_Message: {
			std::unique_ptr<CreateMessageData> dataPackage02 = std::make_unique<CreateMessageData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Regular_Message_Edit: {
			std::unique_ptr<EditMessageData> dataPackage02 = std::make_unique<EditMessageData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Follow_Up_Message: {
			std::unique_ptr<CreateFollowUpMessageData> dataPackage02 = std::make_unique<CreateFollowUpMessageData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Follow_Up_Message_Edit: {
			std::unique_ptr<EditFollowUpMessageData> dataPackage02 = std::make_unique<EditFollowUpMessageData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Ephemeral_Follow_Up_Message: {
			std::unique_ptr<CreateEphemeralFollowUpMessageData> dataPackage02 = std::make_unique<CreateEphemeralFollowUpMessageData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Send_Dm: {
			std::unique_ptr<SendDMData> dataPackage02 = std::make_unique<SendDMData>(dataPackage);
			return InputEvents::respondToEvent(*dataPackage02);
		}
		case InputEventResponseType::Unset: {
			std::cout << "Failed to set input event response type!" << std::endl;
			break;
		}
		default: {
			return InputEventData();
		}
		}
		return InputEventData();
	}

	CoRoutine<void> InputEvents::deleteInputEventResponseAsync(InputEventData dataPackage,  int32_t timeDelayNew) {
		co_await NewThreadAwaitable<void>();
		if (dataPackage.eventType == InputEventType::Regular_Message || dataPackage.responseType == InputEventResponseType::Regular_Message || dataPackage.responseType == InputEventResponseType::Regular_Message_Edit) {
			std::unique_ptr<DeleteMessageData> deleteData = std::make_unique<DeleteMessageData>();
			deleteData->messageData = dataPackage.getMessageData();
			deleteData->timeDelay = timeDelayNew;
			Messages::deleteMessageAsync(*deleteData).get();
		}
		else if (dataPackage.responseType == InputEventResponseType::Follow_Up_Message || dataPackage.responseType == InputEventResponseType::Follow_Up_Message_Edit || dataPackage.responseType == InputEventResponseType::Ephemeral_Follow_Up_Message) {
			std::unique_ptr<DeleteFollowUpMessageData> dataPackageNewer = std::make_unique<DeleteFollowUpMessageData>(dataPackage);
			dataPackageNewer->timeDelay = timeDelayNew;
			Interactions::deleteFollowUpMessageAsync(*dataPackageNewer).get();
		}
		else if (dataPackage.responseType == InputEventResponseType::Interaction_Response || dataPackage.responseType == InputEventResponseType::Interaction_Response_Edit || dataPackage.responseType == InputEventResponseType::Ephemeral_Interaction_Response
			|| dataPackage.responseType == InputEventResponseType::Deferred_Response) {
			std::unique_ptr<DeleteInteractionResponseData> dataPackageNewer = std::make_unique<DeleteInteractionResponseData>(dataPackage);
			dataPackageNewer->timeDelay = timeDelayNew;
			Interactions::deleteInteractionResponseAsync(*dataPackageNewer).get();
		}
		co_return;
	}

	InputEventData InputEvents::respondToEvent(CreateDeferredInteractionResponseData dataPackage) {
		Interactions::createDeferredInteractionResponseAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Deferred_Response;
		dataPackageNewer->interactionData.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer->interactionData.token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer->interactionData.id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer->eventType = InputEventType::Application_Command_Interaction;
		dataPackageNewer->interactionData.channelId = dataPackage.channelId;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(CreateInteractionResponseData dataPackage) {
		Message messageData = Interactions::createInteractionResponseAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->interactionData.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer->interactionData.channelId = dataPackage.messagePackage.channelId;
		dataPackageNewer->interactionData.token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer->responseType = InputEventResponseType::Interaction_Response;
		dataPackageNewer->interactionData.message.components = dataPackage.data.data.components;
		dataPackageNewer->interactionData.id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer->eventType = InputEventType::Application_Command_Interaction;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		dataPackageNewer->messageData = messageData;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(EditInteractionResponseData dataPackage) {
		Message messageData = Interactions::editInteractionResponseAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->interactionData.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer->responseType = InputEventResponseType::Interaction_Response_Edit;
		dataPackageNewer->interactionData.token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer->interactionData.id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer->eventType = InputEventType::Application_Command_Interaction;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		dataPackageNewer->messageData = messageData;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(CreateFollowUpMessageData dataPackage) {
		Message messageData = Interactions::createFollowUpMessageAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Follow_Up_Message;
		dataPackageNewer->interactionData.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer->interactionData.token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer->interactionData.id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer->eventType = InputEventType::Application_Command_Interaction;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		dataPackageNewer->messageData = messageData;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(EditFollowUpMessageData dataPackage) {
		Message messageData = Interactions::editFollowUpMessageAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Follow_Up_Message_Edit;
		dataPackageNewer->interactionData.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer->interactionData.token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer->interactionData.id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer->eventType = InputEventType::Application_Command_Interaction;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		dataPackageNewer->messageData = messageData;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(CreateEphemeralInteractionResponseData dataPackage) {
		std::unique_ptr<CreateInteractionResponseData> newData = std::make_unique<CreateInteractionResponseData>(dataPackage);
		Message newMessage = Interactions::createInteractionResponseAsync(*newData).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Ephemeral_Interaction_Response;
		dataPackageNewer->interactionData.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer->interactionData.token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer->interactionData.id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer->eventType = InputEventType::Application_Command_Interaction;
		dataPackageNewer->messageData = newMessage;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(CreateEphemeralFollowUpMessageData dataPackage) {
		std::unique_ptr<CreateFollowUpMessageData> dataPackageNew = std::make_unique<CreateFollowUpMessageData>(dataPackage);
		Message messageData = Interactions::createFollowUpMessageAsync(*dataPackageNew).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Ephemeral_Follow_Up_Message;
		dataPackageNewer->interactionData.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer->interactionData.token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer->interactionData.id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer->eventType = InputEventType::Application_Command_Interaction;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		dataPackageNewer->messageData = messageData;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(SendDMData dataPackage) {
		auto channel = Channels::createDMChannelAsync({ .userId = dataPackage.targetUserId }).get();
		dataPackage.channelId = channel.id;
		Message message = Messages::createMessageAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Regular_Message;
		dataPackageNewer->eventType = InputEventType::Regular_Message;
		dataPackageNewer->messageData = message;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		return *dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(CreateMessageData dataPackage) {
		Message message = Messages::createMessageAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Regular_Message;
		dataPackageNewer->eventType = InputEventType::Regular_Message;
		dataPackageNewer->messageData = message;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		return *dataPackageNewer;
	}
	
	InputEventData InputEvents::respondToEvent(EditMessageData dataPackage) {
		Message message = Messages::editMessageAsync(dataPackage).get();
		std::unique_ptr<InputEventData> dataPackageNewer = std::make_unique<InputEventData>();
		dataPackageNewer->responseType = InputEventResponseType::Regular_Message_Edit;
		dataPackageNewer->eventType = InputEventType::Regular_Message;
		dataPackageNewer->requesterId = dataPackage.requesterId;
		dataPackageNewer->messageData = message;
		return *dataPackageNewer;
	}

	void InputEvents::respondToEvent(DeferComponentResponseData dataPackage) {
		std::unique_ptr<CreateInteractionResponseData> dataPackageNew = std::make_unique<CreateInteractionResponseData>(dataPackage);
		Interactions::createInteractionResponseAsync(*dataPackageNew).get();
	}
}
