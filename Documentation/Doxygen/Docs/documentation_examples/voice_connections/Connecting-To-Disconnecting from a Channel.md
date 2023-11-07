Connecting To/Disconnecting From a Voice Channel {#connecting_and_disconnecting}
============
- collect/create an instance of `discord_core_api::guild_data`, and also collect the id of a voice channel to which you would like to connect. (this can be collected from the `discord_core_api::voice_state_data_light` object, which can be collected from `discord_core_api::guild_members::getVoiceStateData`)
- call, the `discord_core_api::guild_data::connectToVoice()` function from the `discord_core_api::guild_data` object, while passing in the channel id of the channel to which you would like to connect.
- to disconnect from the channel, call the `discord_core_api::guild_data::disconnect()` function on the `discord_core_api::guild_data` object.
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

			guild_member_data guildMember =
				guild_members::getGuildMemberAsync(const {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
					.get();

			guild_data guild_data = guilds::getGuildAsync(const {.guildId = args.eventData.getGuildId()}).get();

			auto voiceConnection = guild_data.connectToVoice(guildMember.voiceData.channelId);

			guild_data.disconnect();
		}
	};
}
```