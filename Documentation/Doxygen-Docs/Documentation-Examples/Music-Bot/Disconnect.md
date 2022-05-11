Disconnect {#Disconnect}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {

	class Disconnect : public DiscordCoreAPI::BaseFunction {
	  public:
		Disconnect() {
			this->commandName = "disconnect";
			this->helpDescription = "Disconnect the bot from voice chat.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /disconnect!\n------");
			msgEmbed.setTitle("__**Disconnect Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Disconnect>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({.channelId = args.eventData->getChannelId()}).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({args.eventData->getGuildId()}).get();
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
				if (guild.areWeConnected()) {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**I'm disconnecting from the voice channel!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Disconnected:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					InputEvents::deleteInputEventResponseAsync(std::move(newEvent), 20000);
					SongAPI::stop(guild.id);
					guild.disconnect();
					savePlaylist(discordGuild);
				} else {
					EmbedData newEmbed;
					newEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					newEmbed.setDescription("------\n__**I was already disconnected!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Disconnected:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
				}

				return;
			} catch (...) {
				reportException("Disconnect::execute Error: ");
			}
		}
		virtual ~Disconnect();
	};
}
```