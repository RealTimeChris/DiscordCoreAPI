/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// EventManager.hpp - Header for the event manager class.
/// Jun 7, 2021
/// https://discordcoreapi.com
/// \file EventManager.cpp

#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::AutoModerationActionExecutionEventData> {
		using ValueType = DiscordCoreAPI::AutoModerationActionExecutionEventData;
		static constexpr auto parseValue = object("alert_system_message_id", &ValueType::alertSystemMessageId, "rule_trigger_type",
			&ValueType::ruleTriggerType, "matched_keyword", &ValueType::matchedKeyword, "matched_content", &ValueType::matchedContent, "content",
			&ValueType::content, "channel_id", &ValueType::channelId, "message_id", &ValueType::messageId, "action", &ValueType::action, "guild_id",
			&ValueType::guildId, "rule_id", &ValueType::ruleId, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::VoiceStateData> {
		using ValueType2 = DiscordCoreAPI::VoiceStateData;
		static constexpr auto parseValue = object("request_to_speak_timestamp", &ValueType2::requestToSpeakTimestamp, "session_id",
			&ValueType2::sessionId, "self_stream", &ValueType2::selfStream, "self_video", &ValueType2::selfVideo, "self_deaf", &ValueType2::selfDeaf,
			"self_mute", &ValueType2::selfMute, "suppress", &ValueType2::suppress, "deaf", &ValueType2::deaf, "mute", &ValueType2::mute, "user_id",
			&ValueType2::id, "channel_id", &ValueType2::channelId, "guild_id", &ValueType2::guildId);
	};

	template<> struct Core<DiscordCoreAPI::GuildEmojisUpdateEventData> {
		using ValueType = DiscordCoreAPI::GuildEmojisUpdateEventData;
		static constexpr auto parseValue = object("emojis", &ValueType::emojis, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::ChannelPinsUpdateEventData> {
		using ValueType = DiscordCoreAPI::ChannelPinsUpdateEventData;
		static constexpr auto parseValue =
			object("last_pin_timestamp", &ValueType::lastPinTimeStamp, "channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::TypingStartData> {
		using ValueType = DiscordCoreAPI::TypingStartData;
		static constexpr auto parseValue = object("member", &ValueType::member, "channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId,
			"timestamp", &ValueType::timeStamp, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::ReactionRemoveData> {
		using ValueType = DiscordCoreAPI::ReactionRemoveData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "message_id", &ValueType::messageId, "guild_id",
			&ValueType::guildId, "user_id", &ValueType::userId, "emoji", &ValueType::emoji);
	};

	template<> struct Core<DiscordCoreAPI::GuildStickersUpdateEventData> {
		using ValueType = DiscordCoreAPI::GuildStickersUpdateEventData;
		static constexpr auto parseValue = object("stickers", &ValueType::stickers, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::VoiceServerUpdateData> {
		using ValueType = DiscordCoreAPI::VoiceServerUpdateData;
		static constexpr auto parseValue = object("endpoint", &ValueType::endpoint, "guild_id", &ValueType::guildId, "token", &ValueType::token);
	};

	template<> struct Core<DiscordCoreAPI::GuildMembersChunkEventData> {
		using ValueType = DiscordCoreAPI::GuildMembersChunkEventData;
		static constexpr auto parseValue =
			object("presences", &ValueType::presences, "members", &ValueType::members, "not_found", &ValueType::notFound, "chunk_index",
				&ValueType::chunkIndex, "chunk_count", &ValueType::chunkCount, "guild_id", &ValueType::guildId, "nonce", &ValueType::nonce);
	};

	template<> struct Core<DiscordCoreAPI::GuildMemberRemoveData> {
		using ValueType = DiscordCoreAPI::GuildMemberRemoveData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::GuildBanAddData> {
		using ValueType = DiscordCoreAPI::GuildBanAddData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::GuildBanRemoveData> {
		using ValueType = DiscordCoreAPI::GuildBanRemoveData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::RoleCreationData> {
		using ValueType = DiscordCoreAPI::RoleCreationData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "role", &ValueType::role);
	};

	template<> struct Core<DiscordCoreAPI::RoleUpdateData> {
		using ValueType = DiscordCoreAPI::RoleUpdateData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "role", &ValueType::role);
	};

	template<> struct Core<DiscordCoreAPI::RoleDeletionData> {
		using ValueType = DiscordCoreAPI::RoleDeletionData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "role", &ValueType::role);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventUserAddData> {
		using ValueType = DiscordCoreAPI::GuildScheduledEventUserAddData;
		static constexpr auto parseValue =
			object("guild_scheduled_event_id", &ValueType::guildScheduledEventId, "guild_id", &ValueType::guildId, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::GuildScheduledEventUserRemoveData> {
		using ValueType = DiscordCoreAPI::GuildScheduledEventUserRemoveData;
		static constexpr auto parseValue =
			object("guild_scheduled_event_id", &ValueType::guildScheduledEventId, "guild_id", &ValueType::guildId, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::IntegrationCreationData> {
		using ValueType = DiscordCoreAPI::IntegrationCreationData;
		static constexpr auto parseValue = object("integration_data", &ValueType::integrationData, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::IntegrationUpdateData> {
		using ValueType = DiscordCoreAPI::IntegrationUpdateData;
		static constexpr auto parseValue = object("integration_data", &ValueType::integrationData, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::IntegrationDeletionData> {
		using ValueType = DiscordCoreAPI::IntegrationDeletionData;
		static constexpr auto parseValue = object("application_id", &ValueType::applicationId, "guild_id", &ValueType::guildId, "id", &ValueType::id);
	};

	template<> struct Core<DiscordCoreAPI::InviteDeletionData> {
		using ValueType = DiscordCoreAPI::InviteDeletionData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId, "code", &ValueType::code);
	};

	template<> struct Core<DiscordCoreAPI::MessageDeletionData> {
		using ValueType = DiscordCoreAPI::MessageDeletionData;
		static constexpr auto parseValue =
			object("message_id", &ValueType::messageId, "channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::MessageDeletionBulkData> {
		using ValueType = DiscordCoreAPI::MessageDeletionBulkData;
		static constexpr auto parseValue = object("ids", &ValueType::ids, "channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::ReactionRemoveAllData> {
		using ValueType = DiscordCoreAPI::ReactionRemoveAllData;
		static constexpr auto parseValue =
			object("channel_id", &ValueType::channelId, "message_id", &ValueType::messageId, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::ReactionRemoveEmojiData> {
		using ValueType = DiscordCoreAPI::ReactionRemoveEmojiData;
		static constexpr auto parseValue = object("message_id", &ValueType::messageId, "channel_id", &ValueType::channelId, "guild_id",
			&ValueType::guildId, "emoji", &ValueType::emoji);
	};

	template<> struct Core<DiscordCoreAPI::WebHookUpdateData> {
		using ValueType = DiscordCoreAPI::WebHookUpdateData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "guild_id", &ValueType::guildId);
	};

}

namespace DiscordCoreAPI {

	OnInputEventCreationData::OnInputEventCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnApplicationCommandPermissionsUpdateData::OnApplicationCommandPermissionsUpdateData(Jsonifier::JsonifierCore& parser,
		std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnAutoModerationRuleCreationData::OnAutoModerationRuleCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnAutoModerationRuleUpdateData::OnAutoModerationRuleUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnAutoModerationRuleDeletionData::OnAutoModerationRuleDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnAutoModerationActionExecutionData::OnAutoModerationActionExecutionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnChannelCreationData::OnChannelCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Channels::doWeCacheChannels()) {
			if (Guilds::getCache().contains(value.guildId)) {
				Guilds::getCache()[value.guildId].channels.emplace_back(value.id);
			}
			if (Channels::doWeCacheChannels()) {
				Channels::insertChannel(std::move(value));
			}
		}
	}

	OnChannelUpdateData::OnChannelUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<UpdatedEventData*>(this), dataToParse);
		oldValue = Channels::getCachedChannel({ .channelId = value.id });
		if (Channels::doWeCacheChannels()) {
			Channels::insertChannel(std::move(value));
		}
	}

	OnChannelDeletionData::OnChannelDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Channels::doWeCacheChannels()) {
			if (Guilds::getCache().contains(value.guildId)) {
				for (uint64_t x = 0; x < Guilds::getCache()[value.guildId].channels.size(); ++x) {
					if (Guilds::getCache()[value.guildId].channels[x] == value.id) {
						Guilds::getCache()[value.guildId].channels.erase(Guilds::getCache()[value.guildId].channels.begin() + x);
					}
				}
			}
			if (Channels::doWeCacheChannels()) {
				Channels::removeChannel(value.id);
			}
		}
	}

	OnChannelPinsUpdateData::OnChannelPinsUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnThreadCreationData::OnThreadCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			Guilds::getCache()[value.guildId].threads.emplace_back(value.id);
		}
	}

	OnThreadUpdateData::OnThreadUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnThreadDeletionData::OnThreadDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			for (uint64_t x = 0; x < Guilds::getCache()[value.guildId].threads.size(); ++x) {
				if (Guilds::getCache()[value.guildId].threads[x] == value.id) {
					Guilds::getCache()[value.guildId].threads.erase(Guilds::getCache()[value.guildId].threads.begin() + x);
				}
			}
		}
	}

	OnThreadListSyncData::OnThreadListSyncData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnThreadMemberUpdateData::OnThreadMemberUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnThreadMembersUpdateData::OnThreadMembersUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnGuildCreationData::OnGuildCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse, DiscordCoreClient* client) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		value.discordCoreClient = client;
		if (GuildMembers::doWeCacheGuildMembers()) {
			for (auto& valueNew: value.members) {
				try {
					valueNew.guildId = value.id;
					GuildMembers::insertGuildMember(valueNew);
					Users::insertUser(std::move(valueNew.user));
				} catch (...) {
				}
			}
		}
		if (Channels::doWeCacheChannels()) {
			for (auto& valueNew: value.channels) {
				try {
					valueNew.guildId = value.id;
					Channels::insertChannel(std::move(valueNew));
				} catch (...) {
				}
			}
		}
		if (Roles::doWeCacheRoles()) {
			for (auto& valueNew: value.roles) {
				try {
					valueNew.guildId = value.id;
					Roles::insertRole(std::move(valueNew));
				} catch (...) {
				}
			}
		}
		for (auto& valueNew: value.voiceStates) {
			try {
				valueNew.guildId = value.id;
				GuildMembers::insertVoiceState(std::move(valueNew));
			} catch (...) {
			}
		}
		if (Guilds::doWeCacheGuilds()) {
			if (Guilds::doWeCacheGuilds()) {
				Guilds::insertGuild(value);
			}
		}
	}

	OnGuildUpdateData::OnGuildUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse, DiscordCoreClient* clientNew) {
		parser.parseJson<true, true>(*static_cast<UpdatedEventData*>(this), dataToParse);
		value.discordCoreClient = clientNew;
		if (Guilds::doWeCacheGuilds()) {
			Guilds::insertGuild(value);
		}
	}

	OnGuildDeletionData::OnGuildDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse, DiscordCoreClient* clientNew) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::doWeCacheGuilds()) {
			Guilds::removeGuild(value.id);
		}
		for (auto& valueNew: value.members) {
			GuildMemberData guildMember = GuildMembers::getCachedGuildMember({ .guildMemberId = valueNew.user.id, .guildId = value.id });
			GuildMembers::removeGuildMember(GuildMemberKey{ .guildId = guildMember.guildId, .userId = guildMember.user.id });
		}
		for (auto& valueNew: value.channels) {
			Channels::removeChannel(valueNew.id);
		}
		for (auto& valueNew: value.roles) {
			Roles::removeRole(valueNew.id);
		}
	}

	OnGuildBanAddData::OnGuildBanAddData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnGuildBanRemoveData::OnGuildBanRemoveData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnGuildEmojisUpdateData::OnGuildEmojisUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			Guilds::getCache()[value.guildId].emoji.clear();
			for (auto& valueNew: value.emojis) {
				Guilds::getCache()[value.guildId].emoji.emplace_back(valueNew.id);
			}
		}
	}

	OnGuildStickersUpdateData::OnGuildStickersUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			Guilds::getCache()[value.guildId].stickers.clear();
			for (auto& valueNew: value.stickers) {
				Guilds::getCache()[value.guildId].stickers.emplace_back(valueNew.id);
			}
		}
	}

	OnGuildIntegrationsUpdateData::OnGuildIntegrationsUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnGuildMemberAddData::OnGuildMemberAddData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse, DiscordCoreClient* client) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		discordCoreClient = client;
		if (GuildMembers::doWeCacheGuildMembers()) {
			GuildMembers::insertGuildMember(value);
			if (Guilds::getCache().contains(value.guildId)) {
				++Guilds::getCache()[value.guildId].memberCount;
			}
		}
	}

	OnGuildMemberRemoveData::OnGuildMemberRemoveData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse, DiscordCoreClient* client) {
		discordCoreClient = client;
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		GuildMember guildMember = GuildMembers::getCachedGuildMember({ .guildMemberId = value.user.id, .guildId = value.guildId });
		if (GuildMembers::doWeCacheGuildMembers()) {
			GuildMembers::removeGuildMember(GuildMemberKey{ .guildId = guildMember.guildId, .userId = guildMember.user.id });
			if (Guilds::getCache().contains(value.guildId)) {
				for (uint64_t x = 0; x < Guilds::getCache()[value.guildId].members.size(); ++x) {
					if (Guilds::getCache()[value.guildId].members[x] == value.user.id) {
						--Guilds::getCache()[value.guildId].memberCount;
						Guilds::getCache()[value.guildId].members.erase(Guilds::getCache()[value.guildId].members.begin() + x);
					}
				}
			}
		}
	}

	OnGuildMemberUpdateData::OnGuildMemberUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse, DiscordCoreClient* client) {
		parser.parseJson<true, true>(*static_cast<UpdatedEventData*>(this), dataToParse);
		oldValue = GuildMembers::getCachedGuildMember({ .guildMemberId = value.user.id, .guildId = value.guildId });
		if (GuildMembers::doWeCacheGuildMembers()) {
			GuildMembers::insertGuildMember(value);
		}
	}

	OnGuildMembersChunkData::OnGuildMembersChunkData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnRoleCreationData::OnRoleCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			Guilds::getCache()[value.guildId].roles.emplace_back(std::move(value.role.id));
		}
		if (Roles::doWeCacheRoles()) {
			Roles::insertRole(std::move(value.role));
		}
	}

	OnRoleUpdateData::OnRoleUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<UpdatedEventData*>(this), dataToParse);
		oldValue = Roles::getCachedRole({ .guildId = value.guildId, .roleId = value.role.id });
		if (Roles::doWeCacheRoles()) {
			Roles::insertRole(std::move(value.role));
		}
	}

	OnRoleDeletionData::OnRoleDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Roles::doWeCacheRoles()) {
			Roles::removeRole(value.role.id);
			if (Guilds::getCache().contains(value.guildId)) {
				for (uint64_t x = 0; x < Guilds::getCache()[value.guildId].roles.size(); ++x) {
					if (Guilds::getCache()[value.guildId].roles[x] == value.role.id) {
						Guilds::getCache()[value.guildId].roles.erase(Guilds::getCache()[value.guildId].roles.begin() + x);
					}
				}
			}
		}
	}

	OnVoiceServerUpdateData::OnVoiceServerUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse,
		DiscordCoreInternal::WebSocketClient* sslShard) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (sslShard->areWeCollectingData && !sslShard->serverUpdateCollected && !sslShard->stateUpdateCollected) {
			sslShard->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
			sslShard->voiceConnectionData.endPoint = value.endpoint;
			sslShard->voiceConnectionData.token = value.token;
			sslShard->serverUpdateCollected = true;
		} else if (sslShard->areWeCollectingData && !sslShard->serverUpdateCollected) {
			sslShard->voiceConnectionData.endPoint = value.endpoint;
			sslShard->voiceConnectionData.token = value.token;
			if (sslShard->voiceConnectionDataBufferMap.contains(value.guildId.operator uint64_t())) {
				sslShard->voiceConnectionDataBufferMap[value.guildId.operator uint64_t()]->send(std::move(sslShard->voiceConnectionData));
			}
			sslShard->serverUpdateCollected = false;
			sslShard->stateUpdateCollected = false;
			sslShard->areWeCollectingData = false;
		}
	};

	OnGuildScheduledEventCreationData::OnGuildScheduledEventCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			Guilds::getCache()[value.guildId].guildScheduledEvents.emplace_back(value.id);
		}
	}

	OnGuildScheduledEventUpdateData::OnGuildScheduledEventUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnGuildScheduledEventDeletionData::OnGuildScheduledEventDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			for (uint64_t x = 0; x < Guilds::getCache()[value.guildId].guildScheduledEvents.size(); ++x) {
				if (Guilds::getCache()[value.guildId].guildScheduledEvents[x] == value.id) {
					Guilds::getCache()[value.guildId].guildScheduledEvents.erase(Guilds::getCache()[value.guildId].guildScheduledEvents.begin() + x);
				}
			}
		}
	}

	OnGuildScheduledEventUserAddData::OnGuildScheduledEventUserAddData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnGuildScheduledEventUserRemoveData::OnGuildScheduledEventUserRemoveData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnIntegrationCreationData::OnIntegrationCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnIntegrationUpdateData::OnIntegrationUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnIntegrationDeletionData::OnIntegrationDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnInteractionCreationData::OnInteractionCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse,
		DiscordCoreClient* discordCoreClient) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		UniquePtr<InputEventData> eventData{ makeUnique<InputEventData>(value) };
		switch (value.type) {
			case InteractionType::Application_Command: {
				eventData->responseType = InputEventResponseType::Unset;
				*eventData->interactionData = value;
				UniquePtr<CommandData> commandData{ makeUnique<CommandData>(*eventData) };
				discordCoreClient->getCommandController().checkForAndRunCommand(*commandData);
				UniquePtr<OnInputEventCreationData> eventCreationData{ makeUnique<OnInputEventCreationData>(parser, dataToParse) };
				eventCreationData->value = *eventData;
				break;
			}
			case InteractionType::Message_Component: {
				switch (value.data.componentType) {
					case ComponentType::Action_Row:
					case ComponentType::Button: {
						eventData->responseType = InputEventResponseType::Unset;
						*eventData->interactionData = value;
						if (ButtonCollector::buttonInteractionBuffersMap.contains(eventData->getChannelData().id + eventData->getMessageData().id)) {
							ButtonCollector::buttonInteractionBuffersMap[eventData->getChannelData().id + eventData->getMessageData().id]->send(
								std::move(value));
						}
						ButtonCollector::buttonInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
					case ComponentType::Channel_Select:
					case ComponentType::Mentionable_Select:
					case ComponentType::Role_Select:
					case ComponentType::User_Select:
					case ComponentType::String_Select: {
						eventData->responseType = InputEventResponseType::Unset;
						*eventData->interactionData = value;
						if (SelectMenuCollector::selectMenuInteractionBuffersMap.contains(
								eventData->getChannelData().id + eventData->getMessageData().id)) {
							SelectMenuCollector::selectMenuInteractionBuffersMap[eventData->getChannelData().id + eventData->getMessageData().id]
								->send(std::move(value));
						}
						SelectMenuCollector::selectMenuInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
					case ComponentType::Text_Input: {
						break;
					}
				}
				break;
			}
			case InteractionType::Modal_Submit: {
				eventData->responseType = InputEventResponseType::Unset;
				*eventData->interactionData = value;
				UniquePtr<OnInputEventCreationData> eventCreationData{ makeUnique<OnInputEventCreationData>(parser, dataToParse) };
				eventCreationData->value = *eventData;
				if (ModalCollector::modalInteractionBuffersMap.contains(eventData->getChannelData().id)) {
					ModalCollector::modalInteractionBuffersMap[eventData->getChannelData().id]->send(eventData->getInteractionData());
					ModalCollector::modalInteractionEventsMap.operator()(*eventData->interactionData);
				}
				break;
			}
			case InteractionType::Application_Command_Autocomplete: {
				eventData->responseType = InputEventResponseType::Unset;
				*eventData->interactionData = value;
				UniquePtr<OnAutoCompleteEntryData> autocompleteEntryData{ makeUnique<OnAutoCompleteEntryData>(parser, dataToParse) };
				autocompleteEntryData->value = *eventData;
				discordCoreClient->getEventManager().onAutoCompleteEntryEvent(*autocompleteEntryData);
				break;
			}
			case InteractionType::Ping: {
				break;
			}
		}
	}

	OnInviteCreationData::OnInviteCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnInviteDeletionData::OnInviteDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnMessageCreationData::OnMessageCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		for (auto& [key, valueNew]: MessageCollector::objectsBuffersMap) {
			valueNew->send(std::move(value));
		}
	}

	OnMessageUpdateData::OnMessageUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		for (auto& [key, valueNew]: MessageCollector::objectsBuffersMap) {
			valueNew->send(std::move(value));
		}
	}

	OnMessageDeletionData::OnMessageDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnMessageDeleteBulkData::OnMessageDeleteBulkData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnReactionAddData::OnReactionAddData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		for (auto& [key, valueNew]: ReactionCollector::objectsBuffersMap) {
			valueNew->send(std::move(value));
		}
	}

	OnReactionRemoveData::OnReactionRemoveData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnReactionRemoveAllData::OnReactionRemoveAllData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnReactionRemoveEmojiData::OnReactionRemoveEmojiData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnPresenceUpdateData::OnPresenceUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnStageInstanceCreationData::OnStageInstanceCreationData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			Guilds::getCache()[value.guildId].stageInstances.emplace_back(value.id);
		}
	}

	OnStageInstanceUpdateData::OnStageInstanceUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnStageInstanceDeletionData::OnStageInstanceDeletionData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (Guilds::getCache().contains(value.guildId)) {
			for (uint64_t x = 0; x < Guilds::getCache()[value.guildId].stageInstances.size(); ++x) {
				if (Guilds::getCache()[value.guildId].stageInstances[x] == value.id) {
					Guilds::getCache()[value.guildId].stageInstances.erase(Guilds::getCache()[value.guildId].stageInstances.begin() + x);
				}
			}
		}
	}

	OnTypingStartData::OnTypingStartData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnUserUpdateData::OnUserUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<UpdatedEventData*>(this), dataToParse);
		oldValue = Users::getCachedUser({ value.id });
		if (Users::doWeCacheUsers()) {
			Users::insertUser(std::move(value));
		}
	}

	OnVoiceStateUpdateData::OnVoiceStateUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse,
		DiscordCoreInternal::WebSocketClient* sslShard) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
		if (sslShard->areWeCollectingData && !sslShard->stateUpdateCollected && !sslShard->serverUpdateCollected && value.id == sslShard->userId) {
			sslShard->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
			sslShard->voiceConnectionData.sessionId = value.sessionId;
			sslShard->stateUpdateCollected = true;
		} else if (sslShard->areWeCollectingData && !sslShard->stateUpdateCollected && value.id == sslShard->userId) {
			sslShard->voiceConnectionData.sessionId = value.sessionId;
			if (sslShard->voiceConnectionDataBufferMap.contains(value.guildId.operator uint64_t())) {
				sslShard->voiceConnectionDataBufferMap[value.guildId.operator uint64_t()]->send(std::move(sslShard->voiceConnectionData));
			}
			sslShard->serverUpdateCollected = false;
			sslShard->stateUpdateCollected = false;
			sslShard->areWeCollectingData = false;
		}
		GuildMembers::insertVoiceState(std::move(value));
	}

	OnWebhookUpdateData::OnWebhookUpdateData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	OnAutoCompleteEntryData::OnAutoCompleteEntryData(Jsonifier::JsonifierCore& parser, std::string_view dataToParse) {
		parser.parseJson<true, true>(*static_cast<EventData*>(this), dataToParse);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onApplicationCommandsPermissionsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> handler) {
		return onApplicationCommandPermissionsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onApplicationCommandPermissionsUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleCreationData> handler) {
		return onAutoModerationRuleCreationEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationRuleCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleUpdateData> handler) {
		return onAutoModerationRuleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationRuleUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleDeletionData> handler) {
		return onAutoModerationRuleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationRuleDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationActionExecution(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationActionExecutionData> handler) {
		return onAutoModerationActionExecutionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationActionExecutionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoCompleteEntry(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoCompleteEntryData> handler) {
		return onAutoCompleteEntryEvent.add(std::move(handler));
	}

	void EventManager::onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token) {
		return onInputEventCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInputEventCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInputEventCreationData> handler) {
		return onInputEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return onInputEventCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelCreationData> handler) {
		return onChannelCreationEvent.add(std::move(handler));
	}

	void EventManager::onChannelCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelUpdateData> handler) {
		return onChannelUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelDeletionData> handler) {
		return onChannelDeletionEvent.add(std::move(handler));
	}

	void EventManager::onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelPinsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelPinsUpdateData> handler) {
		return onChannelPinsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onChannelPinsUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadCreationData> handler) {
		return onThreadCreationEvent.add(std::move(handler));
	}

	void EventManager::onThreadCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadUpdateData> handler) {
		return onThreadUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadDeletionData> handler) {
		return onThreadDeletionEvent.add(std::move(handler));
	}

	void EventManager::onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadListSync(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadListSyncData> handler) {
		return onThreadListSyncEvent.add(std::move(handler));
	}

	void EventManager::onThreadListSync(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadListSyncEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMemberUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMemberUpdateData> handler) {
		return onThreadMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadMemberUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMembersUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMembersUpdateData> handler) {
		return onThreadMembersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadMembersUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildCreationData> handler) {
		return onGuildCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildUpdateData> handler) {
		return onGuildUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildDeletionData> handler) {
		return onGuildDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanAddData> handler) {
		return onGuildBanAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanRemoveData> handler) {
		return onGuildBanRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildEmojisUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildEmojisUpdateData> handler) {
		return onGuildEmojisUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildEmojisUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildStickersUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildStickersUpdateData> handler) {
		return onGuildStickersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildStickersUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildIntegrationsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildIntegrationsUpdateData> handler) {
		return onGuildIntegrationsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildIntegrationsUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMembersChunk(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMembersChunkData> handler) {
		return onGuildMembersChunkEvent.add(std::move(handler));
	}

	void EventManager::onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMembersChunkEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberAddData> handler) {
		return onGuildMemberAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberRemoveData> handler) {
		return onGuildMemberRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberUpdateData> handler) {
		return onGuildMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleCreationData> handler) {
		return onRoleCreationEvent.add(std::move(handler));
	}

	void EventManager::onRoleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleUpdateData> handler) {
		return onRoleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleDeletionData> handler) {
		return onRoleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventCreationData> handler) {
		return onGuildScheduledEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUpdateData> handler) {
		return onGuildScheduledEventUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventDeletionData> handler) {
		return onGuildScheduledEventDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUserAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserAddData> handler) {
		return onGuildScheduledEventUserAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUserAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUserAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUserRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> handler) {
		return onGuildScheduledEventUserRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUserRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUserRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationCreationData> handler) {
		return onIntegrationCreationEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationUpdateData> handler) {
		return onIntegrationUpdateEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationDeletionData> handler) {
		return onIntegrationDeletionEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInteractionCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInteractionCreationData> handler) {
		return onInteractionCreationEvent.add(std::move(handler));
	}

	void EventManager::onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInteractionCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteCreationData> handler) {
		return onInviteCreationEvent.add(std::move(handler));
	}

	void EventManager::onInviteCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteDeletionData> handler) {
		return onInviteDeletionEvent.add(std::move(handler));
	}

	void EventManager::onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageCreationData> handler) {
		return onMessageCreationEvent.add(std::move(handler));
	}

	void EventManager::onMessageCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageUpdateData> handler) {
		return onMessageUpdateEvent.add(std::move(handler));
	}

	void EventManager::onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeletionData> handler) {
		return onMessageDeletionEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeleteBulk(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeleteBulkData> handler) {
		return onMessageDeleteBulkEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeleteBulkEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionAddData> handler) {
		return onReactionAddEvent.add(std::move(handler));
	}

	void EventManager::onReactionAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveData> handler) {
		return onReactionRemoveEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveAll(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveAllData> handler) {
		return onReactionRemoveAllEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveAllEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveEmoji(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveEmojiData> handler) {
		return onReactionRemoveEmojiEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEmojiEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onPresenceUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnPresenceUpdateData> handler) {
		return onPresenceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onPresenceUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceCreationData> handler) {
		return onStageInstanceCreationEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceUpdateData> handler) {
		return onStageInstanceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceDeletionData> handler) {
		return onStageInstanceDeletionEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onTypingStart(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnTypingStartData> handler) {
		return onTypingStartEvent.add(std::move(handler));
	}

	void EventManager::onTypingStart(DiscordCoreInternal::EventDelegateToken& token) {
		onTypingStartEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onUserUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnUserUpdateData> handler) {
		return onUserUpdateEvent.add(std::move(handler));
	}

	void EventManager::onUserUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onUserUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceStateUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceStateUpdateData> handler) {
		return onVoiceStateUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceStateUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceServerUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceServerUpdateData> handler) {
		return onVoiceServerUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceServerUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onWebhookUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnWebhookUpdateData> handler) {
		return onWebhookUpdateEvent.add(std::move(handler));
	}

	void EventManager::onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onWebhookUpdateEvent.erase(token);
	}


};
