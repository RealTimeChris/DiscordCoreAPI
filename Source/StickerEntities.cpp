/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
	/*
	void Stickers::initialize(DiscordCoreInternal::HttpsClient* client) {
		Stickers::httpsClient = client;
	}

	CoRoutine<Sticker> Stickers::getStickerAsync(GetStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/stickers/" + dataPackage.stickerId;
		workload.callStack = "Stickers::getStickerAsync()";
		Sticker returnData{};
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload, returnData);
	}

	CoRoutine<std::vector<StickerPackData>> Stickers::getNitroStickerPacksAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Nitro_Sticker_Packs };
		co_await NewThreadAwaitable<std::vector<StickerPackData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/sticker-packs";
		workload.callStack = "Stickers::getNitroStickerPacksAsync()";
		StickerPackDataVector returnData{};
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<StickerPackDataVector>(workload, returnData);
	}

	CoRoutine<std::vector<Sticker>> Stickers::getGuildStickersAsync(GetGuildStickersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Stickers };
		co_await NewThreadAwaitable<std::vector<Sticker>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
		workload.callStack = "Stickers::getGuildStickersAsync()";
		StickerVector returnData{};
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<StickerVector>(workload, returnData);
	}

	CoRoutine<Sticker> Stickers::createGuildStickerAsync(CreateGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
		EtfSerializer data{};
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		data["tags"] = dataPackage.tags;
		data["file"] = dataPackage.file;
		data.refreshString(SerializerSerializeType::Etf);
		workload.content = data.operator std::string();
		workload.callStack = "Stickers::createGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Sticker returnData{};
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload, returnData);
	}

	CoRoutine<Sticker> Stickers::modifyGuildStickerAsync(ModifyGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
		EtfSerializer data{};
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		data["tags"] = dataPackage.tags;
		data.refreshString(SerializerSerializeType::Etf);
		workload.content = data.operator std::string();
		workload.callStack = "Stickers::modifyGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Sticker returnData{};
		co_return Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(workload, returnData);
	}

	CoRoutine<void> Stickers::deleteGuildStickerAsync(DeleteGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Sticker };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
		workload.callStack = "Stickers::deleteGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Stickers::httpsClient->submitWorkloadAndGetResult(workload);
	}
	DiscordCoreInternal::HttpsClient* Stickers::httpsClient{ nullptr };*/
};
