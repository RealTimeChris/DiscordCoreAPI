PlayRN {#Play-Right-Now}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class PlayRN : public BaseFunction {
	  public:
		PlayRN() {
			commandName = "playrn";
			helpDescription = "Add a song to the front of the queue and play it if nothing is playing.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /playrn SONGNAME OR Url.\n------");
			msgEmbed.setTitle("__**PlayRN Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<PlayRN>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannel({ newArgs.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuild({ newArgs.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				InputEventData newEvent = newArgs.eventData;

				int64_t currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
				int64_t previousPlayedTime{};
				if (Play::timeOfLastPlay.contains(newEvent.getGuildId())) {
					previousPlayedTime = Play::timeOfLastPlay.at(newEvent.getGuildId());
				}

				if (currentTime - previousPlayedTime < 5000) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but please wait a total of 5 seconds in between plays!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Timing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
					return;
				}

				previousPlayedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
				Play::timeOfLastPlay.insert_or_assign(newEvent.getGuildId(), previousPlayedTime);

				RespondToInputEventData dataPackage(newEvent);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				VoiceStateData voiceStateData{};
				if (guild.voiceStates.contains(guildMember.id)) {
					voiceStateData = guild.voiceStates.at(guildMember.id);
				} else {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}
				VoiceConnection* voiceConnection = guild.connectToVoice(guildMember.id, 0, true, false);

				if (voiceConnection == nullptr) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Connection Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}
				loadPlaylist(discordGuild);
				auto playlistOld = SongAPI::getPlaylist(guild.id);

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}

				if (newArgs.optionsArgs.size() == 0 && !SongAPI::isThereAnySongs(guild.id)) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but there's nothing to play!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				} else if (newArgs.optionsArgs.size() == 0 && SongAPI::areWeCurrentlyPlaying(guild.id)) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but there's already something playing!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}


				std::vector<Song> searchResults{};
				if (newArgs.optionsArgs.size() > 0) {
					searchResults = SongAPI::searchForSong(newArgs.optionsArgs[0], guild.id);
				}

				std::vector<EmbedData> embedsFromSearch;
				int32_t x = 0;
				for (Song& value: searchResults) {
					x += 1;
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("__**Title:**__ [" + value.songTitle + "](" + value.viewUrl + ")" + "\n__**Description:**__ " + value.description +
						"\n__**Duration:**__ " + value.duration);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**" + std::to_string(x) + " of " + std::to_string(searchResults.size()) + "**__");
					newEmbed->setImage(value.thumbnailUrl);
					newEmbed->setColor(discordGuild.data.borderColor);
					embedsFromSearch.emplace_back(*newEmbed);
				}

				if (embedsFromSearch.size() > 0) {
					auto playlist = SongAPI::getPlaylist(guild.id);
					Song song = searchResults[0];
					song.addedByUserId = guildMember.id;
					song.addedByUserName = guildMember.userName;

					std::vector<Song> songVector{};

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
					playlist.currentSong = Song();
					SongAPI::setPlaylist(playlist, guild.id);
					auto newPlaylist = SongAPI::getPlaylist(guild.id);
					savePlaylist(discordGuild);
				}
				auto newChannelId = newArgs.eventData.getChannelId();
				if (!SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::function<CoRoutine<void>(SongCompletionEventData)> theTask = [=](SongCompletionEventData eventData) mutable noexcept -> CoRoutine<void> {
						co_await NewThreadAwaitable<void>();
						if (SongAPI::isThereAnySongs(guild.id)) {
							UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
							if (!eventData.wasItAFail) {
								if (!SongAPI::sendNextSong(guildMember)) {
									InputEvents::deleteInputEventResponseAsync(newEvent);
									SongAPI::play(guildMember.guildId);
									co_return;
								}
								savePlaylist(discordGuild);
								newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
								newEmbed->setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(eventData.guild.id).songTitle + "](" +
									SongAPI::getCurrentSong(eventData.guild.id).viewUrl + ")" + "\n__**Description:**__ " +
									SongAPI::getCurrentSong(eventData.guild.id).description + "\n__**Duration:**__ " + SongAPI::getCurrentSong(eventData.guild.id).duration +
									"\n__**Added By:**__ <@!" + std::to_string(SongAPI::getCurrentSong(eventData.guild.id).addedByUserId) + "> (" +
									SongAPI::getCurrentSong(eventData.guild.id).addedByUserName + ")");
								newEmbed->setImage(SongAPI::getCurrentSong(eventData.guild.id).thumbnailUrl);
								newEmbed->setTimeStamp(getTimeAndDate());
								newEmbed->setTitle("__**Now Playing:**__");
								newEmbed->setColor(discordGuild.data.borderColor);
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("✅ Loop-All, ✅ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("❌ Loop-All, ✅ Loop-Song");
								}
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("✅ Loop-All, ❌ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("❌ Loop-All, ❌ Loop-Song");
								}
								CreateMessageData dataPackage02(newChannelId);
								dataPackage02.addMessageEmbed(*newEmbed);
								Messages::createMessageAsync(dataPackage02).get();
							} else {
								GuildMember guildMemberNew{ eventData.guildMember };
								SongAPI::sendNextSong(guildMemberNew);
								savePlaylist(discordGuild);
								loadPlaylist(discordGuild);
								newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
								newEmbed->setDescription("__**It appears as though there was an error when trying to play the following track!**__\n__**Title:**__ [" +
									eventData.previousSong.songTitle + "](" + eventData.previousSong.viewUrl + ")" + "\n__**Description:**__ " +
									eventData.previousSong.description + "\n__**Duration:**__ " + eventData.previousSong.duration + "\n__**Added By:**__ <@!" +
									std::to_string(eventData.previousSong.addedByUserId) + "> (" + eventData.previousSong.addedByUserName + ")");
								newEmbed->setImage(eventData.previousSong.thumbnailUrl);
								newEmbed->setTimeStamp(getTimeAndDate());
								newEmbed->setTitle("__**Playing Error:**__");
								newEmbed->setColor("fe0000");
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("✅ Loop-All, ✅ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("❌ Loop-All, ✅ Loop-Song");
								}
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("✅ Loop-All, ❌ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed->setFooter("❌ Loop-All, ❌ Loop-Song");
								}
								CreateMessageData dataPackage02(newChannelId);
								dataPackage02.addMessageEmbed(*newEmbed);
								Messages::createMessageAsync(dataPackage02).get();

								if (!SongAPI::areWeCurrentlyPlaying(eventData.guild.id)) {
									newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
									newEmbed->setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(eventData.guild.id).songTitle + "](" +
										SongAPI::getCurrentSong(eventData.guild.id).viewUrl + ")" + "\n__**Description:**__ " +
										SongAPI::getCurrentSong(eventData.guild.id).description + "\n__**Duration:**__ " + SongAPI::getCurrentSong(eventData.guild.id).duration +
										"\n__**Added By:**__ <@!" + std::to_string(SongAPI::getCurrentSong(eventData.guild.id).addedByUserId) + "> (" +
										SongAPI::getCurrentSong(eventData.guild.id).addedByUserName + ")");
									newEmbed->setImage(SongAPI::getCurrentSong(eventData.guild.id).thumbnailUrl);
									newEmbed->setTimeStamp(getTimeAndDate());
									newEmbed->setTitle("__**Now Playing:**__");
									newEmbed->setColor(discordGuild.data.borderColor);
									if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed->setFooter("✅ Loop-All, ✅ Loop-Song");
									}
									if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed->setFooter("❌ Loop-All, ✅ Loop-Song");
									}
									if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed->setFooter("✅ Loop-All, ❌ Loop-Song");
									}
									if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed->setFooter("❌ Loop-All, ❌ Loop-Song");
									}
									CreateMessageData dataPackage03(newChannelId);
									dataPackage03.addMessageEmbed(*newEmbed);
									Messages::createMessageAsync(dataPackage03).get();
								}
							}
							savePlaylist(discordGuild);
						} else {
							UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
							newEmbed->setAuthor(eventData.guildMember.userName, eventData.guildMember.avatar);
							newEmbed->setDescription("------\n__**Sorry, but there's nothing left to play here!**__\n------");
							newEmbed->setTimeStamp(getTimeAndDate());
							newEmbed->setTitle("__**Now Playing:**__");
							newEmbed->setColor(discordGuild.data.borderColor);
							if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed->setFooter("✅ Loop-All, ✅ Loop-Song");
							} else if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed->setFooter("❌ Loop-All, ✅ Loop-Song");
							} else if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed->setFooter("✅ Loop-All, ❌ Loop-Song");
							} else if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed->setFooter("❌ Loop-All, ❌ Loop-Song");
							}
							CreateMessageData dataPackage02(newChannelId);
							dataPackage02.addMessageEmbed(*newEmbed);
							Messages::createMessageAsync(dataPackage02).get();
						}
						co_return;
					};
					SongAPI::onSongCompletion(theTask, guild.id);
				}
				if (SongAPI::isThereAnySongs(guild.id)) {
					if (!SongAPI::sendNextSong(guildMember)) {
						SongAPI::play(guild.id);
						InputEvents::deleteInputEventResponseAsync(newEvent);
						return;
					}
					DiscordGuild discordGuildNew = discordGuild;

					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewUrl + ")" +
						"\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " + SongAPI::getCurrentSong(guild.id).duration +
						"\n__**Added By:**__ <@!" + std::to_string(SongAPI::getCurrentSong(guild.id).addedByUserId) + "> (" + SongAPI::getCurrentSong(guild.id).addedByUserName +
						")");
					newEmbed->setImage(SongAPI::getCurrentSong(guild.id).thumbnailUrl);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Now Playing:**__");
					newEmbed->setColor(discordGuildNew.data.borderColor);
					if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("✅ Loop-All, ✅ Loop-Song");
					} else if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("❌ Loop-All, ✅ Loop-Song");
					} else if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("✅ Loop-All, ❌ Loop-Song");
					} else if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("❌ Loop-All, ❌ Loop-Song");
					}
					RespondToInputEventData dataPackage{ newEvent };
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					savePlaylist(discordGuild);
					SongAPI::play(guild.id);
				}
				return;
			} catch (...) {
				reportException("PlayRN::execute()");
			}
		};
		~PlayRN(){};
	};

};```