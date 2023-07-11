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
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>
#include <fstream>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::CreateGuildEmojiData> {
		using ValueType = DiscordCoreAPI::CreateGuildEmojiData;
		static constexpr auto parseValue = object("roles", &ValueType::roles, "imageFilePath", &ValueType::imageFilePath, "reason",
			&ValueType::reason, "guildId", &ValueType::guildId, "name", &ValueType::name, "type", &ValueType::type);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildEmojiData> {
		using ValueType = DiscordCoreAPI::ModifyGuildEmojiData;
		static constexpr auto parseValue = object("roles", &ValueType::roles, "reason", &ValueType::reason, "guildId", &ValueType::guildId, "emojiId",
			&ValueType::emojiId, "name", &ValueType::name);
	};

}

namespace DiscordCoreAPI {



	void Reactions::initialize(DiscordCoreInternal::HttpsClient* client) {
		Reactions::httpsClient = client;
	}

	CoRoutine<Reaction> Reactions::createReactionAsync(CreateReactionData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Reaction };
		co_await NewThreadAwaitable<Reaction>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath =
			"/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/@me";
		workload.callStack = "Reactions::createReactionAsync()";
		Reaction returnData{};
		Reactions::httpsClient->submitWorkloadAndGetResult<Reaction>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<void> Reactions::deleteOwnReactionAsync(DeleteOwnReactionData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Own_Reaction };
		co_await NewThreadAwaitable<void>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath =
			"/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/@me";
		workload.callStack = "Reactions::deleteOwnReactionAsync()";
		Reactions::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<void> Reactions::deleteUserReactionAsync(DeleteUserReactionData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_User_Reaction };
		co_await NewThreadAwaitable<void>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;

		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath =
			"/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/" + dataPackage.userId;
		workload.callStack = "Reactions::deleteUserReactionAsync()";
		Reactions::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<UserVector> Reactions::getReactionsAsync(GetReactionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Reactions };
		co_await NewThreadAwaitable<UserVector>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji;
		if (dataPackage.afterId != 0) {
			workload.relativePath += "?after=" + dataPackage.afterId;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Reactions::getReactionsAsync()";
		UserVector returnData{};
		Reactions::httpsClient->submitWorkloadAndGetResult<UserVector>(std::move(workload), returnData);
		co_return std::move(returnData);
	}


	CoRoutine<void> Reactions::deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_All_Reactions };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
		workload.callStack = "Reactions::deleteAllReactionsAsync()";
		Reactions::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<void> Reactions::deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Reactions_By_Emoji };
		co_await NewThreadAwaitable<void>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji);
		workload.callStack = "Reactions::deleteReactionsByEmojiAsync()";
		Reactions::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<EmojiDataVector> Reactions::getEmojiListAsync(GetEmojiListData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Emoji_List };
		co_await NewThreadAwaitable<EmojiDataVector>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis";
		workload.callStack = "Reactions::getEmojiListAsync()";
		EmojiDataVector returnData{};
		Reactions::httpsClient->submitWorkloadAndGetResult<EmojiDataVector>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<EmojiData> Reactions::getGuildEmojiAsync(GetGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Emoji };
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		workload.callStack = "Reactions::getGuildEmojiAsync()";
		EmojiData returnData{};
		Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<EmojiData> Reactions::createGuildEmojiAsync(CreateGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Emoji };
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		std::ifstream fin(dataPackage.imageFilePath, std::ios::binary);
		fin.seekg(0, std::ios::end);
		std::string data{};
		data.resize(fin.tellg());
		fin.seekg(0, std::ios::beg);
		fin.read(data.data(), data.size());
		std::string newerFile = base64Encode(std::move(data));
		switch (dataPackage.type) {
			case ImageType::Jpg: {
				dataPackage.imageDataFinal = "data:image/jpeg;base64,";
				dataPackage.imageDataFinal.insert(dataPackage.imageDataFinal.end(), newerFile.begin(), newerFile.end());
				break;
			}
			case ImageType::Png: {
				dataPackage.imageDataFinal = "data:image/png;base64,";
				dataPackage.imageDataFinal.insert(dataPackage.imageDataFinal.end(), newerFile.begin(), newerFile.end());
				break;
			}
			case ImageType::Gif: {
				dataPackage.imageDataFinal = "data:image/gif;base64,";
				dataPackage.imageDataFinal.insert(dataPackage.imageDataFinal.end(), newerFile.begin(), newerFile.end());
				break;
			}
		}
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Reactions::createGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		EmojiData returnData{};
		Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<EmojiData> Reactions::modifyGuildEmojiAsync(ModifyGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Emoji };
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Reactions::modifyGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		EmojiData returnData{};
		Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<void> Reactions::deleteGuildEmojiAsync(DeleteGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Emoji };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		workload.callStack = "Reactions::deleteGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Reactions::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}
	DiscordCoreInternal::HttpsClient* Reactions::httpsClient{};
}
