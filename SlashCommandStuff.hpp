// SlashCommandStuff.hpp - Stuff for slash commands.
// May 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SLASH_COMMAND_STUFF_
#define _SLASH_COMMAND_STUFF_

#include "pch.h"
#include "JSONifier.hpp"
#include "HttpStuff.hpp"
#include "UserStuff.hpp"

namespace DiscordCoreAPI {

    struct ApplicationCommandOptionChoiceData {
        string name;
        int	valueInt = 0;
        string valueString = "";
    };

    struct ApplicationCommandOptionData {
        ApplicationCommandOptionType type;
        string name;
        string description;
        bool required;
        vector<ApplicationCommandOptionChoiceData>	choices;
        vector<ApplicationCommandOptionData> options;
    };

    struct ApplicationCommandData {
        string id;
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct EditApplicationCommandData {
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct CreateApplicationCommandData {
        ApplicationCommandType type;
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission = true;
    };

    struct DeleteApplicationCommandData {
        string name;
    };

    class ApplicationCommand {
    public:
        DiscordCoreInternal::ApplicationCommandData data;

        ApplicationCommand() {};

    protected:
        friend class SlashCommandManager;
        
        ApplicationCommand(DiscordCoreInternal::ApplicationCommandData dataNew) {
            this->data = dataNew;
        }
    };

	class SlashCommandManager {
	public:
		SlashCommandManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, string applicationIdNew) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
            this->applicationId = applicationIdNew;
		}

        ~SlashCommandManager() {
            this->threadContext->releaseGroup();
        }

		task<ApplicationCommand> createGlobalApplicationCommandAsync(CreateApplicationCommandData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::CreateApplicationCommandData dataPackageNew;
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
            requestAgent.getError("SlashCommandManager::createGlobalApplicationCommandAsync");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::createGlobalApplicationCommandAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::createGlobalApplicationCommandAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreInternal::ApplicationCommandData appCommandData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_return appCommandData;
		}

        vector<ApplicationCommand> getGlobalApplicationCommands() {
            DiscordCoreInternal::HttpWorkload workload;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.relativePath = "/applications/" + this->applicationId + "/commands";
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SLASH_COMMANDS;
            DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("SlashCommandManager::getGlobalApplicationCommands");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::getGlobalApplicationCommandsAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::getGlobalApplicationCommandsAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            vector<ApplicationCommand> appCommands;
            for (unsigned int x = 0; x < returnData.data.size(); x += 1) {
                DiscordCoreInternal::ApplicationCommandData appCommandData;
                DiscordCoreInternal::DataParser::parseObject(returnData.data.at(x), &appCommandData);
                ApplicationCommand appCommand(appCommandData);
                appCommands.push_back(appCommand);
            }
            return appCommands;
        }

        task<ApplicationCommand> editGlobalApplicationCommandAsync(EditApplicationCommandData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            vector<ApplicationCommand> appCommands = getGlobalApplicationCommands();
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
                cout << "SlashCommandManager::editGlobalApplicationCommand_00 Error: Sorry, it could not be found!" << endl;
                ApplicationCommand appCommand;
                co_return appCommand;
            }
            DiscordCoreInternal::EditApplicationCommandData dataPackageNew;
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
            requestAgent.getError("SlashCommandManager::editGlobalApplicationCommandAsync");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::editGlobalApplicationCommandsAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::editGlobalApplicationCommandsAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreInternal::ApplicationCommandData appCommandData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &appCommandData);
            ApplicationCommand appCommand(appCommandData);
            co_return appCommand;
        }

        task<void> deleteGlobalApplicationCommandAsync(DeleteApplicationCommandData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            vector<ApplicationCommand> appCommands = getGlobalApplicationCommands();
            string commandId;
            bool isItFound = false;
            for (auto const& [value] : appCommands) {
                if (value.name == dataPackage.name) {
                    commandId = value.id;
                    isItFound = true;
                }
            }
            if (isItFound == false) {
                cout << "SlashCommandManager::deleteGlobalApplicationCommand_00 Error: Sorry, it could not be found!" << endl;
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
            requestAgent.getError("SlashCommandManager::deleteGlobalApplicationCommandAsync");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "SlashCommandManager::deleteGlobalApplicationCommandAsync_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "SlashCommandManager::deleteGlobalApplicationCommandAsync_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            co_return;
        }

        task<void> displayGlobalApplicationCommandsAsync() {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            vector<ApplicationCommand> applicationCommands = getGlobalApplicationCommands();
            for (unsigned int x = 0; x < applicationCommands.size(); x += 1) {
                cout << "Command Name: " << applicationCommands.at(x).data.name << endl;
                cout << "Command Description: " << applicationCommands.at(x).data.description << endl;
                displayOptions(applicationCommands.at(x).data.options);
            }
            co_return;
        }

	protected:
        DiscordCoreInternal::HttpAgentResources agentResources;
        shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        string applicationId;

        void displayOptions(vector<DiscordCoreInternal::ApplicationCommandOptionData> applicationCommandOptionData) {
            for (unsigned int x = 0; x < applicationCommandOptionData.size(); x += 1) {
                string indentAmount;
                if (applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
                    indentAmount += "   ";
                }
                if (applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND) {
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
                if (applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND || applicationCommandOptionData.at(x).type == DiscordCoreInternal::ApplicationCommandOptionType::SUB_COMMAND_GROUP) {
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
