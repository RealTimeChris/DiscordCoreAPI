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
/// ChannelEntities.cpp - Source file for the classes related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ChannelEntities.cpp

#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	EditChannelPermissionOverwritesData::operator std::string() {
		nlohmann::json data{};
		data["allow"] = this->allow;
		data["deny"] = this->deny;
		data["type"] = this->type;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	CreateChannelInviteData::operator std::string() {
		nlohmann::json data{};
		if (this->targetUserId != 0) {
			data["target_application_id"] = this->targetApplicationId;
			data["target_user_id"] = std::to_string(this->targetUserId);
			data["target_type"] = this->targetType;
		}
		data["temporary"] = this->temporary;
		data["max_uses"] = this->maxUses;
		data["max_age"] = this->maxAge;
		data["unique"] = this->unique;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	FollowNewsChannelData::operator std::string() {
		nlohmann::json data{};
		data["webhook_channel_id"] = std::to_string(this->targetChannelId);
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	CreateGuildChannelData::operator std::string() {
		nlohmann::json data{};
		if (this->type == DiscordCoreAPI::ChannelType::Guild_Voice || this->type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			data["user_limit"] = this->userLimit;
			data["bitrate"] = this->bitrate;
		}
		nlohmann::json overwrites{};
		for (auto& value: this->permissionOverwrites) {
			nlohmann::json newData{};
			newData["allow"] = value.allow;
			newData["deny"] = value.deny;
			newData["type"] = value.type;
			newData["id"] = std::to_string(value.id);
			overwrites.emplace_back(newData);
		}
		data["default_auto_archive_duration"] = this->defaultAutoArchiveDuration;
		data["rate_limit_per_user"] = this->rateLimitPerUser;
		data["permission_overwrites"] = overwrites;
		data["parent_id"] = std::to_string(this->parentId);
		data["position"] = this->position;
		data["topic"] = this->topic;
		data["name"] = this->name;
		data["nsfw"] = this->nsfw;
		data["type"] = this->type;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	ModifyGuildChannelPositionsData::operator std::string() {
		nlohmann::json data{};
		for (auto& value: this->modifyChannelData) {
			nlohmann::json dataNew{};
			dataNew["lock_permissions"] = value.lockPermissions;
			if (value.parentId != 0) {
				dataNew["parent_id"] = value.parentId;
			}
			dataNew["position"] = value.position;
			dataNew["id"] = std::to_string(value.id);
			data.emplace_back(dataNew);
		}
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	Channel& Channel::operator=(ChannelData&& other) noexcept {
		if (this != &other) {
			this->permissionOverwrites = std::move(other.permissionOverwrites);
			this->memberCount = other.memberCount;
			this->name = std::move(other.name);
			this->parentId = other.parentId;
			this->position = other.position;
			this->guildId = other.guildId;
			this->ownerId = other.ownerId;
			this->topic = other.topic;
			this->flags = other.flags;
			this->type = other.type;
			this->id = other.id;
		}
		return *this;
	}

	Channel::Channel(ChannelData&& other) noexcept {
		*this = std::move(other);
	}

	Channel& Channel::operator=(const ChannelData& other) noexcept {
		if (this != &other) {
			this->permissionOverwrites = other.permissionOverwrites;
			this->memberCount = other.memberCount;
			this->parentId = other.parentId;
			this->position = other.position;
			this->guildId = other.guildId;
			this->ownerId = other.ownerId;
			this->topic = other.topic;
			this->flags = other.flags;
			this->type = other.type;
			this->name = other.name;
			this->id = other.id;
		}
		return *this;
	}

	Channel::Channel(const ChannelData& other) noexcept {
		*this = other;
	}

	std::string Channel::getIconUrl() noexcept {
		std::string theStringNew{ "https://cdn.discordapp.com/" };
		theStringNew += "splashes/" + std::to_string(this->id) + "/" + this->icon.getIconHash() + ".png";
		return theStringNew;
	}

	ChannelVector::operator std::vector<Channel>() {
		return this->theChannels;
	}

	ModifyChannelData::ModifyChannelData(Channel newData) {
		this->channelData.nsfw = getBool<int8_t, ChannelFlags>(newData.flags, ChannelFlags::NSFW);
		this->channelData.permissionOverwrites = newData.permissionOverwrites;
		this->channelData.rateLimitPerUser = newData.rateLimitPerUser;
		this->channelData.userLimit = newData.userLimit;
		this->channelData.rtcRgion = newData.rtcRegion;
		this->channelData.parentId = newData.parentId;
		this->channelData.position = newData.position;
		this->channelData.topic = newData.topic;
		this->channelData.name = newData.name;
		this->channelData.type = newData.type;
	};

	ModifyChannelData::operator std::string() {
		nlohmann::json permOws{};
		for (auto& value: this->channelData.permissionOverwrites) {
			nlohmann::json newData{};
			newData["allow"] = value.allow;
			newData["deny"] = value.deny;
			newData["type"] = value.type;
			newData["id"] = value.id;
			permOws.emplace_back(newData);
		}
		nlohmann::json data{};
		data["default_auto_archive_duration"] = this->channelData.defaultAutoArchiveDuration;
		data["video_quality_mode"] = this->channelData.videoQualityMode;
		data["rate_limit_per_user"] = this->channelData.rateLimitPerUser;
		data["user_limit"] = this->channelData.userLimit;
		data["rtc_region"] = std::string{ this->channelData.rtcRgion };
		data["parent_id"] = std::string{ this->channelData.parentId };
		data["position"] = this->channelData.position;
		data["bitrate"] = this->channelData.bitrate;
		data["topic"] = std::string{ this->channelData.topic };
		data["nsfw"] = this->channelData.nsfw;
		data["name"] = std::string{ this->channelData.name };
		data["type"] = this->channelData.type;
		data["permission_overwrites"] = permOws;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
	}

	void Channels::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		Channels::configManager = configManagerNew;
		Channels::httpsClient = theClient;
	}

	CoRoutine<Channel> Channels::getChannelAsync(GetChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId);
		workload.callStack = "Channels::getChannelAsync()";
		Channel theData{};
		if (Channels::cache.contains(dataPackage.channelId)) {
			theData = Channels::getCachedChannelAsync({ .channelId = dataPackage.channelId }).get();
		}
		theData = Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, &theData);
		co_return theData;
	}

	CoRoutine<Channel> Channels::getCachedChannelAsync(GetChannelData dataPackage) {
		co_await NewThreadAwaitable<Channel>();
		std::shared_lock theLock{ Channels::theMutex };
		if (!Channels::cache.contains(dataPackage.channelId)) {
			theLock.unlock();
			co_return Channels::getChannelAsync(dataPackage).get();
		} else {
			co_return *Channels::cache[dataPackage.channelId];
		}
	}

	CoRoutine<Channel> Channels::modifyChannelAsync(ModifyChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId);
		workload.content = dataPackage;
		workload.callStack = "Channels::modifyChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channel theData{};
		if (Channels::cache.contains(dataPackage.channelId)) {
			theData = Channels::getCachedChannelAsync({ .channelId = dataPackage.channelId }).get();
		}
		theData = Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, &theData);
		co_return theData;
	}

	CoRoutine<void> Channels::deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Channel };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId);
		workload.callStack = "Channels::deleteOrCloseAChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> Channels::editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Channel_Permission_Overwrites };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/permissions/" + std::to_string(dataPackage.roleOrUserId);
		workload.content = dataPackage;
		workload.callStack = "Channels::editChannelPermissionOverwritesAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<InviteData>> Channels::getChannelInvitesAsync(GetChannelInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Invites };
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/invites";
		workload.callStack = "Channels::getChannelInvitesAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<InviteDataVector>(workload);
	}

	CoRoutine<InviteData> Channels::createChannelInviteAsync(CreateChannelInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Channel_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/invites";
		workload.content = dataPackage;
		workload.callStack = "Channels::createChannelInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Channels::httpsClient->submitWorkloadAndGetResult<InviteData>(workload);
	}

	CoRoutine<void> Channels::deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Channel_Permission_Overwrites };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/permissions/" + std::to_string(dataPackage.roleOrUserId);
		workload.callStack = "Channels::deleteChannelPermissionOverwritesAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<Channel> Channels::followNewsChannelAsync(FollowNewsChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Follow_News_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/followers";
		workload.content = dataPackage;
		workload.callStack = "Channels::followNewsChannelAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload);
	}

	CoRoutine<void> Channels::triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Trigger_Typing_Indicator };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/typing";
		workload.callStack = "Channels::triggerTypingIndicatorAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Channel>> Channels::getGuildChannelsAsync(GetGuildChannelsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Channels };
		co_await NewThreadAwaitable<std::vector<Channel>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/channels";
		workload.callStack = "Channels::getGuildChannelsAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<ChannelVector>(workload);
	}

	CoRoutine<Channel> Channels::createGuildChannelAsync(CreateGuildChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/channels";
		workload.content = dataPackage;
		workload.callStack = "Channels::createGuildChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload);
	}

	CoRoutine<void> Channels::modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Channel_Positions };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/channels";
		workload.content = dataPackage;
		workload.callStack = "Channels::modifyGuildChannelPositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		co_return Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<Channel> Channels::createDMChannelAsync(CreateDMChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Create_User_Dm };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/users/@me/channels";
		workload.callStack = "Channels::createDMChannelAsync()";
		nlohmann::json theValue = { { "recipient_id", dataPackage.userId } };
		workload.content = theValue.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		co_return Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload);
	}

	CoRoutine<std::vector<VoiceRegionData>> Channels::getVoiceRegionsAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Voice_Regions };
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/voice/regions";
		workload.callStack = "Channels::getVoiceRegionsAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<VoiceRegionDataVector>(workload);
	}

	void Channels::insertChannel(std::unique_ptr<ChannelData> channel) {
		if (channel->id == 0) {
			return;
		}
		if (Channels::configManager->doWeCacheChannels()) {
			auto channelId = channel->id;
			auto theResult = Channels::cache.find(channelId);
			if (theResult == Channels::cache.end()) {
				std::unique_lock theLock{ Channels::theMutex };
				Channels::cache.emplace(channelId, std::move(channel));
			} else {
				std::unique_lock theLock{ Channels::theMutex };
				Channels::cache.insert_or_assign(channelId, std::move(channel));
			}
			if (Channels::cache.size() % 1000 == 0) {
				std::cout << "CHANNEL COUNT: " << Channels::cache.size() << std::endl;
			}
		}
	}

	void Channels::removeChannel(const Snowflake channelId) {
		std::unique_lock theLock{ Channels::theMutex };
		if (Channels::cache.contains(channelId)) {
			Channels::cache.erase(channelId);
		}
	};

	std::unordered_map<Snowflake, std::unique_ptr<ChannelData>> Channels::cache{};
	DiscordCoreInternal::HttpsClient* Channels::httpsClient{ nullptr };
	ConfigManager* Channels::configManager{ nullptr };
	std::shared_mutex Channels::theMutex{};
}
