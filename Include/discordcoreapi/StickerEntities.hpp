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
/// StickerEntities.hpp - Header for the sticker related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file StickerEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	/// \brief For getting a Sticker object for the given sticker ID.
	struct DiscordCoreAPI_Dll GetStickerData {
		Snowflake stickerId{};///< The chosen Sticker's Snowflake.
	};

	/// \brief For collecting a list of Stickers from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildStickersData {
		Snowflake guildId{};///< The chosen Guild from which you would like to collect the Stickers from.
	};

	/// \brief For creating a single Sticker.
	struct DiscordCoreAPI_Dll CreateGuildStickerData {
		std::string description{};///< Description of the Sticker.
		std::string reason{};///< The reason for creating the Sticker.
		Snowflake guildId{};///< The Guild within which to create the Sticker.
		std::string file{};///< The sticker file to upload, must be a Png, Apng, or Lottie JSON file, max 500 KB.
		std::string name{};///< Name of the Sticker.
		std::string tags{};///< Autocomplete / suggestion tags for the sticker(max 200 characters).
	};

	/// \brief For modifying a single Sticker.
	struct DiscordCoreAPI_Dll ModifyGuildStickerData {
		std::string description{};///< Description of the Sticker.
		Snowflake stickerId{};///< The Sticker you wish to modify.
		std::string reason{};///< The reason for modifying the Sticker.
		Snowflake guildId{};///< The Guild within which to modify the Sticker.
		std::string name{};///< Name of the Sticker.
		std::string tags{};///< Autocomplete / suggestion tags for the sticker(max 200 characters).
	};

	/// \brief For deleting a single Sticker.
	struct DiscordCoreAPI_Dll DeleteGuildStickerData {
		Snowflake stickerId{};///< The Sticker you wish to delete.
		std::string reason{};///< The reason for deleting the Sticker.
		Snowflake guildId{};///< The Guild within which to delete the Sticker.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the Sticker related Discord endpoints.
	class DiscordCoreAPI_Dll Stickers {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Gets a single Sticker item.
		/// \param dataPackage A GetStickerData structure.
		/// \returns A CoRoutine containing a Sticker.
		static CoRoutine<Sticker> getStickerAsync(GetStickerData dataPackage);

		/// \brief Gets a list of nitro-available Sticker packs
		/// \returns A CoRoutine containing a StickerPackDataVector.
		static CoRoutine<std::vector<StickerPackData>> getNitroStickerPacksAsync();

		/// \brief Gets a list of Stickers from a Guild.
		/// \param dataPackage A GetGuildStickersData structure.
		/// \returns A CoRoutine containing a StickerVector.
		static CoRoutine<std::vector<Sticker>> getGuildStickersAsync(GetGuildStickersData dataPackage);

		/// \brief Creates a new Sticker within a chosen Guild.
		/// \param dataPackage A CreateGuildStickerData structure.
		/// \returns A CoRoutine containing a Sticker.
		static CoRoutine<Sticker> createGuildStickerAsync(CreateGuildStickerData dataPackage);

		/// \brief Modifies a Sticker within a chosen Guild.
		/// \param dataPackage A ModifyGuildStickerData structure.
		/// \returns A CoRoutine containing a Sticker.
		static CoRoutine<Sticker> modifyGuildStickerAsync(ModifyGuildStickerData dataPackage);

		/// \brief Deletes a Sticker within a chosen Guild.
		/// \param dataPackage A DeleteGuildStickerData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildStickerAsync(DeleteGuildStickerData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
};
