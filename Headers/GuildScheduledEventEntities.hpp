// GuildScheduledEventEntities.hpp - Header for the Guild Scheduled Events stuff.
// Nov 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"
#include "Http.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// For collecting a list of a Guild's scheduled events. \brief For collecting a list of a Guild's scheduled events.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventsData {
		bool withUserCount{ false };///< Do we collect the user counts?
		string guildId{ "" };///< Guild from which we would like to collect the events.
	};

	/// For creating a GuildScheduledEvent. \brief For creating a GuildScheduledEvent.
	struct DiscordCoreAPI_Dll CreateGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel	privacyLevel{ GuildScheduledEventPrivacyLevel::GUILD_ONLY};///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};/// The entity metadata of the scheduled event.
		GuildScheduledEventEntityType	entityType{};///< The entity type of the scheduled event.
		string scheduledStartTime{ "" };///< The time to schedule the scheduled event.
		string scheduledEndTime{ "" };///< The time when the scheduled event is scheduled to end.
		string description{ "" };///< The description of the scheduled event.
		string channelId{ "" };///< The channel id of the scheduled event.
		string guildId{ "" };///< The guild within which to create the event.
		string name{ "" };///< The name of the scheduled event.
	};
	
	/// A single GuildScheduledEvent data structure. \brief A single GuildScheduledEvent data structure.
	class DiscordCoreAPI_Dll GuildScheduledEvent : public GuildScheduledEventData {
	public:

		friend class DiscordCoreInternal::HttpRequestAgent;
		template<typename returnValueType>
		friend class CoRoutine;
		friend class GuildScheduledEvents;

		virtual ~GuildScheduledEvent() {};

	protected:
		GuildScheduledEvent(GuildScheduledEventData dataPackage);

		GuildScheduledEvent();

	};

	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the GuildScheduledEvent related Discord endpoints. \brief An interface class for the GuildScheduledEvent related Discord endpoints.
	class DiscordCoreAPI_Dll GuildScheduledEvents {
	public:

		/// Gets a list of a given Guild's scheduled events. \brief Gets a list of a given Guild's scheduled events.
		/// \param dataPackage A GetGuildScheduledEventData structure.
		/// \returns A CoRoutine containing vector<GuildScheduledEvent>.
		static CoRoutine<vector<GuildScheduledEvent>> getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage);

		/// Creates a new GuildScheduledEvent within a chosen Guild. \brief Creates a new GuildScheduledEvent within a chosen Guild.
		/// \param dataPackage A CreateGuildScheduledEventData structure.
		/// \returns A CoRoutine containing GuildScheduledEvent.
		static CoRoutine<GuildScheduledEvent> createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage);
	};
	/**@}*/
}
