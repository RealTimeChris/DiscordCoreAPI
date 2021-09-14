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

	class Application : public ApplicationData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Application>;
		friend class DiscordCoreInternal::UserManagerAgent;
		friend class DiscordCoreInternal::UserManager;

		Application() {};

		Application(ApplicationData dataNew) {
			this->botRequireCodeGrant = dataNew.botRequireCodeGrant;
			this->termsOfServiceUrl = dataNew.termsOfServiceUrl;
			this->privacyPolicyUrl = dataNew.privacyPolicyUrl;
			this->primarySkuId = dataNew.primarySkuId;
			this->description = dataNew.description;
			this->rpcOrigins = dataNew.rpcOrigins;
			this->coverImage = dataNew.coverImage;
			this->botPublic = dataNew.botPublic;
			this->verifyKey = dataNew.verifyKey;
			this->summary = dataNew.summary;
			this->guildId = dataNew.guildId;
			this->owner = dataNew.owner;
			this->flags = dataNew.flags;
			this->team = dataNew.team;
			this->slug = dataNew.slug;
			this->name = dataNew.name;
			this->icon = dataNew.icon;
			this->id = dataNew.id;
		}
	};

	class User : public UserData {
	protected:

		friend struct Concurrency::details::_ResultHolder<User>;
		friend class DiscordCoreInternal::UserManagerAgent;
		friend class DiscordCoreInternal::UserManager;
		friend struct OnGuildMemberRemoveData;
		friend struct OnGuildBanRemoveData;
		friend struct OnGuildBanAddData;
		friend struct OnUserUpdateData;
		friend class DiscordCoreClient;
		friend class Guild;

		User() {};

		User(UserData dataNew) {
			this->discordCoreClient = dataNew.discordCoreClient;
			this->discriminator = dataNew.discriminator;
			this->premiumType = dataNew.premiumType;
			this->publicFlags = dataNew.publicFlags;
			this->mfaEnabled = dataNew.mfaEnabled;
			this->createdAt = dataNew.createdAt;
			this->userName = dataNew.userName;
			this->verified = dataNew.verified;
			this->locale = dataNew.locale;
			this->system = dataNew.system;
			this->avatar = dataNew.avatar;
			this->email = dataNew.email;
			this->flags = dataNew.flags;
			this->bot = dataNew.bot;
			this->id = dataNew.id;
		}
	};

	struct UpdatePresenceData {
		vector<ActivityData> activities{};
		string status{ "" };
		bool afk{ false };
	};

	struct UpdateVoiceStateData {
		string channelId{ "" };
		bool selfMute{ false };
		bool selfDeaf{ false };
		string guildId{ "" };
	};

	class BotUser : public UserData {
	public:

		friend class DiscordCoreClientBase;
		friend class DiscordCoreClient;

		BotUser(BotUser* dataPackage) {
			if (dataPackage != nullptr) {
				*this = *dataPackage;
			}
		}

		void updateVoiceStatus(UpdateVoiceStateData dataPackage) {
			DiscordCoreInternal::UpdateVoiceStateData dataPackageNew;
			dataPackageNew.channelId = dataPackage.channelId;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.selfDeaf = dataPackage.selfDeaf;
			dataPackageNew.selfMute = dataPackage.selfMute;
			string payload = DiscordCoreInternal::getVoiceStateUpdatePayload(dataPackageNew);
			this->pConnectionWebSocketAgent->sendMessage(payload);
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
		}

	protected:

		BotUser(UserData dataPackage) {
			this->discordCoreClient = dataPackage.discordCoreClient;
			this->discriminator = dataPackage.discriminator;
			this->premiumType = dataPackage.premiumType;
			this->publicFlags = dataPackage.publicFlags;
			this->mfaEnabled = dataPackage.mfaEnabled;
			this->createdAt = dataPackage.createdAt;
			this->userName = dataPackage.userName;
			this->verified = dataPackage.verified;
			this->avatar = dataPackage.avatar;
			this->locale = dataPackage.locale;
			this->system = dataPackage.system;
			this->email = dataPackage.email;
			this->flags = dataPackage.flags;
			this->bot = dataPackage.bot;
			this->id = dataPackage.id;
		}

		void Initialize(shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pConnectionWebSocketAgentNew) {
			this->pConnectionWebSocketAgent = pConnectionWebSocketAgentNew;
		}

		~BotUser() {};

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

		static shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient;
		static overwrite_buffer<map<string, DiscordCoreAPI::User>> cache;
		static shared_ptr<ThreadContext> threadContext;
		static HttpAgentResources agentResources;

		unbounded_buffer<DiscordCoreAPI::Application> outApplicationBuffer{ nullptr };
		unbounded_buffer<GetApplicationData> requestGetApplicationBuffer{ nullptr };
		unbounded_buffer<CollectUserData> requestCollectUserBuffer{ nullptr };
		unbounded_buffer<LeaveGuildData> requestLeaveGuildBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::User> outUserBuffer{ nullptr };
		unbounded_buffer<GetUserData> requestGetUserBuffer{ nullptr };		
		concurrent_queue<DiscordCoreAPI::User> usersToInsert{};
		unbounded_buffer<exception> errorBuffer{ nullptr };		

		UserManagerAgent()
			:agent(*UserManagerAgent::threadContext->scheduler->scheduler) {}

		static void intialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			UserManagerAgent::threadContext = ThreadManager::getThreadContext().get();
			UserManagerAgent::discordCoreClient = discordCoreClientNew;
			UserManagerAgent::agentResources = agentResourcesNew;
		}

		static void cleanup() {
			UserManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::UserManagerAgent Error: " << error.what() << endl << endl;
			}
		}

		DiscordCoreAPI::User getObjectData(GetUserData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_GUILD;
			if (dataPackage.userType == GetUserDataType::USER) {
				workload.relativePath = "/users/" + dataPackage.userId;
			}
			else if (dataPackage.userType == GetUserDataType::SELF) {
				workload.relativePath = "/users/@me";
			}
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "UserManagerAgent::getObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::UserData userData;
			userData.discordCoreClient = this->discordCoreClient;
			DataParser::parseObject(returnData.data, &userData);
			DiscordCoreAPI::User userNew(userData);
			return userNew;
		}

		DiscordCoreAPI::Application getObjectData(GetApplicationData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_APPLICATION;
			workload.relativePath = "/oauth2/applications/@me";
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "UserManagerAgent::getObjectData_01");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::getObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::getObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::ApplicationData applicationData;
			DataParser::parseObject(returnData.data, &applicationData);
			DiscordCoreAPI::Application application(applicationData);
			return application;
		}

		void deleteObjectData(LeaveGuildData dataPackage) {
			HttpWorkload workload;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.workloadType = HttpWorkloadType::DELETE_LEAVE_GUILD;
			workload.relativePath = "/users/@me/guilds/" + dataPackage.guildId;
			HttpRequestAgent requestAgent(dataPackage.agentResources);
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "UserManagerAgent::deleteObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
		}

		void run() {
			try {
				CollectUserData dataPackage01;
				if (try_receive(this->requestCollectUserBuffer, dataPackage01)) {
					map<string, DiscordCoreAPI::User> cacheTemp;
					if (try_receive(UserManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.userId)) {
							DiscordCoreAPI::User user = cacheTemp.at(dataPackage01.userId);
							send(this->outUserBuffer, user);
						}
					}
				}
				GetUserData dataPackage02;
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
				GetApplicationData dataPackage03;
				if (try_receive(this->requestGetApplicationBuffer, dataPackage03)) {
					DiscordCoreAPI::Application application = getObjectData(dataPackage03);
					send(this->outApplicationBuffer, application);
				}
				LeaveGuildData dataPackage04;
				if (try_receive(this->requestLeaveGuildBuffer, dataPackage04)) {
					deleteObjectData(dataPackage04);
				}
				DiscordCoreAPI::UserData dataPackage05;
				dataPackage05.discordCoreClient = this->discordCoreClient;
				DiscordCoreAPI::User user(dataPackage05);
				while (this->usersToInsert.try_pop(user)) {
					map<string, DiscordCoreAPI::User> cacheTemp;
					if (try_receive(UserManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(user.id)) {
							cacheTemp.erase(user.id);
						}
					}
					cacheTemp.insert(make_pair(user.id, user));
					send(UserManagerAgent::cache, cacheTemp);
				}

			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
		}
	};

	class UserManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClientBase;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Users;
		friend class DiscordCoreAPI::Guild;

		UserManager(UserManager* pointer) 
		: ThreadContext(*ThreadManager::getThreadContext().get()) {
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		HttpAgentResources agentResources{};

		UserManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->agentResources = agentResourcesNew;
			return *this;
		}

		task<DiscordCoreAPI::User> fetchAsync(DiscordCoreAPI::FetchUserData dataPackage) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.userType = GetUserDataType::USER;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchAsync");
			DiscordCoreAPI::UserData userData;
			userData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::User user(userData);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<DiscordCoreAPI::User> fetchCurrentUserAsync() {
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userType = GetUserDataType::SELF;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchCurrentUserAsync");
			DiscordCoreAPI::UserData userData;
			userData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::User user(userData);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<DiscordCoreAPI::User> getUserAsync(DiscordCoreAPI::GetUserData dataPackage) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			CollectUserData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.userId = dataPackage.userId;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestCollectUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::getUserAsync");
			DiscordCoreAPI::UserData userData;
			userData.discordCoreClient = this->discordCoreClient;
			DiscordCoreAPI::User user(userData);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<void> leaveGuildAsync(DiscordCoreAPI::LeaveGuildData dataPackage) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			LeaveGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestLeaveGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::leaveGuildAsync");
			co_return;
		}

		task<DiscordCoreAPI::Application> fetchApplicationDataAsync() {
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetApplicationData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestGetApplicationBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("UserManager::fetchApplicationAsync");
			DiscordCoreAPI::ApplicationData applicationData;
			DiscordCoreAPI::Application application(applicationData);
			try_receive(requestAgent.outApplicationBuffer, application);
			co_return application;
		}

		task<void> insertUserAsync(DiscordCoreAPI::User user) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			UserManagerAgent requestAgent{};
			requestAgent.usersToInsert.push(user);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			requestAgent.getError("UserManager::insertUserAsync");
			co_return;
		}

		~UserManager() {}
	};
	shared_ptr<DiscordCoreAPI::DiscordCoreClient> UserManagerAgent::discordCoreClient{ nullptr };
	overwrite_buffer<map<string, DiscordCoreAPI::User>> UserManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> UserManagerAgent::threadContext{ nullptr };
	HttpAgentResources UserManagerAgent::agentResources{};
}
#endif