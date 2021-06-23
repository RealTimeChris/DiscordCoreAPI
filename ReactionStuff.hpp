// ReactionStuff.hpp - Reaction related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REACTION_STUFF_
#define _REACTION_STUFF_

#include "../pch.h"
#include "MessageStuff.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class Reaction {
	public:

		DiscordCoreInternal::ReactionData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		Reaction() {};

		Reaction(DiscordCoreInternal::ReactionData reactionData, DiscordCoreClient* discordCoreClientNew) {
			this->data = reactionData;
			this->discordCoreClient = discordCoreClientNew;
		}

	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;
		friend class  ReactionManagerAgent;
	};

	struct CreateReactionData {
		string channelId;
		string messageId;
		string emojiName;
		string emojiId;
	};

	struct GetReactionData {
		string channelId;
		string messageId;
		string userId;
		string emojiName;
	};

	struct DeleteUserReactionData {
		string channelId;
		string messageId;
		string userId;
		string emojiName;
		string emojiId;
	};

	struct DeleteOwnReactionData {
		string channelId;
		string messageId;
		string emojiName;
		string emojiId;
	};
	
	struct DeleteReactionsByEmojiData {
		string channelId;
		string messageId;
		string emojiName;
		string emojiId;
	};

	struct DeleteAllReactionsData {
		string channelId;
		string messageId;
	};

	class ReactionManagerAgent : public agent {
	public:
		static overwrite_buffer<map<string, Reaction>> cache;
	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;

		static unbounded_buffer<DiscordCoreInternal::PutReactionData>* requestPutReactionBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* requestDeleteReactionBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetReactionData>* requestGetReactionBuffer;
		static unbounded_buffer<Reaction>* outReactionBuffer;
		static concurrent_queue<Reaction> reactionsToInsert;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient{ nullptr };

		ReactionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* coreClientNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = coreClientNew;
		}

		static void initialize() {
			ReactionManagerAgent::requestDeleteReactionBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>;
			ReactionManagerAgent::requestGetReactionBuffer = new unbounded_buffer<DiscordCoreInternal::GetReactionData>;
			ReactionManagerAgent::requestPutReactionBuffer = new unbounded_buffer<DiscordCoreInternal::PutReactionData>;
			ReactionManagerAgent::outReactionBuffer = new unbounded_buffer<Reaction>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		Reaction putObjectData(DiscordCoreInternal::PutReactionData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji + "/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManagerAgent::putObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ReactionManagerAgent::putObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ReactionManagerAgent::putObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ReactionData reactionData;
			DiscordCoreInternal::parseObject(returnData.data, &reactionData);
			Reaction reaction(reactionData, this->discordCoreClient);
			return reaction;
		}

		void deleteObjectData(DiscordCoreInternal::DeleteReactionDataAll dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_REACTION;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			if (dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::SELF_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/@me";
			}
			else if(dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::EMOJI_DELETE){
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji;
			}
			else if (dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::ALL_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
			}
			else if (dataPackage.deletionType == DiscordCoreInternal::ReactionDeletionType::USER_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/" + dataPackage.userId;
			}
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManagerAgent::deleteObject() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ReactionManagerAgent::deleteObject() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ReactionManagerAgent::deleteObject() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void run() {
			try {
				DiscordCoreInternal::PutReactionData dataPackage01;
				if (try_receive(ReactionManagerAgent::requestPutReactionBuffer, dataPackage01)) {
					Reaction reaction = this->putObjectData(dataPackage01);
					send(ReactionManagerAgent::outReactionBuffer, reaction);
				}
				DiscordCoreInternal::DeleteReactionDataAll dataPackage02;
				if (try_receive(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackage02)) {
					this->deleteObjectData(dataPackage02);
				}
				DiscordCoreInternal::GetReactionData dataPackage03;
				if (try_receive(ReactionManagerAgent::requestGetReactionBuffer, dataPackage03)) {
					map<string, Reaction> cacheTemp;
					if (try_receive(ReactionManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage03.messageId + dataPackage03.userId + dataPackage03.emojiName)) {
							Reaction reaction = cacheTemp.at(dataPackage03.messageId + dataPackage03.userId + dataPackage03.emojiName);
							send(ReactionManagerAgent::outReactionBuffer, reaction);
						}
					}
				}
				DiscordCoreInternal::ReactionData dataPackage04;
				Reaction reaction(dataPackage04, this->discordCoreClient);
				while (ReactionManagerAgent::reactionsToInsert.try_pop(reaction)) {
					map<string, Reaction> cacheTemp;
					if (try_receive(ReactionManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(reaction.data.messageId + reaction.data.userId + reaction.data.emoji.name)) {
							cacheTemp.erase(reaction.data.messageId + reaction.data.userId + reaction.data.emoji.name);
						}
					}
					cacheTemp.insert(make_pair(reaction.data.messageId + reaction.data.userId + reaction.data.emoji.name, reaction));
					asend(ReactionManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& error) {
				send(errorBuffer, error);
			}
			done();
		}
	};

	class ReactionManager {
	public:

		task<Reaction> createReactionAsync(CreateReactionData dataPackage){
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PutReactionData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.agentResources = this->agentResources;
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.emoji = output;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(ReactionManagerAgent::requestPutReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::createReactionAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->discordCoreClient);
			try_receive(ReactionManagerAgent::outReactionBuffer, reaction);
			this->threadContext->releaseGroup(groupIdNew);
			co_return reaction;
		}

		task<Reaction> getReactionAsync(GetReactionData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetReactionData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.emojiName = dataPackage.emojiName;
			dataPackageNew.userId = dataPackage.userId;
			ReactionManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(ReactionManagerAgent::requestGetReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::getReactionAsync() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->discordCoreClient);
			try_receive(ReactionManagerAgent::outReactionBuffer, reaction);
			this->threadContext->releaseGroup(groupIdNew);
			co_return reaction;
		}

		task<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::USER_DELETE;
			dataPackageNew.agentResources = this->agentResources;
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
				
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.encodedEmoji = output;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteUserReactionAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::SELF_DELETE;
			dataPackageNew.agentResources = this->agentResources;
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.encodedEmoji = output;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteOwnReactionAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::EMOJI_DELETE;
			dataPackageNew.agentResources = this->agentResources;
			string emoji;
			if (dataPackage.emojiId != string()) {
				emoji += ":" + dataPackage.emojiName + ":" + dataPackage.emojiId;
			}
			else {
				emoji = dataPackage.emojiName;
			}
			CURL* curl = curl_easy_init();
			char* output = nullptr;
			if (curl) {
				output = curl_easy_escape(curl, emoji.c_str(), 0);
			}
			dataPackageNew.encodedEmoji = output;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteReactionsByEmojiAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::ALL_DELETE;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->threadContext, this->discordCoreClient);
			send(ReactionManagerAgent::requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::deleteAllReactionsAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> insertReactionAsync(Reaction reaction) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			ReactionManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			ReactionManagerAgent::reactionsToInsert.push(reaction);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ReactionManager::insertReactionAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeReactionAsync(string messageId, string userId, string emojiName) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Reaction> cache;
			try_receive(ReactionManagerAgent::cache, cache);
			if (cache.contains(messageId + userId +emojiName)) {
				cache.erase(messageId + userId + emojiName);
			}
			asend(ReactionManagerAgent::cache, cache);
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

	protected:
		friend class Message;
		friend class ReactionManagerAgent;
		friend class DiscordCoreClient;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient{ nullptr };
		unsigned int groupId;

		ReactionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
			this->groupId = this->threadContext->createGroup();
		}

		~ReactionManager() {
			this->threadContext->releaseGroup(this->groupId);
		}
	};

	unbounded_buffer<DiscordCoreInternal::PutReactionData>* ReactionManagerAgent::requestPutReactionBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll>* ReactionManagerAgent::requestDeleteReactionBuffer;
	unbounded_buffer<DiscordCoreInternal::GetReactionData>* ReactionManagerAgent::requestGetReactionBuffer;
	unbounded_buffer<Reaction>* ReactionManagerAgent::outReactionBuffer;
	concurrent_queue<Reaction> ReactionManagerAgent::reactionsToInsert;
	overwrite_buffer<map<string, Reaction>> ReactionManagerAgent::cache;
}
#endif
