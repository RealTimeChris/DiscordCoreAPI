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
            for (auto value : dataPackage.guild.members) {
                GuildMembers::removeGuildMemberAsync(value.guildId, value.user.id);
            }
            for (auto value : dataPackage.guild.channels) {
                Channels::removeChannelAsync(value.id);
            }
            for (auto value : dataPackage.guild.roles) {
                Roles::removeRoleAsync(value.id);
            }
            Guilds::removeGuildAsync(dataPackage.guild).get();
        }

        static void onGuildMemberAdd(OnGuildMemberAddData dataPackage) {
            GuildMembers::insertGuildMemberAsync(dataPackage.guildMember).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildMember.guildId }).get();
            guild.members.push_back(dataPackage.guildMember);
            guild.memberCount += 1;
            Guilds::insertGuildAsync(guild).get();
        }

        static void onGuildMemberRemove(OnGuildMemberRemoveData dataPackage) {
            GuildMembers::removeGuildMemberAsync(dataPackage.guildId, dataPackage.user.id).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildId }).get();
            for (int x = 0; x < guild.members.size(); x += 1) {
                if (guild.members[x].user.id == dataPackage.user.id) {
                    guild.members.erase(guild.members.begin() + x);
                }
            }
            guild.memberCount -= 1;
            Guilds::insertGuildAsync(guild).get();
        }

        static void onGuildMemberUpdate(OnGuildMemberUpdateData dataPackage) {
            GuildMembers::insertGuildMemberAsync(dataPackage.guildMemberNew).get();
            Guild guild = Guilds::getGuildAsync({ dataPackage.guildMemberNew.guildId }).get();
            for (int x = 0; x < guild.members.size(); x += 1) {
                if (guild.members[x].user.id == dataPackage.guildMemberNew.user.id) {
                    guild.members.erase(guild.members.begin() + x);
                }
            }
            guild.members.push_back(dataPackage.guildMemberNew);
            Guilds::insertGuildAsync(guild).get();
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
            GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = dataPackage.voiceStateData.userId,.guildId = dataPackage.voiceStateData.guildId }).get();
            guildMember.voiceData = dataPackage.voiceStateData;
            GuildMembers::insertGuildMemberAsync(guildMember).get();
            Guild guild = Guilds::getGuildAsync({ .guildId = dataPackage.voiceStateData.guildId }).get();
            for (int x = 0; x < guild.voiceStates.size(); x += 1) {
                if (guild.voiceStates[x].userId == dataPackage.voiceStateData.userId) {
                    guild.voiceStates.erase(guild.voiceStates.begin() + x);
                    break;
                }
            }
            guild.voiceStates.push_back(dataPackage.voiceStateData);
            Guilds::insertGuildAsync(guild).get();
            if (dataPackage.voiceStateData.userId == EventHandler::discordCoreClient->currentUser.id) {
                if (dataPackage.voiceStateData.channelId == "") {
                    auto voiceConnection = guild.connectToVoice(dataPackage.voiceStateData.channelId);
                    if (voiceConnection != nullptr) {
                        if (guild.areWeConnected()) {
                            guild.disconnect();
                        }
                    }
                }
            }
        }
    };

    DiscordCoreClient* EventHandler::discordCoreClient{ nullptr };
}
#endif
