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
/// StageInstanceEntities.hpp - Source file for the stage instance related stuff.
/// Nov 29, 2021 Chris M.
/// https://discordcoreapi.com
/// \file StageInstanceEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace discord_core_api {

	/// @brief For creating a stage_instance_data.
	struct create_stage_instance_data {
		stage_instance_privacy_level privacyLevel{};///< The privacy level of the stage instance(default Guild_Only).
		snowflake channelId{};///< The id of the stage channel_data.
		jsonifier::string reason{};///< The reason for starting it.
		jsonifier::string topic{};///< The topic of the stage instance(1 - 120 characters).
	};

	/// @brief For collecting a single stage_instance_data.
	struct get_stage_instance_data {
		snowflake channelId{};///< The channel_data id from which you would like to collect the stage_instance_data.
	};

	/// @brief For modifying a single stage_instance_data.
	struct modify_stage_instance_data {
		stage_instance_privacy_level privacyLevel{};///< The privacy level of the stage instance.
		snowflake channelId{};///< The channel_data snowflake of the stage_instance_data.
		jsonifier::string reason{};///< Reason for modifying the stage_instance_data.
		jsonifier::string topic{};///< The topic of the stage instance(1 - 120 characters).
	};

	/// @brief For deleting a single stage_instance_data.
	struct delete_stage_instance_data {
		snowflake channelId{};///< The channel_data snowflake of the stage_instance_data.
		jsonifier::string reason{};///< Reason for deleting the stage_instance_data.
	};

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the stage_instance_data related discord endpoints.
	class DiscordCoreAPI_Dll stage_instances {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Creates a stage_instance_data.
		/// @param dataPackage a create_stage_instance_data structure.
		/// @return a co_routine containing a stage_instance_data.
		static co_routine<stage_instance_data> createStageInstanceAsync(create_stage_instance_data dataPackage);

		/// @brief Collects a stage_instance_data.
		/// @param dataPackage a get_stage_instance_data structure.
		/// @return a co_routine containing a stage_instance_data.
		static co_routine<stage_instance_data> getStageInstanceAsync(get_stage_instance_data dataPackage);

		/// @brief Modifies a stage_instance_data.
		/// @param dataPackage a modify_stage_instance_data structure.
		/// @return a co_routine containing a stage_instance_data.
		static co_routine<stage_instance_data> modifyStageInstanceAsync(modify_stage_instance_data dataPackage);

		/// @brief Deletes a stage_instance_data.
		/// @param dataPackage a delete_stage_instance_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteStageInstanceAsync(delete_stage_instance_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};

	/**@}*/
}
