// InputEvents.hpp - Header for the input-event handler class.
// Aug 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INPUT_EVENTS_
#define _INPUT_EVENTS_

#include "IndexInitial.hpp"
#include "MessageEntities.hpp"
#include "InteractionEntities.hpp"
#include "GuildEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// InputEventHandler class - for handling InputEventHandler and responses. \brief InputEventHandler class - for handling InputEventHandler and responses.
	class DiscordCoreAPI_Dll InputEventHandler {
	public:
		/// Responds to one of a number of types of "InputEventHandler". \brief Responds to one of a number of types of InputEventHandler.
		/// \param dataPackage A RespondToInputEventData structure.
		/// \returns An InputEventData structure.
		InputEventData respondToEvent(RespondToInputEventData dataPackage);

		/// Deletes a previously sent "input-event-response". \brief Deletes a previously sent "input-event-response".
		/// \param dataPackage A InputEventData structure.
		/// \param timeDelayNew An int representing the desired delay until the message is deleted, in milliseconds.
		/// \returns A CoRoutine containing void.
		CoRoutine<void> deleteInputEventResponseAsync(InputEventData dataPackage, __int32 timeDelayNew);
		
	protected:

		InputEventData respondToEvent(CreateDeferredInteractionResponseData dataPackage);

		InputEventData respondToEvent(CreateInteractionResponseData dataPackage);

		InputEventData respondToEvent(EditInteractionResponseData dataPackage);

		InputEventData respondToEvent(CreateFollowUpMessageData dataPackage);

		InputEventData respondToEvent(EditFollowUpMessageData dataPackage);

		InputEventData respondToEvent(CreateEphemeralInteractionResponseData dataPackage);

		InputEventData respondToEvent(CreateEphemeralFollowUpMessageData dataPackage);

		InputEventData respondToEvent(CreateMessageData dataPackage);

		InputEventData respondToEvent(EditMessageData dataPackage);

		InputEventData respondToEvent(SendDMData dataPackage);

		void respondToEvent(DeferComponentResponseData dataPackage);
	};
	/**@}*/
}

#endif