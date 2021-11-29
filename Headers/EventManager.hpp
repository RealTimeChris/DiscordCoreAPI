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

	/// Data that is received as part of a thread creation event. \brief Data that is received as part of a thread creation event.
	struct DiscordCoreAPI_Dll OnThreadCreationData {
		Channel channel{}; ///< The new thread's Channel.
	};

	/// Data that is received as part of a thread update event. \brief Data that is received as part of a thread update event.
	struct DiscordCoreAPI_Dll OnThreadUpdateData {
		Channel channel{}; ///< The new thread's Channel.
	};

	/// Data that is received as part of a thread deletion event. \brief Data that is received as part of a thread deletion event.
	struct DiscordCoreAPI_Dll OnThreadDeletionData {
		Channel channel{}; ///< The deleted thread's Channel.
	};

	/// Data that is received as part of a thread list sync event. \brief Data that is received as part of a thread list sync event.
	struct DiscordCoreAPI_Dll OnThreadListSyncData {
		ThreadListSyncData threadListSyncData{}; ///< The thread list sync data.
	};

	/// Data that is received as part of a thread member update event. \brief Data that is received as part of a thread member update event.
	struct DiscordCoreAPI_Dll OnThreadMemberUpdateData {
		ThreadMemberData threadMember{}; ///< Thread member update data.
	};

	/// Data that is received as part of a thread members update event. \brief Data that is received as part of a thread members update event.
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
		User user{};	///< The user id of the person who was banned.
	};

	/// Data that is received as part of a Guild ban remove event. \brief Data that is received as part of a Guild ban add event.
	struct DiscordCoreAPI_Dll OnGuildBanRemoveData {
		string guildId{ "" }; ///< The Guild they were un-banned from.
		User user{};///< The user id of the person who was un-banned.
	};

	/// Data that is received as part of a Guild emojis update event. \brief Data that is received as part of a Guild emojis update event.
	struct DiscordCoreAPI_Dll OnGuildEmojisUpdateData {
		GuildEmojisUpdateEventData updateData{}; ///< The Guild emoji's update data.
	};

	/// Data that is received as part of a Guild sticker update event. \brief Data that is received as part of a Guild sticker update event.
	struct DiscordCoreAPI_Dll OnGuildStickersUpdateData {
		GuildStickersUpdateEventData updateData{}; ///< The GuildStickersUpdateEventData.
	};

	/// Data that is received as part of a Guild integration update event. \brief Data that is received as part of a Guild integration update event.
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

	/// Data that is received as part of an integration creation event. \brief Data that is received as part of an integration creation event.
	struct DiscordCoreAPI_Dll OnIntegrationCreationData {
		IntegrationData integrationData{}; ///< The new IntegrationData structure.
		string guildId{ "" };///< The id of the Guild for which this Integration was created.
	};

	/// Data that is received as part of an integration update event. \brief Data that is received as part of an integration update event.
	struct DiscordCoreAPI_Dll OnIntegrationUpdateData {
		IntegrationData integrationData{}; ///< New IntegrationData structure.
		string guildId{ "" };///< The id of the Guild for which the Integration was updated.
	};

	/// Data that is received as part of an integration deletion event. \brief Data that is received as part of an integration deletion event.
	struct DiscordCoreAPI_Dll OnIntegrationDeletionData {
		string applicationId{ "" }; ///< Application id of the current application.
		string guildId{ "" };///< The id of the Guild for which the Integration was deleted.
		string id{ "" };///< The id of the deleted Integration.
	};

	/// Data that is received as part of an invite creation event. \brief Data that is received as part of an invite creation event.
	struct DiscordCoreAPI_Dll OnInviteCreationData {
		InviteData invite{}; ///< Thew new InviteData structure.
	};

	/// Data that is received as part of an invite deletion event. \brief Data that is received as part of an invite deletion event.
	struct DiscordCoreAPI_Dll OnInviteDeletionData {
		string channelId{ "" }; ///< The id of the Channel for which the invite existed.
		string guildId{ "" };	///< The id of the Guild for which the invite existed.
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

	/// Data that is received as part of a stage instance creation event. \brief Data that is received as part of a stage instance creation event.
	struct DiscordCoreAPI_Dll OnStageInstanceCreationData {
		StageInstanceData stageInstance{}; ///< The new StageInstanceData.
	};

	/// Data that is received as part of a stage instance update event. \brief Data that is received as part of a stage instance update event.
	struct DiscordCoreAPI_Dll OnStageInstanceUpdateData {
		StageInstanceData stageInstance{}; ///< The new StageInstanceData.
	};

	/// Data that is received as part of a stage instance deletion event. \brief Data that is received as part of a stage instance deletion event.
	struct DiscordCoreAPI_Dll OnStageInstanceDeletionData {
		StageInstanceData stageInstance{}; ///< The deleted StageInstanceData.
	};

	/// Data that is received as part of a typing start event. \brief Data that is received as part of a typing start event.
	struct DiscordCoreAPI_Dll OnTypingStartData {
		TypingStartData typingStartData{}; ///< TypingStartData of the event.
	};

	/// Data that is received as part of a user update event. \brief Data that is received as part of a user update event.
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

	/// Data that is received as part of a webhook update event. \brief Data that is received as part of a webhook update event.
	struct DiscordCoreAPI_Dll OnWebhookUpdateData {
		string channelId{ "" };///< Id of the Channel for which the Webhook Update is occurring.
		string guildId{ "" };///< Id of the Guild for which the Webhook Update is occurring.
	};

	/// Class for handling the assignment of event-handling functions. \brief Class for handling the assignment of event-handling functions.
	class DiscordCoreAPI_Dll EventManager {
	public:

		friend class DiscordCoreClient;
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInputEventCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onInputEventCreation(delegate<OnInputEventCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onInputEventCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onApplicationCommandCreation(delegate<OnApplicationCommandCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onApplicationCommandCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onApplicationCommandUpdate(delegate<OnApplicationCommandUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onApplicationCommandUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnApplicationCommandDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onApplicationCommandDeletion(delegate<OnApplicationCommandDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onApplicationCommandDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onChannelCreation(delegate<OnChannelCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onChannelCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onChannelUpdate(delegate<OnChannelUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onChannelUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onChannelDeletion(delegate<OnChannelDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onChannelDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnChannelPinsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onChannelPinsUpdate(delegate<OnChannelPinsUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onChannelPinsUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onThreadCreation(delegate<OnThreadCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onThreadCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onThreadUpdate(delegate<OnThreadUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onThreadUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onThreadDeletion(delegate<OnThreadDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onThreadDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadListSyncData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onThreadListSync(delegate<OnThreadListSyncData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onThreadListSync(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onThreadMemberUpdate(delegate<OnThreadMemberUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onThreadMemberUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnThreadMembersUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onThreadMembersUpdate(delegate<OnThreadMembersUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onThreadMembersUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildCreation(delegate<OnGuildCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildUpdate(delegate<OnGuildUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildDeletion(delegate<OnGuildDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildBanAdd(delegate<OnGuildBanAddData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildBanAdd(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildBanRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildBanRemove(delegate<OnGuildBanRemoveData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token event_token const& from the original event registration.
		/// \returns void
		void onGuildBanRemove(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildEmojisUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildEmojisUpdate(delegate<OnGuildEmojisUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildEmojisUpdate(event_token const& token);
		
		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildStickersUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildStickersUpdate(delegate<OnGuildStickersUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildStickersUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildIntegrationsUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildIntegrationsUpdate(delegate<OnGuildIntegrationsUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildIntegrationsUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMembersChunkData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildMembersChunk(delegate<OnGuildMembersChunkData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildMembersChunk(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildMemberAdd(delegate<OnGuildMemberAddData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildMemberAdd(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildMemberRemove(delegate<OnGuildMemberRemoveData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildMemberRemove(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnGuildMemberUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onGuildMemberUpdate(delegate<OnGuildMemberUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onGuildMemberUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onRoleCreation(delegate<OnRoleCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onRoleCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onRoleUpdate(delegate<OnRoleUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onRoleUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnRoleDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onRoleDeletion(delegate<OnRoleDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onRoleDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onIntegrationCreation(delegate<OnIntegrationCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onIntegrationCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onIntegrationUpdate(delegate<OnIntegrationUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onIntegrationUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnIntegrationDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onIntegrationDeletion(delegate<OnIntegrationDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onIntegrationDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInteractionCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onInteractionCreation(delegate<OnInteractionCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onInteractionCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onInviteCreation(delegate<OnInviteCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onInviteCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnInviteDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onInviteDeletion(delegate<OnInviteDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onInviteDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onMessageCreation(delegate<OnMessageCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onMessageCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onMessageUpdate(delegate<OnMessageUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onMessageUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onMessageDeletion(delegate<OnMessageDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onMessageDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnMessageDeleteBulkData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onMessageDeleteBulk(delegate<OnMessageDeleteBulkData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onMessageDeleteBulk(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionAddData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onReactionAdd(delegate<OnReactionAddData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onReactionAdd(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onReactionRemove(delegate<OnReactionRemoveData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onReactionRemove(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveAllData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onReactionRemoveAll(delegate<OnReactionRemoveAllData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onReactionRemoveAll(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnReactionRemoveEmojiData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onReactionRemoveEmoji(delegate<OnReactionRemoveEmojiData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onReactionRemoveEmoji(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnPresenceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onPresenceUpdate(delegate<OnPresenceUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onPresenceUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceCreationData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onStageInstanceCreation(delegate<OnStageInstanceCreationData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onStageInstanceCreation(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onStageInstanceUpdate(delegate<OnStageInstanceUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onStageInstanceUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnStageInstanceDeletionData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onStageInstanceDeletion(delegate<OnStageInstanceDeletionData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onStageInstanceDeletion(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnTypingStartData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onTypingStart(delegate<OnTypingStartData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onTypingStart(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnUserUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onUserUpdate(delegate<OnUserUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onUserUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceStateUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onVoiceStateUpdate(delegate<OnVoiceStateUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onVoiceStateUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnVoiceServerUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onVoiceServerUpdate(delegate<OnVoiceServerUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onVoiceServerUpdate(event_token const& token);

		/// For adding a function to handle this event. \brief For adding a function to handle this event.
		/// \param handler A delegate taking an OnWebhookUpdateData structure as an argument.
		/// \returns An event_token for later de-registering the event.
		event_token onWebhookUpdate(delegate<OnWebhookUpdateData> const& handler);
		/// For removing a function from handling this event. \brief For removing a function from handling this event.
		/// \param token An event_token const& from the original event registration.
		/// \returns void
		void onWebhookUpdate(event_token const& token);

	protected:

		winrt::event<delegate<OnInputEventCreationData>> onInputEventCreationEvent;

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
