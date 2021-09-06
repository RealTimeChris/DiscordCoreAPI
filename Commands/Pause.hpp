// Pause.hpp - Header for the "pause" command.
// Aug 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _PAUSE_
#define _PAUSE_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Pause :public BaseFunction {
	public:
		Pause() {
			this->commandName = "pause";
			this->helpDescription = "Pauses the currently playing music.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !pause or /pause!\n------");
			msgEmbed.setTitle("__**Pause Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Pause* create() {
			return new Pause;
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

			bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(args->eventData, discordGuild);

			if (!checkIfAllowedInChannel ) {
				co_return;
			}

			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();

			bool doWeHaveControl = checkIfWeHaveControl(args->eventData, discordGuild, guildMember);

			if (!doWeHaveControl) {
				co_return;
			}

			shared_ptr<VoiceConnection> voiceConnection = *guild.connectToVoice(guildMember.voiceData.channelId);

			if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Pauseping Issue:**__");
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

			if (!voiceConnection->areWeConnected() || !voiceConnection->areWeCurrentlyPlaying()) {
				string msgString = "------\n**There's no music playing to be paused!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Pausing Issue:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				co_return;
			}
			voiceConnection->pauseToggle();

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("\n------\n__**Songs Remaining In Queue:**__ " + to_string(YouTubeAPI::getQueue(guild.id)->size()) + "\n------");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Paused Playback:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
			}
			else {
				CreateEphemeralInteractionResponseData dataPackage(args->eventData);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
			}

			co_return;
		}
	};

}

#endif
