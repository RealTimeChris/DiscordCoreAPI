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

	Channel::Channel(ChannelData dataNew) {
		this->permissionOverwrites = dataNew.permissionOverwrites;
		this->rateLimitPerUser = dataNew.rateLimitPerUser;
		this->lastPinTimestamp = dataNew.lastPinTimestamp;
		this->videoQualityMode = dataNew.videoQualityMode;
		this->threadMetadata = dataNew.threadMetadata;
		this->lastMessageId = dataNew.lastMessageId;
		this->applicationId = dataNew.applicationId;
		this->messageCount = dataNew.messageCount;
		this->channelFlags = dataNew.channelFlags;
		this->memberCount = dataNew.memberCount;
		this->recipients = dataNew.recipients;
		this->userLimit = dataNew.userLimit;
		this->rtcRegion = dataNew.rtcRegion;
		this->position = dataNew.position;
		this->parentId = dataNew.parentId;
		this->ownerId = dataNew.ownerId;
		this->bitrate = dataNew.bitrate;
		this->guildId = dataNew.guildId;
		this->member = dataNew.member;
		this->topic = dataNew.topic;
		this->type = dataNew.type;
		this->name = dataNew.name;
		this->icon = dataNew.icon;
		this->id = dataNew.id;
	}

	void Channels::initialize(DiscordCoreInternal::HttpClient* theClient) {
		Channels::httpClient = theClient;
	}

	CoRoutine<Channel> Channels::getChannelAsync(GetChannelData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Channel];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Channel] += 1;
			co_await NewThreadAwaitable<Channel>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Channel;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			workload.callStack = "Channels::getChannelAsync";
			auto channelNew = DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
			Channels::insertChannel(channelNew);
			co_return channelNew;
		} catch (...) {
			reportException("Channels::getChannelAsync()");
		}
	}

	CoRoutine<Channel> Channels::getCachedChannelAsync(GetChannelData dataPackage) {
		try {
			co_await NewThreadAwaitable<Channel>();
			Channel channel{};
			if (Channels::cache.contains(dataPackage.channelId)) {
				channel = Channels::cache[dataPackage.channelId];
			} else {
				channel = Channels::getChannelAsync(dataPackage).get();
			}
			co_return channel;
		} catch (...) {
			reportException("Channels::getCachedChannelAsync()");
		}
	}

	CoRoutine<Channel> Channels::modifyChannelAsync(ModifyChannelData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Channel];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Channel] += 1;
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
		} catch (...) {
			reportException("Channels::modifyChannelAsync()");
		}
	}

	CoRoutine<void> Channels::deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Channel];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Channel] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Channel;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId;
			workload.callStack = "Channels::deleteOrCloseAChannelAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::deleteOrCloseChannelAsync()");
		}
	}

	CoRoutine<void> Channels::editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Put_Channel_Permission_Overwrites];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Put_Channel_Permission_Overwrites] += 1;
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
		} catch (...) {
			reportException("Channels::editChannelPermissionOverwritesAsync()");
		}
	}

	CoRoutine<std::vector<InviteData>> Channels::getChannelInvitesAsync(GetChannelInvitesData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Channel_Invites];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Channel_Invites] += 1;
			co_await NewThreadAwaitable<std::vector<InviteData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Channel_Invites;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/invites";
			workload.callStack = "Channels::getChannelInvitesAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<InviteData>>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::getChannelInvitesAsync()");
		}
	}

	CoRoutine<InviteData> Channels::createChannelInviteAsync(CreateChannelInviteData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Channel_Invite];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Channel_Invite] += 1;
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
		} catch (...) {
			reportException("Channels::createChannelInviteAsync()");
		}
	}

	CoRoutine<void> Channels::deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Channel_Permission_Overwrites];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Delete_Channel_Permission_Overwrites] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Channel_Permission_Overwrites;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/permissions/" + dataPackage.roleOrUserId;
			workload.callStack = "Channels::deleteChannelPermissionOverwritesAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::deleteChannelPermissionOverwritesAsync()");
		}
	}

	CoRoutine<Channel> Channels::followNewsChannelAsync(FollowNewsChannelData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Follow_News_Channel];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Follow_News_Channel] += 1;
			co_await NewThreadAwaitable<Channel>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Follow_News_Channel;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/followers";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			workload.callStack = "Channels::followNewsChannelAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::followNewsChannelAsync()");
		}
	}

	CoRoutine<void> Channels::triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Trigger_Typing_Indicator];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Trigger_Typing_Indicator] += 1;
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Trigger_Typing_Indicator;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/typing";
			workload.callStack = "Channels::triggerTypingIndicatorAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::triggerTypingIndicatorAsync()");
		}
	}

	CoRoutine<std::vector<Channel>> Channels::getGuildChannelsAsync(GetGuildChannelsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Guild_Channels];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Guild_Channels] += 1;
			co_await NewThreadAwaitable<std::vector<Channel>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Channels;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/channels";
			workload.callStack = "Channels::getGuildChannelsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Channel>>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::getGuildChannelsAsync()");
		}
	}

	CoRoutine<Channel> Channels::createGuildChannelAsync(CreateGuildChannelData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Guild_Channel];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Guild_Channel] += 1;
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
		} catch (...) {
			reportException("Channels::createGuildChannelAsync()");
		}
	}

	CoRoutine<void> Channels::modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId =
				DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Channel_Positions];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Channel_Positions] += 1;
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
		} catch (...) {
			reportException("Channels::modifyGuildChannelPositionsAsync()");
		}
	}

	CoRoutine<Channel> Channels::createDMChannelAsync(CreateDMChannelData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Create_User_Dm];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Post_Create_User_Dm] += 1;
			co_await NewThreadAwaitable<Channel>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Create_User_Dm;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/users/@me/channels";
			workload.callStack = "Channels::createDMChannelAsync";
			nlohmann::json theValue = { { "recipient_id", dataPackage.userId } };
			workload.content = theValue.dump();
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Channel>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::getDMChannelAsyn()");
		}
	}

	CoRoutine<std::vector<VoiceRegionData>> Channels::getVoiceRegionsAsync() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Voice_Regions];
			DiscordCoreInternal::HttpWorkloadData::workloadIdsExternal[DiscordCoreInternal::HttpWorkloadType::Get_Voice_Regions] += 1;
			co_await NewThreadAwaitable<std::vector<VoiceRegionData>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Voice_Regions;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/voice/regions";
			workload.callStack = "Channels::getVoiceRegions";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<VoiceRegionData>>(*Channels::httpClient, workload);
		} catch (...) {
			reportException("Channels::getVoiceRegions()");
		}
	}

	void Channels::insertChannel(Channel channel) {
		try {
			if (channel.id == "") {
				return;
			}
			Channels::cache.insert_or_assign(channel.id, channel);
		} catch (...) {
			reportException("Channels::insertChannel()");
		}
	}

	void Channels::removeChannel(std::string channelId) {
		try {
			Channels::cache.erase(channelId);
		} catch (...) {
			reportException("Channels::removeChannel()");
		}
	};

	DiscordCoreInternal::HttpClient* Channels::httpClient{ nullptr };
	std::unordered_map<std::string, Channel> Channels::cache{};

}
