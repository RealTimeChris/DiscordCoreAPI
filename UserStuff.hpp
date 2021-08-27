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
		DiscordCoreInternal::ApplicationData data{};

		Application() {};

		Application(DiscordCoreInternal::ApplicationData dataNew) {
			this->data = dataNew;
		}

	protected:
		friend class UserManager;
		friend class UserManagerAgent;
	};

	class User {
	public:

		DiscordCoreInternal::UserData data{};
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		User() {};

		User(DiscordCoreInternal::UserData dataNew, shared_ptr<DiscordCoreClient> coreClientNew) {
			this->data = dataNew;
			this->discordCoreClient = coreClientNew;
		}
	};

	struct UpdatePresenceData {
		vector<ActivityData> activities{};
		string status{ "" };
		bool afk{ false };
	};

	struct UpdateVoiceStateData {
		string guildId{ "" };
		string channelId{ "" };
		bool selfMute{ false };
		bool selfDeaf{ false };
	};

	class BotUser : public User {
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

		~BotUser() {}

	protected:
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pConnectionWebSocketAgent{ nullptr };
	};

	struct GetUserData {
		string userId{ "" };
	};

	struct FetchUserData {
		string userId{ "" };
	};

	struct LeaveGuildData {
		string guildId{ "" };
	};
};

namespace DiscordCoreInternal {
	class UserManagerAgent : agent {
	protected:
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class UserManager;

		static overwrite_buffer<map<string, DiscordCoreAPI::User>> cache;
		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;

		unbounded_buffer<DiscordCoreInternal::GetApplicationData> requestGetApplicationBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::LeaveGuildData> requestLeaveGuildBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetUserData> requestGetUserBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::CollectUserData> requestCollectUserBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Application> outApplicationBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::User> outUserBuffer{ nullptr };
		concurrent_queue<DiscordCoreAPI::User> usersToInsert{};

		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };

		UserManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> coreClientNew)
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

		DiscordCoreAPI::User getObjectData(DiscordCoreInternal::GetUserData dataPackage) {
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
			DiscordCoreAPI::UserData userData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &userData);
			DiscordCoreAPI::User userNew(userData, this->discordCoreClient);
			return userNew;
		}

		DiscordCoreAPI::Application getObjectData(DiscordCoreInternal::GetApplicationData dataPackage) {
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
			DiscordCoreAPI::ApplicationData applicationData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &applicationData);
			DiscordCoreAPI::Application application(applicationData);
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
				DiscordCoreInternal::CollectUserData dataPackage01;
				if (try_receive(this->requestCollectUserBuffer, dataPackage01)) {
					map<string, DiscordCoreAPI::User> cacheTemp;
					if (try_receive(UserManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.userId)) {
							DiscordCoreAPI::User user = cacheTemp.at(dataPackage01.userId);
							send(this->outUserBuffer, user);
						}
					}
				}
				DiscordCoreInternal::GetUserData dataPackage02;
				if (try_receive(this->requestGetUserBuffer, dataPackage02)) {
					map<string, DiscordCoreAPI::User> cacheTemp;
					if (try_receive(UserManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.userId)) {
							cacheTemp.erase(dataPackage02.userId);
						}
					}
					DiscordCoreAPI::User user = getObjectData(dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.userId, user));
					send(this->outUserBuffer, user);
					send(UserManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::GetApplicationData dataPackage03;
				if (try_receive(this->requestGetApplicationBuffer, dataPackage03)) {
					DiscordCoreAPI::Application application = getObjectData(dataPackage03);
					send(this->outApplicationBuffer, application);
				}
				DiscordCoreInternal::LeaveGuildData dataPackage04;
				if (try_receive(this->requestLeaveGuildBuffer, dataPackage04)) {
					deleteObjectData(dataPackage04);
				}
				DiscordCoreInternal::UserData dataPackage05;
				DiscordCoreAPI::User user(dataPackage05, this->discordCoreClient);
				while (this->usersToInsert.try_pop(user)) {
					map<string, DiscordCoreAPI::User> cacheTemp;
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

		UserManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->threadContext = threadContextNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = discordCoreClientNew;
		}

		task<DiscordCoreAPI::User> fetchAsync(DiscordCoreAPI::FetchUserData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.userType = DiscordCoreInternal::GetUserDataType::USER;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchAsync");
			DiscordCoreInternal::UserData userData;
			DiscordCoreAPI::User user(userData, this->discordCoreClient);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<DiscordCoreAPI::User> fetchCurrentUserAsync() {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userType = DiscordCoreInternal::GetUserDataType::SELF;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchCurrentUserAsync");
			DiscordCoreInternal::UserData userData;
			DiscordCoreAPI::User user(userData, this->discordCoreClient);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<DiscordCoreAPI::User> getUserAsync(DiscordCoreAPI::GetUserData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::CollectUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestCollectUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::getUserAsync");
			DiscordCoreInternal::UserData userData;
			DiscordCoreAPI::User user(userData, this->discordCoreClient);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<void> leaveGuildAsync(DiscordCoreAPI::LeaveGuildData dataPackage) {
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

		task<DiscordCoreAPI::Application> fetchApplicationDataAsync() {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetApplicationData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			UserManagerAgent requestAgent(dataPackageNew.agentResources, this->discordCoreClient);
			send(requestAgent.requestGetApplicationBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchApplicationAsync");
			DiscordCoreInternal::ApplicationData applicationData;
			DiscordCoreAPI::Application application(applicationData);
			try_receive(requestAgent.outApplicationBuffer, application);
			co_return application;
		}

		task<void> insertUserAsync(DiscordCoreAPI::User user) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			UserManagerAgent requestAgent(this->agentResources, this->discordCoreClient);
			requestAgent.usersToInsert.push(user);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			requestAgent.getError("UserManager::insertUserAsync");
			co_return;
		}

		~UserManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class DiscordCoreClientBase;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class EventHandler;
		friend class Guild;

		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };

	};
	overwrite_buffer<map<string, DiscordCoreAPI::User>> UserManagerAgent::cache{ nullptr };
	shared_ptr<DiscordCoreInternal::ThreadContext> UserManagerAgent::threadContext{ nullptr };
}
#endif