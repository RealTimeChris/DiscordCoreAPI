Skip {#Skip}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {
	class Skip : public DiscordCoreAPI::BaseFunction {
	  public:
		static std::unordered_map<std::string, int64_t> timeOfLastSkip;

		Skip() {
			this->commandName = "skip";
			this->helpDescription = "Skips to the next song in the queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /skip.\n------");
			msgEmbed.setTitle("__**Skip Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Skip>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({.channelId = args.eventData->getChannelId()}).get();
				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm) {
					return;
				}

				Guild guild = Guilds::getCachedGuildAsync({.guildId = args.eventData->getGuildId()}).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedPlayingInChannel(*args.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({.guildMemberId = args.eventData->getAuthorId(), .guildId = args.eventData->getGuildId()}).get();

				bool doWeHaveControl = checkIfWeHaveControl(*args.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}
				std::unique_ptr<InputEventData> newEvent = std::make_unique<InputEventData>(*args.eventData);

				int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				int64_t previousSkippedTime {0};
				if (Skip::timeOfLastSkip.contains(args.eventData->getGuildId())) {
					previousSkippedTime = Skip::timeOfLastSkip.at(args.eventData->getGuildId());
				}

				if (currentTime-previousSkippedTime < 5000) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but please wait a total of 5 seconds in between skips!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Timing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				previousSkippedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				Skip::timeOfLastSkip[args.eventData->getGuildId()] = previousSkippedTime;

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

				if (guildMember.voiceData.channelId == 0 || guildMember.voiceData.channelId != voiceConnection->getChannelId()) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Skipping Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage02(*args.eventData);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(newEmbed);
					InputEvents::respondToInputEventAsync(dataPackage02);

					return;
				}

				if (!guild.areWeConnected() || !SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::string msgString = "------\n**There's no music playing to be skipped!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Skipping Issue:**__");
					RespondToInputEventData dataPackage02(*args.eventData);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage02);

					return;
				}

				if (!SongAPI::isThereAnySongs(guild.id)) {
					std::string msgString = "------\n**There's no more songs for us to skip to!**\n------";
					EmbedData msgEmbed02;
					msgEmbed02.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed02.setColor(discordGuild.data.borderColor);
					msgEmbed02.setTimeStamp(getTimeAndDate());
					msgEmbed02.setDescription(msgString);
					msgEmbed02.setTitle("__**Song Queue Issue:**__");
					RespondToInputEventData dataPackage02(*args.eventData);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed02);
					InputEvents::respondToInputEventAsync(dataPackage02);

					return;
				} else {
					if (SongAPI::areWeCurrentlyPlaying(guild.id) && SongAPI::isThereAnySongs(guild.id)) {
						std::string msgString = "------\n**We're skipping to the next song!**\n------";
						EmbedData msgEmbed02;
						msgEmbed02.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl())
							.setColor(discordGuild.data.borderColor)
							.setTimeStamp(getTimeAndDate())
							.setDescription(msgString)
							.setTitle("__**Song Skip:**__");
						RespondToInputEventData dataPackage02(*args.eventData);
						dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbed02);
						auto newEvent02 = InputEvents::respondToInputEventAsync(dataPackage02);
						SongAPI::skip(guildMember);
						savePlaylist(discordGuild);
					} else if (!SongAPI::isThereAnySongs(guild.id)) {
						EmbedData newEmbed;
						newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						newEmbed.setDescription("------\n__**Sorry, but there's nothing left to play here!**__\n------");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**Now Playing:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
						} else if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
						} else if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
						} else if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
							newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
						}
						RespondToInputEventData dataPackage02(*args.eventData);
						dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(newEmbed);
						InputEvents::respondToInputEventAsync(dataPackage02);
						return;
					} else {
						std::string msgString = "------\n**There's no music playing to be skipped!**\n------";
						EmbedData msgEmbed02;
						msgEmbed02.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed02.setColor(discordGuild.data.borderColor);
						msgEmbed02.setTimeStamp(getTimeAndDate());
						msgEmbed02.setDescription(msgString);
						msgEmbed02.setTitle("__**Skipping Issue:**__");
						RespondToInputEventData dataPackage02(*args.eventData);
						dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbed02);
						InputEvents::respondToInputEventAsync(dataPackage02);
						return;
					}
				}
				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();
				return;
			} catch (...) {
				reportException("Skip::execute Error: ");
			}
		}
		virtual ~Skip();
	};
	std::unordered_map<std::string, int64_t> Skip::timeOfLastSkip;
}
```