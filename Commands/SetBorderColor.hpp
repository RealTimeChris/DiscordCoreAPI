// SetBorderColor.hpp - Header for the "set border color" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SET_BORDER_COLOR_
#define _SET_BORDER_COLOR_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

    class SetBorderColor :public BaseFunction {
    public:
        SetBorderColor() {
            this->commandName = "setbordercolor";
            this->helpDescription = "Set the bot's default border color for message embeds.";
            EmbedData msgEmbed;
            msgEmbed.setDescription("------\nSimply enter !setbordercolor = <BOTNAME>, <HEXCOLORVALUE> /setbordercolor <HEXCOLORVALUE>!\n------");
            msgEmbed.setTitle("__**Set Border Color Usage:**__");
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setColor("FeFeFe");
            this->helpEmbed = msgEmbed;
        }

        SetBorderColor* create() {
            return new SetBorderColor;
        }

        virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

            Channel channel = Channels::getChannelAsync({ args->eventData.getChannelId() }).get();

            bool areWeInADm = areWeInADM(args->eventData, channel);

            if (areWeInADm) {
                co_return;
            }

            Guild guild = Guilds::getGuildAsync({ args->eventData.getGuildId() }).get();
            DiscordGuild discordGuild(guild);

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
                    RespondToInputEventData dataPackage(args->eventData);
                    dataPackage.type = DesiredInputEventResponseType::RegularMessage;
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                    InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
                }
                else {
                    RespondToInputEventData dataPackage(args->eventData);
                    dataPackage.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                }
                co_return;
            }
            else if (args->argumentsArray[0] != "musichouse") {
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
                    RespondToInputEventData dataPackage(args->eventData);
                    dataPackage.type = DesiredInputEventResponseType::RegularMessage;
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                    InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
                }
                else {
                    RespondToInputEventData dataPackage(args->eventData);
                    dataPackage.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
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
                    RespondToInputEventData dataPackage(args->eventData);
                    dataPackage.type = DesiredInputEventResponseType::RegularMessage;
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                }
                else {
                    RespondToInputEventData dataPackage(args->eventData);
                    dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                }
                co_return;
            }
        };
    };
}
#endif
