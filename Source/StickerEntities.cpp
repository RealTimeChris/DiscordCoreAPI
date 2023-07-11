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
/// StickerEntities.cpp - Source file for the sticker related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file StickerEntities.cpp

#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::CreateGuildStickerData> {
		using ValueType = DiscordCoreAPI::CreateGuildStickerData;
		static constexpr auto parseValue = object("description", &ValueType::description, "reason", &ValueType::reason, "guildId",
			&ValueType::guildId, "file", &ValueType::file, "name", &ValueType::name, "tags", &ValueType::tags);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildStickerData> {
		using ValueType = DiscordCoreAPI::ModifyGuildStickerData;
		static constexpr auto parseValue = object("description", &ValueType::description, "stickerId", &ValueType::stickerId, "reason",
			&ValueType::reason, "guildId", &ValueType::guildId, "name", &ValueType::name, "tags", &ValueType::tags);
	};

}

namespace DiscordCoreAPI {



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
		Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<std::vector<StickerPackData>> Stickers::getNitroStickerPacksAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Nitro_Sticker_Packs };
		co_await NewThreadAwaitable<std::vector<StickerPackData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/sticker-packs";
		workload.callStack = "Stickers::getNitroStickerPacksAsync()";
		StickerPackDataVector returnData{};
		Stickers::httpsClient->submitWorkloadAndGetResult<StickerPackDataVector>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<std::vector<Sticker>> Stickers::getGuildStickersAsync(GetGuildStickersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Stickers };
		co_await NewThreadAwaitable<std::vector<Sticker>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
		workload.callStack = "Stickers::getGuildStickersAsync()";
		StickerVector returnData{};
		Stickers::httpsClient->submitWorkloadAndGetResult<StickerVector>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<Sticker> Stickers::createGuildStickerAsync(CreateGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Stickers::createGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Sticker returnData{};
		Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<Sticker> Stickers::modifyGuildStickerAsync(ModifyGuildStickerData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Sticker };
		co_await NewThreadAwaitable<Sticker>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Stickers::modifyGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Sticker returnData{};
		Stickers::httpsClient->submitWorkloadAndGetResult<Sticker>(std::move(workload), returnData);
		co_return std::move(returnData);
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
		Stickers::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	DiscordCoreInternal::HttpsClient* Stickers::httpsClient{};
};
