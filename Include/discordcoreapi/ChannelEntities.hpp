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
/// ChannelEntities.hpp - Header for the classes related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ChannelEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief For collecting a Channel from Discord's servers.
	struct DiscordCoreAPI_Dll GetChannelData {
		DiscordCoreAPI::Snowflake channelId{};///< The id of the Channel to acquire.
	};

	/// \brief For deleting or closing a Channel.
	struct DiscordCoreAPI_Dll DeleteOrCloseChannelData {
		DiscordCoreAPI::Snowflake channelId{};///< The id of the Channel to close/delete.
		std::string reason{};///< The reason for deleting/closing the Channel.
	};

	/// \brief For editing the PermissionTypes overwrites of a given Channel, for a given Role or User.
	struct DiscordCoreAPI_Dll EditChannelPermissionOverwritesData {
		PermissionOverwritesType type{};///< The type of overwrite.
		DiscordCoreAPI::Snowflake roleOrUserId{};///< The Role or User DiscordCoreAPI::Snowflake to modify the permissions for.
		DiscordCoreAPI::Snowflake channelId{};///< The Channel id of the desired Channel to modify the permissions for.
		std::string reason{};///< Reason for editing the permission overwrites.
		std::string allow{};///< The permissions to list as "allowed".
		std::string deny{};///< The permissions to list as "deny".
	};

	/// \brief For collecting the invites to a given Channel.
	struct DiscordCoreAPI_Dll GetChannelInvitesData {
		DiscordCoreAPI::Snowflake channelId{};
	};

	/// \brief For creating an invite to a given Channel.
	struct DiscordCoreAPI_Dll CreateChannelInviteData {
		DiscordCoreAPI::Snowflake targetApplicationId{};///< The id of the embedded application to open for this invite, required if target_type is 2.
		InviteTargetTypes targetType{};///<	The type of target for this voice Channel invite.
		DiscordCoreAPI::Snowflake
			targetUserId{};///< DiscordCoreAPI::Snowflake of the user whose stream to display for this invite, required if target_type is 1.
		DiscordCoreAPI::Snowflake channelId{};///< The id of the Channel to create the invite for.
		std::string reason{};///< Reason for creating the invite.
		int32_t maxUses{};///< Max number of uses or 0 for unlimited.between 0 and 100.
		int32_t maxAge{};///< Duration of invite in seconds before expiry, or 0 for never.between 0 and 604800 (7 days)	86400 (24 hours).
		bool temporary{};///< Whether this invite only grants temporary membership.
		bool unique{};///< If true, don't try to reuse a similar invite (useful for creating many unique one time use invites).
	};

	/// \brief For deleting the PermissionTypes overwrites of a given Channel for a given Role or User.
	struct DiscordCoreAPI_Dll DeleteChannelPermissionOverwritesData {
		DiscordCoreAPI::Snowflake roleOrUserId{};///< The Role or User id for whom to delete the PermissionTypes overwrites.
		DiscordCoreAPI::Snowflake channelId{};///< The id of the Channel to modify permissions for.
		std::string reason{};///< Reason for deleting these permission overwrites.
	};

	/// \brief For following a news Channel.
	struct DiscordCoreAPI_Dll FollowNewsChannelData {
		DiscordCoreAPI::Snowflake targetChannelId{};
		DiscordCoreAPI::Snowflake channelId{};
	};

	/// \brief For triggering the typing indicator in a given Channel.
	struct DiscordCoreAPI_Dll TriggerTypingIndicatorData {
		DiscordCoreAPI::Snowflake channelId{};
	};

	/// \brief For acquiring a list of Channels from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildChannelsData {
		DiscordCoreAPI::Snowflake guildId{};///< The Guild from which to collect the Channels from.
	};

	/// \brief For creating a new Channel within a chosen Guild.
	struct DiscordCoreAPI_Dll CreateGuildChannelData {
		std::vector<OverWriteData> permissionOverwrites{};///< Array of overwrite objects	the Channel's permission overwrites.
		int32_t defaultAutoArchiveDuration{};///< Defualt duration of time before archiving a thread.
		int32_t rateLimitPerUser{};///< Amount of seconds a user has to wait before sending another message(0 - 21600).
		std::string reason{};///< Reason for creating the Channel.
		DiscordCoreAPI::Snowflake parentId{};///< DiscordCoreAPI::Snowflake of the parent category for a Channel.
		std::string topic{};///< Channel topic(0 - 1024 characters).
		int32_t userLimit{};///< The user limit of the voice Channel(voice only).
		DiscordCoreAPI::Snowflake guildId{};///< The Guild within which to create the Channel.
		int32_t position{};///< Sorting position of the Channel.
		std::string name{};///< The name of the Channel.
		ChannelType type{};///< The type of Channel.
		int32_t bitrate{};///< The bitrate(in bits) of the voice Channel(voice only).
		bool nsfw{};///<  Whether the Channel is nsfw.
	};

	/// \brief For modifying the Channel position responseData of a single Channel.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionData {
		bool lockPermissions{};///< Syncs the permission overwrites with the new parent, if moving to a new category.
		DiscordCoreAPI::Snowflake parentId{};///< The new parent ID for the Channel that is moved.
		int32_t position{};///< Sorting position of the Channel.
		DiscordCoreAPI::Snowflake id{};///< Channel id.
	};

	/// \brief For modifying the current positions of one or more Channels in the Guild.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionsData {
		std::vector<ModifyGuildChannelPositionData> modifyChannelData{};///< Array of new Channel position's responseData.
		std::string reason{};///< Reason for re-ordering the Channel positions.
		DiscordCoreAPI::Snowflake guildId{};///< Guild within which to re-order the Channel positions.
	};

	/// \brief For collecting a direct-messaging Channel.
	struct DiscordCoreAPI_Dll CreateDMChannelData {
		DiscordCoreAPI::Snowflake userId{};///< The User for whom to collect the direct-messaging Channel to.
	};

	/// \brief For modifying a Channel's properties.
	struct DiscordCoreAPI_Dll ModifyChannelData {
		UpdateChannelData channelData{};///< The responseData of the Channel to be updated.
		DiscordCoreAPI::Snowflake channelId{};///< The id of the Channel to modify.
		std::string reason{};///< A reason for modifying the Channel.

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

		/// \brief Delete a Channel, or close a protected message.
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
		static CoRoutine<ChannelVector> getGuildChannelsAsync(GetGuildChannelsData dataPackage);

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

		static ChannelData& insertChannel(ChannelData&& channel);

		static void removeChannel(DiscordCoreAPI::Snowflake channelId);

		static bool doWeCacheChannels();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<DiscordCoreAPI::Snowflake, ChannelData> cache;
		static bool doWeCacheChannelsBool;
	};
	/**@}*/
}
