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
/// ApplicationCommandEntities.cpp - Source file for the ApplicationCommandData classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.cpp

#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace jsonifier {

	template<> struct core<DiscordCoreAPI::CreateGlobalApplicationCommandData> {
		using ValueType					 = DiscordCoreAPI::CreateGlobalApplicationCommandData;
		static constexpr auto parseValue = createObject("id", &ValueType::id, "application_id", &ValueType::applicationId, "name", &ValueType::name, "description",
			&ValueType::description, "type", &ValueType::type, "default_permission", &ValueType::defaultMemberPermissions, "options", &ValueType::options, "version",
			&ValueType::version, "guild_id", &ValueType::guildId, "dm_permission", &ValueType::dmPermission, "name_localizations", &ValueType::nameLocalizations,
			"description_localizations", &ValueType::descriptionLocalizations);
	};

	template<> struct core<DiscordCoreAPI::CreateGuildApplicationCommandData> {
		using ValueType					 = DiscordCoreAPI::CreateGuildApplicationCommandData;
		static constexpr auto parseValue = createObject("id", &ValueType::id, "application_id", &ValueType::applicationId, "name", &ValueType::name, "description",
			&ValueType::description, "type", &ValueType::type, "default_permission", &ValueType::defaultMemberPermissions, "options", &ValueType::options, "version",
			&ValueType::version, "guild_id", &ValueType::guildId, "dm_permission", &ValueType::dmPermission, "name_localizations", &ValueType::nameLocalizations,
			"description_localizations", &ValueType::descriptionLocalizations);
	};

	template<> struct core<DiscordCoreAPI::BulkOverwriteGlobalApplicationCommandsData> {
		using ValueType					 = DiscordCoreAPI::BulkOverwriteGlobalApplicationCommandsData;
		static constexpr auto parseValue = createObject("commands", &ValueType::responseData, "application_id", &ValueType::applicationId);
	};

	template<> struct core<DiscordCoreAPI::EditGlobalApplicationCommandData> {
		using ValueType					 = DiscordCoreAPI::EditGlobalApplicationCommandData;
		static constexpr auto parseValue = createObject("name", &ValueType::name, "description", &ValueType::description, "options", &ValueType::options, "default_permission",
			&ValueType::defaultMemberPermissions, "name_localization", &ValueType::nameLocalizations, "description_localization", &ValueType::descriptionLocalizations,
			"application_id", &ValueType::applicationId, "dm_permission", &ValueType::dmPermission);
	};

	template<> struct core<DiscordCoreAPI::EditGuildApplicationCommandData> {
		using ValueType					 = DiscordCoreAPI::EditGuildApplicationCommandData;
		static constexpr auto parseValue = createObject("description_localizations", &ValueType::descriptionLocalizations, "name_localizations", &ValueType::nameLocalizations,
			"options", &ValueType::options, "default_permission", &ValueType::defaultMemberPermissions, "description", &ValueType::description, "application_id",
			&ValueType::applicationId, "guild_id", &ValueType::guildId, "name", &ValueType::name);
	};

	template<> struct core<DiscordCoreAPI::BulkOverwriteGuildApplicationCommandsData> {
		using ValueType					 = DiscordCoreAPI::BulkOverwriteGuildApplicationCommandsData;
		static constexpr auto parseValue = createObject("commands", &ValueType::responseData, "application_id", &ValueType::applicationId, "guild_id", &ValueType::guildId);
	};

	template<> struct core<DiscordCoreAPI::EditGuildApplicationCommandPermissionsData> {
		using ValueType					 = DiscordCoreAPI::EditGuildApplicationCommandPermissionsData;
		static constexpr auto parseValue = createObject("permissions", &ValueType::permissions, "command", &ValueType::commandName, "application_id", &ValueType::applicationId,
			"id", &ValueType::commandId, "guild_id", &ValueType::guildId);
	};

	template<> struct core<DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData> {
		using ValueType					 = DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData;
		static constexpr auto parseValue = createObject("permissions", &ValueType::permissions, "application_id", &ValueType::applicationId, "guild_id", &ValueType::guildId);
	};
}

namespace DiscordCoreAPI {

	void ApplicationCommands::initialize(DiscordCoreInternal::HttpsClient* client) {
		ApplicationCommands::httpsClient = client;
	}

	CoRoutine<jsonifier::vector<ApplicationCommandData>> ApplicationCommands::getGlobalApplicationCommandsAsync(GetGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Commands };
		co_await NewThreadAwaitable<jsonifier::vector<ApplicationCommandData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGlobalApplicationCommandsAsync()";
		jsonifier::vector<ApplicationCommandData> returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommandData> ApplicationCommands::createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommandData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		dataPackage.generateExcludedKeys();
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		parser.serializeJson<true>(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::createGlobalApplicationCommandAsync()";
		ApplicationCommandData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommandData> ApplicationCommands::getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommandData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands/" + dataPackage.commandId;
		workload.callStack	   = "ApplicationCommands::getGlobalApplicationCommandAsync()";
		ApplicationCommandData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommandData> ApplicationCommands::editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommandData>();
		jsonifier::vector<ApplicationCommandData> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		bool isItFound{};
		std::string appCommandId{};
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id;
				isItFound	 = true;
			}
		}
		if (!isItFound) {
			co_return ApplicationCommandData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands/" + appCommandId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::editGlobalApplicationCommandAsync()";
		ApplicationCommandData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> ApplicationCommands::deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Global_Application_Command };
		co_await NewThreadAwaitable<void>();
		jsonifier::vector<ApplicationCommandData> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		std::string commandId{};
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
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands/" + commandId;
		workload.callStack	   = "ApplicationCommands::deleteGlobalApplicationCommandAsync()";
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<jsonifier::vector<ApplicationCommandData>> ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync(BulkOverwriteGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Global_Application_Commands };
		co_await NewThreadAwaitable<jsonifier::vector<ApplicationCommandData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/commands";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync()";
		jsonifier::vector<ApplicationCommandData> returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<jsonifier::vector<ApplicationCommandData>> ApplicationCommands::getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands };
		co_await NewThreadAwaitable<jsonifier::vector<ApplicationCommandData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandsAsync()";
		jsonifier::vector<ApplicationCommandData> returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommandData> ApplicationCommands::createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommandData>();
		dataPackage.applicationId = dataPackage.applicationId;
		workload.workloadClass	  = DiscordCoreInternal::HttpsWorkloadClass::Post;
		dataPackage.generateExcludedKeys();
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		parser.serializeJson<true>(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::createGuildApplicationCommandAsync()";
		ApplicationCommandData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommandData> ApplicationCommands::getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommandData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + std::to_string(dataPackage.commandId);
		workload.callStack	   = "ApplicationCommands::getGuildApplicationCommandAsync()";
		ApplicationCommandData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<ApplicationCommandData> ApplicationCommands::editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommandData>();
		jsonifier::vector<ApplicationCommandData> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		bool isItFound										  = false;
		std::string appCommandId;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id;
				isItFound	 = true;
			}
		}
		if (!isItFound) {
			co_return ApplicationCommandData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + appCommandId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandAsync()";
		ApplicationCommandData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> ApplicationCommands::deleteGuildApplicationCommandAsync(DeleteGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Application_Command };
		co_await NewThreadAwaitable<void>();
		jsonifier::vector<ApplicationCommandData> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
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
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId;
		workload.callStack	   = "ApplicationCommands::deleteGuildApplicationCommandAsync()";
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<jsonifier::vector<ApplicationCommandData>> ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Guild_Application_Commands };
		co_await NewThreadAwaitable<jsonifier::vector<ApplicationCommandData>>();
		parser.serializeJson(dataPackage, workload.content);
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		workload.callStack	   = "ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync()";
		jsonifier::vector<ApplicationCommandData> returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<jsonifier::vector<GuildApplicationCommandPermissionsData>> ApplicationCommands::getGuildApplicationCommandPermissionsAsync(
		GetGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands_Permissions };
		co_await NewThreadAwaitable<jsonifier::vector<GuildApplicationCommandPermissionsData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/permissions";
		workload.callStack	   = "ApplicationCommands::getGuildApplicationCommandPermissionsAsync()";
		jsonifier::vector<GuildApplicationCommandPermissionsData> returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::getApplicationCommandPermissionsAsync(GetApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command_Permissions };
		co_await NewThreadAwaitable<GuildApplicationCommandPermissionsData>();
		jsonifier::vector<ApplicationCommandData> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		workload.callStack	   = "ApplicationCommands::getApplicationCommandPermissionsAsync()";
		GuildApplicationCommandPermissionsData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::editGuildApplicationCommandPermissionsAsync(EditGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Application_Command_Permissions };
		co_await NewThreadAwaitable<GuildApplicationCommandPermissionsData>();
		jsonifier::vector<ApplicationCommandData> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath  = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandPermissionsAsync()";
		GuildApplicationCommandPermissionsData returnData{};
		ApplicationCommands::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	DiscordCoreInternal::HttpsClient* ApplicationCommands::httpsClient{};
}
