Skip {#Skip}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {
	class skip : public base_function {
	  public:
		static unordered_map<uint64_t, int64_t> timeOfLastSkip;

		skip() {
			commandName = "skip";
			helpDescription = "skips to the next song in the queue.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /skip.\n------");
			msgEmbed.setTitle("__**skip usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<skip>();
		}

		void execute(base_function_arguments& newArgs) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				guild_data guild_data = guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get();
				discord_guild discordGuild(guild_data);

				bool areWeAllowed = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				guild_member_data guildMember =
					guild_members::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}
				input_event_data newEvent = newArgs.eventData;

				int64_t currentTime = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count();
				int64_t previousSkippedTime{};
				if (skip::timeOfLastSkip.contains(newArgs.eventData.getGuildId())) {
					previousSkippedTime = skip::timeOfLastSkip.at(newArgs.eventData.getGuildId());
				}

				if (currentTime - previousSkippedTime < 5000) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but please wait a total of 5 seconds in between skips!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**timing issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				previousSkippedTime = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count();
				skip::timeOfLastSkip.insert_or_assign(newArgs.eventData.getGuildId(), previousSkippedTime);
				voice_connection* voiceConnection{};
				voice_state_data voiceStateData{};
				if (guild_data.voiceStates.contains(guildMember.id)) {
					voiceStateData = guild_data.voiceStates.at(guildMember.id);
					voiceConnection = guild_data.connectToVoice(0, voiceStateData.channelId, true, false);
				} else {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**connection issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				loadPlaylist(discordGuild);
				if (voiceConnection == nullptr) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**connection issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**skipping issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage02(newArgs.eventData);
					dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(newEmbed);
					input_events::respondToInputEventAsync(dataPackage02).get();

					return;
				}

				if (!guild_data.areWeConnected() || !song_api::areWeCurrentlyPlaying(guild_data.id)) {
					jsonifier::string msgString = "------\n**there's no music playing to be skipped!**\n------";
					embed_data msgEmbed;
					msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**skipping issue:**__");
					respond_to_input_event_data& dataPackage02(newArgs.eventData);
					dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed);
					input_events::respondToInputEventAsync(dataPackage02).get();

					return;
				}

				if (!song_api::isThereAnySongs(guild_data.id)) {
					jsonifier::string msgString = "------\n**there's no more songs for us to skip to!**\n------";
					embed_data msgEmbed02;
					msgEmbed02.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed02.setColor(discordGuild.data.borderColor);
					msgEmbed02.setTimeStamp(getTimeAndDate());
					msgEmbed02.setDescription(msgString);
					msgEmbed02.setTitle("__**song queue issue:**__");
					respond_to_input_event_data& dataPackage02(newArgs.eventData);
					dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed02);
					input_events::respondToInputEventAsync(dataPackage02).get();

					return;
				} else {
					if (song_api::areWeCurrentlyPlaying(guild_data.id) && song_api::isThereAnySongs(guild_data.id)) {
						jsonifier::string msgString = "------\n**we're skipping to the next song!**\n------";
						embed_data msgEmbed02{};
						msgEmbed02.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl())
							.setColor(discordGuild.data.borderColor)
							.setTimeStamp(getTimeAndDate())
							.setDescription(msgString)
							.setTitle("__**song skip:**__");
						respond_to_input_event_data& dataPackage02(newArgs.eventData);
						dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbed02);
						auto newEvent02 = input_events::respondToInputEventAsync(dataPackage02).get();
						song_api::skip(guildMember);
						savePlaylist(discordGuild);
					} else if (!song_api::isThereAnySongs(guild_data.id)) {
						embed_data newEmbed{};
						newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						newEmbed.setDescription("------\n__**sorry, but there's nothing left to play here!**__\n------");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**now playing:**__");
						newEmbed.setColor(discordGuild.data.borderColor);
						if (song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed.setFooter("✅ loop-all, ✅ loop-song");
						} else if (!song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed.setFooter("❌ loop-all, ✅ loop-song");
						} else if (song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed.setFooter("✅ loop-all, ❌ loop-song");
						} else if (!song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
							newEmbed.setFooter("❌ loop-all, ❌ loop-song");
						}
						respond_to_input_event_data& dataPackage02(newArgs.eventData);
						dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(newEmbed);
						input_events::respondToInputEventAsync(dataPackage02).get();
						return;
					} else {
						jsonifier::string msgString = "------\n**there's no music playing to be skipped!**\n------";
						embed_data msgEmbed02;
						msgEmbed02.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						msgEmbed02.setColor(discordGuild.data.borderColor);
						msgEmbed02.setTimeStamp(getTimeAndDate());
						msgEmbed02.setDescription(msgString);
						msgEmbed02.setTitle("__**skipping issue:**__");
						respond_to_input_event_data& dataPackage02(newArgs.eventData);
						dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage02.addMessageEmbed(msgEmbed02);
						input_events::respondToInputEventAsync(dataPackage02).get();
						return;
					}
				}
				return;
			} catch (...) {
				reportException("skip::execute()");
			}
		}
		~skip(){};
	};
	unordered_map<uint64_t, int64_t> skip::timeOfLastSkip{};
}
```