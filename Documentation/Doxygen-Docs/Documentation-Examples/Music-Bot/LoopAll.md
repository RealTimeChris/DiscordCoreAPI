Loop All {#Loop-All}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {
	class LoopAll : public DiscordCoreAPI::BaseFunction {
	  public:
		LoopAll() {
			this->commandName = "loopall";
			this->helpDescription = "Enables or disables looping of the current queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /loopall!\n------");
			msgEmbed.setTitle("__**Loop-All Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<LoopAll>();
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

				bool areWeAllowed = checkIfAllowedPlayingInChannel(*args.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({.guildMemberId = args.eventData->getAuthorId(), .guildId = args.eventData->getGuildId()}).get();

				bool doWeHaveControl = checkIfWeHaveControl(*args.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				if (SongAPI::isLoopAllEnabled(guild.id)) {
					SongAPI::setLoopAllStatus(false, guild.id);
				} else {
					SongAPI::setLoopAllStatus(true, guild.id);
				}
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				if (SongAPI::isLoopAllEnabled(guild.id)) {
					msgEmbed.setDescription("\n------\n__**Looping-All has been enabled!**__\n------\n");
				} else {
					msgEmbed.setDescription("\n------\n__**Looping-All has been disabled!**__\n------\n");
				}
				savePlaylist(discordGuild);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Looping-All Change:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);

				return;
			} catch (...) {
				reportException("LoopAll::execute Error: ");
			}
		}
		virtual ~LoopAll();
	};
}
```