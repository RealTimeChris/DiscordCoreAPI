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
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::edit_channel_permission_overwrites_data> {
		using value_type				 = discord_core_api::edit_channel_permission_overwrites_data;
		static constexpr auto parseValue = createValue("allow", &value_type::allow, "deny", &value_type::deny, "channel_id", &value_type::channelId,
			"type", &value_type::type, "id", &value_type::roleOrUserId);
	};

	template<> struct core<discord_core_api::create_channel_invite_data> {
		using value_type				 = discord_core_api::create_channel_invite_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "max_uses", &value_type::maxUses, "max_age", &value_type::maxAge, "temporary",
			&value_type::temporary, "unique", &value_type::unique, "target_type", &value_type::targetType, "target_user_id", &value_type::targetUserId, "target_application_id",
			&value_type::targetApplicationId);
	};

	template<> struct core<discord_core_api::create_guild_channel_data> {
		using value_type				 = discord_core_api::create_guild_channel_data;
		static constexpr auto parseValue = createValue("name", &value_type::name, "type", &value_type::type, "position", &value_type::position, "parent_id", &value_type::parentId,
			"permission_overwrites", &value_type::permissionOverwrites, "nsfw", &value_type::nsfw, "rate_limit_per_user", &value_type::rateLimitPerUser, "topic",
			&value_type::topic, "user_limit", &value_type::userLimit, "bitrate", &value_type::bitrate, "default_auto_archive_duration", &value_type::defaultAutoArchiveDuration,
			"guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::modify_guild_channel_position_data> {
		using value_type = discord_core_api::modify_guild_channel_position_data;
		static constexpr auto parseValue =
			createValue("id", &value_type::id, "position", &value_type::position, "parent_id", &value_type::parentId, "lock_permissions", &value_type::lockPermissions);
	};

	template<> struct core<discord_core_api::modify_guild_channel_positions_data> {
		using value_type				 = discord_core_api::modify_guild_channel_positions_data;
		static constexpr auto parseValue = createValue(&value_type::modifyChannelData);
	};

	template<> struct core<discord_core_api::modify_channel_data> {
		using value_type				 = discord_core_api::modify_channel_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "channel_data", &value_type::channelData);
	};

	template<> struct core<discord_core_api::create_dmchannel_data> {
		using value_type				 = discord_core_api::create_dmchannel_data;
		static constexpr auto parseValue = createValue("user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::follow_news_channel_data> {
		using value_type				 = discord_core_api::follow_news_channel_data;
		static constexpr auto parseValue = createValue("channel_id", &value_type::channelId, "target_channel_id", &value_type::targetChannelId);
	};

	template<> struct core<discord_core_api::update_channel_data> {
		using value_type				 = discord_core_api::update_channel_data;
		static constexpr auto parseValue = createValue("permission_overwrites", &value_type::permissionOverwrites, "default_auto_archive_duration",
			&value_type::defaultAutoArchiveDuration, "video_quality_mode", &value_type::videoQualityMode, "rate_limit_per_user", &value_type::rateLimitPerUser, "bitrate",
			&value_type::bitrate, "parent_id", &value_type::parentId, "rtc_region", &value_type::rtcRgion, "user_limit", &value_type::userLimit, "topic", &value_type::topic,
			"position", &value_type::position, "type", &value_type::type, "name", &value_type::name, "nsfw", &value_type::nsfw);
	};
}

namespace discord_core_api {

	channel_data::channel_data(snowflake newId) {
		id = newId;
	}

	channel_cache_data& channel_cache_data::operator=(const channel_data& other) {
		if (other.permissionOverwrites.size() > 0) {
			permissionOverwrites = other.permissionOverwrites;
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(channel_flags::managed, other.managed);
		setFlagValue(channel_flags::nsfw, other.nsfw);
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

	channel_cache_data::channel_cache_data(const channel_data& other) {
		*this = other;
	}

	channel_cache_data& channel_cache_data::operator=(channel_data&& other) noexcept {
		if (other.permissionOverwrites.size() > 0) {
			permissionOverwrites = std::move(other.permissionOverwrites);
		}
		if (static_cast<int64_t>(other.flags) != 0) {
			flags = other.flags;
		}
		setFlagValue(channel_flags::managed, other.managed);
		setFlagValue(channel_flags::nsfw, other.nsfw);
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

	channel_cache_data::operator channel_data() {
		channel_data returnData{};
		returnData.managed				= getFlagValue(channel_flags::managed);
		returnData.permissionOverwrites = permissionOverwrites;
		returnData.nsfw					= getFlagValue(channel_flags::nsfw);
		returnData.topic				= topic;
		returnData.name					= name;
		returnData.memberCount			= memberCount;
		returnData.parentId				= parentId;
		returnData.position				= position;
		returnData.guildId				= guildId;
		returnData.ownerId				= ownerId;
		returnData.flags				= flags;
		returnData.type					= type;
		returnData.id					= id;
		return returnData;
	}

	channel_cache_data::channel_cache_data(channel_data&& other) noexcept {
		*this = std::move(other);
	}

	jsonifier::string channel_data::getIconUrl() {
		jsonifier::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "splashes/" + id.operator jsonifier::string() + "/" + icon + ".png";
		return stringNew;
	}

	modify_channel_data::modify_channel_data(channel_data newData) {
		channelData.parentId			 = newData.parentId.operator jsonifier::string();
		channelData.nsfw				 = newData.getFlagValue(channel_flags::nsfw);
		channelData.permissionOverwrites = newData.permissionOverwrites;
		channelData.rateLimitPerUser	 = newData.rateLimitPerUser;
		channelData.userLimit			 = newData.userLimit;
		channelData.rtcRgion			 = newData.rtcRegion;
		channelData.position			 = newData.position;
		channelData.topic				 = newData.topic;
		channelData.name				 = newData.name;
		channelData.type				 = newData.type;
	};

	void channels::initialize(discord_core_internal::https_client* client, config_manager* configManagerNew) {
		channels::doWeCacheChannelsBool = configManagerNew->doWeCacheChannels();
		channels::httpsClient			= client;
	}

	co_routine<channel_data> channels::getChannelAsync(get_channel_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Channel };
		co_await newThreadAwaitable<channel_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId;
		workload.callStack	   = "channels::getChannelAsync()";
		channel_data data{ dataPackage.channelId };
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheChannelsBool) {
			insertChannel(static_cast<channel_cache_data>(data));
		}
		co_return data;
	}

	channel_cache_data channels::getCachedChannel(get_channel_data dataPackage) {
		if (channels::cache.contains(dataPackage.channelId)) {
			return cache[dataPackage.channelId];
		} else {
			return getChannelAsync(dataPackage).get();
		}
	}

	co_routine<channel_data> channels::modifyChannelAsync(modify_channel_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Channel };
		co_await newThreadAwaitable<channel_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/channels/" + dataPackage.channelId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "channels::modifyChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		channel_data data{ dataPackage.channelId };
		if (cache.contains(data.id)) {
			data = cache[data.id];
		}
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), data);
		if (doWeCacheChannelsBool) {
			insertChannel(static_cast<channel_cache_data>(data));
		}
		co_return data;
	}

	co_routine<void> channels::deleteOrCloseChannelAsync(delete_or_close_channel_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Channel };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId;
		workload.callStack	   = "channels::deleteOrCloseAChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> channels::editChannelPermissionOverwritesAsync(edit_channel_permission_overwrites_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Channel_Permission_Overwrites };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "channels::editChannelPermissionOverwritesAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<invite_data>> channels::getChannelInvitesAsync(get_channel_invites_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Channel_Invites };
		co_await newThreadAwaitable<jsonifier::vector<invite_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/invites";
		workload.callStack	   = "channels::getChannelInvitesAsync()";
		jsonifier::vector<invite_data> returnData{};
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<invite_data> channels::createChannelInviteAsync(create_channel_invite_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Channel_Invite };
		co_await newThreadAwaitable<invite_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/invites";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "channels::createChannelInviteAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		invite_data returnData{};
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> channels::deleteChannelPermissionOverwritesAsync(delete_channel_permission_overwrites_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Channel_Permission_Overwrites };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
		workload.callStack	   = "channels::deleteChannelPermissionOverwritesAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<channel_data> channels::followNewsChannelAsync(follow_news_channel_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Follow_News_Channel };
		co_await newThreadAwaitable<channel_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/followers";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "channels::followNewsChannelAsync()";
		channel_data returnData{};
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> channels::triggerTypingIndicatorAsync(trigger_typing_indicator_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Trigger_Typing_Indicator };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/typing";
		workload.callStack	   = "channels::triggerTypingIndicatorAsync()";
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<channel_data>> channels::getGuildChannelsAsync(get_guild_channels_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Channels };
		co_await newThreadAwaitable<jsonifier::vector<channel_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/channels";
		workload.callStack	   = "channels::getGuildChannelsAsync()";
		jsonifier::vector<channel_data> returnData{};
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<channel_data> channels::createGuildChannelAsync(create_guild_channel_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Channel };
		co_await newThreadAwaitable<channel_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/channels";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "channels::createGuildChannelAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		channel_data returnData{};
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> channels::modifyGuildChannelPositionsAsync(modify_guild_channel_positions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Channel_Positions };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/channels";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "channels::modifyGuildChannelPositionsAsync()";
		if (dataPackage.reason != "") {
			workload.headersToInsert["x-audit-log-reason"] = dataPackage.reason;
		}
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<channel_data> channels::createDMChannelAsync(create_dmchannel_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Create_User_Dm };
		co_await newThreadAwaitable<channel_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/users/@me/channels";
		workload.callStack	   = "channels::createDMChannelAsync()";
		parser.serializeJson(dataPackage, workload.content);
		channel_data returnData{};
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<voice_region_data>> channels::getVoiceRegionsAsync() {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Voice_Regions };
		co_await newThreadAwaitable<jsonifier::vector<voice_region_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/voice/regions";
		workload.callStack	   = "channels::getVoiceRegionsAsync()";
		jsonifier::vector<voice_region_data> returnData{};
		channels::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	void channels::removeChannel(const channel_cache_data& channelId) {
		channels::cache.erase(channelId);
	};

	bool channels::doWeCacheChannels() {
		return channels::doWeCacheChannelsBool;
	}

	object_cache<channel_cache_data> channels::cache{};
	discord_core_internal::https_client* channels::httpsClient{};
	bool channels::doWeCacheChannelsBool{};
}
