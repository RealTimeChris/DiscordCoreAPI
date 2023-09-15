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
	/// @brief For collecting a list of a Guild's scheduled events.
	struct GetGuildScheduledEventsData {
		bool withUserCount{};///< Do we collect the user counts?
		Snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// @brief For creating a GuildScheduledEventData.
	struct CreateGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};/// The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		jsonifier::string scheduledStartTime{};///< The time to schedule the scheduled event.
		jsonifier::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		jsonifier::string description{};///< The description of the scheduled event.
		Snowflake channelId{};///< The ChannelData id of the scheduled event.
		Snowflake guildId{};///< The Guild within which to create the event.
		jsonifier::string name{};///< The name of the scheduled event.
	};

	/// @brief For collecting a single Guild scheduled event.
	struct GetGuildScheduledEventData {
		Snowflake guildScheduledEventId{};///< The id of the desired scheduled event.
		bool withUserCount{};///< Do we collect the user counts?
		Snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// @brief For modifying a single Guild Scheduled Event.
	struct ModifyGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventStatus status{ GuildScheduledEventStatus::Active };///< The status of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		jsonifier::string scheduledStartTime{};///< The time to schedule the scheduled event.
		jsonifier::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		jsonifier::string description{};///< The description of the scheduled event.
		Snowflake channelId{};///< The ChannelData id of the scheduled event, set to null if changing entity type to External.
		Snowflake guildId{};///< The Guild within which to modify the event.
		jsonifier::string name{};///< The name of the scheduled event.
	};

	/// @brief For deleting a single Guild Scheduled Event.
	struct DeleteGuildScheduledEventData {
		Snowflake guildScheduledEventId{};///< The id of the event to modify.
		Snowflake guildId{};///< The Guild within which to modify the event.
	};

	/// @brief For collecting a list of Guild Scheduled Event Users.
	struct GetGuildScheduledEventUsersData {
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
	/// @brief An interface class for the GuildScheduledEventData related Discord endpoints.
	class DiscordCoreAPI_Dll GuildScheduledEvents {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Gets a list of a given Guild's scheduled events.
		/// @param dataPackage A GetGuildScheduledEventsstructure.
		/// @return A CoRoutine containing a vector<GuildScheduledEventData>.
		static CoRoutine<jsonifier::vector<GuildScheduledEventData>> getGuildScheduledEventsAsync(GetGuildScheduledEventsData dataPackage);

		/// @brief Creates a new GuildScheduledEventData within a chosen Guild.
		/// @param dataPackage A CreateGuildScheduledEventData structure.
		/// @return A CoRoutine containing a GuildScheduledEventData.
		static CoRoutine<GuildScheduledEventData> createGuildScheduledEventAsync(CreateGuildScheduledEventData dataPackage);

		/// @brief Collects a single GuildScheduledEventData.
		/// @param dataPackage A GetGuildScheduledEventData structure.
		/// @return A CoRoutine containing a GuildScheduledEventData.
		static CoRoutine<GuildScheduledEventData> getGuildScheduledEventAsync(GetGuildScheduledEventData dataPackage);

		/// @brief Modifies a single GuildScheduledEventData.
		/// @param dataPackage A ModifyGuildScheduledEventData structure.
		/// @return A CoRoutine containing a GuildScheduledEventData.
		static CoRoutine<GuildScheduledEventData> modifyGuildScheduledEventAsync(ModifyGuildScheduledEventData dataPackage);

		/// @brief Deletes a single GuildScheduledEventData.
		/// @param dataPackage A DeleteGuildScheduledEventData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteGuildScheduledEventAsync(DeleteGuildScheduledEventData dataPackage);

		/// @brief Collects a list of Users for a given GuildScheduledEventData.
		/// @param dataPackage A GetGuildScheduledEventUsersData structure.
		/// @return A CoRoutine containing a vector<GuildScheduledEventUser>.
		static CoRoutine<jsonifier::vector<GuildScheduledEventUserData>> getGuildScheduledEventUsersAsync(GetGuildScheduledEventUsersData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
}
