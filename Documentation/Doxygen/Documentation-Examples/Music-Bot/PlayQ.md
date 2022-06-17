PlayQ {#Play-Q}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {

	class PlayQ : public DiscordCoreAPI::BaseFunction {
	  public:
		PlayQ() {
			this->commandName = "playq";
			this->helpDescription = "Plays a specific song from the current queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /playq SONGNUMBER.\n------");
			msgEmbed.setTitle("__**PlayQ Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<PlayQ>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({args.eventData->getChannelId()}).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({args.eventData->getGuildId()}).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedPlayingInChannel(*args.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({.guildMemberId = args.eventData->getAuthorId(), .guildId = args.eventData->getGuildId()}).get();

				bool doWeHaveControl = checkIfWeHaveControl(*args.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				std::unique_ptr<InputEventData> newEvent = std::make_unique<InputEventData>(*args.eventData);

				int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				int64_t previousPlayedTime {0};
				if (Play::timeOfLastPlay.contains(args.eventData->getGuildId())) {
					previousPlayedTime = Play::timeOfLastPlay.at(args.eventData->getGuildId());
				}

				if (currentTime-previousPlayedTime < 5000) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but please wait a total of 5 seconds in between plays!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Timing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				previousPlayedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				Play::timeOfLastPlay[args.eventData->getGuildId()] = previousPlayedTime;

				VoiceConnection* voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId, true, false);
				loadPlaylist(discordGuild);
				if (voiceConnection == nullptr) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Connection Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				if (guildMember.voiceData.channelId == 0 || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Playing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}
				int32_t trackNumber;
				try {
					trackNumber = stoi(args.commandData.optionsArgs[0]);
				} catch (...) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but please enter a valid number for the track number!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Playing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}
				if (trackNumber < 1) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but please enter a valid number for the track number!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Playing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				if (!SongAPI::isThereAnySongs(guild.id)) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but there's nothing to play!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Playing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				if (trackNumber > SongAPI::getPlaylist(guild.id).songQueue.size()) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but that number is out of the range of the current track list!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Playing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				SongAPI::stop(guild.id);
				Playlist currentPlaylist = SongAPI::getPlaylist(guild.id);
				Song currentNew = currentPlaylist.songQueue.at(trackNumber);
				currentPlaylist.songQueue.erase(currentPlaylist.songQueue.begin() + trackNumber);
				std::vector<Song> newVector;
				Playlist newPlaylist;
				newVector.push_back(currentNew);
				for (auto& value: currentPlaylist.songQueue) {
					newVector.push_back(value);
				}
				newPlaylist.songQueue = newVector;
				newPlaylist.isLoopAllEnabled = SongAPI::isLoopAllEnabled(guild.id);
				newPlaylist.isLoopSongEnabled = SongAPI::isLoopSongEnabled(guild.id);
				SongAPI::setPlaylist(newPlaylist, guild.id);
				savePlaylist(discordGuild);
				InputEventData newerEvent = *newEvent;
				BaseFunctionArguments newArgs = *args;
				uint64_t channelId = newArgs.eventData->getChannelId();
				if (!SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::function<CoRoutine<void>(SongCompletionEventData)> theTask = [=](SongCompletionEventData eventData) noexcept -> CoRoutine<void> {
						co_await NewThreadAwaitable<void>();
						std::unique_ptr<InputEventData> newestEvent = std::make_unique<InputEventData>(newerEvent);
						if (SongAPI::isThereAnySongs(guild.id)) {
							EmbedData newEmbed;
							if (!eventData.wasItAFail) {
								if (!SongAPI::sendNextSong(guildMember)) {
									InputEvents::deleteInputEventResponseAsync(std::move(newestEvent));
									SongAPI::play(guildMember.guildId);
									co_return;
								}
								savePlaylist(discordGuild);
								newEmbed.setAuthor(eventData.guildMember.user.userName, eventData.guildMember.user.avatar);
								newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(eventData.guild.id).songTitle + "](" +
														SongAPI::getCurrentSong(eventData.guild.id).viewUrl + ")" + "\n__**Description:**__ " +
														SongAPI::getCurrentSong(eventData.guild.id).description + "\n__**Duration:**__ " +
														SongAPI::getCurrentSong(eventData.guild.id).duration + "\n__**Added By:**__ <@!" +
														SongAPI::getCurrentSong(eventData.guild.id).addedByUserId + "> (" +
														SongAPI::getCurrentSong(eventData.guild.id).addedByUserName + ")");
								newEmbed.setImage(SongAPI::getCurrentSong(eventData.guild.id).thumbnailUrl);
								newEmbed.setTimeStamp(getTimeAndDate());
								newEmbed.setTitle("__**Now Playing:**__");
								newEmbed.setColor(discordGuild.data.borderColor);
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
								}
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
								}
								CreateMessageData dataPackage02(channelId);
								dataPackage02.addMessageEmbed(newEmbed);
								Messages::createMessageAsync(dataPackage02).get();
							} else {
								SongAPI::sendNextSong(eventData.guildMember);
								savePlaylist(discordGuild);
								loadPlaylist(discordGuild);
								newEmbed.setAuthor(eventData.guildMember.user.userName, eventData.guildMember.user.avatar);
								newEmbed.setDescription(
									"__**It appears as though there was an error when trying to play the following track!**__\n__**Title:**__ [" +
									eventData.previousSong.songTitle + "](" + eventData.previousSong.viewUrl + ")" + "\n__**Description:**__ " +
									eventData.previousSong.description + "\n__**Duration:**__ " + eventData.previousSong.duration + "\n__**Added By:**__ <@!" +
									eventData.previousSong.addedByUserId + "> (" + eventData.previousSong.addedByUserName + ")");
								newEmbed.setImage(eventData.previousSong.thumbnailUrl);
								newEmbed.setTimeStamp(getTimeAndDate());
								newEmbed.setTitle("__**Playing Error:**__");
								newEmbed.setColor("fe0000");
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
								}
								if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
								}
								CreateMessageData dataPackage02(channelId);
								dataPackage02.addMessageEmbed(newEmbed);
								Messages::createMessageAsync(dataPackage02).get();

								if (!SongAPI::areWeCurrentlyPlaying(eventData.guild.id)) {
									newEmbed.setAuthor(eventData.guildMember.user.userName, eventData.guildMember.user.avatar);
									newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(eventData.guild.id).songTitle + "](" +
															SongAPI::getCurrentSong(eventData.guild.id).viewUrl + ")" + "\n__**Description:**__ " +
															SongAPI::getCurrentSong(eventData.guild.id).description + "\n__**Duration:**__ " +
															SongAPI::getCurrentSong(eventData.guild.id).duration + "\n__**Added By:**__ <@!" +
															SongAPI::getCurrentSong(eventData.guild.id).addedByUserId + "> (" +
															SongAPI::getCurrentSong(eventData.guild.id).addedByUserName + ")");
									newEmbed.setImage(SongAPI::getCurrentSong(eventData.guild.id).thumbnailUrl);
									newEmbed.setTimeStamp(getTimeAndDate());
									newEmbed.setTitle("__**Now Playing:**__");
									newEmbed.setColor(discordGuild.data.borderColor);
									if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
									}
									if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
									}
									if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
									}
									if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
										newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
									}
									CreateMessageData dataPackage02(channelId);
									dataPackage02.addMessageEmbed(newEmbed);
									Messages::createMessageAsync(dataPackage02).get();
								}
							}
							savePlaylist(discordGuild);
							SongAPI::play(eventData.guild.id);
						} else {
							EmbedData newEmbed;
							newEmbed.setAuthor(eventData.guildMember.user.userName, eventData.guildMember.user.avatar);
							newEmbed.setDescription("------\n__**Sorry, but there's nothing left to play here!**__\n------");
							newEmbed.setTimeStamp(getTimeAndDate());
							newEmbed.setTitle("__**Now Playing:**__");
							newEmbed.setColor(discordGuild.data.borderColor);
							if (SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
							} else if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
							} else if (SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
							} else if (!SongAPI::isLoopAllEnabled(eventData.guild.id) && !SongAPI::isLoopSongEnabled(eventData.guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
							}
							CreateMessageData dataPackage02(channelId);
							dataPackage02.addMessageEmbed(newEmbed);
							Messages::createMessageAsync(dataPackage02).get();
						}
					};
					SongAPI::onSongCompletion(theTask, guild.id);
				}
				if (SongAPI::isThereAnySongs(guild.id)) {
					if (!SongAPI::sendNextSong(guildMember)) {
						InputEvents::deleteInputEventResponseAsync(std::move(newEvent));
						return;
					}
					savePlaylist(discordGuild);

					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" +
											SongAPI::getCurrentSong(guild.id).viewUrl + ")" + "\n__**Description:**__ " +
											SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " +
											SongAPI::getCurrentSong(guild.id).duration + "\n__**Added By:**__ <@!" +
											SongAPI::getCurrentSong(guild.id).addedByUserId + "> (" + SongAPI::getCurrentSong(guild.id).addedByUserName + ")");
					newEmbed.setImage(SongAPI::getCurrentSong(guild.id).thumbnailUrl);
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Now Playing:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
					} else if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
					} else if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
					} else if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
						newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
					}
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					SongAPI::play(guild.id);
				}
				return;
			} catch (...) {
				reportException("PlayQ::execute Error: ");
			}
		};
		virtual ~PlayQ();
	};
}
```