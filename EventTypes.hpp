// EventTypes.hpp - Header for the event types.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_TYPES_
#define _EVENT_TYPES_

#include "../pch.h"
#include "GuildMemberEntities.hpp"
#include "ApplicationCommandEntities.hpp"

namespace DiscordCoreAPI {

	struct OnApplicationCommandCreationData {
		ApplicationCommand applicationCommand{};
	};

	struct OnApplicationCommandUpdateData {
		ApplicationCommand applicationCommand{};
	};

	struct OnApplicationCommandDeletionData {
		ApplicationCommand applicationCommand{};
	};

	struct OnChannelCreationData {
		Channel channel{};
	};

	struct OnChannelUpdateData {
		Channel channelOld{};
		Channel channelNew{};
	};

	struct OnChannelDeletionData {
		Channel channel{};
	};

	struct OnChannelPinsUpdateData {
		ChannelPinsUpdateEventData dataPackage{};
	};

	struct OnThreadCreationData {
		Channel channel{};
	};

	struct OnThreadUpdateData {
		Channel channel{};
	};

	struct OnThreadDeletionData {
		Channel channel{};
	};

	struct OnThreadListSyncData {
		ThreadListSyncData threadListSyncData{};
	};

	struct OnThreadMemberUpdateData {
		ThreadMemberData threadMember{};
	};

	struct OnThreadMembersUpdateData {
		ThreadMembersUpdateData threadMembersUpdateData{};
	};

	struct OnGuildCreationData {
		Guild guild{};
	};

	struct OnGuildUpdateData {
		Guild guildOld{};
		Guild guildNew{};
	};

	struct OnGuildDeletionData {
		Guild guild{};
	};

	struct OnGuildBanAddData {
		string guildId{ "" };
		User user{};
	};

	struct OnGuildBanRemoveData {
		string guildId{ "" };
		User user{};
	};

	struct OnGuildEmojisUpdateData {
		GuildEmojisUpdateEventData updateData{};
	};

	struct OnGuildStickersUpdateData {
		GuildStickersUpdateEventData updateData{};
	};

	struct OnGuildIntegrationsUpdateData {
		string guildId{ "" };
	};

	struct OnGuildMemberAddData {
		GuildMember guildMember{};
	};

	struct OnGuildMemberRemoveData {
		User user{};
		string guildId{ "" };
	};

	struct OnGuildMemberUpdateData {
		GuildMember guildMemberOld{};
		GuildMember guildMemberNew{};
	};

	struct OnGuildMembersChunkData {
		GuildMembersChunkEventData chunkEventData{};
	};

	struct OnRoleCreationData {
		Role role{};
		string guildId{ "" };
	};

	struct OnRoleUpdateData {
		string guildId{ "" };
		Role roleOld{};
		Role roleNew{};
	};

	struct OnRoleDeletionData {
		string guildId{ "" };
		Role roleOld{};		
	};

	struct OnIntegrationCreationData {
		IntegrationData integrationData{};
		string guildId{ "" };
	};

	struct OnIntegrationUpdateData {
		IntegrationData integrationData{};
		string guildId{ "" };
	};

	struct OnIntegrationDeletionData {
		string applicationId{ "" };
		string guildId{ "" };
		string id{ "" };
	};

	struct OnInviteCreationData {
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		InviteData invite{};
	};

	struct OnInviteDeletionData {
		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		string channelId{ "" };
		string guildId{ "" };
		string code{ "" };
	};

	struct OnInteractionCreationData {
		InputEventData eventData{};
	};

	struct OnMessageCreationData {
		Message message{};
	};

	struct OnMessageUpdateData {
		Message messageNew{};
	};

	struct OnMessageDeletionData {
		string messageId{ "" };
		string channelId{ "" };
		string guildId{ "" };
	};

	struct OnMessageDeleteBulkData {
		string channelId{ "" };
		string guildId{ "" };
		vector<string> ids{};
	};

	struct OnReactionAddData {
		Reaction reaction{};
	};

	struct OnReactionRemoveData {
		ReactionRemoveData reactionRemoveData{};
	};

	struct OnReactionRemoveAllData {
		string channelId{ "" };
		string messageId{ "" };
		string guildId{ "" };
	};

	struct OnReactionRemoveEmojiData {
		string messageId{ "" };
		string channelId{ "" };
		string guildId{ "" };
		EmojiData emoji{};
	};

	struct OnPresenceUpdateData {
		PresenceUpdateData presenceData{};
	};

	struct OnStageInstanceCreationData {
		StageInstanceData stageInstance{};
	};

	struct OnStageInstanceUpdateData {
		StageInstanceData stageInstance{};
	};

	struct OnStageInstanceDeletionData {
		StageInstanceData stageInstance{};
	};

	struct OnTypingStartData {
		TypingStartData typingStartData{};
	};

	struct OnUserUpdateData {
		User userOld{};
		User userNew{};
	};

	struct OnVoiceStateUpdateData {
		VoiceStateData voiceStateData{};
	};

	struct OnVoiceServerUpdateData {
		string endpoint{ "" };
		string guildId{ "" };
		string token{ "" };
	};

	struct OnWebhookUpdateData {
		string channelId{ "" };
		string guildId{ "" };
	};

	class EventManager {
	public:

		friend class DiscordCoreClient;

		event_token onApplicationCommandCreation(delegate<OnApplicationCommandCreationData> const& handler) {
			return onApplicationCommandCreationEvent.add(handler);
		}

		void onApplicationCommandCreation(event_token const& handler) {
			return onApplicationCommandCreationEvent.remove(handler);
		}

		event_token onApplicationCommandUpdate(delegate<OnApplicationCommandUpdateData> const& handler) {
			return onApplicationCommandUpdateEvent.add(handler);
		}

		void onApplicationCommandUpdate(event_token const& handler) {
			return onApplicationCommandUpdateEvent.remove(handler);
		}

		event_token onApplicationCommandDeletion(delegate<OnApplicationCommandDeletionData> const& handler) {
			return onApplicationCommandDeletionEvent.add(handler);
		}

		void onApplicationCommandDeletion(event_token const& handler) {
			return onApplicationCommandDeletionEvent.remove(handler);
		}

		event_token onChannelCreation(delegate<OnChannelCreationData> const& handler) {
			return onChannelCreationEvent.add(handler);
		}

		void onChannelCreation(event_token const& token) {
			onChannelCreationEvent.remove(token);
		}

		event_token onChannelUpdate(delegate<OnChannelUpdateData> const& handler) {
			return onChannelUpdateEvent.add(handler);
		}

		void onChannelUpdate(event_token const& token) {
			onChannelUpdateEvent.remove(token);
		}

		event_token onChannelDeletion(delegate<OnChannelDeletionData> const& handler) {
			return onChannelDeletionEvent.add(handler);
		}

		void onChannelDeletion(event_token const& token) {
			onChannelDeletionEvent.remove(token);
		}

		event_token onChannelPinsUpdate(delegate<OnChannelPinsUpdateData> const& handler) {
			return onChannelPinsUpdateEvent.add(handler);
		}

		void onChannelPinsUpdate(event_token const& handler) {
			return onChannelPinsUpdateEvent.remove(handler);
		}

		event_token onThreadCreation(delegate<OnThreadCreationData> const& handler) {
			return onThreadCreationEvent.add(handler);
		}

		void onThreadCreation(event_token const& handler) {
			return onThreadCreationEvent.remove(handler);
		}

		event_token onThreadUpdate(delegate<OnThreadUpdateData> const& handler) {
			return onThreadUpdateEvent.add(handler);
		}

		void onThreadUpdate(event_token const& handler) {
			return onThreadUpdateEvent.remove(handler);
		}

		event_token onThreadDeletion(delegate<OnThreadDeletionData> const& handler) {
			return onThreadDeletionEvent.add(handler);
		}

		void onThreadDeletion(event_token const& handler) {
			return onThreadDeletionEvent.remove(handler);
		}

		event_token onThreadListSync(delegate<OnThreadListSyncData> const& handler) {
			return onThreadListSyncEvent.add(handler);
		}

		void onThreadListSync(event_token const& handler) {
			return onThreadListSyncEvent.remove(handler);
		}

		event_token onThreadMemberUpdate(delegate<OnThreadMemberUpdateData> const& handler) {
			return onThreadMemberUpdateEvent.add(handler);
		}

		void onThreadMemberUpdate(event_token const& handler) {
			return onThreadMemberUpdateEvent.remove(handler);
		}

		event_token onThreadMembersUpdate(delegate<OnThreadMembersUpdateData> const& handler) {
			return onThreadMembersUpdateEvent.add(handler);
		}

		void onThreadMembersUpdate(event_token const& handler) {
			return onThreadMembersUpdateEvent.remove(handler);
		}

		event_token onGuildCreation(delegate<OnGuildCreationData> const& handler) {
			return onGuildCreationEvent.add(handler);
		}

		void onGuildCreation(event_token const& token) {
			onGuildCreationEvent.remove(token);
		}

		event_token onGuildUpdate(delegate<OnGuildUpdateData> const& handler) {
			return onGuildUpdateEvent.add(handler);
		}

		void onGuildUpdate(event_token const& token) {
			onGuildUpdateEvent.remove(token);
		}

		event_token onGuildDeletion(delegate<OnGuildDeletionData> const& handler) {
			return onGuildDeletionEvent.add(handler);
		}

		void onGuildDeletion(event_token const& token) {
			onGuildDeletionEvent.remove(token);
		}

		event_token onGuildBanAdd(delegate<OnGuildBanAddData> const& handler) {
			return onGuildBanAddEvent.add(handler);
		}

		void onGuildBanAdd(event_token const& token) {
			onGuildBanAddEvent.remove(token);
		}

		event_token onGuildBanRemove(delegate<OnGuildBanRemoveData> const& handler) {
			return onGuildBanRemoveEvent.add(handler);
		}

		void onGuildBanRemove(event_token const& token) {
			onGuildBanRemoveEvent.remove(token);
		}

		event_token onGuildEmojisUpdate(delegate<OnGuildEmojisUpdateData> const& handler) {
			return onGuildEmojisUpdateEvent.add(handler);
		}

		void onGuildEmojisUpdate(event_token const& token) {
			onGuildEmojisUpdateEvent.remove(token);
		}

		event_token onGuildStickersUpdate(delegate<OnGuildStickersUpdateData> const& handler) {
			return onGuildStickersUpdateEvent.add(handler);
		}

		void onGuildStickersUpdate(event_token const& token) {
			onGuildStickersUpdateEvent.remove(token);
		}

		event_token onGuildIntegrationsUpdate(delegate<OnGuildIntegrationsUpdateData> const& handler) {
			return onGuildIntegrationsUpdateEvent.add(handler);
		}

		void onGuildIntegrationsUpdate(event_token const& token) {
			onGuildIntegrationsUpdateEvent.remove(token);
		}

		event_token onGuildMembersChunk(delegate<OnGuildMembersChunkData> const& handler) {
			return onGuildMembersChunkEvent.add(handler);
		}

		void onGuildMembersChunk(event_token const& token) {
			onGuildMembersChunkEvent.remove(token);
		}

		event_token onGuildMemberAdd(delegate<OnGuildMemberAddData> const& handler) {
			return onGuildMemberAddEvent.add(handler);
		}

		void onGuildMemberAdd(event_token const& token) {
			onGuildMemberAddEvent.remove(token);
		}

		event_token onGuildMemberRemove(delegate<OnGuildMemberRemoveData> const& handler) {
			return onGuildMemberRemoveEvent.add(handler);
		}

		void onGuildMemberRemove(event_token const& token) {
			onGuildMemberRemoveEvent.remove(token);
		}

		event_token onGuildMemberUpdate(delegate<OnGuildMemberUpdateData> const& handler) {
			return onGuildMemberUpdateEvent.add(handler);
		}

		void onGuildMemberUpdate(event_token const& token) {
			onGuildMemberUpdateEvent.remove(token);
		}

		event_token onRoleCreation(delegate<OnRoleCreationData> const& handler) {
			return onRoleCreationEvent.add(handler);
		}

		void onRoleCreation(event_token const& token) {
			onRoleCreationEvent.remove(token);
		}

		event_token onRoleUpdate(delegate<OnRoleUpdateData> const& handler) {
			return onRoleUpdateEvent.add(handler);
		}

		void onRoleUpdate(event_token const& token) {
			onRoleUpdateEvent.remove(token);
		}

		event_token onRoleDeletion(delegate<OnRoleDeletionData> const& handler) {
			return onRoleDeletionEvent.add(handler);
		}

		void onRoleDeletion(event_token const& token) {
			onRoleDeletionEvent.remove(token);
		}

		event_token onIntegrationCreation(delegate<OnIntegrationCreationData> const& handler) {
			return onIntegrationCreationEvent.add(handler);
		}

		void onIntegrationCreation(event_token const& token) {
			onIntegrationCreationEvent.remove(token);
		}

		event_token onIntegrationUpdate(delegate<OnIntegrationUpdateData> const& handler) {
			return onIntegrationUpdateEvent.add(handler);
		}

		void onIntegrationUpdate(event_token const& token) {
			onIntegrationUpdateEvent.remove(token);
		}

		event_token onIntegrationDeletion(delegate<OnIntegrationDeletionData> const& handler) {
			return onIntegrationDeletionEvent.add(handler);
		}

		void onIntegrationDeletion(event_token const& token) {
			onIntegrationDeletionEvent.remove(token);
		}

		event_token onInteractionCreation(delegate<OnInteractionCreationData> const& handler) {
			return onInteractionCreationEvent.add(handler);
		}

		void onInteractionCreation(event_token const& token) {
			onInteractionCreationEvent.remove(token);
		}

		event_token onInviteCreation(delegate<OnInviteCreationData> const& handler) {
			return onInviteCreationEvent.add(handler);
		}

		void onInviteCreation(event_token const& token) {
			onInviteCreationEvent.remove(token);
		}

		event_token onInviteDeletion(delegate<OnInviteDeletionData> const& handler) {
			return onInviteDeletionEvent.add(handler);
		}

		void onInviteDeletion(event_token const& token) {
			onInviteDeletionEvent.remove(token);
		}

		event_token onMessageCreation(delegate<OnMessageCreationData> const& handler) {
			return onMessageCreationEvent.add(handler);
		}

		void onMessageCreation(event_token const& token) {
			onMessageCreationEvent.remove(token);
		}

		event_token onMessageUpdate(delegate<OnMessageUpdateData> const& handler) {
			return onMessageUpdateEvent.add(handler);
		}

		void onMessageUpdate(event_token const& token) {
			onMessageUpdateEvent.remove(token);
		}

		event_token onMessageDeletion(delegate<OnMessageDeletionData> const& handler) {
			return onMessageDeletionEvent.add(handler);
		}

		void onMessageDeletion(event_token const& token) {
			onMessageDeletionEvent.remove(token);
		}

		event_token onMessageDeleteBulk(delegate<OnMessageDeleteBulkData> const& handler) {
			return onMessageDeleteBulkEvent.add(handler);
		}

		void onMessageDeleteBulk(event_token const& token) {
			onMessageDeleteBulkEvent.remove(token);
		}

		event_token onReactionAdd(delegate<OnReactionAddData> const& handler) {
			return onReactionAddEvent.add(handler);
		}

		void onReactionAdd(event_token const& token) {
			onReactionAddEvent.remove(token);
		}

		event_token onReactionRemove(delegate<OnReactionRemoveData> const& handler) {
			return onReactionRemoveEvent.add(handler);
		}

		void onReactionRemove(event_token const& token) {
			onReactionRemoveEvent.remove(token);
		}

		event_token onReactionRemoveAll(delegate<OnReactionRemoveAllData> const& handler) {
			return onReactionRemoveAllEvent.add(handler);
		}

		void onReactionRemoveAll(event_token const& token) {
			onReactionRemoveAllEvent.remove(token);
		}

		event_token onReactionRemoveEmoji(delegate<OnReactionRemoveEmojiData> const& handler) {
			return onReactionRemoveEmojiEvent.add(handler);
		}

		void onReactionRemoveEmoji(event_token const& token) {
			onReactionRemoveEmojiEvent.remove(token);
		}

		event_token onPresenceUpdate(delegate<OnPresenceUpdateData> const& handler) {
			return onPresenceUpdateEvent.add(handler);
		}

		void onPresenceUpdate(event_token const& token) {
			onPresenceUpdateEvent.remove(token);
		}

		event_token onStageInstanceCreation(delegate<OnStageInstanceCreationData> const& handler) {
			return onStageInstanceCreationEvent.add(handler);
		}

		void onStageInstanceCreation(event_token const& token) {
			onStageInstanceCreationEvent.remove(token);
		}

		event_token onStageInstanceUpdate(delegate<OnStageInstanceUpdateData> const& handler) {
			return onStageInstanceUpdateEvent.add(handler);
		}

		void onStageInstanceUpdate(event_token const& token) {
			onStageInstanceUpdateEvent.remove(token);
		}

		event_token onStageInstanceDeletion(delegate<OnStageInstanceDeletionData> const& handler) {
			return onStageInstanceDeletionEvent.add(handler);
		}

		void onStageInstanceDeletion(event_token const& token) {
			onStageInstanceDeletionEvent.remove(token);
		}

		event_token onTypingStart(delegate<OnTypingStartData> const& handler) {
			return onTypingStartEvent.add(handler);
		}

		void onTypingStar(event_token const& token) {
			onTypingStartEvent.remove(token);
		}

		event_token onUserUpdate(delegate<OnUserUpdateData> const& handler) {
			return onUserUpdateEvent.add(handler);
		}

		void onUserUpdate(event_token const& token) {
			onUserUpdateEvent.remove(token);
		}

		event_token onVoiceStateUpdate(delegate<OnVoiceStateUpdateData> const& handler) {
			return onVoiceStateUpdateEvent.add(handler);
		}

		void onVoiceStateUpdate(event_token const& token) {
			onVoiceStateUpdateEvent.remove(token);
		}

		event_token onVoiceServerUpdate(delegate<OnVoiceServerUpdateData> const& handler) {
			return onVoiceServerUpdateEvent.add(handler);
		}

		void onVoiceServerUpdate(event_token const& token) {
			onVoiceServerUpdateEvent.remove(token);
		}
	
		event_token onWebhookUpdate(delegate<OnWebhookUpdateData> const& handler) {
			return onWebhookUpdateEvent.add(handler);
		}

		void onWebhookUpdate(event_token const& token) {
			onWebhookUpdateEvent.remove(token);
		}

	protected:

		winrt::event<delegate<OnApplicationCommandCreationData>> onApplicationCommandCreationEvent;

		winrt::event<delegate<OnApplicationCommandUpdateData>> onApplicationCommandUpdateEvent;

		winrt::event<delegate<OnApplicationCommandDeletionData>> onApplicationCommandDeletionEvent;

		winrt::event<delegate<OnChannelCreationData>> onChannelCreationEvent;

		winrt::event<delegate<OnChannelUpdateData>> onChannelUpdateEvent;

		winrt::event<delegate<OnChannelDeletionData>> onChannelDeletionEvent;

		winrt::event<delegate<OnChannelPinsUpdateData>> onChannelPinsUpdateEvent;

		winrt::event<delegate<OnThreadCreationData>> onThreadCreationEvent;

		winrt::event<delegate<OnThreadUpdateData>> onThreadUpdateEvent;

		winrt::event<delegate<OnThreadDeletionData>> onThreadDeletionEvent;

		winrt::event<delegate<OnThreadListSyncData>> onThreadListSyncEvent;

		winrt::event<delegate<OnThreadMemberUpdateData>> onThreadMemberUpdateEvent;

		winrt::event<delegate<OnThreadMembersUpdateData>> onThreadMembersUpdateEvent;
		
		winrt::event<delegate<OnGuildCreationData>> onGuildCreationEvent;

		winrt::event<delegate<OnGuildUpdateData>> onGuildUpdateEvent;

		winrt::event<delegate<OnGuildDeletionData>> onGuildDeletionEvent;

		winrt::event<delegate<OnGuildBanAddData>> onGuildBanAddEvent;

		winrt::event<delegate<OnGuildBanRemoveData>> onGuildBanRemoveEvent;

		winrt::event<delegate<OnGuildEmojisUpdateData>> onGuildEmojisUpdateEvent;

		winrt::event<delegate<OnGuildStickersUpdateData>> onGuildStickersUpdateEvent;
		
		winrt::event<delegate<OnGuildIntegrationsUpdateData>> onGuildIntegrationsUpdateEvent;

		winrt::event<delegate<OnGuildMemberAddData>> onGuildMemberAddEvent;

		winrt::event<delegate<OnGuildMemberRemoveData>> onGuildMemberRemoveEvent;

		winrt::event<delegate<OnGuildMemberUpdateData>> onGuildMemberUpdateEvent;

		winrt::event<delegate<OnGuildMembersChunkData>> onGuildMembersChunkEvent;

		winrt::event<delegate<OnRoleCreationData>> onRoleCreationEvent;

		winrt::event<delegate<OnRoleUpdateData>> onRoleUpdateEvent;

		winrt::event<delegate<OnRoleDeletionData>> onRoleDeletionEvent;

		winrt::event<delegate<OnIntegrationCreationData>> onIntegrationCreationEvent;

		winrt::event<delegate<OnIntegrationUpdateData>> onIntegrationUpdateEvent;

		winrt::event<delegate<OnIntegrationDeletionData>> onIntegrationDeletionEvent;

		winrt::event<delegate<OnInteractionCreationData>> onInteractionCreationEvent;

		winrt::event<delegate<OnInviteCreationData>> onInviteCreationEvent;

		winrt::event<delegate<OnInviteDeletionData>> onInviteDeletionEvent;

		winrt::event<delegate<OnMessageCreationData>> onMessageCreationEvent;

		winrt::event<delegate<OnMessageUpdateData>> onMessageUpdateEvent;

		winrt::event<delegate<OnMessageDeletionData>> onMessageDeletionEvent;

		winrt::event<delegate<OnMessageDeleteBulkData>>onMessageDeleteBulkEvent;

		winrt::event<delegate<OnReactionAddData>> onReactionAddEvent;

		winrt::event<delegate<OnReactionRemoveData>> onReactionRemoveEvent;

		winrt::event<delegate<OnReactionRemoveAllData>> onReactionRemoveAllEvent;

		winrt::event<delegate<OnReactionRemoveEmojiData>> onReactionRemoveEmojiEvent;

		winrt::event<delegate<OnPresenceUpdateData>> onPresenceUpdateEvent;

		winrt::event<delegate<OnStageInstanceCreationData>> onStageInstanceCreationEvent;

		winrt::event<delegate<OnStageInstanceUpdateData>> onStageInstanceUpdateEvent;

		winrt::event<delegate<OnStageInstanceDeletionData>> onStageInstanceDeletionEvent;

		winrt::event<delegate<OnTypingStartData>> onTypingStartEvent;

		winrt::event<delegate<OnUserUpdateData>> onUserUpdateEvent;

		winrt::event<delegate<OnVoiceStateUpdateData>> onVoiceStateUpdateEvent;

		winrt::event<delegate<OnVoiceServerUpdateData>> onVoiceServerUpdateEvent;

		winrt::event<delegate<OnWebhookUpdateData>> onWebhookUpdateEvent;

	};
}
#endif
