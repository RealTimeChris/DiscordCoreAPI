// EventManager.hpp - Header for the event manager class.
// Apr 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "ChannelEntities.hpp"
#include "GuildEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "UserEntities.hpp"
#include "RoleEntities.hpp"
#include "ReactionEntities.hpp"
#include "MessageEntities.hpp"
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
		ChannelPinsUpdateEventData dataPackage{}; ///< The Channel pins update data.
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
		ThreadListSyncData threadListSyncData{}; ///< The Thread list sync data.
	};

	/// Data that is received as part of a Thread member update event. \brief Data that is received as part of a Thread member update event.
	struct DiscordCoreAPI_Dll OnThreadMemberUpdateData {
		ThreadMemberData threadMember{}; ///< Thread member update data.
	};

	/// Data that is received as part of a Thread members update event. \brief Data that is received as part of a Thread members update event.
	struct DiscordCoreAPI_Dll OnThreadMembersUpdateData {
		ThreadMembersUpdateData threadMembersUpdateData{}; ///< Thread member's update data.
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
		GuildEmojisUpdateEventData updateData{}; ///< The Guild emoji's update data.
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
		User user{}; ///< The User data of the removed GuildMember.
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

	/// Class for handling the assignment of event-handling functions. \brief Class for handling the assignment of event-handling functions.int
	class DiscordCoreAPI_Dll EventManager {
	public:

		event_token onChannelCreationTwo(delegate<OnChannelCreationData>  handler);

		friend class DiscordCoreClient;
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnInputEventCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onInputEventCreation(EventHandler<OnInputEventCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onInputEventCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnApplicationCommandCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onApplicationCommandCreation(EventHandler<OnApplicationCommandCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onApplicationCommandCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnApplicationCommandUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onApplicationCommandUpdate(EventHandler<OnApplicationCommandUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onApplicationCommandUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnApplicationCommandDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onApplicationCommandDeletion(EventHandler<OnApplicationCommandDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onApplicationCommandDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnChannelCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onChannelCreation(EventHandler<OnChannelCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onChannelCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnChannelUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onChannelUpdate(EventHandler<OnChannelUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onChannelUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnChannelDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onChannelDeletion(EventHandler<OnChannelDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onChannelDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnChannelPinsUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onChannelPinsUpdate(EventHandler<OnChannelPinsUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onChannelPinsUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnThreadCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onThreadCreation(EventHandler<OnThreadCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onThreadCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnThreadUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onThreadUpdate(EventHandler<OnThreadUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onThreadUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnThreadDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onThreadDeletion(EventHandler<OnThreadDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onThreadDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnThreadListSyncData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onThreadListSync(EventHandler<OnThreadListSyncData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onThreadListSync(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnThreadMemberUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onThreadMemberUpdate(EventHandler<OnThreadMemberUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onThreadMemberUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnThreadMembersUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onThreadMembersUpdate(EventHandler<OnThreadMembersUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onThreadMembersUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildCreation(EventHandler<OnGuildCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildUpdate(EventHandler<OnGuildUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildDeletion(EventHandler<OnGuildDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildBanAddData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildBanAdd(EventHandler<OnGuildBanAddData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildBanAdd(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildBanRemoveData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildBanRemove(EventHandler<OnGuildBanRemoveData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token EventToken const& from the original event registration.
		/// \returns void
		void onGuildBanRemove(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildEmojisUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildEmojisUpdate(EventHandler<OnGuildEmojisUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildEmojisUpdate(EventToken token);
		
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildStickersUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildStickersUpdate(EventHandler<OnGuildStickersUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildStickersUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildIntegrationsUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildIntegrationsUpdate(EventHandler<OnGuildIntegrationsUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildIntegrationsUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildMembersChunkData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildMembersChunk(EventHandler<OnGuildMembersChunkData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildMembersChunk(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildMemberAddData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildMemberAdd(EventHandler<OnGuildMemberAddData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildMemberAdd(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildMemberRemoveData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildMemberRemove(EventHandler<OnGuildMemberRemoveData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildMemberRemove(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnGuildMemberUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onGuildMemberUpdate(EventHandler<OnGuildMemberUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onGuildMemberUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnRoleCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onRoleCreation(EventHandler<OnRoleCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onRoleCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnRoleUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onRoleUpdate(EventHandler<OnRoleUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onRoleUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnRoleDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onRoleDeletion(EventHandler<OnRoleDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onRoleDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnIntegrationCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onIntegrationCreation(EventHandler<OnIntegrationCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onIntegrationCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnIntegrationUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onIntegrationUpdate(EventHandler<OnIntegrationUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onIntegrationUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnIntegrationDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onIntegrationDeletion(EventHandler<OnIntegrationDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onIntegrationDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnInteractionCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onInteractionCreation(EventHandler<OnInteractionCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onInteractionCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnInviteCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onInviteCreation(EventHandler<OnInviteCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onInviteCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnInviteDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onInviteDeletion(EventHandler<OnInviteDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onInviteDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnMessageCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onMessageCreation(EventHandler<OnMessageCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onMessageCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnMessageUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onMessageUpdate(EventHandler<OnMessageUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onMessageUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnMessageDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onMessageDeletion(EventHandler<OnMessageDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onMessageDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnMessageDeleteBulkData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onMessageDeleteBulk(EventHandler<OnMessageDeleteBulkData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onMessageDeleteBulk(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnReactionAddData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onReactionAdd(EventHandler<OnReactionAddData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onReactionAdd(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnReactionRemoveData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onReactionRemove(EventHandler<OnReactionRemoveData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onReactionRemove(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnReactionRemoveAllData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onReactionRemoveAll(EventHandler<OnReactionRemoveAllData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onReactionRemoveAll(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnReactionRemoveEmojiData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onReactionRemoveEmoji(EventHandler<OnReactionRemoveEmojiData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onReactionRemoveEmoji(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnPresenceUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onPresenceUpdate(EventHandler<OnPresenceUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onPresenceUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnStageInstanceCreationData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onStageInstanceCreation(EventHandler<OnStageInstanceCreationData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onStageInstanceCreation(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnStageInstanceUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onStageInstanceUpdate(EventHandler<OnStageInstanceUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onStageInstanceUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnStageInstanceDeletionData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onStageInstanceDeletion(EventHandler<OnStageInstanceDeletionData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onStageInstanceDeletion(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnTypingStartData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onTypingStart(EventHandler<OnTypingStartData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onTypingStart(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnUserUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onUserUpdate(EventHandler<OnUserUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onUserUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnVoiceStateUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onVoiceStateUpdate(EventHandler<OnVoiceStateUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onVoiceStateUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnVoiceServerUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onVoiceServerUpdate(EventHandler<OnVoiceServerUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onVoiceServerUpdate(EventToken token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler An EventHandlerNew taking an OnWebhookUpdateData structure as an argument.
		/// \returns An EventToken for later de-registering the event.
		EventToken onWebhookUpdate(EventHandler<OnWebhookUpdateData>::EventHandlerNew handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An EventToken const& from the original event registration.
		/// \returns void
		void onWebhookUpdate(EventToken token);

	protected:

		winrt::event<delegate<OnChannelCreationData>> onChannelCreationEventTwo {};

		Event<EventHandler<OnInputEventCreationData>> onInputEventCreationEvent{};

		Event<EventHandler<OnApplicationCommandCreationData>> onApplicationCommandCreationEvent{};

		Event<EventHandler<OnApplicationCommandUpdateData>> onApplicationCommandUpdateEvent{};

		Event<EventHandler<OnApplicationCommandDeletionData>> onApplicationCommandDeletionEvent{};

		Event<EventHandler<OnChannelCreationData>> onChannelCreationEvent{};

		Event<EventHandler<OnChannelUpdateData>> onChannelUpdateEvent{};

		Event<EventHandler<OnChannelDeletionData>> onChannelDeletionEvent{};

		Event<EventHandler<OnChannelPinsUpdateData>> onChannelPinsUpdateEvent{};

		Event<EventHandler<OnThreadCreationData>> onThreadCreationEvent{};

		Event<EventHandler<OnThreadUpdateData>> onThreadUpdateEvent{};

		Event<EventHandler<OnThreadDeletionData>> onThreadDeletionEvent{};

		Event<EventHandler<OnThreadListSyncData>> onThreadListSyncEvent{};

		Event<EventHandler<OnThreadMemberUpdateData>> onThreadMemberUpdateEvent{};

		Event<EventHandler<OnThreadMembersUpdateData>> onThreadMembersUpdateEvent{};

		Event<EventHandler<OnGuildCreationData>> onGuildCreationEvent{};

		Event<EventHandler<OnGuildUpdateData>> onGuildUpdateEvent{};

		Event<EventHandler<OnGuildDeletionData>> onGuildDeletionEvent{};

		Event<EventHandler<OnGuildBanAddData>> onGuildBanAddEvent{};

		Event<EventHandler<OnGuildBanRemoveData>> onGuildBanRemoveEvent{};

		Event<EventHandler<OnGuildEmojisUpdateData>> onGuildEmojisUpdateEvent{};

		Event<EventHandler<OnGuildStickersUpdateData>> onGuildStickersUpdateEvent{};

		Event<EventHandler<OnGuildIntegrationsUpdateData>> onGuildIntegrationsUpdateEvent{};

		Event<EventHandler<OnGuildMemberAddData>> onGuildMemberAddEvent{};

		Event<EventHandler<OnGuildMemberRemoveData>> onGuildMemberRemoveEvent{};

		Event<EventHandler<OnGuildMemberUpdateData>> onGuildMemberUpdateEvent{};

		Event<EventHandler<OnGuildMembersChunkData>> onGuildMembersChunkEvent{};

		Event<EventHandler<OnRoleCreationData>> onRoleCreationEvent{};

		Event<EventHandler<OnRoleUpdateData>> onRoleUpdateEvent{};

		Event<EventHandler<OnRoleDeletionData>> onRoleDeletionEvent{};

		Event<EventHandler<OnIntegrationCreationData>> onIntegrationCreationEvent{};

		Event<EventHandler<OnIntegrationUpdateData>> onIntegrationUpdateEvent{};

		Event<EventHandler<OnIntegrationDeletionData>> onIntegrationDeletionEvent{};

		Event<EventHandler<OnInteractionCreationData>> onInteractionCreationEvent{};

		Event<EventHandler<OnInviteCreationData>> onInviteCreationEvent{};

		Event<EventHandler<OnInviteDeletionData>> onInviteDeletionEvent{};

		Event<EventHandler<OnMessageCreationData>> onMessageCreationEvent{};

		Event<EventHandler<OnMessageUpdateData>> onMessageUpdateEvent{};

		Event<EventHandler<OnMessageDeletionData>> onMessageDeletionEvent{};

		Event<EventHandler<OnMessageDeleteBulkData>> onMessageDeleteBulkEvent{};

		Event<EventHandler<OnReactionAddData>> onReactionAddEvent{};

		Event<EventHandler<OnReactionRemoveData>> onReactionRemoveEvent{};

		Event<EventHandler<OnReactionRemoveAllData>> onReactionRemoveAllEvent{};

		Event<EventHandler<OnReactionRemoveEmojiData>> onReactionRemoveEmojiEvent{};

		Event<EventHandler<OnPresenceUpdateData>> onPresenceUpdateEvent{};

		Event<EventHandler<OnStageInstanceCreationData>> onStageInstanceCreationEvent{};

		Event<EventHandler<OnStageInstanceUpdateData>> onStageInstanceUpdateEvent{};

		Event<EventHandler<OnStageInstanceDeletionData>> onStageInstanceDeletionEvent{};

		Event<EventHandler<OnTypingStartData>> onTypingStartEvent{};

		Event<EventHandler<OnUserUpdateData>> onUserUpdateEvent{};

		Event<EventHandler<OnVoiceStateUpdateData>> onVoiceStateUpdateEvent{};

		Event<EventHandler<OnVoiceServerUpdateData>> onVoiceServerUpdateEvent{};

		Event<EventHandler<OnWebhookUpdateData>> onWebhookUpdateEvent{};

	};
	/**@}*/
	class DiscordCoreAPI_Dll EventHandlerTwo {
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
