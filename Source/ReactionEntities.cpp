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
/// ReactionEntities.cpp - Source file for the reaction related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ReactionEntities.cpp

#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>
#include <fstream>

namespace DiscordCoreAPI {

	void Reactions::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		Reactions::httpsClient = theClient;
	}

	CoRoutine<Reaction> Reactions::createReactionAsync(CreateReactionData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Put_Reaction);
		co_await NewThreadAwaitable<Reaction>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + std::to_string(dataPackage.emojiId);
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Put_Reaction;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath =
			"/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/reactions/" + urlEncode(emoji) + "/@me";
		workload.callStack = "Reactions::createReactionAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<Reaction>(workload);
	}

	CoRoutine<void> Reactions::deleteOwnReactionAsync(DeleteOwnReactionData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Own_Reaction);
		co_await NewThreadAwaitable<void>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + std::to_string(dataPackage.emojiId);
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Own_Reaction;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath =
			"/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/reactions/" + urlEncode(emoji) + "/@me";
		workload.callStack = "Reactions::deleteOwnReactionAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Reactions::deleteUserReactionAsync(DeleteUserReactionData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_User_Reaction);
		co_await NewThreadAwaitable<void>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + std::to_string(dataPackage.emojiId);

		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_User_Reaction;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/reactions/" + urlEncode(emoji) +
			"/" + std::to_string(dataPackage.userId);
		workload.callStack = "Reactions::deleteUserReactionAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<UserVector> Reactions::getReactionsAsync(GetReactionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Reactions);
		co_await NewThreadAwaitable<UserVector>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Reactions;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/reactions/" + dataPackage.emoji;
		if (dataPackage.afterId != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.afterId);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Reactions::getReactionsAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<UserVector>(workload);
	}


	CoRoutine<void> Reactions::deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_All_Reactions);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_All_Reactions;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/reactions";
		workload.callStack = "Reactions::deleteAllReactionsAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Reactions::deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Reactions_By_Emoji);
		co_await NewThreadAwaitable<void>();
		std::string emoji;
		if (dataPackage.emojiId != 0) {
			emoji += ":" + dataPackage.emojiName + ":" + std::to_string(dataPackage.emojiId);
		} else {
			emoji = dataPackage.emojiName;
		}
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Reactions_By_Emoji;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/reactions/" + urlEncode(emoji);
		workload.callStack = "Reactions::deleteReactionsByEmojiAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<EmojiDataVector> Reactions::getEmojiListAsync(GetEmojiListData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Emoji_List);
		co_await NewThreadAwaitable<EmojiDataVector>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Emoji_List;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/emojis";
		workload.callStack = "Reactions::getEmojiListAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiDataVector>(workload);
	}

	CoRoutine<EmojiData> Reactions::getGuildEmojiAsync(GetGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Emoji);
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Emoji;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/emojis/" + dataPackage.emojiId;
		workload.callStack = "Reactions::getGuildEmojiAsync";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(workload);
	}

	CoRoutine<EmojiData> Reactions::createGuildEmojiAsync(CreateGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Emoji);
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Emoji;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		std::ifstream fin(dataPackage.imageFilePath, std::ios::binary);
		fin.seekg(0, std::ios::end);
		std::string data{};
		data.resize(fin.tellg());
		fin.seekg(0, std::ios::beg);
		fin.read(data.data(), data.size());
		std::string newerFile = base64Encode(data);
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
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/emojis";
		workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage);
		workload.callStack = "Reactions::createGuildEmojiAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(workload);
	}

	CoRoutine<EmojiData> Reactions::modifyGuildEmojiAsync(ModifyGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Emoji);
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Emoji;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/emojis/" + std::to_string(dataPackage.emojiId);
		workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage);
		workload.callStack = "Reactions::modifyGuildEmojiAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(workload);
	}

	CoRoutine<void> Reactions::deleteGuildEmojiAsync(DeleteGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Emoji);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Emoji;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/emojis/" + std::to_string(dataPackage.emojiId);
		workload.callStack = "Reactions::deleteGuildEmojiAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}
	DiscordCoreInternal::HttpsClient* Reactions::httpsClient{ nullptr };
}
