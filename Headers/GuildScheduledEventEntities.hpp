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

		/// Getes a list of a given Guild's scheduled events. \brief Getes a list of a given Guild's scheduled events.
		/// \param dataPackage A GetGuildScheduledEventData structure.
		/// \returns A CoRoutine containing AuditLogData.
		static CoRoutine<vector<GuildScheduledEvent>> getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage);
	};
	/**@}*/
}
