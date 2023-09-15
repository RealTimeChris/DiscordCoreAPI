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
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace DiscordCoreAPI {

	/// @brief For creating a StageInstanceData.
	struct CreateStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance(default Guild_Only).
		Snowflake channelId{};///< The id of the Stage ChannelData.
		jsonifier::string reason{};///< The reason for starting it.
		jsonifier::string topic{};///< The topic of the Stage instance(1 - 120 characters).
	};

	/// @brief For collecting a single StageInstanceData.
	struct GetStageInstanceData {
		Snowflake channelId{};///< The ChannelData id from which you would like to collect the StageInstanceData.
	};

	/// @brief For modifying a single StageInstanceData.
	struct ModifyStageInstanceData {
		StageInstancePrivacyLevel privacyLevel{};///< The privacy level of the Stage instance.
		Snowflake channelId{};///< The ChannelData Snowflake of the StageInstanceData.
		jsonifier::string reason{};///< Reason for modifying the StageInstanceData.
		jsonifier::string topic{};///< The topic of the Stage instance(1 - 120 characters).
	};

	/// @brief For deleting a single StageInstanceData.
	struct DeleteStageInstanceData {
		Snowflake channelId{};///< The ChannelData Snowflake of the StageInstanceData.
		jsonifier::string reason{};///< Reason for deleting the StageInstanceData.
	};

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the StageInstanceData related Discord endpoints.
	class DiscordCoreAPI_Dll StageInstances {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Creates a StageInstanceData.
		/// @param dataPackage A CreateStageInstanceData structure.
		/// @return A CoRoutine containing a StageInstanceData.
		static CoRoutine<StageInstanceData> createStageInstanceAsync(CreateStageInstanceData dataPackage);

		/// @brief Collects a StageInstanceData.
		/// @param dataPackage A GetStageInstanceData structure.
		/// @return A CoRoutine containing a StageInstanceData.
		static CoRoutine<StageInstanceData> getStageInstanceAsync(GetStageInstanceData dataPackage);

		/// @brief Modifies a StageInstanceData.
		/// @param dataPackage A ModifyStageInstanceData structure.
		/// @return A CoRoutine containing a StageInstanceData.
		static CoRoutine<StageInstanceData> modifyStageInstanceAsync(ModifyStageInstanceData dataPackage);

		/// @brief Deletes a StageInstanceData.
		/// @param dataPackage A DeleteStageInstanceData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteStageInstanceAsync(DeleteStageInstanceData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};

	/**@}*/
}
