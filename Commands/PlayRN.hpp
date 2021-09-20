// PlayRN.hpp - Header for the "play right now" command.
// Sep 9, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _PLAY_RN_
#define _PLAY_RN_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class PlayRN : public BaseFunction {
	public:

		PlayRN() {
			this->commandName = "playrn";
			this->helpDescription = "Add a song to the front of the queue and play it if nothing is playing.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !playrn = SONGNAME OR URL, or /playrn SONGNAME OR URL.\n------");
			msgEmbed.setTitle("__**PlayRN Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		PlayRN* create() {
			return new PlayRN;
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

			shared_ptr<VoiceConnection>* voiceConnectionRaw = guild.connectToVoice(guildMember.voiceData.channelId);

			SongAPI::setPlaylist(discordGuild.data.playlist, discordGuild.data.guildId);

			InputEventData newEvent = args->eventData;
			if (voiceConnectionRaw == nullptr) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Connection Issue:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				RespondToInputEventData dataPackage(newEvent);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
				}
				else {
					dataPackage.type = DesiredInputEventResponseType::InteractionResponseEdit;
				}
				dataPackage.addMessageEmbed(newEmbed);
				newEvent = InputEvents::respondToEvent(dataPackage);
				InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				co_return;
			}

			auto voiceConnection = *voiceConnectionRaw;
			if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateDeferredInteractionResponseData dataPackage(newEvent);
				newEvent = InputEvents::respondToEvent(dataPackage);
			}
			vector<Song> searchResults;
			if (args->argumentsArray.size() > 0) {
				searchResults = SongAPI::searchForSong(args->argumentsArray[0], guild.id);
			}

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

			if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Playing Issue:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				embedsFromSearch.push_back(newEmbed);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponseEdit;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				co_return;
			}

			if (embedsFromSearch.size() > 0) {
				Song song{};
				if (searchResults[0].type == SongType::SoundCloud) {
					song = SongAPI::addSoundCloudSongToQueue(guildMember, guild.id, (SoundCloudSong)searchResults[0]);
					discordGuild.data.playlist.songList.push_back(song);
					discordGuild.writeDataToDB();
				}
				else {
					song = SongAPI::addYouTubeSongToQueue(guildMember, guild.id, (YouTubeSong)searchResults[0]);
					discordGuild.data.playlist.songList.push_back(song);
					discordGuild.writeDataToDB();
				}
				SongAPI::stop(guild.id);
				auto playlist = SongAPI::getPlaylist(guild.id);

				vector<Song> songVector{};
				playlist.songQueue.erase(playlist.songQueue.end() - 1);
				songVector.push_back(song);
				for (auto value : playlist.songQueue) {
					songVector.push_back(value);
				}
				playlist.songQueue = songVector;
				SongAPI::setPlaylist(playlist, guild.id);
				discordGuild.data.playlist.songList = songVector;
				discordGuild.writeDataToDB();
			}

			if (!voiceConnection->areWeCurrentlyPlaying()) {
				voiceConnection->onSongCompletion([=](VoiceConnection* voiceConnection) mutable noexcept ->task<void> {
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
			}
			if (SongAPI::isThereAnySongs(guild.id) && (args->argumentsArray.size() > 0 || !voiceConnection->areWeCurrentlyPlaying())) {
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
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponseEdit;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
				voiceConnection->play();
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
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponseEdit;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
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
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					RespondToInputEventData dataPackage(newEvent);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponseEdit;
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
			}
			co_return;
		};

	};

}

#endif