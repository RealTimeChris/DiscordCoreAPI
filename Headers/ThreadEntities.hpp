/// ThreadEntities.hpp - Header file for the thread-related stuff.
/// Nov 29, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For starting a Thread, based on a Message. \brief For starting a Thread, based on a Message.
	struct DiscordCoreAPI_Dll StartThreadWithMessageData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::SHORTEST };///< The duration before it is auto-archived, in minutes.
		string threadName{ "" };///< The name of the new Thread.
		string messageId{ "" };///< The Message Id to base the Thread off of.
		string channelId{ "" };///< The Channel to start the Thread in.
		string reason{ "" };///< Reason for starting the Thread.
	};

	/// For starting a Thread, not based on a Message. \brief For starting a Thread, not based on a Message.
	struct DiscordCoreAPI_Dll StartThreadWithoutMessageData {
		ThreadAutoArchiveDuration autoArchiveDuration{ ThreadAutoArchiveDuration::SHORTEST };///< The duration before it is auto-archived, in minutes.
		ThreadType type{ ThreadType::GUILD_PUBLIC_THREAD };///< Type of Thread to create.
		bool invitable{ false };///< Whether non-moderators can add other non - moderators to a thread; only available when creating a private thread.
		string threadName{ "" };///< The name of the new Thread.
		string channelId{ "" };///< The Channel to start the Thread in.
		string reason{ "" };///< Reason for starting the Thread.
	};

	/// For joining a Thread. \brief For joining a Thread.
	struct DiscordCoreAPI_Dll JoinThreadData {
		string channelId{ "" };///< The id of the Thread to join.
	};

	/// Adds a chosen User to a chosen Thread. \brief Adds a chosen User to a chosen Thread.
	struct DiscordCoreAPI_Dll AddThreadMemberData {
		string channelId{ "" };///< The id of the Thread to join.
		string userId{ "" };///< The id of the User to add to the Thread.
	};

	/// For leaving a Thread. \brief For leaving a Thread.
	struct DiscordCoreAPI_Dll LeaveThreadData {
		string channelId{ "" };///< The id of the Thread to leave.
	};

	/// For removing a chosen User from a Thread. \brief For removing a chosen User from a Thread.
	struct DiscordCoreAPI_Dll RemoveThreadMemberData {
		string channelId{ "" };///< The id of the Thread to remove them from.
		string userId{ "" };///< The id of the User to remove from the Thread.
	};

	/// For collecting a ThreadMember data structure for a given ThreadMember. \brief For collecting a ThreadMember data structure for a given ThreadMember.
	struct DiscordCoreAPI_Dll GetThreadMemberData {
		string channelId{ "" };///< The id of the Thread to collect them from.
		string userId{ "" };///< The id of the User to collect from the Thread.
	};

	/// For collecting the list of ThreadMembers from a Thread. \brief For collecting the list of ThreadMembers from a Thread.
	struct DiscordCoreAPI_Dll GetThreadMembersData {
		string channelId{ "" };///< The id of the Thread to collect them from.
	};

	/// For collecting the list of active Threads. \brief For collecting the list of active Threads.
	struct DiscordCoreAPI_Dll GetActiveThreadsData {
		string channelId{ "" };///< The id of the Channel to collect the Threads from.
	};

	/// For collecting puiblic archived Threads from a given Channel. \brief For collecting puiblic archived Threads from a given Channel.
	struct DiscordCoreAPI_Dll GetPublicArchivedThreadsData {
		string channelId{ "" };///< The Channel to acquire the Threads from.
		string before{ "" };///< Returns threads before this timestamp.
		int32_t limit{ 0 };///< Maximum number of threads to return.
	};

	/// For collecting private archived Threads from a given Channel. \brief For collecting private archived Threads from a given Channel.
	struct DiscordCoreAPI_Dll GetPrivateArchivedThreadsData {
		string channelId{ "" };///< The Channel to acquire the Threads from.
		string before{ "" };///< Returns threads before this timestamp.
		int32_t limit{ 0 };///< Maximum number of threads to return.
	};

	/// For collecting joined private archived Threads from a given Channel. \brief For collecting joined private archived Threads from a given Channel.
	struct DiscordCoreAPI_Dll GetJoinedPrivateArchivedThreadsData {
		string channelId{ "" };///< The Channel to acquire the Threads from.
		string before{ "" };///< Returns threads before this timestamp.
		int32_t limit{ 0 };///< Maximum number of threads to return.
	};

	/// For listing the active Threads in a chosen Guild. \brief For listing the active Threads in a chosen Guild.
	struct DiscordCoreAPI_Dll GetActiveGuildThreadsData {
		string guildId{ "" };///< The Guild from which to list the Threads from.
	};

	/// Represents a single Thread. \brief Represents a single Thread.
	class Thread :public ChannelData {
	public:
		friend struct OnThreadCreationData;
		friend struct OnThreadDeletionData;
		friend struct OnThreadUpdateData;
		friend class Threads;

		Thread();

		virtual ~Thread();

	protected:

		Thread(ChannelData dataNew);
	};

	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Thread related endpoints. \brief An interface class for the Thread related endpoints.
	class DiscordCoreAPI_Dll Threads {
	public:

		/// Starts a Thread, based on a starting Message. \brief Starts a Thread, based on a starting Message.
		/// \param dataPackage A StartThreadWithMessageData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Thread> startThreadWithMessageAsync(StartThreadWithMessageData dataPackage);

		/// Starts a Thread, not based on a starting Message. \brief Starts a Thread, not based on a starting Message.
		/// \param dataPackage A StartThreadWithoutMessageData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Thread> startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage);

		/// Joins a Thread. \brief Joins a Thread.
		/// \param dataPackage A JoinThreadData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> joinThreadAsync(JoinThreadData dataPackage);

		/// Adds a new User to a chosen Thread. \brief Adds a new User to a chosen Thread.
		/// \param dataPackage A AddThreadMemberData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addThreadMemberAsync(AddThreadMemberData dataPackage);

		/// Leaves a Thread. \brief Leaves a Thread.
		/// \param dataPackage A LeaveThreadData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> leaveThreadAsync(LeaveThreadData dataPackage);

		/// Removes a User from a chosen Thread. \brief Removes a User from a chosen Thread.
		/// \param dataPackage A RemoveThreadMemberData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeThreadMemberAsync(RemoveThreadMemberData dataPackage);

		/// Collects a ThreadMember if they exist. \brief Collects a ThreadMember if they exist.
		/// \param dataPackage A GetThreadMemberData structure.
		/// \returns A CoRoutine containing ThreadMemberData.
		static CoRoutine<ThreadMemberData> getThreadMemberAsync(GetThreadMemberData dataPackage);

		/// Collects a list of ThreadMembers if they exist. \brief Collects a list of ThreadMembers if they exist.
		/// \param dataPackage A GetThreadMembersData structure.
		/// \returns A CoRoutine containing a vector<ThreadMemberData>.
		static CoRoutine<vector<ThreadMemberData>> getThreadMembersAsync(GetThreadMembersData dataPackage);

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
		/// \returns A CoRoutine containing a vector of Channels.
		static CoRoutine<ActiveThreadsData> getActiveGuildThreadsAsync(GetActiveGuildThreadsData dataPackage);
	};
	/**@}*/
};