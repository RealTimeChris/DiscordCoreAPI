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
		this->totalMessageSent = getUint32(jsonObjectData, "tital_message_sent");

		uint8_t newFlags{}; 
		newFlags = getUint8(jsonObjectData, "flags");

		this->type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->defaultSortOrder = static_cast<SortOrderTypes>(getUint8(jsonObjectData, "default_sort_order"));

		this->defaultAutoArchiveDuration = getUint32(jsonObjectData, "default_auto_archive_duration");

		this->position = getUint32(jsonObjectData, "position");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "available_tags", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ForumTagData newData{ value.value() };
				this->availableTags.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "permission_overwrites", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				OverWriteData dataNew{ value.value() };
				this->permissionOverwrites.emplace_back(std::move(dataNew));
			}
		}

		if (getArray(arrayValue, "applied_tags", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->appliedTags.emplace_back(strtoull(value.value().get_string().take_value()));
			}
		}

		this->name = getString(jsonObjectData, "name");

		this->topic = getString(jsonObjectData, "topic");

		this->permissions = getString(jsonObjectData, "permissions");

		newFlags = setBool(newFlags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		this->lastMessageId = getString(jsonObjectData, "last_message_id");

		this->bitrate = getUint32(jsonObjectData, "bitrate");

		this->userLimit = getUint32(jsonObjectData, "user_limit");

		this->rateLimitPerUser = getUint32(jsonObjectData, "rate_limit_per_user");

		if (getArray(arrayValue, "recipients", jsonObjectData)) {
			this->recipients.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				UserData dataNew{ value.value() };
				this->recipients.emplace(dataNew.id, std::move(dataNew));
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

		simdjson::ondemand::value object{};
		if (getObject(object, "thread_metadata", jsonObjectData)) {
			this->threadMetadata = ThreadMetadataData{ object };
		}

		if (getObject(object, "default_reaction_emoji", jsonObjectData)) {
			this->defaultReactionEmoji = DefaultReactionData{ object };
		}

		if (getObject(object, "member", jsonObjectData)) {
			this->member = ThreadMemberData{ object };
		}

		this->flags = static_cast<ChannelFlags>(newFlags);
	}

	ChannelVector::ChannelVector(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				Channel newData{ value.value() };
				this->channels.emplace_back(std::move(newData));
			}
		}
	}

	EditChannelPermissionOverwritesData::operator Jsonifier() {
		Jsonifier data{};
		data["allow"] = this->allow;
		data["deny"] = this->deny;
		data["type"] = this->type;
		return data;
	}

	CreateChannelInviteData::operator Jsonifier() {
		Jsonifier data{};
		if (this->targetUserId != 0) {
			data["target_application_id"] = this->targetApplicationId;
			data["target_user_id"] = this->targetUserId;
			data["target_type"] = this->targetType;
		}
		data["temporary"] = this->temporary;
		data["max_uses"] = this->maxUses;
		data["max_age"] = this->maxAge;
		data["unique"] = this->unique;
		return data;
	}

	FollowNewsChannelData::operator Jsonifier() {
		Jsonifier data{};
		data["webhook_channel_id"] = this->targetChannelId;
		return data;
	}

	CreateGuildChannelData::operator Jsonifier() {
		Jsonifier data{};
		if (this->type == DiscordCoreAPI::ChannelType::Guild_Voice || this->type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			data["user_limit"] = this->userLimit;
			data["bitrate"] = this->bitrate;
		}
		for (auto& value: this->permissionOverwrites) {
			Jsonifier newData{};
			data["allow"] = value.allow;
			data["deny"] = value.deny;
			data["type"] = value.type;
			data["id"] = value.id;
			data["permission_overwrites"].emplaceBack(newData);
		}
		data["default_auto_archive_duration"] = this->defaultAutoArchiveDuration;
		data["rate_limit_per_user"] = this->rateLimitPerUser;
		data["parent_id"] = this->parentId;
		data["position"] = this->position;
		data["topic"] = this->topic;
		data["name"] = this->name;
		data["nsfw"] = this->nsfw;
		data["type"] = this->type;
		return data;
	}

	ModifyGuildChannelPositionsData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->modifyChannelData) {
			Jsonifier dataNew{};
			dataNew["lock_permissions"] = value.lockPermissions;
			if (value.parentId != 0) {
				dataNew["parent_id"] = value.parentId;
			}
			dataNew["position"] = value.position;
			dataNew["id"] = value.id;
			data.emplaceBack(dataNew);
		}
		return data;
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
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "splashes/" + this->id + "/" + this->icon.getIconHash() + ".png";
		return stringNew;
	}

	ChannelVector::operator std::vector<Channel>() {
		return this->channels;
	}

	ModifyChannelData::ModifyChannelData(Channel newData) {
		this->channelData.nsfw = getBool<ChannelFlags>(newData.flags, ChannelFlags::NSFW);
		this->channelData.permissionOverwrites = newData.permissionOverwrites;
		this->channelData.parentId = newData.parentId;
		this->channelData.rateLimitPerUser = newData.rateLimitPerUser;
		this->channelData.userLimit = newData.userLimit;
		this->channelData.rtcRgion = newData.rtcRegion;
		this->channelData.position = newData.position;
		this->channelData.topic = newData.topic;
		this->channelData.name = newData.name;
		this->channelData.type = newData.type;
	};

	ModifyChannelData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->channelData.permissionOverwrites) {
			Jsonifier newData{};
			newData["allow"] = value.allow;
			newData["deny"] = value.deny;
			newData["type"] = value.type;
			newData["id"] = value.id;
			data["permission_overwrites"].emplaceBack(newData);
		}
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
		return data;
	}

	void Channels::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		Channels::doWeCacheChannelsBool = configManagerNew->doWeCacheChannels();
		Channels::httpsClient = client;
	}

	CoRoutine<Channel> Channels::getChannelAsync(GetChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		workload.callStack = "Channels::getChannelAsync()";
		Channel data{};
		data = Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, &data);
		if (Channels::cache.contains(data)) {
			data = Channels::cache.at(data);
		} else {
			Channels::insertChannel(data);
		}
		co_return std::move(data);
	}

	ChannelData Channels::getCachedChannel(GetChannelData dataPackage) {
		ChannelData channel{};
		channel.id = dataPackage.channelId;
		if (!Channels::cache.contains(channel)) {
			return Channels::getChannelAsync(dataPackage).get();
		} else {
			channel = Channels::cache.at(channel);
			return channel;
		}
	}

	CoRoutine<Channel> Channels::modifyChannelAsync(ModifyChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Channels::modifyChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channel data{};
		data = Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, &data);
		if (Channels::cache.contains(data)) {
			data = Channels::cache.at(data);
		} else {
			Channels::insertChannel(data);
		}
		co_return std::move(data);
	}

	CoRoutine<void> Channels::deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Channel };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId;
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
		workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
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
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		workload.callStack = "Channels::getChannelInvitesAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<InviteDataVector>(workload);
	}

	CoRoutine<InviteData> Channels::createChannelInviteAsync(CreateChannelInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Channel_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
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
		workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
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
		workload.relativePath = "/channels/" + dataPackage.channelId + "/followers";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "Channels::followNewsChannelAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload);
	}

	CoRoutine<void> Channels::triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Trigger_Typing_Indicator };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/typing";
		workload.callStack = "Channels::triggerTypingIndicatorAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<Channel>> Channels::getGuildChannelsAsync(GetGuildChannelsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Channels };
		co_await NewThreadAwaitable<std::vector<Channel>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		workload.callStack = "Channels::getGuildChannelsAsync()";
		co_return Channels::httpsClient->submitWorkloadAndGetResult<ChannelVector>(workload);
	}

	CoRoutine<Channel> Channels::createGuildChannelAsync(CreateGuildChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
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
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
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
		Jsonifier value{};
		value["recipient_id"] = dataPackage.userId;
		value.refreshString(JsonifierSerializeType::Json);
		workload.content = value.operator std::string();
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
		if (channel.id == 0) {
			return;
		}
		if (Channels::doWeCacheChannelsBool) {
			if (!Channels::cache.contains(channel)) {
				Channels::cache.emplace(std::move(channel));
			} else {
				Channels::cache[channel] = std::move(channel);
			}
			if (Channels::cache.size() % 10000 == 0) {
				//std::cout << "CHANNEL COUNT: " << Channels::cache.size() << std::endl;
			}
		}
	}

	void Channels::removeChannel(Snowflake channelId) {
		ChannelData data{};
		data.id = channelId;
		Channels::cache.erase(data);
	};

	bool Channels::doWeCacheChannels() {
		return Channels::doWeCacheChannelsBool;
	}

	DiscordCoreInternal::HttpsClient* Channels::httpsClient{ nullptr };
	ObjectCache<ChannelData> Channels::cache{};
	bool Channels::doWeCacheChannelsBool{ false };
}
