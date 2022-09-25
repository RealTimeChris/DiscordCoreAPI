/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// ThreadEntities.cpp - Source file for the thread-related stuff.
/// Nov 29, 2021
/// https://discordcoreapi.com
/// \file ThreadEntities.cpp

#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	StartThreadWithMessageData::operator JsonObject() {
		JsonObject theData{};
		theData["auto_archive_duration"] = this->autoArchiveDuration;
		theData["rate_limit_per_user"] = this->rateLimitPerUser;
		theData["name"] = this->threadName;
		return theData;
	}

	StartThreadWithoutMessageData::operator JsonObject() {
		JsonObject theData{};
		theData["auto_archive_duration"] = this->autoArchiveDuration;
		theData["rate_limit_per_user"] = this->rateLimitPerUser;
		theData["invitable"] = this->invitable;
		theData["name"] = this->threadName;
		theData["type"] = this->type;
		return theData;
	}

	StartThreadInForumChannelData::operator JsonObject() {
		JsonObject theData{};
		if (this->message.allowedMentions.parse.size() > 0 || this->message.allowedMentions.roles.size() > 0 || this->message.allowedMentions.users.size() > 0) {
			theData["message"]["allowed_mentions"]["roles"] = this->message.allowedMentions.roles;
			theData["message"]["allowed_mentions"]["parse"] = this->message.allowedMentions.parse;
			theData["message"]["allowed_mentions"]["users"] = this->message.allowedMentions.users;
		}
		for (auto& value: this->message.attachments) {
			theData["message"].pushBack("attachments", value);
		}
		for (auto& value: this->message.components) {
			theData["message"].pushBack("components", value);
		}
		for (auto& value: this->message.stickerIds) {
			theData["message"].pushBack("sticker_ids", value);
		}
		for (auto& value: this->message.embeds) {
			theData["message"].pushBack("embeds", value);
		}
		if (this->message.content != "") {
			theData["message"]["content"] = this->message.content;
		}
		theData["message"]["flags"] = this->message.flags;
		theData["name"] = this->name;
		theData["auto_archive_duration"] = this->autoArchiveDuration;
		theData["rate_limit_per_user"] = this->rateLimitPerUser;
		return theData;
	}

	void Threads::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		Threads::httpsClient = theClient;
	}

	CoRoutine<Thread> Threads::startThreadWithMessageAsync(StartThreadWithMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_With_Message };
		co_await NewThreadAwaitable<Thread>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/messages/" + std::to_string(dataPackage.messageId) + "/threads";
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Threads::startThreadWithMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Threads::httpsClient->submitWorkloadAndGetResult<Thread>(workload);
	}

	CoRoutine<Thread> Threads::startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_Without_Message };
		co_await NewThreadAwaitable<Thread>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/threads";
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Threads::startThreadWithoutMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Threads::httpsClient->submitWorkloadAndGetResult<Thread>(workload);
	}

	CoRoutine<Thread> Threads::startThreadInForumChannelAsync(StartThreadInForumChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_In_Forum_Channel };
		co_await NewThreadAwaitable<Thread>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/threads";
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Threads::startThreadInForumChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Threads::httpsClient->submitWorkloadAndGetResult<Thread>(workload);
	}

	CoRoutine<void> Threads::joinThreadAsync(JoinThreadData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Self_In_Thread };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/thread-members/@me";
		workload.callStack = "Threads::joinThreadAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Threads::addThreadMemberAsync(AddThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Thread_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/thread-members/" + std::to_string(dataPackage.userId);
		workload.callStack = "Threads::addThreadMemberAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Threads::leaveThreadAsync(LeaveThreadData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Self_From_Thread };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/thread-members/@me";
		workload.callStack = "Threads::leaveThreadAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Threads::removeThreadMemberAsync(RemoveThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Thread_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/thread-members/" + std::to_string(dataPackage.userId);
		workload.callStack = "Threads::removeThreadMemberAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<ThreadMemberData> Threads::getThreadMemberAsync(GetThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Thread_Member };
		co_await NewThreadAwaitable<ThreadMemberData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/thread-members/" + std::to_string(dataPackage.userId);
		workload.callStack = "Threads::getThreadMemberAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<ThreadMemberData>(workload);
	}

	CoRoutine<std::vector<ThreadMemberData>> Threads::getThreadMembersAsync(GetThreadMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Thread_Members };
		co_await NewThreadAwaitable<std::vector<ThreadMemberData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/thread-members";
		workload.callStack = "Threads::getThreadMembersAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<ThreadMemberDataVector>(workload);
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveThreadsAsync(GetActiveThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Active_Threads };
		co_await NewThreadAwaitable<ActiveThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/threads/active";
		workload.callStack = "Threads::getActiveThreadsAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<ActiveThreadsData>(workload);
	}

	CoRoutine<ArchivedThreadsData> Threads::getPublicArchivedThreadsAsync(GetPublicArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Public_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/threads/archived/public";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Threads::getPublicArchivedThreadsAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<ArchivedThreadsData>(workload);
	}

	CoRoutine<ArchivedThreadsData> Threads::getPrivateArchivedThreadsAsync(GetPrivateArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Private_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/threads/archived/private";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Threads::getPrivateArchivedThreadsAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<ArchivedThreadsData>(workload);
	}

	CoRoutine<ArchivedThreadsData> Threads::getJoinedPrivateArchivedThreadsAsync(GetJoinedPrivateArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Joined_Private_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/users/@me/threads/archived/private";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Threads::getJoinedPrivateArchivedThreadsAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<ArchivedThreadsData>(workload);
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveGuildThreadsAsync(GetActiveGuildThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Active_Threads };
		co_await NewThreadAwaitable<ActiveThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/threads/active";
		workload.callStack = "Threads::listActiveThreadsAsync()";
		co_return Threads::httpsClient->submitWorkloadAndGetResult<ActiveThreadsData>(workload);
	}
	DiscordCoreInternal::HttpsClient* Threads::httpsClient{ nullptr };
}