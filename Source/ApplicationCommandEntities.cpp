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
/// ApplicationCommandEntities.cpp - Source file for the application_command_data classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.cpp

#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::create_global_application_command_data> {
		using value_type				 = discord_core_api::create_global_application_command_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "application_id", &value_type::applicationId, "name", &value_type::name, "description",
			&value_type::description, "type", &value_type::type, "default_permission", &value_type::defaultMemberPermissions, "options", &value_type::options, "version",
			&value_type::version, "guild_id", &value_type::guildId, "dm_permission", &value_type::dmPermission, "name_localizations", &value_type::nameLocalizations,
			"description_localizations", &value_type::descriptionLocalizations);
	};

	template<> struct core<discord_core_api::create_guild_application_command_data> {
		using value_type				 = discord_core_api::create_guild_application_command_data;
		static constexpr auto parseValue = createValue("id", &value_type::id, "application_id", &value_type::applicationId, "name", &value_type::name, "description",
			&value_type::description, "type", &value_type::type, "default_permission", &value_type::defaultMemberPermissions, "options", &value_type::options, "version",
			&value_type::version, "guild_id", &value_type::guildId, "dm_permission", &value_type::dmPermission, "name_localizations", &value_type::nameLocalizations,
			"description_localizations", &value_type::descriptionLocalizations);
	};

	template<> struct core<discord_core_api::bulk_overwrite_global_application_commands_data> {
		using value_type				 = discord_core_api::bulk_overwrite_global_application_commands_data;
		static constexpr auto parseValue = createValue("commands", &value_type::responseData, "application_id", &value_type::applicationId);
	};

	template<> struct core<discord_core_api::edit_global_application_command_data> {
		using value_type				 = discord_core_api::edit_global_application_command_data;
		static constexpr auto parseValue = createValue("name", &value_type::name, "description", &value_type::description, "options", &value_type::options, "default_permission",
			&value_type::defaultMemberPermissions, "name_localization", &value_type::nameLocalizations, "description_localization", &value_type::descriptionLocalizations,
			"application_id", &value_type::applicationId, "dm_permission", &value_type::dmPermission);
	};

	template<> struct core<discord_core_api::edit_guild_application_command_data> {
		using value_type				 = discord_core_api::edit_guild_application_command_data;
		static constexpr auto parseValue = createValue("description_localizations", &value_type::descriptionLocalizations, "name_localizations", &value_type::nameLocalizations,
			"options", &value_type::options, "default_permission", &value_type::defaultMemberPermissions, "description", &value_type::description, "application_id",
			&value_type::applicationId, "guild_id", &value_type::guildId, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::bulk_overwrite_guild_application_commands_data> {
		using value_type				 = discord_core_api::bulk_overwrite_guild_application_commands_data;
		static constexpr auto parseValue = createValue("commands", &value_type::responseData, "application_id", &value_type::applicationId, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::edit_guild_application_command_permissions_data> {
		using value_type				 = discord_core_api::edit_guild_application_command_permissions_data;
		static constexpr auto parseValue = createValue("permissions", &value_type::permissions, "command", &value_type::commandName, "application_id", &value_type::applicationId,
			"id", &value_type::commandId, "guild_id", &value_type::guildId);
	};

	template<> struct core<discord_core_api::batch_edit_guild_application_command_permissions_data> {
		using value_type				 = discord_core_api::batch_edit_guild_application_command_permissions_data;
		static constexpr auto parseValue = createValue("permissions", &value_type::permissions, "application_id", &value_type::applicationId, "guild_id", &value_type::guildId);
	};
}

namespace discord_core_api {

	void application_commands::initialize(discord_core_internal::https_client* client) {
		application_commands::httpsClient = client;
	}

	co_routine<jsonifier::vector<application_command_data>> application_commands::getGlobalApplicationCommandsAsync(get_global_application_commands_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Global_Application_Commands };
		co_await newThreadAwaitable<jsonifier::vector<application_command_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "application_commands::getGlobalApplicationCommandsAsync()";
		jsonifier::vector<application_command_data> returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<application_command_data> application_commands::createGlobalApplicationCommandAsync(create_global_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Global_Application_Command };
		co_await newThreadAwaitable<application_command_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		dataPackage.generateExcludedKeys();
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "application_commands::createGlobalApplicationCommandAsync()";
		application_command_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<application_command_data> application_commands::getGlobalApplicationCommandAsync(get_global_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Global_Application_Command };
		co_await newThreadAwaitable<application_command_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands/" + dataPackage.commandId;
		workload.callStack	   = "application_commands::getGlobalApplicationCommandAsync()";
		application_command_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<application_command_data> application_commands::editGlobalApplicationCommandAsync(edit_global_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Global_Application_Command };
		co_await newThreadAwaitable<application_command_data>();
		jsonifier::vector<application_command_data> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		bool isItFound{};
		snowflake appCommandId{};
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id;
				isItFound	 = true;
			}
		}
		if (!isItFound) {
			co_return application_command_data();
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands/" + appCommandId.operator jsonifier::string();
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "application_commands::editGlobalApplicationCommandAsync()";
		application_command_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> application_commands::deleteGlobalApplicationCommandAsync(delete_global_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Global_Application_Command };
		co_await newThreadAwaitable<void>();
		jsonifier::vector<application_command_data> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		snowflake commandId{};
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return;
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands/" + commandId.operator jsonifier::string();
		workload.callStack	   = "application_commands::deleteGlobalApplicationCommandAsync()";
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<application_command_data>> application_commands::bulkOverwriteGlobalApplicationCommandsAsync(
		bulk_overwrite_global_application_commands_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Bulk_Put_Global_Application_Commands };
		co_await newThreadAwaitable<jsonifier::vector<application_command_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "application_commands::bulkOverwriteGlobalApplicationCommandsAsync()";
		jsonifier::vector<application_command_data> returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<application_command_data>> application_commands::getGuildApplicationCommandsAsync(get_guild_application_commands_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Application_Commands };
		co_await newThreadAwaitable<jsonifier::vector<application_command_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "application_commands::getGuildApplicationCommandsAsync()";
		jsonifier::vector<application_command_data> returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<application_command_data> application_commands::createGuildApplicationCommandAsync(create_guild_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Guild_Application_Command };
		co_await newThreadAwaitable<application_command_data>();
		dataPackage.applicationId = dataPackage.applicationId;
		workload.workloadClass	  = discord_core_internal::https_workload_class::Post;
		dataPackage.generateExcludedKeys();
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "application_commands::createGuildApplicationCommandAsync()";
		application_command_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<application_command_data> application_commands::getGuildApplicationCommandAsync(get_guild_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Application_Command };
		co_await newThreadAwaitable<application_command_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + dataPackage.commandId;
		workload.callStack	   = "application_commands::getGuildApplicationCommandAsync()";
		application_command_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<application_command_data> application_commands::editGuildApplicationCommandAsync(edit_guild_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Guild_Application_Command };
		co_await newThreadAwaitable<application_command_data>();
		jsonifier::vector<application_command_data> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		bool isItFound											= false;
		snowflake appCommandId{};
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id;
				isItFound	 = true;
			}
		}
		if (!isItFound) {
			co_return application_command_data();
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + appCommandId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "application_commands::editGuildApplicationCommandAsync()";
		application_command_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> application_commands::deleteGuildApplicationCommandAsync(delete_guild_application_command_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Guild_Application_Command };
		co_await newThreadAwaitable<void>();
		jsonifier::vector<application_command_data> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		snowflake commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return;
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId;
		workload.callStack	   = "application_commands::deleteGuildApplicationCommandAsync()";
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<jsonifier::vector<application_command_data>> application_commands::bulkOverwriteGuildApplicationCommandsAsync(
		bulk_overwrite_guild_application_commands_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Bulk_Put_Guild_Application_Commands };
		co_await newThreadAwaitable<jsonifier::vector<application_command_data>>();
		parser.serializeJson(dataPackage, workload.content);
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		workload.callStack	   = "application_commands::bulkOverwriteGuildApplicationCommandsAsync()";
		jsonifier::vector<application_command_data> returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<guild_application_command_permissions_data>> application_commands::getGuildApplicationCommandPermissionsAsync(
		get_guild_application_command_permissions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Application_Commands_Permissions };
		co_await newThreadAwaitable<jsonifier::vector<guild_application_command_permissions_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/permissions";
		workload.callStack	   = "application_commands::getGuildApplicationCommandPermissionsAsync()";
		jsonifier::vector<guild_application_command_permissions_data> returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_application_command_permissions_data> application_commands::getApplicationCommandPermissionsAsync(get_application_command_permissions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Application_Command_Permissions };
		co_await newThreadAwaitable<guild_application_command_permissions_data>();
		jsonifier::vector<application_command_data> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		snowflake commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return guild_application_command_permissions_data();
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		workload.callStack	   = "application_commands::getApplicationCommandPermissionsAsync()";
		guild_application_command_permissions_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<guild_application_command_permissions_data> application_commands::editGuildApplicationCommandPermissionsAsync(
		edit_guild_application_command_permissions_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Put_Guild_Application_Command_Permissions };
		co_await newThreadAwaitable<guild_application_command_permissions_data>();
		jsonifier::vector<application_command_data> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		snowflake commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return guild_application_command_permissions_data();
		}
		workload.workloadClass = discord_core_internal::https_workload_class::Put;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "application_commands::editGuildApplicationCommandPermissionsAsync()";
		guild_application_command_permissions_data returnData{};
		application_commands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	discord_core_internal::https_client* application_commands::httpsClient{};
}
