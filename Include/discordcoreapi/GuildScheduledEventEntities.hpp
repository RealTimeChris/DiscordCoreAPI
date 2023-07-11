/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// GuildScheduledEventEntities.hpp - Header for the Guild Scheduled Events stuff.
/// Nov 27, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	/// \brief For collecting a list of a Guild's scheduled events.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventsData {
		bool withUserCount{};///< Do we collect the user counts?
		Snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// \brief For creating a GuildScheduledEvent.
	struct DiscordCoreAPI_Dll CreateGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};/// The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		std::string scheduledStartTime{};///< The time to schedule the scheduled event.
		std::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		std::string description{};///< The description of the scheduled event.
		Snowflake channelId{};///< The Channel id of the scheduled event.
		Snowflake guildId{};///< The Guild within which to create the event.
		std::string name{};///< The name of the scheduled event.
	};

	/// \brief For collecting a single Guild scheduled event.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventData {
		Snowflake guildScheduledEventId{};///< The id of the desired scheduled event.
		bool withUserCount{};///< Do we collect the user counts?
		Snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// \brief For modifying a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll ModifyGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventStatus status{ GuildScheduledEventStatus::Active };///< The status of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		std::string scheduledStartTime{};///< The time to schedule the scheduled event.
		std::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		std::string description{};///< The description of the scheduled event.
		Snowflake channelId{};///< The Channel id of the scheduled event, set to null if changing entity type to External.
		Snowflake guildId{};///< The Guild within which to modify the event.
		std::string name{};///< The name of the scheduled event.
	};

	/// \brief For deleting a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll DeleteGuildScheduledEventData {
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		Snowflake guildId{};///< The Guild within which to modify the event.
	};

	/// \brief For collecting a list of Guild Scheduled Event Users.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventUsersData {
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		Snowflake guildId{};///< The Guild within which to modify the event.
		Snowflake before{};///< Consider only users before given user id.
		bool withMember{};///< Include Guild member responseData if it exists.
		Snowflake after{};///< Consider only users after given user id.
		uint32_t limit{};///< How many users to receive from the event.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the GuildScheduledEvent related Discord endpoints.
	class DiscordCoreAPI_Dll GuildScheduledEvents {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Gets a list of a given Guild's scheduled events.
		/// \param dataPackage A GetGuildScheduledEventsstructure.
		/// \returns A CoRoutine containing a vector<GuildScheduledEvent>.
		static CoRoutine<GuildScheduledEventVector> getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage);

		/// \brief Creates a new GuildScheduledEvent within a chosen Guild.
		/// \param dataPackage A CreateGuildScheduledEventData structure.
		/// \returns A CoRoutine containing a GuildScheduledEvent.
		static CoRoutine<GuildScheduledEvent> createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage);

		/// \brief Collects a single GuildScheduledEvent.
		/// \param dataPackage A GetGuildScheduledEventData structure.
		/// \returns A CoRoutine containing a GuildScheduledEvent.
		static CoRoutine<GuildScheduledEvent> getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage);

		/// \brief Modifies a single GuildScheduledEvent.
		/// \param dataPackage A ModifyGuildScheduledEventData structure.
		/// \returns A CoRoutine containing a GuildScheduledEvent.
		static CoRoutine<GuildScheduledEvent> modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage);

		/// \brief Deletes a single GuildScheduledEvent.
		/// \param dataPackage A DeleteGuildScheduledEventData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage);

		/// \brief Collects a list of Users for a given GuildScheduledEvent.
		/// \param dataPackage A GetGuildScheduledEventUsersData structure.
		/// \returns A CoRoutine containing a vector<GuildScheduledEventUser>.
		static CoRoutine<std::vector<GuildScheduledEventUserData>> getGuildScheduledEventUsersAsync(GetGuildScheduledEventUsersData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
}
