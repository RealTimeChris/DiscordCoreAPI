PlayQ {#Play-Q}
============

```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class play_q : public base_function {
	  public:
		static std::unordered_map<uint64_t, int64_t> timeOfLastPlay;

		play_q() {
			commandName = "playq";
			helpDescription = "plays a specific song from the current queue.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /playq songnumber.\n------");
			msgEmbed.setTitle("__**play_q usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<play_q>();
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
				if (play_q::timeOfLastPlay.contains(newArgs.eventData.getGuildId())) {
					previousPlayedTime = play_q::timeOfLastPlay.at(newArgs.eventData.getGuildId());
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
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					return;
				}

				respond_to_input_event_data& dataPackage(newArgs.eventData);
				dataPackage.setResponseType(input_event_response_type::Deferred_Response);
				newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();

				previousPlayedTime = std::chrono::duration_cast<milliseconds>(hrclock::now().time_since_epoch()).count();
				play_q::timeOfLastPlay.insert_or_assign(newArgs.eventData.getGuildId(), previousPlayedTime);

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
					auto newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}

				voice_connection* voiceConnection = guild_data.connectToVoice(guildMember.id, 0, true, false);

				loadPlaylist(discordGuild);
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
					auto newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}

				if (!song_api::isThereAnySongs(guild_data.id)) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but there's nothing to play!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}

				int32_t trackNumber = jsonifier::strToInt64(newArgs.optionsArgs[0]) - 1;

				if (trackNumber >= song_api::getPlaylist(guild_data.id).songQueue.size()) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**sorry, but that number is out of the range of the current track list!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**playing issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					newerEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					input_events::deleteInputEventResponseAsync(const newEvent).get();
					input_events::deleteInputEventResponseAsync(const newerEvent, 20000);
					return;
				}

				playlist currentPlaylist = song_api::getPlaylist(guild_data.id);
				auto currentSong = song_api::getCurrentSong(guild_data.id);
				song currentNew = currentPlaylist.songQueue.at(trackNumber);
				currentPlaylist.songQueue.erase(currentPlaylist.songQueue.begin() + trackNumber);
				jsonifier::vector<song> newVector{};
				playlist newPlaylist{};
				newVector.emplace_back(currentNew);
				newVector.emplace_back(currentSong);
				for (auto& value: currentPlaylist.songQueue) {
					newVector.emplace_back(value);
				}
				newPlaylist.songQueue = newVector;
				newPlaylist.isLoopAllEnabled = song_api::isLoopAllEnabled(guild_data.id);
				newPlaylist.isLoopSongEnabled = song_api::isLoopSongEnabled(guild_data.id);
				song_api::setPlaylist(newPlaylist, guild_data.id);
				savePlaylist(discordGuild);
				auto channelId = newArgs.eventData.getChannelId();
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
								create_message_data& dataPackage02(channelId);
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
								create_message_data& dataPackage02(channelId);
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
									create_message_data& dataPackage03(channelId);
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
							create_message_data& dataPackage02(channelId);
							dataPackage02.addMessageEmbed(*newEmbed);
							messages::createMessageAsync(const dataPackage02).get();
						}
						co_return;
					};
					song_api::onSongCompletion(theTask, guild_data.id);
				}
				if (song_api::isThereAnySongs(guild_data.id)) {
					if (!song_api::sendNextSong(guildMember)) {
						input_events::deleteInputEventResponseAsync(const newEvent);
						return;
					}
					savePlaylist(discordGuild);

					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("__**title:**__ [" + song_api::getCurrentSong(guild_data.id).songTitle + "](" + song_api::getCurrentSong(guild_data.id).viewUrl + ")" +
						"\n__**description:**__ " + song_api::getCurrentSong(guild_data.id).description + "\n__**duration:**__ " + song_api::getCurrentSong(guild_data.id).duration +
						"\n__**added by:**__ <@!" + jsonifier::toString(song_api::getCurrentSong(guild_data.id).addedByUserId) + "> (" + song_api::getCurrentSong(guild_data.id).addedByUserName +
						")");
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
					respond_to_input_event_data& dataPackage(newEvent);
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					song_api::play(guild_data.id);
				}
				return;
			} catch (...) {
				reportException("play_q::execute()");
			}
		};
		~play_q(){};
	};
	std::unordered_map<uint64_t, int64_t> play_q::timeOfLastPlay{};

}```