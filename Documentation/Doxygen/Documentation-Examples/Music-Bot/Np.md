NP(Now-Playing) {#NP}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Np : public BaseFunction {
	  public:
		Np() {
			commandName = "np";
			helpDescription = "Displays the currently playing song.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /np!\n------");
			msgEmbed.setTitle("__**Now Playing Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Np>();
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
				loadPlaylist(discordGuild);
				EmbedData newEmbed{};
				newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewUrl + ")" +
					"\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " + SongAPI::getCurrentSong(guild.id).duration +
					"\n__**Added By:**__ <@!" + std::to_string(SongAPI::getCurrentSong(guild.id).addedByUserId) + "> (" + SongAPI::getCurrentSong(guild.id).addedByUserName + ")");
				newEmbed.setImage(SongAPI::getCurrentSong(guild.id).thumbnailUrl);
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**Now Playing:**__");
				newEmbed.setColor(discordGuild.data.borderColor);
				if (SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
					newEmbed.setFooter("✅ Loop-All, ✅ Loop-Song");
				}
				if (!SongAPI::isLoopAllEnabled(guild.id) && SongAPI::isLoopSongEnabled(guild.id)) {
					newEmbed.setFooter("❌ Loop-All, ✅ Loop-Song");
				}
				if (SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
					newEmbed.setFooter("✅ Loop-All, ❌ Loop-Song");
				}
				if (!SongAPI::isLoopAllEnabled(guild.id) && !SongAPI::isLoopSongEnabled(guild.id)) {
					newEmbed.setFooter("❌ Loop-All, ❌ Loop-Song");
				}
				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(newEmbed);
				auto newEvent02 = InputEvents::respondToInputEventAsync(dataPackage).get();


				return;
			} catch (...) {
				reportException("NP:execute()");
			}
		}
		~Np(){};
	};

}
```