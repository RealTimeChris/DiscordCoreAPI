// Disconnect.hpp - Header for the "disconnect" function.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCONNECT_
#define _DISCONNECT_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Disconnect :public BaseFunction {
	public:
		Disconnect() {
			this->commandName = "disconnect";
			this->helpDescription = "Disconnect the bot from voice chat.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !disconnect or /disconnect!\n------");
			msgEmbed.setTitle("__**Disconnect Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Disconnect* create() {
			return new Disconnect;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = Channels::getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

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

			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();

			bool doWeHaveControl = checkIfWeHaveControl(args->eventData, discordGuild, guildMember);

			if (!doWeHaveControl) {
				co_return;
			}

			if ((*guild.connectToVoice(guildMember.voiceData.channelId))->areWeConnected()) {
				guild.disconnect();
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**I have disconnected from the channel!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Disconnected:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					CreateInteractionResponseData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
			}
			else {
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("------\n__**I was already disconnected!**__\n------");
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Disconnected:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
				else {
					CreateInteractionResponseData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
				}
			}

			co_return;
		}
	};

}

#endif