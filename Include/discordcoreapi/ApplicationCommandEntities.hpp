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
/// ApplicationCommandEntities.hpp - Header for the ApplicationCommandData classes
/// and structs. Aug 25, 2021 Chris M.
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	*/

	/// @brief For getting all of the Global Application Commands.
	struct GetGlobalApplicationCommandsData {
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		bool withLocalizations{};///< Do we collect the name-and-description localizations?
	};

	/// @brief For collecting a single global ApplicationCommandData.
	struct GetGlobalApplicationCommandData {
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		std::string commandId{};///< The id of the command which you would like to collect.
	};

	/// @brief For creating an application command.
	struct CreateApplicationCommandData : public ApplicationCommandData {
		friend class DiscordCoreClient;

	  protected:
		bool alwaysRegister{};
	};

	/// @brief For creating a single global ApplicationCommandData.
	struct CreateGlobalApplicationCommandData : public CreateApplicationCommandData {};

	/// @brief For editing a single global ApplicationCommandData.
	struct EditGlobalApplicationCommandData {
		UnorderedMap<std::string, std::string>
			descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the description field.
		UnorderedMap<std::string, std::string>
			nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommandData.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		std::string description{};///< A description of the command.
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		bool dmPermission{};///< Indicates whether the command is available in DMs with the app.
		std::string name{};///< A name for the new command.
	};

	/// @brief For deleting a single global ApplicationCommandData.
	struct DeleteGlobalApplicationCommandData {
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		std::string name{};///< The name of the command to delete.
	};

	/// @brief For bulk-overwriting a collection of global ApplicationCommands.
	struct BulkOverwriteGlobalApplicationCommandsData {
		std::vector<CreateGlobalApplicationCommandData> responseData{};///< A vector of the options for the ApplicationCommands.
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
	};

	/// @brief For acquiring all of the Guild ApplicationCommands of a single Guild.
	struct GetGuildApplicationCommandsData {
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		bool withLocalizations{};///< Do we collect the name-and-description localizations?
		Snowflake guildId{};///< The id of the Guild for which you would like to acquire the ApplicationCommands from.
	};

	/// @brief For creating a single Guild ApplicationCommandData.
	struct CreateGuildApplicationCommandData : public CreateGlobalApplicationCommandData {};

	/// @brief For acquiring a single Guild ApplicationCommandData.
	struct GetGuildApplicationCommandData {
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		uint64_t commandId{};///< The command id which you would like to acquire.
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the ApplicationCommandData from.
	};

	/// @brief For editing a single Guild ApplicationCommandData.
	struct EditGuildApplicationCommandData {
		UnorderedMap<std::string, std::string> descriptionLocalizations{};///< Dictionary with keys in available locales.
		UnorderedMap<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales.
		std::vector<ApplicationCommandOptionData> options{};///< The options for the ApplicationCommandData.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		std::string description{};///< A description of the command.
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		bool dmPermission{};///< Indicates whether the command is available in DMs with the app.
		Snowflake guildId{};///< The id of the Guild which you would like to add the new command to.
		std::string name{};///< A name for the new command.
	};

	/// @brief For deleting a single Guild ApplicationCommandData.
	struct DeleteGuildApplicationCommandData {
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		Snowflake guildId{};///< The id of the Guild which you would like to delete the command from.
		std::string name{};///< A name of the command which you would like to delete.
	};

	/// @brief For bulk-overwriting a collection of Guild ApplicationCommands.
	struct BulkOverwriteGuildApplicationCommandsData {
		std::vector<CreateGuildApplicationCommandData> responseData{};///< A vector of the options for the ApplicationCommands.
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		Snowflake guildId{};///< The id of the Guild which you would like to overwrite the commands of.
	};

	/// @brief For acquiring the permissions of a collection of Guild ApplicationCommands.
	struct GetGuildApplicationCommandPermissionsData {
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the command permissions.
	};

	/// @brief For acquiring the permissions of a single Guild ApplicationCommandData.
	struct GetApplicationCommandPermissionsData {
		std::string commandName{};///< The name of the command which you would like to collect the permissions of.
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		Snowflake guildId{};///< The id of the Guild from which you would like to acquire the command permissions.
	};

	/// @brief For editing the permissions of a single Guild ApplicationCommandData.
	struct EditGuildApplicationCommandPermissionsData {
		std::vector<ApplicationCommandPermissionData> permissions{};///< A vector of ApplicationCommandData permissions.
		std::string commandName{};///< The command name which you would like to edit the permissions of.
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		uint64_t commandId{};///< The command id which you would like to edit the permissions of.
		Snowflake guildId{};///< The Guild id of the Guild for which you would like to edit the command permissions.
	};

	/// @brief For batch editing the permissions of a collection of Guild ApplicationCommands.
	struct BatchEditGuildApplicationCommandPermissionsData {
		std::vector<GuildApplicationCommandPermissionsData> permissions{};///< A vector of edit-Guild-application-permissions responseData to edit.
		Snowflake applicationId{};///< The current application's Snowflake (The Bot's UserData Snowflake).
		Snowflake guildId{};///< The Guild id of the Guild for which you would like to batch edit Guild application permissions.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the ApplicationCommandData related Discord endpoints.
	class ApplicationCommands {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Get all of the global ApplicationCommands for this bot.
		/// @param dataPackage The info for acquiring the GlobalApplicationCommandData.
		/// @return A CoRoutine containing a vector<ApplicationCommandData>.
		static CoRoutine<std::vector<ApplicationCommandData>> getGlobalApplicationCommandsAsync(GetGlobalApplicationCommandsData dataPackage);

		/// @brief Create a global ApplicationCommandData for this bot.
		/// @param dataPackage A CreateGlobalApplicationCommandData structure.
		/// @return A CoRoutine containing an ApplicationCommandData.
		static CoRoutine<ApplicationCommandData> createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage);

		/// @brief Get a single global ApplicationCommandData for this bot.
		/// @param dataPackage A GetGlobalApplicationCommandData structure.
		/// @return A CoRoutine containing an ApplicationCommandData.
		static CoRoutine<ApplicationCommandData> getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage);

		/// @brief Edit a global ApplicationCommandData for this bot.
		/// @param dataPackage An EditGlobalApplicationCommandData structure.
		/// @return A CoRoutine containing an ApplicationCommandData.
		static CoRoutine<ApplicationCommandData> editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage);

		/// @brief Delete a global ApplicationCommandData for this bot.
		/// @param dataPackage A DeleteGlobalApplicationCommandData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage);

		/// @brief Bulk overwrites a collection of global ApplicationCommands.
		/// @param dataPackage A BulkOverwriteGlobalApplicationCommandsData structure.
		/// @return A CoRoutine containing a vector<ApplicationCommandData>.
		static CoRoutine<std::vector<ApplicationCommandData>> bulkOverwriteGlobalApplicationCommandsAsync(
			BulkOverwriteGlobalApplicationCommandsData dataPackage);

		/// @brief Get all of the Guild ApplicationCommands for a single Guild for this bot.
		/// @param dataPackage A GetGuildApplicationCommandsData structure.
		/// @return A CoRoutine containing a vector<ApplicationCommandData>.
		static CoRoutine<std::vector<ApplicationCommandData>> getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage);

		/// @brief Create a Guild ApplicationCommandData for a single server for this bot.
		/// @param dataPackage A CreateGuildApplicationCommandData structure.
		/// @return A CoRoutine containing an ApplicationCommandData.
		static CoRoutine<ApplicationCommandData> createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage);

		/// @brief Get a single Guild ApplicationCommandData for a single server for this bot.
		/// @param dataPackage A GetGuildApplicationCommandData structure.
		/// @return A CoRoutine containing an ApplicationCommandData.
		static CoRoutine<ApplicationCommandData> getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage);

		/// @brief Edit a single Guild ApplicationCommandData for a single server for this bot.
		/// @param dataPackage An EditGuildApplicationCommandData structure.
		/// @return A CoRoutine containing an ApplicationCommandData.
		static CoRoutine<ApplicationCommandData> editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage);

		/// @brief Delete a single Guild ApplicationCommandData for a single server for this bot.
		/// @param dataPackage A DeleteGuildApplicationCommandData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteGuildApplicationCommandAsync(DeleteGuildApplicationCommandData dataPackage);

		/// @brief Bulkoverwrites some Guild ApplicationCommands for this bot.
		/// @param dataPackage A BulkOverwriteGuildApplicationCommandsData structure.
		/// @return A CoRoutine containing a vector<ApplicationCommandData>.
		static CoRoutine<std::vector<ApplicationCommandData>> bulkOverwriteGuildApplicationCommandsAsync(
			BulkOverwriteGuildApplicationCommandsData dataPackage);

		/// @brief Gets Guild ApplicationCommandData permissions for a single server on this bot.
		/// @param dataPackage A GetGuildApplicationCommandPermissionsData structure.
		/// @return A CoRoutine containing a vector<GuildApplicationCommandPermissionsData>.
		static CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> getGuildApplicationCommandPermissionsAsync(
			GetGuildApplicationCommandPermissionsData dataPackage);

		/// @brief Get ApplicationCommandData permissions for a single command on this bot.
		/// @param dataPackage A GetApplicationCommandPermissionsData structure.
		/// @return A CoRoutine containing a GuildApplicationCommandPermissionsData.
		static CoRoutine<GuildApplicationCommandPermissionsData> getApplicationCommandPermissionsAsync(
			GetApplicationCommandPermissionsData dataPackage);

		/// @brief Edit Guild ApplicationCommandData permissions for a server on this bot.
		/// @param dataPackage An EditGuildApplicationCommandPermissionsData structure.
		/// @return A CoRoutine containing a GuildApplicationCommandPermissionsData.
		static CoRoutine<GuildApplicationCommandPermissionsData> editGuildApplicationCommandPermissionsAsync(
			EditGuildApplicationCommandPermissionsData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};

	/**@}*/

}
