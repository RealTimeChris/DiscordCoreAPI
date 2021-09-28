// DiscordCoreClient02.hpp - Second header for the DiscordCoreClient class.
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_02_
#define _DISCORD_CORE_CLIENT_02_

#include "EventHandler.hpp"

namespace DiscordCoreAPI {

    void DiscordCoreClient::finalSetup(string botTokenNew, string commandPrefixNew, vector<RepeatedFunctionData>* functionsToExecuteNew = nullptr) {
        try {
            DiscordCoreAPI::commandPrefix = commandPrefixNew;
            DiscordCoreInternal::ThreadManager::intialize();
            DiscordCoreClient* pDiscordCoreClient = new DiscordCoreClient(botTokenNew);
            DiscordCoreClient::thisPointer = pDiscordCoreClient;
            DiscordCoreClient::thisPointer->functionsToExecute = functionsToExecuteNew;
            EventHandler::discordCoreClient = pDiscordCoreClient;
            DiscordCoreClient::thisPointer->initialize().get();
            pDiscordCoreClient->eventManager->onChannelCreation(&EventHandler::onChannelCreation);
            pDiscordCoreClient->eventManager->onChannelUpdate(&EventHandler::onChannelUpdate);
            pDiscordCoreClient->eventManager->onChannelDeletion(&EventHandler::onChannelDeletion);
            pDiscordCoreClient->eventManager->onGuildCreation(&EventHandler::onGuildCreation);
            pDiscordCoreClient->eventManager->onGuildUpdate(&EventHandler::onGuildUpdate);
            pDiscordCoreClient->eventManager->onGuildDeletion(&EventHandler::onGuildDeletion);
            pDiscordCoreClient->eventManager->onGuildMemberAdd(&EventHandler::onGuildMemberAdd);
            pDiscordCoreClient->eventManager->onGuildMemberRemove(&EventHandler::onGuildMemberRemove);
            pDiscordCoreClient->eventManager->onGuildMemberUpdate(&EventHandler::onGuildMemberUpdate);
            pDiscordCoreClient->eventManager->onRoleCreation(&EventHandler::onRoleCreation);
            pDiscordCoreClient->eventManager->onRoleUpdate(&EventHandler::onRoleUpdate);
            pDiscordCoreClient->eventManager->onRoleDeletion(&EventHandler::onRoleDeletion);
            pDiscordCoreClient->eventManager->onInteractionCreation(&EventHandler::onInteractionCreation);
            pDiscordCoreClient->eventManager->onVoiceStateUpdate(&EventHandler::onVoiceStateUpdate);
        }
        catch (exception& e) {
            cout << "DiscordCoreAPI::finalSetup Error: " << e.what() << endl;
        }
    };
}
#endif
