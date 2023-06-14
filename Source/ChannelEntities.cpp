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
/// ChannelEntities.cpp - Source file for the classes related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ChannelEntities.cpp

#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::EditChannelPermissionOverwritesData> {
		using ValueType = DiscordCoreAPI::EditChannelPermissionOverwritesData;
		static constexpr auto parseValue = object("allow", &ValueType::allow, "deny", &ValueType::deny, "channel_id", &ValueType::channelId, "reason",
			&ValueType::reason, "type", &ValueType::type, "id", &ValueType::roleOrUserId);
	};

	template<> struct Core<DiscordCoreAPI::CreateChannelInviteData> {
		using ValueType = DiscordCoreAPI::CreateChannelInviteData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "max_uses", &ValueType::maxUses, "max_age", &ValueType::maxAge,
			"temporary", &ValueType::temporary, "unique", &ValueType::unique, "target_type", &ValueType::targetType, "target_user_id",
			&ValueType::targetUserId, "target_application_id", &ValueType::targetApplicationId, "reason", &ValueType::reason);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildChannelData> {
		using ValueType = DiscordCoreAPI::CreateGuildChannelData;
		static constexpr auto parseValue = object("name", &ValueType::name, "type", &ValueType::type, "position", &ValueType::position, "parent_id",
			&ValueType::parentId, "permission_overwrites", &ValueType::permissionOverwrites, "nsfw", &ValueType::nsfw, "rate_limit_per_user",
			&ValueType::rateLimitPerUser, "topic", &ValueType::topic, "user_limit", &ValueType::userLimit, "bitrate", &ValueType::bitrate,
			"default_auto_archive_duration", &ValueType::defaultAutoArchiveDuration, "guild_id", &ValueType::guildId, "reason", &ValueType::reason);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildChannelPositionData> {
		using ValueType = DiscordCoreAPI::ModifyGuildChannelPositionData;
		static constexpr auto parseValue = object("id", &ValueType::id, "position", &ValueType::position, "parent_id", &ValueType::parentId,
			"lock_permissions", &ValueType::lockPermissions);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildChannelPositionsData> {
		using ValueType = DiscordCoreAPI::ModifyGuildChannelPositionsData;
		static constexpr auto parseValue = array(&ValueType::modifyChannelData);
	};

	template<> struct Core<DiscordCoreAPI::ModifyChannelData> {
		using ValueType = DiscordCoreAPI::ModifyChannelData;
		static constexpr auto parseValue =
			object("channel_id", &ValueType::channelId, "channel_data", &ValueType::channelData, "reason", &ValueType::reason);
	};

	template<> struct Core<DiscordCoreAPI::CreateDMChannelData> {
		using ValueType = DiscordCoreAPI::CreateDMChannelData;
		static constexpr auto parseValue = object("user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::FollowNewsChannelData> {
		using ValueType = DiscordCoreAPI::FollowNewsChannelData;
		static constexpr auto parseValue = object("channel_id", &ValueType::channelId, "target_channel_id", &ValueType::targetChannelId);
	};

	template<> struct Core<DiscordCoreAPI::UpdateChannelData> {
		using ValueType = DiscordCoreAPI::UpdateChannelData;
		static constexpr auto parseValue = object("permission_overwrites", &ValueType::permissionOverwrites, "default_auto_archive_duration",
			&ValueType::defaultAutoArchiveDuration, "video_quality_mode", &ValueType::videoQualityMode, "rate_limit_per_user",
			&ValueType::rateLimitPerUser, "bitrate", &ValueType::bitrate, "parent_id", &ValueType::parentId, "rtc_region", &ValueType::rtcRgion,
			"user_limit", &ValueType::userLimit, "topic", &ValueType::topic, "position", &ValueType::position, "type", &ValueType::type, "name",
			&ValueType::name, "nsfw", &ValueType::nsfw);
	};
}

namespace DiscordCoreAPI {

	Channel& Channel::operator=(ChannelData&& other) noexcept {
		if (this != &other) {
			permissionOverwrites = std::move(other.permissionOverwrites);
			memberCount = other.memberCount;
			topic = std::move(other.topic);
			name = std::move(other.name);
			parentId = other.parentId;
			position = other.position;
			guildId = other.guildId;
			ownerId = other.ownerId;
			flags = other.flags;
			type = other.type;
			id = other.id;
		}
		return *this;
	}

	Channel::Channel(ChannelData&& other) noexcept {
		*this = std::move(other);
	}

	Channel& Channel::operator=(const ChannelData& other) noexcept {
		if (this != &other) {
			permissionOverwrites = other.permissionOverwrites;
			memberCount = other.memberCount;
			parentId = other.parentId;
			position = other.position;
			guildId = other.guildId;
			ownerId = other.ownerId;
			topic = other.topic;
			flags = other.flags;
			type = other.type;
			name = other.name;
			id = other.id;
		}
		return *this;
	}

	Channel::Channel(const ChannelData& other) noexcept {
		*this = other;
	}

	std::string Channel::getIconUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "splashes/" + id + "/" + icon.operator std::string() + ".png";
		return stringNew;
	}

	ModifyChannelData::ModifyChannelData(Channel newData) {
		channelData.nsfw = getBool<ChannelFlags>(newData.flags, ChannelFlags::NSFW);
		channelData.permissionOverwrites = newData.permissionOverwrites;
		channelData.parentId = newData.parentId.operator std::string();
		channelData.rateLimitPerUser = newData.rateLimitPerUser;
		channelData.userLimit = newData.userLimit;
		channelData.rtcRgion = newData.rtcRegion;
		channelData.position = newData.position;
		channelData.topic = newData.topic;
		channelData.name = newData.name;
		channelData.type = newData.type;
	};

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
		Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, data);
		if (Channels::cache.contains(dataPackage.channelId)) {
			data = Channels::cache.find(dataPackage.channelId);
		} else {
			Channels::insertChannel(std::move(data));
		}
		co_return data;
	}

	ChannelData Channels::getCachedChannel(GetChannelData dataPackage) {
		if (!Channels::cache.contains(dataPackage.channelId)) {
			return Channels::getChannelAsync(dataPackage).get();
		} else {
			return Channels::cache.find(dataPackage.channelId);
		}
	}

	CoRoutine<Channel> Channels::modifyChannelAsync(ModifyChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::modifyChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channel data{};
		Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, data);
		if (Channels::cache.contains(dataPackage.channelId)) {
			data = Channels::cache.find(dataPackage.channelId);
		} else {
			Channels::insertChannel(std::move(data));
		}
		co_return data;
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
		Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<void> Channels::editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Channel_Permission_Overwrites };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::editChannelPermissionOverwritesAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<std::vector<InviteData>> Channels::getChannelInvitesAsync(GetChannelInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Invites };
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		workload.callStack = "Channels::getChannelInvitesAsync()";
		InviteDataVector returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult<InviteDataVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<InviteData> Channels::createChannelInviteAsync(CreateChannelInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Channel_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::createChannelInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		InviteData returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult<InviteData>(workload, returnData);
		co_return returnData;
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
		Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<Channel> Channels::followNewsChannelAsync(FollowNewsChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Follow_News_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/followers";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::followNewsChannelAsync()";
		Channel returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> Channels::triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Trigger_Typing_Indicator };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/typing";
		workload.callStack = "Channels::triggerTypingIndicatorAsync()";
		Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<std::vector<Channel>> Channels::getGuildChannelsAsync(GetGuildChannelsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Channels };
		co_await NewThreadAwaitable<std::vector<Channel>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		workload.callStack = "Channels::getGuildChannelsAsync()";
		ChannelVector returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult<ChannelVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Channel> Channels::createGuildChannelAsync(CreateGuildChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Channel };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::createGuildChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channel returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> Channels::modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Channel_Positions };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::modifyGuildChannelPositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channels::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<Channel> Channels::createDMChannelAsync(CreateDMChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Create_User_Dm };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/users/@me/channels";
		workload.callStack = "Channels::createDMChannelAsync()";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		Channel returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult<Channel>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<VoiceRegionData>> Channels::getVoiceRegionsAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Voice_Regions };
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/voice/regions";
		workload.callStack = "Channels::getVoiceRegionsAsync()";
		VoiceRegionDataVector returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult<VoiceRegionDataVector>(workload, returnData);
		co_return returnData;
	}

	ChannelData& Channels::insertChannel(ChannelData&& channel) {
		if (channel.id == 0) {
			return *Channels::cache.end();
		}
		if (Channels::doWeCacheChannelsBool) {
			auto id = channel.id;
			Channels::cache.emplace(std::forward<ChannelData>(channel));
			if (Channels::cache.count() % 10 == 0) {
				std::cout << "CHANNEL COUNT: " << Channels::cache.count() << std::endl;
			}
			return cache.find(id);
		}
		return *cache.end();
	}

	void Channels::removeChannel(Snowflake channelId) {
		Channels::cache.erase(channelId);
	};

	bool Channels::doWeCacheChannels() {
		return Channels::doWeCacheChannelsBool;
	}

	DiscordCoreInternal::HttpsClient* Channels::httpsClient{};
	ObjectCache<Snowflake, ChannelData> Channels::cache{};
	bool Channels::doWeCacheChannelsBool{};
}
