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

	class Channel {
	public:
		ChannelData data;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		Channel() {};

	protected:
		friend class DiscordCoreClient;
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		friend class UserManagerAgent;

		Channel(ChannelData dataNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = discordCoreClientNew;
			return;
		}
	};

	class ChannelManagerAgent : agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class ChannelManager;

		static overwrite_buffer<map<string, Channel>> cache;
		
		unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData> requestDeleteChannelPermOWsBuffer;
		unbounded_buffer<DiscordCoreInternal::PutPermissionOverwritesData> requestPutChannelPermOWsBuffer;
		unbounded_buffer<DiscordCoreInternal::CollectChannelData> requestCollectChannelBuffer;
		unbounded_buffer<DiscordCoreInternal::GetDMChannelData>requestGetDMChannelBuffer;
		unbounded_buffer<DiscordCoreInternal::GetChannelData> requestGetChannelBuffer;
		unbounded_buffer<Channel> outChannelBuffer;
		concurrent_queue<Channel>channelsToInsert;
		unbounded_buffer<exception> errorBuffer;

		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		ChannelManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreClient> coreClientNew)
			:agent(*ChannelManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
		}

		static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
			ChannelManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			ChannelManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::ChannelManagerAgent Error: " << error.what() << endl << endl;
			}
			return;
		}

		Channel getObjectData(DiscordCoreInternal::GetChannelData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManagerAgent::getObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ChannelData channelData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->discordCoreClient);
			return channelNew;
		}

		Channel postObjectData(DiscordCoreInternal::GetDMChannelData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_USER_DM;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
			workload.relativePath = "/users/@me/channels";
			json theValue = { {"recipient_id", dataPackage.userId } };
			workload.content = theValue.dump();
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManagerAgent::postObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ChannelData channelData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->discordCoreClient);
			return channelNew;
		}

		void putObjectData(DiscordCoreInternal::PutPermissionOverwritesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManagerAgent::putObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void deleteObjectData(DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManagerAgent::deleteObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void run() {
			try {
				DiscordCoreInternal::CollectChannelData dataPackage01;
				if (try_receive(this->requestCollectChannelBuffer, dataPackage01)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.channelId)) {
							Channel channel = cacheTemp.at(dataPackage01.channelId);
							send(this->outChannelBuffer, channel);
						}
					}
				}
				DiscordCoreInternal::GetChannelData dataPackage02;
				if (try_receive(this->requestGetChannelBuffer, dataPackage02)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.channelId)) {
							cacheTemp.erase(dataPackage02.channelId);
						}
					}
					Channel channel = getObjectData (dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.channelId, channel));
					send(this->outChannelBuffer, channel);
					send(cache, cacheTemp);
				}
				DiscordCoreInternal::PutPermissionOverwritesData dataPackage03;
				if (try_receive(this->requestPutChannelPermOWsBuffer, dataPackage03)) {
					putObjectData(dataPackage03);
				}
				DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackage04;
				if (try_receive(this->requestDeleteChannelPermOWsBuffer, dataPackage04)) {
					deleteObjectData(dataPackage04);
				}
				DiscordCoreInternal::GetDMChannelData dataPackage05;
				if (try_receive(this->requestGetDMChannelBuffer, dataPackage05)) {
					Channel channel = postObjectData(dataPackage05);
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channel.data.id)) {
							cacheTemp.erase(channel.data.id);
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(this->outChannelBuffer, channel);
							send(cache, cacheTemp);
						}
	 					else {
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(this->outChannelBuffer, channel);
							send(cache, cacheTemp);
						}
					}
					else {
						cacheTemp.insert(make_pair(channel.data.id, channel));
						send(this->outChannelBuffer, channel);
						send(cache, cacheTemp);
					}
				}
				ChannelData dataPackage06;
				Channel channelNew(dataPackage06, this->discordCoreClient);
				while (this->channelsToInsert.try_pop(channelNew)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channelNew.data.id)) {
							cacheTemp.erase(channelNew.data.id);
						}
					}
					cacheTemp.insert(make_pair(channelNew.data.id, channelNew));
					send(ChannelManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
		}
	};

	class ChannelManager {
	public:

		task<Channel> fetchAsync(FetchChannelData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestGetChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::fetchAsync");
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(requestAgent.outChannelBuffer, channel);
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::CollectChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestCollectChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::getChannelAsync");
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(requestAgent.outChannelBuffer, channel);
			co_return channel;
		}

		task<Channel> fetchDMChannelAsync(FetchDMChannelData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetDMChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			ChannelManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestGetDMChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::fetchDMChannelAsync");
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(requestAgent.outChannelBuffer, channel);
			co_return channel;
		}

		task<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PutPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			dataPackageNew.content = DiscordCoreInternal::getEditChannelPermissionOverwritesPayload(dataPackage);
			ChannelManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestPutChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::editChannelPermissionOverwritesAsync");
			co_return;
		}

		task<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			ChannelManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			send(requestAgent.requestDeleteChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("ChannelManager::deleteChannelPermissionOverwritesAsync");
			co_return;
		}

		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->threadContext = threadContextNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		~ChannelManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class Guild;
		friend class ChannelManagerAgent;
		friend class DiscordCoreClient;
		friend class Channels;
		friend class DiscordCoreClientBase;
		friend class EventHandler;

		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		task<void> insertChannelAsync(Channel channel) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			ChannelManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			requestAgent.channelsToInsert.push(channel);
			requestAgent.start();
			requestAgent.wait(&requestAgent);
			requestAgent.getError("ChannelManager::inserChannelAsync");
			co_return;
		}

		task<void> removeChannelAsync(string channelId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Channel> cache;
			try_receive(ChannelManagerAgent::cache, cache);
			if (cache.contains(channelId)) {
				cache.erase(channelId);
			}
			send(ChannelManagerAgent::cache, cache);
			co_return;
		}
				
	};
	overwrite_buffer<map<string, Channel>> ChannelManagerAgent::cache;
	shared_ptr<DiscordCoreInternal::ThreadContext> ChannelManagerAgent::threadContext{ nullptr };
}
#endif