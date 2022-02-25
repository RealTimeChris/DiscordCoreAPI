/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// StageInstanceEntities.hpp - Source file for the Stage Instance related stuff.
/// Nov 29, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file StageInstanceEntities.hpp

#pragma once

#include <FoundationEntities.hpp>

namespace DiscordCoreAPI {

	/// For creating a StageInstance. \brief For creating a StageInstance.
	struct DiscordCoreAPI_Dll CreateStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance(default Guild_Only).
		std::string channelId{ "" };///< The id of the Stage channel.
		std::string reason{ "" };///< The reason for starting it.
		std::string topic{ "" };///< The topic of the Stage instance(1 - 120 characters).
	};
	
	/// For collecting a single StageInstance. \brief For collecting a single StageInstance.
	struct DiscordCoreAPI_Dll GetStageInstanceData {
		std::string channelId{ "" };///< The channel id from which you would like to collect the StageInstance.
	};

	/// For modifying a single StageInstance. \brief For modifying a single StageInstance.
	struct DiscordCoreAPI_Dll ModifyStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance.
		std::string channelId{ "" };///< The channel Id of the StageInstance.
		std::string reason{ "" };///< Reason for modifying the StageInstance.
		std::string topic{ "" };///< The topic of the Stage instance(1 - 120 characters).
	};

	/// For deleting a single StageInstance. \brief For deleting a single StageInstance.
	struct DiscordCoreAPI_Dll DeleteStageInstanceData {
		std::string channelId{ "" };///< The channel Id of the StageInstance.
		std::string reason{ "" };///< Reason for deleting the StageInstance.
	};

	/// A single StageInstance. \brief A single StageInstance.
	class DiscordCoreAPI_Dll StageInstance : public StageInstanceData {
	public:

		StageInstance() = default;

		StageInstance(StageInstanceData dataNew);

	};
	
	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class for the StageInstance related Discord endpoints. \brief An interface class for the StageInstance related Discord endpoints.
	class DiscordCoreAPI_Dll StageInstances {
	public:

		static void initialize(DiscordCoreInternal::HttpClient*);

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

		static DiscordCoreInternal::HttpClient* httpClient;
	};

	/**@}*/
}
