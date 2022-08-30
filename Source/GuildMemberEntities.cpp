/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// GuildMemberEntities.cpp - Source file for the GuildMember related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	AddGuildMemberData::operator std::string() {
		nlohmann::json data{};
		data["access_token"] = this->accessToken;
		data["roles"] = this->roles;
		data["deaf"] = this->deaf;
		data["mute"] = this->mute;
		data["nick"] = this->nick;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	ModifyGuildMemberData::operator std::string() {
		nlohmann::json data{};
		data["nick"] = this->nick;
		data["communication_disabled_until"] = std::string(this->communicationDisabledUntil);
		if (this->roleIds.size() == 0) {
			data["roles"] = nullptr;
		} else {
			nlohmann::json roleIdArray{};
			for (auto& value: this->roleIds) {
				roleIdArray.emplace_back(value);
			}
			data["roles"] = roleIdArray;
		}
		if (this->newVoiceChannelId != 0) {
			data["channel_id"] = std::to_string(this->newVoiceChannelId);
			data["mute"] = this->mute;
			data["deaf"] = this->deaf;
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	void GuildMemberData::insertGuildMember(std::unique_ptr<GuildMemberData> other) {
		GuildMembers::insertGuildMember(std::move(other));
	}

	std::string GuildMemberData::getAvatarUrl() {
		if (this->avatar.getIconHash() != "") {
			std::string theStringNew{ "https://cdn.discordapp.com/" };
			theStringNew += "guilds/" + std::to_string(this->guildId) + "/users/" + std::to_string(this->id) + "/avatars/" + this->avatar.getIconHash();
			return theStringNew;
		} else {
			return this->getUserData().getAvatarUrl();
		}
	}

	UserData GuildMemberData::getUserData() {
		if (this->id != 0) {
			return Users::getCachedUserAsync({ .userId = this->id }).get();
		} else {
			return {};
		}
	}

	GuildMember& GuildMember::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			this->permissions = std::move(other.permissions);
			this->voiceChannelId = other.voiceChannelId;
			this->joinedAt = std::move(other.joinedAt);
			this->avatar = std::move(other.avatar);
			this->roles = std::move(other.roles);
			this->nick = std::move(other.nick);
			this->guildId = other.guildId;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	};

	GuildMember::GuildMember(GuildMemberData&& other) noexcept {
		*this = std::move(other);
	}

	GuildMember& GuildMember::operator=(const GuildMemberData& other) noexcept {
		if (this != &other) {
			this->voiceChannelId = other.voiceChannelId;
			this->permissions = other.permissions;
			this->joinedAt = other.joinedAt;
			this->guildId = other.guildId;
			this->avatar = other.avatar;
			this->roles = other.roles;
			this->flags = other.flags;
			this->nick = other.nick;
			this->id = other.id;
		}
		return *this;
	};

	GuildMember::GuildMember(const GuildMemberData& other) noexcept {
		*this = other;
	}

	void GuildMember::parseObject(nlohmann::json& jsonObjectData) {
		DiscordCoreAPI::parseObject(jsonObjectData, *this);
	}

	GuildMemberVector::operator std::vector<GuildMember>() {
		return this->theGuildMembers;
	}

	GuildMemberVector& GuildMemberVector::operator=(nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildMemberVector::GuildMemberVector(nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	void GuildMembers::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		GuildMembers::configManager = configManagerNew;
		GuildMembers::httpsClient = theClient;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.callStack = "GuildMembers::getGuildMemberAsync()";
		GuildMemberData* theData{};
		std::unique_lock theLock{ GuildMembers::theMutex };
		if (!GuildMembers::cache.contains(dataPackage.guildId)) {
			GuildMembers::cache[dataPackage.guildId] = GuildMemberHolder{};
		}
		if (!GuildMembers::cache[dataPackage.guildId].cache.contains(dataPackage.guildMemberId)) {
			GuildMembers::cache[dataPackage.guildId].cache[dataPackage.guildMemberId] = std::make_unique<GuildMemberData>();
		}
		theData = GuildMembers::cache[dataPackage.guildId].cache[dataPackage.guildMemberId].get();
		*theData = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, static_cast<GuildMember*>(theData));
		theData->guildId = dataPackage.guildId;
		co_return *theData;
	}

	CoRoutine<GuildMember> GuildMembers::getCachedGuildMemberAsync(GetGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMember>();
		std::shared_lock theLock{ GuildMembers::theMutex };
		if (GuildMembers::cache.contains(dataPackage.guildId)) {
			if (GuildMembers::cache[dataPackage.guildId].cache.contains(dataPackage.guildMemberId)) {
				co_return *GuildMembers::cache[dataPackage.guildId].cache[dataPackage.guildMemberId];
			}
		}
		co_return GuildMembers::getGuildMemberAsync(dataPackage).get();
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members";
		if (dataPackage.after != 0) {
			workload.relativePath += "?after=" + std::to_string(dataPackage.after);
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::listGuildMembersAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMemberVector>(workload);
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::searchGuildMembersAsync(SearchGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Search_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/search";
		if (dataPackage.query != "") {
			workload.relativePath += "?query=" + dataPackage.query;
			if (dataPackage.limit != 0) {
				workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
			}
		} else if (dataPackage.limit != 0) {
			workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
		}
		workload.callStack = "GuildMembers::searchGuildMembersAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMemberVector>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::addGuildMemberAsync(AddGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.userId);
		workload.content = dataPackage;
		workload.callStack = "GuildMembers::addGuildMemberAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/@me";
		workload.content = nlohmann::json{ { "nick", dataPackage.nick } }.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
		workload.callStack = "GuildMembers::modifyCurrentGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.content = dataPackage;
		workload.callStack = "GuildMembers::modifyGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMemberData* theData{};
		std::unique_lock theLock{ GuildMembers::theMutex };
		if (!GuildMembers::cache.contains(dataPackage.guildId)) {
			GuildMembers::cache[dataPackage.guildId] = GuildMemberHolder{};
		}
		if (!GuildMembers::cache[dataPackage.guildId].cache.contains(dataPackage.guildMemberId)) {
			GuildMembers::cache[dataPackage.guildId].cache[dataPackage.guildMemberId] = std::make_unique<GuildMemberData>();
		}
		theData = GuildMembers::cache[dataPackage.guildId].cache[dataPackage.guildMemberId].get();
		*theData = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, static_cast<GuildMember*>(theData));
		theData->guildId = dataPackage.guildId;
		co_return *theData;
	}

	CoRoutine<void> GuildMembers::removeGuildMemberAsync(RemoveGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Member };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.callStack = "GuildMembers::removeGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		auto theGuildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::timeoutGuildMemberAsync(TimeoutGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMember>();
		GuildMemberData guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.guildMemberId, .guildId = dataPackage.guildId }).get();
		ModifyGuildMemberData dataPackage01{};
		dataPackage01.deaf = getBool<int8_t, GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Deaf);
		dataPackage01.guildId = guildMember.guildId;
		dataPackage01.guildMemberId = guildMember.id;
		dataPackage01.mute = getBool<int8_t, GuildMemberFlags>(guildMember.flags, GuildMemberFlags::Mute);
		dataPackage01.roleIds = guildMember.roles;
		dataPackage01.nick = guildMember.nick;
		dataPackage01.reason = dataPackage.reason;
		TimeStamp<std::chrono::milliseconds> theTimeStamp{};
		switch (dataPackage.numOfMinutesToTimeoutFor) {
			case TimeoutDurations::Day: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 1, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Five_Minutes: {
				theTimeStamp.convertToFutureISO8601TimeStamp(5, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Hour: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 1, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Ten_Minutes: {
				theTimeStamp.convertToFutureISO8601TimeStamp(10, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Week: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 7, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::Minute: {
				theTimeStamp.convertToFutureISO8601TimeStamp(1, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
			case TimeoutDurations::None: {
				theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theTimeStamp;
				break;
			}
		}
		co_return GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
	}

	void GuildMembers::insertGuildMember(std::unique_ptr<GuildMemberData> guildMember) {
		std::unique_lock theLock{ GuildMembers::theMutex };
		if (guildMember->id == 0) {
			return;
		}
		if (GuildMembers::configManager->doWeCacheGuildMembers()) {
			auto guildMemberId = guildMember->id;
			auto guildId = guildMember->guildId;
			if (!GuildMembers::cache.contains(guildId)) {
				GuildMembers::cache.emplace(guildId, GuildMemberHolder{});
			}
			if (!GuildMembers::cache[guildId].cache.contains(guildMemberId)) {
				GuildMembers::cache[guildId].cache.emplace(guildMemberId, std::move(guildMember));
			} else {
				GuildMembers::cache[guildId].cache.insert_or_assign(guildMemberId, std::move(guildMember));
			}
		}
	}

	void GuildMembers::removeGuildMember(GuildMemberData guildMemberId) {
		std::unique_lock theLock{ GuildMembers::theMutex };
		if (GuildMembers::cache.contains(guildMemberId.guildId)) {
			if (GuildMembers::cache[guildMemberId.guildId].cache.contains(guildMemberId.id)) {
				GuildMembers::cache[guildMemberId.guildId].cache.erase(guildMemberId.id);
			}
		}
	};

	DiscordCoreInternal::HttpsClient* GuildMembers::httpsClient{ nullptr };
	std::map<Snowflake, GuildMemberHolder> GuildMembers::cache{};
	ConfigManager* GuildMembers::configManager{ nullptr };
	std::shared_mutex GuildMembers::theMutex{};
};
