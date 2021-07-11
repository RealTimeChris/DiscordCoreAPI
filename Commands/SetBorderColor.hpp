// SetBorderColor.hpp - Header for the "set border color" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SET_BORDER_COLOR_
#define _SET_BORDER_COLOR_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

    class SetBorderColor :public BaseFunction {
    public:
        SetBorderColor() {
            this->commandName = "setbordercolor";
            this->helpDescription = "__**Set Border Color Usage:**__ !setbordercolor = HEXCOLORVALUE, or /setbordercolor HEXCOLORVALUE";
        }

        virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
            Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

            bool areWeInADm = areWeInADM(args->eventData, channel);

            if (areWeInADm) {
                co_return;
            }

            Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
            DiscordGuild discordGuild(guild.data);

            string borderColor;
            if (args->argumentsArray.size() == 0 || (args->argumentsArray.at(0) != "janny" && args->argumentsArray.at(0) != "gamehouse" && args->argumentsArray.at(0) != "musichouse")) {
                string msgString = "------\n**Please, enter a bot's name as the first argument to this command! (!setbordercolor = BOTNAME, HEXCOLORVALUE)**\n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                    InputEventManager::deleteInputEventResponse(newEvent, 20000);
                }
                else {
                    CreateEphemeralInteractionResponseData dataPackage(args->eventData);
                    dataPackage.data.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                }
                co_return;
            }
            else if (args->argumentsArray[0] != "gamehouse") {
                co_return;
            }
            if (stoll(args->argumentsArray[1], 0, 16) < 0 || stoll(args->argumentsArray[1], 0, 16) > stoll("fefefe", 0, 16)) {
                string msgString = "------\n**Please, enter a hex-colorvalue between 0 and FeFeFe! (!setbordercolor = BOTNAME, HEXCOLORVALUE)**\n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                    InputEventManager::deleteInputEventResponse(newEvent, 20000);
                }
                else {
                    CreateEphemeralInteractionResponseData dataPackage(args->eventData);
                    dataPackage.data.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                }
                co_return;
            }
            else {
                borderColor = args->argumentsArray[1];

                discordGuild.data.borderColor = borderColor;
                discordGuild.writeDataToDB();

                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription("Nicely done, you've updated the default border color for this bot!\n------\n__**Border Color Values:**__ " + discordGuild.data.borderColor + "\n------");
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Updated Border Color:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                }
                else {
                    CreateInteractionResponseData dataPackage(args->eventData);
                    dataPackage.data.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                }
                co_return;
            }
        };
    };
    SetBorderColor setBorderColor;
}
#endif
