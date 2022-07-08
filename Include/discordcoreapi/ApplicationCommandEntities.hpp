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
/// ApplicationCommandEntities.hpp - Header for the ApplicationCommand classes
/// and structs. Aug 25, 2021 Chris M.
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For getting all of the Global Application Commands. \brief For getting all of the Global Application Commands.
	struct DiscordCoreAPI_Dll GetGlobalApplicationCommandsData {
		bool withLocalizations{ false };///< Do we collect the name-and-description localizations?
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
	};

	/// For collecting a single global ApplicationCommand. \brief For collecting a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll GetGlobalApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		std::string commandId{};///< The id of the command which you would like to collect.
	};

	/// For creating a single global ApplicationCommand. \brief For creating a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll CreateGlobalApplicationCommandData {
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the description field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app,
		std::string description{};///< A description of the command.
		Snowflake applicationId{};///< Application id.
		std::string name{};///< A name for the new command.

		operator std::string();
	};

	/// For editing a single global ApplicationCommand. \brief For editing a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGlobalApplicationCommandData {
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the description field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app.
		std::string description{};///< A description of the command.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		std::string name{};///< A name for the new command.

		operator std::string();
	};

	/// For deleting a single global ApplicationCommand. \brief For deleting a single global ApplicationCommand.
	struct DiscordCoreAPI_Dll DeleteGlobalApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		std::string name{};///< The name of the command to delete.
	};

	/// For bulk-overwriting a collection of global ApplicationCommands. \brief For bulk-overwriting a collection of global ApplicationCommands.
	struct DiscordCoreAPI_Dll BulkOverwriteGlobalApplicationCommandsData {
		std::vector<CreateGlobalApplicationCommandData> responseData{};///< A vector of the options for the ApplicationCommands.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
	};

	/// For acquiring all of the Guild ApplicationCommands of a single Guild. \brief For acquiring all of the Guild ApplicationCommands of a single Guild.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandsData {
		bool withLocalizations{ false };///< Do we collect the name-and-description localizations?
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild for which you would like to acquire the ApplicationCommands from.
	};

	/// For creating a single Guild ApplicationCommand. \brief For creating a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll CreateGuildApplicationCommandData {
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the description field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app,
		std::string description{};///< A description of the command.
		Snowflake applicationId{};///< Application id.
		Snowflake guildId{};///< The guild's id.
		std::string name{};///< A name for the new command.

		operator std::string();
	};

	/// For acquiring a single Guild ApplicationCommand. \brief For acquiring a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll GetGuildApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		uint64_t commandId{};///< The command id which you would like to acquire.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the ApplicationCommand from.
	};

	/// For editing a single Guild ApplicationCommand. \brief For editing a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandData {
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the description field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app.
		std::string description{};///< A description of the command.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild which you would like to add the new command to.
		std::string name{};///< A name for the new command.

		operator std::string();
	};

	/// For deleting a single Guild ApplicationCommand. \brief For deleting a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll DeleteGuildApplicationCommandData {
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild which you would like to delete the command from.
		std::string name{};///< A name of the command which you would like to delete.
	};

	/// For bulk-overwriting a collection of Guild ApplicationCommands. \brief For bulk-overwriting a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll BulkOverwriteGuildApplicationCommandsData {
		std::vector<CreateGuildApplicationCommandData> responseData{};///< A vector of the options for the ApplicationCommands.
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
		std::string commandName{};///< The name of the command which you would like to collect the permissions of.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the command permissions.
	};

	/// For editing the permissions of a single Guild ApplicationCommand. \brief For editing the permissions of a single Guild ApplicationCommand.
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandPermissionsData {
		std::vector<ApplicationCommandPermissionData> permissions{};///< A vector of ApplicationCommand permissions.
		std::string commandName{};///< The command name which you would like to edit the permissions of.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		uint64_t commandId{};///< The command id which you would like to edit the permissions of.
		Snowflake guildId{};///< The Guild id of the Guild for which you would like to edit the command permissions.

		operator std::string();
	};

	/// For batch editing the permissions of a collection of Guild ApplicationCommands. \brief For batch editing the permissions of a collection of Guild ApplicationCommands.
	struct DiscordCoreAPI_Dll BatchEditGuildApplicationCommandPermissionsData {
		std::vector<GuildApplicationCommandPermissionsData> permissions{};///< A vector of edit-Guild-application-permissions responseData to edit.
		Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		Snowflake guildId{};///< The Guild id of the Guild for which you would like to batch edit Guild application permissions.
	};

	/// A single ApplicationCommand. \brief A single ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommand : public ApplicationCommandData, public DataParser<ApplicationCommand> {
	  public:
		ApplicationCommand() = default;

		ApplicationCommand& operator=(const nlohmann::json& jsonObjectData);

		ApplicationCommand(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationCommand() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommand* pDataStructure);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the ApplicationCommand related Discord endpoints. \brief An interface class for the ApplicationCommand related Discord endpoints.
	class DiscordCoreAPI_Dll ApplicationCommands {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// Get all of the global ApplicationCommands for this bot. \brief Get all of the global ApplicationCommands for this bot.
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
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
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
		static CoRoutine<std::vector<ApplicationCommand>> bulkOverwriteGlobalApplicationCommandsAsync(BulkOverwriteGlobalApplicationCommandsData dataPackage);

		/// Get all of the Guild ApplicationCommands for a single Guild for this bot. \brief Get all of the Guild ApplicationCommands for a single Guild for this bot.
		/// \param dataPackage A GetGuildApplicationCommandsData structure.
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
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
		/// \returns A CoRoutine containing a vector<ApplicationCommand>.
		static CoRoutine<std::vector<ApplicationCommand>> bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage);

		/// Gets Guild ApplicationCommand permissions for a single server on this bot. \brief Gets Guild ApplicationCommand permissions for a single server on this bot.
		/// \param dataPackage A GetGuildApplicationCommandPermissionsData structure.
		/// \returns A CoRoutine containing a vector<GuildApplicationCommandPermissionsData>.
		static CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> getGuildApplicationCommandPermissionsAsync(GetGuildApplicationCommandPermissionsData dataPackage);

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
