Queue {#Queue}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

    std::vector<EmbedData> updateMessageEmbeds(std::vector<Song> playlist, DiscordGuild discordGuild, InputEventData interaction,InputEventData originalEvent, int32_t currentPageIndex ){
        std::vector<std::vector<EmbedFieldData>> msgEmbedFields;
        msgEmbedFields.push_back(std::vector<EmbedFieldData>());
        int32_t msgEmbedFieldsPage{ 0 };
       for (int32_t y = 0; y < playlist.size(); y++) {
           if (y % 25 == 0 && y > 0) {
               msgEmbedFieldsPage++;
               msgEmbedFields.push_back(std::vector<EmbedFieldData>());
           }
           EmbedFieldData msgEmbedField = { .value = "__**Title:**__ [" + playlist.at(y).songTitle + "](" + playlist.at(y).viewUrl + ")\n__**Added By:**__ <@!" + playlist.at(y).addedByUserId + "> (" + playlist.at(y).addedByUserName + ")", .name = "__**" + std::to_string(y + 1) + " of " + std::to_string(playlist.size()) + "**__",.Inline = false };
           msgEmbedFields[msgEmbedFieldsPage].push_back(msgEmbedField);
       }
       msgEmbedFieldsPage = 0;
       std::vector<EmbedData> newMsgEmbeds;
       for (int32_t y = 0; y < msgEmbedFields.size(); y++) {
           EmbedData msgEmbed;
           msgEmbed.setAuthor(originalEvent.getUserName(), originalEvent.getAvatarUrl());
           msgEmbed.setColor(discordGuild.data.borderColor);
           msgEmbed.setTimeStamp(getTimeAndDate());
           msgEmbed.setTitle("__**Playlist, Page " + std::to_string(y + 1) + " of " + std::to_string(msgEmbedFields.size()) + "**__");
           msgEmbed.setFooter("React with ✅ to edit the contents of the current page. React with ❌ to exit!");
           msgEmbed.setDescription("__**React with ✅ to edit the contents of the current page. React with ❌ to exit!**__").fields = msgEmbedFields[y];
           newMsgEmbeds.push_back(msgEmbed);
       }
       RespondToInputEventData dataPackage(originalEvent);
       dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
       dataPackage.addMessageEmbed(newMsgEmbeds[currentPageIndex]);
       dataPackage.addContent("");
       dataPackage.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
       dataPackage.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
       dataPackage.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
       dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
       InputEvents::respondToInputEventAsync(dataPackage);
       return newMsgEmbeds;
    }

	class TheQueue :public DiscordCoreAPI::BaseFunction {
	public:

        TheQueue() {
			this->commandName = "queue";
            this->helpDescription = "View and edit the song queue.";
            EmbedData msgEmbed;
            msgEmbed.setDescription("------\nSimply enter /queue, and follow the instructions!\n------");
            msgEmbed.setTitle("__**Queue Usage:**__");
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setColor("FeFeFe");
            this->helpEmbed = msgEmbed;
		}

        std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
            return std::make_unique<TheQueue>();
        }

        virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
            try {
                Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();

                bool areWeInADm = areWeInADM(*args.eventData, channel);

                if (areWeInADm) {
                    return;
                }

                InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

                Guild guild = Guilds::getCachedGuildAsync({ args.eventData->getGuildId() }).get();
                DiscordGuild discordGuild(guild);

                bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(*args.eventData, discordGuild);

                if (!checkIfAllowedInChannel) {
                    return;
                }

                GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId() }).get();

                bool doWeHaveControl = checkIfWeHaveControl(*args.eventData, discordGuild, guildMember);

                if (!doWeHaveControl) {
                    return;
                }

                std::unique_ptr<InputEventData> newEvent{ std::make_unique<InputEventData>(*args.eventData) };

                loadPlaylist(discordGuild);

                if (SongAPI::getPlaylist(guild.id).songQueue.size() == 0) {
                    EmbedData msgEmbed;
                    msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
                    msgEmbed.setColor(discordGuild.data.borderColor);
                    msgEmbed.setTimeStamp(getTimeAndDate());
                    msgEmbed.setTitle("__**Empty Playlist:**__");
                    msgEmbed.setDescription("------\n__**Sorry, but there is nothing here to display!**__\n------");
                    RespondToInputEventData dataPackage(*args.eventData);
                    dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
                    dataPackage.addMessageEmbed(msgEmbed);
                    newEvent = InputEvents::respondToInputEventAsync(dataPackage);
                    return;
                }

                int32_t currentPageIndex = 0;
                RespondToInputEventData dataPackage(*args.eventData);
                dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
                newEvent = InputEvents::respondToInputEventAsync(dataPackage);

                std::vector<std::vector<EmbedFieldData>> msgEmbedFields;
                msgEmbedFields.push_back(std::vector<EmbedFieldData>());
                int32_t msgEmbedFieldsPage{ 0 };
                for (int32_t y = 0; y < SongAPI::getPlaylist(guild.id).songQueue.size(); y++) {
                    if (y % 25 == 0 && y > 0) {
                        if (y > 0) {
                            msgEmbedFieldsPage++;
                        }
                        msgEmbedFields.push_back(std::vector<EmbedFieldData>());
                    }
                    EmbedFieldData msgEmbedField = { .value = "__**Title:**__ [" + SongAPI::getPlaylist(guild.id).songQueue.at(y).songTitle + "](" + SongAPI::getPlaylist(guild.id).songQueue.at(y).viewUrl + ")\n__**Added By:**__ <@!" +
                        SongAPI::getPlaylist(guild.id).songQueue.at(y).addedByUserId + "> (" + SongAPI::getPlaylist(guild.id).songQueue.at(y).addedByUserName + ")",.name = "__**" + std::to_string(y + 1) + " of " + std::to_string(SongAPI::getPlaylist(guild.id).songQueue.size()) + "**__",.Inline = false };
                    msgEmbedFields[msgEmbedFieldsPage].push_back(msgEmbedField);
                }
                std::vector<EmbedData> msgEmbeds;
                msgEmbedFieldsPage = 0;
                for (int32_t y = 0; y < msgEmbedFields.size(); y++) {
                    EmbedData msgEmbed;
                    msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl()).setColor(discordGuild.data.borderColor).setTimeStamp(getTimeAndDate()).setTitle("__**Playlist, Page " + std::to_string(y + 1) + " of " + std::to_string(msgEmbedFields.size()) + "**__").
                        setFooter("React with ✅ to edit the contents of the current page. React with ❌ to exit!").setDescription("__**React with ✅ to edit the contents of the current page. React with ❌ to exit!**__").fields = msgEmbedFields[y];
                    msgEmbeds.push_back(msgEmbed);
                }
                RespondToInputEventData dataPackage02(*newEvent);
                dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                dataPackage02.addMessageEmbed(msgEmbeds[currentPageIndex]);
                dataPackage02.addContent("");
                dataPackage02.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
                dataPackage02.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
                dataPackage02.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
                dataPackage02.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
                newEvent = InputEvents::respondToInputEventAsync(dataPackage02);
                for (int32_t y = 0; y < 1; y) {
                    ButtonCollector button(*newEvent);
                    auto buttonCollectedData = button.collectButtonData(false, 120000, 1, args.eventData->getAuthorId()).get();
                    std::string userID = args.eventData->getAuthorId();
                    if (buttonCollectedData.size() == 0 || buttonCollectedData.at(0).buttonId == "exit" || buttonCollectedData.at(0).buttonId == "empty") {
                        RespondToInputEventData dataPackage(*newEvent);
                        dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        InputEvents::respondToInputEventAsync(dataPackage);
                        break;
                    }
                    else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex == (msgEmbeds.size()-1))) {
                        currentPageIndex = 0;
                        RespondToInputEventData dataPackage(buttonCollectedData.at(0).interactionData);
                        dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
                        dataPackage.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
                        dataPackage.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
                        dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
                        InputEvents::respondToInputEventAsync(dataPackage);
                        continue;
                    }
                    else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex < msgEmbeds.size())) {
                        currentPageIndex++;
                        RespondToInputEventData dataPackage(buttonCollectedData.at(0).interactionData);
                        dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
                        dataPackage.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
                        dataPackage.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
                        dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
                        InputEvents::respondToInputEventAsync(dataPackage);
                        continue;
                    }
                    else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex > 0)) {
                        currentPageIndex--;
                        RespondToInputEventData dataPackage(buttonCollectedData.at(0).interactionData);
                        dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
                        dataPackage.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
                        dataPackage.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
                        dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
                        InputEvents::respondToInputEventAsync(dataPackage);
                        continue;
                    }
                    else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex == 0)) {
                        currentPageIndex = (int32_t)msgEmbeds.size()-1;
                        RespondToInputEventData dataPackage(buttonCollectedData.at(0).interactionData);
                        dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                        dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                        dataPackage.addContent("");
                        dataPackage.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
                        dataPackage.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
                        dataPackage.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
                        dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
                        InputEvents::respondToInputEventAsync(dataPackage);
                        continue;
                    }
                    else if (buttonCollectedData.at(0).buttonId == "check") {
                        bool doWeQuit{ false };
                        msgEmbeds[currentPageIndex].setDescription("__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.__\n");
                        msgEmbeds[currentPageIndex].setFooter("Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                        while (!doWeQuit) {
                            RespondToInputEventData dataPackage(buttonCollectedData.at(0).interactionData);
                            dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                            dataPackage.addMessageEmbed(msgEmbeds[currentPageIndex]);
                            dataPackage.addContent("");
                            InputEvents::respondToInputEventAsync(dataPackage);

                            std::function<bool(Message)> messageFilter= [=](Message message)-> bool {
                                if (userID == message.author.id) {
                                    return true;
                                }
                                else {
                                    return false;
                                }
                            };

                            MessageCollector messageCollector;
                            auto returnedMessages = messageCollector.collectMessages(1, 120000, userID, messageFilter);
                            if (returnedMessages.get().messages.size() == 0) {
                                msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                                msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild.id).songQueue, discordGuild, *newEvent, *args.eventData, currentPageIndex);
                                doWeQuit = true;
                                break;
                            }
                            std::vector<std::string> args2;
                            std::string newString = convertToLowerCase(returnedMessages.get().messages.at(0).content);
                            std::regex wordRegex("[a-z]{1,12}");
                            std::smatch wordRegexMatch;
                            regex_search(newString, wordRegexMatch, wordRegex, std::regex_constants::match_flag_type::match_any | std::regex_constants::match_flag_type::match_not_null | std::regex_constants::match_flag_type::match_prev_avail);
                            args2.push_back(wordRegexMatch.str());
                            std::regex_iterator<const char*>::regex_type rx("\\d{1,4}");
                            std::regex_iterator<const char*> next(newString.c_str(), newString.c_str() + strlen(newString.c_str()), rx);
                            std::regex_iterator<const char*> end;

                            for (; next != end; ++next) {
                                args2.push_back(next->str());
                            }
                            std::regex digitRegex("\\d{1,3}");
                            if (args2.size() == 0 || convertToLowerCase(args2[0]) == "exit") {
                                Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                                msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild.id).songQueue, discordGuild, *newEvent, *args.eventData, currentPageIndex);
                                doWeQuit = true;
                                break;
                            }
                            else if (convertToLowerCase(args2[0]) != "remove" && convertToLowerCase(args2[0]) != "swap" && convertToLowerCase(args2[0]) != "exit" && convertToLowerCase(args2[0]) != "shuffle") {
                                msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                                msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                                Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                RespondToInputEventData dataPackage03(*newEvent);
                                dataPackage03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                                dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                dataPackage03.addContent("");
                                InputEvents::respondToInputEventAsync(dataPackage03);
                                continue;
                            }
                            else if (convertToLowerCase(args2[0]) == "remove") {
                                if (args2.size() < 2 || !regex_search(args2[1].c_str(), digitRegex)) {
                                    msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                                    msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                                    Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                    RespondToInputEventData dataPackage02(*newEvent);
                                    dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                                    dataPackage02.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                    dataPackage02.addContent("");
                                    InputEvents::respondToInputEventAsync(dataPackage02);
                                    continue;
                                }
                                if ((stoll(args2[1])-1) < 0 || (size_t)(stoll(args2[1])-1) >= SongAPI::getPlaylist(guild.id).songQueue.size() || args2.size() < 1) {
                                    msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                                    msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                                    Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                    RespondToInputEventData dataPackage02(*newEvent);
                                    dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                                    dataPackage02.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                    dataPackage02.addContent("");
                                    InputEvents::respondToInputEventAsync(dataPackage02);
                                    continue;
                                }
                                int32_t removeIndex = (int32_t)stoll(args2[1]);

                                auto playlist = SongAPI::getPlaylist(guild.id);
                                playlist.songQueue.erase(playlist.songQueue.begin() + removeIndex-1, playlist.songQueue.begin() + removeIndex);
                                SongAPI::setPlaylist(playlist, guild.id);
                                Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                                msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild.id).songQueue, discordGuild, *newEvent, *args.eventData, currentPageIndex);
                                doWeQuit = true;
                                savePlaylist(discordGuild);
                                break;
                            }
                            else if (convertToLowerCase(args2[0]) == "swap") {
                                if (args2.size() < 3 || !regex_search(args2[1].c_str(), digitRegex) || !regex_search(args2[2].c_str(), digitRegex)) {
                                    msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                                    msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                                    Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                    RespondToInputEventData dataPackage02(*newEvent);
                                    dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                                    dataPackage02.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                    dataPackage02.addContent("");
                                    InputEvents::respondToInputEventAsync(dataPackage02);
                                    continue;
                                }
                                if (args2.size() < 2 || ((stoll(args2[1])-1) < 0 || (size_t)(stoll(args2[1])-1) >= SongAPI::getPlaylist(guild.id).songQueue.size() || (stoll(args2[2])-1) < 0 || (size_t)(stoll(args2[2])-1) >= SongAPI::getPlaylist(guild.id).songQueue.size() || args2.size() < 2)) {
                                    msgEmbeds[currentPageIndex].setDescription("__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
                                    msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
                                    Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                    RespondToInputEventData dataPackage02(*newEvent);
                                    dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
                                    dataPackage02.addMessageEmbed(msgEmbeds[currentPageIndex]);
                                    dataPackage02.addContent("");
                                    InputEvents::respondToInputEventAsync(dataPackage02);
                                    continue;
                                }

                                int32_t sourceIndex = (int32_t)stoll(args2[1])-1;
                                int32_t destinationIndex = (int32_t)stoll(args2[2])-1;
                                SongAPI::modifyQueue(sourceIndex, destinationIndex, guild.id);
                                Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                                msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild.id).songQueue, discordGuild, *newEvent, *newEvent, currentPageIndex);
                                doWeQuit = true;
                                savePlaylist(discordGuild);
                                break;
                            }
                            else if (convertToLowerCase(args2[0]) == "shuffle") {
                                auto oldSongArray = SongAPI::getPlaylist(guild.id);
                                std::vector<Song> newVector;
                                while (oldSongArray.songQueue.size() > 0) {
                                    std::mt19937_64 randomEngine{ static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
                                    int32_t randomIndex = static_cast<uint32_t>((static_cast<float>(randomEngine())/static_cast<float>(randomEngine.max()) * static_cast<float>(oldSongArray.songQueue.size())));
                                    newVector.push_back(oldSongArray.songQueue.at(randomIndex));
                                    oldSongArray.songQueue.erase(oldSongArray.songQueue.begin() + randomIndex, oldSongArray.songQueue.begin() + randomIndex + 1);
                                }
                                oldSongArray.songQueue = newVector;
                                SongAPI::setPlaylist(oldSongArray, guild.id);
                                Messages::deleteMessageAsync({ .messageData = returnedMessages.get().messages[0],.reason = "Deleting the message!" }).get();
                                msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                                msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild.id).songQueue, discordGuild, *newEvent, *newEvent, currentPageIndex);
                                doWeQuit = true;
                                savePlaylist(discordGuild);
                                break;
                            }

                        }
                        if (doWeQuit) {
                            savePlaylist(discordGuild);
                            continue;
                        }

                    };
                    return;
                }
            }
            catch(...){
                reportException("Queue::execute Error: ");
            
            
            };
        };
        virtual ~TheQueue() ;

	};

};
```