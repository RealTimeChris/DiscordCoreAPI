/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// InputEvents.cpp - Source file for the input-event handler class.
/// Aug 18, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file InputEvents.cpp

#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	CoRoutine<InputEventData> InputEvents::respondToInputEventAsync(RespondToInputEventData dataPackage) {
		co_await NewThreadAwaitable<InputEventData>();
		if (dataPackage.type == InputEventResponseType::Unset) {
			throw std::runtime_error("Please set an input-event-response-type!");
		}
		if (dataPackage.eventType == InteractionType::Message_Component) {
			CreateInteractionResponseData dataPackage02{ dataPackage };
			if (dataPackage.type == InputEventResponseType::Deferred_Response) {
				dataPackage02.data.type = InteractionCallbackType::Deferred_Update_Message;
				dataPackage02.data.data.flags = 64;
			} else {
				dataPackage02.data.type = InteractionCallbackType::Update_Message;
			}
			InputEventData newEvent = InputEventData(InputEvents::respondToInputEvent(dataPackage02));
			if (dataPackage.type == InputEventResponseType::Interaction_Response || dataPackage.type == InputEventResponseType::Ephemeral_Interaction_Response ||
				dataPackage.type == InputEventResponseType::Edit_Interaction_Response) {
				newEvent.responseType = InputEventResponseType::Edit_Interaction_Response;
			} else if (dataPackage.type == InputEventResponseType::Follow_Up_Message || dataPackage.type == InputEventResponseType::Ephemeral_Follow_Up_Message ||
				dataPackage.type == InputEventResponseType::Edit_Follow_Up_Message) {
				newEvent.responseType = InputEventResponseType::Edit_Follow_Up_Message;
			}
			co_return newEvent;
		}
		switch (dataPackage.type) {
			case InputEventResponseType::Ephemeral_Deferred_Response: {
				auto dataPackage02 = CreateDeferredInteractionResponseData{ dataPackage };
				dataPackage02.data.data.flags = 64;
				co_return InputEvents::respondToInputEvent(dataPackage02);
			}
			case InputEventResponseType::Deferred_Response: {
				[[fallthrough]];
			}
			case InputEventResponseType::Deferred_Response_With_Source: {
				co_return InputEvents::respondToInputEvent(CreateDeferredInteractionResponseData{ dataPackage });
			}
			case InputEventResponseType::Interaction_Response: {
				co_return InputEvents::respondToInputEvent(CreateInteractionResponseData{ dataPackage });
			}
			case InputEventResponseType::Edit_Interaction_Response: {
				co_return InputEvents::respondToInputEvent(EditInteractionResponseData{ dataPackage });
			}
			case InputEventResponseType::Ephemeral_Interaction_Response: {
				co_return InputEvents::respondToInputEvent(CreateEphemeralInteractionResponseData{ dataPackage });
			}
			case InputEventResponseType::Follow_Up_Message: {
				co_return InputEvents::respondToInputEvent(CreateFollowUpMessageData{ dataPackage });
			}
			case InputEventResponseType::Edit_Follow_Up_Message: {
				co_return InputEvents::respondToInputEvent(EditFollowUpMessageData{ dataPackage });
			}
			case InputEventResponseType::Ephemeral_Follow_Up_Message: {
				co_return InputEvents::respondToInputEvent(CreateEphemeralFollowUpMessageData{ dataPackage });
			}
			case InputEventResponseType::Unset: {
				std::cout << shiftToBrightRed() << "Failed to set input event response type!" << reset() << std::endl;
				break;
			}
			default: {
				co_return InputEventData();
			}
		}
		co_return InputEventData();
	}

	CoRoutine<void> InputEvents::deleteInputEventResponseAsync(InputEventData dataPackage, int32_t timeDelayNew) {
		co_await NewThreadAwaitable<void>();
		if (dataPackage.responseType == InputEventResponseType::Follow_Up_Message || dataPackage.responseType == InputEventResponseType::Edit_Follow_Up_Message) {
			RespondToInputEventData dataPackageNew{ dataPackage };
			DeleteFollowUpMessageData dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteFollowUpMessageAsync(dataPackageNewer).get();
		} else if (dataPackage.responseType == InputEventResponseType::Interaction_Response || dataPackage.responseType == InputEventResponseType::Edit_Interaction_Response ||
			dataPackage.responseType == InputEventResponseType::Deferred_Response) {
			RespondToInputEventData dataPackageNew{ dataPackage };
			DeleteInteractionResponseData dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteInteractionResponseAsync(dataPackageNewer).get();
		}
		co_return;
	}

	InputEventData InputEvents::respondToInputEvent(CreateDeferredInteractionResponseData dataPackage) {
		CreateInteractionResponseData dataPackage02{ dataPackage };
		Interactions::createInteractionResponseAsync(dataPackage02).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Deferred_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.interactionData->channelId = dataPackage.channelId;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(CreateInteractionResponseData dataPackage) {
		Message messageData = Interactions::createInteractionResponseAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->channelId = dataPackage.messagePackage.channelId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.responseType = InputEventResponseType::Interaction_Response;
		dataPackageNewer.interactionData->message.components = dataPackage.data.data.components;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		*dataPackageNewer.messageData = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(EditInteractionResponseData dataPackage) {
		Message messageData = Interactions::editInteractionResponseAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.responseType = InputEventResponseType::Edit_Interaction_Response;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		*dataPackageNewer.messageData = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(CreateFollowUpMessageData dataPackage) {
		Message messageData = Interactions::createFollowUpMessageAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		*dataPackageNewer.messageData = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(EditFollowUpMessageData dataPackage) {
		Message messageData = Interactions::editFollowUpMessageAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Edit_Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		*dataPackageNewer.messageData = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(CreateEphemeralInteractionResponseData dataPackage) {
		CreateInteractionResponseData newData{ dataPackage };
		Message newMessage = Interactions::createInteractionResponseAsync(newData).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Ephemeral_Interaction_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		*dataPackageNewer.messageData = newMessage;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(CreateEphemeralFollowUpMessageData dataPackage) {
		CreateFollowUpMessageData dataPackageNew{ dataPackage };
		Message messageData = Interactions::createFollowUpMessageAsync(dataPackageNew).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Ephemeral_Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		*dataPackageNewer.messageData = messageData;
		return dataPackageNewer;
	}
}
