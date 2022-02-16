// HelperFunctions.hpp - Header for some helper functions.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

#include <../Include/Index.hpp>

namespace DiscordCoreAPI {

    bool areWeInADM(InputEventData eventData, Channel channel, bool displayResponse = true) {
        auto currentChannelType = channel.type;
        if (currentChannelType == ChannelType::Dm) {
            if (displayResponse) {
                std::string msgString = "------\n**Sorry, but we can't do that in a direct message!**\n------";
                 std::shared_ptr<EmbedData> msgEmbed(new EmbedData());
                msgEmbed->setAuthor(eventData.getMessageData().interaction.user.userName, eventData.getMessageData().author.avatar);
                msgEmbed->setColor("FEFEFE");
                msgEmbed->setDescription(msgString);
                msgEmbed->setTimeStamp(getTimeAndDate());
                msgEmbed->setTitle("__**Direct Message Issue:**__");
                if (eventData.eventType == InputEventType::Regular_Message) {
                     std::shared_ptr<RespondToInputEventData> responseData(new RespondToInputEventData(eventData));
                    responseData->type = InputEventResponseType::Regular_Message;
                    responseData->addMessageEmbed(*msgEmbed);
                    auto event01 = InputEvents::respondToEvent(*responseData);
                    InputEvents::deleteInputEventResponseAsync(event01, 20000);
                }
                else if (eventData.eventType == InputEventType::Application_Command_Interaction) {
                     std::shared_ptr<RespondToInputEventData> responseData(new RespondToInputEventData(eventData));
                    responseData->type = InputEventResponseType::Ephemeral_Interaction_Response;
                    responseData->addMessageEmbed(*msgEmbed);
                    auto event01 = InputEvents::respondToEvent(*responseData);
                }
            }
            return true;
        }
        return false;
    }
}
#endif