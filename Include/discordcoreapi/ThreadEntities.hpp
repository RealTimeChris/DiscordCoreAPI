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
/// ThreadEntities.hpp - Header file for the thread-related stuff.
/// Nov 29, 2021
/// https://discordcoreapi.com
/// \file ThreadEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/ChannelEntities.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For starting a thread_data, based on a message.
	struct start_thread_with_message_data {
		thread_auto_archive_duration autoArchiveDuration{ thread_auto_archive_duration::Shortest };///< The duration before it is auto-archived, in minutes.
		int32_t rateLimitPerUser{};///< Integer amount of seconds a user has to wait before sending another message(0 - 21600).
		jsonifier::string threadName{};///< The name of the new thread_data.
		snowflake messageId{};///< The message snowflake to base the thread_data off of.
		snowflake channelId{};///< The channel_data to start the thread_data in.
		jsonifier::string reason{};///< Reason for starting the thread_data.
	};

	/// @brief For starting a thread_data, not based on a message.
	struct start_thread_without_message_data {
		thread_auto_archive_duration autoArchiveDuration{ thread_auto_archive_duration::Shortest };///< The duration before it is auto-archived, in minutes.
		thread_type type{ thread_type::Guild_Public_Thread };///< Type of thread_data to create.
		int32_t rateLimitPerUser{};///< Integer amount of seconds a user has to wait before sending another message(0 - 21600).
		jsonifier::string threadName{};///< The name of the new thread_data.
		snowflake channelId{};///< The channel_data to start the thread_data in.
		jsonifier::string reason{};///< Reason for starting the thread_data.
		bool invitable{};///< Whether non-moderators can add other non - moderators to a thread; only available when creating a protected thread.
	};

	/// @brief For starting a thread_data, in a forum channel.
	struct start_thread_in_forum_channel_data {
		thread_auto_archive_duration autoArchiveDuration{ thread_auto_archive_duration::Longest };/// duration in minutes to automatically archive.
		forum_thread_message_data message{};///< A forum thread message params object contents of the first message in the forum thread.
		int32_t rateLimitPerUser{};///< Integer amount of seconds a user has to wait before sending another message(0 - 21600).
		snowflake channelId{};///< The id of the channel.
		jsonifier::string reason{};///< Reason for starting the thread_data.
		jsonifier::string name{};///< 1-100 character channel name auto_archive_duration.
	};

	/// @brief For joining a thread_data.
	struct join_thread_data {
		snowflake channelId{};///< The id of the thread_data to join.
	};

	/// @brief For adding a chosen user_data to a chosen thread_data.
	struct add_thread_member_data {
		snowflake channelId{};///< The id of the thread_data to join.
		snowflake userId{};///< The id of the user_data to add to the thread_data.
	};

	/// @brief For leaving a thread_data.
	struct leave_thread_data {
		snowflake channelId{};///< The id of the thread_data to leave.
	};

	/// @brief For removing a chosen user_data from a thread_data.
	struct remove_thread_member_data {
		snowflake channelId{};///< The id of the thread_data to remove them from.
		snowflake userId{};///< The id of the user_data to remove from the thread_data.
	};

	/// @brief For collecting a thread_member responseData structure for a given thread_member.
	struct get_thread_member_data {
		snowflake channelId{};///< The id of the thread_data to collect them from.
		snowflake userId{};///< The id of the user_data to collect from the thread_data.
	};

	/// @brief For collecting the list of thread_members from a thread_data.
	struct get_thread_members_data {
		snowflake channelId{};///< The id of the thread_data to collect them from.
	};

	/// @brief For collecting the list of active threads.
	struct get_active_threads_data {
		snowflake channelId{};///< The id of the channel_data to collect the threads from.
	};

	/// @brief For collecting puiblic archived threads from a given channel.
	struct get_public_archived_threads_data {
		snowflake channelId{};///< The channel_data to acquire the threads from.
		jsonifier::string before{};///< Returns threads before this timeStamp.
		int32_t limit{};///< Maximum number of threads to return.
	};

	/// @brief For collecting protected archived threads from a given channel.
	struct get_private_archived_threads_data {
		snowflake channelId{};///< The channel_data to acquire the threads from.
		jsonifier::string before{};///< Returns threads before this timeStamp.
		int32_t limit{};///< Maximum number of threads to return.
	};

	/// @brief For collecting joined protected archived threads from a given channel.
	struct get_joined_private_archived_threads_data {
		snowflake channelId{};///< The channel_data to acquire the threads from.
		jsonifier::string before{};///< Returns threads before this timeStamp.
		int32_t limit{};///< Maximum number of threads to return.
	};

	/// @brief For listing the active threads in a chosen guild.
	struct get_active_guild_threads_data {
		snowflake guildId{};///< The guild from which to list the threads from.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the thread_data related endpoints.
	class DiscordCoreAPI_Dll threads {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Starts a thread_data, based on a starting message_data.
		/// @param dataPackage a start_thread_with_message_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<thread_data> startThreadWithMessageAsync(start_thread_with_message_data dataPackage);

		/// @brief Starts a thread_data, not based on a starting message_data.
		/// @param dataPackage a start_thread_without_message_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<thread_data> startThreadWithoutMessageAsync(start_thread_without_message_data dataPackage);

		/// @brief Starts a thread_data, in a forum channel.
		/// @param dataPackage a start_thread_in_forum_channel_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<thread_data> startThreadInForumChannelAsync(start_thread_in_forum_channel_data dataPackage);

		/// @brief Joins a thread_data.
		/// @param dataPackage a join_thread_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> joinThreadAsync(join_thread_data dataPackage);

		/// @brief Adds a new user_data to a chosen thread_data.
		/// @param dataPackage an add_thread_member_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> addThreadMemberAsync(add_thread_member_data dataPackage);

		/// @brief Leaves a thread_data.
		/// @param dataPackage a leave_thread_data  structure.
		/// @return a co_routine containing void.
		static co_routine<void> leaveThreadAsync(leave_thread_data dataPackage);

		/// @brief Removes a user from a chosen thread_data.
		/// @param dataPackage a remove_thread_member_data  structure.
		/// @return a co_routine containing void.
		static co_routine<void> removeThreadMemberAsync(remove_thread_member_data dataPackage);

		/// @brief Collects a thread_member if they exist.
		/// @param dataPackage a get_thread_member_data structure.
		/// @return a co_routine containing a thread_member_data.
		static co_routine<thread_member_data> getThreadMemberAsync(get_thread_member_data dataPackage);

		/// @brief Collects a list of thread_members if they exist.
		/// @param dataPackage a get_thread_members_data structure.
		/// @return a co_routine containing a vector<thread_member_data>.
		static co_routine<jsonifier::vector<thread_member_data>> getThreadMembersAsync(get_thread_members_data dataPackage);

		/// @brief Collects a list of threads from a given channel.
		/// @param dataPackage a get_active_threads_data structure.
		/// @return a co_routine containing a active_threads_data.
		static co_routine<active_threads_data> getActiveThreadsAsync(get_active_threads_data dataPackage);

		/// @brief Collects a list of public archived threads from a given channel.
		/// @param dataPackage a get_public_archived_threads_data structure.
		/// @return a co_routine containing a archived_threads_data.
		static co_routine<archived_threads_data> getPublicArchivedThreadsAsync(get_public_archived_threads_data dataPackage);

		/// @brief Collects a list of protected archived threads from a given channel.
		/// @param dataPackage a get_private_archived_threads_data structure.
		/// @return a co_routine containing a archived_threads_data.
		static co_routine<archived_threads_data> getPrivateArchivedThreadsAsync(get_private_archived_threads_data dataPackage);

		/// @brief Collects a list of joined protected archived threads from a given channel.
		/// @param dataPackage a get_private_archived_threads_data structure.
		/// @return a co_routine containing a archived_threads_data.
		static co_routine<archived_threads_data> getJoinedPrivateArchivedThreadsAsync(get_joined_private_archived_threads_data dataPackage);

		/// @brief Lists all of the active threads of a chosen guild.
		/// @param dataPackage a list_active_threads_data structure.
		/// @return a co_routine containing a vector<channel_data>.
		static co_routine<active_threads_data> getActiveGuildThreadsAsync(get_active_guild_threads_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};
	/**@}*/
};
