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
/// InteractionEntities.hpp - Header for the Jsonifier::Core specializations.
/// Mar 24, 2023 Chris M.
/// https://discordcoreapi.com
/// \file JsonSpecializations.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	template<typename OTy> struct HttpsDataNew {
		OTy data{};
	};

	template<> struct WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData> {
		WebSocketMessageData() noexcept = default;
		WebSocketMessageData(const DiscordCoreAPI::UpdateVoiceStateData& data) noexcept;
		std::unordered_set<std::string> excludedKeys{};
		using type = DiscordCoreAPI::UpdateVoiceStateData;
		int64_t op{ -1 };
		DiscordCoreAPI::UpdateVoiceStateData d{};
		operator DiscordCoreAPI::EtfSerializer() noexcept;
	};

	template<> struct WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateDataDC> {
		WebSocketMessageData() noexcept = default;
		WebSocketMessageData(const DiscordCoreAPI::UpdateVoiceStateData& data) noexcept;
		std::unordered_set<std::string> excludedKeys{};
		using type = DiscordCoreAPI::UpdateVoiceStateDataDC;
		int64_t op{ -1 };
		DiscordCoreAPI::UpdateVoiceStateDataDC d{};
		operator DiscordCoreAPI::EtfSerializer() noexcept;
	};
}

namespace Jsonifier {

	class IconHash;

	template<DiscordCoreAPI::TextFormat textFormat> struct Core<DiscordCoreInternal::ReadyData<textFormat>> {
		using OTy2 = DiscordCoreInternal::ReadyData<textFormat>;
		constexpr static auto parseValue = object("resume_gateway_url", &OTy2::resumeGatewayUrl, "shard", &OTy2::shard, "v", &OTy2::v, "session_id",
			&OTy2::sessionId, "user", &OTy2::user, "application", &OTy2::application);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::InvalidSessionData>> {
		using OTy2 = DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::InvalidSessionData>;
		constexpr static auto parseValue = object("d", &OTy2::d);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketMessage> {
		using OTy2 = DiscordCoreInternal::WebSocketMessage;
		constexpr static auto parseValue = object("op", &OTy2::op, "s", &OTy2::s, "t", &OTy2::t);
	};

	template<typename OTy> struct Core<DiscordCoreInternal::WebSocketMessageData<OTy>> {
		using OTy2 = DiscordCoreInternal::WebSocketMessageData<OTy>;
		constexpr static auto parseValue = object("d", &OTy2::d, "op", &OTy2::op, "s", &OTy2::s, "t", &OTy2::t);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData>> {
		using OTy2 = DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData>;
		constexpr static auto parseValue = object("op", &OTy2::op, "d", &OTy2::d);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateDataDC>> {
		using OTy2 = DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateDataDC>;
		constexpr static auto parseValue = object("op", &OTy2::op, "d", &OTy2::d);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::WebSocketIdentifyData>> {
		using OTy2 = DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::WebSocketIdentifyData>;
		constexpr static auto parseValue = object("d", &OTy2::d, "op", &OTy2::op, "s", &OTy2::s);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::WebSocketResumeData>> {
		using OTy2 = DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::WebSocketResumeData>;
		constexpr static auto parseValue = object("d", &OTy2::d, "op", &OTy2::op);
	};

}
namespace JsonifierInternal {

	template<> inline void SerializeWithKeys::op<DiscordCoreAPI::Snowflake, Jsonifier::String>(DiscordCoreAPI::Snowflake& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeWithKeys::op(newString, buffer, index);
	}

	template<> inline void SerializeWithKeys::op<DiscordCoreAPI::Permissions, Jsonifier::String>(DiscordCoreAPI::Permissions& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeWithKeys::op(newString, buffer, index);
	}

	template<> inline void SerializeWithKeys::op<DiscordCoreAPI::IconHash, Jsonifier::String>(DiscordCoreAPI::IconHash& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeWithKeys::op(newString, buffer, index);
	}

	template<> inline void SerializeNoKeys::op<DiscordCoreAPI::Snowflake, Jsonifier::String>(DiscordCoreAPI::Snowflake& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeNoKeys::op(newString, buffer, index);
	}

	template<> inline void SerializeNoKeys::op<DiscordCoreAPI::Permissions, Jsonifier::String>(DiscordCoreAPI::Permissions& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeNoKeys::op(newString, buffer, index);
	}

	template<> inline void SerializeNoKeys::op<DiscordCoreAPI::LightString, Jsonifier::String>(DiscordCoreAPI::LightString& value,
		Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeNoKeys::op(newString, buffer, index);
	}

	template<> inline void SerializeNoKeys::op(DiscordCoreAPI::IconHash& value, Jsonifier::String& buffer, uint64_t& index) {
		std::string newString{ static_cast<std::string>(value) };
		SerializeNoKeys::op(newString, buffer, index);
	}

	template<> inline static void ParseWithKeys::op<true, DiscordCoreAPI::Permissions>(DiscordCoreAPI::Permissions& value, StructuralIterator& it) {
		auto newPtr = it;
		int64_t newValue{};
		parseNumber(newValue, *newPtr);
		value = static_cast<DiscordCoreAPI::Permissions>(newValue);
		++it;
	}

	template<> inline static void ParseWithKeys::op<true, DiscordCoreAPI::IconHash>(DiscordCoreAPI::IconHash& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<true>(newString, it);
		value = newString;
	}

	template<> inline static void ParseWithKeys::op<true, DiscordCoreAPI::TimeStamp>(DiscordCoreAPI::TimeStamp& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<true>(newString, it);
		value = newString;
	}

	template<> inline static void ParseWithKeys::op<true, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<true>(newString, it);
		value = newString;
	}

	template<> inline static void ParseWithKeys::op<false, DiscordCoreAPI::Permissions>(DiscordCoreAPI::Permissions& value, StructuralIterator& it) {
		auto newPtr = it;
		int64_t newValue{};
		parseNumber(newValue, *newPtr);
		value = static_cast<DiscordCoreAPI::Permissions>(newValue);
		++it;
	}

	template<> inline static void ParseWithKeys::op<false, DiscordCoreAPI::IconHash>(DiscordCoreAPI::IconHash& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<false>(newString, it);
		value = newString;
	}

	template<> inline static void ParseWithKeys::op<false, DiscordCoreAPI::TimeStamp>(DiscordCoreAPI::TimeStamp& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<false>(newString, it);
		value = newString;
	}

	template<> inline static void ParseWithKeys::op<false, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseWithKeys::op<false>(newString, it);
		value = newString;
	}

	template<> inline static void ParseNoKeys::op<true, DiscordCoreAPI::Permissions>(DiscordCoreAPI::Permissions& value, StructuralIterator& it) {
		auto newPtr = it;
		int64_t newValue{};
		parseNumber(newValue, *newPtr);
		value = static_cast<DiscordCoreAPI::Permissions>(newValue);
		++it;
	}

	template<> inline static void ParseNoKeys::op<true, DiscordCoreAPI::IconHash>(DiscordCoreAPI::IconHash& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<true>(newString, it);
		value = newString;
		return;
	}

	template<> inline static void ParseNoKeys::op<true, DiscordCoreAPI::LightString>(DiscordCoreAPI::LightString& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<true>(newString, it);
		value = newString;
		return;
	}

	template<> inline static void ParseNoKeys::op<true, DiscordCoreAPI::TimeStamp>(DiscordCoreAPI::TimeStamp& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<true>(newString, it);
		value = newString;
		return;
	}

	template<> inline static void ParseNoKeys::op<true, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<true>(newString, it);
		value = newString;
		return;
	}

	template<> inline static void ParseNoKeys::op<false, DiscordCoreAPI::Permissions>(DiscordCoreAPI::Permissions& value, StructuralIterator& it) {
		auto newPtr = it;
		int64_t newValue{};
		parseNumber(newValue, *newPtr);
		value = static_cast<DiscordCoreAPI::Permissions>(newValue);
		++it;
	}

	template<> inline static void ParseNoKeys::op<false, DiscordCoreAPI::IconHash>(DiscordCoreAPI::IconHash& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<false>(newString, it);
		value = newString;
		return;
	}

	template<> inline static void ParseNoKeys::op<false, DiscordCoreAPI::LightString>(DiscordCoreAPI::LightString& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<false>(newString, it);
		value = newString;
		return;
	}

	template<> inline static void ParseNoKeys::op<false, DiscordCoreAPI::TimeStamp>(DiscordCoreAPI::TimeStamp& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<false>(newString, it);
		value = newString;
		return;
	}

	template<> inline static void ParseNoKeys::op<false, DiscordCoreAPI::Snowflake>(DiscordCoreAPI::Snowflake& value, StructuralIterator& it) {
		std::string newString{};
		ParseNoKeys::op<false>(newString, it);
		value = newString;
		return;
	}
}

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::Id> {
		using OTy2 = DiscordCoreAPI::Id;
		constexpr static auto parseValue = object("id", &OTy2::id);
	};

	template<typename OTy> struct Core<DiscordCoreInternal::HttpsDataNew<OTy>> {
		using OTy2 = DiscordCoreInternal::HttpsDataNew<OTy>;
		constexpr static auto parseValue = object("d", &OTy2::data);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandPermissionData> {
		using OTy2 = DiscordCoreAPI::ApplicationCommandPermissionData;
		constexpr static auto parseValue = object("id", &OTy2::id, "permission", &OTy2::permission, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::GuildApplicationCommandPermissionsData> {
		using OTy2 = DiscordCoreAPI::GuildApplicationCommandPermissionsData;
		constexpr static auto parseValue =
			object("application_id", &OTy2::applicationId, "permissions", &OTy2::permissions, "guild_id", &OTy2::guildId, "id", &OTy2::id);
	};

	template<> struct Core<DiscordCoreAPI::SessionStartData> {
		using OTy2 = DiscordCoreAPI::SessionStartData;
		constexpr static auto parseValue =
			object("max_concurrency", &OTy2::maxConcurrency, "remaining", &OTy2::remaining, "reset_after", &OTy2::resetAfter, "total", &OTy2::total);
	};

	template<> struct Core<DiscordCoreAPI::AuthorizationInfoData> {
		using OTy2 = DiscordCoreAPI::AuthorizationInfoData;
		constexpr static auto parseValue =
			object("application", &OTy2::application, "expires", &OTy2::expires, "scopes", &OTy2::scopes, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::TriggerMetaData> {
		using OTy2 = DiscordCoreAPI::TriggerMetaData;
		constexpr static auto parseValue = object("allow_list", &OTy2::allowList, "keyword_filter", &OTy2::keywordFilter, "mention_total_limit",
			&OTy2::mentionTotalLimit, "presets", &OTy2::presets);
	};

	template<> struct Core<DiscordCoreAPI::PartialEmojiData> {
		using OTy2 = DiscordCoreAPI::PartialEmojiData;
		constexpr static auto parseValue = object("id", &OTy2::id, "name", &OTy2::name, "animated", &OTy2::animated);
	};

	template<> struct Core<DiscordCoreAPI::EmojiData> {
		using OTy2 = DiscordCoreAPI::EmojiData;
		constexpr static auto parseValue = object("id", &OTy2::id, "name", &OTy2::name, "roles", &OTy2::roles, "require_colons", &OTy2::requireColons,
			"managed", &OTy2::managed, "animated", &OTy2::animated, "available", &OTy2::available);
	};

	template<> struct Core<DiscordCoreAPI::TeamMembersObjectData> {
		using OTy2 = DiscordCoreAPI::TeamMembersObjectData;
		constexpr static auto parseValue =
			object("membership_state", &OTy2::membershipState, "permissions", &OTy2::permissions, "team_id", &OTy2::teamId, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::TeamObjectData> {
		using OTy2 = DiscordCoreAPI::TeamObjectData;
		constexpr static auto parseValue =
			object("icon", &OTy2::icon, "id", &OTy2::id, "members", &OTy2::members, "owner_user_id", &OTy2::ownerUserId);
	};

	template<> struct Core<DiscordCoreAPI::InstallParamsData> {
		using OTy2 = DiscordCoreAPI::InstallParamsData;
		constexpr static auto parseValue = object("scopes", &OTy2::scopes, "permissions", &OTy2::permissions);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationData> {
		using OTy2 = DiscordCoreAPI::ApplicationData;
		constexpr static auto parseValue = object("bot_public", &OTy2::botPublic, "bot_require_code_grant", &OTy2::botRequireCodeGrant, "cover_image",
			&OTy2::coverImage, "custom_install_url", &OTy2::customInstallUrl, "description", &OTy2::description, "", "flags", &OTy2::flags,
			"guild_id", &OTy2::guildId, "icon", &OTy2::icon, "name", &OTy2::name, "owner", &OTy2::owner, "params", &OTy2::params, "primary_sku_id",
			&OTy2::primarySkuId, "privacy_policy_url", &OTy2::privacyPolicyUrl, "rpc_origins", &OTy2::rpcOrigins, "slug", &OTy2::slug, "summary",
			&OTy2::summary, "tags", &OTy2::tags, "team", &OTy2::team, "terms_of_service_url", &OTy2::termsOfServiceUrl, "verify_key",
			&OTy2::verifyKey);
	};

	template<> struct Core<DiscordCoreAPI::ThreadMetadataData> {
		using OTy2 = DiscordCoreAPI::ThreadMetadataData;
		constexpr static auto parseValue = object("archived", &OTy2::archived, "archived_timestamp", &OTy2::archiveTimestamp, "auto_archive_duration",
			&OTy2::autoArchiveDuration, "invitable", &OTy2::invitable, "locked", &OTy2::locked);
	};

	template<> struct Core<DiscordCoreAPI::GatewayBotData> {
		using OTy2 = DiscordCoreAPI::GatewayBotData;
		constexpr static auto parseValue = object("session_start_limit", &OTy2::sessionStartLimit, "shards", &OTy2::shards, "url", &OTy2::url);
	};

	template<> struct Core<DiscordCoreAPI::UserIdBase> {
		using OTy2 = DiscordCoreAPI::UserIdBase;
		constexpr static auto parseValue = object("id", &OTy2::id);
	};

	template<> struct Core<DiscordCoreAPI::UserBase> {
		using OTy2 = DiscordCoreAPI::UserBase;
		constexpr static auto parseValue = object("user", &OTy2::userIdBase);
	};

	template<> struct Core<DiscordCoreAPI::GuildMember> {
		using OTy2 = DiscordCoreAPI::GuildMember;
		constexpr static auto parseValue = object("avatar", &OTy2::avatar, "communication_disabled_until", &OTy2::communicationDisabledUntil, "flags",
			&OTy2::flags, "guild_id", &OTy2::guildId, "joined_at", &OTy2::joinedAt, "nick", &OTy2::nick, "permissions", &OTy2::permissions,
			"premiums_since", &OTy2::premiumSince, "roles", &OTy2::roles, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::UserData> {
		using OTy2 = DiscordCoreAPI::UserData;
		constexpr static auto parseValue =
			object("mfa_enabled", &OTy2::flags, "verified", &OTy2::flags, "system", &OTy2::flags, "bot", &OTy2::flags, "public_flags", &OTy2::flags,
				"username", &OTy2::userName, "discriminator", &OTy2::discriminator, "id", &OTy2::id, "avatar", &OTy2::avatar);
	};

	template<> struct Core<DiscordCoreAPI::User> {
		using OTy2 = DiscordCoreAPI::User;
		constexpr static auto parseValue = object("mfa_enabled", &OTy2::flags, "verified", &OTy2::flags, "system", &OTy2::flags, "bot", &OTy2::flags,
			"public_flags", &OTy2::flags, "username", &OTy2::userName, "discriminator", &OTy2::discriminator, "id", &OTy2::id, "avatar",
			&OTy2::avatar, "accent_color", &OTy2::accentColor, "banner", &OTy2::banner, "email", &OTy2::email, "flags", &OTy2::flags, "locale",
			&OTy2::locale, "premium_type", &OTy2::premiumType);
	};

	template<> struct Core<DiscordCoreAPI::WelcomeScreenChannelData> {
		using OTy2 = DiscordCoreAPI::WelcomeScreenChannelData;
		constexpr static auto parseValue =
			object("channel_id", &OTy2::channelId, "description", &OTy2::description, "emoji_id", &OTy2::emojiId, "emoji_name", &OTy2::emojiName);
	};

	template<> struct Core<DiscordCoreAPI::WelcomeScreenData> {
		using OTy2 = DiscordCoreAPI::WelcomeScreenData;
		constexpr static auto parseValue = object("description", &OTy2::description, "welcome_screen_channels", &OTy2::welcomeChannels);
	};

	template<> struct Core<DiscordCoreAPI::ActionMetaData> {
		using OTy2 = DiscordCoreAPI::ActionMetaData;
		constexpr static auto parseValue = object("channel_id", &OTy2::channelId, "duration_seconds", &OTy2::durationSeconds);
	};

	template<> struct Core<DiscordCoreAPI::ActionData> {
		using OTy2 = DiscordCoreAPI::ActionData;
		constexpr static auto parseValue = object("meta_data", &OTy2::metadata, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::GuildPreviewData> {
		using OTy2 = DiscordCoreAPI::GuildPreviewData;
		constexpr static auto parseValue =
			object("approximate_member_count", &OTy2::approximateMemberCount, "approximate_presence_count", &OTy2::approximatePresenceCount,
				"description", &OTy2::description, "discovery_splash", &OTy2::discoverySplash, "emojis", &OTy2::emojis, "features", &OTy2::features,
				"icon", &OTy2::icon, "id", &OTy2::id, "name", &OTy2::name, "splash", &OTy2::splash, "stickers", &OTy2::stickers);
	};

	template<> struct Core<DiscordCoreAPI::ThreadMemberData> {
		using OTy2 = DiscordCoreAPI::ThreadMemberData;
		constexpr static auto parseValue = object("flags", &OTy2::flags, "join_timestamp", &OTy2::joinTimestamp, "user_id", &OTy2::userId);
	};

	template<> struct Core<DiscordCoreAPI::AuditLogData> {
		using OTy2 = DiscordCoreAPI::AuditLogData;
		constexpr static auto parseValue = object("audit_log_entries", &OTy2::auditLogEntries, "auto_moderation_rules", &OTy2::autoModerationRules,
			"guild_scheduled_events", &OTy2::guildScheduledEvents, "integrations", &OTy2::integrations, "threads", &OTy2::threads, "users",
			&OTy2::users, "webhooks", &OTy2::webhooks);
	};

	template<> struct Core<DiscordCoreAPI::BanData> {
		using OTy2 = DiscordCoreAPI::BanData;
		constexpr static auto parseValue = object("failed_due_to_perms", &OTy2::failedDueToPerms, "reason", &OTy2::reason, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::UpdateVoiceStateData> {
		using OTy2 = DiscordCoreAPI::UpdateVoiceStateData;
		constexpr static auto parseValue =
			object("channel_id", &OTy2::channelId, "guild_id", &OTy2::guildId, "self_deaf", &OTy2::selfDeaf, "self_mute", &OTy2::selfMute);
	};

	template<> struct Core<DiscordCoreAPI::UpdateVoiceStateDataDC> {
		using OTy2 = DiscordCoreAPI::UpdateVoiceStateDataDC;
		constexpr static auto parseValue =
			object("channel_id", &OTy2::channelId, "guild_id", &OTy2::guildId, "self_deaf", &OTy2::selfDeaf, "self_mute", &OTy2::selfMute);
	};

	template<> struct Core<DiscordCoreAPI::Reaction> {
		using OTy2 = DiscordCoreAPI::Reaction;
		constexpr static auto parseValue = object("channel_id", &OTy2::channelId, "count", &OTy2::count, "id", &OTy2::id, "emoji", &OTy2::emoji,
			"guild_id", &OTy2::guildId, "me", &OTy2::me, "member", &OTy2::member, "message_id", &OTy2::messageId, "user_id", &OTy2::userId);
	};

	template<> struct Core<DiscordCoreAPI::ChannelData> {
		using OTy2 = DiscordCoreAPI::ChannelData;
		constexpr static auto parseValue = object("guild_id", &OTy2::guildId, "default_sort_order", &OTy2::defaultSortOrder, "flags", &OTy2::flags,
			"id", &OTy2::id, "member_count", &OTy2::memberCount, "name", &OTy2::name, "owner_id", &OTy2::ownerId, "parent_id", &OTy2::parentId,
			"permission_overwrites", &OTy2::permissionOverwrites, "permissions", "topic", &OTy2::topic, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::GuildWidgetData> {
		using OTy2 = DiscordCoreAPI::GuildWidgetData;
		constexpr static auto parseValue = object("channel_id", &OTy2::channelId, "enabled", &OTy2::enabled);
	};

	template<> struct Core<DiscordCoreAPI::Channel> {
		using OTy2 = DiscordCoreAPI::Channel;
		constexpr static auto parseValue = object("application_id", &OTy2::applicationId, "applied_tags", &OTy2::appliedTags, "available_tags",
			&OTy2::availableTags, "guild_id", &OTy2::guildId, "bitrate", &OTy2::bitrate, "default_auto_archive_duration",
			&OTy2::defaultAutoArchiveDuration, "default_reaction_emoji", &OTy2::defaultReactionEmoji, "default_sort_order", &OTy2::defaultSortOrder,
			"default_thread_rate_limit_per_user", &OTy2::defaultThreadRateLimitPerUser, "flags", &OTy2::flags, "icon", &OTy2::icon, "id", &OTy2::id,
			"last_message_id", &OTy2::lastMessageId, "last_pin_timestamp", &OTy2::lastPinTimestamp, "member", &OTy2::member, "member_count",
			&OTy2::memberCount, "message_count", &OTy2::messageCount, "name", &OTy2::name, "owner_id", &OTy2::ownerId, "parent_id", &OTy2::parentId,
			"permission_overwrites", &OTy2::permissionOverwrites, "permissions", &OTy2::permissions, "position", &OTy2::position,
			"rate_limit_per_user", &OTy2::rateLimitPerUser, "recipients", &OTy2::recipients, "rtc_region", &OTy2::rtcRegion, "thread_metadata",
			&OTy2::threadMetadata, "topic", &OTy2::topic, "total_message_sent", &OTy2::totalMessageSent, "type", &OTy2::type, "user_limit",
			&OTy2::userLimit, "video_quality_mode", &OTy2::videoQualityMode);
	};

	template<> struct Core<DiscordCoreAPI::GuildMemberCacheData> {
		using OTy2 = DiscordCoreAPI::GuildMemberCacheData;
		constexpr static auto parseValue = object("avatar", &OTy2::avatar, "flags", &OTy2::flags, "guild_id", &OTy2::guildId, "joined_at",
			&OTy2::joinedAt, "nick", &OTy2::nick, "permissions", &OTy2::permissions, "roles", &OTy2::roles, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::GuildMemberData> {
		using OTy2 = DiscordCoreAPI::GuildMemberData;
		constexpr static auto parseValue = object("avatar", &OTy2::avatar, "flags", &OTy2::flags, "guild_id", &OTy2::guildId, "joined_at",
			&OTy2::joinedAt, "nick", &OTy2::nick, "permissions", &OTy2::permissions, "roles", &OTy2::roles, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::ThreadListSyncData> {
		using OTy2 = DiscordCoreAPI::ThreadListSyncData;
		constexpr static auto parseValue =
			object("members", &OTy2::members, "channel_ids", &OTy2::channelIds, "threads", &OTy2::threads, "guild_id", &OTy2::guildId);
	};

	template<> struct Core<DiscordCoreAPI::InteractionDataData> {
		using OTy2 = DiscordCoreAPI::InteractionDataData;
		constexpr static auto parseValue = object("options", &OTy2::options, "type", &OTy2::type, "resolved", &OTy2::resolved, "guild_id",
			&OTy2::guildId, "name", &OTy2::name, "values", &OTy2::values, "component_type", &OTy2::componentType, "custom_id", &OTy2::customId,
			"custom_id_small", &OTy2::customIdSmall, "value", &OTy2::value, "target_id", &OTy2::targetId);
	};

	template<> struct Core<DiscordCoreAPI::ResolvedData> {
		using OTy2 = DiscordCoreAPI::ResolvedData;
		constexpr static auto parseValue = object("attachments", &OTy2::attachments, "members", &OTy2::members, "messages", &OTy2::messages,
			"channels", &OTy2::channels, "users", &OTy2::users, "roles", &OTy2::roles);
	};

	template<> struct Core<DiscordCoreAPI::MessageData> {
		using OTy2 = DiscordCoreAPI::MessageData;
		constexpr static auto parseValue = object("activity", &OTy2::activity, "application", &OTy2::application, "application_id",
			&OTy2::applicationId, "attachments", &OTy2::attachments, "author", &OTy2::author, "channel_id", &OTy2::channelId, "components",
			&OTy2::components, "content", &OTy2::content, "edited_timestamp", &OTy2::editedTimestamp, "embeds", &OTy2::embeds, "flags", &OTy2::flags,
			"guild_id", &OTy2::guildId, "id", &OTy2::id, "interaction", &OTy2::interaction, "mention_channels", &OTy2::mentionChannels,
			"mention_everyone", &OTy2::mentionEveryone, "mention_roles", &OTy2::mentionRoles, "mentions", &OTy2::mentions, "message_reference",
			&OTy2::messageReference, "nonce", &OTy2::nonce, "pinned", &OTy2::pinned, "reactions", &OTy2::reactions, "sticker_items",
			&OTy2::stickerItems, "stickers", &OTy2::stickers, "thread", &OTy2::thread, "timestamp", &OTy2::timeStamp, "tts", &OTy2::tts, "type",
			&OTy2::type, "webhook_id", &OTy2::webHookId);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandInteractionDataOption> {
		using OTy2 = DiscordCoreAPI::ApplicationCommandInteractionDataOption;
		constexpr static auto parseValue =
			object("name", &OTy2::name, "type", &OTy2::type, "value", &OTy2::value, "focused", &OTy2::focused, "options", &OTy2::options);
	};

	template<> struct Core<DiscordCoreAPI::InteractionData> {
		using OTy2 = DiscordCoreAPI::InteractionData;
		constexpr static auto parseValue = object("app_permissions", &OTy2::appPermissions, "data", &OTy2::data, "guild_locale", &OTy2::guildLocale,
			"application_id", &OTy2::applicationId, "member", &OTy2::member, "type", &OTy2::type, "message", &OTy2::message, "channel_id",
			&OTy2::channelId, "locale", &OTy2::locale, "guild_id", &OTy2::guildId, "token", &OTy2::token, "version", &OTy2::version, "user",
			&OTy2::user, "id", &OTy2::id);
	};

	template<> struct Core<DiscordCoreAPI::InputEventData> {
		using OTy2 = DiscordCoreAPI::InputEventData;
		constexpr static auto parseValue = object("response_type", &OTy2::responseType, "interaction_data", &OTy2::interactionData);
	};

	template<> struct Core<DiscordCoreAPI::ThreadMembersUpdateData> {
		using OTy2 = DiscordCoreAPI::ThreadMembersUpdateData;
		constexpr static auto parseValue = object("added_members", &OTy2::addedMembers, "member_count", &OTy2::memberCount, "guild_id",
			&OTy2::guildId, "removed_member_ids", &OTy2::removedMemberIds);
	};

	template<> struct Core<DiscordCoreAPI::WebHookData> {
		using OTy2 = DiscordCoreAPI::WebHookData;
		constexpr static auto parseValue = object("application_id", &OTy2::applicationId, "avatar", &OTy2::avatar, "channel_id", &OTy2::channelId,
			"guild_id", &OTy2::guildId, "id", &OTy2::id, "name", &OTy2::name, "source_channel", &OTy2::sourceChannel, "source_guild",
			&OTy2::sourceGuild, "token", &OTy2::token, "type", &OTy2::type, "url", &OTy2::url, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::AuditLogChangeData> {
		using OTy2 = DiscordCoreAPI::AuditLogChangeData;
		constexpr static auto parseValue = object("key", &OTy2::key, "new_value", &OTy2::newValue, "old_value", &OTy2::oldValue);
	};

	template<> struct Core<DiscordCoreAPI::OptionalAuditEntryInfoData> {
		using OTy2 = DiscordCoreAPI::OptionalAuditEntryInfoData;
		constexpr static auto parseValue = object("delete_member_days", &OTy2::deleteMemberDays, "members_removed", &OTy2::membersRemoved,
			"application_id", &OTy2::applicationId, "role_name", &OTy2::roleName, "channel_id", &OTy2::channelId, "message_id", &OTy2::messageId,
			"count", &OTy2::count, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::AutoModerationRuleData> {
		using OTy2 = DiscordCoreAPI::AutoModerationRuleData;
		constexpr static auto parseValue = object("actions", &OTy2::actions, "creator_id", &OTy2::creatorId, "enabled", &OTy2::enabled, "event_type",
			&OTy2::eventType, "exempt_channels", &OTy2::exemptChannels, "exempt_roles", &OTy2::exemptRoles, "guild_id", &OTy2::guildId, "id",
			&OTy2::id, "name", &OTy2::name, "trigger_metadata", &OTy2::triggerMetaData, "trigger_type", &OTy2::triggerType);
	};

	template<> struct Core<DiscordCoreAPI::ArchivedThreadsData> {
		using OTy2 = DiscordCoreAPI::ArchivedThreadsData;
		constexpr static auto parseValue = object("has_more", &OTy2::hasMore, "member", &OTy2::members, "threads", &OTy2::threads);
	};

	template<> struct Core<DiscordCoreAPI::Sticker> {
		using OTy2 = DiscordCoreAPI::Sticker;
		constexpr static auto parseValue = object("asset", &OTy2::asset, "description", &OTy2::description, "flags", &OTy2::flags, "format_type",
			&OTy2::formatType, "guild_id", &OTy2::guildId, "id", &OTy2::id, "name", &OTy2::name, "nsfw_level", &OTy2::nsfwLevel, "pack_id",
			&OTy2::packId, "sort_value", &OTy2::sortValue, "tags", &OTy2::tags, "type", &OTy2::type, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::ConnectionData> {
		using OTy2 = DiscordCoreAPI::ConnectionData;
		constexpr static auto parseValue = object("friend_sync", &OTy2::friendSync, "id", &OTy2::id, "integrations", &OTy2::integrations, "name",
			&OTy2::name, "revoked", &OTy2::revoked, "show_activity", &OTy2::showActivity, "type", &OTy2::type, "verified", &OTy2::verified,
			"visibility", &OTy2::visibility);
	};

	template<> struct Core<DiscordCoreAPI::GuildPruneCountData> {
		using OTy2 = DiscordCoreAPI::GuildPruneCountData;
		constexpr static auto parseValue = object("count", &OTy2::count);
	};

	template<> struct Core<DiscordCoreAPI::ThreadData> {
		using OTy2 = DiscordCoreAPI::ThreadData;
		constexpr static auto parseValue = object("guild_id", &OTy2::guildId, "default_sort_order", &OTy2::defaultSortOrder, "flags", &OTy2::flags,
			"id", "member_count", &OTy2::memberCount, "name", &OTy2::name, "owner_id", &OTy2::ownerId, "parent_id", &OTy2::parentId,
			"permission_overwrites", &OTy2::permissionOverwrites, "position", &OTy2::position, "topic", &OTy2::topic, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::Thread> {
		using OTy2 = DiscordCoreAPI::Thread;
		constexpr static auto parseValue = object("application_id", &OTy2::applicationId, "applied_tags", &OTy2::appliedTags, "available_tags",
			&OTy2::availableTags, "guild_id", &OTy2::guildId, "bitrate", &OTy2::bitrate, "default_auto_archive_duration",
			&OTy2::defaultAutoArchiveDuration, "default_reaction_emoji", &OTy2::defaultReactionEmoji, "default_sort_order", &OTy2::defaultSortOrder,
			"default_thread_rate_limit_per_user", &OTy2::defaultThreadRateLimitPerUser, "flags", &OTy2::flags, "icon", &OTy2::icon, "id", &OTy2::id,
			"last_message_id", &OTy2::lastMessageId, "last_pin_timestamp", &OTy2::lastPinTimestamp, "member", &OTy2::member, "member_count",
			&OTy2::memberCount, "message_count", &OTy2::messageCount, "name", &OTy2::name, "owner_id", &OTy2::ownerId, "parent_id", &OTy2::parentId,
			"permission_overwrites", &OTy2::permissionOverwrites, "permissions", &OTy2::permissions, "position", &OTy2::position,
			"rate_limit_per_user", &OTy2::rateLimitPerUser, "recipients", &OTy2::recipients, "rtc_region", &OTy2::rtcRegion, "thread_metadata",
			&OTy2::threadMetadata, "topic", &OTy2::topic, "total_message_sent", &OTy2::totalMessageSent, "type", &OTy2::type, "user_limit",
			&OTy2::userLimit, "video_quality_mode", &OTy2::videoQualityMode);
	};

	template<> struct Core<DiscordCoreAPI::OverWriteData> {
		using OTy2 = DiscordCoreAPI::OverWriteData;
		constexpr static auto parseValue = object("allow", &OTy2::allow, "deny", &OTy2::deny, "id", &OTy2::id, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::Role> {
		using OTy2 = DiscordCoreAPI::Role;
		constexpr static auto parseValue = object("color", &OTy2::color, "flags", &OTy2::flags, "id", &OTy2::id, "guild_id", &OTy2::guildId, "icon",
			&OTy2::icon, "name", &OTy2::name, "permissions", &OTy2::permissions, "position", &OTy2::position, "tags", &OTy2::tags, "unicode_emoji",
			&OTy2::unicodeEmoji);
	};

	template<> struct Core<DiscordCoreAPI::ForumTagData> {
		using OTy2 = DiscordCoreAPI::ForumTagData;
		constexpr static auto parseValue =
			object("emoji_id", &OTy2::emojiId, "emoji_name", &OTy2::emojiName, "id", &OTy2::id, "moderated", &OTy2::moderated, "name", &OTy2::name);
	};

	template<> struct Core<DiscordCoreAPI::DefaultReactionData> {
		using OTy2 = DiscordCoreAPI::DefaultReactionData;
		constexpr static auto parseValue = object("emoji_id", &OTy2::emojiId, "emoji_name", &OTy2::emojiName);
	};

	template<> struct Core<DiscordCoreAPI::VoiceRegionData> {
		using OTy2 = DiscordCoreAPI::VoiceRegionData;
		constexpr static auto parseValue =
			object("custom", &OTy2::custom, "depracated", &OTy2::deprecated, "id", &OTy2::id, "name", &OTy2::name, "optimal", &OTy2::optimal);
	};

	template<> struct Core<DiscordCoreAPI::InviteData> {
		using OTy2 = DiscordCoreAPI::InviteData;
		constexpr static auto parseValue =
			object("approximate_member_count", &OTy2::approximateMemberCount, "approximate_presence_count", &OTy2::approximatePresenceCount,
				"channel", &OTy2::channel, "guild_id", &OTy2::guildId, "code", &OTy2::code, "created_at", &OTy2::createdAt, "expires_at",
				&OTy2::expiresAt, "guild", &OTy2::guild, "guild_scheduled_event", &OTy2::guildScheduledEvent, "inviter", &OTy2::inviter, "max_age",
				&OTy2::maxAge, "max_uses", &OTy2::maxUses, "stage_instance", &OTy2::stageInstance, "target_application", &OTy2::targetApplication,
				"target_type", &OTy2::targetType, "target_user", &OTy2::targetUser, "temporary", &OTy2::temporary, "uses", &OTy2::uses);
	};

	template<> struct Core<DiscordCoreAPI::AutoModerationRule> {
		using OTy2 = DiscordCoreAPI::AutoModerationRule;
		constexpr static auto parseValue = object("actions", &OTy2::actions, "creator_id", &OTy2::creatorId, "enabled", &OTy2::enabled, "guild_id",
			&OTy2::guildId, "event_type", &OTy2::eventType, "exempt_channels", &OTy2::exemptChannels, "exempt_roles", &OTy2::exemptRoles, "id",
			&OTy2::id, "trigger_meta_data", &OTy2::triggerMetaData, "trigger_type", &OTy2::triggerType);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandData> {
		using OTy2 = DiscordCoreAPI::ApplicationCommandData;
		constexpr static auto parseValue = object("application_id", &OTy2::applicationId, "default_member_permissions",
			&OTy2::defaultMemberPermissions, "description", &OTy2::description, "guild_id", &OTy2::guildId, "description_localizations",
			&OTy2::descriptionLocalizations, "dm_permission", &OTy2::dmPermission, "id", &OTy2::id, "name", &OTy2::name, "name_localizations",
			&OTy2::nameLocalizations, "options", &OTy2::options, "type", &OTy2::type, "version", &OTy2::version);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommand> {
		using OTy2 = DiscordCoreAPI::ApplicationCommand;
		constexpr static auto parseValue = object("application_id", &OTy2::applicationId, "default_member_permissions",
			&OTy2::defaultMemberPermissions, "description", &OTy2::description, "guild_id", &OTy2::guildId, "description_localizations",
			&OTy2::descriptionLocalizations, "dm_permission", &OTy2::dmPermission, "id", &OTy2::id, "name", &OTy2::name, "name_localizations",
			&OTy2::nameLocalizations, "options", &OTy2::options, "type", &OTy2::type, "version", &OTy2::version);
	};

	template<> struct Core<DiscordCoreAPI::StickerData> {
		using OTy2 = DiscordCoreAPI::StickerData;
		constexpr static auto parseValue = object("asset", &OTy2::asset, "description", &OTy2::description, "format_type", &OTy2::formatType, "id",
			&OTy2::id, "name", &OTy2::name, "pack_id", &OTy2::packId, "tags", &OTy2::tags, "type", &OTy2::type, "user", &OTy2::user, "guild_id",
			&OTy2::guildId, "sort_value", &OTy2::sortValue, "nsfw_level", &OTy2::nsfwLevel, "flags", &OTy2::flags);
	};

	template<> struct Core<DiscordCoreAPI::MessageActivityData> {
		using OTy2 = DiscordCoreAPI::MessageActivityData;
		constexpr static auto parseValue = object("type", &OTy2::type, "party_id", &OTy2::partyId);
	};

	template<> struct Core<DiscordCoreAPI::ActiveThreadsData> {
		using OTy2 = DiscordCoreAPI::ActiveThreadsData;
		constexpr static auto parseValue = object("has_more", &OTy2::hasMore, "members", &OTy2::members, "threads", &OTy2::threads);
	};

	template<> struct Core<DiscordCoreAPI::ClientStatus> {
		using OTy2 = DiscordCoreAPI::ClientStatus;
		constexpr static auto parseValue = object("web", &OTy2::web, "mobile", &OTy2::mobile, "desktop", &OTy2::desktop);
	};

	template<> struct Core<DiscordCoreAPI::ActivityData> {
		using OTy2 = DiscordCoreAPI::ActivityData;
		constexpr static auto parseValue = object("name", &OTy2::name, "type", &OTy2::type, "url", &OTy2::url);
	};

	template<> struct Core<DiscordCoreAPI::PresenceUpdateData> {
		using OTy2 = DiscordCoreAPI::PresenceUpdateData;
		constexpr static auto parseValue = object("client_status", &OTy2::clientStatus, "guild_id", &OTy2::guildId, "status", &OTy2::status, "user",
			&OTy2::user, "activities", &OTy2::activities);
	};

	template<> struct Core<DiscordCoreAPI::AccountData> {
		using OTy2 = DiscordCoreAPI::AccountData;
		constexpr static auto parseValue = object("name", &OTy2::name);
	};

	template<> struct Core<DiscordCoreAPI::EmbedData> {
		using OTy2 = DiscordCoreAPI::EmbedData;
		constexpr static auto parseValue = object("author", &OTy2::author, "color", &OTy2::hexColorValue, "description", &OTy2::description, "fields",
			&OTy2::fields, "footer", &OTy2::footer, "image", &OTy2::image, "provider", &OTy2::provider, "thumbnail", &OTy2::thumbnail, "timestamp",
			&OTy2::timeStamp, "title", &OTy2::title, "type", &OTy2::type, "url", &OTy2::url, "video", &OTy2::video);
	};

	template<> struct Core<DiscordCoreAPI::MessageDataOld> {
		using OTy2 = DiscordCoreAPI::MessageDataOld;
		constexpr static auto parseValue = object("activity", &OTy2::activity, "application", &OTy2::application, "application_id",
			&OTy2::applicationId, "attachments", &OTy2::attachments, "author", &OTy2::author, "channel_id", &OTy2::channelId, "components",
			&OTy2::components, "content", &OTy2::content, "edited_timestamp", &OTy2::editedTimestamp, "embeds", &OTy2::embeds, "flags", &OTy2::flags,
			"guild_id", &OTy2::guildId, "id", &OTy2::id, "interaction", &OTy2::interaction, "mention_channels", &OTy2::mentionChannels,
			"mention_everyone", &OTy2::mentionEveryone, "mention_roles", &OTy2::mentionRoles, "mentions", &OTy2::mentions, "message_reference",
			&OTy2::messageReference, "nonce", &OTy2::nonce, "pinned", &OTy2::pinned, "reactions", &OTy2::reactions, "sticker_items",
			&OTy2::stickerItems, "stickers", &OTy2::stickers, "thread", &OTy2::thread, "timestamp", &OTy2::timeStamp, "tts", &OTy2::tts, "type",
			&OTy2::type, "webhook_id", &OTy2::webHookId);
	};

	template<> struct Core<DiscordCoreAPI::ReactionData> {
		using OTy2 = DiscordCoreAPI::ReactionData;
		constexpr static auto parseValue = object("channel_id", &OTy2::channelId, "count", &OTy2::count, "emoji", &OTy2::emoji, "guild_id",
			&OTy2::guildId, "id", &OTy2::id, "member", &OTy2::member, "me", &OTy2::me, "message_id", &OTy2::messageId, "user_id", &OTy2::userId);
	};

	template<> struct Core<DiscordCoreAPI::StickerItemData> {
		using OTy2 = DiscordCoreAPI::StickerItemData;
		constexpr static auto parseValue = object("format_type", &OTy2::formatType, "name", &OTy2::name);
	};

	template<> struct Core<DiscordCoreAPI::AttachmentData> {
		using OTy2 = DiscordCoreAPI::AttachmentData;
		constexpr static auto parseValue = object("content_type", &OTy2::contentType, "description", &OTy2::description, "filename", &OTy2::filename,
			"height", &OTy2::height, "id", &OTy2::id, "proxy_url", &OTy2::proxyUrl, "size", &OTy2::size, "url", &OTy2::url, "width", &OTy2::width,
			"ephemeral", &OTy2::ephemeral);
	};

	template<> struct Core<DiscordCoreAPI::ChannelMentionData> {
		using OTy2 = DiscordCoreAPI::ChannelMentionData;
		constexpr static auto parseValue = object("id", &OTy2::id, "guild_id", &OTy2::guildId, "name", &OTy2::name, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::ActionRowData> {
		using OTy2 = DiscordCoreAPI::ActionRowData;
		constexpr static auto parseValue = object("components", &OTy2::components, "type", &OTy2::type);
	};

	template<> struct Core<DiscordCoreAPI::EmbedFooterData> {
		using OTy2 = DiscordCoreAPI::EmbedFooterData;
		constexpr static auto parseValue = object("proxy_icon_url", &OTy2::proxyIconUrl, "icon_url", &OTy2::iconUrl, "text", &OTy2::text);
	};

	template<> struct Core<DiscordCoreAPI::EmbedImageData> {
		using OTy2 = DiscordCoreAPI::EmbedImageData;
		constexpr static auto parseValue = object("proxy_url", &OTy2::proxyUrl, "url", &OTy2::url, "height", &OTy2::height, "width", &OTy2::width);
	};

	template<> struct Core<DiscordCoreAPI::EmbedThumbnailData> {
		using OTy2 = DiscordCoreAPI::EmbedThumbnailData;
		constexpr static auto parseValue = object("proxy_url", &OTy2::proxyUrl, "url", &OTy2::url, "height", &OTy2::height, "width", &OTy2::width);
	};

	template<> struct Core<DiscordCoreAPI::EmbedVideoData> {
		using OTy2 = DiscordCoreAPI::EmbedVideoData;
		constexpr static auto parseValue = object("proxy_url", &OTy2::proxyUrl, "url", &OTy2::url, "height", &OTy2::height, "width", &OTy2::width);
	};

	template<> struct Core<DiscordCoreAPI::EmbedProviderData> {
		using OTy2 = DiscordCoreAPI::EmbedProviderData;
		constexpr static auto parseValue = object("name", &OTy2::name, "url", &OTy2::url);
	};

	template<> struct Core<DiscordCoreAPI::EmbedAuthorData> {
		using OTy2 = DiscordCoreAPI::EmbedAuthorData;
		constexpr static auto parseValue =
			object("proxy_icon_url", &OTy2::proxyIconUrl, "icon_url", &OTy2::iconUrl, "name", &OTy2::name, "url", &OTy2::url);
	};

	template<> struct Core<DiscordCoreAPI::EmbedFieldData> {
		using OTy2 = DiscordCoreAPI::EmbedFieldData;
		constexpr static auto parseValue = object("name", &OTy2::name, "value", &OTy2::value, "inline", &OTy2::Inline);
	};

	template<> struct Core<DiscordCoreAPI::GuildData> {
		using OTy2 = DiscordCoreAPI::GuildData;
		constexpr static auto parseValue = object("widget_enabled", &OTy2::flags, "unavailable", &OTy2::flags, "owner", &OTy2::flags, "large",
			&OTy2::flags, "member_count", &OTy2::memberCount, "id", &OTy2::id, "channels", &OTy2::channels, "roles", &OTy2::roles, "members",
			&OTy2::members, "name", &OTy2::name, "emoji", &OTy2::emoji, "flags", &OTy2::flags, "guild_scheduled_events", &OTy2::guildScheduledEvents,
			"icon", &OTy2::icon, "joined_at", &OTy2::joinedAt, "owner_id", &OTy2::ownerId, "presences", &OTy2::presences, "stage_instances",
			&OTy2::stageInstances, "stickers", &OTy2::stickers, "threads", &OTy2::threads);
	};

	template<> struct Core<DiscordCoreAPI::AuditLogEntryData> {
		using OTy2 = DiscordCoreAPI::AuditLogEntryData;
		constexpr static auto parseValue = object("action_type", &OTy2::actionType, "changes", &OTy2::changes, "created_at", &OTy2::createdTimeStamp,
			"id", &OTy2::id, "options", &OTy2::options, "reason", &OTy2::reason, "target_id", &OTy2::targetId, "user_id", &OTy2::userId);
	};

	template<> struct Core<DiscordCoreAPI::ComponentData> {
		using OTy2 = DiscordCoreAPI::ComponentData;
		constexpr static auto parseValue = object("type", &OTy2::type, "custom_id", &OTy2::customId, "options", &OTy2::options, "channel_types",
			&OTy2::channelTypes, "placeholder", &OTy2::placeholder, "min_values", &OTy2::minValues, "max_values", &OTy2::maxValues, "min_length",
			&OTy2::minLength, "max_length", &OTy2::maxLength, "label", &OTy2::label, "value", &OTy2::value, "title", &OTy2::title, "emoji",
			&OTy2::emoji, "url", &OTy2::url, "required", &OTy2::required, "disabled", &OTy2::disabled, "style", &OTy2::style);
	};

	template<> struct Core<DiscordCoreAPI::SelectOptionData> {
		using OTy2 = DiscordCoreAPI::SelectOptionData;
		constexpr static auto parseValue = object("description", &OTy2::description, "label", &OTy2::label, "value", &OTy2::value, "emoji",
			&OTy2::emoji, "default", &OTy2::_default);
	};

	template<> struct Core<DiscordCoreAPI::VoiceStateDataLight> {
		using OTy2 = DiscordCoreAPI::VoiceStateDataLight;
		constexpr static auto parseValue = object("channel_id", &OTy2::channelId, "guild_id", &OTy2::guildId, "user_id", &OTy2::id);
	};

	template<> struct Core<DiscordCoreAPI::GuildCacheData> {
		using OTy2 = DiscordCoreAPI::GuildCacheData;
		constexpr static auto parseValue = object("widget_enabled", &OTy2::flags, "unavailable", &OTy2::flags, "owner", &OTy2::flags, "channels",
			&OTy2::channels, "emoji", &OTy2::emoji, "flags", &OTy2::flags, "guild_scheduled_events", &OTy2::guildScheduledEvents, "icon", &OTy2::icon,
			"id", &OTy2::id, "joined_at", &OTy2::joinedAt, "member_count", &OTy2::memberCount, "members", &OTy2::members, "name", &OTy2::name,
			"owner_id", &OTy2::ownerId, "presences", &OTy2::presences, "roles", &OTy2::roles, "stage_instances", &OTy2::stageInstances, "stickers",
			&OTy2::stickers, "threads", &OTy2::threads, "voice_states", &OTy2::voiceStates);
	};

	template<> struct Core<DiscordCoreAPI::Guild> {
		using OTy2 = DiscordCoreAPI::Guild;
		constexpr static auto parseValue =
			object("widget_enabled", &OTy2::flags, "unavailable", &OTy2::flags, "owner", &OTy2::flags, "large", &OTy2::flags, "member_count",
				&OTy2::memberCount, "id", &OTy2::id, "channels", &OTy2::channels, "roles", &OTy2::roles, "members", &OTy2::members, "name",
				&OTy2::name, "emoji", &OTy2::emoji, "flags", &OTy2::flags, "guild_scheduled_events", &OTy2::guildScheduledEvents, "icon", &OTy2::icon,
				"joined_at", &OTy2::joinedAt, "owner_id", &OTy2::ownerId, "presences", &OTy2::presences, "stage_instances", &OTy2::stageInstances,
				"stickers", &OTy2::stickers, "threads", &OTy2::threads, "afk_channel_id", &OTy2::afkChannelId, "afk_timeout", &OTy2::afkTimeOut,
				"approximate_member_count", &OTy2::approximateMemberCount, "approximate_presence_count", &OTy2::approximatePresenceCount,
				"application_id", &OTy2::applicationId, "banner", &OTy2::banner, "channels", &OTy2::channels, "default_message_notifications",
				&OTy2::defaultMessageNotifications, "description", &OTy2::description, "discovery_splash", &OTy2::discoverySplash, "emojis",
				&OTy2::emoji, "explicit_content_filter", &OTy2::explicitContentFilter, "features", &OTy2::features, "guild_scheduled_events",
				&OTy2::guildScheduledEvents, "icon", &OTy2::icon, "id", &OTy2::id, "max_members", &OTy2::maxMembers, "max_presences",
				&OTy2::maxPresences, "max_video_channel_users", &OTy2::maxVideoChannelUsers, "members", &OTy2::members, "mfa_level", &OTy2::mfaLevel,
				"name", &OTy2::name, "nsfw_level", &OTy2::nsfwLevel, "owner_id", &OTy2::ownerId, "preferred_locale", &OTy2::preferredLocale,
				"premium_subscription_count", &OTy2::premiumSubscriptionCount, "premium_tier", &OTy2::premiumTier, "presences", &OTy2::presences,
				"public_updates_channel_id", &OTy2::publicUpdatesChannelId, "region", &OTy2::region, "rules_channel_id", &OTy2::rulesChannelId,
				"splash", &OTy2::splash, "stage_instances", &OTy2::stageInstances, "system_channel_flags", &OTy2::systemChannelFlags,
				"system_channel_id", &OTy2::systemChannelId, "threads", &OTy2::threads, "vanity_url_code", &OTy2::vanityUrlCode, "verification_level",
				&OTy2::verificationLevel, "welcome_screen", &OTy2::welcomeScreen, "widget_channel_id", &OTy2::widgetChannelId);
	};

	template<> struct Core<DiscordCoreAPI::IntegrationData> {
		using OTy2 = DiscordCoreAPI::IntegrationData;
		constexpr static auto parseValue = object("account", &OTy2::account, "application", &OTy2::application, "enabled", &OTy2::enabled,
			"enable_emoticons", &OTy2::enableEmoticons, "expire_behavior", &OTy2::expireBehavior, "expire_grace_period", &OTy2::expireGracePeriod,
			"id", &OTy2::id, "name", &OTy2::name, "revoked", &OTy2::revoked, "role_id", &OTy2::roleId, "subscriber_count", &OTy2::subscriberCount,
			"syncing", &OTy2::syncing, "synced_at", &OTy2::syncedAt, "type", &OTy2::type, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::File> {
		using OTy2 = DiscordCoreAPI::File;
		static constexpr auto parseValue = array(&OTy2::data);
	};

	template<> struct Core<DiscordCoreAPI::AllowedMentionsData> {
		using OTy2 = DiscordCoreAPI::AllowedMentionsData;
		static constexpr auto parseValue =
			object("replied_user", &OTy2::repliedUser, "parse", &OTy2::parse, "roles", &OTy2::roles, "users", &OTy2::users);
	};

	template<> struct Core<DiscordCoreAPI::MessageReferenceData> {
		using OTy2 = DiscordCoreAPI::MessageReferenceData;
		constexpr static auto parseValue = object("fail_if_not_exists", &OTy2::failIfNotExists, "message_id", &OTy2::messageId, "channel_id",
			&OTy2::channelId, "guild_id", &OTy2::guildId);
	};

	template<> struct Core<DiscordCoreAPI::MessageInteractionData> {
		using OTy2 = DiscordCoreAPI::MessageInteractionData;
		constexpr static auto parseValue =
			object("id", &OTy2::id, "member", &OTy2::member, "name", &OTy2::name, "type", &OTy2::type, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventData> {
		using OTy2 = DiscordCoreAPI::GuildScheduledEventData;
		constexpr static auto parseValue = object("id", &OTy2::id, "privacy_level", &OTy2::privacyLevel, "entity_metadata", &OTy2::entityMetadata,
			"entity_type", &OTy2::entityType, "status", &OTy2::status, "scheduled_start_time", &OTy2::scheduledStartTime, "scheduled_end_time",
			&OTy2::scheduledEndTime, "description", &OTy2::description, "creator_id", &OTy2::creatorId, "channel_id", &OTy2::channelId, "entity_id",
			&OTy2::entityId, "user_count", &OTy2::userCount, "guild_id", &OTy2::guildId, "creator", &OTy2::creator, "name", &OTy2::name);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventUserData> {
		using OTy2 = DiscordCoreAPI::GuildScheduledEventUserData;
		constexpr static auto parseValue =
			object("guild_scheduled_event_id", &OTy2::guildScheduledEventId, "member", &OTy2::member, "user", &OTy2::user);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEvent> {
		using OTy2 = DiscordCoreAPI::GuildScheduledEvent;
		constexpr static auto parseValue = object("id", &OTy2::id, "privacy_level", &OTy2::privacyLevel, "entity_metadata", &OTy2::entityMetadata,
			"entity_type", &OTy2::entityType, "status", &OTy2::status, "scheduled_start_time", &OTy2::scheduledStartTime, "scheduled_end_time",
			&OTy2::scheduledEndTime, "description", &OTy2::description, "creator_id", &OTy2::creatorId, "channel_id", &OTy2::channelId, "entity_id",
			&OTy2::entityId, "user_count", &OTy2::userCount, "guild_id", &OTy2::guildId, "creator", &OTy2::creator, "name", &OTy2::name);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventMetadata> {
		using OTy2 = DiscordCoreAPI::GuildScheduledEventMetadata;
		constexpr static auto parseValue = object("location", &OTy2::location);
	};

	template<> struct Core<DiscordCoreAPI::RoleData> {
		using OTy2 = DiscordCoreAPI::RoleData;
		constexpr static auto parseValue = object("color", &OTy2::color, "flags", &OTy2::flags, "guild_id", &OTy2::guildId, "id", &OTy2::id, "name",
			&OTy2::name, "permissions", &OTy2::permissions, "position", &OTy2::position, "unicode_emoji", &OTy2::unicodeEmoji);
	};

	template<> struct Core<DiscordCoreAPI::Message> {
		using OTy2 = DiscordCoreAPI::Message;
		constexpr static auto parseValue = object("activity", &OTy2::activity, "application", &OTy2::application, "application_id",
			&OTy2::applicationId, "attachments", &OTy2::attachments, "author", &OTy2::author, "channel_id", &OTy2::channelId, "components",
			&OTy2::components, "content", &OTy2::content, "edited_timestamp", &OTy2::editedTimestamp, "embeds", &OTy2::embeds, "flags", &OTy2::flags,
			"guild_id", &OTy2::guildId, "id", &OTy2::id, "interaction", &OTy2::interaction, "member", &OTy2::member, "mention_channels",
			&OTy2::mentionChannels, "mention_everyone", &OTy2::mentionEveryone, "mention_roles", &OTy2::mentionRoles, "mentions", &OTy2::mentions,
			"message_reference", &OTy2::messageReference, "nonce", &OTy2::nonce, "pinned", &OTy2::pinned, "reactions", &OTy2::reactions,
			"referenced_message", &OTy2::referencedMessage, "sticker_items", &OTy2::stickerItems, "stickers", &OTy2::stickers, "thread",
			&OTy2::thread, "timestamp", &OTy2::timeStamp, "tts", &OTy2::tts, "type", &OTy2::type, "webhook_id", &OTy2::webHookId);
	};

	template<> struct Core<DiscordCoreAPI::StickerPackData> {
		using OTy2 = DiscordCoreAPI::StickerPackData;
		constexpr static auto parseValue = object("banner_asset_id", &OTy2::bannerAssetId, "cover_sticker_id", &OTy2::coverStickerId, "description",
			&OTy2::description, "id", &OTy2::Id, "name", &OTy2::name, "sku_id", &OTy2::skuId, "stickers", &OTy2::stickers);
	};

	template<> struct Core<DiscordCoreAPI::GuildWidgetImageData> {
		using OTy2 = DiscordCoreAPI::GuildWidgetImageData;
		constexpr static auto parseValue = object("url", &OTy2::url);
	};

	template<> struct Core<DiscordCoreAPI::GuildTemplateData> {
		using OTy2 = DiscordCoreAPI::GuildTemplateData;
		static constexpr auto parseValue = object("code", &OTy2::code, "created_at", &OTy2::createdAt, "creator", &OTy2::creator, "creator_id",
			&OTy2::creatorId, "description", &OTy2::description, "is_dirty", &OTy2::isDirty, "name", &OTy2::name, "serialized_source_guild",
			&OTy2::serializedSourceGuild, "source_guild_id", &OTy2::sourceGuildId, "updated_at", &OTy2::updatedAt);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandOptionData> {
		using OTy2 = DiscordCoreAPI::ApplicationCommandOptionData;
		constexpr static auto parseValue = object("type", &OTy2::type, "name", &OTy2::name, "description", &OTy2::description, "required",
			&OTy2::required, "autocomplete", &OTy2::autocomplete, "min_value", &OTy2::minValue, "max_value", &OTy2::maxValue, "channel_types",
			&OTy2::channelTypes, "description_localizations", &OTy2::descriptionLocalizations, "name_localizations", &OTy2::nameLocalizations,
			"choices", &OTy2::choices, "options", &OTy2::options);
	};

	template<> struct Core<DiscordCoreAPI::StageInstanceData> {
		using OTy2 = DiscordCoreAPI::StageInstanceData;
		constexpr static auto parseValue = object("channel_id", &OTy2::channelId, "discoverable_disabled", &OTy2::discoverableDisabled, "guild_id",
			&OTy2::guildId, "id", &OTy2::id, "privacy_level", &OTy2::privacyLevel, "topic", &OTy2::topic);
	};

	template<> struct Core<DiscordCoreAPI::StageInstance> {
		using OTy2 = DiscordCoreAPI::StageInstance;
		constexpr static auto parseValue = object("privacy_level", &OTy2::privacyLevel, "discoverable_disabled", &OTy2::discoverableDisabled,
			"channel_id", &OTy2::channelId, "guild_id", &OTy2::guildId, "topic", &OTy2::topic);
	};

	template<> struct Core<DiscordCoreAPI::ApplicationCommandOptionChoiceData> {
		using OTy2 = DiscordCoreAPI::ApplicationCommandOptionChoiceData;
		constexpr static auto parseValue =
			object("name", &OTy2::name, "value", &OTy2::value, "type", &OTy2::type, "name_localized", &OTy2::nameLocalizations);
	};

	template<> struct Core<DiscordCoreAPI::RoleTagsData> {
		using OTy2 = DiscordCoreAPI::RoleTagsData;
		constexpr static auto parseValue =
			object("premium_subscriber", &OTy2::premiumSubscriber, "integration_id", &OTy2::integrationId, "bot_id", &OTy2::botId);
	};

	template<> struct Core<DiscordCoreInternal::ConnectProperties> {
		using OTy2 = DiscordCoreInternal::ConnectProperties;
		constexpr static auto parseValue = object("os", &OTy2::os, "device", &OTy2::device, "browser", &OTy2::browser);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketIdentifyData> {
		using OTy2 = DiscordCoreInternal::WebSocketIdentifyData;
		constexpr static auto parseValue = object("token", &OTy2::botToken, "shard", &OTy2::shard, "intents", &OTy2::intents, "large_threshold",
			&OTy2::largeThreshold, "presence", &OTy2::presence, "properties", &OTy2::properties);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketResumeData> {
		using OTy2 = DiscordCoreInternal::WebSocketResumeData;
		constexpr static auto parseValue = object("token", &OTy2::botToken, "session_id", &OTy2::sessionId, "seq", &OTy2::lastNumberReceived);
	};

	template<> struct Core<DiscordCoreInternal::VoiceSocketProtocolPayloadDataData> {
		using OTy2 = DiscordCoreInternal::VoiceSocketProtocolPayloadDataData;
		static constexpr auto parseValue = object("address", &OTy2::address, "mode", &OTy2::mode, "port", &OTy2::port);
	};

	template<> struct Core<DiscordCoreInternal::VoiceSocketProtocolPayloadData> {
		using OTy2 = DiscordCoreInternal::VoiceSocketProtocolPayloadData;
		static constexpr auto parseValue = object("data", &OTy2::data, "protocol", &OTy2::protocol);
	};

	template<> struct Core<DiscordCoreInternal::VoiceIdentifyData> {
		using OTy2 = DiscordCoreInternal::VoiceIdentifyData;
		static constexpr auto parseValue =
			object("session_id", &OTy2::sessionId, "user_id", &OTy2::userId, "token", &OTy2::token, "server_id", &OTy2::serverId);
	};

	template<> struct Core<DiscordCoreInternal::SendSpeakingData> {
		using OTy2 = DiscordCoreInternal::SendSpeakingData;
		static constexpr auto parseValue = object("speaking", &OTy2::type, "delay", &OTy2::delay, "ssrc", &OTy2::ssrc);
	};

	template<> struct Core<DiscordCoreInternal::HelloData> {
		using OTy2 = DiscordCoreInternal::HelloData;
		constexpr static auto parseValue = object("heartbeat_interval", &OTy2::heartbeatInterval, "_trace", &OTy2::_trace);
	};

	template<> struct Core<DiscordCoreAPI::UpdatePresenceData> {
		using OTy2 = DiscordCoreAPI::UpdatePresenceData;
		constexpr static auto parseValue = object("afk", &OTy2::afk, "since", &OTy2::since, "status", &OTy2::status, "activities", &OTy2::activities);
	};
}
