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
        class IndexHost {
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
                Guild guild = dataPackage.guildMember.discordCoreClient->guilds->getGuildAsync({ dataPackage.guildMember.data.guildId }).get();
                guild.data.memberCount += 1;
                dataPackage.guildMember.discordCoreClient->guilds->insertGuildAsync(guild).get();
                return;
            }

            static void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
                dataPackage.user.discordCoreClient->guildMembers->removeGuildMemberAsync(dataPackage.guildId, dataPackage.user.data.id).get();
                Guild guild = dataPackage.user.discordCoreClient->guilds->getGuildAsync({ dataPackage.guildId }).get();
                guild.data.memberCount -= 1;
                dataPackage.user.discordCoreClient->guilds->insertGuildAsync(guild).get();
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

            static task<void> onInteractionCreationToBeWrapped(OnInteractionCreationData dataPackage) {
                apartment_context mainThread;
                co_await resume_background();
                try {
                    if (dataPackage.eventData.eventType != InputEventType::BUTTON_INTERACTION) {
                        CommandData commandData(dataPackage.eventData);
                        commandData.eventData = dataPackage.eventData;
                        cout << "GUILD ID: " << dataPackage.eventData.getGuildId() << endl;
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
                        if (Button::buttonInteractionMap.contains(dataPackageNew.channelId + dataPackageNew.message.id)) {
                            asend(Button::buttonInteractionMap.at(dataPackageNew.channelId + dataPackageNew.message.id), dataPackageNew);
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

            static task<void> onInteractionCreation(OnInteractionCreationData dataPackage) {
                apartment_context mainThread;
                co_await resume_background();
                create_task(IndexHost::onInteractionCreationToBeWrapped(dataPackage)).then([&](task<void> previousTask)->task<void> {
                    try {
                        previousTask.get();
                    }
                    catch (const exception& e) {
                        cout << "Exception: " << e.what() << endl;
                    }
                    catch (const winrt::hresult_invalid_argument& e) {
                        cout << "Exception: " << to_string(e.message()) << endl;
                    }
                    catch (winrt::hresult_error& e) {
                        cout << "Exception: " << to_string(e.message()) << endl;
                    }
                    co_await mainThread;
                    co_return;
                    }).get();
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
                if (try_receive(ReactionManagerAgent::cache, reactionMap)) {
                    for (auto [key, value] : reactionMap) {
                        if (key.find(dataPackage.messageId) != string::npos) {
                            reactionMap.erase(key);
                        }
                    }
                    asend(ReactionManagerAgent::cache, reactionMap);
                }
                return;
            }

            static void onReactionRemoveEmoji(OnReactionRemoveEmojiData dataPackage) {
                map<string, Reaction> reactionMap;
                if (try_receive(ReactionManagerAgent::cache, reactionMap)) {
                    for (auto [key, value] : reactionMap) {
                        if (key.find(dataPackage.messageId) != string::npos && key.find(dataPackage.emoji.name) != string::npos) {
                            reactionMap.erase(key);
                        }
                    }
                    asend(ReactionManagerAgent::cache, reactionMap);
                }
                return;
            }

            static void onVoiceStateUpdate(OnVoiceStateUpdateData dataPackage){
                map<string, GuildMember> guildMemberMap;
                if (try_receive(GuildMemberManagerAgent::cache, guildMemberMap)) {
                    if (guildMemberMap.contains(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId)) {
                        GuildMember guildMember = guildMemberMap.at(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId);
                        guildMemberMap.erase(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId);
                        guildMember.data.voiceData = dataPackage.voiceStateData;
                        guildMemberMap.insert(std::make_pair(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId, guildMember));
                    }
                    asend(GuildMemberManagerAgent::cache, guildMemberMap);
                }
            }

        };

    shared_ptr<DiscordCoreClient> DiscordCoreClient::finalSetup(string botToken) {
        try {
            DiscordCoreInternal::ThreadManager::initialize();
            pDiscordCoreClient = make_shared<DiscordCoreClient>(to_hstring(botToken));
            pDiscordCoreClient->initialize().get();
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
            pDiscordCoreClient->eventManager->onMessageCreation(&IndexHost::onMessageCreation);
            pDiscordCoreClient->eventManager->onMessageUpdate(&IndexHost::onMessageUpdate);
            pDiscordCoreClient->eventManager->onMessageDeletion(&IndexHost::onMessageDeletion);
            pDiscordCoreClient->eventManager->onMessageDeleteBulk(&IndexHost::onMessageDeleteBulk);
            pDiscordCoreClient->eventManager->onReactionAdd(&IndexHost::onReactionAdd);
            pDiscordCoreClient->eventManager->onReactionRemove(&IndexHost::onReactionRemove);
            pDiscordCoreClient->eventManager->onReactionRemoveAll(&IndexHost::onReactionRemoveAll);
            pDiscordCoreClient->eventManager->onReactionRemoveEmoji(&IndexHost::onReactionRemoveEmoji);
            pDiscordCoreClient->eventManager->onVoiceStateUpdate(&IndexHost::onVoiceStateUpdate);
            CommandController::addCommand(&botInfo);
            CommandController::addCommand(&displayGuildsData);
            CommandController::addCommand(&help);
            CommandController::addCommand(&play);
            CommandController::addCommand(&registerSlashCommands);
            CommandController::addCommand(&setBorderColor);
            CommandController::addCommand(&setMusicChannel);
            CommandController::addCommand(&test);
            return pDiscordCoreClient;
        }
        catch (exception& e) {
            cout << "DiscordCoreAPI::finalSetup Error: " << e.what() << endl;
            return pDiscordCoreClient;
        }
    };
}
#endif
