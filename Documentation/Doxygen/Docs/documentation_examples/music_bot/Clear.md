Clear {#Clear}
============
```cpp

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class clear : public base_function {
	  public:
		clear() {
			commandName = "clear";
			helpDescription = "clears the current song queue.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /clear! and that's it!\n------");
			msgEmbed.setTitle("__**clear usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<clear>();
		}

		void execute(base_function_arguments& newArgs) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({ .channelId = newArgs.eventData.getChannelId() }).get();

				guild_data guild_data = guilds::getCachedGuild({ .guildId = newArgs.eventData.getGuildId() }).get();

				discord_guild discordGuild(guild_data);

				bool checkIfAllowedInChannel = checkIfAllowedPlayingInChannel(newArgs.eventData, discordGuild);

				if (!checkIfAllowedInChannel) {
					return;
				}

				guild_member_data guildMember =
					guild_members::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();

				bool doWeHaveControl = checkIfWeHaveControl(newArgs.eventData, discordGuild, guildMember);

				if (!doWeHaveControl) {
					return;
				}

				auto playlist = song_api::getPlaylist(guild_data.id);
				playlist.songQueue.clear();
				song_api::setPlaylist(playlist, guild_data.id);
				savePlaylist(discordGuild);

				embed_data msgEmbed;
				msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("\n------\n__**you have cleared the song queue!**__\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**queue cleared:**__");
				respond_to_input_event_data& dataPackage(newArgs.eventData);
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();

				return;
			} catch (...) {
				reportException("clear::execute()");
			}
		}
		~clear(){};
	};

}
```