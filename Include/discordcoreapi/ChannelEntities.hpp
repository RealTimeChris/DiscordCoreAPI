/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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
/// ChannelEntities.hpp - Header for the classes related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ChannelEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	DiscordCoreAPI_Dll inline bool operator==(const ChannelData& lhs, const ChannelData& rhs) {
		return lhs.id == rhs.id;
	}

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief For collecting a Channel from Discord's servers.
	struct DiscordCoreAPI_Dll GetChannelData {
		Snowflake channelId{};///< The id of the Channel to acquire.
	};

	/// \brief For deleting or closing a Channel.
	struct DiscordCoreAPI_Dll DeleteOrCloseChannelData {
		Snowflake channelId{};///< The id of the Channel to close/delete.
		std::string reason{};///< The reason for deleting/closing the Channel.
	};

	/// \brief For editing the PermissionTypes overwrites of a given Channel, for a given Role or User.
	struct DiscordCoreAPI_Dll EditChannelPermissionOverwritesData {
		PermissionOverwritesType type{};///< The type of overwrite.
		Snowflake roleOrUserId{};///< The Role or User Id to modify the permissions for.
		Snowflake channelId{};///< The Channel id of the desired Channel to modify the permissions for.
		std::string reason{};///< Reason for editing the permission overwrites.
		std::string allow{};///< The permissions to list as "allowed".
		std::string deny{};///< The permissions to list as "deny".

		operator Jsonifier();
	};

	/// \brief For collecting the invites to a given Channel.
	struct DiscordCoreAPI_Dll GetChannelInvitesData {
		Snowflake channelId{};
	};

	/// \brief For creating an invite to a given Channel.
	struct DiscordCoreAPI_Dll CreateChannelInviteData {
		Snowflake targetApplicationId{};///< The id of the embedded application to open for this invite, required if target_type is 2.
		InviteTargetTypes targetType{};///<	The type of target for this voice Channel invite.
		Snowflake targetUserId{};///< Id of the user whose stream to display for this invite, required if target_type is 1.
		Snowflake channelId{};///< The id of the Channel to create the invite for.
		std::string reason{};///< Reason for creating the invite.
		int32_t maxUses{};///< Max number of uses or 0 for unlimited.between 0 and 100.
		int32_t maxAge{};///< Duration of invite in seconds before expiry, or 0 for never.between 0 and 604800 (7 days)	86400 (24 hours).
		bool temporary{};///< Whether this invite only grants temporary membership.
		bool unique{};///< If true, don't try to reuse a similar invite (useful for creating many unique one time use invites).

		operator Jsonifier();
	};

	/// \brief For deleting the PermissionTypes overwrites of a given Channel for a given Role or User.
	struct DiscordCoreAPI_Dll DeleteChannelPermissionOverwritesData {
		Snowflake roleOrUserId{};///< The Role or User id for whom to delete the PermissionTypes overwrites.
		Snowflake channelId{};///< The id of the Channel to modify permissions for.
		std::string reason{};///< Reason for deleting these permission overwrites.
	};

	/// \brief For following a news Channel.
	struct DiscordCoreAPI_Dll FollowNewsChannelData {
		Snowflake targetChannelId{};
		Snowflake channelId{};

		operator Jsonifier();
	};

	/// \brief For triggering the typing indicator in a given Channel.
	struct DiscordCoreAPI_Dll TriggerTypingIndicatorData {
		Snowflake channelId{};
	};

	/// \brief For acquiring a list of Channels from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildChannelsData {
		Snowflake guildId{};///< The Guild from which to collect the Channels from.
	};

	/// \brief For creating a new Channel within a chosen Guild.
	struct DiscordCoreAPI_Dll CreateGuildChannelData {
		std::vector<OverWriteData> permissionOverwrites{};///< Array of overwrite objects	the Channel's permission overwrites.
		int32_t defaultAutoArchiveDuration{};///< Defualt duration of time before archiving a thread.
		int32_t rateLimitPerUser{};///< Amount of seconds a user has to wait before sending another message(0 - 21600).
		std::string reason{};///< Reason for creating the Channel.
		Snowflake parentId{};///< Id of the parent category for a Channel.
		std::string topic{};///< Channel topic(0 - 1024 characters).
		int32_t userLimit{};///< The user limit of the voice Channel(voice only).
		Snowflake guildId{};///< The Guild within which to create the Channel.
		int32_t position{};///< Sorting position of the Channel.
		std::string name{};///< The name of the Channel.
		ChannelType type{};///< The type of Channel.
		int32_t bitrate{};///< The bitrate(in bits) of the voice Channel(voice only).
		bool nsfw{};///<  Whether the Channel is nsfw.

		operator Jsonifier();
	};

	/// \brief For modifying the Channel position responseData of a single Channel.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionData {
		bool lockPermissions{};///< Syncs the permission overwrites with the new parent, if moving to a new category.
		Snowflake parentId{};///< The new parent ID for the Channel that is moved.
		int32_t position{};///< Sorting position of the Channel.
		Snowflake id{};///< Channel id.
	};

	/// \brief For modifying the current positions of one or more Channels in the Guild.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionsData {
		std::vector<ModifyGuildChannelPositionData> modifyChannelData{};///< Array of new Channel position's responseData.
		std::string reason{};///< Reason for re-ordering the Channel positions.
		Snowflake guildId{};///< Guild within which to re-order the Channel positions.

		operator Jsonifier();
	};

	/// \brief For collecting a direct-messaging Channel.
	struct DiscordCoreAPI_Dll CreateDMChannelData {
		Snowflake userId{};///< The User for whom to collect the direct-messaging Channel to.
	};

	/// \brief A Channel object.
	class DiscordCoreAPI_Dll Channel : public ChannelData {
	  public:
		std::unordered_map<uint64_t, UserData> recipients{};///< Recipients, in the case of a group DM or m.
		DefaultReactionData defaultReactionEmoji{};///< The emoji to show in the add reaction button on a thread in a GUILD_FORUM channel.
		std::vector<ForumTagData> availableTags{};///< The set of tags that can be used in a GUILD_FORUM channel.
		int32_t defaultThreadRateLimitPerUser{};///< The initial rate_limit_per_user to set on newly created threads in a channel.
		int32_t defaultAutoArchiveDuration{};///< Default time it takes to archive a thread.
		std::vector<Snowflake> appliedTags{};///< The IDs of the set of tags that have been applied to a thread in a GUILD_FORUM channel.
		ThreadMetadataData threadMetadata{};///< Metadata in the case that this Channel is a Thread.
		StringWrapper lastMessageId{};///< Id of the last Message.
		TimeStamp lastPinTimestamp{};///< Timestamp of the last pinned Message.
		StringWrapper permissions{};///< Computed permissions for the invoking user in the channel, including overwrites.
		int32_t videoQualityMode{};///< Video quality mode.
		int32_t rateLimitPerUser{};///< Amount of seconds a User has to wait before sending another Message.
		int32_t totalMessageSent{};///< Number of messages ever sent in a thread it's similar to message_count on message creation.
		Snowflake applicationId{};///< Application id of the current application.
		StringWrapper rtcRegion{};///< Real-time clock region.
		ThreadMemberData member{};///< Thread member object for the current User, if they have joined the Thread.
		int32_t messageCount{};///< An approximate count of Messages in a Thread stops counting at 50.
		StringWrapper topic{};///< The Channel's topic.
		int32_t userLimit{};///< User limit, in the case of voice channels.
		int32_t bitrate{};///< Bitrate of the Channel, if it is a voice Channel.
		IconHash icon{};///< Icon for the Channel, if applicable.

		Channel() noexcept = default;

		Channel& operator=(ChannelData&&) noexcept;

		Channel(ChannelData&&) noexcept;

		Channel& operator=(const ChannelData&) noexcept;

		Channel(const ChannelData&) noexcept;

		Channel(simdjson::ondemand::value jsonObjectData);

		~Channel() noexcept = default;

		std::string getIconUrl() noexcept;
	};

	class DiscordCoreAPI_Dll ChannelVector {
	  public:
		ChannelVector() noexcept = default;

		operator std::vector<Channel>();

		ChannelVector(simdjson::ondemand::value jsonObjectData);

		virtual ~ChannelVector() noexcept = default;

	  protected:
		std::vector<Channel> channels{};
	};

	/// \brief For modifying a Channel's properties.
	struct DiscordCoreAPI_Dll ModifyChannelData {
		UpdateChannelData channelData{};///< The responseData of the Channel to be updated.
		Snowflake channelId{};///< The id of the Channel to modify.
		std::string reason{};///< A reason for modifying the Channel.

		operator Jsonifier();

		ModifyChannelData(Channel newData);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the Channel-related endpoints.
	class DiscordCoreAPI_Dll Channels {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreClient;
		friend class ChannelData;
		friend class GuildData;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManager);

		/// \brief Collects a Channel from the Discord servers.
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> getChannelAsync(GetChannelData dataPackage);

		/// \brief Collects a Channel from the library's cache.
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static ChannelData getCachedChannel(GetChannelData dataPackage);

		/// \brief Modifies a Channel's properties.
		/// \param dataPackage A ModifyChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> modifyChannelAsync(ModifyChannelData dataPackage);

		/// \brief Delete a Channel, or close a private message.
		/// \param dataPackage A DeleteOrCloseChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<void> deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage);

		/// \brief Edit the given Permissions overwrites for a given User or Role.
		/// \param dataPackage An EditChannelPermissionOverwritesData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage);

		/// \brief Collects a vector of the invites to a given Channel.
		/// \param dataPackage A GetChannelInvitesData structure.
		/// \returns A CoRoutine containing a InviteDataVector.
		static CoRoutine<std::vector<InviteData>> getChannelInvitesAsync(GetChannelInvitesData dataPackage);

		/// \brief Creates an invite to a selected Channel.
		/// \param dataPackage A CreateChannelInviteData structure.
		/// \returns A CoRoutine containing an InviteData.
		static CoRoutine<InviteData> createChannelInviteAsync(CreateChannelInviteData dataPackage);

		/// \brief Delete the given Permissions overwrites for a given User or Role.
		/// \param dataPackage A DeleteChannelPermissionOverwritesData structure
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage);

		/// \brief Follows a given new Channel with another Channel.
		/// \param dataPackage A FollowNewsChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> followNewsChannelAsync(FollowNewsChannelData dataPackage);

		/// \brief Triggers the typing indicator for the bot in the given Channel.
		/// \param dataPackage A TriggerTypingIndicatorData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage);

		/// \brief Collects a list of Channels from a chosen Guild.
		/// \param dataPackage A GetGuildChannelsData structure.
		/// \returns A CoRoutine containing a ChannelVector.
		static CoRoutine<std::vector<Channel>> getGuildChannelsAsync(GetGuildChannelsData dataPackage);

		/// \brief Creates a new Channel within a chosen Guild.
		/// \param dataPackage A CreateGuildChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> createGuildChannelAsync(CreateGuildChannelData dataPackage);

		/// \brief Re-orders the Channel positions, within a chosen Guild.
		/// \param dataPackage A ModifyGuildChannelPositionsData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage);

		/// \brief Collect a direct-Message Channel between the bot and the User.
		/// \param dataPackage A CreateDMChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> createDMChannelAsync(CreateDMChannelData dataPackage);

		/// \brief Collect a list of voice regions that are usable for the RTC-Region option of a given Channel.
		/// \returns A CoRoutine containing a VoiceRegionDataVector.
		static CoRoutine<std::vector<VoiceRegionData>> getVoiceRegionsAsync();

		static void insertChannel(ChannelData channel);

		static void removeChannel(Snowflake channelId);

		static bool doWeCacheChannels();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<ChannelData> cache;
		static bool doWeCacheChannelsBool;
	};
	/**@}*/
}// namespace DiscordCoreAPI
