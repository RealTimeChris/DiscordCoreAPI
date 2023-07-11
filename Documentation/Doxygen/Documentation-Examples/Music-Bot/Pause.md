Pause {#Pause}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Pause : public BaseFunction {
	  public:
		Pause() {
			commandName = "pause";
			helpDescription = "Pauses the currently playing music.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /pause!\n------");
			msgEmbed.setTitle("__**Pause Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Pause>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);
				bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);
				if (!checkIfAllowedInChannel) {
					return;
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();
				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);
				if (!doWeHaveControl) {
					return;
				}

				RespondToInputEventData dataPackage00(newArgs.eventData);
				dataPackage00.setResponseType(InputEventResponseType::Ephemeral_Deferred_Response);
				InputEvents::respondToInputEventAsync(dataPackage00).get();
				VoiceConnection* voiceConnection{};
				VoiceStateData voiceStateData{};
				if (guild.voiceStates.contains(guildMember.id)) {
					voiceStateData = guild.voiceStates.at(guildMember.id);
					voiceConnection = guild.connectToVoice(0, voiceStateData.channelId, true, false);
				} else {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Connection Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				loadPlaylist(discordGuild);
				if (voiceConnection == nullptr || !guild.areWeConnected()) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Connection Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Pausing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (!SongAPI::areWeCurrentlyPlaying(guild.id)) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but I need to be either playing or paused for this command to be possible!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Pausing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				SongAPI::pauseToggle(guild.id);

				EmbedData msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**Songs Remaining In Queue:**__ " + std::to_string(SongAPI::getPlaylist(guild.id).songQueue.size()) + "\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Paused Playback:**__");
				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (...) {
				reportException("Pause::execute()");
			}
		}
		~Pause(){};
	};

}
```