// Clear.hpp - Header for the "clear" command.
// Aug 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CLEAR_
#define _CLEAR_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Clear :public BaseFunction {
	public:
		Clear() {
			this->commandName = "clear";
			this->helpDescription = "Clears the current song queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !clear or /clear! And that's it!\n------");
			msgEmbed.setTitle("__**Clear Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Clear* create() {
			return new Clear;
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

			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = args->eventData.getAuthorId(),.guildId = args->eventData.getGuildId() }).get();

			bool doWeHaveControl = checkIfWeHaveControl(args->eventData, discordGuild, guildMember);

			if (!doWeHaveControl) {
				co_return;
			}

			shared_ptr<VoiceConnection>* voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

			if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != (*voiceConnection)->getChannelId()) {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Clear Issue:**__");
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

			auto songQueue = SongAPI::getQueue(guild.id);
			songQueue->clear();

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("\n------\n__**You have cleared the song queue!**__\n------");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Queue Cleared:**__");
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
	};

}

#endif
