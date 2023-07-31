PlayQ {#Play-Q}
============

```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class PlayQ : public BaseFunction {
	  public:
		static UnorderedMap<uint64_t, int64_t> timeOfLastPlay;

		PlayQ() {
			commandName = "playq";
			helpDescription = "Plays a specific song from the current queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /playq SONGNUMBER.\n------");
			msgEmbed.setTitle("__**PlayQ Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<PlayQ>();
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
				if (PlayQ::timeOfLastPlay.contains(newArgs.eventData.getGuildId())) {
					previousPlayedTime = PlayQ::timeOfLastPlay.at(newArgs.eventData.getGuildId());
				}

				if (currentTime - previousPlayedTime < 5000) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but please wait a total of 5 seconds in between plays!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Timing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				previousPlayedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
				PlayQ::timeOfLastPlay.insert_or_assign(newArgs.eventData.getGuildId(), previousPlayedTime);

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

				loadPlaylist(discordGuild);
				if (voiceConnection == nullptr) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
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

				if (!SongAPI::isThereAnySongs(guild.id)) {
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
				}

				int32_t trackNumber = stoi(newArgs.optionsArgs[0]) - 1;

				if (trackNumber >= SongAPI::getPlaylist(guild.id).songQueue.size()) {
					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but that number is out of the range of the current track list!**__\n------");
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

				Playlist currentPlaylist = SongAPI::getPlaylist(guild.id);
				auto currentSong = SongAPI::getCurrentSong(guild.id);
				Song currentNew = currentPlaylist.songQueue.at(trackNumber);
				currentPlaylist.songQueue.erase(currentPlaylist.songQueue.begin() + trackNumber);
				std::vector<Song> newVector{};
				Playlist newPlaylist{};
				newVector.emplace_back(currentNew);
				newVector.emplace_back(currentSong);
				for (auto& value: currentPlaylist.songQueue) {
					newVector.emplace_back(value);
				}
				newPlaylist.songQueue = newVector;
				newPlaylist.isLoopAllEnabled = SongAPI::isLoopAllEnabled(guild.id);
				newPlaylist.isLoopSongEnabled = SongAPI::isLoopSongEnabled(guild.id);
				SongAPI::setPlaylist(newPlaylist, guild.id);
				savePlaylist(discordGuild);
				auto channelId = newArgs.eventData.getChannelId();
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
								CreateMessageData dataPackage02(channelId);
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
								CreateMessageData dataPackage02(channelId);
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
									CreateMessageData dataPackage03(channelId);
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
							CreateMessageData dataPackage02(channelId);
							dataPackage02.addMessageEmbed(*newEmbed);
							Messages::createMessageAsync(dataPackage02).get();
						}
						co_return;
					};
					SongAPI::onSongCompletion(theTask, guild.id);
				}
				if (SongAPI::isThereAnySongs(guild.id)) {
					if (!SongAPI::sendNextSong(guildMember)) {
						InputEvents::deleteInputEventResponseAsync(newEvent);
						return;
					}
					savePlaylist(discordGuild);

					UniquePtr<EmbedData> newEmbed{ makeUnique<EmbedData>() };
					newEmbed->setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
					newEmbed->setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewUrl + ")" +
						"\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " + SongAPI::getCurrentSong(guild.id).duration +
						"\n__**Added By:**__ <@!" + std::to_string(SongAPI::getCurrentSong(guild.id).addedByUserId) + "> (" + SongAPI::getCurrentSong(guild.id).addedByUserName +
						")");
					newEmbed->setImage(SongAPI::getCurrentSong(guild.id).thumbnailUrl);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Now Playing:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("✅ Loop-All, ✅ Loop-Song");
					} else if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("❌ Loop-All, ✅ Loop-Song");
					} else if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("✅ Loop-All, ❌ Loop-Song");
					} else if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed->setFooter("❌ Loop-All, ❌ Loop-Song");
					}
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					SongAPI::play(guild.id);
				}
				return;
			} catch (...) {
				reportException("PlayQ::execute()");
			}
		};
		~PlayQ(){};
	};
	UnorderedMap<uint64_t, int64_t> PlayQ::timeOfLastPlay{};

}```