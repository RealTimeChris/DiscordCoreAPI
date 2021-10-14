// EndpointWrappers.hpp - Header for the endpoint wrapper classes.
// Oct 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ENDPOINT_WRAPPERS_
#define _ENDPOINT_WRAPPERS_

#include "IndexInitial.hpp"
#include "DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the ApplicationCommand related Discord endpoints. \brief An interface class for the ApplicationCommand related Discord endpoints.
	class DiscordCoreAPI_Dll ApplicationCommands {
	public:
		/// Get all of the global ApplicationCommands for this bot. \brief Get all of the global ApplicationCommands for this bot.
		/// \returns A task containing a vector of ApplicationCommand.
		static task<vector<ApplicationCommand>> getGlobalApplicationCommandsAsync();

		/// Create a global ApplicationCommand for this bot. \brief Create a global ApplicationCommand for this bot.
		/// \param dataPackage A CreateGlobalApplicationCommandData  structure.
		/// \returns A task containing an ApplicationCommand.
		static task<ApplicationCommand> createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage);

		/// Get a single global ApplicationCommand for this bot. \brief Get a single global ApplicationCommand for this bot.
		/// \param dataPackage A GetGlobalApplicationCommandData structure.
		/// \returns A task containing an ApplicationCommand.
		static task<ApplicationCommand> getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage);

		/// Edit a global ApplicationCommand for this bot. \brief Edit a global ApplicationCommand for this bot.
		/// \param dataPackage A EditGlobalApplicationCommandData structure.
		/// \returns A task containing an ApplicationCommand.
		static task<ApplicationCommand> editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage);

		/// Delete a global ApplicationCommand for this bot. \brief Delete a global ApplicationCommand for this bot.
		/// \param dataPackage A DeleteGlobalApplicationCommandData structure.
		/// \returns void
		static void deleteGlobalApplicationCommand(DeleteGlobalApplicationCommandData dataPackage);

		/// Bulk overwrites a collection of global ApplicationCommands. \brief Bulk overwrites a collection of global ApplicationCommands.
		/// \param dataPackage A BulkOverwriteGlobalApplicationCommandsData structure.
		/// \returns A task containing a vector of ApplicationCommand.
		static task<vector<ApplicationCommand>> bulkOverwriteGlobalApplicationCommandsAsync(BulkOverwriteGlobalApplicationCommandsData dataPackage);

		/// Get all of the Guild ApplicationCommands for a single Guild for this bot. \brief Get all of the Guild ApplicationCommands for a single Guild for this bot.
		/// \param dataPackage A GetGuildApplicationCommandsData structure.
		/// \returns A task containing a vector of ApplicationCommand.
		static task<vector<ApplicationCommand>> getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage);

		/// Create a Guild ApplicationCommand for a single server for this bot. \brief Create a Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage A CreateGuildApplicationCommandData structure.
		/// \returns A task containing an ApplicationCommand.
		static task<ApplicationCommand> createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage);

		/// Get a single Guild ApplicationCommand for a single server for this bot. \brief Get a single Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage A GetGuildApplicationCommandData structure.
		/// \returns A task containing an ApplicationCommand.
		static task<ApplicationCommand> getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage);

		/// Edit a single Guild ApplicationCommand for a single server for this bot. \brief Edit a single Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage A EditGuildApplicationCommandData structure.
		/// \returns A task containing an ApplicationCommand.
		static task<ApplicationCommand> editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage);

		/// Delete a single Guild ApplicationCommand for a single server for this bot. \brief Delete a single Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage A DeleteGuildApplicationCommandData structure.
		/// \returns void
		static void deleteGuildApplicationCommand(DeleteGuildApplicationCommandData dataPackage);

		/// Bulk overwrites some Guild ApplicationCommands for this bot. \brief Bulk overwrites some Guild ApplicationCommands for this bot.
		/// \param dataPackage A BulkOverwriteGuildApplicationCommandsData structure.
		/// \returns A task containing a vector of ApplicationCommand.
		static task<vector<ApplicationCommand>>  bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage);

		/// Gets Guild ApplicationCommand permissions for a single server on this bot. \brief Gets Guild ApplicationCommand permissions for a single server on this bot.
		/// \param dataPackage A GetGuildApplicationCommandPermissionsData structure.
		/// \returns A task containing a vector of GuildApplicationCommandPermissionsData.
		static task<vector<GuildApplicationCommandPermissionsData>> getGuildApplicationCommandPermissionsAsync(GetGuildApplicationCommandPermissionsData dataPackage);

		/// Get ApplicationCommand permissions for a single command on this bot. \brief Get ApplicationCommand permissions for a single command on this bot.
		/// \param dataPackage A GetApplicationCommandPermissionsData structure.
		/// \returns A GuildApplicationCommandPermissionsData structure.
		static GuildApplicationCommandPermissionsData getApplicationCommandPermissions(GetApplicationCommandPermissionsData dataPackage);

		/// Edit Guild ApplicationCommand permissions for a server on this bot. \brief Edit Guild ApplicationCommand permissions for a server on this bot.
		/// \param dataPackage A EditGuildApplicationCommandPermissionsData structure.
		/// \returns A GuildApplicationCommandPermissionsData structure.
		static GuildApplicationCommandPermissionsData editGuildApplicationCommandPermissions(EditGuildApplicationCommandPermissionsData dataPackage);

		/// Batch edit Guild ApplicationCommand permissions for a single server on this bot. \brief Batch edit Guild ApplicationCommand permissions for a single server on this bot.
		/// \param dataPackage A BatchEditGuildApplicationCommandPermissionsData structure.
		/// \returns A vector of GuildApplicationCommandPermissionsData.
		static vector<GuildApplicationCommandPermissionsData> batchEditGuildApplicationCommandPermissions(BatchEditGuildApplicationCommandPermissionsData dataPackage);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Channel related endpoints. \brief An interface class for the Channel-related endpoints.
	class DiscordCoreAPI_Dll Channels {
	public:

		friend class EventHandler;

		/// Collects a Channel from the library's cache. \brief Collects a Channel from the library's cache.		
		/// \param dataPackage A GetChannelData structure.
		/// \returns A task containing a Channel.
		static task<Channel> getCachedChannelAsync(GetChannelData dataPackage);

		/// Collects a Channel from the Discord servers. \brief Collects a Channel from the Discord servers.
		/// \param dataPackage A GetChannelData structure.
		/// \returns A task containing a Channel.
		static task<Channel> getChannelAsync(GetChannelData dataPackage);

		/// Edit the given Permissions overwrites for a given User or Role. \brief Edit the given Permissions overwrites for a given User or Role.
		/// \param dataPackage A EditChannelPermissionOverwritesData structure.
		/// \returns A task containing void.
		static task<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage);

		/// Delete the given Permissions overwrites for a given User or Role. \brief Delete the given Permissions overwrites for a given User or Role.
		/// \param dataPackage A DeleteChannelPermissionOverwritesData structure.
		/// \returns A task containing void.
		static task<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage);

		/// Collect a direct-Message Channel between the bot and the User. \brief Collect a direct-Message Channel between the bot and the User.
		/// \param dataPackage A GetDMChannelData structure.
		/// \returns A task containing a Channel.
		static task<Channel> getDMChannelAsync(GetDMChannelData dataPackage);

	protected:
		static task<void> insertChannelAsync(Channel dataPackage);

		static task<void> removeChannelAsync(string channelId);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Guild related Discord endpoints. \brief An interface class for the Guild related Discord endpoints.
	class DiscordCoreAPI_Dll Guilds {
	public:
		friend class EventHandler;

		/// Collects a Guild from the library's cache. \brief Collects a Guild from the library's cache.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A task containing a Guild.
		static task<Guild> getCachedGuildAsync(GetGuildData dataPackage);

		/// Collects a Guild from the Discord servers. \brief Collects a Guild from the Discord servers.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A task containing a Guild.
		static task<Guild> getGuildAsync(GetGuildData dataPackage);

		/// Getes an audit log from the Discord servers. \brief Getes an audit log from the Discord servers.
		/// \param dataPackage A GetAuditLogData structure.
		/// \returns A task containing AuditLogData.
		static task<AuditLogData> getAuditLogDataAsync(GetAuditLogData dataPackage);

		/// Getes an invite from the Discord servers. \brief Getes an invite from the Discord servers.
		/// \param dataPackage A GetInviteData structure.
		/// \returns A task containing InviteData.
		static task<InviteData> getInviteAsync(GetInviteData dataPackage);

		/// Getes multiple invites from the Discord servers. \brief Getes multiple invites from the Discord servers.
		/// \param dataPackage A GetInvitesData structure.
		/// \returns A task containing a vector of InviteData.
		static task<vector<InviteData>> getInvitesAsync(GetInvitesData dataPackage);

		/// Getes the vanity invite data from a particular server. \brief Getes the vanity invite data from a particular server.
		/// \param dataPackage A GetVanityInviteData structure.
		/// \returns A task containing InviteData.
		static task<InviteData> getVanityInviteAsync(GetVanityInviteData dataPackage);

		/// Bans a GuildMember. \brief Bans a GuildMember.
		/// \param dataPackage A CreateGuildBanData structure.
		/// \returns A task containing BanData.
		static task<BanData> createGuildBanAsync(CreateGuildBanData dataPackage);

		/// Returns all of the Guilds that the current bot is in. \brief Returns all of the Guilds that the current bot is in.
		/// \returns A task containing a vector of Guild.
		static task<vector<Guild>> getAllGuildsAsync();

	protected:
		static task<void> insertGuildAsync(Guild Guild);

		static task<void> removeGuildAsync(Guild Guild);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the GuildMember related Discord endpoints. \brief An interface class for the GuildMember related Discord endpoints.
	class DiscordCoreAPI_Dll GuildMembers {
	public:
		friend class EventHandler;

		/// Collects a GuildMember from the library's cache. \brief Collects a GuildMember from the library's cache.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A task containing a GuildMember.
		static task<GuildMember> getCachedGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Collects a GuildMember from the Discord servers. \brief Collects a GuildMember from the Discord servers.
		/// \param dataPackage A GetGuildMemberData structure.
		/// \returns A task containing a GuildMember.
		static task<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage);

		/// Modify's a GuildMember's properties. \brief Modify's a GuildMember's properties.
		/// \param dataPackage A ModifyGuildMemberData structure.
		/// \returns A task containing a GuildMember.
		static task<GuildMember> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage);

	protected:
		static task<void> insertGuildMemberAsync(GuildMember dataPackage);

		static task<void> removeGuildMemberAsync(string guildId, string guildMemberId);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Interaction related Discord endpoints. \brief An interface class for the Interaction related Discord endpoints.
	class DiscordCoreAPI_Dll Interactions {
	public:

		/// Creates a deferred response to an input Interaction. \brief Creates a deferred response to an input Interaction.
		/// \param dataPackage A CreateDeferredInteractionResponseData structure.
		/// \returns A task containing void.
		static task<void> createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData dataPackage);

		/// Creates a response to an input Interaction. \brief Creates a response to an input Interaction.
		/// \param dataPackage A CreateInteractionResponseData structure.
		/// \returns A task containing a MessageData.
		static task<MessageData> createInteractionResponseAsync(CreateInteractionResponseData dataPackage);

		/// Collects an Interaction response. \brief Collects an Interaction response.
		/// \param dataPackage A GetInteractionResponseData structure.
		/// \returns A task containing an InteractionResponseData.
		static task<InteractionResponseData> getInteractionResponseAsync(GetInteractionResponseData dataPackage);

		/// Edits an Interaction response. \brief Edits an Interaction response.
		/// \param dataPackage A EditInteractionResponseData structure.
		/// \returns A task containing a MessageData.
		static task<MessageData> editInteractionResponseAsync(EditInteractionResponseData dataPackage);

		/// Deletes an Interaction respnose. \brief Deletes an Interaction respnose.
		/// \param dataPackage A DeleteInteractionResponseData structure.
		/// \returns A task containing void.
		static task<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage);

		/// Creates a follow up Message to an input Interaction. \brief Creates a follow up Message to an input Interaction.
		/// \param dataPackage A CreateFollowUpMessageData structure.
		/// \returns A task containing a MessageData.
		static task<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage);
		
		/// Edits a follow up Message. \brief Edits a follow up Message.
		/// \param dataPackage A EditFollowUpMessageData structure.
		/// \returns A task containing a MessageData.
		static task<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage);

		/// Deletes a follow up Message. \brief Deletes a follow up Message.
		/// \param dataPackage A DeleteFollowUpMessageData structure.
		/// \returns A task containing void.
		static task<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Message related Discord endpoints. \brief An interface class for the Message related Discord endpoints;
	class DiscordCoreAPI_Dll Messages {
	public:

		/// Collects a Message from the Discord servers. \brief Collects a Message from the Discord servers.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A task containing a Message.
		static task<Message> getMessageAsync(GetMessageData dataPackage);

		/// Creates a new Message. \brief Creates a new Message.
		/// \param dataPackage A CreateMessageData structure.
		/// \returns A task containing a Message.
		static task<Message> createMessageAsync(CreateMessageData dataPackage);

		/// Deletes a Message. \brief Deletes a Message.
		/// \param dataPackage A DeleteMessageData structure.
		/// \returns A task containing void.
		static task<void> deleteMessageAsync(DeleteMessageData dataPackage);

		/// Deletes a collection of Messages. \brief Deletes a collection of Messages.
		/// \param dataPackage A DeleteMessagesBulkData structure.
		/// \returns A task containing void.
		static task<void> deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage);

		/// Edit a Message. \brief Edit a Message.
		/// \param dataPackage A EditMessageData structure.
		/// \returns A task containing a Message.
		static task<Message> editMessageAsync(EditMessageData dataPackage);

		/// Collects a collection of Message from the Discord servers. \brief Collects a collection of Message from the Discord servers.
		/// \param dataPackage A GetMessagesData structure.
		/// \returns A task containing an optional vector of Message.
		static task<std::optional<vector<Message>>> getMessagesAsync(GetMessagesData dataPackage);

		/// Collects a collection of pinned Messages from the Discord servers. \brief Collects a collection of pinned Messages from the Discord servers.
		/// \param dataPackage A GetPinnedMessagesData structure.
		/// \returns A task containing a vector of Message.
		static task<vector<Message>> getPinnedMessagesAsync(GetPinnedMessagesData dataPackage);

		/// Pins a Message to a given Channel. \brief Pins a Message to a given Channel.
		/// \param dataPackage A PinMessageData structure.
		/// \returns A task containing void.
		static task<void> pinMessageAsync(PinMessageData dataPackage);

		/// Reply to a given Message. \brief Reply to a given Message.
		/// \param dataPackage A ReplyMessageData structure.
		/// \returns A task containing a Message.
		static task<Message> replyAsync(ReplyMessageData dataPackage);

		/// Send a direct Message to a given User. \brief Send a direct Message to a given User.
		/// \param dataPackage A SendDMData structure.
		/// \returns A task containing a Message.
		static task<Message> sendDMAsync(SendDMData dataPackage);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Reaction related Discord endpoints. \brief An interface class for the Reaction related Discord endpoints.
	class DiscordCoreAPI_Dll Reactions {
	public:

		/// Creates a Reaction on a Message. \brief Creates a Reaction on a Message.
		/// \param dataPackage A CreateReactionData structure.
		/// \returns A task containing a Reaction.
		static task<Reaction> createReactionAsync(CreateReactionData dataPackage);

		/// Deletes all of the Reactions from a given Message. \brief Deletes all of the Reactions from a given Message.
		/// \param dataPackage A DeleteAllReactionsData structure.
		/// \returns A task containing void.
		static task<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage);

		/// Deletes your own Reactions from a given Message. \brief Deletes your own Reactions from a given Message.
		/// \param dataPackage A DeleteOwnReactionData structure.
		/// \returns A task containing void.
		static task<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage);

		/// Deletes all of the Reactions by a specific emoji from a given Message. \brief Deletes all of the Reactions by a specific emoji from a given Message. 
		/// \param dataPackage A DeleteReactionsByEmojiData structure.
		/// \returns A task containing void.
		static task<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage);

		/// Deletes all of the Reactions by a specific User from a given Message. \brief Deletes all of the Reactions by a specific User from a given Message. 
		/// \param dataPackage A DeleteUserReactionData structure.
		/// \returns A task containing void.
		static task<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Role related Discord endpoints. \brief An interface class for the Role related Discord endpoints.
	class DiscordCoreAPI_Dll Roles {
	public:
		friend class EventHandler;

		/// Collects a given Role from the library's cache. \brief Collects a given Role from the library's cache.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A task containing a Role.
		static task<Role> getCachedRoleAsync(GetRoleData dataPackage);

		/// Adds a role to a chosen Guild member. \brief Adds a Role to a chosen Guild member.
		/// \param dataPackage A AddRoleToGuildMemberData structure.
		/// \returns A task containing void.
		static task<void> addRoleToGuildMemberAsync(AddRoleToGuildMemberData dataPackage);

		/// Creates a new Role within the given Guild. \brief Creates a new Role within the given Guild.
		/// \param dataPackage A CreateRoleData structure.
		/// \returns A Role.
		static Role createRole(CreateRoleData dataPackage);

		/// Collects a Role from the Discord servers. \brief Collects a Role from the Discord servers.
		/// \param dataPackage A GetRoleData structure.
		/// \returns A Role.
		static Role getRole(GetRoleData dataPackage);

		/// Collects the Roles that a GuildMember has. \brief Collects the Roles that a GuildMember has.
		/// \param dataPackage A GetGuildMemberRolesData structure.
		/// \returns A vector of Roles.
		static vector<Role> getGuildMemberRoles(GetGuildMemberRolesData dataPackage);

		/// Collects the Roles that a Guild has. \brief Collects the Roles that a Guild has.
		/// \param dataPackage A GetGuildRolesData structure.
		/// \returns A task containing a vector of Roles.
		static task<vector<Role>> getGuildRolesAsync(GetGuildRolesData dataPackage);

		/// Removes a given Role from a Guild. \brief Removes a given Role from a Guild.
		/// \param dataPackage A RemoveRoleFromGuildData structure.
		/// \returns A task containing void.
		static task<void> removeRoleFromGuildAsync(RemoveRoleFromGuildData dataPackage);

		/// Removes a given Role from a chosen GuildMember. \brief Removes a given Role from a chosen GuildMember.
		/// \param dataPackage A RemoveRoleFromGuildMemberData structure.
		/// \returns A task containing void.
		static task<void> removeRoleFromGuildMemberAsync(RemoveRoleFromGuildMemberData dataPackage);

		/// Updates a given Role's properties. \brief Updates a given Role's properties.
		/// \param dataPackage A UpdateRoleData structure.
		/// \returns A task containing a Role.
		static task<Role> updateRoleAsync(UpdateRoleData dataPackage);

		/// Updates a Role's positions. \brief Updates a Role's positions.
		/// \param dataPackage A UpdateRolePositionData structure.
		/// \returns A vector containing Roles.
		static vector<Role> updateRolePositions(UpdateRolePositionData dataPackage);

	protected:
		static task<void> insertRoleAsync(Role dataPackage);

		static task<void> removeRoleAsync(string roleId);
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the User related Discord endpoints. \brief An interface class for the User related Discord endpoints.
	class DiscordCoreAPI_Dll Users {
	public:
		friend class EventHandler;

		/// Collects a given User from the library's cache. \brief Collects a given User from the library's cache.
		/// \param dataPackage A GetUserData structure.
		/// \returns A task containing a User.
		 static task<User> getCachedUserAsync(GetUserData dataPackage);

		 /// Collects a given User from the Discord servers. \brief Collects a given User from the Discord servers.
		/// \param dataPackage A GetUserData structure.
		/// \returns A task containing a User.
		 static task<User> getUserAsync(GetUserData dataPackage);

	protected:
		 static task<void> insertUserAsync(User useruser);
	};
	/**@}*/

};

#endif