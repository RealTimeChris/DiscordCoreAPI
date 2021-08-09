// ReactionStuff.hpp - Reaction related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REACTION_STUFF_
#define _REACTION_STUFF_

#include "../pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class Reaction {
	public:

		DiscordCoreInternal::ReactionData data;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		Reaction() {};

	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;
		friend class  ReactionManagerAgent;

		Reaction(DiscordCoreInternal::ReactionData reactionData, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->data = reactionData;
			this->discordCoreClient = discordCoreClientNew;
		}
	};

	struct CreateReactionData {
		string channelId;
		string messageId;
		string emojiName;
		string emojiId;
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

	class ReactionManagerAgent : agent {
	protected:
		friend class DiscordCoreClient;
		friend class ReactionManager;
		friend class IndexHost;

		unbounded_buffer<DiscordCoreInternal::DeleteReactionDataAll> requestDeleteReactionBuffer;
		unbounded_buffer<DiscordCoreInternal::PutReactionData> requestPutReactionBuffer;
		unbounded_buffer<Reaction> outReactionBuffer;
		unbounded_buffer<exception> errorBuffer;

		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		ReactionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreClient> discordCoreClientNew)
			:agent(*ReactionManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
			ReactionManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			ReactionManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::ReactionManagerAgent Error: " << error.what() << endl << endl;
			}
			return;
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
			requestAgent.getError("ReactionManagerAgent::putObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "this->putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "this->putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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
			requestAgent.getError("ReactionManagerAgent::deleteObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "this->deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "this->deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void run() {
			try {
				DiscordCoreInternal::PutReactionData dataPackage01;
				if (try_receive(this->requestPutReactionBuffer, dataPackage01)) {
					Reaction reaction = this->putObjectData(dataPackage01);
					send(this->outReactionBuffer, reaction);
				}
				DiscordCoreInternal::DeleteReactionDataAll dataPackage02;
				if (try_receive(this->requestDeleteReactionBuffer, dataPackage02)) {
					this->deleteObjectData(dataPackage02);
				}
			}
			catch (const exception& error) {
				send(this->errorBuffer, error);
			}
			done();
		}
	};

	class ReactionManager {
	public:

		task<Reaction> createReactionAsync(CreateReactionData dataPackage){
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
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestPutReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManager::createReactionAsync");
			DiscordCoreInternal::ReactionData reactionData;
			Reaction reaction(reactionData, this->discordCoreClient);
			try_receive(requestAgent.outReactionBuffer, reaction);
			co_return reaction;
		}

		task<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage) {
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
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManager::deleteUserReactionAsync");
			co_return;
		}

		task<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage) {
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
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManager::deleteOwnReactionAsync");
			co_return;
		}

		task<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage) {
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
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManager::deleteReactionByEmojiAsync");
			co_return;
		}

		task<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteReactionDataAll dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = DiscordCoreInternal::ReactionDeletionType::ALL_DELETE;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManager::deleteAllReactionsAsync");
			co_return;
		}

		ReactionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		~ReactionManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class Message;
		friend class ReactionManagerAgent;
		friend class DiscordCoreClient;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
	};
	shared_ptr<DiscordCoreInternal::ThreadContext> ReactionManagerAgent::threadContext;
}
#endif
