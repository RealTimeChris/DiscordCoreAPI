// Queue.hpp - Header for the "Queue" command.
// Sep 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _QUEUE_
#define _QUEUE_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

    vector<EmbedData> updateMessageEmbeds(vector<YouTubeSong> playlist, DiscordGuild discordGuild, InputEventData interaction,InputEventData originalEvent, int currentPageIndex ){
        vector<vector<EmbedFieldData>> msgEmbedFields{};
        msgEmbedFields.push_back(vector<EmbedFieldData>());
        int msgEmbedFieldsPage{ 0 };
       for (int y = 0; y < playlist.size(); y += 1) {
           if (y % 25 == 0 && y > 0) {
               msgEmbedFieldsPage += 1;
               msgEmbedFields.push_back(vector<EmbedFieldData>());
           }
           EmbedFieldData msgEmbedField = { .Inline = false,.value = "__**Title:**__ [" + playlist.at(y).title + "](" + playlist.at(y).url + ")\n__**Added By:**__ <@!" + playlist.at(y).addedById + "> (" + playlist.at(y).addedByUserName + ")", .name = "__**" + to_string(y + 1) + " of " + to_string(playlist.size()) + "**__" };
           msgEmbedFields[msgEmbedFieldsPage].push_back(msgEmbedField);
       }
       msgEmbedFieldsPage = 0;
       vector<EmbedData> newMsgEmbeds{};
       for (int y = 0; y < msgEmbedFields.size(); y += 1) {
           EmbedData msgEmbed;
           msgEmbed.setAuthor(originalEvent.getUserName(), originalEvent.getAvatarURL());
           msgEmbed.setColor(discordGuild.data.borderColor);
           msgEmbed.setTimeStamp(getTimeAndDate());
           msgEmbed.setTitle("__**Playlist, Page " + to_string(y + 1) + " of " + to_string(msgEmbedFields.size()) + "**__");
           msgEmbed.setFooter("React with ✅ to edit the contents of the current page. React with ❌ to exit!");
           msgEmbed.setDescription("__**React with ✅ to edit the contents of the current page. React with ❌ to exit!**__")->fields = msgEmbedFields[y];
           newMsgEmbeds.push_back(msgEmbed);
       }
       if (interaction.eventType == InputEventType::REGULAR_MESSAGE) {
           EditMessageData dataPackage(interaction);
           dataPackage.addMessageEmbed(newMsgEmbeds[currentPageIndex]);
           dataPackage.addContent("");
           dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
           dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
           dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
           dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
           InputEvents::respondToEvent(dataPackage);
       }
       else {
           EditInteractionResponseData dataPackage(interaction);
           dataPackage.addMessageEmbed(newMsgEmbeds[currentPageIndex]);
           dataPackage.addContent("");
           dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
           dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
           dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
           dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
           InputEvents::respondToEvent(dataPackage);
       }
       return newMsgEmbeds;
    }

	class Queue :public BaseFunction {
	public:

		Queue() {
			this->commandName = "queue";
            this->helpDescription = "View and edit the song queue.";
            EmbedData msgEmbed;
            msgEmbed.setDescription("------\nSimply enter !queue or /queue, and follow the instructions!\n------");
            msgEmbed.setTitle("__**Queue Usage:**__");
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setColor("FeFeFe");
            this->helpEmbed = msgEmbed;
		}

		Queue* create() {
			return new Queue;
		}

        virtual task<void> execute(shared_ptr<BaseFunctionArguments>args) {
            Channel channel = Channels::getChannelAsync({ args->eventData.getChannelId() }).get();

            bool areWeInADm = areWeInADM(args->eventData, channel);

            if (areWeInADm) {
                co_return;
            }

            InputEvents::deleteInputEventResponseAsync(args->eventData).get();

            Guild guild = Guilds::getGuildAsync({ args->eventData.getGuildId() }).get();
            DiscordGuild discordGuild(guild);

            bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(args->eventData, discordGuild);

            if (!checkIfAllowedInChannel) {
                co_return;
            }

            GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();

            bool doWeHaveControl = checkIfWeHaveControl(args->eventData, discordGuild, guildMember);

            if (!doWeHaveControl) {
                co_return;
            }

            auto voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

            if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
                EmbedData newEmbed;
                newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
                newEmbed.setTimeStamp(getTimeAndDate());
                newEmbed.setTitle("__**Voice Channel Issue:**__");
                newEmbed.setColor(discordGuild.data.borderColor);
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.addMessageEmbed(newEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                    InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
                }
                else {
                    CreateEphemeralInteractionResponseData dataPackage(args->eventData);
                    dataPackage.addMessageEmbed(newEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                }
                co_return;
            }

            int currentPageIndex = 0;
            InputEventData newEvent{ args->eventData };
            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                ReplyMessageData dataPackage(args->eventData);
                dataPackage.addContent("test");
                newEvent = InputEvents::respondToEvent(dataPackage);
            }
            else {
                CreateInteractionResponseData dataPackage(args->eventData);
                dataPackage.addContent("test");
                newEvent = InputEvents::respondToEvent(dataPackage);
                CreateEphemeralFollowUpMessageData dataPackage02(newEvent);

                InputEvents::respondToEvent(dataPackage02);
            }

            if (YouTubeAPI::getQueue(guild.id)->size() == 0) {
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Empty Playlist:**__");
                msgEmbed.setDescription("------\n__**Sorry, but there is nothing here to display!**__\n------");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    EditMessageData dataPackage(newEvent);
                    dataPackage.addMessageEmbed(msgEmbed);
                    newEvent = InputEvents::respondToEvent(dataPackage);
                }
                else {
                    EditInteractionResponseData dataPackage(newEvent);
                    dataPackage.addMessageEmbed(msgEmbed);
                    newEvent = InputEvents::respondToEvent(dataPackage);
                }
                co_return;
            }

            vector<vector<EmbedFieldData>> msgEmbedFields;
            msgEmbedFields.push_back(vector<EmbedFieldData>());
            int msgEmbedFieldsPage{ 0 };
            for (int y = 0; y < YouTubeAPI::getQueue(guild.id)->size(); y += 1) {
                if (y % 25 == 0 && y > 0) {
                    if (y > 0) {
                        msgEmbedFieldsPage += 1;
                    }
                    msgEmbedFields.push_back(vector<EmbedFieldData>());
                }
                EmbedFieldData msgEmbedField = { .Inline = false,.value = "__**Title:**__ [" + YouTubeAPI::getQueue(guild.id)->at(y).title + "](" + YouTubeAPI::getQueue(guild.id)->at(y).url + ")\n__**Added By:**__ <@!" +
                    YouTubeAPI::getQueue(guild.id)->at(y).addedById + "> (" + YouTubeAPI::getQueue(guild.id)->at(y).addedByUserName + ")",.name = "__**" + to_string(y + 1) + " of " + to_string(YouTubeAPI::getQueue(guild.id)->size()) + "**__" };
                msgEmbedFields[msgEmbedFieldsPage].push_back(msgEmbedField);
            }
            vector<EmbedData> msgEmbeds;
            msgEmbedFieldsPage = 0;
            for (int y = 0; y < msgEmbedFields.size(); y += 1) {
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL())->setColor(discordGuild.data.borderColor)->setTimeStamp(getTimeAndDate())->setTitle("__**Playlist, Page " + to_string(y + 1) + " of " + to_string(msgEmbedFields.size()) + "**__")->
                    setFooter("React with ✅ to edit the contents of the current page. React with ❌ to exit!")->setDescription("__**React with ✅ to edit the contents of the current page. React with ❌ to exit!**__")->fields = msgEmbedFields[y];
                msgEmbeds.push_back(msgEmbed);
            }
            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                EditMessageData dataPackage(newEvent);
                dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                dataPackage.addContent("");
                dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                newEvent = InputEvents::respondToEvent(dataPackage);
            }
            else {
                EditInteractionResponseData dataPackage(newEvent);
                dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                dataPackage.addContent("");
                dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                newEvent = InputEvents::respondToEvent(dataPackage);
            }
            for (int y = 0; y < 1; y) {
                Button button(newEvent);
                auto buttonCollectedData = button.collectButtonData(false, 120000, args->eventData.getAuthorId());
                string userID = args->eventData.getAuthorId();
                if (buttonCollectedData.size() == 0 || buttonCollectedData.at(0).buttonId == "exit") {
                    if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                        EditMessageData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    else {
                        EditInteractionResponseData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    break;
                }
                else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex == (msgEmbeds.size() - 1))) {
                    currentPageIndex = 0;
                    if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                        EditMessageData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    else {
                        EditInteractionResponseData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    continue;
                }
                else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex < msgEmbeds.size())) {
                    currentPageIndex += 1;
                    if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                        EditMessageData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    else {
                        EditInteractionResponseData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    continue;
                }
                else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex > 0)) {
                    currentPageIndex -= 1;
                    if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                        EditMessageData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    else {
                        EditInteractionResponseData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    continue;
                }
                else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex == 0)) {
                    currentPageIndex = (int)msgEmbeds.size() - 1;
                    if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                        EditMessageData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    else {
                        EditInteractionResponseData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", "✅", ButtonStyle::Success);
                        dataPackage.addButton(false, "back", "Back", "◀️", ButtonStyle::Success);
                        dataPackage.addButton(false, "next", "Next", "▶️", ButtonStyle::Success);
                        dataPackage.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    continue;
                }
                else if (buttonCollectedData.at(0).buttonId == "check") {
                bool doWeQuit{ false };
                msgEmbeds[currentPageIndex].setDescription("__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.__\n");
                msgEmbeds[currentPageIndex].setFooter("Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                while (!doWeQuit) {
                    if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                        EditMessageData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }
                    else {
                        EditInteractionResponseData dataPackage(newEvent);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        newEvent = InputEvents::respondToEvent(dataPackage);
                    }

                    function<bool(DiscordCoreAPI::Message)> messageFilter = [=](DiscordCoreAPI::Message message)-> bool {
                        if (userID == message.author.id) {
                            return true;
                        }
                        else {
                            return false;
                        }
                    };

                    MessageCollector messageCollector(1, 120000, userID, messageFilter);
                    auto returnedMessages = messageCollector.collectMessages().get();
                    if (returnedMessages.messages.size() == 0) {
                        msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                        msgEmbeds = updateMessageEmbeds(*YouTubeAPI::getQueue(guild.id), discordGuild, newEvent, args->eventData, currentPageIndex);
                        doWeQuit = true;
                        break;
                    }
                    vector<string> args2;
                    string newString = convertToLowerCase(returnedMessages.messages.at(0).content);
                    regex wordRegex("[a-z]{1,12}");
                    smatch wordRegexMatch;
                    regex_search(newString, wordRegexMatch, wordRegex, regex_constants::match_flag_type::match_any | regex_constants::match_flag_type::match_not_null | regex_constants::match_flag_type::match_prev_avail);
                    args2.push_back(wordRegexMatch.str());
                    regex_iterator<const char*>::regex_type rx("\\d{1,4}");
                    regex_iterator<const char*> next(newString.c_str(), newString.c_str() + strlen(newString.c_str()), rx);
                    regex_iterator<const char*> end;

                    for (; next != end; ++next) {
                        args2.push_back(next->str());
                    }

                    if (args2.size() == 0 || convertToLowerCase(args2[0]) == "exit") {
                        DeleteMessageData dataPackage(returnedMessages.messages.at(0));
                        InputEvents::deleteInputEventResponseAsync(dataPackage);
                        msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                        msgEmbeds = updateMessageEmbeds(*YouTubeAPI::getQueue(guild.id), discordGuild, newEvent, args->eventData, currentPageIndex);
                        doWeQuit = true;
                        break;
                    }
                    else if (convertToLowerCase(args2[0]) != "remove" && convertToLowerCase(args2[0]) != "swap" && convertToLowerCase(args2[0]) != "exit" && convertToLowerCase(args2[0]) != "shuffle") {
                        msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                        msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                        DeleteMessageData dataPackage02(returnedMessages.messages.at(0));
                        InputEvents::deleteInputEventResponseAsync(dataPackage02);
                        if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                            EditMessageData dataPackage(newEvent);
                            dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                            dataPackage.addContent("");
                            newEvent = InputEvents::respondToEvent(dataPackage);
                        }
                        else {
                            EditInteractionResponseData dataPackage(newEvent);
                            dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                            dataPackage.addContent("");
                            newEvent = InputEvents::respondToEvent(dataPackage);
                        }
                        continue;
                    }
                    else if (convertToLowerCase(args2[0]) == "remove") {
                        if ((stoll(args2[1]) - 1) < 0 || (size_t)(stoll(args2[1]) - 1) >= YouTubeAPI::getQueue(guild.id)->size() || args2.size() < 1) {
                            msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                            msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                            DeleteMessageData dataPackage01(returnedMessages.messages.at(0));
                            InputEvents::deleteInputEventResponseAsync(dataPackage01);
                            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                                EditMessageData dataPackage(newEvent);
                                dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                dataPackage.addContent("");
                                newEvent = InputEvents::respondToEvent(dataPackage);
                            }
                            else {
                                EditInteractionResponseData dataPackage(newEvent);
                                dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                dataPackage.addContent("");
                                newEvent = InputEvents::respondToEvent(dataPackage);
                            }
                            continue;
                        }
                        int removeIndex = (int)stoll(args2[1]) - 1;

                        YouTubeAPI::getQueue(guild.id)->erase(YouTubeAPI::getQueue(guild.id)->begin() + removeIndex, YouTubeAPI::getQueue(guild.id)->begin() + removeIndex + 1);
                        DeleteMessageData dataPackage(returnedMessages.messages.at(0));
                        InputEvents::deleteInputEventResponseAsync(dataPackage);
                        msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                        msgEmbeds = updateMessageEmbeds(*YouTubeAPI::getQueue(guild.id), discordGuild, newEvent, args->eventData, currentPageIndex);
                        doWeQuit = true;
                        break;
                    }
                    else if (convertToLowerCase(args2[0]) == "swap") {
                        if ((stoll(args2[1]) - 1) < 0 || (size_t)(stoll(args2[1]) - 1) >= YouTubeAPI::getQueue(guild.id)->size() || (stoll(args2[2]) - 1) < 0 || (size_t)(stoll(args2[2]) - 1) >= YouTubeAPI::getQueue(guild.id)->size() || args2.size() < 2) {
                            msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                            msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                            DeleteMessageData dataPackage01(returnedMessages.messages.at(0));
                            InputEvents::deleteInputEventResponseAsync(dataPackage01);
                            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                                EditMessageData dataPackage(newEvent);
                                dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                dataPackage.addContent("");
                                newEvent = InputEvents::respondToEvent(dataPackage);
                            }
                            else {
                                EditInteractionResponseData dataPackage(newEvent);
                                dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                dataPackage.addContent("");
                                newEvent = InputEvents::respondToEvent(dataPackage);
                            }
                            continue;
                        }

                        int sourceIndex = (int)stoll(args2[1]) - 1;
                        int destinationIndex = (int)stoll(args2[2]) - 1;
                        YouTubeAPI::modifyQueue(sourceIndex, destinationIndex, guild.id);
                        DeleteMessageData dataPackage(returnedMessages.messages.at(0));
                        InputEvents::deleteInputEventResponseAsync(dataPackage);
                        msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                        msgEmbeds = updateMessageEmbeds(*YouTubeAPI::getQueue(guild.id), discordGuild, newEvent, args->eventData, currentPageIndex);
                        doWeQuit = true;
                        break;
                    }
                    else if (convertToLowerCase(args2[0]) == "shuffle") {
                        auto oldSongArray = YouTubeAPI::getQueue(guild.id);
                        vector<YouTubeSong> newVector{};
                        while (oldSongArray->size() > 0) {
                            srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
                            int randomIndex = (unsigned int)trunc(((float)rand() / (float)RAND_MAX) * oldSongArray->size());
                            newVector.push_back(oldSongArray->at(randomIndex));
                            oldSongArray->erase(oldSongArray->begin() + randomIndex, oldSongArray->begin() + randomIndex + 1);
                        }
                        YouTubeAPI::setQueue(newVector, guild.id);
                        DeleteMessageData dataPackage(returnedMessages.messages.at(0));
                        InputEvents::deleteInputEventResponseAsync(dataPackage);
                        msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                        msgEmbeds = updateMessageEmbeds(*YouTubeAPI::getQueue(guild.id), discordGuild, newEvent, args->eventData, currentPageIndex);
                        doWeQuit = true;
                        break;
                    }
                    
                }
                if (doWeQuit) {
                    continue;
                }
                    
                };
                co_return;
            };
        };

	};

};
#endif