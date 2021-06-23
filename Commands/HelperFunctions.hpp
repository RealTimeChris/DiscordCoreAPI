// HelperFunctions.hpp - Header for some helper functions.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELPER_FUNCTIONS_
#define _HELPER_FUNCTIONS_

#include "../pch.h"
#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

    string convertToLowerCase(string stringToConvert) {
        string newString;
        for (auto& value : stringToConvert) {
            if (isupper(value)) {
                newString += (char)tolower(value);
            }
            else {
                newString += value;
            }
        }
        return newString;
    }

    bool areWeInADM(InputEventData eventData, Channel channel) {
        auto currentChannelType = channel.data.type;

        if (currentChannelType == ChannelType::DM) {
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
                responseData.embeds.push_back(msgEmbed);
                event01 = InputEventManager::respondToEvent(responseData).get();
                InputEventManager::deleteInputEventResponse(event01, 20000).get();
            }
            else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                CreateInteractionResponseData responseData(eventData);
                responseData.data.embeds.push_back(msgEmbed);
                event01 = InputEventManager::respondToEvent(responseData).get();
                InputEventManager::deleteInputEventResponse(event01, 20000).get();
            }
            return true;
        }
        return false;
    }

    bool checkIfAllowedGamingInChannel(DiscordCoreAPI::InputEventData eventData,  DiscordGuild discordGuild) {
        bool isItFound = true;
        if (discordGuild.data.gameChannelIds.size() > 0) {
            isItFound = false;
            string msgString = "------\n**Sorry, but please do that in one of the following channels:**\n------\n";
            EmbedData msgEmbed;
            for (auto& value : discordGuild.data.gameChannelIds) {
                if (eventData.getChannelId() == value) {
                    isItFound = true;
                    break;
                }
                else {
                    msgString += "<#" + value + ">\n";
                }
            }
            msgString += "------";
            if (isItFound == false) {
                msgEmbed.setAuthor(eventData.getMessageData().author.username, eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTitle("__**Permissions Issue:**__");
                if (eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData replyMessageData(eventData);
                    replyMessageData.embeds.push_back(msgEmbed);
                    InputEventData event01 = InputEventManager::respondToEvent(replyMessageData).get();
                    InputEventManager::deleteInputEventResponse(event01, 20000).get();
                }
                else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                    CreateInteractionResponseData responseData(eventData);
                    responseData.data.embeds.push_back(msgEmbed);
                    InputEventData event01 = InputEventManager::respondToEvent(responseData).get();
                    InputEventManager::deleteInputEventResponse(event01, 20000).get();
                }
            }
            
        }
        return isItFound;
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
            for (auto value:permissionsToAdd) {
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
            for (auto value: permissionsToRemove) {
                permissionsInteger &= ~(__int64)value;
            }
            stringstream sstream;
            sstream << permissionsInteger;
            return sstream.str();
        }

        static void displayPermissions(string permissionString) {
            if (permissionString == "") {
                permissionString = "0";
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
            }
            if (permissionsInteger & (1 << 1)) {
                sstream << "KICK_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 2)) {
                sstream << "BAN_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 3)) {
                sstream << "ADMINISTRATOR" << endl;
            }
            if (permissionsInteger & (1 << 4)) {
                sstream << "MANAGE_CHANNELS" << endl;
            }
            if (permissionsInteger & (1 << 5)) {
                sstream << "MANAGE_GUILD" << endl;
            }
            if (permissionsInteger & (1 << 6)) {
                sstream << "ADD_REACTIONS" << endl;
            }
            if (permissionsInteger & (1 << 7)) {
                sstream << "VIEW_AUDIT_LOG" << endl;
            }
            if (permissionsInteger & (1 << 8)) {
                sstream << "PRIORITY_SPEAKER" << endl;
            }
            if (permissionsInteger & (1 << 9)) {
                sstream << "STREAM" << endl;
            }
            if (permissionsInteger & (1 << 10)) {
                sstream << "VIEW_CHANNEL" << endl;
            }
            if (permissionsInteger & (1 << 11)) {
                sstream << "SEND_MESSAGES" << endl;
            }
            if (permissionsInteger & (1 << 12)) {
                sstream << "SEND_TTS_MESSAGES" << endl;
            }
            if (permissionsInteger & (1 << 13)) {
                sstream << "MANAGE_MESSAGES" << endl;
            }
            if (permissionsInteger & (1 << 14)) {
                sstream << "EMBED_LINKS" << endl;
            }
            if (permissionsInteger & (1 << 15)) {
                sstream << "ATTACH_FILES" << endl;
            }
            if (permissionsInteger & (1 << 16)) {
                sstream << "READ_MESSAGE_HISTORY" << endl;
            }
            if (permissionsInteger & (1 << 17)) {
                sstream << "MENTION_EVERYONE" << endl;
            }
            if (permissionsInteger & (1 << 18)) {
                sstream << "USE_EXTERNAL_EMOJI" << endl;
            }
            if (permissionsInteger & (1 << 19)) {
                sstream << "VIEW_GUILD_INSIGHTS" << endl;
            }
            if (permissionsInteger & (1 << 20)) {
                sstream << "CONNECT" << endl;
            }
            if (permissionsInteger & (1 << 21)) {
                sstream << "SPEAK" << endl;
            }
            if (permissionsInteger & (1 << 22)) {
                sstream << "MUTE_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 23)) {
                sstream << "DEAFEN_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 24)) {
                sstream << "MOVE_MEMBERS" << endl;
            }
            if (permissionsInteger & (1 << 25)) {
                sstream << "USE_VAD" << endl;
            }
            if (permissionsInteger & (1 << 26)) {
                sstream << "CHANGE_NICKNAME" << endl;
            }
            if (permissionsInteger & (1 << 27)) {
                sstream << "MANAGE_NICKNAMES" << endl;
            }
            if (permissionsInteger & (1 << 28)) {
                sstream << "MANAGE_ROLES" << endl;
            }
            if (permissionsInteger & (1 << 29)) {
                sstream << "MANAGE_WEBHOOKS" << endl;
            }
            if (permissionsInteger & (1 << 30)) {
                sstream << "MANAGE_EMOJIS" << endl;
            }
            if (permissionsInteger & (1 << 31)) {
                sstream << "USE_SLASH_COMMANDS" << endl;
            }
            if (permissionsInteger & (1ull << 32)) {
                sstream << "REQUEST_TO_SPEAK" << endl;
            }
            if (permissionsInteger & (1ull << 34)) {
                sstream << "MANAGE_THREADS" << endl;
            }
            if (permissionsInteger & (1ull << 35)) {
                sstream << "USE_PUBLIC_THREADS" << endl;
            }
            if (permissionsInteger & (1ull << 36)) {
                sstream << "USE_PRIVATE_THREADS" << endl;
            }
            cout << "PERMISSIONS: " << endl << sstream.str() << endl;
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

        static string computeBasePermissions(GuildMember guildMember, GuildManager guilds, RoleManager roles) {
            Guild guild = guilds.getGuildAsync({ guildMember.data.guildId }).get();

            if (guild.data.ownerID == guildMember.data.user.id) {
                return getAllPermissions();
            }

            Role everyone = roles.getRoleAsync({ .guildId = guild.data.id, .roleId = guild.data.id }).get();
            string permissionsString = everyone.data.permissions;
            __int64 permissionsInt = stoll(permissionsString);

            for (auto& role : guildMember.data.roles) {
                Role currentRole = roles.getRoleAsync({ .guildId = guild.data.id, .roleId = role }).get();
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

            Guild guild = guildMember.discordCoreClient->guilds->getGuildAsync({ .guildId = guildMember.data.guildId }).get();

            DiscordCoreInternal::OverWriteData overwriteEveryone = channel.data.permissionOverwrites.at(guild.data.id);

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
            permissions = computeBasePermissions(guildMember, *guildMember.discordCoreClient->guilds, *guildMember.discordCoreClient->roles);
            permissions = computeOverwrites(permissions, guildMember, channel);
            return permissions;
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

    bool doWeHaveAdminPermissions(InputEventData eventData,DiscordGuild discordGuild, Channel channel, GuildMember guildMember) {
        bool doWeHaveAdmin = PermissionsConverter::checkForPermission(guildMember, channel, Permissions::ADMINISTRATOR);

        if (doWeHaveAdmin) {
            return true;
        }

        bool areWeACommander = checkForBotCommanderStatus(guildMember, *eventData.discordCoreClient->discordUser);

        if (areWeACommander) {
            return true;
        }

        string msgString = "------\n**Sorry, but you don't have the permissions required for that!**\n------";
        EmbedData msgEmbed;
        msgEmbed.setAuthor(guildMember.data.user.username, guildMember.data.user.avatar);
        msgEmbed.setColor(discordGuild.data.borderColor);
        msgEmbed.setDescription(msgString);
        msgEmbed.setTimeStamp(getTimeAndDate());
        msgEmbed.setTitle("__**Permissions Issue:**__");
        if (eventData.eventType == InputEventType::REGULAR_MESSAGE) {
            ReplyMessageData responseData(eventData);
            responseData.embeds.push_back(msgEmbed);
            InputEventData event01 = InputEventManager::respondToEvent(responseData).get();
            InputEventManager::deleteInputEventResponse(event01, 20000).get();
        }
        else if (eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
            CreateInteractionResponseData responseData(eventData);
            responseData.data.embeds.push_back(msgEmbed);
            InputEventData event = InputEventManager::respondToEvent(responseData).get();
            InputEventManager::deleteInputEventResponse(event, 20000).get();
        }
        return false;
    }

    // Recurses through a succession of messages.
    task<void> recurseThroughMessagePages(string userID, InputEventData originalEvent, unsigned int currentPageIndex, vector<EmbedData> messageEmbeds, bool deleteAfter, unsigned int waitForMaxMs) {
        apartment_context mainThread;
        co_await resume_background();
        unsigned int newCurrentPageIndex = currentPageIndex;
        try {
            InputEventData event01;
            bool doWeQuit = false;
            if (originalEvent.eventType == InputEventType::REGULAR_MESSAGE) {
                ReplyMessageData responseDataRegularMessage(originalEvent);
                responseDataRegularMessage.embeds.push_back(messageEmbeds[currentPageIndex]);
                responseDataRegularMessage.addButton(false, "backwards", "Prev Page", "◀️", ButtonStyle::Primary);
                responseDataRegularMessage.addButton(false, "forwards", "Next Page", "▶️", ButtonStyle::Primary);
                responseDataRegularMessage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Primary);
                event01 = InputEventManager::respondToEvent(responseDataRegularMessage).get();
            }
            else if (originalEvent.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                CreateInteractionResponseData responseDataInteraction(originalEvent);
                responseDataInteraction.data.embeds.push_back(messageEmbeds[currentPageIndex]);
                responseDataInteraction.addButton(false, "backwards", "Prev Page", "◀️", ButtonStyle::Primary);
                responseDataInteraction.addButton(false, "forwards", "Next Page", "▶️", ButtonStyle::Primary);
                responseDataInteraction.addButton(false, "exit", "Exit", "❌", ButtonStyle::Primary);
                event01 = InputEventManager::respondToEvent(responseDataInteraction).get();
                EditInteractionResponseData editResponseData(event01);
                editResponseData.embeds = responseDataInteraction.data.embeds;
                editResponseData.components = responseDataInteraction.data.components;
                event01 = InputEventManager::respondToEvent(editResponseData).get();
            }

            while (doWeQuit == false) {
                Button button(event01);

                ButtonInteractionData buttonIntData = button.getOurButtonData(false, waitForMaxMs);
                if (button.getButtonId() == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
                    newCurrentPageIndex = 0;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditMessageData editMessageData(event01);
                        editMessageData.components = event01.getComponents();
                        editMessageData.embeds.push_back(messageEmbed);
                        event01 = InputEventManager::respondToEvent(editMessageData).get();
                    }
                    else {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditInteractionResponseData responseData(event01);
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseData.components = event01.getComponents();
                        responseData.embeds = embeds;
                        event01 = InputEventManager::respondToEvent(responseData).get();
                    }
                }
                else if (button.getButtonId() == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
                    newCurrentPageIndex += 1;
                   EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditMessageData editMessageData(event01);
                        editMessageData.components = event01.getComponents();
                        editMessageData.embeds.push_back(messageEmbed);
                        event01 = InputEventManager::respondToEvent(editMessageData).get();

                    }
                    else {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditInteractionResponseData responseData(event01);
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseData.components = event01.getComponents();
                        responseData.embeds = embeds;
                        event01 = InputEventManager::respondToEvent(responseData).get();
                    }
                }
                else if (button.getButtonId() == "backwards" && (newCurrentPageIndex > 0)) {
                    newCurrentPageIndex -= 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditMessageData editMessageData(event01);
                        editMessageData.components = event01.getComponents();
                        editMessageData.embeds.push_back(messageEmbed);
                        event01 = InputEventManager::respondToEvent(editMessageData).get();
                    }
                    else {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditInteractionResponseData responseData(event01);
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseData.components = event01.getComponents();
                        responseData.embeds = embeds;
                        event01 = InputEventManager::respondToEvent(responseData).get();
                    }
                }
                else if (button.getButtonId() == "backwards" && (newCurrentPageIndex == 0)) {
                    newCurrentPageIndex = (unsigned int)messageEmbeds.size() - 1;
                    EmbedData messageEmbed = messageEmbeds[newCurrentPageIndex];
                    if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditMessageData editMessageData(event01);
                        editMessageData.components = event01.getComponents();
                        editMessageData.embeds.push_back(messageEmbed);
                        event01 = InputEventManager::respondToEvent(editMessageData).get();
                    }
                    else {
                        DeferButtonResponseData newData(buttonIntData);
                        InputEventManager::respondToEvent(newData).get();
                        EditInteractionResponseData responseData(event01);
                        vector<EmbedData> embeds;
                        embeds.push_back(messageEmbed);
                        responseData.components = event01.getComponents();
                        responseData.embeds = embeds;
                        event01 = InputEventManager::respondToEvent(responseData).get();
                    }
                }
                else if (button.getButtonId() == "exit" || button.getButtonId() == "") {
                    if (deleteAfter == true) {
                        InputEventManager::deleteInputEventResponse(event01);
                    }
                    else {
                        if (event01.eventType == InputEventType::REGULAR_MESSAGE) {
                            EditMessageData dataPackage(event01);
                            dataPackage.embeds = event01.getEmbeds();
                            dataPackage.embeds.at(0).setColor("00FE00");
                            InputEventManager::respondToEvent(dataPackage).get();
                        }
                        else {
                            EditInteractionResponseData dataPackage(event01);
                            dataPackage.embeds = event01.getEmbeds();
                            cout << dataPackage.embeds.at(0).hexColorValue << endl;
                            dataPackage.embeds.at(0).setColor("00FE00");
                            InputEventManager::respondToEvent(dataPackage).get();
                        }
                    }
                    doWeQuit = true;

                }
                
            }
            co_await mainThread;
            co_return;
        }
        catch (exception& e) {
            cout << "recurseThroughMessagePages() Error: " << e.what() << endl << endl;
            co_return;
        }
        
    };
    
    float applyAsymptoticTransform(float inputModValue, float horizontalStretch, float ceiling ){
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