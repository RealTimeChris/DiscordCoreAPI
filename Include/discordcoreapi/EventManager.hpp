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

#ifndef EVENT_MANAGER
	#define EVENT_MANAGER

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
	/// Data that is received as part of an InputEvent creation event. \brief Data that is received as part of an InputEvent creation event.
	struct DiscordCoreAPI_Dll OnInputEventCreationData {
		InputEventData inputEventData{};///< InputEventData representing the input-event.
	};

	/// Data that is received as part of an ApplicationCommandPermissions update event. \brief Data that is received as part of an ApplicationCommandPermissions update event.
	struct DiscordCoreAPI_Dll OnApplicationCommandPermissionsUpdateData {
		GuildApplicationCommandPermissionsData permissionData{};
	};

	/// Data that is received as part of an AutoModerationRule creation event. \brief Data that is received as part of an AutoModerationRule creation event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleCreationData {
		AutoModerationRule theRule{};
	};

	/// Data that is received as part of an AutoModerationRule update event. \brief Data that is received as part of an AutoModerationRule update event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleUpdateData {
		AutoModerationRule theRule{};
	};

	/// Data that is received as part of an AutoModerationRule delete event. \brief Data that is received as part of an AutoModerationRule delete event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleDeletionData {
		AutoModerationRule theRule{};
	};

	/// Data that is received as part of an AutoModerationAction execution event. \brief Data that is received as part of an AutoModerationAction execution event.
	struct DiscordCoreAPI_Dll OnAutoModerationActionExecutionData {
		AutoModerationActionExecutionEventData theData{};
	};

	/// Data that is received as part of a Channel creation event. \brief Data that is received as part of a Channel creation event.
	struct DiscordCoreAPI_Dll OnChannelCreationData {
		OnChannelCreationData(UniquePtr<ChannelData>);
		OnChannelCreationData& operator=(const OnChannelCreationData&);
		OnChannelCreationData(const OnChannelCreationData&);
		UniquePtr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
	};

	/// Data that is received as part of a Channel update event. \brief Data that is received as part of a Channel update event.
	struct DiscordCoreAPI_Dll OnChannelUpdateData {
		OnChannelUpdateData(UniquePtr<ChannelData>);
		OnChannelUpdateData& operator=(const OnChannelUpdateData&);
		OnChannelUpdateData(const OnChannelUpdateData&);
		UniquePtr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
		~OnChannelUpdateData() noexcept;
	};

	/// Data that is received as part of a Channel deletion event. \brief Data that is received as part of a Channel deletion event.
	struct DiscordCoreAPI_Dll OnChannelDeletionData {
		OnChannelDeletionData(UniquePtr<ChannelData>);
		OnChannelDeletionData& operator=(const OnChannelDeletionData&);
		OnChannelDeletionData(const OnChannelDeletionData&);
		UniquePtr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The deleted Channel.
	};

	/// Data that is received as part of a Channel pins update event. \brief Data that is received as part of a Channel pins update event.
	struct DiscordCoreAPI_Dll OnChannelPinsUpdateData {
		ChannelPinsUpdateEventData dataPackage{};///< The Channel pins update responseData.
	};

	/// Data that is received as part of a Thread creation event. \brief Data that is received as part of a Thread creation event.
	struct DiscordCoreAPI_Dll OnThreadCreationData {
		Thread thread{};///< The new Thread's Channel.
	};

	/// Data that is received as part of a Thread update event. \brief Data that is received as part of a Thread update event.
	struct DiscordCoreAPI_Dll OnThreadUpdateData {
		Thread thread{};///< The new Thread's Channel.
	};

	/// Data that is received as part of a Thread deletion event. \brief Data that is received as part of a Thread deletion event.
	struct DiscordCoreAPI_Dll OnThreadDeletionData {
		Thread thread{};///< The deleted Thread's Channel.
	};

	/// Data that is received as part of a Thread list sync event. \brief Data that is received as part of a Thread list sync event.
	struct DiscordCoreAPI_Dll OnThreadListSyncData {
		ThreadListSyncData threadListSyncData{};///< The Thread list sync responseData.
	};

	/// Data that is received as part of a Thread member update event. \brief Data that is received as part of a Thread member update event.
	struct DiscordCoreAPI_Dll OnThreadMemberUpdateData {
		ThreadMemberData threadMember{};///< Thread member update responseData.
	};

	/// Data that is received as part of a Thread members update event. \brief Data that is received as part of a Thread members update event.
	struct DiscordCoreAPI_Dll OnThreadMembersUpdateData {
		ThreadMembersUpdateData threadMembersUpdateData{};///< Thread member's update responseData.
	};

	/// Data that is received as part of a Guild creation event. \brief Data that is received as part of a Guild creation event.
	struct DiscordCoreAPI_Dll OnGuildCreationData {
		OnGuildCreationData(UniquePtr<GuildData> guild, DiscordCoreClient*);
		OnGuildCreationData& operator=(const OnGuildCreationData&);
		OnGuildCreationData(const OnGuildCreationData&);
		UniquePtr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
	};

	/// Data that is received as part of a Guild update event. \brief Data that is received as part of a Guild update event.
	struct DiscordCoreAPI_Dll OnGuildUpdateData {
		OnGuildUpdateData(UniquePtr<GuildData> guild, DiscordCoreClient*);
		OnGuildUpdateData& operator=(const OnGuildUpdateData&);
		OnGuildUpdateData(const OnGuildUpdateData&);
		UniquePtr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
	};

	/// Data that is received as part of a Guild deletion event. \brief Data that is received as part of a Guild deletion event.
	struct DiscordCoreAPI_Dll OnGuildDeletionData {
		OnGuildDeletionData(UniquePtr<GuildData>);
		OnGuildDeletionData& operator=(const OnGuildDeletionData&);
		OnGuildDeletionData(const OnGuildDeletionData&);
		UniquePtr<GuildData> guild{ std::make_unique<GuildData>() };///< The deleted Guild.
	};

	/// Data that is received as part of a Guild ban add event. \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanAddData {
		Snowflake guildId{};///< The Guild they were banned from.
		User user{};///< The User id of the person who was banned.
	};

	/// Data that is received as part of a Guild ban remove event. \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanRemoveData {
		Snowflake guildId{};///< The Guild they were un-banned from.
		User user{};///< The User id of the person who was un-banned.
	};

	/// Data that is received as part of a Guild emojis update event. \brief Data that is received as part of a Guild emojis update event.
	struct DiscordCoreAPI_Dll OnGuildEmojisUpdateData {
		GuildEmojisUpdateEventData updateData{};///< The Guild emoji's update responseData.
	};

	/// Data that is received as part of a Guild sticker update event. \brief Data that is received as part of a Guild sticker update event.
	struct DiscordCoreAPI_Dll OnGuildStickersUpdateData {
		GuildStickersUpdateEventData updateData{};///< The GuildStickersUpdateEventData.
	};

	/// Data that is received as part of a Guild Integration update event. \brief Data that is received as part of a Guild Integration update event.
	struct DiscordCoreAPI_Dll OnGuildIntegrationsUpdateData {
		Snowflake guildId{};///< The id of the Guild for which the integrations were updated.
	};

	/// Data that is received as part of a GuildMember add event. \brief Data that is received as part of a GuildMember add event.
	struct DiscordCoreAPI_Dll OnGuildMemberAddData {
		OnGuildMemberAddData(UniquePtr<GuildMemberData>, DiscordCoreClient*);
		OnGuildMemberAddData& operator=(const OnGuildMemberAddData&);
		OnGuildMemberAddData(const OnGuildMemberAddData&);
		DiscordCoreClient* discordCoreClient{ nullptr };
		UniquePtr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
	};

	/// Data that is received as part of a GuildMember update event. \brief Data that is received as part of a GuildMember update event.
	struct DiscordCoreAPI_Dll OnGuildMemberUpdateData {
		OnGuildMemberUpdateData(UniquePtr<GuildMemberData>);
		OnGuildMemberUpdateData& operator=(const OnGuildMemberUpdateData&);
		OnGuildMemberUpdateData(const OnGuildMemberUpdateData&);
		UniquePtr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
	};

	/// Data that is received as part of a GuildMember remove event. \brief Data that is received as part of a GuildMember remove event.
	struct DiscordCoreAPI_Dll OnGuildMemberRemoveData {
		OnGuildMemberRemoveData(UniquePtr<UserData>, DiscordCoreClient*, Snowflake);
		OnGuildMemberRemoveData& operator=(const OnGuildMemberRemoveData&);
		OnGuildMemberRemoveData(const OnGuildMemberRemoveData&);
		DiscordCoreClient* discordCoreClient{ nullptr };
		UniquePtr<UserData> user{ std::make_unique<UserData>() };///< The User responseData of the removed GuildMember.
		Snowflake guildId{};///< The id of the Guild from which they were removed.
	};

	/// Data that is received as part of a GuildMembers chunk event. \brief Data that is received as part of a GuildMembers chunk event.
	struct DiscordCoreAPI_Dll OnGuildMembersChunkData {
		GuildMembersChunkEventData chunkEventData{};///< GuildMembersChunkEventData structure.
	};

	/// Data that is received as part of a Role creation event. \brief Data that is received as part of a Role creation event.
	struct DiscordCoreAPI_Dll OnRoleCreationData {
		OnRoleCreationData(UniquePtr<RoleData> theRole, Snowflake guildId);
		OnRoleCreationData& operator=(const OnRoleCreationData&);
		OnRoleCreationData(const OnRoleCreationData&);
		Snowflake guildId{};///< The id of the Guild within which the Role was created.
		UniquePtr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
	};

	/// Data that is received as part of a Role update event. \brief Data that is received as part of a Role update event.
	struct DiscordCoreAPI_Dll OnRoleUpdateData {
		OnRoleUpdateData(UniquePtr<RoleData>, Snowflake);
		OnRoleUpdateData& operator=(const OnRoleUpdateData&);
		OnRoleUpdateData(const OnRoleUpdateData&);
		Snowflake guildId{};///< The id of the Guild within which the Role was updated.
		UniquePtr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
	};

	/// Data that is received as part of a Role deletion event. \brief Data that is received as part of a Role deletion event.
	struct DiscordCoreAPI_Dll OnRoleDeletionData {
		OnRoleDeletionData(UniquePtr<RoleData>, Snowflake);
		OnRoleDeletionData& operator=(const OnRoleDeletionData&);
		OnRoleDeletionData(const OnRoleDeletionData&);
		UniquePtr<RoleData> role{ std::make_unique<RoleData>() };///< The deleted Role.
		Snowflake guildId{};///< The id of the Guild from which the Role was deleted.
	};

	/// Data that is received as part of a GuildScheduledEvent creation event. \brief Data that is received as part of a GuildScheduledEvent creation event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventCreationData {
		GuildScheduledEventData guildScheduledEvent{};
	};

	/// Data that is received as part of a GuildScheduledEvent update event. \brief Data that is received as part of a GuildScheduledEvent update event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUpdateData {
		GuildScheduledEventData guildScheduledEvent{};
	};

	/// Data that is received as part of a GuildScheduledEvent delete event. \brief Data that is received as part of a GuildScheduledEvent delete event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventDeletionData {
		GuildScheduledEventData guildScheduledEvent{};
	};

	/// Data that is received as part of a GuildScheduledEvent User add event. \brief Data that is received as part of a GuildScheduledEvent User add event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserAddData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
	};

	/// Data that is received as part of a GuildScheduledEvent User remove event. \brief Data that is received as part of a GuildScheduledEvent User remove event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserRemoveData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
	};

	/// Data that is received as part of an Integration creation event. \brief Data that is received as part of an Integration creation event.
	struct DiscordCoreAPI_Dll OnIntegrationCreationData {
		IntegrationData integrationData{};///< The new IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which this Integration was created.
	};

	/// Data that is received as part of an Integration update event. \brief Data that is received as part of an Integration update event.
	struct DiscordCoreAPI_Dll OnIntegrationUpdateData {
		IntegrationData integrationData{};///< New IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which the Integration was updated.
	};

	/// Data that is received as part of an Integration deletion event. \brief Data that is received as part of an Integration deletion event.
	struct DiscordCoreAPI_Dll OnIntegrationDeletionData {
		Snowflake applicationId{};///< Application id of the current application.
		Snowflake guildId{};///< The id of the Guild for which the Integration was deleted.
		Snowflake id{};///< The id of the deleted Integration.
	};

	/// Data that is received as part of an Invite creation event. \brief Data that is received as part of an Invite creation event.
	struct DiscordCoreAPI_Dll OnInviteCreationData {
		InviteData invite{};///< Thew new InviteData structure.
	};

	/// Data that is received as part of an Invite deletion event. \brief Data that is received as part of an Invite deletion event.
	struct DiscordCoreAPI_Dll OnInviteDeletionData {
		Snowflake channelId{};///< The id of the Channel for which the Invite existed.
		Snowflake guildId{};///< The id of the Guild for which the Invite existed.
		String code{};///< The code of the Invite.
	};

	/// Data that is received as part of an Interaction creation event. \brief Data that is received as part of an Interaction creation event.
	struct DiscordCoreAPI_Dll OnInteractionCreationData {
		InteractionData interactionData{};///< The InteractionData representing the Interaction.
	};

	/// Data that is received as part of a Message creation event. \brief Data that is received as part of a Message creation event.
	struct DiscordCoreAPI_Dll OnMessageCreationData {
		Message message{};///< The new Message.
	};

	/// Data that is received as part of a Message update event. \brief Data that is received as part of a Message update event.
	struct DiscordCoreAPI_Dll OnMessageUpdateData {
		Message messageNew{};///< The new Message.
	};

	/// Data that is received as part of a Message deletion event. \brief Data that is received as part of a Message deletion event.
	struct DiscordCoreAPI_Dll OnMessageDeletionData {
		Snowflake messageId{};///< The id of the Message which was deleted.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
	};

	/// Data that is received as part of a Message delete bulk event. \brief Data that is received as part of a Message delete bulk event.
	struct DiscordCoreAPI_Dll OnMessageDeleteBulkData {
		Vector<Snowflake> ids{};///< A vector containing the list of deleted Message ids.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
	};

	/// Data that is received as part of a Reaction add event. \brief Data that is received as part of a Reaction add event.
	struct DiscordCoreAPI_Dll OnReactionAddData {
		Reaction reaction{};///< The Reaction that was added.
	};

	/// Data that is received as part of a Reaction remove event. \brief Data that is received as part of a Reaction remove event.
	struct DiscordCoreAPI_Dll OnReactionRemoveData {
		ReactionRemoveData reactionRemoveData{};///< The ReactionRemoveData.
	};

	/// Data that is received as part of a Reaction remove all event. \brief Data that is received as part of a Reaction remove all event.
	struct DiscordCoreAPI_Dll OnReactionRemoveAllData {
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
	};

	/// Data that is received as part of a Reaction remove emoji event. \brief Data that is received as part of a Reaction remove emoji event.
	struct DiscordCoreAPI_Dll OnReactionRemoveEmojiData {
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
		EmojiData emoji{};///< The id of the Emoji which was removed from the Message.
	};

	/// Data that is received as part of a presence update event. \brief Data that is received as part of a presence update event.
	struct DiscordCoreAPI_Dll OnPresenceUpdateData {
		PresenceUpdateData presenceData{};///< PresenceUpdateData..
	};

	/// Data that is received as part of a StageInstance creation event. \brief Data that is received as part of a StageInstance creation event.
	struct DiscordCoreAPI_Dll OnStageInstanceCreationData {
		StageInstanceData stageInstance{};///< The new StageInstanceData.
	};

	/// Data that is received as part of a StageInstance update event. \brief Data that is received as part of a StageInstance update event.
	struct DiscordCoreAPI_Dll OnStageInstanceUpdateData {
		StageInstanceData stageInstance{};///< The new StageInstanceData.
	};

	/// Data that is received as part of a StageInstance deletion event. \brief Data that is received as part of a StageInstance deletion event.
	struct DiscordCoreAPI_Dll OnStageInstanceDeletionData {
		StageInstanceData stageInstance{};///< The deleted StageInstanceData.
	};

	/// Data that is received as part of a typing start event. \brief Data that is received as part of a typing start event.
	struct DiscordCoreAPI_Dll OnTypingStartData {
		TypingStartData typingStartData{};///< TypingStartData of the event.
	};

	/// Data that is received as part of a User update event. \brief Data that is received as part of a User update event.
	struct DiscordCoreAPI_Dll OnUserUpdateData {
		OnUserUpdateData(UniquePtr<UserData>);
		OnUserUpdateData& operator=(const OnUserUpdateData&);
		OnUserUpdateData(const OnUserUpdateData&);
		UniquePtr<UserData> user{ std::make_unique<UserData>() };///< The new User.
	};

	/// Data that is received as part of a voice state update event. \brief Data that is received as part of a voice state update event.
	struct DiscordCoreAPI_Dll OnVoiceStateUpdateData {
		VoiceStateData voiceStateData{};///< VoiceStateData for the new voice state.
	};

	/// Data that is received as part of a voice server update event. \brief Data that is received as part of a voice server update event.
	struct DiscordCoreAPI_Dll OnVoiceServerUpdateData {
		OnVoiceServerUpdateData() noexcept = default;
		OnVoiceServerUpdateData(simdjson::ondemand::value);
		String endpoint{};///< The new endpoint.
		String token{};///< The token of the server update event.
		Snowflake guildId{};///< The id of the Guild for which the server update is occurring.
	};

	/// Data that is received as part of a WebHook update event. \brief Data that is received as part of a WebHook update event.
	struct DiscordCoreAPI_Dll OnWebhookUpdateData {
		Snowflake channelId{};///< Id of the Channel for which the WebHook Update is occurring.
		Snowflake guildId{};///< Id of the Guild for which the WebHook Update is occurring.
	};

	/// Data that is received upon the bot receiving an autocomplete entry. \brief Data that is received upon the bot receiving an autocomplete entry.
	struct DiscordCoreAPI_Dll OnAutoCompleteEntryData {
		InputEventData inputEvent{};///< The input-event representing the autocomplete entry.
	};

	/// Class for handling the assignment of event-handling functions. \brief Class for handling the assignment of event-handling functions.Int32
	class DiscordCoreAPI_Dll EventManager {
	  public:
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandPermissionsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onApplicationCommandsPermissionsUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnApplicationCommandPermissionsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnAutoModerationRuleCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnAutoModerationRuleUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnAutoModerationRuleDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationActionExecutionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationActionExecution(
			const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnAutoModerationActionExecutionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token);

		// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoCompleteEntry(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnAutoCompleteEntryData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInputEventCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnInputEventCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnChannelCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onChannelCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnChannelUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnChannelDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelPinsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelPinsUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnChannelPinsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing afunction from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnThreadCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onThreadCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnThreadUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnThreadDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadListSyncData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadListSync(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnThreadListSyncData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onThreadListSync(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMemberUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnThreadMemberUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMembersUpdateData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMembersUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnThreadMembersUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildDeletionData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildBanAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanRemove(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildBanRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildEmojisUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildEmojisUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildEmojisUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildStickersUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildStickersUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildStickersUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildIntegrationsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildIntegrationsUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildIntegrationsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMembersChunkData structure as an argument.
		/// returns An DiscordCoreInternal::EventDelegateToken, > for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMembersChunk(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildMembersChunkData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildMemberAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberRemove(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildMemberRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildMemberUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnRoleCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onRoleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleUpdateData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnRoleUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnRoleDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildScheduledEventCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildScheduledEventCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildScheduledEventUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildScheduledEventUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildScheduledEventDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildScheduledEventDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventUserAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUserAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildScheduledEventUserAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildScheduledEventUserAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildScheduledEventUserRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUserRemove(
			const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnGuildScheduledEventUserRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onGuildScheduledEventUserRemove(DiscordCoreInternal::EventDelegateToken& token);


		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnIntegrationCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnIntegrationUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnIntegrationDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInteractionCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInteractionCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnInteractionCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnInviteCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onInviteCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnInviteDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnMessageCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onMessageCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnMessageUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnMessageDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeleteBulkData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeleteBulk(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnMessageDeleteBulkData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnReactionAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onReactionAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemove(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnReactionRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken,from the original event registration.
		Void onReactionRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveAllData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveAll(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnReactionRemoveAllData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveEmojiData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveEmoji(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnReactionRemoveEmojiData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnPresenceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onPresenceUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnPresenceUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnStageInstanceCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnStageInstanceUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnStageInstanceDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnTypingStartData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onTypingStart(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnTypingStartData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onTypingStart(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnUserUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onUserUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnUserUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onUserUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceStateUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceStateUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnVoiceStateUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceServerUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceServerUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnVoiceServerUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnWebhookUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onWebhookUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<Void>, OnWebhookUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		Void onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token);

		DiscordCoreInternal::Event<CoRoutine<Void>, OnApplicationCommandPermissionsUpdateData> onApplicationCommandPermissionsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnAutoModerationRuleCreationData> onAutoModerationRuleCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnAutoModerationRuleUpdateData> onAutoModerationRuleUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnAutoModerationRuleDeletionData> onAutoModerationRuleDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnAutoModerationActionExecutionData> onAutoModerationActionExecutionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnAutoCompleteEntryData> onAutoCompleteEntryEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnInputEventCreationData> onInputEventCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnChannelCreationData> onChannelCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnChannelUpdateData> onChannelUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnChannelDeletionData> onChannelDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnChannelPinsUpdateData> onChannelPinsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnThreadCreationData> onThreadCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnThreadUpdateData> onThreadUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnThreadDeletionData> onThreadDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnThreadListSyncData> onThreadListSyncEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnThreadMemberUpdateData> onThreadMemberUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnThreadMembersUpdateData> onThreadMembersUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildCreationData> onGuildCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildUpdateData> onGuildUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildDeletionData> onGuildDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildBanAddData> onGuildBanAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildBanRemoveData> onGuildBanRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildEmojisUpdateData> onGuildEmojisUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildStickersUpdateData> onGuildStickersUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildIntegrationsUpdateData> onGuildIntegrationsUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildMemberAddData> onGuildMemberAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildMemberRemoveData> onGuildMemberRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildMemberUpdateData> onGuildMemberUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildMembersChunkData> onGuildMembersChunkEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnRoleCreationData> onRoleCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnRoleUpdateData> onRoleUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnRoleDeletionData> onRoleDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildScheduledEventCreationData> onGuildScheduledEventCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildScheduledEventUpdateData> onGuildScheduledEventUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildScheduledEventDeletionData> onGuildScheduledEventDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildScheduledEventUserAddData> onGuildScheduledEventUserAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnGuildScheduledEventUserRemoveData> onGuildScheduledEventUserRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnIntegrationCreationData> onIntegrationCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnIntegrationUpdateData> onIntegrationUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnIntegrationDeletionData> onIntegrationDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnInteractionCreationData> onInteractionCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnInviteCreationData> onInviteCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnInviteDeletionData> onInviteDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnMessageCreationData> onMessageCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnMessageUpdateData> onMessageUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnMessageDeletionData> onMessageDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnMessageDeleteBulkData> onMessageDeleteBulkEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnReactionAddData> onReactionAddEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnReactionRemoveData> onReactionRemoveEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnReactionRemoveAllData> onReactionRemoveAllEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnReactionRemoveEmojiData> onReactionRemoveEmojiEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnPresenceUpdateData> onPresenceUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnStageInstanceCreationData> onStageInstanceCreationEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnStageInstanceUpdateData> onStageInstanceUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnStageInstanceDeletionData> onStageInstanceDeletionEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnTypingStartData> onTypingStartEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnUserUpdateData> onUserUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnVoiceStateUpdateData> onVoiceStateUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnVoiceServerUpdateData> onVoiceServerUpdateEvent{};

		DiscordCoreInternal::Event<CoRoutine<Void>, OnWebhookUpdateData> onWebhookUpdateEvent{};
	};
	/**@}*/

}// namespace DiscordCoreAPI
#endif