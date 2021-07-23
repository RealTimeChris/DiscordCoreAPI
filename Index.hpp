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

            static shared_ptr<DiscordCoreClient> discordCoreClient;

            static void onChannelCreation(OnChannelCreationData dataPackage) {
                dataPackage.channel.discordCoreClient->channels->insertChannelAsync(dataPackage.channel).get();
            }

            static void onChannelUpdate(OnChannelUpdateData dataPackage) {
                dataPackage.channelNew.discordCoreClient->channels->insertChannelAsync(dataPackage.channelNew).get();
            }

            static void onChannelDeletion(OnChannelDeletionData dataPackage) {
                dataPackage.channel.discordCoreClient->channels->removeChannelAsync(dataPackage.channel.data.id).get();
            }

            static void onGuildCreation(OnGuildCreationData dataPackage) {
                dataPackage.guild.discordCoreClient->guilds->insertGuildAsync(dataPackage.guild).get();
            }

            static void onGuildUpdate(OnGuildUpdateData dataPackage) {
                dataPackage.guildNew.discordCoreClient->guilds->insertGuildAsync(dataPackage.guildNew).get();
            }

            static void onGuildDeletion(OnGuildDeletionData dataPackage) {
                dataPackage.guild.discordCoreClient->guilds->removeGuildAsync(dataPackage.guild.data.id).get();
            }

            static void onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
                dataPackage.guildMember.discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMember, dataPackage.guildMember.data.guildId).get();
                Guild guild = dataPackage.guildMember.discordCoreClient->guilds->getGuildAsync({ dataPackage.guildMember.data.guildId }).get();
                guild.data.memberCount += 1;
                dataPackage.guildMember.discordCoreClient->guilds->insertGuildAsync(guild).get();
            }

            static void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
                dataPackage.user.discordCoreClient->guildMembers->removeGuildMemberAsync(dataPackage.guildId, dataPackage.user.data.id).get();
                Guild guild = dataPackage.user.discordCoreClient->guilds->getGuildAsync({ dataPackage.guildId }).get();
                guild.data.memberCount -= 1;
                dataPackage.user.discordCoreClient->guilds->insertGuildAsync(guild).get();
            }

            static void onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
                dataPackage.guildMemberNew.discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMemberNew, dataPackage.guildMemberNew.data.guildId).get();
            }

            static void onRoleCreation(OnRoleCreationData dataPackage) {
                dataPackage.role.discordCoreClient->roles->insertRoleAsync(dataPackage.role).get();
            }

            static void onRoleUpdate(OnRoleUpdateData dataPackage) {
                dataPackage.roleNew.discordCoreClient->roles->insertRoleAsync(dataPackage.roleNew).get();
            }

            static void onRoleDeletion(OnRoleDeletionData dataPackage) {
                dataPackage.roleOld.discordCoreClient->roles->removeRoleAsync(dataPackage.roleOld.data.id).get();
            }

            static void onInteractionCreationToBeWrapped(OnInteractionCreationData dataPackage) {
                try {
                    if (dataPackage.eventData.eventType != InputEventType::BUTTON_INTERACTION) {
                        CommandData commandData(dataPackage.eventData);
                        commandData.eventData = dataPackage.eventData;
                        CommandCenter::checkForAndRunCommand(commandData);
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
                }
                catch (exception& e) {
                    cout << "onMessageCreated() Error: " << e.what() << endl << endl;
                }
            }

            static task<void> onInteractionCreation(OnInteractionCreationData dataPackage) {
                apartment_context mainThread;
                co_await resume_background();
                try {
                    IndexHost::onInteractionCreationToBeWrapped(dataPackage);
                    co_await mainThread;
                    co_return;
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
            }

            static void onMessageCreation(OnMessageCreationData dataPackage) {
                dataPackage.message.discordCoreClient->messages->insertMessageAsync(dataPackage.message).get();
            }

            static void onMessageUpdate(OnMessageUpdateData dataPackage) {
                dataPackage.messageNew.discordCoreClient->messages->insertMessageAsync(dataPackage.messageNew).get();
            }

            static void onMessageDeletion(OnMessageDeletionData dataPackage) {
                dataPackage.discordCoreClient->messages->removeMessageAsync(dataPackage.channelId, dataPackage.messageId).get();
            }

            static void onMessageDeleteBulk(OnMessageDeleteBulkData dataPackage) {
                for (auto value : dataPackage.ids) {
                    dataPackage.discordCoreClient->messages->removeMessageAsync(dataPackage.channelId, value).get();
                }
            }

            static void onReactionAdd(OnReactionAddData dataPackage) {
                dataPackage.discordCoreClient->reactions->insertReactionAsync(dataPackage.reaction).get();
            }

            static void onReactionRemove(OnReactionRemoveData dataPackage) {
                dataPackage.reactionRemoveData.discordCoreClient->reactions->removeReactionAsync(dataPackage.reactionRemoveData.messageId, dataPackage.reactionRemoveData.userId, dataPackage.reactionRemoveData.emoji.name).get();
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
            }

            static void onVoiceStateUpdate(OnVoiceStateUpdateData dataPackage) {
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
                map<string, Guild> guildMap;
                if (try_receive(GuildManagerAgent::cache, guildMap)) {
                    if (dataPackage.voiceStateData.userId == IndexHost::discordCoreClient->currentUser->data.id) {
                        if (dataPackage.voiceStateData.channelId == "") {
                            Guild guild = guildMap.at(dataPackage.voiceStateData.guildId);
                            guild.disconnectFromVoice();
                            guildMap.erase(dataPackage.voiceStateData.guildId);
                            guildMap.insert(make_pair(dataPackage.voiceStateData.guildId, guild));
                            asend(GuildManagerAgent::cache, guildMap);
                        }
                    }
                }
            }
        };

    shared_ptr<DiscordCoreClient> DiscordCoreClient::finalSetup(string botToken) {
        try {
            pDiscordCoreClient = make_shared<DiscordCoreClient>(to_hstring(botToken));
            IndexHost::discordCoreClient = pDiscordCoreClient;
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
            CommandCenter::registerFunction("botinfo", new BotInfo);
            CommandCenter::registerFunction("test", new Test);
            return pDiscordCoreClient;
        }
        catch (exception& e) {
            cout << "DiscordCoreAPI::finalSetup Error: " << e.what() << endl;
            return pDiscordCoreClient;
        }
    };
    shared_ptr<DiscordCoreClient> IndexHost::discordCoreClient{ nullptr };
}
#endif
