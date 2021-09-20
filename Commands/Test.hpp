// Test.hpp - Header for the "test" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Test* create() {
			return new Test;
		}
		
		virtual  task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {

			Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild);
			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = args->eventData.getAuthorId(), .guildId = args->eventData.getGuildId() }).get();
			shared_ptr<VoiceConnection>* voiceConnectionRaw = guild.connectToVoice(guildMember.voiceData.channelId);
			InputEventData newEvent = args->eventData;
			if (voiceConnectionRaw == nullptr) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Connection Issue:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					EditInteractionResponseData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
				co_return;
			}
			SongAPI::setPlaylist(discordGuild.data.playlist, discordGuild.data.guildId);

			vector<Song> searchResults;
			searchResults = SongAPI::searchForSong("skrillex", guild.id);

			vector<EmbedData> embedsFromSearch;
			unsigned int x = 0;
			for (auto value : searchResults) {
				x += 1;
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("__**Title:**__ [" + value.songTitle + "](" + value.viewURL + ")" + "\n__**Description:**__ " + value.description + "\n__**Duration:**__ " + value.duration);
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**" + to_string(x) + " of " + to_string(searchResults.size()) + "**__");
				newEmbed.setImage(value.thumbnailURL);
				newEmbed.setColor(discordGuild.data.borderColor);
				embedsFromSearch.push_back(newEmbed);
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
					if (searchResults[currentPageIndex].type == SongType::SoundCloud) {
						auto song = SongAPI::addSoundCloudSongToQueue(guildMember, guild.id, (SoundCloudSong)searchResults[currentPageIndex]);
						discordGuild.data.playlist.songList.push_back(song);
						discordGuild.writeDataToDB();
					}
					else {
						auto song = SongAPI::addYouTubeSongToQueue(guildMember, guild.id, (YouTubeSong)searchResults[currentPageIndex]);
						discordGuild.data.playlist.songList.push_back(song);
						discordGuild.writeDataToDB();
					}
				}
			}
			if (!(*voiceConnectionRaw)->areWeCurrentlyPlaying()) {
				(*voiceConnectionRaw)->onSongCompletion([=](VoiceConnection* voiceConnection) mutable noexcept ->task<void> {
					co_await resume_background();
					discordGuild.getDataFromDB();
					if (SongAPI::isThereAnySongs(guild.id)) {
						SongAPI::sendNextSong(guild.id, guildMember);
						discordGuild.data.playlist = SongAPI::getPlaylist(guild.id);
						discordGuild.writeDataToDB();
						EmbedData newEmbed;
						newEmbed.setAuthor(guildMember.user.userName, guildMember.user.avatar);
						newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewURL + ")" + "\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " +
							SongAPI::getCurrentSong(guild.id).duration + "\n__**Added By:**__ <@!" + SongAPI::getCurrentSong(guild.id).addedByUserId + "> (" + SongAPI::getCurrentSong(guild.id).addedByUserName + ")");
						newEmbed.setImage(SongAPI::getCurrentSong(guild.id).thumbnailURL);
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Now Playing:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
						}
						if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
						}
						if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
						}
						if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
						}
						CreateMessageData dataPackage(newEvent);
						dataPackage.addMessageEmbed(newEmbed);
						auto newEvent02 = InputEvents::respondToEvent(dataPackage);
						voiceConnection->play();
					}
					else {
						discordGuild.getDataFromDB();
						EmbedData newEmbed;
						newEmbed.setAuthor(guildMember.user.userName, guildMember.user.avatar);
						newEmbed.setDescription("------\n__**Sorry, but there's nothing left to play here!**__\n------");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Now Playing:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
						}
						else if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
						}
						else if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
						}
						else if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
						}
						CreateMessageData dataPackage(newEvent);
						dataPackage.addMessageEmbed(newEmbed);
						auto newEvent02 = InputEvents::respondToEvent(dataPackage);
					}
					});

				if (!(*voiceConnectionRaw)->areWeCurrentlyPlaying()) {
					if (SongAPI::isThereAnySongs(guild.id)) {
						SongAPI::sendNextSong(guild.id, guildMember);
						discordGuild.data.playlist = SongAPI::getPlaylist(guild.id);
						discordGuild.writeDataToDB();
						EmbedData newEmbed;
						newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewURL + ")" + "\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " +
							SongAPI::getCurrentSong(guild.id).duration + "\n__**Added By:**__ <@!" + SongAPI::getCurrentSong(guild.id).addedByUserId + "> (" + SongAPI::getCurrentSong(guild.id).addedByUserName + ")");
						newEmbed.setImage(SongAPI::getCurrentSong(guild.id).thumbnailURL);
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Now Playing:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
						}
						else if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
						}
						else if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
						}
						else if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
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
						(*voiceConnectionRaw)->play();
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
				else if (!SongAPI::isThereAnySongs(guild.id)) {
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
				discordGuild.data.playlist = SongAPI::getPlaylist(guild.id);
				discordGuild.writeDataToDB();
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Song Title:**__ [" + searchResults[returnData.currentPageIndex].songTitle + "](" + searchResults[returnData.currentPageIndex].viewURL + ")\n__**Duration:**__ " + searchResults[returnData.currentPageIndex].duration + "\n__**Description:**__ " +
					searchResults[returnData.currentPageIndex].description + "\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Song Added To Queue Position: " + to_string(SongAPI::getPlaylist(guild.id).songQueue.size()) + "**__ ");
				newEmbed.setColor(discordGuild.data.borderColor);
				newEmbed.setThumbnail(searchResults[returnData.currentPageIndex].thumbnailURL);
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

		}
	};
}
#endif
