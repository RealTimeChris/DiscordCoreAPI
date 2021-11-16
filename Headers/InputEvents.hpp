// InputEvents.hpp - Header for the input-event handler class.
// Aug 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "MessageEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	class DiscordCoreAPI_Dll InputEvents {
	public:

		/// Responds to one of a number of types of "InputEventHandler". \brief Responds to one of a number of types of InputEventHandler.
		/// \param dataPackage A RespondToInputEventData structure.
		/// \returns An InputEventData structure.
		static InputEventData respondToEvent(RespondToInputEventData dataPackage);

		/// Deletes a previously sent "input-event-response". \brief Deletes a previously sent "input-event-response".
		/// \param dataPackage A InputEventData structure.
		/// \param timeDelayNew An __int32 representing the desired delay until the message is deleted, in milliseconds.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInputEventResponseAsync(InputEventData dataPackage, __int32 timeDelayNew = 0);
		
	protected:

		static InputEventData respondToEvent(CreateDeferredInteractionResponseData dataPackage);

		static InputEventData respondToEvent(CreateInteractionResponseData dataPackage);

		static InputEventData respondToEvent(EditInteractionResponseData dataPackage);

		static InputEventData respondToEvent(CreateFollowUpMessageData dataPackage);

		static InputEventData respondToEvent(EditFollowUpMessageData dataPackage);

		static InputEventData respondToEvent(CreateEphemeralInteractionResponseData dataPackage);

		static InputEventData respondToEvent(CreateEphemeralFollowUpMessageData dataPackage);

		static InputEventData respondToEvent(CreateMessageData dataPackage);

		static InputEventData respondToEvent(EditMessageData dataPackage);

		static InputEventData respondToEvent(SendDMData dataPackage);

		static void respondToEvent(DeferComponentResponseData dataPackage);
	};
	/**@}*/
}
