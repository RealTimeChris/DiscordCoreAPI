Play {#Play}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	MoveThroughMessagePagesData recurseThroughOptions(MoveThroughMessagePagesData returnData, Int32 currentPageIndex, InputEventData newEvent,
		std::vector<EmbedData> embedsFromSearch, BaseFunctionArguments& newArgs, std::vector<Int32> arrayOfIndices, GuildMember guildMember, std::vector<Song> searchResults) {
		if (returnData.buttonId == "exit") {
			auto currentQueue = SongAPI::getPlaylist(guildMember.guildId);
			Int32 songSize = currentQueue.songQueue.size();
			arrayOfIndices.erase(arrayOfIndices.end() - 1, arrayOfIndices.end());
			for (auto& value: arrayOfIndices) {
				if (value != -1) {
					auto song = SongAPI::addSongToQueue(guildMember, searchResults[value]);
				}
			}
			std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
			String descriptionString{};
			currentQueue = SongAPI::getPlaylist(guildMember.guildId);
			descriptionString = "------\n__**Added the following songs to the queue:\n";
			for (Uint32 x = 0; x < arrayOfIndices.size(); x += 1) {
				descriptionString += "[" + searchResults[arrayOfIndices[x]].songTitle + "](" + searchResults[arrayOfIndices[x]].viewUrl + ")\n" +
					"Position: " + std::to_string(songSize + x + 1) + "\n";
			}
			descriptionString += "**__\n------";
			newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
			newEmbed->setDescription(descriptionString);
			newEmbed->setTimeStamp(getTimeAndDate());
			newEmbed->setTitle("__**Playing New Songs:**__");
			newEmbed->setColor("fefefe");
			RespondToInputEventData dataPackage(returnData.inputEventData);
			dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
			dataPackage.addMessageEmbed(*newEmbed);
			auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
			returnData.currentPageIndex -= 1;
			return returnData;
		} else if (returnData.buttonId == "empty") {
			returnData.currentPageIndex -= 1;
			return returnData;
		} else {
			returnData =
				moveThroughMessagePages(std::to_string(newArgs.eventData.getAuthorId()), InputEventData(newEvent), currentPageIndex, embedsFromSearch, false, 120000, true);
			arrayOfIndices.emplace_back(returnData.currentPageIndex);
			return recurseThroughOptions(returnData, returnData.currentPageIndex, returnData.inputEventData, embedsFromSearch, newArgs, arrayOfIndices, guildMember, searchResults);
		}
	}

	class Play : public BaseFunction {
	  public:
		static std::unordered_map<Int64, Int64> timeOfLastPlay;

		Play() {
			this->commandName = "play";
			this->helpDescription = "Add a song to the queue and play it if nothing is playing.";
			EmbedData newEmbed;
			newEmbed.setDescription("------\nSimply enter /play SONGNAME OR Url.\n------");
			newEmbed.setTitle("__**Play Usage:**__");
			newEmbed.setTimeStamp(getTimeAndDate());
			newEmbed.setColor("FeFeFe");
			this->helpEmbed = newEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Play>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ newArgs.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ newArgs.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				Bool areWeAllowed = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				Bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				InputEventData newEvent = newArgs.eventData;

				Int64 currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				Int64 previousPlayedTime{ 0 };
				if (Play::timeOfLastPlay.contains(newArgs.eventData.getGuildId())) {
					previousPlayedTime = Play::timeOfLastPlay.at(newArgs.eventData.getGuildId());
				}

				if (currentTime - previousPlayedTime < 5000) {
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but please wait a total of 5 seconds in between plays!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Timing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}

				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				previousPlayedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				Play::timeOfLastPlay.insert_or_assign(newArgs.eventData.getGuildId(), previousPlayedTime);
				VoiceStateData voiceStateData{};
				if (guild.voiceStates.contains(guildMember.id)) {
					voiceStateData = guild.voiceStates.at(guildMember.id);
				} else {
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
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
				VoiceConnection* voiceConnection = guild.connectToVoice(guildMember.id, 0, false, false);
				if (voiceConnection == nullptr) {
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
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
				loadPlaylist(discordGuild);

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
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

				std::vector<Song> searchResults{};
				if (newArgs.optionsArgs.size() > 0) {
					searchResults = SongAPI::searchForSong(newArgs.optionsArgs[0], guild.id);
				}

				if (searchResults.size() <= 0 && newArgs.optionsArgs.size() > 0) {
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**No songs could be found as a result of your search!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Search Issue:**__");
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

				std::vector<EmbedData> embedsFromSearch;
				Uint32 x = 0;
				for (Song& value: searchResults) {
					x += 1;
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("__**Title:**__ [" + value.songTitle + "](" + value.viewUrl + ")" + "\n__**Description:**__ " + value.description +
						"\n__**Duration:**__ " + value.duration);
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**" + std::to_string(x) + " of " + std::to_string(searchResults.size()) + "**__");
					newEmbed->setImage(value.thumbnailUrl);
					newEmbed->setColor(discordGuild.data.borderColor);
					embedsFromSearch.emplace_back(*newEmbed);
				}

				Uint32 currentPageIndex = 0;
				MoveThroughMessagePagesData returnData{};
				std::vector<Int32> arrayOfIndices{};
				if (embedsFromSearch.size() > 0) {
					RespondToInputEventData dataPackage0(newEvent);
					dataPackage0.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage0.addMessageEmbed(embedsFromSearch[0]);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage0).get();

					returnData = recurseThroughOptions(returnData, currentPageIndex, newEvent, embedsFromSearch, newArgs, arrayOfIndices, guildMember, searchResults);
					savePlaylist(discordGuild);
					discordGuild.writeDataToDB();
				}
				auto channelId = newArgs.eventData.getChannelId();
				if (!SongAPI::areWeCurrentlyPlaying(guild.id)) {
					auto theTask = [=](SongCompletionEventData eventData) mutable noexcept -> CoRoutine<void> {
						co_await NewThreadAwaitable<void>();
						std::this_thread::sleep_for(150ms);
						if (SongAPI::isThereAnySongs(guild.id)) {
							std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
							if (!eventData.wasItAFail) {
								if (!SongAPI::sendNextSong(guildMember)) {
									InputEvents::deleteInputEventResponseAsync(newEvent);
									SongAPI::play(guildMember.guildId);
									co_return;
								}
								savePlaylist(discordGuild);
								newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
								newEmbed->setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(eventData.guild.id).songTitle + "](" +
									SongAPI::getCurrentSong(eventData.guild.id).viewUrl + ")" + "\n__**Description:**__ " +
									SongAPI::getCurrentSong(eventData.guild.id).description + "\n__**Duration:**__ " + SongAPI::getCurrentSong(eventData.guild.id).duration +
									"\n__**Added By:**__ <@!" + std::to_string(SongAPI::getCurrentSong(guild.id).addedByUserId) + "> (" +
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
								newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
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
									newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
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
							std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
							newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
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
					if (SongAPI::isThereAnySongs(guild.id)) {
						if (!SongAPI::sendNextSong(guildMember)) {
							InputEvents::deleteInputEventResponseAsync(newEvent);
							SongAPI::play(guild.id);
						}
						std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
						newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						newEmbed->setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewUrl + ")" +
							"\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " + SongAPI::getCurrentSong(guild.id).duration +
							"\n__**Added By:**__ <@!" + std::to_string(SongAPI::getCurrentSong(guild.id).addedByUserId) + "> (" +
							SongAPI::getCurrentSong(guild.id).addedByUserName + ")");
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
						RespondToInputEventData dataPackage02(newEvent);
						dataPackage02.setResponseType(InputEventResponseType::Follow_Up_Message);
						dataPackage02.addMessageEmbed(*newEmbed);
						newEvent = InputEvents::respondToInputEventAsync(dataPackage02).get();
						savePlaylist(discordGuild);
						SongAPI::play(guild.id);

					} else {
						std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
						newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						newEmbed->setDescription("------\n__**Sorry, but there's nothing to play!**__\n------");
						newEmbed->setTimeStamp(getTimeAndDate());
						newEmbed->setTitle("__**Playing Issue:**__");
						newEmbed->setColor(discordGuild.data.borderColor);
						embedsFromSearch.emplace_back(*newEmbed);
						RespondToInputEventData dataPackage(newEvent);
						dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
						dataPackage.addMessageEmbed(*newEmbed);
						auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
						newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
						InputEvents::deleteInputEventResponseAsync(newEvent).get();
						InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					}
					SongAPI::onSongCompletion(theTask, guild.id);
				} else if (newArgs.optionsArgs.size() == 0 && SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but there's already something playing!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					embedsFromSearch.emplace_back(*newEmbed);
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				} else if (returnData.currentPageIndex != -1 && arrayOfIndices.size() < 2) {
					discordGuild.data.playlist = SongAPI::getPlaylist(guild.id);
					discordGuild.writeDataToDB();
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					newEmbed->setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**Song Title:**__ [" + searchResults[returnData.currentPageIndex].songTitle + "](" +
						searchResults[returnData.currentPageIndex].viewUrl + ")\n__**Duration:**__ " + searchResults[returnData.currentPageIndex].duration +
						"\n__**Description:**__ " + searchResults[returnData.currentPageIndex].description + "\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Song Added To Queue Position: " + std::to_string(SongAPI::getPlaylist(guild.id).songQueue.size()) + "**__ ");
					newEmbed->setColor(discordGuild.data.borderColor);
					newEmbed->setThumbnail(searchResults[returnData.currentPageIndex].thumbnailUrl);
					embedsFromSearch.emplace_back(*newEmbed);
					RespondToInputEventData dataPackage02(newEvent);
					dataPackage02.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage02.addMessageEmbed(*newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage02).get();
				}
				return;
			} catch (...) {
				reportException("Play::execute()");
			}
		};
		~Play(){};
	};
	std::unordered_map<Int64, Int64> Play::timeOfLastPlay{};

};
```