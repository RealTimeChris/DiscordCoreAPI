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
/// EventManager.hpp - Header for the event manager class.
/// Jun 7, 2021
/// https://discordcoreapi.com
/// \file EventManager.cpp

#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	OnInputEventCreationData::OnInputEventCreationData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data) {
		this->inputEventData = message.processJsonMessage<InputEventData>(data, "d");
	}

	OnApplicationCommandPermissionsUpdateData::OnApplicationCommandPermissionsUpdateData(DiscordCoreInternal::WebSocketMessage& message,
		simdjson::ondemand::value data) {
		this->permissionData = message.processJsonMessage<GuildApplicationCommandPermissionsData>(data, "d");
	}

	OnAutoModerationRuleCreationData::OnAutoModerationRuleCreationData(DiscordCoreInternal::WebSocketMessage& message,
		simdjson::ondemand::value data) {
		this->theRule = message.processJsonMessage<AutoModerationRule>(data, "d");
	}

	OnAutoModerationRuleUpdateData::OnAutoModerationRuleUpdateData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data) {
		this->theRule = message.processJsonMessage<AutoModerationRule>(data, "d");
	}

	OnAutoModerationRuleDeletionData::OnAutoModerationRuleDeletionData(DiscordCoreInternal::WebSocketMessage& message,
		simdjson::ondemand::value data) {
		this->theRule = message.processJsonMessage<AutoModerationRule>(data, "d");
	}

	OnAutoModerationActionExecutionData::OnAutoModerationActionExecutionData(DiscordCoreInternal::WebSocketMessage& message,
		simdjson::ondemand::value data) {
		this->data = message.processJsonMessage<AutoModerationActionExecutionEventData>(data, "d");
	}

	OnChannelCreationData::OnChannelCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channel = std::make_unique<ChannelData>(data.processJsonMessage<ChannelData>(dataReal, "d"));
		if (DiscordCoreAPI::Channels::doWeCacheChannels()) {
			DiscordCoreAPI::GuildData guild{};
			guild.id = this->channel->guildId;
			if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
				DiscordCoreAPI::Guilds::getCache()[guild].channels.emplace_back(this->channel->id);
			}
			if (DiscordCoreAPI::Channels::doWeCacheChannels()) {
				DiscordCoreAPI::Channels::insertChannel(*this->channel);
			}
		}
	}

	OnChannelCreationData& OnChannelCreationData::operator=(const OnChannelCreationData& other) {
		*this->channel = *other.channel;
		return *this;
	}

	OnChannelCreationData::OnChannelCreationData(const OnChannelCreationData& other) {
		*this = other;
	}

	OnChannelUpdateData::OnChannelUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channel = std::make_unique<ChannelData>(data.processJsonMessage<ChannelData>(dataReal, "d"));
		if (DiscordCoreAPI::Channels::doWeCacheChannels()) {
			DiscordCoreAPI::Channels::insertChannel(*this->channel);
		}
	}

	OnChannelUpdateData& OnChannelUpdateData::operator=(const OnChannelUpdateData& other) {
		*this->channel = *other.channel;
		return *this;
	}

	OnChannelUpdateData::OnChannelUpdateData(const OnChannelUpdateData& other) {
		*this = other;
	}

	OnChannelDeletionData::OnChannelDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channel = std::make_unique<ChannelData>(data.processJsonMessage<ChannelData>(dataReal, "d"));
		if (DiscordCoreAPI::Channels::doWeCacheChannels()) {
			DiscordCoreAPI::GuildData guild{};
			guild.id = this->channel->guildId;
			if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
				for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::getCache()[guild].channels.size(); ++x) {
					if (DiscordCoreAPI::Guilds::getCache()[guild].channels[x] == this->channel->id) {
						DiscordCoreAPI::Guilds::getCache()[guild].channels.erase(DiscordCoreAPI::Guilds::getCache()[guild].channels.begin() + x);
					}
				}
			}
			if (DiscordCoreAPI::Channels::doWeCacheChannels()) {
				DiscordCoreAPI::Channels::removeChannel(this->channel->id);
			}
		}
	}

	OnChannelDeletionData& OnChannelDeletionData::operator=(const OnChannelDeletionData& other) {
		*this->channel = *other.channel;
		return *this;
	}

	OnChannelDeletionData::OnChannelDeletionData(const OnChannelDeletionData& other) {
		*this = other;
	}

	OnChannelPinsUpdateData::OnChannelPinsUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->dataPackage = data.processJsonMessage<ChannelPinsUpdateEventData>(dataReal, "d");
	}

	OnThreadCreationData::OnThreadCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->thread = data.processJsonMessage<Thread>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->thread.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			DiscordCoreAPI::Guilds::getCache()[guild].threads.emplace_back(this->thread.id);
		}
	}

	OnThreadUpdateData::OnThreadUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->thread = data.processJsonMessage<Thread>(dataReal, "d");
	}

	OnThreadDeletionData::OnThreadDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->thread = data.processJsonMessage<Thread>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->thread.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::getCache()[guild].threads.size(); ++x) {
				if (DiscordCoreAPI::Guilds::getCache()[guild].threads[x] == this->thread.id) {
					DiscordCoreAPI::Guilds::getCache()[guild].threads.erase(DiscordCoreAPI::Guilds::getCache()[guild].threads.begin() + x);
				}
			}
		}
	}

	OnThreadListSyncData::OnThreadListSyncData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->threadListSyncData = data.processJsonMessage<ThreadListSyncData>(dataReal, "d");
	}

	OnThreadMemberUpdateData::OnThreadMemberUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->threadMember = data.processJsonMessage<ThreadMemberData>(dataReal, "d");
	}

	OnThreadMembersUpdateData::OnThreadMembersUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->threadMembersUpdateData = data.processJsonMessage<ThreadMembersUpdateData>(dataReal, "d");
	}

	OnGuildCreationData::OnGuildCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreClient* client) {
		this->guild = std::make_unique<GuildData>(data.processJsonMessage<GuildData>(dataReal, "d"));
		this->guild->discordCoreClient = client;
		if (DiscordCoreAPI::Guilds::doWeCacheGuilds()) {
			if (DiscordCoreAPI::Guilds::doWeCacheGuilds()) {
				DiscordCoreAPI::Guilds::insertGuild(*this->guild);
			}
		}
	}

	OnGuildCreationData& OnGuildCreationData::operator=(const OnGuildCreationData& other) {
		*this->guild = *other.guild;
		return *this;
	}

	OnGuildCreationData::OnGuildCreationData(const OnGuildCreationData& other) {
		*this = other;
	}

	OnGuildUpdateData::OnGuildUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreClient* clientNew) {
		this->guild = std::make_unique<GuildData>(data.processJsonMessage<GuildData>(dataReal, "d"));
		this->guild->discordCoreClient = clientNew;
		if (DiscordCoreAPI::Guilds::doWeCacheGuilds()) {
			DiscordCoreAPI::Guilds::insertGuild(*this->guild);
		}
	}

	OnGuildUpdateData& OnGuildUpdateData::operator=(const OnGuildUpdateData& other) {
		*this->guild = *other.guild;
		return *this;
	}

	OnGuildUpdateData::OnGuildUpdateData(const OnGuildUpdateData& other) {
		*this = other;
	}

	OnGuildDeletionData::OnGuildDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreClient* clientNew) {
		this->guild = std::make_unique<GuildData>(data.processJsonMessage<GuildData>(dataReal, "d"));
		if (DiscordCoreAPI::Guilds::doWeCacheGuilds()) {
			DiscordCoreAPI::Guilds::removeGuild(this->guild->id);
		}
		for (auto& valueNew: this->guild->members) {
			DiscordCoreAPI::GuildMemberData guildMember =
				DiscordCoreAPI::GuildMembers::getCachedGuildMember({ .guildMemberId = valueNew, .guildId = this->guild->id });
			DiscordCoreAPI::GuildMembers::removeGuildMember(guildMember);
		}
		for (auto& valueNew: this->guild->channels) {
			DiscordCoreAPI::Channels::removeChannel(valueNew);
		}
		for (auto& valueNew: this->guild->roles) {
			DiscordCoreAPI::Roles::removeRole(valueNew);
		}
	}

	OnGuildDeletionData& OnGuildDeletionData::operator=(const OnGuildDeletionData& other) {
		*this->guild = *other.guild;
		return *this;
	}

	OnGuildDeletionData::OnGuildDeletionData(const OnGuildDeletionData& other) {
		*this = other;
	}

	OnGuildBanAddData::OnGuildBanAddData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		this->user = data.processJsonMessage<UserData>(dataReal["d"], "user");
	}

	OnGuildBanRemoveData::OnGuildBanRemoveData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		this->user = data.processJsonMessage<UserData>(dataReal["d"], "user");
	}

	OnGuildEmojisUpdateData::OnGuildEmojisUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->updateData = data.processJsonMessage<GuildEmojisUpdateEventData>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->updateData.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			DiscordCoreAPI::Guilds::getCache()[guild].emoji.clear();
			for (auto& valueNew: this->updateData.emojis) {
				DiscordCoreAPI::Guilds::getCache()[guild].emoji.emplace_back(valueNew.id);
			}
		}
	}

	OnGuildStickersUpdateData::OnGuildStickersUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->updateData = data.processJsonMessage<GuildStickersUpdateEventData>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->updateData.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			DiscordCoreAPI::Guilds::getCache()[guild].stickers.clear();
			for (auto& valueNew: this->updateData.stickers) {
				DiscordCoreAPI::Guilds::getCache()[guild].stickers.emplace_back(valueNew.id);
			}
		}
	}

	OnGuildIntegrationsUpdateData::OnGuildIntegrationsUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->guildId = getId(dataReal["d"], "guild_id");
	}

	OnGuildMemberAddData::OnGuildMemberAddData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreClient* client) {
		this->guildMember = std::make_unique<GuildMemberData>(data.processJsonMessage<GuildMemberData>(dataReal, "d"));
		this->discordCoreClient = client;
		if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers()) {
			DiscordCoreAPI::GuildMembers::insertGuildMember(*this->guildMember);
			DiscordCoreAPI::GuildData guild{};
			guild.id = guildMember->guildId;
			if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
				++DiscordCoreAPI::Guilds::getCache()[guild].memberCount;
			}
		}
	}

	OnGuildMemberAddData& OnGuildMemberAddData::operator=(const OnGuildMemberAddData& other) {
		this->discordCoreClient = other.discordCoreClient;
		*this->guildMember = *other.guildMember;
		return *this;
	}

	OnGuildMemberAddData::OnGuildMemberAddData(const OnGuildMemberAddData& other) {
		*this = other;
	}

	OnGuildMemberRemoveData::OnGuildMemberRemoveData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreClient* client) {
		this->discordCoreClient = client;
		this->guildId = getId(dataReal["d"], "guild_id");
		this->user = std::make_unique<UserData>(data.processJsonMessage<UserData>(dataReal["d"], "user"));
		GuildMember guildMember = GuildMembers::getCachedGuildMember({ .guildMemberId = this->user->id, .guildId = this->guildId });
		if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers()) {
			DiscordCoreAPI::GuildData guild{};
			guild.id = this->guildId;
			DiscordCoreAPI::GuildMembers::removeGuildMember(guildMember);
			if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
				for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::getCache()[guild].members.size(); ++x) {
					if (DiscordCoreAPI::Guilds::getCache()[guild].members[x] == this->user->id) {
						--DiscordCoreAPI::Guilds::getCache()[guild].memberCount;
						DiscordCoreAPI::Guilds::getCache()[guild].members.erase(DiscordCoreAPI::Guilds::getCache()[guild].members.begin() + x);
					}
				}
			}
		}
	}

	OnGuildMemberRemoveData& OnGuildMemberRemoveData::operator=(const OnGuildMemberRemoveData& other) {
		this->discordCoreClient = other.discordCoreClient;
		this->guildId = other.guildId;
		*this->user = *other.user;
		return *this;
	}

	OnGuildMemberRemoveData::OnGuildMemberRemoveData(const OnGuildMemberRemoveData& other) {
		*this = other;
	}

	OnGuildMemberUpdateData::OnGuildMemberUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreClient* client) {
		this->guildMember = std::make_unique<GuildMemberData>(data.processJsonMessage<GuildMemberData>(dataReal, "d"));
		if (DiscordCoreAPI::GuildMembers::doWeCacheGuildMembers()) {
			DiscordCoreAPI::GuildMembers::insertGuildMember(*this->guildMember);
		}
	}

	OnGuildMemberUpdateData& OnGuildMemberUpdateData::operator=(const OnGuildMemberUpdateData& other) {
		*this->guildMember = *other.guildMember;
		return *this;
	}

	OnGuildMemberUpdateData::OnGuildMemberUpdateData(const OnGuildMemberUpdateData& other) {
		*this = other;
	}

	OnGuildMembersChunkData::OnGuildMembersChunkData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->chunkEventData = data.processJsonMessage<GuildMembersChunkEventData>(dataReal, "d");
	}

	OnRoleCreationData::OnRoleCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->guildId = getId(dataReal["d"], "guild_id");
		this->role = std::make_unique<RoleData>(data.processJsonMessage<RoleData>(dataReal["d"], "role"));
		DiscordCoreAPI::GuildData guild{};
		guild.id = guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			DiscordCoreAPI::Guilds::getCache()[guild].roles.emplace_back(this->role->id);
		}
		if (DiscordCoreAPI::Roles::doWeCacheRoles()) {
			DiscordCoreAPI::Roles::insertRole(*this->role);
		}
	}

	OnRoleCreationData& OnRoleCreationData::operator=(const OnRoleCreationData& other) {
		this->guildId = other.guildId;
		*this->role = *other.role;
		return *this;
	}

	OnRoleCreationData::OnRoleCreationData(const OnRoleCreationData& other) {
		*this = other;
	}

	OnRoleUpdateData::OnRoleUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->guildId = getId(dataReal["d"], "guild_id");
		this->role = std::make_unique<RoleData>(data.processJsonMessage<RoleData>(dataReal["d"], "role"));
		if (DiscordCoreAPI::Roles::doWeCacheRoles()) {
			DiscordCoreAPI::Roles::insertRole(*this->role);
		}
	}

	OnRoleUpdateData& OnRoleUpdateData::operator=(const OnRoleUpdateData& other) {
		this->guildId = other.guildId;
		*this->role = *other.role;
		return *this;
	}

	OnRoleUpdateData::OnRoleUpdateData(const OnRoleUpdateData& other) {
		*this = other;
	}

	OnRoleDeletionData::OnRoleDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->role->id = getId(dataReal["d"], "role_id");
		this->guildId = getId(dataReal["d"], "guild_id");
		this->role = std::make_unique<RoleData>(Roles::getCachedRole({ .guildId = this->guildId, .roleId = this->role->id }));
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->guildId;
		if (DiscordCoreAPI::Roles::doWeCacheRoles()) {
			DiscordCoreAPI::Roles::removeRole(this->role->id);
			if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
				for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::getCache()[guild].roles.size(); ++x) {
					if (DiscordCoreAPI::Guilds::getCache()[guild].roles[x] == this->role->id) {
						DiscordCoreAPI::Guilds::getCache()[guild].roles.erase(DiscordCoreAPI::Guilds::getCache()[guild].roles.begin() + x);
					}
				}
			}
		}
	}

	OnRoleDeletionData& OnRoleDeletionData::operator=(const OnRoleDeletionData& other) {
		this->guildId = other.guildId;
		*this->role = *other.role;
		return *this;
	}

	OnRoleDeletionData::OnRoleDeletionData(const OnRoleDeletionData& other) {
		*this = other;
	}

	OnUserUpdateData& OnUserUpdateData::operator=(const OnUserUpdateData& other) {
		*this->user = *other.user;
		return *this;
	}

	OnUserUpdateData::OnUserUpdateData(const OnUserUpdateData& other) {
		*this = other;
	}

	OnVoiceServerUpdateData::OnVoiceServerUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreInternal::WebSocketClient* sslShard) {
		this->endpoint = getString(dataReal["d"], "endpoint");
		this->guildId = getId(dataReal["d"], "guild_id");
		this->token = getString(dataReal["d"], "token");
		sslShard->voiceConnectionData.endPoint = this->endpoint;
		sslShard->voiceConnectionData.token = this->token;
		if (sslShard->areWeCollectingData && !sslShard->serverUpdateCollected && !sslShard->stateUpdateCollected) {
			sslShard->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
			sslShard->serverUpdateCollected = true;
		} else if (sslShard->areWeCollectingData && !sslShard->serverUpdateCollected) {
			if (sslShard->voiceConnectionDataBuffersMap.contains(this->guildId.operator size_t())) {
				sslShard->voiceConnectionDataBuffersMap[this->guildId.operator size_t()]->send(sslShard->voiceConnectionData);
			}
			sslShard->serverUpdateCollected = false;
			sslShard->stateUpdateCollected = false;
			sslShard->areWeCollectingData = false;
		}
	};

	OnGuildScheduledEventCreationData::OnGuildScheduledEventCreationData(DiscordCoreInternal::WebSocketMessage& data,
		simdjson::ondemand::value dataReal) {
		this->guildScheduledEvent = data.processJsonMessage<GuildScheduledEventData>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->guildScheduledEvent.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			DiscordCoreAPI::Guilds::getCache()[guild].guildScheduledEvents.emplace_back(this->guildScheduledEvent.id);
		}
	}

	OnGuildScheduledEventUpdateData::OnGuildScheduledEventUpdateData(DiscordCoreInternal::WebSocketMessage& data,
		simdjson::ondemand::value dataReal) {
		this->guildScheduledEvent = data.processJsonMessage<GuildScheduledEventData>(dataReal, "d");
	}

	OnGuildScheduledEventDeletionData::OnGuildScheduledEventDeletionData(DiscordCoreInternal::WebSocketMessage& data,
		simdjson::ondemand::value dataReal) {
		this->guildScheduledEvent = data.processJsonMessage<GuildScheduledEventData>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->guildScheduledEvent.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::getCache()[guild].guildScheduledEvents.size(); ++x) {
				if (DiscordCoreAPI::Guilds::getCache()[guild].guildScheduledEvents[x] == this->guildScheduledEvent.id) {
					DiscordCoreAPI::Guilds::getCache()[guild].guildScheduledEvents.erase(
						DiscordCoreAPI::Guilds::getCache()[guild].guildScheduledEvents.begin() + x);
				}
			}
		}
	}

	OnGuildScheduledEventUserAddData::OnGuildScheduledEventUserAddData(DiscordCoreInternal::WebSocketMessage& data,
		simdjson::ondemand::value dataReal) {
		this->guildScheduledEventId = DiscordCoreAPI::getId(dataReal["d"], "guild_scheduled_event_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		this->userId = DiscordCoreAPI::getId(dataReal["d"], "user_id");
	}

	OnGuildScheduledEventUserRemoveData::OnGuildScheduledEventUserRemoveData(DiscordCoreInternal::WebSocketMessage& data,
		simdjson::ondemand::value dataReal) {
		this->guildScheduledEventId = DiscordCoreAPI::getId(dataReal["d"], "guild_scheduled_event_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		this->userId = DiscordCoreAPI::getId(dataReal["d"], "user_id");
	}

	OnIntegrationCreationData::OnIntegrationCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		simdjson::ondemand::value objectNew{};
		if (dataReal["d"]["integration"].get(objectNew) == simdjson::error_code::SUCCESS) {
			this->integrationData = DiscordCoreAPI::IntegrationData{ objectNew };
		}
	}

	OnIntegrationUpdateData::OnIntegrationUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		simdjson::ondemand::value objectNew{};
		if (dataReal["d"]["integration"].get(objectNew) == simdjson::error_code::SUCCESS) {
			this->integrationData = DiscordCoreAPI::IntegrationData{ objectNew };
		}
	}

	OnIntegrationDeletionData::OnIntegrationDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->applicationId = DiscordCoreAPI::getId(dataReal["d"], "application_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		this->id = DiscordCoreAPI::getId(dataReal["d"], "id");
	}

	OnInteractionCreationData::OnInteractionCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreClient* clientPtr) {
		this->interactionData = DiscordCoreAPI::InteractionData{ data.processJsonMessage<DiscordCoreAPI::InteractionData>(dataReal, "d") };
		std::unique_ptr<DiscordCoreAPI::InputEventData> eventData{ std::make_unique<DiscordCoreAPI::InputEventData>(interactionData) };
		switch (interactionData.type) {
			case DiscordCoreAPI::InteractionType::Application_Command: {
				eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
				*eventData->interactionData = this->interactionData;
				std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>(*eventData) };
				commandData->discordCoreClient = eventData->getGuildData().discordCoreClient;
				clientPtr->getCommandController().checkForAndRunCommand(*commandData);
				std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
					std::make_unique<DiscordCoreAPI::OnInputEventCreationData>(data, dataReal)
				};
				eventCreationData->inputEventData = *eventData;
				break;
			}
			case DiscordCoreAPI::InteractionType::Message_Component: {
				switch (interactionData.data.componentData.componentType) {
					case DiscordCoreAPI::ComponentType::Button: {
						eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
						*eventData->interactionData = this->interactionData;
						if (DiscordCoreAPI::ButtonCollector::buttonInteractionBuffersMap.contains(
								eventData->getChannelData().id + eventData->getMessageData().id)) {
							DiscordCoreAPI::ButtonCollector::buttonInteractionBuffersMap[eventData->getChannelData().id + eventData->getMessageData().id]->send(
								interactionData);
						}
						DiscordCoreAPI::ButtonCollector::buttonInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
					case DiscordCoreAPI::ComponentType::Channel_Select:
					case DiscordCoreAPI::ComponentType::Mentionable_Select:
					case DiscordCoreAPI::ComponentType::Role_Select:
					case DiscordCoreAPI::ComponentType::User_Select:
					case DiscordCoreAPI::ComponentType::String_Select: {
						eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
						*eventData->interactionData = this->interactionData;
						if (DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBuffersMap.contains(
								eventData->getChannelData().id + eventData->getMessageData().id)) {
							DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionBuffersMap[eventData->getChannelData().id +
								eventData->getMessageData().id]
								->send(interactionData);
						}
						DiscordCoreAPI::SelectMenuCollector::selectMenuInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
				}
				break;
			}
			case DiscordCoreAPI::InteractionType::Modal_Submit: {
				eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
				*eventData->interactionData = this->interactionData;
				std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{
					std::make_unique<DiscordCoreAPI::OnInputEventCreationData>(data, dataReal)
				};
				eventCreationData->inputEventData = *eventData;
				if (DiscordCoreAPI::ModalCollector::modalInteractionBuffersMap.contains(eventData->getChannelData().id)) {
					DiscordCoreAPI::ModalCollector::modalInteractionBuffersMap[eventData->getChannelData().id]->send(eventData->getInteractionData());
					DiscordCoreAPI::ModalCollector::modalInteractionEventsMap.operator()(*eventData->interactionData);
				}
				break;
			}
			case DiscordCoreAPI::InteractionType::Application_Command_Autocomplete: {
				eventData->responseType = DiscordCoreAPI::InputEventResponseType::Unset;
				*eventData->interactionData = this->interactionData;
				std::unique_ptr<DiscordCoreAPI::OnAutoCompleteEntryData> autocompleteEntryData{
					std::make_unique<DiscordCoreAPI::OnAutoCompleteEntryData>(data, dataReal)
				};
				autocompleteEntryData->inputEvent = *eventData;
				clientPtr->getEventManager().onAutoCompleteEntryEvent(*autocompleteEntryData);
				break;
			}
		}
	}

	OnInviteCreationData::OnInviteCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->invite = data.processJsonMessage<DiscordCoreAPI::InviteData>(dataReal, "d");
	}

	OnInviteDeletionData::OnInviteDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channelId = DiscordCoreAPI::getId(dataReal["d"], "channel_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		this->code = DiscordCoreAPI::getId(dataReal["d"], "code");
	}

	OnMessageCreationData::OnMessageCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->message = data.processJsonMessage<Message>(dataReal, "d");
		for (auto& [key, dValue]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBuffersMap) {
			dValue->send(this->message);
		}
	}

	OnMessageUpdateData::OnMessageUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->messageNew = data.processJsonMessage<Message>(dataReal, "d");
		for (auto& [key, dValue]: DiscordCoreAPI::ObjectCollector<DiscordCoreAPI::Message>::objectsBuffersMap) {
			dValue->send(this->messageNew);
		}
	}

	OnMessageDeletionData::OnMessageDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channelId = DiscordCoreAPI::getId(dataReal["d"], "channel_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		this->messageId = DiscordCoreAPI::getId(dataReal["d"], "id");
	}

	OnMessageDeleteBulkData::OnMessageDeleteBulkData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channelId = DiscordCoreAPI::getId(dataReal["d"], "channel_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		simdjson::ondemand::array arrayValue{};
		if (dataReal["d"]["ids"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			for (auto dValue: arrayValue) {
				this->ids.emplace_back(DiscordCoreAPI::strtoull(dValue.get_string().take_value()));
			}
		}
	}

	OnReactionAddData::OnReactionAddData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->reaction = data.processJsonMessage<DiscordCoreAPI::Reaction>(dataReal, "d");
	}

	OnReactionRemoveData::OnReactionRemoveData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->reactionRemoveData = data.processJsonMessage<DiscordCoreAPI::ReactionRemoveData>(dataReal, "d");
	}

	OnReactionRemoveAllData::OnReactionRemoveAllData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->messageId = DiscordCoreAPI::getId(dataReal["d"], "message_id");
		this->channelId = DiscordCoreAPI::getId(dataReal["d"], "channel_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
	}

	OnReactionRemoveEmojiData::OnReactionRemoveEmojiData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channelId = DiscordCoreAPI::getId(dataReal["d"], "channel_id");
		this->messageId = DiscordCoreAPI::getId(dataReal["d"], "message_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
		simdjson::ondemand::value objectNew{};
		if (dataReal["d"]["emoji"].get(objectNew) == simdjson::error_code::SUCCESS) {
			this->emoji = DiscordCoreAPI::EmojiData{ objectNew };
		}
	}

	OnPresenceUpdateData::OnPresenceUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->presenceData = data.processJsonMessage<DiscordCoreAPI::PresenceUpdateData>(dataReal, "d");
	}

	OnStageInstanceCreationData::OnStageInstanceCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->stageInstance = data.processJsonMessage<DiscordCoreAPI::StageInstance>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->stageInstance.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			DiscordCoreAPI::Guilds::getCache()[guild].stageInstances.emplace_back(this->stageInstance.id);
		}
	}

	OnStageInstanceUpdateData::OnStageInstanceUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->stageInstance = data.processJsonMessage<DiscordCoreAPI::StageInstance>(dataReal, "d");
	}

	OnStageInstanceDeletionData::OnStageInstanceDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->stageInstance = data.processJsonMessage<DiscordCoreAPI::StageInstance>(dataReal, "d");
		DiscordCoreAPI::GuildData guild{};
		guild.id = this->stageInstance.guildId;
		if (DiscordCoreAPI::Guilds::getCache().contains(guild)) {
			for (uint64_t x = 0; x < DiscordCoreAPI::Guilds::getCache()[guild].stageInstances.size(); ++x) {
				if (DiscordCoreAPI::Guilds::getCache()[guild].stageInstances[x] == this->stageInstance.id) {
					DiscordCoreAPI::Guilds::getCache()[guild].stageInstances.erase(
						DiscordCoreAPI::Guilds::getCache()[guild].stageInstances.begin() + x);
				}
			}
		}
	}

	OnTypingStartData::OnTypingStartData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->typingStartData = data.processJsonMessage<DiscordCoreAPI::TypingStartData>(dataReal, "d");
	}

	OnUserUpdateData::OnUserUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->user = std::make_unique<UserData>(data.processJsonMessage<UserData>(dataReal, "d"));
		if (DiscordCoreAPI::Users::doWeCacheUsers()) {
			DiscordCoreAPI::Users::insertUser(*user);
		}
	}

	OnVoiceStateUpdateData::OnVoiceStateUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
		DiscordCoreInternal::WebSocketClient* sslShard) {
		this->voiceStateData = data.processJsonMessage<DiscordCoreAPI::VoiceStateData>(dataReal, "d");
		sslShard->voiceConnectionData.sessionId = this->voiceStateData.sessionId;
		if (sslShard->areWeCollectingData && !sslShard->stateUpdateCollected && !sslShard->serverUpdateCollected &&
			this->voiceStateData.userId == sslShard->userId) {
			sslShard->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
			sslShard->voiceConnectionData.sessionId = this->voiceStateData.sessionId;
			sslShard->stateUpdateCollected = true;
		} else if (sslShard->areWeCollectingData && !sslShard->stateUpdateCollected) {
			sslShard->voiceConnectionData.sessionId = this->voiceStateData.sessionId;
			if (sslShard->voiceConnectionDataBuffersMap.contains(this->voiceStateData.guildId.operator size_t())) {
				sslShard->voiceConnectionDataBuffersMap[this->voiceStateData.guildId.operator size_t()]->send(sslShard->voiceConnectionData);
			}
			sslShard->serverUpdateCollected = false;
			sslShard->stateUpdateCollected = false;
			sslShard->areWeCollectingData = false;
		}
		DiscordCoreAPI::GuildMembers::insertVoiceState(this->voiceStateData);
	}

	OnWebhookUpdateData::OnWebhookUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->channelId = DiscordCoreAPI::getId(dataReal["d"], "channel_id");
		this->guildId = DiscordCoreAPI::getId(dataReal["d"], "guild_id");
	}

	OnAutoCompleteEntryData::OnAutoCompleteEntryData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal) {
		this->inputEvent = data.processJsonMessage<InputEventData>(dataReal, "d");
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onApplicationCommandsPermissionsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> handler) {
		return this->onApplicationCommandPermissionsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onApplicationCommandPermissionsUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleCreationData> handler) {
		return this->onAutoModerationRuleCreationEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationRuleCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleUpdateData> handler) {
		return this->onAutoModerationRuleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationRuleUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleDeletionData> handler) {
		return this->onAutoModerationRuleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationRuleDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationActionExecution(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationActionExecutionData> handler) {
		return this->onAutoModerationActionExecutionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationActionExecutionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoCompleteEntry(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoCompleteEntryData> handler) {
		return this->onAutoCompleteEntryEvent.add(std::move(handler));
	}

	void EventManager::onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onInputEventCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInputEventCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInputEventCreationData> handler) {
		return this->onInputEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onInputEventCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelCreationData> handler) {
		return this->onChannelCreationEvent.add(std::move(handler));
	}

	void EventManager::onChannelCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelUpdateData> handler) {
		return this->onChannelUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelDeletionData> handler) {
		return this->onChannelDeletionEvent.add(std::move(handler));
	}

	void EventManager::onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelPinsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelPinsUpdateData> handler) {
		return this->onChannelPinsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onChannelPinsUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadCreationData> handler) {
		return this->onThreadCreationEvent.add(std::move(handler));
	}

	void EventManager::onThreadCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadUpdateData> handler) {
		return this->onThreadUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadDeletionData> handler) {
		return this->onThreadDeletionEvent.add(std::move(handler));
	}

	void EventManager::onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadListSync(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadListSyncData> handler) {
		return this->onThreadListSyncEvent.add(std::move(handler));
	}

	void EventManager::onThreadListSync(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadListSyncEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMemberUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMemberUpdateData> handler) {
		return this->onThreadMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadMemberUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMembersUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMembersUpdateData> handler) {
		return this->onThreadMembersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadMembersUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildCreationData> handler) {
		return this->onGuildCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildUpdateData> handler) {
		return this->onGuildUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildDeletionData> handler) {
		return this->onGuildDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanAddData> handler) {
		return this->onGuildBanAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanAddEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanRemoveData> handler) {
		return this->onGuildBanRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanRemoveEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildEmojisUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildEmojisUpdateData> handler) {
		return this->onGuildEmojisUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildEmojisUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildStickersUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildStickersUpdateData> handler) {
		return this->onGuildStickersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildStickersUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildIntegrationsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildIntegrationsUpdateData> handler) {
		return this->onGuildIntegrationsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildIntegrationsUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMembersChunk(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMembersChunkData> handler) {
		return this->onGuildMembersChunkEvent.add(std::move(handler));
	}

	void EventManager::onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMembersChunkEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberAddData> handler) {
		return this->onGuildMemberAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberAddEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberRemoveData> handler) {
		return this->onGuildMemberRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberRemoveEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberUpdateData> handler) {
		return this->onGuildMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleCreationData> handler) {
		return this->onRoleCreationEvent.add(std::move(handler));
	}

	void EventManager::onRoleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleUpdateData> handler) {
		return this->onRoleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleDeletionData> handler) {
		return this->onRoleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventCreationData> handler) {
		return this->onGuildScheduledEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUpdateData> handler) {
		return this->onGuildScheduledEventUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventDeletionData> handler) {
		return this->onGuildScheduledEventDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUserAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserAddData> handler) {
		return this->onGuildScheduledEventUserAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUserAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUserAddEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUserRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> handler) {
		return this->onGuildScheduledEventUserRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUserRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUserRemoveEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationCreationData> handler) {
		return this->onIntegrationCreationEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationUpdateData> handler) {
		return this->onIntegrationUpdateEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationDeletionData> handler) {
		return this->onIntegrationDeletionEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInteractionCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInteractionCreationData> handler) {
		return this->onInteractionCreationEvent.add(std::move(handler));
	}

	void EventManager::onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInteractionCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteCreationData> handler) {
		return this->onInviteCreationEvent.add(std::move(handler));
	}

	void EventManager::onInviteCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteDeletionData> handler) {
		return this->onInviteDeletionEvent.add(std::move(handler));
	}

	void EventManager::onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageCreationData> handler) {
		return this->onMessageCreationEvent.add(std::move(handler));
	}

	void EventManager::onMessageCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageUpdateData> handler) {
		return this->onMessageUpdateEvent.add(std::move(handler));
	}

	void EventManager::onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeletionData> handler) {
		return this->onMessageDeletionEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeleteBulk(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeleteBulkData> handler) {
		return this->onMessageDeleteBulkEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeleteBulkEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionAddData> handler) {
		return this->onReactionAddEvent.add(std::move(handler));
	}

	void EventManager::onReactionAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionAddEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveData> handler) {
		return this->onReactionRemoveEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveAll(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveAllData> handler) {
		return this->onReactionRemoveAllEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveAllEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveEmoji(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveEmojiData> handler) {
		return this->onReactionRemoveEmojiEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEmojiEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onPresenceUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnPresenceUpdateData> handler) {
		return this->onPresenceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onPresenceUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceCreationData> handler) {
		return this->onStageInstanceCreationEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceUpdateData> handler) {
		return this->onStageInstanceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceDeletionData> handler) {
		return this->onStageInstanceDeletionEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onTypingStart(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnTypingStartData> handler) {
		return this->onTypingStartEvent.add(std::move(handler));
	}

	void EventManager::onTypingStart(DiscordCoreInternal::EventDelegateToken& token) {
		onTypingStartEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onUserUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnUserUpdateData> handler) {
		return this->onUserUpdateEvent.add(std::move(handler));
	}

	void EventManager::onUserUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onUserUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceStateUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceStateUpdateData> handler) {
		return this->onVoiceStateUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceStateUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceServerUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceServerUpdateData> handler) {
		return this->onVoiceServerUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceServerUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onWebhookUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnWebhookUpdateData> handler) {
		return this->onWebhookUpdateEvent.add(std::move(handler));
	}

	void EventManager::onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onWebhookUpdateEvent.remove(token);
	}

};