/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
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
		co_await NewThreadAwaitable<InputEventData>();
		if (dataPackage.type == InputEventResponseType::Unset) {
			throw DCAException("InputEvents::respondToInputEventAsync() Error: Please set an "
							   "input-event-response-type!");
		}
		if (dataPackage.eventType == InteractionType::Message_Component) {
			CreateInteractionResponseData dataPackage02{ dataPackage };
			if (dataPackage.type == InputEventResponseType::Deferred_Response) {
				dataPackage02.type = InteractionCallbackType::Deferred_Update_Message;
			} else {
				dataPackage02.type = InteractionCallbackType::Update_Message;
			}
			InputEventData newEvent = InputEvents::respondToInputEvent(dataPackage02);
			if (dataPackage.type == InputEventResponseType::Interaction_Response ||
				dataPackage.type == InputEventResponseType::Ephemeral_Interaction_Response ||
				dataPackage.type == InputEventResponseType::Edit_Interaction_Response) {
				newEvent.responseType = InputEventResponseType::Edit_Interaction_Response;
			} else if (dataPackage.type == InputEventResponseType::Follow_Up_Message ||
				dataPackage.type == InputEventResponseType::Ephemeral_Follow_Up_Message ||
				dataPackage.type == InputEventResponseType::Edit_Follow_Up_Message) {
				newEvent.responseType = InputEventResponseType::Edit_Follow_Up_Message;
			}
			co_return std::move(newEvent);
		} else if (dataPackage.eventType == InteractionType::Application_Command_Autocomplete) {
			CreateInteractionResponseData dataPackage02{ dataPackage };
			dataPackage02.type = InteractionCallbackType::Application_Command_Autocomplete_Result;
			InputEventData newEvent = InputEvents::respondToInputEvent(dataPackage02);
			newEvent.responseType = InputEventResponseType::Application_Command_AutoComplete_Result;
			co_return std::move(newEvent);
		}
		switch (dataPackage.type) {
			case InputEventResponseType::Ephemeral_Deferred_Response: {
				CreateDeferredInteractionResponseData dataPackage02{ dataPackage };
				dataPackage02.data.flags = 64;
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
		co_return InputEventData();
	}

	CoRoutine<void> InputEvents::deleteInputEventResponseAsync(InputEventData& dataPackage, int32_t timeDelayNew) {
		InputEventData newPackage = dataPackage;
		co_await NewThreadAwaitable<void>();
		if (newPackage.responseType == InputEventResponseType::Follow_Up_Message ||
			newPackage.responseType == InputEventResponseType::Edit_Follow_Up_Message ||
			newPackage.responseType == InputEventResponseType::Ephemeral_Follow_Up_Message) {
			RespondToInputEventData dataPackageNew{ newPackage };
			DeleteFollowUpMessageData dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteFollowUpMessageAsync(dataPackageNewer).get();
		} else if (newPackage.responseType == InputEventResponseType::Interaction_Response ||
			newPackage.responseType == InputEventResponseType::Edit_Interaction_Response ||
			newPackage.responseType == InputEventResponseType::Ephemeral_Interaction_Response ||
			newPackage.responseType == InputEventResponseType::Ephemeral_Deferred_Response ||
			newPackage.responseType == InputEventResponseType::Deferred_Response) {
			RespondToInputEventData dataPackageNew{ newPackage };
			DeleteInteractionResponseData dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			Interactions::deleteInteractionResponseAsync(dataPackageNewer).get();
		}
		co_return;
	}

	InputEventData InputEvents::respondToInputEvent(CreateDeferredInteractionResponseData& dataPackage) {
		dataPackage.generateExcludedKeys();
		CreateInteractionResponseData dataPackageNew{ dataPackage };
		auto result = Interactions::createInteractionResponse(dataPackageNew);
		InputEventData dataPackageNewer{};
		dataPackageNewer.responseType = InputEventResponseType::Deferred_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token = dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->channelId = result.channelId;
		dataPackageNewer.interactionData->message.id = result.referencedMessage.id;
		dataPackageNewer.interactionData->id = dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->user = result.author;
		return dataPackageNewer;
	}

	InputEventData InputEvents::respondToInputEvent(CreateInteractionResponseData& dataPackage) {
		dataPackage.generateExcludedKeys();
		MessageData messageData = Interactions::createInteractionResponse(dataPackage);
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
		dataPackage.generateExcludedKeys();
		MessageData messageData = Interactions::editInteractionResponse(dataPackage);
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
		dataPackage.generateExcludedKeys();
		MessageData messageData = Interactions::createFollowUpMessage(dataPackage);
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
		MessageData messageData = Interactions::editFollowUpMessage(dataPackage);
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
		dataPackage.generateExcludedKeys();
		CreateInteractionResponseData dataPackageNew{ dataPackage };
		MessageData messageData = Interactions::createInteractionResponse(dataPackageNew);
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
		MessageData messageData = Interactions::createFollowUpMessage(dataPackageNew);
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
