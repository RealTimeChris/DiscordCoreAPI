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
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For collecting a Channel from Discord's servers.
	struct GetChannelData {
		Snowflake channelId{};///< The id of the ChannelData to acquire.
	};

	/// @brief For deleting or closing a Channel.
	struct DeleteOrCloseChannelData {
		Snowflake channelId{};///< The id of the ChannelData to close/delete.
		std::string reason{};///< The reason for deleting/closing the ChannelData.
	};

	/// @brief For editing the PermissionTypes overwrites of a given Channel, for a given Role or UserData.
	struct EditChannelPermissionOverwritesData {
		PermissionOverwritesType type{};///< The type of overwrite.
		Snowflake roleOrUserId{};///< The RoleData or UserData Snowflake to modify the permissions for.
		Snowflake channelId{};///< The ChannelData id of the desired ChannelData to modify the permissions for.
		std::string reason{};///< Reason for editing the permission overwrites.
		std::string allow{};///< The permissions to list as "allowed".
		std::string deny{};///< The permissions to list as "deny".
	};

	/// @brief For collecting the invites to a given Channel.
	struct GetChannelInvitesData {
		Snowflake channelId{};
	};

	/// @brief For creating an invite to a given Channel.
	struct CreateChannelInviteData {
		Snowflake targetApplicationId{};///< The id of the embedded application to open for this invite, required if target_type is 2.
		InviteTargetTypes targetType{};///<	The type of target for this voice ChannelData invite.
		Snowflake targetUserId{};///< Snowflake of the user whose stream to display for this invite, required if target_type is 1.
		Snowflake channelId{};///< The id of the ChannelData to create the invite for.
		std::string reason{};///< Reason for creating the invite.
		int32_t maxUses{};///< Max number of uses or 0 for unlimited.between 0 and 100.
		int32_t maxAge{};///< Duration of invite in seconds before expiry, or 0 for never.between 0 and 604800 (7 days)	86400 (24 hours).
		bool temporary{};///< Whether this invite only grants temporary membership.
		bool unique{};///< If true, don't try to reuse a similar invite (useful for creating many unique one time use invites).
	};

	/// @brief For deleting the PermissionTypes overwrites of a given Channel for a given Role or UserData.
	struct DeleteChannelPermissionOverwritesData {
		Snowflake roleOrUserId{};///< The RoleData or UserData id for whom to delete the PermissionTypes overwrites.
		Snowflake channelId{};///< The id of the ChannelData to modify permissions for.
		std::string reason{};///< Reason for deleting these permission overwrites.
	};

	/// @brief For following a news ChannelData.
	struct FollowNewsChannelData {
		Snowflake targetChannelId{};
		Snowflake channelId{};
	};

	/// @brief For triggering the typing indicator in a given Channel.
	struct TriggerTypingIndicatorData {
		Snowflake channelId{};
	};

	/// @brief For acquiring a list of Channels from a chosen Guild.
	struct GetGuildChannelsData {
		Snowflake guildId{};///< The Guild from which to collect the Channels from.
	};

	/// @brief For creating a new ChannelData within a chosen Guild.
	struct CreateGuildChannelData {
		std::vector<OverWriteData> permissionOverwrites{};///< Array of overwrite objects	the ChannelData's permission overwrites.
		int32_t defaultAutoArchiveDuration{};///< Defualt duration of time before archiving a thread.
		int32_t rateLimitPerUser{};///< Amount of seconds a user has to wait before sending another message(0 - 21600).
		std::string reason{};///< Reason for creating the ChannelData.
		Snowflake parentId{};///< Snowflake of the parent category for a Channel.
		std::string topic{};///< ChannelData topic(0 - 1024 characters).
		int32_t userLimit{};///< The user limit of the voice ChannelData(voice only).
		Snowflake guildId{};///< The Guild within which to create the ChannelData.
		int32_t position{};///< Sorting position of the ChannelData.
		std::string name{};///< The name of the ChannelData.
		ChannelType type{};///< The type of ChannelData.
		int32_t bitrate{};///< The bitrate(in bits) of the voice ChannelData(voice only).
		bool nsfw{};///<  Whether the ChannelData is nsfw.
	};

	/// @brief For modifying the ChannelData position responseData of a single ChannelData.
	struct ModifyGuildChannelPositionData {
		bool lockPermissions{};///< Syncs the permission overwrites with the new parent, if moving to a new category.
		Snowflake parentId{};///< The new parent ID for the ChannelData that is moved.
		int32_t position{};///< Sorting position of the ChannelData.
		Snowflake id{};///< ChannelData id.
	};

	/// @brief For modifying the current positions of one or more Channels in the Guild.
	struct ModifyGuildChannelPositionsData {
		std::vector<ModifyGuildChannelPositionData> modifyChannelData{};///< Array of new ChannelData position's responseData.
		std::string reason{};///< Reason for re-ordering the ChannelData positions.
		Snowflake guildId{};///< Guild within which to re-order the ChannelData positions.
	};

	/// @brief For collecting a direct-messaging ChannelData.
	struct CreateDMChannelData {
		Snowflake userId{};///< The UserData for whom to collect the direct-messaging ChannelData to.
	};

	/// @brief For modifying a Channel's properties.
	struct DiscordCoreAPI_Dll ModifyChannelData {
		UpdateChannelData channelData{};///< The responseData of the ChannelData to be updated.
		Snowflake channelId{};///< The id of the ChannelData to modify.
		std::string reason{};///< A reason for modifying the ChannelData.

		ModifyChannelData(ChannelData newData);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the ChannelData-related endpoints.
	class DiscordCoreAPI_Dll Channels {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreClient;
		friend class ChannelData;
		friend class GuildData;
		friend class Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManager);

		/// @brief Collects a Channel from the Discord servers.
		/// @param dataPackage A GetChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ChannelData> getChannelAsync(GetChannelData dataPackage);

		/// @brief Collects a Channel from the library's cache.
		/// @param dataPackage A GetChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static ChannelCacheData getCachedChannel(GetChannelData dataPackage);

		/// @brief Modifies a Channel's properties.
		/// @param dataPackage A ModifyChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ChannelData> modifyChannelAsync(ModifyChannelData dataPackage);

		/// @brief Delete a Channel, or close a protected message.
		/// @param dataPackage A DeleteOrCloseChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<void> deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage);

		/// @brief Edit the given std::string overwrites for a given User or RoleData.
		/// @param dataPackage An EditChannelPermissionOverwritesData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage);

		/// @brief Collects a vector of the invites to a given Channel.
		/// @param dataPackage A GetChannelInvitesData structure.
		/// @return A CoRoutine containing a std::vector<InviteData>.
		static CoRoutine<std::vector<InviteData>> getChannelInvitesAsync(GetChannelInvitesData dataPackage);

		/// @brief Creates an invite to a selected ChannelData.
		/// @param dataPackage A CreateChannelInviteData structure.
		/// @return A CoRoutine containing an InviteData.
		static CoRoutine<InviteData> createChannelInviteAsync(CreateChannelInviteData dataPackage);

		/// @brief Delete the given std::string overwrites for a given User or RoleData.
		/// @param dataPackage A DeleteChannelPermissionOverwritesData structure
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage);

		/// @brief Follows a given new ChannelData with another ChannelData.
		/// @param dataPackage A FollowNewsChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ChannelData> followNewsChannelAsync(FollowNewsChannelData dataPackage);

		/// @brief Triggers the typing indicator for the bot in the given Channel.
		/// @param dataPackage A TriggerTypingIndicatorData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage);

		/// @brief Collects a list of Channels from a chosen Guild.
		/// @param dataPackage A GetGuildChannelsData structure.
		/// @return A CoRoutine containing a std::vector<ChannelData>.
		static CoRoutine<std::vector<ChannelData>> getGuildChannelsAsync(GetGuildChannelsData dataPackage);

		/// @brief Creates a new ChannelData within a chosen Guild.
		/// @param dataPackage A CreateGuildChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ChannelData> createGuildChannelAsync(CreateGuildChannelData dataPackage);

		/// @brief Re-orders the ChannelData positions, within a chosen Guild.
		/// @param dataPackage A ModifyGuildChannelPositionsData  structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage);

		/// @brief Collect a direct-MessageData Channel between the bot and the UserData.
		/// @param dataPackage A CreateDMChannelData structure.
		/// @return A CoRoutine containing a Channel.
		static CoRoutine<ChannelData> createDMChannelAsync(CreateDMChannelData dataPackage);

		/// @brief Collect a list of voice regions that are usable for the RTC-Region option of a given Channel.
		/// @return A CoRoutine containing a VoiceRegionDataVector.
		static CoRoutine<std::vector<VoiceRegionData>> getVoiceRegionsAsync();

		template<typename ChannelType> inline static void insertChannel(ChannelType&& channel) {
			if (doWeCacheChannelsBool) {
				if (channel.id == 0) {
					throw DCAException{ "Sorry, but there was no id set for that channel." };
				}
				cache.emplace(std::forward<ChannelType>(channel));
			}
		}

		static void removeChannel(const ChannelCacheData& channelId);

		static bool doWeCacheChannels();

	  protected:
		static ObjectCache<ChannelCacheData> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static bool doWeCacheChannelsBool;
	};
	/**@}*/
}
