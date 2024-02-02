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

	template<> struct core<discord_core_api::forum_thread_message_data> {
		using value_type = discord_core_api::forum_thread_message_data;
		static constexpr auto parseValue =
			createValue("attachments", &value_type::attachments, "components", &value_type::components, "allowedMentions", &value_type::allowedMentions, "stickerIds",
				&value_type::stickerIds, "embeds", &value_type::embeds, "files", &value_type::files, "content", &value_type::content, "flags", &value_type::flags);
	};

	template<> struct core<discord_core_api::start_thread_with_message_data> {
		using value_type				 = discord_core_api::start_thread_with_message_data;
		static constexpr auto parseValue = createValue("autoArchiveDuration", &value_type::autoArchiveDuration, "rateLimitPerUser", &value_type::rateLimitPerUser, "threadName",
			&value_type::threadName, "messageId", &value_type::messageId, "channelId", &value_type::channelId);
	};

	template<> struct core<discord_core_api::start_thread_without_message_data> {
		using value_type = discord_core_api::start_thread_without_message_data;
		static constexpr auto parseValue =
			createValue("autoArchiveDuration", &value_type::autoArchiveDuration, "type", &value_type::type, "rateLimitPerUser", &value_type::rateLimitPerUser, "threadName",
				&value_type::threadName, "channelId", &value_type::channelId, "invitable", &value_type::invitable);
	};

	template<> struct core<discord_core_api::start_thread_in_forum_channel_data> {
		using value_type				 = discord_core_api::start_thread_in_forum_channel_data;
		static constexpr auto parseValue = createValue("autoArchiveDuration", &value_type::autoArchiveDuration, "message", &value_type::message, "rateLimitPerUser",
			&value_type::rateLimitPerUser, "channelId", &value_type::channelId, "name", &value_type::name);
	};

}

namespace discord_core_api {

	void threads::initialize(discord_core_internal::https_client* client) {
		threads::httpsClient = client;
	}

	co_routine<thread_data> threads::startThreadWithMessageAsync(start_thread_with_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Thread_With_Message };
		co_await newThreadAwaitable<thread_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "threads::startThreadWithMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		thread_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<thread_data> threads::startThreadWithoutMessageAsync(start_thread_without_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Thread_Without_Message };
		co_await newThreadAwaitable<thread_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "threads::startThreadWithoutMessageAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		thread_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<thread_data> threads::startThreadInForumChannelAsync(start_thread_in_forum_channel_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Thread_In_Forum_Channel };
		co_await newThreadAwaitable<thread_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "threads::startThreadInForumChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		thread_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> threads::joinThreadAsync(join_thread_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Self_In_Thread };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/@me";
		workload.callStack	   = "threads::joinThreadAsync()";
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> threads::addThreadMemberAsync(add_thread_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Thread_Member };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack	   = "threads::addThreadMemberAsync()";
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> threads::leaveThreadAsync(leave_thread_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Self_From_Thread };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/@me";
		workload.callStack	   = "threads::leaveThreadAsync()";
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> threads::removeThreadMemberAsync(remove_thread_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Thread_Member };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack	   = "threads::removeThreadMemberAsync()";
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<thread_member_data> threads::getThreadMemberAsync(get_thread_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Thread_Member };
		co_await newThreadAwaitable<thread_member_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
		workload.callStack	   = "threads::getThreadMemberAsync()";
		thread_member_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<thread_member_data>> threads::getThreadMembersAsync(get_thread_members_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Thread_Members };
		co_await newThreadAwaitable<jsonifier::vector<thread_member_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/thread-members";
		workload.callStack	   = "threads::getThreadMembersAsync()";
		jsonifier::vector<thread_member_data> returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<active_threads_data> threads::getActiveThreadsAsync(get_active_threads_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Active_Threads };
		co_await newThreadAwaitable<active_threads_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads/active";
		workload.callStack	   = "threads::getActiveThreadsAsync()";
		active_threads_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<archived_threads_data> threads::getPublicArchivedThreadsAsync(get_public_archived_threads_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Public_Archived_Threads };
		co_await newThreadAwaitable<archived_threads_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads/archived/public";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "threads::getPublicArchivedThreadsAsync()";
		archived_threads_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<archived_threads_data> threads::getPrivateArchivedThreadsAsync(get_private_archived_threads_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Private_Archived_Threads };
		co_await newThreadAwaitable<archived_threads_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/threads/archived/protected";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "threads::getPrivateArchivedThreadsAsync()";
		archived_threads_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<archived_threads_data> threads::getJoinedPrivateArchivedThreadsAsync(get_joined_private_archived_threads_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Joined_Private_Archived_Threads };
		co_await newThreadAwaitable<archived_threads_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/users/@me/threads/archived/protected";
		if (dataPackage.before != "") {
			workload.relativePath += "?before=" + dataPackage.before;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "threads::getJoinedPrivateArchivedThreadsAsync()";
		archived_threads_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<active_threads_data> threads::getActiveGuildThreadsAsync(get_active_guild_threads_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Active_Threads };
		co_await newThreadAwaitable<active_threads_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/threads/active";
		workload.callStack	   = "threads::listActiveThreadsAsync()";
		active_threads_data returnData{};
		threads::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	discord_core_internal::https_client* threads::httpsClient{};
}
