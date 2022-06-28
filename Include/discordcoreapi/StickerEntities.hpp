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
/// StickerEntities.hpp - Header for the sticker related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file StickerEntities.hpp

#pragma once

#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	/// For getting a Sticker object for the given sticker ID. \brief For getting a Sticker object for the given sticker ID.
	struct DiscordCoreAPI_Dll GetStickerData {
		std::string stickerId{};///< The chosen Sticker's Id.
	};

	/// For collecting a list of Stickers from a chosen Guild. \brief For collecting a list of Stickers from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildStickersData {
		uint64_t guildId{};///< The chosen Guild from which you would like to collect the Stickers from.
	};

	/// For creating a single Sticker. \brief For creating a single Sticker.
	struct DiscordCoreAPI_Dll CreateGuildStickerData {
		std::vector<uint8_t> file{};///< The sticker file to upload, must be a Png, Apng, or Lottie JSON file, max 500 KB.
		std::string description{};///< Description of the Sticker.
		uint64_t guildId{};///< The Guild within which to create the Sticker.
		std::string reason{};///< The reason for creating the Sticker.
		std::string name{};///< Name of the Sticker.
		std::string tags{};///< Autocomplete / suggestion tags for the sticker(max 200 characters).
	};

	/// For modifying a single Sticker. \brief For modifying a single Sticker.
	struct DiscordCoreAPI_Dll ModifyGuildStickerData {
		std::string description{};///< Description of the Sticker.
		std::string stickerId{};///< The Sticker you wish to modify.
		uint64_t guildId{};///< The Guild within which to modify the Sticker.
		std::string reason{};///< The reason for modifying the Sticker.
		std::string name{};///< Name of the Sticker.
		std::string tags{};///< Autocomplete / suggestion tags for the sticker(max 200 characters).
	};

	/// For deleting a single Sticker. \brief For deleting a single Sticker.
	struct DiscordCoreAPI_Dll DeleteGuildStickerData {
		std::string stickerId{};///< The Sticker you wish to delete.
		uint64_t guildId{};///< The Guild within which to delete the Sticker.
		std::string reason{};///< The reason for deleting the Sticker.
	};

	/// A single Sticker. \brief A single Sticker.
	class DiscordCoreAPI_Dll Sticker : public StickerData {
	  public:
		Sticker() = default;

		Sticker& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		Sticker(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~Sticker() = default;

		inline void parseObject(const nlohmann::json& jsonObjectData, Sticker* pDataStructure) {
			if (jsonObjectData.contains("asset") && !jsonObjectData["asset"].is_null()) {
				pDataStructure->asset = jsonObjectData["asset"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
				pDataStructure->formatType = jsonObjectData["format_type"].get<StickerFormatType>();
			}

			if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
				pDataStructure->stickerFlags = setBool<int32_t, StickerFlags>(pDataStructure->stickerFlags, StickerFlags::Available, jsonObjectData["available"].get<bool>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("pack_id") && !jsonObjectData["pack_id"].is_null()) {
				pDataStructure->packId = jsonObjectData["pack_id"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<StickerType>();
			}

			if (jsonObjectData.contains("sort_value") && !jsonObjectData["sort_value"].is_null()) {
				pDataStructure->sortValue = jsonObjectData["sort_value"].get<int32_t>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}
		}
	};

	class DiscordCoreAPI_Dll StickerVector {
	  public:

		StickerVector() = default;

		operator std::vector<Sticker>() {
			return this->theStickers;
		}

		StickerVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerVector() = default;

	  protected:
		std::vector<Sticker> theStickers{};

		inline void parseObject(const nlohmann::json& jsonObjectData, StickerVector* pDataStructure) {
			pDataStructure->theStickers.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::Sticker newData{ value };
				pDataStructure->theStickers.push_back(newData);
			}
			pDataStructure->theStickers.shrink_to_fit();
		}
	};


	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class DiscordCoreAPI_Dll for the Sticker related Discord endpoints. \brief An interface class DiscordCoreAPI_Dll for the Sticker related Discord endpoints.
	class DiscordCoreAPI_Dll Stickers {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// Gets a single Sticker item. \brief Gets a single Sticker item.
		/// \param dataPackage A GetStickerData structure.
		/// \returns A CoRoutine containing a Sticker.
		static CoRoutine<Sticker> getStickerAsync(GetStickerData dataPackage);

		/// Gets a list of nitro-available Sticker packs. \brief Gets a list of nitro-available Sticker packs
		/// \returns A CoRoutine containing a StickerPackDataVector.
		static CoRoutine<std::vector<StickerPackData>> getNitroStickerPacksAsync();

		/// Gets a list of Stickers from a Guild. \brief Gets a list of Stickers from a Guild.
		/// \param dataPackage A GetGuildStickersData structure.
		/// \returns A CoRoutine containing a StickerVector.
		static CoRoutine<std::vector<Sticker>> getGuildStickersAsync(GetGuildStickersData dataPackage);

		/// Creates a new Sticker within a chosen Guild. \brief Creates a new Sticker within a chosen Guild.
		/// \param dataPackage A CreateGuildStickerData structure.
		/// \returns A CoRoutine containing a Sticker.
		static CoRoutine<Sticker> createGuildStickerAsync(CreateGuildStickerData dataPackage);

		/// Modifies a Sticker within a chosen Guild. \brief Modifies a Sticker within a chosen Guild.
		/// \param dataPackage A ModifyGuildStickerData structure.
		/// \returns A CoRoutine containing a Sticker.
		static CoRoutine<Sticker> modifyGuildStickerAsync(ModifyGuildStickerData dataPackage);

		/// Deletes a Sticker within a chosen Guild. \brief Deletes a Sticker within a chosen Guild.
		/// \param dataPackage A DeleteGuildStickerData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildStickerAsync(DeleteGuildStickerData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
};// namespace DiscordCoreAPI
