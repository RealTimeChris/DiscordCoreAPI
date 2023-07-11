Queue {#Queue}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"
#include <regex>

namespace DiscordCoreAPI {

	std::vector<EmbedData> updateMessageEmbeds(std::vector<Song> playlist, DiscordGuild* discordGuild, InputEventData interaction, InputEventData originalEvent, User user,
		int32_t currentPageIndex) {
		std::vector<std::vector<EmbedFieldData>> msgEmbedFields{};
		msgEmbedFields.emplace_back(std::vector<EmbedFieldData>());
		int32_t msgEmbedFieldsPage{};
		for (int32_t y = 0; y < playlist.size(); y += 1) {
			if (y % 25 == 0 && y > 0) {
				msgEmbedFieldsPage += 1;
				msgEmbedFields.emplace_back(std::vector<EmbedFieldData>());
			}
			EmbedFieldData msgEmbedField{};
			msgEmbedField.Inline = false;
			msgEmbedField.value = "__**Title:**__ [" + playlist.at(y).songTitle + "](" + playlist.at(y).viewUrl + ")\n__**Added By:**__ <@!" +
				std::to_string(playlist.at(y).addedByUserId) + "> (" + playlist.at(y).addedByUserName + ")";

			msgEmbedField.name = "__**" + std::to_string(y + 1) + " of " + std::to_string(playlist.size()) + "**__";
			msgEmbedFields[msgEmbedFieldsPage].emplace_back(msgEmbedField);
		}
		msgEmbedFieldsPage = 0;
		std::vector<EmbedData> newMsgEmbeds{};
		for (int32_t y = 0; y < msgEmbedFields.size(); y += 1) {
			UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
			newEmbed->setAuthor(user.userName, user.avatar);
			newEmbed->setColor(discordGuild->data.borderColor);
			newEmbed->setTimeStamp(getTimeAndDate());
			newEmbed->setTitle("__**Playlist, Page " + std::to_string(y + 1) + " of " + std::to_string(msgEmbedFields.size()) + "**__");
			newEmbed->setFooter("React with ✅ to edit the contents of the current page. React with ❌ to exit!");
			newEmbed->setDescription("__**React with ✅ to edit the contents of the current page. React with ❌ to exit!**__").fields = msgEmbedFields[y];
			newMsgEmbeds.emplace_back(*newEmbed);
		}
		RespondToInputEventData dataPackage(originalEvent);
		dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
		dataPackage.addMessageEmbed(newMsgEmbeds[currentPageIndex]);
		dataPackage.addContent("");
		dataPackage.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
		dataPackage.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
		dataPackage.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
		dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
		InputEvents::respondToInputEventAsync(dataPackage).get();
		return newMsgEmbeds;
	}

	class TheQueue : public BaseFunction {
	  public:
		TheQueue() {
			commandName = "queue";
			helpDescription = "View and edit the song queue.";
			UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
			newEmbed->setDescription("------\nSimply enter /queue, and follow the instructions!\n------");
			newEmbed->setTitle("__**Queue Usage:**__");
			newEmbed->setTimeStamp(getTimeAndDate());
			newEmbed->setColor("FeFeFe");
			helpEmbed = *newEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<TheQueue>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {

				UniquePtr<Channel> channel{ makeUnique<Channel>(Channels::getCachedChannel({ newArgs.eventData.getChannelId() }).get()) };

				UniquePtr<Guild> guild{ makeUnique<Guild>(Guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get()) };
				UniquePtr<DiscordGuild> discordGuild(makeUnique<DiscordGuild>(*guild));

				bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(newArgs.eventData, *discordGuild);

				if (!checkIfAllowedInChannel) {
					return;
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, *discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}				

				InputEventData newEvent{ newArgs.eventData };
				loadPlaylist(*discordGuild);

				if (SongAPI::getPlaylist(guild->id).songQueue.size() == 0) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setColor(discordGuild->data.borderColor);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Empty Playlist:**__");
					newEmbed->setDescription("------\n__**Sorry, but there is nothing here to display!**__\n------");
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				int32_t currentPageIndex = 0;

				std::vector<std::vector<EmbedFieldData>> msgEmbedFields;
				msgEmbedFields.emplace_back(std::vector<EmbedFieldData>());
				int32_t msgEmbedFieldsPage{};
				for (int32_t y = 0; y < SongAPI::getPlaylist(guild->id).songQueue.size(); y += 1) {
					if (y % 25 == 0 && y > 0) {
						if (y > 0) {
							msgEmbedFieldsPage += 1;
						}
						msgEmbedFields.emplace_back(std::vector<EmbedFieldData>());
					}
					EmbedFieldData msgEmbedField{};
					msgEmbedField.Inline = false;
					msgEmbedField.value = "__**Title:**__ [" + SongAPI::getPlaylist(guild->id).songQueue.at(y).songTitle + "](" +
						SongAPI::getPlaylist(guild->id).songQueue.at(y).viewUrl + ")\n__**Added By:**__ <@!" +
						std::to_string(SongAPI::getPlaylist(guild->id).songQueue.at(y).addedByUserId) + "> (" + SongAPI::getPlaylist(guild->id).songQueue.at(y).addedByUserName +
						")";
					msgEmbedField.name = "__**" + std::to_string(y + 1) + " of " + std::to_string(SongAPI::getPlaylist(guild->id).songQueue.size()) + "**__";
					msgEmbedFields[msgEmbedFieldsPage].emplace_back(msgEmbedField);
				}
				std::vector<EmbedData> msgEmbeds;
				msgEmbedFieldsPage = 0;
				for (int32_t y = 0; y < msgEmbedFields.size(); y += 1) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setColor(discordGuild->data.borderColor)
						.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl())
						.setTimeStamp(getTimeAndDate())
						.setTitle("__**Playlist, Page " + std::to_string(y + 1) + " of " + std::to_string(msgEmbedFields.size()) + "**__")
						.setFooter("React with ✅ to edit the contents of the current page. React with ❌ to exit!")
						.setDescription("__**React with ✅ to edit the contents of the current page. React with ❌ to exit!**__")
						.fields = msgEmbedFields[y];
					msgEmbeds.emplace_back(*newEmbed);
				}
				RespondToInputEventData dataPackage0(newEvent);
				dataPackage0.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage0.addMessageEmbed(msgEmbeds[currentPageIndex]);
				dataPackage0.addContent("");
				dataPackage0.addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
				dataPackage0.addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
				dataPackage0.addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
				dataPackage0.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
				newEvent = InputEvents::respondToInputEventAsync(dataPackage0).get();
				for (int32_t y = 0; y < 1; y) {
					bool doWeQuit{};
					UniquePtr<ButtonCollector> button{ makeUnique<ButtonCollector>(newEvent) };
					auto buttonCollectedData = button->collectButtonData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					newEvent = *buttonCollectedData[0].interactionData;
					uint64_t userID = newArgs.eventData.getAuthorId();
					if (buttonCollectedData.size() == 0 || buttonCollectedData.at(0).buttonId == "exit" || buttonCollectedData.at(0).buttonId == "empty" || doWeQuit) {
						RespondToInputEventData dataPackage02(*buttonCollectedData.at(0).interactionData);
						dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02.addContent("");
						newEvent = InputEvents::respondToInputEventAsync(dataPackage02).get();
						break;
					} else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex == (msgEmbeds.size() - 1))) {
						currentPageIndex = 0;
						UniquePtr<RespondToInputEventData> dataPackage02{ makeUnique<RespondToInputEventData>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
						dataPackage02->addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
						dataPackage02->addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
						dataPackage02->addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
						newEvent = InputEvents::respondToInputEventAsync(*dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex < msgEmbeds.size())) {
						currentPageIndex += 1;
						UniquePtr<RespondToInputEventData> dataPackage02{ makeUnique<RespondToInputEventData>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
						dataPackage02->addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
						dataPackage02->addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
						dataPackage02->addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
						newEvent = InputEvents::respondToInputEventAsync(*dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex > 0)) {
						currentPageIndex -= 1;
						UniquePtr<RespondToInputEventData> dataPackage02{ makeUnique<RespondToInputEventData>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
						dataPackage02->addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
						dataPackage02->addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
						dataPackage02->addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
						newEvent = InputEvents::respondToInputEventAsync(*dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex == 0)) {
						currentPageIndex = ( int32_t )msgEmbeds.size() - 1;
						UniquePtr<RespondToInputEventData> dataPackage02{ makeUnique<RespondToInputEventData>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "Edit", ButtonStyle::Success, "✅");
						dataPackage02->addButton(false, "back", "Back", ButtonStyle::Success, "◀️");
						dataPackage02->addButton(false, "next", "Next", ButtonStyle::Success, "▶️");
						dataPackage02->addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
						newEvent = InputEvents::respondToInputEventAsync(*dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "check") {
						msgEmbeds[currentPageIndex].setDescription("__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> "
																   "<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle "
																   "the playlist.\nType 'exit' to exit.__\n");
						msgEmbeds[currentPageIndex].setFooter(
							"Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType "
							"'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
						newEvent = *buttonCollectedData.at(0).interactionData;
						UniquePtr<RespondToInputEventData> dataPackage03{ makeUnique<RespondToInputEventData>(*buttonCollectedData.at(0).interactionData) };
						dataPackage03->setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage03->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage03->addContent("");
						newEvent = InputEvents::respondToInputEventAsync(*dataPackage03).get();
						while (!doWeQuit) {
							UniquePtr<RespondToInputEventData> dataPackage02{ makeUnique<RespondToInputEventData>(newEvent) };
							dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
							dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
							dataPackage02->addContent("");
							newEvent = InputEvents::respondToInputEventAsync(*dataPackage02).get();

							std::function<bool(Message)> messageFilter = [=](Message message) -> bool {
								if (userID == message.author.id) {
									return true;
								} else {
									return false;
								}
							};
							User user = Users::getCachedUser({ newArgs.eventData.getAuthorId() }).get();
							UniquePtr<MessageCollector> messageCollector{ makeUnique<MessageCollector>() };
							auto returnedMessagesOld = messageCollector->collectMessages(1, 120000, messageFilter);
							auto returnedMessages = returnedMessagesOld.get();
							if (returnedMessages.messages.size() == 0) {
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds =
									updateMessageEmbeds(SongAPI::getPlaylist(guild->id).songQueue, discordGuild.get(), newEvent, newArgs.eventData, user, currentPageIndex);
								doWeQuit = true;
								break;
							}
							std::vector<std::string> args2;
							std::string newString = convertToLowerCase(returnedMessages.messages.at(0).content);
							std::regex wordRegex("[a-z]{1,12}");
							std::smatch wordRegexMatch;
							regex_search(newString, wordRegexMatch, wordRegex,
								std::regex_constants::match_flag_type::match_any | std::regex_constants::match_flag_type::match_not_null |
									std::regex_constants::match_flag_type::match_prev_avail);
							args2.emplace_back(wordRegexMatch.str());
							std::regex_iterator<const char*>::regex_type rx("\\d{1,4}");
							std::regex_iterator<const char*> next(newString.c_str(), newString.c_str() + strlen(newString.c_str()), rx);
							std::regex_iterator<const char*> end;

							for (; next != end; ++next) {
								args2.emplace_back(next->str());
							}

							std::regex digitRegex("\\d{1,3}");
							if (args2.size() == 0 || convertToLowerCase(args2[0]) == "exit") {
								Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "Deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
								doWeQuit = true;
								savePlaylist(*discordGuild);
								break;
							} else if (convertToLowerCase(args2[0]) != "remove" && convertToLowerCase(args2[0]) != "swap" && convertToLowerCase(args2[0]) != "exit" &&
								convertToLowerCase(args2[0]) != "shuffle") {
								msgEmbeds[currentPageIndex].setDescription(
									"__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> "
									"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
								msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType "
																	  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																	  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
								Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "Deleting the message!" })
									.get();
								RespondToInputEventData dataPackage03(newEvent);
								dataPackage03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
								dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
								dataPackage03.addContent("");
								newEvent = InputEvents::respondToInputEventAsync(dataPackage03).get();
								continue;
							} else if (convertToLowerCase(args2[0]) == "remove") {
								if (args2.size() < 2 || !regex_search(args2[1].c_str(), digitRegex)) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "Deleting the message!" })
										.get();
									RespondToInputEventData dataPackage03(newEvent);
									dataPackage03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = InputEvents::respondToInputEventAsync(dataPackage03).get();
									continue;
								}
								if ((stoll(args2[1]) - 1) < 0 || ( uint64_t )(stoll(args2[1]) - 1) >= SongAPI::getPlaylist(guild->id).songQueue.size() || args2.size() < 1) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "Deleting the message!" })
										.get();
									RespondToInputEventData dataPackage03(newEvent);
									dataPackage03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = InputEvents::respondToInputEventAsync(dataPackage03).get();
									continue;
								}
								int32_t removeIndex = ( int32_t )stoll(args2[1]);

								auto playlist = SongAPI::getPlaylist(guild->id);
								playlist.songQueue.erase(playlist.songQueue.begin() + removeIndex - 1, playlist.songQueue.begin() + removeIndex);
								SongAPI::setPlaylist(playlist, guild->id);
								Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "Deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
								doWeQuit = true;
								savePlaylist(*discordGuild);
								break;
							} else if (convertToLowerCase(args2[0]) == "swap") {
								if (args2.size() < 3 || !regex_search(args2[1].c_str(), digitRegex) || !regex_search(args2[2].c_str(), digitRegex)) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "Deleting the message!" })
										.get();
									RespondToInputEventData dataPackage03(newEvent);
									dataPackage03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = InputEvents::respondToInputEventAsync(dataPackage03).get();
									continue;
								}
								if (args2.size() < 2 ||
									((stoll(args2[1]) - 1) < 0 || ( uint64_t )(stoll(args2[1]) - 1) >= SongAPI::getPlaylist(guild->id).songQueue.size() ||
										(stoll(args2[2]) - 1) < 0 || ( uint64_t )(stoll(args2[2]) - 1) >= SongAPI::getPlaylist(guild->id).songQueue.size() || args2.size() < 2)) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**PLEASE ENTER A PROPER INPUT!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("PLEASE ENTER A PROPER INPUT!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "Deleting the message!" })
										.get();
									RespondToInputEventData dataPackage03(newEvent);
									dataPackage03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = InputEvents::respondToInputEventAsync(dataPackage03).get();
									continue;
								}

								int32_t sourceIndex = ( int32_t )stoll(args2[1]) - 1;
								int32_t destinationIndex = ( int32_t )stoll(args2[2]) - 1;
								SongAPI::modifyQueue(sourceIndex, destinationIndex, guild->id);
								Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "Deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
								doWeQuit = true;
								savePlaylist(*discordGuild);
								break;
							} else if (convertToLowerCase(args2[0]) == "shuffle") {
								auto oldSongArray = SongAPI::getPlaylist(guild->id);
								std::vector<Song> newVector{};
								while (oldSongArray.songQueue.size() > 0) {
									std::mt19937_64 randomEngine{ static_cast<uint32_t>(
										std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) };
									int32_t randomIndex = static_cast<uint32_t>(
										(static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max()) * static_cast<double>(oldSongArray.songQueue.size())));
									newVector.emplace_back(oldSongArray.songQueue.at(randomIndex));
									oldSongArray.songQueue.erase(oldSongArray.songQueue.begin() + randomIndex, oldSongArray.songQueue.begin() + randomIndex + 1);
								}
								oldSongArray.songQueue = newVector;
								SongAPI::setPlaylist(oldSongArray, guild->id);
								Messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "Deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(SongAPI::getPlaylist(guild->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
								doWeQuit = true;
								savePlaylist(*discordGuild);
								break;
							}
						}
						if (doWeQuit) {
							savePlaylist(*discordGuild);
							continue;
						}
					};
					return;
				}
			} catch (...) {
				reportException("Queue::execute()");
			};
		};
		~TheQueue(){};
	};

};
```