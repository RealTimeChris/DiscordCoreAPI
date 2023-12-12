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
/// InputEvents.hpp - Header for the input-event handler class.
/// Aug 18, 2021
/// https://discordcoreapi.com
/// \file InputEvents.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief For handling user_data input - messages or interactions.
	class DiscordCoreAPI_Dll input_events {
	  public:
		/// @brief Responds to one of a number of types of "input_events".
		/// @param dataPackage a respond_to_input_event_data structure.
		/// @return an unique_ptr containing an input_event_data structure.
		static co_routine<input_event_data> respondToInputEventAsync(respond_to_input_event_data dataPackage);

		/// @brief Deletes a previously sent "input-event-response".
		/// @param dataPackage a unique_ptr containing an input_event_data structure.
		/// @param timeDelayNew an int32_t representing the desired delay until the message is deleted, in milliseconds.
		/// @return a co_routine containing void.
		static co_routine<void> deleteInputEventResponseAsync(input_event_data dataPackage, uint32_t timeDelayNew = 0);

	  protected:
		static input_event_data respondToInputEvent(create_ephemeral_interaction_response_data dataPackage);

		static input_event_data respondToInputEvent(create_deferred_interaction_response_data dataPackage);

		static input_event_data respondToInputEvent(create_ephemeral_follow_up_message_data dataPackage);

		static input_event_data respondToInputEvent(create_interaction_response_data dataPackage);

		static input_event_data respondToInputEvent(edit_interaction_response_data dataPackage);

		static input_event_data respondToInputEvent(create_follow_up_message_data dataPackage);

		static input_event_data respondToInputEvent(edit_follow_up_message_data dataPackage);

		static input_event_data respondToInputEvent(create_message_data dataPackage);

		static input_event_data respondToInputEvent(edit_message_data dataPackage);
	};
	/**@}*/
}
