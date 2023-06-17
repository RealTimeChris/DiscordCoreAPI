/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::ForumThreadMessageData> {
		using ValueType = DiscordCoreAPI::ForumThreadMessageData;
		static constexpr auto parseValue = object("attachments", &ValueType::attachments, "components", &ValueType::components, "allowedMentions",
			&ValueType::allowedMentions, "stickerIds", &ValueType::stickerIds, "embeds", &ValueType::embeds, "files", &ValueType::files, "content",
			&ValueType::content, "flags", &ValueType::flags);
	};

	template<> struct Core<DiscordCoreAPI::StartThreadWithMessageData> {
		using ValueType = DiscordCoreAPI::StartThreadWithMessageData;
		static constexpr auto parseValue =
			object("autoArchiveDuration", &ValueType::autoArchiveDuration, "rateLimitPerUser", &ValueType::rateLimitPerUser, "threadName",
				&ValueType::threadName, "messageId", &ValueType::messageId, "channelId", &ValueType::channelId, "reason", &ValueType::reason);
	};

	template<> struct Core<DiscordCoreAPI::StartThreadWithoutMessageData> {
		using ValueType = DiscordCoreAPI::StartThreadWithoutMessageData;
		static constexpr auto parseValue = object("autoArchiveDuration", &ValueType::autoArchiveDuration, "type", &ValueType::type,
			"rateLimitPerUser", &ValueType::rateLimitPerUser, "threadName", &ValueType::threadName, "channelId", &ValueType::channelId, "reason",
			&ValueType::reason, "invitable", &ValueType::invitable);
	};

	template<> struct Core<DiscordCoreAPI::StartThreadInForumChannelData> {
		using ValueType = DiscordCoreAPI::StartThreadInForumChannelData;
		static constexpr auto parseValue =
			object("autoArchiveDuration", &ValueType::autoArchiveDuration, "message", &ValueType::message, "rateLimitPerUser",
				&ValueType::rateLimitPerUser, "channelId", &ValueType::channelId, "reason", &ValueType::reason, "name", &ValueType::name);
	};

}

namespace DiscordCoreAPI {



	void Threads::initialize(DiscordCoreInternal::HttpsClient* client) {
		Threads::httpsClient = client;
	}

	CoRoutine<Thread> Threads::startThreadWithMessageAsync(StartThreadWithMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_With_Message };
		co_await NewThreadAwaitable<Thread>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Threads::startThreadWithMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Thread returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<Thread>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<Thread> Threads::startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_Without_Message };
		co_await NewThreadAwaitable<Thread>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Threads::startThreadWithoutMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Thread returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<Thread>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<Thread> Threads::startThreadInForumChannelAsync(StartThreadInForumChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_In_Forum_Channel };
		co_await NewThreadAwaitable<Thread>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Threads::startThreadInForumChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Thread returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<Thread>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<void> Threads::joinThreadAsync(JoinThreadData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Self_In_Thread };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/@me";
		workload.callStack = "Threads::joinThreadAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<void> Threads::addThreadMemberAsync(AddThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Thread_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack = "Threads::addThreadMemberAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<void> Threads::leaveThreadAsync(LeaveThreadData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Self_From_Thread };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/@me";
		workload.callStack = "Threads::leaveThreadAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<void> Threads::removeThreadMemberAsync(RemoveThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Thread_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack = "Threads::removeThreadMemberAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult<void>(std::move(workload));
		co_return;
	}

	CoRoutine<ThreadMemberData> Threads::getThreadMemberAsync(GetThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Thread_Member };
		co_await NewThreadAwaitable<ThreadMemberData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack = "Threads::getThreadMemberAsync()";
		ThreadMemberData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<ThreadMemberData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<ThreadMemberDataVector> Threads::getThreadMembersAsync(GetThreadMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Thread_Members };
		co_await NewThreadAwaitable<ThreadMemberDataVector>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members";
		workload.callStack = "Threads::getThreadMembersAsync()";
		ThreadMemberDataVector returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<ThreadMemberDataVector>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveThreadsAsync(GetActiveThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Active_Threads };
		co_await NewThreadAwaitable<ActiveThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/threads/active";
		workload.callStack = "Threads::getActiveThreadsAsync()";
		ActiveThreadsData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<ActiveThreadsData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<ArchivedThreadsData> Threads::getPublicArchivedThreadsAsync(GetPublicArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Public_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/threads/archived/public";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Threads::getPublicArchivedThreadsAsync()";
		ArchivedThreadsData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<ArchivedThreadsData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<ArchivedThreadsData> Threads::getPrivateArchivedThreadsAsync(GetPrivateArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Private_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/threads/archived/protected";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Threads::getPrivateArchivedThreadsAsync()";
		ArchivedThreadsData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<ArchivedThreadsData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<ArchivedThreadsData> Threads::getJoinedPrivateArchivedThreadsAsync(GetJoinedPrivateArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Joined_Private_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/users/@me/threads/archived/protected";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "Threads::getJoinedPrivateArchivedThreadsAsync()";
		ArchivedThreadsData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<ArchivedThreadsData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveGuildThreadsAsync(GetActiveGuildThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Active_Threads };
		co_await NewThreadAwaitable<ActiveThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/threads/active";
		workload.callStack = "Threads::listActiveThreadsAsync()";
		ActiveThreadsData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult<ActiveThreadsData>(std::move(workload), returnData);
		co_return std::move(returnData);
	}

	DiscordCoreInternal::HttpsClient* Threads::httpsClient{};
}