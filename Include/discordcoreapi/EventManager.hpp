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
		OnChannelCreationData(std::unique_ptr<ChannelData>);
		OnChannelCreationData& operator=(const OnChannelCreationData&);
		OnChannelCreationData(const OnChannelCreationData&);
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
	};

	/// Data that is received as part of a Channel update event. \brief Data that is received as part of a Channel update event.
	struct DiscordCoreAPI_Dll OnChannelUpdateData {
		OnChannelUpdateData(std::unique_ptr<ChannelData>);
		OnChannelUpdateData& operator=(const OnChannelUpdateData&);
		OnChannelUpdateData(const OnChannelUpdateData&);
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
	};

	/// Data that is received as part of a Channel deletion event. \brief Data that is received as part of a Channel deletion event.
	struct DiscordCoreAPI_Dll OnChannelDeletionData {
		OnChannelDeletionData(std::unique_ptr<ChannelData>);
		OnChannelDeletionData& operator=(const OnChannelDeletionData&);
		OnChannelDeletionData(const OnChannelDeletionData&);
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The deleted Channel.
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
		OnGuildCreationData(std::unique_ptr<GuildData> guild, DiscordCoreClient*);
		OnGuildCreationData& operator=(const OnGuildCreationData&);
		OnGuildCreationData(const OnGuildCreationData&);
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
	};

	/// Data that is received as part of a Guild update event. \brief Data that is received as part of a Guild update event.
	struct DiscordCoreAPI_Dll OnGuildUpdateData {
		OnGuildUpdateData(std::unique_ptr<GuildData> guild, DiscordCoreClient*);
		OnGuildUpdateData& operator=(const OnGuildUpdateData&);
		OnGuildUpdateData(const OnGuildUpdateData&);
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
	};

	/// Data that is received as part of a Guild deletion event. \brief Data that is received as part of a Guild deletion event.
	struct DiscordCoreAPI_Dll OnGuildDeletionData {
		OnGuildDeletionData(std::unique_ptr<GuildData>);
		OnGuildDeletionData& operator=(const OnGuildDeletionData&);
		OnGuildDeletionData(const OnGuildDeletionData&);
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The deleted Guild.
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
		OnGuildMemberAddData(std::unique_ptr<GuildMemberData>, DiscordCoreClient*);
		OnGuildMemberAddData& operator=(const OnGuildMemberAddData&);
		OnGuildMemberAddData(const OnGuildMemberAddData&);
		DiscordCoreClient* discordCoreClient{ nullptr };
		std::unique_ptr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
	};

	/// Data that is received as part of a GuildMember update event. \brief Data that is received as part of a GuildMember update event.
	struct DiscordCoreAPI_Dll OnGuildMemberUpdateData {
		OnGuildMemberUpdateData(std::unique_ptr<GuildMemberData>);
		OnGuildMemberUpdateData& operator=(const OnGuildMemberUpdateData&);
		OnGuildMemberUpdateData(const OnGuildMemberUpdateData&);
		std::unique_ptr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
	};

	/// Data that is received as part of a GuildMember remove event. \brief Data that is received as part of a GuildMember remove event.
	struct DiscordCoreAPI_Dll OnGuildMemberRemoveData {
		OnGuildMemberRemoveData(std::unique_ptr<UserData>, DiscordCoreClient*, Snowflake);
		OnGuildMemberRemoveData& operator=(const OnGuildMemberRemoveData&);
		OnGuildMemberRemoveData(const OnGuildMemberRemoveData&);
		DiscordCoreClient* discordCoreClient{ nullptr };
		std::unique_ptr<UserData> user{ std::make_unique<UserData>() };///< The User responseData of the removed GuildMember.
		Snowflake guildId{};///< The id of the Guild from which they were removed.
	};

	/// Data that is received as part of a GuildMembers chunk event. \brief Data that is received as part of a GuildMembers chunk event.
	struct DiscordCoreAPI_Dll OnGuildMembersChunkData {
		GuildMembersChunkEventData chunkEventData{};///< GuildMembersChunkEventData structure.
	};

	/// Data that is received as part of a Role creation event. \brief Data that is received as part of a Role creation event.
	struct DiscordCoreAPI_Dll OnRoleCreationData {
		OnRoleCreationData(std::unique_ptr<RoleData> theRole, Snowflake guildId);
		OnRoleCreationData& operator=(const OnRoleCreationData&);
		OnRoleCreationData(const OnRoleCreationData&);
		Snowflake guildId{};///< The id of the Guild within which the Role was created.
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
	};

	/// Data that is received as part of a Role update event. \brief Data that is received as part of a Role update event.
	struct DiscordCoreAPI_Dll OnRoleUpdateData {
		OnRoleUpdateData(std::unique_ptr<RoleData>, Snowflake);
		OnRoleUpdateData& operator=(const OnRoleUpdateData&);
		OnRoleUpdateData(const OnRoleUpdateData&);
		Snowflake guildId{};///< The id of the Guild within which the Role was updated.
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
	};

	/// Data that is received as part of a Role deletion event. \brief Data that is received as part of a Role deletion event.
	struct DiscordCoreAPI_Dll OnRoleDeletionData {
		OnRoleDeletionData(std::unique_ptr<RoleData>, Snowflake);
		OnRoleDeletionData& operator=(const OnRoleDeletionData&);
		OnRoleDeletionData(const OnRoleDeletionData&);
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The deleted Role.
		Snowflake guildId{};///< The id of the Guild from which the Role was deleted.
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
		std::string code{};///< The code of the Invite.
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
		std::vector<Snowflake> ids{};///< A vector containing the list of deleted Message ids.
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
		OnUserUpdateData(std::unique_ptr<UserData>);
		OnUserUpdateData& operator=(const OnUserUpdateData&);
		OnUserUpdateData(const OnUserUpdateData&);
		std::unique_ptr<UserData> user{ std::make_unique<UserData>() };///< The new User.
	};

	/// Data that is received as part of a voice state update event. \brief Data that is received as part of a voice state update event.
	struct DiscordCoreAPI_Dll OnVoiceStateUpdateData {
		VoiceStateData voiceStateData{};///< VoiceStateData for the new voice state.
	};

	/// Data that is received as part of a voice server update event. \brief Data that is received as part of a voice server update event.
	struct DiscordCoreAPI_Dll OnVoiceServerUpdateData {
		std::string endpoint{};///< The new endpoint.
		std::string token{};///< The token of the server update event.
		Snowflake guildId{};///< The id of the Guild for which the server update is occurring.
	};

	template<> void parseObject(nlohmann::json& theJsonData, OnVoiceServerUpdateData& theData);

	/// Data that is received as part of a WebHook update event. \brief Data that is received as part of a WebHook update event.
	struct DiscordCoreAPI_Dll OnWebhookUpdateData {
		Snowflake channelId{};///< Id of the Channel for which the WebHook Update is occurring.
		Snowflake guildId{};///< Id of the Guild for which the WebHook Update is occurring.
	};

	/// Data that is received upon the bot receiving an autocomplete entry. \brief Data that is received upon the bot receiving an autocomplete entry.
	struct DiscordCoreAPI_Dll OnAutoCompleteEntryData {
		InputEventData inputEvent{};///< The input-event representing the autocomplete entry.
	};

	/// Class for handling the assignment of event-handling functions. \brief Class for handling the assignment of event-handling functions.int32_t
	class DiscordCoreAPI_Dll EventManager {
	  public:
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandPermissionsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onApplicationCommandsPermissionsUpdate(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationRuleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationRuleDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnAutoModerationActionExecutionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoModerationActionExecution(
			const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationActionExecutionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token);

		// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onAutoCompleteEntry(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoCompleteEntryData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInputEventCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInputEventCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelPinsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onChannelPinsUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelPinsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing afunction from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadListSyncData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadListSync(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadListSyncData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadListSync(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMemberUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMemberUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMembersUpdateData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onThreadMembersUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMembersUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildDeletionData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildBanRemove(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildEmojisUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildEmojisUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildEmojisUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildStickersUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildStickersUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildStickersUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildIntegrationsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildIntegrationsUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildIntegrationsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMembersChunkData structure as an argument.
		/// returns An DiscordCoreInternal::EventDelegateToken, > for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMembersChunk(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMembersChunkData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberRemove(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onGuildMemberUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleUpdateData structure as an argument
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onRoleDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onIntegrationDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInteractionCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInteractionCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInteractionCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInviteCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onInviteDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeleteBulkData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onMessageDeleteBulk(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeleteBulkData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionAdd(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionAdd(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemove(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken,from the original event registration.
		void onReactionRemove(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveAllData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveAll(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveAllData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveEmojiData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onReactionRemoveEmoji(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveEmojiData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnPresenceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onPresenceUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnPresenceUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceCreation(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onStageInstanceDeletion(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnTypingStartData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onTypingStart(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnTypingStartData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onTypingStart(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnUserUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onUserUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnUserUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onUserUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceStateUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceStateUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceStateUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceServerUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onVoiceServerUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceServerUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnWebhookUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		DiscordCoreInternal::EventDelegateToken onWebhookUpdate(const DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnWebhookUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An DiscordCoreInternal::EventDelegateToken, from the original event registration.
		void onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token);

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
