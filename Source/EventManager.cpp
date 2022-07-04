/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
/// EventManager.hpp - Header for the event manager class.
/// Jun 7, 2021
/// https://discordcoreapi.com
/// \file EventManager.cpp

#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	DiscordCoreInternal::EventDelegateToken EventManager::onApplicationCommandsPermissionsUpdate(
		DiscordCoreInternal::EventDelegate<void, OnApplicationCommandPermissionsUpdateData> handler) {
		return this->onApplicationCommandPermissionsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onApplicationCommandPermissionsUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegate<void, OnAutoModerationRuleCreationData> handler) {
		return this->onAutoModerationRuleCreationEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationRuleCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegate<void, OnAutoModerationRuleUpdateData> handler) {
		return this->onAutoModerationRuleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationRuleUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegate<void, OnAutoModerationRuleDeletionData> handler) {
		return this->onAutoModerationRuleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationRuleDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationActionExecution(DiscordCoreInternal::EventDelegate<void, OnAutoModerationActionExecutionData> handler) {
		return this->onAutoModerationActionExecutionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onAutoModerationActionExecutionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoCompleteEntry(DiscordCoreInternal::EventDelegate<void, OnAutoCompleteEntryData> handler) {
		return this->onAutoCompleteEntryEvent.add(std::move(handler));
	}

	void EventManager::onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onInputEventCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInputEventCreation(DiscordCoreInternal::EventDelegate<void, OnInputEventCreationData> handler) {
		return this->onInputEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onInputEventCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelCreation(DiscordCoreInternal::EventDelegate<void, OnChannelCreationData> handler) {
		return this->onChannelCreationEvent.add(std::move(handler));
	}

	void EventManager::onChannelCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelUpdate(DiscordCoreInternal::EventDelegate<void, OnChannelUpdateData> handler) {
		return this->onChannelUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelDeletion(DiscordCoreInternal::EventDelegate<void, OnChannelDeletionData> handler) {
		return this->onChannelDeletionEvent.add(std::move(handler));
	}

	void EventManager::onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelPinsUpdate(DiscordCoreInternal::EventDelegate<void, OnChannelPinsUpdateData> handler) {
		return this->onChannelPinsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onChannelPinsUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadCreation(DiscordCoreInternal::EventDelegate<void, OnThreadCreationData> handler) {
		return this->onThreadCreationEvent.add(std::move(handler));
	}

	void EventManager::onThreadCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadUpdate(DiscordCoreInternal::EventDelegate<void, OnThreadUpdateData> handler) {
		return this->onThreadUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadDeletion(DiscordCoreInternal::EventDelegate<void, OnThreadDeletionData> handler) {
		return this->onThreadDeletionEvent.add(std::move(handler));
	}

	void EventManager::onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadListSync(DiscordCoreInternal::EventDelegate<void, OnThreadListSyncData> handler) {
		return this->onThreadListSyncEvent.add(std::move(handler));
	}

	void EventManager::onThreadListSync(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadListSyncEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMemberUpdate(DiscordCoreInternal::EventDelegate<void, OnThreadMemberUpdateData> handler) {
		return this->onThreadMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadMemberUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMembersUpdate(DiscordCoreInternal::EventDelegate<void, OnThreadMembersUpdateData> handler) {
		return this->onThreadMembersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return this->onThreadMembersUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildCreation(DiscordCoreInternal::EventDelegate<void, OnGuildCreationData> handler) {
		return this->onGuildCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildUpdate(DiscordCoreInternal::EventDelegate<void, OnGuildUpdateData> handler) {
		return this->onGuildUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildDeletion(DiscordCoreInternal::EventDelegate<void, OnGuildDeletionData> handler) {
		return this->onGuildDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanAdd(DiscordCoreInternal::EventDelegate<void, OnGuildBanAddData> handler) {
		return this->onGuildBanAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanAddEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanRemove(DiscordCoreInternal::EventDelegate<void, OnGuildBanRemoveData> handler) {
		return this->onGuildBanRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanRemoveEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildEmojisUpdate(DiscordCoreInternal::EventDelegate<void, OnGuildEmojisUpdateData> handler) {
		return this->onGuildEmojisUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildEmojisUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildStickersUpdate(DiscordCoreInternal::EventDelegate<void, OnGuildStickersUpdateData> handler) {
		return this->onGuildStickersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildStickersUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegate<void, OnGuildIntegrationsUpdateData> handler) {
		return this->onGuildIntegrationsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildIntegrationsUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMembersChunk(DiscordCoreInternal::EventDelegate<void, OnGuildMembersChunkData> handler) {
		return this->onGuildMembersChunkEvent.add(std::move(handler));
	}

	void EventManager::onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMembersChunkEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberAdd(DiscordCoreInternal::EventDelegate<void, OnGuildMemberAddData> handler) {
		return this->onGuildMemberAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberAddEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberRemove(DiscordCoreInternal::EventDelegate<void, OnGuildMemberRemoveData> handler) {
		return this->onGuildMemberRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberRemoveEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberUpdate(DiscordCoreInternal::EventDelegate<void, OnGuildMemberUpdateData> handler) {
		return this->onGuildMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleCreation(DiscordCoreInternal::EventDelegate<void, OnRoleCreationData> handler) {
		return this->onRoleCreationEvent.add(std::move(handler));
	}

	void EventManager::onRoleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleUpdate(DiscordCoreInternal::EventDelegate<void, OnRoleUpdateData> handler) {
		return this->onRoleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleDeletion(DiscordCoreInternal::EventDelegate<void, OnRoleDeletionData> handler) {
		return this->onRoleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationCreation(DiscordCoreInternal::EventDelegate<void, OnIntegrationCreationData> handler) {
		return this->onIntegrationCreationEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationUpdate(DiscordCoreInternal::EventDelegate<void, OnIntegrationUpdateData> handler) {
		return this->onIntegrationUpdateEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationDeletion(DiscordCoreInternal::EventDelegate<void, OnIntegrationDeletionData> handler) {
		return this->onIntegrationDeletionEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInteractionCreation(DiscordCoreInternal::EventDelegate<void, OnInteractionCreationData> handler) {
		return this->onInteractionCreationEvent.add(std::move(handler));
	}

	void EventManager::onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInteractionCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteCreation(DiscordCoreInternal::EventDelegate<void, OnInviteCreationData> handler) {
		return this->onInviteCreationEvent.add(std::move(handler));
	}

	void EventManager::onInviteCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteDeletion(DiscordCoreInternal::EventDelegate<void, OnInviteDeletionData> handler) {
		return this->onInviteDeletionEvent.add(std::move(handler));
	}

	void EventManager::onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageCreation(DiscordCoreInternal::EventDelegate<void, OnMessageCreationData> handler) {
		return this->onMessageCreationEvent.add(std::move(handler));
	}

	void EventManager::onMessageCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageUpdate(DiscordCoreInternal::EventDelegate<void, OnMessageUpdateData> handler) {
		return this->onMessageUpdateEvent.add(std::move(handler));
	}

	void EventManager::onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeletion(DiscordCoreInternal::EventDelegate<void, OnMessageDeletionData> handler) {
		return this->onMessageDeletionEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeleteBulk(DiscordCoreInternal::EventDelegate<void, OnMessageDeleteBulkData> handler) {
		return this->onMessageDeleteBulkEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeleteBulkEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionAdd(DiscordCoreInternal::EventDelegate<void, OnReactionAddData> handler) {
		return this->onReactionAddEvent.add(std::move(handler));
	}

	void EventManager::onReactionAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionAddEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemove(DiscordCoreInternal::EventDelegate<void, OnReactionRemoveData> handler) {
		return this->onReactionRemoveEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveAll(DiscordCoreInternal::EventDelegate<void, OnReactionRemoveAllData> handler) {
		return this->onReactionRemoveAllEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveAllEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveEmoji(DiscordCoreInternal::EventDelegate<void, OnReactionRemoveEmojiData> handler) {
		return this->onReactionRemoveEmojiEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEmojiEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onPresenceUpdate(DiscordCoreInternal::EventDelegate<void, OnPresenceUpdateData> handler) {
		return this->onPresenceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onPresenceUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceCreation(DiscordCoreInternal::EventDelegate<void, OnStageInstanceCreationData> handler) {
		return this->onStageInstanceCreationEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceCreationEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceUpdate(DiscordCoreInternal::EventDelegate<void, OnStageInstanceUpdateData> handler) {
		return this->onStageInstanceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceDeletion(DiscordCoreInternal::EventDelegate<void, OnStageInstanceDeletionData> handler) {
		return this->onStageInstanceDeletionEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceDeletionEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onTypingStart(DiscordCoreInternal::EventDelegate<void, OnTypingStartData> handler) {
		return this->onTypingStartEvent.add(std::move(handler));
	}

	void EventManager::onTypingStart(DiscordCoreInternal::EventDelegateToken& token) {
		onTypingStartEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onUserUpdate(DiscordCoreInternal::EventDelegate<void, OnUserUpdateData> handler) {
		return this->onUserUpdateEvent.add(std::move(handler));
	}

	void EventManager::onUserUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onUserUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceStateUpdate(DiscordCoreInternal::EventDelegate<void, OnVoiceStateUpdateData> handler) {
		return this->onVoiceStateUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceStateUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceServerUpdate(DiscordCoreInternal::EventDelegate<void, OnVoiceServerUpdateData> handler) {
		return this->onVoiceServerUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceServerUpdateEvent.remove(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onWebhookUpdate(DiscordCoreInternal::EventDelegate<void, OnWebhookUpdateData> handler) {
		return this->onWebhookUpdateEvent.add(std::move(handler));
	}

	void EventManager::onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onWebhookUpdateEvent.remove(token);
	}

	void EventHandler::initialize(ConfigManager* configManager) {
		EventHandler::configManager = configManager;
	}

	void EventHandler::onChannelCreation(OnChannelCreationData dataPackage) {
		if (EventHandler::configManager->doWeCacheChannels()) {
			Channels::insertChannel(dataPackage.channel);
			GuildData guild = Guilds::getCachedGuildAsync({ dataPackage.channel.guildId }).get();
			guild.channels.push_back(dataPackage.channel.id);
			Guilds::insertGuild(guild);
		}
	}

	void EventHandler::onChannelUpdate(OnChannelUpdateData dataPackage) {
		if (EventHandler::configManager->doWeCacheChannels()) {
			Channels::insertChannel(dataPackage.channelNew);
		}
	}

	void EventHandler::onChannelDeletion(OnChannelDeletionData dataPackage) {
		if (EventHandler::configManager->doWeCacheChannels()) {
			Channels::removeChannel(dataPackage.channel.id);
			GuildData guild = Guilds::getCachedGuildAsync({ dataPackage.channel.guildId }).get();
			for (uint64_t x = 0; x < guild.channels.size(); x++) {
				if (guild.channels[x] == dataPackage.channel.id) {
					guild.channels.erase(guild.channels.begin() + x);
				}
			}
			Guilds::insertGuild(guild);
		}
	}

	void EventHandler::onGuildCreation(OnGuildCreationData dataPackage) {
		if (EventHandler::configManager->doWeCacheGuilds()) {
			Guilds::insertGuild(dataPackage.guild);
		}
	}

	void EventHandler::onGuildUpdate(OnGuildUpdateData dataPackage) {
		if (EventHandler::configManager->doWeCacheGuilds()) {
			Guilds::insertGuild(dataPackage.guildNew);
		}
	}

	void EventHandler::onGuildDeletion(OnGuildDeletionData dataPackage) {
		if (EventHandler::configManager->doWeCacheGuilds()) {
			for (auto& value: dataPackage.guild.members) {
				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = value, .guildId = dataPackage.guild.id }).get();
				GuildMembers::removeGuildMember(guildMember);
			}
			for (auto& value: dataPackage.guild.channels) {
				Channels::removeChannel(value);
			}
			for (auto& value: dataPackage.guild.roles) {
				Roles::removeRole(value);
			}
			Guilds::removeGuild(dataPackage.guild.id);
		}
	}

	void EventHandler::onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
		if (EventHandler::configManager->doWeCacheGuildMembers()) {
			GuildMembers::insertGuildMember(dataPackage.guildMember);
			GuildData guild = Guilds::getCachedGuildAsync({ dataPackage.guildMember.guildId }).get();
			guild.members.push_back(dataPackage.guildMember.id);
			guild.memberCount++;
			Guilds::insertGuild(guild);
		}
	}

	void EventHandler::onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
		if (EventHandler::configManager->doWeCacheGuildMembers()) {
			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.user.id, .guildId = dataPackage.guildId }).get();
			std::string globalId = std::to_string(guildMember.guildId) + " + " + std::to_string(guildMember.id);
			GuildMembers::removeGuildMember(guildMember);
			GuildData guild = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get();
			for (uint64_t x = 0; x < guild.members.size(); x++) {
				if (guild.members[x] == dataPackage.user.id) {
					guild.members.erase(guild.members.begin() + x);
				}
			}
			guild.memberCount--;
			Guilds::insertGuild(guild);
		}
	}

	void EventHandler::onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
		if (EventHandler::configManager->doWeCacheGuildMembers()) {
			GuildMembers::insertGuildMember(dataPackage.guildMemberNew);
		}
	}

	void EventHandler::onRoleCreation(OnRoleCreationData dataPackage) {
		if (EventHandler::configManager->doWeCacheRoles()) {
			Roles::insertRole(dataPackage.role);
			GuildData guild = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get();
			guild.roles.push_back(dataPackage.role.id);
			Guilds::insertGuild(guild);
		}
	}

	void EventHandler::onRoleUpdate(OnRoleUpdateData dataPackage) {
		if (EventHandler::configManager->doWeCacheRoles()) {
			Roles::insertRole(dataPackage.roleNew);
		}
	}

	void EventHandler::onRoleDeletion(OnRoleDeletionData dataPackage) {
		if (EventHandler::configManager->doWeCacheRoles()) {
			Roles::removeRole(dataPackage.roleOld.id);
			GuildData guild = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get();
			for (uint64_t x = 0; x < guild.roles.size(); x++) {
				if (guild.roles[x] == dataPackage.roleOld.id) {
					guild.roles.erase(guild.roles.begin() + x);
				}
			}
			Guilds::insertGuild(guild);
		}
	}

	void EventHandler::onUserUpdate(OnUserUpdateData dataPackage) {
		if (EventHandler::configManager->doWeCacheUsers()) {
			Users::insertUser(dataPackage.userNew);
		}
	}

	void EventHandler::onVoiceStateUpdate(OnVoiceStateUpdateData dataPackage) {
		if (EventHandler::configManager->doWeCacheGuildMembers() && EventHandler::configManager->doWeCacheGuilds()) {
			GuildData guild = Guilds::getCachedGuildAsync({ .guildId = dataPackage.voiceStateData.guildId }).get();
			guild.voiceStates[dataPackage.voiceStateData.userId] = dataPackage.voiceStateData;
			Guilds::insertGuild(guild);
		}
	}

	ConfigManager* EventHandler::configManager{};
};