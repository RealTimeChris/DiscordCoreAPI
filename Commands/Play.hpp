// Play.hpp - Header for the "play" command.
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _PLAY_
#define _PLAY_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Play : public BaseFunction {
	public:
		Play() {
			this->commandName = "play";
			this->helpDescription = "Add a song to the queue and play it if nothing is playing.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !play = SONGNAME OR URL, or /play SONGNAME OR URL.\n------");
			msgEmbed.setTitle("__**Play Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Play* create() {
			return new Play;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = Channels::getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm) {
				co_return;
			}

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			Guild guild = Guilds::getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			bool areWeAllowed = checkIfAllowedPlayingInChannel(args->eventData, discordGuild);

			if (!areWeAllowed) {
				co_return;
			}

			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = args->eventData.getAuthorId(),.guildId = args->eventData.getGuildId() }).get();

			bool doWeHaveControl = checkIfWeHaveControl(args->eventData, discordGuild, guildMember);

			if (!doWeHaveControl) {
				co_return;
			}

			shared_ptr<VoiceConnection> voiceConnection = *guild.connectToVoice(guildMember.voiceData.channelId);

			InputEventData newEvent = args->eventData;
			if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateDeferredInteractionResponseData dataPackage(newEvent);
				newEvent = InputEvents::respondToEvent(dataPackage);
			}
			vector<YouTubeSearchResult> searchResults;
			if (args->argumentsArray.size() > 0) {
				searchResults = YouTubeAPI::searchForVideo(args->argumentsArray[0], guild.id);
			}

			vector<EmbedData> embedsFromSearch;
			unsigned int x = 0;
			for (auto value : searchResults) {
				x += 1;
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("__**Title:**__ [" + value.videoTitle + "](" + value.videoURL + ")" + "\n__**Description:**__ " + value.description + "\n__**Duration:**__ " + value.duration);
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**" + to_string(x) + " of " + to_string(searchResults.size()) + "**__");
				newEmbed.setImage(value.thumbNailURL);
				newEmbed.setColor(discordGuild.data.borderColor);
				embedsFromSearch.push_back(newEmbed);
			}

			if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Playing Issue:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				embedsFromSearch.push_back(newEmbed);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					EditInteractionResponseData dataPackage(newEvent);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				co_return;
			}
			unsigned int currentPageIndex = 0;
			RecurseThroughMessagePagesData returnData;
			if (embedsFromSearch.size() > 0) {
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(embedsFromSearch[0]);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					CreateInteractionResponseData dataPackage(newEvent);
					dataPackage.addMessageEmbed(embedsFromSearch[0]);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}

				returnData = recurseThroughMessagePages(args->eventData.getAuthorId(), newEvent, currentPageIndex, embedsFromSearch, false, 120000, true);
				discordGuild.getDataFromDB();
				if (returnData.buttonId == "exit") {
					co_return;
				}
				if (returnData.currentPageIndex != -1) {
					YouTubeAPI::addSongToQueue(searchResults[returnData.currentPageIndex], guildMember, guild.id);
				}
			}

			if (!voiceConnection->areWeCurrentlyPlaying()) {
				voiceConnection->onSongCompletion([=](VoiceConnection* voiceConnection) mutable noexcept ->task<void> {
					co_await resume_background();
					discordGuild.getDataFromDB();
					if (YouTubeAPI::isThereAnySongs(guild.id)) {
						try {
							auto returnValue01 = YouTubeAPI::sendNextSong(guild.id);
							voiceConnection->play();
						}
						catch (exception& e) {
							EmbedData newEmbed;
							newEmbed.setAuthor(guildMember.user.userName, guildMember.user.avatar);
							string newString = "__**Sorry, there was an error while trying to play that last song!\nIt was: ";
							newString += e.what();
							newString += "**__";
							newEmbed.setDescription(newString);
							newEmbed.setTimeStamp(getTimeAndDate());
							newEmbed.setTitle("__**Error Playing:**__");
							newEmbed.setColor(discordGuild.data.borderColor);
							if (YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
							}
							if (!YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
							}
							if (YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
							}
							if (!YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
							}
							if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
								ReplyMessageData dataPackage(newEvent);
								dataPackage.addMessageEmbed(newEmbed);
								auto newEvent02 = InputEvents::respondToEvent(dataPackage);
							}
							else {
								CreateFollowUpMessageData dataPackage(newEvent);
								dataPackage.addMessageEmbed(newEmbed);
								auto newEvent02 = InputEvents::respondToEvent(dataPackage);
							}
						}
						EmbedData newEmbed;
						newEmbed.setAuthor(guildMember.user.userName, guildMember.user.avatar);
						newEmbed.setDescription("__**Title:**__ [" + YouTubeAPI::getCurrentSong(guild.id).title + "](" + YouTubeAPI::getCurrentSong(guild.id).url + ")" + "\n__**Description:**__ " + YouTubeAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " +
							YouTubeAPI::getCurrentSong(guild.id).duration + "\n__**Added By:**__ <@!" + YouTubeAPI::getCurrentSong(guild.id).addedById + "> (" + YouTubeAPI::getCurrentSong(guild.id).addedByUserName + ")");
						newEmbed.setImage(YouTubeAPI::getCurrentSong(guild.id).imageURL);
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Now Playing:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						if (YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
						}
						if (!YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
						}
						if (YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
						}
						if (!YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
						}
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData dataPackage(newEvent);
							dataPackage.addMessageEmbed(newEmbed);
							auto newEvent02 = InputEvents::respondToEvent(dataPackage);
						}
						else {
							CreateFollowUpMessageData dataPackage(newEvent);
							dataPackage.addMessageEmbed(newEmbed);
							auto newEvent02 = InputEvents::respondToEvent(dataPackage);
						}
					}
					else {
						discordGuild.getDataFromDB();
						EmbedData newEmbed;
						newEmbed.setAuthor(guildMember.user.userName, guildMember.user.avatar);
						newEmbed.setDescription("------\n__**Sorry, but there's nothing left to play here!**__\n------");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Now Playing:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						if (YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
						}
						else if (!YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
						}
						else if (YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
						}
						else if (!YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
						}
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData dataPackage(newEvent);
							dataPackage.addMessageEmbed(newEmbed);
							auto newEvent02 = InputEvents::respondToEvent(dataPackage);
						}
						else {
							CreateEphemeralFollowUpMessageData dataPackage(newEvent);
							dataPackage.addMessageEmbed(newEmbed);
							auto newEvent02 = InputEvents::respondToEvent(dataPackage);
						}
					}
					});

				if (!voiceConnection->areWeCurrentlyPlaying()) {
					if (YouTubeAPI::isThereAnySongs(guild.id)) {
						try {
							auto returnValue02 = YouTubeAPI::sendNextSong(guild.id);
							EmbedData newEmbed;
							newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
							newEmbed.setDescription("__**Title:**__ [" + YouTubeAPI::getCurrentSong(guild.id).title + "](" + YouTubeAPI::getCurrentSong(guild.id).url + ")" + "\n__**Description:**__ " + YouTubeAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " +
								YouTubeAPI::getCurrentSong(guild.id).duration + "\n__**Added By:**__ <@!" + YouTubeAPI::getCurrentSong(guild.id).addedById + "> (" + YouTubeAPI::getCurrentSong(guild.id).addedByUserName + ")");
							newEmbed.setImage(YouTubeAPI::getCurrentSong(guild.id).imageURL);
							newEmbed.setTimeStamp(getTimeAndDate());
							newEmbed.setTitle("__**Now Playing:**__");
							newEmbed.setColor(discordGuild.data.borderColor);
							if (YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
							}
							else if (!YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
							}
							else if (YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
							}
							else if (!YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
							}
							if (newEvent.eventType == InputEventType::REGULAR_MESSAGE) {
								ReplyMessageData dataPackage(newEvent);
								dataPackage.addMessageEmbed(newEmbed);
								newEvent = InputEvents::respondToEvent(dataPackage);
							}
							else {
								CreateFollowUpMessageData dataPackage(newEvent);
								dataPackage.addMessageEmbed(newEmbed);
								newEvent = InputEvents::respondToEvent(dataPackage);
							}
							voiceConnection->play();
						}
						catch (exception& e) {
							EmbedData newEmbed;
							newEmbed.setAuthor(guildMember.user.userName, guildMember.user.avatar);
							string newString = "__**Sorry, there was an error while trying to play that last song!\nIt was: "; 
							newString += e.what(); 
							newString += "**__";
							newEmbed.setDescription(newString);
							newEmbed.setTimeStamp(getTimeAndDate());
							newEmbed.setTitle("__**Error Playing:**__");
							newEmbed.setColor(discordGuild.data.borderColor);
							if (YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
							}
							if (!YouTubeAPI::isLoopAllEnabled(guild.id) && YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
							}
							if (YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
							}
							if (!YouTubeAPI::isLoopAllEnabled(guild.id) && !YouTubeAPI::isLoopSongEnabled(guild.id)) {
								newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
							}
							if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
								ReplyMessageData dataPackage(newEvent);
								dataPackage.addMessageEmbed(newEmbed);
								auto newEvent02 = InputEvents::respondToEvent(dataPackage);
							}
							else {
								CreateFollowUpMessageData dataPackage(newEvent);
								dataPackage.addMessageEmbed(newEmbed);
								auto newEvent02 = InputEvents::respondToEvent(dataPackage);
							}
						}
					}
					else {
						EmbedData newEmbed;
						newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						newEmbed.setDescription("------\n__**Sorry, but there's nothing to play!**__\n------");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Playing Issue:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						embedsFromSearch.push_back(newEmbed);
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData dataPackage(newEvent);
							dataPackage.addMessageEmbed(newEmbed);
							newEvent = InputEvents::respondToEvent(dataPackage);
							InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
						}
						else {
							CreateEphemeralFollowUpMessageData dataPackage(newEvent);
							dataPackage.addMessageEmbed(newEmbed);
							newEvent = InputEvents::respondToEvent(dataPackage);
						}
					}
				}
				else if (!YouTubeAPI::isThereAnySongs(guild.id)) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					newEmbed.setDescription("------\n__**Sorry, but there's nothing to play!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Playing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					embedsFromSearch.push_back(newEmbed);
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData dataPackage(newEvent);
						dataPackage.addMessageEmbed(newEmbed);
						newEvent = InputEvents::respondToEvent(dataPackage);
						InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
					}
					else {
						CreateEphemeralFollowUpMessageData dataPackage(newEvent);
						dataPackage.addMessageEmbed(newEmbed);
						newEvent = InputEvents::respondToEvent(dataPackage);
					}
				}
			}
			else if (args->argumentsArray.size() == 0) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but there's already something playing!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Playing Issue:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				embedsFromSearch.push_back(newEmbed);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					CreateEphemeralFollowUpMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
			}
			else if (returnData.currentPageIndex != -1) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Song Title:**__ [" + searchResults[returnData.currentPageIndex].videoTitle + "](" + searchResults[returnData.currentPageIndex].videoURL + ")\n__**Duration:**__ " + searchResults[returnData.currentPageIndex].duration + "\n__**Description:**__ " +
					searchResults[returnData.currentPageIndex].description + "\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Song Added To Queue Position: " + to_string(YouTubeAPI::getQueue(guild.id)->size()) + "**__ ");
				newEmbed.setColor(discordGuild.data.borderColor);
				newEmbed.setThumbnail(searchResults[returnData.currentPageIndex].thumbNailURL);
				embedsFromSearch.push_back(newEmbed);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					CreateFollowUpMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
			}
			co_return;


			
		};

	};
		
	
};
#endif