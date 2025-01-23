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

namespace jsonifier::internal {

	template<typename value_type>
	concept snowflake_t = std::same_as<discord_core_api::snowflake, std::remove_cvref_t<value_type>>;

	template<snowflake_t value_type, typename context_type, serialize_options options, typename json_entity_type>
	struct serialize_impl<value_type, context_type, options, json_entity_type> {
		template<typename value_type_new> JSONIFIER_INLINE static void impl(value_type_new&& value, context_type& context) noexcept {
			jsonifier::string newString{ static_cast<jsonifier::string>(value) };
			serialize<options, json_entity_type>::impl(newString, context);
		}
	};

	template<snowflake_t value_type, typename context_type, parse_options optionsNew, bool minified>
	struct parse_impl<value_type, context_type, optionsNew, minified> {
		static constexpr parse_options options{ optionsNew };
		using base = derailleur<options, context_type>;
		JSONIFIER_INLINE static void impl(value_type& value, context_type& context) noexcept {
			jsonifier::raw_json_data newString{};
			parse<options, minified>::impl(newString, context);
			if (newString.getType() == jsonifier::json_type::string) {
				value = newString.getString();
			} else {
				value = newString.getUint();
			}
		}
	};

	template<typename value_type>
	concept time_stamp_t = std::same_as<discord_core_api::time_stamp, std::remove_cvref_t<value_type>>;

	template<time_stamp_t value_type, typename context_type, serialize_options options, typename json_entity_type>
	struct serialize_impl<value_type, context_type, options, json_entity_type> {
		template<typename value_type_new> JSONIFIER_INLINE static void impl(value_type_new&& value, context_type& context) noexcept {
			jsonifier::string newString{ static_cast<jsonifier::string>(value) };
			serialize<options, json_entity_type>::impl(newString, context);
		}
	};

	template<time_stamp_t value_type, typename context_type, parse_options optionsNew, bool minified>
	struct parse_impl<value_type, context_type, optionsNew, minified> {
		static constexpr parse_options options{ optionsNew };
		using base = derailleur<options, context_type>;
		JSONIFIER_INLINE static void impl(value_type& value, context_type& context) noexcept {
			jsonifier::raw_json_data newString{};
			parse<options, minified>::impl(newString, context);
			if (newString.getType() == jsonifier::json_type::string) {
				value = newString.getString();
			} else {
				value = newString.getUint();
			}
		}
	};
}

namespace jsonifier {

	template<> struct core<discord_core_api::ready_data> {
		using value_type				 = discord_core_api::ready_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::resumeGatewayUrl, "resume_gateway_url">(), makeJsonEntity<&value_type::shard, "shard">(),
			makeJsonEntity<&value_type::v, "v">(), makeJsonEntity<&value_type::sessionId, "session_id">(), makeJsonEntity<&value_type::user, "user">(),
			makeJsonEntity<&value_type::application, "application">(), makeJsonEntity<&value_type::trace, "_trace">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::websocket_message> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_message;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::op, "op">(), makeJsonEntity<&value_type::s, "s">(), makeJsonEntity<&value_type::t, "t">()>();
	};

	template<typename oty2> struct core<discord_core_api::discord_core_internal::websocket_message_data<oty2>> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_message_data<oty2>;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::d, "d">(), makeJsonEntity<&value_type::op, "op">(), makeJsonEntity<&value_type::s, "s">()>();
	};

	template<> struct core<discord_core_api::application_command_permission_data> {
		using value_type = discord_core_api::application_command_permission_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::permission, "permission">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::guild_application_command_permissions_data> {
		using value_type				 = discord_core_api::guild_application_command_permissions_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::applicationId, "application_id">(), makeJsonEntity<&value_type::permissions, "permissions">(),
			makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::id, "id">()>();
	};

	template<> struct core<discord_core_api::session_start_data> {
		using value_type				 = discord_core_api::session_start_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::maxConcurrency, "max_concurrency">(), makeJsonEntity<&value_type::remaining, "remaining">(),
			makeJsonEntity<&value_type::resetAfter, "reset_after">(), makeJsonEntity<&value_type::total, "total">()>();
	};

	template<> struct core<discord_core_api::authorization_info_data> {
		using value_type				 = discord_core_api::authorization_info_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::application, "application">(), makeJsonEntity<&value_type::expires, "expires">(),
			makeJsonEntity<&value_type::scopes, "scopes">(), makeJsonEntity<&value_type::user, "user">()>();
	};


	template<> struct core<discord_core_api::trigger_meta_data> {
		using value_type				 = discord_core_api::trigger_meta_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::allowList, "allow_list">(), makeJsonEntity<&value_type::keywordFilter, "keyword_filter">(),
			makeJsonEntity<&value_type::mentionTotalLimit, "mention_total_limit">(), makeJsonEntity<&value_type::presets, "presets">()>();
	};

	template<> struct core<discord_core_api::partial_emoji_data> {
		using value_type = discord_core_api::partial_emoji_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::animated, "animated">()>();
	};

	template<> struct core<discord_core_api::emoji_data> {
		using value_type				 = discord_core_api::emoji_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::name, "name">(),
			makeJsonEntity<&value_type::roles, "roles">(), makeJsonEntity<&value_type::requireColons, "require_colons">(), makeJsonEntity<&value_type::managed, "managed">(),
			makeJsonEntity<&value_type::animated, "animated">(), makeJsonEntity<&value_type::available, "available">()>();
	};

	template<> struct core<discord_core_api::team_members_object_data> {
		using value_type				 = discord_core_api::team_members_object_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::membershipState, "membership_state">(),
			makeJsonEntity<&value_type::permissions, "permissions">(), makeJsonEntity<&value_type::teamId, "team_id">(), makeJsonEntity<&value_type::user, "user">()>();
	};

	template<> struct core<discord_core_api::team_object_data> {
		using value_type				 = discord_core_api::team_object_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::icon, "icon">(), makeJsonEntity<&value_type::id, "id">(),
			makeJsonEntity<&value_type::members, "members">(), makeJsonEntity<&value_type::ownerUserId, "owner_user_id">()>();
	};

	template<> struct core<discord_core_api::install_params_data> {
		using value_type				 = discord_core_api::install_params_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::scopes, "scopes">(), makeJsonEntity<&value_type::permissions, "permissions">()>();
	};

	template<> struct core<discord_core_api::application_data> {
		using value_type				 = discord_core_api::application_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::botPublic, "bot_public">(),
			makeJsonEntity<&value_type::botRequireCodeGrant, "bot_require_code_grant">(), makeJsonEntity<&value_type::coverImage, "cover_image">(),
			makeJsonEntity<&value_type::customInstallUrl, "custom_install_url">(), makeJsonEntity<&value_type::description, "description">(),
			makeJsonEntity<&value_type::flags, "flags">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::icon, "icon">(),
			makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::owner, "owner">(), makeJsonEntity<&value_type::params, "params">(),
			makeJsonEntity<&value_type::primarySkuId, "primary_sku_id">(), makeJsonEntity<&value_type::privacyPolicyUrl, "privacy_policy_url">(),
			makeJsonEntity<&value_type::rpcOrigins, "rpc_origins">(), makeJsonEntity<&value_type::slug, "slug">(), makeJsonEntity<&value_type::summary, "summary">(),
			makeJsonEntity<&value_type::tags, "tags">(), makeJsonEntity<&value_type::team, "team">(), makeJsonEntity<&value_type::termsOfServiceUrl, "terms_of_service_url">(),
			makeJsonEntity<&value_type::verifyKey, "verify_key">()>();
	};

	template<> struct core<discord_core_api::thread_metadata_data> {
		using value_type				 = discord_core_api::thread_metadata_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::archived, "archived">(), makeJsonEntity<&value_type::archiveTimestamp, "archived_timestamp">(),
			makeJsonEntity<&value_type::autoArchiveDuration, "auto_archive_duration">(), makeJsonEntity<&value_type::invitable, "invitable">(),
			makeJsonEntity<&value_type::locked, "locked">()>();
	};

	template<> struct core<discord_core_api::gateway_bot_data> {
		using value_type				 = discord_core_api::gateway_bot_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::sessionStartLimit, "session_start_limit">(), makeJsonEntity<&value_type::shards, "shards">(),
			makeJsonEntity<&value_type::url, "url">()>();
	};

	template<> struct core<discord_core_api::user_id_base> {
		using value_type				 = discord_core_api::user_id_base;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::id, "id">()>();
	};

	template<> struct core<discord_core_api::user_data> {
		using value_type = discord_core_api::user_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::userName, "username">(), makeJsonEntity<&value_type::discriminator, "discriminator">(),
				makeJsonEntity<&value_type::globalName, "global_name">(), makeJsonEntity<&value_type::avatar, "avatar">(), makeJsonEntity<&value_type::bot, "bot">(),
				makeJsonEntity<&value_type::system, "system">(), makeJsonEntity<&value_type::mfaEnabled, "mfa_enabled">(), makeJsonEntity<&value_type::banner, "banner">(),
				makeJsonEntity<&value_type::accentColor, "accentColor">(), makeJsonEntity<&value_type::locale, "locale">(), makeJsonEntity<&value_type::verified, "verified">(),
				makeJsonEntity<&value_type::email, "email">(), makeJsonEntity<&value_type::flags, "flags">(), makeJsonEntity<&value_type::premiumType, "premium_type">(),
				makeJsonEntity<&value_type::publicFlags, "public_flags">(), makeJsonEntity<&value_type::avatarDecoration, "avatar_decoration">()>();
	};

	template<> struct core<discord_core_api::welcome_screen_channel_data> {
		using value_type				 = discord_core_api::welcome_screen_channel_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::description, "description">(),
			makeJsonEntity<&value_type::emojiId, "emoji_id">(), makeJsonEntity<&value_type::emojiName, "emoji_name">()>();
	};

	template<> struct core<discord_core_api::welcome_screen_data> {
		using value_type = discord_core_api::welcome_screen_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::description, "description">(), makeJsonEntity<&value_type::welcomeChannels, "welcome_screen_channels">()>();
	};

	template<> struct core<discord_core_api::action_meta_data> {
		using value_type = discord_core_api::action_meta_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::durationSeconds, "duration_seconds">()>();
	};

	template<> struct core<discord_core_api::action_data> {
		using value_type				 = discord_core_api::action_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::metadata, "meta_data">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::guild_preview_data> {
		using value_type				 = discord_core_api::guild_preview_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::approximateMemberCount, "approximate_member_count">(),
			makeJsonEntity<&value_type::approximatePresenceCount, "approximate_presence_count">(), makeJsonEntity<&value_type::description, "description">(),
			makeJsonEntity<&value_type::discoverySplash, "discovery_splash">(), makeJsonEntity<&value_type::emojis, "emojis">(),
			makeJsonEntity<&value_type::features, "features">(), makeJsonEntity<&value_type::icon, "icon">(), makeJsonEntity<&value_type::id, "id">(),
			makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::splash, "splash">(), makeJsonEntity<&value_type::stickers, "stickers">()>();
	};

	template<> struct core<discord_core_api::thread_member_data> {
		using value_type				 = discord_core_api::thread_member_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::flags, "flags">(), makeJsonEntity<&value_type::joinTimestamp, "join_timestamp">(),
			makeJsonEntity<&value_type::userId, "user_id">()>();
	};

	template<> struct core<discord_core_api::audit_log_data> {
		using value_type = discord_core_api::audit_log_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::auditLogEntries, "audit_log_entries">(), makeJsonEntity<&value_type::autoModerationRules, "auto_moderation_rules">(),
				makeJsonEntity<&value_type::guildScheduledEvents, "guild_scheduled_events">(), makeJsonEntity<&value_type::integrations, "integrations">(),
				makeJsonEntity<&value_type::threads, "threads">(), makeJsonEntity<&value_type::users, "users">(), makeJsonEntity<&value_type::webhooks, "webhooks">()>();
	};

	template<> struct core<discord_core_api::ban_data> {
		using value_type				 = discord_core_api::ban_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::failedDueToPerms, "failed_due_to_perms">(), makeJsonEntity<&value_type::user, "user">()>();
	};

	template<> struct core<discord_core_api::update_voice_state_data> {
		using value_type				 = discord_core_api::update_voice_state_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(),
			makeJsonEntity<&value_type::selfDeaf, "self_deaf">(), makeJsonEntity<&value_type::selfMute, "self_mute">()>();
	};	

	template<> struct core<discord_core_api::message_data> {
		using value_type				 = discord_core_api::message_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::mentionChannels, "mention_channels">(),
			makeJsonEntity<&value_type::mentionRoles, "mention_roles">(), makeJsonEntity<&value_type::stickerItems, "sticker_items">(),
			makeJsonEntity<&value_type::attachments, "attachments">(), makeJsonEntity<&value_type::components, "components">(),
			makeJsonEntity<&value_type::reactions, "reactions">(), makeJsonEntity<&value_type::stickers, "stickers">(),
			makeJsonEntity<&value_type::messageReference, "message_reference">(), makeJsonEntity<&value_type::mentions, "mentions">(),
			makeJsonEntity<&value_type::embeds, "embeds">(), makeJsonEntity<&value_type::interaction, "interaction">(),
			makeJsonEntity<&value_type::editedTimestamp, "edited_timestamp">(), makeJsonEntity<&value_type::activity, "activity">(),
			makeJsonEntity<&value_type::application, "application">(), makeJsonEntity<&value_type::content, "content">(), makeJsonEntity<&value_type::member, "member">(),
			makeJsonEntity<&value_type::applicationId, "application_id">(), makeJsonEntity<&value_type::nonce, "nonce">(), makeJsonEntity<&value_type::timeStamp, "timestamp">(),
			makeJsonEntity<&value_type::mentionEveryone, "mention_everyone">(), makeJsonEntity<&value_type::channelId, "channel_id">(),
			makeJsonEntity<&value_type::webHookId, "webhook_id">(), makeJsonEntity<&value_type::thread, "thread">(), makeJsonEntity<&value_type::guildId, "guild_id">(),
			makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::author, "author">(), makeJsonEntity<&value_type::flags, "flags">(),
			makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::pinned, "pinned">(), makeJsonEntity<&value_type::tts, "tts">()>();
	};

	template<> struct core<discord_core_api::update_voice_state_data_dc> {
		using value_type				 = discord_core_api::update_voice_state_data_dc;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(),
			makeJsonEntity<&value_type::selfDeaf, "self_deaf">(), makeJsonEntity<&value_type::selfMute, "self_mute">()>();
	};

	template<> struct core<discord_core_api::guild_widget_data> {
		using value_type				 = discord_core_api::guild_widget_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::enabled, "enabled">()>();
	};

	template<> struct core<discord_core_api::channel_data> {
		using value_type = discord_core_api::channel_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::permissionOverwrites, "permission_overwrites">(), makeJsonEntity<&value_type::defaultReactionEmoji, "default_reaction_emoji">(),
				makeJsonEntity<&value_type::availableTags, "available_tags">(), makeJsonEntity<&value_type::defaultThreadRateLimitPerUser, "default_thread_rate_limit_per_user">(),
				makeJsonEntity<&value_type::appliedTags, "applied_tags">(), makeJsonEntity<&value_type::defaultForumLayout, "default_forum_layout">(),
				makeJsonEntity<&value_type::defaultAutoArchiveDuration, "default_auto_archive_duration">(), makeJsonEntity<&value_type::threadMetadata, "thread_metadata">(),
				makeJsonEntity<&value_type::recipients, "recipients">(), makeJsonEntity<&value_type::defaultSortOrder, "default_sort_order">(),
				makeJsonEntity<&value_type::lastPinTimestamp, "last_pin_timestamp">(), makeJsonEntity<&value_type::lastMessageId, "last_message_id">(),
				makeJsonEntity<&value_type::applicationId, "application_id">(), makeJsonEntity<&value_type::totalMessageSent, "total_message_sent">(),
				makeJsonEntity<&value_type::rateLimitPerUser, "rate_limit_per_user">(), makeJsonEntity<&value_type::videoQualityMode, "video_quality_mode">(),
				makeJsonEntity<&value_type::member, "member">(), makeJsonEntity<&value_type::permissions, "permissions">(), makeJsonEntity<&value_type::rtcRegion, "rtc_region">(),
				makeJsonEntity<&value_type::messageCount, "message_count">(), makeJsonEntity<&value_type::ownerId, "owner_id">(),
				makeJsonEntity<&value_type::memberCount, "member_count">(), makeJsonEntity<&value_type::flags, "flags">(), makeJsonEntity<&value_type::topic, "topic">(),
				makeJsonEntity<&value_type::userLimit, "user_limit">(), makeJsonEntity<&value_type::parentId, "parent_id">(), makeJsonEntity<&value_type::type, "type">(),
				makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::position, "position">(), makeJsonEntity<&value_type::name, "name">(),
				makeJsonEntity<&value_type::icon, "icon">(), makeJsonEntity<&value_type::bitrate, "bitrate">(), makeJsonEntity<&value_type::id, "id">(),
				makeJsonEntity<&value_type::managed, "managed">(), makeJsonEntity<&value_type::nsfw, "nsfw">()>();
	};

	template<> struct core<discord_core_api::guild_member_data> {
		using value_type				 = discord_core_api::guild_member_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::user, "user">(), makeJsonEntity<&value_type::nick, "nick">(),
			makeJsonEntity<&value_type::avatar, "avatar">(), makeJsonEntity<&value_type::roles, "roles">(), makeJsonEntity<&value_type::joinedAt, "joined_at">(),
			makeJsonEntity<&value_type::premiumSince, "premium_since">(), makeJsonEntity<&value_type::deaf, "deaf">(), makeJsonEntity<&value_type::mute, "mute">(),
			makeJsonEntity<&value_type::flags, "flags">(), makeJsonEntity<&value_type::pending, "pending">(), makeJsonEntity<&value_type::permissions, "permissions">(),
			makeJsonEntity<&value_type::communicationDisabledUntil, "communication_disabled_until">(), makeJsonEntity<&value_type::guildId, "guild_id">()>();
	};

	template<> struct core<discord_core_api::thread_list_sync_data> {
		using value_type				 = discord_core_api::thread_list_sync_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::members, "members">(), makeJsonEntity<&value_type::channelIds, "channel_ids">(),
			makeJsonEntity<&value_type::threads, "threads">(), makeJsonEntity<&value_type::guildId, "guild_id">()>();
	};

	template<> struct core<discord_core_api::interaction_data_data> {
		using value_type				 = discord_core_api::interaction_data_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::options, "options">(), makeJsonEntity<&value_type::values, "values">(),
			makeJsonEntity<&value_type::components, "components">(), makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::componentType, "component_type">(),
			makeJsonEntity<&value_type::customId, "custom_id">(), makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::resolved, "resolved">(),
			makeJsonEntity<&value_type::targetId, "target_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::id, "id">()>();
	};

	template<> struct core<discord_core_api::resolved_data> {
		using value_type				 = discord_core_api::resolved_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::attachments, "attachments">(), makeJsonEntity<&value_type::members, "members">(),
			makeJsonEntity<&value_type::messages, "messages">(), makeJsonEntity<&value_type::channels, "channels">(), makeJsonEntity<&value_type::users, "users">(),
			makeJsonEntity<&value_type::roles, "roles">()>();
	};

	template<> struct core<discord_core_api::application_command_interaction_data_option> {
		using value_type				 = discord_core_api::application_command_interaction_data_option;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::type, "type">(),
			makeJsonEntity<&value_type::value, "value">(), makeJsonEntity<&value_type::focused, "focused">(), makeJsonEntity<&value_type::options, "options">()>();
	};

	template<> struct core<discord_core_api::interaction_data> {
		using value_type				 = discord_core_api::interaction_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::appPermissions, "app_permissions">(), makeJsonEntity<&value_type::guildLocale, "guild_locale">(),
			makeJsonEntity<&value_type::data, "data">(), makeJsonEntity<&value_type::locale, "locale">(), makeJsonEntity<&value_type::member, "member">(),
			makeJsonEntity<&value_type::applicationId, "application_id">(), makeJsonEntity<&value_type::token, "token">(), makeJsonEntity<&value_type::message, "message">(),
			makeJsonEntity<&value_type::channel, "channel">(), makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(),
			makeJsonEntity<&value_type::guild, "guild">(), makeJsonEntity<&value_type::version, "version">(), makeJsonEntity<&value_type::user, "user">(),
			makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::input_event_data> {
		using value_type = discord_core_api::input_event_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::responseType, "response_type">(), makeJsonEntity<&value_type::interactionData, "interaction_data">()>();
	};

	template<> struct core<discord_core_api::thread_members_update_data> {
		using value_type				 = discord_core_api::thread_members_update_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::addedMembers, "added_members">(), makeJsonEntity<&value_type::memberCount, "member_count">(),
			makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::removedMemberIds, "removed_member_ids">()>();
	};

	template<> struct core<discord_core_api::web_hook_data> {
		using value_type				 = discord_core_api::web_hook_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::applicationId, "application_id">(), makeJsonEntity<&value_type::avatar, "avatar">(),
			makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::id, "id">(),
			makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::sourceChannel, "source_channel">(), makeJsonEntity<&value_type::sourceGuild, "source_guild">(),
			makeJsonEntity<&value_type::token, "token">(), makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::url, "url">(),
			makeJsonEntity<&value_type::user, "user">()>();
	};

	template<> struct core<discord_core_api::audit_log_change_data> {
		using value_type = discord_core_api::audit_log_change_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::key, "key">(), makeJsonEntity<&value_type::newValue, "new_value">(), makeJsonEntity<&value_type::oldValue, "old_value">()>();
	};

	template<> struct core<discord_core_api::optional_audit_entry_info_data> {
		using value_type				 = discord_core_api::optional_audit_entry_info_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::deleteMemberDays, "delete_member_days">(),
			makeJsonEntity<&value_type::membersRemoved, "members_removed">(), makeJsonEntity<&value_type::applicationId, "application_id">(),
			makeJsonEntity<&value_type::roleName, "role_name">(), makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::messageId, "message_id">(),
			makeJsonEntity<&value_type::count, "count">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::archived_threads_data> {
		using value_type = discord_core_api::archived_threads_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::hasMore, "has_more">(), makeJsonEntity<&value_type::members, "member">(), makeJsonEntity<&value_type::threads, "threads">()>();
	};

	template<> struct core<discord_core_api::connection_data> {
		using value_type				 = discord_core_api::connection_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::friendSync, "friend_sync">(), makeJsonEntity<&value_type::id, "id">(),
			makeJsonEntity<&value_type::integrations, "integrations">(), makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::revoked, "revoked">(),
			makeJsonEntity<&value_type::showActivity, "show_activity">(), makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::verified, "verified">(),
			makeJsonEntity<&value_type::visibility, "visibility">()>();
	};

	template<> struct core<discord_core_api::guild_prune_count_data> {
		using value_type				 = discord_core_api::guild_prune_count_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::count, "count">()>();
	};

	template<> struct core<discord_core_api::thread_data> {
		using value_type				 = discord_core_api::thread_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::applicationId, "application_id">(), makeJsonEntity<&value_type::appliedTags, "applied_tags">(),
			makeJsonEntity<&value_type::availableTags, "available_tags">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::bitrate, "bitrate">(),
			makeJsonEntity<&value_type::defaultAutoArchiveDuration, "default_auto_archive_duration">(),
			makeJsonEntity<&value_type::defaultReactionEmoji, "default_reaction_emoji">(), makeJsonEntity<&value_type::flags, "flags">(),
			makeJsonEntity<&value_type::icon, "icon">(), makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::lastMessageId, "last_message_id">(),
			makeJsonEntity<&value_type::lastPinTimestamp, "last_pin_timestamp">(), makeJsonEntity<&value_type::member, "member">(),
			makeJsonEntity<&value_type::memberCount, "member_count">(), makeJsonEntity<&value_type::messageCount, "message_count">(), makeJsonEntity<&value_type::name, "name">(),
			makeJsonEntity<&value_type::ownerId, "owner_id">(), makeJsonEntity<&value_type::parentId, "parent_id">(),
			makeJsonEntity<&value_type::permissionOverwrites, "permission_overwrites">(), makeJsonEntity<&value_type::permissions, "permissions">(),
			makeJsonEntity<&value_type::position, "position">(), makeJsonEntity<&value_type::rateLimitPerUser, "rate_limit_per_user">(),
			makeJsonEntity<&value_type::recipients, "recipients">(), makeJsonEntity<&value_type::rtcRegion, "rtc_region">(),
			makeJsonEntity<&value_type::threadMetadata, "thread_metadata">(), makeJsonEntity<&value_type::topic, "topic">(),
			makeJsonEntity<&value_type::totalMessageSent, "total_message_sent">(), makeJsonEntity<&value_type::type, "type">(),
			makeJsonEntity<&value_type::userLimit, "user_limit">(), makeJsonEntity<&value_type::videoQualityMode, "video_quality_mode">()>();
	};

	template<> struct core<discord_core_api::over_write_data> {
		using value_type				 = discord_core_api::over_write_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::allow, "allow">(), makeJsonEntity<&value_type::deny, "deny">(),
			makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::forum_tag_data> {
		using value_type				 = discord_core_api::forum_tag_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::emojiId, "emoji_id">(), makeJsonEntity<&value_type::emojiName, "emoji_name">(),
			makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::moderated, "moderated">(), makeJsonEntity<&value_type::name, "name">()>();
	};

	template<> struct core<discord_core_api::default_reaction_data> {
		using value_type				 = discord_core_api::default_reaction_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::emojiId, "emoji_id">(), makeJsonEntity<&value_type::emojiName, "emoji_name">()>();
	};

	template<> struct core<discord_core_api::active_threads_data> {
		using value_type = discord_core_api::active_threads_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::hasMore, "has_more">(), makeJsonEntity<&value_type::members, "members">(), makeJsonEntity<&value_type::threads, "threads">()>();
	};

	template<> struct core<discord_core_api::client_status> {
		using value_type = discord_core_api::client_status;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::web, "web">(), makeJsonEntity<&value_type::mobile, "mobile">(), makeJsonEntity<&value_type::desktop, "desktop">()>();
	};

	template<> struct core<discord_core_api::activity_data> {
		using value_type				 = discord_core_api::activity_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::type, "type">(),
			makeJsonEntity<&value_type::url, "url">(), makeJsonEntity<&value_type::state, "state">(), makeJsonEntity<&value_type::createdAt, "created_at">(),
			makeJsonEntity<&value_type::details, "details">(), makeJsonEntity<&value_type::applicationId, "application_id">()>();
	};

	template<> struct core<discord_core_api::presence_update_data> {
		using value_type				 = discord_core_api::presence_update_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::clientStatus, "client_status">(), makeJsonEntity<&value_type::guildId, "guild_id">(),
			makeJsonEntity<&value_type::status, "status">(), makeJsonEntity<&value_type::user, "user">(), makeJsonEntity<&value_type::activities, "activities">()>();
	};

	template<> struct core<discord_core_api::account_data> {
		using value_type				 = discord_core_api::account_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::name, "name">()>();
	};

	template<> struct core<discord_core_api::embed_data> {
		using value_type				 = discord_core_api::embed_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::author, "author">(), makeJsonEntity<&value_type::hexColorValue, "color">(),
			makeJsonEntity<&value_type::description, "description">(), makeJsonEntity<&value_type::fields, "fields">(), makeJsonEntity<&value_type::footer, "footer">(),
			makeJsonEntity<&value_type::image, "image">(), makeJsonEntity<&value_type::provider, "provider">(), makeJsonEntity<&value_type::thumbnail, "thumbnail">(),
			makeJsonEntity<&value_type::timeStamp, "timestamp">(), makeJsonEntity<&value_type::title, "title">(), makeJsonEntity<&value_type::type, "type">(),
			makeJsonEntity<&value_type::url, "url">(), makeJsonEntity<&value_type::video, "video">()>();
	};

	template<> struct core<discord_core_api::reaction_data> {
		using value_type = discord_core_api::reaction_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::count, "count">(), makeJsonEntity<&value_type::emoji, "emoji">(),
				makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::member, "member">(),
				makeJsonEntity<&value_type::me, "me">(), makeJsonEntity<&value_type::messageId, "message_id">(), makeJsonEntity<&value_type::userId, "user_id">()>();
	};

	template<> struct core<discord_core_api::sticker_item_data> {
		using value_type				 = discord_core_api::sticker_item_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::formatType, "format_type">(), makeJsonEntity<&value_type::name, "name">()>();
	};

	template<> struct core<discord_core_api::attachment_data> {
		using value_type				 = discord_core_api::attachment_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::contentType, "content_type">(), makeJsonEntity<&value_type::description, "description">(),
			makeJsonEntity<&value_type::filename, "filename">(), makeJsonEntity<&value_type::height, "height">(), makeJsonEntity<&value_type::id, "id">(),
			makeJsonEntity<&value_type::proxyUrl, "proxy_url">(), makeJsonEntity<&value_type::size, "size">(), makeJsonEntity<&value_type::url, "url">(),
			makeJsonEntity<&value_type::width, "width">(), makeJsonEntity<&value_type::ephemeral, "ephemeral">()>();
	};

	template<> struct core<discord_core_api::channel_mention_data> {
		using value_type				 = discord_core_api::channel_mention_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::guildId, "guild_id">(),
			makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::select_default_value_data> {
		using value_type				 = discord_core_api::select_default_value_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::action_row_data> {
		using value_type				 = discord_core_api::action_row_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::components, "components">(), makeJsonEntity<&value_type::type, "type">()>();
	};

	template<> struct core<discord_core_api::embed_footer_data> {
		using value_type				 = discord_core_api::embed_footer_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::proxyIconUrl, "proxy_icon_url">(), makeJsonEntity<&value_type::iconUrl, "icon_url">(),
			makeJsonEntity<&value_type::text, "text">()>();
	};

	template<> struct core<discord_core_api::embed_image_data> {
		using value_type				 = discord_core_api::embed_image_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::proxyUrl, "proxy_url">(), makeJsonEntity<&value_type::url, "url">(),
			makeJsonEntity<&value_type::height, "height">(), makeJsonEntity<&value_type::width, "width">()>();
	};

	template<> struct core<discord_core_api::embed_thumbnail_data> {
		using value_type				 = discord_core_api::embed_thumbnail_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::proxyUrl, "proxy_url">(), makeJsonEntity<&value_type::url, "url">(),
			makeJsonEntity<&value_type::height, "height">(), makeJsonEntity<&value_type::width, "width">()>();
	};

	template<> struct core<discord_core_api::embed_video_data> {
		using value_type				 = discord_core_api::embed_video_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::proxyUrl, "proxy_url">(), makeJsonEntity<&value_type::url, "url">(),
			makeJsonEntity<&value_type::height, "height">(), makeJsonEntity<&value_type::width, "width">()>();
	};

	template<> struct core<discord_core_api::embed_provider_data> {
		using value_type				 = discord_core_api::embed_provider_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::url, "url">()>();
	};

	template<> struct core<discord_core_api::embed_author_data> {
		using value_type				 = discord_core_api::embed_author_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::proxyIconUrl, "proxy_icon_url">(), makeJsonEntity<&value_type::iconUrl, "icon_url">(),
			makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::url, "url">()>();
	};

	template<> struct core<discord_core_api::embed_field_data> {
		using value_type = discord_core_api::embed_field_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::value, "value">(), makeJsonEntity<&value_type::Inline, "inline">()>();
	};

	template<> struct core<discord_core_api::guild_member_cache_data> {
		using value_type				 = discord_core_api::guild_member_cache_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::avatar, "avatar">(), makeJsonEntity<&value_type::flags, "flags">(),
			makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::joinedAt, "joined_at">(), makeJsonEntity<&value_type::nick, "nick">(),
			makeJsonEntity<&value_type::permissionsVal, "permissions">(), makeJsonEntity<&value_type::roles, "roles">(), makeJsonEntity<&value_type::user, "user">()>();
	};

	template<> struct core<discord_core_api::audit_log_entry_data> {
		using value_type				 = discord_core_api::audit_log_entry_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::actionType, "action_type">(), makeJsonEntity<&value_type::changes, "changes">(),
			makeJsonEntity<&value_type::createdTimeStamp, "created_at">(), makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::options, "options">(),
			makeJsonEntity<&value_type::targetId, "target_id">(), makeJsonEntity<&value_type::userId, "user_id">()>();
	};

	template<> struct core<discord_core_api::component_data> {
		using value_type				 = discord_core_api::component_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::customId, "custom_id">(),
			makeJsonEntity<&value_type::options, "options">(), makeJsonEntity<&value_type::channelTypes, "channel_types">(),
			makeJsonEntity<&value_type::placeholder, "placeholder">(), makeJsonEntity<&value_type::minValues, "min_values">(),
			makeJsonEntity<&value_type::maxValues, "max_values">(), makeJsonEntity<&value_type::minLength, "min_length">(), makeJsonEntity<&value_type::maxLength, "max_length">(),
			makeJsonEntity<&value_type::label, "label">(), makeJsonEntity<&value_type::value, "value">(), makeJsonEntity<&value_type::title, "title">(),
			makeJsonEntity<&value_type::emoji, "emoji">(), makeJsonEntity<&value_type::url, "url">(), makeJsonEntity<&value_type::required, "required">(),
			makeJsonEntity<&value_type::disabled, "disabled">(), makeJsonEntity<&value_type::style, "style">()>();
	};

	template<> struct core<discord_core_api::select_option_data> {
		using value_type				 = discord_core_api::select_option_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::description, "description">(), makeJsonEntity<&value_type::label, "label">(),
			makeJsonEntity<&value_type::value, "value">(), makeJsonEntity<&value_type::emoji, "emoji">(), makeJsonEntity<&value_type::_default, "default">()>();
	};

	template<> struct core<discord_core_api::voice_state_data_light> {
		using value_type				 = discord_core_api::voice_state_data_light;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(),
			makeJsonEntity<&value_type::userId, "user_id">()>();
	};

	template<> struct core<discord_core_api::guild_data> {
		using value_type				 = discord_core_api::guild_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::defaultMessageNotifications, "default_message_notifications">(),
			makeJsonEntity<&value_type::guildScheduledEvents, "guild_scheduled_events">(), makeJsonEntity<&value_type::explicitContentFilter, "explicit_content_filter">(),
			makeJsonEntity<&value_type::stageInstances, "stage_instances">(), makeJsonEntity<&value_type::presences, "presences">(),
			makeJsonEntity<&value_type::systemChannelFlags, "system_channel_flags">(), makeJsonEntity<&value_type::widgetEnabled, "widget_enabled">(),
			makeJsonEntity<&value_type::unavailable, "unavailable">(), makeJsonEntity<&value_type::owner, "owner">(), makeJsonEntity<&value_type::large, "large">(),
			makeJsonEntity<&value_type::memberCount, "member_count">(), makeJsonEntity<&value_type::verificationLevel, "verification_level">(),
			makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::channels, "channels">(), makeJsonEntity<&value_type::roles, "roles">(),
			makeJsonEntity<&value_type::members, "members">(), makeJsonEntity<&value_type::voiceStates, "voice_states">(), makeJsonEntity<&value_type::ownerId, "owner_id">(),
			makeJsonEntity<&value_type::permissions, "permissions">(), makeJsonEntity<&value_type::afkChannelId, "afk_channel_id">(),
			makeJsonEntity<&value_type::features, "features">(), makeJsonEntity<&value_type::stickers, "stickers">(),
			makeJsonEntity<&value_type::maxStageVideoChannelUsers, "max_stage_video_channel_users">(),
			makeJsonEntity<&value_type::publicUpdatesChannelId, "public_updates_channel_id">(),
			makeJsonEntity<&value_type::premiumSubscriptionCount, "premium_subscription_count">(),
			makeJsonEntity<&value_type::approximatePresenceCount, "approximate_presence_count">(), makeJsonEntity<&value_type::threads, "threads">(),
			makeJsonEntity<&value_type::welcomeScreen, "welcome_screen">(), makeJsonEntity<&value_type::safetyAlertsChannelId, "safety_alerts_channel_id">(),
			makeJsonEntity<&value_type::approximateMemberCount, "approximate_member_count">(),
			makeJsonEntity<&value_type::premiumProgressBarEnabled, "premium_progress_bar_enabled">(),
			makeJsonEntity<&value_type::maxVideoChannelUsers, "max_video_channel_users">(), makeJsonEntity<&value_type::emoji, "emoji">(),
			makeJsonEntity<&value_type::preferredLocale, "preferred_locale">(), makeJsonEntity<&value_type::vanityUrlCode, "vanity_url_code">(),
			makeJsonEntity<&value_type::systemChannelId, "system_channel_id">(), makeJsonEntity<&value_type::discoverySplash, "discovery_splash">(),
			makeJsonEntity<&value_type::widgetChannelId, "widget_channel_id">(), makeJsonEntity<&value_type::rulesChannelId, "rules_channel_id">(),
			makeJsonEntity<&value_type::nsfwLevel, "nsfw_level">(), makeJsonEntity<&value_type::applicationId, "application_id">(),
			makeJsonEntity<&value_type::description, "description">(), makeJsonEntity<&value_type::premiumTier, "premium_tier">(),
			makeJsonEntity<&value_type::afkTimeout, "afk_timeout">(), makeJsonEntity<&value_type::maxMembers, "max_members">(),
			makeJsonEntity<&value_type::mfaLevel, "mfa_level">(), makeJsonEntity<&value_type::splash, "splash">(), makeJsonEntity<&value_type::name, "name">(),
			makeJsonEntity<&value_type::banner, "banner">(), makeJsonEntity<&value_type::icon, "icon">()>();
	};

	template<> struct core<discord_core_api::partial_guild_data> {
		using value_type				 = discord_core_api::partial_guild_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::unavailable, "unavailable">(), makeJsonEntity<&value_type::id, "id">()>();
	};

	template<> struct core<discord_core_api::integration_data> {
		using value_type				 = discord_core_api::integration_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::account, "account">(), makeJsonEntity<&value_type::application, "application">(),
			makeJsonEntity<&value_type::enabled, "enabled">(), makeJsonEntity<&value_type::enableEmoticons, "enable_emoticons">(),
			makeJsonEntity<&value_type::expireBehavior, "expire_behavior">(), makeJsonEntity<&value_type::expireGracePeriod, "expire_grace_period">(),
			makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::revoked, "revoked">(),
			makeJsonEntity<&value_type::roleId, "role_id">(), makeJsonEntity<&value_type::subscriberCount, "subscriber_count">(), makeJsonEntity<&value_type::syncing, "syncing">(),
			makeJsonEntity<&value_type::syncedAt, "synced_at">(), makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::user, "user">()>();
	};

	template<> struct core<discord_core_api::file> {
		using value_type				 = discord_core_api::file;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::data, "data">()>();
	};

	template<> struct core<discord_core_api::allowed_mentions_data> {
		using value_type				 = discord_core_api::allowed_mentions_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::repliedUser, "replied_user">(), makeJsonEntity<&value_type::parse, "parse">(),
			makeJsonEntity<&value_type::roles, "roles">(), makeJsonEntity<&value_type::users, "users">()>();
	};

	template<> struct core<discord_core_api::message_reference_data> {
		using value_type				 = discord_core_api::message_reference_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::failIfNotExists, "fail_if_not_exists">(), makeJsonEntity<&value_type::messageId, "message_id">(),
			makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::guildId, "guild_id">()>();
	};

	template<> struct core<discord_core_api::message_interaction_data> {
		using value_type				 = discord_core_api::message_interaction_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::member, "member">(),
			makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::user, "user">()>();
	};

	template<> struct core<discord_core_api::guild_scheduled_event_data> {
		using value_type				 = discord_core_api::guild_scheduled_event_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::privacyLevel, "privacy_level">(),
			makeJsonEntity<&value_type::entityMetadata, "entity_metadata">(), makeJsonEntity<&value_type::entityType, "entity_type">(),
			makeJsonEntity<&value_type::status, "status">(), makeJsonEntity<&value_type::scheduledStartTime, "scheduled_start_time">(),
			makeJsonEntity<&value_type::scheduledEndTime, "scheduled_end_time">(), makeJsonEntity<&value_type::description, "description">(),
			makeJsonEntity<&value_type::creatorId, "creator_id">(), makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::entityId, "entity_id">(),
			makeJsonEntity<&value_type::userCount, "user_count">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::creator, "creator">(),
			makeJsonEntity<&value_type::name, "name">()>();
	};

	template<> struct core<discord_core_api::guild_scheduled_event_user_data> {
		using value_type				 = discord_core_api::guild_scheduled_event_user_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::guildScheduledEventId, "guild_scheduled_event_id">(),
			makeJsonEntity<&value_type::member, "member">(), makeJsonEntity<&value_type::user, "user">()>();
	};

	template<> struct core<discord_core_api::guild_scheduled_event_metadata> {
		using value_type				 = discord_core_api::guild_scheduled_event_metadata;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::location, "location">()>();
	};

	template<> struct core<discord_core_api::role_data> {
		using value_type = discord_core_api::role_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::id, "id">(), makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::color, "color">(),
				makeJsonEntity<&value_type::hoist, "hoist">(), makeJsonEntity<&value_type::icon, "icon">(), makeJsonEntity<&value_type::unicodeEmoji, "unicode_emoji">(),
				makeJsonEntity<&value_type::position, "position">(), makeJsonEntity<&value_type::permissions, "permissions">(), makeJsonEntity<&value_type::managed, "managed">(),
				makeJsonEntity<&value_type::mentionable, "mentionable">(), makeJsonEntity<&value_type::tags, "tags">(), makeJsonEntity<&value_type::flags, "flags">()>();
	};

	template<> struct core<discord_core_api::sticker_pack_data> {
		using value_type				 = discord_core_api::sticker_pack_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::bannerAssetId, "banner_asset_id">(),
			makeJsonEntity<&value_type::coverStickerId, "cover_sticker_id">(), makeJsonEntity<&value_type::description, "description">(), makeJsonEntity<&value_type::id, "id">(),
			makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::skuId, "sku_id">(), makeJsonEntity<&value_type::stickers, "stickers">()>();
	};

	template<> struct core<discord_core_api::guild_widget_image_data> {
		using value_type				 = discord_core_api::guild_widget_image_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::url, "url">()>();
	};

	template<> struct core<discord_core_api::guild_template_data> {
		using value_type				 = discord_core_api::guild_template_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::code, "code">(), makeJsonEntity<&value_type::createdAt, "created_at">(),
			makeJsonEntity<&value_type::creator, "creator">(), makeJsonEntity<&value_type::creatorId, "creator_id">(), makeJsonEntity<&value_type::description, "description">(),
			makeJsonEntity<&value_type::isDirty, "is_dirty">(), makeJsonEntity<&value_type::name, "name">(),
			makeJsonEntity<&value_type::serializedSourceGuild, "serialized_source_guild">(), makeJsonEntity<&value_type::sourceGuildId, "source_guild_id">(),
			makeJsonEntity<&value_type::updatedAt, "updated_at">()>();
	};

	template<> struct core<discord_core_api::application_command_option_data> {
		using value_type				 = discord_core_api::application_command_option_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::type, "type">(), makeJsonEntity<&value_type::name, "name">(),
			makeJsonEntity<&value_type::description, "description">(), makeJsonEntity<&value_type::required, "required">(),
			makeJsonEntity<&value_type::autocomplete, "autocomplete">(), makeJsonEntity<&value_type::minValue, "min_value">(), makeJsonEntity<&value_type::maxValue, "max_value">(),
			makeJsonEntity<&value_type::channelTypes, "channel_types">(), makeJsonEntity<&value_type::descriptionLocalizations, "description_localizations">(),
			makeJsonEntity<&value_type::nameLocalizations, "name_localizations">(), makeJsonEntity<&value_type::choices, "choices">(),
			makeJsonEntity<&value_type::options, "options">()>();
	};

	template<> struct core<discord_core_api::stage_instance_data> {
		using value_type = discord_core_api::stage_instance_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::privacyLevel, "privacy_level">(), makeJsonEntity<&value_type::discoverableDisabled, "discoverable_disabled">(),
				makeJsonEntity<&value_type::channelId, "channel_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::topic, "topic">()>();
	};

	template<> struct core<discord_core_api::application_command_option_choice_data> {
		using value_type				 = discord_core_api::application_command_option_choice_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::value, "value">(),
			makeJsonEntity<&value_type::nameLocalizations, "name_localizations">()>();
	};

	template<> struct core<discord_core_api::role_tags_data> {
		using value_type				 = discord_core_api::role_tags_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::premiumSubscriber, "premium_subscriber">(),
			makeJsonEntity<&value_type::integrationId, "integration_id">(), makeJsonEntity<&value_type::botId, "bot_id">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::connect_properties> {
		using value_type = discord_core_api::discord_core_internal::connect_properties;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::os, "os">(), makeJsonEntity<&value_type::device, "device">(), makeJsonEntity<&value_type::browser, "browser">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::websocket_identify_data> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_identify_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::botToken, "token">(), makeJsonEntity<&value_type::shard, "shard">(),
			makeJsonEntity<&value_type::intents, "intents">(), makeJsonEntity<&value_type::largeThreshold, "large_threshold">(),
			makeJsonEntity<&value_type::presence, "presence">(), makeJsonEntity<&value_type::properties, "properties">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::websocket_resume_data> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_resume_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::botToken, "token">(), makeJsonEntity<&value_type::sessionId, "session_id">(),
			makeJsonEntity<&value_type::lastNumberReceived, "seq">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::voice_socket_protocol_payload_data_data> {
		using value_type = discord_core_api::discord_core_internal::voice_socket_protocol_payload_data_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::address, "address">(), makeJsonEntity<&value_type::mode, "mode">(), makeJsonEntity<&value_type::port, "port">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::voice_socket_protocol_payload_data> {
		using value_type				 = discord_core_api::discord_core_internal::voice_socket_protocol_payload_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::data, "data">(), makeJsonEntity<&value_type::protocol, "protocol">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::voice_identify_data> {
		using value_type				 = discord_core_api::discord_core_internal::voice_identify_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::sessionId, "session_id">(), makeJsonEntity<&value_type::userId, "user_id">(),
			makeJsonEntity<&value_type::token, "token">(), makeJsonEntity<&value_type::serverId, "server_id">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::send_speaking_data> {
		using value_type = discord_core_api::discord_core_internal::send_speaking_data;
		static constexpr auto parseValue =
			createValue<makeJsonEntity<&value_type::type, "speaking">(), makeJsonEntity<&value_type::delay, "delay">(), makeJsonEntity<&value_type::ssrc, "ssrc">()>();
	};

	template<> struct core<discord_core_api::discord_core_internal::hello_data> {
		using value_type				 = discord_core_api::discord_core_internal::hello_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::heartbeatInterval, "heartbeat_interval">(), makeJsonEntity<&value_type::_trace, "_trace">()>();
	};

	template<> struct core<discord_core_api::invite_data> {
		using value_type				 = discord_core_api::invite_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::guildScheduledEvent, "guild_scheduled_event">(),
			makeJsonEntity<&value_type::targetApplication, "target_application">(), makeJsonEntity<&value_type::approximatePresenceCount, "approximate_presence_count">(),
			makeJsonEntity<&value_type::stageInstance, "stage_instance">(), makeJsonEntity<&value_type::approximateMemberCount, "approximate_member_count">(),
			makeJsonEntity<&value_type::expiresAt, "expires_at">(), makeJsonEntity<&value_type::createdAt, "created_at">(), makeJsonEntity<&value_type::guild, "guild">(),
			makeJsonEntity<&value_type::code, "code">(), makeJsonEntity<&value_type::targetUser, "target_user">(), makeJsonEntity<&value_type::channel, "channel">(),
			makeJsonEntity<&value_type::targetType, "target_type">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::inviter, "inviter">(),
			makeJsonEntity<&value_type::maxUses, "max_uses">(), makeJsonEntity<&value_type::maxAge, "max_age">(), makeJsonEntity<&value_type::temporary, "temporary">(),
			makeJsonEntity<&value_type::uses, "uses">()>();
	};

	template<> struct core<discord_core_api::auto_moderation_rule_data> {
		using value_type				 = discord_core_api::auto_moderation_rule_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::exemptChannels, "exempt_channels">(), makeJsonEntity<&value_type::exemptRoles, "exempt_roles">(),
			makeJsonEntity<&value_type::actions, "actions">(), makeJsonEntity<&value_type::triggerMetaData, "trigger_meta_data">(),
			makeJsonEntity<&value_type::triggerType, "trigger_type">(), makeJsonEntity<&value_type::name, "name">(), makeJsonEntity<&value_type::eventType, "event_type">(),
			makeJsonEntity<&value_type::creatorId, "creator_id">(), makeJsonEntity<&value_type::guildId, "guild_id">(), makeJsonEntity<&value_type::enabled, "enabled">(),
			makeJsonEntity<&value_type::id, "id">()>();
	};

	template<> struct core<discord_core_api::update_presence_data> {
		using value_type				 = discord_core_api::update_presence_data;
		static constexpr auto parseValue = createValue<makeJsonEntity<&value_type::afk, "afk">(), makeJsonEntity<&value_type::since, "since">(),
			makeJsonEntity<&value_type::statusReal, "status">(), makeJsonEntity<&value_type::activities, "activities">()>();
	};

}
