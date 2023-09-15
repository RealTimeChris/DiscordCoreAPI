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
/// EventManager.hpp - Header for the event manager class.
/// Apr 26, 2021
/// https://discordcoreapi.com
/// \file EventManager.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
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

	template<typename ValueType> struct EventData {
	  public:
		ValueType value{};
	};

	template<typename ValueType, typename OldValueType> struct UpdatedEventData {
	  public:
		OldValueType oldValue{};
		ValueType value{};
	};

	struct GuildMemberRemoveData {
		Snowflake guildId{};
		UserData user{};
	};

	struct GuildBanAddData {
		Snowflake guildId{};
		UserData user{};
	};

	struct GuildBanRemoveData {
		Snowflake guildId{};
		UserData user{};
	};

	struct RoleCreationData {
		Snowflake guildId{};
		RoleData role{};
	};

	struct RoleUpdateData {
		Snowflake guildId{};
		RoleData role{};
	};

	struct RoleDeletionData {
		Snowflake guildId{};
		RoleData role{};
	};

	struct GuildScheduledEventUserAddData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
	};

	struct GuildScheduledEventUserRemoveData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
	};

	struct IntegrationCreationData {
		IntegrationData integrationData{};///< The new IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which this Integration was created.
	};

	struct IntegrationUpdateData {
		IntegrationData integrationData{};///< New IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which the Integration was updated.
	};

	struct IntegrationDeletionData {
		Snowflake applicationId{};///< Application id of the current application.
		Snowflake guildId{};///< The id of the Guild for which the Integration was deleted.
		Snowflake id{};///< The id of the deleted Integration.
	};

	struct InviteDeletionData {
		Snowflake channelId{};///< The id of the Channel for which the Invite existed.
		Snowflake guildId{};///< The id of the Guild for which the Invite existed.
		jsonifier::string code{};///< The code of the Invite.
	};

	struct MessageDeletionData {
		Snowflake messageId{};///< The id of the Message which was deleted.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
	};

	struct MessageDeletionBulkData {
		jsonifier::vector<Snowflake> ids{};///< A vector containing the list of deleted Message ids.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
	};

	struct ReactionRemoveAllData {
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
	};

	struct ReactionRemoveEmojiData {
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
		EmojiData emoji{};///< The id of the Emoji which was removed from the MessageData.
	};

	struct WebHookUpdateData {
		Snowflake channelId{};///< Id of the Channel for which the WebHook Update is occurring.
		Snowflake guildId{};///< Id of the Guild for which the WebHook Update is occurring.
	};

	/// @brief Data that is received as part of a Gateway ping event.
	struct OnGatewayPingData : public EventData<int32_t> {
		int32_t timeUntilNextPing{};///< The number of milliseconds until the next ping.
	};

	/// @brief Data that is received as part of an InputEvent creation event.
	struct DiscordCoreAPI_Dll OnInputEventCreationData : public EventData<InputEventData> {
		OnInputEventCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an ApplicationCommandPermissions update event.
	struct DiscordCoreAPI_Dll OnApplicationCommandPermissionsUpdateData : public EventData<GuildApplicationCommandPermissionsData> {
		OnApplicationCommandPermissionsUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an AutoModerationRuleData creation event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleCreationData : public EventData<AutoModerationRuleData> {
		OnAutoModerationRuleCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an AutoModerationRuleData update event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleUpdateData : public EventData<AutoModerationRuleData> {
		OnAutoModerationRuleUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an AutoModerationRuleData delete event.
	struct DiscordCoreAPI_Dll OnAutoModerationRuleDeletionData : public EventData<AutoModerationRuleData> {
		OnAutoModerationRuleDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an AutoModerationAction execution event.
	struct DiscordCoreAPI_Dll OnAutoModerationActionExecutionData : public EventData<AutoModerationActionExecutionEventData> {
		OnAutoModerationActionExecutionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Channel creation event.
	struct DiscordCoreAPI_Dll OnChannelCreationData : public EventData<ChannelData> {
		OnChannelCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Channel update event.
	struct DiscordCoreAPI_Dll OnChannelUpdateData : public UpdatedEventData<ChannelData, ChannelData> {
		OnChannelUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Channel deletion event.
	struct DiscordCoreAPI_Dll OnChannelDeletionData : public EventData<ChannelData> {
		OnChannelDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Channel pins update event.
	struct DiscordCoreAPI_Dll OnChannelPinsUpdateData : public EventData<ChannelPinsUpdateEventData> {
		OnChannelPinsUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a ThreadData creation event.
	struct DiscordCoreAPI_Dll OnThreadCreationData : public EventData<ThreadData> {
		OnThreadCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a ThreadData update event.
	struct DiscordCoreAPI_Dll OnThreadUpdateData : public EventData<ThreadData> {
		OnThreadUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a ThreadData deletion event.
	struct DiscordCoreAPI_Dll OnThreadDeletionData : public EventData<ThreadData> {
		OnThreadDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a ThreadData list sync event.
	struct DiscordCoreAPI_Dll OnThreadListSyncData : public EventData<ThreadListSyncData> {
		OnThreadListSyncData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a ThreadData member update event.
	struct DiscordCoreAPI_Dll OnThreadMemberUpdateData : public EventData<ThreadMemberData> {
		OnThreadMemberUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a ThreadData members update event.
	struct DiscordCoreAPI_Dll OnThreadMembersUpdateData : public EventData<ThreadMembersUpdateData> {
		OnThreadMembersUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild creation event.
	struct DiscordCoreAPI_Dll OnGuildCreationData : public EventData<GuildData> {
		OnGuildCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild update event.
	struct DiscordCoreAPI_Dll OnGuildUpdateData : public UpdatedEventData<GuildData, GuildData> {
		OnGuildUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild deletion event.
	struct DiscordCoreAPI_Dll OnGuildDeletionData : public EventData<GuildData> {
		OnGuildDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanAddData : public EventData<GuildBanAddData> {
		OnGuildBanAddData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanRemoveData : public EventData<GuildBanRemoveData> {
		OnGuildBanRemoveData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild emojis update event.
	struct DiscordCoreAPI_Dll OnGuildEmojisUpdateData : public EventData<GuildEmojisUpdateEventData> {
		OnGuildEmojisUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild sticker update event.
	struct DiscordCoreAPI_Dll OnGuildStickersUpdateData : public EventData<GuildStickersUpdateEventData> {
		OnGuildStickersUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Guild Integration update event.
	struct DiscordCoreAPI_Dll OnGuildIntegrationsUpdateData : public EventData<Snowflake> {
		OnGuildIntegrationsUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildMember add event.
	struct DiscordCoreAPI_Dll OnGuildMemberAddData : public EventData<GuildMemberData> {
		OnGuildMemberAddData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildMember update event.
	struct DiscordCoreAPI_Dll OnGuildMemberUpdateData : public UpdatedEventData<GuildMemberData, GuildMemberData> {
		OnGuildMemberUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildMember remove event.
	struct DiscordCoreAPI_Dll OnGuildMemberRemoveData : public EventData<GuildMemberRemoveData> {
		OnGuildMemberRemoveData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildMembers chunk event.
	struct DiscordCoreAPI_Dll OnGuildMembersChunkData : public EventData<GuildMembersChunkEventData> {
		OnGuildMembersChunkData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a RoleData creation event.
	struct DiscordCoreAPI_Dll OnRoleCreationData : public EventData<RoleCreationData> {
		OnRoleCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a RoleData update event.
	struct DiscordCoreAPI_Dll OnRoleUpdateData : public UpdatedEventData<RoleUpdateData, RoleData> {
		OnRoleUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a RoleData deletion event.
	struct DiscordCoreAPI_Dll OnRoleDeletionData : public EventData<RoleDeletionData> {
		OnRoleDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildScheduledEventData creation event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventCreationData : public EventData<GuildScheduledEventData> {
		OnGuildScheduledEventCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildScheduledEventData update event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUpdateData : public EventData<GuildScheduledEventData> {
		OnGuildScheduledEventUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildScheduledEventData delete event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventDeletionData : public EventData<GuildScheduledEventData> {
		OnGuildScheduledEventDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildScheduledEventData User add event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserAddData : public EventData<GuildScheduledEventUserAddData> {
		OnGuildScheduledEventUserAddData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a GuildScheduledEventData User remove event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserRemoveData : public EventData<GuildScheduledEventUserRemoveData> {
		OnGuildScheduledEventUserRemoveData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an Integration creation event.
	struct DiscordCoreAPI_Dll OnIntegrationCreationData : public EventData<IntegrationCreationData> {
		OnIntegrationCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an Integration update event.
	struct DiscordCoreAPI_Dll OnIntegrationUpdateData : public EventData<IntegrationUpdateData> {
		OnIntegrationUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an Integration deletion event.
	struct DiscordCoreAPI_Dll OnIntegrationDeletionData : public EventData<IntegrationDeletionData> {
		OnIntegrationDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an Invite creation event.
	struct DiscordCoreAPI_Dll OnInviteCreationData : public EventData<InviteData> {
		OnInviteCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an Invite deletion event.
	struct DiscordCoreAPI_Dll OnInviteDeletionData : public EventData<InviteDeletionData> {
		OnInviteDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an Interaction creation event.
	struct DiscordCoreAPI_Dll OnInteractionCreationData : public EventData<InteractionData> {
		OnInteractionCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Message creation event.
	struct DiscordCoreAPI_Dll OnMessageCreationData : public EventData<MessageData> {
		OnMessageCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Message update event.
	struct DiscordCoreAPI_Dll OnMessageUpdateData : public EventData<MessageData> {
		OnMessageUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Message deletion event.
	struct DiscordCoreAPI_Dll OnMessageDeletionData : public EventData<MessageDeletionData> {
		OnMessageDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Message delete bulk event.
	struct DiscordCoreAPI_Dll OnMessageDeleteBulkData : public EventData<MessageDeletionBulkData> {
		OnMessageDeleteBulkData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Reaction add event.
	struct DiscordCoreAPI_Dll OnReactionAddData : public EventData<ReactionData> {
		OnReactionAddData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Reaction remove event.
	struct DiscordCoreAPI_Dll OnReactionRemoveData : public EventData<ReactionRemoveData> {
		ReactionRemoveData reactionRemoveData{};///< The ReactionRemoveData.
		OnReactionRemoveData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Reaction remove all event.
	struct DiscordCoreAPI_Dll OnReactionRemoveAllData : public EventData<ReactionRemoveAllData> {
		OnReactionRemoveAllData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a Reaction remove emoji event.
	struct DiscordCoreAPI_Dll OnReactionRemoveEmojiData : public EventData<ReactionRemoveEmojiData> {
		OnReactionRemoveEmojiData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a presence update event.
	struct DiscordCoreAPI_Dll OnPresenceUpdateData : public EventData<PresenceUpdateData> {
		OnPresenceUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a StageInstanceData creation event.
	struct DiscordCoreAPI_Dll OnStageInstanceCreationData : public EventData<StageInstanceData> {
		OnStageInstanceCreationData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a StageInstanceData update event.
	struct DiscordCoreAPI_Dll OnStageInstanceUpdateData : public EventData<StageInstanceData> {
		OnStageInstanceUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a StageInstanceData deletion event.
	struct DiscordCoreAPI_Dll OnStageInstanceDeletionData : public EventData<StageInstanceData> {
		OnStageInstanceDeletionData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a typing start event.
	struct DiscordCoreAPI_Dll OnTypingStartData : public EventData<TypingStartData> {
		OnTypingStartData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a User update event.
	struct DiscordCoreAPI_Dll OnUserUpdateData : public UpdatedEventData<UserData, UserData> {
		OnUserUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a voice state update event.
	struct DiscordCoreAPI_Dll OnVoiceStateUpdateData : public EventData<VoiceStateData> {
		OnVoiceStateUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse, DiscordCoreInternal::WebSocketClient* sslShard);
	};

	/// @brief Data that is received as part of a voice server update event.
	struct DiscordCoreAPI_Dll OnVoiceServerUpdateData : public EventData<VoiceServerUpdateData> {
		OnVoiceServerUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse, DiscordCoreInternal::WebSocketClient* sslShard);
	};

	/// @brief Data that is received as part of a WebHook update event.
	struct DiscordCoreAPI_Dll OnWebhookUpdateData : public EventData<WebHookUpdateData> {
		OnWebhookUpdateData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received upon the bot receiving an autocomplete entry.
	struct DiscordCoreAPI_Dll OnAutoCompleteEntryData : public EventData<InputEventData> {
		OnAutoCompleteEntryData(jsonifier::jsonifier_core& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Class for handling the assignment of event-handling functions.int32_t
	class DiscordCoreAPI_Dll EventManager {
	  public:
		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGatewayPingData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGatewayPing(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGatewayPingData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGatewayPing(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnApplicationCommandPermissionsUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onApplicationCommandsPermissionsUpdate(
			DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnAutoModerationRuleCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnAutoModerationRuleUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnAutoModerationRuleDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnAutoModerationActionExecutionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationActionExecution(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationActionExecutionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoCompleteEntry(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoCompleteEntryData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInputEventCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInputEventCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnChannelCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnChannelUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnChannelDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnChannelPinsUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelPinsUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelPinsUpdateData> handler);
		/// @brief For removing afunction from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnThreadCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnThreadUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnThreadDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnThreadListSyncData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadListSync(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadListSyncData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadListSync(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnThreadMemberUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMemberUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMemberUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnThreadMembersUpdateData structure as an argument
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMembersUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMembersUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildDeletionData structure as an argument
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildBanAddData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanAdd(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanAddData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildBanRemoveData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanRemove(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanRemoveData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildEmojisUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildEmojisUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildEmojisUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildStickersUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildStickersUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildStickersUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildIntegrationsUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildIntegrationsUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildMembersChunkData structure as an argument.
		/// @return An DiscordCoreInternal::EventDelegateToken, > for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMembersChunk(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMembersChunkData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildMemberAddData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberAdd(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberAddData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildMemberRemoveData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberRemove(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberRemoveData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildMemberUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnRoleCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnRoleUpdateData structure as an argument
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnRoleDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildScheduledEventCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildScheduledEventUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildScheduledEventDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildScheduledEventUserAddData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUserAdd(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserAddData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventUserAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnGuildScheduledEventUserRemoveData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildScheduledEventUserRemove(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildScheduledEventUserRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnIntegrationCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnIntegrationUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnIntegrationDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnInteractionCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInteractionCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInteractionCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnInviteCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInviteCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnInviteDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnMessageCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnMessageUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnMessageDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnMessageDeleteBulkData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeleteBulk(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeleteBulkData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnReactionAddData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionAdd(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionAddData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnReactionRemoveData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemove(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken,from the original event registration.
		void onReactionRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnReactionRemoveAllData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveAll(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveAllData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnReactionRemoveEmojiData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveEmoji(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveEmojiData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnPresenceUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onPresenceUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnPresenceUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnStageInstanceCreationData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceCreationData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnStageInstanceUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnStageInstanceDeletionData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceDeletionData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnTypingStartData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onTypingStart(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnTypingStartData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onTypingStart(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnUserUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onUserUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnUserUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onUserUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnVoiceStateUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceStateUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceStateUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnVoiceServerUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceServerUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceServerUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// @brief For adding a function to handle this event.
		/// @param handler A delegate taking an OnWebhookUpdateData structure as an argument.
		/// @return An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onWebhookUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnWebhookUpdateData> handler);
		/// @brief For removing a function from handling this event.
		/// @param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token);

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGatewayPingData> onGatewayPingEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> onApplicationCommandPermissionsUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleCreationData> onAutoModerationRuleCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleUpdateData> onAutoModerationRuleUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationRuleDeletionData> onAutoModerationRuleDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnAutoModerationActionExecutionData> onAutoModerationActionExecutionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnAutoCompleteEntryData> onAutoCompleteEntryEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnInputEventCreationData> onInputEventCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnChannelCreationData> onChannelCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnChannelUpdateData> onChannelUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnChannelDeletionData> onChannelDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnChannelPinsUpdateData> onChannelPinsUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnThreadCreationData> onThreadCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnThreadUpdateData> onThreadUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnThreadDeletionData> onThreadDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnThreadListSyncData> onThreadListSyncEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnThreadMemberUpdateData> onThreadMemberUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnThreadMembersUpdateData> onThreadMembersUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildCreationData> onGuildCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildUpdateData> onGuildUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildDeletionData> onGuildDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildBanAddData> onGuildBanAddEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildBanRemoveData> onGuildBanRemoveEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildEmojisUpdateData> onGuildEmojisUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildStickersUpdateData> onGuildStickersUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildIntegrationsUpdateData> onGuildIntegrationsUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberAddData> onGuildMemberAddEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberRemoveData> onGuildMemberRemoveEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMemberUpdateData> onGuildMemberUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildMembersChunkData> onGuildMembersChunkEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnRoleCreationData> onRoleCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnRoleUpdateData> onRoleUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnRoleDeletionData> onRoleDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventCreationData> onGuildScheduledEventCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUpdateData> onGuildScheduledEventUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventDeletionData> onGuildScheduledEventDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUserAddData> onGuildScheduledEventUserAddEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> onGuildScheduledEventUserRemoveEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationCreationData> onIntegrationCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationUpdateData> onIntegrationUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnIntegrationDeletionData> onIntegrationDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnInteractionCreationData> onInteractionCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnInviteCreationData> onInviteCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnInviteDeletionData> onInviteDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnMessageCreationData> onMessageCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnMessageUpdateData> onMessageUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnMessageDeletionData> onMessageDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnMessageDeleteBulkData> onMessageDeleteBulkEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnReactionAddData> onReactionAddEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveData> onReactionRemoveEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveAllData> onReactionRemoveAllEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnReactionRemoveEmojiData> onReactionRemoveEmojiEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnPresenceUpdateData> onPresenceUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceCreationData> onStageInstanceCreationEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceUpdateData> onStageInstanceUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnStageInstanceDeletionData> onStageInstanceDeletionEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnTypingStartData> onTypingStartEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnUserUpdateData> onUserUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnVoiceStateUpdateData> onVoiceStateUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnVoiceServerUpdateData> onVoiceServerUpdateEvent{};

		inline static DiscordCoreInternal::Event<CoRoutine<void>, OnWebhookUpdateData> onWebhookUpdateEvent{};
	};
	/**@}*/

}// namespace DiscordCoreAPI
