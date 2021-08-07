// DiscordCoreClient02.hpp - Second header for the DiscordCoreClient class.
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_02_
#define _DISCORD_CORE_CLIENT_02_

#include "EventHandler.hpp"

namespace DiscordCoreAPI {


    void DiscordCoreClient::finalSetup(string botToken) {
        try {
            DiscordCoreInternal::ThreadManager::intialize();
            shared_ptr<DiscordCoreClient> pDiscordCoreClient = make_shared<DiscordCoreClient>(to_hstring(botToken));
            DiscordCoreClient::thisPointer = pDiscordCoreClient;
            IndexHost::discordCoreClient = pDiscordCoreClient;
            pDiscordCoreClient->initialize().get();
            executeFunctionAfterTimePeriod([]() {
                cout << "Heart beat!" << endl << endl;
                }, 60000, true);
            pDiscordCoreClient->eventManager->onChannelCreation(&IndexHost::onChannelCreation);
            pDiscordCoreClient->eventManager->onChannelUpdate(&IndexHost::onChannelUpdate);
            pDiscordCoreClient->eventManager->onChannelDeletion(&IndexHost::onChannelDeletion);
            pDiscordCoreClient->eventManager->onGuildCreation(&IndexHost::onGuildCreation);
            pDiscordCoreClient->eventManager->onGuildUpdate(&IndexHost::onGuildUpdate);
            pDiscordCoreClient->eventManager->onGuildDeletion(&IndexHost::onGuildDeletion);
            pDiscordCoreClient->eventManager->onGuildMemberAdd(&IndexHost::onGuildMemberAdd);
            pDiscordCoreClient->eventManager->onGuildMemberRemove(&IndexHost::onGuildMemberRemove);
            pDiscordCoreClient->eventManager->onGuildMemberUpdate(&IndexHost::onGuildMemberUpdate);
            pDiscordCoreClient->eventManager->onRoleCreation(&IndexHost::onRoleCreation);
            pDiscordCoreClient->eventManager->onRoleUpdate(&IndexHost::onRoleUpdate);
            pDiscordCoreClient->eventManager->onRoleDeletion(&IndexHost::onRoleDeletion);
            pDiscordCoreClient->eventManager->onInteractionCreation(&IndexHost::onInteractionCreation);
            pDiscordCoreClient->eventManager->onVoiceStateUpdate(&IndexHost::onVoiceStateUpdate);
            CommandCenter::registerFunction("botinfo", new BotInfo);
            CommandCenter::registerFunction("play", new Play);
            CommandCenter::registerFunction("test", new Test);
        }
        catch (exception& e) {
            cout << "DiscordCoreAPI::finalSetup Error: " << e.what() << endl;
        }
    };
}

#endif
