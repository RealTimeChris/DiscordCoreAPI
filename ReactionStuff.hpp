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

	class Reaction : public ReactionData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Reaction>;
		friend class DiscordCoreInternal::ReactionManagerAgent;
		friend class DiscordCoreInternal::ReactionManager;
		friend struct OnReactionAddData;
		friend class DiscordCoreClient;

		Reaction() {};

		Reaction(ReactionData dataNew) {
			this->count = dataNew.count;
			this->me = dataNew.me;
			this->emoji = dataNew.emoji;
			this->userId = dataNew.userId;
			this->channelId = dataNew.channelId;
			this->messageId = dataNew.messageId;
			this->guildId = dataNew.guildId;
			this->member = dataNew.member;
			this->discordCoreClient = dataNew.discordCoreClient;
		}
	};

	struct CreateReactionData {
		string channelId{ "" };
		string messageId{ "" };
		string emojiName{ "" };
		string emojiId{ "" };
	};

	struct DeleteUserReactionData {
		string channelId{ "" };
		string messageId{ "" };
		string userId{ "" };
		string emojiName{ "" };
		string emojiId{ "" };
	};

	struct DeleteOwnReactionData {
		string channelId{ "" };
		string messageId{ "" };
		string emojiName{ "" };
		string emojiId{ "" };
	};

	struct DeleteReactionsByEmojiData {
		string channelId{ "" };
		string messageId{ "" };
		string emojiName{ "" };
		string emojiId{ "" };
	};

	struct DeleteAllReactionsData {
		string channelId{ "" };
		string messageId{ "" };
	};
};

namespace DiscordCoreInternal {

	class ReactionManagerAgent : agent {
	protected:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class ReactionManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<DeleteReactionDataAll> requestDeleteReactionBuffer{ nullptr };
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Reaction> outReactionBuffer{ nullptr };
		unbounded_buffer<PutReactionData> requestPutReactionBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		HttpAgentResources agentResources{};
		
		ReactionManagerAgent(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew)
			:agent(*ReactionManagerAgent::threadContext->scheduler->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		static void initialize(shared_ptr<ThreadContext> threadContextNew) {
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

		DiscordCoreAPI::Reaction putObjectData(PutReactionData dataPackage) {
			HttpWorkload workload;
			workload.workloadType = HttpWorkloadType::PUT_REACTION;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji + "/@me";
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManagerAgent::putObjectData_00");
			HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "this->putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "this->putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::ReactionData reactionData;
			reactionData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &reactionData);
			DiscordCoreAPI::Reaction reaction(reactionData);
			return reaction;
		}

		void deleteObjectData(DeleteReactionDataAll dataPackage) {
			HttpWorkload workload;
			workload.workloadType = HttpWorkloadType::DELETE_REACTION;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			if (dataPackage.deletionType == ReactionDeletionType::SELF_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/@me";
			}
			else if(dataPackage.deletionType == ReactionDeletionType::EMOJI_DELETE){
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji;
			}
			else if (dataPackage.deletionType == ReactionDeletionType::ALL_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions";
			}
			else if (dataPackage.deletionType == ReactionDeletionType::USER_DELETE) {
				workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.encodedEmoji + "/" + dataPackage.userId;
			}
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManagerAgent::deleteObjectData_00");
			HttpData returnData;
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
				PutReactionData dataPackage01;
				if (try_receive(this->requestPutReactionBuffer, dataPackage01)) {
					DiscordCoreAPI::Reaction reaction = this->putObjectData(dataPackage01);
					send(this->outReactionBuffer, reaction);
				}
				DeleteReactionDataAll dataPackage02;
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

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Reactions;

		ReactionManager(ReactionManager* pointer) {
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<ThreadContext> threadContext{ nullptr };
		HttpAgentResources agentResources{};

		ReactionManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<ThreadContext> threadContextNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
			this->threadContext = threadContextNew;
			return *this;
		}

		task<DiscordCoreAPI::Reaction> createReactionAsync(DiscordCoreAPI::CreateReactionData dataPackage){
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			PutReactionData dataPackageNew;
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
			DiscordCoreAPI::ReactionData reactionData;
			reactionData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Reaction reaction(reactionData);
			try_receive(requestAgent.outReactionBuffer, reaction);
			co_await mainThread;
			co_return reaction;
		}

		task<void> deleteUserReactionAsync(DiscordCoreAPI::DeleteUserReactionData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.deletionType = ReactionDeletionType::USER_DELETE;
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
			co_await mainThread;
			co_return;
		}

		task<void> deleteOwnReactionAsync(DiscordCoreAPI::DeleteOwnReactionData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = ReactionDeletionType::SELF_DELETE;
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
			co_await mainThread;
			co_return;
		}

		task<void> deleteReactionsByEmojiAsync(DiscordCoreAPI::DeleteReactionsByEmojiData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteReactionDataAll dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = ReactionDeletionType::EMOJI_DELETE;
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
			co_await mainThread;
			co_return;
		}

		task<void> deleteAllReactionsAsync(DiscordCoreAPI::DeleteAllReactionsData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DeleteReactionDataAll dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = ReactionDeletionType::ALL_DELETE;
			ReactionManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ReactionManager::deleteAllReactionsAsync");
			co_await mainThread;
			co_return;
		}

		~ReactionManager() {}
	};
	shared_ptr<ThreadContext> ReactionManagerAgent::threadContext{ nullptr };
}
#endif
