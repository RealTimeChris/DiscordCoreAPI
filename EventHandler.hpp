// EventHandler.hpp - Header for the event handler class.
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

        static DiscordCoreClient* discordCoreClient;

        static void onChannelCreation(OnChannelCreationData dataPackage) {
            Channels::insertChannelAsync(dataPackage.channel).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.channel.guildId }).get();
            guild.channels.push_back(dataPackage.channel);
            Guilds::insertGuildAsync(guild).get();            
        }

        static void onChannelUpdate(OnChannelUpdateData dataPackage) {
            Channels::insertChannelAsync(dataPackage.channelNew).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.channelNew.guildId }).get();
            for (int x = 0; x < guild.channels.size(); x += 1) {
                if (guild.channels[x].id == dataPackage.channelNew.id) {
                    guild.channels.erase(guild.channels.begin() + x);
                }
            }
            guild.channels.push_back(dataPackage.channelNew);
            Guilds::insertGuildAsync(guild).get();
        }

        static void onChannelDeletion(OnChannelDeletionData dataPackage) {
            Channels::removeChannelAsync(dataPackage.channel.id).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.channel.guildId }).get();
            for (int x = 0; x < guild.channels.size(); x += 1) {
                if (guild.channels[x].id == dataPackage.channel.id) {
                    guild.channels.erase(guild.channels.begin() + x);
                }
            }
            Guilds::insertGuildAsync(guild).get();
        }
        
        static void onGuildCreation(OnGuildCreationData dataPackage) {
            Guilds::insertGuildAsync(dataPackage.guild).get();
        }        

        static void onGuildUpdate(OnGuildUpdateData dataPackage) {
            Guilds::insertGuildAsync(dataPackage.guildNew).get();
        }

        static void onGuildDeletion(OnGuildDeletionData dataPackage) {
            Guilds::removeGuildAsync(dataPackage.guild).get();
        }

        static void onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
            GuildMembers::insertGuildMemberAsync(dataPackage.guildMember).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildMember.guildId }).get();
            guild.memberCount += 1;
            Guilds::insertGuildAsync(guild).get();
        }

        static void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
            GuildMembers::removeGuildMemberAsync(dataPackage.guildId, dataPackage.user.id).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildId }).get();
            guild.memberCount -= 1;
            Guilds::insertGuildAsync(guild).get();
        }

        static void onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
            GuildMembers::insertGuildMemberAsync(dataPackage.guildMemberNew).get();
        }

        static void onRoleCreation(OnRoleCreationData dataPackage) {
            Roles::insertRoleAsync(dataPackage.role).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildId }).get();
            guild.roles.push_back(dataPackage.role);
            Guilds::insertGuildAsync(guild).get();
        }

        static void onRoleUpdate(OnRoleUpdateData dataPackage) {
            Roles::insertRoleAsync(dataPackage.roleNew).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildId }).get();
            for (int x = 0; x < guild.roles.size(); x += 1) {
                if (guild.roles[x].id == dataPackage.roleNew.id) {
                    guild.roles.erase(guild.roles.begin() + x);
                }
            }
            guild.roles.push_back(dataPackage.roleNew);
            Guilds::insertGuildAsync(guild).get();
        }

        static void onRoleDeletion(OnRoleDeletionData dataPackage) {
            Roles::removeRoleAsync(dataPackage.roleOld.id).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildId }).get();
            for (int x = 0; x < guild.roles.size(); x += 1) {
                if (guild.roles[x].id == dataPackage.roleOld.id) {
                    guild.roles.erase(guild.roles.begin() + x);
                }
            }
            Guilds::insertGuildAsync(guild).get();
        }

        static task<void> onInteractionCreation(OnInteractionCreationData dataPackage) {
            apartment_context mainThread;
            co_await resume_background();
            try {
                shared_ptr<unbounded_buffer<MessageData>> bufferBlock = make_shared<unbounded_buffer<MessageData>>();
                DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.insert(make_pair(dataPackage.eventData.getInteractionId(), bufferBlock));
                send(*bufferBlock, dataPackage.eventData.getMessageData());
                if (dataPackage.eventData.eventType == InputEventType::REGULAR_MESSAGE || dataPackage.eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                    CommandData commandData(dataPackage.eventData);
                    commandData.eventData = dataPackage.eventData;
                    CommandCenter::checkForAndRunCommand(commandData);
                }
                else if (dataPackage.eventData.eventType == InputEventType::BUTTON_INTERACTION) {
                    if (ButtonCollector::buttonInteractionBufferMap.contains(dataPackage.eventData.getChannelId() + dataPackage.eventData.getMessageId())) {
                        send(ButtonCollector::buttonInteractionBufferMap.at(dataPackage.eventData.getChannelId() + dataPackage.eventData.getMessageId()), dataPackage.eventData.getInteractionData());
                    }
                }
                else if (dataPackage.eventData.eventType == InputEventType::SELECT_MENU_INPUT) {
                    if (SelectMenuCollector::selectMenuInteractionBufferMap.contains(dataPackage.eventData.getChannelId() + dataPackage.eventData.getMessageId())) {
                        send(SelectMenuCollector::selectMenuInteractionBufferMap.at(dataPackage.eventData.getChannelId() + dataPackage.eventData.getMessageId()), dataPackage.eventData.getInteractionData());
                    }
                }
                else if (dataPackage.eventData.eventType == InputEventType::MESSAGE_COMMAND_INTERACTION) {
                    CommandData commandData(dataPackage.eventData);
                    commandData.eventData = dataPackage.eventData;
                    CommandCenter::checkForAndRunCommand(commandData);
                }
                else if (dataPackage.eventData.eventType == InputEventType::USER_COMMAND_INTERACTION) {
                    CommandData commandData(dataPackage.eventData);
                    commandData.eventData = dataPackage.eventData;
                    CommandCenter::checkForAndRunCommand(commandData);
                }
                co_await mainThread;
                co_return;
            }
            catch (...) {
                rethrowException("EventHandler::onInteractionCreation() Error: ");
            }
            co_await mainThread;
            co_return;
        }

        static void onVoiceStateUpdate(OnVoiceStateUpdateData dataPackage) {
            map<string, GuildMember> guildMemberMap;
            if (try_receive(DiscordCoreInternal::GuildMemberManagerAgent::cache, guildMemberMap)) {
                if (guildMemberMap.contains(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId)) {
                    GuildMember guildMember = guildMemberMap.at(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId);
                    guildMemberMap.erase(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId);
                    guildMember.voiceData = dataPackage.voiceStateData;
                    guildMemberMap.insert(std::make_pair(dataPackage.voiceStateData.guildId + " + " + dataPackage.voiceStateData.userId, guildMember));
                }
                send(DiscordCoreInternal::GuildMemberManagerAgent::cache, guildMemberMap);
            }
            map<string, Guild> guildMap;
            if (try_receive(DiscordCoreInternal::GuildManagerAgent::cache, guildMap)) {
                if (dataPackage.voiceStateData.userId == EventHandler::discordCoreClient->currentUser.id) {
                    if (dataPackage.voiceStateData.channelId == "") {
                        Guild guild = guildMap.at(dataPackage.voiceStateData.guildId);
                        auto voiceConnection = guild.connectToVoice(dataPackage.voiceStateData.channelId);
                        if (voiceConnection != nullptr) {
                            if (guild.areWeConnected()) {
                                guild.disconnect();
                            }
                        }
                        guildMap.erase(dataPackage.voiceStateData.guildId);
                        guildMap.insert(make_pair(dataPackage.voiceStateData.guildId, guild));
                    }
                }
                send(DiscordCoreInternal::GuildManagerAgent::cache, guildMap);
            }
        }
    };

    DiscordCoreClient* EventHandler::discordCoreClient{ nullptr };
}
#endif
