Clear {#Clear}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {

	class Clear : public DiscordCoreAPI::BaseFunction {
	  public:
		Clear() {
			this->commandName = "clear";
			this->helpDescription = "Clears the current song queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /clear! And that's it!\n------");
			msgEmbed.setTitle("__**Clear Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Clear>();
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

				auto playlist = SongAPI::getPlaylist(guild.id);
				playlist.songQueue.clear();
				SongAPI::setPlaylist(playlist, guild.id);
				savePlaylist(guild);

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**You have cleared the song queue!**__\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Queue Cleared:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (...) {
				reportException("Clear::execute Error: ");
			}
		}
		virtual ~Clear();
	};
}
```