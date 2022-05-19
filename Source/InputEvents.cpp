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

	CoRoutine<InputEventData> InputEvents::respondToInputEventAsync(const RespondToInputEventData& dataPackage) {
		co_await NewThreadAwaitable<InputEventData>();
		if (dataPackage.type == InputEventResponseType::Unset) {
			throw std::runtime_error("Please set an input-event-response-type!");
		}
		if (dataPackage.eventType == InteractionType::Message_Component) {
			CreateInteractionResponseData dataPackage02{ dataPackage };
			if (dataPackage.type == InputEventResponseType::Deferred_Response) {
				dataPackage02.data.type = InteractionCallbackType::Deferred_Update_Message;
			} else {
				dataPackage02.data.type = InteractionCallbackType::Update_Message;
			}
			InputEventData newEvent = InputEvents::respondToInputEvent(dataPackage02);
			if (dataPackage.type == InputEventResponseType::Interaction_Response || dataPackage.type == InputEventResponseType::Ephemeral_Interaction_Response ||
				dataPackage.type == InputEventResponseType::Edit_Interaction_Response) {
				newEvent.responseType = InputEventResponseType::Edit_Interaction_Response;
			} else if (dataPackage.type == InputEventResponseType::Follow_Up_Message || dataPackage.type == InputEventResponseType::Ephemeral_Follow_Up_Message ||
				dataPackage.type == InputEventResponseType::Edit_Follow_Up_Message) {
				newEvent.responseType = InputEventResponseType::Edit_Follow_Up_Message;
			}
			co_return newEvent;
		} else if (dataPackage.eventType == InteractionType::Application_Command_Autocomplete) {
			CreateInteractionResponseData dataPackage02{ dataPackage };
			dataPackage02.data.type = InteractionCallbackType::Application_Command_Autocomplete_Result;
			InputEventData newEvent = InputEvents::respondToInputEvent(dataPackage02);
			newEvent.responseType = InputEventResponseType::Application_Command_AutoComplete_Result;
			co_return newEvent;
		}
		switch (dataPackage.type) {
			case InputEventResponseType::Ephemeral_Deferred_Response: {
				auto dataPackage02 = CreateDeferredInteractionResponseData{ dataPackage };
				dataPackage02.data.data.flags = 64;
				co_return InputEvents::respondToInputEvent(dataPackage02);
			}
			case InputEventResponseType::Deferred_Response: {
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
				break;
			}
			default: {
				co_return InputEventData();
			}
		}
		co_return InputEventData();
	}

	CoRoutine<void> InputEvents::deleteInputEventResponseAsync(const InputEventData& dataPackage, int32_t timeDelayNew) {
		InputEventData newPackage = dataPackage;
		co_await NewThreadAwaitable<void>();
		if (newPackage.responseType == InputEventResponseType::Follow_Up_Message || newPackage.responseType == InputEventResponseType::Edit_Follow_Up_Message) {
			RespondToInputEventData dataPackageNew{ newPackage };
			DeleteFollowUpMessageData dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteFollowUpMessageAsync(dataPackageNewer).get();
		} else if (newPackage.responseType == InputEventResponseType::Interaction_Response || newPackage.responseType == InputEventResponseType::Edit_Interaction_Response ||
			newPackage.responseType == InputEventResponseType::Deferred_Response) {
			RespondToInputEventData dataPackageNew{ newPackage };
			DeleteInteractionResponseData dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteInteractionResponseAsync(dataPackageNewer).get();
		}
		co_return;
	}

	InputEventData InputEvents::respondToInputEvent(const CreateDeferredInteractionResponseData& dataPackage) {
		CreateInteractionResponseData dataPackageNew{ dataPackage };
		Interactions::createInteractionResponseAsync(dataPackageNew).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Deferred_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(const CreateInteractionResponseData& dataPackage) {
		Message messageData = Interactions::createInteractionResponseAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Interaction_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId = messageData.channelId;
		dataPackageNewer.interactionData->user = messageData.author;
		dataPackageNewer.interactionData->message = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(const EditInteractionResponseData& dataPackage) {
		Message messageData = Interactions::editInteractionResponseAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Edit_Interaction_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId = messageData.channelId;
		dataPackageNewer.interactionData->user = messageData.author;
		dataPackageNewer.interactionData->message = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(const CreateFollowUpMessageData& dataPackage) {
		Message messageData = Interactions::createFollowUpMessageAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId = messageData.channelId;
		dataPackageNewer.interactionData->user = messageData.author;
		dataPackageNewer.interactionData->message = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(const EditFollowUpMessageData& dataPackage) {
		Message messageData = Interactions::editFollowUpMessageAsync(dataPackage).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Edit_Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId = messageData.channelId;
		dataPackageNewer.interactionData->user = messageData.author;
		dataPackageNewer.interactionData->message = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(const CreateEphemeralInteractionResponseData& dataPackage) {
		CreateInteractionResponseData dataPackageNew{ dataPackage };
		Message messageData = Interactions::createInteractionResponseAsync(dataPackageNew).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Ephemeral_Interaction_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId = messageData.channelId;
		dataPackageNewer.interactionData->user = messageData.author;
		dataPackageNewer.interactionData->message = messageData;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(const CreateEphemeralFollowUpMessageData& dataPackage) {
		CreateFollowUpMessageData dataPackageNew{ dataPackage };
		Message messageData = Interactions::createFollowUpMessageAsync(dataPackageNew).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Ephemeral_Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId = messageData.channelId;
		dataPackageNewer.interactionData->user = messageData.author;
		dataPackageNewer.interactionData->message = messageData;
		return dataPackageNewer;
	}
}
