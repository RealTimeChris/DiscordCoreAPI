/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// InteractionEntities.hpp - Header for the Jsonifier::Core specializations.
/// Mar 24, 2023 Chris M.
/// https://discordcoreapi.com
/// \file JsonSpecializations.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <discordcoreapi/EventManager.hpp>

namespace Jsonifier {

	template<typename OTy> struct Core<DiscordCoreAPI::EventData<OTy>> {
		using OTy2 = DiscordCoreAPI::EventData<OTy>;
		static constexpr auto parseValue = object("d", &OTy2::value);
	};

	/*
	template<> struct Core<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> {
		using OTy2 = DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData;
		constexpr static auto parseValue = object("d", &OTy2::permissionData);
	};

	template<> struct Core<DiscordCoreAPI::OnAutoModerationRuleCreationData> {
		using OTy2 = DiscordCoreAPI::OnAutoModerationRuleCreationData;
		constexpr static auto parseValue = object("d", &OTy2::theRule);
	};

	template<> struct Core<DiscordCoreAPI::OnAutoModerationRuleUpdateData> {
		using OTy2 = DiscordCoreAPI::OnAutoModerationRuleUpdateData;
		constexpr static auto parseValue = object("d", &OTy2::theRule);
	};

	template<> struct Core<DiscordCoreAPI::OnAutoModerationRuleDeletionData> {
		using OTy2 = DiscordCoreAPI::OnAutoModerationRuleDeletionData;
		constexpr static auto parseValue = object("d", &OTy2::theRule);
	};
	
	/// \brief Data that is received as part of an AutoModerationAction execution event.
	struct DiscordCoreAPI_Dll OnAutoModerationActionExecutionData {
		AutoModerationActionExecutionEventData data{};
		OnAutoModerationActionExecutionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Channel creation event.
	struct DiscordCoreAPI_Dll OnChannelCreationData {
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
		OnChannelCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
		OnChannelCreationData& operator=(const OnChannelCreationData& other);
		OnChannelCreationData(const OnChannelCreationData& other);
	};

	/// \brief Data that is received as part of a Channel update event.
	struct DiscordCoreAPI_Dll OnChannelUpdateData {
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The new Channel.
		OnChannelUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
		OnChannelUpdateData& operator=(const OnChannelUpdateData& other);
		OnChannelUpdateData(const OnChannelUpdateData& other);
	};

	/// \brief Data that is received as part of a Channel deletion event.
	struct DiscordCoreAPI_Dll OnChannelDeletionData {
		std::unique_ptr<ChannelData> channel{ std::make_unique<ChannelData>() };///< The deleted Channel.
		OnChannelDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
		OnChannelDeletionData& operator=(const OnChannelDeletionData&);
		OnChannelDeletionData(const OnChannelDeletionData&);
	};

	/// \brief Data that is received as part of a Channel pins update event.
	struct DiscordCoreAPI_Dll OnChannelPinsUpdateData {
		ChannelPinsUpdateEventData dataPackage{};///< The Channel pins update responseData.
		OnChannelPinsUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Thread creation event.
	struct DiscordCoreAPI_Dll OnThreadCreationData {
		Thread thread{};///< The new Thread's Channel.
		OnThreadCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Thread update event.
	struct DiscordCoreAPI_Dll OnThreadUpdateData {
		Thread thread{};///< The new Thread's Channel.
		OnThreadUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Thread deletion event.
	struct DiscordCoreAPI_Dll OnThreadDeletionData {
		Thread thread{};///< The deleted Thread's Channel.
		OnThreadDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Thread list sync event.
	struct DiscordCoreAPI_Dll OnThreadListSyncData {
		ThreadListSyncData threadListSyncData{};///< The Thread list sync responseData.
		OnThreadListSyncData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Thread member update event.
	struct DiscordCoreAPI_Dll OnThreadMemberUpdateData {
		ThreadMemberData threadMember{};///< Thread member update responseData.
		OnThreadMemberUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Thread members update event.
	struct DiscordCoreAPI_Dll OnThreadMembersUpdateData {
		ThreadMembersUpdateData threadMembersUpdateData{};///< Thread member's update responseData.
		OnThreadMembersUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Guild creation event.
	struct DiscordCoreAPI_Dll OnGuildCreationData {
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
		OnGuildCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view stringData, DiscordCoreClient*);
		OnGuildCreationData& operator=(const OnGuildCreationData&);
		OnGuildCreationData(const OnGuildCreationData&);
	};

	/// \brief Data that is received as part of a Guild update event.
	struct DiscordCoreAPI_Dll OnGuildUpdateData {
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The new Guild.
		OnGuildUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view stringData, DiscordCoreClient*);
		OnGuildUpdateData& operator=(const OnGuildUpdateData&);
		OnGuildUpdateData(const OnGuildUpdateData&);
	};

	/// \brief Data that is received as part of a Guild deletion event.
	struct DiscordCoreAPI_Dll OnGuildDeletionData {
		OnGuildDeletionData() noexcept = default;
		std::unique_ptr<GuildData> guild{ std::make_unique<GuildData>() };///< The deleted Guild.
		OnGuildDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view stringData, DiscordCoreClient*);
		OnGuildDeletionData& operator=(const OnGuildDeletionData&);
		OnGuildDeletionData(const OnGuildDeletionData&);
	};

	/// \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanAddData {
		OnGuildBanAddData() noexcept = default;
		Snowflake guildId{};///< The Guild they were banned from.
		User user{};///< The User id of the person who was banned.
		OnGuildBanAddData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanRemoveData {
		Snowflake guildId{};///< The Guild they were un-banned from.
		User user{};///< The User id of the person who was un-banned.
		OnGuildBanRemoveData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Guild emojis update event.
	struct DiscordCoreAPI_Dll OnGuildEmojisUpdateData {
		GuildEmojisUpdateEventData updateData{};///< The Guild emoji's update responseData.
		OnGuildEmojisUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Guild sticker update event.
	struct DiscordCoreAPI_Dll OnGuildStickersUpdateData {
		GuildStickersUpdateEventData updateData{};///< The GuildStickersUpdateEventData.
		OnGuildStickersUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Guild Integration update event.
	struct DiscordCoreAPI_Dll OnGuildIntegrationsUpdateData {
		Snowflake guildId{};///< The id of the Guild for which the integrations were updated.
		OnGuildIntegrationsUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a GuildMember add event.
	struct DiscordCoreAPI_Dll OnGuildMemberAddData {
		std::unique_ptr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
		DiscordCoreClient* discordCoreClient{ nullptr };
		OnGuildMemberAddData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse, DiscordCoreClient*);
		OnGuildMemberAddData& operator=(const OnGuildMemberAddData&);
		OnGuildMemberAddData(const OnGuildMemberAddData&);
	};

	/// \brief Data that is received as part of a GuildMember update event.
	struct DiscordCoreAPI_Dll OnGuildMemberUpdateData {
		std::unique_ptr<GuildMemberData> guildMember{ std::make_unique<GuildMemberData>() };///< The new GuildMember.
		OnGuildMemberUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse, DiscordCoreClient*);
		OnGuildMemberUpdateData& operator=(const OnGuildMemberUpdateData&);
		OnGuildMemberUpdateData(const OnGuildMemberUpdateData&);
	};

	/// \brief Data that is received as part of a GuildMember remove event.
	struct DiscordCoreAPI_Dll OnGuildMemberRemoveData {
		std::unique_ptr<UserData> user{ std::make_unique<UserData>() };///< The User responseData of the removed GuildMember.
		DiscordCoreClient* discordCoreClient{ nullptr };
		Snowflake guildId{};///< The id of the Guild from which they were removed.
		OnGuildMemberRemoveData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse, DiscordCoreClient*);
		OnGuildMemberRemoveData& operator=(const OnGuildMemberRemoveData&);
		OnGuildMemberRemoveData(const OnGuildMemberRemoveData&);
	};

	/// \brief Data that is received as part of a GuildMembers chunk event.
	struct DiscordCoreAPI_Dll OnGuildMembersChunkData {
		GuildMembersChunkEventData chunkEventData{};///< GuildMembersChunkEventData structure.
		OnGuildMembersChunkData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Role creation event.
	struct DiscordCoreAPI_Dll OnRoleCreationData {
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
		Snowflake guildId{};///< The id of the Guild within which the Role was created.
		OnRoleCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
		OnRoleCreationData& operator=(const OnRoleCreationData&);
		OnRoleCreationData(const OnRoleCreationData&);
	};

	/// \brief Data that is received as part of a Role update event.
	struct DiscordCoreAPI_Dll OnRoleUpdateData {
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The new Role.
		Snowflake guildId{};///< The id of the Guild within which the Role was updated.
		OnRoleUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
		OnRoleUpdateData& operator=(const OnRoleUpdateData&);
		OnRoleUpdateData(const OnRoleUpdateData&);
	};

	/// \brief Data that is received as part of a Role deletion event.
	struct DiscordCoreAPI_Dll OnRoleDeletionData {
		std::unique_ptr<RoleData> role{ std::make_unique<RoleData>() };///< The deleted Role.
		Snowflake guildId{};///< The id of the Guild from which the Role was deleted.
		OnRoleDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
		OnRoleDeletionData& operator=(const OnRoleDeletionData&);
		OnRoleDeletionData(const OnRoleDeletionData&);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent creation event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventCreationData {
		GuildScheduledEventData guildScheduledEvent{};
		OnGuildScheduledEventCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent update event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUpdateData {
		GuildScheduledEventData guildScheduledEvent{};
		OnGuildScheduledEventUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent delete event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventDeletionData {
		GuildScheduledEventData guildScheduledEvent{};
		OnGuildScheduledEventDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent User add event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserAddData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
		OnGuildScheduledEventUserAddData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a GuildScheduledEvent User remove event.
	struct DiscordCoreAPI_Dll OnGuildScheduledEventUserRemoveData {
		Snowflake guildScheduledEventId{};
		Snowflake guildId{};
		Snowflake userId{};
		OnGuildScheduledEventUserRemoveData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of an Integration creation event.
	struct DiscordCoreAPI_Dll OnIntegrationCreationData {
		IntegrationData integrationData{};///< The new IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which this Integration was created.
		OnIntegrationCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of an Integration update event.
	struct DiscordCoreAPI_Dll OnIntegrationUpdateData {
		IntegrationData integrationData{};///< New IntegrationData structure.
		Snowflake guildId{};///< The id of the Guild for which the Integration was updated.
		OnIntegrationUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of an Integration deletion event.
	struct DiscordCoreAPI_Dll OnIntegrationDeletionData {
		Snowflake applicationId{};///< Application id of the current application.
		Snowflake guildId{};///< The id of the Guild for which the Integration was deleted.
		Snowflake id{};///< The id of the deleted Integration.
		OnIntegrationDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of an Invite creation event.
	struct DiscordCoreAPI_Dll OnInviteCreationData {
		InviteData invite{};///< Thew new InviteData structure.
		OnInviteCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of an Invite deletion event.
	struct DiscordCoreAPI_Dll OnInviteDeletionData {
		Snowflake channelId{};///< The id of the Channel for which the Invite existed.
		Snowflake guildId{};///< The id of the Guild for which the Invite existed.
		std::string code{};///< The code of the Invite.
		OnInviteDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of an Interaction creation event.
	struct DiscordCoreAPI_Dll OnInteractionCreationData {
		InteractionData interactionData{};///< The InteractionData representing the Interaction.
		OnInteractionCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse, DiscordCoreClient* clientPtr);
	};

	/// \brief Data that is received as part of a Message creation event.
	struct DiscordCoreAPI_Dll OnMessageCreationData {
		Message message{};///< The new Message.
		OnMessageCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Message update event.
	struct DiscordCoreAPI_Dll OnMessageUpdateData {
		Message messageNew{};///< The new Message.
		OnMessageUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Message deletion event.
	struct DiscordCoreAPI_Dll OnMessageDeletionData {
		Snowflake messageId{};///< The id of the Message which was deleted.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
		OnMessageDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Message delete bulk event.
	struct DiscordCoreAPI_Dll OnMessageDeleteBulkData {
		std::vector<Id> ids{};///< A vector containing the list of deleted Message ids.
		Snowflake channelId{};///< The id of the Channel from which the Message was deleted.
		Snowflake guildId{};///< The id of the Guild from which the Message was deleted.
		OnMessageDeleteBulkData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Reaction add event.
	struct DiscordCoreAPI_Dll OnReactionAddData {
		Reaction reaction{};///< The Reaction that was added.
		OnReactionAddData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Reaction remove event.
	struct DiscordCoreAPI_Dll OnReactionRemoveData {
		ReactionRemoveData reactionRemoveData{};///< The ReactionRemoveData.
		OnReactionRemoveData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Reaction remove all event.
	struct DiscordCoreAPI_Dll OnReactionRemoveAllData {
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
		OnReactionRemoveAllData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a Reaction remove emoji event.
	struct DiscordCoreAPI_Dll OnReactionRemoveEmojiData {
		Snowflake messageId{};///< The id of the Message from which the Reactions were deleted.
		Snowflake channelId{};///< The id of the Channel from which the Reactions were deleted.
		Snowflake guildId{};///< The id of the Guild from which the Reactions were deleted.
		EmojiData emoji{};///< The id of the Emoji which was removed from the Message.
		OnReactionRemoveEmojiData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a presence update event.
	struct DiscordCoreAPI_Dll OnPresenceUpdateData {
		PresenceUpdateData presenceData{};///< PresenceUpdateData..
		OnPresenceUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a StageInstance creation event.
	struct DiscordCoreAPI_Dll OnStageInstanceCreationData {
		StageInstanceData stageInstance{};///< The new StageInstanceData.
		OnStageInstanceCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a StageInstance update event.
	struct DiscordCoreAPI_Dll OnStageInstanceUpdateData {
		StageInstanceData stageInstance{};///< The new StageInstanceData.
		OnStageInstanceUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a StageInstance deletion event.
	struct DiscordCoreAPI_Dll OnStageInstanceDeletionData {
		StageInstanceData stageInstance{};///< The deleted StageInstanceData.
		OnStageInstanceDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a typing start event.
	struct DiscordCoreAPI_Dll OnTypingStartData {
		TypingStartData typingStartData{};///< TypingStartData of the event.
		OnTypingStartData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
	};

	/// \brief Data that is received as part of a User update event.
	struct DiscordCoreAPI_Dll OnUserUpdateData {
		std::unique_ptr<UserData> user{ std::make_unique<UserData>() };///< The new User.
		OnUserUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse);
		OnUserUpdateData& operator=(const OnUserUpdateData&);
	}
	

	template<> struct Core<DiscordCoreAPI::OnGuildCreationData> {
		using OTy2 = DiscordCoreAPI::OnGuildCreationData;
		constexpr static auto parseValue = object("d", &OTy2::value);
	};

	template<> struct Core<DiscordCoreAPI::OnGuildUpdateData> {
		using OTy2 = DiscordCoreAPI::OnGuildUpdateData;
		constexpr static auto parseValue = object("d", &OTy2::value);
	};

	template<> struct Core<DiscordCoreAPI::OnGuildDeletionData> {
		using OTy2 = DiscordCoreAPI::OnGuildUpdateData;
		constexpr static auto parseValue = object("d", &OTy2::value);
	};

	template<> struct Core<DiscordCoreAPI::OnInteractionCreationData> {
		using OTy2 = DiscordCoreAPI::OnInteractionCreationData;
		constexpr static auto parseValue = object("d", &OTy2::interactionData);
	};

	template<> struct Core<DiscordCoreAPI::OnPresenceUpdateData> {
		using OTy2 = DiscordCoreAPI::OnPresenceUpdateData;
		constexpr static auto parseValue = object("d", &OTy2::presenceData);
	};
	*/

};