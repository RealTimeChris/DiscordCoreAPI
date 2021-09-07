// LoopSong.hpp - Header for the "loop song" command.
// Aug 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _LOOP_SONG_
#define _LOOP_SONG_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {
	class LoopSong :public BaseFunction {
	public:
		LoopSong() {
			this->commandName = "LoopSong";
			this->helpDescription = "Enables or disables looping of the current song.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !loopsong or /loopsong!\n------");
			msgEmbed.setTitle("__**Loop-Song Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		LoopSong* create() {
			return new LoopSong;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = Channels::getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm) {
				co_return;
			}

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();

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

			auto voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

			if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != (*voiceConnection)->getChannelId()) {
				if ((*voiceConnection)->getChannelId() == "" && guildMember.voiceData.channelId != "") {
					voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);
				}
				else {
					EmbedData newEmbed;
					newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Looping Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData dataPackage(args->eventData);
						dataPackage.addMessageEmbed(newEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
					}
					else {
						CreateEphemeralInteractionResponseData dataPackage(args->eventData);
						dataPackage.addMessageEmbed(newEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
					}
					co_return;
				}
			}
			if (YouTubeAPI::isLoopSongEnabled(guild.id)) {
				YouTubeAPI::setLoopSongStatus(false, guild.id);
			}
			else {
				YouTubeAPI::setLoopSongStatus(true, guild.id);
			}
			discordGuild.writeDataToDB();
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			if (YouTubeAPI::isLoopSongEnabled(guild.id)) {
				msgEmbed.setDescription("\n------\n__**Looping-Song has been enabled!**__\n------\n");
			}
			else {
				msgEmbed.setDescription("\n------\n__**Looping-Song has been disabled!**__\n------\n");
			}
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Looping-Song Change:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
			}
			else {
				CreateEphemeralInteractionResponseData dataPackage(args->eventData);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
			}
			co_return;

			co_return;
		}
	};

}

#endif