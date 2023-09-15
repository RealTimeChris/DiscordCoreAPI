/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// ThreadEntities.cpp - Source file for the thread-related stuff.
/// Nov 29, 2021
/// https://discordcoreapi.com
/// \file ThreadEntities.cpp

#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace jsonifier {

	template<> struct core<DiscordCoreAPI::ForumThreadMessageData> {
		using ValueType = DiscordCoreAPI::ForumThreadMessageData;
		static constexpr auto parseValue =
			createObject("attachments", &ValueType::attachments, "components", &ValueType::components, "allowedMentions", &ValueType::allowedMentions, "stickerIds",
				&ValueType::stickerIds, "embeds", &ValueType::embeds, "files", &ValueType::files, "content", &ValueType::content, "flags", &ValueType::flags);
	};

	template<> struct core<DiscordCoreAPI::StartThreadWithMessageData> {
		using ValueType					 = DiscordCoreAPI::StartThreadWithMessageData;
		static constexpr auto parseValue = createObject("autoArchiveDuration", &ValueType::autoArchiveDuration, "rateLimitPerUser", &ValueType::rateLimitPerUser, "threadName",
			&ValueType::threadName, "messageId", &ValueType::messageId, "channelId", &ValueType::channelId, "reason", &ValueType::reason);
	};

	template<> struct core<DiscordCoreAPI::StartThreadWithoutMessageData> {
		using ValueType = DiscordCoreAPI::StartThreadWithoutMessageData;
		static constexpr auto parseValue =
			createObject("autoArchiveDuration", &ValueType::autoArchiveDuration, "type", &ValueType::type, "rateLimitPerUser", &ValueType::rateLimitPerUser, "threadName",
				&ValueType::threadName, "channelId", &ValueType::channelId, "reason", &ValueType::reason, "invitable", &ValueType::invitable);
	};

	template<> struct core<DiscordCoreAPI::StartThreadInForumChannelData> {
		using ValueType					 = DiscordCoreAPI::StartThreadInForumChannelData;
		static constexpr auto parseValue = createObject("autoArchiveDuration", &ValueType::autoArchiveDuration, "message", &ValueType::message, "rateLimitPerUser",
			&ValueType::rateLimitPerUser, "channelId", &ValueType::channelId, "reason", &ValueType::reason, "name", &ValueType::name);
	};

}

namespace DiscordCoreAPI {

	void Threads::initialize(DiscordCoreInternal::HttpsClient* client) {
		Threads::httpsClient = client;
	}

	CoRoutine<ThreadData> Threads::startThreadWithMessageAsync(StartThreadWithMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_With_Message };
		co_await NewThreadAwaitable<ThreadData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Threads::startThreadWithMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		ThreadData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ThreadData> Threads::startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_Without_Message };
		co_await NewThreadAwaitable<ThreadData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Threads::startThreadWithoutMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		ThreadData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ThreadData> Threads::startThreadInForumChannelAsync(StartThreadInForumChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Thread_In_Forum_Channel };
		co_await NewThreadAwaitable<ThreadData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Threads::startThreadInForumChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		ThreadData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Threads::joinThreadAsync(JoinThreadData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Self_In_Thread };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/@me";
		workload.callStack	   = "Threads::joinThreadAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Threads::addThreadMemberAsync(AddThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Thread_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack	   = "Threads::addThreadMemberAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Threads::leaveThreadAsync(LeaveThreadData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Self_From_Thread };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/@me";
		workload.callStack	   = "Threads::leaveThreadAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Threads::removeThreadMemberAsync(RemoveThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Thread_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack	   = "Threads::removeThreadMemberAsync()";
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<ThreadMemberData> Threads::getThreadMemberAsync(GetThreadMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Thread_Member };
		co_await NewThreadAwaitable<ThreadMemberData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack	   = "Threads::getThreadMemberAsync()";
		ThreadMemberData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<jsonifier::vector<ThreadMemberData>> Threads::getThreadMembersAsync(GetThreadMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Thread_Members };
		co_await NewThreadAwaitable<jsonifier::vector<ThreadMemberData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members";
		workload.callStack	   = "Threads::getThreadMembersAsync()";
		jsonifier::vector<ThreadMemberData> returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveThreadsAsync(GetActiveThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Active_Threads };
		co_await NewThreadAwaitable<ActiveThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads/active";
		workload.callStack	   = "Threads::getActiveThreadsAsync()";
		ActiveThreadsData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ArchivedThreadsData> Threads::getPublicArchivedThreadsAsync(GetPublicArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Public_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads/archived/public";
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
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ArchivedThreadsData> Threads::getPrivateArchivedThreadsAsync(GetPrivateArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Private_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads/archived/protected";
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
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ArchivedThreadsData> Threads::getJoinedPrivateArchivedThreadsAsync(GetJoinedPrivateArchivedThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Joined_Private_Archived_Threads };
		co_await NewThreadAwaitable<ArchivedThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/users/@me/threads/archived/protected";
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
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveGuildThreadsAsync(GetActiveGuildThreadsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Active_Threads };
		co_await NewThreadAwaitable<ActiveThreadsData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/threads/active";
		workload.callStack	   = "Threads::listActiveThreadsAsync()";
		ActiveThreadsData returnData{};
		Threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	DiscordCoreInternal::HttpsClient* Threads::httpsClient{};
}
