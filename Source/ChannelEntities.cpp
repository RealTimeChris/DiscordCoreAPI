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

	Channel::Channel(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->flags = getUint8(jsonObjectData, "flags");

		this->type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->defaultAutoArchiveDuration = getUint32(jsonObjectData, "default_auto_archive_duration");

		this->position = getUint32(jsonObjectData, "position");

		simdjson::ondemand::value theArray{};
		auto theResult = jsonObjectData["permission_overwrites"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->permissionOverwrites.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				OverWriteData theDataNew{ value.value() };
				this->permissionOverwrites.emplace_back(std::move(theDataNew));
			}
		}

		this->name = getString(jsonObjectData, "name");

		this->topic = getString(jsonObjectData, "topic");

		this->permissions = getString(jsonObjectData, "permissions");

		this->flags = setBool(this->flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		this->lastMessageId = getString(jsonObjectData, "last_message_id");

		this->bitrate = getUint32(jsonObjectData, "bitrate");

		this->userLimit = getUint32(jsonObjectData, "user_limit");

		this->rateLimitPerUser = getUint32(jsonObjectData, "rate_limit_per_user");

		theResult = jsonObjectData["recipients"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->recipients.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				UserData theDataNew{ value.value() };
				this->recipients.emplace(theDataNew.id, std::move(theDataNew));
			}
		}

		this->icon = getString(jsonObjectData, "icon");

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->applicationId = getId(jsonObjectData, "application_id");

		this->parentId = getId(jsonObjectData, "parent_id");

		this->lastPinTimestamp = getString(jsonObjectData, "last_pin_timestamp");

		this->rtcRegion = getString(jsonObjectData, "rtc_region");

		this->videoQualityMode = getUint32(jsonObjectData, "video_quality_mode");

		this->messageCount = getUint32(jsonObjectData, "message_count");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		simdjson::ondemand::value theObject{};
		theResult = jsonObjectData["thread_metadata"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->threadMetadata = ThreadMetadataData{ theObject };
		}

		theResult = jsonObjectData["member"].get(theObject);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->member = ThreadMemberData{ theObject };
		}
	}

	ChannelVector::ChannelVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				this->theChannels.reserve(theArray.count_elements().take_value());
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					Channel newData{ value.value() };
					this->theChannels.push_back(std::move(newData));
				}
				this->theChannels.shrink_to_fit();
			}
		}
	}

	EditChannelPermissionOverwritesData::operator JsonObject() {
		JsonObject theData{};
		theData["allow"] = this->allow;
		theData["deny"] = this->deny;
		theData["type"] = this->type;
		return theData;
	}

	CreateChannelInviteData::operator JsonObject() {
		JsonObject theData{};
		if (this->targetUserId.operator size_t() != 0) {
			theData["target_application_id"] = std::to_string(this->targetApplicationId);
			theData["target_user_id"] = std::to_string(this->targetUserId);
			theData["target_type"] = this->targetType;
		}
		theData["temporary"] = this->temporary;
		theData["max_uses"] = this->maxUses;
		theData["max_age"] = this->maxAge;
		theData["unique"] = this->unique;
		return theData;
	}

	FollowNewsChannelData::operator JsonObject() {
		JsonObject theData{};
		theData["webhook_channel_id"] = std::to_string(this->targetChannelId);
		return theData;
	}

	CreateGuildChannelData::operator JsonObject() {
		JsonObject theData{};
		if (this->type == DiscordCoreAPI::ChannelType::Guild_Voice || this->type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			theData["user_limit"] = this->userLimit;
			theData["bitrate"] = this->bitrate;
		}
		for (auto& value: this->permissionOverwrites) {
			JsonObject newData{};
			theData["allow"] = value.allow;
			theData["deny"] = value.deny;
			theData["type"] = value.type;
			theData["id"] = std::to_string(value.id);
			theData.pushBack("permission_overwrites", newData);
		}
		theData["default_auto_archive_duration"] = this->defaultAutoArchiveDuration;
		theData["rate_limit_per_user"] = this->rateLimitPerUser;
		theData["parent_id"] = std::to_string(this->parentId);
		theData["position"] = this->position;
		theData["topic"] = this->topic;
		theData["name"] = this->name;
		theData["nsfw"] = this->nsfw;
		theData["type"] = this->type;
		return theData;
	}

	ModifyGuildChannelPositionsData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->modifyChannelData) {
			JsonObject dataNew{};
			dataNew["lock_permissions"] = value.lockPermissions;
			if (value.parentId.operator size_t() != 0) {
				dataNew["parent_id"] = std::to_string(value.parentId);
			}
			dataNew["position"] = value.position;
			dataNew["id"] = std::to_string(value.id);
			theData.pushBack("", dataNew);
		}
		return theData;
	}

	Channel& Channel::operator=(ChannelData&& other) noexcept {
		if (this != &other) {
			this->permissionOverwrites = std::move(other.permissionOverwrites);
			this->memberCount = other.memberCount;
			this->topic = std::move(other.topic);
			this->name = std::move(other.name);
			this->parentId = other.parentId;
			this->position = other.position;
			this->guildId = other.guildId;
			this->ownerId = other.ownerId;
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
		this->channelData.parentId = std::to_string(newData.parentId);
		this->channelData.rateLimitPerUser = newData.rateLimitPerUser;
		this->channelData.userLimit = newData.userLimit;
		this->channelData.rtcRgion = newData.rtcRegion;
		this->channelData.position = newData.position;
		this->channelData.topic = newData.topic;
		this->channelData.name = newData.name;
		this->channelData.type = newData.type;
	};

	ModifyChannelData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->channelData.permissionOverwrites) {
			JsonObject newData{};
			newData["allow"] = value.allow;
			newData["deny"] = value.deny;
			newData["type"] = value.type;
			newData["id"] = std::to_string(value.id);
			theData.pushBack("permission_overwrites", newData);
		}
		theData["default_auto_archive_duration"] = this->channelData.defaultAutoArchiveDuration;
		theData["video_quality_mode"] = this->channelData.videoQualityMode;
		theData["rate_limit_per_user"] = this->channelData.rateLimitPerUser;
		theData["user_limit"] = this->channelData.userLimit;
		theData["rtc_region"] = std::string{ this->channelData.rtcRgion };
		theData["parent_id"] = std::string{ this->channelData.parentId };
		theData["position"] = this->channelData.position;
		theData["bitrate"] = this->channelData.bitrate;
		theData["topic"] = std::string{ this->channelData.topic };
		theData["nsfw"] = this->channelData.nsfw;
		theData["name"] = std::string{ this->channelData.name };
		theData["type"] = this->channelData.type;
		return theData;
	}

	void Channels::initialize(DiscordCoreInternal::HttpsClient* theClient, ConfigManager* configManagerNew) {
		Channels::doWeCacheChannels = configManagerNew->doWeCacheChannels();
		Channels::httpsClient = theClient;
	}

	CoRoutine<Channel> Channels::getChannelAsync(GetChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId);
		workload.callStack = "Channels::getChannelAsync()";
		Channel theData{};
		theData.id = dataPackage.channelId;
		if (!Channels::cache.contains(( ChannelData& )theData)) {
			Channels::cache.emplace(( ChannelData& )theData);
		}
		theData = Channels::cache.at(theData);
		theData = Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, &theData);
		Channels::insertChannel(theData);
		co_return theData;
	}

	CoRoutine<ChannelData> Channels::getCachedChannelAsync(GetChannelData dataPackage) {
		co_await NewThreadAwaitable<ChannelData>();
		ChannelData theChannel{};
		theChannel.id = dataPackage.channelId;
		if (!Channels::cache.contains(theChannel)) {
			co_return Channels::getChannelAsync(dataPackage).get();
		} else {
			theChannel = Channels::cache.at(theChannel);
			co_return theChannel;
		}
	}

	CoRoutine<Channel> Channels::modifyChannelAsync(ModifyChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId);
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		workload.callStack = "Channels::modifyChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channel theData{};
		theData.id = dataPackage.channelId;
		if (!Channels::cache.contains(theData)) {
			Channels::cache.emplace(theData);
		}
		theData = Channels::cache.at(theData);
		theData = Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, &theData);
		Channels::insertChannel(theData);
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
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
		JsonObject theValue{};
		theValue["recipient_id"] = std::to_string(dataPackage.userId);
		workload.content = theValue;
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

	void Channels::insertChannel(ChannelData channel) {
		if (channel.id.operator size_t() == 0) {
			return;
		}
		if (Channels::doWeCacheChannels) {
			Channels::cache.emplace(std::move(channel));
			if (Channels::cache.size() % 1000 == 0) {
				std::cout << "CHANNEL COUNT: " << Channels::cache.size() << std::endl;
			}
		}
	}

	void Channels::removeChannel(Snowflake channelId) {
		ChannelData theData{};
		theData.id = channelId;
		Channels::cache.erase(theData);
	};

	DiscordCoreInternal::HttpsClient* Channels::httpsClient{ nullptr };
	ObjectCache<ChannelData> Channels::cache{};
	bool Channels::doWeCacheChannels{ false };
}
