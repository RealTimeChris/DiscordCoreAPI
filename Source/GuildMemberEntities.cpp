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
/// GuildMemberEntities.cpp - Source file for the GuildMemberData related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace jsonifier {

	template<> struct core<DiscordCoreAPI::AddGuildMemberData> {
		using ValueType					 = DiscordCoreAPI::AddGuildMemberData;
		static constexpr auto parseValue = createObject("roles", &ValueType::roles, "access_token", &ValueType::accessToken, "guild_id", &ValueType::guildId, "user_id",
			&ValueType::userId, "nick", &ValueType::nick, "mute", &ValueType::mute, "deaf", &ValueType::deaf);
	};

	template<> struct core<DiscordCoreAPI::ModifyCurrentGuildMemberData> {
		using ValueType					 = DiscordCoreAPI::ModifyCurrentGuildMemberData;
		static constexpr auto parseValue = createObject("guild_id", &ValueType::guildId, "nick", &ValueType::nick, "reason", &ValueType::reason);
	};

	template<> struct core<DiscordCoreAPI::ModifyGuildMemberData> {
		using ValueType = DiscordCoreAPI::ModifyGuildMemberData;
		static constexpr auto parseValue =
			createObject("channel_id", &ValueType::currentChannelId, "deaf", &ValueType::deaf, "guild_id", &ValueType::guildId, "mute", &ValueType::mute, "nick", &ValueType::nick,
				"roles", &ValueType::roleIds, "user_id", &ValueType::guildMemberId, "voice_channel_id", &ValueType::newVoiceChannelId, "reason", &ValueType::reason);
	};
}

namespace DiscordCoreAPI {

	GuildMemberCacheData& GuildMemberCacheData::operator=(const GuildMemberData& other) {
		setFlagValue(GuildMemberFlags::Pending, other.pending);
		setFlagValue(GuildMemberFlags::Deaf, other.deaf);
		setFlagValue(GuildMemberFlags::Mute, other.mute);
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		if (other.permissions != "") {
			permissions = other.permissions;
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

	GuildMemberCacheData::GuildMemberCacheData(const GuildMemberData& other) {
		*this = other;
	}

	GuildMemberCacheData& GuildMemberCacheData::operator=(GuildMemberData&& other) noexcept {
		setFlagValue(GuildMemberFlags::Pending, other.pending);
		setFlagValue(GuildMemberFlags::Deaf, other.deaf);
		setFlagValue(GuildMemberFlags::Mute, other.mute);
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		if (other.permissions != "") {
			permissions = std::move(other.permissions);
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

	GuildMemberCacheData::operator GuildMemberData() {
		GuildMemberData returnData{};
		returnData.pending	   = getFlagValue(GuildMemberFlags::Pending);
		returnData.permissions = permissions.operator std::string();
		returnData.deaf		   = getFlagValue(GuildMemberFlags::Deaf);
		returnData.mute		   = getFlagValue(GuildMemberFlags::Mute);
		returnData.joinedAt	   = joinedAt.operator std::string();
		returnData.nick		   = nick.operator std::string();
		returnData.guildId	   = guildId;
		returnData.user.id	   = user.id;
		returnData.avatar	   = avatar;
		returnData.roles	   = roles;
		returnData.flags	   = flags;
		return returnData;
	}

	GuildMemberCacheData::GuildMemberCacheData(GuildMemberData&& other) noexcept {
		*this = std::move(other);
	}

	void GuildMembers::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		GuildMembers::doWeCacheGuildMembersBool = configManagerNew->doWeCacheGuildMembers();
		GuildMembers::doWeCacheVoiceStatesBool	= configManagerNew->doWeCacheVoiceStates();
		GuildMembers::httpsClient				= client;
	}

	CoRoutine<GuildMemberData> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Member };
		co_await NewThreadAwaitable<GuildMemberData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack	   = "GuildMembers::getGuildMemberAsync()";
		GuildMemberData data{};
		data.user.id = dataPackage.guildMemberId;
		data.guildId = dataPackage.guildId;
		TwoIdKey key{ data };
		if (cache.contains(key)) {
			data = cache[key];
		}
		GuildMembers::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheGuildMembersBool) {
			insertGuildMember(static_cast<GuildMemberCacheData>(data));
		}
		co_return data;
	}

	GuildMemberCacheData GuildMembers::getCachedGuildMember(GetGuildMemberData dataPackage) {
		GuildMemberCacheData data{};
		data.user.id = dataPackage.guildMemberId;
		data.guildId = dataPackage.guildId;
		TwoIdKey key{ data };
		if (cache.contains(key)) {
			return cache[key];
		} else {
			return {};
		}
	}

	CoRoutine<jsonifier::vector<GuildMemberData>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Members };
		co_await NewThreadAwaitable<jsonifier::vector<GuildMemberData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + dataPackage.after;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::listGuildMembersAsync()";
		jsonifier::vector<GuildMemberData> returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<jsonifier::vector<GuildMemberData>> GuildMembers::searchGuildMembersAsync(SearchGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Search_Guild_Members };
		co_await NewThreadAwaitable<jsonifier::vector<GuildMemberData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/search";
		if (dataPackage.query != "") {
			workload.relativePath += "?query=" + dataPackage.query;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::searchGuildMembersAsync()";
		jsonifier::vector<GuildMemberData> returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildMemberData> GuildMembers::addGuildMemberAsync(AddGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Member };
		co_await NewThreadAwaitable<GuildMemberData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.userId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildMembers::addGuildMemberAsync()";
		GuildMemberData returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildMemberData> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_Guild_Member };
		co_await NewThreadAwaitable<GuildMemberData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/@me";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildMembers::modifyCurrentGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMemberData returnData{};
		GuildMembers::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildMemberData> GuildMembers::modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Member };
		co_await NewThreadAwaitable<GuildMemberData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "GuildMembers::modifyGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMemberData data{};
		data.user.id = dataPackage.guildMemberId;
		data.guildId = dataPackage.guildId;
		TwoIdKey key{ data };
		if (cache.contains(key)) {
			data = cache[key];
		}
		GuildMembers::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheGuildMembersBool) {
			insertGuildMember(static_cast<GuildMemberCacheData>(data));
		}
		co_return data;
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/members/" + dataPackage.guildMemberId;
		workload.callStack	   = "GuildMembers::removeGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMembers::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<GuildMemberData> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMemberData>();
		GuildMemberData guildMember = GuildMembers::getCachedGuildMember({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId });
		ModifyGuildMemberData dataPackage01{};
		dataPackage01.deaf			= guildMember.getFlagValue(GuildMemberFlags::Deaf);
		dataPackage01.guildId		= guildMember.guildId;
		dataPackage01.guildMemberId = guildMember.user.id;
		dataPackage01.mute			= guildMember.getFlagValue(GuildMemberFlags::Mute);
		for (auto& value: guildMember.roles) {
			dataPackage01.roleIds.emplace_back(value);
		}
		dataPackage01.nick	 = guildMember.nick;
		dataPackage01.reason = dataPackage.reason;
		TimeStamp timeStamp{};
		switch (dataPackage.numOfMinutesToTimeoutFor) {
			case TimeoutDurations::Day: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 1, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Five_Minutes: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(5, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Hour: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 1, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Ten_Minutes: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(10, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Week: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 7, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::Minute: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(1, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
			case TimeoutDurations::None: {
				auto string								 = timeStamp.convertToFutureISO8601TimeStamp(0, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = string;
				break;
			}
		}
		GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
		co_return guildMember;
	}

	VoiceStateDataLight GuildMembers::getVoiceStateData(const TwoIdKey& key) {
		if (vsCache.contains(key)) {
			return vsCache[key];
		} else {
			return {};
		}
	}

	void GuildMembers::removeGuildMember(const TwoIdKey& key) {
		cache.erase(key);
	};

	void GuildMembers::removeVoiceState(const TwoIdKey& key) {
		vsCache.erase(key);
	}

	bool GuildMembers::doWeCacheGuildMembers() {
		return GuildMembers::doWeCacheGuildMembersBool;
	}

	bool GuildMembers::doWeCacheVoiceStates() {
		return GuildMembers::doWeCacheVoiceStatesBool;
	}

	ObjectCache<VoiceStateDataLight> GuildMembers::vsCache{};
	ObjectCache<GuildMemberCacheData> GuildMembers::cache{};
	DiscordCoreInternal::HttpsClient* GuildMembers::httpsClient{};
	bool GuildMembers::doWeCacheGuildMembersBool{};
	bool GuildMembers::doWeCacheVoiceStatesBool{};
};
