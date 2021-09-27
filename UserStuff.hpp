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

		BotUser() {};

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

		static overwrite_buffer<map<string, DiscordCoreAPI::User>> cache;
		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<DiscordCoreAPI::Application> outApplicationBuffer{ nullptr };
		unbounded_buffer<CollectUserData> requestCollectUserBuffer{ nullptr };
		unbounded_buffer<LeaveGuildData> requestLeaveGuildBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::User> outUserBuffer{ nullptr };
		unbounded_buffer<bool> requestGetApplicationBuffer{ nullptr };
		unbounded_buffer<GetUserData> requestGetUserBuffer{ nullptr };		
		concurrent_queue<DiscordCoreAPI::User> usersToInsert{};

		UserManagerAgent()
			:agent(*UserManagerAgent::threadContext->scheduler->scheduler) {}

		static void intialize() {
			UserManagerAgent::threadContext = ThreadManager::getThreadContext().get();
		}

		static void cleanup() {
			UserManagerAgent::threadContext->releaseGroup();
		}

		DiscordCoreAPI::User getObjectData(GetUserData dataPackage) {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_GUILD;
			if (dataPackage.userType == GetUserDataType::USER) {
				workload.relativePath = "/users/" + dataPackage.userId;
			}
			else if (dataPackage.userType == GetUserDataType::SELF) {
				workload.relativePath = "/users/@me";
			}
			HttpRequestAgent requestAgent{};
			HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "UserManagerAgent::getObjectData_00");
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "UserManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "UserManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreAPI::UserData userData;
			DataParser::parseObject(returnData.data, &userData);
			DiscordCoreAPI::User userNew(userData);
			return userNew;
		}

		DiscordCoreAPI::Application getObjectData() {
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::GET;
			workload.workloadType = HttpWorkloadType::GET_APPLICATION;
			workload.relativePath = "/oauth2/applications/@me";
			HttpRequestAgent requestAgent{};
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
			HttpWorkloadData workload;
			workload.workloadClass = HttpWorkloadClass::DELETED;
			workload.workloadType = HttpWorkloadType::DELETE_LEAVE_GUILD;
			workload.relativePath = "/users/@me/guilds/" + dataPackage.guildId;
			HttpRequestAgent requestAgent{};
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
				bool dataPackage03;
				if (try_receive(this->requestGetApplicationBuffer, dataPackage03)) {
					DiscordCoreAPI::Application application = getObjectData();
					send(this->outApplicationBuffer, application);
				}
				LeaveGuildData dataPackage04;
				if (try_receive(this->requestLeaveGuildBuffer, dataPackage04)) {
					deleteObjectData(dataPackage04);
				}
				DiscordCoreAPI::UserData dataPackage05;
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
			catch (...) {
				DiscordCoreAPI::rethrowException("UserManagerAgent::run() Error: ");
			}
			this->done();
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

		task<DiscordCoreAPI::User> fetchAsync(DiscordCoreAPI::FetchUserData dataPackage) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetUserData dataPackageNew;
			dataPackageNew.userId = dataPackage.userId;
			dataPackageNew.userType = GetUserDataType::USER;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::UserData userData;
			DiscordCoreAPI::User user(userData);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<DiscordCoreAPI::User> fetchCurrentUserAsync() {
			co_await resume_foreground(*this->dispatcherQueue.get());
			GetUserData dataPackageNew;
			dataPackageNew.userType = GetUserDataType::SELF;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestGetUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::UserData userData;
			DiscordCoreAPI::User user(userData);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<DiscordCoreAPI::User> getUserAsync(DiscordCoreAPI::GetUserData dataPackage) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			CollectUserData dataPackageNew;
			dataPackageNew.userId = dataPackage.userId;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestCollectUserBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::UserData userData;
			DiscordCoreAPI::User user(userData);
			try_receive(requestAgent.outUserBuffer, user);
			co_return user;
		}

		task<void> leaveGuildAsync(DiscordCoreAPI::LeaveGuildData dataPackage) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			LeaveGuildData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestLeaveGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_return;
		}

		task<DiscordCoreAPI::Application> fetchApplicationDataAsync() {
			co_await resume_foreground(*this->dispatcherQueue.get());
			UserManagerAgent requestAgent{};
			send(requestAgent.requestGetApplicationBuffer, true);
			requestAgent.start();
			agent::wait(&requestAgent);
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
			co_return;
		}
	};
	overwrite_buffer<map<string, DiscordCoreAPI::User>> UserManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> UserManagerAgent::threadContext{ nullptr };
}
#endif