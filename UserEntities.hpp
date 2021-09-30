// UserEntities.hpp - Header for user related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _USER_ENTITIES_
#define _USER_ENTITIES_

#include "../pch.h"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "WebSocketEntities.hpp"

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
		friend BotUser getBotData();

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

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<DiscordCoreAPI::Application> outApplicationBuffer{ nullptr };
		unbounded_buffer<LeaveGuildData> requestLeaveGuildBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::User> outUserBuffer{ nullptr };
		unbounded_buffer<bool> requestGetApplicationBuffer{ nullptr };
		unbounded_buffer<GetUserData> requestGetUserBuffer{ nullptr };

		UserManagerAgent()
			:agent(*UserManagerAgent::threadContext->scheduler->scheduler) {}

		static void intialize() {
			UserManagerAgent::threadContext = ThreadManager::getThreadContext().get();
		}

		static void cleanup() {
			UserManagerAgent::threadContext->releaseContext();
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
				GetUserData dataPackage02;
				if (try_receive(this->requestGetUserBuffer, dataPackage02)) {
					DiscordCoreAPI::User user = getObjectData(dataPackage02);
					send(this->outUserBuffer, user);
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
			this->cache = new overwrite_buffer<map<string, DiscordCoreAPI::User>>();
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

		~UserManager() {
			delete this->cache;
			this->cache = nullptr;
		}

	protected:

		overwrite_buffer<map<string, DiscordCoreAPI::User>>* cache{};

		task<DiscordCoreAPI::User> fetchAsync(DiscordCoreAPI::FetchUserData dataPackage) {
			apartment_context mainThread{};
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
			map<string, DiscordCoreAPI::User> cacheTemp;
			try_receive(this->cache, cacheTemp);
			cacheTemp.insert_or_assign(dataPackage.userId, user);
			send(this->cache, cacheTemp);
			co_await mainThread;
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
			map<string, DiscordCoreAPI::User> cacheTemp;
			try_receive(this->cache, cacheTemp);
			if (user.id != "") {
				cacheTemp.insert_or_assign(user.id, user);
			}
			send(this->cache, cacheTemp);
			co_return user;
		}

		task<DiscordCoreAPI::User> getUserAsync(DiscordCoreAPI::GetUserData dataPackage) {
			co_await resume_foreground(*this->dispatcherQueue);
			map<string, DiscordCoreAPI::User> cacheTemp;
			DiscordCoreAPI::UserData userData;
			DiscordCoreAPI::User user(userData);
			if (try_receive(this->cache, cacheTemp)) {
				if (cacheTemp.contains(dataPackage.userId)) {
					user = cacheTemp.at(dataPackage.userId);
				}
			}
			co_return user;
		}

		task<void> leaveGuildAsync(DiscordCoreAPI::LeaveGuildData dataPackage) {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			LeaveGuildData dataPackageNew;
			dataPackageNew.guildId = dataPackage.guildId;
			UserManagerAgent requestAgent{};
			send(requestAgent.requestLeaveGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			co_await mainThread;
			co_return;
		}

		task<DiscordCoreAPI::Application> fetchApplicationDataAsync() {
			apartment_context mainThread{};
			co_await resume_foreground(*this->dispatcherQueue.get());
			UserManagerAgent requestAgent{};
			send(requestAgent.requestGetApplicationBuffer, true);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreAPI::ApplicationData applicationData;
			DiscordCoreAPI::Application application(applicationData);
			try_receive(requestAgent.outApplicationBuffer, application);
			co_await mainThread;
			co_return application;
		}

		task<void> insertUserAsync(DiscordCoreAPI::User user) {
			co_await resume_foreground(*this->dispatcherQueue.get());
			map<string, DiscordCoreAPI::User> cacheTemp{};
			try_receive(this->cache, cacheTemp);
			cacheTemp.insert_or_assign(user.id, user);
			send(this->cache, cacheTemp);
			co_return;
		}
	};
	shared_ptr<ThreadContext> UserManagerAgent::threadContext{ nullptr };
}
#endif