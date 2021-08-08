// UserStuff.hpp - Header for user-related classes.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _USER_STUFF_
#define _USER_STUFF_

#include "../pch.h"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "WebSocketStuff.hpp"

namespace DiscordCoreAPI {

	class Application {
	public:
		DiscordCoreInternal::ApplicationData data;

		Application() {};

	protected:
		friend class UserManager;
		friend class UserManagerAgent;

		Application(DiscordCoreInternal::ApplicationData dataNew) {
			this->data = dataNew;
		}
	};

	class User {
	public:

		DiscordCoreInternal::UserData data;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		User() {};

		User(DiscordCoreInternal::UserData dataNew, shared_ptr<DiscordCoreClient> coreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = coreClientNew;
		}
	};

	struct UpdatePresenceData {
		vector<ActivityData> activities;
		string status;
		bool afk;
	};

	struct UpdateVoiceStateData {
		string guildId;
		string channelId;
		bool selfMute;
		bool selfDeaf;
	};

	class BotUser: public User {
	public:
		BotUser(DiscordCoreInternal::UserData userDataNew, shared_ptr<DiscordCoreClient> coreClientNew, shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pConnectionWebSocketAgentNew) {
			this->pConnectionWebSocketAgent = pConnectionWebSocketAgentNew;
			this->data = userDataNew;
			this->discordCoreClient = coreClientNew;
		}

		void updateVoiceStatus(UpdateVoiceStateData dataPackage) {
			DiscordCoreInternal::UpdateVoiceStateData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.selfDeaf = dataPackage.selfDeaf;
			dataPackageNew.selfMute = dataPackage.selfMute;
			string payload = DiscordCoreInternal::getVoiceStateUpdatePayload(dataPackageNew);
			this->pConnectionWebSocketAgent->sendMessage(payload);
			return;
		}

		void updatePresence(UpdatePresenceData dataPackage) {
			DiscordCoreInternal::UpdatePresenceData dataPackageNew;
			for (auto value : dataPackage.activities) {
				dataPackageNew.activities.push_back(value);
			}
			dataPackageNew.afk = dataPackage.afk;
			dataPackageNew.since = (__int64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			dataPackageNew.status = dataPackage.status;
			string payload = DiscordCoreInternal::getPresenceUpdatePayload(dataPackageNew);
			this->pConnectionWebSocketAgent->sendMessage(payload);
			return;
		}

	protected:
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pConnectionWebSocketAgent;
	};

	struct GetUserData {
		string userId;
	};

	struct FetchUserData {
		string userId;
	};

	struct LeaveGuildData {
		string guildId;
	};

	class UserManagerAgent : agent {
	protected:
		friend class DiscordCoreClient;
		friend class UserManager;

		static overwrite_buffer<map<string, User>> cache;

		unbounded_buffer<DiscordCoreInternal::GetApplicationData> requestGetApplicationBuffer;
		unbounded_buffer<DiscordCoreInternal::LeaveGuildData> requestLeaveGuildBuffer;
		unbounded_buffer<DiscordCoreInternal::FetchUserData> requestFetchUserBuffer;
		unbounded_buffer<DiscordCoreInternal::GetUserData> requestGetUserBuffer;
		unbounded_buffer<Application> outApplicationBuffer;
		unbounded_buffer<exception> errorBuffer;
		unbounded_buffer<User> outUserBuffer;
		concurrent_queue<User> usersToInsert;

		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		UserManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreClient> coreClientNew)
			:agent(*UserManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
		}

		static void intialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
			UserManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			UserManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::UserManagerAgent Error: " << error.what() << endl << endl;
			}
			return;
		}

		User getObjectData(DiscordCoreInternal::FetchUserData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			if (dataPackage.userType == DiscordCoreInternal::GetUserDataType::USER) {
				workload.relativePath = "/users/" + dataPackage.userId;
			}
			else if (dataPackage.userType == DiscordCoreInternal::GetUserDataType::SELF) {
				workload.relativePath = "/users/@me";
			}
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManagerAgent::getObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			UserData userData;
			DiscordCoreInternal::parseObject(returnData.data, &userData);
			User userNew(userData, this->discordCoreClient);
			return userNew;
		}

		Application getObjectData(DiscordCoreInternal::GetApplicationData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_APPLICATION;
			workload.relativePath = "/oauth2/applications/@me";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManagerAgent::getObjectData_01");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::getObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::getObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			ApplicationData applicationData;
			DiscordCoreInternal::parseObject(returnData.data, &applicationData);
			Application application(applicationData);
			return application;
		}

		void deleteObjectData(DiscordCoreInternal::LeaveGuildData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_LEAVE_GUILD;
			workload.relativePath = "/users/@me/guilds/" + dataPackage.guildId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManagerAgent::deleteObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void run() {
			try {
				DiscordCoreInternal::GetUserData dataPackage01;
				if (try_receive(this->requestGetUserBuffer, dataPackage01)) {
					map<string, User> cacheTemp;
					if (try_receive(UserManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.userId)) {
							User user = cacheTemp.at(dataPackage01.userId);
							send(this->outUserBuffer, user);
						}
					}
				}
				DiscordCoreInternal::FetchUserData dataPackage02;
				if (try_receive(this->requestFetchUserBuffer, dataPackage02)) {
					map<string, User> cacheTemp;
					if (try_receive(UserManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.userId)) {
							cacheTemp.erase(dataPackage02.userId);
						}
					}
					User user = getObjectData(dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.userId, user));
					send(this->outUserBuffer, user);
					send(UserManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::GetApplicationData dataPackage03;
				if (try_receive(this->requestGetApplicationBuffer, dataPackage03)) {
					Application application = getObjectData(dataPackage03);
					send(this->outApplicationBuffer, application);
				}
				DiscordCoreInternal::LeaveGuildData dataPackage04;
				if (try_receive(this->requestLeaveGuildBuffer, dataPackage04)) {
					deleteObjectData(dataPackage04);
				}
				DiscordCoreInternal::UserData dataPackage05;
				User user(dataPackage05, this->discordCoreClient);
				while (this->usersToInsert.try_pop(user)) {
					map<string, User> cacheTemp;
					if (try_receive(UserManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(user.data.id)) {
							cacheTemp.erase(user.data.id);
						}
					}
					cacheTemp.insert(make_pair(user.data.id, user));
					send(UserManagerAgent::cache, cacheTemp);
				}

			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
		}
	};

	class UserManager {
	public:

		task<User> fetchAsync(FetchUserData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.userType = DiscordCoreInternal::GetUserDataType::USER;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestFetchUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchAsync");
			DiscordCoreInternal::UserData userData;
			User user(userData, this->discordCoreClient);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<User> fetchCurrentUserAsync() {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::FetchUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userType = DiscordCoreInternal::GetUserDataType::SELF;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestFetchUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchCurrentUserAsync");
			DiscordCoreInternal::UserData userData;
			User user(userData, this->discordCoreClient);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<User> getUserAsync(GetUserData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::getUserAsync");
			DiscordCoreInternal::UserData userData;
			User user(userData, this->discordCoreClient);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<void> leaveGuildAsync(LeaveGuildData dataPackage){ 
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::LeaveGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestLeaveGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::leaveGuildAsync");
			co_return;
		}

		task<Application> getApplicationDataAsync() {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetApplicationData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetApplicationBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::getApplicationAsync");
			DiscordCoreInternal::ApplicationData applicationData;
			Application application(applicationData);
			try_receive(requestAgent.outApplicationBuffer, application);
			co_return application;
		}

		task<void> insertUserAsync(User user) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			UserManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			requestAgent.usersToInsert.push(user);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			requestAgent.getError("UserManager::insertUserAsync");
			co_return;
		}

		UserManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->threadContext = threadContextNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		~UserManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class DiscordCoreClient;
		friend class DiscordCoreClientBase;

		DiscordCoreInternal::HttpAgentResources agentResources;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
	};
	overwrite_buffer<map<string, User>> UserManagerAgent::cache;
	shared_ptr<DiscordCoreInternal::ThreadContext> UserManagerAgent::threadContext;
}
#endif