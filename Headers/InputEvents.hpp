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
	/// InputEvents class - for handling InputEvents and responses. \brief InputEvents class - for handling InputEvents and responses.
	class DiscordCoreAPI_Dll InputEvents {
	public:
		static void initialize(shared_ptr<DiscordCoreInternal::MessageManager> messagesNew, shared_ptr<DiscordCoreInternal::InteractionManager> interactionsNew);

		/// Responds to one of a number of types of "InputEvents". \brief Responds to one of a number of types of InputEvents.
		/// \param dataPackage A RespondToInputEventData structure.
		/// \returns An InputEventData structure.
		static InputEventData respondToEvent(RespondToInputEventData dataPackage);

		/// Deletes a previously sent "input-event-response". \brief Deletes a previously sent "input-event-response".
		/// \param dataPackage A InputEventData structure.
		/// \param timeDelayNew An int representing the desired delay until the message is deleted, in milliseconds.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInputEventResponseAsync(InputEventData dataPackage, __int32 timeDelayNew = 0);
		
	protected:

		static shared_ptr<DiscordCoreInternal::InteractionManager> interactions;
		static shared_ptr<DiscordCoreInternal::MessageManager> messages;

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

#endif