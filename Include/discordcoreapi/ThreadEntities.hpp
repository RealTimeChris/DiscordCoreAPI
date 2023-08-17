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

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For starting a ThreadData, based on a Message.
	struct StartThreadWithMessageData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::Shortest };///< The duration before it is auto-archived, in minutes.
		int32_t rateLimitPerUser{};///< Integer amount of seconds a user has to wait before sending another message(0 - 21600).
		std::string threadName{};///< The name of the new ThreadData.
		Snowflake messageId{};///< The Message Snowflake to base the ThreadData off of.
		Snowflake channelId{};///< The ChannelData to start the ThreadData in.
		std::string reason{};///< Reason for starting the ThreadData.
	};

	/// @brief For starting a ThreadData, not based on a Message.
	struct StartThreadWithoutMessageData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::Shortest };///< The duration before it is auto-archived, in minutes.
		ThreadType type{ ThreadType::Guild_Public_Thread };///< Type of ThreadData to create.
		int32_t rateLimitPerUser{};///< Integer amount of seconds a user has to wait before sending another message(0 - 21600).
		std::string threadName{};///< The name of the new ThreadData.
		Snowflake channelId{};///< The ChannelData to start the ThreadData in.
		std::string reason{};///< Reason for starting the ThreadData.
		bool invitable{};///< Whether non-moderators can add other non - moderators to a thread; only available when creating a protected thread.
	};

	/// @brief For starting a ThreadData, in a forum channel.
	struct StartThreadInForumChannelData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::Longest };/// Duration in minutes to automatically archive.
		ForumThreadMessageData message{};///< A forum thread message params object contents of the first message in the forum thread.
		int32_t rateLimitPerUser{};///< Integer amount of seconds a user has to wait before sending another message(0 - 21600).
		Snowflake channelId{};///< The id of the channel.
		std::string reason{};///< Reason for starting the ThreadData.
		std::string name{};///< 1-100 character channel name auto_archive_duration.
	};

	/// @brief For joining a ThreadData.
	struct JoinThreadData {
		Snowflake channelId{};///< The id of the ThreadData to join.
	};

	/// @brief For adding a chosen UserData to a chosen ThreadData.
	struct AddThreadMemberData {
		Snowflake channelId{};///< The id of the ThreadData to join.
		Snowflake userId{};///< The id of the UserData to add to the ThreadData.
	};

	/// @brief For leaving a ThreadData.
	struct LeaveThreadData {
		Snowflake channelId{};///< The id of the ThreadData to leave.
	};

	/// @brief For removing a chosen UserData from a ThreadData.
	struct RemoveThreadMemberData {
		Snowflake channelId{};///< The id of the ThreadData to remove them from.
		Snowflake userId{};///< The id of the UserData to remove from the ThreadData.
	};

	/// @brief For collecting a ThreadMember responseData structure for a given ThreadMember.
	struct GetThreadMemberData {
		Snowflake channelId{};///< The id of the ThreadData to collect them from.
		Snowflake userId{};///< The id of the UserData to collect from the ThreadData.
	};

	/// @brief For collecting the list of ThreadMembers from a ThreadData.
	struct GetThreadMembersData {
		Snowflake channelId{};///< The id of the ThreadData to collect them from.
	};

	/// @brief For collecting the list of active Threads.
	struct GetActiveThreadsData {
		Snowflake channelId{};///< The id of the ChannelData to collect the Threads from.
	};

	/// @brief For collecting puiblic archived Threads from a given Channel.
	struct GetPublicArchivedThreadsData {
		Snowflake channelId{};///< The ChannelData to acquire the Threads from.
		std::string before{};///< Returns threads before this timeStamp.
		int32_t limit{};///< Maximum number of threads to return.
	};

	/// @brief For collecting protected archived Threads from a given Channel.
	struct GetPrivateArchivedThreadsData {
		Snowflake channelId{};///< The ChannelData to acquire the Threads from.
		std::string before{};///< Returns threads before this timeStamp.
		int32_t limit{};///< Maximum number of threads to return.
	};

	/// @brief For collecting joined protected archived Threads from a given Channel.
	struct GetJoinedPrivateArchivedThreadsData {
		Snowflake channelId{};///< The ChannelData to acquire the Threads from.
		std::string before{};///< Returns threads before this timeStamp.
		int32_t limit{};///< Maximum number of threads to return.
	};

	/// @brief For listing the active Threads in a chosen Guild.
	struct GetActiveGuildThreadsData {
		Snowflake guildId{};///< The Guild from which to list the Threads from.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the ThreadData related endpoints.
	class DiscordCoreAPI_Dll Threads {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Starts a ThreadData, based on a starting MessageData.
		/// @param dataPackage A StartThreadWithMessageData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ThreadData> startThreadWithMessageAsync(StartThreadWithMessageData dataPackage);

		/// @brief Starts a ThreadData, not based on a starting MessageData.
		/// @param dataPackage A StartThreadWithoutMessageData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ThreadData> startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage);

		/// @brief Starts a ThreadData, in a forum channel.
		/// @param dataPackage A StartThreadInForumChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ThreadData> startThreadInForumChannelAsync(StartThreadInForumChannelData dataPackage);

		/// @brief Joins a ThreadData.
		/// @param dataPackage A JoinThreadData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> joinThreadAsync(JoinThreadData dataPackage);

		/// @brief Adds a new UserData to a chosen ThreadData.
		/// @param dataPackage An AddThreadMemberData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> addThreadMemberAsync(AddThreadMemberData dataPackage);

		/// @brief Leaves a ThreadData.
		/// @param dataPackage A LeaveThreadData  structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> leaveThreadAsync(LeaveThreadData dataPackage);

		/// @brief Removes a User from a chosen ThreadData.
		/// @param dataPackage A RemoveThreadMemberData  structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> removeThreadMemberAsync(RemoveThreadMemberData dataPackage);

		/// @brief Collects a ThreadMember if they exist.
		/// @param dataPackage A GetThreadMemberData structure.
		/// @return A CoRoutine containing a ThreadMemberData.
		static CoRoutine<ThreadMemberData> getThreadMemberAsync(GetThreadMemberData dataPackage);

		/// @brief Collects a list of ThreadMembers if they exist.
		/// @param dataPackage A GetThreadMembersData structure.
		/// @return A CoRoutine containing a vector<ThreadMemberData>.
		static CoRoutine<Jsonifier::Vector<ThreadMemberData>> getThreadMembersAsync(GetThreadMembersData dataPackage);

		/// @brief Collects a list of Threads from a given Channel.
		/// @param dataPackage A GetActiveThreadsData structure.
		/// @return A CoRoutine containing a ActiveThreadsData.
		static CoRoutine<ActiveThreadsData> getActiveThreadsAsync(GetActiveThreadsData dataPackage);

		/// @brief Collects a list of public archived Threads from a given Channel.
		/// @param dataPackage A GetPublicArchivedThreadsData structure.
		/// @return A CoRoutine containing a ArchivedThreadsData.
		static CoRoutine<ArchivedThreadsData> getPublicArchivedThreadsAsync(GetPublicArchivedThreadsData dataPackage);

		/// @brief Collects a list of protected archived Threads from a given Channel.
		/// @param dataPackage A GetPrivateArchivedThreadsData structure.
		/// @return A CoRoutine containing a ArchivedThreadsData.
		static CoRoutine<ArchivedThreadsData> getPrivateArchivedThreadsAsync(GetPrivateArchivedThreadsData dataPackage);

		/// @brief Collects a list of joined protected archived Threads from a given Channel.
		/// @param dataPackage A GetPrivateArchivedThreadsData structure.
		/// @return A CoRoutine containing a ArchivedThreadsData.
		static CoRoutine<ArchivedThreadsData> getJoinedPrivateArchivedThreadsAsync(GetJoinedPrivateArchivedThreadsData dataPackage);

		/// @brief Lists all of the active Threads of a chosen Guild.
		/// @param dataPackage A ListActiveThreadsData structure.
		/// @return A CoRoutine containing a vector<ChannelData>.
		static CoRoutine<ActiveThreadsData> getActiveGuildThreadsAsync(GetActiveGuildThreadsData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
};
