/*
*
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
/// Chris M.
/// https://discordcoreapi.com
/// \file StickerEntities.cpp

#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/Http.hpp>

namespace DiscordCoreAPI {

	void Stickers::initialize(DiscordCoreInternal::HttpClient* theClient) {
		Stickers::httpClient = theClient;
	}

	CoRoutine<Sticker> Stickers::getStickerAsync(GetStickerData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Sticker);
			co_await NewThreadAwaitable<Sticker>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/stickers/" + dataPackage.stickerId;
			workload.callStack = "Stickers::getStickerAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Sticker>(*Stickers::httpClient, workload);
		} catch (...) {
			reportException("Stickers::getStickerAsync()");
		}
	}

	CoRoutine<std::vector<StickerPackData>> Stickers::getNitroStickerPacksAsync() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Nitro_Sticker_Packs);
			co_await NewThreadAwaitable<std::vector<StickerPackData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Nitro_Sticker_Packs;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/sticker-packs";
			workload.callStack = "Stickers::getNitroStickerPacksAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<StickerPackData>>(*Stickers::httpClient, workload);
		} catch (...) {
			reportException("Stickers::getNitroStickerPacksAsync()");
		}
	}

	CoRoutine<std::vector<Sticker>> Stickers::getGuildStickersAsync(GetGuildStickersData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Stickers);
			co_await NewThreadAwaitable<std::vector<Sticker>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Stickers;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
			workload.callStack = "Stickers::getGuildStickersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Sticker>>(*Stickers::httpClient, workload);
		} catch (...) {
			reportException("Stickers::getGuildStickersAsync()");
		}
	}

	CoRoutine<Sticker> Stickers::createGuildStickerAsync(CreateGuildStickerData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Sticker);
			co_await NewThreadAwaitable<Sticker>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
			nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name }, { "tags", dataPackage.tags }, { "file", dataPackage.file } };
			workload.content = responseData.dump();
			workload.callStack = "Stickers::createGuildStickerAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Sticker>(*Stickers::httpClient, workload);
		} catch (...) {
			reportException("Stickers::createGuildStickerAsync()");
		}
	}

	CoRoutine<Sticker> Stickers::modifyGuildStickerAsync(ModifyGuildStickerData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Sticker);
			co_await NewThreadAwaitable<Sticker>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
			nlohmann::json responseData = { { "description", dataPackage.description }, { "name", dataPackage.name }, { "tags", dataPackage.tags } };
			workload.content = responseData.dump();
			workload.callStack = "Stickers::modifyGuildStickerAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Sticker>(*Stickers::httpClient, workload);
		} catch (...) {
			reportException("Stickers::modifyGuildStickerAsync()");
		}
	}

	CoRoutine<void> Stickers::deleteGuildStickerAsync(DeleteGuildStickerData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Sticker);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
			workload.callStack = "Stickers::deleteGuildStickerAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Stickers::httpClient, workload);
		} catch (...) {
			reportException("Stickers::deleteGuildStickerAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* Stickers::httpClient{ nullptr };
};
