// EventManager.hpp - Header for the event manager class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "EventEntities.hpp"
#include "ChannelEntities.hpp"
#include "GuildEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "UserEntities.hpp"
#include "RoleEntities.hpp"
#include "ReactionEntities.hpp"
#include "MessageEntities.hpp"
#include "UserEntities.hpp"
#include "ApplicationCommandEntities.hpp"
#include "ThreadEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup discord_events
	* @{
	*/
	/// Data that is received as part of an InputEvent creation event. \brief Data that is received as part of an InputEvent creation event.
	struct DiscordCoreAPI_Dll OnInputEventCreationData {
		InputEventData inputEventData{};///< InputEventData representing the input-event.
	};

	/// Data that is received as part of an ApplicationCommand creation event. \brief Data that is received as part of an ApplicationCommand creation event.
	struct DiscordCoreAPI_Dll OnApplicationCommandCreationData {
		ApplicationCommand applicationCommand{}; ///< The new ApplicationCommand.
	};

	/// Data that is received as part of an ApplicationCommand update event. \brief Data that is received as part of an ApplicationCommand update event.
	struct DiscordCoreAPI_Dll OnApplicationCommandUpdateData {
		ApplicationCommand applicationCommand{}; ///< The new ApplicationCommand.
	};

	/// Data that is received as part of an ApplicationCommand deletion event. \brief Data that is received as part of an ApplicationCommand deletion event.
	struct DiscordCoreAPI_Dll OnApplicationCommandDeletionData {
		ApplicationCommand applicationCommand{}; ///< The deleted ApplicationCommand.
	};

	/// Data that is received as part of a Channel creation event. \brief Data that is received as part of a Channel creation event.
	struct DiscordCoreAPI_Dll OnChannelCreationData {
		Channel channel{}; ///< The new Channel.
	};

	/// Data that is received as part of a Channel update event. \brief Data that is received as part of a Channel update event.
	struct DiscordCoreAPI_Dll OnChannelUpdateData {
		Channel channelOld{}; ///< The old Channel.
		Channel channelNew{};	///< The new Channel.
	};

	/// Data that is received as part of a Channel deletion event. \brief Data that is received as part of a Channel deletion event.
	struct DiscordCoreAPI_Dll OnChannelDeletionData {
		Channel channel{}; ///< The deleted Channel.
	};
	
	/// Data that is received as part of a Channel pins update event. \brief Data that is received as part of a Channel pins update event.
	struct DiscordCoreAPI_Dll OnChannelPinsUpdateData {
		ChannelPinsUpdateEventData dataPackage{}; ///< The Channel pins update responseData.
	};

	/// Data that is received as part of a Thread creation event. \brief Data that is received as part of a Thread creation event.
	struct DiscordCoreAPI_Dll OnThreadCreationData {
		Thread thread{}; ///< The new Thread's Channel.
	};

	/// Data that is received as part of a Thread update event. \brief Data that is received as part of a Thread update event.
	struct DiscordCoreAPI_Dll OnThreadUpdateData {
		Thread thread{}; ///< The new Thread's Channel.
	};

	/// Data that is received as part of a Thread deletion event. \brief Data that is received as part of a Thread deletion event.
	struct DiscordCoreAPI_Dll OnThreadDeletionData {
		Thread thread{}; ///< The deleted Thread's Channel.
	};

	/// Data that is received as part of a Thread list sync event. \brief Data that is received as part of a Thread list sync event.
	struct DiscordCoreAPI_Dll OnThreadListSyncData {
		ThreadListSyncData threadListSyncData{}; ///< The Thread list sync responseData.
	};

	/// Data that is received as part of a Thread member update event. \brief Data that is received as part of a Thread member update event.
	struct DiscordCoreAPI_Dll OnThreadMemberUpdateData {
		ThreadMemberData threadMember{}; ///< Thread member update responseData.
	};

	/// Data that is received as part of a Thread members update event. \brief Data that is received as part of a Thread members update event.
	struct DiscordCoreAPI_Dll OnThreadMembersUpdateData {
		ThreadMembersUpdateData threadMembersUpdateData{}; ///< Thread member's update responseData.
	};

	/// Data that is received as part of a Guild creation event. \brief Data that is received as part of a Guild creation event.
	struct DiscordCoreAPI_Dll OnGuildCreationData {
		Guild guild{}; ///< The new Guild.
	};

	/// Data that is received as part of a Guild update event. \brief Data that is received as part of a Guild update event.
	struct DiscordCoreAPI_Dll OnGuildUpdateData {
		Guild guildOld{}; ///< The old Guild.
		Guild guildNew{};	///< The new, updated Guild.
	};

	/// Data that is received as part of a Guild deletion event. \brief Data that is received as part of a Guild deletion event.
	struct DiscordCoreAPI_Dll OnGuildDeletionData {
		Guild guild{}; ///< The deleted Guild.
	};

	/// Data that is received as part of a Guild ban add event. \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanAddData {
		string guildId{ "" }; ///< The Guild they were banned from.
		User user{};	///< The User id of the person who was banned.
	};

	/// Data that is received as part of a Guild ban remove event. \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanRemoveData {
		string guildId{ "" }; ///< The Guild they were un-banned from.
		User user{};///< The User id of the person who was un-banned.
	};

	/// Data that is received as part of a Guild emojis update event. \brief Data that is received as part of a Guild emojis update event.
	struct DiscordCoreAPI_Dll OnGuildEmojisUpdateData {
		GuildEmojisUpdateEventData updateData{}; ///< The Guild emoji's update responseData.
	};

	/// Data that is received as part of a Guild sticker update event. \brief Data that is received as part of a Guild sticker update event.
	struct DiscordCoreAPI_Dll OnGuildStickersUpdateData {
		GuildStickersUpdateEventData updateData{}; ///< The GuildStickersUpdateEventData.
	};

	/// Data that is received as part of a Guild Integration update event. \brief Data that is received as part of a Guild Integration update event.
	struct DiscordCoreAPI_Dll OnGuildIntegrationsUpdateData {
		string guildId{ "" }; ///< The id of the Guild for which the integrations were updated.
	};

	/// Data that is received as part of a GuildMember add event. \brief Data that is received as part of a GuildMember add event.
	struct DiscordCoreAPI_Dll OnGuildMemberAddData {
		GuildMember guildMember{}; ///< The new GuildMember.
	};

	/// Data that is received as part of a GuildMember remove event. \brief Data that is received as part of a GuildMember remove event.
	struct DiscordCoreAPI_Dll OnGuildMemberRemoveData {
		User user{}; ///< The User responseData of the removed GuildMember.
		string guildId{ "" }; ///< The id of the Guild from which they were removed.
	};

	/// Data that is received as part of a GuildMember update event. \brief Data that is received as part of a GuildMember update event.
	struct DiscordCoreAPI_Dll OnGuildMemberUpdateData {
		GuildMember guildMemberOld{}; ///< The old GuildMember.
		GuildMember guildMemberNew{};///< The new GuildMember.
	};

	/// Data that is received as part of a GuildMembers chunk event. \brief Data that is received as part of a GuildMembers chunk event.
	struct DiscordCoreAPI_Dll OnGuildMembersChunkData {
		GuildMembersChunkEventData chunkEventData{}; ///< GuildMembersChunkEventData structure.
	};

	/// Data that is received as part of a Role creation event. \brief Data that is received as part of a Role creation event.
	struct DiscordCoreAPI_Dll OnRoleCreationData {
		Role role{}; ///< The new Role.
		string guildId{ "" };///< The id of the Guild within which the Role was created.
	};

	/// Data that is received as part of a Role update event. \brief Data that is received as part of a Role update event.
	struct DiscordCoreAPI_Dll OnRoleUpdateData {
		string guildId{ "" }; ///< The id of the Guild within which the Role was updated.
		Role roleOld{}; ///< The old Role.
		Role roleNew{};	///< The new Role.
	};

	/// Data that is received as part of a Role deletion event. \brief Data that is received as part of a Role deletion event.
	struct DiscordCoreAPI_Dll OnRoleDeletionData {
		string guildId{ "" }; ///< The id of the Guild from which the Role was deleted.
		Role roleOld{}; ///< The deleted Role.
	};

	/// Data that is received as part of an Integration creation event. \brief Data that is received as part of an Integration creation event.
	struct DiscordCoreAPI_Dll OnIntegrationCreationData {
		IntegrationData integrationData{}; ///< The new IntegrationData structure.
		string guildId{ "" };///< The id of the Guild for which this Integration was created.
	};

	/// Data that is received as part of an Integration update event. \brief Data that is received as part of an Integration update event.
	struct DiscordCoreAPI_Dll OnIntegrationUpdateData {
		IntegrationData integrationData{}; ///< New IntegrationData structure.
		string guildId{ "" };///< The id of the Guild for which the Integration was updated.
	};

	/// Data that is received as part of an Integration deletion event. \brief Data that is received as part of an Integration deletion event.
	struct DiscordCoreAPI_Dll OnIntegrationDeletionData {
		string applicationId{ "" }; ///< Application id of the current application.
		string guildId{ "" };///< The id of the Guild for which the Integration was deleted.
		string id{ "" };///< The id of the deleted Integration.
	};

	/// Data that is received as part of an Invite creation event. \brief Data that is received as part of an Invite creation event.
	struct DiscordCoreAPI_Dll OnInviteCreationData {
		InviteData invite{}; ///< Thew new InviteData structure.
	};

	/// Data that is received as part of an Invite deletion event. \brief Data that is received as part of an Invite deletion event.
	struct DiscordCoreAPI_Dll OnInviteDeletionData {
		string channelId{ "" }; ///< The id of the Channel for which the Invite existed.
		string guildId{ "" };	///< The id of the Guild for which the Invite existed.
		string code{ "" }; ///< The code of the Invite.
	};

	/// Data that is received as part of an Interaction creation event. \brief Data that is received as part of an Interaction creation event.
	struct DiscordCoreAPI_Dll OnInteractionCreationData {
		InteractionData interactionData{};///< The InteractionData representing the Interaction.
	};

	/// Data that is received as part of a Message creation event. \brief Data that is received as part of a Message creation event.
	struct DiscordCoreAPI_Dll OnMessageCreationData {
		Message message{}; ///< The new Message.
	};

	/// Data that is received as part of a Message update event. \brief  Data that is received as part of a Message update event.
	struct DiscordCoreAPI_Dll OnMessageUpdateData {
		Message messageNew{}; ///< The new Message.
	};

	/// Data that is received as part of a Message deletion event. \brief Data that is received as part of a Message deletion event.
	struct DiscordCoreAPI_Dll OnMessageDeletionData {
		string messageId{ "" };///< The id of the Message which was deleted.
		string channelId{ "" };///< The id of the Channel from which the Message was deleted.
		string guildId{ "" };///< The id of the Guild from which the Message was deleted.
	};

	/// Data that is received as part of a Message delete bulk event. \brief Data that is received as part of a Message delete bulk event.
	struct DiscordCoreAPI_Dll OnMessageDeleteBulkData {
		string channelId{ "" };///< The id of the Channel from which the Message was deleted.
		string guildId{ "" };///< The id of the Guild from which the Message was deleted.
		vector<string> ids{};///< A vector containing the list of deleted Message ids.
	};

	/// Data that is received as part of a Reaction add event. \brief Data that is received as part of a Reaction add event.
	struct DiscordCoreAPI_Dll OnReactionAddData {
		Reaction reaction{};///< The Reaction that was added.
	};

	/// Data that is received as part of a Reaction remove event. \brief Data that is received as part of a Reaction remove event.
	struct DiscordCoreAPI_Dll OnReactionRemoveData {
		ReactionRemoveData reactionRemoveData{}; ///< The ReactionRemoveData.
	};

	/// Data that is received as part of a Reaction remove all event. \brief Data that is received as part of a Reaction remove all event.
	struct DiscordCoreAPI_Dll OnReactionRemoveAllData {
		string channelId{ "" };///< The id of the Channel from which the Reactions were deleted.
		string messageId{ "" };///< The id of the Message from which the Reactions were deleted.
		string guildId{ "" };///< The id of the Guild from which the Reactions were deleted.
	};

	/// Data that is received as part of a Reaction remove emoji event. \brief Data that is received as part of a Reaction remove emoji event.
	struct DiscordCoreAPI_Dll OnReactionRemoveEmojiData {
		string messageId{ "" };///< The id of the Message from which the Reactions were deleted.
		string channelId{ "" };///< The id of the Channel from which the Reactions were deleted.
		string guildId{ "" };///< The id of the Guild from which the Reactions were deleted.
		EmojiData emoji{};///< The id of the Emoji which was removed from the Message.
	};

	/// Data that is received as part of a presence update event. \brief Data that is received as part of a presence update event.
	struct DiscordCoreAPI_Dll OnPresenceUpdateData {
		PresenceUpdateData presenceData{}; ///< PresenceUpdateData..
	};

	/// Data that is received as part of a StageInstance creation event. \brief Data that is received as part of a StageInstance creation event.
	struct DiscordCoreAPI_Dll OnStageInstanceCreationData {
		StageInstanceData stageInstance{}; ///< The new StageInstanceData.
	};

	/// Data that is received as part of a StageInstance update event. \brief Data that is received as part of a StageInstance update event.
	struct DiscordCoreAPI_Dll OnStageInstanceUpdateData {
		StageInstanceData stageInstance{}; ///< The new StageInstanceData.
	};

	/// Data that is received as part of a StageInstance deletion event. \brief Data that is received as part of a StageInstance deletion event.
	struct DiscordCoreAPI_Dll OnStageInstanceDeletionData {
		StageInstanceData stageInstance{}; ///< The deleted StageInstanceData.
	};

	/// Data that is received as part of a typing start event. \brief Data that is received as part of a typing start event.
	struct DiscordCoreAPI_Dll OnTypingStartData {
		TypingStartData typingStartData{}; ///< TypingStartData of the event.
	};

	/// Data that is received as part of a User update event. \brief Data that is received as part of a User update event.
	struct DiscordCoreAPI_Dll OnUserUpdateData {
		User userOld{}; ///< The old User.
		User userNew{};///< The new User.
	};

	/// Data that is received as part of a voice state update event. \brief Data that is received as part of a voice state update event.
	struct DiscordCoreAPI_Dll OnVoiceStateUpdateData {
		VoiceStateData voiceStateData{};///< VoiceStateData for the new voice state.
	};

	/// Data that is received as part of a voice server update event. \brief Data that is received as part of a voice server update event.
	struct DiscordCoreAPI_Dll OnVoiceServerUpdateData {
		string endpoint{ "" };///< The new endpoint.
		string guildId{ "" };///< The id of the Guild for which the server update is occurring.
		string token{ "" };///< The token of the server update event.
	};

	/// Data that is received as part of a WebHook update event. \brief Data that is received as part of a WebHook update event.
	struct DiscordCoreAPI_Dll OnWebhookUpdateData {
		string channelId{ "" };///< Id of the Channel for which the WebHook Update is occurring.
		string guildId{ "" };///< Id of the Guild for which the WebHook Update is occurring.
	};

	/// class for handling the assignment of event-handling functions. \brief class for handling the assignment of event-handling functions.int32_t
	class DiscordCoreAPI_Dll EventManager {
	public:

		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreClient;
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onInputEventCreation(EventDelegate<void, OnInputEventCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onInputEventCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onApplicationCommandCreation(EventDelegate<void, OnApplicationCommandCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onApplicationCommandCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onApplicationCommandUpdate(EventDelegate<void, OnApplicationCommandUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onApplicationCommandUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onApplicationCommandDeletion(EventDelegate<void, OnApplicationCommandDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onApplicationCommandDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onChannelCreation(EventDelegate<void, OnChannelCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onChannelCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onChannelUpdate(EventDelegate<void, OnChannelUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onChannelUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onChannelDeletion(EventDelegate<void, OnChannelDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onChannelDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelPinsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onChannelPinsUpdate(EventDelegate<void, OnChannelPinsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onChannelPinsUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onThreadCreation(EventDelegate<void, OnThreadCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onThreadCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onThreadUpdate(EventDelegate<void, OnThreadUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onThreadUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onThreadDeletion(EventDelegate<void, OnThreadDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onThreadDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadListSyncData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onThreadListSync(EventDelegate<void, OnThreadListSyncData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onThreadListSync(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onThreadMemberUpdate(EventDelegate<void, OnThreadMemberUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onThreadMemberUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMembersUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onThreadMembersUpdate(EventDelegate<void, OnThreadMembersUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onThreadMembersUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildCreation(EventDelegate<void, OnGuildCreationData>  handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onGuildCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildUpdate(EventDelegate<void, OnGuildUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onGuildUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildDeletion(EventDelegate<void, OnGuildDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		void onGuildDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildBanAdd(EventDelegate<void, OnGuildBanAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildBanAdd(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildBanRemove(EventDelegate<void, OnGuildBanRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token EventDelegateToken<void, from the original event registration.
		
		void onGuildBanRemove(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildEmojisUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildEmojisUpdate(EventDelegate<void, OnGuildEmojisUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildEmojisUpdate(EventDelegateToken token);
		
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildStickersUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildStickersUpdate(EventDelegate<void, OnGuildStickersUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildStickersUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildIntegrationsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildIntegrationsUpdate(EventDelegate<void, OnGuildIntegrationsUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildIntegrationsUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMembersChunkData structure as an argument.
		/// \returns An EventDelegateToken<void, > for later de-registering the event.
		EventDelegateToken onGuildMembersChunk(EventDelegate<void, OnGuildMembersChunkData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildMembersChunk(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildMemberAdd(EventDelegate<void, OnGuildMemberAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildMemberAdd(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildMemberRemove(EventDelegate<void, OnGuildMemberRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildMemberRemove(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onGuildMemberUpdate(EventDelegate<void, OnGuildMemberUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onGuildMemberUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onRoleCreation(EventDelegate<void, OnRoleCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onRoleCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onRoleUpdate(EventDelegate<void, OnRoleUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onRoleUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onRoleDeletion(EventDelegate<void, OnRoleDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onRoleDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onIntegrationCreation(EventDelegate<void, OnIntegrationCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onIntegrationCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onIntegrationUpdate(EventDelegate<void, OnIntegrationUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onIntegrationUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onIntegrationDeletion(EventDelegate<void, OnIntegrationDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onIntegrationDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInteractionCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onInteractionCreation(EventDelegate<void, OnInteractionCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onInteractionCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onInviteCreation(EventDelegate<void, OnInviteCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onInviteCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onInviteDeletion(EventDelegate<void, OnInviteDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onInviteDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onMessageCreation(EventDelegate<void, OnMessageCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onMessageCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onMessageUpdate(EventDelegate<void, OnMessageUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onMessageUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onMessageDeletion(EventDelegate<void, OnMessageDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onMessageDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeleteBulkData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onMessageDeleteBulk(EventDelegate<void, OnMessageDeleteBulkData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onMessageDeleteBulk(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onReactionAdd(EventDelegate<void, OnReactionAddData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onReactionAdd(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onReactionRemove(EventDelegate<void, OnReactionRemoveData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onReactionRemove(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveAllData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onReactionRemoveAll(EventDelegate<void, OnReactionRemoveAllData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onReactionRemoveAll(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveEmojiData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onReactionRemoveEmoji(EventDelegate<void, OnReactionRemoveEmojiData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onReactionRemoveEmoji(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnPresenceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onPresenceUpdate(EventDelegate<void, OnPresenceUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onPresenceUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onStageInstanceCreation(EventDelegate<void, OnStageInstanceCreationData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onStageInstanceCreation(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onStageInstanceUpdate(EventDelegate<void, OnStageInstanceUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onStageInstanceUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onStageInstanceDeletion(EventDelegate<void, OnStageInstanceDeletionData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onStageInstanceDeletion(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnTypingStartData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onTypingStart(EventDelegate<void, OnTypingStartData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onTypingStart(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnUserUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onUserUpdate(EventDelegate<void, OnUserUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onUserUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceStateUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onVoiceStateUpdate(EventDelegate<void, OnVoiceStateUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onVoiceStateUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceServerUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onVoiceServerUpdate(EventDelegate<void, OnVoiceServerUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onVoiceServerUpdate(EventDelegateToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnWebhookUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		EventDelegateToken onWebhookUpdate(EventDelegate<void, OnWebhookUpdateData> handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventDelegateToken<void, from the original event registration.
		
		void onWebhookUpdate(EventDelegateToken token);

	protected:

		Event<void, OnInputEventCreationData> onInputEventCreationEvent{};

		Event<void, OnApplicationCommandCreationData> onApplicationCommandCreationEvent{};

		Event<void, OnApplicationCommandUpdateData> onApplicationCommandUpdateEvent{};

		Event<void, OnApplicationCommandDeletionData> onApplicationCommandDeletionEvent{};

		Event<void, OnChannelCreationData> onChannelCreationEvent{};

		Event<void, OnChannelUpdateData> onChannelUpdateEvent{};

		Event<void, OnChannelDeletionData> onChannelDeletionEvent{};

		Event<void, OnChannelPinsUpdateData> onChannelPinsUpdateEvent{};

		Event<void, OnThreadCreationData> onThreadCreationEvent{};

		Event<void, OnThreadUpdateData> onThreadUpdateEvent{};

		Event<void, OnThreadDeletionData> onThreadDeletionEvent{};

		Event<void, OnThreadListSyncData> onThreadListSyncEvent{};

		Event<void, OnThreadMemberUpdateData> onThreadMemberUpdateEvent{};

		Event<void, OnThreadMembersUpdateData> onThreadMembersUpdateEvent{};

		Event<void, OnGuildCreationData> onGuildCreationEvent{};

		Event<void, OnGuildUpdateData> onGuildUpdateEvent{};

		Event<void, OnGuildDeletionData> onGuildDeletionEvent{};

		Event<void, OnGuildBanAddData> onGuildBanAddEvent{};

		Event<void, OnGuildBanRemoveData> onGuildBanRemoveEvent{};

		Event<void, OnGuildEmojisUpdateData> onGuildEmojisUpdateEvent{};

		Event<void, OnGuildStickersUpdateData> onGuildStickersUpdateEvent{};

		Event<void, OnGuildIntegrationsUpdateData> onGuildIntegrationsUpdateEvent{};

		Event<void, OnGuildMemberAddData> onGuildMemberAddEvent{};

		Event<void, OnGuildMemberRemoveData> onGuildMemberRemoveEvent{};

		Event<void, OnGuildMemberUpdateData> onGuildMemberUpdateEvent{};

		Event<void, OnGuildMembersChunkData> onGuildMembersChunkEvent{};

		Event<void, OnRoleCreationData> onRoleCreationEvent{};

		Event<void, OnRoleUpdateData> onRoleUpdateEvent{};

		Event<void, OnRoleDeletionData> onRoleDeletionEvent{};

		Event<void, OnIntegrationCreationData> onIntegrationCreationEvent{};

		Event<void, OnIntegrationUpdateData> onIntegrationUpdateEvent{};

		Event<void, OnIntegrationDeletionData> onIntegrationDeletionEvent{};

		Event<void, OnInteractionCreationData> onInteractionCreationEvent{};

		Event<void, OnInviteCreationData> onInviteCreationEvent{};

		Event<void, OnInviteDeletionData> onInviteDeletionEvent{};

		Event<void, OnMessageCreationData> onMessageCreationEvent{};

		Event<void, OnMessageUpdateData> onMessageUpdateEvent{};

		Event<void, OnMessageDeletionData> onMessageDeletionEvent{};

		Event<void, OnMessageDeleteBulkData> onMessageDeleteBulkEvent{};

		Event<void, OnReactionAddData> onReactionAddEvent{};

		Event<void, OnReactionRemoveData> onReactionRemoveEvent{};

		Event<void, OnReactionRemoveAllData> onReactionRemoveAllEvent{};

		Event<void, OnReactionRemoveEmojiData> onReactionRemoveEmojiEvent{};

		Event<void, OnPresenceUpdateData> onPresenceUpdateEvent{};

		Event<void, OnStageInstanceCreationData> onStageInstanceCreationEvent{};

		Event<void, OnStageInstanceUpdateData> onStageInstanceUpdateEvent{};

		Event<void, OnStageInstanceDeletionData> onStageInstanceDeletionEvent{};

		Event<void, OnTypingStartData> onTypingStartEvent{};

		Event<void, OnUserUpdateData> onUserUpdateEvent{};

		Event<void, OnVoiceStateUpdateData> onVoiceStateUpdateEvent{};

		Event<void, OnVoiceServerUpdateData> onVoiceServerUpdateEvent{};

		Event<void, OnWebhookUpdateData> onWebhookUpdateEvent{};

	};
	/**@}*/
	class DiscordCoreAPI_Dll EventHandler {
	public:
		
		static void onChannelCreation(OnChannelCreationData dataPackage);

		static void onChannelUpdate(OnChannelUpdateData dataPackage);

		static void onChannelDeletion(OnChannelDeletionData dataPackage);

		static void onGuildCreation(OnGuildCreationData dataPackage);

		static void onGuildUpdate(OnGuildUpdateData dataPackage);

		static void onGuildDeletion(OnGuildDeletionData dataPackage);

		static void onGuildMemberAdd(OnGuildMemberAddData dataPackage);

		static void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage);

		static void onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage);

		static void onRoleCreation(OnRoleCreationData dataPackage);

		static void onRoleUpdate(OnRoleUpdateData dataPackage);

		static void onRoleDeletion(OnRoleDeletionData dataPackage);
		
		static void onVoiceStateUpdate(OnVoiceStateUpdateData dataPackage);
		
	};
}
