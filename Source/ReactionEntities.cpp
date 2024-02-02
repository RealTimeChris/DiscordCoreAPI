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
/// ReactionEntities.cpp - Source file for the reaction related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ReactionEntities.cpp

#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <fstream>

namespace jsonifier {

	template<> struct core<discord_core_api::create_guild_emoji_data> {
		using value_type				 = discord_core_api::create_guild_emoji_data;
		static constexpr auto parseValue = createValue("roles", &value_type::roles, "image", &value_type::imageDataFinal, "guildId",
			&value_type::guildId, "name", &value_type::name, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::modify_guild_emoji_data> {
		using value_type = discord_core_api::modify_guild_emoji_data;
		static constexpr auto parseValue =
			createValue("roles", &value_type::roles, "guildId", &value_type::guildId, "emojiId", &value_type::emojiId, "name", &value_type::name);
	};

}

namespace discord_core_api {

	template<> unordered_map<jsonifier::string, unbounded_message_block<reaction_data>*> object_collector<reaction_data>::objectsBuffersMap{};

	template<> object_collector<reaction_data>::object_collector() {
		collectorId										 = jsonifier::toString(std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count());
		object_collector::objectsBuffersMap[collectorId] = &objectsBuffer;
	};

	template<> void object_collector<reaction_data>::run(
		std::coroutine_handle<typename discord_core_api::co_routine<discord_core_api::object_collector<discord_core_api::reaction_data>::object_collector_return_data,
			false>::promise_type>& coroHandle) {
		int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count());
		int64_t elapsedTime{};
		while (elapsedTime < msToCollectFor && !coroHandle.promise().stopRequested()) {
			reaction_data message{};
			waitForTimeToPass<reaction_data>(objectsBuffer, message, static_cast<uint64_t>(msToCollectFor - static_cast<uint64_t>(elapsedTime)));
			if (filteringFunction(message)) {
				objectReturnData.objects.emplace_back(message);
			}
			if (static_cast<int32_t>(objectReturnData.objects.size()) >= quantityOfObjectsToCollect) {
				break;
			}

			elapsedTime = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count() - startingTime;
		}
	}

	template<> co_routine<object_collector<reaction_data>::object_collector_return_data, false> object_collector<reaction_data>::collectObjects(int32_t quantityToCollect,
		int32_t msToCollectForNew, object_filter<reaction_data> filteringFunctionNew) {
		auto coroHandle			   = co_await newThreadAwaitable<object_collector_return_data, false>();
		quantityOfObjectsToCollect = quantityToCollect;
		filteringFunction		   = filteringFunctionNew;
		msToCollectFor			   = msToCollectForNew;

		run(coroHandle);
		co_return std::move(objectReturnData);
	}

	template<> object_collector<reaction_data>::~object_collector() {
		if (object_collector::objectsBuffersMap.contains(collectorId)) {
			object_collector::objectsBuffersMap.erase(collectorId);
		}
	};

	void reactions::initialize(discord_core_internal::https_client* client) {
		reactions::httpsClient = client;
	}

	co_routine<reaction_data> reactions::createReactionAsync(create_reaction_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Reaction };
		co_await newThreadAwaitable<reaction_data>();
		jsonifier::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/@me";
		workload.callStack	   = "reactions::createReactionAsync()";
		reaction_data returnData{};
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> reactions::deleteOwnReactionAsync(delete_own_reaction_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Own_Reaction };
		co_await newThreadAwaitable<void>();
		jsonifier::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/@me";
		workload.callStack	   = "reactions::deleteOwnReactionAsync()";
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> reactions::deleteUserReactionAsync(delete_user_reaction_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_User_Reaction };
		co_await newThreadAwaitable<void>();
		jsonifier::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;

		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/" + dataPackage.userId;
		workload.callStack	   = "reactions::deleteUserReactionAsync()";
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<user_data>> reactions::getReactionsAsync(get_reactions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Reactions };
		co_await newThreadAwaitable<jsonifier::vector<user_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji;
		if (dataPackage.afterId != 0) {
			workload.relativePath += "?after=" + dataPackage.afterId;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "reactions::getReactionsAsync()";
		jsonifier::vector<user_data> returnData{};
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}


	co_routine<void> reactions::deleteAllReactionsAsync(delete_all_reactions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_All_Reactions };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
		workload.callStack	   = "reactions::deleteAllReactionsAsync()";
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> reactions::deleteReactionsByEmojiAsync(delete_reactions_by_emoji_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Reactions_By_Emoji };
		co_await newThreadAwaitable<void>();
		jsonifier::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji);
		workload.callStack	   = "reactions::deleteReactionsByEmojiAsync()";
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<emoji_data>> reactions::getEmojiListAsync(get_emoji_list_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Emoji_List };
		co_await newThreadAwaitable<jsonifier::vector<emoji_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/emojis";
		workload.callStack	   = "reactions::getEmojiListAsync()";
		jsonifier::vector<emoji_data> returnData{};
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<emoji_data> reactions::getGuildEmojiAsync(get_guild_emoji_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Emoji };
		co_await newThreadAwaitable<emoji_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		workload.callStack	   = "reactions::getGuildEmojiAsync()";
		emoji_data returnData{};
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<emoji_data> reactions::createGuildEmojiAsync(create_guild_emoji_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Emoji };
		co_await newThreadAwaitable<emoji_data>();
		workload.workloadClass		= discord_core_internal::https_workload_class::Post;
		jsonifier::string newerFile = base64Encode(loadFileContents(dataPackage.imageFilePath));
		switch (dataPackage.type) {
			case image_type::jpg: {
				dataPackage.imageDataFinal = "data:image/jpeg;base64,";
				dataPackage.imageDataFinal.insert(dataPackage.imageDataFinal.end(), newerFile.begin(), newerFile.end());
				break;
			}
			case image_type::png: {
				dataPackage.imageDataFinal = "data:image/png;base64,";
				dataPackage.imageDataFinal.insert(dataPackage.imageDataFinal.end(), newerFile.begin(), newerFile.end());
				break;
			}
			case image_type::gif: {
				dataPackage.imageDataFinal = "data:image/gif;base64,";
				dataPackage.imageDataFinal.insert(dataPackage.imageDataFinal.end(), newerFile.begin(), newerFile.end());
				break;
			}
		}
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "reactions::createGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		emoji_data returnData{};
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<emoji_data> reactions::modifyGuildEmojiAsync(modify_guild_emoji_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Emoji };
		co_await newThreadAwaitable<emoji_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "reactions::modifyGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		emoji_data returnData{};
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> reactions::deleteGuildEmojiAsync(delete_guild_emoji_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Emoji };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		workload.callStack	   = "reactions::deleteGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		reactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	discord_core_internal::https_client* reactions::httpsClient{};
}
