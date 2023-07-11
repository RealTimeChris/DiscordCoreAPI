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
/// StageInstanceEntities.hpp - Source file for the Stage Instance related stuff.
/// Nov 29, 2021 Chris M.
/// https://discordcoreapi.com
/// \file StageInstanceEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/// \brief For creating a StageInstance.
	struct DiscordCoreAPI_Dll CreateStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance(default Guild_Only).
		Snowflake channelId{};///< The id of the Stage Channel.
		std::string reason{};///< The reason for starting it.
		std::string topic{};///< The topic of the Stage instance(1 - 120 characters).
	};

	/// \brief For collecting a single StageInstance.
	struct DiscordCoreAPI_Dll GetStageInstanceData {
		Snowflake channelId{};///< The Channel id from which you would like to collect the StageInstance.
	};

	/// \brief For modifying a single StageInstance.
	struct DiscordCoreAPI_Dll ModifyStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance.
		Snowflake channelId{};///< The Channel Snowflake of the StageInstance.
		std::string reason{};///< Reason for modifying the StageInstance.
		std::string topic{};///< The topic of the Stage instance(1 - 120 characters).
	};

	/// \brief For deleting a single StageInstance.
	struct DiscordCoreAPI_Dll DeleteStageInstanceData {
		Snowflake channelId{};///< The Channel Snowflake of the StageInstance.
		std::string reason{};///< Reason for deleting the StageInstance.
	};

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the StageInstance related Discord endpoints.
	class DiscordCoreAPI_Dll StageInstances {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Creates a StageInstance.
		/// \param dataPackage A CreateStageInstanceData structure.
		/// \returns A CoRoutine containing a StageInstance.
		static CoRoutine<StageInstance> createStageInstanceAsync(CreateStageInstanceData dataPackage);

		/// \brief Collects a StageInstance.
		/// \param dataPackage A GetStageInstanceData structure.
		/// \returns A CoRoutine containing a StageInstance.
		static CoRoutine<StageInstance> getStageInstanceAsync(GetStageInstanceData dataPackage);

		/// \brief Modifies a StageInstance.
		/// \param dataPackage A ModifyStageInstanceData structure.
		/// \returns A CoRoutine containing a StageInstance.
		static CoRoutine<StageInstance> modifyStageInstanceAsync(ModifyStageInstanceData dataPackage);

		/// \brief Deletes a StageInstance.
		/// \param dataPackage A DeleteStageInstanceData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteStageInstanceAsync(DeleteStageInstanceData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};

	/**@}*/
}
