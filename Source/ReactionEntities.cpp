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
/// ReactionEntities.cpp - Source file for the reaction related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ReactionEntities.cpp

#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>
#include <fstream>

namespace DiscordCoreAPI {
	/*
	CreateGuildEmojiData::operator EtfSerializer() {
		EtfSerializer data{};
		for (auto& value: this->roles) {
			data["roles"].emplaceBack(value.operator std::string());
		}
		data["images"] = this->imageDataFinal;
		data["name"] = this->name;
		return data;
	}

	ModifyGuildEmojiData::operator EtfSerializer() {
		EtfSerializer data{};
		for (auto& value: this->roles) {
			data["roles"].emplaceBack(value.operator std::string());
		}
		data["name"] = this->name;
		return data;
	}
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
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<Reaction>(workload, returnData);
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
		co_return Reactions::httpsClient->submitWorkloadAndGetResult(workload);
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
		co_return Reactions::httpsClient->submitWorkloadAndGetResult(workload);
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
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<UserVector>(workload, returnData);
	}


	CoRoutine<void> Reactions::deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_All_Reactions };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
		workload.callStack = "Reactions::deleteAllReactionsAsync()";
		co_return Reactions::httpsClient->submitWorkloadAndGetResult(workload);
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
		co_return Reactions::httpsClient->submitWorkloadAndGetResult(workload);
	}

	CoRoutine<EmojiDataVector> Reactions::getEmojiListAsync(GetEmojiListData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Emoji_List };
		co_await NewThreadAwaitable<EmojiDataVector>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis";
		workload.callStack = "Reactions::getEmojiListAsync()";
		EmojiDataVector returnData{};
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiDataVector>(workload, returnData);
	}

	CoRoutine<EmojiData> Reactions::getGuildEmojiAsync(GetGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Emoji };
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		workload.callStack = "Reactions::getGuildEmojiAsync()";
		EmojiData returnData{};
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(workload, returnData);
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
		Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Reactions::createGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		EmojiData returnData{};
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(workload, returnData);
	}

	CoRoutine<EmojiData> Reactions::modifyGuildEmojiAsync(ModifyGuildEmojiData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Emoji };
		co_await NewThreadAwaitable<EmojiData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
		Jsonifier::JsonifierCore parser{};
				parser.serializeJson(dataPackage, workload.content);		workload.callStack = "Reactions::modifyGuildEmojiAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		EmojiData returnData{};
		co_return Reactions::httpsClient->submitWorkloadAndGetResult<EmojiData>(workload, returnData);
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
		co_return Reactions::httpsClient->submitWorkloadAndGetResult(workload);
	}
	DiscordCoreInternal::HttpsClient* Reactions::httpsClient{ nullptr };*/
}
