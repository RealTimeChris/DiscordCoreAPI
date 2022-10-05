/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// InputEvents.cpp - Source file for the input-event handler class.
/// Aug 18, 2021
/// https://discordcoreapi.com
/// \file InputEvents.cpp

#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	CoRoutine<InputEventData> InputEvents::respondToInputEventAsync(RespondToInputEventData dataPackage) {
		try {
			co_await NewThreadAwaitable<InputEventData>();
			if (dataPackage.type == InputEventResponseType::Unset) {
				throw std::runtime_error("InputEvents::respondToInputEventAsync() Error: Please set an "
										 "input-event-response-type!\n\n");
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
					CreateDeferredInteractionResponseData dataPackage02{ dataPackage };
					co_return InputEvents::respondToInputEvent(dataPackage02);
				}
				case InputEventResponseType::Interaction_Response: {
					CreateInteractionResponseData dataPackage02{ dataPackage };
					co_return InputEvents::respondToInputEvent(dataPackage02);
				}
				case InputEventResponseType::Edit_Interaction_Response: {
					EditInteractionResponseData dataPackage02{ dataPackage };
					co_return InputEvents::respondToInputEvent(dataPackage02);
				}
				case InputEventResponseType::Ephemeral_Interaction_Response: {
					CreateEphemeralInteractionResponseData dataPackage02{ dataPackage };
					co_return InputEvents::respondToInputEvent(dataPackage02);
				}
				case InputEventResponseType::Follow_Up_Message: {
					CreateFollowUpMessageData dataPackage02{ dataPackage };
					co_return InputEvents::respondToInputEvent(dataPackage02);
				}
				case InputEventResponseType::Edit_Follow_Up_Message: {
					EditFollowUpMessageData dataPackage02{ dataPackage };
					co_return InputEvents::respondToInputEvent(dataPackage02);
				}
				case InputEventResponseType::Ephemeral_Follow_Up_Message: {
					CreateEphemeralFollowUpMessageData dataPackage02{ dataPackage };
					co_return InputEvents::respondToInputEvent(dataPackage02);
				}
				case InputEventResponseType::Unset: {
					break;
				}
				default: {
					co_return InputEventData();
				}
			}
		} catch (...) {
			reportException("InputEvents::respondToInputEventAsync()");
			co_return InputEventData();
		}
	}

	CoRoutine<void> InputEvents::deleteInputEventResponseAsync(InputEventData& dataPackage, Int32 timeDelayNew) {
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

	InputEventData InputEvents::respondToInputEvent(CreateDeferredInteractionResponseData& dataPackage) {
		CreateInteractionResponseData dataPackageNew{ dataPackage };
		auto theResult = Interactions::createInteractionResponseAsync(dataPackageNew).get();
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Deferred_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->channelId = theResult.channelId;
		dataPackageNewer.interactionData->message.id = theResult.referencedMessage->id;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(CreateInteractionResponseData& dataPackage) {
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

	InputEventData InputEvents::respondToInputEvent(EditInteractionResponseData& dataPackage) {
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

	InputEventData InputEvents::respondToInputEvent(CreateFollowUpMessageData& dataPackage) {
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

	InputEventData InputEvents::respondToInputEvent(EditFollowUpMessageData& dataPackage) {
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

	InputEventData InputEvents::respondToInputEvent(CreateEphemeralInteractionResponseData& dataPackage) {
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

	InputEventData InputEvents::respondToInputEvent(CreateEphemeralFollowUpMessageData& dataPackage) {
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
