/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// GuildScheduledEventEntities.hpp - Header for the Guild Scheduled Events stuff.
/// Nov 27, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.hpp

#pragma once

#ifndef GUILD_SCHEDULED_EVENT_ENTITIES
#define GUILD_SCHEDULED_EVENT_ENTITIES

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	/// For collecting a list of a Guild's scheduled events. \brief For collecting a list of a Guild's scheduled events.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventsData {
		Bool withUserCount{ false };///< Do we collect the user counts?
		Snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// For creating a GuildScheduledEvent. \brief For creating a GuildScheduledEvent.
	struct DiscordCoreAPI_Dll CreateGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};/// The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		String scheduledStartTime{};///< The time to schedule the scheduled event.
		String scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		String description{};///< The description of the scheduled event.
		Snowflake channelId{};///< The Channel id of the scheduled event.
		Snowflake guildId{};///< The Guild within which to create the event.
		String name{};///< The name of the scheduled event.

		operator JsonObject();
	};

	/// For collecting a single Guild scheduled event. \brief For collecting a single Guild scheduled event.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventData {
		Snowflake guildScheduledEventId{};///< The id of the desired scheduled event.
		Bool withUserCount{ false };///< Do we collect the user counts?
		Snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// For modifying a single Guild Scheduled Event. \brief For modifying a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll ModifyGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventStatus status{ GuildScheduledEventStatus::Active };///< The status of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		String scheduledStartTime{};///< The time to schedule the scheduled event.
		String scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		String description{};///< The description of the scheduled event.
		Snowflake channelId{};///< The Channel id of the scheduled event, set to null if changing entity type to External.
		Snowflake guildId{};///< The Guild within which to modify the event.
		String name{};///< The name of the scheduled event.

		operator JsonObject();
	};

	/// For deleting a single Guild Scheduled Event. \brief For deleting a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll DeleteGuildScheduledEventData {
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		Snowflake guildId{};///< The Guild within which to modify the event.
	};

	/// For collecting a list of Guild Scheduled Event Users. \brief For collecting a list of Guild Scheduled Event Users.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventUsersData {
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		Bool withMember{ false };///< Include Guild member responseData if it exists.
		Snowflake guildId{};///< The Guild within which to modify the event.
		Snowflake before{};///< Consider only users before given user id.
		Snowflake after{};///< Consider only users after given user id.
		Uint32 limit{ 0 };///< How many users to receive from the event.
	};

	/// A single GuildScheduledEvent responseData structure. \brief A single GuildScheduledEvent responseData structure.
	class DiscordCoreAPI_Dll GuildScheduledEvent : public GuildScheduledEventData {
	  public:
		GuildScheduledEvent() noexcept = default;

		GuildScheduledEvent(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildScheduledEvent() noexcept = default;
	};

	class DiscordCoreAPI_Dll GuildScheduledEventVector {
	  public:
		GuildScheduledEventVector() noexcept = default;

		operator std::vector<GuildScheduledEvent>();

		GuildScheduledEventVector(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildScheduledEventVector() noexcept = default;

	  protected:
		std::vector<GuildScheduledEvent> theGuildScheduledEvents{};
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the GuildScheduledEvent related Discord endpoints. \brief An interface class for the GuildScheduledEvent related Discord endpoints.
	class DiscordCoreAPI_Dll GuildScheduledEvents {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// Gets a list of a given Guild's scheduled events. \brief Gets a list of a given Guild's scheduled events.
		/// \param dataPackage A GetGuildScheduledEventsDatastructure.
		/// \returns A CoRoutine containing a vector<GuildScheduledEvent>.
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
		/// \returns A CoRoutine containing a vector<GuildScheduledEventUser>.
		static CoRoutine<std::vector<GuildScheduledEventUserData>> getGuildScheduledEventUsersAsync(GetGuildScheduledEventUsersData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
}// namespace DiscordCoreAPI
#endif