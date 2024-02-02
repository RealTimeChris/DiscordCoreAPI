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

#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::create_guild_sticker_data> {
		using value_type				 = discord_core_api::create_guild_sticker_data;
		static constexpr auto parseValue = createValue("description", &value_type::description, "guildId", &value_type::guildId, "file",
			&value_type::file, "name", &value_type::name, "tags", &value_type::tags);
	};

	template<> struct core<discord_core_api::modify_guild_sticker_data> {
		using value_type				 = discord_core_api::modify_guild_sticker_data;
		static constexpr auto parseValue = createValue("description", &value_type::description, "stickerId", &value_type::stickerId, "guildId",
			&value_type::guildId, "name", &value_type::name, "tags", &value_type::tags);
	};

}

namespace discord_core_api {

	void stickers::initialize(discord_core_internal::https_client* client) {
		stickers::httpsClient = client;
	}

	co_routine<sticker_data> stickers::getStickerAsync(get_sticker_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Sticker };
		co_await newThreadAwaitable<sticker_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/stickers/" + dataPackage.stickerId;
		workload.callStack	   = "stickers::getStickerAsync()";
		sticker_data returnData{};
		stickers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<sticker_pack_data>> stickers::getNitroStickerPacksAsync() {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Nitro_Sticker_Packs };
		co_await newThreadAwaitable<jsonifier::vector<sticker_pack_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/sticker-packs";
		workload.callStack	   = "stickers::getNitroStickerPacksAsync()";
		jsonifier::vector<sticker_pack_data> returnData{};
		stickers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<sticker_data>> stickers::getGuildStickersAsync(get_guild_stickers_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Stickers };
		co_await newThreadAwaitable<jsonifier::vector<sticker_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/stickers";
		workload.callStack	   = "stickers::getGuildStickersAsync()";
		jsonifier::vector<sticker_data> returnData{};
		stickers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<sticker_data> stickers::createGuildStickerAsync(create_guild_sticker_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Sticker };
		co_await newThreadAwaitable<sticker_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/stickers";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "stickers::createGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		sticker_data returnData{};
		stickers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<sticker_data> stickers::modifyGuildStickerAsync(modify_guild_sticker_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Sticker };
		co_await newThreadAwaitable<sticker_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "stickers::modifyGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		sticker_data returnData{};
		stickers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> stickers::deleteGuildStickerAsync(delete_guild_sticker_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Sticker };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
		workload.callStack	   = "stickers::deleteGuildStickerAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		stickers::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	discord_core_internal::https_client* stickers::httpsClient{};
};
