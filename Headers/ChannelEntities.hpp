// ChannelEntities.hpp - Header for the classes related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "Http.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For collecting a Channel from Discord's servers. \brief For collecting a Channel from Discord's servers.
	struct DiscordCoreAPI_Dll GetChannelData {
		string channelId{ "" }; ///< The id of the Channel to acquire.
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
		UpdateChannelData channelData{};///< The responseData of the channel to be updated.
		string channelId{ "" };///< The id of the Channel to modify.
		string reason{ "" };///< A reason for modifying the Channel.
	};

	/// For deleting or closing a Channel. \brief For deleting or closing a Channel.
	struct DiscordCoreAPI_Dll DeleteOrCloseChannelData {
		string channelId{ "" };
		string reason{ "" };
	};

	/// For editing the Permission overwrites of a given Channel, for a given role or User. \brief For editing the Permission overwrites of a given Channel, for a given role or User.
	struct DiscordCoreAPI_Dll EditChannelPermissionOverwritesData {
		PermissionOverwritesType type{}; ///< The type of overwrite.
		string roleOrUserId{ "" };	///< The role or User Id to modify the permissions for.
		string channelId{ "" };	///< The Channel id of the desired Channel to modify the permissions for.
		string allow{ "" };	///< The permissions to list as "allowed".
		string deny{ "" };	///< The permissions to list as "deny".
		string reason{ "" };///< Reason for editing the permission overwrites.
	};

	/// For collecting the invites to a given channel. \brief For collecting the invites to a given channel.
	struct DiscordCoreAPI_Dll GetChannelInvitesData {
		string channelId{ "" };
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

	/// For deleting the Permission overwrites of a given Channel for a given role or User. \brief For deleting the Permission overwrites of a given Channel for a given role or User.
	struct DiscordCoreAPI_Dll DeleteChannelPermissionOverwritesData {
		string roleOrUserId{ "" }; ///< The role or User id for whom to delete the Permission overwrites.
		string channelId{ "" }; ///< The id of the Channel to modify permissions for.
		string reason{ "" };///< Reason for deleting these permission overwrites.
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

	/// For modifying the Channel position responseData of a single Channel. \brief For modifying the Channel position responseData of a single Channel.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionData {
		bool lockPermissions{ false };///< Syncs the permission overwrites with the new parent, if moving to a new category.
		string parentId{ "" };///< The new parent ID for the channel that is moved.
		int32_t position{ 0 };///< Sorting position of the channel.
		string id{ "" };///< Channel id.
	};

	/// For modifying the current positions of one or more Channels in the Guild. \brief For modifying the current positions of one or more Channels in the Guild.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionsData {
		vector<ModifyGuildChannelPositionData> modifyChannelData{};///< Array of new Channel position's responseData.
		string guildId{ "" };///< Guild within which to re-order the Channel positions.
		string reason{ "" };///< Reason for re-ordering the Channel positions.
	};

	/// For collecting a direct-messaging Channel. \brief For collecting a direct-messaging Channel.
	struct DiscordCoreAPI_Dll CreateDMChannelData {
		string userId{ "" }; ///< The User for whom to collect the direct-messaging Channel to.
	};

	/// A Channel object. \brief A Channel object.
	class DiscordCoreAPI_Dll Channel : public ChannelData {
	public:

		Channel();

		Channel(ChannelData dataNew);
	};

	/**@}*/

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class for the Channel related endpoints. \brief An interface class for the Channel-related endpoints.
	class DiscordCoreAPI_Dll Channels {
	public:

		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Guild;

		static void initialize(shared_ptr<DiscordCoreInternal::HttpClient>);

		/// Collects a Channel from the Discord servers. \brief Collects a Channel from the Discord servers.
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> getChannelAsync(GetChannelData dataPackage);

		/// Collects a Channel from the library's cache. \brief Collects a Channel from the library's cache.		
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> getCachedChannelAsync(GetChannelData dataPackage);		

		/// Modifies a Channel's properties. \brief Modifies a Channel's properties.
		/// \param dataPackage A ModifyChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> modifyChannelAsync(ModifyChannelData dataPackage);

		/// Delete a channel, or close a private message. \brief Delete a channel, or close a private message.
		/// \param dataPackage A DeleteOrCloseChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<void> deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage);

		/// Edit the given Permissions overwrites for a given User or Role. \brief Edit the given Permissions overwrites for a given User or Role.
		/// \param dataPackage An EditChannelPermissionOverwritesData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage);

		/// Collects a vector of the invites to a given Channel. \brief Collects a vector of the invites to a given Channel.
		/// \param dataPackage A GetChannelInvitesData structure.
		/// \returns A CoRoutine containing a vector<InviteData>.
		static CoRoutine<vector<InviteData>> getChannelInvitesAsync(GetChannelInvitesData dataPackage);

		/// Creates an invite to a selected Channel. \brief Creates an invite to a selected Channel.
		/// \param dataPackage A CreateChannelInviteData structure.
		/// \returns A CoRoutine containing an InviteData.
		static CoRoutine<InviteData> createChannelInviteAsync(CreateChannelInviteData dataPackage);

		/// Delete the given Permissions overwrites for a given User or Role. \brief Delete the given Permissions overwrites for a given User or Role.
		/// \param dataPackage A DeleteChannelPermissionOverwritesData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage);		

		/// Follows a given new Channel with another Channel. \brief Follows a given new Channel with another Channel.
		/// \param dataPackage A FollowNewsChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> followNewsChannelAsync(FollowNewsChannelData dataPackage);

		/// Triggers the typing indicator for the bot in the given Channel. \brief Triggers the typing indicator for the bot in the given Channel.
		/// \param dataPackage A TriggerTypingIndicatorData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage);

		/// Collects a list of Channels from a chosen Guild. \brief Collects a list of Channels from a chosen Guild.
		/// \param dataPackage A GetGuildChannelsData structure.
		/// \returns A CoRoutine containing a vector<Channel>.
		static CoRoutine<vector<Channel>> getGuildChannelsAsync(GetGuildChannelsData dataPackage);

		/// Creates a new Channel within a chosen Guild. \brief Creates a new Channel within a chosen Guild.
		/// \param dataPackage A CreateGuildChannelData structure.
		/// \returns A CoRoutine containing a vector<Channel>.
		static CoRoutine<Channel> createGuildChannelAsync(CreateGuildChannelData dataPackage);

		/// Re-orders the Channel positions, within a chosen Guild. \brief Re-orders the Channel positions, within a chosen Guild.
		/// \param dataPackage A ModifyGuildChannelPositionsData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage);

		/// Collect a direct-Message Channel between the bot and the User. \brief Collect a direct-Message Channel between the bot and the User.
		/// \param dataPackage A CreateDMChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> createDMChannelAsync(CreateDMChannelData dataPackage);

		/// Collect a list of voice regions that are usable for the RTC-Region option of a given Channel. \brief Collect a list of voice regions that are usable for the RTC-Region option of a given Channel.
		/// \returns A CoRoutine containing a vector<VoiceRegionData>.
		static CoRoutine<vector<VoiceRegionData>> getVoiceRegionsAsync();

	protected:

		static shared_ptr<DiscordCoreInternal::HttpClient> httpClient;
		static unordered_map<string, Channel> cache;

		static void insertChannel(Channel dataPackage);

		static void removeChannel(string channelId);
		
	};
	/**@}*/
}
