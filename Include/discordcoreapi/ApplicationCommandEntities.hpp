/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// ApplicationCommandEntities.hpp - Header for the ApplicationCommand classes
/// and structs. Aug 25, 2021 Chris M.
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For getting all of the Global Application Commands. \brief For getting all of the Global Application Commands.
	struct DiscordCoreAPI_Dll GetGlobalApplicationCommandsData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
	};

	/// For collecting a single global ApplicationCommand. \brief For collecting a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll GetGlobalApplicationCommandData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string commandId{ "" };///< The id of the command which you would like to collect.
	};

	/// For creating a single global ApplicationCommand. \brief For creating a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll CreateGlobalApplicationCommandData {
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		std::string applicationId{ "" };///< Application id.
		bool defaultPermission{ true };///< Whether the command is enabled by default when the bot is added to a Guild.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		std::string description{ "" };///< A description of the command.
		std::string name{ "" };///< A name for the new command.
	};

	/// For editing a single global ApplicationCommand. \brief For editing a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGlobalApplicationCommandData {
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		bool defaultPermission{ false };///< Whether the command is enabled by default when the bot is added to a Guild.
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string description{ "" };///< A description of the command.
		std::string name{ "" };///< A name for the new command.
	};

	/// For deleting a single global ApplicationCommand. \brief For deleting a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll DeleteGlobalApplicationCommandData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string name{ "" };///< The name of the command to delete.
	};

	/// For bulk-overwriting a collection of global ApplicationCommands. \brief For bulk-overwriting a collection of global ApplicationCommands.
	struct DiscordCoreAPI_Dll BulkOverwriteGlobalApplicationCommandsData {
		std::vector<CreateGlobalApplicationCommandData> responseData{};///< A std::vector of the options for the ApplicationCommands.
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
	};

	/// For acquiring all of the Guild ApplicationCommands of a single Guild. \brief For acquiring all of the Guild ApplicationCommands of a single Guild.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandsData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string guildId{ "" };///< The id of the Guild for which you would like to acquire the ApplicationCommands from.
	};

	/// For creating a single Guild ApplicationCommand. \brief For creating a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll CreateGuildApplicationCommandData {
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		std::string applicationId{ "" };///< Application Id.
		bool defaultPermission{ true };///< Whether the command is enabled by default when the bot is added to a Guild.
		std::string description{ "" };///< A description of the command.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		std::string guildId{ "" };///< The id of the Guild which you would like to add the new command to.
		std::string name{ "" };///< A name for the new command.
	};

	/// For acquiring a single Guild ApplicationCommand. \brief For acquiring a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string commandId{ "" };///< The command id which you would like to acquire.
		std::string guildId{ "" };///< The id of the Guild from which you would like to acquire the ApplicationCommand from.
	};

	/// For editing a single Guild ApplicationCommand. \brief For editing a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandData {
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		bool defaultPermission{ true };///< Whether the command is enabled by default when the bot is added to a Guild.
		std::string description{ "" };///< A description of the command.
		std::string guildId{ "" };///< The id of the Guild which you would like to add the new command to.
		std::string name{ "" };///< A name for the new command.
	};

	/// For deleting a single Guild ApplicationCommand. \brief For deleting a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll DeleteGuildApplicationCommandData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string guildId{ "" };///< The id of the Guild which you would like to delete the command from.
		std::string name{ "" };///< A name of the command which you would like to delete.
	};

	/// For bulk-overwriting a collection of Guild ApplicationCommands. \brief For bulk-overwriting a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll BulkOverwriteGuildApplicationCommandsData {
		std::vector<CreateGuildApplicationCommandData> responseData{};///< A std::vector of the options for the ApplicationCommands.
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string guildId{ "" };///< The id of the Guild which you would like to overwrite the commands of.
	};

	/// For acquiring the permissions of a collection of Guild ApplicationCommands. \brief For acquiring the permissions of a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandPermissionsData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string guildId{ "" };///< The id of the Guild from which you would like to acquire the command permissions.
	};

	/// For acquiring the permissions of a single Guild ApplicationCommand. \brief For acquiring the permissions of a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll GetApplicationCommandPermissionsData {
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string commandName{ "" };///< The name of the command which you would like to collect the permissions of.
		std::string guildId{ "" };///< The id of the Guild from which you would like to acquire the command permissions.
	};

	/// For editing the permissions of a single Guild ApplicationCommand. \brief For editing the permissions of a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandPermissionsData {
		std::vector<ApplicationCommandPermissionData> permissions{};///< A std::vector of ApplicationCommand permissions.
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string commandName{ "" };///< The command name which you would like to edit the permissions of.
		std::string commandId{ "" };///< The command id which you would like to edit the permissions of.
		std::string guildId{ "" };///< The Guild id of the Guild for which you would like to edit the command permissions.
	};

	/// For batch editing the permissions of a collection of Guild ApplicationCommands. \brief For batch editing the permissions of a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll BatchEditGuildApplicationCommandPermissionsData {
		std::vector<GuildApplicationCommandPermissionsData> permissions{};///< A std::vector of edit-Guild-application-permissions responseData to edit.
		std::string applicationId{ "" };///< The current application's Id (The Bot's User Id).
		std::string guildId{ "" };///< The Guild id of the Guild for which you would like to batch edit Guild application permissions.
	};

	/// A single ApplicationCommand. \brief A single ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommand : public ApplicationCommandData {
	  public:
		ApplicationCommand() = default;

		ApplicationCommand(ApplicationCommandData dataNew);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the ApplicationCommand related Discord endpoints. \brief An interface class for the ApplicationCommand related Discord endpoints.
	class DiscordCoreAPI_Dll ApplicationCommands {
	  public:
		static void initialize(DiscordCoreInternal::HttpClient*);

		/// Get all of the global ApplicationCommands for this bot. \brief Get all of the global ApplicationCommands for this bot.
		/// \returns A CoRoutine containing a std::vector<ApplicationCommand>.
		static CoRoutine<std::vector<ApplicationCommand>> getGlobalApplicationCommandsAsync(GetGlobalApplicationCommandsData dataPackage);

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
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage);

		/// Bulk overwrites a collection of global ApplicationCommands. \brief Bulk overwrites a collection of global ApplicationCommands.
		/// \param dataPackage A BulkOverwriteGlobalApplicationCommandsData structure.
		/// \returns A CoRoutine containing a std::vector<ApplicationCommand>.
		static CoRoutine<std::vector<ApplicationCommand>> bulkOverwriteGlobalApplicationCommandsAsync(BulkOverwriteGlobalApplicationCommandsData dataPackage);

		/// Get all of the Guild ApplicationCommands for a single Guild for this bot. \brief Get all of the Guild ApplicationCommands for a single Guild for this bot.
		/// \param dataPackage A GetGuildApplicationCommandsData structure.
		/// \returns A CoRoutine containing a std::vector<ApplicationCommand>.
		static CoRoutine<std::vector<ApplicationCommand>> getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage);

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
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildApplicationCommandAsync(DeleteGuildApplicationCommandData dataPackage);

		/// Bulk overwrites some Guild ApplicationCommands for this bot. \brief Bulkoverwrites some Guild ApplicationCommands for this bot.
		/// \param dataPackage A BulkOverwriteGuildApplicationCommandsData structure.
		/// \returns A CoRoutine containing a std::vector<ApplicationCommand>.
		static CoRoutine<std::vector<ApplicationCommand>> bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage);

		/// Gets Guild ApplicationCommand permissions for a single server on this bot. \brief Gets Guild ApplicationCommand permissions for a single server on this bot.
		/// \param dataPackage A GetGuildApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a std::vector<GuildApplicationCommandPermissionsData>.
		static CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> getGuildApplicationCommandPermissionsAsync(
			GetGuildApplicationCommandPermissionsData dataPackage);

		/// Get ApplicationCommand permissions for a single command on this bot. \brief Get ApplicationCommand permissions for a single command on this bot.
		/// \param dataPackage A GetApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a GuildApplicationCommandPermissionsData.
		static CoRoutine<GuildApplicationCommandPermissionsData> getApplicationCommandPermissionsAsync(GetApplicationCommandPermissionsData dataPackage);

		/// Edit Guild ApplicationCommand permissions for a server on this bot. \brief Edit Guild ApplicationCommand permissions for a server on this bot.
		/// \param dataPackage An EditGuildApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a GuildApplicationCommandPermissionsData.
		static CoRoutine<GuildApplicationCommandPermissionsData> editGuildApplicationCommandPermissionsAsync(
			EditGuildApplicationCommandPermissionsData dataPackage);

		/// Batch edit Guild ApplicationCommand permissions for a single server on this bot. \brief Batch edit Guild ApplicationCommand permissions for a single server on this bot.
		/// \param dataPackage A BatchEditGuildApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a std::vector<GuildApplicationCommandPermissionsData>.
		static CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> batchEditGuildApplicationCommandPermissionsAsync(
			BatchEditGuildApplicationCommandPermissionsData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpClient* httpClient;
	};
	/**@}*/

}// namespace DiscordCoreAPI
