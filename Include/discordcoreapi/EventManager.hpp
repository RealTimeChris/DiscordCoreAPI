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
/// Apr 26, 2021
/// https://discordcoreapi.com
/// \file EventManager.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup discord_events
	 * @{
	 */
	/// \brief Data that is received as part of a Gateway ping event.
	struct DiscordCoreAPI_Dll OnGatewayPingData {
		int32_t timeUntilNextPing{};///< The number of milliseconds until the next ping.
	};

	/// \brief Data that is received as part of an InputEvent creation event.
	struct DiscordCoreAPI_Dll OnInputEventCreationData {
		InputEventData inputEventData{};///< InputEventData representing the input-event.
		OnInputEventCreationData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data);
	};

	/// \brief Data that is received as part of an ApplicationCommandPermissions update event.
	struct DiscordCoreAPI_Dll OnApplicationCommandPermissionsUpdateData {
		GuildApplicationCommandPermissionsData permissionData{};
		OnApplicationCommandPermissionsUpdateData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data);
	};

	/// \brief Data that is received as part of an AutoModerationRule creation event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleCreationData {
		AutoModerationRule theRule{};
		OnAutoModerationRuleCreationData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data);
	};

	/// \brief Data that is received as part of an AutoModerationRule update event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleUpdateData {
		AutoModerationRule theRule{};
		OnAutoModerationRuleUpdateData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data);
	};

	/// \brief Data that is received as part of an AutoModerationRule delete event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleDeletionData {
		AutoModerationRule theRule{};
		OnAutoModerationRuleDeletionData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data);
	};

	/// \brief Data that is received as part of an AutoModerationAction execution event.
	struct DiscordCoreAPI_Dll OnAutoModerationActionExecutionData {
		AutoModerationActionExecutionEventData data{};
		OnAutoModerationActionExecutionData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data);
	};

	/// \brief Data that is received as part of a Channel creation event.
	struct DiscordCoreAPI_Dll OnChannelCreationData {
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
		OnChannelCreationData(DiscordCoreInternal::WebSocketMessage& message, simdjson::ondemand::value data);
		OnChannelCreationData& operator=(const OnChannelCreationData& other);
		OnChannelCreationData(const OnChannelCreationData& other);
	};

	/// \brief Data that is received as part of a Channel update event.
	struct DiscordCoreAPI_Dll OnChannelUpdateData {
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
		OnChannelUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
		OnChannelUpdateData& operator=(const OnChannelUpdateData& other);
		OnChannelUpdateData(const OnChannelUpdateData& other);
	};

	/// \brief Data that is received as part of a Channel deletion event.
	struct DiscordCoreAPI_Dll OnChannelDeletionData {
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The deleted Channel.
		OnChannelDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
		OnChannelDeletionData& operator=(const OnChannelDeletionData&);
		OnChannelDeletionData(const OnChannelDeletionData&);
	};

	/// \brief Data that is received as part of a Channel pins update event.
	struct DiscordCoreAPI_Dll OnChannelPinsUpdateData {
		ChannelPinsUpdateEventData dataPackage{};///< The Channel pins update responseData.
		OnChannelPinsUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Thread creation event.
	struct DiscordCoreAPI_Dll OnThreadCreationData {
		Thread thread{};///< The new Thread's Channel.
		OnThreadCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Thread update event.
	struct DiscordCoreAPI_Dll OnThreadUpdateData {
		Thread thread{};///< The new Thread's Channel.
		OnThreadUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Thread deletion event.
	struct DiscordCoreAPI_Dll OnThreadDeletionData {
		Thread thread{};///< The deleted Thread's Channel.
		OnThreadDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Thread list sync event.
	struct DiscordCoreAPI_Dll OnThreadListSyncData {
		ThreadListSyncData threadListSyncData{};///< The Thread list sync responseData.
		OnThreadListSyncData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Thread member update event.
	struct DiscordCoreAPI_Dll OnThreadMemberUpdateData {
		ThreadMemberData threadMember{};///< Thread member update responseData.
		OnThreadMemberUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Thread members update event.
	struct DiscordCoreAPI_Dll OnThreadMembersUpdateData {
		ThreadMembersUpdateData threadMembersUpdateData{};///< Thread member's update responseData.
		OnThreadMembersUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Guild creation event.
	struct DiscordCoreAPI_Dll OnGuildCreationData {
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
		OnGuildCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal, DiscordCoreClient*);
		OnGuildCreationData& operator=(const OnGuildCreationData&);
		OnGuildCreationData(const OnGuildCreationData&);
	};

	/// \brief Data that is received as part of a Guild update event.
	struct DiscordCoreAPI_Dll OnGuildUpdateData {
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
		OnGuildUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal, DiscordCoreClient*);
		OnGuildUpdateData& operator=(const OnGuildUpdateData&);
		OnGuildUpdateData(const OnGuildUpdateData&);
	};

	/// \brief Data that is received as part of a Guild deletion event.
	struct DiscordCoreAPI_Dll OnGuildDeletionData {
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The deleted Guild.
		OnGuildDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal, DiscordCoreClient*);
		OnGuildDeletionData& operator=(const OnGuildDeletionData&);
		OnGuildDeletionData(const OnGuildDeletionData&);
	};

	/// \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanAddData {
		Snowflake guildId{};///< The Guild they were banned from.
		User user{};///< The User id of the person who was banned.
		OnGuildBanAddData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanRemoveData {
		Snowflake guildId{};///< The Guild they were un-banned from.
		User user{};///< The User id of the person who was un-banned.
		OnGuildBanRemoveData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Guild emojis update event.
	struct DiscordCoreAPI_Dll OnGuildEmojisUpdateData {
		GuildEmojisUpdateEventData updateData{};///< The Guild emoji's update responseData.
		OnGuildEmojisUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Guild sticker update event.
	struct DiscordCoreAPI_Dll OnGuildStickersUpdateData {
		GuildStickersUpdateEventData updateData{};///< The GuildStickersUpdateEventData.
		OnGuildStickersUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Guild Integration update event.
	struct DiscordCoreAPI_Dll OnGuildIntegrationsUpdateData {
		Snowflake guildId{};///< The id of the Guild for which the integrations were updated.
		OnGuildIntegrationsUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a GuildMember add event.
	struct DiscordCoreAPI_Dll OnGuildMemberAddData {
		std::unique_ptr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
		DiscordCoreClient* discordCoreClient{ nullptr };
		OnGuildMemberAddData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal, DiscordCoreClient*);
		OnGuildMemberAddData& operator=(const OnGuildMemberAddData&);
		OnGuildMemberAddData(const OnGuildMemberAddData&);
	};

	/// \brief Data that is received as part of a GuildMember update event.
	struct DiscordCoreAPI_Dll OnGuildMemberUpdateData {
		std::unique_ptr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
		OnGuildMemberUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal, DiscordCoreClient*);
		OnGuildMemberUpdateData& operator=(const OnGuildMemberUpdateData&);
		OnGuildMemberUpdateData(const OnGuildMemberUpdateData&);
	};

	/// \brief Data that is received as part of a GuildMember remove event.
	struct DiscordCoreAPI_Dll OnGuildMemberRemoveData {
		std::unique_ptr<UserData> user{ std::make_unique<UserData>() };///< The User responseData of the removed GuildMember.
		DiscordCoreClient* discordCoreClient{ nullptr };
		Snowflake guildId{};///< The id of the Guild from which they were removed.
		OnGuildMemberRemoveData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal, DiscordCoreClient*);
		OnGuildMemberRemoveData& operator=(const OnGuildMemberRemoveData&);
		OnGuildMemberRemoveData(const OnGuildMemberRemoveData&);
	};

	/// \brief Data that is received as part of a GuildMembers chunk event.
	struct DiscordCoreAPI_Dll OnGuildMembersChunkData {
		GuildMembersChunkEventData chunkEventData{};///< GuildMembersChunkEventData structure.
		OnGuildMembersChunkData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Role creation event.
	struct DiscordCoreAPI_Dll OnRoleCreationData {
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
		Snowflake guildId{};///< The id of the Guild within which the Role was created.
		OnRoleCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
		OnRoleCreationData& operator=(const OnRoleCreationData&);
		OnRoleCreationData(const OnRoleCreationData&);
	};

	/// \brief Data that is received as part of a Role update event.
	struct DiscordCoreAPI_Dll OnRoleUpdateData {
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
		Snowflake guildId{};///< The id of the Guild within which the Role was updated.
		OnRoleUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
		OnRoleUpdateData& operator=(const OnRoleUpdateData&);
		OnRoleUpdateData(const OnRoleUpdateData&);
	};

	/// \brief Data that is received as part of a Role deletion event.
	struct DiscordCoreAPI_Dll OnRoleDeletionData {
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The deleted Role.
		Snowflake guildId{};///< The id of the Guild from which the Role was deleted.
		OnRoleDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
		OnRoleDeletionData& operator=(const OnRoleDeletionData&);
		OnRoleDeletionData(const OnRoleDeletionData&);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent creation event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventCreationData {
		GuildScheduledEventData guildScheduledEvent{};
		OnGuildScheduledEventCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent update event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUpdateData {
		GuildScheduledEventData guildScheduledEvent{};
		OnGuildScheduledEventUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent delete event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventDeletionData {
		GuildScheduledEventData guildScheduledEvent{};
		OnGuildScheduledEventDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent User add event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserAddData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
		OnGuildScheduledEventUserAddData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent User remove event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserRemoveData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
		OnGuildScheduledEventUserRemoveData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of an Integration creation event.
	struct DiscordCoreAPI_Dll OnIntegrationCreationData {
		IntegrationData integrationData{};///< The new IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which this Integration was created.
		OnIntegrationCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of an Integration update event.
	struct DiscordCoreAPI_Dll OnIntegrationUpdateData {
		IntegrationData integrationData{};///< New IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which the Integration was updated.
		OnIntegrationUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of an Integration deletion event.
	struct DiscordCoreAPI_Dll OnIntegrationDeletionData {
		Snowflake applicationId{};///< Application id of the current application.
		Snowflake guildId{};///< The id of the Guild for which the Integration was deleted.
		Snowflake id{};///< The id of the deleted Integration.
		OnIntegrationDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of an Invite creation event.
	struct DiscordCoreAPI_Dll OnInviteCreationData {
		InviteData invite{};///< Thew new InviteData structure.
		OnInviteCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of an Invite deletion event.
	struct DiscordCoreAPI_Dll OnInviteDeletionData {
		Snowflake channelId{};///< The id of the Channel for which the Invite existed.
		Snowflake guildId{};///< The id of the Guild for which the Invite existed.
		std::string code{};///< The code of the Invite.
		OnInviteDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of an Interaction creation event.
	struct DiscordCoreAPI_Dll OnInteractionCreationData {
		InteractionData interactionData{};///< The InteractionData representing the Interaction.
		OnInteractionCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal, DiscordCoreClient* clientPtr);
	};

	/// \brief Data that is received as part of a Message creation event.
	struct DiscordCoreAPI_Dll OnMessageCreationData {
		Message message{};///< The new Message.
		OnMessageCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Message update event.
	struct DiscordCoreAPI_Dll OnMessageUpdateData {
		Message messageNew{};///< The new Message.
		OnMessageUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Message deletion event.
	struct DiscordCoreAPI_Dll OnMessageDeletionData {
		Snowflake messageId{};///< The id of the Message which was deleted.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
		OnMessageDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Message delete bulk event.
	struct DiscordCoreAPI_Dll OnMessageDeleteBulkData {
		std::vector<Snowflake> ids{};///< A vector containing the list of deleted Message ids.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
		OnMessageDeleteBulkData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Reaction add event.
	struct DiscordCoreAPI_Dll OnReactionAddData {
		Reaction reaction{};///< The Reaction that was added.
		OnReactionAddData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Reaction remove event.
	struct DiscordCoreAPI_Dll OnReactionRemoveData {
		ReactionRemoveData reactionRemoveData{};///< The ReactionRemoveData.
		OnReactionRemoveData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Reaction remove all event.
	struct DiscordCoreAPI_Dll OnReactionRemoveAllData {
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
		OnReactionRemoveAllData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a Reaction remove emoji event.
	struct DiscordCoreAPI_Dll OnReactionRemoveEmojiData {
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
		EmojiData emoji{};///< The id of the Emoji which was removed from the Message.
		OnReactionRemoveEmojiData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a presence update event.
	struct DiscordCoreAPI_Dll OnPresenceUpdateData {
		PresenceUpdateData presenceData{};///< PresenceUpdateData..
		OnPresenceUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a StageInstance creation event.
	struct DiscordCoreAPI_Dll OnStageInstanceCreationData {
		StageInstanceData stageInstance{};///< The new StageInstanceData.
		OnStageInstanceCreationData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a StageInstance update event.
	struct DiscordCoreAPI_Dll OnStageInstanceUpdateData {
		StageInstanceData stageInstance{};///< The new StageInstanceData.
		OnStageInstanceUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a StageInstance deletion event.
	struct DiscordCoreAPI_Dll OnStageInstanceDeletionData {
		StageInstanceData stageInstance{};///< The deleted StageInstanceData.
		OnStageInstanceDeletionData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a typing start event.
	struct DiscordCoreAPI_Dll OnTypingStartData {
		TypingStartData typingStartData{};///< TypingStartData of the event.
		OnTypingStartData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received as part of a User update event.
	struct DiscordCoreAPI_Dll OnUserUpdateData {
		std::unique_ptr<UserData> user{ std::make_unique<UserData>() };///< The new User.
		OnUserUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
		OnUserUpdateData& operator=(const OnUserUpdateData&);
		OnUserUpdateData(const OnUserUpdateData&);
	};

	/// \brief Data that is received as part of a voice state update event.
	struct DiscordCoreAPI_Dll OnVoiceStateUpdateData {
		VoiceStateData voiceStateData{};///< VoiceStateData for the new voice state.
		OnVoiceStateUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
			DiscordCoreInternal::WebSocketSSLShard* sslShard);
	};

	/// \brief Data that is received as part of a voice server update event.
	struct DiscordCoreAPI_Dll OnVoiceServerUpdateData {
		Snowflake guildId{};///< The id of the Guild for which the server update is occurring.
		std::string endpoint{};///< The new endpoint.
		std::string token{};///< The token of the server update event.
		OnVoiceServerUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal,
			DiscordCoreInternal::WebSocketSSLShard* sslShard);
	};

	/// \brief Data that is received as part of a WebHook update event.
	struct DiscordCoreAPI_Dll OnWebhookUpdateData {
		Snowflake channelId{};///< Id of the Channel for which the WebHook Update is occurring.
		Snowflake guildId{};///< Id of the Guild for which the WebHook Update is occurring.
		OnWebhookUpdateData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Data that is received upon the bot receiving an autocomplete entry.
	struct DiscordCoreAPI_Dll OnAutoCompleteEntryData {
		InputEventData inputEvent{};///< The input-event representing the autocomplete entry.
		OnAutoCompleteEntryData(DiscordCoreInternal::WebSocketMessage& data, simdjson::ondemand::value dataReal);
	};

	/// \brief Class for handling the assignment of event-handling functions.int32_t
	class DiscordCoreAPI_Dll EventManager {
	  public:
		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGatewayPingData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGatewayPing(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGatewayPingData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGatewayPing(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandPermissionsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onApplicationCommandsPermissionsUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationActionExecutionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationActionExecution(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationActionExecutionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token);

		 \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoCompleteEntry(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoCompleteEntryData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInputEventCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInputEventCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelPinsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelPinsUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelPinsUpdateData> handler);
		/// \brief For removing afunction from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadListSyncData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadListSync(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadListSyncData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadListSync(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMemberUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMemberUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMembersUpdateData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMembersUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMembersUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildDeletionData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanAddData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanRemove(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanRemoveData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildEmojisUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildEmojisUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildEmojisUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildStickersUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildStickersUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildStickersUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildIntegrationsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildIntegrationsUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildIntegrationsUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMembersChunkData structure as an argument.
		/// returns An DiscordCoreInternal::EventDelegateToken, > for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMembersChunk(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMembersChunkData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberAdd(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberAddData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberRemove(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberRemoveData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleUpdateData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventUserAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUserAdd(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserAddData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventUserAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventUserRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUserRemove(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventUserRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInteractionCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInteractionCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInteractionCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInviteCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeleteBulkData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeleteBulk(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeleteBulkData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionAddData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemove(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken,from the original event registration.
		void onReactionRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveAllData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveAll(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveAllData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveEmojiData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveEmoji(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveEmojiData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnPresenceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onPresenceUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnPresenceUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceCreation(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceCreationData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceDeletion(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceDeletionData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnTypingStartData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onTypingStart(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnTypingStartData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onTypingStart(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnUserUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onUserUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnUserUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onUserUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceStateUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceStateUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceStateUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceServerUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceServerUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceServerUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnWebhookUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onWebhookUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnWebhookUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token);

		DiscordCoreInternal::Event<CoRoutine<void>, OnGatewayPingData> onGatewayPingEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> onApplicationCommandPermissionsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleCreationData> onAutoModerationRuleCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleUpdateData> onAutoModerationRuleUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleDeletionData> onAutoModerationRuleDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationActionExecutionData> onAutoModerationActionExecutionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoCompleteEntryData> onAutoCompleteEntryEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInputEventCreationData> onInputEventCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelCreationData> onChannelCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelUpdateData> onChannelUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelDeletionData> onChannelDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelPinsUpdateData> onChannelPinsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadCreationData> onThreadCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadUpdateData> onThreadUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadDeletionData> onThreadDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadListSyncData> onThreadListSyncEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadMemberUpdateData> onThreadMemberUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadMembersUpdateData> onThreadMembersUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildCreationData> onGuildCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildUpdateData> onGuildUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildDeletionData> onGuildDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildBanAddData> onGuildBanAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildBanRemoveData> onGuildBanRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildEmojisUpdateData> onGuildEmojisUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildStickersUpdateData> onGuildStickersUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildIntegrationsUpdateData> onGuildIntegrationsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberAddData> onGuildMemberAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberRemoveData> onGuildMemberRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberUpdateData> onGuildMemberUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMembersChunkData> onGuildMembersChunkEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnRoleCreationData> onRoleCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnRoleUpdateData> onRoleUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnRoleDeletionData> onRoleDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventCreationData> onGuildScheduledEventCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUpdateData> onGuildScheduledEventUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventDeletionData> onGuildScheduledEventDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUserAddData> onGuildScheduledEventUserAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> onGuildScheduledEventUserRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationCreationData> onIntegrationCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationUpdateData> onIntegrationUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationDeletionData> onIntegrationDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInteractionCreationData> onInteractionCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInviteCreationData> onInviteCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInviteDeletionData> onInviteDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageCreationData> onMessageCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageUpdateData> onMessageUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageDeletionData> onMessageDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageDeleteBulkData> onMessageDeleteBulkEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionAddData> onReactionAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveData> onReactionRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveAllData> onReactionRemoveAllEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveEmojiData> onReactionRemoveEmojiEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnPresenceUpdateData> onPresenceUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceCreationData> onStageInstanceCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceUpdateData> onStageInstanceUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceDeletionData> onStageInstanceDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnTypingStartData> onTypingStartEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnUserUpdateData> onUserUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnVoiceStateUpdateData> onVoiceStateUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnVoiceServerUpdateData> onVoiceServerUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnWebhookUpdateData> onWebhookUpdateEvent{};
	};
	/**@}*/

}// namespace DiscordCoreAPI
 adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceServerUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceServerUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceServerUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnWebhookUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onWebhookUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnWebhookUpdateData> handler);
		/// \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token);

		DiscordCoreInternal::Event<CoRoutine<void>, OnGatewayPingData> onGatewayPingEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> onApplicationCommandPermissionsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleCreationData> onAutoModerationRuleCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleUpdateData> onAutoModerationRuleUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleDeletionData> onAutoModerationRuleDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationActionExecutionData> onAutoModerationActionExecutionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnAutoCompleteEntryData> onAutoCompleteEntryEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInputEventCreationData> onInputEventCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelCreationData> onChannelCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelUpdateData> onChannelUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelDeletionData> onChannelDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnChannelPinsUpdateData> onChannelPinsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadCreationData> onThreadCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadUpdateData> onThreadUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadDeletionData> onThreadDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadListSyncData> onThreadListSyncEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadMemberUpdateData> onThreadMemberUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnThreadMembersUpdateData> onThreadMembersUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildCreationData> onGuildCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildUpdateData> onGuildUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildDeletionData> onGuildDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildBanAddData> onGuildBanAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildBanRemoveData> onGuildBanRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildEmojisUpdateData> onGuildEmojisUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildStickersUpdateData> onGuildStickersUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildIntegrationsUpdateData> onGuildIntegrationsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberAddData> onGuildMemberAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberRemoveData> onGuildMemberRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberUpdateData> onGuildMemberUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMembersChunkData> onGuildMembersChunkEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnRoleCreationData> onRoleCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnRoleUpdateData> onRoleUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnRoleDeletionData> onRoleDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventCreationData> onGuildScheduledEventCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUpdateData> onGuildScheduledEventUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventDeletionData> onGuildScheduledEventDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUserAddData> onGuildScheduledEventUserAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> onGuildScheduledEventUserRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationCreationData> onIntegrationCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationUpdateData> onIntegrationUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationDeletionData> onIntegrationDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInteractionCreationData> onInteractionCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInviteCreationData> onInviteCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnInviteDeletionData> onInviteDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageCreationData> onMessageCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageUpdateData> onMessageUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageDeletionData> onMessageDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnMessageDeleteBulkData> onMessageDeleteBulkEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionAddData> onReactionAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveData> onReactionRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveAllData> onReactionRemoveAllEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveEmojiData> onReactionRemoveEmojiEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnPresenceUpdateData> onPresenceUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceCreationData> onStageInstanceCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceUpdateData> onStageInstanceUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceDeletionData> onStageInstanceDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnTypingStartData> onTypingStartEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnUserUpdateData> onUserUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnVoiceStateUpdateData> onVoiceStateUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnVoiceServerUpdateData> onVoiceServerUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<void>, OnWebhookUpdateData> onWebhookUpdateEvent{};
	};
	/**@}*/

}// namespace DiscordCoreAPI
;

		DiscordCoreInternal::Event<CoRoutine<void>, OnWebhookUpdateData> onWebhookUpdateEvent{};
	};
	/**@}*/

}// namespace DiscordCoreAPI
