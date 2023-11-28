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
/// GuildScheduledEventEntities.hpp - Header for the guild scheduled events stuff.
/// Nov 27, 2021 Chris M.
/// https://discordcoreapi.com
/// \file GuildScheduledEventEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	/// @brief For collecting a list of a guild's scheduled events.
	struct get_guild_scheduled_events_data {
		bool withUserCount{};///< Do we collect the user counts?
		snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// @brief For creating a guild_scheduled_event_data.
	struct create_guild_scheduled_event_data {
		guild_scheduled_event_privacy_level privacyLevel{ guild_scheduled_event_privacy_level::Guild_Only };///< The privacy level of the scheduled event.
		guild_scheduled_event_metadata entityMetadata{};/// the entity metadata of the scheduled event.
		guild_scheduled_event_entity_type entityType{};///< The entity type of the scheduled event.
		jsonifier::string scheduledStartTime{};///< The time to schedule the scheduled event.
		jsonifier::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		jsonifier::string description{};///< The description of the scheduled event.
		snowflake channelId{};///< The channel_data id of the scheduled event.
		snowflake guildId{};///< The guild within which to create the event.
		jsonifier::string name{};///< The name of the scheduled event.
	};

	/// @brief For collecting a single guild scheduled event.
	struct get_guild_scheduled_event_data {
		snowflake guildScheduledEventId{};///< The id of the desired scheduled event.
		bool withUserCount{};///< Do we collect the user counts?
		snowflake guildId{};///< Guild from which we would like to collect the events.
	};

	/// @brief For modifying a single guild scheduled event.
	struct modify_guild_scheduled_event_data {
		guild_scheduled_event_privacy_level privacyLevel{ guild_scheduled_event_privacy_level::Guild_Only };///< The privacy level of the scheduled event.
		guild_scheduled_event_status status{ guild_scheduled_event_status::Active };///< The status of the scheduled event.
		guild_scheduled_event_metadata entityMetadata{};///< The entity metadata of the scheduled event.
		guild_scheduled_event_entity_type entityType{};///< The entity type of the scheduled event.
		snowflake guildScheduledEventId{};///< The id of the event to modify.
		jsonifier::string scheduledStartTime{};///< The time to schedule the scheduled event.
		jsonifier::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		jsonifier::string description{};///< The description of the scheduled event.
		snowflake channelId{};///< The channel_data id of the scheduled event, set to null if changing entity type to external.
		snowflake guildId{};///< The guild within which to modify the event.
		jsonifier::string name{};///< The name of the scheduled event.
	};

	/// @brief For deleting a single guild scheduled event.
	struct delete_guild_scheduled_event_data {
		snowflake guildScheduledEventId{};///< The id of the event to modify.
		snowflake guildId{};///< The guild within which to modify the event.
	};

	/// @brief For collecting a list of guild scheduled event users.
	struct get_guild_scheduled_event_users_data {
		snowflake guildScheduledEventId{};///< The id of the event to modify.
		snowflake guildId{};///< The guild within which to modify the event.
		snowflake before{};///< consider only users before given user id.
		bool withMember{};///< Include guild member responseData if it exists.
		snowflake after{};///< consider only users after given user id.
		uint32_t limit{};///< How many users to receive from the event.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the guild_scheduled_event_data related discord endpoints.
	class DiscordCoreAPI_Dll guild_scheduled_events {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Gets a list of a given guild's scheduled events.
		/// @param dataPackage a get_guild_scheduled_eventsstructure.
		/// @return a co_routine containing a vector<guild_scheduled_event_data>.
		static co_routine<jsonifier::vector<guild_scheduled_event_data>> getGuildScheduledEventsAsync(get_guild_scheduled_events_data dataPackage);

		/// @brief Creates a new guild_scheduled_event_data within a chosen guild.
		/// @param dataPackage a create_guild_scheduled_event_data structure.
		/// @return a co_routine containing a guild_scheduled_event_data.
		static co_routine<guild_scheduled_event_data> createGuildScheduledEventAsync(create_guild_scheduled_event_data dataPackage);

		/// @brief Collects a single guild_scheduled_event_data.
		/// @param dataPackage a get_guild_scheduled_event_data structure.
		/// @return a co_routine containing a guild_scheduled_event_data.
		static co_routine<guild_scheduled_event_data> getGuildScheduledEventAsync(get_guild_scheduled_event_data dataPackage);

		/// @brief Modifies a single guild_scheduled_event_data.
		/// @param dataPackage a modify_guild_scheduled_event_data structure.
		/// @return a co_routine containing a guild_scheduled_event_data.
		static co_routine<guild_scheduled_event_data> modifyGuildScheduledEventAsync(modify_guild_scheduled_event_data dataPackage);

		/// @brief Deletes a single guild_scheduled_event_data.
		/// @param dataPackage a delete_guild_scheduled_event_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteGuildScheduledEventAsync(delete_guild_scheduled_event_data dataPackage);

		/// @brief Collects a list of users for a given guild_scheduled_event_data.
		/// @param dataPackage a get_guild_scheduled_event_users_data structure.
		/// @return a co_routine containing a vector<guild_scheduled_event_user>.
		static co_routine<jsonifier::vector<guild_scheduled_event_user_data>> getGuildScheduledEventUsersAsync(get_guild_scheduled_event_users_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};
	/**@}*/
}
