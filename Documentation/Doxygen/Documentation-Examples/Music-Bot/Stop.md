Stop {#Stop}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"
	namespace DiscordCoreAPI {

	class Stop : public DiscordCoreAPI::BaseFunction {
	  public:
		Stop() {
			this->commandName = "stop";
			this->helpDescription = "Stops the current song from playing.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /stop!\n------");
			msgEmbed.setTitle("__**Stop Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Stop>();
		}
		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({.channelId = args.eventData->getChannelId()}).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm) {
					return;
				}

				Guild guild = Guilds::getCachedGuildAsync({.guildId = args.eventData->getGuildId()}).get();
				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();
				DiscordGuild discordGuild(guild);

				bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(*args.eventData, discordGuild);

				if (!checkIfAllowedInChannel) {
					return;
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({.guildMemberId = args.eventData->getAuthorId(), .guildId = args.eventData->getGuildId()}).get();

				bool doWeHaveControl = checkIfWeHaveControl(*args.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				std::unique_ptr<InputEventData> newEvent = std::make_unique<InputEventData>(*args.eventData);

				int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				int64_t previousPlayedTime {0};
				if (Play::timeOfLastPlay.contains(args.eventData->getGuildId())) {
					previousPlayedTime = Play::timeOfLastPlay.at(args.eventData->getGuildId());
				}

				if (currentTime-previousPlayedTime < 5000) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but please wait a total of 5 seconds after playing, before attempting to stop!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Timing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				VoiceConnection* voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId, true, false);
				loadPlaylist(discordGuild);
				if (voiceConnection == nullptr) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Connection Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				if (guildMember.voiceData.channelId == "" || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Stopping Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				if (!guild.areWeConnected() || !SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::string msgString = "------\n**There's no music playing to be stopped!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Stopping Issue:**__");
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				SongAPI::stop(guild.id);
				savePlaylist(discordGuild);
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**Songs Remaining In Queue:**__ " + std::to_string(SongAPI::getPlaylist(guild.id).songQueue.size()) +
										"\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Stopping Playback:**__");
				RespondToInputEventData dataPackage02(*args.eventData);
				dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage02.addMessageEmbed(msgEmbed);
				InputEvents::respondToInputEventAsync(dataPackage02);
				return;
			} catch (...) {
				reportException("Stop::execute Error: ");
			}
		}
		virtual ~Stop();
	};
}
```