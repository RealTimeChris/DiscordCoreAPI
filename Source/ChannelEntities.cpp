/*
*
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
/// ChannelEntities.cpp - Source file for the classes related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file ChannelEntities.cpp

#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	Channel& Channel::operator=(ChannelData&& other) {
		this->permissionOverwrites = other.permissionOverwrites;
		this->memberCount = other.memberCount;
		this->parentId = other.parentId;
		this->position = other.position;
		this->guildId = other.guildId;
		this->ownerId = other.ownerId;
		this->flags = other.flags;
		this->name = other.name;
		this->type = other.type;
		this->id = other.id;
		return *this;
	}

	Channel::Channel(ChannelData&& other) {
		*this = other;
	}

	Channel& Channel::operator=(ChannelData& other) {
		this->permissionOverwrites = other.permissionOverwrites;
		this->memberCount = other.memberCount;
		this->parentId = other.parentId;
		this->position = other.position;
		this->guildId = other.guildId;
		this->ownerId = other.ownerId;
		this->flags = other.flags;
		this->name = other.name;
		this->type = other.type;
		this->id = other.id;
		return *this;
	}

	Channel::Channel(ChannelData& other) {
		*this = other;
	}

	void Channels::initialize(DiscordCoreInternal::HttpClient* theClient, bool doWeCacheNew) {
		Channels::doWeCache = doWeCacheNew;
		Channels::httpClient = theClient;
	}

	CoRoutine<Channel> Channels::getChannelAsync(GetChannelData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Channel);
		co_await NewThreadAwaitable<Channel>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Channel;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		workload.callStack = "Channels::getChannelAsync";
		auto channelNew = DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
		Channels::insertChannel(channelNew);
		co_return channelNew;
	}

	CoRoutine<ChannelData> Channels::getCachedChannelAsync(GetChannelData dataPackage) {
		co_await NewThreadAwaitable<ChannelData>();
		if (Channels::cache.contains(dataPackage.channelId)) {
			co_return Channels::cache[dataPackage.channelId];
		} else {
			co_return Channels::getChannelAsync(dataPackage).get();
		}
	}

	CoRoutine<Channel> Channels::modifyChannelAsync(ModifyChannelData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Channel);
		co_await NewThreadAwaitable<Channel>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Channel;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Channels::modifyChannelAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		auto channelNew = DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
		Channels::insertChannel(channelNew);
		co_return channelNew;
	}

	CoRoutine<void> Channels::deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Channel);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Channel;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId;
		workload.callStack = "Channels::deleteOrCloseAChannelAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
	}

	CoRoutine<void> Channels::editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Put_Channel_Permission_Overwrites);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Channel_Permission_Overwrites;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Channels::editChannelPermissionOverwritesAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
	}

	CoRoutine<std::vector<InviteData>> Channels::getChannelInvitesAsync(GetChannelInvitesData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Channel_Invites);
		co_await NewThreadAwaitable<std::vector<InviteData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Channel_Invites;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		workload.callStack = "Channels::getChannelInvitesAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<InviteData>>(*Channels::httpClient, workload);
	}

	CoRoutine<InviteData> Channels::createChannelInviteAsync(CreateChannelInviteData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Channel_Invite);
		co_await NewThreadAwaitable<InviteData>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Channel_Invite;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Channels::createChannelInviteAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<InviteData>(*Channels::httpClient, workload);
	}

	CoRoutine<void> Channels::deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Channel_Permission_Overwrites);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Channel_Permission_Overwrites;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
		workload.callStack = "Channels::deleteChannelPermissionOverwritesAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
	}

	CoRoutine<Channel> Channels::followNewsChannelAsync(FollowNewsChannelData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Follow_News_Channel);
		co_await NewThreadAwaitable<Channel>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Follow_News_Channel;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/followers";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Channels::followNewsChannelAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
	}

	CoRoutine<void> Channels::triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Trigger_Typing_Indicator);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Trigger_Typing_Indicator;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/typing";
		workload.callStack = "Channels::triggerTypingIndicatorAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
	}

	CoRoutine<std::vector<Channel>> Channels::getGuildChannelsAsync(GetGuildChannelsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Channels);
		co_await NewThreadAwaitable<std::vector<Channel>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Channels;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		workload.callStack = "Channels::getGuildChannelsAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Channel>>(*Channels::httpClient, workload);
	}

	CoRoutine<Channel> Channels::createGuildChannelAsync(CreateGuildChannelData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Guild_Channel);
		co_await NewThreadAwaitable<Channel>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Channel;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Channels::createGuildChannelAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		auto channelNew = DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
		Channels::insertChannel(channelNew);
		co_return channelNew;
	}

	CoRoutine<void> Channels::modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Channel_Positions);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Channel_Positions;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
		workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		workload.callStack = "Channels::modifyGuildChannelPositionsAsync";
		if (dataPackage.reason != "") {
			workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
		}
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
	}

	CoRoutine<Channel> Channels::createDMChannelAsync(CreateDMChannelData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Create_User_Dm);
		co_await NewThreadAwaitable<Channel>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Create_User_Dm;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/users/@me/channels";
		workload.callStack = "Channels::createDMChannelAsync";
		nlohmann::json theValue = { { "recipient_id", dataPackage.userId } };
		workload.content = theValue.dump();
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
	}

	CoRoutine<std::vector<VoiceRegionData>> Channels::getVoiceRegionsAsync() {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Voice_Regions);
		co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Voice_Regions;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/voice/regions";
		workload.callStack = "Channels::getVoiceRegions";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<VoiceRegionData>>(*Channels::httpClient, workload);
	}

	void Channels::insertChannel(ChannelData channel) {
		std::lock_guard<std::mutex> theLock{ Channels::theMutex };
		if (channel.id == "") {
			return;
		}
		if (Channels::doWeCache) {
			Channels::cache.insert_or_assign(channel.id, channel);
		}
	}

	void Channels::removeChannel(const std::string& channelId) {
		std::lock_guard<std::mutex> theLock{ Channels::theMutex };
		Channels::cache.erase(channelId);
	};

	DiscordCoreInternal::HttpClient* Channels::httpClient{ nullptr };
	std::unordered_map<std::string, ChannelData> Channels::cache{};
	bool Channels::doWeCache{ false };
	std::mutex Channels::theMutex{};
}
