Play {#Play}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	move_through_message_pages_data recurseThroughOptions(move_through_message_pages_data returnData, int32_t currentPageIndex, input_event_data newEvent,
		jsonifier::vector<embed_data> embedsFromSearch, base_function_arguments& newArgs, jsonifier::vector<int32_t> arrayOfIndices, guild_member_data guildMember, jsonifier::vector<song> searchResults) {
		if (returnData.buttonId == "exit") {
			auto currentQueue = song_api::getPlaylist(guildMember.guildId);
			int32_t songSize = currentQueue.songQueue.size();
			arrayOfIndices.erase(arrayOfIndices.end() - 1, arrayOfIndices.end());
			for (auto& value: arrayOfIndices) {
				if (value != -1) {
					auto song = song_api::addSongToQueue(guildMember, searchResults[value]);
				}
			}
			unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
			jsonifier::string descriptionString{};
			currentQueue = song_api::getPlaylist(guildMember.guildId);
			descriptionString = "------\n__**added the following songs to the queue:\n";
			for (int32_t x = 0; x < arrayOfIndices.size(); x += 1) {
				descriptionString += "[" + searchResults[arrayOfIndices[x]].songTitle + "](" + searchResults[arrayOfIndices[x]].viewUrl + ")\n" +
					"position: " + jsonifier::toString(songSize + x + 1) + "\n";
			}
			descriptionString += "**__\n------";
			newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
			newEmbed->setDescription(descriptionString);
			newEmbed->setTimeStamp(getTimeAndDate());
			newEmbed->setTitle("__**playing new songs:**__");
			newEmbed->setColor("fefefe");
			respond_to_input_event_data& dataPackage(returnData.inputEventData);
			dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
			dataPackage.addMessageEmbed(*newEmbed);
			auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
			returnData.currentPageIndex -= 1;
			return returnData;
		} else if (returnData.buttonId == "empty") {
			returnData.currentPageIndex -= 1;
			return returnData;
		} else {
			returnData =
				moveThroughMessagePages(jsonifier::toString(newArgs.eventData.getAuthorId()), input_event_data(newEvent), currentPageIndex, embedsFromSearch, false, 120000, true);
			arrayOfIndices.emplace_back(returnData.currentPageIndex);
			return recurseThroughOptions(returnData, returnData.currentPageIndex, returnData.inputEventData, embedsFromSearch, newArgs, arrayOfIndices, guildMember, searchResults);
		}
	}

	class play : public base_function {
	  public:
		static unordered_map<int64_t, int64_t> timeOfLastPlay;

		play() {
			commandName = "play";
			helpDescription = "add a song to the queue and play it if nothing is playing.";
			embed_data newEmbed;
			newEmbed.setDescription("------\nSimply enter /play songname or url.\n------");
			newEmbed.setTitle("__**play usage:**__");
			newEmbed.setTimeStamp(getTimeAndDate());
			newEmbed.setColor("fe_fe_fe");
			helpEmbed = newEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<play>();
		}

		void execute(base_function_arguments& newArgs) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({ newArgs.eventData.getChannelId() }).get();

				guild_data guild_data = guilds::getCachedGuild({ newArgs.eventData.getGuildId() }).get();
				discord_guild discordGuild(guild_data);

				bool areWeAllowed = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				guild_member_data guildMember =
					guild_members::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				input_event_data newEvent = newArgs.eventData;

				int64_t currentTime = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count();
				int64_t previousPlayedTime{};
				if (play::timeOfLastPlay.contains(newArgs.eventData.getGuildId())) {
					previousPlayedTime = play::timeOfLastPlay.at(newArgs.eventData.getGuildId());
				}

				if (currentTime - previousPlayedTime < 5000) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but please wait a total of 5 seconds in between plays!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**timing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent).get();
					input_events::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}

				respond_to_input_event_data& dataPackage(newArgs.eventData);
				dataPackage.setResponseType(input_event_response_type::Deferred_Response);
				newEvent = input_events::respondToInputEventAsync(dataPackage).get();

				previousPlayedTime = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count();
				play::timeOfLastPlay.insert_or_assign(newArgs.eventData.getGuildId(), previousPlayedTime);
				voice_state_data voiceStateData{};
				if (guild_data.voiceStates.contains(guildMember.id)) {
					voiceStateData = guild_data.voiceStates.at(guildMember.id);
				} else {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent).get();
					input_events::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}
				voice_connection* voiceConnection = guild_data.connectToVoice(guildMember.id, 0, false, false);
				if (voiceConnection == nullptr) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**connection issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent).get();
					input_events::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}
				loadPlaylist(discordGuild);

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent).get();
					input_events::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}

				jsonifier::vector<song> searchResults{};
				if (newArgs.optionsArgs.size() > 0) {
					searchResults = song_api::searchForSong(newArgs.optionsArgs[0], guild_data.id);
				}

				if (searchResults.size() <= 0 && newArgs.optionsArgs.size() > 0) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**no songs could be found as a result of your search!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**search issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent).get();
					input_events::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}

				jsonifier::vector<embed_data> embedsFromSearch;
				int32_t x = 0;
				for (song& value: searchResults) {
					x += 1;
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("__**title:**__ [" + value.songTitle + "](" + value.viewUrl + ")" + "\n__**description:**__ " + value.description +
						"\n__**duration:**__ " + value.duration);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**" + jsonifier::toString(x) + " of " + jsonifier::toString(searchResults.size()) + "**__");
					newEmbed->setImage(value.thumbnailUrl);
					newEmbed->setColor(discordGuild.data.borderColor);
					embedsFromSearch.emplace_back(*newEmbed);
				}

				uint32_t currentPageIndex = 0;
				move_through_message_pages_data returnData{};
				jsonifier::vector<int32_t> arrayOfIndices{};
				if (embedsFromSearch.size() > 0) {
					respond_to_input_event_data& dataPackage0(newEvent);
					dataPackage0.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage0.addMessageEmbed(embedsFromSearch[0]);
					newEvent = input_events::respondToInputEventAsync(dataPackage0).get();

					returnData = recurseThroughOptions(returnData, currentPageIndex, newEvent, embedsFromSearch, newArgs, arrayOfIndices, guildMember, searchResults);
					savePlaylist(discordGuild);
					discordGuild.writeDataToDB();
				}
				auto channelId = newArgs.eventData.getChannelId();
				if (!song_api::areWeCurrentlyPlaying(guild_data.id)) {
					auto theTask = [=](song_completion_event_data eventData) mutable noexcept -> co_routine<void> {
						co_await newThreadAwaitable<void>();
						std::this_thread::sleep_for(150ms);
						if (song_api::isThereAnySongs(guild_data.id)) {
							unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
							if (!eventData.wasItAFail) {
								if (!song_api::sendNextSong(guildMember)) {
									input_events::deleteInputEventResponseAsync(newEvent);
									song_api::play(guildMember.guildId);
									co_return;
								}
								savePlaylist(discordGuild);
								newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
								newEmbed->setDescription("__**title:**__ [" + song_api::getCurrentSong(eventData.guild_data.id).songTitle + "](" +
									song_api::getCurrentSong(eventData.guild_data.id).viewUrl + ")" + "\n__**description:**__ " +
									song_api::getCurrentSong(eventData.guild_data.id).description + "\n__**duration:**__ " + song_api::getCurrentSong(eventData.guild_data.id).duration +
									"\n__**added by:**__ <@!" + jsonifier::toString(song_api::getCurrentSong(guild_data.id).addedByUserId) + "> (" +
									song_api::getCurrentSong(eventData.guild_data.id).addedByUserName + ")");
								newEmbed->setImage(song_api::getCurrentSong(eventData.guild_data.id).thumbnailUrl);
								newEmbed->setTimeStamp(getTimeAndDate());
								newEmbed->setTitle("__**now playing:**__");
								newEmbed->setColor(discordGuild.data.borderColor);
								if (song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("✅ loop-all, ✅ loop-song");
								}
								if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("❌ loop-all, ✅ loop-song");
								}
								if (song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("✅ loop-all, ❌ loop-song");
								}
								if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("❌ loop-all, ❌ loop-song");
								}
								create_message_data& dataPackage02(channelId);
								dataPackage02.addMessageEmbed(*newEmbed);
								messages::createMessageAsync(dataPackage02).get();
							} else {
								guild_member_data guildMemberNew{ eventData.guildMember };
								song_api::sendNextSong(guildMemberNew);
								savePlaylist(discordGuild);
								loadPlaylist(discordGuild);
								newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
								newEmbed->setDescription("__**it appears as though there was an error when trying to play the following track!**__\n__**title:**__ [" +
									eventData.previousSong.songTitle + "](" + eventData.previousSong.viewUrl + ")" + "\n__**description:**__ " +
									eventData.previousSong.description + "\n__**duration:**__ " + eventData.previousSong.duration + "\n__**added by:**__ <@!" +
									jsonifier::toString(eventData.previousSong.addedByUserId) + "> (" + eventData.previousSong.addedByUserName + ")");
								newEmbed->setImage(eventData.previousSong.thumbnailUrl);
								newEmbed->setTimeStamp(getTimeAndDate());
								newEmbed->setTitle("__**playing error:**__");
								newEmbed->setColor("fe0000");
								if (song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("✅ loop-all, ✅ loop-song");
								}
								if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("❌ loop-all, ✅ loop-song");
								}
								if (song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("✅ loop-all, ❌ loop-song");
								}
								if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
									newEmbed->setFooter("❌ loop-all, ❌ loop-song");
								}
								create_message_data& dataPackage02(channelId);
								dataPackage02.addMessageEmbed(*newEmbed);
								messages::createMessageAsync(dataPackage02).get();

								if (!song_api::areWeCurrentlyPlaying(eventData.guild_data.id)) {
									newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
									newEmbed->setDescription("__**title:**__ [" + song_api::getCurrentSong(eventData.guild_data.id).songTitle + "](" +
										song_api::getCurrentSong(eventData.guild_data.id).viewUrl + ")" + "\n__**description:**__ " +
										song_api::getCurrentSong(eventData.guild_data.id).description + "\n__**duration:**__ " + song_api::getCurrentSong(eventData.guild_data.id).duration +
										"\n__**added by:**__ <@!" + jsonifier::toString(song_api::getCurrentSong(eventData.guild_data.id).addedByUserId) + "> (" +
										song_api::getCurrentSong(eventData.guild_data.id).addedByUserName + ")");
									newEmbed->setImage(song_api::getCurrentSong(eventData.guild_data.id).thumbnailUrl);
									newEmbed->setTimeStamp(getTimeAndDate());
									newEmbed->setTitle("__**now playing:**__");
									newEmbed->setColor(discordGuild.data.borderColor);
									if (song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
										newEmbed->setFooter("✅ loop-all, ✅ loop-song");
									}
									if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
										newEmbed->setFooter("❌ loop-all, ✅ loop-song");
									}
									if (song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
										newEmbed->setFooter("✅ loop-all, ❌ loop-song");
									}
									if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
										newEmbed->setFooter("❌ loop-all, ❌ loop-song");
									}
									create_message_data& dataPackage03(channelId);
									dataPackage03.addMessageEmbed(*newEmbed);
									messages::createMessageAsync(dataPackage03).get();
								}
							}
							savePlaylist(discordGuild);
						} else {
							unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
							newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
							newEmbed->setDescription("------\n__**sorry, but there's nothing left to play here!**__\n------");
							newEmbed->setTimeStamp(getTimeAndDate());
							newEmbed->setTitle("__**now playing:**__");
							newEmbed->setColor(discordGuild.data.borderColor);
							if (song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
								newEmbed->setFooter("✅ loop-all, ✅ loop-song");
							} else if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && song_api::isLoopSongEnabled(eventData.guild_data.id)) {
								newEmbed->setFooter("❌ loop-all, ✅ loop-song");
							} else if (song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
								newEmbed->setFooter("✅ loop-all, ❌ loop-song");
							} else if (!song_api::isLoopAllEnabled(eventData.guild_data.id) && !song_api::isLoopSongEnabled(eventData.guild_data.id)) {
								newEmbed->setFooter("❌ loop-all, ❌ loop-song");
							}
							create_message_data& dataPackage02(channelId);
							dataPackage02.addMessageEmbed(*newEmbed);
							messages::createMessageAsync(dataPackage02).get();
						}
						co_return;
					};
					if (song_api::isThereAnySongs(guild_data.id)) {
						if (!song_api::sendNextSong(guildMember)) {
							input_events::deleteInputEventResponseAsync(newEvent);
							song_api::play(guild_data.id);
						}
						unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
						newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						newEmbed->setDescription("__**title:**__ [" + song_api::getCurrentSong(guild_data.id).songTitle + "](" + song_api::getCurrentSong(guild_data.id).viewUrl + ")" +
							"\n__**description:**__ " + song_api::getCurrentSong(guild_data.id).description + "\n__**duration:**__ " + song_api::getCurrentSong(guild_data.id).duration +
							"\n__**added by:**__ <@!" + jsonifier::toString(song_api::getCurrentSong(guild_data.id).addedByUserId) + "> (" +
							song_api::getCurrentSong(guild_data.id).addedByUserName + ")");
						newEmbed->setImage(song_api::getCurrentSong(guild_data.id).thumbnailUrl);
						newEmbed->setTimeStamp(getTimeAndDate());
						newEmbed->setTitle("__**now playing:**__");
						newEmbed->setColor(discordGuild.data.borderColor);
						if (song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed->setFooter("✅ loop-all, ✅ loop-song");
						} else if (!song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed->setFooter("❌ loop-all, ✅ loop-song");
						} else if (song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed->setFooter("✅ loop-all, ❌ loop-song");
						} else if (!song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed->setFooter("❌ loop-all, ❌ loop-song");
						}
						respond_to_input_event_data& dataPackage02(newEvent);
						dataPackage02.setResponseType(input_event_response_type::Follow_Up_Message);
						dataPackage02.addMessageEmbed(*newEmbed);
						newEvent = input_events::respondToInputEventAsync(dataPackage02).get();
						savePlaylist(discordGuild);
						song_api::play(guild_data.id);

					} else {
						unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
						newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						newEmbed->setDescription("------\n__**sorry, but there's nothing to play!**__\n------");
						newEmbed->setTimeStamp(getTimeAndDate());
						newEmbed->setTitle("__**playing issue:**__");
						newEmbed->setColor(discordGuild.data.borderColor);
						embedsFromSearch.emplace_back(*newEmbed);
						respond_to_input_event_data& dataPackage(newEvent);
						dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
						dataPackage.addMessageEmbed(*newEmbed);
						auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
						dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
						newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
						input_events::deleteInputEventResponseAsync(newEvent).get();
						input_events::deleteInputEventResponseAsync(newerEvent, 20000);
					}
					song_api::onSongCompletion(theTask, guild_data.id);
				} else if (newArgs.optionsArgs.size() == 0 && song_api::areWeCurrentlyPlaying(guild_data.id)) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but there's already something playing!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					embedsFromSearch.emplace_back(*newEmbed);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent).get();
					input_events::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				} else if (returnData.currentPageIndex != -1 && arrayOfIndices.size() < 2) {
					discordGuild.data.playlist = song_api::getPlaylist(guild_data.id);
					discordGuild.writeDataToDB();
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**song title:**__ [" + searchResults[returnData.currentPageIndex].songTitle + "](" +
						searchResults[returnData.currentPageIndex].viewUrl + ")\n__**duration:**__ " + searchResults[returnData.currentPageIndex].duration +
						"\n__**description:**__ " + searchResults[returnData.currentPageIndex].description + "\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**song added to queue position: " + jsonifier::toString(song_api::getPlaylist(guild_data.id).songQueue.size()) + "**__ ");
					newEmbed->setColor(discordGuild.data.borderColor);
					newEmbed->setThumbnail(searchResults[returnData.currentPageIndex].thumbnailUrl);
					embedsFromSearch.emplace_back(*newEmbed);
					respond_to_input_event_data& dataPackage02(newEvent);
					dataPackage02.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage02.addMessageEmbed(*newEmbed);
					newEvent = input_events::respondToInputEventAsync(dataPackage02).get();
				}
				return;
			} catch (...) {
				reportException("play::execute()");
			}
		};
		~play(){};
	};
	unordered_map<int64_t, int64_t> play::timeOfLastPlay{};

};
```