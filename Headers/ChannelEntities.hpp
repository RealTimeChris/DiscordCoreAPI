// ChannelEntities.hpp - Header for the classes related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// For editing the Permission overwrites of a given Channel, for a given role or User. \brief For editing the Permission overwrites of a given Channel, for a given role or User.
	struct DiscordCoreAPI_Dll EditChannelPermissionOverwritesData {
		PermissionOverwritesType type{}; ///< The type of overwrite.
		string roleOrUserId{ "" };	///< The role or User Id to modify the permissions for.
		string channelId{ "" };	///< The Channel id of the desired Channel to modify the permissions for.
		string allow{ "" };	///< The permissions to list as "allowed".
		string deny{ "" };	///< The permissions to list as "deny".
		string reason{ "" };///< Reason for editing the permission overwrites.
	};

	/// For creating an invite to a given Channel. \brief For creating an invite to a given Channel.
	struct DiscordCoreAPI_Dll CreateChannelInviteData {
		string targetApplicationId{ "" };///< The id of the embedded application to open for this invite, required if target_type is 2, the application must have the EMBEDDED flag.
		InviteTargetTypes targetType{};///<	The type of target for this voice channel invite.
		string targetUserId{ "" };///< Id of the user whose stream to display for this invite, required if target_type is 1, the user must be streaming in the channel.
		bool temporary{ false };///< Whether this invite only grants temporary membership	false.
		string channelId{ "" };///< The id of the Channel to create the invite for.
		bool unique{ false };///< If true, don't try to reuse a similar invite (useful for creating many unique one time use invites).
		int32_t maxUses{ 0 };///< Max number of uses or 0 for unlimited.between 0 and 100.
		int32_t maxAge{ 0 };///< Duration of invite in seconds before expiry, or 0 for never.between 0 and 604800 (7 days)	86400 (24 hours).
		string reason{ "" };///< Reason for creating the invite.
	};

	/// For collecting a Channel from Discord's servers. \brief For collecting a Channel from Discord's servers.
	struct DiscordCoreAPI_Dll GetChannelData {
		string channelId{ "" }; ///< The id of the Channel to acquire.
	};

	/// For deleting or closing a Channel. \brief For deleting or closing a Channel.
	struct DiscordCoreAPI_Dll DeleteOrCloseChannelData {
		string channelId{ "" };
		string reason{ "" };
	};

	/// For modifying a channel's properties. \brief For modifying a channel's properties.
	struct DiscordCoreAPI_Dll ModifyChannelData {
		ModifyChannelData(ChannelData newData) {
			this->channelData.permissionOverwrites = newData.permissionOverwrites;
			this->channelData.rateLimitPerUser = newData.rateLimitPerUser;
			this->channelData.userLimit = newData.userLimit;
			this->channelData.rtcRgion = newData.rtcRegion;
			this->channelData.parentId = newData.parentId;
			this->channelData.position = newData.position;
			this->channelData.topic = newData.topic;
			this->channelData.name = newData.name;
			this->channelData.nsfw = newData.nsfw;
			this->channelData.type = newData.type;
		};
		UpdateChannelData channelData{};///< The data of the channel to be updated.
		string channelId{ "" };///< The id of the Channel to modify.
		string reason{ "" };///< A reason for modifying the Channel.
	};

	/// For deleting the Permission overwrites of a given Channel for a given role or User. \brief For deleting the Permission overwrites of a given Channel for a given role or User.
	struct DiscordCoreAPI_Dll DeleteChannelPermissionOverwritesData {
		string roleOrUserId{ "" }; ///< The role or User id for whom to delete the Permission overwrites.
		string channelId{ "" }; ///< The id of the Channel to modify permissions for.
		string reason{ "" };///< Reason for deleting these permission overwrites.
	};

	/// For collecting a direct-messaging Channel. \brief For collecting a direct-messaging Channel.
	struct DiscordCoreAPI_Dll GetDMChannelData {
		string userId{ "" }; ///< The User for whom to collect the direct-messaging Channel to.
	};

	/// For collecting the invites to a given channel. \brief For collecting the invites to a given channel.
	struct DiscordCoreAPI_Dll GetChannelInvitesData {
		string channelId{ "" };
	};

	/// For following a news Channel. \brief For following a news Channel.
	struct DiscordCoreAPI_Dll FollowNewsChannelData {
		string targetChannelId{ "" };
		string channelId{ "" };
	};

	/// For triggering the typing indicator in a given Channel. \brief For triggering the typing indicator in a given Channel.
	struct DiscordCoreAPI_Dll TriggerTypingIndicatorData {
		string channelId{ "" };
	};

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

	/// For acquiring a list of Channels from a chosen Guild. /brief For acquiring a list of Channels from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildChannelsData {
		string guildId{ "" };///< The Guild from which to collect the Channels from.
	};

	/// For creating a new Channel within a chosen Guild. \brief For creating a new Channel within a chosen Guild.
	struct DiscordCoreAPI_Dll CreateGuildChannelData {
		vector<OverWriteData> permissionOverwrites{};///< Array of overwrite objects	the channel's permission overwrites.
		int32_t rateLimitPerUser{ 0 };///< Amount of seconds a user has to wait before sending another message(0 - 21600); bots, as well as users with the permission manage_messages or manage_channel, are unaffected.
		int32_t userLimit{ 0 };///< The user limit of the voice channel(voice only).
		string parentId{ "" };///< Id of the parent category for a channel.
		int32_t position{ 0 };///< Sorting position of the channel.
		int32_t bitrate{ 0 };///< The bitrate(in bits) of the voice channel(voice only).
		string guildId{ "" };///< The Guild within which to create the Channel.
		string reason{ "" };///< Reason for creating the channel.
		ChannelType type{};///< The type of channel.
		string topic{ "" };///< Channel topic(0 - 1024 characters).
		bool nsfw{ false };///<  Whether the channel is nsfw.
		string name{ "" };///< The name of the Channel.
	};

	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionData {
		bool lockPermissions{ false };///< Syncs the permission overwrites with the new parent, if moving to a new category.
		string parentId{ "" };///< The new parent ID for the channel that is moved.
		int32_t position{ 0 };///< Sorting position of the channel.
		string id{ "" };///< Channel id.
	};

	/// For modifying the current positions of one or more Channels in the Guild. \brief For modifying the current positions of one or more Channels in the Guild.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionsData {
		vector<ModifyGuildChannelPositionData> modifyChannelData{};///< Array of new Channel position's data.
		string guildId{ "" };///< Guild within which to re-order the Channel positions.
		string reason{ "" };///< Reason for re-ordering the Channel positions.
	};

	/// For listing the active Threads in a chosen Guild. \brief For listing the active Threads in a chosen Guild.
	struct DiscordCoreAPI_Dll ListActiveThreadsData {
		string guildId{ "" };///< The Guild from which to list the Threads from.
	};

	/// A Channel object. \brief A Channel object.
	class DiscordCoreAPI_Dll Channel : public ChannelData {
	public:

		friend class DiscordCoreInternal::HttpRequestAgent;
		template<typename returnValueType>
		friend class CoRoutine;
		friend class DiscordCoreInternal::DataParser;
		friend struct OnChannelDeletionData;
		friend struct OnChannelCreationData;
		friend struct OnThreadCreationData;
		friend struct OnThreadDeletionData;
		friend struct OnChannelUpdateData;
		friend struct OnThreadUpdateData;
		friend class DiscordCoreClient;
		friend class Channels;
		friend class Guild;

		Channel();

		virtual ~Channel() {};

	protected:

		Channel(ChannelData dataNew);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class DiscordCoreAPI_Dll for the Channel related endpoints. \brief An interface class DiscordCoreAPI_Dll for the Channel-related endpoints.
	class DiscordCoreAPI_Dll Channels {
	public:

		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Guild;		

		/// Collects a Channel from the library's cache. \brief Collects a Channel from the library's cache.		
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> getCachedChannelAsync(GetChannelData dataPackage);

		/// Collects a Channel from the Discord servers. \brief Collects a Channel from the Discord servers.
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> getChannelAsync(GetChannelData dataPackage);

		/// Modifies a Channel's properties. \brief Modifies a Channel's properties.
		/// \param dataPackage A ModifyChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> modifyChannelAsync(ModifyChannelData dataPackage);

		/// Delete a channel, or close a private message. \brief Delete a channel, or close a private message.
		/// \param dataPackage A DeleteChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<void> deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage);

		/// Edit the given Permissions overwrites for a given User or Role. \brief Edit the given Permissions overwrites for a given User or Role.
		/// \param dataPackage A EditChannelPermissionOverwritesData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage);

		/// Delete the given Permissions overwrites for a given User or Role. \brief Delete the given Permissions overwrites for a given User or Role.
		/// \param dataPackage A DeleteChannelPermissionOverwritesData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage);

		/// Collect a direct-Message Channel between the bot and the User. \brief Collect a direct-Message Channel between the bot and the User.
		/// \param dataPackage A GetDMChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> getDMChannelAsync(GetDMChannelData dataPackage);

		/// Collects a vector of the invites to a given Channel. \brief Collects a vector of the invites to a given Channel.
		/// \param dataPackage A GetChannelInvitesData structure.
		/// \returns A CoRoutine containing a vector of InviteData.
		static CoRoutine<vector<InviteData>> getChannelInvitesAsync(GetChannelInvitesData dataPackage);

		/// Creates an invite to a selected Channel. \brief Creates an invite to a selected Channel.
		/// \param dataPackage A CreateChannelInviteData structure.
		/// \returns A CoRoutine containing an InviteData structure.
		static CoRoutine<InviteData> createChannelInviteAsync(CreateChannelInviteData dataPackage);

		/// Follows a given new Channel with another Channel. \brief Follows a given new Channel with another Channel.
		/// \param dataPackage A FollowNewsChannelData structure.
		/// \returns A CoRoutine containing a Channel structure.
		static CoRoutine<Channel> followNewsChannelAsync(FollowNewsChannelData dataPackage);

		/// Triggers the typing indicator for the bot in the given Channel. \brief Triggers the typing indicator for the bot in the given Channel.
		/// \param dataPackage A TriggerTypingIndicatorData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage);

		/// Starts a Thread, based on a starting Message. \brief Starts a Thread, based on a starting Message.
		/// \param dataPackage A StartThreadWithMessageData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> startThreadWithMessageAsync(StartThreadWithMessageData dataPackage);

		/// Starts a Thread, not based on a starting Message. \brief Starts a Thread, not based on a starting Message.
		/// \param dataPackage A StartThreadWithoutMessageData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage);

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

		/// Collects a list of Channels from a chosen Guild. \brief Collects a list of Channels from a chosen Guild.
		/// \param dataPackage A GetGuildChannelsData structure.
		/// \returns A CoRoutine containing a vector of Channels.
		static CoRoutine<vector<Channel>> getGuildChannelsAsync(GetGuildChannelsData dataPackage);

		/// Creates a new Channel within a chosen Guild. \brief Creates a new Channel within a chosen Guild.
		/// \param dataPackage A GetGuildChannelsData structure.
		/// \returns A CoRoutine containing a vector of Channels.
		static CoRoutine<Channel> createGuildChannelAsync(CreateGuildChannelData dataPackage);

		/// Re-orders the Channel positions, within a chosen Guild. \brief Re-orders the Channel positions, within a chosen Guild.
		/// \param dataPackage A ModifyGuildChannelPositionsData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage);

		/// Lists all of the active Threads of a chosen Guild. \brief Lists all of the active Threads of a chosen Guild.
		/// \param dataPackage A ListActiveThreadsData structure.
		/// \returns A CoRoutine containing a vector of Channels.
		static CoRoutine<ActiveThreadsData> listActiveThreadsAsync(ListActiveThreadsData dataPackage);

	protected:
		static ObjectCache<Channel> cache;

		static void insertChannel(Channel dataPackage);

		static void removeChannel(string channelId);
	};
	/**@}*/
}
