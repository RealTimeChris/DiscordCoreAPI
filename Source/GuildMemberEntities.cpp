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
/// GuildMemberEntities.cpp - Source file for the guild_member_data related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::add_guild_member_data> {
		using value_type				 = discord_core_api::add_guild_member_data;
		static constexpr auto parseValue = createValue("roles", &value_type::roles, "access_token", &value_type::accessToken, "guild_id", &value_type::guildId, "user_id",
			&value_type::userId, "nick", &value_type::nick, "mute", &value_type::mute, "deaf", &value_type::deaf);
	};

	template<> struct core<discord_core_api::modify_current_guild_member_data> {
		using value_type				 = discord_core_api::modify_current_guild_member_data;
		static constexpr auto parseValue = createValue("nick", &value_type::nick);
	};

	template<> struct core<discord_core_api::modify_guild_member_data> {
		using value_type				 = discord_core_api::modify_guild_member_data;
		static constexpr auto parseValue = createValue("deaf", &value_type::deaf, "mute", &value_type::mute, "nick", &value_type::nick, "roles", &value_type::roleIds, "channel_id",
			&value_type::channelId, "flags", &value_type::flags);
	};
}

namespace discord_core_api {

	modify_guild_member_data::modify_guild_member_data(const guild_member_data& other) {
		this->channelId = guild_members::getVoiceStateData(other).channelId;
		this->roleIds	= other.roles;
		if (roleIds.size() == 0) {
			jsonifierExcludedKeys.emplace("roles");
		}
		this->deaf			= other.deaf;
		this->mute			= other.mute;
		this->nick			= other.nick;
		this->flags			= static_cast<uint32_t>(other.flags);
		this->guildId		= other.guildId;
		this->guildMemberId = other.user.id;
	}

	guild_member_cache_data& guild_member_cache_data::operator=(const guild_member_data& other) {
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(guild_member_flags::Pending, other.pending);
		setFlagValue(guild_member_flags::Deaf, other.deaf);
		setFlagValue(guild_member_flags::Mute, other.mute);
		if (other.permissions.operator std::string_view() != "") {
			permissionsVal = other.permissions;
		}
		if (other.joinedAt != "") {
			joinedAt = other.joinedAt;
		}
		if (other.user.id != 0) {
			user.id = other.user.id;
		}
		if (other.guildId != 0) {
			guildId = other.guildId;
		}
		if (other.avatar != "") {
			avatar = other.avatar;
		}
		if (other.roles.size() > 0) {
			roles = other.roles;
		}
		if (other.nick != "") {
			nick = other.nick;
		}
		return *this;
	};

	guild_member_cache_data::guild_member_cache_data(const guild_member_data& other) {
		*this = other;
	}

	guild_member_cache_data& guild_member_cache_data::operator=(guild_member_data&& other) noexcept {
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(guild_member_flags::Pending, other.pending);
		setFlagValue(guild_member_flags::Deaf, other.deaf);
		setFlagValue(guild_member_flags::Mute, other.mute);
		if (other.permissions.operator std::string_view() != "") {
			permissionsVal = std::move(other.permissions);
		}
		if (other.joinedAt != "") {
			joinedAt = std::move(other.joinedAt);
		}
		if (other.avatar != "") {
			avatar = std::move(other.avatar);
		}
		if (other.roles.size() > 0) {
			roles = std::move(other.roles);
		}
		if (other.nick != "") {
			nick = std::move(other.nick);
		}
		if (other.user.id != 0) {
			user.id = other.user.id;
		}
		if (other.guildId != 0) {
			guildId = other.guildId;
		}
		return *this;
	};

	guild_member_cache_data::operator guild_member_data() {
		guild_member_data returnData{};
		returnData.permissions = permissionsVal.operator jsonifier::string();
		returnData.pending	   = getFlagValue(guild_member_flags::Pending);
		returnData.deaf		   = getFlagValue(guild_member_flags::Deaf);
		returnData.mute		   = getFlagValue(guild_member_flags::Mute);
		returnData.joinedAt	   = joinedAt.operator jsonifier::string();
		returnData.guildId	   = guildId;
		returnData.user.id	   = user.id;
		returnData.avatar	   = avatar;
		returnData.roles	   = roles;
		returnData.flags	   = flags;
		returnData.nick		   = nick;
		return returnData;
	}

	guild_member_cache_data::guild_member_cache_data(guild_member_data&& other) noexcept {
		*this = std::move(other);
	}

	void guild_members::initialize(discord_core_internal::https_client* client, config_manager* configManagerNew) {
		guild_members::doWeCacheGuildMembersBool = configManagerNew->doWeCacheGuildMembers();
		guild_members::doWeCacheVoiceStatesBool	 = configManagerNew->doWeCacheVoiceStates();
		guild_members::httpsClient				 = client;
	}

	co_routine<guild_member_data> guild_members::getGuildMemberAsync(get_guild_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Member };
		co_await newThreadAwaitable<guild_member_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack	   = "guild_members::getGuildMemberAsync()";
		guild_member_data data{};
		data.user.id = dataPackage.guildMemberId;
		data.guildId = dataPackage.guildId;
		two_id_key key{ data };
		if (cache.contains(key)) {
			data = cache[key];
		}
		guild_members::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheGuildMembersBool) {
			insertGuildMember(static_cast<guild_member_cache_data>(data));
		}
		co_return data;
	}

	guild_member_cache_data guild_members::getCachedGuildMember(get_guild_member_data dataPackage) {
		guild_member_cache_data data{};
		data.user.id = dataPackage.guildMemberId;
		data.guildId = dataPackage.guildId;
		two_id_key key{ data };
		if (cache.contains(key)) {
			return cache[key];
		} else {
			return getGuildMemberAsync(dataPackage).get();
		}
	}

	co_routine<jsonifier::vector<guild_member_data>> guild_members::listGuildMembersAsync(list_guild_members_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Members };
		co_await newThreadAwaitable<jsonifier::vector<guild_member_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "guild_members::listGuildMembersAsync()";
		jsonifier::vector<guild_member_data> returnData{};
		guild_members::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<guild_member_data>> guild_members::searchGuildMembersAsync(search_guild_members_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Search_Guild_Members };
		co_await newThreadAwaitable<jsonifier::vector<guild_member_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/search";
		if (dataPackage.query != "") {
			workload.relativePath += "?query=" + dataPackage.query;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + jsonifier::toString(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + jsonifier::toString(dataPackage.limit);
		}
		workload.callStack = "guild_members::searchGuildMembersAsync()";
		jsonifier::vector<guild_member_data> returnData{};
		guild_members::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_member_data> guild_members::addGuildMemberAsync(add_guild_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Guild_Member };
		co_await newThreadAwaitable<guild_member_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guild_members::addGuildMemberAsync()";
		guild_member_data returnData{};
		guild_members::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_member_data> guild_members::modifyCurrentGuildMemberAsync(modify_current_guild_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Current_Guild_Member };
		co_await newThreadAwaitable<guild_member_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/@me";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guild_members::modifyCurrentGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guild_member_data returnData{};
		guild_members::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_member_data> guild_members::modifyGuildMemberAsync(modify_guild_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Member };
		co_await newThreadAwaitable<guild_member_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		if (dataPackage.communicationDisabledUntil.operator jsonifier::string_base<char>() == "") {
			dataPackage.jsonifierExcludedKeys.emplace("communication_disabled_until");
		}
		if (dataPackage.channelId.operator jsonifier::string_base<char>() == "0" || dataPackage.channelId.operator const uint64_t &() == 0) {
			dataPackage.jsonifierExcludedKeys.emplace("channel_id");
			dataPackage.jsonifierExcludedKeys.emplace("mute");
			dataPackage.jsonifierExcludedKeys.emplace("deaf");
		}
		if (dataPackage.flags == 0) {
			dataPackage.jsonifierExcludedKeys.emplace("flags");
		}
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "guild_members::modifyGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guild_member_data data{};
		data.user.id = dataPackage.guildMemberId;
		data.guildId = dataPackage.guildId;
		two_id_key key{ data };
		if (cache.contains(key)) {
			data = cache[key];
		}
		try {
			guild_members::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		} catch (const std::runtime_error& error) {
			std::cout << error.what() << std::endl;
		}
		if (doWeCacheGuildMembersBool) {
			insertGuildMember(static_cast<guild_member_cache_data>(data));
		}
		co_return data;
	}

	co_routine<void> guild_members::removeGuildMemberAsync(remove_guild_member_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Member };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack	   = "guild_members::removeGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		guild_members::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<guild_member_data> guild_members::timeoutGuildMemberAsync(timeout_guild_member_data dataPackage) {
		co_await newThreadAwaitable<guild_member_data>();
		get_guild_member_data dataNew{ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId };
		guild_member_data guildMember = guild_members::getCachedGuildMember(dataNew);
		modify_guild_member_data dataPackage01{ guildMember };
		dataPackage01.deaf			= guildMember.getFlagValue(guild_member_flags::Deaf);
		dataPackage01.guildId		= guildMember.guildId;
		dataPackage01.guildMemberId = guildMember.user.id;
		dataPackage01.mute			= guildMember.getFlagValue(guild_member_flags::Mute);
		for (auto& value: guildMember.roles) {
			dataPackage01.roleIds.emplace_back(value);
		}
		dataPackage01.nick	 = guildMember.nick;
		dataPackage01.reason = dataPackage.reason;
		time_stamp timeStamp{};
		switch (dataPackage.numOfMinutesToTimeoutFor) {
			case timeout_durations::Day: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 1, 0, 0, time_format::long_date_time);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case timeout_durations::Five_Minutes: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(5, 0, 0, 0, 0, time_format::long_date_time);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case timeout_durations::Hour: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 1, 0, 0, 0, time_format::long_date_time);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case timeout_durations::Ten_Minutes: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(10, 0, 0, 0, 0, time_format::long_date_time);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case timeout_durations::Week: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 7, 0, 0, time_format::long_date_time);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case timeout_durations::Minute: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(1, 0, 0, 0, 0, time_format::long_date_time);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case timeout_durations::None: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 0, 0, 0, time_format::long_date_time);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
		}
		guild_members::modifyGuildMemberAsync(dataPackage01).get();
		co_return guildMember;
	}

	voice_state_data_light guild_members::getVoiceStateData(const two_id_key& key) {
		if (vsCache.contains(key)) {
			return vsCache[key];
		} else {
			return {};
		}
	}

	void guild_members::removeGuildMember(const two_id_key& key) {
		cache.erase(key);
	};

	void guild_members::removeVoiceState(const two_id_key& key) {
		vsCache.erase(key);
	}

	bool guild_members::doWeCacheGuildMembers() {
		return guild_members::doWeCacheGuildMembersBool;
	}

	bool guild_members::doWeCacheVoiceStates() {
		return guild_members::doWeCacheVoiceStatesBool;
	}

	object_cache<voice_state_data_light> guild_members::vsCache{};
	object_cache<guild_member_cache_data> guild_members::cache{};
	discord_core_internal::https_client* guild_members::httpsClient{};
	bool guild_members::doWeCacheGuildMembersBool{};
	bool guild_members::doWeCacheVoiceStatesBool{};
};
