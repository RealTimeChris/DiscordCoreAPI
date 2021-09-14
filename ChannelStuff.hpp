// ChannelStuff.hpp - Header for the Channels classes et al.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_STUFF_
#define _CHANNEL_STUFF_

#include "../pch.h"
#include "FoundationEntities.hpp"
#include "HttpStuff.hpp"

namespace DiscordCoreAPI {
	
	class Channel : public ChannelData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Channel>;
		friend class DiscordCoreInternal::ChannelManagerAgent;
		friend class DiscordCoreInternal::ChannelManager;
		friend struct OnChannelDeletionData;
		friend struct OnChannelCreationData;
		friend struct OnChannelUpdateData;
		friend class DiscordCoreClient;
		friend class Guild;

		Channel() {};

		Channel(ChannelData dataNew) {
			this->permissionOverwrites = dataNew.permissionOverwrites;
			this->discordCoreClient = dataNew.discordCoreClient;
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
};

namespace DiscordCoreInternal	{

	class ChannelManagerAgent : agent {
	protected:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class ChannelManager;

		static shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient;
		static overwrite_buffer<map<string, DiscordCoreAPI::Channel>> cache;
		static shared_ptr<ThreadContext> threadContext;
		static HttpAgentResources agentResources;

		unbounded_buffer<DeleteChannelPermissionOverwritesData> requestDeleteChannelPermOWsBuffer{ nullptr };
		unbounded_buffer<PutPermissionOverwritesData> requestPutChannelPermOWsBuffer{ nullptr };
		unbounded_buffer<CollectChannelData> requestCollectChannelBuffer{ nullptr };
		unbounded_buffer<GetDMChannelData>requestGetDMChannelBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Channel> outChannelBuffer{ nullptr };
		unbounded_buffer<GetChannelData> requestGetChannelBuffer{ nullptr };
		concurrent_queue<DiscordCoreAPI::Channel>channelsToInsert{};
		unbounded_buffer<exception> errorBuffer{ nullptr };
		
		ChannelManagerAgent()
			: agent(*ChannelManagerAgent::threadContext->scheduler->scheduler) {};

		static void initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			ChannelManagerAgent::threadContext = ThreadManager::getThreadContext().get();
			ChannelManagerAgent::discordCoreClient = discordCoreClientNew;
			ChannelManagerAgent::agentResources = agentResourcesNew;
			
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::ChannelManagerAgent Error: " << error.what() << endl << endl;
			}
		}

		DiscordCoreAPI::Channel getObjectData(GetChannelData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ChannelManagerAgent::getObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::ChannelData channelData;
			channelData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &channelData);
			DiscordCoreAPI::Channel channelNew(channelData);
			return channelNew;
		}

		DiscordCoreAPI::Channel postObjectData(GetDMChannelData dataPackage) {
			HttpWorkload workload;
			workload.workloadType = HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = HttpWorkloadClass::POST;
			workload.relativePath = "/users/@me/channels";
			json theValue = { {"recipient_id", dataPackage.userId } };
			workload.content = theValue.dump();
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ChannelManagerAgent::postObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::ChannelData channelData;
			channelData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &channelData);
			DiscordCoreAPI::Channel channelNew(channelData);
			return channelNew;
		}

		void putObjectData(PutPermissionOverwritesData dataPackage) {
			HttpWorkload workload;
			workload.workloadType = HttpWorkloadType::PUT_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			workload.content = getEditChannelPermissionOverwritesPayload(dataPackage);
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "ChannelManagerAgent::putObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void deleteObjectData(DeleteChannelPermissionOverwritesData dataPackage) {
			HttpWorkload workload;
			workload.workloadType = HttpWorkloadType::DELETE_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
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
				CollectChannelData dataPackage01;
				if (try_receive(this->requestCollectChannelBuffer, dataPackage01)) {
					map<string, DiscordCoreAPI::Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.channelId)) {
							DiscordCoreAPI::Channel channel = cacheTemp.at(dataPackage01.channelId);
							send(this->outChannelBuffer, channel);
						}
					}
				}
				GetChannelData dataPackage02;
				if (try_receive(this->requestGetChannelBuffer, dataPackage02)) {
					map<string, DiscordCoreAPI::Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.channelId)) {
							cacheTemp.erase(dataPackage02.channelId);
						}
					}
					DiscordCoreAPI::Channel channel = getObjectData (dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.channelId, channel));
					send(this->outChannelBuffer, channel);
					send(cache, cacheTemp);
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
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channel.id)) {
							cacheTemp.erase(channel.id);
							cacheTemp.insert(make_pair(channel.id, channel));
							send(this->outChannelBuffer, channel);
							send(cache, cacheTemp);
						}
	 					else {
							cacheTemp.insert(make_pair(channel.id, channel));
							send(this->outChannelBuffer, channel);
							send(cache, cacheTemp);
						}
					}
					else {
						cacheTemp.insert(make_pair(channel.id, channel));
						send(this->outChannelBuffer, channel);
						send(cache, cacheTemp);
					}
				}
				DiscordCoreAPI::ChannelData dataPackage06;
				dataPackage06.discordCoreClient = this->discordCoreClient;
				DiscordCoreAPI::Channel channelNew(dataPackage06);
				while (this->channelsToInsert.try_pop(channelNew)) {
					map<string, DiscordCoreAPI::Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channelNew.id)) {
							cacheTemp.erase(channelNew.id);
						}
					}
					cacheTemp.insert(make_pair(channelNew.id, channelNew));
					send(ChannelManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
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
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		HttpAgentResources agentResources{};

		ChannelManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->agentResources = agentResourcesNew;
			return *this;
		}

		task<DiscordCoreAPI::Channel> fetchAsync(DiscordCoreAPI::FetchChannelData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestGetChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::fetchAsync");
			DiscordCoreAPI::ChannelData channelData;
			channelData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Channel channel(channelData);
			try_receive(requestAgent.outChannelBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<DiscordCoreAPI::Channel> getChannelAsync(DiscordCoreAPI::GetChannelData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			CollectChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestCollectChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::getChannelAsync");
			DiscordCoreAPI::ChannelData channelData;
			channelData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Channel channel(channelData);
			try_receive(requestAgent.outChannelBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<DiscordCoreAPI::Channel> fetchDMChannelAsync(DiscordCoreAPI::FetchDMChannelData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetDMChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestGetDMChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::fetchDMChannelAsync");
			DiscordCoreAPI::ChannelData channelData;
			channelData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::Channel channel(channelData);
			try_receive(requestAgent.outChannelBuffer, channel);
			co_await mainThread;
			co_return channel;
		}

		task<void> editChannelPermissionOverwritesAsync(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			PutPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allow = dataPackage.allow;
			dataPackageNew.deny = dataPackage.deny;
			dataPackageNew.type = (EditChannelPermissionOverwritesType)dataPackage.type;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestPutChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::editChannelPermissionOverwritesAsync");
			co_await mainThread;
			co_return;
		}

		task<void> deleteChannelPermissionOverwritesAsync(DiscordCoreAPI::DeleteChannelPermissionOverwritesData dataPackage) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			DeleteChannelPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			ChannelManagerAgent requestAgent{};
			send(requestAgent.requestDeleteChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::deleteChannelPermissionOverwritesAsync");
			co_await mainThread;
			co_return;
		}

		task<void> insertChannelAsync(DiscordCoreAPI::Channel channel) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			ChannelManagerAgent requestAgent{};
			requestAgent.channelsToInsert.push(channel);
			requestAgent.start();
			requestAgent.wait(&requestAgent);
			requestAgent.getError("ChannelManager::inserChannelAsync");
			co_await mainThread;
			co_return;
		}

		task<void> removeChannelAsync(string channelId) {
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::Channel> cache;
			try_receive(ChannelManagerAgent::cache, cache);
			if (cache.contains(channelId)) {
				cache.erase(channelId);
			}
			send(ChannelManagerAgent::cache, cache);
			co_await mainThread;
			co_return;
		}

		~ChannelManager() {}
	};
	shared_ptr<DiscordCoreAPI::DiscordCoreClient> ChannelManagerAgent::discordCoreClient{ nullptr };
	overwrite_buffer<map<string, DiscordCoreAPI::Channel>> ChannelManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> ChannelManagerAgent::threadContext{ nullptr };
	HttpAgentResources ChannelManagerAgent::agentResources{};
}
#endif