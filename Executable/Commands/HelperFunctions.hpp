// HelperFunctions.hpp - Header for some helper functions.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

namespace DiscordCoreAPI {

    bool areWeInADM(InputEventData eventData, Channel channel, bool displayResponse = true) {
        auto currentChannelType = channel.type;
        if (currentChannelType == ChannelType::Dm) {
            if (displayResponse) {
                std::string msgString = "------\n**Sorry, but we can't do that in a direct message!**\n------";
                std::unique_ptr<EmbedData> msgEmbed(new EmbedData());
                msgEmbed->setAuthor(eventData.getMessageData().interaction.user.userName, eventData.getMessageData().author.avatar);
                msgEmbed->setColor("FEFEFE");
                msgEmbed->setDescription(msgString);
                msgEmbed->setTimeStamp(getTimeAndDate());
                msgEmbed->setTitle("__**Direct Message Issue:**__");
                if (eventData.eventType == InputEventType::Regular_Message) {
                    std::unique_ptr<RespondToInputEventData> responseData(new RespondToInputEventData(eventData));
                    responseData->type = InputEventResponseType::Regular_Message;
                    responseData->addMessageEmbed(*msgEmbed);
                    auto event01 = InputEvents::respondToEvent(*responseData);
                    InputEvents::deleteInputEventResponseAsync(std::move(event01), 20000);
                }
                else if (eventData.eventType == InputEventType::Application_Command_Interaction) {
                    std::unique_ptr<RespondToInputEventData> responseData(new RespondToInputEventData(eventData));
                    responseData->type = InputEventResponseType::Ephemeral_Interaction_Response;
                    responseData->addMessageEmbed(*msgEmbed);
                    auto event01 = InputEvents::respondToEvent(*responseData);
                }
            }
            return true;
        }
        return false;
    }

    float applyAsymptoticTransform(float inputModValue, float horizontalStretch, float ceiling) {
        float finalModValue = 0;
        float newInputModValue = inputModValue;
        if (newInputModValue == 0) {
            newInputModValue += 1;
        }
        if (newInputModValue <= 0) {
            float newInputValue = newInputModValue * -1;

            finalModValue = -1 * (float)trunc((ceiling * pow(newInputValue, 3)) / ((pow(newInputValue, 3) + (int64_t)horizontalStretch * (int64_t)newInputValue)));
            return finalModValue;
        }

        finalModValue = (float)trunc((ceiling * pow(newInputModValue, 3)) / ((pow(newInputModValue, 3) + (int64_t)horizontalStretch * (int64_t)newInputModValue)));

        return finalModValue;
    }

}
