/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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
		uint64_t guildId{};///< Guild from which we would like to collect the events.
	};

	/// For creating a GuildScheduledEvent. \brief For creating a GuildScheduledEvent.
	struct DiscordCoreAPI_Dll CreateGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};/// The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		std::string scheduledStartTime{};///< The time to schedule the scheduled event.
		std::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		std::string description{};///< The description of the scheduled event.
		uint64_t channelId{};///< The Channel id of the scheduled event.
		uint64_t guildId{};///< The Guild within which to create the event.
		std::string name{};///< The name of the scheduled event.
	};

	/// For collecting a single Guild scheduled event. \brief For collecting a single Guild scheduled event.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventData {
		uint64_t guildScheduledEventId{};///< The id of the desired scheduled event.
		bool withUserCount{ false };///< Do we collect the user counts?
		uint64_t guildId{};///< Guild from which we would like to collect the events.
	};

	/// For modifying a single Guild Scheduled Event. \brief For modifying a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll ModifyGuildScheduledEventData {
		GuildScheduledEventPrivacyLevel privacyLevel{ GuildScheduledEventPrivacyLevel::Guild_Only };///< The privacy level of the scheduled event.
		GuildScheduledEventStatus status{ GuildScheduledEventStatus::Active };///< The status of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< The entity metadata of the scheduled event.
		GuildScheduledEventEntityType entityType{};///< The entity type of the scheduled event.
		uint64_t guildScheduledEventId{};///< The id of the event to modify.
		std::string scheduledStartTime{};///< The time to schedule the scheduled event.
		std::string scheduledEndTime{};///< The time when the scheduled event is scheduled to end.
		std::string description{};///< The description of the scheduled event.
		uint64_t channelId{};///< The Channel id of the scheduled event, set to null if changing entity type to External.
		uint64_t guildId{};///< The Guild within which to modify the event.
		std::string name{};///< The name of the scheduled event.
	};

	/// For deleting a single Guild Scheduled Event. \brief For deleting a single Guild Scheduled Event.
	struct DiscordCoreAPI_Dll DeleteGuildScheduledEventData {
		uint64_t guildScheduledEventId{};///< The id of the event to modify.
		uint64_t guildId{};///< The Guild within which to modify the event.
	};

	/// For collecting a list of Guild Scheduled Event Users. \brief For collecting a list of Guild Scheduled Event Users.
	struct DiscordCoreAPI_Dll GetGuildScheduledEventUsersData {
		uint64_t guildScheduledEventId{};///< The id of the event to modify.
		bool withMember{ false };///< Include Guild member responseData if it exists.
		uint64_t guildId{};///< The Guild within which to modify the event.
		uint64_t before{};///< Consider only users before given user id.
		uint64_t after{};///< Consider only users after given user id.
		uint32_t limit{ 0 };///< How many users to receive from the event.
	};

	/// A single GuildScheduledEvent responseData structure. \brief A single GuildScheduledEvent responseData structure.
	class DiscordCoreAPI_Dll GuildScheduledEvent : public GuildScheduledEventData {
	  public:
		GuildScheduledEvent() = default;

		GuildScheduledEvent& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildScheduledEvent(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildScheduledEvent() = default;

	  	inline void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEvent* pDataStructure) {
			if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
				pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<GuildScheduledEventPrivacyLevel>();
			}

			if (jsonObjectData.contains("entity_type") && !jsonObjectData["entity_type"].is_null()) {
				pDataStructure->entityType = jsonObjectData["entity_type"].get<GuildScheduledEventEntityType>();
			}

			if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
				pDataStructure->status = jsonObjectData["status"].get<GuildScheduledEventStatus>();
			}

			if (jsonObjectData.contains("entity_metadata") && !jsonObjectData["entity_metadata"].is_null()) {
				pDataStructure->entityMetadata = jsonObjectData["entity_metadata"];
			}

			if (jsonObjectData.contains("scheduled_start_time") && !jsonObjectData["scheduled_start_time"].is_null()) {
				pDataStructure->scheduledStartTime = jsonObjectData["scheduled_start_time"].get<std::string>();
			}

			if (jsonObjectData.contains("scheduled_end_time") && !jsonObjectData["scheduled_end_time"].is_null()) {
				pDataStructure->scheduledEndTime = jsonObjectData["scheduled_end_time"].get<std::string>();
			}

			if (jsonObjectData.contains("user_count") && !jsonObjectData["user_count"].is_null()) {
				pDataStructure->userCount = jsonObjectData["user_count"].get<uint32_t>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
				pDataStructure->creatorId = jsonObjectData["creator_id"].get<std::string>();
			}

			if (jsonObjectData.contains("entity_id") && !jsonObjectData["entity_id"].is_null()) {
				pDataStructure->entityId = jsonObjectData["entity_id"].get<std::string>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
				pDataStructure->creator = jsonObjectData["creator"];
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}
		}
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
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
}// namespace DiscordCoreAPI
