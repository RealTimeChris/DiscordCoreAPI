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
/// InteractionEntities.hpp - Header for the Jsonifier::Core specializations.
/// Mar 24, 2023 Chris M.
/// https://discordcoreapi.com
/// \file JsonSpecializations.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		template<> struct WebSocketMessageData<UpdateVoiceStateData> {
			WebSocketMessageData() = default;
			WebSocketMessageData(const UpdateVoiceStateData& data);
			UnorderedSet<std::string> excludedKeys{};
			using type = UpdateVoiceStateData;
			int64_t op{ -1 };
			std::string t{};
			int32_t s{};
			type d{};
			operator EtfSerializer();
		};

		template<> struct WebSocketMessageData<UpdateVoiceStateDataDC> {
			WebSocketMessageData() = default;
			WebSocketMessageData(const UpdateVoiceStateData& data);
			UnorderedSet<std::string> excludedKeys{};
			using type = UpdateVoiceStateDataDC;
			int64_t op{ -1 };
			std::string t{};
			int32_t s{};
			type d{};
			operator EtfSerializer();
		};
	}
}

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::ReadyData> {
		using ValueType = DiscordCoreAPI::ReadyData;
		static constexpr auto parseValue = object("resume_gateway_url", &ValueType::resumeGatewayUrl, "shard", &ValueType::shard, "v", &ValueType::v,
			"session_id", &ValueType::sessionId, "user", &ValueType::user, "application", &ValueType::application, "_trace", &ValueType::trace);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::WebSocketMessage> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::WebSocketMessage;
		static constexpr auto parseValue = object("op", &ValueType::op, "s", &ValueType::s, "t", &ValueType::t);
	};

	template<typename OTy2> struct Core<DiscordCoreAPI::DiscordCoreInternal::WebSocketMessageData<OTy2>> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::WebSocketMessageData<OTy2>;
		static constexpr auto parseValue = object("d", &ValueType::d, "op", &ValueType::op, "s", &ValueType::s);
	};

}

namespace JsonifierInternal {

	template<> inline void SerializeWithKeys::op<DiscordCoreAPI::Snowflake, Jsonifier::String>(DiscordCoreAPI::Snowflake& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeWithKeys::op(newString, buffer, index);
	}

	template<> inline void SerializeNoKeys::op<DiscordCoreAPI::Snowflake, Jsonifier::String>(DiscordCoreAPI::Snowflake& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeNoKeys::op(newString, buffer, index);
	}

	template<> inline void ParseWithKeys::op<true, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<true>(newString, it);
		value = newString;
	}

	template<> inline void ParseWithKeys::op<false, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<false>(newString, it);
		value = newString;
	}

	template<> inline void ParseNoKeys::op<true, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<true>(newString, it);
		value = newString;
		return;
	}

	template<> inline void ParseNoKeys::op<false, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<false>(newString, it);
		value = newString;
		return;
	}
}

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::ApplicationCommandPermissionData> {
		using ValueType = DiscordCoreAPI::ApplicationCommandPermissionData;
		static constexpr auto parseValue = object("id", &ValueType::id, "permission", &ValueType::permission, "type", &ValueType::type);
	};

	template<> struct Core<DiscordCoreAPI::GuildApplicationCommandPermissionsData> {
		using ValueType = DiscordCoreAPI::GuildApplicationCommandPermissionsData;
		static constexpr auto parseValue = object("application_id", &ValueType::applicationId, "permissions", &ValueType::permissions, "guild_id",
			&ValueType::guildId, "id", &ValueType::id);
	};

	template<> struct Core<DiscordCoreAPI::SessionStartData> {
		using ValueType = DiscordCoreAPI::SessionStartData;
		static constexpr auto parseValue = object("max_concurrency", &ValueType::maxConcurrency, "remaining", &ValueType::remaining, "reset_after",
			&ValueType::resetAfter, "total", &ValueType::total);
	};

	template<> struct Core<DiscordCoreAPI::AuthorizationInfoData> {
		using ValueType = DiscordCoreAPI::AuthorizationInfoData;
		static constexpr auto parseValue =
			object("application", &ValueType::application, "expires", &ValueType::expires, "scopes", &ValueType::scopes, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::TriggerMetaData> {
		using ValueType = DiscordCoreAPI::TriggerMetaData;
		static constexpr auto parseValue = object("allow_list", &ValueType::allowList, "keyword_filter", &ValueType::keywordFilter,
			"mention_total_limit", &ValueType::mentionTotalLimit, "presets", &ValueType::presets);
	};

	template<> struct Core<DiscordCoreAPI::PartialEmojiData> {
		using ValueType = DiscordCoreAPI::PartialEmojiData;
		static constexpr auto parseValue = object("id", &ValueType::id, "name", &ValueType::name, "animated", &ValueType::animated);
	};

	template<> struct Core<DiscordCoreAPI::EmojiData> {
		using ValueType = DiscordCoreAPI::EmojiData;
		static constexpr auto parseValue = object("id", &ValueType::id, "name", &ValueType::name, "roles", &ValueType::roles, "require_colons",
			&ValueType::requireColons, "managed", &ValueType::managed, "animated", &ValueType::animated, "available", &ValueType::available);
	};

	template<> struct Core<DiscordCoreAPI::TeamMembersObjectData> {
		using ValueType = DiscordCoreAPI::TeamMembersObjectData;
		static constexpr auto parseValue = object("membership_state", &ValueType::membershipState, "permissions", &ValueType::permissions, "team_id",
			&ValueType::teamId, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::TeamObjectData> {
		using ValueType = DiscordCoreAPI::TeamObjectData;
		static constexpr auto parseValue =
			object("icon", &ValueType::icon, "id", &ValueType::id, "members", &ValueType::members, "owner_user_id", &ValueType::ownerUserId);
	};

	template<> struct Core<DiscordCoreAPI::InstallParamsData> {
		using ValueType = DiscordCoreAPI::InstallParamsData;
		static constexpr auto parseValue = object("scopes", &ValueType::scopes, "permissions", &ValueType::permissions);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationData> {
		using ValueType = DiscordCoreAPI::ApplicationData;
		static constexpr auto parseValue = object("bot_public", &ValueType::botPublic, "bot_require_code_grant", &ValueType::botRequireCodeGrant,
			"cover_image", &ValueType::coverImage, "custom_install_url", &ValueType::customInstallUrl, "description", &ValueType::description, "",
			"flags", &ValueType::flags, "guild_id", &ValueType::guildId, "icon", &ValueType::icon, "name", &ValueType::name, "owner",
			&ValueType::owner, "params", &ValueType::params, "primary_sku_id", &ValueType::primarySkuId, "privacy_policy_url",
			&ValueType::privacyPolicyUrl, "rpc_origins", &ValueType::rpcOrigins, "slug", &ValueType::slug, "summary", &ValueType::summary, "tags",
			&ValueType::tags, "team", &ValueType::team, "terms_of_service_url", &ValueType::termsOfServiceUrl, "verify_key", &ValueType::verifyKey);
	};

	template<> struct Core<DiscordCoreAPI::ThreadMetadataData> {
		using ValueType = DiscordCoreAPI::ThreadMetadataData;
		static constexpr auto parseValue = object("archived", &ValueType::archived, "archived_timestamp", &ValueType::archiveTimestamp,
			"auto_archive_duration", &ValueType::autoArchiveDuration, "invitable", &ValueType::invitable, "locked", &ValueType::locked);
	};

	template<> struct Core<DiscordCoreAPI::GatewayBotData> {
		using ValueType = DiscordCoreAPI::GatewayBotData;
		static constexpr auto parseValue =
			object("session_start_limit", &ValueType::sessionStartLimit, "shards", &ValueType::shards, "url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreAPI::UserIdBase> {
		using ValueType = DiscordCoreAPI::UserIdBase;
		static constexpr auto parseValue = object("id", &ValueType::id);
	};

	template<> struct Core<DiscordCoreAPI::UserData> {
		using ValueType = DiscordCoreAPI::UserData;
		static constexpr auto parseValue = object("id", &ValueType::id, "username", &ValueType::userName, "discriminator", &ValueType::discriminator,
			"global_name", &ValueType::globalName, "avatar", &ValueType::avatar, "bot", &ValueType::bot, "system", &ValueType::system, "mfa_enabled",
			&ValueType::mfaEnabled, "banner", &ValueType::banner, "accentColor", &ValueType::accentColor, "locale", &ValueType::locale, "verified",
			&ValueType::verified, "email", &ValueType::email, "flags", &ValueType::flags, "premium_type", &ValueType::premiumType, "public_flags",
			&ValueType::publicFlags, "avatar_decoration", &ValueType::avatarDecoration);
	};

	template<> struct Core<DiscordCoreAPI::WelcomeScreenChannelData> {
		using ValueType = DiscordCoreAPI::WelcomeScreenChannelData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "description", &ValueType::description, "emoji_id",
			&ValueType::emojiId, "emoji_name", &ValueType::emojiName);
	};

	template<> struct Core<DiscordCoreAPI::WelcomeScreenData> {
		using ValueType = DiscordCoreAPI::WelcomeScreenData;
		static constexpr auto parseValue = object("description", &ValueType::description, "welcome_screen_channels", &ValueType::welcomeChannels);
	};

	template<> struct Core<DiscordCoreAPI::ActionMetaData> {
		using ValueType = DiscordCoreAPI::ActionMetaData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "duration_seconds", &ValueType::durationSeconds);
	};

	template<> struct Core<DiscordCoreAPI::ActionData> {
		using ValueType = DiscordCoreAPI::ActionData;
		static constexpr auto parseValue = object("meta_data", &ValueType::metadata, "type", &ValueType::type);
	};

	template<> struct Core<DiscordCoreAPI::GuildPreviewData> {
		using ValueType = DiscordCoreAPI::GuildPreviewData;
		static constexpr auto parseValue = object("approximate_member_count", &ValueType::approximateMemberCount, "approximate_presence_count",
			&ValueType::approximatePresenceCount, "description", &ValueType::description, "discovery_splash", &ValueType::discoverySplash, "emojis",
			&ValueType::emojis, "features", &ValueType::features, "icon", &ValueType::icon, "id", &ValueType::id, "name", &ValueType::name, "splash",
			&ValueType::splash, "stickers", &ValueType::stickers);
	};

	template<> struct Core<DiscordCoreAPI::ThreadMemberData> {
		using ValueType = DiscordCoreAPI::ThreadMemberData;
		static constexpr auto parseValue =
			object("flags", &ValueType::flags, "join_timestamp", &ValueType::joinTimestamp, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::AuditLogData> {
		using ValueType = DiscordCoreAPI::AuditLogData;
		static constexpr auto parseValue = object("audit_log_entries", &ValueType::auditLogEntries, "auto_moderation_rules",
			&ValueType::autoModerationRules, "guild_scheduled_events", &ValueType::guildScheduledEvents, "integrations", &ValueType::integrations,
			"threads", &ValueType::threads, "users", &ValueType::users, "webhooks", &ValueType::webhooks);
	};

	template<> struct Core<DiscordCoreAPI::BanData> {
		using ValueType = DiscordCoreAPI::BanData;
		static constexpr auto parseValue =
			object("failed_due_to_perms", &ValueType::failedDueToPerms, "reason", &ValueType::reason, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::UpdateVoiceStateData> {
		using ValueType = DiscordCoreAPI::UpdateVoiceStateData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId, "self_deaf",
			&ValueType::selfDeaf, "self_mute", &ValueType::selfMute);
	};

	template<> struct Core<DiscordCoreAPI::UpdateVoiceStateDataDC> {
		using ValueType = DiscordCoreAPI::UpdateVoiceStateDataDC;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId, "self_deaf",
			&ValueType::selfDeaf, "self_mute", &ValueType::selfMute);
	};

	template<> struct Core<DiscordCoreAPI::GuildWidgetData> {
		using ValueType = DiscordCoreAPI::GuildWidgetData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "enabled", &ValueType::enabled);
	};

	template<> struct Core<DiscordCoreAPI::ChannelData> {
		using ValueType = DiscordCoreAPI::ChannelData;
		static constexpr auto parseValue =
			object("permission_overwrites", &ValueType::permissionOverwrites, "default_reaction_emoji", &ValueType::defaultReactionEmoji,
				"available_tags", &ValueType::availableTags, "default_thread_rate_limit_per_user", &ValueType::defaultThreadRateLimitPerUser,
				"applied_tags", &ValueType::appliedTags, "default_forum_layout", &ValueType::defaultForumLayout, "default_auto_archive_duration",
				&ValueType::defaultAutoArchiveDuration, "thread_metadata", &ValueType::threadMetadata, "recipients", &ValueType::recipients,
				"default_sort_order", &ValueType::defaultSortOrder, "last_pin_timestamp", &ValueType::lastPinTimestamp, "last_message_id",
				&ValueType::lastMessageId, "application_id", &ValueType::applicationId, "total_message_sent", &ValueType::totalMessageSent,
				"rate_limit_per_user", &ValueType::rateLimitPerUser, "video_quality_mode", &ValueType::videoQualityMode, "member", &ValueType::member,
				"permissions", &ValueType::permissions, "rtc_region", &ValueType::rtcRegion, "message_count", &ValueType::messageCount, "owner_id",
				&ValueType::ownerId, "member_count", &ValueType::memberCount, "flags", &ValueType::flags, "topic", &ValueType::topic, "user_limit",
				&ValueType::userLimit, "parent_id", &ValueType::parentId, "type", &ValueType::type, "guild_id", &ValueType::guildId, "position",
				&ValueType::position, "name", &ValueType::name, "icon", &ValueType::icon, "bitrate", &ValueType::bitrate, "id", &ValueType::id,
				"managed", &ValueType::managed, "nsfw", &ValueType::nsfw);
	};

	template<> struct Core<DiscordCoreAPI::GuildMemberData> {
		using ValueType = DiscordCoreAPI::GuildMemberData;
		static constexpr auto parseValue = object("user", &ValueType::user, "nick", &ValueType::nick, "avatar", &ValueType::avatar, "roles",
			&ValueType::roles, "joined_at", &ValueType::joinedAt, "premium_since", &ValueType::premiumSince, "deaf", &ValueType::deaf, "mute",
			&ValueType::mute, "flags", &ValueType::flags, "pending", &ValueType::pending, "permissions", &ValueType::permissions,
			"communication_disabled_until", &ValueType::communicationDisabledUntil, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::ThreadListSyncData> {
		using ValueType = DiscordCoreAPI::ThreadListSyncData;
		static constexpr auto parseValue = object("members", &ValueType::members, "channel_ids", &ValueType::channelIds, "threads",
			&ValueType::threads, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::InteractionDataData> {
		using ValueType = DiscordCoreAPI::InteractionDataData;
		static constexpr auto parseValue =
			object("options", &ValueType::options, "type", &ValueType::type, "resolved", &ValueType::resolved, "guild_id", &ValueType::guildId,
				"name", &ValueType::name, "values", &ValueType::values, "component_type", &ValueType::componentType, "custom_id",
				&ValueType::customId, "custom_id_small", &ValueType::customIdSmall, "value", &ValueType::value, "target_id", &ValueType::targetId);
	};

	template<> struct Core<DiscordCoreAPI::ResolvedData> {
		using ValueType = DiscordCoreAPI::ResolvedData;
		static constexpr auto parseValue = object("attachments", &ValueType::attachments, "members", &ValueType::members, "messages",
			&ValueType::messages, "channels", &ValueType::channels, "users", &ValueType::users, "roles", &ValueType::roles);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandInteractionDataOption> {
		using ValueType = DiscordCoreAPI::ApplicationCommandInteractionDataOption;
		static constexpr auto parseValue = object("name", &ValueType::name, "type", &ValueType::type, "value", &ValueType::value, "focused",
			&ValueType::focused, "options", &ValueType::options);
	};

	template<> struct Core<DiscordCoreAPI::InteractionData> {
		using ValueType = DiscordCoreAPI::InteractionData;
		static constexpr auto parseValue = object("app_permissions", &ValueType::appPermissions, "data", &ValueType::data, "guild_locale",
			&ValueType::guildLocale, "application_id", &ValueType::applicationId, "member", &ValueType::member, "type", &ValueType::type, "message",
			&ValueType::message, "channel_id", &ValueType::channelId, "locale", &ValueType::locale, "guild_id", &ValueType::guildId, "token",
			&ValueType::token, "version", &ValueType::version, "user", &ValueType::user, "id", &ValueType::id);
	};

	template<> struct Core<DiscordCoreAPI::InputEventData> {
		using ValueType = DiscordCoreAPI::InputEventData;
		static constexpr auto parseValue = object("response_type", &ValueType::responseType, "interaction_data", &ValueType::interactionData);
	};

	template<> struct Core<DiscordCoreAPI::ThreadMembersUpdateData> {
		using ValueType = DiscordCoreAPI::ThreadMembersUpdateData;
		static constexpr auto parseValue = object("added_members", &ValueType::addedMembers, "member_count", &ValueType::memberCount, "guild_id",
			&ValueType::guildId, "removed_member_ids", &ValueType::removedMemberIds);
	};

	template<> struct Core<DiscordCoreAPI::WebHookData> {
		using ValueType = DiscordCoreAPI::WebHookData;
		static constexpr auto parseValue =
			object("application_id", &ValueType::applicationId, "avatar", &ValueType::avatar, "channel_id", &ValueType::channelId, "guild_id",
				&ValueType::guildId, "id", &ValueType::id, "name", &ValueType::name, "source_channel", &ValueType::sourceChannel, "source_guild",
				&ValueType::sourceGuild, "token", &ValueType::token, "type", &ValueType::type, "url", &ValueType::url, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::AuditLogChangeData> {
		using ValueType = DiscordCoreAPI::AuditLogChangeData;
		static constexpr auto parseValue = object("key", &ValueType::key, "new_value", &ValueType::newValue, "old_value", &ValueType::oldValue);
	};

	template<> struct Core<DiscordCoreAPI::OptionalAuditEntryInfoData> {
		using ValueType = DiscordCoreAPI::OptionalAuditEntryInfoData;
		static constexpr auto parseValue = object("delete_member_days", &ValueType::deleteMemberDays, "members_removed", &ValueType::membersRemoved,
			"application_id", &ValueType::applicationId, "role_name", &ValueType::roleName, "channel_id", &ValueType::channelId, "message_id",
			&ValueType::messageId, "count", &ValueType::count, "type", &ValueType::type);
	};

	template<> struct Core<DiscordCoreAPI::ArchivedThreadsData> {
		using ValueType = DiscordCoreAPI::ArchivedThreadsData;
		static constexpr auto parseValue = object("has_more", &ValueType::hasMore, "member", &ValueType::members, "threads", &ValueType::threads);
	};

	template<> struct Core<DiscordCoreAPI::ConnectionData> {
		using ValueType = DiscordCoreAPI::ConnectionData;
		static constexpr auto parseValue = object("friend_sync", &ValueType::friendSync, "id", &ValueType::id, "integrations",
			&ValueType::integrations, "name", &ValueType::name, "revoked", &ValueType::revoked, "show_activity", &ValueType::showActivity, "type",
			&ValueType::type, "verified", &ValueType::verified, "visibility", &ValueType::visibility);
	};

	template<> struct Core<DiscordCoreAPI::GuildPruneCountData> {
		using ValueType = DiscordCoreAPI::GuildPruneCountData;
		static constexpr auto parseValue = object("count", &ValueType::count);
	};

	template<> struct Core<DiscordCoreAPI::ThreadData> {
		using ValueType = DiscordCoreAPI::ThreadData;
		static constexpr auto parseValue = object("application_id", &ValueType::applicationId, "applied_tags", &ValueType::appliedTags,
			"available_tags", &ValueType::availableTags, "guild_id", &ValueType::guildId, "bitrate", &ValueType::bitrate,
			"default_auto_archive_duration", &ValueType::defaultAutoArchiveDuration, "default_reaction_emoji", &ValueType::defaultReactionEmoji,
			"flags", &ValueType::flags, "icon", &ValueType::icon, "id", &ValueType::id, "last_message_id", &ValueType::lastMessageId,
			"last_pin_timestamp", &ValueType::lastPinTimestamp, "member", &ValueType::member, "member_count", &ValueType::memberCount,
			"message_count", &ValueType::messageCount, "name", &ValueType::name, "owner_id", &ValueType::ownerId, "parent_id", &ValueType::parentId,
			"permission_overwrites", &ValueType::permissionOverwrites, "permissions", &ValueType::permissions, "position", &ValueType::position,
			"rate_limit_per_user", &ValueType::rateLimitPerUser, "recipients", &ValueType::recipients, "rtc_region", &ValueType::rtcRegion,
			"thread_metadata", &ValueType::threadMetadata, "topic", &ValueType::topic, "total_message_sent", &ValueType::totalMessageSent, "type",
			&ValueType::type, "user_limit", &ValueType::userLimit, "video_quality_mode", &ValueType::videoQualityMode);
	};

	template<> struct Core<DiscordCoreAPI::OverWriteData> {
		using ValueType = DiscordCoreAPI::OverWriteData;
		static constexpr auto parseValue =
			object("allow", &ValueType::allow, "deny", &ValueType::deny, "id", &ValueType::id, "type", &ValueType::type);
	};

	template<> struct Core<DiscordCoreAPI::ForumTagData> {
		using ValueType = DiscordCoreAPI::ForumTagData;
		static constexpr auto parseValue = object("emoji_id", &ValueType::emojiId, "emoji_name", &ValueType::emojiName, "id", &ValueType::id,
			"moderated", &ValueType::moderated, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::DefaultReactionData> {
		using ValueType = DiscordCoreAPI::DefaultReactionData;
		static constexpr auto parseValue = object("emoji_id", &ValueType::emojiId, "emoji_name", &ValueType::emojiName);
	};

	template<> struct Core<DiscordCoreAPI::VoiceRegionData> {
		using ValueType = DiscordCoreAPI::VoiceRegionData;
		static constexpr auto parseValue = object("custom", &ValueType::custom, "depracated", &ValueType::deprecated, "id", &ValueType::id, "name",
			&ValueType::name, "optimal", &ValueType::optimal);
	};

	template<> struct Core<DiscordCoreAPI::InviteData> {
		using ValueType = DiscordCoreAPI::InviteData;
		static constexpr auto parseValue = object("approximate_member_count", &ValueType::approximateMemberCount, "approximate_presence_count",
			&ValueType::approximatePresenceCount, "channel", &ValueType::channel, "guild_id", &ValueType::guildId, "code", &ValueType::code,
			"created_at", &ValueType::createdAt, "expires_at", &ValueType::expiresAt, "guild", &ValueType::guild, "guild_scheduled_event",
			&ValueType::guildScheduledEvent, "inviter", &ValueType::inviter, "max_age", &ValueType::maxAge, "max_uses", &ValueType::maxUses,
			"stage_instance", &ValueType::stageInstance, "target_application", &ValueType::targetApplication, "target_type", &ValueType::targetType,
			"target_user", &ValueType::targetUser, "temporary", &ValueType::temporary, "uses", &ValueType::uses);
	};

	template<> struct Core<DiscordCoreAPI::AutoModerationRuleData> {
		using ValueType = DiscordCoreAPI::AutoModerationRuleData;
		static constexpr auto parseValue = object("actions", &ValueType::actions, "creator_id", &ValueType::creatorId, "enabled", &ValueType::enabled,
			"guild_id", &ValueType::guildId, "event_type", &ValueType::eventType, "exempt_channels", &ValueType::exemptChannels, "exempt_roles",
			&ValueType::exemptRoles, "id", &ValueType::id, "trigger_meta_data", &ValueType::triggerMetaData, "trigger_type", &ValueType::triggerType);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandData> {
		using ValueType = DiscordCoreAPI::ApplicationCommandData;
		static constexpr auto parseValue =
			object("application_id", &ValueType::applicationId, "default_member_permissions", &ValueType::defaultMemberPermissions, "description",
				&ValueType::description, "guild_id", &ValueType::guildId, "description_localizations", &ValueType::descriptionLocalizations,
				"dm_permission", &ValueType::dmPermission, "id", &ValueType::id, "name", &ValueType::name, "name_localizations",
				&ValueType::nameLocalizations, "options", &ValueType::options, "type", &ValueType::type, "version", &ValueType::version);
	};

	template<> struct Core<DiscordCoreAPI::StickerData> {
		using ValueType = DiscordCoreAPI::StickerData;
		static constexpr auto parseValue = object("asset", &ValueType::asset, "description", &ValueType::description, "format_type",
			&ValueType::formatType, "id", &ValueType::id, "name", &ValueType::name, "pack_id", &ValueType::packId, "tags", &ValueType::tags, "type",
			&ValueType::type, "user", &ValueType::user, "guild_id", &ValueType::guildId, "sort_value", &ValueType::sortValue, "nsfw_level",
			&ValueType::nsfwLevel, "flags", &ValueType::flags);
	};

	template<> struct Core<DiscordCoreAPI::MessageActivityData> {
		using ValueType = DiscordCoreAPI::MessageActivityData;
		static constexpr auto parseValue = object("type", &ValueType::type, "party_id", &ValueType::partyId);
	};

	template<> struct Core<DiscordCoreAPI::ActiveThreadsData> {
		using ValueType = DiscordCoreAPI::ActiveThreadsData;
		static constexpr auto parseValue = object("has_more", &ValueType::hasMore, "members", &ValueType::members, "threads", &ValueType::threads);
	};

	template<> struct Core<DiscordCoreAPI::ClientStatus> {
		using ValueType = DiscordCoreAPI::ClientStatus;
		static constexpr auto parseValue = object("web", &ValueType::web, "mobile", &ValueType::mobile, "desktop", &ValueType::desktop);
	};

	template<> struct Core<DiscordCoreAPI::ActivityData> {
		using ValueType = DiscordCoreAPI::ActivityData;
		static constexpr auto parseValue = object("name", &ValueType::name, "type", &ValueType::type, "url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreAPI::PresenceUpdateData> {
		using ValueType = DiscordCoreAPI::PresenceUpdateData;
		static constexpr auto parseValue = object("client_status", &ValueType::clientStatus, "guild_id", &ValueType::guildId, "status",
			&ValueType::status, "user", &ValueType::user, "activities", &ValueType::activities);
	};

	template<> struct Core<DiscordCoreAPI::AccountData> {
		using ValueType = DiscordCoreAPI::AccountData;
		static constexpr auto parseValue = object("name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::EmbedData> {
		using ValueType = DiscordCoreAPI::EmbedData;
		static constexpr auto parseValue = object("author", &ValueType::author, "color", &ValueType::hexColorValue, "description",
			&ValueType::description, "fields", &ValueType::fields, "footer", &ValueType::footer, "image", &ValueType::image, "provider",
			&ValueType::provider, "thumbnail", &ValueType::thumbnail, "timestamp", &ValueType::timeStamp, "title", &ValueType::title, "type",
			&ValueType::type, "url", &ValueType::url, "video", &ValueType::video);
	};

	template<> struct Core<DiscordCoreAPI::MessageDataOld> {
		using ValueType = DiscordCoreAPI::MessageDataOld;
		static constexpr auto parseValue = object("activity", &ValueType::activity, "application", &ValueType::application, "application_id",
			&ValueType::applicationId, "attachments", &ValueType::attachments, "author", &ValueType::author, "channel_id", &ValueType::channelId,
			"components", &ValueType::components, "content", &ValueType::content, "edited_timestamp", &ValueType::editedTimestamp, "embeds",
			&ValueType::embeds, "flags", &ValueType::flags, "guild_id", &ValueType::guildId, "id", &ValueType::id, "interaction",
			&ValueType::interaction, "mention_channels", &ValueType::mentionChannels, "mention_everyone", &ValueType::mentionEveryone,
			"mention_roles", &ValueType::mentionRoles, "mentions", &ValueType::mentions, "message_reference", &ValueType::messageReference, "nonce",
			&ValueType::nonce, "pinned", &ValueType::pinned, "reactions", &ValueType::reactions, "sticker_items", &ValueType::stickerItems,
			"stickers", &ValueType::stickers, "thread", &ValueType::thread, "timestamp", &ValueType::timeStamp, "tts", &ValueType::tts, "type",
			&ValueType::type, "webhook_id", &ValueType::webHookId);
	};

	template<> struct Core<DiscordCoreAPI::ReactionData> {
		using ValueType = DiscordCoreAPI::ReactionData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "count", &ValueType::count, "emoji", &ValueType::emoji,
			"guild_id", &ValueType::guildId, "id", &ValueType::id, "member", &ValueType::member, "me", &ValueType::me, "message_id",
			&ValueType::messageId, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::StickerItemData> {
		using ValueType = DiscordCoreAPI::StickerItemData;
		static constexpr auto parseValue = object("format_type", &ValueType::formatType, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::AttachmentData> {
		using ValueType = DiscordCoreAPI::AttachmentData;
		static constexpr auto parseValue = object("content_type", &ValueType::contentType, "description", &ValueType::description, "filename",
			&ValueType::filename, "height", &ValueType::height, "id", &ValueType::id, "proxy_url", &ValueType::proxyUrl, "size", &ValueType::size,
			"url", &ValueType::url, "width", &ValueType::width, "ephemeral", &ValueType::ephemeral);
	};

	template<> struct Core<DiscordCoreAPI::ChannelMentionData> {
		using ValueType = DiscordCoreAPI::ChannelMentionData;
		static constexpr auto parseValue =
			object("id", &ValueType::id, "guild_id", &ValueType::guildId, "name", &ValueType::name, "type", &ValueType::type);
	};

	template<> struct Core<DiscordCoreAPI::ActionRowData> {
		using ValueType = DiscordCoreAPI::ActionRowData;
		static constexpr auto parseValue = object("components", &ValueType::components, "type", &ValueType::type);
	};

	template<> struct Core<DiscordCoreAPI::EmbedFooterData> {
		using ValueType = DiscordCoreAPI::EmbedFooterData;
		static constexpr auto parseValue =
			object("proxy_icon_url", &ValueType::proxyIconUrl, "icon_url", &ValueType::iconUrl, "text", &ValueType::text);
	};

	template<> struct Core<DiscordCoreAPI::EmbedImageData> {
		using ValueType = DiscordCoreAPI::EmbedImageData;
		static constexpr auto parseValue =
			object("proxy_url", &ValueType::proxyUrl, "url", &ValueType::url, "height", &ValueType::height, "width", &ValueType::width);
	};

	template<> struct Core<DiscordCoreAPI::EmbedThumbnailData> {
		using ValueType = DiscordCoreAPI::EmbedThumbnailData;
		static constexpr auto parseValue =
			object("proxy_url", &ValueType::proxyUrl, "url", &ValueType::url, "height", &ValueType::height, "width", &ValueType::width);
	};

	template<> struct Core<DiscordCoreAPI::EmbedVideoData> {
		using ValueType = DiscordCoreAPI::EmbedVideoData;
		static constexpr auto parseValue =
			object("proxy_url", &ValueType::proxyUrl, "url", &ValueType::url, "height", &ValueType::height, "width", &ValueType::width);
	};

	template<> struct Core<DiscordCoreAPI::EmbedProviderData> {
		using ValueType = DiscordCoreAPI::EmbedProviderData;
		static constexpr auto parseValue = object("name", &ValueType::name, "url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreAPI::EmbedAuthorData> {
		using ValueType = DiscordCoreAPI::EmbedAuthorData;
		static constexpr auto parseValue =
			object("proxy_icon_url", &ValueType::proxyIconUrl, "icon_url", &ValueType::iconUrl, "name", &ValueType::name, "url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreAPI::EmbedFieldData> {
		using ValueType = DiscordCoreAPI::EmbedFieldData;
		static constexpr auto parseValue = object("name", &ValueType::name, "value", &ValueType::value, "inline", &ValueType::Inline);
	};

	template<> struct Core<DiscordCoreAPI::GuildMemberCacheData> {
		using ValueType = DiscordCoreAPI::GuildMemberCacheData;
		static constexpr auto parseValue =
			object("avatar", &ValueType::avatar, "flags", &ValueType::flags, "guild_id", &ValueType::guildId, "joined_at", &ValueType::joinedAt,
				"nick", &ValueType::nick, "permissions", &ValueType::permissions, "roles", &ValueType::roles, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::AuditLogEntryData> {
		using ValueType = DiscordCoreAPI::AuditLogEntryData;
		static constexpr auto parseValue = object("action_type", &ValueType::actionType, "changes", &ValueType::changes, "created_at",
			&ValueType::createdTimeStamp, "id", &ValueType::id, "options", &ValueType::options, "reason", &ValueType::reason, "target_id",
			&ValueType::targetId, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::ComponentData> {
		using ValueType = DiscordCoreAPI::ComponentData;
		static constexpr auto parseValue = object("type", &ValueType::type, "custom_id", &ValueType::customId, "options", &ValueType::options,
			"channel_types", &ValueType::channelTypes, "placeholder", &ValueType::placeholder, "min_values", &ValueType::minValues, "max_values",
			&ValueType::maxValues, "min_length", &ValueType::minLength, "max_length", &ValueType::maxLength, "label", &ValueType::label, "value",
			&ValueType::value, "title", &ValueType::title, "emoji", &ValueType::emoji, "url", &ValueType::url, "required", &ValueType::required,
			"disabled", &ValueType::disabled, "style", &ValueType::style);
	};

	template<> struct Core<DiscordCoreAPI::SelectOptionData> {
		using ValueType = DiscordCoreAPI::SelectOptionData;
		static constexpr auto parseValue = object("description", &ValueType::description, "label", &ValueType::label, "value", &ValueType::value,
			"emoji", &ValueType::emoji, "default", &ValueType::_default);
	};

	template<> struct Core<DiscordCoreAPI::VoiceStateDataLight> {
		using ValueType = DiscordCoreAPI::VoiceStateDataLight;
		static constexpr auto parseValue =
			object("channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::GuildData> {
		using ValueType = DiscordCoreAPI::GuildData;
		static constexpr auto parseValue = object("default_message_notifications", &ValueType::defaultMessageNotifications, "guild_scheduled_events",
			&ValueType::guildScheduledEvents, "explicit_content_filter", &ValueType::explicitContentFilter, "stage_instances",
			&ValueType::stageInstances, "presences", &ValueType::presences, "system_channel_flags", &ValueType::systemChannelFlags, "widget_enabled",
			&ValueType::widgetEnabled, "unavailable", &ValueType::unavailable, "owner", &ValueType::owner, "large", &ValueType::large, "member_count",
			&ValueType::memberCount, "verification_level", &ValueType::verificationLevel, "id", &ValueType::id, "channels", &ValueType::channels,
			"roles", &ValueType::roles, "members", &ValueType::members, "voice_states", &ValueType::voiceStates, "owner_id", &ValueType::ownerId,
			"permissions", &ValueType::permissions, "afk_channel_id", &ValueType::afkChannelId, "features", &ValueType::features, "stickers",
			&ValueType::stickers, "max_stage_video_channel_users", &ValueType::maxStageVideoChannelUsers, "public_updates_channel_id",
			&ValueType::publicUpdatesChannelId, "premium_subscription_count", &ValueType::premiumSubscriptionCount, "approximate_presence_count",
			&ValueType::approximatePresenceCount, "threads", &ValueType::threads, "welcome_screen", &ValueType::welcomeScreen,
			"safety_alerts_channel_id", &ValueType::safetyAlertsChannelId, "approximate_member_count", &ValueType::approximateMemberCount,
			"premium_progress_bar_enabled", &ValueType::premiumProgressBarEnabled, "max_video_channel_users", &ValueType::maxVideoChannelUsers,
			"emoji", &ValueType::emoji, "preferred_locale", &ValueType::preferredLocale, "vanity_url_code", &ValueType::vanityUrlCode,
			"system_channel_id", &ValueType::systemChannelId, "discovery_splash", &ValueType::discoverySplash, "widget_channel_id",
			&ValueType::widgetChannelId, "rules_channel_id", &ValueType::rulesChannelId, "nsfw_level", &ValueType::nsfwLevel, "application_id",
			&ValueType::applicationId, "description", &ValueType::description, "premium_tier", &ValueType::premiumTier, "afk_timeout",
			&ValueType::afkTimeout, "max_members", &ValueType::maxMembers, "mfa_level", &ValueType::mfaLevel, "splash", &ValueType::splash, "name",
			&ValueType::name, "banner", &ValueType::banner, "icon", &ValueType::icon);
	};

	template<> struct Core<DiscordCoreAPI::PartialGuildData> {
		using ValueType = DiscordCoreAPI::PartialGuildData;
		static constexpr auto parseValue = object("unavailable", &ValueType::unavailable, "id", &ValueType::id);
	};

	template<> struct Core<DiscordCoreAPI::IntegrationData> {
		using ValueType = DiscordCoreAPI::IntegrationData;
		static constexpr auto parseValue = object("account", &ValueType::account, "application", &ValueType::application, "enabled",
			&ValueType::enabled, "enable_emoticons", &ValueType::enableEmoticons, "expire_behavior", &ValueType::expireBehavior,
			"expire_grace_period", &ValueType::expireGracePeriod, "id", &ValueType::id, "name", &ValueType::name, "revoked", &ValueType::revoked,
			"role_id", &ValueType::roleId, "subscriber_count", &ValueType::subscriberCount, "syncing", &ValueType::syncing, "synced_at",
			&ValueType::syncedAt, "type", &ValueType::type, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::File> {
		using ValueType = DiscordCoreAPI::File;
		static constexpr auto parseValue = array(&ValueType::data);
	};

	template<> struct Core<DiscordCoreAPI::AllowedMentionsData> {
		using ValueType = DiscordCoreAPI::AllowedMentionsData;
		static constexpr auto parseValue =
			object("replied_user", &ValueType::repliedUser, "parse", &ValueType::parse, "roles", &ValueType::roles, "users", &ValueType::users);
	};

	template<> struct Core<DiscordCoreAPI::MessageReferenceData> {
		using ValueType = DiscordCoreAPI::MessageReferenceData;
		static constexpr auto parseValue = object("fail_if_not_exists", &ValueType::failIfNotExists, "message_id", &ValueType::messageId,
			"channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::MessageInteractionData> {
		using ValueType = DiscordCoreAPI::MessageInteractionData;
		static constexpr auto parseValue =
			object("id", &ValueType::id, "member", &ValueType::member, "name", &ValueType::name, "type", &ValueType::type, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventData> {
		using ValueType = DiscordCoreAPI::GuildScheduledEventData;
		static constexpr auto parseValue = object("id", &ValueType::id, "privacy_level", &ValueType::privacyLevel, "entity_metadata",
			&ValueType::entityMetadata, "entity_type", &ValueType::entityType, "status", &ValueType::status, "scheduled_start_time",
			&ValueType::scheduledStartTime, "scheduled_end_time", &ValueType::scheduledEndTime, "description", &ValueType::description, "creator_id",
			&ValueType::creatorId, "channel_id", &ValueType::channelId, "entity_id", &ValueType::entityId, "user_count", &ValueType::userCount,
			"guild_id", &ValueType::guildId, "creator", &ValueType::creator, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventUserData> {
		using ValueType = DiscordCoreAPI::GuildScheduledEventUserData;
		static constexpr auto parseValue =
			object("guild_scheduled_event_id", &ValueType::guildScheduledEventId, "member", &ValueType::member, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventMetadata> {
		using ValueType = DiscordCoreAPI::GuildScheduledEventMetadata;
		static constexpr auto parseValue = object("location", &ValueType::location);
	};

	template<> struct Core<DiscordCoreAPI::RoleData> {
		using ValueType = DiscordCoreAPI::RoleData;
		static constexpr auto parseValue =
			object("id", &ValueType::id, "name", &ValueType::name, "color", &ValueType::color, "hoist", &ValueType::hoist, "icon", &ValueType::icon,
				"unicode_emoji", &ValueType::unicodeEmoji, "position", &ValueType::position, "permissions", &ValueType::permissions, "managed",
				&ValueType::managed, "mentionable", &ValueType::mentionable, "tags", &ValueType::tags, "flags", &ValueType::flags);
	};

	template<> struct Core<DiscordCoreAPI::MessageData> {
		using ValueType = DiscordCoreAPI::MessageData;
		static constexpr auto parseValue = object("activity", &ValueType::activity, "application", &ValueType::application, "application_id",
			&ValueType::applicationId, "attachments", &ValueType::attachments, "author", &ValueType::author, "channel_id", &ValueType::channelId,
			"components", &ValueType::components, "content", &ValueType::content, "edited_timestamp", &ValueType::editedTimestamp, "embeds",
			&ValueType::embeds, "flags", &ValueType::flags, "guild_id", &ValueType::guildId, "id", &ValueType::id, "interaction",
			&ValueType::interaction, "member", &ValueType::member, "mention_channels", &ValueType::mentionChannels, "mention_everyone",
			&ValueType::mentionEveryone, "mention_roles", &ValueType::mentionRoles, "mentions", &ValueType::mentions, "message_reference",
			&ValueType::messageReference, "nonce", &ValueType::nonce, "pinned", &ValueType::pinned, "reactions", &ValueType::reactions,
			"referenced_message", &ValueType::referencedMessage, "sticker_items", &ValueType::stickerItems, "stickers", &ValueType::stickers,
			"thread", &ValueType::thread, "timestamp", &ValueType::timeStamp, "tts", &ValueType::tts, "type", &ValueType::type, "webhook_id",
			&ValueType::webHookId);
	};

	template<> struct Core<DiscordCoreAPI::StickerPackData> {
		using ValueType = DiscordCoreAPI::StickerPackData;
		static constexpr auto parseValue = object("banner_asset_id", &ValueType::bannerAssetId, "cover_sticker_id", &ValueType::coverStickerId,
			"description", &ValueType::description, "id", &ValueType::snowflake, "name", &ValueType::name, "sku_id", &ValueType::skuId, "stickers",
			&ValueType::stickers);
	};

	template<> struct Core<DiscordCoreAPI::GuildWidgetImageData> {
		using ValueType = DiscordCoreAPI::GuildWidgetImageData;
		static constexpr auto parseValue = object("url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreAPI::GuildTemplateData> {
		using ValueType = DiscordCoreAPI::GuildTemplateData;
		static constexpr auto parseValue =
			object("code", &ValueType::code, "created_at", &ValueType::createdAt, "creator", &ValueType::creator, "creator_id", &ValueType::creatorId,
				"description", &ValueType::description, "is_dirty", &ValueType::isDirty, "name", &ValueType::name, "serialized_source_guild",
				&ValueType::serializedSourceGuild, "source_guild_id", &ValueType::sourceGuildId, "updated_at", &ValueType::updatedAt);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandOptionData> {
		using ValueType = DiscordCoreAPI::ApplicationCommandOptionData;
		static constexpr auto parseValue = object("type", &ValueType::type, "name", &ValueType::name, "description", &ValueType::description,
			"required", &ValueType::required, "autocomplete", &ValueType::autocomplete, "min_value", &ValueType::minValue, "max_value",
			&ValueType::maxValue, "channel_types", &ValueType::channelTypes, "description_localizations", &ValueType::descriptionLocalizations,
			"name_localizations", &ValueType::nameLocalizations, "choices", &ValueType::choices, "options", &ValueType::options);
	};

	template<> struct Core<DiscordCoreAPI::StageInstanceData> {
		using ValueType = DiscordCoreAPI::StageInstanceData;
		static constexpr auto parseValue = object("privacy_level", &ValueType::privacyLevel, "discoverable_disabled",
			&ValueType::discoverableDisabled, "channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId, "topic", &ValueType::topic);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandOptionChoiceData> {
		using ValueType = DiscordCoreAPI::ApplicationCommandOptionChoiceData;
		static constexpr auto parseValue =
			object("name", &ValueType::name, "value", &ValueType::value, "name_localized", &ValueType::nameLocalizations);
	};

	template<> struct Core<DiscordCoreAPI::RoleTagsData> {
		using ValueType = DiscordCoreAPI::RoleTagsData;
		static constexpr auto parseValue =
			object("premium_subscriber", &ValueType::premiumSubscriber, "integration_id", &ValueType::integrationId, "bot_id", &ValueType::botId);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::ConnectProperties> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::ConnectProperties;
		static constexpr auto parseValue = object("os", &ValueType::os, "device", &ValueType::device, "browser", &ValueType::browser);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::WebSocketIdentifyData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::WebSocketIdentifyData;
		static constexpr auto parseValue = object("token", &ValueType::botToken, "shard", &ValueType::shard, "intents", &ValueType::intents,
			"large_threshold", &ValueType::largeThreshold, "presence", &ValueType::presence, "properties", &ValueType::properties);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::WebSocketResumeData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::WebSocketResumeData;
		static constexpr auto parseValue =
			object("token", &ValueType::botToken, "session_id", &ValueType::sessionId, "seq", &ValueType::lastNumberReceived);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::VoiceSocketProtocolPayloadDataData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::VoiceSocketProtocolPayloadDataData;
		static constexpr auto parseValue = object("address", &ValueType::address, "mode", &ValueType::mode, "port", &ValueType::port);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::VoiceSocketProtocolPayloadData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::VoiceSocketProtocolPayloadData;
		static constexpr auto parseValue = object("data", &ValueType::data, "protocol", &ValueType::protocol);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::VoiceIdentifyData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::VoiceIdentifyData;
		static constexpr auto parseValue =
			object("session_id", &ValueType::sessionId, "user_id", &ValueType::userId, "token", &ValueType::token, "server_id", &ValueType::serverId);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::SendSpeakingData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::SendSpeakingData;
		static constexpr auto parseValue = object("speaking", &ValueType::type, "delay", &ValueType::delay, "ssrc", &ValueType::ssrc);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::HelloData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::HelloData;
		static constexpr auto parseValue = object("heartbeat_interval", &ValueType::heartbeatInterval, "_trace", &ValueType::_trace);
	};

	template<> struct Core<DiscordCoreAPI::UpdatePresenceData> {
		using ValueType = DiscordCoreAPI::UpdatePresenceData;
		static constexpr auto parseValue =
			object("afk", &ValueType::afk, "since", &ValueType::since, "status", &ValueType::statusReal, "activities", &ValueType::activities);
	};

	template<typename ValueType> struct Core<DiscordCoreAPI::EventData<ValueType>> {
		using ValueType02 = DiscordCoreAPI::EventData<ValueType>;
		static constexpr auto parseValue = object("d", &ValueType02::value);
	};

	template<typename ValueType, typename ValueType01> struct Core<DiscordCoreAPI::UpdatedEventData<ValueType, ValueType01>> {
		using ValueType02 = DiscordCoreAPI::UpdatedEventData<ValueType, ValueType01>;
		static constexpr auto parseValue = object("d", &ValueType02::value);
	};
}
