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

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For getting a sticker_data object for the given sticker id.
	struct get_sticker_data {
		snowflake stickerId{};///< The chosen sticker_data's snowflake.
	};

	/// @brief For collecting a list of stickers from a chosen guild.
	struct get_guild_stickers_data {
		snowflake guildId{};///< The chosen guild from which you would like to collect the stickers from.
	};

	/// @brief For creating a single sticker_data.
	struct create_guild_sticker_data {
		jsonifier::string description{};///< Description of the sticker_data.
		jsonifier::string reason{};///< The reason for creating the sticker_data.
		snowflake guildId{};///< The guild within which to create the sticker_data.
		jsonifier::string file{};///< The sticker file to upload, must be a png, apng, or lottie json file, max 500 kb.
		jsonifier::string name{};///< Name of the sticker_data.
		jsonifier::string tags{};///< Autocomplete / suggestion tags for the sticker(max 200 characters).
	};

	/// @brief For modifying a single sticker_data.
	struct modify_guild_sticker_data {
		jsonifier::string description{};///< Description of the sticker_data.
		snowflake stickerId{};///< The sticker_data you wish to modify.
		jsonifier::string reason{};///< The reason for modifying the sticker_data.
		snowflake guildId{};///< The guild within which to modify the sticker_data.
		jsonifier::string name{};///< Name of the sticker_data.
		jsonifier::string tags{};///< Autocomplete / suggestion tags for the sticker(max 200 characters).
	};

	/// @brief For deleting a single sticker_data.
	struct delete_guild_sticker_data {
		snowflake stickerId{};///< The sticker_data you wish to delete.
		jsonifier::string reason{};///< The reason for deleting the sticker_data.
		snowflake guildId{};///< The guild within which to delete the sticker_data.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// @brief An interface class for the sticker_data related discord endpoints.
	class DiscordCoreAPI_Dll stickers {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Gets a single sticker_data item.
		/// @param dataPackage a get_sticker_data structure.
		/// @return a co_routine containing a sticker_data.
		static co_routine<sticker_data> getStickerAsync(get_sticker_data dataPackage);

		/// @brief Gets a list of nitro-available sticker_data packs
		/// @return a co_routine containing a sticker_pack_data
		static co_routine<jsonifier::vector<sticker_pack_data>> getNitroStickerPacksAsync();

		/// @brief Gets a list of stickers from a guild.
		/// @param dataPackage a get_guild_stickers_data structure.
		/// @return a co_routine containing a jsonifier::vector<sticker_data>.
		static co_routine<jsonifier::vector<sticker_data>> getGuildStickersAsync(get_guild_stickers_data dataPackage);

		/// @brief Creates a new sticker_data within a chosen guild.
		/// @param dataPackage a create_guild_sticker_data structure.
		/// @return a co_routine containing a sticker_data.
		static co_routine<sticker_data> createGuildStickerAsync(create_guild_sticker_data dataPackage);

		/// @brief Modifies a sticker_data within a chosen guild.
		/// @param dataPackage a modify_guild_sticker_data structure.
		/// @return a co_routine containing a sticker_data.
		static co_routine<sticker_data> modifyGuildStickerAsync(modify_guild_sticker_data dataPackage);

		/// @brief Deletes a sticker_data within a chosen guild.
		/// @param dataPackage a delete_guild_sticker_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteGuildStickerAsync(delete_guild_sticker_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};
	/**@}*/
};
