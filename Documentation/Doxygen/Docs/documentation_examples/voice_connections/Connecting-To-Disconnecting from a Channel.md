Connecting To/Disconnecting From a Voice Channel {#connectingtodisconnecting}
============
- collect/create an instance of `guild`, and also collect the id of a voice channel to which you would like to connect. (this can be collected from the `guild_member::voiceData` object.)
- call, the `guild::connectToVoice()` function from the `guild` object, while passing in the channel id of the channel to which you would like to connect.
- to disconnect from the channel, call the `guild::disconnect()` function on the `guild` object.
```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "index.hpp"

	namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			input_events::deleteInputEventResponseAsync(const args.eventData);

			guild_member guildMember =
				guild_members::getGuildMemberAsync(const {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
					.get();

			guild guild = guilds::getGuildAsync(const {.guildId = args.eventData.getGuildId()}).get();

			auto voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

			guild.disconnect();
		}
	};
}
```