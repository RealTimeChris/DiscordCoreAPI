// Stop.hpp - Header for the "stop" command.
// Aug 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _STOP_
#define _STOP_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Stop :public BaseFunction {
	public:
		Stop() {
			this->commandName = "stop";
			this->helpDescription = "Stops the current song from playing.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !stop or /stop!\n------");
			msgEmbed.setTitle("__**Stop Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Stop* create() {
			return new Stop;
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

			if (!checkIfAllowedInChannel) {
				co_return;
			}

			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();

			bool doWeHaveControl = checkIfWeHaveControl(args->eventData, discordGuild, guildMember);

			if (!doWeHaveControl) {
				co_return;
			}

			InputEventData newEvent = args->eventData;
			if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateDeferredInteractionResponseData dataPackage(args->eventData);
				newEvent = InputEvents::respondToEvent(dataPackage);
			}

			shared_ptr<VoiceConnection> voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

			if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Stopping Issue:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					CreateFollowUpMessageData dataPackage(newEvent);
					dataPackage.addContent("Empty Content");
					newEvent = InputEvents::respondToEvent(dataPackage);
					CreateEphemeralFollowUpMessageData dataPackage02(newEvent);
					dataPackage02.addMessageEmbed(newEmbed);
					InputEvents::respondToEvent(dataPackage02);
					InputEvents::deleteInputEventResponseAsync(newEvent);
				}
				
				co_return;
			}

			if (!voiceConnection->areWeConnected() || !voiceConnection->areWeCurrentlyPlaying()) {
				string msgString = "------\n**There's no music playing to be stopped!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Stopping Issue:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
				}
				else {
					CreateFollowUpMessageData dataPackage(newEvent);
					dataPackage.addContent("Empty Content");
					newEvent = InputEvents::respondToEvent(dataPackage);
					CreateEphemeralFollowUpMessageData dataPackage02(newEvent);
					dataPackage02.addMessageEmbed(msgEmbed);
					InputEvents::respondToEvent(dataPackage02);
					InputEvents::deleteInputEventResponseAsync(newEvent);
				}
				
				co_return;
			}
			if (YouTubeAPI::stop(guild.id)) {
				voiceConnection->stop();
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**Songs Remaining In Queue:**__ " + to_string(YouTubeAPI::getQueue(guild.id)->size()) + "\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Stopping Playback:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
				}
				else {
					CreateFollowUpMessageData dataPackage(newEvent);
					dataPackage.addContent("Empty Content");
					newEvent = InputEvents::respondToEvent(dataPackage);
					CreateEphemeralFollowUpMessageData dataPackage02(newEvent);
					dataPackage02.addMessageEmbed(msgEmbed);
					InputEvents::respondToEvent(dataPackage02);
					InputEvents::deleteInputEventResponseAsync(newEvent);
				}
			}
			else {
				string msgString = "------\n**There's no music playing to be stopped!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Stopping Issue:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(newEvent);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
				}
				else {
					CreateFollowUpMessageData dataPackage(newEvent);
					dataPackage.addContent("Empty Content");
					newEvent = InputEvents::respondToEvent(dataPackage);
					CreateEphemeralFollowUpMessageData dataPackage02(newEvent);
					dataPackage02.addMessageEmbed(msgEmbed);
					InputEvents::respondToEvent(dataPackage02);
					InputEvents::deleteInputEventResponseAsync(newEvent);
				}
			}

			

			co_return;
		}
	};

}

#endif
