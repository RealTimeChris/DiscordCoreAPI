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
/// ApplicationCommandEntities.hpp - Header for the application_command_data classes
/// and structs. aug 25, 2021 Chris M.
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	*/

	/// @brief For getting all of the global application commands.
	struct get_global_application_commands_data {
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		bool withLocalizations{};///< Do we collect the name-and-description localizations?
	};

	/// @brief For collecting a single global application_command_data.
	struct get_global_application_command_data {
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		jsonifier::string commandId{};///< The id of the command which you would like to collect.
	};

	/// @brief For creating an application command.
	struct create_application_command_data : public application_command_data {
		friend class discord_core_client;

	  protected:
		bool alwaysRegister{};
	};

	/// @brief For creating a single global application_command_data.
	struct create_global_application_command_data : public create_application_command_data {};

	/// @brief For editing a single global application_command_data.
	struct edit_global_application_command_data {
		unordered_map<jsonifier::string, jsonifier::string>
			descriptionLocalizations{};///< Dictionary with keys in available locales localization dictionary for the description field.
		unordered_map<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary with keys in available locales localization dictionary for the name field.
		jsonifier::vector<application_command_option_data> options{};///< The options for the application_command_data.
		jsonifier::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		jsonifier::string description{};///< A description of the command.
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		bool dmPermission{};///< Indicates whether the command is available in dms with the app.
		jsonifier::string name{};///< A name for the new command.
	};

	/// @brief For deleting a single global application_command_data.
	struct delete_global_application_command_data {
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		jsonifier::string name{};///< The name of the command to delete.
	};

	/// @brief For bulk-overwriting a collection of global application_commands.
	struct bulk_overwrite_global_application_commands_data {
		jsonifier::vector<create_global_application_command_data> responseData{};///< A vector of the options for the application_commands.
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
	};

	/// @brief For acquiring all of the guild application_commands of a single guild.
	struct get_guild_application_commands_data {
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		bool withLocalizations{};///< Do we collect the name-and-description localizations?
		snowflake guildId{};///< The id of the guild for which you would like to acquire the application_commands from.
	};

	/// @brief For creating a single guild application_command_data.
	struct create_guild_application_command_data : public create_global_application_command_data {};

	/// @brief For acquiring a single guild application_command_data.
	struct get_guild_application_command_data {
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		snowflake commandId{};///< The command id which you would like to acquire.
		snowflake guildId{};///< The id of the guild from which you would like to acquire the application_command_data from.
	};

	/// @brief For editing a single guild application_command_data.
	struct edit_guild_application_command_data {
		unordered_map<jsonifier::string, jsonifier::string> descriptionLocalizations{};///< Dictionary with keys in available locales.
		unordered_map<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary with keys in available locales.
		jsonifier::vector<application_command_option_data> options{};///< The options for the application_command_data.
		jsonifier::string defaultMemberPermissions{};///< Set of permissions represented as a bit set. only for globally - scoped commands.
		jsonifier::string description{};///< A description of the command.
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		bool dmPermission{};///< Indicates whether the command is available in dms with the app.
		snowflake guildId{};///< The id of the guild which you would like to add the new command to.
		jsonifier::string name{};///< A name for the new command.
	};

	/// @brief For deleting a single guild application_command_data.
	struct delete_guild_application_command_data {
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		snowflake guildId{};///< The id of the guild which you would like to delete the command from.
		jsonifier::string name{};///< A name of the command which you would like to delete.
	};

	/// @brief For bulk-overwriting a collection of guild application_commands.
	struct bulk_overwrite_guild_application_commands_data {
		jsonifier::vector<create_guild_application_command_data> responseData{};///< A vector of the options for the application_commands.
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		snowflake guildId{};///< The id of the guild which you would like to overwrite the commands of.
	};

	/// @brief For acquiring the permissions of a collection of guild application_commands.
	struct get_guild_application_command_permissions_data {
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		snowflake guildId{};///< The id of the guild from which you would like to acquire the command permissions.
	};

	/// @brief For acquiring the permissions of a single guild application_command_data.
	struct get_application_command_permissions_data {
		jsonifier::string commandName{};///< The name of the command which you would like to collect the permissions of.
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		snowflake guildId{};///< The id of the guild from which you would like to acquire the command permissions.
	};

	/// @brief For editing the permissions of a single guild application_command_data.
	struct edit_guild_application_command_permissions_data {
		jsonifier::vector<application_command_permission_data> permissions{};///< A vector of application_command_data permissions.
		jsonifier::string commandName{};///< The command name which you would like to edit the permissions of.
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		snowflake commandId{};///< The command id which you would like to edit the permissions of.
		snowflake guildId{};///< The guild id of the guild for which you would like to edit the command permissions.
	};

	/// @brief For batch editing the permissions of a collection of guild application_commands.
	struct batch_edit_guild_application_command_permissions_data {
		jsonifier::vector<guild_application_command_permissions_data> permissions{};///< A vector of edit-guild-application-permissions responseData to edit.
		snowflake applicationId{};///< The current application's snowflake (the bot's user_data snowflake).
		snowflake guildId{};///< The guild id of the guild for which you would like to batch edit guild application permissions.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the application_command_data related discord endpoints.
	class application_commands {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Get all of the global application_commands for this bot.
		/// @param dataPackage the info for acquiring the global_application_command_data.
		/// @return a co_routine containing a vector<application_command_data>.
		static co_routine<jsonifier::vector<application_command_data>> getGlobalApplicationCommandsAsync(get_global_application_commands_data dataPackage);

		/// @brief Create a global application_command_data for this bot.
		/// @param dataPackage a create_global_application_command_data structure.
		/// @return a co_routine containing an application_command_data.
		static co_routine<application_command_data> createGlobalApplicationCommandAsync(create_global_application_command_data dataPackage);

		/// @brief Get a single global application_command_data for this bot.
		/// @param dataPackage a get_global_application_command_data structure.
		/// @return a co_routine containing an application_command_data.
		static co_routine<application_command_data> getGlobalApplicationCommandAsync(get_global_application_command_data dataPackage);

		/// @brief Edit a global application_command_data for this bot.
		/// @param dataPackage an edit_global_application_command_data structure.
		/// @return a co_routine containing an application_command_data.
		static co_routine<application_command_data> editGlobalApplicationCommandAsync(edit_global_application_command_data dataPackage);

		/// @brief Delete a global application_command_data for this bot.
		/// @param dataPackage a delete_global_application_command_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteGlobalApplicationCommandAsync(delete_global_application_command_data dataPackage);

		/// @brief Bulk overwrites a collection of global application_commands.
		/// @param dataPackage a bulk_overwrite_global_application_commands_data structure.
		/// @return a co_routine containing a vector<application_command_data>.
		static co_routine<jsonifier::vector<application_command_data>> bulkOverwriteGlobalApplicationCommandsAsync(bulk_overwrite_global_application_commands_data dataPackage);

		/// @brief Get all of the guild application_commands for a single guild for this bot.
		/// @param dataPackage a get_guild_application_commands_data structure.
		/// @return a co_routine containing a vector<application_command_data>.
		static co_routine<jsonifier::vector<application_command_data>> getGuildApplicationCommandsAsync(get_guild_application_commands_data dataPackage);

		/// @brief Create a guild application_command_data for a single server for this bot.
		/// @param dataPackage a create_guild_application_command_data structure.
		/// @return a co_routine containing an application_command_data.
		static co_routine<application_command_data> createGuildApplicationCommandAsync(create_guild_application_command_data dataPackage);

		/// @brief Get a single guild application_command_data for a single server for this bot.
		/// @param dataPackage a get_guild_application_command_data structure.
		/// @return a co_routine containing an application_command_data.
		static co_routine<application_command_data> getGuildApplicationCommandAsync(get_guild_application_command_data dataPackage);

		/// @brief Edit a single guild application_command_data for a single server for this bot.
		/// @param dataPackage an edit_guild_application_command_data structure.
		/// @return a co_routine containing an application_command_data.
		static co_routine<application_command_data> editGuildApplicationCommandAsync(edit_guild_application_command_data dataPackage);

		/// @brief Delete a single guild application_command_data for a single server for this bot.
		/// @param dataPackage a delete_guild_application_command_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteGuildApplicationCommandAsync(delete_guild_application_command_data dataPackage);

		/// @brief Bulkoverwrites some guild application_commands for this bot.
		/// @param dataPackage a bulk_overwrite_guild_application_commands_data structure.
		/// @return a co_routine containing a vector<application_command_data>.
		static co_routine<jsonifier::vector<application_command_data>> bulkOverwriteGuildApplicationCommandsAsync(bulk_overwrite_guild_application_commands_data dataPackage);

		/// @brief Gets guild application_command_data permissions for a single server on this bot.
		/// @param dataPackage a get_guild_application_command_permissions_data structure.
		/// @return a co_routine containing a vector<guild_application_command_permissions_data>.
		static co_routine<jsonifier::vector<guild_application_command_permissions_data>> getGuildApplicationCommandPermissionsAsync(
			const get_guild_application_command_permissions_data dataPackage);

		/// @brief Get application_command_data permissions for a single command on this bot.
		/// @param dataPackage a get_application_command_permissions_data structure.
		/// @return a co_routine containing a guild_application_command_permissions_data.
		static co_routine<guild_application_command_permissions_data> getApplicationCommandPermissionsAsync(get_application_command_permissions_data dataPackage);

		/// @brief Edit guild application_command_data permissions for a server on this bot.
		/// @param dataPackage an edit_guild_application_command_permissions_data structure.
		/// @return a co_routine containing a guild_application_command_permissions_data.
		static co_routine<guild_application_command_permissions_data> editGuildApplicationCommandPermissionsAsync(
			const edit_guild_application_command_permissions_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};

	/**@}*/

}
