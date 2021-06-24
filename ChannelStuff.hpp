// ChannelStuff.hpp - Header for the Channels classes et al.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_STUFF_
#define _CHANNEL_STUFF_

#include "../pch.h"
#include "FoundationEntities.hpp"
#include "MessageStuff.hpp"
#include "HttpStuff.hpp"

namespace DiscordCoreAPI {

	class Channel {
	public:
		ChannelData data;
		DiscordCoreClient* discordCoreClient{ nullptr };

		Channel() {};

	protected:
		friend class DiscordCoreClient;
		friend class ChannelManagerAgent;
		friend class ChannelManager;
		friend class Guild;
		friend class UserManagerAgent;

		Channel(ChannelData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->initialize(dataNew, discordCoreClientNew);
			return;
		}

		void initialize(ChannelData dataNew, DiscordCoreClient* discordCoreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = discordCoreClientNew;
			return;
		}
	};

	struct GetChannelData {
		string channelId;
	};

	struct FetchChannelData {
		string channelId;
	};

	struct EditChannelPermissionOverwritesData {
		string allow;
		string deny;
		unsigned int type;
		string roleOrUserId;
		string channelId;
	};

	struct DeleteChannelPermissionOverwritesData {
		string channelId;
		string roleOrUserId;
	};

	struct GetDMChannelData {
		string userId;
	};

	class ChannelManagerAgent : public agent {
	protected:
		friend class DiscordCoreClient;
		friend class Guild;
		friend class ChannelManager;

		static unbounded_buffer<DiscordCoreInternal::FetchChannelData>* requestFetchChannelBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetChannelData>* requestGetChannelBuffer;
		static unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* requestGetDMChannelBuffer;
		static unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>* requestPutChannelPermOWsBuffer;
		static unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>* requestDeleteChannelPermOWsBuffer;
		static unbounded_buffer<Channel>* outChannelBuffer;
		static concurrent_queue<Channel> channelsToInsert;
		static overwrite_buffer<map<string, Channel>> cache;
		unbounded_buffer<exception> errorBuffer;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreClient* discordCoreClient;

		ChannelManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* coreClientNew)
			:agent(*threadContextNew->scheduler) {
			this->agentResources = agentResourcesNew;
			this->threadContext = threadContextNew;
			this->discordCoreClient = coreClientNew;
		}

		static void initialize() {
			ChannelManagerAgent::requestFetchChannelBuffer = new unbounded_buffer<DiscordCoreInternal::FetchChannelData>;
			ChannelManagerAgent::requestGetChannelBuffer = new unbounded_buffer<DiscordCoreInternal::GetChannelData>;
			ChannelManagerAgent::requestGetDMChannelBuffer = new unbounded_buffer<DiscordCoreInternal::GetDMChannelData>;
			ChannelManagerAgent::requestPutChannelPermOWsBuffer = new unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>;
			ChannelManagerAgent::requestDeleteChannelPermOWsBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>;
			ChannelManagerAgent::outChannelBuffer = new unbounded_buffer<Channel>;
			return;
		}

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		Channel getObjectData(DiscordCoreInternal::FetchChannelData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)){
				cout << "ChannelManagerAgent::getObject () Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::getObject () Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::getObject () Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ChannelData channelData;
			DiscordCoreInternal::parseObject(returnData.data, &channelData);
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManagerAgent::postObject () Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::postObject () Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::postObject () Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ChannelData channelData;
			DiscordCoreInternal::parseObject(returnData.data, &channelData);
			Channel channelNew(channelData, this->discordCoreClient);
			return channelNew;
		}

		void putObjectData(DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_CHANNEL_PERMISSION_OVERWRITES;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			workload.content = dataPackage.content;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManagerAgent::putObject () Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::putObject () Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::putObject () Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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
			exception error;
			while (requestAgent.getError(error)) {
				cout << "ChannelManagerAgent::deleteObject () Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "ChannelManagerAgent::deleteObject () Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "ChannelManagerAgent::deleteObject () Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			return;
		}

		void run() {
			try {
				DiscordCoreInternal::GetChannelData dataPackage01;
				if (try_receive(ChannelManagerAgent::requestGetChannelBuffer, dataPackage01)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.channelId)) {
							Channel channel = cacheTemp.at(dataPackage01.channelId);
							send(ChannelManagerAgent::outChannelBuffer, channel);
						}
					}
				}
				DiscordCoreInternal::FetchChannelData dataPackage02;
				if (try_receive(ChannelManagerAgent::requestFetchChannelBuffer, dataPackage02)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.channelId)) {
							cacheTemp.erase(dataPackage02.channelId);
						}
					}
					Channel channel = getObjectData (dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.channelId, channel));
					send(ChannelManagerAgent::outChannelBuffer, channel);
					asend(cache, cacheTemp);
				}
				DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackage03;
				if (try_receive(ChannelManagerAgent::requestPutChannelPermOWsBuffer, dataPackage03)) {
					putObjectData(dataPackage03);
				}
				DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackage04;
				if (try_receive(ChannelManagerAgent::requestDeleteChannelPermOWsBuffer, dataPackage04)) {
					deleteObjectData(dataPackage04);
				}
				DiscordCoreInternal::GetDMChannelData dataPackage05;
				if (try_receive(ChannelManagerAgent::requestGetDMChannelBuffer, dataPackage05)) {
					Channel channel = postObjectData (dataPackage05);
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channel.data.id)) {
							cacheTemp.erase(channel.data.id);
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(ChannelManagerAgent::outChannelBuffer, channel);
							asend(cache, cacheTemp);
						}
	 					else {
							cacheTemp.insert(make_pair(channel.data.id, channel));
							send(ChannelManagerAgent::outChannelBuffer, channel);
							asend(cache, cacheTemp);
						}
					}
					else {
						cacheTemp.insert(make_pair(channel.data.id, channel));
						send(ChannelManagerAgent::outChannelBuffer, channel);
						asend(cache, cacheTemp);
					}
				}
				ChannelData dataPackage06;
				Channel channelNew(dataPackage06, this->discordCoreClient);
				while (ChannelManagerAgent::channelsToInsert.try_pop(channelNew)) {
					map<string, Channel> cacheTemp;
					if (try_receive(ChannelManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(channelNew.data.id)) {
							cacheTemp.erase(channelNew.data.id);
						}
					}
					cacheTemp.insert(make_pair(channelNew.data.id, channelNew));
					asend(ChannelManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}
	};

	class ChannelManager {
	public:

		task<Channel> fetchAsync(FetchChannelData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(ChannelManagerAgent::requestFetchChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "this->fetchAsync() Error: " << error.what() << endl << endl;
			}
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(ChannelManagerAgent::outChannelBuffer, channel);
			this->threadContext->releaseGroup(groupIdNew);
			co_return channel;
		}

		task<Channel> getChannelAsync(GetChannelData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			
			send(ChannelManagerAgent::requestGetChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "this->getChannelAsync() Error: " << error.what() << endl << endl;
			}
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(ChannelManagerAgent::outChannelBuffer, channel);
			this->threadContext->releaseGroup(groupIdNew);
			co_return channel;
		}

		task<Channel> getDMChannelAsync(GetDMChannelData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetDMChannelData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(ChannelManagerAgent::requestGetDMChannelBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "this->getDMChannelAsync() Error: " << error.what() << endl << endl;
			}
			ChannelData channelData;
			Channel channel(channelData, this->discordCoreClient);
			try_receive(ChannelManagerAgent::outChannelBuffer, channel);
			this->threadContext->releaseGroup(groupIdNew);
			co_return channel;
		}

		task<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::EditChannelPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.allow = dataPackage.allow;
			dataPackageNew.deny = dataPackage.deny;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			dataPackageNew.type = dataPackage.type;
			dataPackageNew.content = DiscordCoreInternal::getEditChannelPermissionOverwritesPayload(dataPackageNew);
			ChannelManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(ChannelManagerAgent::requestPutChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "this->editChannelPermissionOverwritesAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::DeleteChannelPermissionOverwritesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.roleOrUserId = dataPackage.roleOrUserId;
			ChannelManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			send(ChannelManagerAgent::requestDeleteChannelPermOWsBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "this->deleteChannelPermissionOverwritesAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> insertChannelAsync(Channel channel) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			ChannelManagerAgent requestAgent(this->agentResources, this->threadContext, this->discordCoreClient);
			ChannelManagerAgent::channelsToInsert.push(channel);
			requestAgent.start();
			requestAgent.wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "this->insertChannelAsync() Error: " << error.what() << endl << endl;
			}
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

		task<void> removeChannelAsync(string channelId) {
			unsigned int groupIdNew;
			if (this->threadContext->schedulerGroups.size() == 0) {
				groupIdNew = this->threadContext->createGroup();
			}
			else {
				groupIdNew = this->threadContext->schedulerGroups.at(0)->Id();
			}
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Channel> cache;
			try_receive(ChannelManagerAgent::cache, cache);
			if (cache.contains(channelId)) {
				cache.erase(channelId);
			}
			asend(ChannelManagerAgent::cache, cache);
			this->threadContext->releaseGroup(groupIdNew);
			co_return;
		}

	protected:
		friend class Guild;
		friend class ChannelManagerAgent;
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;

		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		DiscordCoreClient* discordCoreClient{ nullptr };
		unsigned int groupId;
		
		ChannelManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreClient* discordCoreClientNew) {
			this->threadContext = threadContextNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
			this->groupId = this->threadContext->createGroup();
		}

		~ChannelManager() {
			for (auto value : this->threadContext->schedulerGroups) {
				if (value->Id() == this->groupId) {
					value->Release();
				}
			}
		}
	};
	overwrite_buffer<map<string, Channel>> ChannelManagerAgent::cache;
	unbounded_buffer<DiscordCoreInternal::FetchChannelData>* ChannelManagerAgent::requestFetchChannelBuffer;
	unbounded_buffer<DiscordCoreInternal::GetChannelData>* ChannelManagerAgent::requestGetChannelBuffer;
	unbounded_buffer<DiscordCoreInternal::GetDMChannelData>* ChannelManagerAgent::requestGetDMChannelBuffer;
	unbounded_buffer<DiscordCoreInternal::EditChannelPermissionOverwritesData>* ChannelManagerAgent::requestPutChannelPermOWsBuffer;
	unbounded_buffer<DiscordCoreInternal::DeleteChannelPermissionOverwritesData>* ChannelManagerAgent::requestDeleteChannelPermOWsBuffer;
	unbounded_buffer<Channel>* ChannelManagerAgent::outChannelBuffer;
	concurrent_queue<Channel> ChannelManagerAgent::channelsToInsert;
}
#endif