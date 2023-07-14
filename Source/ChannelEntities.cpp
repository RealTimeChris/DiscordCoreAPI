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

	ChannelData::ChannelData(Snowflake newId) {
		id = newId;
	}

	ChannelCacheData& ChannelCacheData::operator=(const ChannelData& other) {
		if (other.permissionOverwrites.size() > 0) {
			permissionOverwrites = other.permissionOverwrites;
		}
		setFlagValue(ChannelFlags::Managed, other.managed);
		setFlagValue(ChannelFlags::NSFW, other.nsfw);
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		if (other.memberCount != 0) {
			memberCount = other.memberCount;
		}
		if (other.parentId != 0) {
			parentId = other.parentId;
		}
		if (other.position != 0) {
			position = other.position;
		}
		if (other.guildId != 0) {
			guildId = other.guildId;
		}
		if (other.ownerId != 0) {
			ownerId = other.ownerId;
		}
		if (other.topic != "") {
			topic = other.topic;
		}
		if (other.name != "") {
			name = other.name;
		}
		if (other.id != 0) {
			id = other.id;
		}
		type = other.type;
		return *this;
	}

	ChannelCacheData::ChannelCacheData(const ChannelData& other) {
		*this = other;
	}

	ChannelCacheData& ChannelCacheData::operator=(ChannelData&& other) {
		if (other.permissionOverwrites.size() > 0) {
			permissionOverwrites = std::move(other.permissionOverwrites);
		}
		setFlagValue(ChannelFlags::Managed, other.managed);
		setFlagValue(ChannelFlags::NSFW, other.nsfw);
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		if (other.memberCount != 0) {
			memberCount = other.memberCount;
		}
		if (other.topic != "") {
			topic = std::move(other.topic);
		}
		if (other.name != "") {
			name = std::move(other.name);
		}
		if (other.parentId != 0) {
			parentId = other.parentId;
		}
		if (other.position != 0) {
			position = other.position;
		}
		if (other.guildId != 0) {
			guildId = other.guildId;
		}
		if (other.ownerId != 0) {
			ownerId = other.ownerId;
		}
		if (other.id != 0) {
			id = other.id;
		}
		type = other.type;
		return *this;
	}

	ChannelCacheData::operator DiscordCoreAPI::ChannelData() {
		ChannelData returnData{};
		returnData.managed = getFlagValue(ChannelFlags::Managed);
		returnData.permissionOverwrites = permissionOverwrites;
		returnData.nsfw = getFlagValue(ChannelFlags::NSFW);
		returnData.topic = topic.operator std::string();
		returnData.name = name.operator std::string();
		returnData.memberCount = memberCount;
		returnData.parentId = parentId;
		returnData.position = position;
		returnData.guildId = guildId;
		returnData.ownerId = ownerId;
		returnData.flags = flags;
		returnData.type = type;
		returnData.id = id;
		return returnData;
	}

	ChannelCacheData::ChannelCacheData(ChannelData&& other) {
		*this = std::move(other);
	}

	std::string ChannelData::getIconUrl() {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "splashes/" + id.operator std::string() + "/" + icon + ".png";
		return stringNew;
	}

	ModifyChannelData::ModifyChannelData(ChannelData newData) {
		channelData.permissionOverwrites = newData.permissionOverwrites;
		channelData.nsfw = newData.getFlagValue(ChannelFlags::NSFW);
		channelData.rateLimitPerUser = newData.rateLimitPerUser;
		channelData.userLimit = newData.userLimit;
		channelData.rtcRgion = newData.rtcRegion;
		channelData.position = newData.position;
		channelData.parentId = newData.parentId;
		channelData.topic = newData.topic;
		channelData.name = newData.name;
		channelData.type = newData.type;
	};

	void Channels::initialize(DiscordCoreInternal::HttpsClient* client, ConfigManager* configManagerNew) {
		Channels::doWeCacheChannelsBool = configManagerNew->doWeCacheChannels();
		Channels::httpsClient = client;
	}

	CoRoutine<ChannelData> Channels::getChannelAsync(GetChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel };
		co_await NewThreadAwaitable<ChannelData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		workload.callStack = "Channels::getChannelAsync()";
		ChannelData data{ dataPackage.channelId };
		if (cache.contains(dataPackage.channelId)) {
			data = cache[dataPackage.channelId];
		}
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheChannelsBool) {
			insertChannel(data);
		}
		co_return data;
	}

	ChannelCacheData Channels::getCachedChannel(GetChannelData dataPackage) {
		if (!Channels::cache.contains(dataPackage.channelId)) {
			return Channels::getChannelAsync(dataPackage).get();
		} else {
			return cache[dataPackage.channelId];
		}
	}

	CoRoutine<ChannelData> Channels::modifyChannelAsync(ModifyChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Channel };
		co_await NewThreadAwaitable<ChannelData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::modifyChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		ChannelData data{ dataPackage.channelId };
		if (cache.contains(dataPackage.channelId)) {
			data = cache[dataPackage.channelId];
		}
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheChannelsBool) {
			insertChannel(data);
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
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> Channels::editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Channel_Permission_Overwrites };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::editChannelPermissionOverwritesAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<std::vector<InviteData>> Channels::getChannelInvitesAsync(GetChannelInvitesData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Invites };
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		workload.callStack = "Channels::getChannelInvitesAsync()";
		std::vector<InviteData> returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<InviteData> Channels::createChannelInviteAsync(CreateChannelInviteData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Channel_Invite };
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::createChannelInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		InviteData returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
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
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<ChannelData> Channels::followNewsChannelAsync(FollowNewsChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Follow_News_Channel };
		co_await NewThreadAwaitable<ChannelData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/followers";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::followNewsChannelAsync()";
		ChannelData returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Channels::triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Trigger_Typing_Indicator };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/typing";
		workload.callStack = "Channels::triggerTypingIndicatorAsync()";
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<std::vector<ChannelData>> Channels::getGuildChannelsAsync(GetGuildChannelsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Channels };
		co_await NewThreadAwaitable<std::vector<ChannelData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		workload.callStack = "Channels::getGuildChannelsAsync()";
		std::vector<ChannelData> returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ChannelData> Channels::createGuildChannelAsync(CreateGuildChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Channel };
		co_await NewThreadAwaitable<ChannelData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::createGuildChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		ChannelData returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> Channels::modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Channel_Positions };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "Channels::modifyGuildChannelPositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["X-Audit-Log-Reason"] = dataPackage.reason;
		}
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<ChannelData> Channels::createDMChannelAsync(CreateDMChannelData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Create_User_Dm };
		co_await NewThreadAwaitable<ChannelData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/users/@me/channels";
		workload.callStack = "Channels::createDMChannelAsync()";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		ChannelData returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<VoiceRegionData>> Channels::getVoiceRegionsAsync() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Voice_Regions };
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/voice/regions";
		workload.callStack = "Channels::getVoiceRegionsAsync()";
		std::vector<VoiceRegionData> returnData{};
		Channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	void Channels::insertChannel(const ChannelData& channel) {
		if (doWeCacheChannelsBool) {
			if (channel.id == 0) {
				throw DCAException{ "Sorry, but there was no id set for that channel.", std::source_location::current() };
			}
			cache.emplace(channel);
			if (Channels::cache.count() % 1000 == 0) {
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

	DiscordCoreInternal::HttpsClient* Channels::httpsClient{};

	ObjectCache<Snowflake, ChannelCacheData> Channels::cache{};
	bool Channels::doWeCacheChannelsBool{};
}
