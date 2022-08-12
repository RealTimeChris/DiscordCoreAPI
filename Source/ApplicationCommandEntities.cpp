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
/// ApplicationCommandEntities.cpp - Source file for the ApplicationCommand classes and structs.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file ApplicationCommandEntities.cpp

#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	CreateGlobalApplicationCommandData::operator std::string() {
		nlohmann::json data{};
		if (this->defaultMemberPermissions != "") {
			data["default_member_permissions"] = this->defaultMemberPermissions;
		}
		data["description_localizations"] = this->descriptionLocalizations;
		data["name_localizations"] = this->nameLocalizations;
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		data["type"] = this->type;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].push_back(nlohmann::json(this->options[x]));
			}
		}
		return data.dump();
	}

	EditGlobalApplicationCommandData::operator std::string() {
		nlohmann::json data{};
		if (this->defaultMemberPermissions != "") {
			data["default_member_permissions"] = this->defaultMemberPermissions;
		}
		data["description_localizations"] = this->descriptionLocalizations;
		data["name_localizations"] = this->nameLocalizations;
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].push_back(nlohmann::json{ this->options[x] });
			}
		}
		return data.dump();
	}

	CreateGuildApplicationCommandData::operator std::string() {
		nlohmann::json data{};
		if (this->defaultMemberPermissions != "") {
			data["default_member_permissions"] = this->defaultMemberPermissions;
		}
		data["description_localizations"] = this->descriptionLocalizations;
		data["name_localizations"] = this->nameLocalizations;
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		data["type"] = this->type;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].push_back(nlohmann::json{ this->options[x] });
			}
			data["options"] = nlohmann::json{};
		}
		return data.dump();
	}

	EditGuildApplicationCommandData::operator std::string() {
		nlohmann::json data{};
		if (this->defaultMemberPermissions != "") {
			data["default_member_permissions"] = this->defaultMemberPermissions;
		}
		data["description_localizations"] = this->descriptionLocalizations;
		data["name_localizations"] = this->nameLocalizations;
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].push_back(nlohmann::json{ this->options[x] });
			}
			data["options"] = nlohmann::json{};
		}
		return data.dump();
	}

	EditGuildApplicationCommandPermissionsData::operator std::string() {
		nlohmann::json newDataArray{};
		for (auto& value: this->permissions) {
			nlohmann::json newData{};
			newData["permission"] = value.permission;
			newData["type"] = value.type;
			newData["id"] = std::to_string(value.id);
			newDataArray.push_back(newData);
		}
		return newDataArray.dump();
	}

	ApplicationCommand& ApplicationCommand::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommand::ApplicationCommand(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	void ApplicationCommands::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		ApplicationCommands::httpsClient = theClient;
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGlobalApplicationCommandsAsync(GetGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGlobalApplicationCommandsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands";
		workload.content = dataPackage;
		workload.callStack = "ApplicationCommands::createGlobalApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands/" + dataPackage.commandId;
		workload.callStack = "ApplicationCommands::getGlobalApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		std::vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		bool isItFound{ false };
		std::string appCommandId{};
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id;
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands/" + appCommandId;
		workload.content = dataPackage;
		workload.callStack = "ApplicationCommands::editGlobalApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload);
	}

	CoRoutine<void> ApplicationCommands::deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Global_Application_Command };
		co_await NewThreadAwaitable<void>();
		std::vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		std::string commandId{};
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands/" + commandId;
		workload.callStack = "ApplicationCommands::deleteGlobalApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync(BulkOverwriteGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Global_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		auto newDataArray = nlohmann::json::array();
		std::vector<CreateGlobalApplicationCommandData> newVector{};
		for (auto& value: dataPackage.responseData) {
			CreateGlobalApplicationCommandData dataPackageNew;
			dataPackageNew.applicationId = dataPackage.applicationId;
			dataPackageNew.description = value.description;
			dataPackageNew.name = value.name;
			for (auto& value01: value.options) {
				dataPackageNew.options.push_back(value01);
			}
			dataPackageNew.type = static_cast<ApplicationCommandType>(value.type);
			newVector.push_back(dataPackageNew);
		}
		for (auto& value: newVector) {
			std::string newData = value;
			newDataArray.push_back(std::string{ newData });
		}
		nlohmann::json dataNew = newDataArray;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands";
		workload.content = dataNew.dump();
		workload.callStack = "ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(workload);
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		dataPackage.applicationId = dataPackage.applicationId;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands";
		workload.content = dataPackage;
		workload.callStack = "ApplicationCommands::createGuildApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath =
			"/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/" + std::to_string(dataPackage.commandId);
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		bool isItFound = false;
		std::string appCommandId;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id;
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/" + appCommandId;
		workload.content = dataPackage;
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload);
	}

	CoRoutine<void> ApplicationCommands::deleteGuildApplicationCommandAsync(DeleteGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Guild_Application_Command };
		co_await NewThreadAwaitable<void>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/" + commandId;
		workload.callStack = "ApplicationCommands::deleteGuildApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Guild_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		auto newDataArray = nlohmann::json::array();
		std::vector<CreateGuildApplicationCommandData> newVector{};
		for (auto& value: dataPackage.responseData) {
			CreateGuildApplicationCommandData dataPackageNew;
			dataPackageNew.applicationId = dataPackage.applicationId;
			dataPackageNew.description = value.description;
			dataPackageNew.name = value.name;
			for (auto& value01: value.options) {
				dataPackageNew.options.push_back(value01);
			}
			dataPackageNew.type = static_cast<ApplicationCommandType>(value.type);
			newVector.push_back(dataPackageNew);
		}
		for (auto& value: newVector) {
			std::string newData = value;
			newDataArray.push_back(newData);
		}
		nlohmann::json dataNew = newDataArray;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands";
		workload.content = dataNew.dump();
		workload.callStack = "ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(workload);
	}

	CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> ApplicationCommands::getGuildApplicationCommandPermissionsAsync(
		GetGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands_Permissions };
		co_await NewThreadAwaitable<std::vector<GuildApplicationCommandPermissionsData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/permissions";
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandPermissionsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<std::vector<GuildApplicationCommandPermissionsData>>(workload);
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::getApplicationCommandPermissionsAsync(GetApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command_Permissions };
		co_await NewThreadAwaitable<GuildApplicationCommandPermissionsData>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath =
			"/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/" + commandId + "/permissions";
		workload.callStack = "ApplicationCommands::getApplicationCommandPermissionsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsData>(workload);
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::editGuildApplicationCommandPermissionsAsync(EditGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Application_Command_Permissions };
		co_await NewThreadAwaitable<GuildApplicationCommandPermissionsData>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = value.id;
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath =
			"/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/" + commandId + "/permissions";
		nlohmann::json newData = { { "permissions", std::string{ dataPackage } } };
		workload.content = newData.dump();
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandPermissionsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsData>(workload);
	}

	DiscordCoreInternal::HttpsClient* ApplicationCommands::httpsClient{ nullptr };
}
