Play {#Play}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {

	class Play : public DiscordCoreAPI::BaseFunction {
	  public:
		static std::unordered_map<std::string, int64_t> timeOfLastPlay;

		Play() {
			this->commandName = "play";
			this->helpDescription = "Add a song to the queue and play it if nothing is playing.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /play SONGNAME OR Url.\n------");
			msgEmbed.setTitle("__**Play Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Play>();
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
				loadPlaylist(discordGuild);

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

				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				newEvent = InputEvents::respondToInputEventAsync(dataPackage);

				std::vector<Song> searchResults;
				if (args.commandData.optionsArgs.size() > 0) {
					searchResults = SongAPI::searchForSong(args.commandData.optionsArgs[0], guild.id);
				}

				if (searchResults.size() <= 0 && args.commandData.optionsArgs.size() > 0) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**No songs could be found as a result of your search!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Search Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*newEvent);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				std::vector<EmbedData> embedsFromSearch;
				uint32_t x = 0;
				for (Song& value: searchResults) {
					x += 1;
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("__**Title:**__ [" + value.songTitle + "](" + value.viewUrl + ")" + "\n__**Description:**__ " + value.description +
											"\n__**Duration:**__ " + value.duration);
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**" + std::to_string(x) + " of " + std::to_string(searchResults.size()) + "**__");
					newEmbed.setImage(value.thumbnailUrl);
					newEmbed.setColor(discordGuild.data.borderColor);
					embedsFromSearch.push_back(newEmbed);
				}

				uint32_t currentPageIndex = 0;
				MoveThroughMessagePagesData returnData;
				if (embedsFromSearch.size() > 0) {
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(embedsFromSearch[0]);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);

					returnData = moveThroughMessagePages(args.eventData->getAuthorId(), std::make_unique<InputEventData>(*newEvent), currentPageIndex,
														 embedsFromSearch, false, 120000, true);

					if (returnData.buttonId == "exit" || returnData.buttonId == "empty") {
						return;
					}
					if (returnData.currentPageIndex != -1) {
						auto song = SongAPI::addSongToQueue(guildMember, searchResults[returnData.currentPageIndex]);
					}
				}
				InputEventData newestEvent = *newEvent;
				Snowflake channelId = args.eventData->getChannelId();
				if (!SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::function<CoRoutine<void>(SongCompletionEventData)> theTask = [=](SongCompletionEventData eventDataNew) noexcept -> CoRoutine<void> {
						co_await NewThreadAwaitable<void>();
						std::unique_ptr<InputEventData> newerEvent = std::make_unique<InputEventData>(newestEvent);
						if (SongAPI::isThereAnySongs(guild.id)) {
							EmbedData newEmbed;
							if (!eventDataNew.wasItAFail) {
								if (!SongAPI::sendNextSong(guildMember)) {
									InputEvents::deleteInputEventResponseAsync(std::move(newerEvent));
									SongAPI::play(guildMember.guildId);
									co_return;
								}
								savePlaylist(discordGuild);
								newEmbed.setAuthor(eventDataNew.guildMember.user.userName, eventDataNew.guildMember.user.avatar);
								newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(eventDataNew.guild.id).songTitle + "](" +
														SongAPI::getCurrentSong(eventDataNew.guild.id).viewUrl + ")" + "\n__**Description:**__ " +
														SongAPI::getCurrentSong(eventDataNew.guild.id).description + "\n__**Duration:**__ " +
														SongAPI::getCurrentSong(eventDataNew.guild.id).duration + "\n__**Added By:**__ <@!" +
														SongAPI::getCurrentSong(eventDataNew.guild.id).addedByUserId + "> (" +
														SongAPI::getCurrentSong(eventDataNew.guild.id).addedByUserName + ")");
								newEmbed.setImage(SongAPI::getCurrentSong(eventDataNew.guild.id).thumbnailUrl);
								newEmbed.setTimeStamp(getTimeAndDate());
								newEmbed.setTitle("__**Now Playing:**__");
								newEmbed.setColor(discordGuild.data.borderColor);
								if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
								}
								if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
								}
								CreateMessageData dataPackage02(channelId);
								dataPackage02.addMessageEmbed(newEmbed);
								Messages::createMessageAsync(dataPackage02).get();
							} else {
								SongAPI::sendNextSong(eventDataNew.guildMember);
								savePlaylist(discordGuild);
								loadPlaylist(discordGuild);
								newEmbed.setAuthor(eventDataNew.guildMember.user.userName, eventDataNew.guildMember.user.avatar);
								newEmbed.setDescription(
									"__**It appears as though there was an error when trying to play the following track!**__\n__**Title:**__ [" +
									eventDataNew.previousSong.songTitle + "](" + eventDataNew.previousSong.viewUrl + ")" + "\n__**Description:**__ " +
									eventDataNew.previousSong.description + "\n__**Duration:**__ " + eventDataNew.previousSong.duration +
									"\n__**Added By:**__ <@!" + eventDataNew.previousSong.addedByUserId + "> (" + eventDataNew.previousSong.addedByUserName +
									")");
								newEmbed.setImage(eventDataNew.previousSong.thumbnailUrl);
								newEmbed.setTimeStamp(getTimeAndDate());
								newEmbed.setTitle("__**Playing Error:**__");
								newEmbed.setColor("fe0000");
								if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
								}
								if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
								}
								if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
									newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
								}
								CreateMessageData dataPackage02(channelId);
								dataPackage02.addMessageEmbed(newEmbed);
								Messages::createMessageAsync(dataPackage02).get();

								if (!SongAPI::areWeCurrentlyPlaying(eventDataNew.guild.id)) {
									newEmbed.setAuthor(eventDataNew.guildMember.user.userName, eventDataNew.guildMember.user.avatar);
									newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(eventDataNew.guild.id).songTitle + "](" +
															SongAPI::getCurrentSong(eventDataNew.guild.id).viewUrl + ")" + "\n__**Description:**__ " +
															SongAPI::getCurrentSong(eventDataNew.guild.id).description + "\n__**Duration:**__ " +
															SongAPI::getCurrentSong(eventDataNew.guild.id).duration + "\n__**Added By:**__ <@!" +
															SongAPI::getCurrentSong(eventDataNew.guild.id).addedByUserId + "> (" +
															SongAPI::getCurrentSong(eventDataNew.guild.id).addedByUserName + ")");
									newEmbed.setImage(SongAPI::getCurrentSong(eventDataNew.guild.id).thumbnailUrl);
									newEmbed.setTimeStamp(getTimeAndDate());
									newEmbed.setTitle("__**Now Playing:**__");
									newEmbed.setColor(discordGuild.data.borderColor);
									if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
										newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
									}
									if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
										newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
									}
									if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
										newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
									}
									if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
										newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
									}
									CreateMessageData dataPackage02(channelId);
									dataPackage02.addMessageEmbed(newEmbed);
									Messages::createMessageAsync(dataPackage02).get();
								}
							}
							savePlaylist(discordGuild);
							SongAPI::play(eventDataNew.guild.id);
						} else {
							EmbedData newEmbed;
							newEmbed.setAuthor(eventDataNew.guildMember.user.userName, eventDataNew.guildMember.user.avatar);
							newEmbed.setDescription("------\n__**Sorry, but there's nothing left to play here!**__\n------");
							newEmbed.setTimeStamp(getTimeAndDate());
							newEmbed.setTitle("__**Now Playing:**__");
							newEmbed.setColor(discordGuild.data.borderColor);
							if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
							} else if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
							} else if (SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
							} else if (!SongAPI::isLoopAllEnabled(eventDataNew.guild.id) && !SongAPI::isLoopSongEnabled(eventDataNew.guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
							}
							CreateMessageData dataPackage02(channelId);
							dataPackage02.addMessageEmbed(newEmbed);
							Messages::createMessageAsync(dataPackage02).get();
							co_return;
						}
					};

					if (SongAPI::isThereAnySongs(guild.id)) {
						if (!SongAPI::sendNextSong(guildMember)) {
							InputEvents::deleteInputEventResponseAsync(std::move(newEvent));
							SongAPI::play(guild.id);
						}
						EmbedData newEmbed;
						newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						newEmbed.setDescription(
							"__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewUrl + ")" +
							"\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " +
							SongAPI::getCurrentSong(guild.id).duration + "\n__**Added By:**__ <@!" + SongAPI::getCurrentSong(guild.id).addedByUserId + "> (" +
							SongAPI::getCurrentSong(guild.id).addedByUserName + ")");
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
						dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
						dataPackage.addMessageEmbed(newEmbed);
						newEvent = InputEvents::respondToInputEventAsync(dataPackage);
						savePlaylist(discordGuild);
						SongAPI::play(guild.id);

					} else {
						EmbedData newEmbed;
						newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						newEmbed.setDescription("------\n__**Sorry, but there's nothing to play!**__\n------");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Playing Issue:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						embedsFromSearch.push_back(newEmbed);
						RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(newEmbed);
						newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					}
					SongAPI::onSongCompletion(theTask, guild.id);
				} else if (args.commandData.optionsArgs.size() == 0) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but there's already something playing!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Playing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					embedsFromSearch.push_back(newEmbed);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				} else if (returnData.currentPageIndex != -1) {
					discordGuild.data.playlist = SongAPI::getPlaylist(guild.id);
					discordGuild.writeDataToDB();
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Song Title:**__ [" + searchResults[returnData.currentPageIndex].songTitle + "](" +
											searchResults[returnData.currentPageIndex].viewUrl + ")\n__**Duration:**__ " +
											searchResults[returnData.currentPageIndex].duration + "\n__**Description:**__ " +
											searchResults[returnData.currentPageIndex].description + "\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Song Added To Queue Position: " + std::to_string(SongAPI::getPlaylist(guild.id).songQueue.size()) + "**__ ");
					newEmbed.setColor(discordGuild.data.borderColor);
					newEmbed.setThumbnail(searchResults[returnData.currentPageIndex].thumbnailUrl);
					embedsFromSearch.push_back(newEmbed);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
				}

				return;
			} catch (...) {
				reportException("Play::execute Error: ");
			}
		};
		virtual ~Play();
	};
	std::unordered_map<std::string, int64_t> Play::timeOfLastPlay;
};
```