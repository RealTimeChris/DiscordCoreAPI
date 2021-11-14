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
		EditChannelPermissionOverwritesType type{}; ///< The type of overwrite.
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
		__int32 maxUses{ 0 };///< Max number of uses or 0 for unlimited.between 0 and 100.
		__int32 maxAge{ 0 };///< Duration of invite in seconds before expiry, or 0 for never.between 0 and 604800 (7 days)	86400 (24 hours).
		string reason{ "" };///< Reason for creating the invite.
	};

	/// For collecting a Channel from Discord's servers. \brief For collecting a Channel from Discord's servers.
	struct DiscordCoreAPI_Dll GetChannelData {
		string channelId{ "" }; ///< The id of the Channel to acquire.
	};

	/// For deleting or closing a Channel. \brief For deleting or closing a Channel.
	struct DiscordCoreAPI_Dll DeleteChannelData {
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

	/// A Channel object. \brief A Channel object.
	class DiscordCoreAPI_Dll Channel : public ChannelData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Channel>;
		friend class DiscordCoreInternal::HttpRequestAgent;
		friend class DiscordCoreInternal::ChannelManager;
		template<typename returnValueType>
		friend class DiscordCoreAPI::CoRoutine;
		friend struct OnChannelDeletionData;
		friend struct OnChannelCreationData;
		friend struct OnThreadCreationData;
		friend struct OnThreadDeletionData;
		friend struct OnChannelUpdateData;
		friend struct OnThreadUpdateData;
		friend class DiscordCoreClient;
		friend class Guild;

		Channel();

		Channel(ChannelData dataNew);
	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll ChannelManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Channels;
		friend class DiscordCoreAPI::Guild;

		ChannelManager();

	protected:

		shared_ptr<concurrent_unordered_map<string, DiscordCoreAPI::Channel>> cache{ nullptr };

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> getCachedChannelAsync(DiscordCoreAPI::GetChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> getChannelAsync(DiscordCoreAPI::GetChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> modifyChannelAsync(DiscordCoreAPI::ModifyChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<void>  deleteOrCloseChannelAsync(DiscordCoreAPI::DeleteChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> getDMChannelAsync(DiscordCoreAPI::GetDMChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<void> editChannelPermissionOverwritesAsync(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

		DiscordCoreAPI::CoRoutine<vector<DiscordCoreAPI::InviteData>> getChannelInvitesAsync(DiscordCoreAPI::GetChannelInvitesData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::InviteData> createChannelInviteAsync(DiscordCoreAPI::CreateChannelInviteData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteChannelPermissionOverwritesAsync(DiscordCoreAPI::DeleteChannelPermissionOverwritesData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> followNewsChannelAsync(DiscordCoreAPI::FollowNewsChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<void> triggerTypingIndicatorAsync(DiscordCoreAPI::TriggerTypingIndicatorData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> startThreadWithMessageAsync(DiscordCoreAPI::StartThreadWithMessageData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> startThreadWithoutMessageAsync(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage);

		void insertChannel(DiscordCoreAPI::Channel channel);

		void removeChannel(string channelId);
	};
}
