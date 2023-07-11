Disconnect {#Disconnect}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Disconnect : public BaseFunction {
	  public:
		Disconnect() {
			commandName = "disconnect";
			helpDescription = "Disconnect the bot from voice chat.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /disconnect!\n------");
			msgEmbed.setTitle("__**Disconnect Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Disconnect>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				GuildData guild = Guilds::getCachedGuild({ newArgs.eventData.getGuildId() }).get();
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
				if (guild.areWeConnected()) {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**I'm disconnecting from the voice channel!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Disconnected:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
					guild.disconnect();
					savePlaylist(discordGuild);
				} else {
					EmbedData newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**I was already disconnected!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**Disconnected:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				}

				return;
			} catch (...) {
				reportException("Disconnect::execute()");
			}
		}
		~Disconnect(){};
	};

}
```