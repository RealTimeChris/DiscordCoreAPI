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
			this->discordCoreClient = dataNew.discordCoreClient;
			this->channelId = dataNew.channelId;
			this->messageId = dataNew.messageId;
			this->guildId = dataNew.guildId;
			this->userId = dataNew.userId;
			this->member = dataNew.member;
			this->count = dataNew.count;
			this->emoji = dataNew.emoji;
			this->me = dataNew.me;
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
		string emojiName{ "" };
		string emojiId{ "" };
		string userId{ "" };
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

		static shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient;
		static shared_ptr<ThreadContext> threadContext;
		static HttpAgentResources agentResources;

		unbounded_buffer<DeleteReactionDataAll> requestDeleteReactionBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Reaction> outReactionBuffer{ nullptr };
		unbounded_buffer<PutReactionData> requestPutReactionBuffer{ nullptr };
		
		ReactionManagerAgent()
			:agent(*ReactionManagerAgent::threadContext->scheduler->scheduler) {}

		static void initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			ReactionManagerAgent::threadContext = ThreadManager::getThreadContext().get();
			ReactionManagerAgent::discordCoreClient = discordCoreClientNew;
			ReactionManagerAgent::agentResources = agentResourcesNew;
		}

		static void cleanup() {
			ReactionManagerAgent::threadContext->releaseGroup();
		}

		DiscordCoreAPI::Reaction putObjectData(PutReactionData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadType = HttpWorkloadType::PUT_REACTION;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/reactions/" + dataPackage.emoji + "/@me";
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ReactionManagerAgent::putObjectData_00");
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
			HttpWorkloadData workload;
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
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ReactionManagerAgent::deleteObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "this->deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "this->deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
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
			catch (...) {
				DiscordCoreAPI::rethrowException("ReactionManagerAgent::run() Error: ");
			}
			this->done();
		}
	};

	class ReactionManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Reactions;

		ReactionManager(ReactionManager* pointer)
			: ThreadContext(*ThreadManager::getThreadContext().get()) {
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		HttpAgentResources agentResources{};

		ReactionManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->agentResources = agentResourcesNew;
			return *this;
		}

		task<DiscordCoreAPI::Reaction> createReactionAsync(DiscordCoreAPI::CreateReactionData dataPackage){
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
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
			ReactionManagerAgent requestAgent{};
			send(requestAgent.requestPutReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::ReactionData reactionData;
			reactionData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Reaction reaction(reactionData);
			try_receive(requestAgent.outReactionBuffer, reaction);
			co_await mainThread;
			co_return reaction;
		}

		task<void> deleteUserReactionAsync(DiscordCoreAPI::DeleteUserReactionData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
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
			ReactionManagerAgent requestAgent{};
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> deleteOwnReactionAsync(DiscordCoreAPI::DeleteOwnReactionData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
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
			ReactionManagerAgent requestAgent{};
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> deleteReactionsByEmojiAsync(DiscordCoreAPI::DeleteReactionsByEmojiData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
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
			ReactionManagerAgent requestAgent{};
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> deleteAllReactionsAsync(DiscordCoreAPI::DeleteAllReactionsData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			DeleteReactionDataAll dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.messageId = dataPackage.messageId;
			dataPackageNew.deletionType = ReactionDeletionType::ALL_DELETE;
			ReactionManagerAgent requestAgent{};
			send(requestAgent.requestDeleteReactionBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		~ReactionManager() {}
	};
	shared_ptr<DiscordCoreAPI::DiscordCoreClient> ReactionManagerAgent::discordCoreClient{ nullptr };
	shared_ptr<ThreadContext> ReactionManagerAgent::threadContext{ nullptr };
	HttpAgentResources ReactionManagerAgent::agentResources{};
}
#endif
