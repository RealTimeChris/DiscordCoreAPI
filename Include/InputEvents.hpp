// InputEvents.hpp - Header for the input-event handler class DiscordCoreAPI_Dll.
// Aug 18, 2021
// Chris M.
// https://github.com/RealTimeChris

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