/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// InputEvents.hpp - Header for the input-event handler class.
/// Aug 18, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <FoundationEntities.hpp>
#include <MessageEntities.hpp>

namespace DiscordCoreAPI {

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// For handling User input - Messages or Interactions. \brief For handling User input - Messages or Interactions.
	class DiscordCoreAPI_Dll InputEvents {
	public:

		/// Responds to one of a number of types of "InputEvents". \brief Responds to one of a number of types of "InputEvents".
		/// \param dataPackage A RespondToInputEventData structure.
		/// \returns An InputEventData structure.
		static InputEventData respondToEvent(RespondToInputEventData dataPackage);

		/// Deletes a previously sent "input-event-response". \brief Deletes a previously sent "input-event-response".
		/// \param dataPackage A InputEventData structure.
		/// \param timeDelayNew An int32_t representing the desired delay until the message is deleted, in std::chrono::milliseconds.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInputEventResponseAsync(InputEventData dataPackage, int32_t timeDelayNew = 0);

	protected:

		static InputEventData respondToEvent(CreateEphemeralInteractionResponseData dataPackage);

		static InputEventData respondToEvent(CreateDeferredInteractionResponseData dataPackage);

		static InputEventData respondToEvent(CreateEphemeralFollowUpMessageData dataPackage);

		static InputEventData respondToEvent(CreateInteractionResponseData dataPackage);

		static InputEventData respondToEvent(EditInteractionResponseData dataPackage);

		static InputEventData respondToEvent(CreateFollowUpMessageData dataPackage);

		static InputEventData respondToEvent(EditFollowUpMessageData dataPackage);

		static InputEventData respondToEvent(CreateMessageData dataPackage);

		static void respondToEvent(DeferComponentResponseData dataPackage);

		static InputEventData respondToEvent(EditMessageData dataPackage);

		static InputEventData respondToEvent(SendDMData dataPackage);
	};
	/**@}*/
}