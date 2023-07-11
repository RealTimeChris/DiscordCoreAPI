Stop {#Stop}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Stop : public BaseFunction {
	  public:
		Stop() {
			commandName = "stop";
			helpDescription = "Stops the current song from playing.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /stop!\n------");
			msgEmbed.setTitle("__**Stop Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Stop>();
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

				InputEventData newEvent = newArgs.eventData;

				int64_t currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
				int64_t previousPlayedTime{};
				if (Play::timeOfLastPlay.contains(newArgs.eventData.getGuildId())) {
					previousPlayedTime = Play::timeOfLastPlay.at(newArgs.eventData.getGuildId());
				}

				if (currentTime - previousPlayedTime < 5000) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but please wait a total of 5 seconds after playing, before attempting to stop!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Timing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
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
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				loadPlaylist(discordGuild);
				if (voiceConnection == nullptr) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Connection Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Stopping Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (!guild.areWeConnected() || !SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::string msgString = "------\n**There's no music playing to be stopped!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Stopping Issue:**__");
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				SongAPI::stop(guild.id);
				savePlaylist(discordGuild);
				EmbedData msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**Songs Remaining In Queue:**__ " + std::to_string(SongAPI::getPlaylist(guild.id).songQueue.size()) + "\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Stopping Playback:**__");
				RespondToInputEventData dataPackage02(newArgs.eventData);
				dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage02.addMessageEmbed(msgEmbed);
				InputEvents::respondToInputEventAsync(dataPackage02).get();
				return;
			} catch (...) {
				reportException("Stop::execute()");
			}
		}
		~Stop(){};
	};

}
```