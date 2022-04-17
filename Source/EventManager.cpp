/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

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
/// Chris M.
/// https://discordcoreapi.com
/// \file EventManager.cpp

#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	EventDelegateToken EventManager::onInputEventCreation(EventDelegate<void, OnInputEventCreationData> handler) {
		return onInputEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onInputEventCreation(EventDelegateToken token) {
		return onInputEventCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onApplicationCommandCreation(EventDelegate<void, OnApplicationCommandCreationData> handler) {
		return onApplicationCommandCreationEvent.add(std::move(handler));
	}

	void EventManager::onApplicationCommandCreation(EventDelegateToken token) {
		return onApplicationCommandCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onApplicationCommandUpdate(EventDelegate<void, OnApplicationCommandUpdateData> handler) {
		return onApplicationCommandUpdateEvent.add(std::move(handler));
	}

	void EventManager::onApplicationCommandUpdate(EventDelegateToken token) {
		return onApplicationCommandUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onApplicationCommandDeletion(EventDelegate<void, OnApplicationCommandDeletionData> handler) {
		return onApplicationCommandDeletionEvent.add(std::move(handler));
	}

	void EventManager::onApplicationCommandDeletion(EventDelegateToken token) {
		return onApplicationCommandDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onChannelCreation(EventDelegate<void, OnChannelCreationData> handler) {
		return onChannelCreationEvent.add(std::move(handler));
	}

	void EventManager::onChannelCreation(EventDelegateToken token) {
		onChannelCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onChannelUpdate(EventDelegate<void, OnChannelUpdateData> handler) {
		return onChannelUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelUpdate(EventDelegateToken token) {
		onChannelUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onChannelDeletion(EventDelegate<void, OnChannelDeletionData> handler) {
		return onChannelDeletionEvent.add(std::move(handler));
	}

	void EventManager::onChannelDeletion(EventDelegateToken token) {
		onChannelDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onChannelPinsUpdate(EventDelegate<void, OnChannelPinsUpdateData> handler) {
		return onChannelPinsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelPinsUpdate(EventDelegateToken token) {
		return onChannelPinsUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onThreadCreation(EventDelegate<void, OnThreadCreationData> handler) {
		return onThreadCreationEvent.add(std::move(handler));
	}

	void EventManager::onThreadCreation(EventDelegateToken token) {
		return onThreadCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onThreadUpdate(EventDelegate<void, OnThreadUpdateData> handler) {
		return onThreadUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadUpdate(EventDelegateToken token) {
		return onThreadUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onThreadDeletion(EventDelegate<void, OnThreadDeletionData> handler) {
		return onThreadDeletionEvent.add(std::move(handler));
	}

	void EventManager::onThreadDeletion(EventDelegateToken token) {
		return onThreadDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onThreadListSync(EventDelegate<void, OnThreadListSyncData> handler) {
		return onThreadListSyncEvent.add(std::move(handler));
	}

	void EventManager::onThreadListSync(EventDelegateToken token) {
		return onThreadListSyncEvent.remove(token);
	}

	EventDelegateToken EventManager::onThreadMemberUpdate(EventDelegate<void, OnThreadMemberUpdateData> handler) {
		return onThreadMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMemberUpdate(EventDelegateToken token) {
		return onThreadMemberUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onThreadMembersUpdate(EventDelegate<void, OnThreadMembersUpdateData> handler) {
		return onThreadMembersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMembersUpdate(EventDelegateToken token) {
		return onThreadMembersUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildCreation(EventDelegate<void, OnGuildCreationData> handler) {
		return onGuildCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildCreation(EventDelegateToken token) {
		onGuildCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildUpdate(EventDelegate<void, OnGuildUpdateData> handler) {
		return onGuildUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildUpdate(EventDelegateToken token) {
		onGuildUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildDeletion(EventDelegate<void, OnGuildDeletionData> handler) {
		return onGuildDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildDeletion(EventDelegateToken token) {
		onGuildDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildBanAdd(EventDelegate<void, OnGuildBanAddData> handler) {
		return onGuildBanAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanAdd(EventDelegateToken token) {
		onGuildBanAddEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildBanRemove(EventDelegate<void, OnGuildBanRemoveData> handler) {
		return onGuildBanRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanRemove(EventDelegateToken token) {
		onGuildBanRemoveEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildEmojisUpdate(EventDelegate<void, OnGuildEmojisUpdateData> handler) {
		return onGuildEmojisUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildEmojisUpdate(EventDelegateToken token) {
		onGuildEmojisUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildStickersUpdate(EventDelegate<void, OnGuildStickersUpdateData> handler) {
		return onGuildStickersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildStickersUpdate(EventDelegateToken token) {
		onGuildStickersUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildIntegrationsUpdate(EventDelegate<void, OnGuildIntegrationsUpdateData> handler) {
		return onGuildIntegrationsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildIntegrationsUpdate(EventDelegateToken token) {
		onGuildIntegrationsUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildMembersChunk(EventDelegate<void, OnGuildMembersChunkData> handler) {
		return onGuildMembersChunkEvent.add(std::move(handler));
	}

	void EventManager::onGuildMembersChunk(EventDelegateToken token) {
		onGuildMembersChunkEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildMemberAdd(EventDelegate<void, OnGuildMemberAddData> handler) {
		return onGuildMemberAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberAdd(EventDelegateToken token) {
		onGuildMemberAddEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildMemberRemove(EventDelegate<void, OnGuildMemberRemoveData> handler) {
		return onGuildMemberRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberRemove(EventDelegateToken token) {
		onGuildMemberRemoveEvent.remove(token);
	}

	EventDelegateToken EventManager::onGuildMemberUpdate(EventDelegate<void, OnGuildMemberUpdateData> handler) {
		return onGuildMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberUpdate(EventDelegateToken token) {
		onGuildMemberUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onRoleCreation(EventDelegate<void, OnRoleCreationData> handler) {
		return onRoleCreationEvent.add(std::move(handler));
	}

	void EventManager::onRoleCreation(EventDelegateToken token) {
		onRoleCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onRoleUpdate(EventDelegate<void, OnRoleUpdateData> handler) {
		return onRoleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onRoleUpdate(EventDelegateToken token) {
		onRoleUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onRoleDeletion(EventDelegate<void, OnRoleDeletionData> handler) {
		return onRoleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onRoleDeletion(EventDelegateToken token) {
		onRoleDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onIntegrationCreation(EventDelegate<void, OnIntegrationCreationData> handler) {
		return onIntegrationCreationEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationCreation(EventDelegateToken token) {
		onIntegrationCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onIntegrationUpdate(EventDelegate<void, OnIntegrationUpdateData> handler) {
		return onIntegrationUpdateEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationUpdate(EventDelegateToken token) {
		onIntegrationUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onIntegrationDeletion(EventDelegate<void, OnIntegrationDeletionData> handler) {
		return onIntegrationDeletionEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationDeletion(EventDelegateToken token) {
		onIntegrationDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onInteractionCreation(EventDelegate<void, OnInteractionCreationData> handler) {
		return onInteractionCreationEvent.add(std::move(handler));
	}

	void EventManager::onInteractionCreation(EventDelegateToken token) {
		onInteractionCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onInviteCreation(EventDelegate<void, OnInviteCreationData> handler) {
		return onInviteCreationEvent.add(std::move(handler));
	}

	void EventManager::onInviteCreation(EventDelegateToken token) {
		onInviteCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onInviteDeletion(EventDelegate<void, OnInviteDeletionData> handler) {
		return onInviteDeletionEvent.add(std::move(handler));
	}

	void EventManager::onInviteDeletion(EventDelegateToken token) {
		onInviteDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onMessageCreation(EventDelegate<void, OnMessageCreationData> handler) {
		return onMessageCreationEvent.add(std::move(handler));
	}

	void EventManager::onMessageCreation(EventDelegateToken token) {
		onMessageCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onMessageUpdate(EventDelegate<void, OnMessageUpdateData> handler) {
		return onMessageUpdateEvent.add(std::move(handler));
	}

	void EventManager::onMessageUpdate(EventDelegateToken token) {
		onMessageUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onMessageDeletion(EventDelegate<void, OnMessageDeletionData> handler) {
		return onMessageDeletionEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeletion(EventDelegateToken token) {
		onMessageDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onMessageDeleteBulk(EventDelegate<void, OnMessageDeleteBulkData> handler) {
		return onMessageDeleteBulkEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeleteBulk(EventDelegateToken token) {
		onMessageDeleteBulkEvent.remove(token);
	}

	EventDelegateToken EventManager::onReactionAdd(EventDelegate<void, OnReactionAddData> handler) {
		return onReactionAddEvent.add(std::move(handler));
	}

	void EventManager::onReactionAdd(EventDelegateToken token) {
		onReactionAddEvent.remove(token);
	}

	EventDelegateToken EventManager::onReactionRemove(EventDelegate<void, OnReactionRemoveData> handler) {
		return onReactionRemoveEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemove(EventDelegateToken token) {
		onReactionRemoveEvent.remove(token);
	}

	EventDelegateToken EventManager::onReactionRemoveAll(EventDelegate<void, OnReactionRemoveAllData> handler) {
		return onReactionRemoveAllEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveAll(EventDelegateToken token) {
		onReactionRemoveAllEvent.remove(token);
	}

	EventDelegateToken EventManager::onReactionRemoveEmoji(EventDelegate<void, OnReactionRemoveEmojiData> handler) {
		return onReactionRemoveEmojiEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveEmoji(EventDelegateToken token) {
		onReactionRemoveEmojiEvent.remove(token);
	}

	EventDelegateToken EventManager::onPresenceUpdate(EventDelegate<void, OnPresenceUpdateData> handler) {
		return onPresenceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onPresenceUpdate(EventDelegateToken token) {
		onPresenceUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onStageInstanceCreation(EventDelegate<void, OnStageInstanceCreationData> handler) {
		return onStageInstanceCreationEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceCreation(EventDelegateToken token) {
		onStageInstanceCreationEvent.remove(token);
	}

	EventDelegateToken EventManager::onStageInstanceUpdate(EventDelegate<void, OnStageInstanceUpdateData> handler) {
		return onStageInstanceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceUpdate(EventDelegateToken token) {
		onStageInstanceUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onStageInstanceDeletion(EventDelegate<void, OnStageInstanceDeletionData> handler) {
		return onStageInstanceDeletionEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceDeletion(EventDelegateToken token) {
		onStageInstanceDeletionEvent.remove(token);
	}

	EventDelegateToken EventManager::onTypingStart(EventDelegate<void, OnTypingStartData> handler) {
		return onTypingStartEvent.add(std::move(handler));
	}

	void EventManager::onTypingStart(EventDelegateToken token) {
		onTypingStartEvent.remove(token);
	}

	EventDelegateToken EventManager::onUserUpdate(EventDelegate<void, OnUserUpdateData> handler) {
		return onUserUpdateEvent.add(std::move(handler));
	}

	void EventManager::onUserUpdate(EventDelegateToken token) {
		onUserUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onVoiceStateUpdate(EventDelegate<void, OnVoiceStateUpdateData> handler) {
		return onVoiceStateUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceStateUpdate(EventDelegateToken token) {
		onVoiceStateUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onVoiceServerUpdate(EventDelegate<void, OnVoiceServerUpdateData> handler) {
		return onVoiceServerUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceServerUpdate(EventDelegateToken token) {
		onVoiceServerUpdateEvent.remove(token);
	}

	EventDelegateToken EventManager::onWebhookUpdate(EventDelegate<void, OnWebhookUpdateData> handler) {
		return onWebhookUpdateEvent.add(std::move(handler));
	}

	void EventManager::onWebhookUpdate(EventDelegateToken token) {
		onWebhookUpdateEvent.remove(token);
	}

	void EventHandler::initialize(CacheOptions newOptions) {
		EventHandler::options = newOptions;
	}

	void EventHandler::onChannelCreation(OnChannelCreationData dataPackage) {
		try {
			if (EventHandler::options.cacheChannels) {
				Channels::insertChannel(dataPackage.channel);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.channel.guildId }).get();
				guild.channels.insert_or_assign(dataPackage.channel.id, dataPackage.channel);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onChannelCreation()");
		}
	}

	void EventHandler::onChannelUpdate(OnChannelUpdateData dataPackage) {
		try {
			if (EventHandler::options.cacheChannels) {
				Channels::insertChannel(dataPackage.channelNew);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.channelNew.guildId }).get();
				guild.channels.insert_or_assign(dataPackage.channelNew.id, dataPackage.channelNew);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onChannelUpdate()");
		}
	}

	void EventHandler::onChannelDeletion(OnChannelDeletionData dataPackage) {
		try {
			if (EventHandler::options.cacheChannels) {
				Channels::removeChannel(dataPackage.channel.id);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.channel.guildId }).get();
				guild.channels.erase(dataPackage.channel.id);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onChannelDeletion()");
		}
	}

	void EventHandler::onGuildCreation(OnGuildCreationData dataPackage) {
		try {
			if (EventHandler::options.cacheGuilds) {
				Guilds::insertGuild(dataPackage.guild);
			}
		} catch (...) {
			reportException("EventHandler::onGuildCreation()");
		}
	}

	void EventHandler::onGuildUpdate(OnGuildUpdateData dataPackage) {
		try {
			if (EventHandler::options.cacheGuilds) {
				Guilds::insertGuild(dataPackage.guildNew);
			}
		} catch (...) {
			reportException("EventHandler::onGuildUpdate()");
		}
	}

	void EventHandler::onGuildDeletion(OnGuildDeletionData dataPackage) {
		try {
			if (EventHandler::options.cacheGuilds) {
				for (auto& [key, value]: dataPackage.guild.members) {
					GuildMembers::removeGuildMember(value);
				}
				for (auto& [key, value]: dataPackage.guild.channels) {
					Channels::removeChannel(value.id);
				}
				for (auto& [key, value]: dataPackage.guild.roles) {
					Roles::removeRole(value.id);
				}
				Guilds::removeGuild(dataPackage.guild.id);
			}
		} catch (...) {
			reportException("EventHandler::onGuildDeletion()");
		}
	}

	void EventHandler::onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
		try {
			if (EventHandler::options.cacheGuildMembers) {
				GuildMembers::insertGuildMember(dataPackage.guildMember);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.guildMember.guildId }).get();
				guild.members.insert_or_assign(dataPackage.guildMember.user.id, dataPackage.guildMember);
				guild.memberCount += 1;
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onGuildMemberAdd()");
		}
	}

	void EventHandler::onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
		try {
			if (EventHandler::options.cacheGuildMembers) {
				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = dataPackage.user.id, .guildId = dataPackage.guildId }).get();
				GuildMembers::removeGuildMember(guildMember);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get();
				guild.members.erase(guildMember.user.id);
				guild.memberCount -= 1;
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onGuildMemberRemove()");
		}
	}

	void EventHandler::onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
		try {
			if (EventHandler::options.cacheGuildMembers) {
				GuildMembers::insertGuildMember(dataPackage.guildMemberNew);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.guildMemberNew.guildId }).get();
				guild.members.insert_or_assign(dataPackage.guildMemberNew.user.id, dataPackage.guildMemberNew);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onGuildMemberUpdate()");
		}
	}

	void EventHandler::onRoleCreation(OnRoleCreationData dataPackage) {
		try {
			if (EventHandler::options.cacheRoles) {
				Roles::insertRole(dataPackage.role);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get();
				guild.roles.insert_or_assign(dataPackage.role.id, dataPackage.role);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onRoleCreation()");
		}
	}

	void EventHandler::onRoleUpdate(OnRoleUpdateData dataPackage) {
		try {
			if (EventHandler::options.cacheRoles) {
				Roles::insertRole(dataPackage.roleNew);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get();
				guild.roles.insert_or_assign(dataPackage.roleNew.id, dataPackage.roleNew);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onRoleUpdate()");
		}
	}

	void EventHandler::onRoleDeletion(OnRoleDeletionData dataPackage) {
		try {
			if (EventHandler::options.cacheRoles) {
				Roles::removeRole(dataPackage.roleOld.id);
				Guild guild = Guilds::getCachedGuildAsync({ dataPackage.guildId }).get();
				guild.roles.erase(dataPackage.roleOld.id);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onRoleDeletion()");
		}
	}

	void EventHandler::onUserUpdate(OnUserUpdateData dataPackage) {
		if (EventHandler::options.cacheUsers) {
			Users::insertUser(dataPackage.userNew);
		}
	}

	void EventHandler::onVoiceStateUpdate(OnVoiceStateUpdateData dataPackage) {
		try {
			if (EventHandler::options.cacheGuildMembers && EventHandler::options.cacheGuilds) {
				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync(
					{ .guildMemberId = dataPackage.voiceStateData.userId, .guildId = dataPackage.voiceStateData.guildId })
											  .get();
				guildMember.voiceData = dataPackage.voiceStateData;
				GuildMembers::insertGuildMember(guildMember);
				Guild guild = Guilds::getCachedGuildAsync({ .guildId = dataPackage.voiceStateData.guildId }).get();
				guild.voiceStates.insert_or_assign(dataPackage.voiceStateData.userId, dataPackage.voiceStateData);
				Guilds::insertGuild(guild);
			}
		} catch (...) {
			reportException("EventHandler::onVoiceStateUpdate()");
		}
	}

	CacheOptions EventHandler::options{};
};