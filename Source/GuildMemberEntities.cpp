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
/// GuildMemberEntities.cpp - Source file for the GuildMember related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::AddGuildMemberData> {
		using ValueType = DiscordCoreAPI::AddGuildMemberData;
		static constexpr auto parseValue = object("roles", &ValueType::roles, "access_token", &ValueType::accessToken, "guild_id",
			&ValueType::guildId, "user_id", &ValueType::userId, "nick", &ValueType::nick, "mute", &ValueType::mute, "deaf", &ValueType::deaf);
	};

	template<> struct Core<DiscordCoreAPI::ModifyCurrentGuildMemberData> {
		using ValueType = DiscordCoreAPI::ModifyCurrentGuildMemberData;
		static constexpr auto parseValue = object("guild_id", &ValueType::guildId, "nick", &ValueType::nick, "reason", &ValueType::reason);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildMemberData> {
		using ValueType = DiscordCoreAPI::ModifyGuildMemberData;
		static constexpr auto parseValue = object("channel_id", &ValueType::currentChannelId, "deaf", &ValueType::deaf, "guild_id",
			&ValueType::guildId, "mute", &ValueType::mute, "nick", &ValueType::nick, "roles", &ValueType::roleIds, "user_id",
			&ValueType::guildMemberId, "voice_channel_id", &ValueType::newVoiceChannelId, "reason", &ValueType::reason);
	};
}

namespace DiscordCoreAPI {

	GuildMemberData& GuildMemberData::operator=(GuildMemberCacheData&& other) noexcept {
		permissions = std::move(other.permissions);
		joinedAt = std::move(other.joinedAt);
		avatar = std::move(other.avatar);
		roles = std::move(other.roles);
		nick = std::move(other.nick);
		guildId = other.guildId;
		user.id = other.user.id;
		flags = other.flags;
		return *this;
	};

	GuildMemberData::GuildMemberData(GuildMemberCacheData&& other) noexcept {
		*this = std::move(other);
	}

	GuildMemberData& GuildMemberData::operator=(const GuildMemberCacheData& other) noexcept {
		permissions = other.permissions;
		joinedAt = other.joinedAt;
		guildId = other.guildId;
		user.id = other.user.id;
		avatar = other.avatar;
		flags = other.flags;
		roles = other.roles;
		nick = other.nick;
		return *this;
	};

	GuildMemberData::GuildMemberData(const GuildMemberCacheData& other) noexcept {
		*this = other;
	}

	GuildMember& GuildMember::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			permissions = std::move(other.permissions);
			joinedAt = std::move(other.joinedAt);
			avatar = std::move(other.avatar);
			roles = std::move(other.roles);
			nick = std::move(other.nick);
			user.id = other.user.id;
			guildId = other.guildId;
			flags = other.flags;
		}
		return *this;
	};

	GuildMember::GuildMember(GuildMemberData&& other) noexcept {
		*this = std::move(other);
	}

	GuildMember& GuildMember::operator=(const GuildMemberData& other) noexcept {
		if (this != &other) {
			permissions = other.permissions;
			joinedAt = other.joinedAt;
			user.id = other.user.id;
			guildId = other.guildId;
			avatar = other.avatar;
			roles = other.roles;
			flags = other.flags;
			nick = other.nick;
		}
		return *this;
	};

	GuildMember::GuildMember(const GuildMemberData& other) noexcept {
		*this = other;
	}

	void GuildMembers::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		GuildMembers::doWeCacheGuildMembersBool = configManagerNew->doWeCacheUsers();
		GuildMembers::httpsClient = client;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack = "GuildMembers::getGuildMemberAsync()";
		GuildMember data{};
		data.guildId = dataPackage.guildId;
		data.user.id = dataPackage.guildMemberId;
		GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, data);
		if (cache.contains(GuildMemberKey{ .guildId = dataPackage.guildId, .userId = dataPackage.guildMemberId })) {
			data = cache.find(GuildMemberKey{ .guildId = dataPackage.guildId, .userId = dataPackage.guildMemberId });
		} else {
			data = GuildMembers::insertGuildMember(std::move(data));
		}
		co_return data;
	}

	GuildMemberData GuildMembers::getCachedGuildMember(GetGuildMemberData dataPackage) {
		GuildMemberData key{};
		key.user.id = dataPackage.guildMemberId;
		key.guildId = dataPackage.guildId;
		if (cache.contains(GuildMemberKey{ .guildId = dataPackage.guildId, .userId = dataPackage.guildMemberId })) {
			return cache.find(GuildMemberKey{ .guildId = dataPackage.guildId, .userId = dataPackage.guildMemberId });
		} else {
		}
		return GuildMembers::getGuildMemberAsync(dataPackage).get();
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::listGuildMembersAsync()";
		GuildMemberVector returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMemberVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::searchGuildMembersAsync(SearchGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Search_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/search";
		if (dataPackage.query != "") {
			workload.relativePath += "?query=" + dataPackage.query;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::searchGuildMembersAsync()";
		GuildMemberVector returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMemberVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildMember> GuildMembers::addGuildMemberAsync(AddGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildMembers::addGuildMemberAsync()";
		GuildMember returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/@me";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildMembers::modifyCurrentGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMember returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildMember> GuildMembers::modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildMembers::modifyGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMember data{};
		data.guildId = dataPackage.guildId;
		data.user.id = dataPackage.guildMemberId;
		GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, data);
		if (cache.contains(GuildMemberKey{ .guildId = dataPackage.guildId, .userId = dataPackage.guildMemberId })) {
			data = cache.find(GuildMemberKey{ .guildId = dataPackage.guildId, .userId = dataPackage.guildMemberId });
		} else {
			data = GuildMembers::insertGuildMember(std::move(data));
		}
		co_return data;
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack = "GuildMembers::removeGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto guildMember = GuildMembers::getCachedGuildMember({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId });
		GuildMembers::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<GuildMember> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMember>();
		GuildMemberData guildMember =
			GuildMembers::getCachedGuildMember({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId });
		ModifyGuildMemberData dataPackage01{};
		dataPackage01.deaf = getBool<GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Deaf);
		dataPackage01.guildId = guildMember.guildId;
		dataPackage01.guildMemberId = guildMember.user.id;
		dataPackage01.mute = getBool<GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Mute);
		for (auto& value: guildMember.roles) {
			dataPackage01.roleIds.emplace_back(value);
		}
		dataPackage01.nick = guildMember.nick;
		dataPackage01.reason = dataPackage.reason;
		TimeStamp timeStamp{};
		switch (dataPackage.numOfMinutesToTimeoutFor) {
			case TimeoutDurations::Day: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 1, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Five_Minutes: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(5, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Hour: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 1, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Ten_Minutes: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(10, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Week: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 7, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Minute: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(1, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::None: {
				auto string = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
		}
		GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
		co_return guildMember;
	}

	VoiceStateDataLight& GuildMembers::insertVoiceState(VoiceStateDataLight&& voiceState) {
		if (voiceState.id == 0) {
			throw DCAException{ "Sorry, but there was no id set for that voice state." };
		}
		auto id = voiceState.id;
		auto guildId = voiceState.guildId;
		vsCache.emplace(std::forward<VoiceStateDataLight>(voiceState));
		return vsCache.find(GuildMemberKey{ .guildId = guildId, .userId = id });
	}

	void GuildMembers::removeVoiceState(const VoiceStateDataLight& voiceState) {
		vsCache.erase(GuildMemberKey{ .guildId = voiceState.guildId, .userId = voiceState.id });
	}

	VoiceStateDataLight GuildMembers::getVoiceStateData(const GuildMember& voiceState) {
		if (vsCache.contains(GuildMemberKey{ .guildId = voiceState.guildId, .userId = voiceState.user.id })) {
			return vsCache.find(GuildMemberKey{ .guildId = voiceState.guildId, .userId = voiceState.user.id });
		} else {
			return {};
		}
	}

	ObjectCache<GuildMemberKey, GuildMemberData>& GuildMembers::getCache() {
		return cache;
	}

	GuildMemberData& GuildMembers::insertGuildMember(GuildMemberData&& guildMember) {
		if (guildMember.guildId == 0 || guildMember.user.id == 0) {
			throw DCAException{ "Sorry, but there was no id set for that guildmember." };
		}
		GuildMemberKey key{ .guildId = guildMember.guildId, .userId = guildMember.user.id };
		if (GuildMembers::doWeCacheGuildMembersBool) {
			cache.emplace(std::forward<GuildMemberData>(guildMember));
		}
		return cache.find(key);
	}

	void GuildMembers::removeGuildMember(const GuildMemberData& guildMember) {
		cache.erase(GuildMemberKey{ .guildId = guildMember.guildId, .userId = guildMember.user.id });
	};

	bool GuildMembers::doWeCacheGuildMembers() {
		return GuildMembers::doWeCacheGuildMembersBool;
	}

	ObjectCache<GuildMemberKey, VoiceStateDataLight> vsCache{};
	ObjectCache<GuildMemberKey, GuildMemberData> cache{};
	DiscordCoreInternal::HttpsClient* GuildMembers::httpsClient{};
	bool GuildMembers::doWeCacheGuildMembersBool{};
};
