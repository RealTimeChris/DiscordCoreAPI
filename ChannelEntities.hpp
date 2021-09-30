// ChannelEntities.hpp - Header for the classes related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_ENTITIES_
#define _CHANNEL_ENTITIES_

#include "../pch.h"
#include "FoundationEntities.hpp"
#include "Http.hpp"

namespace DiscordCoreAPI {

	struct EditChannelPermissionOverwritesData {
		EditChannelPermissionOverwritesType type{};
		string roleOrUserId{ "" };
		string channelId{ "" };
		string allow{ "" };
		string deny{ "" };
	};

	struct FetchChannelData {
		string channelId{ "" };
	};

	struct GetChannelData {
		string channelId{ "" };
	};

	struct DeleteChannelPermissionOverwritesData {
		string roleOrUserId{ "" };
		string channelId{ "" };
	};

	struct FetchDMChannelData {
		string userId{ "" };
	};

	class Channel : public ChannelData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Channel>;
		friend class DiscordCoreInternal::ChannelManagerAgent;
		friend class DiscordCoreInternal::ChannelManager;
		friend struct OnChannelDeletionData;
		friend struct OnChannelCreationData;
		friend struct OnChannelUpdateData;
		friend struct OnThreadCreationData;
		friend struct OnThreadDeletionData;
		friend struct OnThreadUpdateData;
		friend class DiscordCoreClient;
		friend class Guild;

		Channel() {};

		Channel(ChannelData dataNew) {
			this->permissionOverwrites = dataNew.permissionOverwrites;
			this->rateLimitPerUser = dataNew.rateLimitPerUser;
			this->lastPinTimestamp = dataNew.lastPinTimestamp;
			this->videoQualityMode = dataNew.videoQualityMode;
			this->threadMetadata = dataNew.threadMetadata;
			this->lastMessageId = dataNew.lastMessageId;
			this->applicationId = dataNew.applicationId;
			this->messageCount = dataNew.messageCount;
			this->memberCount = dataNew.memberCount;
			this->recipients = dataNew.recipients;
			this->userLimit = dataNew.userLimit;
			this->rtcRegion = dataNew.rtcRegion;
			this->position = dataNew.position;
			this->parentId = dataNew.parentId;
			this->ownerId = dataNew.ownerId;
			this->bitrate = dataNew.bitrate;
			this->guildId = dataNew.guildId;
			this->member = dataNew.member;
			this->topic = dataNew.topic;
			this->type = dataNew.type;
			this->name = dataNew.name;
			this->nsfw = dataNew.nsfw;
			this->icon = dataNew.icon;
			this->id = dataNew.id;
		}
	};
};

namespace DiscordCoreInternal	{

	class ChannelManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class ChannelManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<DeleteChannelPermissionOverwritesData> requestDeleteChannelPermOWsBuffer{ nullptr };
		unbounded_buffer<PutPermissionOverwritesData> requestPutChannelPermOWsBuffer{ nullptr };
		unbounded_buffer<GetDMChannelData>requestGetDMChannelBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Channel> outChannelBuffer{ nullptr };
		unbounded_buffer<GetChannelData> requestGetChannelBuffer{ nullptr };
		
		ChannelManagerAgent()
			: agent(*ChannelManagerAgent::threadContext->scheduler->scheduler) {};

		static void initialize() {
			ChannelManagerAgent::threadContext = ThreadManager::getThreadContext().get();
		}

		static void cleanup() {
			ChannelManagerAgent::threadContext->releaseContext();
		};

		DiscordCoreAPI::Channel getObjectData(GetChannelData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ChannelManagerAgent::getObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::ChannelData channelData;
			DataParser::parseObject(returnData.data, &channelData);
			DiscordCoreAPI::Channel channelNew(channelData);
			return channelNew;
		}

		DiscordCoreAPI::Channel postObjectData(GetDMChannelData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadType = HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.relativePath = "/users/@me/channels";
			json theValue = { {"recipient_id", dataPackage.userId } };
			workload.content = theValue.dump();
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ChannelManagerAgent::postObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::ChannelData channelData;
			DataParser::parseObject(returnData.data, &channelData);
			DiscordCoreAPI::Channel channelNew(channelData);
			return channelNew;
		}

		void putObjectData(PutPermissionOverwritesData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadType = HttpWorkloadType::PUT_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			workload.content = getEditChannelPermissionOverwritesPayload(dataPackage);
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ChannelManagerAgent::putObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void deleteObjectData(DeleteChannelPermissionOverwritesData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadType = HttpWorkloadType::DELETE_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ChannelManagerAgent::deleteObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void run() {
			try {
				GetChannelData dataPackage02;
				if (try_receive(this->requestGetChannelBuffer, dataPackage02)) {
					DiscordCoreAPI::Channel channel = getObjectData(dataPackage02);
					send(this->outChannelBuffer, channel);
				}
				PutPermissionOverwritesData dataPackage03;
				if (try_receive(this->requestPutChannelPermOWsBuffer, dataPackage03)) {
					putObjectData(dataPackage03);
				}
				DeleteChannelPermissionOverwritesData dataPackage04;
				if (try_receive(this->requestDeleteChannelPermOWsBuffer, dataPackage04)) {
					deleteObjectData(dataPackage04);
				}
				GetDMChannelData dataPackage05;
				if (try_receive(this->requestGetDMChannelBuffer, dataPackage05)) {
					DiscordCoreAPI::Channel channel = postObjectData(dataPackage05);
					map<string, DiscordCoreAPI::Channel> cacheTemp;
					send(this->outChannelBuffer, channel);
				}
			}
			catch (...) {
				DiscordCoreAPI::rethrowException("ChannelManagerAgent::run() Error: ");
			}
			this->done();
		}

	};

	class ChannelManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClientBase;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::InputEvents;
		friend class DiscordCoreAPI::Channels;
		friend class DiscordCoreAPI::Guild;

		ChannelManager(ChannelManager* pointer) 
			: ThreadContext(*ThreadManager::getThreadContext().get()) {
			this->cache = new overwrite_buffer<map<string, DiscordCoreAPI::Channel>>();
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

		~ChannelManager() {
			delete this->cache;
			this->cache = nullptr;
		}

	protected:

		overwrite_buffer<map<string, DiscordCoreAPI::Channel>>* cache{};

		task<DiscordCoreAPI::Channel> fetchAsync(DiscordCoreAPI::FetchChannelData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetChannelData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestGetChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::ChannelData channelData{};
			DiscordCoreAPI::Channel channel(channelData);
			try_receive(requestAgent.outChannelBuffer, channel);
			map<string, DiscordCoreAPI::Channel> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			cacheTemp.insert_or_assign(dataPackage.channelId, channel);
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return channel;
		}

		task<DiscordCoreAPI::Channel> getChannelAsync(DiscordCoreAPI::GetChannelData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue);
			map<string, DiscordCoreAPI::Channel> cacheTemp{};
			DiscordCoreAPI::ChannelData channelData{};
			DiscordCoreAPI::Channel channel(channelData);
			try_receive(this->cache, cacheTemp);
			if (cacheTemp.contains(dataPackage.channelId)) {
				channel = cacheTemp.at(dataPackage.channelId);
			}
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return channel;
		}

		task<DiscordCoreAPI::Channel> fetchDMChannelAsync(DiscordCoreAPI::FetchDMChannelData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetDMChannelData dataPackageNew;
			dataPackageNew.userId = dataPackage.userId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestGetDMChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::ChannelData channelData;
			DiscordCoreAPI::Channel channel(channelData);
			try_receive(requestAgent.outChannelBuffer, channel);
			map<string, DiscordCoreAPI::Channel> cacheTemp;
			try_receive(this->cache, cacheTemp);
			cacheTemp.insert_or_assign(channel.id, channel);
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return channel;
		}

		task<void> editChannelPermissionOverwritesAsync(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			PutPermissionOverwritesData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allow = dataPackage.allow;
			dataPackageNew.deny = dataPackage.deny;
			dataPackageNew.type = (EditChannelPermissionOverwritesType)dataPackage.type;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestPutChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> deleteChannelPermissionOverwritesAsync(DiscordCoreAPI::DeleteChannelPermissionOverwritesData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			DeleteChannelPermissionOverwritesData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestDeleteChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<void> insertChannelAsync(DiscordCoreAPI::Channel channel) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Channel> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			cacheTemp.insert_or_assign(channel.id, channel);
			send(this->cache, cacheTemp);
			co_return;
		}

		task<void> removeChannelAsync(string channelId) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Channel> cacheTemp;
			try_receive(this->cache, cacheTemp);
			if (cacheTemp.contains(channelId)) {
				cacheTemp.erase(channelId);
			}
			send(this->cache, cacheTemp);
			co_await mainThread;
			co_return;
		}
	};
	shared_ptr<ThreadContext> ChannelManagerAgent::threadContext{ nullptr };
}
#endif