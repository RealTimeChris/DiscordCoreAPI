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

	ApplicationCommand::ApplicationCommand(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->defaultMemberPermissions = getString(jsonObjectData, "default_member_permissions");

		this->dmPermission = getBool(jsonObjectData, "dm_permission");

		this->version = getString(jsonObjectData, "version");

		this->type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		simdjson::ondemand::object theMap{};
		auto theResult = jsonObjectData["name_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: theMap) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}
		
		theResult = jsonObjectData["description_localizations"].get(theMap);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->descriptionLocalizations.clear();
			for (auto value: theMap) {
				this->descriptionLocalizations.emplace(value.unescaped_key().take_value().data(), value.value().get_string().take_value().data());
			}
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		this->name = getString(jsonObjectData, "name");

		this->description = getString(jsonObjectData, "description");

		this->version = getString(jsonObjectData, "version");

		simdjson::ondemand::array theArray{};
		theResult = jsonObjectData["options"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->options.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ApplicationCommandOptionData theDataNew{ value.value() };
				this->options.emplace_back(std::move(theDataNew));
			}
		}
	}

	ApplicationCommandVector::ApplicationCommandVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			auto theResult = jsonObjectData.get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ApplicationCommand newData{ value.value() };
					this->theApplicationCommands.emplace_back(std::move(newData));
				}
			}
		}
	}

	CreateGlobalApplicationCommandData::operator JsonObject() {
		JsonObject theData{};
		if (this->defaultMemberPermissions != 0) {
			theData["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		theData["description_localizations"] = this->descriptionLocalizations;
		theData["name_localizations"] = this->nameLocalizations;
		theData["dm_permission"] = this->dmPermission;
		theData["description"] = this->description;
		theData["name"] = this->name;
		theData["type"] = this->type;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				theData["options"].pushBack(this->options[x]);
			}
		}
		return theData;
	}

	EditGlobalApplicationCommandData::operator JsonObject() {
		JsonObject theData{};
		if (this->defaultMemberPermissions != 0) {
			theData["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		theData["description_localizations"] = this->descriptionLocalizations;
		theData["name_localizations"] = this->nameLocalizations;
		theData["dm_permission"] = this->dmPermission;
		theData["description"] = this->description;
		theData["name"] = this->name;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				theData["options"].pushBack(this->options[x]);
			}
		}
		return theData;
	}

	CreateGuildApplicationCommandData::operator JsonObject() {
		JsonObject theData{};
		if (this->defaultMemberPermissions != 0) {
			theData["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		theData["description_localizations"] = this->descriptionLocalizations;
		theData["name_localizations"] = this->nameLocalizations;
		theData["dm_permission"] = this->dmPermission;
		theData["description"] = this->description;
		theData["name"] = this->name;
		theData["type"] = this->type;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				theData["options"].pushBack(this->options[x]);
			}
		}
		return theData;
	}

	EditGuildApplicationCommandData::operator JsonObject() {
		JsonObject theData{};
		if (this->defaultMemberPermissions != 0) {
			theData["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		theData["description_localizations"] = this->descriptionLocalizations;
		theData["name_localizations"] = this->nameLocalizations;
		theData["dm_permission"] = this->dmPermission;
		theData["description"] = this->description;
		theData["name"] = this->name;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				theData["options"].pushBack(this->options[x]);
			}
		}
		return theData;
	}

	EditGuildApplicationCommandPermissionsData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->permissions) {
			JsonObject newData{};
			newData["permission"] = value.permission;
			newData["type"] = static_cast<uint8_t>(value.type);
			newData["id"] = std::to_string(value.id);
			theData["permissions"].pushBack(newData);
		}
		return theData;
	}

	ApplicationCommandVector::operator std::vector<ApplicationCommand>() {
		return this->theApplicationCommands;
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
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands";
		workload.content = dataPackage.operator JsonObject();
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
				appCommandId = std::to_string(value.id);
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands/" + appCommandId;
		workload.content = dataPackage.operator JsonObject();
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
				commandId = std::to_string(value.id);
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
		std::string newDataArray{};
		std::vector<CreateGlobalApplicationCommandData> newVector{};
		for (auto& value: dataPackage.responseData) {
			CreateGlobalApplicationCommandData dataPackageNew;
			dataPackageNew.applicationId = dataPackage.applicationId;
			dataPackageNew.description = value.description;
			dataPackageNew.name = value.name;
			for (auto& value01: value.options) {
				dataPackageNew.options.emplace_back(value01);
			}
			dataPackageNew.type = static_cast<ApplicationCommandType>(value.type);
			newVector.emplace_back(dataPackageNew);
		}
		for (auto& value: newVector) {
			std::string newData = value.operator DiscordCoreAPI::JsonObject();
			newDataArray += newData;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/commands";
		workload.content = newDataArray;
		workload.callStack = "ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload);
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
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		dataPackage.applicationId = dataPackage.applicationId;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands";
		workload.content = dataPackage.operator JsonObject();
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
				appCommandId = std::to_string(value.id);
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/" + appCommandId;
		workload.content = dataPackage.operator JsonObject();
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
				commandId = std::to_string(value.id);
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
		std::string newDataArray{};
		std::vector<CreateGuildApplicationCommandData> newVector{};
		for (auto& value: dataPackage.responseData) {
			CreateGuildApplicationCommandData dataPackageNew;
			dataPackageNew.applicationId = dataPackage.applicationId;
			dataPackageNew.description = value.description;
			dataPackageNew.name = value.name;
			for (auto& value01: value.options) {
				dataPackageNew.options.emplace_back(value01);
			}
			dataPackageNew.type = static_cast<ApplicationCommandType>(value.type);
			newVector.emplace_back(dataPackageNew);
		}
		for (auto& value: newVector) {
			std::string newData = value.operator DiscordCoreAPI::JsonObject();
			newDataArray += newData;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands";
		workload.content = newDataArray;
		workload.callStack = "ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload);
	}

	CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> ApplicationCommands::getGuildApplicationCommandPermissionsAsync(
		GetGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands_Permissions };
		co_await NewThreadAwaitable<std::vector<GuildApplicationCommandPermissionsData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/permissions";
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandPermissionsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsDataVector>(workload);
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::getApplicationCommandPermissionsAsync(GetApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command_Permissions };
		co_await NewThreadAwaitable<GuildApplicationCommandPermissionsData>();
		std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
		std::string commandId;
		bool isItFound = false;
		for (auto& value: appCommands) {
			if (value.name == dataPackage.commandName) {
				commandId = std::to_string(value.id);
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
				commandId = std::to_string(value.id);
				isItFound = true;
			}
		}
		if (isItFound == false) {
			co_return GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath =
			"/applications/" + std::to_string(dataPackage.applicationId) + "/guilds/" + std::to_string(dataPackage.guildId) + "/commands/" + commandId + "/permissions";
		std::string newData{ dataPackage.operator DiscordCoreAPI::JsonObject() };
		workload.content = newData;
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandPermissionsAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsData>(workload);
	}

	DiscordCoreInternal::HttpsClient* ApplicationCommands::httpsClient{ nullptr };
}
