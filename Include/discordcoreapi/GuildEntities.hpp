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
/// GuildEntities.hpp - Header for the Guild classes and structs.
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

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For geting a Guild's audit logs.
	struct GetGuildAuditLogsData {
		AuditLogEvent actionType{};///< The action type to acquire audit-logs for.
		Snowflake guildId{};///< The guiild id for the Guild which you wish to query the log of.
		Snowflake before{};///< Entries that preceded a specific audit log entry ID.
		Snowflake userId{};///< The UserData for whom to look for the actions of.
		int32_t limit{};///< The maximum number of actions to acquire from the log.
	};

	/// @brief For creating a Guild.
	struct CreateGuildData {
		AfkTimeOutDurations afkTimeout{ AfkTimeOutDurations::Shortest };///< Afk timeout in seconds.
		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default message notification level.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filter level.
		jsonifier::vector<ChannelData> channels{};///< Array of partial ChannelData objects.
		int32_t systemChannelFlags{};///< System ChannelData flags.
		Snowflake systemChannelId{};///< The id of the ChannelData where Guild notices such as welcome messages and boost events are posted.
		int32_t verificationLevel{};///< Verification level.
		Snowflake afkChannelId{};///< Snowflake for afk ChannelData.
		std::string region{};///< The region that the servers are in.
		std::string icon{};///< base64 128x128 image for the Guild icon.
		std::string name{};///< The name of the new Guild.
		jsonifier::vector<RoleData> roles{};///< Array of RoleData objects.
	};

	/// @brief For getting a Guild from the library's cache or a Discord server.
	struct GetGuildData {
		Snowflake guildId{};///< The id of the Guild to acquire.
	};

	/// @brief For acquiring a Guild preview of a chosen Guild.
	struct GetGuildPreviewData {
		Snowflake guildId{};///< The id of the Guild's preview to acquire.
	};

	/// @brief For deleting a Guild.
	struct DeleteGuildData {
		Snowflake guildId{};///< The Guild you would like to delete.
	};

	/// @brief For getting a list of Guild bans.
	struct GetGuildBansData {
		Snowflake guildId{};///< The Guild from which to collect the list of bans.
		Snowflake before{};///< Consider only users before given user id.
		Snowflake after{};///< Consider only users after given user id null.
		uint64_t limit{};///< Number of users to return ( up to maximum 1000 ).
	};

	/// @brief For getting a single Guild Ban.
	struct GetGuildBanData {
		Snowflake guildId{};///< The Guild from which to collect the Ban from.
		Snowflake userId{};///< The UserData for whom to collect the Ban of.
	};

	/// @brief For banning a current GuildMemberData.
	struct CreateGuildBanData {
		int32_t deleteMessageDays{};///< The number of days of their Messages to delete.
		Snowflake guildMemberId{};///< The id of the member to be banned.
		std::string reason{};///< The reason for the ban.
		Snowflake guildId{};///< The id of the Guild from which to ban the member.
	};

	/// @brief For removing a previous created Ban.
	struct RemoveGuildBanData {
		std::string reason{};///< The reason for removing this Ban.
		Snowflake guildId{};///< The Guild from which to erase the Ban.
		Snowflake userId{};///< The user Snowflake of the user who's ban to erase.
	};

	/// @brief For collecting the Guild prune count.
	struct GetGuildPruneCountData {
		jsonifier::vector<Snowflake> includeRoles{};///< Roles to be included in the prune.
		Snowflake guildId{};///< The Guild to be pruned.
		int32_t days{};///< The number of days beyond which to prune the user's for inactivity.
	};

	/// @brief For pruning a number of GuildMembers from the Guild.
	struct BeginGuildPruneData {
		jsonifier::vector<Snowflake> includeRoles{};/// Roles to be included in the prune.
		bool computePruneCount{};/// Whether 'pruned' is returned, discouraged for large guilds.
		std::string reason{};///< Reason for pruning the GuildMembers.
		Snowflake guildId{};///< Guild within which to perform the prune.
		int32_t days{};/// Number of days after which to prune a  given GuildMember.
	};

	/// @brief For collecting a list of Guild voice regions.
	struct GetGuildVoiceRegionsData {
		Snowflake guildId{};///< The Guild for which to collect the voice regions from.
	};

	/// @brief For geting all of the current invites from a Guild.
	struct GetGuildInvitesData {
		Snowflake guildId{};///< The id of the Guild you wish to acquire.
	};

	/// @brief
	struct GetGuildIntegrationsData {
		Snowflake guildId{};///< The Guild for which to collect the integrations from.
	};

	/// @brief For deleting a Guild integration.
	struct DeleteGuildIntegrationData {
		Snowflake integrationId{};///< The integration's id which we are going to delete.
		std::string reason{};///< Reason for deleting the integration.
		Snowflake guildId{};///< The Guild from which to delete the integration from.
	};

	/// @brief For collecting a Guild's widget settings.
	struct GetGuildWidgetSettingsData {
		Snowflake guildId{};///< The Guild from which to collect the widget from.
	};

	/// @brief For modifying a Guild's widget.
	struct ModifyGuildWidgetData {
		GuildWidgetData widgetData{};///< The new Guild widget responseData.
		std::string reason{};///< Reason for modifying the widget.
		Snowflake guildId{};///< The Guild for which to modify the widget of.
	};

	/// @brief For collecting a Guild's widget.
	struct GetGuildWidgetData {
		Snowflake guildId{};///< The Guild from which to collect the widget from.
	};

	/// @brief For geting the vanity invite responseData of a Guild.
	struct GetGuildVanityInviteData {
		Snowflake guildId{};///< The id of the Guild to acquire the vanity invite from.
	};

	/// @brief For collecting a Guild's widget image.
	struct GetGuildWidgetImageData {
		WidgetStyleOptions widgetStlye{};///< The style of widget image to collect.
		Snowflake guildId{};///< The Guild for which to collect the widget image from.
	};

	/// @brief For collecting a Guild's welcome screen.
	struct GetGuildWelcomeScreenData {
		Snowflake guildId{};///< The Guild for which to collect the widget image from.
	};

	/// @brief For modifying a Guild's welcome screen.
	struct ModifyGuildWelcomeScreenData {
		jsonifier::vector<WelcomeScreenChannelData> welcomeChannels{};///< Welcome channels for the welcome screen.
		std::string description{};///< The description of the welcome screen.
		std::string reason{};///< The reason for modifying the welcome screen.
		Snowflake guildId{};///< The Guild for which to modify the welcome screen of.
		bool enabled{};///< Is it enabled?
	};

	/// @brief For collecting a Guild's template.
	struct GetGuildTemplateData {
		std::string templateCode{};///< Code for the desired Template.
	};

	/// @brief For creating a Guild from a Guild template.
	struct CreateGuildFromGuildTemplateData {
		std::string templateCode{};///< Code for the desired Template to use.
		std::string imageData{};///< base64 128x128 image for the Guild icon.
		std::string name{};///< Desired name of the Guild.
	};

	/// @brief For collecting a list of Guild Templates from a chosen Guild.
	struct GetGuildTemplatesData {
		Snowflake guildId{};///< Guild from which you would like to collect the Templates from.
	};

	/// @brief For creating a Guild Template.
	struct CreateGuildTemplateData {
		std::string description{};///< Description for the template (0 - 120 characters).
		Snowflake guildId{};///< Guild within which you wuold like to create the template.
		std::string name{};///< Name of the template (1 - 100 characters).
	};

	/// @brief For syncing a Guild Template.
	struct SyncGuildTemplateData {
		std::string templateCode{};///< Template code for which template you would like to sync.
		Snowflake guildId{};///< Guild for which you would like to sync the template of.
	};

	/// @brief For modifying a Guild Template.
	struct ModifyGuildTemplateData {
		std::string templateCode{};/// Template which you would like to modify.
		std::string description{};///< Description for the template (0 - 120 characters).
		Snowflake guildId{};///< Guild within which you would like to modify the Template.
		std::string name{};///< Name of the template (1 - 100 characters).
	};

	/// @brief For deleting a Guild Template.
	struct DeleteGuildTemplateData {
		std::string templateCode{};///< The template which you would like to delete.
		Snowflake guildId{};///< The Guild within which you would like to delete the Template.
	};

	/// @brief For geting a single invite's responseData from a Guild.
	struct GetInviteData {
		Snowflake guildScheduledEventId{};///< The Guild scheduled event to include with the invite.
		bool withExpiration{};///< Collect expiration time/date?
		Snowflake inviteId{};///< The id of the invite you wish to acquire.
		bool withCount{};///< Collect usage etc counts?
	};

	/// @brief For deleting a single Guild Invite.
	struct DeleteInviteData {
		std::string reason{};///< Reason for deleting the Invite.
		Snowflake inviteId{};///< The Invite which you would like to delete.
	};

	/// @brief For collecting a list of Guild's that the Bot is in.
	struct GetCurrentUserGuildsData {
		Snowflake before{};///< Get guilds before this Guild ID.
		Snowflake after{};///< Get guilds after this Guild ID.
		uint32_t limit{};///< Max number of guilds to return (1 - 200).
	};

	/// @brief For leaving a particular Guild.
	struct LeaveGuildData {
		Snowflake guildId{};///< The id of the Guild you would like the bot to leave.
	};

	/// @brief For modifying the properties of a chosen Guild.
	struct DiscordCoreAPI_Dll ModifyGuildData {
		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default message notification level.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filter level.
		SystemChannelFlags systemChannelFlags{};///< System ChannelData flags.
		VerificationLevel verificationLevel{};///< Verification level.
		jsonifier::vector<std::string> features{};///< Array of Guild feature strings enabled Guild features.
		Snowflake publicUpdatesChannelId{};///< The id of the ChannelData where admins and moderators of Community guilds receive notices from Discord.
		AfkTimeOutDurations afkTimeout{};///< Afk timeout in seconds.
		bool premiumProgressBarEnabled{};///< Whether or not the progress bar is enabled.
		std::string preferredLocale{};///< The preferred locale of a Community Guild used in server discovery.
		std::string discoverySplash{};/// Base64 16 : 9 png / jpeg image for the Guild discovery splash(when the server has the DISCOVERABLE feature).
		Snowflake systemChannelId{};///< The id of the ChannelData where Guild notices such as welcome messages and boost events are posted.
		Snowflake rulesChannelId{};///< The id of the ChannelData where Community guilds display rules and /or guidelines.
		std::string description{};///< The description for the Guild, if the Guild is discoverable.
		Snowflake afkChannelId{};///< Snowflake for afk channels.
		std::string banner{};///< Base64 16 : 9 png / jpeg image for the Guild banner (when the server has the BANNER feature).
		std::string splash{};///< Base64 16 : 9 png / jpeg image for the Guild splash (when the server has the INVITE_SPLASH feature).
		std::string reason{};///< Reason for modifying the Guild.
		Snowflake ownerId{};///< UserData id to transfer Guild ownership to (must be owner).
		Snowflake guildId{};///< Snowflake of the chosen Guild to modify.
		std::string icon{};///< Base64 1024x1024 png / jpeg / gif image for the Guild icon.
		std::string name{};///< Desired name of the Guild.

		ModifyGuildData(const GuildData& dataPackage);
	};

	/**@}*/

	inline StopWatch<Milliseconds> stopWatchNew{ Milliseconds{ 100 } };

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// @brief An interface class for the Guild related Discord endpoints.
	class DiscordCoreAPI_Dll Guilds {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManager);

		/// @brief Gets an audit log from the Discord servers.
		/// @param dataPackage A GetGuildAuditLogsData structure.
		/// @returns A CoRoutine containing an AuditLogData.
		static CoRoutine<AuditLogData> getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage);

		/// @brief Creates a new Guild.
		/// @param dataPackage A CreateGuildData structure.
		/// @returns A CoRoutine containing a Guild.
		static CoRoutine<GuildData> createGuildAsync(CreateGuildData dataPackage);

		/// @brief Returns all of the Guilds that the current bot is in.
		/// @returns A CoRoutine containing a jsonifier::vector<GuildData>.
		static jsonifier::vector<GuildData> getAllGuildsAsync();

		/// @brief Collects a Guild from the Discord servers.
		/// @param dataPackage A GetGuildData structure.
		/// @returns A CoRoutine containing a Guild.
		static CoRoutine<GuildData> getGuildAsync(GetGuildData dataPackage);

		/// @brief Collects a Guild from the library's cache.
		/// @param dataPackage A GetGuildData structure.
		/// @returns A CoRoutine containing a Guild.
		static GuildCacheData getCachedGuild(GetGuildData dataPackage);

		/// @brief Acquires the preview Data of a chosen Guild.
		/// @param dataPackage A GetGuildPreviewData structure.
		/// @returns A CoRoutine containing a GuildPreviewData.
		static CoRoutine<GuildPreviewData> getGuildPreviewAsync(GetGuildPreviewData dataPackage);

		/// @brief Modifies a chosen Guild's properties.
		/// @param dataPackage A ModifyGuildData structure.
		/// @returns A CoRoutine containing a Guild.
		static CoRoutine<GuildData> modifyGuildAsync(ModifyGuildData dataPackage);

		/// @brief Deletes a chosen Guild.
		/// @param dataPackage A DeleteGuildData structure.
		/// @returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildAsync(DeleteGuildData dataPackage);

		/// @brief Collects a list of Bans from a chosen Guild.
		/// @param dataPackage A GetGuildBansData structure.
		/// @returns A CoRoutine containing a BanDataVector.
		static CoRoutine<jsonifier::vector<BanData>> getGuildBansAsync(GetGuildBansData dataPackage);

		/// @brief Collects a single Ban from a chosen Guild.
		/// @param dataPackage A GetGuildBanData structure.
		/// @returns A CoRoutine containing a BanData.
		static CoRoutine<BanData> getGuildBanAsync(GetGuildBanData dataPackage);

		/// @brief Bans a GuildMember.
		/// @param dataPackage A CreateGuildBanData structure.
		/// @returns A CoRoutine containing a BanData.
		static CoRoutine<void> createGuildBanAsync(CreateGuildBanData dataPackage);

		/// @brief Removes a previously created ban.
		/// @param dataPackage A RemoveGuildBanData structure.
		/// @returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildBanAsync(RemoveGuildBanData dataPackage);

		/// @brief For collecting the Guild prune count.
		/// @param dataPackage A GetGuildPruneCountData structure.
		/// @returns A CoRoutine containing GuildPruneCountData.
		static CoRoutine<GuildPruneCountData> getGuildPruneCountAsync(GetGuildPruneCountData dataPackage);

		/// @brief For performing a pruning of the GuildMembers of the Guild, based on days of inactivity.
		/// @param dataPackage A BeginGuildPruneData structure.
		/// @returns A CoRoutine containing a GuildPruneCountData.
		static CoRoutine<GuildPruneCountData> beginGuildPruneAsync(BeginGuildPruneData dataPackage);

		/// @brief Gets the list of voice regions for a particular server.
		/// @param dataPackage A GetGuildVoiceRegionsData structure.
		/// @returns A CoRoutine containing a VoiceRegionDataVector.
		static CoRoutine<jsonifier::vector<VoiceRegionData>> getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage);

		/// @brief Gets multiple invites from the Discord servers.
		/// @param dataPackage A GetGuildInvitesData structure.
		/// @returns A CoRoutine containing a jsonifier::vector<InviteData>.
		static CoRoutine<jsonifier::vector<InviteData>> getGuildInvitesAsync(GetGuildInvitesData dataPackage);

		/// @brief Gets the list of Guild integrations for a particular server.
		/// @param dataPackage A GetGuildIntegrationsData structure.
		/// @returns A CoRoutine containing a jsonifier::vector<IntegrationData>.
		static CoRoutine<jsonifier::vector<IntegrationData>> getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage);

		/// @brief Deletes an integration from a Guild.
		/// @param dataPackage A DeleteGuildIntegrationData structure.
		/// @returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage);

		/// @brief Gets the Guild widget's settings for a particular server.
		/// @param dataPackage A GetGuildWidgetSettingsData structure.
		/// @returns A CoRoutine containing a GuildWidgetData.
		static CoRoutine<GuildWidgetData> getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage);

		/// @brief Modifies the Guild widget for a particular server.
		/// @param dataPackage A ModifyGuildWidgetData structure.
		/// @returns A CoRoutine containing a GuildWidgetData.
		static CoRoutine<GuildWidgetData> modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage);

		/// @brief Gets the Guild widget for a particular server.
		/// @param dataPackage A GetGuildWidgetData structure.
		/// @returns A CoRoutine containing a GuildWidgetData.
		static CoRoutine<GuildWidgetData> getGuildWidgetAsync(GetGuildWidgetData dataPackage);

		/// @brief Gets the vanity invite responseData from a particular server.
		/// @param dataPackage A GetGuildVanityInviteData structure.
		/// @returns A CoRoutine containing InviteData.
		static CoRoutine<InviteData> getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage);

		/// @brief Gets the Guild widget image for a particular server.
		/// @param dataPackage A GetGuildWidgetImageData structure.
		/// @returns A CoRoutine containing a GuildWidgetImageData.
		static CoRoutine<GuildWidgetImageData> getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage);

		/// @brief Gets the Guild welcome screen for a particular server.
		/// @param dataPackage A GetGuildWelcomeScreenData structure.
		/// @returns A CoRoutine containing a WelcomeScreenData.
		static CoRoutine<WelcomeScreenData> getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage);

		/// @brief Modifies the Guild welcome screen.
		/// @param dataPackage A ModifyGuildWelcomeScreenData structure.
		/// @returns A CoRoutine containing a WelcomeScreenData.
		static CoRoutine<WelcomeScreenData> modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage);

		/// @brief Gets the Guild Template from a particular server.
		/// @param dataPackage A GetGuildTemplateData structure.
		/// @returns A CoRoutine containing a GuildTemplateData.
		static CoRoutine<GuildTemplateData> getGuildTemplateAsync(GetGuildTemplateData dataPackage);

		/// @brief Creates a Guild from the Guild Template.
		/// @param dataPackage A CreateGuildFromGuildTemplateData structure.
		/// @returns A CoRoutine containing a Guild.
		static CoRoutine<GuildData> createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage);

		/// @brief Collects a list of Guild Templates from a chosen Guild.
		/// @param dataPackage A GetGuildTemplatesData structure.
		/// @returns A CoRoutine containing a GuiildTemplateDataVector.
		static CoRoutine<jsonifier::vector<GuildTemplateData>> getGuildTemplatesAsync(GetGuildTemplatesData dataPackage);

		/// @brief Creates a Guild Template.
		/// @param dataPackage A CreateGuildTemplateData structure.
		/// @returns A CoRoutine containing a GuiildTemplateData.
		static CoRoutine<GuildTemplateData> createGuildTemplateAsync(CreateGuildTemplateData dataPackage);

		/// @brief Syncs a Guild Template.
		/// @param dataPackage A SyncGuildTemplateData structure.
		/// @returns A CoRoutine containing a GuiildTemplateData.
		static CoRoutine<GuildTemplateData> syncGuildTemplateAsync(SyncGuildTemplateData dataPackage);

		/// @brief Modifies a Guild Template.
		/// @param dataPackage A ModifyGuildTemplateData structure.
		/// @returns A CoRoutine containing a GuiildTemplateData.
		static CoRoutine<GuildTemplateData> modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage);

		/// @brief Deletes a Guild Template.
		/// @param dataPackage A DeleteGuildTemplateData structure.
		/// @returns A CoRoutine containing a void.
		static CoRoutine<void> deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage);

		/// @brief Gets an invite from the Discord servers.
		/// @param dataPackage A GetInviteData structure.
		/// @returns A CoRoutine containing an InviteData.
		static CoRoutine<InviteData> getInviteAsync(GetInviteData dataPackage);

		/// @brief Deletes an invite from the Discord servers
		/// @param dataPackage A DeleteInviteData structure.
		/// @returns A CoRoutine containing void.
		static CoRoutine<void> deleteInviteAsync(DeleteInviteData dataPackage);

		/// @brief Collects a list of Guilds that the Bot is in.
		/// @param dataPackage A GetCurrentUserGuildsData structure.
		/// @returns A CoRoutine containing a GuildVector.
		static CoRoutine<jsonifier::vector<GuildData>> getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage);

		/// @brief Removes the bot from a chosen Guild.
		/// @param dataPackage A LeaveGuildData structure.
		/// @returns A CoRoutine containing void.
		static CoRoutine<void> leaveGuildAsync(LeaveGuildData dataPackage);

		template<typename GuildType> inline static void insertGuild(GuildType&& guild) {
			if (doWeCacheGuildsBool) {
				if (guild.id == 0) {
					throw DCAException{ "Sorry, but there was no id set for that guild." };
				}
				cache.emplace(std::forward<GuildType>(guild));
				if (cache.count() % 1000 == 0) {
					std::cout << "GUILD COUNT: " << cache.count() << ", AFTER: " << stopWatchNew.totalTimeElapsed().count() << std::endl;
				}
			}
		}

		static ObjectCache<GuildCacheData>& getCache();

		static void removeGuild(const GuildCacheData& GuildId);

		static bool doWeCacheGuilds();

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ObjectCache<GuildCacheData> cache;
		static bool doWeCacheGuildsBool;
	};
	/**@}*/
}
