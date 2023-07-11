Loop Song {#Loop-Song}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class LoopSong : public BaseFunction {
	  public:
		LoopSong() {
			commandName = "LoopSong";
			helpDescription = "Enables or disables looping of the current song.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /loopsong!\n------");
			msgEmbed.setTitle("__**Loop-Song Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<LoopSong>();
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

				if (SongAPI::isLoopSongEnabled(guild.id)) {
					SongAPI::setLoopSongStatus(false, guild.id);
				} else {
					SongAPI::setLoopSongStatus(true, guild.id);
				}
				EmbedData msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				if (SongAPI::isLoopSongEnabled(guild.id)) {
					msgEmbed.setDescription("\n------\n__**Looping-Song has been enabled!**__\n------\n");
				} else {
					msgEmbed.setDescription("\n------\n__**Looping-Song has been disabled!**__\n------\n");
				}
				savePlaylist(discordGuild);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Looping-Song Change:**__");
				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (...) {
				reportException("LoopSong::execute()");
			}
		}
		~LoopSong(){};
	};

}
```