PlayRN {#Play-Right-Now}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class play_rn : public base_function {
	  public:
		play_rn() {
			commandName = "playrn";
			helpDescription = "add a song to the front of the queue and play it if nothing is playing.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /playrn songname or url.\n------");
			msgEmbed.setTitle("__**play_rn usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<play_rn>();
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

				int64_t currentTime = std::chrono::duration_cast<milliseconds>(hrclock::now().time_since_epoch()).count();
				int64_t previousPlayedTime{};
				if (play::timeOfLastPlay.contains(newEvent.getGuildId())) {
					previousPlayedTime = play::timeOfLastPlay.at(newEvent.getGuildId());
				}

				if (currentTime - previousPlayedTime < 5000) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but please wait a total of 5 seconds in between plays!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**timing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent, 20000);
					return;
				}

				previousPlayedTime = std::chrono::duration_cast<milliseconds>(hrclock::now().time_since_epoch()).count();
				play::timeOfLastPlay.insert_or_assign(newEvent.getGuildId(), previousPlayedTime);

				respond_to_input_event_data dataPackage(newEvent);
				dataPackage.setResponseType(input_event_response_type::Deferred_Response);
				newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
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
					respond_to_input_event_data dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}
				voice_connection* voiceConnection = guild_data.connectToVoice(guildMember.id, 0, true, false);

				if (voiceConnection == nullptr) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**connection issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}
				loadPlaylist(discordGuild);
				auto playlistOld = song_api::getPlaylist(guild_data.id);

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}

				if (newArgs.optionsArgs.size() == 0 && !song_api::isThereAnySongs(guild_data.id)) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but there's nothing to play!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				} else if (newArgs.optionsArgs.size() == 0 && song_api::areWeCurrentlyPlaying(guild_data.id)) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but there's already something playing!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}


				jsonifier::vector<song> searchResults{};
				if (newArgs.optionsArgs.size() > 0) {
					searchResults = song_api::searchForSong(newArgs.optionsArgs[0], guild_data.id);
				}

				jsonifier::vector<embed_data> embedsFromSearch;
				int32_t x = 0;
				for (song& value: searchResults) {
					x += 1;
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("__**title:**__ [" + value.songTitle + "](" + value.viewUrl + ")" + "\n__**description:**__ " + value.description +
						"\n__**duration:**__ " + value.duration);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**" + jsonifier::toString(x) + " of " + jsonifier::toString(searchResults.size()) + "**__");
					newEmbed->setImage(value.thumbnailUrl);
					newEmbed->setColor(discordGuild.data.borderColor);
					embedsFromSearch.emplace_back(*newEmbed);
				}

				if (embedsFromSearch.size() > 0) {
					auto playlist = song_api::getPlaylist(guild_data.id);
					song song = searchResults[0];
					song.addedByUserId = guildMember.id;
					song.addedByUserName = guildMember.userName;

					jsonifier::vector<song> songVector{};

					if (playlist.songQueue.size() > 0) {
						if (playlist.currentSong.songId != "") {
							songVector.resize(playlist.songQueue.size() + 2);
							songVector[1] = playlist.currentSong;
							songVector[0] = song;
							for (uint8_t y = 0; y < playlist.songQueue.size(); y += 1) {
								songVector[static_cast<int64_t>(y) + 2] = playlist.songQueue[static_cast<int64_t>(y)];
							}

						} else {
							songVector.resize(playlist.songQueue.size() + 1);
							songVector[0] = song;

							for (uint8_t y = 0; y < playlist.songQueue.size(); y += 1) {
								songVector[static_cast<int64_t>(y) + 1] = playlist.songQueue[static_cast<int64_t>(y)];
							}
						}
					} else {
						if (playlist.currentSong.songId != "") {
							songVector.resize(2);
							songVector[1] = playlist.currentSong;
							songVector[0] = song;
						} else {
							songVector.resize(1);
							songVector[0] = song;
						}
					}

					playlist.songQueue = songVector;
					playlist.currentSong = song();
					song_api::setPlaylist(playlist, guild_data.id);
					auto newPlaylist = song_api::getPlaylist(guild_data.id);
					savePlaylist(discordGuild);
				}
				auto newChannelId = newArgs.eventData.getChannelId();
				if (!song_api::areWeCurrentlyPlaying(guild_data.id)) {
					std::function<co_routine<void>(song_completion_event_data)> theTask = [=](song_completion_event_data eventData) mutable noexcept -> co_routine<void> {
						co_await newThreadAwaitable<void>();
						if (song_api::isThereAnySongs(guild_data.id)) {
							unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
							if (!eventData.wasItAFail) {
								if (!song_api::sendNextSong(guildMember)) {
									input_events::deleteInputEventResponseAsync(const newEvent);
									song_api::play(guildMember.guildId);
									co_return;
								}
								savePlaylist(discordGuild);
								newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
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
								create_message_data& dataPackage02(newChannelId);
								dataPackage02.addMessageEmbed(*newEmbed);
								messages::createMessageAsync(const dataPackage02).get();
							} else {
								guild_member_data guildMemberNew{ eventData.guildMember };
								song_api::sendNextSong(guildMemberNew);
								savePlaylist(discordGuild);
								loadPlaylist(discordGuild);
								newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
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
								create_message_data& dataPackage02(newChannelId);
								dataPackage02.addMessageEmbed(*newEmbed);
								messages::createMessageAsync(const dataPackage02).get();

								if (!song_api::areWeCurrentlyPlaying(eventData.guild_data.id)) {
									newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
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
									create_message_data& dataPackage03(newChannelId);
									dataPackage03.addMessageEmbed(*newEmbed);
									messages::createMessageAsync(const dataPackage03).get();
								}
							}
							savePlaylist(discordGuild);
						} else {
							unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
							newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
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
							create_message_data& dataPackage02(newChannelId);
							dataPackage02.addMessageEmbed(*newEmbed);
							messages::createMessageAsync(const dataPackage02).get();
						}
						co_return;
					};
					song_api::onSongCompletion(theTask, guild_data.id);
				}
				if (song_api::isThereAnySongs(guild_data.id)) {
					if (!song_api::sendNextSong(guildMember)) {
						song_api::play(guild_data.id);
						input_events::deleteInputEventResponseAsync(const newEvent);
						return;
					}
					discord_guild discordGuildNew = discordGuild;

					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("__**title:**__ [" + song_api::getCurrentSong(guild_data.id).songTitle + "](" + song_api::getCurrentSong(guild_data.id).viewUrl + ")" +
						"\n__**description:**__ " + song_api::getCurrentSong(guild_data.id).description + "\n__**duration:**__ " + song_api::getCurrentSong(guild_data.id).duration +
						"\n__**added by:**__ <@!" + jsonifier::toString(song_api::getCurrentSong(guild_data.id).addedByUserId) + "> (" + song_api::getCurrentSong(guild_data.id).addedByUserName +
						")");
					newEmbed->setImage(song_api::getCurrentSong(guild_data.id).thumbnailUrl);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**now playing:**__");
					newEmbed->setColor(discordGuildNew.data.borderColor);
					if (song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
						newEmbed->setFooter("✅ loop-all, ✅ loop-song");
					} else if (!song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
						newEmbed->setFooter("❌ loop-all, ✅ loop-song");
					} else if (song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
						newEmbed->setFooter("✅ loop-all, ❌ loop-song");
					} else if (!song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
						newEmbed->setFooter("❌ loop-all, ❌ loop-song");
					}
					respond_to_input_event_data dataPackage{ newEvent };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					savePlaylist(discordGuild);
					song_api::play(guild_data.id);
				}
				return;
			} catch (...) {
				reportException("play_rn::execute()");
			}
		};
		~play_rn(){};
	};

};```