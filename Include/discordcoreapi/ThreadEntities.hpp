/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// ThreadEntities.hpp - Header file for the thread-related stuff.
/// Nov 29, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file ThreadEntities.hpp

#pragma once

#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For starting a Thread, based on a Message. \brief For starting a Thread, based on a Message.
	struct DiscordCoreAPI_Dll StartThreadWithMessageData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::Shortest };///< The duration before it is auto-archived, in minutes.
		std::string threadName{ "" };///< The name of the new Thread.
		std::string messageId{ "" };///< The Message Id to base the Thread off of.
		std::string channelId{ "" };///< The Channel to start the Thread in.
		std::string reason{ "" };///< Reason for starting the Thread.
	};

	/// For starting a Thread, not based on a Message. \brief For starting a Thread, not based on a Message.
	struct DiscordCoreAPI_Dll StartThreadWithoutMessageData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::Shortest };///< The duration before it is auto-archived, in minutes.
		ThreadType type{ ThreadType::Guild_Public_Thread };///< Type of Thread to create.
		std::string threadName{ "" };///< The name of the new Thread.
		std::string channelId{ "" };///< The Channel to start the Thread in.
		std::string reason{ "" };///< Reason for starting the Thread.
		bool invitable{ false };///< Whether non-moderators can add other non - moderators to a thread; only available when creating a private thread.
	};

	/// For starting a Thread, in a forum channel. \brief For starting a Thread, in a forum channel.
	struct DiscordCoreAPI_Dll StartThreadInForumChannelData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::Longest };/// Duration in minutes to automatically archive the thread.
		ForumThreadMessageData message{};///< A forum thread message params object contents of the first message in the forum thread.
		std::string channelId{ "" };///< The id of the channel.
		int32_t rateLimitPerUser{};///< Integer amount of seconds a user has to wait before sending another message(0 - 21600).
		std::string reason{ "" };///< Reason for starting the Thread.
		std::string name{ "" };///< 1-100 character channel name auto_archive_duration.
	};

	/// For joining a Thread. \brief For joining a Thread.
	struct DiscordCoreAPI_Dll JoinThreadData {
		std::string channelId{ "" };///< The id of the Thread to join.
	};

	/// For adding a chosen User to a chosen Thread. \brief For adding a chosen User to a chosen Thread.
	struct DiscordCoreAPI_Dll AddThreadMemberData {
		std::string channelId{ "" };///< The id of the Thread to join.
		std::string userId{ "" };///< The id of the User to add to the Thread.
	};

	/// For leaving a Thread. \brief For leaving a Thread.
	struct DiscordCoreAPI_Dll LeaveThreadData {
		std::string channelId{ "" };///< The id of the Thread to leave.
	};

	/// For removing a chosen User from a Thread. \brief For removing a chosen User from a Thread.
	struct DiscordCoreAPI_Dll RemoveThreadMemberData {
		std::string channelId{ "" };///< The id of the Thread to remove them from.
		std::string userId{ "" };///< The id of the User to remove from the Thread.
	};

	/// For collecting a ThreadMember responseData structure for a given ThreadMember. \brief For collecting a ThreadMember responseData structure for a given ThreadMember.
	struct DiscordCoreAPI_Dll GetThreadMemberData {
		std::string channelId{ "" };///< The id of the Thread to collect them from.
		std::string userId{ "" };///< The id of the User to collect from the Thread.
	};

	/// For collecting the list of ThreadMembers from a Thread. \brief For collecting the list of ThreadMembers from a Thread.
	struct DiscordCoreAPI_Dll GetThreadMembersData {
		std::string channelId{ "" };///< The id of the Thread to collect them from.
	};

	/// For collecting the list of active Threads. \brief For collecting the list of active Threads.
	struct DiscordCoreAPI_Dll GetActiveThreadsData {
		std::string channelId{ "" };///< The id of the Channel to collect the Threads from.
	};

	/// For collecting puiblic archived Threads from a given Channel. \brief For collecting puiblic archived Threads from a given Channel.
	struct DiscordCoreAPI_Dll GetPublicArchivedThreadsData {
		std::string channelId{ "" };///< The Channel to acquire the Threads from.
		std::string before{ "" };///< Returns threads before this timestamp.
		int32_t limit{ 0 };///< Maximum number of threads to return.
	};

	/// For collecting private archived Threads from a given Channel. \brief For collecting private archived Threads from a given Channel.
	struct DiscordCoreAPI_Dll GetPrivateArchivedThreadsData {
		std::string channelId{ "" };///< The Channel to acquire the Threads from.
		std::string before{ "" };///< Returns threads before this timestamp.
		int32_t limit{ 0 };///< Maximum number of threads to return.
	};

	/// For collecting joined private archived Threads from a given Channel. \brief For collecting joined private archived Threads from a given Channel.
	struct DiscordCoreAPI_Dll GetJoinedPrivateArchivedThreadsData {
		std::string channelId{ "" };///< The Channel to acquire the Threads from.
		std::string before{ "" };///< Returns threads before this timestamp.
		int32_t limit{ 0 };///< Maximum number of threads to return.
	};

	/// For listing the active Threads in a chosen Guild. \brief For listing the active Threads in a chosen Guild.
	struct DiscordCoreAPI_Dll GetActiveGuildThreadsData {
		std::string guildId{ "" };///< The Guild from which to list the Threads from.
	};

	/// Represents a single Thread. \brief Represents a single Thread.
	class DiscordCoreAPI_Dll Thread : public ChannelData {
	  public:
		Thread() = default;

		virtual ~Thread() = default;
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the Thread related endpoints. \brief An interface class for the Thread related endpoints.
	class DiscordCoreAPI_Dll Threads {
	  public:
		static void initialize(DiscordCoreInternal::HttpClient*);

		/// Starts a Thread, based on a starting Message. \brief Starts a Thread, based on a starting Message.
		/// \param dataPackage A StartThreadWithMessageData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Thread> startThreadWithMessageAsync(StartThreadWithMessageData dataPackage);

		/// Starts a Thread, not based on a starting Message. \brief Starts a Thread, not based on a starting Message.
		/// \param dataPackage A StartThreadWithoutMessageData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Thread> startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage);

		/// Starts a Thread, in a forum channel. \brief Starts a Thread, in a forum channel.
		/// \param dataPackage A StartThreadInForumChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Thread> startThreadInForumChannelAsync(StartThreadInForumChannelData dataPackage);

		/// Joins a Thread. \brief Joins a Thread.
		/// \param dataPackage A JoinThreadData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> joinThreadAsync(JoinThreadData dataPackage);

		/// Adds a new User to a chosen Thread. \brief Adds a new User to a chosen Thread.
		/// \param dataPackage An AddThreadMemberData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addThreadMemberAsync(AddThreadMemberData dataPackage);

		/// Leaves a Thread. \brief Leaves a Thread.
		/// \param dataPackage A LeaveThreadData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> leaveThreadAsync(LeaveThreadData dataPackage);

		/// Removes a User from a chosen Thread. \brief Removes a User from a chosen Thread.
		/// \param dataPackage A RemoveThreadMemberData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeThreadMemberAsync(RemoveThreadMemberData dataPackage);

		/// Collects a ThreadMember if they exist. \brief Collects a ThreadMember if they exist.
		/// \param dataPackage A GetThreadMemberData structure.
		/// \returns A CoRoutine containing a ThreadMemberData.
		static CoRoutine<ThreadMemberData> getThreadMemberAsync(GetThreadMemberData dataPackage);

		/// Collects a list of ThreadMembers if they exist. \brief Collects a list of ThreadMembers if they exist.
		/// \param dataPackage A GetThreadMembersData structure.
		/// returns A CoRoutine containing a std::vector<ThreadMemberData>.
		static CoRoutine<std::vector<ThreadMemberData>> getThreadMembersAsync(GetThreadMembersData dataPackage);

		/// Collects a list of Threads from a given Channel. \brief Collects a list of Threads from a given Channel.
		/// \param dataPackage A GetActiveThreadsData structure.
		/// \returns A CoRoutine containing a ActiveThreadsData.
		static CoRoutine<ActiveThreadsData> getActiveThreadsAsync(GetActiveThreadsData dataPackage);

		/// Collects a list of public archived Threads from a given Channel. \brief Collects a list of public archived Threads from a given Channel.
		/// \param dataPackage A GetPublicArchivedThreadsData structure.
		/// \returns A CoRoutine containing a ArchivedThreadsData.
		static CoRoutine<ArchivedThreadsData> getPublicArchivedThreadsAsync(GetPublicArchivedThreadsData dataPackage);

		/// Collects a list of private archived Threads from a given Channel. \brief Collects a list of private archived Threads from a given Channel.
		/// \param dataPackage A GetPrivateArchivedThreadsData structure.
		/// \returns A CoRoutine containing a ArchivedThreadsData.
		static CoRoutine<ArchivedThreadsData> getPrivateArchivedThreadsAsync(GetPrivateArchivedThreadsData dataPackage);

		/// Collects a list of joined private archived Threads from a given Channel. \brief Collects a list of joined private archived Threads from a given Channel.
		/// \param dataPackage A GetPrivateArchivedThreadsData structure.
		/// \returns A CoRoutine containing a ArchivedThreadsData.
		static CoRoutine<ArchivedThreadsData> getJoinedPrivateArchivedThreadsAsync(GetJoinedPrivateArchivedThreadsData dataPackage);

		/// Lists all of the active Threads of a chosen Guild. \brief Lists all of the active Threads of a chosen Guild.
		/// \param dataPackage A ListActiveThreadsData structure.
		/// \returns A CoRoutine containing a std::vector<Channel>.
		static CoRoutine<ActiveThreadsData> getActiveGuildThreadsAsync(GetActiveGuildThreadsData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpClient* httpClient;
	};
	/**@}*/
};// namespace DiscordCoreAPI
