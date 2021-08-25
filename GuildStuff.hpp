// GuildStuff.hpp - First header for the "Guild Stuff" of the DiscordCore library.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_
#define _GUILD_STUFF_

#include "../pch.h"
#include "DataParsingFunctions.hpp"
#include "HttpStuff.hpp"
#include "WebSocketStuff.hpp"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"
#include "VoiceConnectionStuff.hpp"
#include "DiscordCoreClientBase.hpp"
#include "YouTubeStuff.hpp"

namespace DiscordCoreAPI {

	class Guild {
	public:
		GuildData data{};
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		shared_ptr<VoiceConnection> connectToVoice(string channelId) {
			shared_ptr<VoiceConnection> voiceConnectionPtr;
			if (DiscordCoreClientBase::voiceConnectionMap->contains(this->data.id)) {
				voiceConnectionPtr = DiscordCoreClientBase::voiceConnectionMap->at(this->data.id);
				return voiceConnectionPtr;
			}
			else if (channelId != "") {
				if ((voiceConnectionPtr == nullptr || voiceConnectionPtr->voiceConnectionData.channelId != channelId)) {
					auto voiceConnectData = DiscordCoreClientBase::pWebSocketConnectionAgent->getVoiceConnectionData(channelId, this->data.id);
					voiceConnectData.channelId = channelId;
					voiceConnectData.guildId = this->data.id;
					voiceConnectData.endpoint = "wss://" + voiceConnectData.endpoint + "/?v=4";
					voiceConnectData.userId = this->discordCoreClientBase->currentUser->data.id;
					if (this->discordCoreClientBase->audioBuffersMap.contains(this->data.id)) {
						voiceConnectionPtr = make_shared<VoiceConnection>(DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get(), voiceConnectData, DiscordCoreClientBase::audioBuffersMap.at(this->data.id));
					}
					else {
						this->discordCoreClientBase->audioBuffersMap.insert(make_pair(this->data.id, make_shared<unbounded_buffer<AudioFrameData*>>()));
						voiceConnectionPtr = make_shared<VoiceConnection>(DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get(), voiceConnectData, DiscordCoreClientBase::audioBuffersMap.at(this->data.id));
					}
					DiscordCoreClientBase::pWebSocketConnectionAgent->setVoiceConnectionWebSocket(voiceConnectionPtr->voicechannelWebSocketAgent);
					DiscordCoreClientBase::voiceConnectionMap->insert(make_pair(this->data.id, voiceConnectionPtr));
					return voiceConnectionPtr;
				}
			}
			return voiceConnectionPtr;
		}

		shared_ptr<YouTubeAPI> getYouTubeAPI() {
			if (DiscordCoreClientBase::youtubeAPIMap->contains(this->data.id)) {
				return DiscordCoreClientBase::youtubeAPIMap->at(this->data.id);
			}
			else {
				return nullptr;
			}
		}

	protected:
		friend class GuildManagerAgent;
		friend class DiscordCoreClient;
		friend class GuildManager;
		friend class VoiceConnection;
		friend class DiscordCoreClientBase;
		friend struct concurrency::details::_ResultHolder<Guild>;
		friend struct OnGuildCreationData;
		friend struct OnGuildUpdateData;
		friend struct OnGuildDeletionData;
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };

		Guild() {};

		Guild(DiscordCoreInternal::HttpAgentResources agentResourcesNew, GuildData dataNew, shared_ptr<DiscordCoreClient> discordCoreClientNew, shared_ptr<DiscordCoreClientBase> discordCoreClientBaseNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->discordCoreClientBase = discordCoreClientBaseNew;
			this->data = dataNew;
			if (DiscordCoreClientBase::youtubeAPIMap->contains(this->data.id)) {
			}
			else {
				if (DiscordCoreClientBase::audioBuffersMap.contains(this->data.id)) {
					shared_ptr<YouTubeAPI> sharedPtr = make_shared<YouTubeAPI>(DiscordCoreClientBase::audioBuffersMap.at(this->data.id), this->data.id, DiscordCoreClientBase::youtubeAPIMap);
					DiscordCoreClientBase::youtubeAPIMap->insert_or_assign(this->data.id, sharedPtr);
				}
				else {
					shared_ptr<unbounded_buffer<AudioFrameData*>> sharedPtrBuffer = make_shared<unbounded_buffer<AudioFrameData*>>();
					DiscordCoreClientBase::audioBuffersMap.insert(make_pair(this->data.id, sharedPtrBuffer));
					shared_ptr<YouTubeAPI> sharedPtr = make_shared<YouTubeAPI>(DiscordCoreClientBase::audioBuffersMap.at(this->data.id), this->data.id, DiscordCoreClientBase::youtubeAPIMap);
					DiscordCoreClientBase::youtubeAPIMap->insert_or_assign(this->data.id, sharedPtr);
				}
			}
			return;
		}

		void initialize() {
			try {
				cout << "Caching guild: " << this->data.name << endl;
				cout << "Caching channels for guild: " << this->data.name << endl;
				for (auto value : data.channels) {
					value.guildId = this->data.id;
					ChannelData channelData = value;
					Channel channel(channelData, this->discordCoreClient);
					this->discordCoreClientBase->channels->insertChannelAsync(channel).get();
				}
				cout << "Caching guild members for guild: " << this->data.name << endl;
				for (unsigned int x = 0; x < this->data.members.size(); x += 1) {
					GuildMemberData guildMemberData = this->data.members.at(x);
					for (auto value : this->data.voiceStates) {
						if (value.userId == guildMemberData.user.id) {
							guildMemberData.voiceData = value;
						}
					}
					guildMemberData.guildId = this->data.id;
					DiscordGuildMember discordGuildMember(guildMemberData);
					discordGuildMember.writeDataToDB();
					GuildMember guildMember(guildMemberData, this->data.id, this->discordCoreClient);
					this->discordCoreClientBase->guildMembers->insertGuildMemberAsync(guildMember, this->data.id).get();
				}
				cout << "Caching roles for guild: " << this->data.name << endl;
				for (auto const& value : data.roles) {
					RoleData roleData = value;
					Role role(roleData, this->discordCoreClient);
					this->discordCoreClientBase->roles->insertRoleAsync(role).get();
				}
				cout << "Caching users for guild: " << this->data.name << endl << endl;
				for (auto value : data.members) {
					DiscordCoreInternal::UserData userData = value.user;
					User user(userData, this->discordCoreClient);
					this->discordCoreClientBase->users->insertUserAsync(user).get();
				}
			}
			catch (exception error) {
				cout << "Error: " << error.what() << endl;
			}
			return;
		}
	};

	struct GetGuildData {
		string guildId{ "" };
	};

	struct FetchVanityInviteData {
		string guildId{ "" };
	};

	struct FetchGuildData {
		string guildId{ "" };
	};

	struct FetchAuditLogData {
		AuditLogEvent actionType{};
		string guildId{ "" };
		unsigned int limit{ 0 };
		string userId{ "" };
	};

	struct FetchInviteData {
		string inviteId{ "" };
	};

	struct FetchInvitesData {
		string guildId{ "" };
	};

	class GuildManagerAgent : agent {
	protected:
		friend class DiscordCoreClient;
		friend class GuildManager;
		friend class Guild;
		friend class EventHandler;

		static overwrite_buffer<map<string, Guild>> cache;
		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;

		unbounded_buffer<DiscordCoreInternal::GetVanityInviteData> requestGetVanityInviteBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::PutGuildBanData> requestPutGuildBanBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetAuditLogData> requestGetAuditLogBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetGuildData> requestGetGuildBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetInvitesData> requestGetInvitesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::GetInviteData> requestGetInviteBuffer{ nullptr };
		unbounded_buffer<DiscordCoreInternal::CollectGuildData> requestCollectGuildBuffer{ nullptr };
		unbounded_buffer<vector<InviteData>> outInvitesBuffer{ nullptr };
		unbounded_buffer<AuditLogData> outAuditLogBuffer{ nullptr };
		unbounded_buffer<InviteData> outInviteBuffer{ nullptr };
		unbounded_buffer<exception> errorBuffer{ nullptr };
		unbounded_buffer<BanData> outBanBuffer{ nullptr };
		unbounded_buffer<Guild> outGuildBuffer{ nullptr };
		concurrent_queue<Guild> guildsToInsert{};
		
		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };

		GuildManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew,  shared_ptr<DiscordCoreClient> coreClientNew, shared_ptr<DiscordCoreClientBase> coreClientBaseNew)
			:agent(*GuildManagerAgent::threadContext->scheduler) {
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
			return;
		}

		static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
			GuildManagerAgent::threadContext = threadContextNew;
		}

		static void cleanup() {
			GuildManagerAgent::threadContext->releaseGroup();
		}

		void getError(string stackTrace) {
			exception error;
			while (try_receive(errorBuffer, error)) {
				cout << stackTrace + "::GuildManagerAgent Error: " << error.what() << endl << endl;
			}
			return;
		}

		Guild getObjectData(DiscordCoreInternal::GetGuildData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_GUILD;
			workload.relativePath = "/guilds/" + dataPackage.guildId;
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			GuildData guildData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &guildData);
			Guild guildNew(this->agentResources, guildData, this->discordCoreClient, this->discordCoreClientBase);
			return guildNew;
		}

		InviteData getObjectData(DiscordCoreInternal::GetInviteData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_INVITE;
			workload.relativePath = "/invites/" + dataPackage.inviteId + "?with_counts=true&with_expiration=true";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData_01");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			InviteData inviteData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &inviteData);
			return inviteData;
		}

		vector<InviteData> getObjectData(DiscordCoreInternal::GetInvitesData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_INVITES;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/invites";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData_02");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			vector<InviteData> inviteData;
			for (auto value : returnData.data) {
				InviteData inviteDataNew;
				DiscordCoreInternal::DataParser::parseObject(value, &inviteDataNew);
				inviteData.push_back(inviteDataNew);
			}
			return inviteData;
		}

		InviteData getObjectData(DiscordCoreInternal::GetVanityInviteData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_VANITY_INVITE;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/vanity-url";
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData_03");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_03 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_03 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			InviteData inviteData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &inviteData);
			return inviteData;
		}

		AuditLogData getObjectData(DiscordCoreInternal::GetAuditLogData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_AUDIT_LOG;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/audit-logs";
			if (dataPackage.userId != "") {
				workload.relativePath += "?user_id=" + dataPackage.userId;
				if (to_string((int)dataPackage.actionType) != "") {
					workload.relativePath += "&action_type=" + to_string((int)dataPackage.actionType);
				}
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (to_string((int)dataPackage.actionType) != "") {
				workload.relativePath += "?action_type=" + to_string((int)dataPackage.actionType);
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + to_string(dataPackage.limit);
				}
			}
			else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + to_string(dataPackage.limit);
			}
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::getObjectData_04");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::getObjectData_04 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			else {
				cout << "GuildManagerAgent::getObjectData_04 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			AuditLogData auditLogData;
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &auditLogData);
			return auditLogData;
		}

		BanData putObjectData(DiscordCoreInternal::PutGuildBanData dataPackage) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PUT;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PUT_GUILD_BAN;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/bans/" + dataPackage.guildMemberId;
			workload.content = DiscordCoreInternal::getAddBanPayload(dataPackage);
			DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManagerAgent::putObjectData_00");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			BanData banData;
			if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
				cout << "GuildManagerAgent::putObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				banData.failedDueToPerms = true;
			}
			else {
				cout << "GuildManagerAgent::putObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
			}
			DiscordCoreInternal::DataParser::parseObject(returnData.data, &banData);
			return banData;
		}

		void run() {
			try {
				DiscordCoreInternal::CollectGuildData dataPackage01;
				if (try_receive(this->requestCollectGuildBuffer, dataPackage01)) {
					map<string, Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage01.guildId)) {
							Guild guild = cacheTemp.at(dataPackage01.guildId);
							send(this->outGuildBuffer, guild);
						}
					}
				}
				DiscordCoreInternal::GetGuildData dataPackage02;
				if (try_receive(this->requestGetGuildBuffer, dataPackage02)) {
					map<string, Guild> cacheTemp;
					if (try_receive(GuildManagerAgent::cache, cacheTemp)) {
						if (cacheTemp.contains(dataPackage02.guildId)) {
							cacheTemp.erase(dataPackage02.guildId);
						}
					}
					Guild guild = getObjectData(dataPackage02);
					cacheTemp.insert(make_pair(dataPackage02.guildId, guild));
					send(this->outGuildBuffer, guild);
					send(GuildManagerAgent::cache, cacheTemp);
				}
				DiscordCoreInternal::GetAuditLogData dataPackage03;
				if (try_receive(this->requestGetAuditLogBuffer, dataPackage03)) {
					AuditLogData auditLog = getObjectData(dataPackage03);
					send(this->outAuditLogBuffer, auditLog);
				}
				DiscordCoreInternal::GetInvitesData dataPackage04;
				if (try_receive(this->requestGetInvitesBuffer, dataPackage04)) {
					vector<InviteData> inviteData = getObjectData(dataPackage04);
					send(this->outInvitesBuffer, inviteData);
				}
				DiscordCoreInternal::GetInviteData dataPackage05;
				if (try_receive(this->requestGetInviteBuffer, dataPackage05)) {
					InviteData inviteData = getObjectData(dataPackage05);
					send(this->outInviteBuffer, inviteData);
				}
				DiscordCoreInternal::GetVanityInviteData dataPackage06;
				if (try_receive(this->requestGetVanityInviteBuffer, dataPackage06)) {
					InviteData inviteData = getObjectData(dataPackage06);
					send(this->outInviteBuffer, inviteData);
				}
				DiscordCoreInternal::PutGuildBanData dataPackage07;
				if (try_receive(this->requestPutGuildBanBuffer, dataPackage07)) {
					BanData inviteData = putObjectData(dataPackage07);
					send(this->outBanBuffer, inviteData);
				}
				Guild guildNew;
				while (this->guildsToInsert.try_pop(guildNew)) {
					map<string, Guild> cacheTemp;
					try_receive(GuildManagerAgent::cache, cacheTemp);
					if (cacheTemp.contains(guildNew.data.id)) {
						cacheTemp.erase(guildNew.data.id);
					}
					guildNew.initialize();
					cacheTemp.insert(make_pair(guildNew.data.id, guildNew));
					send(GuildManagerAgent::cache, cacheTemp);
				}
			}
			catch (const exception& e) {
				send(this->errorBuffer, e);
			}
			done();
			return;
		}
	};

	class GuildManager {
	public:

		GuildManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, shared_ptr<DiscordCoreClient> coreClientNew, shared_ptr<DiscordCoreClientBase> coreClientBaseNew) {
			this->threadContext = threadContextNew;
			this->agentResources = agentResourcesNew;
			this->discordCoreClient = coreClientNew;
			this->discordCoreClientBase = coreClientBaseNew;
		}

		task<Guild> fetchAsync(FetchGuildData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::fetchAsync");
			GuildData guildData;
			Guild guild(this->agentResources, guildData, this->discordCoreClient, this->discordCoreClientBase);
			try_receive(requestAgent.outGuildBuffer, guild);
			co_return guild;
		}

		task<vector<InviteData>> fetchInvitesAsync(FetchInvitesData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetInvitesData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetInvitesBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::getInvitesAsync");
			vector<InviteData> inviteData;
			try_receive(requestAgent.outInvitesBuffer, inviteData);
			co_return inviteData;
		}

		task<BanData> createGuildBanAsync(CreateGuildBanData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::PutGuildBanData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.guildMemberId = dataPackage.guildMemberId;
			dataPackageNew.deleteMessageDays = dataPackage.deleteMessageDays;
			dataPackageNew.reason = dataPackage.reason;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestPutGuildBanBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::createGuildBanAsync");
			BanData banData;
			try_receive(requestAgent.outBanBuffer, banData);
			co_return banData;
		}

		task<InviteData> fetchVanityInviteAsync(FetchVanityInviteData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetVanityInviteData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetVanityInviteBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::getVanityInviteAsync");
			InviteData inviteData;
			try_receive(requestAgent.outInviteBuffer, inviteData);
			co_return inviteData;
		}

		task<InviteData> fetchInviteAsync(FetchInviteData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetInviteData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.inviteId = dataPackage.inviteId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetInviteBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::getInviteAsync");
			InviteData inviteData;
			try_receive(requestAgent.outInviteBuffer, inviteData);
			co_return inviteData;
		}

		task<AuditLogData> fetchAuditLogDataAsync(FetchAuditLogData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::GetAuditLogData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			dataPackageNew.actionType = (DiscordCoreInternal::AuditLogEvent)dataPackage.actionType;
			dataPackageNew.limit = dataPackage.limit;
			dataPackageNew.userId = dataPackage.userId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestGetAuditLogBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::getAuditLogDataAsync");
			AuditLogData auditLog;
			try_receive(requestAgent.outAuditLogBuffer, auditLog);
			co_return auditLog;
		}

		task<Guild> getGuildAsync(GetGuildData dataPackage) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			DiscordCoreInternal::CollectGuildData dataPackageNew;
			dataPackageNew.agentResources = this->agentResources;
			dataPackageNew.guildId = dataPackage.guildId;
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			send(requestAgent.requestCollectGuildBuffer, dataPackageNew);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::getGuildAsync");
			GuildData guildData;
			Guild guild(this->agentResources, guildData, this->discordCoreClient, this->discordCoreClientBase);
			try_receive(requestAgent.outGuildBuffer, guild);
			co_return guild;
		}

		task<vector<Guild>> getAllGuildsAsync() {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Guild>cache;
			try_receive(GuildManagerAgent::cache, cache);
			vector<Guild> guildVector;
			for (auto [key, value] : cache) {
				guildVector.push_back(value);
			}
			send(GuildManagerAgent::cache, cache);
			co_return guildVector;
		}

		~GuildManager() {
			this->threadContext->releaseGroup();
		}

	protected:
		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Guilds;

		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources{};
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };

		task<void> insertGuildAsync(Guild guild) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			GuildManagerAgent requestAgent(this->agentResources, this->discordCoreClient, this->discordCoreClientBase);
			requestAgent.guildsToInsert.push(guild);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("GuildManager::insertGuildAsync");
			co_return;
		}

		task<void> removeGuildAsync(string guildId) {
			co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
			map<string, Guild> cache;
			try_receive(GuildManagerAgent::cache, cache);
			if (cache.contains(guildId)) {
				cache.erase(guildId);
			}
			send(GuildManagerAgent::cache, cache);
			co_return;
		}

	};
	overwrite_buffer<map<string, Guild>> GuildManagerAgent::cache{ nullptr };
	shared_ptr<DiscordCoreInternal::ThreadContext> GuildManagerAgent::threadContext{ nullptr };
}
#endif
