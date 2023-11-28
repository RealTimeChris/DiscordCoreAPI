Disconnect {#Disconnect}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class disconnect : public base_function {
	  public:
		disconnect() {
			commandName = "disconnect";
			helpDescription = "disconnect the bot from voice chat.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /disconnect!\n------");
			msgEmbed.setTitle("__**disconnect usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<disconnect>();
		}

		void execute(base_function_arguments& newArgs) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				guild_data guild_data = guilds::getCachedGuild({ newArgs.eventData.getGuildId() }).get();
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
				if (guild_data.areWeConnected()) {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**i'm disconnecting from the voice channel!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**disconnected:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent, 20000);
					guild_data.disconnect();
					savePlaylist(discordGuild);
				} else {
					embed_data newEmbed{};
					newEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					newEmbed.setDescription("------\n__**i was already disconnected!**__\n------");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**disconnected:**__");
					newEmbed.setColor(discordGuild.data.borderColor);
					respond_to_input_event_data& dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(newEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				}

				return;
			} catch (...) {
				reportException("disconnect::execute()");
			}
		}
		~disconnect(){};
	};

}
```