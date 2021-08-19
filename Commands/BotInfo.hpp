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
			this->helpDescription = "__**Bot Info Usage:**__ Enter !botinfo or /botinfo";
		}

        BotInfo* create() {
            return new BotInfo;
        }

        virtual task<void>execute(shared_ptr<BaseFunctionArguments> args) {

            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE && ChannelStuff::getChannelAsync({ args->eventData.getChannelId() }).get().data.type != ChannelType::DM) {
                InputEventStuff::deleteInputEventResponseAsync(args->eventData);
            }
            else if(ChannelStuff::getChannelAsync({ args->eventData.getChannelId() }).get().data.type == ChannelType::DM || args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION){
                args->argumentsArray.push_back("gamehouse");
            }

            Guild guild = GuildStuff::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
            DiscordGuild discordGuild(guild.data);

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
                    dataPackage.embeds.push_back(msgEmbed);
                    auto eventNew = InputEventStuff::respondToEvent(dataPackage);
                    InputEventStuff::deleteInputEventResponseAsync(eventNew, 20000);
                }
                else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                    CreateEphemeralInteractionResponseData responseData(args->eventData);
                    responseData.data.embeds.push_back(msgEmbed);
                    auto eventNew = InputEventStuff::respondToEvent(responseData);
                    InputEventStuff::deleteInputEventResponseAsync(eventNew, 20000);
                }
                co_return;
            }
            if (args->argumentsArray.at(0) != "gamehouse") {
                co_return;
            }

            EmbedData messageEmbed;
            messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
            messageEmbed.setImage(args->eventData.discordCoreClient->currentUser->data.avatar);
            messageEmbed.setColor("FEFEFE");
            messageEmbed.setTitle("__**Bot Info:**__");
            messageEmbed.setTimeStamp(getTimeAndDate());
            messageEmbed.addField("__Bot Name:__", args->eventData.discordCoreClient->currentUser->data.username, true);
            messageEmbed.addField("__Bot ID:__", args->eventData.discordCoreClient->currentUser->data.id, true);
            messageEmbed.addField("__Guild Count:__", to_string(args->eventData.discordCoreClient->discordUser->data.guildCount), true);
            messageEmbed.addField("__Created At:__", args->eventData.discordCoreClient->currentUser->data.createdAt, true);
            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                ReplyMessageData dataPackage(args->eventData);
                dataPackage.embeds.push_back(messageEmbed);
                auto eventNew = InputEventStuff::respondToEvent(dataPackage);
            }
            else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
                CreateInteractionResponseData dataPackage(args->eventData);
                dataPackage.data.embeds.push_back(messageEmbed);
                auto eventNew = InputEventStuff::respondToEvent(dataPackage);
            }
            co_return;
		}

    };
}
#endif