/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

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

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For geting a Guild's audit logs. \brief For geting a Guild's audit logs.
	struct DiscordCoreAPI_Dll GetGuildAuditLogsData {
		AuditLogEvent actionType{};///< The action type to acquire audit-logs for.
		uint64_t guildId{};///< The guiild id for the Guild which you wish to query the log of.
		uint64_t before{};///< Entries that preceded a specific audit log entry ID.
		uint64_t userId{};///< The User for whom to look for the actions of.
		int32_t limit{ 0 };///< The maximum number of actions to acquire from the log.
	};

	/// For creating a Guild. \brief For creating a Guild.
	struct DiscordCoreAPI_Dll CreateGuildData {
		AfkTimeOutDurations afkTimeout{ AfkTimeOutDurations::Shortest };///< Afk timeout in seconds.
		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default message notification level.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filter level.
		std::vector<ChannelData> channels{};///< Array of partial Channel objects.
		int32_t systemChannelFlags{ 0 };///< System Channel flags.
		int32_t verificationLevel{ 0 };///< Verification level.
		std::vector<Role> roles{};///< Array of Role objects.
		std::string systemChannelId{};///< The id of the Channel where Guild notices such as welcome messages and boost events are posted.
		std::vector<uint8_t> icon{};///< base64 128x128 image for the Guild icon.
		std::string afkChannelId{};///< Id for afk Channel.
		std::string region{};///< The region that the servers are in.
		std::string name{};///< The name of the new Guild.
	};

	/// For getting a Guild from the library's cache or a Discord server. \brief For getting a Guild from the library's cache or a Discord server.
	struct DiscordCoreAPI_Dll GetGuildData {
		uint64_t guildId{};///< The id of the Guild to acquire.
	};

	/// For acquiring a Guild preview of a chosen Guild. \brief For acquiring a Guild preview of a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildPreviewData {
		uint64_t guildId{};///< The id of the Guild's preview to acquire.
	};

	///	For deleting a Guild. \brief For deleting a Guild.
	struct DiscordCoreAPI_Dll DeleteGuildData {
		uint64_t guildId{};///< The Guild you would like to delete.
	};

	/// For getting a list of Guild bans. \brief For getting a list of Guild bans.
	struct DiscordCoreAPI_Dll GetGuildBansData {
		uint64_t guildId{};///< The Guild from which to collect the list of bans.
		uint64_t before{};///< Consider only users before given user id.
		uint64_t after{};///< Consider only users after given user id null.
		uint64_t limit{};///< Number of users to return ( up to maximum 1000 ).
	};

	/// For getting a single Guild Ban. \brief For getting a single Guild Ban.
	struct DiscordCoreAPI_Dll GetGuildBanData {
		uint64_t guildId{};///< The Guild from which to collect the Ban from.
		uint64_t userId{};///< The User for whom to collect the Ban of.
	};

	/// For banning a current GuildMember. \brief For banning a current GuildMember.
	struct DiscordCoreAPI_Dll CreateGuildBanData {
		int32_t deleteMessageDays{ 0 };///< The number of days of their Messages to delete.
		uint64_t guildMemberId{};///< The id of the member to be banned.
		uint64_t guildId{};///< The id of the Guild from which to ban the member.
		std::string reason{};///< The reason for the ban.
	};

	/// For removing a previous created Ban. \brief For removing a previous created Ban.
	struct DiscordCoreAPI_Dll RemoveGuildBanData {
		uint64_t guildId{};///< The Guild from which to remove the Ban.
		uint64_t userId{};///< The user Id of the user who's ban to remove.
		std::string reason{};///< The reason for removing this Ban.
	};

	/// For collecting the Guild prune count. \brief For collecting the Guild prune count.
	struct DiscordCoreAPI_Dll GetGuildPruneCountData {
		std::vector<uint64_t> includeRoles{};///< Roles to be included in the prune.
		uint64_t guildId{};///< The Guild to be pruned.
		int32_t days{ 0 };///< The number of days beyond which to prune the user's for inactivity.
	};

	/// For pruning a number of GuildMembers from the Guild. \brief For pruning a number of GuildMembers from the Guild.
	struct DiscordCoreAPI_Dll BeginGuildPruneData {
		std::vector<uint64_t> includeRoles{};/// Roles to be included in the prune.
		bool computePruneCount{ false };/// Whether 'pruned' is returned, discouraged for large guilds.
		std::string reason{};///< Reason for pruning the GuildMembers.
		uint64_t guildId{};///< Guild within which to perform the prune.
		int32_t days{ 0 };/// Number of days after which to prune a given GuildMember.
	};

	/// For collecting a list of Guild voice regions. \brief For collecting a list of Guild voice regions.
	struct DiscordCoreAPI_Dll GetGuildVoiceRegionsData {
		uint64_t guildId{};///< The Guild for which to collect the voice regions from.
	};

	/// For geting all of the current invites from a Guild. \brief For geting all of the current invites from a Guild.
	struct DiscordCoreAPI_Dll GetGuildInvitesData {
		uint64_t guildId{};///< The id of the Guild you wish to acquire.
	};

	/// For collecting a list of Guild voice integrations. \brief
	struct DiscordCoreAPI_Dll GetGuildIntegrationsData {
		uint64_t guildId{};///< The Guild for which to collect the integrations from.
	};

	/// For deleting a Guild integration. \brief For deleting a Guild integration.
	struct DiscordCoreAPI_Dll DeleteGuildIntegrationData {
		uint64_t integrationId{};///< The integration's id which we are going to delete.
		std::string reason{};///< Reason for deleting the integration.
		uint64_t guildId{};///< The Guild from which to delete the integration from.
	};

	/// For collecting a Guild's widget settings. \brief For collecting a Guild's widget settings.
	struct DiscordCoreAPI_Dll GetGuildWidgetSettingsData {
		uint64_t guildId{};///< The Guild from which to collect the widget from.
	};

	/// For modifying a Guild's widget. \brief For modifying a Guild's widget.
	struct DiscordCoreAPI_Dll ModifyGuildWidgetData {
		GuildWidgetData widgetData{};///< The new Guild widget responseData.
		std::string reason{};///< Reason for modifying the widget.
		uint64_t guildId{};///< The Guild for which to modify the widget of.
	};

	/// For collecting a Guild's widget. \brief For collecting a Guild's widget.
	struct DiscordCoreAPI_Dll GetGuildWidgetData {
		uint64_t guildId{};///< The Guild from which to collect the widget from.
	};

	/// For geting the vanity invite responseData of a Guild. \brief For geting the vanity invite responseData of a Guild.
	struct DiscordCoreAPI_Dll GetGuildVanityInviteData {
		uint64_t guildId{};///< The id of the Guild to acquire the vanity invite from.
	};

	/// For collecting a Guild's widget image. \brief For collecting a Guild's widget image.
	struct DiscordCoreAPI_Dll GetGuildWidgetImageData {
		WidgetStyleOptions widgetStlye{};///< The style of widget image to collect.
		uint64_t guildId{};///< The Guild for which to collect the widget image from.
	};

	/// For collecting a Guild's welcome screen. \brief For collecting a Guild's welcome screen.
	struct DiscordCoreAPI_Dll GetGuildWelcomeScreenData {
		uint64_t guildId{};///< The Guild for which to collect the widget image from.
	};

	/// For modifying a Guild's welcome screen. \brief For modifying a Guild's welcome screen.
	struct DiscordCoreAPI_Dll ModifyGuildWelcomeScreenData {
		std::vector<WelcomeScreenChannelData> welcomeChannels{};///< Welcome channels for the welcome screen.
		std::string description{};///< The description of the welcome screen.
		bool enabled{ false };///< Is it enabled?
		std::string reason{};///< The reason for modifying the welcome screen.
		uint64_t guildId{};///< The Guild for which to modify the welcome screen of.
	};

	/// For collecting a Guild's template. \brief For collecting a Guild's template.
	struct DiscordCoreAPI_Dll GetGuildTemplateData {
		std::string templateCode{};///< Code for the desired Template.
	};

	/// For creating a Guild from a Guild template. \brief For creating a Guild from a Guild template.
	struct DiscordCoreAPI_Dll CreateGuildFromGuildTemplateData {
		std::vector<uint8_t> imageData{};///< base64 128x128 image for the Guild icon.
		std::string templateCode{};///< Code for the desired Template to use.
		std::string name{};///< Desired name of the Guild.
	};

	/// For collecting a list of Guild Templates from a chosen Guild. \brief For collecting a list of Guild Templates from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildTemplatesData {
		uint64_t guildId{};///< Guild from which you would like to collect the Templates from.
	};

	/// For creating a Guild Template. \brief For creating a Guild Template.
	struct DiscordCoreAPI_Dll CreateGuildTemplateData {
		std::string description{};///< Description for the template (0 - 120 characters).
		uint64_t guildId{};///< Guild within which you wuold like to create the template.
		std::string name{};///< Name of the template (1 - 100 characters).
	};

	/// For syncing a Guild Template. \brief For syncing a Guild Template.
	struct DiscordCoreAPI_Dll SyncGuildTemplateData {
		std::string templateCode{};///< Template code for which template you would like to sync.
		uint64_t guildId{};///< Guild for which you would like to sync the template of.
	};

	/// For modifying a Guild Template. \brief For modifying a Guild Template.
	struct DiscordCoreAPI_Dll ModifyGuildTemplateData {
		std::string templateCode{};/// Template which you would like to modify.
		std::string description{};///< Description for the template (0 - 120 characters).
		uint64_t guildId{};///< Guild within which you would like to modify the Template.
		std::string name{};///< Name of the template (1 - 100 characters).
	};

	/// For deleting a Guild Template. \brief For deleting a Guild Template.
	struct DiscordCoreAPI_Dll DeleteGuildTemplateData {
		std::string templateCode{};///< The template which you would like to delete.
		uint64_t guildId{};///< The Guild within which you would like to delete the Template.
	};

	/// For geting a single invite's responseData from a Guild. \brief For geting a single invite's responseData from a Guild.
	struct DiscordCoreAPI_Dll GetInviteData {
		uint64_t guildScheduledEventId{};///< The Guild scheduled event to include with the invite.
		bool withExpiration{ false };///< Collect expiration time/date?
		bool withCount{ false };///< Collect usage etc counts?
		uint64_t inviteId{};///< The id of the invite you wish to acquire.
	};

	/// For deleting a single Guild Invite. \brief For deleting a single Guild Invite.
	struct DiscordCoreAPI_Dll DeleteInviteData {
		std::string reason{};///< Reason for deleting the Invite.
		uint64_t inviteId{};///< The Invite which you would like to delete.
	};

	/// For collecting a list of Guild's that the Bot is in. \brief For collecting a list of Guild's that the Bot is in.
	struct DiscordCoreAPI_Dll GetCurrentUserGuildsData {
		uint64_t before{};///< Get guilds before this Guild ID.
		uint32_t limit{ 0 };///< Max number of guilds to return (1 - 200).
		uint64_t after{};///< Get guilds after this Guild ID.
	};

	/// For leaving a particular Guild. \brief For leaving a particular Guild.
	struct DiscordCoreAPI_Dll LeaveGuildData {
		uint64_t guildId{};///< The id of the Guild you would like the bot to leave.
	};

	/// A discord Guild. Used to connect to/disconnect from voice. \brief A discord Guild. Used to connect to/disconnect from voice.
	class DiscordCoreAPI_Dll Guild : public GuildData {
	  public:
		friend Guilds;

		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default Message notification level.
		GuildNSFWLevel nsfwLevel{ GuildNSFWLevel::Default };///< NSFW warning level.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filtering level, by default.
		SystemChannelFlags systemChannelFlags{};///< System Channel flags.
		int32_t premiumSubscriptionCount{ 0 };///< Premium subscription count.
		int32_t approximatePresenceCount{ 0 };///< Approximate quantity of presences.
		VerificationLevel verificationLevel{};///< Verification level required.
		std::string publicUpdatesChannelId{};///< Id of the public updates Channel.
		int32_t approximateMemberCount{ 0 };///< Approximate member count.
		WelcomeScreenData welcomeScreen{};///< Welcome screen for the Guild.
		int32_t maxVideoChannelUsers{ 0 };///< Maximum quantity of users per video Channel.
		AfkTimeOutDurations afkTimeOut{};///< Time for an individual to time out as afk.
		std::string discoverySplash{};///< Link to the discovery image's splash.
		std::string preferredLocale{};///< Preferred locale, for voice chat servers.
		std::string widgetChannelId{};///< Channel id for the Guild's widget.
		std::string systemChannelId{};///< Channel id for the Guild's system Channel.
		std::string rulesChannelId{};///< Channel id for the Guild's rules Channel.
		std::string vanityUrlCode{};///< Vanity Url code, if applicable.
		uint64_t applicationId{};///< The current application id.
		std::string afkChannelId{};///< Channel if of the "afk" Channel.
		std::string description{};///< Description of the Guild.
		Permissions permissions{};///< Current Permissions for the bot in the Guild.
		PremiumTier premiumTier{};///< What is the premium tier?
		int32_t maxPresences{ 0 };///< Max number of presences allowed.
		int32_t maxMembers{ 0 };///< Max quantity of members.
		std::string iconHash{};///< Url to the Guild's icon.
		std::string region{};///< Region of the world where the Guild's servers are.
		std::string splash{};///< Url to the Guild's splash.
		std::string banner{};///< Url to the Guild's banner.
		MFALevel mfaLevel{};///< MFA level.

		Guild() = default;

		Guild& operator=(GuildData&&);

		Guild(GuildData&&);

		Guild& operator=(GuildData&);

		Guild(GuildData&);

		Guild& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		Guild(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~Guild() = default;

		inline void parseObject(const nlohmann::json& jsonObjectData, Guild* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
				pDataStructure->afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
			}

			if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
				std::string iconUrlString = "https://cdn.discordapp.com/";
				iconUrlString += "icons/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["icon"].get<std::string>() + ".png";
				pDataStructure->icon = iconUrlString;
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("icon_hash") && !jsonObjectData["icon_hash"].is_null()) {
				pDataStructure->iconHash = jsonObjectData["icon_hash"].get<std::string>();
			}

			if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
				std::string iconUrlString = "https://cdn.discordapp.com/";
				iconUrlString += "splashes/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["splash"].get<std::string>() + ".png";
				pDataStructure->splash = iconUrlString;
			}

			if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
				std::string discordSplashUrlString = "https://cdn.discordapp.com/";
				discordSplashUrlString += "discovery-splashes/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["discovery_splash"].get<std::string>() + ".png";
				pDataStructure->discoverySplash = discordSplashUrlString;
			}

			if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
				pDataStructure->flags =
					DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Owner, jsonObjectData["owner"].get<bool>());
			}

			if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
				pDataStructure->ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
			}

			if (jsonObjectData.contains("preferred_locale") && !jsonObjectData["preferred_locale"].is_null()) {
				pDataStructure->preferredLocale = jsonObjectData["preferred_locale"].get<std::string>();
			}

			if (jsonObjectData.contains("public_updates_channel_id") && !jsonObjectData["public_updates_channel_id"].is_null()) {
				pDataStructure->publicUpdatesChannelId = jsonObjectData["public_updates_channel_id"].get<std::string>();
			}

			if (jsonObjectData.contains("vanity_url_code") && !jsonObjectData["vanity_url_code"].is_null()) {
				pDataStructure->vanityUrlCode = jsonObjectData["vanity_url_code"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("banner") && !jsonObjectData["banner"].is_null()) {
				std::string guildBannerUrl = "https://cdn.discordapp.com/";
				guildBannerUrl += "banners/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["banner"].get<std::string>() + ".png";
				pDataStructure->banner = guildBannerUrl;
			}

			if (jsonObjectData.contains("rule_Channel_id") && !jsonObjectData["rule_Channel_id"].is_null()) {
				pDataStructure->rulesChannelId = jsonObjectData["rule_Channel_id"].get<std::string>();
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
				pDataStructure->joinedAt = jsonObjectData["joined_at"].get<std::string>();
			}

			if (jsonObjectData.contains("widget_channel_id") && !jsonObjectData["widget_channel_id"].is_null()) {
				pDataStructure->widgetChannelId = jsonObjectData["widget_channel_id"].get<std::string>();
			}

			if (jsonObjectData.contains("system_channel_id") && !jsonObjectData["system_channel_id"].is_null()) {
				pDataStructure->systemChannelId = jsonObjectData["system_channel_id"].get<std::string>();
			}

			if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
				pDataStructure->region = jsonObjectData["region"].get<std::string>();
			}

			if (jsonObjectData.contains("afk_channel_id") && !jsonObjectData["afk_channel_id"].is_null()) {
				pDataStructure->afkChannelId = jsonObjectData["afk_channel_id"].get<std::string>();
			}

			if (jsonObjectData.contains("region") && !jsonObjectData["region"].is_null()) {
				pDataStructure->region = jsonObjectData["region"].get<std::string>();
			}

			if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
				for (auto& value: jsonObjectData["features"].get<std::vector<std::string>>()) {
					pDataStructure->features.push_back(value);
				}
			}

			if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
				pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
			}

			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				for (auto& value: jsonObjectData["roles"]) {
					DiscordCoreAPI::RoleData newData{ value };
					pDataStructure->roles.push_back(newData.id);
					DiscordCoreAPI::Roles::insertRole(newData);
				}
			}

			if (jsonObjectData.contains("afk_timeout") && !jsonObjectData["afk_timeout"].is_null()) {
				pDataStructure->afkTimeOut = jsonObjectData["afk_timeout"].get<DiscordCoreAPI::AfkTimeOutDurations>();
			}

			if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
				pDataStructure->flags =
					DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Owner, jsonObjectData["owner"].get<bool>());
			}

			if (jsonObjectData.contains("widget_enabled") && !jsonObjectData["widget_enabled"].is_null()) {
				pDataStructure->flags = DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::WidgetEnabled,
					jsonObjectData["widget_enabled"].get<bool>());
			}

			if (jsonObjectData.contains("verification_level") && !jsonObjectData["verification_level"].is_null()) {
				pDataStructure->verificationLevel = jsonObjectData["verification_level"].get<DiscordCoreAPI::VerificationLevel>();
			}

			if (jsonObjectData.contains("default_message_notification_level") && !jsonObjectData["default_message_notification_level"].is_null()) {
				pDataStructure->defaultMessageNotifications = jsonObjectData["default_message_notification_level"].get<DiscordCoreAPI::DefaultMessageNotificationLevel>();
			}

			if (jsonObjectData.contains("explicit_content_filter_level") && !jsonObjectData["explicit_content_filter_level"].is_null()) {
				pDataStructure->explicitContentFilter = jsonObjectData["explicit_content_filter_level"].get<DiscordCoreAPI::ExplicitContentFilterLevel>();
			}

			if (jsonObjectData.contains("mfa_level") && !jsonObjectData["mfa_level"].is_null()) {
				pDataStructure->mfaLevel = jsonObjectData["mfa_level"].get<DiscordCoreAPI::MFALevel>();
			}

			if (jsonObjectData.contains("system_channel_flags") && !jsonObjectData["system_channel_flags"].is_null()) {
				pDataStructure->systemChannelFlags = jsonObjectData["system_channel_flags"].get<DiscordCoreAPI::SystemChannelFlags>();
			}

			if (jsonObjectData.contains("large") && !jsonObjectData["large"].is_null()) {
				pDataStructure->flags =
					DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Large, jsonObjectData["large"].get<bool>());
			}

			if (jsonObjectData.contains("unavailable") && !jsonObjectData["unavailable"].is_null()) {
				pDataStructure->flags = DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::GuildFlags>(pDataStructure->flags, DiscordCoreAPI::GuildFlags::Unavailable,
					jsonObjectData["unavailable"].get<bool>());
			}

			if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
				pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
				pDataStructure->voiceStates.clear();
				for (auto& value: jsonObjectData["voice_states"]) {
					DiscordCoreAPI::VoiceStateData newData{ value };
					uint64_t userId = newData.userId;
					pDataStructure->voiceStates[userId] = newData;
				}
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				for (auto& value: jsonObjectData["members"]) {
					DiscordCoreAPI::GuildMemberData newData{ value };
					newData.guildId = pDataStructure->id;
					pDataStructure->members.push_back(newData.id);
					DiscordCoreAPI::GuildMembers::insertGuildMember(newData);
				}
			}

			if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
				for (auto& value: jsonObjectData["channels"]) {
					DiscordCoreAPI::ChannelData newData{ value };
					newData.guildId = pDataStructure->id;
					pDataStructure->channels.push_back(newData.id);
					DiscordCoreAPI::Channels::insertChannel(newData);
				}
			}

			if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
				pDataStructure->presences.clear();
				for (auto& value: jsonObjectData["presences"]) {
					DiscordCoreAPI::PresenceUpdateData newData{ value };
					uint64_t presenceId = newData.user.id;
					pDataStructure->presences[presenceId] = newData;
				}
			}

			if (jsonObjectData.contains("max_presences") && !jsonObjectData["max_presences"].is_null()) {
				pDataStructure->maxPresences = jsonObjectData["max_presences"].get<int32_t>();
			}

			if (jsonObjectData.contains("max_members") && !jsonObjectData["max_members"].is_null()) {
				pDataStructure->maxMembers = jsonObjectData["max_members"].get<int32_t>();
			}

			if (jsonObjectData.contains("premium_subscription_count") && !jsonObjectData["premium_subscription_count"].is_null()) {
				pDataStructure->premiumSubscriptionCount = jsonObjectData["premium_subscription_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("premium_tier") && !jsonObjectData["premium_tier"].is_null()) {
				pDataStructure->premiumTier = jsonObjectData["premium_tier"].get<DiscordCoreAPI::PremiumTier>();
			}

			if (jsonObjectData.contains("max_video_channel_users") && !jsonObjectData["max_video_channel_users"].is_null()) {
				pDataStructure->maxVideoChannelUsers = jsonObjectData["max_video_channel_users"].get<int32_t>();
			}

			if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
				pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
				pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("welcome_screen") && !jsonObjectData["welcome_screen"].is_null()) {
				pDataStructure->welcomeScreen = jsonObjectData["welcome_screen"];
			}

			if (jsonObjectData.contains("nsfw_level") && !jsonObjectData["nsfw_level"].is_null()) {
				pDataStructure->nsfwLevel = jsonObjectData["nsfw_level"].get<DiscordCoreAPI::GuildNSFWLevel>();
			}
		}
	};

	class DiscordCoreAPI_Dll GuildVector {
	  public:
		friend class Guilds;

		GuildVector() = default;

		operator std::vector<Guild>() {
			return this->theGuilds;
		}

		GuildVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildVector() = default;

	  protected:
		std::vector<Guild> theGuilds{};

		inline void parseObject(const nlohmann::json& jsonObjectData, GuildVector* pDataStructure) {
			pDataStructure->theGuilds.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::Guild newData{ value };
				pDataStructure->theGuilds.push_back(newData);
			}
			pDataStructure->theGuilds.shrink_to_fit();
		}
	};

	/// For modifying the properties of a chosen Guild. \brief For modifying the properties of a chosen Guild.
	struct DiscordCoreAPI_Dll ModifyGuildData {
		ModifyGuildData(Guild dataPackage) {
			this->premiumProgressBarEnabled = getBool<int8_t, GuildFlags>(dataPackage.flags, GuildFlags::Premium_Progress_Bar_Enabled);
			this->defaultMessageNotifications = dataPackage.defaultMessageNotifications;
			this->publicUpdatesChannelId = dataPackage.publicUpdatesChannelId;
			this->explicitContentFilter = dataPackage.explicitContentFilter;
			this->systemChannelFlags = dataPackage.systemChannelFlags;
			this->verificationLevel = dataPackage.verificationLevel;
			this->preferredLocale = dataPackage.preferredLocale;
			this->systemChannelId = dataPackage.systemChannelId;
			this->rulesChannelId = dataPackage.rulesChannelId;
			this->afkChannelId = dataPackage.afkChannelId;
			this->description = dataPackage.description;
			this->afkTimeout = dataPackage.afkTimeOut;
			for (auto& value: dataPackage.features) {
				this->features.push_back(value);
			}
			this->ownerId = dataPackage.ownerId;
			this->guildId = dataPackage.id;
			this->name = dataPackage.name;
		}
		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default message notification level.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filter level.
		SystemChannelFlags systemChannelFlags{ 0 };///< System Channel flags.
		bool premiumProgressBarEnabled{ false };///< Whether or not the progress bar is enabled.
		std::vector<uint8_t> discoverySplash{};/// Base64 16 : 9 png / jpeg image for the Guild discovery splash(when the server has the DISCOVERABLE feature).
		VerificationLevel verificationLevel{};///< Verification level.
		std::string publicUpdatesChannelId{};///< The id of the Channel where admins and moderators of Community guilds receive notices from Discord.
		std::vector<std::string> features{};///< Array of Guild feature strings enabled Guild features.
		AfkTimeOutDurations afkTimeout{};///< Afk timeout in seconds.
		std::vector<uint8_t> banner{};///< Base64 16 : 9 png / jpeg image for the Guild banner (when the server has the BANNER feature).
		std::vector<uint8_t> splash{};///< Base64 16 : 9 png / jpeg image for the Guild splash (when the server has the INVITE_SPLASH feature).
		std::string preferredLocale{};///< The preferred locale of a Community Guild used in server discovery and notices from Discord; defaults to "en-US".
		std::string systemChannelId{};///< The id of the Channel where Guild notices such as welcome messages and boost events are posted.
		std::string rulesChannelId{};///< The id of the Channel where Community guilds display rules and /or guidelines.
		std::vector<uint8_t> icon{};///< Base64 1024x1024 png / jpeg / gif image for the Guild icon (can be animated gif when the server has the ANIMATED_ICON).
		std::string afkChannelId{};///< Id for afk channels.
		std::string description{};///< The description for the Guild, if the Guild is discoverable.
		std::string ownerId{};///< User id to transfer Guild ownership to (must be owner).
		uint64_t guildId{};///< Id of the chosen Guild to modify.
		std::string reason{};///< Reason for modifying the Guild.
		std::string name{};///< Desired name of the Guild.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// An interface class DiscordCoreAPI_Dll for the Guild related Discord endpoints. \brief An interface class DiscordCoreAPI_Dll for the Guild related Discord endpoints.
	class DiscordCoreAPI_Dll Guilds {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend DiscordCoreClient;
		friend EventHandler;

		static void initialize(DiscordCoreInternal::HttpsClient* theClient, DiscordCoreClient* discordCoreClientNew, ConfigManager* configManager);

		/// Gets an audit log from the Discord servers. \brief Gets an audit log from the Discord servers.
		/// \param dataPackage A GetGuildAuditLogsData structure.
		/// \returns A CoRoutine containing an AuditLogData.
		static CoRoutine<AuditLogData> getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage);

		/// Creates a new Guild. \brief Creates a new Guild.
		/// \param dataPackage A CreateGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> createGuildAsync(CreateGuildData dataPackage);

		/// Returns all of the Guilds that the current bot is in. \brief Returns all of the Guilds that the current bot is in.
		/// \returns A CoRoutine containing a GuildDataVector.
		static CoRoutine<std::vector<GuildData>> getAllGuildsAsync();

		/// Collects a Guild from the Discord servers. \brief Collects a Guild from the Discord servers.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> getGuildAsync(GetGuildData dataPackage);

		/// Collects a Guild from the library's cache. \brief Collects a Guild from the library's cache.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<GuildData> getCachedGuildAsync(GetGuildData dataPackage);

		/// Acquires the preview Data of a chosen Guild. \brief Acquires the preview Data of a chosen Guild.
		/// \param dataPackage A GetGuildPreviewData structure.
		/// \returns A CoRoutine containing a GuildPreviewData.
		static CoRoutine<GuildPreviewData> getGuildPreviewAsync(GetGuildPreviewData dataPackage);

		/// Modifies a chosen Guild's properties. \brief Modifies a chosen Guild's properties.
		/// \param dataPackage A ModifyGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> modifyGuildAsync(ModifyGuildData dataPackage);

		/// Deletes a chosen Guild. \brief Deletes a chosen Guild.
		/// \param dataPackage A DeleteGuildData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildAsync(DeleteGuildData dataPackage);

		/// Collects a list of Bans from a chosen Guild. \brief Collects a list of Bans from a chosen Guild.
		/// \param dataPackage A GetGuildBansData structure.
		/// \returns A CoRoutine containing a BanDataVector.
		static CoRoutine<std::vector<BanData>> getGuildBansAsync(GetGuildBansData dataPackage);

		/// Collects a single Ban from a chosen Guild. \brief Collects a single Ban from a chosen Guild.
		/// \param dataPackage A GetGuildBanData structure.
		/// \returns A CoRoutine containing a BanData.
		static CoRoutine<BanData> getGuildBanAsync(GetGuildBanData dataPackage);

		/// Bans a GuildMember. \brief Bans a GuildMember.
		/// \param dataPackage A CreateGuildBanData structure.
		/// \returns A CoRoutine containing a BanData.
		static CoRoutine<void> createGuildBanAsync(CreateGuildBanData dataPackage);

		/// Removes a previously created ban. \brief Removes a previously created ban.
		/// \param dataPackage A RemoveGuildBanData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeGuildBanAsync(RemoveGuildBanData dataPackage);

		/// For collecting the Guild prune count. \brief For collecting the Guild prune count.
		/// \param dataPackage A GetGuildPruneCountData structure.
		/// \returns A CoRoutine containing GuildPruneCountData.
		static CoRoutine<GuildPruneCountData> getGuildPruneCountAsync(GetGuildPruneCountData dataPackage);

		/// For performing a pruning of the GuildMembers of the Guild, based on days of inactivity. \brief For performing a pruning of the GuildMembers of the Guild, based on days of inactivity.
		/// \param dataPackage A BeginGuildPruneData structure.
		/// \returns A CoRoutine containing a GuildPruneCountData.
		static CoRoutine<GuildPruneCountData> beginGuildPruneAsync(BeginGuildPruneData dataPackage);

		/// Gets the list of voice regions for a particular server. \brief Gets the list of voice regions for a particular server.
		/// \param dataPackage A GetGuildVoiceRegionsData structure.
		/// \returns A CoRoutine containing a VoiceRegionDataVector.
		static CoRoutine<std::vector<VoiceRegionData>> getGuildVoiceRegionsAsync(GetGuildVoiceRegionsData dataPackage);

		/// Gets multiple invites from the Discord servers. \brief Gets multiple invites from the Discord servers.
		/// \param dataPackage A GetGuildInvitesData structure.
		/// \returns A CoRoutine containing a InviteDataVector.
		static CoRoutine<std::vector<InviteData>> getGuildInvitesAsync(GetGuildInvitesData dataPackage);

		/// Gets the list of Guild integrations for a particular server. \brief Gets the list of Guild integrations for a particular server.
		/// \param dataPackage A GetGuildIntegrationsData structure.
		/// \returns A CoRoutine containing a IntegrationDataVector.
		static CoRoutine<std::vector<IntegrationData>> getGuildIntegrationsAsync(GetGuildIntegrationsData dataPackage);

		/// Deletes an integration from a Guild. \brief Deletes an integration from a Guild.
		/// \param dataPackage A DeleteGuildIntegrationData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildIntegrationAsync(DeleteGuildIntegrationData dataPackage);

		/// Gets the Guild widget's settings for a particular server. \brief Gets the Guild widget's settings for a particular server.
		/// \param dataPackage A GetGuildWidgetSettingsData structure.
		/// \returns A CoRoutine containing a GuildWidgetData.
		static CoRoutine<GuildWidgetData> getGuildWidgetSettingsAsync(GetGuildWidgetSettingsData dataPackage);

		/// Modifies the Guild widget for a particular server. \brief Modifies the Guild widget for a particular server.
		/// \param dataPackage A ModifyGuildWidgetData structure.
		/// \returns A CoRoutine containing a GuildWidgetData.
		static CoRoutine<GuildWidgetData> modifyGuildWidgetAsync(ModifyGuildWidgetData dataPackage);

		/// Gets the Guild widget for a particular server. \brief Gets the Guild widget for a particular server.
		/// \param dataPackage A GetGuildWidgetData structure.
		/// \returns A CoRoutine containing a GuildWidgetData.
		static CoRoutine<GuildWidgetData> getGuildWidgetAsync(GetGuildWidgetData dataPackage);

		/// Gets the vanity invite responseData from a particular server. \brief Gets the vanity invite responseData from a particular server.
		/// \param dataPackage A GetGuildVanityInviteData structure.
		/// \returns A CoRoutine containing InviteData.
		static CoRoutine<InviteData> getGuildVanityInviteAsync(GetGuildVanityInviteData dataPackage);

		/// Gets the Guild widget image for a particular server. \brief Gets the Guild widget image for a particular server.
		/// \param dataPackage A GetGuildWidgetImageData structure.
		/// \returns A CoRoutine containing a GuildWidgetImageData.
		static CoRoutine<GuildWidgetImageData> getGuildWidgetImageAsync(GetGuildWidgetImageData dataPackage);

		/// Gets the Guild welcome screen for a particular server. \brief Gets the Guild welcome screen for a particular server.
		/// \param dataPackage A GetGuildWelcomeScreenData structure.
		/// \returns A CoRoutine containing a WelcomeScreenData.
		static CoRoutine<WelcomeScreenData> getGuildWelcomeScreenAsync(GetGuildWelcomeScreenData dataPackage);

		/// Modifies the Guild welcome screen. \brief Modifies the Guild welcome screen.
		/// \param dataPackage A ModifyGuildWelcomeScreenData structure.
		/// \returns A CoRoutine containing a WelcomeScreenData.
		static CoRoutine<WelcomeScreenData> modifyGuildWelcomeScreenAsync(ModifyGuildWelcomeScreenData dataPackage);

		/// Gets the Guild Template from a particular server. \brief Gets the Guild Template from a particular server.
		/// \param dataPackage A GetGuildTemplateData structure.
		/// \returns A CoRoutine containing a GuildTemplateData.
		static CoRoutine<GuildTemplateData> getGuildTemplateAsync(GetGuildTemplateData dataPackage);

		/// Creates a Guild from the Guild Template. \brief Creates a Guild from the Guild Template.
		/// \param dataPackage A CreateGuildFromGuildTemplateData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> createGuildFromGuildTemplateAsync(CreateGuildFromGuildTemplateData dataPackage);

		/// Collects a list of Guild Templates from a chosen Guild. \brief Collects a list of Guild Templates from a chosen Guild.
		/// \param dataPackage A GetGuildTemplatesData structure.
		/// \returns A CoRoutine containing a GuiildTemplateDataVector.
		static CoRoutine<std::vector<GuildTemplateData>> getGuildTemplatesAsync(GetGuildTemplatesData dataPackage);

		/// Creates a Guild Template. \brief Creates a Guild Template.
		/// \param dataPackage A CreateGuildTemplateData structure.
		/// \returns A CoRoutine containing a GuiildTemplateData.
		static CoRoutine<GuildTemplateData> createGuildTemplateAsync(CreateGuildTemplateData dataPackage);

		/// Syncs a Guild Template. \brief Syncs a Guild Template.
		/// \param dataPackage A SyncGuildTemplateData structure.
		/// \returns A CoRoutine containing a GuiildTemplateData.
		static CoRoutine<GuildTemplateData> syncGuildTemplateAsync(SyncGuildTemplateData dataPackage);

		/// Modifies a Guild Template. \brief Modifies a Guild Template.
		/// \param dataPackage A ModifyGuildTemplateData structure.
		/// \returns A CoRoutine containing a GuiildTemplateData.
		static CoRoutine<GuildTemplateData> modifyGuildTemplateAsync(ModifyGuildTemplateData dataPackage);

		/// Deletes a Guild Template. \brief Deletes a Guild Template.
		/// \param dataPackage A DeleteGuildTemplateData structure.
		/// \returns A CoRoutine containing a void.
		static CoRoutine<void> deleteGuildTemplateAsync(DeleteGuildTemplateData dataPackage);

		/// Gets an invite from the Discord servers. \brief Gets an invite from the Discord servers.
		/// \param dataPackage A GetInviteData structure.
		/// \returns A CoRoutine containing an InviteData.
		static CoRoutine<InviteData> getInviteAsync(GetInviteData dataPackage);

		/// Deletes an invite from the Discord servers. \brief Deletes an invite from the Discord servers
		/// \param dataPackage A DeleteInviteData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInviteAsync(DeleteInviteData dataPackage);

		/// Collects a list of Guilds that the Bot is in. \brief Collects a list of Guilds that the Bot is in.
		/// \param dataPackage A GetCurrentUserGuildsData structure.
		/// \returns A CoRoutine containing a GuildVector.
		static CoRoutine<std::vector<Guild>> getCurrentUserGuildsAsync(GetCurrentUserGuildsData dataPackage);

		/// Removes the bot from a chosen Guild. \brief Removes the bot from a chosen Guild.
		/// \param dataPackage A LeaveGuildData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> leaveGuildAsync(LeaveGuildData dataPackage);

	  protected:
		static std::unique_ptr<std::unordered_map<uint64_t, std::unique_ptr<GuildData>>> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static DiscordCoreClient* discordCoreClient;
		static ConfigManager* configManager;
		static std::shared_mutex theMutex;

		static void insertGuild(GuildData guild);

		static void removeGuild(const uint64_t& GuildId);
	};
	/**@}*/
}// namespace DiscordCoreAPI
