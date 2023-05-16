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
		using OTy2 = DiscordCoreAPI::EditChannelPermissionOverwritesData;
		static constexpr auto parseValue = object("allow", &OTy2::allow, "deny", &OTy2::deny, "channel_id", &OTy2::channelId, "reason", &OTy2::reason,
			"type", &OTy2::type, "id", &OTy2::roleOrUserId);
	};

	template<> struct Core<DiscordCoreAPI::CreateChannelInviteData> {
		using OTy2 = DiscordCoreAPI::CreateChannelInviteData;
		static constexpr auto parseValue = object("channel_id", &OTy2::channelId, "max_uses", &OTy2::maxUses, "max_age", &OTy2::maxAge, "temporary",
			&OTy2::temporary, "unique", &OTy2::unique, "target_type", &OTy2::targetType, "target_user_id", &OTy2::targetUserId,
			"target_application_id", &OTy2::targetApplicationId, "reason", &OTy2::reason);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildChannelData> {
		using OTy2 = DiscordCoreAPI::CreateGuildChannelData;
		static constexpr auto parseValue = object("name", &OTy2::name, "type", &OTy2::type, "position", &OTy2::position, "parent_id", &OTy2::parentId,
			"permission_overwrites", &OTy2::permissionOverwrites, "nsfw", &OTy2::nsfw, "rate_limit_per_user", &OTy2::rateLimitPerUser, "topic",
			&OTy2::topic, "user_limit", &OTy2::userLimit, "bitrate", &OTy2::bitrate, "default_auto_archive_duration",
			&OTy2::defaultAutoArchiveDuration, "guild_id", &OTy2::guildId, "reason", &OTy2::reason);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildChannelPositionData> {
		using OTy2 = DiscordCoreAPI::ModifyGuildChannelPositionData;
		static constexpr auto parseValue =
			object("id", &OTy2::id, "position", &OTy2::position, "parent_id", &OTy2::parentId, "lock_permissions", &OTy2::lockPermissions);
	};

	template<> struct Core<DiscordCoreAPI::ModifyGuildChannelPositionsData> {
		using OTy2 = DiscordCoreAPI::ModifyGuildChannelPositionsData;
		static constexpr auto parseValue = array(&OTy2::modifyChannelData);
	};

	template<> struct Core<DiscordCoreAPI::ModifyChannelData> {
		using OTy2 = DiscordCoreAPI::ModifyChannelData;
		static constexpr auto parseValue = object("channel_id", &OTy2::channelId, "channel_data", &OTy2::channelData, "reason", &OTy2::reason);
	};

	template<> struct Core<DiscordCoreAPI::FollowNewsChannelData> {
		using OTy2 = DiscordCoreAPI::FollowNewsChannelData;
		static constexpr auto parseValue = object("channel_id", &OTy2::channelId, "target_channel_id", &OTy2::targetChannelId);
	};

	template<> struct Core<DiscordCoreAPI::UpdateChannelData> {
		using OTy2 = DiscordCoreAPI::UpdateChannelData;
		static constexpr auto parseValue = object("permission_overwrites", &OTy2::permissionOverwrites, "default_auto_archive_duration",
			&OTy2::defaultAutoArchiveDuration, "video_quality_mode", &OTy2::videoQualityMode, "rate_limit_per_user", &OTy2::rateLimitPerUser,
			"bitrate", &OTy2::bitrate, "parent_id", &OTy2::parentId, "rtc_region", &OTy2::rtcRgion, "user_limit", &OTy2::userLimit, "topic",
			&OTy2::topic, "position", &OTy2::position, "type", &OTy2::type, "name", &OTy2::name, "nsfw", &OTy2::nsfw);
	};
}

namespace DiscordCoreAPI {

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
		stringNew += "splashes/" + this->id + "/" + this->icon.operator std::string() + ".png";
		return stringNew;
	}

	ModifyChannelData::ModifyChannelData(Channel newData) {
		this->channelData.nsfw = getBool<ChannelFlags>(newData.flags, ChannelFlags::NSFW);
		this->channelData.permissionOverwrites = newData.permissionOverwrites;
		this->channelData.parentId = newData.parentId.operator std::string();
		this->channelData.rateLimitPerUser = newData.rateLimitPerUser;
		this->channelData.userLimit = newData.userLimit;
		this->channelData.rtcRgion = newData.rtcRegion;
		this->channelData.position = newData.position;
		this->channelData.topic = newData.topic;
		this->channelData.name = newData.name;
		this->channelData.type = newData.type;
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
		return {};
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
		Channels::httpsClient->submitWorkloadAndGetResult(workload);
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
		Channels::httpsClient->submitWorkloadAndGetResult(workload);
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
		Channels::httpsClient->submitWorkloadAndGetResult(workload);
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
		Channels::httpsClient->submitWorkloadAndGetResult(workload);
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
		Channels::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	CoRoutine<Channel> Channels::createDMChannelAsync(CreateDMChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Create_User_Dm };
		co_await NewThreadAwaitable<Channel>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/users/@me/channels";
		workload.callStack = "Channels::createDMChannelAsync()";
		EtfSerializer value{};
		value["recipient_id"] = dataPackage.userId.operator std::string();
		value.refreshString(SerializerSerializeType::Etf);
		workload.content = value.operator std::string();
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

	void Channels::insertChannel(const ChannelData& channel) {
		if (channel.id == 0) {
			return;
		}
		if (Channels::doWeCacheChannelsBool) {
			ChannelData newChannel{ channel };
			Channels::cache.emplace(std::move(newChannel));
			if (Channels::cache.count() % 10 == 0) {
				std::cout << "CHANNEL COUNT: " << Channels::cache.count() << std::endl;
			}
		}
	}

	void Channels::insertChannel(ChannelData&& channel) {
		if (channel.id == 0) {
			return;
		}
		if (Channels::doWeCacheChannelsBool) {
			Channels::cache.emplace(std::forward<ChannelData>(channel));
			if (Channels::cache.count() % 10 == 0) {
				std::cout << "CHANNEL COUNT: " << Channels::cache.count() << std::endl;
			}
		}
	}

	void Channels::removeChannel(Snowflake channelId) {
		Channels::cache.erase(channelId);
	};

	bool Channels::doWeCacheChannels() {
		return Channels::doWeCacheChannelsBool;
	}

	DiscordCoreInternal::HttpsClient* Channels::httpsClient{ nullptr };
	ObjectCache<Snowflake, ChannelData> Channels::cache{};
	bool Channels::doWeCacheChannelsBool{};
}
