// ApplicationCommandStuff.hpp - Header for the "application command" stuff.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _APPLICATION_COMMAND_STUFF_
#define _APPLICATION_COMMAND_STUFF_

#include "../pch.h"
#include "DiscordCoreClient02.hpp"

namespace DiscordCoreInternal {

    class ApplicationCommandManager;

}

namespace DiscordCoreAPI {

    class ApplicationCommands;
    class Interactions;

    struct EditApplicationCommandData {
        vector<DiscordCoreAPI::ApplicationCommandOptionData> options{};
        bool defaultPermission{ false };
        string description{ "" };
        string name{ "" };
    };

    struct CreateApplicationCommandData {
        ApplicationCommandType type{};
        string name{ "" };
        string description{ "" };
        vector<DiscordCoreAPI::ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
    };

    struct GetGlobalApplicationCommandData {
        string commandId{ "" };
    };

    struct DeleteApplicationCommandData {
        string name{ "" };
    };

    class ApplicationCommand : public ApplicationCommandData {
    public:

        friend struct Concurrency::details::_ResultHolder<ApplicationCommand>;
        friend class DiscordCoreInternal::ApplicationCommandManager;

    protected:

        ApplicationCommand() {};

        ApplicationCommand(ApplicationCommandData dataNew) {
            this->applicationId = dataNew.applicationId;
            this->defaultPermission = dataNew.defaultPermission;
            this->description = dataNew.description;
            this->id = dataNew.id;
            this->name = dataNew.name;
            this->options = dataNew.options;
        }
    };
}

namespace DiscordCoreInternal{

    class ApplicationCommandManager {
    public:

        template <class _Ty>
        friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
        friend class DiscordCoreAPI::ApplicationCommands;
        friend class DiscordCoreAPI::DiscordCoreClient;
        friend class DiscordCoreAPI::SelectMenuManager;
        friend class DiscordCoreAPI::ButtonManager;
        friend class DiscordCoreAPI::Interactions;

        ApplicationCommandManager(ApplicationCommandManager* pointer) {
            if (pointer != nullptr) {
                *this = *pointer;
            }
        }

    protected:

        shared_ptr<ThreadContext> threadContext{ nullptr };
        HttpAgentResources agentResources{};
        string applicationId{ "" };

        ApplicationCommandManager() {};

        ApplicationCommandManager(HttpAgentResources agentResourcesNew, shared_ptr<ThreadContext> threadContextNew, string applicationIdNew) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
            this->applicationId = applicationIdNew;
        }

        ApplicationCommandManager(const ApplicationCommandManager& dataPackage) {
            this->agentResources = dataPackage.agentResources;
            this->threadContext = dataPackage.threadContext;
            return;
        }

        ApplicationCommandManager operator=(const ApplicationCommandManager& dataPackage) {
            ApplicationCommandManager pointerToManager{ dataPackage };
            return pointerToManager;
        }

        ApplicationCommandManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<ThreadContext> threadContextNew, string applicationIdNew) {
            this->threadContext = threadContextNew;
            this->agentResources = agentResourcesNew;
            this->applicationId = applicationIdNew;
            return *this;
        }

        task<DiscordCoreAPI::ApplicationCommand> getGlobalApplicationCommandAsync(DiscordCoreAPI::GetGlobalApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            HttpWorkload workload;
            workload.workloadClass = HttpWorkloadClass::GET;
            workload.workloadType = HttpWorkloadType::GET_APPLICATION_COMMAND;
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + dataPackage.commandId;
            HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::getGlobalApplicationCommandAsync");
            HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreAPI::ApplicationCommandData appCommandData;
            DataParser::parseObject(returnData.data, &appCommandData);
            DiscordCoreAPI::ApplicationCommand appCommand(appCommandData);
            co_await mainThread;
            co_return appCommand;
        }

        task<vector<DiscordCoreAPI::ApplicationCommand>> getGlobalApplicationCommandsAsync() {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            HttpWorkload workload;
            workload.workloadClass = HttpWorkloadClass::GET;
            workload.relativePath = "/applications/" + this->applicationId + "/commands";
            workload.workloadType = HttpWorkloadType::GET_APPLICATION_COMMANDS;
            HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::getGlobalApplicationCommands");
            HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandsAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::getGlobalApplicationCommandsAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            vector<DiscordCoreAPI::ApplicationCommand> appCommands;
            for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
                DiscordCoreAPI::ApplicationCommandData appCommandData;
                DataParser::parseObject(returnData.data.at(x), &appCommandData);
                DiscordCoreAPI::ApplicationCommand appCommand(appCommandData);
                appCommands.push_back(appCommand);
            }
            co_await mainThread;
            co_return appCommands;
        }

        task<DiscordCoreAPI::ApplicationCommand> createGlobalApplicationCommandAsync(DiscordCoreAPI::CreateApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            PostApplicationCommandData dataPackageNew;
            dataPackageNew.applicationId = this->applicationId;
            dataPackageNew.defaultPermission = dataPackage.defaultPermission;
            dataPackageNew.description = dataPackage.description;
            dataPackageNew.name = dataPackage.name;
            dataPackageNew.type = (ApplicationCommandType)dataPackage.type;
            copyOptionsData(&dataPackageNew.options, dataPackage.options);
            HttpWorkload workload;
            workload.workloadClass = HttpWorkloadClass::POST;
            workload.relativePath = "/applications/" + this->applicationId + "/commands";
            workload.workloadType = HttpWorkloadType::POST_APPLICATION_COMMAND;
            workload.content = getCreateApplicationCommandPayload(dataPackageNew);
            HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::createGlobalApplicationCommandAsync");
            HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::createGlobalApplicationCommandAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::createGlobalApplicationCommandAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreAPI::ApplicationCommandData appCommandData;
            DataParser::parseObject(returnData.data, &appCommandData);
            DiscordCoreAPI::ApplicationCommand appCommand(appCommandData);
            co_await mainThread;
            co_return appCommandData;
        }

        task<DiscordCoreAPI::ApplicationCommand> editGlobalApplicationCommandAsync(DiscordCoreAPI::EditApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            vector<DiscordCoreAPI::ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync().get();
            bool isItFound = false;
            string appCommandId;
            for (auto value : appCommands) {
                cout << value.name << endl;
                if (value.name == dataPackage.name) {
                    appCommandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                cout << "ApplicationCommandManager::editGlobalApplicationCommand_00 Error: Sorry, it could not be found!" << endl;
                DiscordCoreAPI::ApplicationCommand appCommand;
                co_await mainThread;
                co_return appCommand;
            }
            PatchApplicationCommandData dataPackageNew;
            dataPackageNew.defaultPermission = dataPackage.defaultPermission;
            dataPackageNew.description = dataPackage.description;
            dataPackageNew.name = dataPackage.name;
            copyOptionsData(&dataPackageNew.options, dataPackage.options);
            HttpWorkload workload;
            workload.workloadClass = HttpWorkloadClass::PATCH;
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + appCommandId;
            workload.workloadType = HttpWorkloadType::PATCH_SLASH_COMMAND;
            workload.content = getEditApplicationCommandPayload(dataPackageNew);
            HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::editGlobalApplicationCommandAsync");
            HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "ApplicationCommandManager::editGlobalApplicationCommandsAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "ApplicationCommandManager::editGlobalApplicationCommandsAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreAPI::ApplicationCommandData appCommandData;
            DataParser::parseObject(returnData.data, &appCommandData);
            DiscordCoreAPI::ApplicationCommand appCommand(appCommandData);
            co_await mainThread;
            co_return appCommand;
        }

        task<void> deleteGlobalApplicationCommandAsync(DiscordCoreAPI::DeleteApplicationCommandData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            vector<DiscordCoreAPI::ApplicationCommand> appCommands = getGlobalApplicationCommandsAsync().get();
            string commandId;
            bool isItFound = false;
            for (auto value : appCommands) {
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
            HttpWorkload workload;
            workload.relativePath = "/applications/" + this->applicationId + "/commands/" + commandId;
            workload.workloadClass = HttpWorkloadClass::DELETED;
            workload.workloadType = HttpWorkloadType::DELETE_SLASH_COMMAND;
            HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("ApplicationCommandManager::deleteGlobalApplicationCommandAsync");
            HttpData returnData;
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
            vector<DiscordCoreAPI::ApplicationCommand> applicationCommandsNew = getGlobalApplicationCommandsAsync().get();
            for (unsigned int x = 0; x < applicationCommandsNew.size(); x += 1) {
                cout << "Command Name: " << applicationCommandsNew.at(x).name << endl;
                cout << "Command Description: " << applicationCommandsNew.at(x).description << endl;
                displayOptions(applicationCommandsNew.at(x).options);
            }
            co_await mainThread;
            co_return;
        }

        void displayOptions(vector<DiscordCoreAPI::ApplicationCommandOptionData> applicationCommandOptionData) {
            for (unsigned int x = 0; x < applicationCommandOptionData.size(); x += 1) {
                string indentAmount;
                if (applicationCommandOptionData.at(x).type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                    indentAmount += "   ";
                }
                if (applicationCommandOptionData.at(x).type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND) {
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
                if (applicationCommandOptionData.at(x).type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND || applicationCommandOptionData.at(x).type == DiscordCoreAPI::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                    displayOptions(applicationCommandOptionData.at(x).options);
                }
            }
        }

        void copyOptionsData(vector<ApplicationCommandOptionData>* appCommandOptionData, vector<DiscordCoreAPI::ApplicationCommandOptionData> appCommandOptionDataSource) {

            if (appCommandOptionDataSource.size() > 0) {
                for (unsigned int x = 0; x < appCommandOptionDataSource.size(); x += 1) {
                    ApplicationCommandOptionData appCommandOptionDataNew;
                    appCommandOptionDataNew.description = appCommandOptionDataSource.at(x).description;
                    appCommandOptionDataNew.name = appCommandOptionDataSource.at(x).name;
                    appCommandOptionDataNew.required = appCommandOptionDataSource.at(x).required;
                    appCommandOptionDataNew.type = (ApplicationCommandOptionType)appCommandOptionDataSource.at(x).type;
                    if (appCommandOptionDataSource.at(x).choices.size() > 0) {
                        for (unsigned int y = 0; y < appCommandOptionDataSource.at(x).choices.size(); y += 1) {
                            ApplicationCommandOptionChoiceData choiceData;
                            choiceData.name = appCommandOptionDataSource.at(x).choices.at(y).name;
                            choiceData.valueInt = appCommandOptionDataSource.at(x).choices.at(y).valueInt;
                            choiceData.valueString = appCommandOptionDataSource.at(x).choices.at(y).valueString;
                            appCommandOptionDataNew.choices.push_back(choiceData);
                        }
                    }
                    if (appCommandOptionDataNew.type == ApplicationCommandOptionType::SUB_COMMAND || appCommandOptionDataNew.type == ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                        copyOptionsData(&appCommandOptionDataNew.options, appCommandOptionDataSource.at(x).options);
                    }
                    appCommandOptionData->push_back(appCommandOptionDataNew);

                }

            }

        }

        ~ApplicationCommandManager() {}
    };
}

#endif