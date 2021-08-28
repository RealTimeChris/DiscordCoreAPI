// ApplicationCommandStuff.hpp - Header for the "application command" stuff.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _APPLICATION_COMMAND_STUFF_
#define _APPLICATION_COMMAND_STUFF_

#include "../pch.h"
#include "DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

    struct EditApplicationCommandData {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ false };
        string description{ "" };
        string name{ "" };
    };

    struct CreateApplicationCommandData {
        ApplicationCommandType type{};
        string name{ "" };
        string description{ "" };
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
    };

    struct GetGlobalApplicationCommandData {
        string commandId{ "" };
    };

    struct DeleteApplicationCommandData {
        string name{ "" };
    };

    class ApplicationCommand {
    public:
        friend class ApplicationCommandManager;
        ApplicationCommandData data{};

        ApplicationCommand() {};

    protected:

        ApplicationCommand(ApplicationCommandData dataNew) {
            this->data = dataNew;
        }
    };

    class ApplicationCommandManager {
    public:

        ApplicationCommandManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, string applicationIdNew) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
            this->applicationId = applicationIdNew;
        }

        task<ApplicationCommand> getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_APPLICATION_COMMAND;
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + dataPackage.commandId;
            DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::getGlobalApplicationCommandAsync");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            ApplicationCommandData appCommandData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_await mainThread;
            co_return appCommand;
        }

        task<vector<ApplicationCommand>> getGlobalApplicationCommandsAsync() {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.relativePath = "/applications/" + this->applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_APPLICATION_COMMANDS;
            DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::getGlobalApplicationCommands");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandsAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandsAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            vector<ApplicationCommand> appCommands;
            for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
                ApplicationCommandData appCommandData;
                DiscordCoreInternal::DataParser::parseObject(returnData.data.at(x), &appCommandData);
                ApplicationCommand appCommand(appCommandData);
                appCommands.push_back(appCommand);
            }
            co_await mainThread;
            co_return appCommands;
        }

        task<ApplicationCommand> createGlobalApplicationCommandAsync(CreateApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PostApplicationCommandData dataPackageNew;
            dataPackageNew.applicationId = this->applicationId;
            dataPackageNew.defaultPermission = dataPackage.defaultPermission;
            dataPackageNew.description = dataPackage.description;
            dataPackageNew.name = dataPackage.name;
            dataPackageNew.type = (DiscordCoreInternal::ApplicationCommandType)dataPackage.type;
            copyOptionsData(&dataPackageNew.options, dataPackage.options);
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.relativePath = "/applications/" + this->applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_APPLICATION_COMMAND;
            workload.content = DiscordCoreInternal::getCreateApplicationCommandPayload(dataPackageNew);
            DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::createGlobalApplicationCommandAsync");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::createGlobalApplicationCommandAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::createGlobalApplicationCommandAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            ApplicationCommandData appCommandData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_await mainThread;
            co_return appCommandData;
        }

        task<ApplicationCommand> editGlobalApplicationCommandAsync(EditApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync().get();
            bool isItFound = false;
            string appCommandId;
            for (auto const& [value] : appCommands) {
                cout << value.name << endl;
                if (value.name == dataPackage.name) {
                    appCommandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                cout << "ApplicationCommandManager::editGlobalApplicationCommand_00 Error: Sorry, it could not be found!" << endl;
                ApplicationCommand appCommand;
                co_await mainThread;
                co_return appCommand;
            }
            DiscordCoreInternal::PatchApplicationCommandData dataPackageNew;
            dataPackageNew.defaultPermission = dataPackage.defaultPermission;
            dataPackageNew.description = dataPackage.description;
            dataPackageNew.name = dataPackage.name;
            copyOptionsData(&dataPackageNew.options, dataPackage.options);
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + appCommandId;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_SLASH_COMMAND;
            workload.content = DiscordCoreInternal::getEditApplicationCommandPayload(dataPackageNew);
            DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::editGlobalApplicationCommandAsync");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::editGlobalApplicationCommandsAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::editGlobalApplicationCommandsAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            ApplicationCommandData appCommandData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_await mainThread;
            co_return appCommand;
        }

        task<void> deleteGlobalApplicationCommandAsync(DeleteApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            vector<ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync().get();
            string commandId;
            bool isItFound = false;
            for (auto const& [value] : appCommands) {
                if (value.name == dataPackage.name) {
                    commandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                cout << "ApplicationCommandManager::deleteGlobalApplicationCommand_00 Error: Sorry, it could not be found!" << endl;
                co_await mainThread;
                co_return;
            }
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + commandId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_SLASH_COMMAND;
            DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::deleteGlobalApplicationCommandAsync");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::deleteGlobalApplicationCommandAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::deleteGlobalApplicationCommandAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            co_await mainThread;
            co_return;
        }

        task<void> displayGlobalApplicationCommandsAsync() {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            vector<ApplicationCommand> applicationCommands = getGlobalApplicationCommandsAsync().get();
            for (unsigned int x = 0; x < applicationCommands.size(); x += 1) {
                cout << "Command Name: " << applicationCommands.at(x).data.name << endl;
                cout << "Command Description: " << applicationCommands.at(x).data.description << endl;
                displayOptions(applicationCommands.at(x).data.options);
            }
            co_await mainThread;
            co_return;
        }

        ~ApplicationCommandManager() {
            this->threadContext->releaseGroup();
        }

    protected:
        DiscordCoreInternal::HttpAgentResources agentResources{};
        shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
        string applicationId{ "" };

        void displayOptions(vector<DiscordCoreAPI::ApplicationCommandOptionData> applicationCommandOptionData) {
            for (unsigned int x = 0; x < applicationCommandOptionData.size(); x += 1) {
                string indentAmount;
                if (applicationCommandOptionData.at(x).type == ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                    indentAmount += "   ";
                }
                if (applicationCommandOptionData.at(x).type == ApplicationCommandOptionType::SUB_COMMAND) {
                    indentAmount += "      ";
                }
                cout << indentAmount + "Command Option Name: " << applicationCommandOptionData.at(x).name << endl;
                cout << indentAmount + "Command Option Description: " << applicationCommandOptionData.at(x).description << endl;
                if (applicationCommandOptionData.at(x).choices.size() > 0) {
                    for (unsigned int y = 0; y < applicationCommandOptionData.at(x).choices.size(); y += 1) {
                        cout << "Command Option Choice #" << y << " Name: " << applicationCommandOptionData.at(x).choices.at(y).name << endl;
                        if (applicationCommandOptionData.at(x).choices.at(y).valueString != "") {
                            cout << "         " << "Command Option Choice #" << y << " Value: " << applicationCommandOptionData.at(x).choices.at(y).valueString << endl;
                        }
                        else {
                            cout << "         " << "Command Option Choice #" << y << " Value: " << applicationCommandOptionData.at(x).choices.at(y).valueInt << endl;
                        }
                    }
                }
                if (applicationCommandOptionData.at(x).type == ApplicationCommandOptionType::SUB_COMMAND || applicationCommandOptionData.at(x).type == ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                    displayOptions(applicationCommandOptionData.at(x).options);
                }
            }
        }

        void copyOptionsData(vector<DiscordCoreInternal::ApplicationCommandOptionData>* appCommandOptionData, vector<ApplicationCommandOptionData> appCommandOptionDataSource) {

            if (appCommandOptionDataSource.size() > 0) {
                for (unsigned int x = 0; x < appCommandOptionDataSource.size(); x += 1) {
                    DiscordCoreInternal::ApplicationCommandOptionData appCommandOptionDataNew;
                    appCommandOptionDataNew.description = appCommandOptionDataSource.at(x).description;
                    appCommandOptionDataNew.name = appCommandOptionDataSource.at(x).name;
                    appCommandOptionDataNew.required = appCommandOptionDataSource.at(x).required;
                    appCommandOptionDataNew.type = (DiscordCoreInternal::ApplicationCommandOptionType)appCommandOptionDataSource.at(x).type;
                    if (appCommandOptionDataSource.at(x).choices.size() > 0) {
                        for (unsigned int y = 0; y < appCommandOptionDataSource.at(x).choices.size(); y += 1) {
                            DiscordCoreInternal::ApplicationCommandOptionChoiceData choiceData;
                            choiceData.name = appCommandOptionDataSource.at(x).choices.at(y).name;
                            choiceData.valueInt = appCommandOptionDataSource.at(x).choices.at(y).valueInt;
                            choiceData.valueString = appCommandOptionDataSource.at(x).choices.at(y).valueString;
                            appCommandOptionDataNew.choices.push_back(choiceData);
                        }
                    }
                    if (appCommandOptionDataNew.type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND || appCommandOptionDataNew.type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                        copyOptionsData(&appCommandOptionDataNew.options, appCommandOptionDataSource.at(x).options);
                    }
                    appCommandOptionData->push_back(appCommandOptionDataNew);

                }

            }

        }

    };

}

#endif