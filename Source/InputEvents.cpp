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
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace discord_core_api {

	co_routine<input_event_data> input_events::respondToInputEventAsync(respond_to_input_event_data dataPackage) {
		co_await newThreadAwaitable<input_event_data>();
		if (dataPackage.eventType == interaction_type::Message_Component) {
			create_interaction_response_data dataPackage02{ dataPackage };
			if (dataPackage.type == input_event_response_type::Deferred_Response) {
				dataPackage02.type = interaction_callback_type::Deferred_Update_Message;
			} else {
				dataPackage02.type = interaction_callback_type::Update_Message;
			}
			input_event_data newEvent = respondToInputEvent(dataPackage02);
			if (dataPackage.type == input_event_response_type::Interaction_Response || dataPackage.type == input_event_response_type::Ephemeral_Interaction_Response ||
				dataPackage.type == input_event_response_type::Edit_Interaction_Response) {
				newEvent.responseType = input_event_response_type::Edit_Interaction_Response;
			} else if (dataPackage.type == input_event_response_type::Follow_Up_Message || dataPackage.type == input_event_response_type::Ephemeral_Follow_Up_Message ||
				dataPackage.type == input_event_response_type::Edit_Follow_Up_Message) {
				newEvent.responseType = input_event_response_type::Edit_Follow_Up_Message;
			}
			co_return std::move(newEvent);
		} else if (dataPackage.eventType == interaction_type::Application_Command_Autocomplete) {
			create_interaction_response_data dataPackage02{ dataPackage };
			dataPackage02.type		= interaction_callback_type::Application_Command_Autocomplete_Result;
			input_event_data newEvent =respondToInputEvent(dataPackage02);
			newEvent.responseType	= input_event_response_type::Application_Command_AutoComplete_Result;
			co_return std::move(newEvent);
		}
		switch (dataPackage.type) {
			case input_event_response_type::Ephemeral_Deferred_Response: {
				create_deferred_interaction_response_data dataPackage02{ dataPackage };
				dataPackage02.data.flags = 64;
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Deferred_Response: {
				create_deferred_interaction_response_data dataPackage02{ dataPackage };
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Interaction_Response: {
				create_interaction_response_data dataPackage02{ dataPackage };
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Edit_Interaction_Response: {
				edit_interaction_response_data dataPackage02{ dataPackage };
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Ephemeral_Interaction_Response: {
				create_ephemeral_interaction_response_data dataPackage02{ dataPackage };
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Follow_Up_Message: {
				create_follow_up_message_data dataPackage02{ dataPackage };
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Edit_Follow_Up_Message: {
				edit_follow_up_message_data dataPackage02{ dataPackage };
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Ephemeral_Follow_Up_Message: {
				create_ephemeral_follow_up_message_data dataPackage02{ dataPackage };
				co_return respondToInputEvent(dataPackage02);
			}
			case input_event_response_type::Modal_Interaction_Response: {
				[[fallthrough]];
			}
			case input_event_response_type::Application_Command_AutoComplete_Result: {
				[[fallthrough]];
			}
			case input_event_response_type::Unset: {
				break;
			}
			default: {
				co_return input_event_data();
			}
		}
		co_return input_event_data();
	}

	co_routine<void> input_events::deleteInputEventResponseAsync(input_event_data dataPackage, uint32_t timeDelayNew) {
		input_event_data newPackage = dataPackage;
		co_await newThreadAwaitable<void>();
		if (newPackage.responseType == input_event_response_type::Follow_Up_Message || newPackage.responseType == input_event_response_type::Edit_Follow_Up_Message ||
			newPackage.responseType == input_event_response_type::Ephemeral_Follow_Up_Message) {
			respond_to_input_event_data dataPackageNew{ newPackage };
			delete_follow_up_message_data dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			interactions::deleteFollowUpMessageAsync(dataPackageNewer).get();
		} else if (newPackage.responseType == input_event_response_type::Interaction_Response || newPackage.responseType == input_event_response_type::Edit_Interaction_Response ||
			newPackage.responseType == input_event_response_type::Ephemeral_Interaction_Response ||
			newPackage.responseType == input_event_response_type::Ephemeral_Deferred_Response || newPackage.responseType == input_event_response_type::Deferred_Response) {
			respond_to_input_event_data dataPackageNew{ newPackage };
			delete_interaction_response_data dataPackageNewer{ dataPackageNew };
			dataPackageNewer.timeDelay = timeDelayNew;
			interactions::deleteInteractionResponseAsync(dataPackageNewer).get();
		}
		co_return;
	}

	input_event_data input_events::respondToInputEvent(create_deferred_interaction_response_data dataPackage) {
		create_interaction_response_data dataPackageNew{ dataPackage };
		auto result = interactions::createInteractionResponse(dataPackageNew);
		input_event_data dataPackageNewer{};
		dataPackageNewer.responseType					= input_event_response_type::Deferred_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token			= dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->channelId		= result.channelId;
		dataPackageNewer.interactionData->message.id	= result.messageReference.messageId;
		dataPackageNewer.interactionData->id			= dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->user			= result.author;
		return dataPackageNewer;
	}

	input_event_data input_events::respondToInputEvent(create_interaction_response_data dataPackage) {
		message_data messageData = interactions::createInteractionResponse(dataPackage);
		input_event_data dataPackageNewer{};
		dataPackageNewer.responseType					= input_event_response_type::Interaction_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token			= dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id			= dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId		= messageData.channelId;
		dataPackageNewer.interactionData->user			= messageData.author;
		dataPackageNewer.interactionData->message		= messageData;
		return dataPackageNewer;
	}

	input_event_data input_events::respondToInputEvent(edit_interaction_response_data dataPackage) {
		message_data messageData = interactions::editInteractionResponse(dataPackage);
		input_event_data dataPackageNewer{};
		dataPackageNewer.responseType					= input_event_response_type::Edit_Interaction_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token			= dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id			= dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId		= messageData.channelId;
		dataPackageNewer.interactionData->user			= messageData.author;
		dataPackageNewer.interactionData->message		= messageData;
		return dataPackageNewer;
	}

	input_event_data input_events::respondToInputEvent(create_follow_up_message_data dataPackage) {
		message_data messageData = interactions::createFollowUpMessage(dataPackage);
		input_event_data dataPackageNewer{};
		dataPackageNewer.responseType					= input_event_response_type::Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token			= dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id			= dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId		= messageData.channelId;
		dataPackageNewer.interactionData->user			= messageData.author;
		dataPackageNewer.interactionData->message		= messageData;
		return dataPackageNewer;
	}

	input_event_data input_events::respondToInputEvent(edit_follow_up_message_data dataPackage) {
		message_data messageData = interactions::editFollowUpMessage(dataPackage);
		input_event_data dataPackageNewer{};
		dataPackageNewer.responseType					= input_event_response_type::Edit_Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token			= dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id			= dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId		= messageData.channelId;
		dataPackageNewer.interactionData->user			= messageData.author;
		dataPackageNewer.interactionData->message		= messageData;
		return dataPackageNewer;
	}

	input_event_data input_events::respondToInputEvent(create_ephemeral_interaction_response_data dataPackage) {
		create_interaction_response_data dataPackageNew{ dataPackage };
		message_data messageData = interactions::createInteractionResponse(dataPackageNew);
		input_event_data dataPackageNewer{};
		dataPackageNewer.responseType					= input_event_response_type::Ephemeral_Interaction_Response;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token			= dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id			= dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId		= messageData.channelId;
		dataPackageNewer.interactionData->user			= messageData.author;
		dataPackageNewer.interactionData->message		= messageData;
		return dataPackageNewer;
	}

	input_event_data input_events::respondToInputEvent(create_ephemeral_follow_up_message_data dataPackage) {
		create_follow_up_message_data dataPackageNew{ dataPackage };
		message_data messageData = interactions::createFollowUpMessage(dataPackageNew);
		input_event_data dataPackageNewer{};
		dataPackageNewer.responseType					= input_event_response_type::Ephemeral_Follow_Up_Message;
		dataPackageNewer.interactionData->applicationId = dataPackage.interactionPackage.applicationId;
		dataPackageNewer.interactionData->token			= dataPackage.interactionPackage.interactionToken;
		dataPackageNewer.interactionData->id			= dataPackage.interactionPackage.interactionId;
		dataPackageNewer.interactionData->channelId		= messageData.channelId;
		dataPackageNewer.interactionData->user			= messageData.author;
		dataPackageNewer.interactionData->message		= messageData;
		return dataPackageNewer;
	}

}