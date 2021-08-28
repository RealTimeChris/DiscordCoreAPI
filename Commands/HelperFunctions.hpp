// HelperFunctions.hpp - Header for some helper functions.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELPER_FUNCTIONS_
#define _HELPER_FUNCTIONS_

#include "../pch.h"

namespace DiscordCoreAPI {

    bool areWeInADM(InputEventData eventData, Channel channel, bool displayResponse = false) {
        auto currentChannelType = channel.data.type;
        if (currentChannelType == ChannelType::DM) {
            if (displayResponse) {
                string msgString = "------\n**Sorry, but we can't do that in a direct message!**\n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(eventData.getMessageData().interaction.user.username, eventData.getMessageData().author.avatar);
                msgEmbed.setColor("FEFEFE");
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Direct Message Issue:**__");
                InputEventData event01;
                if (eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData responseData(eventData);
                    responseData.addMessageEmbed(msgEmbed);
                    event01 = InputEvents::respondToEvent(responseData);
                    InputEvents::deleteInputEventResponseAsync(event01, 20000);
                }
                else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                    CreateEphemeralInteractionResponseData responseData(eventData);
                    responseData.addMessageEmbed(msgEmbed);
                    event01 = InputEvents::respondToEvent(responseData);
                }
            }            
            return true;
        }
        return false;
    }

    class PermissionsConverter {
    public:

        static bool checkForPresence(DiscordCoreInternal::Permissions permission, string permissionString) {
            if (permissionString == "") {
                return false;
            }
            __int64 permissionsInteger = stoll(permissionString);
            if ((permissionsInteger & (__int64)permission) == (__int64)permission) {
                return true;
            }
            else {
                return false;
            }
        }

        static string addPermissionsToString(string originalPermissionString, vector<Permissions> permissionsToAdd) {
            if (originalPermissionString == "") {
                originalPermissionString = "0";
            }
            __int64 permissionsInteger = stoll(originalPermissionString);
            for (auto value : permissionsToAdd) {
                permissionsInteger |= (__int64)value;
            }
            stringstream sstream;
            sstream << permissionsInteger;
            return sstream.str();
        }

        static string removePermissionsFromString(string originalPermissionString, vector<Permissions> permissionsToRemove) {
            if (originalPermissionString == "") {
                originalPermissionString = "0";
            }
            __int64 permissionsInteger = stoll(originalPermissionString);
            for (auto value : permissionsToRemove) {
                permissionsInteger &= ~(__int64)value;
            }
            stringstream sstream;
            sstream << permissionsInteger;
            return sstream.str();
        }

        static vector<string> displayPermissions(string permissionString) {
            vector<string> returnVector;
            if (permissionString == "") {
                permissionString = "0";
                cout << "TESTING TESTING TESTING" << endl;
            }
            __int64 permissionsInteger = stoll(permissionString);
            if (permissionsInteger & 1 << 3) {
                for (__int64 x = 0; x < 37; x += 1) {
                    permissionsInteger |= 1i64 << x;
                }
            }
            stringstream sstream;
            if (permissionsInteger & (1 << 0)) {
                sstream << "CREATE_INSTANT_INVITE" << endl;
                returnVector.push_back("Create Instant Invite");
            }
            if (permissionsInteger & (1 << 1)) {
                sstream << "KICK_MEMBERS" << endl;
                returnVector.push_back("Kick Members");
            }
            if (permissionsInteger & (1 << 2)) {
                sstream << "BAN_MEMBERS" << endl;
                returnVector.push_back("Ban Members");
            }
            if (permissionsInteger & (1 << 3)) {
                sstream << "ADMINISTRATOR" << endl;
                returnVector.push_back("Administrator");
            }
            if (permissionsInteger & (1 << 4)) {
                sstream << "MANAGE_CHANNELS" << endl;
                returnVector.push_back("Manage Channels");
            }
            if (permissionsInteger & (1 << 5)) {
                sstream << "MANAGE_GUILD" << endl;
                returnVector.push_back("Manage Guild");
            }
            if (permissionsInteger & (1 << 6)) {
                sstream << "ADD_REACTIONS" << endl;
                returnVector.push_back("Add Reactions");
            }
            if (permissionsInteger & (1 << 7)) {
                sstream << "VIEW_AUDIT_LOG" << endl;
                returnVector.push_back("View Audit Log");
            }
            if (permissionsInteger & (1 << 8)) {
                sstream << "PRIORITY_SPEAKER" << endl;
                returnVector.push_back("Priority Speaker");
            }
            if (permissionsInteger & (1 << 9)) {
                sstream << "STREAM" << endl;
                returnVector.push_back("Stream");
            }
            if (permissionsInteger & (1 << 10)) {
                sstream << "VIEW_CHANNEL" << endl;
                returnVector.push_back("View Channel");
            }
            if (permissionsInteger & (1 << 11)) {
                sstream << "SEND_MESSAGES" << endl;
                returnVector.push_back("Send Messages");
            }
            if (permissionsInteger & (1 << 12)) {
                sstream << "SEND_TTS_MESSAGES" << endl;
                returnVector.push_back("Send TTS Messages");
            }
            if (permissionsInteger & (1 << 13)) {
                sstream << "MANAGE_MESSAGES" << endl;
                returnVector.push_back("Manage Messages");
            }
            if (permissionsInteger & (1 << 14)) {
                sstream << "EMBED_LINKS" << endl;
                returnVector.push_back("Embed Links");
            }
            if (permissionsInteger & (1 << 15)) {
                sstream << "ATTACH_FILES" << endl;
                returnVector.push_back("Attach Files");
            }
            if (permissionsInteger & (1 << 16)) {
                sstream << "READ_MESSAGE_HISTORY" << endl;
                returnVector.push_back("Read Message History");
            }
            if (permissionsInteger & (1 << 17)) {
                sstream << "MENTION_EVERYONE" << endl;
                returnVector.push_back("Mention Everyone");
            }
            if (permissionsInteger & (1 << 18)) {
                sstream << "USE_EXTERNAL_EMOJI" << endl;
                returnVector.push_back("Use External Emoji");
            }
            if (permissionsInteger & (1 << 19)) {
                sstream << "VIEW_GUILD_INSIGHTS" << endl;
                returnVector.push_back("View Guild Insights");
            }
            if (permissionsInteger & (1 << 20)) {
                sstream << "CONNECT" << endl;
                returnVector.push_back("Connect");
            }
            if (permissionsInteger & (1 << 21)) {
                sstream << "SPEAK" << endl;
                returnVector.push_back("Speak");
            }
            if (permissionsInteger & (1 << 22)) {
                sstream << "MUTE_MEMBERS" << endl;
                returnVector.push_back("Mute Members");
            }
            if (permissionsInteger & (1 << 23)) {
                sstream << "DEAFEN_MEMBERS" << endl;
                returnVector.push_back("Deafen Members");
            }
            if (permissionsInteger & (1 << 24)) {
                sstream << "MOVE_MEMBERS" << endl;
                returnVector.push_back("Move Members");
            }
            if (permissionsInteger & (1 << 25)) {
                sstream << "USE_VAD" << endl;
                returnVector.push_back("Use VAD");
            }
            if (permissionsInteger & (1 << 26)) {
                sstream << "CHANGE_NICKNAME" << endl;
                returnVector.push_back("Change Nickname");
            }
            if (permissionsInteger & (1 << 27)) {
                sstream << "MANAGE_NICKNAMES" << endl;
                returnVector.push_back("Manage Nicknames");
            }
            if (permissionsInteger & (1 << 28)) {
                sstream << "MANAGE_ROLES" << endl;
                returnVector.push_back("Manage Roles");
            }
            if (permissionsInteger & (1 << 29)) {
                sstream << "MANAGE_WEBHOOKS" << endl;
                returnVector.push_back("Manage Webhooks");
            }
            if (permissionsInteger & (1 << 30)) {
                sstream << "MANAGE_EMOJIS" << endl;
                returnVector.push_back("Manage Emojis");
            }
            if (permissionsInteger & (1 << 31)) {
                sstream << "USE_SLASH_COMMANDS" << endl;
                returnVector.push_back("Use Slash Commands");
            }
            if (permissionsInteger & (1ull << 32)) {
                sstream << "REQUEST_TO_SPEAK" << endl;
                returnVector.push_back("Request To Speak");
            }
            if (permissionsInteger & (1ull << 34)) {
                sstream << "MANAGE_THREADS" << endl;
                returnVector.push_back("Manage Threads");
            }
            if (permissionsInteger & (1ull << 35)) {
                sstream << "USE_PUBLIC_THREADS" << endl;
                returnVector.push_back("Use Public Threads");
            }
            if (permissionsInteger & (1ull << 36)) {
                sstream << "USE_PRIVATE_THREADS" << endl;
                returnVector.push_back("Use Private Threads");
            }
            cout << "PERMISSIONS: " << endl << sstream.str() << endl;
            return returnVector;
        }

        static string getAllPermissions() {
            __int64 allPerms;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::ADD_REACTIONS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::ATTACH_FILES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::BAN_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::CHANGE_NICKNAME;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::CONNECT;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::CREATE_INSTANT_INVITE;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::DEAFEN_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::EMBED_LINKS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::KICK_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_CHANNELS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_EMOJIS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_GUILD;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_MESSAGES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_NICKNAMES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_ROLES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_THREADS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MANAGE_WEBHOOKS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MENTION_EVERYONE;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MOVE_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::MUTE_MEMBERS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::PRIORITY_SPEAKER;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::READ_MESSAGE_HISTORY;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::REQUEST_TO_SPEAK;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::SEND_MESSAGES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::SEND_TTS_MESSAGES;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::SPEAK;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::STREAM;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_EXTERNAL_EMOJIS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_PRIVATE_THREADS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_PUBLIC_THREADS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_SLASH_COMMANDS;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::USE_VAD;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::VIEW_AUDIT_LOG;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::VIEW_CHANNEL;
            allPerms = allPerms | (__int64)DiscordCoreInternal::Permissions::VIEW_GUILD_INSIGHTS;
            stringstream stream;
            stream << allPerms;
            return stream.str();
        }

        static string getAllOfMyPerrmissions(GuildMember guildMember, Channel channel){
            string permsString = computePermissions(guildMember, channel);
            return permsString;
        }

        static bool checkForPermission(GuildMember guildMember, Channel channel, Permissions permission) {
            string permissionsString = computePermissions(guildMember, channel);
            if ((stoll(permissionsString) & (__int64)permission) == (__int64)permission) {
                return true;
            }
            else {
                return false;
            }
        }

    protected:
        static string computeBasePermissions(GuildMember guildMember, shared_ptr<DiscordCoreInternal::GuildManager> guilds, shared_ptr<DiscordCoreInternal::RoleManager> roles) {
            Guild guild = guilds->getGuildAsync({ guildMember.data.guildId }).get();

            if (guild.data.ownerID == guildMember.data.user.id) {
                return getAllPermissions();
            }

            Role everyone = roles->getRoleAsync({ .guildId = guild.data.id, .roleId = guild.data.id }).get();
            string permissionsString = everyone.data.permissions;
            __int64 permissionsInt = stoll(permissionsString);

            for (auto& role : guildMember.data.roles) {
                Role currentRole = roles->getRoleAsync({ .guildId = guild.data.id, .roleId = role }).get();
                permissionsInt |= stoll(currentRole.data.permissions);
            }

            if ((permissionsInt & (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) == (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) {
                return getAllPermissions();
            }
            stringstream stream;
            stream << permissionsInt;
            return stream.str();
        }

        static string  computeOverwrites(string basePermissions, GuildMember guildMember, Channel channel) {
            __int64 permissionsInt = stoll(basePermissions);
            if ((permissionsInt & (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) == (__int64)DiscordCoreInternal::Permissions::ADMINISTRATOR) {
                return getAllPermissions();
            }

            Guild guild = Guilds::getGuildAsync({ .guildId = guildMember.data.guildId }).get();

            DiscordCoreInternal::OverWriteData overwriteEveryone;
            if (channel.data.permissionOverwrites.contains(guild.data.id)) {
                overwriteEveryone = channel.data.permissionOverwrites.at(guild.data.id);
            }

            if (overwriteEveryone.id != "") {
                permissionsInt &= ~stoll(overwriteEveryone.deny);
                permissionsInt |= stoll(overwriteEveryone.allow);
            }

            map<string, OverWriteData> overWrites = channel.data.permissionOverwrites;
            __int64 allow = 0;
            __int64 deny = 0;
            for (auto& role : guildMember.data.roles) {
                Role currentRole = guildMember.discordCoreClient->roles->getRoleAsync({ .guildId = guildMember.data.guildId, .roleId = role }).get();
                if (overWrites.contains(currentRole.data.id)) {
                    allow |= stoll(overWrites.at(currentRole.data.id).allow);
                    deny |= stoll(overWrites.at(currentRole.data.id).deny);
                }
            }

            permissionsInt &= ~deny;
            permissionsInt |= allow;

            if (overWrites.contains(guildMember.data.user.id)) {
                permissionsInt &= ~stoll(overWrites.at(guildMember.data.user.id).deny);
                permissionsInt |= stoll(overWrites.at(guildMember.data.user.id).allow);
            }

            stringstream stream;
            stream << permissionsInt;
            return stream.str();
        }

        static string computePermissions(GuildMember guildMember, Channel channel) {
            string permissions;
            permissions = computeBasePermissions(guildMember, guildMember.discordCoreClient->guilds, guildMember.discordCoreClient->roles);
            permissions = computeOverwrites(permissions, guildMember, channel);
            return permissions;
        }
    };

    bool checkForBotCommanderStatus(GuildMember guildMember, DiscordUser discordUser) {
        bool areWeACommander;
        for (auto& value : discordUser.data.botCommanders) {
            if (guildMember.data.user.id == value) {
                areWeACommander = true;
                return areWeACommander;
                break;
            }
        }
        return false;
    }

    bool doWeHaveAdminPermissions(InputEventData eventData, DiscordGuild discordGuild, Channel channel, GuildMember guildMember, bool displayResponse = false) {
        bool doWeHaveAdmin = PermissionsConverter::checkForPermission(guildMember, channel, Permissions::ADMINISTRATOR);

        if (doWeHaveAdmin) {
            return true;
        }

        bool areWeACommander = checkForBotCommanderStatus(guildMember, *eventData.discordCoreClient->discordUser);

        if (areWeACommander) {
            return true;
        }

        if (displayResponse) {
            string msgString = "------\n**Sorry, but you don't have the permissions required for that!**\n------";
            EmbedData msgEmbed;
            msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.avatar);
            msgEmbed.setColor(discordGuild.data.borderColor);
            msgEmbed.setDescription(msgString);
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setTitle("__**Permissions Issue:**__");
            if (eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                ReplyMessageData responseData(eventData);
                responseData.addMessageEmbed(msgEmbed);
                InputEventData event01 = InputEvents::respondToEvent(responseData);
                InputEvents::deleteInputEventResponseAsync(event01, 20000);
            }
            else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                CreateInteractionResponseData responseData(eventData);
                responseData.addMessageEmbed(msgEmbed);
                InputEventData event = InputEvents::respondToEvent(responseData);
                InputEvents::deleteInputEventResponseAsync(event, 20000);
            }
        }        
        return false;
    }

    struct RecurseThroughMessagePagesData {
        InputEventData inputEventData;
        unsigned int currentPageIndex;
        string buttonId;
    };

    // Recurses through a succession of messages.
    RecurseThroughMessagePagesData recurseThroughMessagePages(string userID, InputEventData originalEvent, unsigned int currentPageIndex, vector<EmbedData> messageEmbeds, bool deleteAfter, unsigned int waitForMaxMs, bool returnFinalEmbed) {
        unsigned int newCurrentPageIndex = currentPageIndex;
        try {
            InputEventData event01 = originalEvent;
            bool doWeQuit = false;
            if (originalEvent.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || originalEvent.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
                EditMessageData responseDataRegularMessage(event01);
                responseDataRegularMessage.addMessageEmbed(messageEmbeds[currentPageIndex]);
                if (returnFinalEmbed) {
                    responseDataRegularMessage.addButton(false, "select", "Select", "✅", ButtonStyle::Success);
                }
                responseDataRegularMessage.addButton(false, "backwards", "Prev Page", "◀️", ButtonStyle::Primary);
                responseDataRegularMessage.addButton(false, "forwards", "Next Page", "▶️", ButtonStyle::Primary);

                responseDataRegularMessage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Danger);
                event01 = InputEvents::respondToEvent(responseDataRegularMessage);
            }
            else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
                || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
                EditInteractionResponseData editResponseData(event01);
                editResponseData.addMessageEmbed(messageEmbeds[currentPageIndex]);
                if (returnFinalEmbed) {
                    editResponseData.addButton(false, "select", "Select", "✅", ButtonStyle::Success);
                }
                editResponseData.addButton(false, "backwards", "Prev Page", "◀️", ButtonStyle::Primary);
                editResponseData.addButton(false, "forwards", "Next Page", "▶️", ButtonStyle::Primary);
                editResponseData.addButton(false, "exit", "Exit", "❌", ButtonStyle::Danger);
                event01 = InputEvents::respondToEvent(editResponseData);
            }
            else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
                EditFollowUpMessageData dataPackage(event01);
                dataPackage.addMessageEmbed(messageEmbeds[currentPageIndex]);
                if (returnFinalEmbed) {
                    dataPackage.addButton(false, "select", "Select", "✅", ButtonStyle::Success);
                }
                dataPackage.addButton(false, "backwards", "Prev Page", "◀️", ButtonStyle::Primary);
                dataPackage.addButton(false, "forwards", "Next Page", "▶️", ButtonStyle::Primary);
                dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Danger);
                event01 = InputEvents::respondToEvent(dataPackage);
            }

            while (doWeQuit == false) {
                ButtonManager button(event01);

                vector<ButtonResponseData> buttonIntData = button.collectButtonData(false, waitForMaxMs);
                if (buttonIntData.size() == 0 || buttonIntData.at(0).buttonId == "exit" || buttonIntData.at(0).buttonId == "") {
                    if (deleteAfter == true) {
                        InputEvents::deleteInputEventResponseAsync(event01);
                    }
                    else {
                        if (event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
                            EditMessageData dataPackage(event01);
                            for (auto value : event01.getEmbeds()) {
                                dataPackage.addMessageEmbed(value);
                            }
                            InputEvents::respondToEvent(dataPackage);
                        }
                        else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
                            || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
                            EditInteractionResponseData dataPackage(event01);
                            for (auto value : event01.getEmbeds()) {
                                dataPackage.addMessageEmbed(value);
                            }
                            InputEvents::respondToEvent(dataPackage);
                        }
                        else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
                            EditFollowUpMessageData dataPackage(event01);
                            for (auto value : event01.getEmbeds()) {
                                dataPackage.addMessageEmbed(value);
                            }
                            InputEvents::respondToEvent(dataPackage);
                        }
                    }
                    doWeQuit = true;
                    return RecurseThroughMessagePagesData();
                }
                if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
                    newCurrentPageIndex = 0;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
                        EditMessageData editMessageData(event01);
                        for (auto value : event01.getComponents()) {
                            editMessageData.addComponentRow(value);
                        }
                        editMessageData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(editMessageData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
                        || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
                        EditInteractionResponseData responseData(event01);
                        for (auto value : event01.getComponents()) {
                            responseData.addComponentRow(value);
                        }
                        responseData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(responseData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
                        EditFollowUpMessageData dataPackage(event01);
                        for (auto value : event01.getComponents()) {
                            dataPackage.addComponentRow(value);
                        }
                        dataPackage.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(dataPackage);
                    }
                }
                else if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
                    newCurrentPageIndex += 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
                        EditMessageData editMessageData(event01);
                        for (auto value : event01.getComponents()) {
                            editMessageData.addComponentRow(value);
                        }
                        editMessageData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(editMessageData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
                        || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
                        EditInteractionResponseData responseData(event01);
                        for (auto value : event01.getComponents()) {
                            responseData.addComponentRow(value);
                        }
                        responseData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(responseData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
                        EditFollowUpMessageData dataPackage(event01);
                        for (auto value : event01.getComponents()) {
                            dataPackage.addComponentRow(value);
                        }
                        dataPackage.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(dataPackage);
                    }
                }
                else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex > 0)) {
                    newCurrentPageIndex -= 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
                        EditMessageData editMessageData(event01);
                        for (auto value : event01.getComponents()) {
                            editMessageData.addComponentRow(value);
                        }
                        editMessageData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(editMessageData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
                        || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
                        EditInteractionResponseData responseData(event01);
                        for (auto value : event01.getComponents()) {
                            responseData.addComponentRow(value);
                        }
                        responseData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(responseData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
                        EditFollowUpMessageData dataPackage(event01);
                        for (auto value : event01.getComponents()) {
                            dataPackage.addComponentRow(value);
                        }
                        dataPackage.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(dataPackage);
                    }
                }
                else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex == 0)) {
                    newCurrentPageIndex = (unsigned int)messageEmbeds.size() - 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
                        EditMessageData editMessageData(event01);
                        for (auto value : event01.getComponents()) {
                            editMessageData.addComponentRow(value);
                        }
                        editMessageData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(editMessageData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
                        || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
                        EditInteractionResponseData responseData(event01);
                        for (auto value : event01.getComponents()) {
                            responseData.addComponentRow(value);
                        }
                        responseData.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(responseData);
                    }
                    else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
                        EditFollowUpMessageData dataPackage(event01);
                        for (auto value : event01.getComponents()) {
                            dataPackage.addComponentRow(value);
                        }
                        dataPackage.addMessageEmbed(messageEmbed);
                        event01 = InputEvents::respondToEvent(dataPackage);
                    }
                }
                else if (buttonIntData.at(0).buttonId == "select") {
                    if (deleteAfter == true) {
                        InputEvents::deleteInputEventResponseAsync(event01);
                    }
                    else {
                        if (event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || event01.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
                            EditMessageData dataPackage(event01);
                            for (auto value : event01.getEmbeds()) {
                                dataPackage.addMessageEmbed(value);
                            }
                            event01 = InputEvents::respondToEvent(dataPackage);
                        }
                        else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
                            || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
                            EditInteractionResponseData dataPackage(event01);
                            for (auto value : event01.getComponents()) {
                                dataPackage.addComponentRow(value);
                            }
                            event01 = InputEvents::respondToEvent(dataPackage);
                        }
                        else if (originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || originalEvent.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
                            EditFollowUpMessageData dataPackage(event01);
                            for (auto value : event01.getComponents()) {
                                dataPackage.addComponentRow(value);
                            }
                            event01 = InputEvents::respondToEvent(dataPackage);
                        }
                    }
                    doWeQuit = true;
                    RecurseThroughMessagePagesData returnData;
                    returnData.currentPageIndex = newCurrentPageIndex;
                    returnData.inputEventData = event01;
                    returnData.buttonId = buttonIntData.at(0).buttonId;
                    return returnData;
                }
            };
            return RecurseThroughMessagePagesData();
        }
        catch (exception& e) {
            cout << "recurseThroughMessagePages() Error: " << e.what() << endl << endl;
            return RecurseThroughMessagePagesData();
        }
    };

    float applyAsymptoticTransform(float inputModValue, float horizontalStretch, float ceiling) {
        float finalModValue = 0;
        float newInputModValue = inputModValue;
        if (newInputModValue == 0) {
            newInputModValue += 1;
        }
        if (newInputModValue <= 0) {
            float newInputValue = newInputModValue * -1;

            finalModValue = -1 * (float)trunc((ceiling * pow(newInputValue, 3)) / ((pow(newInputValue, 3) + horizontalStretch * newInputValue)));
            return finalModValue;
        }

        finalModValue = (float)trunc((ceiling * pow(newInputModValue, 3)) / ((pow(newInputModValue, 3) + horizontalStretch * newInputModValue)));

        return finalModValue;
    }

}
#endif