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
/// EventManager.cpp - Header for the event manager class.
/// Jun 7, 2021
/// https://discordcoreapi.com
/// \file EventManager.cpp

#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::auto_moderation_action_execution_event_data> {
		using value_type				 = discord_core_api::auto_moderation_action_execution_event_data;
		static constexpr auto parseValue = createValue("alert_system_message_id", &value_type::alertSystemMessageId, "rule_trigger_type", &value_type::ruleTriggerType,
			"matched_keyword", &value_type::matchedKeyword, "matched_content", &value_type::matchedContent, "content", &value_type::content, "channel_id", &value_type::channelId,
			"message_id", &value_type::messageId, "action", &value_type::action, "guild_id", &value_type::guildId, "rule_id", &value_type::ruleId, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::voice_state_data> {
		using value_type2 = discord_core_api::voice_state_data;
		static constexpr auto parseValue =
			createValue("request_to_speak_timestamp", &value_type2::requestToSpeakTimestamp, "session_id", &value_type2::sessionId, "self_stream", &value_type2::selfStream,
				"self_video", &value_type2::selfVideo, "self_deaf", &value_type2::selfDeaf, "self_mute", &value_type2::selfMute, "suppress", &value_type2::suppress, "deaf",
				&value_type2::deaf, "mute", &value_type2::mute, "user_id", &value_type2::userId, "channel_id", &value_type2::channelId, "guild_id", &value_type2::guildId);
	};

	template<> struct core<discord_core_api::guild_emojis_update_event_data> {
		using value_type				 = discord_core_api::guild_emojis_update_event_data;
		static constexpr auto parseValue = createValue("emojis", &value_type::emojis, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::channel_pins_update_event_data> {
		using value_type = discord_core_api::channel_pins_update_event_data;
		static constexpr auto parseValue =
			createValue("last_pin_timestamp", &value_type::lastPinTimeStamp, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::typing_start_data> {
		using value_type				 = discord_core_api::typing_start_data;
		static constexpr auto parseValue = createValue("member", &value_type::member, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "timestamp",
			&value_type::timeStamp, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::reaction_remove_data> {
		using value_type				 = discord_core_api::reaction_remove_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "message_id", &value_type::messageId, "guild_id", &value_type::guildId, "user_id",
			&value_type::userId, "emoji", &value_type::emoji);
	};

	template<> struct core<discord_core_api::guild_stickers_update_event_data> {
		using value_type				 = discord_core_api::guild_stickers_update_event_data;
		static constexpr auto parseValue = createValue("stickers", &value_type::stickers, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::voice_server_update_data> {
		using value_type				 = discord_core_api::voice_server_update_data;
		static constexpr auto parseValue = createValue("endpoint", &value_type::endpoint, "guild_id", &value_type::guildId, "token", &value_type::token);
	};

	template<> struct core<discord_core_api::guild_members_chunk_event_data> {
		using value_type				 = discord_core_api::guild_members_chunk_event_data;
		static constexpr auto parseValue = createValue("presences", &value_type::presences, "members", &value_type::members, "not_found", &value_type::notFound, "chunk_index",
			&value_type::chunkIndex, "chunk_count", &value_type::chunkCount, "guild_id", &value_type::guildId, "nonce", &value_type::nonce);
	};

	template<> struct core<discord_core_api::guild_member_remove_data> {
		using value_type				 = discord_core_api::guild_member_remove_data;
		static constexpr auto parseValue = createValue("guild_id", &value_type::guildId, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::guild_ban_add_data> {
		using value_type				 = discord_core_api::guild_ban_add_data;
		static constexpr auto parseValue = createValue("guild_id", &value_type::guildId, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::guild_ban_remove_data> {
		using value_type				 = discord_core_api::guild_ban_remove_data;
		static constexpr auto parseValue = createValue("guild_id", &value_type::guildId, "user", &value_type::user);
	};

	template<> struct core<discord_core_api::role_creation_data> {
		using value_type				 = discord_core_api::role_creation_data;
		static constexpr auto parseValue = createValue("guild_id", &value_type::guildId, "role", &value_type::role);
	};

	template<> struct core<discord_core_api::role_update_data> {
		using value_type				 = discord_core_api::role_update_data;
		static constexpr auto parseValue = createValue("guild_id", &value_type::guildId, "role", &value_type::role);
	};

	template<> struct core<discord_core_api::role_deletion_data> {
		using value_type				 = discord_core_api::role_deletion_data;
		static constexpr auto parseValue = createValue("guild_id", &value_type::guildId, "role", &value_type::role);
	};

	template<> struct core<discord_core_api::guild_scheduled_event_user_add_data> {
		using value_type = discord_core_api::guild_scheduled_event_user_add_data;
		static constexpr auto parseValue =
			createValue("guild_scheduled_event_id", &value_type::guildScheduledEventId, "guild_id", &value_type::guildId, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::guild_scheduled_event_user_remove_data> {
		using value_type = discord_core_api::guild_scheduled_event_user_remove_data;
		static constexpr auto parseValue =
			createValue("guild_scheduled_event_id", &value_type::guildScheduledEventId, "guild_id", &value_type::guildId, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::integration_creation_data> {
		using value_type				 = discord_core_api::integration_creation_data;
		static constexpr auto parseValue = createValue("integration_data", &value_type::integrationData, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::integration_update_data> {
		using value_type				 = discord_core_api::integration_update_data;
		static constexpr auto parseValue = createValue("integration_data", &value_type::integrationData, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::integration_deletion_data> {
		using value_type				 = discord_core_api::integration_deletion_data;
		static constexpr auto parseValue = createValue("application_id", &value_type::applicationId, "guild_id", &value_type::guildId, "id", &value_type::id);
	};

	template<> struct core<discord_core_api::invite_deletion_data> {
		using value_type				 = discord_core_api::invite_deletion_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "code", &value_type::code);
	};

	template<> struct core<discord_core_api::message_deletion_data> {
		using value_type				 = discord_core_api::message_deletion_data;
		static constexpr auto parseValue = createValue("message_id", &value_type::messageId, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::message_deletion_bulk_data> {
		using value_type				 = discord_core_api::message_deletion_bulk_data;
		static constexpr auto parseValue = createValue("ids", &value_type::ids, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::reaction_remove_all_data> {
		using value_type				 = discord_core_api::reaction_remove_all_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "message_id", &value_type::messageId, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::reaction_remove_emoji_data> {
		using value_type = discord_core_api::reaction_remove_emoji_data;
		static constexpr auto parseValue =
			createValue("message_id", &value_type::messageId, "channel_id", &value_type::channelId, "guild_id", &value_type::guildId, "emoji", &value_type::emoji);
	};

	template<> struct core<discord_core_api::web_hook_update_data> {
		using value_type				 = discord_core_api::web_hook_update_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "guild_id", &value_type::guildId);
	};

}

namespace discord_core_api {

	template<> unordered_map<jsonifier::string, unbounded_message_block<message_data>*> object_collector<message_data>::objectsBuffersMap;

	template<> unordered_map<jsonifier::string, unbounded_message_block<reaction_data>*> object_collector<reaction_data>::objectsBuffersMap;

	on_input_event_creation_data::on_input_event_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_application_command_permissions_update_data::on_application_command_permissions_update_data(jsonifier::jsonifier_core<false>& parserNew,
		jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_auto_moderation_rule_creation_data::on_auto_moderation_rule_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_auto_moderation_rule_update_data::on_auto_moderation_rule_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_auto_moderation_rule_deletion_data::on_auto_moderation_rule_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_auto_moderation_action_execution_data::on_auto_moderation_action_execution_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_channel_creation_data::on_channel_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (channels::doWeCacheChannels()) {
			channels::insertChannel(static_cast<channel_cache_data>(value));
		}
		if (guilds::doWeCacheGuilds()) {
			if (guilds::getCache().contains(value.guildId)) {
				guilds::getCache()[value.guildId].channels.emplace_back(value.id);
			}
		}
	}

	on_channel_update_data::on_channel_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<updated_event_data*>(this), dataToParse);
		if (channels::doWeCacheChannels()) {
			oldValue = channels::getCachedChannel({ .channelId = value.id });
			channels::insertChannel(static_cast<channel_cache_data>(value));
		}
	}

	on_channel_deletion_data::on_channel_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (channels::doWeCacheChannels()) {
			channels::removeChannel(static_cast<channel_cache_data>(value));
		}
		if (guilds::doWeCacheGuilds()) {
			if (guilds::getCache().contains(value.guildId)) {
				for (uint64_t x = 0; x < guilds::getCache().operator[](value.guildId).channels.size(); ++x) {
					if (guilds::getCache().operator[](value.guildId).channels.at(x) == static_cast<uint64_t>(value.id)) {
						guilds::getCache().operator[](value.guildId).channels.erase(guilds::getCache().operator[](value.guildId).channels.begin() + static_cast<int64_t>(x));
					}
				}
			}
		}
	}

	on_channel_pins_update_data::on_channel_pins_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_thread_creation_data::on_thread_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_thread_update_data::on_thread_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_thread_deletion_data::on_thread_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_thread_list_sync_data::on_thread_list_sync_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_thread_member_update_data::on_thread_member_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_thread_members_update_data::on_thread_members_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_creation_data::on_guild_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (guild_members::doWeCacheGuildMembers()) {
			for (auto& valueNew: value.members) {
				try {
					valueNew.guildId = value.id;
					guild_members::insertGuildMember(static_cast<guild_member_cache_data>(valueNew));
				} catch (...) {
					message_printer::printError<print_message_type::general>("on_guild_creation_data::on_guild_creation_data()");
				}
			}
		}
		if (users::doWeCacheUsers()) {
			for (auto& valueNew: value.members) {
				try {
					if (users::doWeCacheUsers()) {
						users::insertUser(static_cast<user_cache_data>(valueNew.user));
					}
				} catch (...) {
					message_printer::printError<print_message_type::general>("on_guild_creation_data::on_guild_creation_data()");
				}
			}
		}
		if (guild_members::doWeCacheVoiceStates()) {
			for (auto& valueNew: value.voiceStates) {
				try {
					valueNew.guildId = value.id;
					guild_members::insertVoiceState(static_cast<voice_state_data_light>(valueNew));
					guild_member_data newMember{};
					newMember.guildId = value.id;
					newMember.user.id = valueNew.userId;
					two_id_key newKey{ newMember };
				} catch (...) {
					message_printer::printError<print_message_type::general>("on_guild_creation_data::on_guild_creation_data()");
				}
			}
		}
		if (channels::doWeCacheChannels()) {
			for (auto& valueNew: value.channels) {
				try {
					valueNew.guildId = value.id;
					channels::insertChannel(static_cast<channel_cache_data>(valueNew));
				} catch (...) {
					message_printer::printError<print_message_type::general>("on_guild_creation_data::on_guild_creation_data()");
				}
			}
		}
		if (roles::doWeCacheRoles()) {
			for (auto& valueNew: value.roles) {
				try {
					roles::insertRole(static_cast<role_cache_data>(valueNew));
				} catch (...) {
					message_printer::printError<print_message_type::general>("on_guild_creation_data::on_guild_creation_data()");
				}
			}
		}
		if (guilds::doWeCacheGuilds()) {
			guilds::insertGuild(static_cast<guild_cache_data>(value));
		}
	}

	on_guild_update_data::on_guild_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<updated_event_data*>(this), dataToParse);
		if (guilds::doWeCacheGuilds()) {
			oldValue = guilds::getCachedGuild({ value.id });
			guilds::insertGuild(static_cast<guild_cache_data>(value));
		}
	}

	on_guild_deletion_data::on_guild_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		for (auto& valueNew: value.members) {
			guild_members::removeGuildMember(valueNew);
		}
		for (auto& valueNew: value.channels) {
			channels::removeChannel(valueNew);
		}
		for (auto& valueNew: value.roles) {
			roles::removeRole(valueNew);
		}
		if (guilds::doWeCacheGuilds()) {
			guilds::removeGuild(value);
		}
	}

	on_guild_ban_add_data::on_guild_ban_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (guilds::getCache().contains(value.guildId)) {
			for (uint64_t x = 0; x < guilds::getCache().operator[](value.guildId).members.size(); ++x) {
				if (guilds::getCache().operator[](value.guildId).members.at(x) == static_cast<uint64_t>(value.user.id)) {
					guilds::getCache().operator[](value.guildId).members.erase(guilds::getCache().operator[](value.guildId).members.begin() + static_cast<int64_t>(x));
					--guilds::getCache()[value.guildId].memberCount;
				}
			}
		}
	}

	on_guild_ban_remove_data::on_guild_ban_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_emojis_update_data::on_guild_emojis_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (guilds::getCache().contains(value.guildId)) {
			guilds::getCache()[value.guildId].emoji.clear();
			for (auto& valueNew: value.emojis) {
				guilds::getCache()[value.guildId].emoji.emplace_back(valueNew.id);
			}
		}
	}

	on_guild_stickers_update_data::on_guild_stickers_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_integrations_update_data::on_guild_integrations_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_member_add_data::on_guild_member_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (guild_members::doWeCacheGuildMembers()) {
			guild_members::insertGuildMember(static_cast<guild_member_cache_data>(value));
		}
		if (guilds::doWeCacheGuilds()) {
			if (guilds::getCache().contains(value.guildId)) {
				++guilds::getCache()[value.guildId].memberCount;
				guilds::getCache()[value.guildId].members.emplace_back(value.user.id);
			}
		}
	}

	on_guild_member_remove_data::on_guild_member_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (guild_members::doWeCacheGuildMembers()) {
			guild_member_data guildMember = guild_members::getCachedGuildMember({ .guildMemberId = value.user.id, .guildId = value.guildId });
			guild_members::removeGuildMember(guildMember);
		}
		if (guilds::doWeCacheGuilds()) {
			if (guilds::getCache().contains(value.guildId)) {
				for (uint64_t x = 0; x < guilds::getCache().operator[](value.guildId).members.size(); ++x) {
					if (guilds::getCache().operator[](value.guildId).members.at(x) == static_cast<uint64_t>(value.user.id)) {
						guilds::getCache().operator[](value.guildId).members.erase(guilds::getCache().operator[](value.guildId).members.begin() + static_cast<int64_t>(x));
						--guilds::getCache()[value.guildId].memberCount;
					}
				}
			}
		}
	}

	on_guild_member_update_data::on_guild_member_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<updated_event_data*>(this), dataToParse);
		if (guild_members::doWeCacheGuildMembers()) {
			oldValue = guild_members::getCachedGuildMember({ .guildMemberId = value.user.id, .guildId = value.guildId });
			guild_members::insertGuildMember(static_cast<guild_member_cache_data>(value));
		}
	}

	on_guild_members_chunk_data::on_guild_members_chunk_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_role_creation_data::on_role_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (roles::doWeCacheRoles()) {
			roles::insertRole(static_cast<role_cache_data>(value.role));
		}
		if (guilds::doWeCacheGuilds()) {
			if (guilds::getCache().contains(value.guildId)) {
				guilds::getCache()[value.guildId].roles.emplace_back(value.role.id);
			}
		}
	}

	on_role_update_data::on_role_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<updated_event_data*>(this), dataToParse);
		if (roles::doWeCacheRoles()) {
			oldValue = roles::getCachedRole({ .guildId = value.guildId, .roleId = value.role.id });
			roles::insertRole(static_cast<role_cache_data>(value.role));
		}
	}

	on_role_deletion_data::on_role_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (roles::doWeCacheRoles()) {
			roles::removeRole(static_cast<role_cache_data>(value.role));
		}
		if (guilds::doWeCacheGuilds()) {
			if (guilds::getCache().contains(value.guildId)) {
				for (uint64_t x = 0; x < guilds::getCache().operator[](value.guildId).roles.size(); ++x) {
					if (guilds::getCache().operator[](value.guildId).roles.at(x) == static_cast<uint64_t>(value.role.id)) {
						guilds::getCache().operator[](value.guildId).roles.erase(guilds::getCache().operator[](value.guildId).roles.begin() + static_cast<int64_t>(x));
					}
				}
			}
		}
	}

	on_voice_server_update_data::on_voice_server_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse,
		discord_core_internal::websocket_client* sslShard) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (sslShard->areWeCollectingData.load(std::memory_order_acquire) && !sslShard->serverUpdateCollected && !sslShard->stateUpdateCollected) {
			sslShard->voiceConnectionData		   = discord_core_internal::voice_connection_data{};
			sslShard->voiceConnectionData.endPoint = value.endpoint;
			sslShard->voiceConnectionData.token	   = value.token;
			sslShard->serverUpdateCollected		   = true;
		} else if (sslShard->areWeCollectingData.load(std::memory_order_acquire) && !sslShard->serverUpdateCollected) {
			sslShard->voiceConnectionData.endPoint = value.endpoint;
			sslShard->voiceConnectionData.token	   = value.token;
			if (sslShard->voiceConnectionDataBufferMap.contains(value.guildId.operator const uint64_t&())) {
				sslShard->voiceConnectionDataBufferMap[value.guildId.operator const uint64_t&()]->send(sslShard->voiceConnectionData);
			}
			sslShard->areWeCollectingData.store(false, std::memory_order_release);
			sslShard->serverUpdateCollected = false;
			sslShard->stateUpdateCollected	= false;
		}
	};

	on_guild_scheduled_event_creation_data::on_guild_scheduled_event_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_scheduled_event_update_data::on_guild_scheduled_event_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_scheduled_event_deletion_data::on_guild_scheduled_event_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_scheduled_event_user_add_data::on_guild_scheduled_event_user_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_guild_scheduled_event_user_remove_data::on_guild_scheduled_event_user_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_integration_creation_data::on_integration_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_integration_update_data::on_integration_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_integration_deletion_data::on_integration_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_interaction_creation_data::on_interaction_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		unique_ptr<input_event_data> eventData{ makeUnique<input_event_data>(value) };
		switch (value.type) {
			case interaction_type::Application_Command: {
				eventData->responseType		= input_event_response_type::Unset;
				eventData->interactionData = makeUnique<interaction_data>(value);
				discord_core_client::getInstance()->getCommandController().checkForAndRunCommand(command_data{ *eventData });
				unique_ptr<on_input_event_creation_data> eventCreationData{ makeUnique<on_input_event_creation_data>(parser, dataToParse) };
				eventCreationData->value = *eventData;
				break;
			}
			case interaction_type::Message_Component: {
				switch (value.data.componentType) {
					case component_type::Action_Row:
						[[fallthrough]];
					case component_type::Button: {
						eventData->responseType	   = input_event_response_type::Unset;
						eventData->interactionData = makeUnique<interaction_data>(value);
						if (button_collector::buttonInteractionBuffersMap.contains(
								eventData->getChannelData().id.operator jsonifier::string() + eventData->getMessageData().id.operator jsonifier::string())) {
							button_collector::buttonInteractionBuffersMap[eventData->getChannelData().id.operator jsonifier::string() +
								eventData->getMessageData().id.operator jsonifier::string()]
								->send(value);
						}
						button_collector::buttonInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
					case component_type::Channel_Select:
						[[fallthrough]];
					case component_type::Mentionable_Select:
						[[fallthrough]];
					case component_type::Role_Select:
						[[fallthrough]];
					case component_type::User_Select:
						[[fallthrough]];
					case component_type::String_Select: {
						eventData->responseType		= input_event_response_type::Unset;
						eventData->interactionData = makeUnique<interaction_data>(value);
						if (select_menu_collector::selectMenuInteractionBuffersMap.contains(
								eventData->getChannelData().id.operator jsonifier::string() + eventData->getMessageData().id.operator jsonifier::string())) {
							select_menu_collector::selectMenuInteractionBuffersMap[eventData->getChannelData().id.operator jsonifier::string() +
								eventData->getMessageData().id.operator jsonifier::string()]
								->send(value);
						}
						select_menu_collector::selectMenuInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
					case component_type::Text_Input: {
						break;
					}
				}
				break;
			}
			case interaction_type::Modal_Submit: {
				eventData->responseType		= input_event_response_type::Unset;
				eventData->interactionData = makeUnique<interaction_data>(value);
				unique_ptr<on_input_event_creation_data> eventCreationData{ makeUnique<on_input_event_creation_data>(parser, dataToParse) };
				eventCreationData->value = *eventData;
				if (modal_collector::modalInteractionBuffersMap.contains(eventData->getChannelData().id.operator jsonifier::string())) {
					modal_collector::modalInteractionBuffersMap[eventData->getChannelData().id.operator jsonifier::string()]->send(eventData->getInteractionData());
					modal_collector::modalInteractionEventsMap.operator()(*eventData->interactionData);
				}
				break;
			}
			case interaction_type::Application_Command_Autocomplete: {
				eventData->responseType	   = input_event_response_type::Unset;
				eventData->interactionData = makeUnique<interaction_data>(value);
				unique_ptr<on_auto_complete_entry_data> autocompleteEntryData{ makeUnique<on_auto_complete_entry_data>(parser, dataToParse) };
				autocompleteEntryData->value = *eventData;
				discord_core_client::getInstance()->getEventManager().onAutoCompleteEntryEvent(*autocompleteEntryData);
				break;
			}
			case interaction_type::Ping: {
				break;
			}
		}
	}

	on_invite_creation_data::on_invite_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_invite_deletion_data::on_invite_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_message_creation_data::on_message_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		for (auto& [key, valueNew]: message_collector::objectsBuffersMap) {
			valueNew->send(value);
		}
	}

	on_message_update_data::on_message_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		for (auto& [key, valueNew]: message_collector::objectsBuffersMap) {
			valueNew->send(value);
		}
	}

	on_message_deletion_data::on_message_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_message_delete_bulk_data::on_message_delete_bulk_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_reaction_add_data::on_reaction_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		for (auto& [key, valueNew]: reaction_collector::objectsBuffersMap) {
			valueNew->send(value);
		}
	}

	on_reaction_remove_data::on_reaction_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_reaction_remove_all_data::on_reaction_remove_all_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_reaction_remove_emoji_data::on_reaction_remove_emoji_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_presence_update_data::on_presence_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_stage_instance_creation_data::on_stage_instance_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_stage_instance_update_data::on_stage_instance_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_stage_instance_deletion_data::on_stage_instance_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_typing_start_data::on_typing_start_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_user_update_data::on_user_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<updated_event_data*>(this), dataToParse);
		oldValue = users::getCachedUser({ value.id });
		if (users::doWeCacheUsers()) {
			users::insertUser(static_cast<user_cache_data>(value));
		}
	}

	on_voice_state_update_data::on_voice_state_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse,
		discord_core_internal::websocket_client* sslShard) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
		if (sslShard->areWeCollectingData.load(std::memory_order_acquire) && !sslShard->stateUpdateCollected && !sslShard->serverUpdateCollected &&
			value.userId == sslShard->userId) {
			sslShard->voiceConnectionData			= discord_core_internal::voice_connection_data{};
			sslShard->voiceConnectionData.sessionId = value.sessionId;
			sslShard->stateUpdateCollected			= true;
		} else if (sslShard->areWeCollectingData.load(std::memory_order_acquire) && !sslShard->stateUpdateCollected && value.userId == sslShard->userId) {
			sslShard->voiceConnectionData.sessionId = value.sessionId;
			if (sslShard->voiceConnectionDataBufferMap.contains(value.guildId.operator const uint64_t&())) {
				sslShard->voiceConnectionDataBufferMap[value.guildId.operator const uint64_t&()]->send(std::move(sslShard->voiceConnectionData));
			}
			sslShard->areWeCollectingData.store(false, std::memory_order_release);
			sslShard->serverUpdateCollected = false;
			sslShard->stateUpdateCollected	= false;
		}
		voice_state_data_light voiceDataNew{};
		voiceDataNew.channelId = value.channelId;
		voiceDataNew.guildId   = value.guildId;
		voiceDataNew.userId	   = value.userId;
		guild_members::insertVoiceState(std::move(voiceDataNew));
	}

	on_webhook_update_data::on_webhook_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	on_auto_complete_entry_data::on_auto_complete_entry_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse) {
		parserNew.parseJson(*static_cast<event_data*>(this), dataToParse);
		if (auto result = parserNew.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				message_printer::printError<print_message_type::general>(valueNew.reportError());
			}
		}
	}

	discord_core_internal::event_delegate_token event_manager::onApplicationCommandsPermissionsUpdate(
		discord_core_internal::event_delegate<co_routine<void>, const on_application_command_permissions_update_data&> handler) {
		return onApplicationCommandPermissionsUpdateEvent.add(std::move(handler));
	}

	void event_manager::onApplicationCommandsPermissionsUpdate(discord_core_internal::event_delegate_token& token) {
		return onApplicationCommandPermissionsUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onAutoModerationRuleCreation(
		discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_rule_creation_data&> handler) {
		return onAutoModerationRuleCreationEvent.add(std::move(handler));
	}

	void event_manager::onAutoModerationRuleCreation(discord_core_internal::event_delegate_token& token) {
		return onAutoModerationRuleCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onAutoModerationRuleUpdate(
		discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_rule_update_data&> handler) {
		return onAutoModerationRuleUpdateEvent.add(std::move(handler));
	}

	void event_manager::onAutoModerationRuleUpdate(discord_core_internal::event_delegate_token& token) {
		return onAutoModerationRuleUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onAutoModerationRuleDeletion(
		discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_rule_deletion_data&> handler) {
		return onAutoModerationRuleDeletionEvent.add(std::move(handler));
	}

	void event_manager::onAutoModerationRuleDeletion(discord_core_internal::event_delegate_token& token) {
		return onAutoModerationRuleDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onAutoModerationActionExecution(
		discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_action_execution_data&> handler) {
		return onAutoModerationActionExecutionEvent.add(std::move(handler));
	}

	void event_manager::onAutoModerationActionExecution(discord_core_internal::event_delegate_token& token) {
		return onAutoModerationActionExecutionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onAutoCompleteEntry(discord_core_internal::event_delegate<co_routine<void>, const on_auto_complete_entry_data&> handler) {
		return onAutoCompleteEntryEvent.add(std::move(handler));
	}

	void event_manager::onAutoCompleteEntry(discord_core_internal::event_delegate_token& token) {
		return onInputEventCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onInputEventCreation(discord_core_internal::event_delegate<co_routine<void>, const on_input_event_creation_data&> handler) {
		return onInputEventCreationEvent.add(std::move(handler));
	}

	void event_manager::onInputEventCreation(discord_core_internal::event_delegate_token& token) {
		return onInputEventCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onChannelCreation(discord_core_internal::event_delegate<co_routine<void>, const on_channel_creation_data&> handler) {
		return onChannelCreationEvent.add(std::move(handler));
	}

	void event_manager::onChannelCreation(discord_core_internal::event_delegate_token& token) {
		onChannelCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onChannelUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_channel_update_data&> handler) {
		return onChannelUpdateEvent.add(std::move(handler));
	}

	void event_manager::onChannelUpdate(discord_core_internal::event_delegate_token& token) {
		onChannelUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onChannelDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_channel_deletion_data&> handler) {
		return onChannelDeletionEvent.add(std::move(handler));
	}

	void event_manager::onChannelDeletion(discord_core_internal::event_delegate_token& token) {
		onChannelDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onChannelPinsUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_channel_pins_update_data&> handler) {
		return onChannelPinsUpdateEvent.add(std::move(handler));
	}

	void event_manager::onChannelPinsUpdate(discord_core_internal::event_delegate_token& token) {
		return onChannelPinsUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onThreadCreation(discord_core_internal::event_delegate<co_routine<void>, const on_thread_creation_data&> handler) {
		return onThreadCreationEvent.add(std::move(handler));
	}

	void event_manager::onThreadCreation(discord_core_internal::event_delegate_token& token) {
		return onThreadCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onThreadUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_thread_update_data&> handler) {
		return onThreadUpdateEvent.add(std::move(handler));
	}

	void event_manager::onThreadUpdate(discord_core_internal::event_delegate_token& token) {
		return onThreadUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onThreadDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_thread_deletion_data&> handler) {
		return onThreadDeletionEvent.add(std::move(handler));
	}

	void event_manager::onThreadDeletion(discord_core_internal::event_delegate_token& token) {
		return onThreadDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onThreadListSync(discord_core_internal::event_delegate<co_routine<void>, const on_thread_list_sync_data&> handler) {
		return onThreadListSyncEvent.add(std::move(handler));
	}

	void event_manager::onThreadListSync(discord_core_internal::event_delegate_token& token) {
		return onThreadListSyncEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onThreadMemberUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_thread_member_update_data&> handler) {
		return onThreadMemberUpdateEvent.add(std::move(handler));
	}

	void event_manager::onThreadMemberUpdate(discord_core_internal::event_delegate_token& token) {
		return onThreadMemberUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onThreadMembersUpdate(
		discord_core_internal::event_delegate<co_routine<void>, const on_thread_members_update_data&> handler) {
		return onThreadMembersUpdateEvent.add(std::move(handler));
	}

	void event_manager::onThreadMembersUpdate(discord_core_internal::event_delegate_token& token) {
		return onThreadMembersUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildCreation(discord_core_internal::event_delegate<co_routine<void>, const on_guild_creation_data&> handler) {
		return onGuildCreationEvent.add(std::move(handler));
	}

	void event_manager::onGuildCreation(discord_core_internal::event_delegate_token& token) {
		onGuildCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_update_data&> handler) {
		return onGuildUpdateEvent.add(std::move(handler));
	}

	void event_manager::onGuildUpdate(discord_core_internal::event_delegate_token& token) {
		onGuildUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_guild_deletion_data&> handler) {
		return onGuildDeletionEvent.add(std::move(handler));
	}

	void event_manager::onGuildDeletion(discord_core_internal::event_delegate_token& token) {
		onGuildDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildBanAdd(discord_core_internal::event_delegate<co_routine<void>, const on_guild_ban_add_data&> handler) {
		return onGuildBanAddEvent.add(std::move(handler));
	}

	void event_manager::onGuildBanAdd(discord_core_internal::event_delegate_token& token) {
		onGuildBanAddEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildBanRemove(discord_core_internal::event_delegate<co_routine<void>, const on_guild_ban_remove_data&> handler) {
		return onGuildBanRemoveEvent.add(std::move(handler));
	}

	void event_manager::onGuildBanRemove(discord_core_internal::event_delegate_token& token) {
		onGuildBanRemoveEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildEmojisUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_emojis_update_data&> handler) {
		return onGuildEmojisUpdateEvent.add(std::move(handler));
	}

	void event_manager::onGuildEmojisUpdate(discord_core_internal::event_delegate_token& token) {
		onGuildEmojisUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildStickersUpdate(
		discord_core_internal::event_delegate<co_routine<void>, const on_guild_stickers_update_data&> handler) {
		return onGuildStickersUpdateEvent.add(std::move(handler));
	}

	void event_manager::onGuildStickersUpdate(discord_core_internal::event_delegate_token& token) {
		onGuildStickersUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildIntegrationsUpdate(
		discord_core_internal::event_delegate<co_routine<void>, const on_guild_integrations_update_data&> handler) {
		return onGuildIntegrationsUpdateEvent.add(std::move(handler));
	}

	void event_manager::onGuildIntegrationsUpdate(discord_core_internal::event_delegate_token& token) {
		onGuildIntegrationsUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildMembersChunk(discord_core_internal::event_delegate<co_routine<void>, const on_guild_members_chunk_data&> handler) {
		return onGuildMembersChunkEvent.add(std::move(handler));
	}

	void event_manager::onGuildMembersChunk(discord_core_internal::event_delegate_token& token) {
		onGuildMembersChunkEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildMemberAdd(discord_core_internal::event_delegate<co_routine<void>, const on_guild_member_add_data&> handler) {
		return onGuildMemberAddEvent.add(std::move(handler));
	}

	void event_manager::onGuildMemberAdd(discord_core_internal::event_delegate_token& token) {
		onGuildMemberAddEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildMemberRemove(discord_core_internal::event_delegate<co_routine<void>, const on_guild_member_remove_data&> handler) {
		return onGuildMemberRemoveEvent.add(std::move(handler));
	}

	void event_manager::onGuildMemberRemove(discord_core_internal::event_delegate_token& token) {
		onGuildMemberRemoveEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildMemberUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_member_update_data&> handler) {
		return onGuildMemberUpdateEvent.add(std::move(handler));
	}

	void event_manager::onGuildMemberUpdate(discord_core_internal::event_delegate_token& token) {
		onGuildMemberUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onRoleCreation(discord_core_internal::event_delegate<co_routine<void>, const on_role_creation_data&> handler) {
		return onRoleCreationEvent.add(std::move(handler));
	}

	void event_manager::onRoleCreation(discord_core_internal::event_delegate_token& token) {
		onRoleCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onRoleUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_role_update_data&> handler) {
		return onRoleUpdateEvent.add(std::move(handler));
	}

	void event_manager::onRoleUpdate(discord_core_internal::event_delegate_token& token) {
		onRoleUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onRoleDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_role_deletion_data&> handler) {
		return onRoleDeletionEvent.add(std::move(handler));
	}

	void event_manager::onRoleDeletion(discord_core_internal::event_delegate_token& token) {
		onRoleDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildScheduledEventCreation(
		discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_creation_data&> handler) {
		return onGuildScheduledEventCreationEvent.add(std::move(handler));
	}

	void event_manager::onGuildScheduledEventCreation(discord_core_internal::event_delegate_token& token) {
		onGuildScheduledEventCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildScheduledEventUpdate(
		discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_update_data&> handler) {
		return onGuildScheduledEventUpdateEvent.add(std::move(handler));
	}

	void event_manager::onGuildScheduledEventUpdate(discord_core_internal::event_delegate_token& token) {
		onGuildScheduledEventUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildScheduledEventDeletion(
		discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_deletion_data&> handler) {
		return onGuildScheduledEventDeletionEvent.add(std::move(handler));
	}

	void event_manager::onGuildScheduledEventDeletion(discord_core_internal::event_delegate_token& token) {
		onGuildScheduledEventDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildScheduledEventUserAdd(
		discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_user_add_data&> handler) {
		return onGuildScheduledEventUserAddEvent.add(std::move(handler));
	}

	void event_manager::onGuildScheduledEventUserAdd(discord_core_internal::event_delegate_token& token) {
		onGuildScheduledEventUserAddEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onGuildScheduledEventUserRemove(
		discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_user_remove_data&> handler) {
		return onGuildScheduledEventUserRemoveEvent.add(std::move(handler));
	}

	void event_manager::onGuildScheduledEventUserRemove(discord_core_internal::event_delegate_token& token) {
		onGuildScheduledEventUserRemoveEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onIntegrationCreation(
		discord_core_internal::event_delegate<co_routine<void>, const on_integration_creation_data&> handler) {
		return onIntegrationCreationEvent.add(std::move(handler));
	}

	void event_manager::onIntegrationCreation(discord_core_internal::event_delegate_token& token) {
		onIntegrationCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onIntegrationUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_integration_update_data&> handler) {
		return onIntegrationUpdateEvent.add(std::move(handler));
	}

	void event_manager::onIntegrationUpdate(discord_core_internal::event_delegate_token& token) {
		onIntegrationUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onIntegrationDeletion(
		discord_core_internal::event_delegate<co_routine<void>, const on_integration_deletion_data&> handler) {
		return onIntegrationDeletionEvent.add(std::move(handler));
	}

	void event_manager::onIntegrationDeletion(discord_core_internal::event_delegate_token& token) {
		onIntegrationDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onInteractionCreation(
		discord_core_internal::event_delegate<co_routine<void>, const on_interaction_creation_data&> handler) {
		return onInteractionCreationEvent.add(std::move(handler));
	}

	void event_manager::onInteractionCreation(discord_core_internal::event_delegate_token& token) {
		onInteractionCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onInviteCreation(discord_core_internal::event_delegate<co_routine<void>, const on_invite_creation_data&> handler) {
		return onInviteCreationEvent.add(std::move(handler));
	}

	void event_manager::onInviteCreation(discord_core_internal::event_delegate_token& token) {
		onInviteCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onInviteDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_invite_deletion_data&> handler) {
		return onInviteDeletionEvent.add(std::move(handler));
	}

	void event_manager::onInviteDeletion(discord_core_internal::event_delegate_token& token) {
		onInviteDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onMessageCreation(discord_core_internal::event_delegate<co_routine<void>, const on_message_creation_data&> handler) {
		return onMessageCreationEvent.add(std::move(handler));
	}

	void event_manager::onMessageCreation(discord_core_internal::event_delegate_token& token) {
		onMessageCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onMessageUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_message_update_data&> handler) {
		return onMessageUpdateEvent.add(std::move(handler));
	}

	void event_manager::onMessageUpdate(discord_core_internal::event_delegate_token& token) {
		onMessageUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onMessageDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_message_deletion_data&> handler) {
		return onMessageDeletionEvent.add(std::move(handler));
	}

	void event_manager::onMessageDeletion(discord_core_internal::event_delegate_token& token) {
		onMessageDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onMessageDeleteBulk(discord_core_internal::event_delegate<co_routine<void>, const on_message_delete_bulk_data&> handler) {
		return onMessageDeleteBulkEvent.add(std::move(handler));
	}

	void event_manager::onMessageDeleteBulk(discord_core_internal::event_delegate_token& token) {
		onMessageDeleteBulkEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onReactionAdd(discord_core_internal::event_delegate<co_routine<void>, const on_reaction_add_data&> handler) {
		return onReactionAddEvent.add(std::move(handler));
	}

	void event_manager::onReactionAdd(discord_core_internal::event_delegate_token& token) {
		onReactionAddEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onReactionRemove(discord_core_internal::event_delegate<co_routine<void>, const on_reaction_remove_data&> handler) {
		return onReactionRemoveEvent.add(std::move(handler));
	}

	void event_manager::onReactionRemove(discord_core_internal::event_delegate_token& token) {
		onReactionRemoveEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onReactionRemoveAll(discord_core_internal::event_delegate<co_routine<void>, const on_reaction_remove_all_data&> handler) {
		return onReactionRemoveAllEvent.add(std::move(handler));
	}

	void event_manager::onReactionRemoveAll(discord_core_internal::event_delegate_token& token) {
		onReactionRemoveAllEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onReactionRemoveEmoji(
		discord_core_internal::event_delegate<co_routine<void>, const on_reaction_remove_emoji_data&> handler) {
		return onReactionRemoveEmojiEvent.add(std::move(handler));
	}

	void event_manager::onReactionRemoveEmoji(discord_core_internal::event_delegate_token& token) {
		onReactionRemoveEmojiEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onPresenceUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_presence_update_data&> handler) {
		return onPresenceUpdateEvent.add(std::move(handler));
	}

	void event_manager::onPresenceUpdate(discord_core_internal::event_delegate_token& token) {
		onPresenceUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onStageInstanceCreation(
		discord_core_internal::event_delegate<co_routine<void>, const on_stage_instance_creation_data&> handler) {
		return onStageInstanceCreationEvent.add(std::move(handler));
	}

	void event_manager::onStageInstanceCreation(discord_core_internal::event_delegate_token& token) {
		onStageInstanceCreationEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onStageInstanceUpdate(
		discord_core_internal::event_delegate<co_routine<void>, const on_stage_instance_update_data&> handler) {
		return onStageInstanceUpdateEvent.add(std::move(handler));
	}

	void event_manager::onStageInstanceUpdate(discord_core_internal::event_delegate_token& token) {
		onStageInstanceUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onStageInstanceDeletion(
		discord_core_internal::event_delegate<co_routine<void>, const on_stage_instance_deletion_data&> handler) {
		return onStageInstanceDeletionEvent.add(std::move(handler));
	}

	void event_manager::onStageInstanceDeletion(discord_core_internal::event_delegate_token& token) {
		onStageInstanceDeletionEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onTypingStart(discord_core_internal::event_delegate<co_routine<void>, const on_typing_start_data&> handler) {
		return onTypingStartEvent.add(std::move(handler));
	}

	void event_manager::onTypingStart(discord_core_internal::event_delegate_token& token) {
		onTypingStartEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onUserUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_user_update_data&> handler) {
		return onUserUpdateEvent.add(std::move(handler));
	}

	void event_manager::onUserUpdate(discord_core_internal::event_delegate_token& token) {
		onUserUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onVoiceStateUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_voice_state_update_data&> handler) {
		return onVoiceStateUpdateEvent.add(std::move(handler));
	}

	void event_manager::onVoiceStateUpdate(discord_core_internal::event_delegate_token& token) {
		onVoiceStateUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onVoiceServerUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_voice_server_update_data&> handler) {
		return onVoiceServerUpdateEvent.add(std::move(handler));
	}

	void event_manager::onVoiceServerUpdate(discord_core_internal::event_delegate_token& token) {
		onVoiceServerUpdateEvent.erase(token);
	}

	discord_core_internal::event_delegate_token event_manager::onWebhookUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_webhook_update_data&> handler) {
		return onWebhookUpdateEvent.add(std::move(handler));
	}

	void event_manager::onWebhookUpdate(discord_core_internal::event_delegate_token& token) {
		onWebhookUpdateEvent.erase(token);
	}


};
