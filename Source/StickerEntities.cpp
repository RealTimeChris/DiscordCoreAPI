/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// StickerEntities.cpp - Source file for the sticker related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file StickerEntities.cpp

#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	Sticker::Sticker(simdjson::ondemand::value jsonObjectData) {
		this->asset = getString(jsonObjectData, "asset");

		this->description = getString(jsonObjectData, "description");

		this->formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		this->stickerFlags = setBool(this->stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->packId = getString(jsonObjectData, "pack_id");

		this->type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		this->sortValue = getUint32(jsonObjectData, "sort_value");

		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}
	}

	StickerVector::StickerVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Sticker newData{ value.value() };
					this->theStickers.emplace_back(std::move(newData));
				}
			}
		}
	}

	StickerVector::operator Vector<Sticker>() {
		return this->theStickers;
	}

	Void Stickers::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		Stickers::httpsClient = theClient;
	}

	CoRoutine<Sticker> Stickers::getStickerAsync(GetStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/stickers/" + dataPackage.stickerId;
		workload.callStack = "Stickers::getStickerAsync()";
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload);
	}

	CoRoutine<Vector<StickerPackData>> Stickers::getNitroStickerPacksAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Nitro_Sticker_Packs };
		co_await NewThreadAwaitable<Vector<StickerPackData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/sticker-packs";
		workload.callStack = "Stickers::getNitroStickerPacksAsync()";
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<StickerPackDataVector>(workload);
	}

	CoRoutine<Vector<Sticker>> Stickers::getGuildStickersAsync(GetGuildStickersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Stickers };
		co_await NewThreadAwaitable<Vector<Sticker>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers";
		workload.callStack = "Stickers::getGuildStickersAsync()";
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<StickerVector>(workload);
	}

	CoRoutine<Sticker> Stickers::createGuildStickerAsync(CreateGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers";
		JsonObject theData{};
		theData["description"] = dataPackage.description;
		theData["name"] = dataPackage.name;
		theData["tags"] = dataPackage.tags;
		theData["file"] = dataPackage.file;
		workload.content = theData;
		workload.callStack = "Stickers::createGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload);
	}

	CoRoutine<Sticker> Stickers::modifyGuildStickerAsync(ModifyGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers/" + std::to_string(dataPackage.stickerId);
		JsonObject theData{};
		theData["description"] = dataPackage.description;
		theData["name"] = dataPackage.name;
		theData["tags"] = dataPackage.tags;
		workload.content = theData;
		workload.callStack = "Stickers::modifyGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload);
	}

	CoRoutine<Void> Stickers::deleteGuildStickerAsync(DeleteGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Sticker };
		co_await NewThreadAwaitable<Void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers/" + std::to_string(dataPackage.stickerId);
		workload.callStack = "Stickers::deleteGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Void>(workload);
	}
	DiscordCoreInternal::HttpsClient* Stickers::httpsClient{ nullptr };
};
