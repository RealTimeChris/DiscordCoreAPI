Pause {#Pause}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class pause : public base_function {
	  public:
		pause() {
			commandName = "pause";
			helpDescription = "pauses the currently playing music.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /pause!\n------");
			msgEmbed.setTitle("__**pause usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<pause>();
		}

		void execute(base_function_arguments& newArgs) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				guild_data guild_data = guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get();
				discord_guild discordGuild(guild_data);
				bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);
				if (!checkIfAllowedInChannel) {
					return;
				}

				guild_member_data guildMember =
					guild_members::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();
				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);
				if (!doWeHaveControl) {
					return;
				}

				respond_to_input_event_data& dataPackage00(newArgs.eventData);
				dataPackage00.setResponseType(input_event_response_type::Ephemeral_Deferred_Response);
				input_events::respondToInputEventAsync(dataPackage00).get();
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
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				loadPlaylist(discordGuild);
				if (voiceConnection == nullptr || !guild_data.areWeConnected()) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**connection issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (voiceStateData.channelId == 0 || voiceStateData.channelId != voiceConnection->getChannelId()) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**pausing issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (!song_api::areWeCurrentlyPlaying(guild_data.id)) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**sorry, but i need to be either playing or paused for this command to be possible!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**pausing issue:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				song_api::pauseToggle(guild_data.id);

				embed_data msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**songs remaining in queue:**__ " + jsonifier::toString(song_api::getPlaylist(guild_data.id).songQueue.size()) + "\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**paused playback:**__");
				respond_to_input_event_data& dataPackage(newArgs.eventData);
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (...) {
				reportException("pause::execute()");
			}
		}
		~pause(){};
	};

}
```