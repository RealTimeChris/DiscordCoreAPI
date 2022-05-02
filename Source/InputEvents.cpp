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

	InputEventData InputEvents::respondToEvent(RespondToInputEventData dataPackage) {
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
			InputEventData newEvent = InputEventData(InputEvents::respondToEvent(dataPackage02));
			if (dataPackage.type == InputEventResponseType::Ephemeral_Interaction_Response ||
				dataPackage.type == InputEventResponseType::Edit_Ephemeral_Interaction_Response) {
				newEvent.responseType = InputEventResponseType::Edit_Ephemeral_Interaction_Response;
			} else if (dataPackage.type == InputEventResponseType::Ephemeral_Follow_Up_Message ||
				dataPackage.type == InputEventResponseType::Edit_Ephemeral_Follow_Up_Message) {
				newEvent.responseType = InputEventResponseType::Edit_Ephemeral_Follow_Up_Message;
			} else if (dataPackage.type == InputEventResponseType::Interaction_Response) {
				newEvent.responseType = InputEventResponseType::Edit_Interaction_Response;
			} else if (dataPackage.type == InputEventResponseType::Follow_Up_Message) {
				newEvent.responseType = InputEventResponseType::Edit_Follow_Up_Message;
			}
			return newEvent;
		}
		switch (dataPackage.type) {
			case InputEventResponseType::Deferred_Response: {
				[[fallthrough]];
			}
			case InputEventResponseType::Deferred_Response_With_Source: {
				auto dataPackage02 = CreateDeferredInteractionResponseData{ dataPackage };
				dataPackage02.data.data.flags = 64;
				return InputEvents::respondToEvent(dataPackage02);
			}
			case InputEventResponseType::Interaction_Response: {
				return InputEvents::respondToEvent(CreateInteractionResponseData{ dataPackage });
			}
			case InputEventResponseType::Edit_Ephemeral_Interaction_Response: {
				return InputEvents::respondToEvent(EditEphemeralInteractionResponseData{ dataPackage });
			}
			case InputEventResponseType::Edit_Interaction_Response: {
				return InputEvents::respondToEvent(EditInteractionResponseData{ dataPackage });
			}
			case InputEventResponseType::Ephemeral_Interaction_Response: {
				CreateEphemeralInteractionResponseData dataPackage02{ dataPackage };
				return InputEventData(InputEvents::respondToEvent(dataPackage02));
			}
			case InputEventResponseType::Follow_Up_Message: {
				CreateFollowUpMessageData dataPackage02{ dataPackage };
				return InputEventData(InputEvents::respondToEvent(dataPackage02));
			}
			case InputEventResponseType::Edit_Ephemeral_Follow_Up_Message: {
				EditEphemeralFollowUpMessageData dataPackage02{ dataPackage };
				return InputEventData(InputEvents::respondToEvent(dataPackage02));
			}
			case InputEventResponseType::Edit_Follow_Up_Message: {
				EditFollowUpMessageData dataPackage02{ dataPackage };
				return InputEventData(InputEvents::respondToEvent(dataPackage02));
			}
			case InputEventResponseType::Ephemeral_Follow_Up_Message: {
				CreateEphemeralFollowUpMessageData dataPackage02{ dataPackage };
				return InputEventData(InputEvents::respondToEvent(dataPackage02));
			}
			case InputEventResponseType::Unset: {
				std::cout << shiftToBrightRed() << "Failed to set input event response type!" << reset() << std::endl;
				break;
			}
			default: {
				return InputEventData();
			}
		}
		return InputEventData();
	}

	CoRoutine<void> InputEvents::deleteInputEventResponseAsync(InputEventData dataPackage, int32_t timeDelayNew) {
		co_await NewThreadAwaitable<void>();
		if (dataPackage.responseType == InputEventResponseType::Follow_Up_Message ||
			dataPackage.responseType == InputEventResponseType::Edit_Follow_Up_Message) {
			DeleteFollowUpMessageData dataPackageNewer{ dataPackage };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteFollowUpMessageAsync(dataPackageNewer).get();
		} else if (dataPackage.responseType == InputEventResponseType::Interaction_Response ||
			dataPackage.responseType == InputEventResponseType::Edit_Interaction_Response ||
			dataPackage.responseType == InputEventResponseType::Deferred_Response) {
			DeleteInteractionResponseData dataPackageNewer{ dataPackage };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteInteractionResponseAsync(dataPackageNewer).get();
		}
		co_return;
	}

	InputEventData InputEvents::respondToEvent(CreateDeferredInteractionResponseData dataPackage) {
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

	InputEventData InputEvents::respondToEvent(CreateInteractionResponseData dataPackage) {
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

	InputEventData InputEvents::respondToEvent(EditInteractionResponseData dataPackage) {
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

	InputEventData InputEvents::respondToEvent(EditEphemeralInteractionResponseData dataPackage) {
		Message messageData = Interactions::editInteractionResponseAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.responseType = InputEventResponseType::Edit_Ephemeral_Interaction_Response;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		*dataPackageNewer.messageData = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(EditEphemeralFollowUpMessageData dataPackage) {
		Message messageData = Interactions::editFollowUpMessageAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.responseType = InputEventResponseType::Edit_Ephemeral_Follow_Up_Message;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.eventType = InteractionType::Application_Command;
		dataPackageNewer.requesterId = dataPackage.requesterId;
		*dataPackageNewer.messageData = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToEvent(CreateFollowUpMessageData dataPackage) {
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

	InputEventData InputEvents::respondToEvent(EditFollowUpMessageData dataPackage) {
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

	InputEventData InputEvents::respondToEvent(CreateEphemeralInteractionResponseData dataPackage) {
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

	InputEventData InputEvents::respondToEvent(CreateEphemeralFollowUpMessageData dataPackage) {
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
