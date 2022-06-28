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
/// StageInstanceEntities.hpp - Source file for the Stage Instance related stuff.
/// Nov 29, 2021 Chris M.
/// https://discordcoreapi.com
/// \file StageInstanceEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/// For creating a StageInstance. \brief For creating a StageInstance.
	struct DiscordCoreAPI_Dll CreateStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance(default Guild_Only).
		uint64_t channelId{};///< The id of the Stage Channel.
		std::string reason{};///< The reason for starting it.
		std::string topic{};///< The topic of the Stage instance(1 - 120 characters).
	};

	/// For collecting a single StageInstance. \brief For collecting a single StageInstance.
	struct DiscordCoreAPI_Dll GetStageInstanceData {
		uint64_t channelId{};///< The Channel id from which you would like to collect the StageInstance.
	};

	/// For modifying a single StageInstance. \brief For modifying a single StageInstance.
	struct DiscordCoreAPI_Dll ModifyStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance.
		uint64_t channelId{};///< The Channel Id of the StageInstance.
		std::string reason{};///< Reason for modifying the StageInstance.
		std::string topic{};///< The topic of the Stage instance(1 - 120 characters).
	};

	/// For deleting a single StageInstance. \brief For deleting a single StageInstance.
	struct DiscordCoreAPI_Dll DeleteStageInstanceData {
		uint64_t channelId{};///< The Channel Id of the StageInstance.
		std::string reason{};///< Reason for deleting the StageInstance.
	};

	/// A single StageInstance. \brief A single StageInstance.
	class DiscordCoreAPI_Dll StageInstance : public StageInstanceData {
	  public:
		StageInstance() = default;

		StageInstance& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StageInstance(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}
		virtual ~StageInstance() = default;

	  	inline void parseObject(const nlohmann::json& jsonObjectData, StageInstance* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
				pDataStructure->topic = jsonObjectData["topic"].get<std::string>();
			}

			if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
				pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<StageInstancePrivacyLevel>();
			}

			if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData["discoverable_disabled"].is_null()) {
				pDataStructure->discoverableDisabled = jsonObjectData["discoverable_disabled"].get<bool>();
			}
		}
	};

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class DiscordCoreAPI_Dll for the StageInstance related Discord endpoints. \brief An interface class DiscordCoreAPI_Dll for the StageInstance related Discord endpoints.
	class DiscordCoreAPI_Dll StageInstances {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// Creates a StageInstance. \brief Creates a StageInstance.
		/// \param dataPackage A CreateStageInstanceData structure.
		/// \returns A CoRoutine containing a StageInstance.
		static CoRoutine<StageInstance> createStageInstanceAsync(CreateStageInstanceData dataPackage);

		/// Collects a StageInstance. \brief Collects a StageInstance.
		/// \param dataPackage A GetStageInstanceData structure.
		/// \returns A CoRoutine containing a StageInstance.
		static CoRoutine<StageInstance> getStageInstanceAsync(GetStageInstanceData dataPackage);

		/// Modifies a StageInstance. \brief Modifies a StageInstance.
		/// \param dataPackage A ModifyStageInstanceData structure.
		/// \returns A CoRoutine containing a StageInstance.
		static CoRoutine<StageInstance> modifyStageInstanceAsync(ModifyStageInstanceData dataPackage);

		/// Deletes a StageInstance. \brief Deletes a StageInstance.
		/// \param dataPackage A DeleteStageInstanceData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteStageInstanceAsync(DeleteStageInstanceData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};

	/**@}*/
}// namespace DiscordCoreAPI
