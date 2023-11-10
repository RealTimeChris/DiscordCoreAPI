NP(Now-Playing) {#NP}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class np : public base_function {
	  public:
		np() {
			commandName = "np";
			helpDescription = "displays the currently playing song.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /np!\n------");
			msgEmbed.setTitle("__**now playing usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<np>();
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
				loadPlaylist(discordGuild);
				embed_data newEmbed{};
				newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				newEmbed.setDescription("__**title:**__ [" + song_api::getCurrentSong(guild_data.id).songTitle + "](" + song_api::getCurrentSong(guild_data.id).viewUrl + ")" +
					"\n__**description:**__ " + song_api::getCurrentSong(guild_data.id).description + "\n__**duration:**__ " + song_api::getCurrentSong(guild_data.id).duration +
					"\n__**added by:**__ <@!" + jsonifier::toString(song_api::getCurrentSong(guild_data.id).addedByUserId) + "> (" + song_api::getCurrentSong(guild_data.id).addedByUserName + ")");
				newEmbed.setImage(song_api::getCurrentSong(guild_data.id).thumbnailUrl);
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**now playing:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				if (song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
					newEmbed.setFooter("✅ loop-all, ✅ loop-song");
				}
				if (!song_api::isLoopAllEnabled(guild_data.id) && song_api::isLoopSongEnabled(guild_data.id)) {
					newEmbed.setFooter("❌ loop-all, ✅ loop-song");
				}
				if (song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
					newEmbed.setFooter("✅ loop-all, ❌ loop-song");
				}
				if (!song_api::isLoopAllEnabled(guild_data.id) && !song_api::isLoopSongEnabled(guild_data.id)) {
					newEmbed.setFooter("❌ loop-all, ❌ loop-song");
				}
				respond_to_input_event_data& dataPackage(newArgs.eventData);
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addMessageEmbed(newEmbed);
				auto newEvent02 = input_events::respondToInputEventAsync(const dataPackage).get();


				return;
			} catch (...) {
				reportException("np:execute()");
			}
		}
		~np(){};
	};

}
```