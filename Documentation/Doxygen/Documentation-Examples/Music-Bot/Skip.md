Skip {#Skip}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class Skip : public BaseFunction {
	  public:
		static UnorderedMap<uint64_t, int64_t> timeOfLastSkip;

		Skip() {
			commandName = "skip";
			helpDescription = "Skips to the next song in the queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /skip.\n------");
			msgEmbed.setTitle("__**Skip Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Skip>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);

				if (!areWeAllowed) {
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
				int64_t previousSkippedTime{};
				if (Skip::timeOfLastSkip.contains(newArgs.eventData.getGuildId())) {
					previousSkippedTime = Skip::timeOfLastSkip.at(newArgs.eventData.getGuildId());
				}

				if (currentTime - previousSkippedTime < 5000) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**Sorry, but please wait a total of 5 seconds in between skips!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Timing Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				previousSkippedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
				Skip::timeOfLastSkip.insert_or_assign(newArgs.eventData.getGuildId(), previousSkippedTime);
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
					newEmbed.setTitle("__**Skipping Issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage02(newArgs.eventData);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(newEmbed);
					InputEvents::respondToInputEventAsync(dataPackage02).get();

					return;
				}

				if (!guild.areWeConnected() || !SongAPI::areWeCurrentlyPlaying(guild.id)) {
					std::string msgString = "------\n**There's no music playing to be skipped!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Skipping Issue:**__");
					RespondToInputEventData dataPackage02(newArgs.eventData);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage02).get();

					return;
				}

				if (!SongAPI::isThereAnySongs(guild.id)) {
					std::string msgString = "------\n**There's no more songs for us to skip to!**\n------";
					EmbedData msgEmbed02;
					msgEmbed02.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed02.setColor(discordGuild.data.borderColor);
					msgEmbed02.setTimeStamp(getTimeAndDate());
					msgEmbed02.setDescription(msgString);
					msgEmbed02.setTitle("__**Song Queue Issue:**__");
					RespondToInputEventData dataPackage02(newArgs.eventData);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed02);
					InputEvents::respondToInputEventAsync(dataPackage02).get();

					return;
				} else {
					if (SongAPI::areWeCurrentlyPlaying(guild.id) && SongAPI::isThereAnySongs(guild.id)) {
						std::string msgString = "------\n**We're skipping to the next song!**\n------";
						EmbedData msgEmbed02{};
						msgEmbed02.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl())
							.setColor(discordGuild.data.borderColor)
							.setTimeStamp(getTimeAndDate())
							.setDescription(msgString)
							.setTitle("__**Song Skip:**__");
						RespondToInputEventData dataPackage02(newArgs.eventData);
						dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbed02);
						auto newEvent02 = InputEvents::respondToInputEventAsync(dataPackage02).get();
						SongAPI::skip(guildMember);
						savePlaylist(discordGuild);
					} else if (!SongAPI::isThereAnySongs(guild.id)) {
						EmbedData newEmbed{};
						newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
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
						RespondToInputEventData dataPackage02(newArgs.eventData);
						dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(newEmbed);
						InputEvents::respondToInputEventAsync(dataPackage02).get();
						return;
					} else {
						std::string msgString = "------\n**There's no music playing to be skipped!**\n------";
						EmbedData msgEmbed02;
						msgEmbed02.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						msgEmbed02.setColor(discordGuild.data.borderColor);
						msgEmbed02.setTimeStamp(getTimeAndDate());
						msgEmbed02.setDescription(msgString);
						msgEmbed02.setTitle("__**Skipping Issue:**__");
						RespondToInputEventData dataPackage02(newArgs.eventData);
						dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbed02);
						InputEvents::respondToInputEventAsync(dataPackage02).get();
						return;
					}
				}
				return;
			} catch (...) {
				reportException("Skip::execute()");
			}
		}
		~Skip(){};
	};
	UnorderedMap<uint64_t, int64_t> Skip::timeOfLastSkip{};
}
```