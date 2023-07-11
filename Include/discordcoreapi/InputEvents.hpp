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

namespace DiscordCoreAPI {

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief For handling User input - Messages or Interactions.
	class DiscordCoreAPI_Dll InputEvents {
	  public:
		/// \brief Responds to one of a number of types of "InputEvents".
		/// \param dataPackage A RespondToInputEventData structure.
		/// \returns An UniquePtr containing an InputEventData structure.
		static CoRoutine<InputEventData> respondToInputEventAsync(RespondToInputEventData dataPackage);

		/// \brief Deletes a previously sent "input-event-response".
		/// \param dataPackage A UniquePtr containing an InputEventData structure.
		/// \param timeDelayNew An int32_t representing the desired delay until the message is deleted, in Milliseconds.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInputEventResponseAsync(InputEventData& dataPackage, int32_t timeDelayNew = 0);

	  protected:
		static InputEventData respondToInputEvent(CreateEphemeralInteractionResponseData& dataPackage);

		static InputEventData respondToInputEvent(CreateDeferredInteractionResponseData& dataPackage);

		static InputEventData respondToInputEvent(CreateEphemeralFollowUpMessageData& dataPackage);

		static InputEventData respondToInputEvent(CreateInteractionResponseData& dataPackage);

		static InputEventData respondToInputEvent(EditInteractionResponseData& dataPackage);

		static InputEventData respondToInputEvent(CreateFollowUpMessageData& dataPackage);

		static InputEventData respondToInputEvent(EditFollowUpMessageData& dataPackage);

		static InputEventData respondToInputEvent(CreateMessageData& dataPackage);

		static InputEventData respondToInputEvent(EditMessageData& dataPackage);
	};
	/**@}*/
}
