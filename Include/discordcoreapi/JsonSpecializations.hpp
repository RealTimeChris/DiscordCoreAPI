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
/// JsonSpecializations.hpp - Header for the jsonifier::core specializations.
/// Mar 24, 2023 Chris M.
/// https://discordcoreapi.com
/// \file JsonSpecializations.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		template<> struct websocket_message_data<update_voice_state_data> {
			websocket_message_data() = default;
			websocket_message_data(const update_voice_state_data& data);
			unordered_set<jsonifier::string> jsonifierExcludedKeys{};
			using type = update_voice_state_data;
			std::optional<jsonifier::string> t{};
			int64_t op{ -1 };
			int32_t s{};
			type d{};
			operator etf_serializer();
		};

		template<> struct websocket_message_data<update_voice_state_data_dc> {
			websocket_message_data() = default;
			websocket_message_data(const update_voice_state_data& data);
			unordered_set<jsonifier::string> jsonifierExcludedKeys{};
			using type = update_voice_state_data_dc;
			std::optional<jsonifier::string> t{};
			int64_t op{ -1 };
			int32_t s{};
			type d{};
			operator etf_serializer();
		};
	}
}

namespace jsonifier_internal {

	template<typename value_type>
	concept snowflake_t = std::same_as<discord_core_api::snowflake, std::remove_cvref_t<value_type>>;

	template<jsonifier::serialize_options options, snowflake_t value_type, jsonifier::concepts::buffer_like buffer_type, typename index_type, typename indent_type>
	struct serialize_impl<options, value_type, buffer_type, index_type, indent_type> {
		template<typename value_type_new> JSONIFIER_ALWAYS_INLINE static void impl(value_type_new&& value, buffer_type& buffer, index_type&index, indent_type& indent) noexcept {
			jsonifier::string newString{ static_cast<jsonifier::string>(value) };
			serialize<options>::impl(newString, buffer, index, indent);
		}
	};

	template<bool minified, jsonifier::parse_options options, snowflake_t value_type, typename parse_context_type> struct parse_impl<minified, options, value_type, parse_context_type> {
		JSONIFIER_ALWAYS_INLINE static void impl(value_type& value, parse_context_type& context) noexcept {
			jsonifier::raw_json_data newString{};
			parse<minified, options>::impl(newString, context);
			if (newString.getType() == jsonifier::json_type::String) {
				value = newString.get<jsonifier::string>();
			} else {
				value = newString.get<uint64_t>();
			}
		}
	};

	template<typename value_type>
	concept time_stamp_t = std::same_as<discord_core_api::time_stamp, std::remove_cvref_t<value_type>>;

	template<jsonifier::serialize_options options, time_stamp_t value_type, jsonifier::concepts::buffer_like buffer_type, typename index_type, typename indent_type>
	struct serialize_impl<options, value_type, buffer_type, index_type, indent_type> {
		template<typename value_type_new> JSONIFIER_ALWAYS_INLINE static void impl(value_type_new&& value, buffer_type& buffer, index_type& index, indent_type& indent) noexcept {
			jsonifier::string newString{ static_cast<jsonifier::string>(value) };
			serialize<options>::impl(newString, buffer, index, indent);
		}
	};

	template<bool minified, jsonifier::parse_options options, time_stamp_t value_type, typename parse_context_type>
	struct parse_impl<minified, options, value_type, parse_context_type> {
		JSONIFIER_ALWAYS_INLINE static void impl(value_type& value, parse_context_type& context) noexcept {
			jsonifier::raw_json_data newString{};
			parse<minified, options>::impl(newString, context);
			if (newString.getType() == jsonifier::json_type::String) {
				value = newString.get<jsonifier::string>();
			} else {
				value = newString.get<uint64_t>();
			}
		}
	};
}

namespace jsonifier {

	template<> struct core<discord_core_api::ready_data> {
		using value_type				 = discord_core_api::ready_data;
		static constexpr auto parseValue = createValue("resume_gateway_url", &value_type::resumeGatewayUrl, "shard", &value_type::shard, "v", &value_type::v, "session_id",
			&value_type::sessionId, "user", &value_type::user, "application", &value_type::application, "_trace", &value_type::trace);
	};

	template<> struct core<discord_core_api::discord_core_internal::websocket_message> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_message;
		static constexpr auto parseValue = createValue("op", &value_type::op, "s", &value_type::s, "t", &value_type::t);
	};

	template<typename oty2> struct core<discord_core_api::discord_core_internal::websocket_message_data<oty2>> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_message_data<oty2>;
		static constexpr auto parseValue = createValue("d", &value_type::d, "op", &value_type::op, "s", &value_type::s);
	};

	template<> struct core<discord_core_api::application_command_permission_data> {
		using value_type				 = discord_core_api::application_command_permission_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "permission", &value_type::permission, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::guild_application_command_permissions_data> {
		using value_type = discord_core_api::guild_application_command_permissions_data;
		static constexpr auto parseValue =
			createValue("application_id", &value_type::applicationId, "permissions", &value_type::permissions, "guild_id", &value_type::guildId, "id", &value_type::id);
	};

	template<> struct core<discord_core_api::session_start_data> {
		using value_type = discord_core_api::session_start_data;
		static constexpr auto parseValue =
			createValue("max_concurrency", &value_type::maxConcurrency, "remaining", &value_type::remaining, "reset_after", &value_type::resetAfter, "total", &value_type::total);
	};

	template<> struct core<discord_core_api::authorization_info_data> {
		using value_type = discord_core_api::authorization_info_data;
		static constexpr auto parseValue =
			createValue("application", &value_type::application, "expires", &value_type::expires, "scopes", &value_type::scopes, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::trigger_meta_data> {
		using value_type				 = discord_core_api::trigger_meta_data;
		static constexpr auto parseValue = createValue("allow_list", &value_type::allowList, "keyword_filter", &value_type::keywordFilter, "mention_total_limit",
			&value_type::mentionTotalLimit, "presets", &value_type::presets);
	};

	template<> struct core<discord_core_api::partial_emoji_data> {
		using value_type				 = discord_core_api::partial_emoji_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "name", &value_type::name, "animated", &value_type::animated);
	};

	template<> struct core<discord_core_api::emoji_data> {
		using value_type				 = discord_core_api::emoji_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "name", &value_type::name, "roles", &value_type::roles, "require_colons", &value_type::requireColons,
			"managed", &value_type::managed, "animated", &value_type::animated, "available", &value_type::available);
	};

	template<> struct core<discord_core_api::team_members_object_data> {
		using value_type = discord_core_api::team_members_object_data;
		static constexpr auto parseValue =
			createValue("membership_state", &value_type::membershipState, "permissions", &value_type::permissions, "team_id", &value_type::teamId, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::team_object_data> {
		using value_type = discord_core_api::team_object_data;
		static constexpr auto parseValue =
			createValue("icon", &value_type::icon, "id", &value_type::id, "members", &value_type::members, "owner_user_id", &value_type::ownerUserId);
	};

	template<> struct core<discord_core_api::install_params_data> {
		using value_type				 = discord_core_api::install_params_data;
		static constexpr auto parseValue = createValue("scopes", &value_type::scopes, "permissions", &value_type::permissions);
	};

	template<> struct core<discord_core_api::application_data> {
		using value_type = discord_core_api::application_data;
		static constexpr auto parseValue =
			createValue("bot_public", &value_type::botPublic, "bot_require_code_grant", &value_type::botRequireCodeGrant, "cover_image", &value_type::coverImage,
				"custom_install_url", &value_type::customInstallUrl, "description", &value_type::description, "flags", &value_type::flags, "guild_id", &value_type::guildId, "icon",
				&value_type::icon, "name", &value_type::name, "owner", &value_type::owner, "params", &value_type::params, "primary_sku_id", &value_type::primarySkuId,
				"privacy_policy_url", &value_type::privacyPolicyUrl, "rpc_origins", &value_type::rpcOrigins, "slug", &value_type::slug, "summary", &value_type::summary, "tags",
				&value_type::tags, "team", &value_type::team, "terms_of_service_url", &value_type::termsOfServiceUrl, "verify_key", &value_type::verifyKey);
	};

	template<> struct core<discord_core_api::thread_metadata_data> {
		using value_type				 = discord_core_api::thread_metadata_data;
		static constexpr auto parseValue = createValue("archived", &value_type::archived, "archived_timestamp", &value_type::archiveTimestamp, "auto_archive_duration",
			&value_type::autoArchiveDuration, "invitable", &value_type::invitable, "locked", &value_type::locked);
	};

	template<> struct core<discord_core_api::gateway_bot_data> {
		using value_type				 = discord_core_api::gateway_bot_data;
		static constexpr auto parseValue = createValue("session_start_limit", &value_type::sessionStartLimit, "shards", &value_type::shards, "url", &value_type::url);
	};

	template<> struct core<discord_core_api::user_id_base> {
		using value_type				 = discord_core_api::user_id_base;
		static constexpr auto parseValue = createValue("id", &value_type::id);
	};

	template<> struct core<discord_core_api::user_data> {
		using value_type				 = discord_core_api::user_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "username", &value_type::userName, "discriminator", &value_type::discriminator, "global_name",
			&value_type::globalName, "avatar", &value_type::avatar, "bot", &value_type::bot, "system", &value_type::system, "mfa_enabled", &value_type::mfaEnabled, "banner",
			&value_type::banner, "accentColor", &value_type::accentColor, "locale", &value_type::locale, "verified", &value_type::verified, "email", &value_type::email, "flags",
			&value_type::flags, "premium_type", &value_type::premiumType, "public_flags", &value_type::publicFlags, "avatar_decoration", &value_type::avatarDecoration);
	};

	template<> struct core<discord_core_api::welcome_screen_channel_data> {
		using value_type = discord_core_api::welcome_screen_channel_data;
		static constexpr auto parseValue =
			createValue("channel_id", &value_type::channelId, "description", &value_type::description, "emoji_id", &value_type::emojiId, "emoji_name", &value_type::emojiName);
	};

	template<> struct core<discord_core_api::welcome_screen_data> {
		using value_type				 = discord_core_api::welcome_screen_data;
		static constexpr auto parseValue = createValue("description", &value_type::description, "welcome_screen_channels", &value_type::welcomeChannels);
	};

	template<> struct core<discord_core_api::action_meta_data> {
		using value_type				 = discord_core_api::action_meta_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "duration_seconds", &value_type::durationSeconds);
	};

	template<> struct core<discord_core_api::action_data> {
		using value_type				 = discord_core_api::action_data;
		static constexpr auto parseValue = createValue("meta_data", &value_type::metadata, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::guild_preview_data> {
		using value_type = discord_core_api::guild_preview_data;
		static constexpr auto parseValue =
			createValue("approximate_member_count", &value_type::approximateMemberCount, "approximate_presence_count", &value_type::approximatePresenceCount, "description",
				&value_type::description, "discovery_splash", &value_type::discoverySplash, "emojis", &value_type::emojis, "features", &value_type::features, "icon",
				&value_type::icon, "id", &value_type::id, "name", &value_type::name, "splash", &value_type::splash, "stickers", &value_type::stickers);
	};

	template<> struct core<discord_core_api::thread_member_data> {
		using value_type				 = discord_core_api::thread_member_data;
		static constexpr auto parseValue = createValue("flags", &value_type::flags, "join_timestamp", &value_type::joinTimestamp, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::audit_log_data> {
		using value_type				 = discord_core_api::audit_log_data;
		static constexpr auto parseValue = createValue("audit_log_entries", &value_type::auditLogEntries, "auto_moderation_rules", &value_type::autoModerationRules,
			"guild_scheduled_events", &value_type::guildScheduledEvents, "integrations", &value_type::integrations, "threads", &value_type::threads, "users", &value_type::users,
			"webhooks", &value_type::webhooks);
	};

	template<> struct core<discord_core_api::ban_data> {
		using value_type				 = discord_core_api::ban_data;
		static constexpr auto parseValue = createValue("failed_due_to_perms", &value_type::failedDueToPerms, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::update_voice_state_data> {
		using value_type = discord_core_api::update_voice_state_data;
		static constexpr auto parseValue =
			createValue("channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "self_deaf", &value_type::selfDeaf, "self_mute", &value_type::selfMute);
	};

	template<> struct core<discord_core_api::update_voice_state_data_dc> {
		using value_type = discord_core_api::update_voice_state_data_dc;
		static constexpr auto parseValue =
			createValue("channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "self_deaf", &value_type::selfDeaf, "self_mute", &value_type::selfMute);
	};

	template<> struct core<discord_core_api::guild_widget_data> {
		using value_type				 = discord_core_api::guild_widget_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "enabled", &value_type::enabled);
	};

	template<> struct core<discord_core_api::channel_data> {
		using value_type				 = discord_core_api::channel_data;
		static constexpr auto parseValue = createValue("permission_overwrites", &value_type::permissionOverwrites, "default_reaction_emoji", &value_type::defaultReactionEmoji,
			"available_tags", &value_type::availableTags, "default_thread_rate_limit_per_user", &value_type::defaultThreadRateLimitPerUser, "applied_tags",
			&value_type::appliedTags, "default_forum_layout", &value_type::defaultForumLayout, "default_auto_archive_duration", &value_type::defaultAutoArchiveDuration,
			"thread_metadata", &value_type::threadMetadata, "recipients", &value_type::recipients, "default_sort_order", &value_type::defaultSortOrder, "last_pin_timestamp",
			&value_type::lastPinTimestamp, "last_message_id", &value_type::lastMessageId, "application_id", &value_type::applicationId, "total_message_sent",
			&value_type::totalMessageSent, "rate_limit_per_user", &value_type::rateLimitPerUser, "video_quality_mode", &value_type::videoQualityMode, "member", &value_type::member,
			"permissions", &value_type::permissions, "rtc_region", &value_type::rtcRegion, "message_count", &value_type::messageCount, "owner_id", &value_type::ownerId,
			"member_count", &value_type::memberCount, "flags", &value_type::flags, "topic", &value_type::topic, "user_limit", &value_type::userLimit, "parent_id",
			&value_type::parentId, "type", &value_type::type, "guild_id", &value_type::guildId, "position", &value_type::position, "name", &value_type::name, "icon",
			&value_type::icon, "bitrate", &value_type::bitrate, "id", &value_type::id, "managed", &value_type::managed, "nsfw", &value_type::nsfw);
	};

	template<> struct core<discord_core_api::guild_member_data> {
		using value_type = discord_core_api::guild_member_data;
		static constexpr auto parseValue =
			createValue("user", &value_type::user, "nick", &value_type::nick, "avatar", &value_type::avatar, "roles", &value_type::roles, "joined_at", &value_type::joinedAt,
				"premium_since", &value_type::premiumSince, "deaf", &value_type::deaf, "mute", &value_type::mute, "flags", &value_type::flags, "pending", &value_type::pending,
				"permissions", &value_type::permissions, "communication_disabled_until", &value_type::communicationDisabledUntil, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::thread_list_sync_data> {
		using value_type = discord_core_api::thread_list_sync_data;
		static constexpr auto parseValue =
			createValue("members", &value_type::members, "channel_ids", &value_type::channelIds, "threads", &value_type::threads, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::interaction_data_data> {
		using value_type				 = discord_core_api::interaction_data_data;
		static constexpr auto parseValue = createValue("options", &value_type::options, "values", &value_type::values, "components", &value_type::components, "type",
			&value_type::type, "component_type", &value_type::componentType, "custom_id", &value_type::customId, "name", &value_type::name, "resolved", &value_type::resolved,
			"target_id", &value_type::targetId, "guild_id", &value_type::guildId, "id", &value_type::id);
	};

	template<> struct core<discord_core_api::resolved_data> {
		using value_type				 = discord_core_api::resolved_data;
		static constexpr auto parseValue = createValue("attachments", &value_type::attachments, "members", &value_type::members, "messages", &value_type::messages, "channels",
			&value_type::channels, "users", &value_type::users, "roles", &value_type::roles);
	};

	template<> struct core<discord_core_api::application_command_interaction_data_option> {
		using value_type = discord_core_api::application_command_interaction_data_option;
		static constexpr auto parseValue =
			createValue("name", &value_type::name, "type", &value_type::type, "value", &value_type::value, "focused", &value_type::focused, "options", &value_type::options);
	};

	template<> struct core<discord_core_api::interaction_data> {
		using value_type				 = discord_core_api::interaction_data;
		static constexpr auto parseValue = createValue("app_permissions", &value_type::appPermissions, "guild_locale", &value_type::guildLocale, "data", &value_type::data,
			"locale", &value_type::locale, "member", &value_type::member, "application_id", &value_type::applicationId, "token", &value_type::token, "message",
			&value_type::message, "channel", &value_type::channel, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "guild", &value_type::guild, "version",
			&value_type::version, "user", &value_type::user, "id", &value_type::id, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::input_event_data> {
		using value_type				 = discord_core_api::input_event_data;
		static constexpr auto parseValue = createValue("response_type", &value_type::responseType, "interaction_data", &value_type::interactionData);
	};

	template<> struct core<discord_core_api::thread_members_update_data> {
		using value_type				 = discord_core_api::thread_members_update_data;
		static constexpr auto parseValue = createValue("added_members", &value_type::addedMembers, "member_count", &value_type::memberCount, "guild_id", &value_type::guildId,
			"removed_member_ids", &value_type::removedMemberIds);
	};

	template<> struct core<discord_core_api::web_hook_data> {
		using value_type				 = discord_core_api::web_hook_data;
		static constexpr auto parseValue = createValue("application_id", &value_type::applicationId, "avatar", &value_type::avatar, "channel_id", &value_type::channelId,
			"guild_id", &value_type::guildId, "id", &value_type::id, "name", &value_type::name, "source_channel", &value_type::sourceChannel, "source_guild",
			&value_type::sourceGuild, "token", &value_type::token, "type", &value_type::type, "url", &value_type::url, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::audit_log_change_data> {
		using value_type				 = discord_core_api::audit_log_change_data;
		static constexpr auto parseValue = createValue("key", &value_type::key, "new_value", &value_type::newValue, "old_value", &value_type::oldValue);
	};

	template<> struct core<discord_core_api::optional_audit_entry_info_data> {
		using value_type				 = discord_core_api::optional_audit_entry_info_data;
		static constexpr auto parseValue = createValue("delete_member_days", &value_type::deleteMemberDays, "members_removed", &value_type::membersRemoved, "application_id",
			&value_type::applicationId, "role_name", &value_type::roleName, "channel_id", &value_type::channelId, "message_id", &value_type::messageId, "count", &value_type::count,
			"type", &value_type::type);
	};

	template<> struct core<discord_core_api::archived_threads_data> {
		using value_type				 = discord_core_api::archived_threads_data;
		static constexpr auto parseValue = createValue("has_more", &value_type::hasMore, "member", &value_type::members, "threads", &value_type::threads);
	};

	template<> struct core<discord_core_api::connection_data> {
		using value_type				 = discord_core_api::connection_data;
		static constexpr auto parseValue = createValue("friend_sync", &value_type::friendSync, "id", &value_type::id, "integrations", &value_type::integrations, "name",
			&value_type::name, "revoked", &value_type::revoked, "show_activity", &value_type::showActivity, "type", &value_type::type, "verified", &value_type::verified,
			"visibility", &value_type::visibility);
	};

	template<> struct core<discord_core_api::guild_prune_count_data> {
		using value_type				 = discord_core_api::guild_prune_count_data;
		static constexpr auto parseValue = createValue("count", &value_type::count);
	};

	template<> struct core<discord_core_api::thread_data> {
		using value_type				 = discord_core_api::thread_data;
		static constexpr auto parseValue = createValue("application_id", &value_type::applicationId, "applied_tags", &value_type::appliedTags, "available_tags",
			&value_type::availableTags, "guild_id", &value_type::guildId, "bitrate", &value_type::bitrate, "default_auto_archive_duration", &value_type::defaultAutoArchiveDuration,
			"default_reaction_emoji", &value_type::defaultReactionEmoji, "flags", &value_type::flags, "icon", &value_type::icon, "id", &value_type::id, "last_message_id",
			&value_type::lastMessageId, "last_pin_timestamp", &value_type::lastPinTimestamp, "member", &value_type::member, "member_count", &value_type::memberCount,
			"message_count", &value_type::messageCount, "name", &value_type::name, "owner_id", &value_type::ownerId, "parent_id", &value_type::parentId, "permission_overwrites",
			&value_type::permissionOverwrites, "permissions", &value_type::permissions, "position", &value_type::position, "rate_limit_per_user", &value_type::rateLimitPerUser,
			"recipients", &value_type::recipients, "rtc_region", &value_type::rtcRegion, "thread_metadata", &value_type::threadMetadata, "topic", &value_type::topic,
			"total_message_sent", &value_type::totalMessageSent, "type", &value_type::type, "user_limit", &value_type::userLimit, "video_quality_mode",
			&value_type::videoQualityMode);
	};

	template<> struct core<discord_core_api::over_write_data> {
		using value_type				 = discord_core_api::over_write_data;
		static constexpr auto parseValue = createValue("allow", &value_type::allow, "deny", &value_type::deny, "id", &value_type::id, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::forum_tag_data> {
		using value_type				 = discord_core_api::forum_tag_data;
		static constexpr auto parseValue = createValue("emoji_id", &value_type::emojiId, "emoji_name", &value_type::emojiName, "id", &value_type::id, "moderated",
			&value_type::moderated, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::default_reaction_data> {
		using value_type				 = discord_core_api::default_reaction_data;
		static constexpr auto parseValue = createValue("emoji_id", &value_type::emojiId, "emoji_name", &value_type::emojiName);
	};

	template<> struct core<discord_core_api::voice_region_data> {
		using value_type = discord_core_api::voice_region_data;
		static constexpr auto parseValue =
			createValue("custom", &value_type::custom, "depracated", &value_type::deprecated, "id", &value_type::id, "name", &value_type::name, "optimal", &value_type::optimal);
	};

	template<> struct core<discord_core_api::invite_data> {
		using value_type = discord_core_api::invite_data;
		static constexpr auto parseValue =
			createValue("approximate_member_count", &value_type::approximateMemberCount, "approximate_presence_count", &value_type::approximatePresenceCount, "channel",
				&value_type::channel, "guild_id", &value_type::guildId, "code", &value_type::code, "created_at", &value_type::createdAt, "expires_at", &value_type::expiresAt,
				"guild", &value_type::guild, "guild_scheduled_event", &value_type::guildScheduledEvent, "inviter", &value_type::inviter, "max_age", &value_type::maxAge, "max_uses",
				&value_type::maxUses, "stage_instance", &value_type::stageInstance, "target_application", &value_type::targetApplication, "target_type", &value_type::targetType,
				"target_user", &value_type::targetUser, "temporary", &value_type::temporary, "uses", &value_type::uses);
	};

	template<> struct core<discord_core_api::auto_moderation_rule_data> {
		using value_type				 = discord_core_api::auto_moderation_rule_data;
		static constexpr auto parseValue = createValue("actions", &value_type::actions, "creator_id", &value_type::creatorId, "enabled", &value_type::enabled, "guild_id",
			&value_type::guildId, "event_type", &value_type::eventType, "exempt_channels", &value_type::exemptChannels, "exempt_roles", &value_type::exemptRoles, "id",
			&value_type::id, "trigger_meta_data", &value_type::triggerMetaData, "trigger_type", &value_type::triggerType);
	};

	template<> struct core<discord_core_api::application_command_data> {
		using value_type				 = discord_core_api::application_command_data;
		static constexpr auto parseValue = createValue("application_id", &value_type::applicationId, "default_member_permissions", &value_type::defaultMemberPermissions,
			"description", &value_type::description, "guild_id", &value_type::guildId, "description_localizations", &value_type::descriptionLocalizations, "dm_permission",
			&value_type::dmPermission, "id", &value_type::id, "name", &value_type::name, "name_localizations", &value_type::nameLocalizations, "options", &value_type::options,
			"type", &value_type::type, "version", &value_type::version);
	};

	template<> struct core<discord_core_api::sticker_data> {
		using value_type				 = discord_core_api::sticker_data;
		static constexpr auto parseValue = createValue("asset", &value_type::asset, "description", &value_type::description, "format_type", &value_type::formatType, "id",
			&value_type::id, "name", &value_type::name, "pack_id", &value_type::packId, "tags", &value_type::tags, "type", &value_type::type, "user", &value_type::user, "guild_id",
			&value_type::guildId, "sort_value", &value_type::sortValue, "nsfw_level", &value_type::nsfwLevel, "flags", &value_type::flags);
	};

	template<> struct core<discord_core_api::message_activity_data> {
		using value_type				 = discord_core_api::message_activity_data;
		static constexpr auto parseValue = createValue("type", &value_type::type, "party_id", &value_type::partyId);
	};

	template<> struct core<discord_core_api::active_threads_data> {
		using value_type				 = discord_core_api::active_threads_data;
		static constexpr auto parseValue = createValue("has_more", &value_type::hasMore, "members", &value_type::members, "threads", &value_type::threads);
	};

	template<> struct core<discord_core_api::client_status> {
		using value_type				 = discord_core_api::client_status;
		static constexpr auto parseValue = createValue("web", &value_type::web, "mobile", &value_type::mobile, "desktop", &value_type::desktop);
	};

	template<> struct core<discord_core_api::activity_data> {
		using value_type				 = discord_core_api::activity_data;
		static constexpr auto parseValue = createValue("name", &value_type::name, "type", &value_type::type, "url", &value_type::url, "state", &value_type::state, "created_at",
			&value_type::createdAt, "details", &value_type::details, "application_id", &value_type::applicationId);
	};

	template<> struct core<discord_core_api::presence_update_data> {
		using value_type				 = discord_core_api::presence_update_data;
		static constexpr auto parseValue = createValue("client_status", &value_type::clientStatus, "guild_id", &value_type::guildId, "status", &value_type::status, "user",
			&value_type::user, "activities", &value_type::activities);
	};

	template<> struct core<discord_core_api::account_data> {
		using value_type				 = discord_core_api::account_data;
		static constexpr auto parseValue = createValue("name", &value_type::name);
	};

	template<> struct core<discord_core_api::embed_data> {
		using value_type				 = discord_core_api::embed_data;
		static constexpr auto parseValue = createValue("author", &value_type::author, "color", &value_type::hexColorValue, "description", &value_type::description, "fields",
			&value_type::fields, "footer", &value_type::footer, "image", &value_type::image, "provider", &value_type::provider, "thumbnail", &value_type::thumbnail, "timestamp",
			&value_type::timeStamp, "title", &value_type::title, "type", &value_type::type, "url", &value_type::url, "video", &value_type::video);
	};

	template<> struct core<discord_core_api::reaction_data> {
		using value_type = discord_core_api::reaction_data;
		static constexpr auto parseValue =
			createValue("channel_id", &value_type::channelId, "count", &value_type::count, "emoji", &value_type::emoji, "guild_id", &value_type::guildId, "id", &value_type::id,
				"member", &value_type::member, "me", &value_type::me, "message_id", &value_type::messageId, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::sticker_item_data> {
		using value_type				 = discord_core_api::sticker_item_data;
		static constexpr auto parseValue = createValue("format_type", &value_type::formatType, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::attachment_data> {
		using value_type				 = discord_core_api::attachment_data;
		static constexpr auto parseValue = createValue("content_type", &value_type::contentType, "description", &value_type::description, "filename", &value_type::filename,
			"height", &value_type::height, "id", &value_type::id, "proxy_url", &value_type::proxyUrl, "size", &value_type::size, "url", &value_type::url, "width",
			&value_type::width, "ephemeral", &value_type::ephemeral);
	};

	template<> struct core<discord_core_api::channel_mention_data> {
		using value_type				 = discord_core_api::channel_mention_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "guild_id", &value_type::guildId, "name", &value_type::name, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::select_default_value_data> {
		using value_type				 = discord_core_api::select_default_value_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::action_row_data> {
		using value_type				 = discord_core_api::action_row_data;
		static constexpr auto parseValue = createValue("components", &value_type::components, "type", &value_type::type);
	};

	template<> struct core<discord_core_api::embed_footer_data> {
		using value_type				 = discord_core_api::embed_footer_data;
		static constexpr auto parseValue = createValue("proxy_icon_url", &value_type::proxyIconUrl, "icon_url", &value_type::iconUrl, "text", &value_type::text);
	};

	template<> struct core<discord_core_api::embed_image_data> {
		using value_type				 = discord_core_api::embed_image_data;
		static constexpr auto parseValue = createValue("proxy_url", &value_type::proxyUrl, "url", &value_type::url, "height", &value_type::height, "width", &value_type::width);
	};

	template<> struct core<discord_core_api::embed_thumbnail_data> {
		using value_type				 = discord_core_api::embed_thumbnail_data;
		static constexpr auto parseValue = createValue("proxy_url", &value_type::proxyUrl, "url", &value_type::url, "height", &value_type::height, "width", &value_type::width);
	};

	template<> struct core<discord_core_api::embed_video_data> {
		using value_type				 = discord_core_api::embed_video_data;
		static constexpr auto parseValue = createValue("proxy_url", &value_type::proxyUrl, "url", &value_type::url, "height", &value_type::height, "width", &value_type::width);
	};

	template<> struct core<discord_core_api::embed_provider_data> {
		using value_type				 = discord_core_api::embed_provider_data;
		static constexpr auto parseValue = createValue("name", &value_type::name, "url", &value_type::url);
	};

	template<> struct core<discord_core_api::embed_author_data> {
		using value_type = discord_core_api::embed_author_data;
		static constexpr auto parseValue =
			createValue("proxy_icon_url", &value_type::proxyIconUrl, "icon_url", &value_type::iconUrl, "name", &value_type::name, "url", &value_type::url);
	};

	template<> struct core<discord_core_api::embed_field_data> {
		using value_type				 = discord_core_api::embed_field_data;
		static constexpr auto parseValue = createValue("name", &value_type::name, "value", &value_type::value, "inline", &value_type::Inline);
	};

	template<> struct core<discord_core_api::guild_member_cache_data> {
		using value_type				 = discord_core_api::guild_member_cache_data;
		static constexpr auto parseValue = createValue("avatar", &value_type::avatar, "flags", &value_type::flags, "guild_id", &value_type::guildId, "joined_at",
			&value_type::joinedAt, "nick", &value_type::nick, "permissions", &value_type::permissionsVal, "roles", &value_type::roles, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::audit_log_entry_data> {
		using value_type				 = discord_core_api::audit_log_entry_data;
		static constexpr auto parseValue = createValue("action_type", &value_type::actionType, "changes", &value_type::changes, "created_at", &value_type::createdTimeStamp, "id",
			&value_type::id, "options", &value_type::options, "target_id", &value_type::targetId, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::component_data> {
		using value_type				 = discord_core_api::component_data;
		static constexpr auto parseValue = createValue("type", &value_type::type, "custom_id", &value_type::customId, "options", &value_type::options, "channel_types",
			&value_type::channelTypes, "placeholder", &value_type::placeholder, "min_values", &value_type::minValues, "max_values", &value_type::maxValues, "min_length",
			&value_type::minLength, "max_length", &value_type::maxLength, "label", &value_type::label, "value", &value_type::value, "title", &value_type::title, "emoji",
			&value_type::emoji, "url", &value_type::url, "required", &value_type::required, "disabled", &value_type::disabled, "style", &value_type::style);
	};

	template<> struct core<discord_core_api::select_option_data> {
		using value_type				 = discord_core_api::select_option_data;
		static constexpr auto parseValue = createValue("description", &value_type::description, "label", &value_type::label, "value", &value_type::value, "emoji",
			&value_type::emoji, "default", &value_type::_default);
	};

	template<> struct core<discord_core_api::voice_state_data_light> {
		using value_type				 = discord_core_api::voice_state_data_light;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::guild_data> {
		using value_type				 = discord_core_api::guild_data;
		static constexpr auto parseValue = createValue("default_message_notifications", &value_type::defaultMessageNotifications, "guild_scheduled_events",
			&value_type::guildScheduledEvents, "explicit_content_filter", &value_type::explicitContentFilter, "stage_instances", &value_type::stageInstances, "presences",
			&value_type::presences, "system_channel_flags", &value_type::systemChannelFlags, "widget_enabled", &value_type::widgetEnabled, "unavailable", &value_type::unavailable,
			"owner", &value_type::owner, "large", &value_type::large, "member_count", &value_type::memberCount, "verification_level", &value_type::verificationLevel, "id",
			&value_type::id, "channels", &value_type::channels, "roles", &value_type::roles, "members", &value_type::members, "voice_states", &value_type::voiceStates, "owner_id",
			&value_type::ownerId, "permissions", &value_type::permissions, "afk_channel_id", &value_type::afkChannelId, "features", &value_type::features, "stickers",
			&value_type::stickers, "max_stage_video_channel_users", &value_type::maxStageVideoChannelUsers, "public_updates_channel_id", &value_type::publicUpdatesChannelId,
			"premium_subscription_count", &value_type::premiumSubscriptionCount, "approximate_presence_count", &value_type::approximatePresenceCount, "threads",
			&value_type::threads, "welcome_screen", &value_type::welcomeScreen, "safety_alerts_channel_id", &value_type::safetyAlertsChannelId, "approximate_member_count",
			&value_type::approximateMemberCount, "premium_progress_bar_enabled", &value_type::premiumProgressBarEnabled, "max_video_channel_users",
			&value_type::maxVideoChannelUsers, "emoji", &value_type::emoji, "preferred_locale", &value_type::preferredLocale, "vanity_url_code", &value_type::vanityUrlCode,
			"system_channel_id", &value_type::systemChannelId, "discovery_splash", &value_type::discoverySplash, "widget_channel_id", &value_type::widgetChannelId,
			"rules_channel_id", &value_type::rulesChannelId, "nsfw_level", &value_type::nsfwLevel, "application_id", &value_type::applicationId, "description",
			&value_type::description, "premium_tier", &value_type::premiumTier, "afk_timeout", &value_type::afkTimeout, "max_members", &value_type::maxMembers, "mfa_level",
			&value_type::mfaLevel, "splash", &value_type::splash, "name", &value_type::name, "banner", &value_type::banner, "icon", &value_type::icon);
	};

	template<> struct core<discord_core_api::partial_guild_data> {
		using value_type				 = discord_core_api::partial_guild_data;
		static constexpr auto parseValue = createValue("unavailable", &value_type::unavailable, "id", &value_type::id);
	};

	template<> struct core<discord_core_api::integration_data> {
		using value_type				 = discord_core_api::integration_data;
		static constexpr auto parseValue = createValue("account", &value_type::account, "application", &value_type::application, "enabled", &value_type::enabled,
			"enable_emoticons", &value_type::enableEmoticons, "expire_behavior", &value_type::expireBehavior, "expire_grace_period", &value_type::expireGracePeriod, "id",
			&value_type::id, "name", &value_type::name, "revoked", &value_type::revoked, "role_id", &value_type::roleId, "subscriber_count", &value_type::subscriberCount,
			"syncing", &value_type::syncing, "synced_at", &value_type::syncedAt, "type", &value_type::type, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::file> {
		using value_type				 = discord_core_api::file;
		static constexpr auto parseValue = createValue(&value_type::data);
	};

	template<> struct core<discord_core_api::allowed_mentions_data> {
		using value_type = discord_core_api::allowed_mentions_data;
		static constexpr auto parseValue =
			createValue("replied_user", &value_type::repliedUser, "parse", &value_type::parse, "roles", &value_type::roles, "users", &value_type::users);
	};

	template<> struct core<discord_core_api::message_reference_data> {
		using value_type				 = discord_core_api::message_reference_data;
		static constexpr auto parseValue = createValue("fail_if_not_exists", &value_type::failIfNotExists, "message_id", &value_type::messageId, "channel_id",
			&value_type::channelId, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::message_interaction_data> {
		using value_type = discord_core_api::message_interaction_data;
		static constexpr auto parseValue =
			createValue("id", &value_type::id, "member", &value_type::member, "name", &value_type::name, "type", &value_type::type, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::guild_scheduled_event_data> {
		using value_type				 = discord_core_api::guild_scheduled_event_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "privacy_level", &value_type::privacyLevel, "entity_metadata", &value_type::entityMetadata,
			"entity_type", &value_type::entityType, "status", &value_type::status, "scheduled_start_time", &value_type::scheduledStartTime, "scheduled_end_time",
			&value_type::scheduledEndTime, "description", &value_type::description, "creator_id", &value_type::creatorId, "channel_id", &value_type::channelId, "entity_id",
			&value_type::entityId, "user_count", &value_type::userCount, "guild_id", &value_type::guildId, "creator", &value_type::creator, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::guild_scheduled_event_user_data> {
		using value_type				 = discord_core_api::guild_scheduled_event_user_data;
		static constexpr auto parseValue = createValue("guild_scheduled_event_id", &value_type::guildScheduledEventId, "member", &value_type::member, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::guild_scheduled_event_metadata> {
		using value_type				 = discord_core_api::guild_scheduled_event_metadata;
		static constexpr auto parseValue = createValue("location", &value_type::location);
	};

	template<> struct core<discord_core_api::role_data> {
		using value_type				 = discord_core_api::role_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "name", &value_type::name, "color", &value_type::color, "hoist", &value_type::hoist, "icon",
			&value_type::icon, "unicode_emoji", &value_type::unicodeEmoji, "position", &value_type::position, "permissions", &value_type::permissions, "managed",
			&value_type::managed, "mentionable", &value_type::mentionable, "tags", &value_type::tags, "flags", &value_type::flags);
	};

	template<> struct core<discord_core_api::message_data> {
		using value_type				 = discord_core_api::message_data;
		static constexpr auto parseValue = createValue("activity", &value_type::activity, "application", &value_type::application, "application_id", &value_type::applicationId,
			"attachments", &value_type::attachments, "author", &value_type::author, "channel_id", &value_type::channelId, "components", &value_type::components, "content",
			&value_type::content, "edited_timestamp", &value_type::editedTimestamp, "embeds", &value_type::embeds, "flags", &value_type::flags, "guild_id", &value_type::guildId,
			"id", &value_type::id, "interaction", &value_type::interaction, "member", &value_type::member, "mention_channels", &value_type::mentionChannels, "mention_everyone",
			&value_type::mentionEveryone, "mention_roles", &value_type::mentionRoles, "mentions", &value_type::mentions, "message_reference", &value_type::messageReference,
			"nonce", &value_type::nonce, "pinned", &value_type::pinned, "reactions", &value_type::reactions, "sticker_items", &value_type::stickerItems, "stickers",
			&value_type::stickers, "thread", &value_type::thread, "timestamp", &value_type::timeStamp, "tts", &value_type::tts, "type", &value_type::type, "webhook_id",
			&value_type::webHookId);
	};

	template<> struct core<discord_core_api::sticker_pack_data> {
		using value_type				 = discord_core_api::sticker_pack_data;
		static constexpr auto parseValue = createValue("banner_asset_id", &value_type::bannerAssetId, "cover_sticker_id", &value_type::coverStickerId, "description",
			&value_type::description, "id", &value_type::id, "name", &value_type::name, "sku_id", &value_type::skuId, "stickers", &value_type::stickers);
	};

	template<> struct core<discord_core_api::guild_widget_image_data> {
		using value_type				 = discord_core_api::guild_widget_image_data;
		static constexpr auto parseValue = createValue("url", &value_type::url);
	};

	template<> struct core<discord_core_api::guild_template_data> {
		using value_type				 = discord_core_api::guild_template_data;
		static constexpr auto parseValue = createValue("code", &value_type::code, "created_at", &value_type::createdAt, "creator", &value_type::creator, "creator_id",
			&value_type::creatorId, "description", &value_type::description, "is_dirty", &value_type::isDirty, "name", &value_type::name, "serialized_source_guild",
			&value_type::serializedSourceGuild, "source_guild_id", &value_type::sourceGuildId, "updated_at", &value_type::updatedAt);
	};

	template<> struct core<discord_core_api::application_command_option_data> {
		using value_type				 = discord_core_api::application_command_option_data;
		static constexpr auto parseValue = createValue("type", &value_type::type, "name", &value_type::name, "description", &value_type::description, "required",
			&value_type::required, "autocomplete", &value_type::autocomplete, "min_value", &value_type::minValue, "max_value", &value_type::maxValue, "channel_types",
			&value_type::channelTypes, "description_localizations", &value_type::descriptionLocalizations, "name_localizations", &value_type::nameLocalizations, "choices",
			&value_type::choices, "options", &value_type::options);
	};

	template<> struct core<discord_core_api::stage_instance_data> {
		using value_type				 = discord_core_api::stage_instance_data;
		static constexpr auto parseValue = createValue("privacy_level", &value_type::privacyLevel, "discoverable_disabled", &value_type::discoverableDisabled, "channel_id",
			&value_type::channelId, "guild_id", &value_type::guildId, "topic", &value_type::topic);
	};

	template<> struct core<discord_core_api::application_command_option_choice_data> {
		using value_type				 = discord_core_api::application_command_option_choice_data;
		static constexpr auto parseValue = createValue("name", &value_type::name, "value", &value_type::value, "name_localizations", &value_type::nameLocalizations);
	};

	template<> struct core<discord_core_api::role_tags_data> {
		using value_type = discord_core_api::role_tags_data;
		static constexpr auto parseValue =
			createValue("premium_subscriber", &value_type::premiumSubscriber, "integration_id", &value_type::integrationId, "bot_id", &value_type::botId);
	};

	template<> struct core<discord_core_api::discord_core_internal::connect_properties> {
		using value_type				 = discord_core_api::discord_core_internal::connect_properties;
		static constexpr auto parseValue = createValue("os", &value_type::os, "device", &value_type::device, "browser", &value_type::browser);
	};

	template<> struct core<discord_core_api::discord_core_internal::websocket_identify_data> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_identify_data;
		static constexpr auto parseValue = createValue("token", &value_type::botToken, "shard", &value_type::shard, "intents", &value_type::intents, "large_threshold",
			&value_type::largeThreshold, "presence", &value_type::presence, "properties", &value_type::properties);
	};

	template<> struct core<discord_core_api::discord_core_internal::websocket_resume_data> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_resume_data;
		static constexpr auto parseValue = createValue("token", &value_type::botToken, "session_id", &value_type::sessionId, "seq", &value_type::lastNumberReceived);
	};

	template<> struct core<discord_core_api::discord_core_internal::voice_socket_protocol_payload_data_data> {
		using value_type				 = discord_core_api::discord_core_internal::voice_socket_protocol_payload_data_data;
		static constexpr auto parseValue = createValue("address", &value_type::address, "mode", &value_type::mode, "port", &value_type::port);
	};

	template<> struct core<discord_core_api::discord_core_internal::voice_socket_protocol_payload_data> {
		using value_type				 = discord_core_api::discord_core_internal::voice_socket_protocol_payload_data;
		static constexpr auto parseValue = createValue("data", &value_type::data, "protocol", &value_type::protocol);
	};

	template<> struct core<discord_core_api::discord_core_internal::voice_identify_data> {
		using value_type = discord_core_api::discord_core_internal::voice_identify_data;
		static constexpr auto parseValue =
			createValue("session_id", &value_type::sessionId, "user_id", &value_type::userId, "token", &value_type::token, "server_id", &value_type::serverId);
	};

	template<> struct core<discord_core_api::discord_core_internal::send_speaking_data> {
		using value_type				 = discord_core_api::discord_core_internal::send_speaking_data;
		static constexpr auto parseValue = createValue("speaking", &value_type::type, "delay", &value_type::delay, "ssrc", &value_type::ssrc);
	};

	template<> struct core<discord_core_api::discord_core_internal::hello_data> {
		using value_type				 = discord_core_api::discord_core_internal::hello_data;
		static constexpr auto parseValue = createValue("heartbeat_interval", &value_type::heartbeatInterval, "_trace", &value_type::_trace);
	};

	template<> struct core<discord_core_api::update_presence_data> {
		using value_type = discord_core_api::update_presence_data;
		static constexpr auto parseValue =
			createValue("afk", &value_type::afk, "since", &value_type::since, "status", &value_type::statusReal, "activities", &value_type::activities);
	};

	template<typename value_type> struct core<discord_core_api::event_data<value_type>> {
		using value_type02				 = discord_core_api::event_data<value_type>;
		static constexpr auto parseValue = createValue("d", &value_type02::value);
	};

	template<typename value_type, typename value_type01> struct core<discord_core_api::updated_event_data<value_type, value_type01>> {
		using value_type02				 = discord_core_api::updated_event_data<value_type, value_type01>;
		static constexpr auto parseValue = createValue("d", &value_type02::value);
	};
}
