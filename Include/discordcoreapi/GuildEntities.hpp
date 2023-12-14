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
/// GuildEntities.hpp - Header for the guild classes and structs.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file GuildEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/StageInstanceEntities.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For geting a guild's audit logs.
	struct get_guild_audit_logs_data {
		audit_log_event actionType{};///< The action type to acquire audit-logs for.
		snowflake guildId{};///< The guiild id for the guild which you wish to query the log of.
		snowflake before{};///< Entries that preceded a specific audit log entry id.
		snowflake userId{};///< The user_data for whom to look for the actions of.
		int32_t limit{};///< The maximum number of actions to acquire from the log.
	};

	/// @brief For creating a guild.
	struct create_guild_data {
		afk_time_out_durations afkTimeout{ afk_time_out_durations::Shortest };///< Afk timeout in seconds.
		default_message_notification_level defaultMessageNotifications{};///< Default message notification level.
		explicit_content_filter_level explicitContentFilter{};///< Explicit content filter level.
		jsonifier::vector<channel_data> channels{};///< Array of partial channel_data objects.
		int32_t systemChannelFlags{};///< System channel_data flags.
		snowflake systemChannelId{};///< The id of the channel_data where guild notices such as welcome messages and boost events are posted.
		int32_t verificationLevel{};///< Verification level.
		snowflake afkChannelId{};///< snowflake for afk channel_data.
		jsonifier::string region{};///< The region that the servers are in.
		jsonifier::string icon{};///< Base64 128x128 image for the guild icon.
		jsonifier::string name{};///< The name of the new guild.
		jsonifier::vector<role_data> roles{};///< Array of role_data objects.
	};

	/// @brief For getting a guild from the library's cache or a discord server.
	struct get_guild_data {
		snowflake guildId{};///< The id of the guild to acquire.
	};

	/// @brief For acquiring a guild preview of a chosen guild.
	struct get_guild_preview_data {
		snowflake guildId{};///< The id of the guild's preview to acquire.
	};

	/// @brief For deleting a guild.
	struct delete_guild_data {
		snowflake guildId{};///< The guild you would like to delete.
	};

	/// @brief For getting a list of guild bans.
	struct get_guild_bans_data {
		snowflake guildId{};///< The guild from which to collect the list of bans.
		snowflake before{};///< consider only users before given user id.
		snowflake after{};///< consider only users after given user id null.
		uint64_t limit{};///< Number of users to return ( up to maximum 1000 ).
	};

	/// @brief For getting a single guild ban.
	struct get_guild_ban_data {
		snowflake guildId{};///< The guild from which to collect the ban from.
		snowflake userId{};///< The user_data for whom to collect the ban of.
	};

	/// @brief For banning a current guild_member_data.
	struct create_guild_ban_data {
		int32_t deleteMessageDays{};///< The number of days of their messages to delete.
		snowflake guildMemberId{};///< The id of the member to be banned.
		jsonifier::string reason{};///< The reason for the ban.
		snowflake guildId{};///< The id of the guild from which to ban the member.
	};

	/// @brief For removing a previous created ban.
	struct remove_guild_ban_data {
		jsonifier::string reason{};///< The reason for removing this ban.
		snowflake guildId{};///< The guild from which to erase the ban.
		snowflake userId{};///< The user snowflake of the user who's ban to erase.
	};

	/// @brief For collecting the guild prune count.
	struct get_guild_prune_count_data {
		jsonifier::vector<snowflake> includeRoles{};///< Roles to be included in the prune.
		snowflake guildId{};///< The guild to be pruned.
		int32_t days{};///< The number of days beyond which to prune the user's for inactivity.
	};

	/// @brief For pruning a number of guild_members from the guild.
	struct begin_guild_prune_data {
		jsonifier::vector<snowflake> includeRoles{};/// roles to be included in the prune.
		bool computePruneCount{};/// whether 'pruned' is returned, discouraged for large guilds.
		jsonifier::string reason{};///< Reason for pruning the guild_members.
		snowflake guildId{};///< Guild within which to perform the prune.
		int32_t days{};/// number of days after which to prune a  given guild_member.
	};

	/// @brief For collecting a list of guild voice regions.
	struct get_guild_voice_regions_data {
		snowflake guildId{};///< The guild for which to collect the voice regions from.
	};

	/// @brief For geting all of the current invites from a guild.
	struct get_guild_invites_data {
		snowflake guildId{};///< The id of the guild you wish to acquire.
	};

	/// @brief
	struct get_guild_integrations_data {
		snowflake guildId{};///< The guild for which to collect the integrations from.
	};

	/// @brief For deleting a guild integration.
	struct delete_guild_integration_data {
		snowflake integrationId{};///< The integration's id which we are going to delete.
		jsonifier::string reason{};///< Reason for deleting the integration.
		snowflake guildId{};///< The guild from which to delete the integration from.
	};

	/// @brief For collecting a guild's widget settings.
	struct get_guild_widget_settings_data {
		snowflake guildId{};///< The guild from which to collect the widget from.
	};

	/// @brief For modifying a guild's widget.
	struct modify_guild_widget_data {
		guild_widget_data widgetData{};///< The new guild widget responseData.
		jsonifier::string reason{};///< Reason for modifying the widget.
		snowflake guildId{};///< The guild for which to modify the widget of.
	};

	/// @brief For collecting a guild's widget.
	struct get_guild_widget_data {
		snowflake guildId{};///< The guild from which to collect the widget from.
	};

	/// @brief For geting the vanity invite responseData of a guild.
	struct get_guild_vanity_invite_data {
		snowflake guildId{};///< The id of the guild to acquire the vanity invite from.
	};

	/// @brief For collecting a guild's widget image.
	struct get_guild_widget_image_data {
		widget_style_options widgetStlye{};///< The style of widget image to collect.
		snowflake guildId{};///< The guild for which to collect the widget image from.
	};

	/// @brief For collecting a guild's welcome screen.
	struct get_guild_welcome_screen_data {
		snowflake guildId{};///< The guild for which to collect the widget image from.
	};

	/// @brief For modifying a guild's welcome screen.
	struct modify_guild_welcome_screen_data {
		jsonifier::vector<welcome_screen_channel_data> welcomeChannels{};///< Welcome channels for the welcome screen.
		jsonifier::string description{};///< The description of the welcome screen.
		jsonifier::string reason{};///< The reason for modifying the welcome screen.
		snowflake guildId{};///< The guild for which to modify the welcome screen of.
		bool enabled{};///< Is it enabled?
	};

	/// @brief For collecting a guild's template.
	struct get_guild_template_data {
		jsonifier::string templateCode{};///< code for the desired template.
	};

	/// @brief For creating a guild from a guild template.
	struct create_guild_from_guild_template_data {
		jsonifier::string templateCode{};///< code for the desired template to use.
		jsonifier::string imageData{};///< Base64 128x128 image for the guild icon.
		jsonifier::string name{};///< Desired name of the guild.
	};

	/// @brief For collecting a list of guild templates from a chosen guild.
	struct get_guild_templates_data {
		snowflake guildId{};///< Guild from which you would like to collect the templates from.
	};

	/// @brief For creating a guild template.
	struct create_guild_template_data {
		jsonifier::string description{};///< Description for the template (0 - 120 characters).
		snowflake guildId{};///< Guild within which you wuold like to create the template.
		jsonifier::string name{};///< Name of the template (1 - 100 characters).
	};

	/// @brief For syncing a guild template.
	struct sync_guild_template_data {
		jsonifier::string templateCode{};///< Template code for which template you would like to sync.
		snowflake guildId{};///< Guild for which you would like to sync the template of.
	};

	/// @brief For modifying a guild template.
	struct modify_guild_template_data {
		jsonifier::string templateCode{};/// template which you would like to modify.
		jsonifier::string description{};///< Description for the template (0 - 120 characters).
		snowflake guildId{};///< Guild within which you would like to modify the template.
		jsonifier::string name{};///< Name of the template (1 - 100 characters).
	};

	/// @brief For deleting a guild template.
	struct delete_guild_template_data {
		jsonifier::string templateCode{};///< The template which you would like to delete.
		snowflake guildId{};///< The guild within which you would like to delete the template.
	};

	/// @brief For geting a single invite's responseData from a guild.
	struct get_invite_data {
		snowflake guildScheduledEventId{};///< The guild scheduled event to include with the invite.
		bool withExpiration{};///< collect expiration time/date?
		snowflake inviteId{};///< The id of the invite you wish to acquire.
		bool withCount{};///< collect usage etc counts?
	};

	/// @brief For deleting a single guild invite.
	struct delete_invite_data {
		jsonifier::string reason{};///< Reason for deleting the invite.
		snowflake inviteId{};///< The invite which you would like to delete.
	};

	/// @brief For collecting a list of guild's that the bot is in.
	struct get_current_user_guilds_data {
		snowflake before{};///< Get guilds before this guild id.
		snowflake after{};///< Get guilds after this guild id.
		uint32_t limit{};///< Max number of guilds to return (1 - 200).
	};

	/// @brief For leaving a particular guild.
	struct leave_guild_data {
		snowflake guildId{};///< The id of the guild you would like the bot to leave.
	};

	/// @brief For modifying the properties of a chosen guild.
	struct DiscordCoreAPI_Dll modify_guild_data {
		default_message_notification_level defaultMessageNotifications{};///< Default message notification level.
		explicit_content_filter_level explicitContentFilter{};///< Explicit content filter level.
		system_channel_flags systemChannelFlags{};///< System channel_data flags.
		verification_level verificationLevel{};///< Verification level.
		jsonifier::vector<jsonifier::string> features{};///< Array of guild feature strings enabled guild features.
		snowflake publicUpdatesChannelId{};///< The id of the channel_data where admins and moderators of community guilds receive notices from discord.
		afk_time_out_durations afkTimeout{};///< Afk timeout in seconds.
		bool premiumProgressBarEnabled{};///< Whether or not the progress bar is enabled.
		jsonifier::string preferredLocale{};///< The preferred locale of a community guild used in server discovery.
		jsonifier::string discoverySplash{};/// base64 16 : 9 png / jpeg image for the guild discovery splash(when the server has the discoverable feature).
		snowflake systemChannelId{};///< The id of the channel_data where guild notices such as welcome messages and boost events are posted.
		snowflake rulesChannelId{};///< The id of the channel_data where community guilds display rules and /or guidelines.
		jsonifier::string description{};///< The description for the guild, if the guild is discoverable.
		snowflake afkChannelId{};///< snowflake for afk channels.
		jsonifier::string banner{};///< Base64 16 : 9 png / jpeg image for the guild banner (when the server has the banner feature).
		jsonifier::string splash{};///< Base64 16 : 9 png / jpeg image for the guild splash (when the server has the INVITE_SPLASH feature).
		jsonifier::string reason{};///< Reason for modifying the guild.
		snowflake ownerId{};///< User_data id to transfer guild ownership to (must be owner).
		snowflake guildId{};///< snowflake of the chosen guild to modify.
		jsonifier::string icon{};///< Base64 1024x1024 png / jpeg / gif image for the guild icon.
		jsonifier::string name{};///< Desired name of the guild.

		modify_guild_data(const guild_data dataPackage);
	};

	/**@}*/

	DCA_INLINE stop_watch<milliseconds> stopWatchNew{ milliseconds{ 100 } };

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// @brief An interface class for the guild_data related discord endpoints.
	class DiscordCoreAPI_Dll guilds {
	  public:
		static void initialize(discord_core_internal::https_client* client, config_manager* configManager);

		/// @brief Gets an audit log from the discord servers.
		/// @param dataPackage a get_guild_audit_logs_data structure.
		/// @return A co_routine containing an audit_log_data.
		static co_routine<audit_log_data> getGuildAuditLogsAsync(get_guild_audit_logs_data dataPackage);

		/// @brief Creates a new guild.
		/// @param dataPackage a create_guild_data structure.
		/// @return A co_routine containing a guild.
		static co_routine<guild_data> createGuildAsync(create_guild_data dataPackage);

		/// @brief Returns all of the guilds that the current bot is in.
		/// @return A co_routine containing a jsonifier::vector<guild_data>.
		static jsonifier::vector<guild_data> getAllGuildsAsync();

		/// @brief Collects a guild from the discord servers.
		/// @param dataPackage a get_guild_data structure.
		/// @return A co_routine containing a guild.
		static co_routine<guild_data> getGuildAsync(get_guild_data dataPackage);

		/// @brief Collects a guild from the library's cache.
		/// @param dataPackage a get_guild_data structure.
		/// @return A co_routine containing a guild.
		static guild_cache_data getCachedGuild(get_guild_data dataPackage);

		/// @brief Acquires the preview data of a chosen guild.
		/// @param dataPackage a get_guild_preview_data structure.
		/// @return A co_routine containing a guild_preview_data.
		static co_routine<guild_preview_data> getGuildPreviewAsync(get_guild_preview_data dataPackage);

		/// @brief Modifies a chosen guild's properties.
		/// @param dataPackage a modify_guild_data structure.
		/// @return A co_routine containing a guild.
		static co_routine<guild_data> modifyGuildAsync(modify_guild_data dataPackage);

		/// @brief Deletes a chosen guild.
		/// @param dataPackage a delete_guild_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> deleteGuildAsync(delete_guild_data dataPackage);

		/// @brief Collects a list of bans from a chosen guild.
		/// @param dataPackage a get_guild_bans_data structure.
		/// @return A co_routine containing a ban_data_vector.
		static co_routine<jsonifier::vector<ban_data>> getGuildBansAsync(get_guild_bans_data dataPackage);

		/// @brief Collects a single ban from a chosen guild.
		/// @param dataPackage a get_guild_ban_data structure.
		/// @return A co_routine containing a ban_data.
		static co_routine<ban_data> getGuildBanAsync(get_guild_ban_data dataPackage);

		/// @brief Bans a guild_member.
		/// @param dataPackage a create_guild_ban_data structure.
		/// @return A co_routine containing a ban_data.
		static co_routine<void> createGuildBanAsync(create_guild_ban_data dataPackage);

		/// @brief Removes a previously created ban.
		/// @param dataPackage a remove_guild_ban_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> removeGuildBanAsync(remove_guild_ban_data dataPackage);

		/// @brief For collecting the guild prune count.
		/// @param dataPackage a get_guild_prune_count_data structure.
		/// @return A co_routine containing guild_prune_count_data.
		static co_routine<guild_prune_count_data> getGuildPruneCountAsync(get_guild_prune_count_data dataPackage);

		/// @brief For performing a pruning of the guild_members of the guild, based on days of inactivity.
		/// @param dataPackage a begin_guild_prune_data structure.
		/// @return A co_routine containing a guild_prune_count_data.
		static co_routine<guild_prune_count_data> beginGuildPruneAsync(begin_guild_prune_data dataPackage);

		/// @brief Gets the list of voice regions for a particular server.
		/// @param dataPackage a get_guild_voice_regions_data structure.
		/// @return A co_routine containing a voice_region_data_vector.
		static co_routine<jsonifier::vector<voice_region_data>> getGuildVoiceRegionsAsync(get_guild_voice_regions_data dataPackage);

		/// @brief Gets multiple invites from the discord servers.
		/// @param dataPackage a get_guild_invites_data structure.
		/// @return A co_routine containing a jsonifier::vector<invite_data>.
		static co_routine<jsonifier::vector<invite_data>> getGuildInvitesAsync(get_guild_invites_data dataPackage);

		/// @brief Gets the list of guild integrations for a particular server.
		/// @param dataPackage a get_guild_integrations_data structure.
		/// @return A co_routine containing a jsonifier::vector<integration_data>.
		static co_routine<jsonifier::vector<integration_data>> getGuildIntegrationsAsync(get_guild_integrations_data dataPackage);

		/// @brief Deletes an integration from a guild.
		/// @param dataPackage a delete_guild_integration_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> deleteGuildIntegrationAsync(delete_guild_integration_data dataPackage);

		/// @brief Gets the guild widget's settings for a particular server.
		/// @param dataPackage a get_guild_widget_settings_data structure.
		/// @return A co_routine containing a guild_widget_data.
		static co_routine<guild_widget_data> getGuildWidgetSettingsAsync(get_guild_widget_settings_data dataPackage);

		/// @brief Modifies the guild widget for a particular server.
		/// @param dataPackage a modify_guild_widget_data structure.
		/// @return A co_routine containing a guild_widget_data.
		static co_routine<guild_widget_data> modifyGuildWidgetAsync(modify_guild_widget_data dataPackage);

		/// @brief Gets the guild widget for a particular server.
		/// @param dataPackage a get_guild_widget_data structure.
		/// @return A co_routine containing a guild_widget_data.
		static co_routine<guild_widget_data> getGuildWidgetAsync(get_guild_widget_data dataPackage);

		/// @brief Gets the vanity invite responseData from a particular server.
		/// @param dataPackage a get_guild_vanity_invite_data structure.
		/// @return A co_routine containing invite_data.
		static co_routine<invite_data> getGuildVanityInviteAsync(get_guild_vanity_invite_data dataPackage);

		/// @brief Gets the guild widget image for a particular server.
		/// @param dataPackage a get_guild_widget_image_data structure.
		/// @return A co_routine containing a guild_widget_image_data.
		static co_routine<guild_widget_image_data> getGuildWidgetImageAsync(get_guild_widget_image_data dataPackage);

		/// @brief Gets the guild welcome screen for a particular server.
		/// @param dataPackage a get_guild_welcome_screen_data structure.
		/// @return A co_routine containing a welcome_screen_data.
		static co_routine<welcome_screen_data> getGuildWelcomeScreenAsync(get_guild_welcome_screen_data dataPackage);

		/// @brief Modifies the guild welcome screen.
		/// @param dataPackage a modify_guild_welcome_screen_data structure.
		/// @return A co_routine containing a welcome_screen_data.
		static co_routine<welcome_screen_data> modifyGuildWelcomeScreenAsync(modify_guild_welcome_screen_data dataPackage);

		/// @brief Gets the guild template from a particular server.
		/// @param dataPackage a get_guild_template_data structure.
		/// @return A co_routine containing a guild_template_data.
		static co_routine<guild_template_data> getGuildTemplateAsync(get_guild_template_data dataPackage);

		/// @brief Creates a guild from the guild template.
		/// @param dataPackage a create_guild_from_guild_template_data structure.
		/// @return A co_routine containing a guild.
		static co_routine<guild_data> createGuildFromGuildTemplateAsync(create_guild_from_guild_template_data dataPackage);

		/// @brief Collects a list of guild templates from a chosen guild.
		/// @param dataPackage a get_guild_templates_data structure.
		/// @return A co_routine containing a guiild_template_data_vector.
		static co_routine<jsonifier::vector<guild_template_data>> getGuildTemplatesAsync(get_guild_templates_data dataPackage);

		/// @brief Creates a guild template.
		/// @param dataPackage a create_guild_template_data structure.
		/// @return A co_routine containing a guiild_template_data.
		static co_routine<guild_template_data> createGuildTemplateAsync(create_guild_template_data dataPackage);

		/// @brief Syncs a guild template.
		/// @param dataPackage a sync_guild_template_data structure.
		/// @return A co_routine containing a guiild_template_data.
		static co_routine<guild_template_data> syncGuildTemplateAsync(sync_guild_template_data dataPackage);

		/// @brief Modifies a guild template.
		/// @param dataPackage a modify_guild_template_data structure.
		/// @return A co_routine containing a guiild_template_data.
		static co_routine<guild_template_data> modifyGuildTemplateAsync(modify_guild_template_data dataPackage);

		/// @brief Deletes a guild template.
		/// @param dataPackage a delete_guild_template_data structure.
		/// @return A co_routine containing a void.
		static co_routine<void> deleteGuildTemplateAsync(delete_guild_template_data dataPackage);

		/// @brief Gets an invite from the discord servers.
		/// @param dataPackage a get_invite_data structure.
		/// @return A co_routine containing an invite_data.
		static co_routine<invite_data> getInviteAsync(get_invite_data dataPackage);

		/// @brief Deletes an invite from the discord servers
		/// @param dataPackage a delete_invite_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> deleteInviteAsync(delete_invite_data dataPackage);

		/// @brief Collects a list of guilds that the bot is in.
		/// @param dataPackage a get_current_user_guilds_data structure.
		/// @return A co_routine containing a guild_vector.
		static co_routine<jsonifier::vector<guild_data>> getCurrentUserGuildsAsync(get_current_user_guilds_data dataPackage);

		/// @brief Removes the bot from a chosen guild.
		/// @param dataPackage a leave_guild_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> leaveGuildAsync(leave_guild_data dataPackage);

		template<typename guild_type> DCA_INLINE static void insertGuild(guild_type&& guild) {
			if (doWeCacheGuildsBool) {
				if (guild.id == 0) {
					throw dca_exception{ "Sorry, but there was no id set for that guild." };
				}
				cache.emplace(std::forward<guild_type>(guild));
				if (cache.count() % 1000 == 0) {
					std::cout << "guild count: " << cache.count() << ", after: " << stopWatchNew.totalTimeElapsed().count() << std::endl;
				}
			}
		}

		static object_cache<guild_cache_data>& getCache();

		static void removeGuild(const guild_cache_data& guild_id);

		static bool doWeCacheGuilds();

	  protected:
		static discord_core_internal::https_client* httpsClient;
		static object_cache<guild_cache_data> cache;
		static bool doWeCacheGuildsBool;
	};
	/**@}*/
}
