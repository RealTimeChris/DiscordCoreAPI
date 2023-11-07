Queue {#Queue}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"
#include <regex>

namespace discord_core_api {

	jsonifier::vector<embed_data> updateMessageEmbeds(jsonifier::vector<song> playlist, discord_guild* discordGuild, input_event_data interaction, input_event_data originalEvent, user user,
		int32_t currentPageIndex) {
		jsonifier::vector<jsonifier::vector<embed_field_data>> msgEmbedFields{};
		msgEmbedFields.emplace_back(jsonifier::vector<embed_field_data>());
		int32_t msgEmbedFieldsPage{};
		for (int32_t y = 0; y < playlist.size(); y += 1) {
			if (y % 25 == 0 && y > 0) {
				msgEmbedFieldsPage += 1;
				msgEmbedFields.emplace_back(jsonifier::vector<embed_field_data>());
			}
			embed_field_data msgEmbedField{};
			msgEmbedField.inline = false;
			msgEmbedField.value = "__**title:**__ [" + playlist.at(y).songTitle + "](" + playlist.at(y).viewUrl + ")\n__**added by:**__ <@!" +
				jsonifier::toString(playlist.at(y).addedByUserId) + "> (" + playlist.at(y).addedByUserName + ")";

			msgEmbedField.name = "__**" + jsonifier::toString(y + 1) + " of " + jsonifier::toString(playlist.size()) + "**__";
			msgEmbedFields[msgEmbedFieldsPage].emplace_back(msgEmbedField);
		}
		msgEmbedFieldsPage = 0;
		jsonifier::vector<embed_data> newMsgEmbeds{};
		for (int32_t y = 0; y < msgEmbedFields.size(); y += 1) {
			unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
			newEmbed->setAuthor(user.userName, user.avatar);
			newEmbed->setColor(discordGuild->data.borderColor);
			newEmbed->setTimeStamp(getTimeAndDate());
			newEmbed->setTitle("__**playlist, page " + jsonifier::toString(y + 1) + " of " + jsonifier::toString(msgEmbedFields.size()) + "**__");
			newEmbed->setFooter("react with ✅ to edit the contents of the current page. react with ❌ to exit!");
			newEmbed->setDescription("__**react with ✅ to edit the contents of the current page. react with ❌ to exit!**__").fields = msgEmbedFields[y];
			newMsgEmbeds.emplace_back(*newEmbed);
		}
		respond_to_input_event_data& dataPackage(originalEvent);
		dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
		dataPackage.addMessageEmbed(newMsgEmbeds[currentPageIndex]);
		dataPackage.addContent("");
		dataPackage.addButton(false, "check", "edit", button_style::success, "✅");
		dataPackage.addButton(false, "back", "back", button_style::success, "◀️");
		dataPackage.addButton(false, "next", "next", button_style::success, "▶️");
		dataPackage.addButton(false, "exit", "exit", button_style::success, "❌");
		input_events::respondToInputEventAsync(const& dataPackage).get();
		return newMsgEmbeds;
	}

	class the_queue : public base_function {
	  public:
		the_queue() {
			commandName = "queue";
			helpDescription = "view and edit the song queue.";
			unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
			newEmbed->setDescription("------\nSimply enter /queue, and follow the instructions!\n------");
			newEmbed->setTitle("__**queue usage:**__");
			newEmbed->setTimeStamp(getTimeAndDate());
			newEmbed->setColor("fe_fe_fe");
			helpEmbed = *newEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<the_queue>();
		}

		void execute(base_function_arguments& newArgs) {
			try {

				unique_ptr<discord_core_api::channel_data> channel{ makeUnique<discord_core_api::channel_data>(discord_core_api::channels::getCachedChannel({ newArgs.eventData.getChannelId() }).get()) };

				unique_ptr<guild_data> guild_data{ makeUnique<guild_data>(guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get()) };
				unique_ptr<discord_guild> discordGuild(makeUnique<discord_guild>(*guild_data));

				bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(newArgs.eventData, *discordGuild);

				if (!checkIfAllowedInChannel) {
					return;
				}

				guild_member_data guildMember =
					guild_members::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, *discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}				

				input_event_data newEvent{ newArgs.eventData };
				loadPlaylist(*discordGuild);

				if (song_api::getPlaylist(guild_data->id).songQueue.size() == 0) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setColor(discordGuild->data.borderColor);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**empty playlist:**__");
					newEmbed->setDescription("------\n__**sorry, but there is nothing here to display!**__\n------");
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					return;
				}

				int32_t currentPageIndex = 0;

				jsonifier::vector<jsonifier::vector<embed_field_data>> msgEmbedFields;
				msgEmbedFields.emplace_back(jsonifier::vector<embed_field_data>());
				int32_t msgEmbedFieldsPage{};
				for (int32_t y = 0; y < song_api::getPlaylist(guild_data->id).songQueue.size(); y += 1) {
					if (y % 25 == 0 && y > 0) {
						if (y > 0) {
							msgEmbedFieldsPage += 1;
						}
						msgEmbedFields.emplace_back(jsonifier::vector<embed_field_data>());
					}
					embed_field_data msgEmbedField{};
					msgEmbedField.inline = false;
					msgEmbedField.value = "__**title:**__ [" + song_api::getPlaylist(guild_data->id).songQueue.at(y).songTitle + "](" +
						song_api::getPlaylist(guild_data->id).songQueue.at(y).viewUrl + ")\n__**added by:**__ <@!" +
						jsonifier::toString(song_api::getPlaylist(guild_data->id).songQueue.at(y).addedByUserId) + "> (" + song_api::getPlaylist(guild_data->id).songQueue.at(y).addedByUserName +
						")";
					msgEmbedField.name = "__**" + jsonifier::toString(y + 1) + " of " + jsonifier::toString(song_api::getPlaylist(guild_data->id).songQueue.size()) + "**__";
					msgEmbedFields[msgEmbedFieldsPage].emplace_back(msgEmbedField);
				}
				jsonifier::vector<embed_data> msgEmbeds;
				msgEmbedFieldsPage = 0;
				for (int32_t y = 0; y < msgEmbedFields.size(); y += 1) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setColor(discordGuild->data.borderColor)
						.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl())
						.setTimeStamp(getTimeAndDate())
						.setTitle("__**playlist, page " + jsonifier::toString(y + 1) + " of " + jsonifier::toString(msgEmbedFields.size()) + "**__")
						.setFooter("react with ✅ to edit the contents of the current page. react with ❌ to exit!")
						.setDescription("__**react with ✅ to edit the contents of the current page. react with ❌ to exit!**__")
						.fields = msgEmbedFields[y];
					msgEmbeds.emplace_back(*newEmbed);
				}
				respond_to_input_event_data& dataPackage0(newEvent);
				dataPackage0.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage0.addMessageEmbed(msgEmbeds[currentPageIndex]);
				dataPackage0.addContent("");
				dataPackage0.addButton(false, "check", "edit", button_style::success, "✅");
				dataPackage0.addButton(false, "back", "back", button_style::success, "◀️");
				dataPackage0.addButton(false, "next", "next", button_style::success, "▶️");
				dataPackage0.addButton(false, "exit", "exit", button_style::success, "❌");
				newEvent = input_events::respondToInputEventAsync(const dataPackage0).get();
				for (int32_t y = 0; y < 1; y) {
					bool doWeQuit{};
					unique_ptr<button_collector> button{ makeUnique<button_collector>(newEvent) };
					auto buttonCollectedData = button->collectButtonData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					newEvent = *buttonCollectedData[0].interactionData;
					uint64_t userID = newArgs.eventData.getAuthorId();
					if (buttonCollectedData.size() == 0 || buttonCollectedData.at(0).buttonId == "exit" || buttonCollectedData.at(0).buttonId == "empty" || doWeQuit) {
						respond_to_input_event_data& dataPackage02(*buttonCollectedData.at(0).interactionData);
						dataPackage02.setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02.addContent("");
						newEvent = input_events::respondToInputEventAsync(const dataPackage02).get();
						break;
					} else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex == (msgEmbeds.size() - 1))) {
						currentPageIndex = 0;
						unique_ptr<respond_to_input_event_data> dataPackage02{ makeUnique<respond_to_input_event_data>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "edit", button_style::success, "✅");
						dataPackage02->addButton(false, "back", "back", button_style::success, "◀️");
						dataPackage02->addButton(false, "next", "next", button_style::success, "▶️");
						dataPackage02->addButton(false, "exit", "exit", button_style::success, "❌");
						newEvent = input_events::respondToInputEventAsync(const *dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "next" && (currentPageIndex < msgEmbeds.size())) {
						currentPageIndex += 1;
						unique_ptr<respond_to_input_event_data> dataPackage02{ makeUnique<respond_to_input_event_data>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "edit", button_style::success, "✅");
						dataPackage02->addButton(false, "back", "back", button_style::success, "◀️");
						dataPackage02->addButton(false, "next", "next", button_style::success, "▶️");
						dataPackage02->addButton(false, "exit", "exit", button_style::success, "❌");
						newEvent = input_events::respondToInputEventAsync(const *dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex > 0)) {
						currentPageIndex -= 1;
						unique_ptr<respond_to_input_event_data> dataPackage02{ makeUnique<respond_to_input_event_data>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "edit", button_style::success, "✅");
						dataPackage02->addButton(false, "back", "back", button_style::success, "◀️");
						dataPackage02->addButton(false, "next", "next", button_style::success, "▶️");
						dataPackage02->addButton(false, "exit", "exit", button_style::success, "❌");
						newEvent = input_events::respondToInputEventAsync(const *dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "back" && (currentPageIndex == 0)) {
						currentPageIndex = ( int32_t )msgEmbeds.size() - 1;
						unique_ptr<respond_to_input_event_data> dataPackage02{ makeUnique<respond_to_input_event_data>(*buttonCollectedData.at(0).interactionData) };
						dataPackage02->setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage02->addContent("");
						dataPackage02->addButton(false, "check", "edit", button_style::success, "✅");
						dataPackage02->addButton(false, "back", "back", button_style::success, "◀️");
						dataPackage02->addButton(false, "next", "next", button_style::success, "▶️");
						dataPackage02->addButton(false, "exit", "exit", button_style::success, "❌");
						newEvent = input_events::respondToInputEventAsync(const *dataPackage02).get();
						continue;
					} else if (buttonCollectedData.at(0).buttonId == "check") {
						msgEmbeds[currentPageIndex].setDescription("__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> "
																   "<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle "
																   "the playlist.\nType 'exit' to exit.__\n");
						msgEmbeds[currentPageIndex].setFooter(
							"type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> <destinationTrackNumber>' to swap tracks.\nType "
							"'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
						newEvent = *buttonCollectedData.at(0).interactionData;
						unique_ptr<respond_to_input_event_data> dataPackage03{ makeUnique<respond_to_input_event_data>(*buttonCollectedData.at(0).interactionData) };
						dataPackage03->setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage03->addMessageEmbed(msgEmbeds[currentPageIndex]);
						dataPackage03->addContent("");
						newEvent = input_events::respondToInputEventAsync(const *dataPackage03).get();
						while (!doWeQuit) {
							unique_ptr<respond_to_input_event_data> dataPackage02{ makeUnique<respond_to_input_event_data>(newEvent) };
							dataPackage02->setResponseType(input_event_response_type::Edit_Interaction_Response);
							dataPackage02->addMessageEmbed(msgEmbeds[currentPageIndex]);
							dataPackage02->addContent("");
							newEvent = input_events::respondToInputEventAsync(const *dataPackage02).get();

							std::function<bool(message)> messageFilter = [=](message message) -> bool {
								if (userID == message.author.id) {
									return true;
								} else {
									return false;
								}
							};
							user user = users::getCachedUser({ newArgs.eventData.getAuthorId() }).get();
							unique_ptr<message_collector> messageCollector{ makeUnique<message_collector>() };
							auto returnedMessagesOld = messageCollector->collectObjects(1, 120000, messageFilter);
							auto returnedMessages = returnedMessagesOld.get();
							if (returnedMessages.messages.size() == 0) {
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds =
									updateMessageEmbeds(song_api::getPlaylist(guild_data->id).songQueue, discordGuild.get(), newEvent, newArgs.eventData, user, currentPageIndex);
								doWeQuit = true;
								break;
							}
							jsonifier::vector<jsonifier::string> args2;
							jsonifier::string newString = convertToLowerCase(returnedMessages.messages.at(0).content);
							std::regex wordRegex("[a-z]{1,12}");
							std::smatch wordRegexMatch;
							regex_search(newString, wordRegexMatch, wordRegex,
								std::regex_constants::match_flag_type::match_any | std::regex_constants::match_flag_type::match_not_null |
									std::regex_constants::match_flag_type::match_prev_avail);
							args2.emplace_back(wordRegexMatch.str());
							std::regex_iterator<const char*>::regex_type rx("\\d{1,4}");
							std::regex_iterator<const char*> next(newString.data(), newString.data() + strlen(newString.data()), rx);
							std::regex_iterator<const char*> end;

							for (; next != end; ++next) {
								args2.emplace_back(next->str());
							}

							std::regex digitRegex("\\d{1,3}");
							if (args2.size() == 0 || convertToLowerCase(args2[0]) == "exit") {
								messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(song_api::getPlaylist(guild_data->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
								doWeQuit = true;
								savePlaylist(*discordGuild);
								break;
							} else if (convertToLowerCase(args2[0]) != "remove" && convertToLowerCase(args2[0]) != "swap" && convertToLowerCase(args2[0]) != "exit" &&
								convertToLowerCase(args2[0]) != "shuffle") {
								msgEmbeds[currentPageIndex].setDescription(
									"__**please enter a proper input!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap <sourceTrackNumber> "
									"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
								msgEmbeds[currentPageIndex].setFooter("please enter a proper input!\nType 'remove <trackNumber>' to remove a track.\nType "
																	  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																	  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
								messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "deleting the message!" })
									.get();
								respond_to_input_event_data& dataPackage03(newEvent);
								dataPackage03.setResponseType(input_event_response_type::Edit_Interaction_Response);
								dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
								dataPackage03.addContent("");
								newEvent = input_events::respondToInputEventAsync(const dataPackage03).get();
								continue;
							} else if (convertToLowerCase(args2[0]) == "remove") {
								if (args2.size() < 2 || !regex_search(args2[1].data(), digitRegex)) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**please enter a proper input!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("please enter a proper input!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "deleting the message!" })
										.get();
									respond_to_input_event_data& dataPackage03(newEvent);
									dataPackage03.setResponseType(input_event_response_type::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = input_events::respondToInputEventAsync(const dataPackage03).get();
									continue;
								}
								if ((jsonifier::strToInt64(args2[1]) - 1) < 0 || ( uint64_t )(jsonifier::strToInt64(args2[1]) - 1) >= song_api::getPlaylist(guild_data->id).songQueue.size() || args2.size() < 1) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**please enter a proper input!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("please enter a proper input!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "deleting the message!" })
										.get();
									respond_to_input_event_data& dataPackage03(newEvent);
									dataPackage03.setResponseType(input_event_response_type::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = input_events::respondToInputEventAsync(const dataPackage03).get();
									continue;
								}
								int32_t removeIndex = ( int32_t )jsonifier::strToInt64(args2[1]);

								auto playlist = song_api::getPlaylist(guild_data->id);
								playlist.songQueue.erase(playlist.songQueue.begin() + removeIndex - 1, playlist.songQueue.begin() + removeIndex);
								song_api::setPlaylist(playlist, guild_data->id);
								messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(song_api::getPlaylist(guild_data->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
								doWeQuit = true;
								savePlaylist(*discordGuild);
								break;
							} else if (convertToLowerCase(args2[0]) == "swap") {
								if (args2.size() < 3 || !regex_search(args2[1].data(), digitRegex) || !regex_search(args2[2].data(), digitRegex)) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**please enter a proper input!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("please enter a proper input!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "deleting the message!" })
										.get();
									respond_to_input_event_data& dataPackage03(newEvent);
									dataPackage03.setResponseType(input_event_response_type::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = input_events::respondToInputEventAsync(const dataPackage03).get();
									continue;
								}
								if (args2.size() < 2 ||
									((jsonifier::strToInt64(args2[1]) - 1) < 0 || ( uint64_t )(jsonifier::strToInt64(args2[1]) - 1) >= song_api::getPlaylist(guild_data->id).songQueue.size() ||
										(jsonifier::strToInt64(args2[2]) - 1) < 0 || ( uint64_t )(jsonifier::strToInt64(args2[2]) - 1) >= song_api::getPlaylist(guild_data->id).songQueue.size() || args2.size() < 2)) {
									msgEmbeds[currentPageIndex].setDescription(
										"__**please enter a proper input!**__\n__Type 'remove <trackNumber>' to remove a track.\nType 'swap "
										"<sourceTrackNumber> "
										"<destinationTrackNumber>' to swap tracks.\nType 'shuffle' to shuffle the playlist.\nType exit to exit.__\n");
									msgEmbeds[currentPageIndex].setFooter("please enter a proper input!\nType 'remove <trackNumber>' to remove a track.\nType "
																		  "'swap <sourceTrackNumber> <destinationTrackNumber>' to swap "
																		  "tracks.\nType 'shuffle' to shuffle the playlist.\nType 'exit' to exit.");
									messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																	 .channelId = returnedMessages.messages[0].channelId,
																	 .messageId = returnedMessages.messages[0].id,
																	 .reason = "deleting the message!" })
										.get();
									respond_to_input_event_data& dataPackage03(newEvent);
									dataPackage03.setResponseType(input_event_response_type::Edit_Interaction_Response);
									dataPackage03.addMessageEmbed(msgEmbeds[currentPageIndex]);
									dataPackage03.addContent("");
									newEvent = input_events::respondToInputEventAsync(const dataPackage03).get();
									continue;
								}

								int32_t sourceIndex = ( int32_t )jsonifier::strToInt64(args2[1]) - 1;
								int32_t destinationIndex = ( int32_t )jsonifier::strToInt64(args2[2]) - 1;
								song_api::modifyQueue(sourceIndex, destinationIndex, guild_data->id);
								messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(song_api::getPlaylist(guild_data->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
								doWeQuit = true;
								savePlaylist(*discordGuild);
								break;
							} else if (convertToLowerCase(args2[0]) == "shuffle") {
								auto oldSongArray = song_api::getPlaylist(guild_data->id);
								jsonifier::vector<song> newVector{};
								while (oldSongArray.songQueue.size() > 0) {
									std::mt19937_64 randomEngine{ static_cast<uint32_t>(
										std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) };
									int32_t randomIndex = static_cast<uint32_t>(
										(static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max()) * static_cast<double>(oldSongArray.songQueue.size())));
									newVector.emplace_back(oldSongArray.songQueue.at(randomIndex));
									oldSongArray.songQueue.erase(oldSongArray.songQueue.begin() + randomIndex, oldSongArray.songQueue.begin() + randomIndex + 1);
								}
								oldSongArray.songQueue = newVector;
								song_api::setPlaylist(oldSongArray, guild_data->id);
								messages::deleteMessageAsync({ .timeStamp = returnedMessages.messages[0].timeStamp,
																 .channelId = returnedMessages.messages[0].channelId,
																 .messageId = returnedMessages.messages[0].id,
																 .reason = "deleting the message!" })
									.get();
								msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
								msgEmbeds = updateMessageEmbeds(song_api::getPlaylist(guild_data->id).songQueue, discordGuild.get(), newEvent, newEvent, user, currentPageIndex);
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
				reportException("queue::execute()");
			};
		};
		~the_queue(){};
	};

};
```