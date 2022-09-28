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
/// GuildMemberEntities.cpp - Source file for the GuildMember related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file GuildMemberEntities.cpp

#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	GuildMemberVector::GuildMemberVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theGuildMembers.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildMember newData{ value.value() };
					this->theGuildMembers.push_back(std::move(newData));
				}
				this->theGuildMembers.shrink_to_fit();
			}
		}
	}

	AddGuildMemberData::operator JsonObject() {
		JsonObject theData{};
		theData["access_token"] = this->accessToken;
		for (auto& value: this->roles) {
			theData.pushBack("roles", std::to_string(value.operator size_t()));
		}
		theData["deaf"] = this->deaf;
		theData["mute"] = this->mute;
		theData["nick"] = this->nick;
		return theData;
	}

	ModifyGuildMemberData::operator JsonObject() {
		JsonObject theData{};
		theData["nick"] = this->nick;
		theData["communication_disabled_until"] = static_cast<std::string>(this->communicationDisabledUntil);
		for (auto& value: this->roleIds) {
			theData.pushBack("roles", std::to_string(value.operator size_t()));
		}
		if (this->newVoiceChannelId.operator size_t() != 0) {
			theData["channel_id"] = std::to_string(this->newVoiceChannelId);
			theData["mute"] = this->mute;
			theData["deaf"] = this->deaf;
		}
		return theData;
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

	GuildMember::GuildMember(simdjson::ondemand::value jsonObjectData) {
		this->flags |= setBool(this->flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		this->flags |= setBool(this->flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		this->flags |= setBool(this->flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->guildId = getId(jsonObjectData, "guild_id");

		try {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData["roles"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->roles.clear();
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					this->roles.emplace_back(stoull(std::string{ value.get_string().take_value().data() }));
				}
			}
		} catch (...) {
			reportException("GuildMember::GuildMember()");
		}
		this->permissions = getString(jsonObjectData, "permissions");

		simdjson::ondemand::value theObject{};
		auto theResult = jsonObjectData["user"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			UserData theUser{ theObject };
			this->id = theUser.id;
			Users::insertUser(std::move(theUser));
		}

		this->avatar = getString(jsonObjectData, "avatar");

		this->flags |= getUint8(jsonObjectData, "flags");

		this->nick = getString(jsonObjectData, "nick");

		this->communicationDisabledUntil = getString(jsonObjectData, "communication_disabled_until");

		this->premiumSince = getString(jsonObjectData, "premium_since");
	}

	GuildMemberVector::operator std::vector<GuildMember>() {
		return this->theGuildMembers;
	}

	void GuildMembers::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		GuildMembers::doWeCacheGuildMembers = configManagerNew->doWeCacheUsers();
		GuildMembers::httpsClient = theClient;
	}

	CoRoutine<GuildMember> GuildMembers::getGuildMemberAsync(GetGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/" + std::to_string(dataPackage.guildMemberId);
		workload.callStack = "GuildMembers::getGuildMemberAsync()";
		GuildMember theData{};
		theData.guildId = dataPackage.guildId;
		theData.id = dataPackage.guildMemberId;
		if (GuildMembers ::cache.contains(theData)) {
			theData = GuildMembers::cache.readOnly(theData);
		}
		theData = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, &theData);
		theData.guildId = dataPackage.guildId;
		GuildMembers::insertGuildMember(theData);
		co_return theData;
	}

	CoRoutine<GuildMemberData> GuildMembers::getCachedGuildMemberAsync(GetGuildMemberData dataPackage) {
		co_await NewThreadAwaitable<GuildMemberData>();
		GuildMemberData theKey{};
		theKey.id = dataPackage.guildMemberId;
		theKey.guildId = dataPackage.guildId;
		if (GuildMembers::cache.contains(theKey)) {
			theKey = GuildMembers::cache.readOnly(theKey);
			co_return theKey;
		}
		co_return GuildMembers::getGuildMemberAsync(dataPackage).get();
	}

	CoRoutine<std::vector<GuildMember>> GuildMembers::listGuildMembersAsync(ListGuildMembersData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Members };
		co_await NewThreadAwaitable<std::vector<GuildMember>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members";
		if (dataPackage.after.operator size_t() != 0) {
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "GuildMembers::addGuildMemberAsync()";
		co_return GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload);
	}

	CoRoutine<GuildMember> GuildMembers::modifyCurrentGuildMemberAsync(ModifyCurrentGuildMemberData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Current_Guild_Member };
		co_await NewThreadAwaitable<GuildMember>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/members/@me";
		JsonObject theData{};
		theData["nick"] = dataPackage.nick;
		workload.content = theData;
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "GuildMembers::modifyGuildMemberAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		GuildMember theData{};
		theData.id = dataPackage.guildMemberId;
		theData.guildId = dataPackage.guildId;
		if (GuildMembers ::cache.contains(theData)) {
			theData = GuildMembers::cache.readOnly(theData);
		}
		theData = GuildMembers::httpsClient->submitWorkloadAndGetResult<GuildMember>(workload, &theData);
		theData.guildId = dataPackage.guildId;
		GuildMembers::insertGuildMember(theData);
		co_return theData;
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
				auto theString = theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 1, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theString;
				break;
			}
			case TimeoutDurations::Five_Minutes: {
				auto theString = theTimeStamp.convertToFutureISO8601TimeStamp(5, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theString;
				break;
			}
			case TimeoutDurations::Hour: {
				auto theString = theTimeStamp.convertToFutureISO8601TimeStamp(0, 1, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theString;
				break;
			}
			case TimeoutDurations::Ten_Minutes: {
				auto theString = theTimeStamp.convertToFutureISO8601TimeStamp(10, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theString;
				break;
			}
			case TimeoutDurations::Week: {
				auto theString = theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 7, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theString;
				break;
			}
			case TimeoutDurations::Minute: {
				auto theString = theTimeStamp.convertToFutureISO8601TimeStamp(1, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theString;
				break;
			}
			case TimeoutDurations::None: {
				auto theString = theTimeStamp.convertToFutureISO8601TimeStamp(0, 0, 0, 0, 0, TimeFormat::LongDateTime);
				dataPackage01.communicationDisabledUntil = theString;
				break;
			}
		}
		co_return GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
	}

	void GuildMembers::insertGuildMember(GuildMemberData guildMember) {
		if (guildMember.id.operator size_t() == 0) {
			return;
		}
		if (GuildMembers::doWeCacheGuildMembers) {
			if (!GuildMembers::cache.contains(guildMember)) {
				GuildMembers::cache.emplace(std::move(guildMember));
			} else {
				GuildMembers::cache.at(guildMember) = std::move(guildMember);
			}
			if (GuildMembers::cache.size() % 1000 == 0) {
				std::cout << "THE GUILDMEMBER COUNT: " << GuildMembers::cache.size() << std::endl;
			}
		}
	}

	void GuildMembers::removeGuildMember(GuildMemberData guildMember) {
		GuildMembers::cache.erase(guildMember);
	};

	DiscordCoreInternal::HttpsClient* GuildMembers::httpsClient{ nullptr };
	ObjectCache<GuildMemberData> GuildMembers::cache{};
	bool GuildMembers::doWeCacheGuildMembers{ false };
};
