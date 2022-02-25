/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// ApplicationCommandEntities.cpp - Source file for the ApplicationCommand classes and structs.
/// Aug 25, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file ApplicationCommandEntities.cpp

#include <ApplicationCommandEntities.hpp>
#include <DiscordCoreClient.hpp>
#include <UserEntities.hpp>
#include <FoundationEntities.hpp>
#include <CoRoutine.hpp>
#include <Http.hpp>
#include <JSONIfier.hpp>

namespace DiscordCoreAPI {

    ApplicationCommand::ApplicationCommand(ApplicationCommandData dataNew) {
        this->defaultPermission = dataNew.defaultPermission;
        this->description = dataNew.description;
        this->options = dataNew.options;
        this->name = dataNew.name;
        this->id = dataNew.id;
    }

    void ApplicationCommands::initialize(DiscordCoreInternal::HttpClient*theClient) {
        ApplicationCommands::httpClient = theClient;
    }

    CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGlobalApplicationCommandsAsync(GetGlobalApplicationCommandsData  dataPackage) {
        try {
            co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Global_Application_Commands;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
            workload.callStack = "ApplicationCommands::getGlobalApplicationCommandsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::getGlobalApplicationCommandsAsync()");
        }
    }    

    CoRoutine<ApplicationCommand> ApplicationCommands::createGlobalApplicationCommandAsync(CreateGlobalApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<ApplicationCommand>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Global_Application_Command;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "ApplicationCommands::createGlobalApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<ApplicationCommand>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::createGlobalApplicationCommandAsync()");
        }
    }
    
    CoRoutine<ApplicationCommand> ApplicationCommands::getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<ApplicationCommand>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Global_Application_Command;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + dataPackage.commandId;
            workload.callStack = "ApplicationCommands::getGlobalApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<ApplicationCommand>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::getGlobalApplicationCommandAsync()");
        }
    }

    CoRoutine<ApplicationCommand> ApplicationCommands::editGlobalApplicationCommandAsync(EditGlobalApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<ApplicationCommand>();
            std::vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
            bool isItFound{ false };
            std::string appCommandId{};
            for (auto& value : appCommands) {
                if (value.name == dataPackage.name) {
                    appCommandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                co_return ApplicationCommand();
            }
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Global_Application_Command;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + appCommandId;
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "ApplicationCommands::editGlobalApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<ApplicationCommand>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::editGlobalApplicationCommandAsync()");
        }
    }

    CoRoutine<void> ApplicationCommands::deleteGlobalApplicationCommandAsync(DeleteGlobalApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<void>();
            std::vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync({ .applicationId = dataPackage.applicationId }).get();
            std::string commandId{};
            bool isItFound = false;
            for (auto& value : appCommands) {
                if (value.name == dataPackage.name) {
                    commandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                co_return;
            }
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Global_Application_Command;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands/" + commandId;
            workload.callStack = "ApplicationCommands::deleteGlobalApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::deleteGlobalApplicationCommandAsync()");
        }
    }

    CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync(BulkOverwriteGlobalApplicationCommandsData dataPackage) {
        try {
            co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
            auto newDataArray = nlohmann::json::array();
            std::vector<CreateGlobalApplicationCommandData> newVector{};
            for (auto& value : dataPackage.responseData) {
                CreateGlobalApplicationCommandData dataPackageNew;
                dataPackageNew.applicationId = dataPackage.applicationId;
                dataPackageNew.defaultPermission = value.defaultPermission;
                dataPackageNew.description = value.description;
                dataPackageNew.name = value.name;
                for (auto& value01 : value.options) {
                    dataPackageNew.options.push_back(value01);
                }
                dataPackageNew.type = (ApplicationCommandType)value.type;
                newVector.push_back(dataPackageNew);
            }
            for (auto& value : newVector) {
                nlohmann::json newData = nlohmann::json::parse(DiscordCoreInternal::JSONIFY(value));
                newDataArray.push_back(newData);
            }
            nlohmann::json dataNew = newDataArray;
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Bulk_Put_Global_Application_Commands;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/commands";
            workload.content = dataNew.dump();
            workload.callStack = "ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync()");
        }
    }

    CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage) {
        try {
            co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Application_Commands;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
            workload.callStack = "ApplicationCommands::getGuildApplicationCommandsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::getGuildApplicationCommandsAsync()");
        }
    }

    CoRoutine<ApplicationCommand> ApplicationCommands::createGuildApplicationCommandAsync(CreateGuildApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<ApplicationCommand>();
            dataPackage.applicationId = dataPackage.applicationId;
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Application_Command;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "ApplicationCommands::createGuildApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<ApplicationCommand>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::createGuildApplicationCommandAsync()");
        }
    }

    CoRoutine<ApplicationCommand> ApplicationCommands::getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<ApplicationCommand>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Application_Command;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + dataPackage.commandId;
            workload.callStack = "ApplicationCommands::getGuildApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<ApplicationCommand>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::getGuildApplicationCommandAsync()");
        }
    }

    CoRoutine<ApplicationCommand> ApplicationCommands::editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<ApplicationCommand>();
            std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
            bool isItFound = false;
            std::string appCommandId;
            for (auto& value : appCommands) {
                if (value.name == dataPackage.name) {
                    appCommandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                co_return ApplicationCommand();
            }
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + appCommandId;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Application_Command;
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "ApplicationCommands::editGuildApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<ApplicationCommand>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::editGuildApplicationCommandAsync()");
        }
    }
    
    CoRoutine<void> ApplicationCommands::deleteGuildApplicationCommandAsync(DeleteGuildApplicationCommandData dataPackage) {
        try {
            co_await NewThreadAwaitable<void>();
            std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
            std::string commandId;
            bool isItFound = false;
            for (auto& value : appCommands) {
                if (value.name == dataPackage.name) {
                    commandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                co_return;
            }
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Application_Command;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId;
            workload.callStack = "ApplicationCommands::deleteGuildApplicationCommandAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::deleteGuildApplicationCommandAsync()");
        }
    }

    CoRoutine<std::vector<ApplicationCommand>> ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage) {
        try {
            co_await NewThreadAwaitable<std::vector<ApplicationCommand>>();
            auto newDataArray = nlohmann::json::array();
            std::vector<CreateGuildApplicationCommandData> newVector{};
            for (auto& value : dataPackage.responseData) {
                CreateGuildApplicationCommandData dataPackageNew;
                dataPackageNew.applicationId = dataPackage.applicationId;
                dataPackageNew.defaultPermission = value.defaultPermission;
                dataPackageNew.description = value.description;
                dataPackageNew.name = value.name;
                for (auto& value01 : value.options) {
                    dataPackageNew.options.push_back(value01);
                }
                dataPackageNew.type = (ApplicationCommandType)value.type;
                newVector.push_back(dataPackageNew);
            }
            for (auto& value : newVector) {
                nlohmann::json newData = nlohmann::json::parse(DiscordCoreInternal::JSONIFY(value));
                newDataArray.push_back(newData);
            }
            nlohmann::json dataNew = newDataArray;
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Bulk_Put_Guild_Application_Commands;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands";
            workload.content = dataNew.dump();
            workload.callStack = "ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<ApplicationCommand>>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync()");
        }
    }

    CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> ApplicationCommands::getGuildApplicationCommandPermissionsAsync(GetGuildApplicationCommandPermissionsData dataPackage){
        try {
            co_await NewThreadAwaitable<std::vector<GuildApplicationCommandPermissionsData>>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Application_Commands_Permissions;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/permissions";
            workload.callStack = "ApplicationCommands::getGuildApplicationCommandPermissionsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildApplicationCommandPermissionsData>>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::getGuildApplicationCommandPermissionsAsync()");
        }
    }
    
    CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::getApplicationCommandPermissionsAsync(GetApplicationCommandPermissionsData dataPackage) {
        try {
            co_await NewThreadAwaitable<GuildApplicationCommandPermissionsData>();
            std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
            std::string commandId;
            bool isItFound = false;
            for (auto& value : appCommands) {
                if (value.name == dataPackage.commandName) {
                    commandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                co_return GuildApplicationCommandPermissionsData();
            }
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Application_Command_Permissions;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
            workload.callStack = "ApplicationCommands::getApplicationCommandPermissionsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildApplicationCommandPermissionsData>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::getApplicationCommandPermissionsAsync()");
        }
    }

    CoRoutine<GuildApplicationCommandPermissionsData> ApplicationCommands::editGuildApplicationCommandPermissionsAsync(EditGuildApplicationCommandPermissionsData dataPackage) {
        try {
            co_await NewThreadAwaitable<GuildApplicationCommandPermissionsData>();
            std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
            std::string commandId;
            bool isItFound = false;
            for (auto& value : appCommands) {
                if (value.name == dataPackage.commandName) {
                    commandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                co_return GuildApplicationCommandPermissionsData();
            }
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Guild_Application_Command_Permissions;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/" + commandId + "/permissions";
            nlohmann::json newData = { {"permissions",nlohmann::json::parse(DiscordCoreInternal::JSONIFY(dataPackage))} };
            workload.content = newData.dump();
            workload.callStack = "ApplicationCommands::editGuildApplicationCommandPermissionsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<GuildApplicationCommandPermissionsData>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::editGuildApplicationCommandPermissionsAsync()");
        }
    }
    
    CoRoutine<std::vector<GuildApplicationCommandPermissionsData>> ApplicationCommands::batchEditGuildApplicationCommandPermissionsAsync(BatchEditGuildApplicationCommandPermissionsData dataPackage) {
        try {
            co_await NewThreadAwaitable<std::vector<GuildApplicationCommandPermissionsData>>();
            std::vector<ApplicationCommand> appCommands = getGuildApplicationCommandsAsync({ .guildId = dataPackage.guildId }).get();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Batch_Put_Guild_Application_Command_Permissions;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
            workload.relativePath = "/applications/" + dataPackage.applicationId + "/guilds/" + dataPackage.guildId + "/commands/permissions";
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "ApplicationCommands::batchEditGuildApplicationCommandPermissionsAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<GuildApplicationCommandPermissionsData>>(*ApplicationCommands::httpClient, workload);
        }
        catch (...) {
            reportException("ApplicationCommands::batchEditGuildApplicationCommandPermissionsAsync()");
        }
    }
    DiscordCoreInternal::HttpClient* ApplicationCommands::httpClient{nullptr};
}
