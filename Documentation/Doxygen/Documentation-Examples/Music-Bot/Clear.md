Clear {#Clear}
============
```cpp

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Clear : public BaseFunction {
	  public:
		Clear() {
			commandName = "clear";
			helpDescription = "Clears the current song queue.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /clear! And that's it!\n------");
			msgEmbed.setTitle("__**Clear Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Clear>();
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

				auto playlist = SongAPI::getPlaylist(guild.id);
				playlist.songQueue.clear();
				SongAPI::setPlaylist(playlist, guild.id);
				savePlaylist(discordGuild);

				EmbedData msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**You have cleared the song queue!**__\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Queue Cleared:**__");
				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (...) {
				reportException("Clear::execute()");
			}
		}
		~Clear(){};
	};

}
```