/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris).

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// GuildScheduledEventEntities.hpp - Header for the Guild Scheduled Events stuff.
/// Nov 27, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.hpp

#pragma once

#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	/// For collecting a list of a Guild's scheduled events. \brief For collecting a list of a Guild's scheduled events.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventsData {
		bool withUserCount{ false };///< Do we collect the user counts?
		std::string guildId{ "" };///< Guild from which we would like to collect the events.
	};

	/// For creating a GuildScheduledEvent. \brief For creating a GuildScheduledEvent.
	struct DiscordCoreAPI_Dll CreateGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};/// The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		std::string scheduledStartTime{ "" };///< The time to schedule the scheduled event.
		std::string scheduledEndTime{ "" };///< The time when the scheduled event is scheduled to end.
		std::string description{ "" };///< The description of the scheduled event.
		std::string channelId{ "" };///< The Channel id of the scheduled event.
		std::string guildId{ "" };///< The Guild within which to create the event.
		std::string name{ "" };///< The name of the scheduled event.
	};

	/// For collecting a single Guild scheduled event. \brief For collecting a single Guild scheduled event.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventData {
		std::string guildScheduledEventId{ "" };///< The id of the desired scheduled event.
		bool withUserCount{ false };///< Do we collect the user counts?
		std::string guildId{ "" };///< Guild from which we would like to collect the events.
	};

	/// For modifying a single Guild Scheduled Event. \brief For modifying a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll ModifyGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventStatus status{ GuildScheduledEventStatus::Active };///< The status of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		std::string guildScheduledEventId{ "" };///< The id of the event to modify.
		std::string scheduledStartTime{ "" };///< The time to schedule the scheduled event.
		std::string scheduledEndTime{ "" };///< The time when the scheduled event is scheduled to end.
		std::string description{ "" };///< The description of the scheduled event.
		std::string channelId{ "" };///< The Channel id of the scheduled event, set to null if changing entity type to External.
		std::string guildId{ "" };///< The Guild within which to modify the event.
		std::string name{ "" };///< The name of the scheduled event.
	};

	/// For deleting a single Guild Scheduled Event. \brief For deleting a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll DeleteGuildScheduledEventData {
		std::string guildScheduledEventId{ "" };///< The id of the event to modify.
		std::string guildId{ "" };///< The Guild within which to modify the event.
	};

	/// For collecting a list of Guild Scheduled Event Users. \brief For collecting a list of Guild Scheduled Event Users.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventUsersData {
		std::string guildScheduledEventId{ "" };///< The id of the event to modify.
		std::string guildId{ "" };///< The Guild within which to modify the event.
		std::string before{ "" };///< Consider only users before given user id.
		bool withMember{ false };///< Include Guild member responseData if it exists.
		std::string after{ "" };///< Consider only users after given user id.
		uint32_t limit{ 0 };///< How many users to receive from the event.
	};

	/// A single GuildScheduledEvent responseData structure. \brief A single GuildScheduledEvent responseData structure.
	class DiscordCoreAPI_Dll GuildScheduledEvent : public GuildScheduledEventData {
	  public:
		GuildScheduledEvent() = default;

		GuildScheduledEvent(GuildScheduledEventData dataNew);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the GuildScheduledEvent related Discord endpoints. \brief An interface class for the GuildScheduledEvent related Discord endpoints.
	class DiscordCoreAPI_Dll GuildScheduledEvents {
	  public:
		static void initialize(DiscordCoreInternal::HttpClient*);

		/// Gets a list of a given Guild's scheduled events. \brief Gets a list of a given Guild's scheduled events.
		/// \param dataPackage A GetGuildScheduledEventsDatastructure.
		/// \returns A CoRoutine containing a std::vector<GuildScheduledEvent>.
		static CoRoutine<std::vector<GuildScheduledEvent>> getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage);

		/// Creates a new GuildScheduledEvent within a chosen Guild. \brief Creates a new GuildScheduledEvent within a chosen Guild.
		/// \param dataPackage A CreateGuildScheduledEventData structure.
		/// \returns A CoRoutine containing a GuildScheduledEvent.
		static CoRoutine<GuildScheduledEvent> createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage);

		/// Collects a single GuildScheduledEvent. \brief Collects a single GuildScheduledEvent.
		/// \param dataPackage A GetGuildScheduledEventData structure.
		/// \returns A CoRoutine containing a GuildScheduledEvent.
		static CoRoutine<GuildScheduledEvent> getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage);

		/// Modifies a single GuildScheduledEvent. \brief Modifies a single GuildScheduledEvent.
		/// \param dataPackage A ModifyGuildScheduledEventData structure.
		/// \returns A CoRoutine containing a GuildScheduledEvent.
		static CoRoutine<GuildScheduledEvent> modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage);

		/// Deletes a single GuildScheduledEvent. \brief Deletes a single GuildScheduledEvent.
		/// \param dataPackage A DeleteGuildScheduledEventData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage);

		/// Collects a list of Users for a given GuildScheduledEvent. \brief Collects a list of Users for a given GuildScheduledEvent.
		/// \param dataPackage A GetGuildScheduledEventUsersData structure.
		/// \returns A CoRoutine containing a std::vector<GuildScheduledEventUser>.
		static CoRoutine<std::vector<GuildScheduledEventUserData>> getGuildScheduledEventUsersAsync(GetGuildScheduledEventUsersData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpClient* httpClient;
	};
	/**@}*/
}// namespace DiscordCoreAPI
