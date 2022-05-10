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
/// InputEvents.hpp - Header for the input-event handler class.
/// Aug 18, 2021
/// Chris M.
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
	/// For handling User input - Messages or Interactions. \brief For handling User input - Messages or Interactions.
	class DiscordCoreAPI_Dll InputEvents {
	  public:
		/// Responds to one of a number of types of "InputEvents". \brief Responds to one of a number of types of "InputEvents".
		/// \param dataPackage A RespondToInputEventData structure.
		/// \returns An std::unique_ptr containing an InputEventData structure.
		static CoRoutine<InputEventData> respondToInputEventAsync(RespondToInputEventData dataPackage);

		/// Deletes a previously sent "input-event-response". \brief Deletes a previously sent "input-event-response".
		/// \param dataPackage A std::unique_ptr containing an InputEventData structure.
		/// \param timeDelayNew An int32_t representing the desired delay until the message is deleted, in std::chrono::milliseconds.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInputEventResponseAsync(InputEventData dataPackage, int32_t timeDelayNew = 0);

	  protected:
		static InputEventData respondToInputEvent(CreateEphemeralInteractionResponseData dataPackage);

		static InputEventData respondToInputEvent(CreateDeferredInteractionResponseData dataPackage);

		static InputEventData respondToInputEvent(CreateEphemeralFollowUpMessageData dataPackage);

		static InputEventData respondToInputEvent(CreateInteractionResponseData dataPackage);

		static InputEventData respondToInputEvent(EditInteractionResponseData dataPackage);

		static InputEventData respondToInputEvent(CreateFollowUpMessageData dataPackage);

		static InputEventData respondToInputEvent(EditFollowUpMessageData dataPackage);

		static InputEventData respondToInputEvent(CreateMessageData dataPackage);

		static InputEventData respondToInputEvent(EditMessageData dataPackage);
	};
	/**@}*/
}// namespace DiscordCoreAPI