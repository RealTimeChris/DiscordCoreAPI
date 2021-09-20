// LoopAll.hpp - Header for the "loop song" command.
// Aug 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _LOOP_ALL_
#define _LOOP_ALL_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {
	class LoopAll :public BaseFunction {
	public:
		LoopAll() {
			this->commandName = "loopall";
			this->helpDescription = "Enables or disables looping of the current queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !loopall or /loopall!\n------");
			msgEmbed.setTitle("__**Loop-All Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		LoopAll* create() {
			return new LoopAll;
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

			shared_ptr<VoiceConnection>* voiceConnectionRaw = guild.connectToVoice(guildMember.voiceData.channelId);

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

			auto voiceConnection = *voiceConnectionRaw;

			if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
				if (voiceConnection->getChannelId() == "" && guildMember.voiceData.channelId != "") {
					voiceConnection = *guild.connectToVoice(guildMember.voiceData.channelId);
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

			if (SongAPI::isLoopAllEnabled(guild.id)) {
				SongAPI::setLoopAllStatus(false, guild.id);
			}
			else {
				SongAPI::setLoopAllStatus(true, guild.id);
			}
			discordGuild.writeDataToDB();
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			if (SongAPI::isLoopAllEnabled(guild.id)) {
				msgEmbed.setDescription("\n------\n__**Looping-All has been enabled!**__\n------\n");
			}
			else {
				msgEmbed.setDescription("\n------\n__**Looping-All has been disabled!**__\n------\n");
			}
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Looping-All Change:**__");
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
			discordGuild.data.playlist = SongAPI::getPlaylist(guild.id);
			discordGuild.writeDataToDB();

			co_return;
		}
	};

}

#endif