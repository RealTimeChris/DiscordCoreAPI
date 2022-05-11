NP(Now-Playing) {#NP}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {

	class Np : public DiscordCoreAPI::BaseFunction {
	  public:
		Np() {
			this->commandName = "np";
			this->helpDescription = "Displays the currently playing song.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /np!\n------");
			msgEmbed.setTitle("__**Now Playing Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Np>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({.channelId = args.eventData->getChannelId()}).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({.guildId = args.eventData->getGuildId()}).get();

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
				loadPlaylist(discordGuild);
				EmbedData newEmbed;
				newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				newEmbed.setDescription("__**Title:**__ [" + SongAPI::getCurrentSong(guild.id).songTitle + "](" + SongAPI::getCurrentSong(guild.id).viewUrl +
										")" + "\n__**Description:**__ " + SongAPI::getCurrentSong(guild.id).description + "\n__**Duration:**__ " +
										SongAPI::getCurrentSong(guild.id).duration + "\n__**Added By:**__ <@!" +
										SongAPI::getCurrentSong(guild.id).addedByUserId + "> (" + SongAPI::getCurrentSong(guild.id).addedByUserName + ")");
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
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(newEmbed);
				auto newEvent02 = InputEvents::respondToInputEventAsync(dataPackage);


				return;
			} catch (...) {
				reportException("NP:execute Error: ");
			}
		}
		virtual ~Np();
	};
}
```