Stop {#Stop}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class stop : public base_function {
	  public:
		stop() {
			commandName = "stop";
			helpDescription = "stops the current song from playing.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /stop!\n------");
			msgEmbed.setTitle("__**stop usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<stop>();
		}
		void execute(base_function_arguments& newArgs) {
			try {
				channel channel = channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				guild guild = guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get();
				discord_guild discordGuild(guild);

				bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);

				if (!checkIfAllowedInChannel) {
					return;
				}

				guild_member guildMember =
					guild_members::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				input_event_data newEvent = newArgs.eventData;

				int64_t currentTime = std::chrono::duration_cast<milliseconds>(hrclock::now().time_since_epoch()).count();
				int64_t previousPlayedTime{};
				if (play::timeOfLastPlay.contains(newArgs.eventData.getGuildId())) {
					previousPlayedTime = play::timeOfLastPlay.at(newArgs.eventData.getGuildId());
				}

				if (currentTime - previousPlayedTime < 5000) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but please wait a total of 5 seconds after playing, before attempting to stop!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**timing issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					return;
				}
				voice_connection* voiceConnection{};
				voice_state_data voiceStateData{};
				if (guild.voiceStates.contains(guildMember.id)) {
					voiceStateData = guild.voiceStates.at(guildMember.id);
					voiceConnection = guild.connectToVoice(0, voiceStateData.channelId, true, false);
				} else {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**connection issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
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
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					return;
				}

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**stopping issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					return;
				}

				if (!guild.areWeConnected() || !song_api::areWeCurrentlyPlaying(guild.id)) {
					jsonifier::string msgString = "------\n**there's no music playing to be stopped!**\n------";
					embed_data msgEmbed;
					msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**stopping issue:**__");
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
					return;
				}

				song_api::stop(guild.id);
				savePlaylist(discordGuild);
				embed_data msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**songs remaining in queue:**__ " + jsonifier::toString(song_api::getPlaylist(guild.id).songQueue.size()) + "\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**stopping playback:**__");
				respond_to_input_event_data dataPackage02(newArgs.eventData);
				dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				dataPackage02.addMessageEmbed(msgEmbed);
				input_events::respondToInputEventAsync(const dataPackage02).get();
				return;
			} catch (...) {
				reportException("stop::execute()");
			}
		}
		~stop(){};
	};

}
```