Loop Song {#Loop-Song}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {
	class loop_song : public base_function {
	  public:
		loop_song() {
			commandName = "loop_song";
			helpDescription = "enables or disables looping of the current song.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /loopsong!\n------");
			msgEmbed.setTitle("__**loop-song usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<loop_song>();
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

				if (song_api::isLoopSongEnabled(guild_data.id)) {
					song_api::setLoopSongStatus(false, guild_data.id);
				} else {
					song_api::setLoopSongStatus(true, guild_data.id);
				}
				embed_data msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				if (song_api::isLoopSongEnabled(guild_data.id)) {
					msgEmbed.setDescription("\n------\n__**looping-song has been enabled!**__\n------\n");
				} else {
					msgEmbed.setDescription("\n------\n__**looping-song has been disabled!**__\n------\n");
				}
				savePlaylist(discordGuild);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**looping-song change:**__");
				respond_to_input_event_data& dataPackage(newArgs.eventData);
				dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();

				return;
			} catch (...) {
				reportException("loop_song::execute()");
			}
		}
		~loop_song(){};
	};

}
```