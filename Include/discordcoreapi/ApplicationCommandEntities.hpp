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
/// ApplicationCommandEntities.hpp - Header for the ApplicationCommand classes
/// and structs. Aug 25, 2021 Chris M.
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.hpp

#pragma once

#ifndef APPLICATION_COMMAND_ENTITIES
	#define APPLICATION_COMMAND_ENTITIES

	#include <discordcoreapi/FoundationEntities.hpp>
	#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For getting all of the Global Application Commands. \brief For getting all of the Global Application Commands.
	struct DiscordCoreAPI_Dll GetGlobalApplicationCommandsData {
		Bool withLocalizations{ false };///< Do we collect the name-and-description localizations?
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
	};

	/// For collecting a single global ApplicationCommand. \brief For collecting a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll GetGlobalApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		String commandId{};///< The id of the command which you would like to collect.
	};

	/// For creating an application command.
	struct DiscordCoreAPI_Dll CreateApplicationCommandData : public ApplicationCommandData {
		friend class DiscordCoreClient;

	  protected:
		Bool alwaysRegister{ false };
	};

	/// For creating a single global ApplicationCommand. \brief For creating a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll CreateGlobalApplicationCommandData : public CreateApplicationCommandData {
		operator JsonObject();
	};

	/// For editing a single global ApplicationCommand. \brief For editing a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGlobalApplicationCommandData {
		UMap<String, String> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the description field.
		UMap<String, String> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		Vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		Permissions defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		Bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app.
		String description{};///< A description of the command.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		String name{};///< A name for the new command.

		operator JsonObject();
	};

	/// For deleting a single global ApplicationCommand. \brief For deleting a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll DeleteGlobalApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		String name{};///< The name of the command to delete.
	};

	/// For bulk-overwriting a collection of global ApplicationCommands. \brief For bulk-overwriting a collection of global ApplicationCommands.
	struct DiscordCoreAPI_Dll BulkOverwriteGlobalApplicationCommandsData {
		Vector<CreateGlobalApplicationCommandData> responseData{};///< A vector of the options for the ApplicationCommands.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
	};

	/// For acquiring all of the Guild ApplicationCommands of a single Guild. \brief For acquiring all of the Guild ApplicationCommands of a single Guild.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandsData {
		Bool withLocalizations{ false };///< Do we collect the name-and-description localizations?
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild for which you would like to acquire the ApplicationCommands from.
	};

	/// For creating a single Guild ApplicationCommand. \brief For creating a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll CreateGuildApplicationCommandData : public CreateGlobalApplicationCommandData {
		operator JsonObject();
	};

	/// For acquiring a single Guild ApplicationCommand. \brief For acquiring a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Uint64 commandId{};///< The command id which you would like to acquire.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the ApplicationCommand from.
	};

	/// For editing a single Guild ApplicationCommand. \brief For editing a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandData {
		UMap<String, String> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the description field.
		UMap<String, String> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		Vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		Permissions defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		Bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app.
		String description{};///< A description of the command.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild which you would like to add the new command to.
		String name{};///< A name for the new command.

		operator JsonObject();
	};

	/// For deleting a single Guild ApplicationCommand. \brief For deleting a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll DeleteGuildApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild which you would like to delete the command from.
		String name{};///< A name of the command which you would like to delete.
	};

	/// For bulk-overwriting a collection of Guild ApplicationCommands. \brief For bulk-overwriting a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll BulkOverwriteGuildApplicationCommandsData {
		Vector<CreateGuildApplicationCommandData> responseData{};///< A vector of the options for the ApplicationCommands.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild which you would like to overwrite the commands of.
	};

	/// For acquiring the permissions of a collection of Guild ApplicationCommands. \brief For acquiring the permissions of a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandPermissionsData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the command permissions.
	};

	/// For acquiring the permissions of a single Guild ApplicationCommand. \brief For acquiring the permissions of a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll GetApplicationCommandPermissionsData {
		String commandName{};///< The name of the command which you would like to collect the permissions of.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the command permissions.
	};

	/// For editing the permissions of a single Guild ApplicationCommand. \brief For editing the permissions of a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandPermissionsData {
		Vector<ApplicationCommandPermissionData> permissions{};///< A vector of ApplicationCommand permissions.
		String commandName{};///< The command name which you would like to edit the permissions of.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Uint64 commandId{};///< The command id which you would like to edit the permissions of.
		Snowflake guildId{};///< The Guild id of the Guild for which you would like to edit the command permissions.

		operator JsonObject();
	};

	/// For batch editing the permissions of a collection of Guild ApplicationCommands. \brief For batch editing the permissions of a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll BatchEditGuildApplicationCommandPermissionsData {
		Vector<GuildApplicationCommandPermissionsData> permissions{};///< A vector of edit-Guild-application-permissions responseData to edit.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The Guild id of the Guild for which you would like to batch edit Guild application permissions.
	};

	/// A single ApplicationCommand. \brief A single ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommand : public ApplicationCommandData {
	  public:
		ApplicationCommand() noexcept = default;

		ApplicationCommand(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommand() noexcept = default;
	};

	class DiscordCoreAPI_Dll ApplicationCommandVector {
	  public:
		friend class ApplicationCommands;

		ApplicationCommandVector() noexcept = default;

		operator Vector<ApplicationCommand>();

		ApplicationCommandVector(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommandVector() noexcept = default;

	  protected:
		Vector<ApplicationCommand> theApplicationCommands{};
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the ApplicationCommand related Discord endpoints. \brief An interface class for the ApplicationCommand related Discord endpoints.
	class DiscordCoreAPI_Dll ApplicationCommands {
	  public:
		static Void initialize(DiscordCoreInternal::HttpsClient*);

		/// Get all of the global ApplicationCommands for this bot. \brief Get all of the global ApplicationCommands for this bot.
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
		static CoRoutine<Vector<ApplicationCommand>> getGlobalApplicationCommandsAsync(GetGlobalApplicationCommandsData dataPackage);

		/// Create a global ApplicationCommand for this bot. \brief Create a global ApplicationCommand for this bot.
		/// \param dataPackage A CreateGlobalApplicationCommandData structure.
		/// \returns A CoRoutine containing an ApplicationCommand.
		static CoRoutine<ApplicationCommand> createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage);

		/// Get a single global ApplicationCommand for this bot. \brief Get a single global ApplicationCommand for this bot.
		/// \param dataPackage A GetGlobalApplicationCommandData structure.
		/// \returns A CoRoutine containing an ApplicationCommand.
		static CoRoutine<ApplicationCommand> getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage);

		/// Edit a global ApplicationCommand for this bot. \brief Edit a global ApplicationCommand for this bot.
		/// \param dataPackage An EditGlobalApplicationCommandData structure.
		/// \returns A CoRoutine containing an ApplicationCommand.
		static CoRoutine<ApplicationCommand> editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage);

		/// Delete a global ApplicationCommand for this bot. \brief Delete a global ApplicationCommand for this bot.
		/// \param dataPackage A DeleteGlobalApplicationCommandData structure.
		/// \returns A CoRoutine containing Void.
		static CoRoutine<Void> deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage);

		/// Bulk overwrites a collection of global ApplicationCommands. \brief Bulk overwrites a collection of global ApplicationCommands.
		/// \param dataPackage A BulkOverwriteGlobalApplicationCommandsData structure.
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
		static CoRoutine<Vector<ApplicationCommand>> bulkOverwriteGlobalApplicationCommandsAsync(BulkOverwriteGlobalApplicationCommandsData dataPackage);

		/// Get all of the Guild ApplicationCommands for a single Guild for this bot. \brief Get all of the Guild ApplicationCommands for a single Guild for this bot.
		/// \param dataPackage A GetGuildApplicationCommandsData structure.
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
		static CoRoutine<Vector<ApplicationCommand>> getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage);

		/// Create a Guild ApplicationCommand for a single server for this bot. \brief Create a Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage A CreateGuildApplicationCommandData structure.
		/// \returns A CoRoutine containing an ApplicationCommand.
		static CoRoutine<ApplicationCommand> createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage);

		/// Get a single Guild ApplicationCommand for a single server for this bot. \brief Get a single Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage A GetGuildApplicationCommandData structure.
		/// \returns A CoRoutine containing an ApplicationCommand.
		static CoRoutine<ApplicationCommand> getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage);

		/// Edit a single Guild ApplicationCommand for a single server for this bot. \brief Edit a single Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage An EditGuildApplicationCommandData structure.
		/// \returns A CoRoutine containing an ApplicationCommand.
		static CoRoutine<ApplicationCommand> editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage);

		/// Delete a single Guild ApplicationCommand for a single server for this bot. \brief Delete a single Guild ApplicationCommand for a single server for this bot.
		/// \param dataPackage A DeleteGuildApplicationCommandData structure.
		/// \returns A CoRoutine containing Void.
		static CoRoutine<Void> deleteGuildApplicationCommandAsync(DeleteGuildApplicationCommandData dataPackage);

		/// Bulk overwrites some Guild ApplicationCommands for this bot. \brief Bulkoverwrites some Guild ApplicationCommands for this bot.
		/// \param dataPackage A BulkOverwriteGuildApplicationCommandsData structure.
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
		static CoRoutine<Vector<ApplicationCommand>> bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage);

		/// Gets Guild ApplicationCommand permissions for a single server on this bot. \brief Gets Guild ApplicationCommand permissions for a single server on this bot.
		/// \param dataPackage A GetGuildApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a vector<GuildApplicationCommandPermissionsData>.
		static CoRoutine<Vector<GuildApplicationCommandPermissionsData>> getGuildApplicationCommandPermissionsAsync(GetGuildApplicationCommandPermissionsData dataPackage);

		/// Get ApplicationCommand permissions for a single command on this bot. \brief Get ApplicationCommand permissions for a single command on this bot.
		/// \param dataPackage A GetApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a GuildApplicationCommandPermissionsData.
		static CoRoutine<GuildApplicationCommandPermissionsData> getApplicationCommandPermissionsAsync(GetApplicationCommandPermissionsData dataPackage);

		/// Edit Guild ApplicationCommand permissions for a server on this bot. \brief Edit Guild ApplicationCommand permissions for a server on this bot.
		/// \param dataPackage An EditGuildApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a GuildApplicationCommandPermissionsData.
		static CoRoutine<GuildApplicationCommandPermissionsData> editGuildApplicationCommandPermissionsAsync(EditGuildApplicationCommandPermissionsData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/

}// namespace DiscordCoreAPI
#endif
