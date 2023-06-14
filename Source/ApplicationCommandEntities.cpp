/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// ApplicationCommandEntities.cpp - Source file for the ApplicationCommand classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.cpp

#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::CreateGlobalApplicationCommandData> {
		using ValueType = DiscordCoreAPI::CreateGlobalApplicationCommandData;
		static constexpr auto parseValue = object("id", &ValueType::id, "application_id", &ValueType::applicationId, "name", &ValueType::name,
			"description", &ValueType::description, "type", &ValueType::type, "default_permission", &ValueType::defaultMemberPermissions, "options",
			&ValueType::options, "version", &ValueType::version, "guild_id", &ValueType::guildId, "dm_permission", &ValueType::dmPermission,
			"name_localizations", &ValueType::nameLocalizations, "description_localizations", &ValueType::descriptionLocalizations);
	};

	template<> struct Core<DiscordCoreAPI::CreateGuildApplicationCommandData> {
		using ValueType = DiscordCoreAPI::CreateGuildApplicationCommandData;
		static constexpr auto parseValue = object("id", &ValueType::id, "application_id", &ValueType::applicationId, "name", &ValueType::name,
			"description", &ValueType::description, "type", &ValueType::type, "default_permission", &ValueType::defaultMemberPermissions, "options",
			&ValueType::options, "version", &ValueType::version, "guild_id", &ValueType::guildId, "dm_permission", &ValueType::dmPermission,
			"name_localizations", &ValueType::nameLocalizations, "description_localizations", &ValueType::descriptionLocalizations);
	};

	template<> struct Core<DiscordCoreAPI::BulkOverwriteGlobalApplicationCommandsData> {
		using ValueType = DiscordCoreAPI::BulkOverwriteGlobalApplicationCommandsData;
		static constexpr auto parseValue = object("commands", &ValueType::responseData, "application_id", &ValueType::applicationId);
	};

	template<> struct Core<DiscordCoreAPI::EditGlobalApplicationCommandData> {
		using ValueType = DiscordCoreAPI::EditGlobalApplicationCommandData;
		static constexpr auto parseValue =
			object("name", &ValueType::name, "description", &ValueType::description, "options", &ValueType::options, "default_permission",
				&ValueType::defaultMemberPermissions, "name_localization", &ValueType::nameLocalizations, "description_localization",
				&ValueType::descriptionLocalizations, "application_id", &ValueType::applicationId, "dm_permission", &ValueType::dmPermission);
	};

	template<> struct Core<DiscordCoreAPI::EditGuildApplicationCommandData> {
		using ValueType = DiscordCoreAPI::EditGuildApplicationCommandData;
		static constexpr auto parseValue = object("description_localizations", &ValueType::descriptionLocalizations, "name_localizations",
			&ValueType::nameLocalizations, "options", &ValueType::options, "default_permission", &ValueType::defaultMemberPermissions, "description",
			&ValueType::description, "application_id", &ValueType::applicationId, "guild_id", &ValueType::guildId, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::BulkOverwriteGuildApplicationCommandsData> {
		using ValueType = DiscordCoreAPI::BulkOverwriteGuildApplicationCommandsData;
		static constexpr auto parseValue =
			object("commands", &ValueType::responseData, "application_id", &ValueType::applicationId, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::EditGuildApplicationCommandPermissionsData> {
		using ValueType = DiscordCoreAPI::EditGuildApplicationCommandPermissionsData;
		static constexpr auto parseValue = object("permissions", &ValueType::permissions, "command", &ValueType::commandName, "application_id",
			&ValueType::applicationId, "id", &ValueType::commandId, "guild_id", &ValueType::guildId);
	};

	template<> struct Core<DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData> {
		using ValueType = DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData;
		static constexpr auto parseValue =
			object("permissions", &ValueType::permissions, "application_id", &ValueType::applicationId, "guild_id", &ValueType::guildId);
	};
}

namespace DiscordCoreAPI {

	void ApplicationCommands::initialize(DiscordCoreInternal::HttpsClient* client) {
		ApplicationCommands::httpsClient = client;
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGlobalApplicationCommandsAsync(GetGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGlobalApplicationCommandsAsync()";
		ApplicationCommandVector returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::createGlobalApplicationCommandAsync()";
		ApplicationCommand returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + dataPackage.commandId;
		workload.callStack = "ApplicationCommands::getGlobalApplicationCommandAsync()";
		ApplicationCommand returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		std::vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		bool isItFound{};
		std::string appCommandId{};
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id.operator std::string();
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + appCommandId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::editGlobalApplicationCommandAsync()";
		ApplicationCommand returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> ApplicationCommands::deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Global_Application_Command };
		co_await NewThreadAwaitable<void>();
		std::vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		std::string commandId{};
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				commandId = value.id.operator std::string();
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + commandId;
		workload.callStack = "ApplicationCommands::deleteGlobalApplicationCommandAsync()";
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync(
		BulkOverwriteGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Global_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync()";
		ApplicationCommandVector returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandsAsync()";
		ApplicationCommandVector returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		dataPackage.applicationId = dataPackage.applicationId;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::createGuildApplicationCommandAsync()";
		ApplicationCommand returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath =
			"/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + std::to_string(dataPackage.commandId);
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandAsync()";
		ApplicationCommand returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		bool isItFound = false;
		std::string appCommandId;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id.operator std::string();
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + appCommandId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandAsync()";
		ApplicationCommand returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> ApplicationCommands::deleteGuildApplicationCommandAsync(DeleteGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Application_Command };
		co_await NewThreadAwaitable<void>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				commandId = value.id.operator std::string();
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId;
		workload.callStack = "ApplicationCommands::deleteGuildApplicationCommandAsync()";
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync(
		BulkOverwriteGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Guild_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		workload.callStack = "ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync()";
		ApplicationCommandVector returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData>> ApplicationCommands::getGuildApplicationCommandPermissionsAsync(
		GetGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands_Permissions };
		co_await NewThreadAwaitable<std::vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/permissions";
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandPermissionsAsync()";
		GuildApplicationCommandPermissionsDataVector returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsDataVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::getApplicationCommandPermissionsAsync(
		GetApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command_Permissions };
		co_await NewThreadAwaitable<DiscordCoreAPI::GuildApplicationCommandPermissionsData>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id.operator std::string();
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return DiscordCoreAPI::GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath =
			"/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		workload.callStack = "ApplicationCommands::getApplicationCommandPermissionsAsync()";
		DiscordCoreAPI::GuildApplicationCommandPermissionsData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<DiscordCoreAPI::GuildApplicationCommandPermissionsData>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::editGuildApplicationCommandPermissionsAsync(
		EditGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Application_Command_Permissions };
		co_await NewThreadAwaitable<DiscordCoreAPI::GuildApplicationCommandPermissionsData>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id.operator std::string();
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return DiscordCoreAPI::GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath =
			"/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandPermissionsAsync()";
		DiscordCoreAPI::GuildApplicationCommandPermissionsData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult<DiscordCoreAPI::GuildApplicationCommandPermissionsData>(workload, returnData);
		co_return returnData;
	}

	DiscordCoreInternal::HttpsClient* ApplicationCommands::httpsClient{};
}
