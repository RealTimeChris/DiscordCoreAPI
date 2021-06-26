// Index.hpp - An index file to round everything up.
// Jun 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INDEX_
#define _INDEX_

#include "DiscordCoreClient.hpp"
#include "Commands\CommandsList.hpp"

namespace DiscordCoreAPI {
    class DiscordCoreClientNew {
    public:
        static void onChannelCreation(OnChannelCreationData dataPackage) {
            dataPackage.channel.discordCoreClient->channels->insertChannelAsync(dataPackage.channel).get();
            return;
        }

        static void onChannelUpdate(OnChannelUpdateData dataPackage) {
            dataPackage.channelNew.discordCoreClient->channels->insertChannelAsync(dataPackage.channelNew).get();
            return;
        }

        static void onChannelDeletion(OnChannelDeletionData dataPackage) {
            dataPackage.channel.discordCoreClient->channels->removeChannelAsync(dataPackage.channel.data.id).get();
            return;
        }

        static void onGuildCreation(OnGuildCreationData dataPackage) {
            dataPackage.guild.discordCoreClient->guilds->insertGuildAsync(dataPackage.guild).get();
            return;
        }

        static void onGuildUpdate(OnGuildUpdateData dataPackage) {
            dataPackage.guildNew.discordCoreClient->guilds->insertGuildAsync(dataPackage.guildNew).get();
            return;
        }

        static void onGuildDeletion(OnGuildDeletionData dataPackage) {
            dataPackage.guild.discordCoreClient->guilds->removeGuildAsync(dataPackage.guild.data.id).get();
            return;
        }

        static void onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
            dataPackage.guildMember.discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMember, dataPackage.guildMember.data.guildId).get();
            return;
        }

        static void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
            dataPackage.user.discordCoreClient->guildMembers->removeGuildMemberAsync(dataPackage.guildId, dataPackage.user.data.id).get();
            return;
        }

        static void onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
            dataPackage.guildMemberNew.discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMemberNew, dataPackage.guildMemberNew.data.guildId).get();
            return;
        }

        static void onRoleCreation(OnRoleCreationData dataPackage) {
            dataPackage.role.discordCoreClient->roles->insertRoleAsync(dataPackage.role).get();
            return;
        }

        static void onRoleUpdate(OnRoleUpdateData dataPackage) {
            dataPackage.roleNew.discordCoreClient->roles->insertRoleAsync(dataPackage.roleNew).get();
            return;
        }

        static void onRoleDeletion(OnRoleDeletionData dataPackage) {
            dataPackage.roleOld.discordCoreClient->roles->removeRoleAsync(dataPackage.roleOld.data.id).get();
            return;
        }

        static task<void> onInteractionCreation(OnInteractionCreationData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            try {
                if (dataPackage.eventData.eventType != InputEventType::BUTTON_INTERACTION) {
                    CommandData commandData(dataPackage.eventData);
                    commandData.eventData = dataPackage.eventData;
                    CommandController::checkForAndRunCommand(commandData);
                }
                else {
                    ButtonInteractionData dataPackageNew;
                    dataPackageNew.applicationId = dataPackage.eventData.getApplicationId();
                    dataPackageNew.channelId = dataPackage.eventData.getChannelId();
                    dataPackageNew.customId = dataPackage.eventData.getInteractionData().customId;
                    dataPackageNew.guildId = dataPackage.eventData.getGuildId();
                    dataPackageNew.id = dataPackage.eventData.getInteractionId();
                    dataPackageNew.member = dataPackage.eventData.getInteractionData().member;
                    dataPackageNew.message = dataPackage.eventData.getInteractionData().message;
                    dataPackageNew.token = dataPackage.eventData.getInteractionToken();
                    dataPackageNew.type = dataPackage.eventData.getInteractionData().type;
                    dataPackageNew.user = dataPackage.eventData.getInteractionData().user;
                    if (Button::buttonInteractioinBufferMap.contains(dataPackageNew.channelId + dataPackageNew.message.id)) {
                        asend(Button::buttonInteractioinBufferMap.at(dataPackageNew.channelId + dataPackageNew.message.id), dataPackageNew);
                    }
                }
                co_await mainThread;
                co_return;
            }
            catch (exception& e) {
                cout << "onMessageCreated() Error: " << e.what() << endl << endl;
                co_return;
            }
        }

        static void onMessageCreation(OnMessageCreationData dataPackage) {
            dataPackage.message.discordCoreClient->messages->insertMessageAsync(dataPackage.message).get();
            return;
        }

        static void onMessageUpdate(OnMessageUpdateData dataPackage) {
            dataPackage.messageNew.discordCoreClient->messages->insertMessageAsync(dataPackage.messageNew).get();
            return;
        }

        static void onMessageDeletion(OnMessageDeletionData dataPackage) {
            dataPackage.discordCoreClient->messages->removeMessageAsync(dataPackage.channelId, dataPackage.messageId).get();
            return;
        }

        static void onMessageDeleteBulk(OnMessageDeleteBulkData dataPackage) {
            for (auto value : dataPackage.ids) {
                dataPackage.discordCoreClient->messages->removeMessageAsync(dataPackage.channelId, value).get();
            }
            return;
        }

        static void onReactionAdd(OnReactionAddData dataPackage) {
            dataPackage.discordCoreClient->reactions->insertReactionAsync(dataPackage.reaction).get();
            return;
        }

        static void onReactionRemove(OnReactionRemoveData dataPackage) {
            dataPackage.reactionRemoveData.discordCoreClient->reactions->removeReactionAsync(dataPackage.reactionRemoveData.messageId, dataPackage.reactionRemoveData.userId, dataPackage.reactionRemoveData.emoji.name).get();
            return;
        }

        static void onReactionRemoveAll(OnReactionRemoveAllData dataPackage) {
            map<string, Reaction> reactionMap;
            if (try_receive(DiscordCoreAPI::ReactionManagerAgent::cache, reactionMap)) {
                for (auto [key, value] : reactionMap) {
                    if (key.find(dataPackage.messageId) != string::npos) {
                        reactionMap.erase(key);
                    }
                }
                asend(DiscordCoreAPI::ReactionManagerAgent::cache, reactionMap);
            }
            return;
        }

        static void onReactionRemoveEmoji(OnReactionRemoveEmojiData dataPackage) {
            map<string, Reaction> reactionMap;
            if (try_receive(DiscordCoreAPI::ReactionManagerAgent::cache, reactionMap)) {
                for (auto [key, value] : reactionMap) {
                    if (key.find(dataPackage.messageId) != string::npos && key.find(dataPackage.emoji.name) != string::npos) {
                        reactionMap.erase(key);
                    }
                }
                asend(DiscordCoreAPI::ReactionManagerAgent::cache, reactionMap);
            }
            return;
        }
    };    

    shared_ptr<DiscordCoreClient> DiscordCoreClient::finalSetup(string botToken) {
        try {
            DiscordCoreInternal::ThreadManager::initialize();
            DiscordCoreAPI::pDiscordCoreClient = make_shared<DiscordCoreClient>(to_hstring(botToken));
            pDiscordCoreClient->login();
            pDiscordCoreClient->eventManager->onChannelCreation(&DiscordCoreClientNew::onChannelCreation);
            pDiscordCoreClient->eventManager->onChannelUpdate(&DiscordCoreClientNew::onChannelUpdate);
            pDiscordCoreClient->eventManager->onChannelDeletion(&DiscordCoreClientNew::onChannelDeletion);
            pDiscordCoreClient->eventManager->onGuildCreation(&DiscordCoreClientNew::onGuildCreation);
            pDiscordCoreClient->eventManager->onGuildUpdate(&DiscordCoreClientNew::onGuildUpdate);
            pDiscordCoreClient->eventManager->onGuildDeletion(&DiscordCoreClientNew::onGuildDeletion);
            pDiscordCoreClient->eventManager->onGuildMemberAdd(&DiscordCoreClientNew::onGuildMemberAdd);
            pDiscordCoreClient->eventManager->onGuildMemberRemove(&DiscordCoreClientNew::onGuildMemberRemove);
            pDiscordCoreClient->eventManager->onGuildMemberUpdate(&DiscordCoreClientNew::onGuildMemberUpdate);
            pDiscordCoreClient->eventManager->onRoleCreation(&DiscordCoreClientNew::onRoleCreation);
            pDiscordCoreClient->eventManager->onRoleUpdate(&DiscordCoreClientNew::onRoleUpdate);
            pDiscordCoreClient->eventManager->onRoleDeletion(&DiscordCoreClientNew::onRoleDeletion);
            pDiscordCoreClient->eventManager->onInteractionCreation(&DiscordCoreClientNew::onInteractionCreation);
            pDiscordCoreClient->eventManager->onMessageCreation(&DiscordCoreClientNew::onMessageCreation);
            pDiscordCoreClient->eventManager->onMessageUpdate(&DiscordCoreClientNew::onMessageUpdate);
            pDiscordCoreClient->eventManager->onMessageDeletion(&DiscordCoreClientNew::onMessageDeletion);
            pDiscordCoreClient->eventManager->onMessageDeleteBulk(&DiscordCoreClientNew::onMessageDeleteBulk);
            pDiscordCoreClient->eventManager->onReactionAdd(&DiscordCoreClientNew::onReactionAdd);
            pDiscordCoreClient->eventManager->onReactionRemove(&DiscordCoreClientNew::onReactionRemove);
            pDiscordCoreClient->eventManager->onReactionRemoveAll(&DiscordCoreClientNew::onReactionRemoveAll);
            pDiscordCoreClient->eventManager->onReactionRemoveEmoji(&DiscordCoreClientNew::onReactionRemoveEmoji);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopItem);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::addShopRole);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::balance);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::blackJack);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::botInfo);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::buy);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::casinoStats);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::coinFlip);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::deposit);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::displayGuildsData);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::duel);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::gamehouseOptions);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::help);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::inventory);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::leaderboard);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::registerSlashCommands);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::removeObject);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::removeShopItem);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::removeShopRole);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::rob);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::roulette);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::shop);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::sellDrugs);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::setGameChannel);
            DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::test);
            return DiscordCoreAPI::pDiscordCoreClient;
        }
        catch (exception& e) {
            cout << "DiscordCoreAPI::finalSetup Error: " << e.what() << endl;
            return pDiscordCoreClient;
        }
    };
}
#endif
