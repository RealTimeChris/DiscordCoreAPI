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

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For collecting a channel from discord's servers.
	struct get_channel_data {
		snowflake channelId{};///< The id of the channel_data to acquire.
	};

	/// @brief For deleting or closing a channel.
	struct delete_or_close_channel_data {
		snowflake channelId{};///< The id of the channel_data to close/delete.
		jsonifier::string reason{};///< The reason for deleting/closing the channel_data.
	};

	/// @brief For editing the permission_types overwrites of a given channel, for a given role or user_data.
	struct edit_channel_permission_overwrites_data {
		permission_overwrites_type type{};///< The type of overwrite.
		snowflake roleOrUserId{};///< The role_data or user_data snowflake to modify the permissions for.
		snowflake channelId{};///< The channel_data id of the desired channel_data to modify the permissions for.
		jsonifier::string reason{};///< Reason for editing the permission overwrites.
		permissions_parse allow{};///< The permissions to list as "allowed".
		permissions_parse deny{};///< The permissions to list as "deny".
	};

	/// @brief For collecting the invites to a given channel.
	struct get_channel_invites_data {
		snowflake channelId{};
	};

	/// @brief For creating an invite to a given channel.
	struct create_channel_invite_data {
		snowflake targetApplicationId{};///< The id of the embedded application to open for this invite, required if target_type is 2.
		invite_target_types targetType{};///<	the type of target for this voice channel_data invite.
		snowflake targetUserId{};///< snowflake of the user whose stream to display for this invite, required if target_type is 1.
		snowflake channelId{};///< The id of the channel_data to create the invite for.
		jsonifier::string reason{};///< Reason for creating the invite.
		int32_t maxUses{};///< Max number of uses or 0 for unlimited.between 0 and 100.
		int32_t maxAge{};///< Duration of invite in seconds before expiry, or 0 for never.between 0 and 604800 (7 days)	86400 (24 hours).
		bool temporary{};///< Whether this invite only grants temporary membership.
		bool unique{};///< If true, don't try to reuse a similar invite (useful for creating many unique one time use invites).
	};

	/// @brief For deleting the permission_types overwrites of a given channel for a given role or user_data.
	struct delete_channel_permission_overwrites_data {
		snowflake roleOrUserId{};///< The role_data or user_data id for whom to delete the permission_types overwrites.
		snowflake channelId{};///< The id of the channel_data to modify permissions for.
		jsonifier::string reason{};///< Reason for deleting these permission overwrites.
	};

	/// @brief For following a news channel_data.
	struct follow_news_channel_data {
		snowflake targetChannelId{};
		snowflake channelId{};
	};

	/// @brief For triggering the typing indicator in a given channel.
	struct trigger_typing_indicator_data {
		snowflake channelId{};
	};

	/// @brief For acquiring a list of channels from a chosen guild_data.
	struct get_guild_channels_data {
		snowflake guildId{};///< The guild_data from which to collect the channels from.
	};

	/// @brief For creating a new channel_data within a chosen guild_data.
	struct create_guild_channel_data {
		jsonifier::vector<over_write_data> permissionOverwrites{};///< Array of overwrite objects	the channel_data's permission overwrites.
		int32_t defaultAutoArchiveDuration{};///< Defualt duration of time before archiving a thread.
		int32_t rateLimitPerUser{};///< Amount of seconds a user has to wait before sending another message(0 - 21600).
		jsonifier::string reason{};///< Reason for creating the channel_data.
		snowflake parentId{};///< snowflake of the parent category for a channel.
		jsonifier::string topic{};///< channel_data topic(0 - 1024 characters).
		int32_t userLimit{};///< The user limit of the voice channel_data(voice only).
		snowflake guildId{};///< The guild_data within which to create the channel_data.
		int32_t position{};///< Sorting position of the channel_data.
		jsonifier::string name{};///< The name of the channel_data.
		channel_type type{};///< The type of channel_data.
		int32_t bitrate{};///< The bitrate(in bits) of the voice channel_data(voice only).
		bool nsfw{};///<  whether the channel_data is nsfw.
	};

	/// @brief For modifying the channel_data position responseData of a single channel_data.
	struct modify_guild_channel_position_data {
		bool lockPermissions{};///< Syncs the permission overwrites with the new parent, if moving to a new category.
		snowflake parentId{};///< The new parent id for the channel_data that is moved.
		int32_t position{};///< Sorting position of the channel_data.
		snowflake id{};///< channel_data id.
	};

	/// @brief For modifying the current positions of one or more channels in the guild_data.
	struct modify_guild_channel_positions_data {
		jsonifier::vector<modify_guild_channel_position_data> modifyChannelData{};///< Array of new channel_data position's responseData.
		jsonifier::string reason{};///< Reason for re-ordering the channel_data positions.
		snowflake guildId{};///< Guild within which to re-order the channel_data positions.
	};

	/// @brief For collecting a direct-messaging channel_data.
	struct create_dmchannel_data {
		snowflake userId{};///< The user_data for whom to collect the direct-messaging channel_data to.
	};

	/// @brief For modifying a channel's properties.
	struct DiscordCoreAPI_Dll modify_channel_data {
		update_channel_data channelData{};///< The responseData of the channel_data to be updated.
		snowflake channelId{};///< The id of the channel_data to modify.
		jsonifier::string reason{};///< A reason for modifying the channel_data.

		modify_channel_data(channel_data newData);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the channel_data related endpoints.
	class DiscordCoreAPI_Dll channels {
	  public:
		friend class discord_core_internal::websocket_client;
		friend class discord_core_client;
		friend class channel_data;
		friend class guild_data;
		friend class guild_data;

		static void initialize(discord_core_internal::https_client*, config_manager* configManager);

		/// @brief Collects a channel from the discord servers.
		/// @param dataPackage a get_channel_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<channel_data> getChannelAsync(get_channel_data dataPackage);

		/// @brief Collects a channel from the library's cache.
		/// @param dataPackage a get_channel_data structure.
		/// @return a co_routine containing a channel.
		static channel_cache_data getCachedChannel(get_channel_data dataPackage);

		/// @brief Modifies a channel's properties.
		/// @param dataPackage a modify_channel_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<channel_data> modifyChannelAsync(modify_channel_data dataPackage);

		/// @brief Delete a channel, or close a protected message.
		/// @param dataPackage a delete_or_close_channel_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<void> deleteOrCloseChannelAsync(delete_or_close_channel_data dataPackage);

		/// @brief Edit the given jsonifier::string overwrites for a given user or role_data.
		/// @param dataPackage an edit_channel_permission_overwrites_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> editChannelPermissionOverwritesAsync(edit_channel_permission_overwrites_data dataPackage);

		/// @brief Collects a vector of the invites to a given channel.
		/// @param dataPackage a get_channel_invites_data structure.
		/// @return a co_routine containing a jsonifier::vector<invite_data>.
		static co_routine<jsonifier::vector<invite_data>> getChannelInvitesAsync(get_channel_invites_data dataPackage);

		/// @brief Creates an invite to a selected channel_data.
		/// @param dataPackage a create_channel_invite_data structure.
		/// @return a co_routine containing an invite_data.
		static co_routine<invite_data> createChannelInviteAsync(create_channel_invite_data dataPackage);

		/// @brief Delete the given jsonifier::string overwrites for a given user or role_data.
		/// @param dataPackage a delete_channel_permission_overwrites_data structure
		/// @return a co_routine containing void.
		static co_routine<void> deleteChannelPermissionOverwritesAsync(delete_channel_permission_overwrites_data dataPackage);

		/// @brief Follows a given new channel_data with another channel_data.
		/// @param dataPackage a follow_news_channel_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<channel_data> followNewsChannelAsync(follow_news_channel_data dataPackage);

		/// @brief Triggers the typing indicator for the bot in the given channel.
		/// @param dataPackage a trigger_typing_indicator_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> triggerTypingIndicatorAsync(trigger_typing_indicator_data dataPackage);

		/// @brief Collects a list of channels from a chosen guild_data.
		/// @param dataPackage a get_guild_channels_data structure.
		/// @return a co_routine containing a jsonifier::vector<channel_data>.
		static co_routine<jsonifier::vector<channel_data>> getGuildChannelsAsync(get_guild_channels_data dataPackage);

		/// @brief Creates a new channel_data within a chosen guild_data.
		/// @param dataPackage a create_guild_channel_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<channel_data> createGuildChannelAsync(create_guild_channel_data dataPackage);

		/// @brief Re-orders the channel_data positions, within a chosen guild_data.
		/// @param dataPackage a modify_guild_channel_positions_data  structure.
		/// @return a co_routine containing void.
		static co_routine<void> modifyGuildChannelPositionsAsync(modify_guild_channel_positions_data dataPackage);

		/// @brief Collect a direct-message_data channel between the bot and the user_data.
		/// @param dataPackage a create_dmchannel_data structure.
		/// @return a co_routine containing a channel.
		static co_routine<channel_data> createDMChannelAsync(create_dmchannel_data dataPackage);

		/// @brief Collect a list of voice regions that are usable for the rtc-region option of a given channel.
		/// @return a co_routine containing a voice_region_data_vector.
		static co_routine<jsonifier::vector<voice_region_data>> getVoiceRegionsAsync();

		template<typename channel_type> DCA_INLINE static void insertChannel(channel_type&& channel) {
			if (doWeCacheChannelsBool) {
				if (channel.id == 0) {
					throw dca_exception{ "Sorry, but there was no id set for that channel." };
				}
				cache.emplace(std::forward<channel_type>(channel));
				if (cache.count() % 1000 == 0) {
					std::cout << "current channel count: " << cache.count() << std::endl;
				}
			}
		}

		static void removeChannel(const channel_cache_data& channelId);

		static bool doWeCacheChannels();

	  protected:
		static object_cache<channel_cache_data> cache;
		static discord_core_internal::https_client* httpsClient;
		static bool doWeCacheChannelsBool;
	};
	/**@}*/
}
