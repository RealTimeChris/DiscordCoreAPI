/*
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
/// StickerEntities.cpp - Source file for the sticker related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file StickerEntities.cpp

#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	Sticker& Sticker::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	Sticker::Sticker(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerVector::operator std::vector<Sticker>() {
		return this->theStickers;
	}

	StickerVector& StickerVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	StickerVector::StickerVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	void Stickers::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		Stickers::httpsClient = theClient;
	}

	CoRoutine<Sticker> Stickers::getStickerAsync(GetStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Sticker);
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Sticker;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/stickers/" + dataPackage.stickerId;
		workload.callStack = "Stickers::getStickerAsync()";
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload);
	}

	CoRoutine<std::vector<StickerPackData>> Stickers::getNitroStickerPacksAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Nitro_Sticker_Packs);
		co_await NewThreadAwaitable<std::vector<StickerPackData>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Nitro_Sticker_Packs;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/sticker-packs";
		workload.callStack = "Stickers::getNitroStickerPacksAsync()";
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<StickerPackDataVector>(workload);
	}

	CoRoutine<std::vector<Sticker>> Stickers::getGuildStickersAsync(GetGuildStickersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Stickers);
		co_await NewThreadAwaitable<std::vector<Sticker>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Stickers;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers";
		workload.callStack = "Stickers::getGuildStickersAsync()";
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<StickerVector>(workload);
	}

	CoRoutine<Sticker> Stickers::createGuildStickerAsync(CreateGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Sticker);
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Sticker;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers";
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name }, { "tags", dataPackage.tags }, { "file", dataPackage.file } };
		workload.content = responseData.dump();
		workload.callStack = "Stickers::createGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload);
	}

	CoRoutine<Sticker> Stickers::modifyGuildStickerAsync(ModifyGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Sticker);
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Sticker;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers/" + std::to_string(dataPackage.stickerId);
		nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name }, { "tags", dataPackage.tags } };
		workload.content = responseData.dump();
		workload.callStack = "Stickers::modifyGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload);
	}

	CoRoutine<void> Stickers::deleteGuildStickerAsync(DeleteGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Sticker);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Sticker;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/stickers/" + std::to_string(dataPackage.stickerId);
		workload.callStack = "Stickers::deleteGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}
	DiscordCoreInternal::HttpsClient* Stickers::httpsClient{ nullptr };
};
