Loop All {#Loop-All}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class LoopAll : public BaseFunction {
	  public:
		LoopAll() {
			commandName = "loopall";
			helpDescription = "Enables or disables looping of the current queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /loopall!\n------");
			msgEmbed.setTitle("__**Loop-All Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<LoopAll>();
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

				if (SongAPI::isLoopAllEnabled(guild.id)) {
					SongAPI::setLoopAllStatus(false, guild.id);
				} else {
					SongAPI::setLoopAllStatus(true, guild.id);
				}
				EmbedData msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				if (SongAPI::isLoopAllEnabled(guild.id)) {
					msgEmbed.setDescription("\n------\n__**Looping-All has been enabled!**__\n------\n");
				} else {
					msgEmbed.setDescription("\n------\n__**Looping-All has been disabled!**__\n------\n");
				}
				savePlaylist(discordGuild);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Looping-All Change:**__");
				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (...) {
				reportException("LoopAll::execute()");
			}
		}
		~LoopAll(){};
	};

}
```