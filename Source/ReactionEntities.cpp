/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// ReactionEntities.cpp - Source file for the reaction related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <ReactionEntities.hpp>
#include <cpp-base64/base64.h>
#include <CoRoutine.hpp>
#include <Http.hpp>
#include <fstream>

namespace DiscordCoreAPI {

	Reaction::Reaction(ReactionData dataNew) {
		this->channelId = dataNew.channelId;
		this->messageId = dataNew.messageId;
		this->guildId = dataNew.guildId;
		this->userId = dataNew.userId;
		this->member = dataNew.member;
		this->count = dataNew.count;
		this->emoji = dataNew.emoji;
		this->me = dataNew.me;
		this->id = dataNew.id;
	}

	void Reactions::initialize(DiscordCoreInternal::HttpClient*theClient) {
		Reactions::httpClient = theClient;
	}

	CoRoutine<Reaction> Reactions::createReactionAsync(CreateReactionData dataPackage) {
		try {
			co_await NewThreadAwaitable<Reaction>();
			std::string emoji;
			if (dataPackage.emojiId != std::string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Reaction;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/@me";
			workload.callStack = "Reactions::createReactionAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Reaction>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::createReactionAsync()");
		}
	}

	CoRoutine<void> Reactions::deleteOwnReactionAsync(DeleteOwnReactionData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			std::string emoji;
			if (dataPackage.emojiId != std::string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Own_Reaction;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/@me";
			workload.callStack = "Reactions::deleteOwnReactionAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::deleteOwnReactionAsync()");
		}
	}

	CoRoutine<void> Reactions::deleteUserReactionAsync(DeleteUserReactionData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			std::string emoji;
			if (dataPackage.emojiId != std::string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;

			}
			else {
				emoji = dataPackage.emojiName;
			}
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_User_Reaction;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji) + "/" + dataPackage.userId;
			workload.callStack = "Reactions::deleteUserReactionAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::deleteUserReactionAsync()");
		}
	}

	CoRoutine<std::vector<User>> Reactions::getReactionsAsync(GetReactionsData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<User>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Reactions;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji;
			if (dataPackage.afterId != "") {
				workload.relativePath += "?after=" + dataPackage.afterId;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "Reactions::getReactionsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<User>>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::getReactionsAsync()");
		}
	}


	CoRoutine<void> Reactions::deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_All_Reactions;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
			workload.callStack = "Reactions::deleteAllReactionsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::deleteAllReactionsAsync()");
		}
	}

	CoRoutine<void> Reactions::deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			std::string emoji;
			if (dataPackage.emojiId != std::string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Reactions_By_Emoji;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + urlEncode(emoji);
			workload.callStack = "Reactions::deleteReactionsByEmojiAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::deleteReactionsByEmojiAsync()");
		}
	}

	CoRoutine<std::vector<EmojiData>> Reactions::getEmojiListAsync(GetEmojiListData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<EmojiData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Emoji_List;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis";
			workload.callStack = "Reactions::getEmojiListAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<EmojiData>>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::getEmojiListAsync()");
		}
	}

	CoRoutine<EmojiData> Reactions::getGuildEmojiAsync(GetGuildEmojiData dataPackage) {
		try {
			co_await NewThreadAwaitable<EmojiData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Emoji;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
			workload.callStack = "Reactions::getGuildEmojiAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<EmojiData>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::getGuildEmojiAsync()");
		}
	}

	CoRoutine<EmojiData> Reactions::createGuildEmojiAsync(CreateGuildEmojiData dataPackage) {
		try {
			co_await NewThreadAwaitable<EmojiData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Emoji;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			std::fstream newFile{ dataPackage.imageFilePath };
			std::string theFile{};
			char theChar{};
			while (newFile.get(theChar)) {
				theFile.push_back(theChar);
			}
			std::string newerFile = base64_encode_pem(theFile);
			switch (dataPackage.type) {
			case ImageType::Jpg: {
				dataPackage.imageDataFinal = "data:image/jpeg;base64,";
				dataPackage.imageDataFinal.insert(dataPackage.imageDataFinal.end(), newerFile.begin() + 3, newerFile.end() - 3);
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
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Reactions::createGuildEmojiAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<EmojiData>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::createGuildEmojiAsync()");
		}
	}

	CoRoutine<EmojiData> Reactions::modifyGuildEmojiAsync(ModifyGuildEmojiData dataPackage) {
		try {
			co_await NewThreadAwaitable<EmojiData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Emoji;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Reactions::modifyGuildEmojiAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<EmojiData>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::modifyGuildEmojiAsync()");
		}
	}

	CoRoutine<void> Reactions::deleteGuildEmojiAsync(DeleteGuildEmojiData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Emoji;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/emojis/" + dataPackage.emojiId;
			workload.callStack = "Reactions::deleteGuildEmojiAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Reactions::httpClient, workload);
		}
		catch (...) {
			reportException("Reactions::deleteGuildEmojiAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* Reactions::httpClient{ nullptr };
}
