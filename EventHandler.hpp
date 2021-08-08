// EventHandler.hpp - An index file to round everything up.
// Jun 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _EVENT_HANDLER_
#define _EVENT_HANDLER_

#include "DiscordCoreClient01.hpp"
#include "Commands\CommandsList.hpp"

namespace DiscordCoreAPI {
    class EventHandler {
    public:

        static shared_ptr<DiscordCoreClient> discordCoreClient;

        static void onChannelCreation(OnChannelCreationData dataPackage) {
            EventHandler::discordCoreClient->channels->insertChannelAsync(dataPackage.channel).get();
        }

        static void onChannelUpdate(OnChannelUpdateData dataPackage) {
            EventHandler::discordCoreClient->channels->insertChannelAsync(dataPackage.channelNew).get();
        }

        static void onChannelDeletion(OnChannelDeletionData dataPackage) {
            EventHandler::discordCoreClient->channels->removeChannelAsync(dataPackage.channel.data.id).get();
        }
        
        static void onGuildCreation(OnGuildCreationData dataPackage) {
            EventHandler::discordCoreClient->guilds->insertGuildAsync(dataPackage.guild).get();
        }
        
        static void onGuildUpdate(OnGuildUpdateData dataPackage) {
            EventHandler::discordCoreClient->guilds->insertGuildAsync(dataPackage.guildNew).get();
        }

        static void onGuildDeletion(OnGuildDeletionData dataPackage) {
            EventHandler::discordCoreClient->guilds->removeGuildAsync(dataPackage.guild.data.id).get();
        }

        static void onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
            EventHandler::discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMember, dataPackage.guildMember.data.guildId).get();
            Guild guild = dataPackage.guildMember.discordCoreClient->guilds->getGuildAsync({ dataPackage.guildMember.data.guildId }).get();
            guild.data.memberCount += 1;
            EventHandler::discordCoreClient->guilds->insertGuildAsync(guild).get();
        }

        static void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
            EventHandler::discordCoreClient->guildMembers->removeGuildMemberAsync(dataPackage.guildId, dataPackage.user.data.id).get();
            Guild guild = EventHandler::discordCoreClient->guilds->getGuildAsync({ dataPackage.guildId }).get();
            guild.data.memberCount -= 1;
            EventHandler::discordCoreClient->guilds->insertGuildAsync(guild).get();
        }

        static void onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
            EventHandler::discordCoreClient->guildMembers->insertGuildMemberAsync(dataPackage.guildMemberNew, dataPackage.guildMemberNew.data.guildId).get();
        }

        static void onRoleCreation(OnRoleCreationData dataPackage) {
            EventHandler::discordCoreClient->roles->insertRoleAsync(dataPackage.role).get();
        }

        static void onRoleUpdate(OnRoleUpdateData dataPackage) {
            EventHandler::discordCoreClient->roles->insertRoleAsync(dataPackage.roleNew).get();
        }

        static void onRoleDeletion(OnRoleDeletionData dataPackage) {
            EventHandler::discordCoreClient->roles->removeRoleAsync(dataPackage.roleOld.data.id).get();
        }

        static task<void> onInteractionCreation(OnInteractionCreationData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            try {
                if (dataPackage.eventData.eventType == InputEventType::REGULAR_MESSAGE || dataPackage.eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                    CommandData commandData(dataPackage.eventData);
                    commandData.eventData = dataPackage.eventData;
                    CommandCenter::checkForAndRunCommand(commandData);
                }
                else if (dataPackage.eventData.eventType == InputEventType::BUTTON_INTERACTION) {
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
                        send(Button::buttonInteractionMap.at(dataPackageNew.channelId + dataPackageNew.message.id), dataPackageNew);
                    }
                }
                else if (dataPackage.eventData.eventType == InputEventType::SELECT_MENU_INPUT) {
                    SelectMenuInteractionData dataPackageNew;
                    dataPackageNew.applicationId = dataPackage.eventData.getApplicationId();
                    dataPackageNew.channelId = dataPackage.eventData.getChannelId();
                    dataPackageNew.customId = dataPackage.eventData.getInteractionData().customId;
                    dataPackageNew.guildId = dataPackage.eventData.getGuildId();
                    dataPackageNew.id = dataPackage.eventData.getInteractionId();
                    dataPackageNew.member = dataPackage.eventData.getInteractionData().member;
                    dataPackageNew.message = dataPackage.eventData.getInteractionData().message;
                    dataPackageNew.token = dataPackage.eventData.getInteractionToken();
                    dataPackageNew.values = dataPackage.eventData.getInteractionData().values;
                    dataPackageNew.type = dataPackage.eventData.getInteractionData().type;
                    dataPackageNew.user = dataPackage.eventData.getInteractionData().user;
                    if (SelectMenu::selectMenuInteractionMap.contains(dataPackageNew.channelId + dataPackageNew.message.id)) {
                        send(SelectMenu::selectMenuInteractionMap.at(dataPackageNew.channelId + dataPackageNew.message.id), dataPackageNew);
                    }
                }
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

        static void onVoiceStateUpdate(OnVoiceStateUpdateData dataPackage) {
            map<string, GuildMember> guildMemberMap;
            if (try_receive(GuildMemberManagerAgent::cache, guildMemberMap)) {
                if (guildMemberMap.contains(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId)) {
                    GuildMember guildMember = guildMemberMap.at(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId);
                    guildMemberMap.erase(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId);
                    guildMember.data.voiceData = dataPackage.voiceStateData;
                    guildMemberMap.insert(std::make_pair(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId, guildMember));
                }
                send(GuildMemberManagerAgent::cache, guildMemberMap);
            }
            map<string, Guild> guildMap;
            if (try_receive(GuildManagerAgent::cache, guildMap)) {
                if (dataPackage.voiceStateData.userId == EventHandler::discordCoreClient->currentUser->data.id) {
                    if (dataPackage.voiceStateData.channelId == "") {
                        Guild guild = guildMap.at(dataPackage.voiceStateData.guildId);
                        guild.disconnectFromVoice();
                        guildMap.erase(dataPackage.voiceStateData.guildId);
                        guildMap.insert(make_pair(dataPackage.voiceStateData.guildId, guild));
                    }
                }
                send(GuildManagerAgent::cache, guildMap);
            }
        }
    };

    shared_ptr<DiscordCoreClient> EventHandler::discordCoreClient{ nullptr };
}
#endif
