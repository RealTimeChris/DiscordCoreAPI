// BotInfo.hpp - Header for the "bot info" command.
// Jun 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BOT_INFO_
#define _BOT_INFO_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {
    class BotInfo : public BaseFunction {
    public:
        BotInfo() {
            this->commandName = "botinfo";
            this->helpDescription = "Displays some info about this bot.";
            EmbedData msgEmbed;
            msgEmbed.setDescription("------\nEnter !botinfo = musichouse or /botinfo.\n------");
            msgEmbed.setTitle("__**Bot Info Usage:**__");
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setColor("FeFeFe");
            this->helpEmbed = msgEmbed;
        }

        BotInfo* create() {
            return new BotInfo;
        }

        virtual task<void>execute(shared_ptr<BaseFunctionArguments> args) {

            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE && Channels::getChannelAsync({ args->eventData.getChannelId() }).get().type != ChannelType::DM) {
                InputEvents::deleteInputEventResponseAsync(args->eventData);
            }
            else if (Channels::getChannelAsync({ args->eventData.getChannelId() }).get().type == ChannelType::DM || args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                args->argumentsArray.push_back("musichouse");
            }

            Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
            DiscordGuild discordGuild(guild);

            if (args->argumentsArray.size() == 0 || (args->argumentsArray.at(0) != "janny" && args->argumentsArray.at(0) != "musichouse" && args->argumentsArray.at(0) != "gamehouse")) {
                string msgString = "------\n**Please, enter the name of a bot as the first argument! (!botinfo = BOTNAME)**\n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Invalid Or Missing Arguments:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto eventNew = InputEvents::respondToEvent(dataPackage);
                    InputEvents::deleteInputEventResponseAsync(eventNew, 20000);
                }
                else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                    CreateEphemeralInteractionResponseData responseData(args->eventData);
                    responseData.addMessageEmbed(msgEmbed);
                    auto eventNew = InputEvents::respondToEvent(responseData);
                    InputEvents::deleteInputEventResponseAsync(eventNew, 20000);
                }
                co_return;
            }
            if (args->argumentsArray.at(0) != "musichouse") {
                co_return;
            }

            auto guilds = Guilds::getAllGuildsAsync().get();
            int userCount{ 0 };
            for (auto value : guilds) {
                userCount += value.memberCount;
            }

            EmbedData messageEmbed;
            messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
            messageEmbed.setImage(args->eventData.discordCoreClient->currentUser.avatar);
            messageEmbed.setColor("FEFEFE");
            messageEmbed.setTitle("__**Bot Info:**__");
            messageEmbed.setTimeStamp(getTimeAndDate());
            messageEmbed.addField("__Bot Name:__", args->eventData.discordCoreClient->currentUser.userName, true);
            messageEmbed.addField("__Bot ID:__", args->eventData.discordCoreClient->currentUser.id, true);
            messageEmbed.addField("__Guild Count:__", to_string(args->eventData.discordCoreClient->discordUser->data.guildCount), true);
            messageEmbed.addField("__Created At:__", args->eventData.discordCoreClient->currentUser.createdAt, true);
            messageEmbed.addField("__Serving Users:__", to_string(userCount), true);
            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                ReplyMessageData dataPackage(args->eventData);
                dataPackage.addMessageEmbed(messageEmbed);
                auto eventNew = InputEvents::respondToEvent(dataPackage);
            }
            else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                CreateInteractionResponseData dataPackage(args->eventData);
                dataPackage.addMessageEmbed(messageEmbed);
                auto eventNew = InputEvents::respondToEvent(dataPackage);
            }
            co_return;
        }

    };
}
#endif