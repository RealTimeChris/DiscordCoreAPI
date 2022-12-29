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

	CreateGlobalApplicationCommandData::operator Jsonifier() {
		Jsonifier data{};
		if (this->defaultMemberPermissions != 0) {
			data["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		if (this->descriptionLocalizations.size() > 0) {
			data["description_localizations"] = this->descriptionLocalizations;
		}
		if (this->nameLocalizations.size() > 0) {
			data["name_localizations"] = this->nameLocalizations;
		}
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		data["type"] = this->type;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].emplaceBack(this->options[x].operator Jsonifier());
			}
		}
		return data;
	}

	EditGlobalApplicationCommandData::operator Jsonifier() {
		Jsonifier data{};
		if (this->defaultMemberPermissions != 0) {
			data["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		if (this->descriptionLocalizations.size() > 0) {
			data["description_localizations"] = this->descriptionLocalizations;
		}
		if (this->nameLocalizations.size() > 0) {
			data["name_localizations"] = this->nameLocalizations;
		}
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].emplaceBack(this->options[x].operator Jsonifier());
			}
		}
		return data;
	}

	CreateGuildApplicationCommandData::operator Jsonifier() {
		Jsonifier data{};
		if (this->defaultMemberPermissions != 0) {
			data["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		if (this->descriptionLocalizations.size() > 0) {
			data["description_localizations"] = this->descriptionLocalizations;
		}
		if (this->nameLocalizations.size() > 0) {
			data["name_localizations"] = this->nameLocalizations;
		}
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		data["type"] = this->type;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].emplaceBack(this->options[x].operator Jsonifier());
			}
		}
		return data;
	}

	EditGuildApplicationCommandData::operator Jsonifier() {
		Jsonifier data{};
		if (this->defaultMemberPermissions != 0) {
			data["default_member_permissions"] = static_cast<uint64_t>(this->defaultMemberPermissions);
		}
		if (this->descriptionLocalizations.size() > 0) {
			data["description_localizations"] = this->descriptionLocalizations;
		}
		if (this->nameLocalizations.size() > 0) {
			data["name_localizations"] = this->nameLocalizations;
		}
		data["dm_permission"] = this->dmPermission;
		data["description"] = this->description;
		data["name"] = this->name;
		if (this->options.size() > 0) {
			for (int32_t x = 0; x < this->options.size(); ++x) {
				data["options"].emplaceBack(this->options[x].operator Jsonifier());
			}
		}
		return data;
	}

	EditGuildApplicationCommandPermissionsData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->permissions) {
			Jsonifier newData{};
			newData["permission"] = value.permission;
			newData["type"] = static_cast<uint8_t>(value.type);
			newData["id"] = value.id;
			data["permissions"].emplaceBack(newData);
		}
		return data;
	}

	ApplicationCommand::ApplicationCommand(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->defaultMemberPermissions = getString(jsonObjectData, "default_member_permissions");

		this->dmPermission = getBool(jsonObjectData, "dm_permission");

		this->version = getString(jsonObjectData, "version");

		this->type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		simdjson::ondemand::object map{};
		if (jsonObjectData["name_localizations"].get(map) == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: map) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		if (jsonObjectData["description_localizations"].get(map) == simdjson::error_code::SUCCESS) {
			this->descriptionLocalizations.clear();
			for (auto value: map) {
				this->descriptionLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		this->name = getString(jsonObjectData, "name");

		this->description = getString(jsonObjectData, "description");

		this->version = getString(jsonObjectData, "version");

		simdjson::ondemand::array arrayValue{};
		if (jsonObjectData["options"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->options.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ApplicationCommandOptionData dataNew{ value.value() };
				this->options.emplace_back(std::move(dataNew));
			}
		}
	}

	ApplicationCommandVector::ApplicationCommandVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					ApplicationCommand newData{ value.value() };
					this->applicationCommands.emplace_back(std::move(newData));
				}
			}
		}
	}

	ApplicationCommandVector::operator std::vector<ApplicationCommand>() {
		return this->applicationCommands;
	}

	void ApplicationCommands::initialize(DiscordCoreInternal::HttpsClient* client) {
		ApplicationCommands::httpsClient = client;
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGlobalApplicationCommandsAsync(
		GetGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGlobalApplicationCommandsAsync()";
		ApplicationCommandVector returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, &returnValue);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "ApplicationCommands::createGlobalApplicationCommandAsync()";
		ApplicationCommand returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, &returnValue);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + dataPackage.commandId;
		workload.callStack = "ApplicationCommands::getGlobalApplicationCommandAsync()";
		ApplicationCommand returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, &returnValue);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Global_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		std::vector<ApplicationCommand> appCommands =
			getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
		bool isItFound{};
		std::string appCommandId{};
		for (auto& value: appCommands) {
			if (value.name == dataPackage.name) {
				appCommandId = value.id;
				isItFound = true;
			}
		}
		if (!isItFound) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + appCommandId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "ApplicationCommands::editGlobalApplicationCommandAsync()";
		ApplicationCommand returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, &returnValue);
	}

	CoRoutine<void> ApplicationCommands::deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Global_Application_Command };
		co_await NewThreadAwaitable<void>();
		std::vector<ApplicationCommand> appCommands =
			getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
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
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + commandId;
		workload.callStack = "ApplicationCommands::deleteGlobalApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync(
		BulkOverwriteGlobalApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Global_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		Jsonifier newDataArray{};
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
			newDataArray.emplaceBack(value);
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
		workload.content = newDataArray.operator std::string();
		workload.callStack = "ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync()";
		ApplicationCommandVector returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, &returnValue);
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGuildApplicationCommandsAsync(
		GetGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		if (dataPackage.withLocalizations) {
			workload.relativePath += "?with_localizations=true";
		}
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandsAsync()";
		ApplicationCommandVector returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, &returnValue);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		dataPackage.applicationId = dataPackage.applicationId;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "ApplicationCommands::createGuildApplicationCommandAsync()";
		ApplicationCommand returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, &returnValue);
	}

	CoRoutine<ApplicationCommand> ApplicationCommands::getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command };
		co_await NewThreadAwaitable<ApplicationCommand>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" +
			std::to_string(dataPackage.commandId);
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandAsync()";
		ApplicationCommand returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, &returnValue);
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
		if (!isItFound) {
			co_return ApplicationCommand();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath =
			"/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + appCommandId;
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandAsync()";
		ApplicationCommand returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommand>(workload, &returnValue);
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
		if (!isItFound) {
			co_return;
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId;
		workload.callStack = "ApplicationCommands::deleteGuildApplicationCommandAsync()";
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync(
		BulkOverwriteGuildApplicationCommandsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Bulk_Put_Guild_Application_Commands };
		co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
		Jsonifier newDataArray{};
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
			newDataArray.emplaceBack(value);
		}
		newDataArray.refreshString(JsonifierSerializeType::Json);
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
		workload.content = newDataArray.operator std::string();
		workload.callStack = "ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync()";
		ApplicationCommandVector returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<ApplicationCommandVector>(workload, &returnValue);
	}

	CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> ApplicationCommands::getGuildApplicationCommandPermissionsAsync(
		GetGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{
			DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Commands_Permissions
		};
		co_await NewThreadAwaitable<std::vector<GuildApplicationCommandPermissionsData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/permissions";
		workload.callStack = "ApplicationCommands::getGuildApplicationCommandPermissionsAsync()";
		GuildApplicationCommandPermissionsDataVector returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsDataVector>(workload,
			&returnValue);
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::getApplicationCommandPermissionsAsync(
		GetApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{
			DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Application_Command_Permissions
		};
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
		if (!isItFound) {
			co_return GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath =
			"/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		workload.callStack = "ApplicationCommands::getApplicationCommandPermissionsAsync()";
		GuildApplicationCommandPermissionsData returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsData>(workload,
			&returnValue);
	}

	CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::editGuildApplicationCommandPermissionsAsync(
		EditGuildApplicationCommandPermissionsData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{
			DiscordCoreInternal::HttpsWorkloadType::Put_Guild_Application_Command_Permissions
		};
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
		if (!isItFound) {
			co_return GuildApplicationCommandPermissionsData();
		}
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Put;
		workload.relativePath =
			"/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
		auto serializer = dataPackage.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		workload.content = serializer.operator std::string();
		workload.callStack = "ApplicationCommands::editGuildApplicationCommandPermissionsAsync()";
		GuildApplicationCommandPermissionsData returnValue{};
		co_return ApplicationCommands::httpsClient->submitWorkloadAndGetResult<GuildApplicationCommandPermissionsData>(workload,
			&returnValue);
	}

	DiscordCoreInternal::HttpsClient* ApplicationCommands::httpsClient{ nullptr };
}
