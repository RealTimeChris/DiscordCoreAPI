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
/// ToEntity.hpp - Header for the final index.
/// Oct 6, 2021
/// https://discordcoreapi.com
/// \file ToEntity.hpp
#pragma once

#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace discord_core_api {

	template<> struct to_entity<guild_data> {
		guild_data toEntity(snowflake id) {
			if (guilds::doWeCacheGuilds()) {
				return guilds::getCachedGuild({ .guildId = id });
			} else {
				return guilds::getGuildAsync({ .guildId = id }).get();
			}
		}
	};

	template<> struct to_entity<user_data> {
		user_data toEntity(snowflake id) {
			if (guilds::doWeCacheGuilds()) {
				return users::getCachedUser({ .userId = id });
			} else {
				return users::getUserAsync({ .userId = id }).get();
			}
		}
	};

	template<> struct to_entity<channel_data> {
		channel_data toEntity(snowflake id) {
			if (guilds::doWeCacheGuilds()) {
				return channels::getCachedChannel({ .channelId = id });
			} else {
				return channels::getChannelAsync({ .channelId = id }).get();
			}
		}
	};

	template<> struct to_entity<role_data> {
		role_data toEntity(snowflake id) {
			if (guilds::doWeCacheGuilds()) {
				return roles::getCachedRole({ .roleId = id });
			} else {
				return roles::getRoleAsync({ .roleId = id }).get();
			}
		}
	};

	template<> struct to_entity<guild_member_data> {
		guild_member_data toEntity(snowflake id, snowflake idTwo) {
			if (guilds::doWeCacheGuilds()) {
				return guild_members::getCachedGuildMember({ .guildMemberId = id, .guildId = idTwo });
			} else {
				return guild_members::getGuildMemberAsync({ .guildMemberId = id, .guildId = idTwo }).get();
			}
		}
	};

	template<> struct to_entity<message_data> {
		message_data toEntity(snowflake idNew, snowflake channelIdNew) {
			return messages::getMessageAsync({ .channelId = channelIdNew, .id = idNew }).get();
		}
	};

	template<> struct to_entity<stage_instance_data> {
		stage_instance_data toEntity(snowflake idNew) {
			return stage_instances::getStageInstanceAsync({ .channelId = idNew }).get();
		}
	};

}
