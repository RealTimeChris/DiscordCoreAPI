Connecting To/Disconnecting From a Voice Channel {#connectingtodisconnecting}
============
- Collect/create an instance of `Guild`, and also collect the Id of a voice channel to which you would like to connect. (This can be collected from the `GuildMember::voiceData` object.)
- Call, the `Guild::connectToVoice()` function from the `Guild` object, while passing in the channel id of the channel to which you would like to connect.
- To disconnect from the channel, call the `Guild::disconnect()` function on the `Guild` object.
```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

	namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			InputEvents::deleteInputEventResponseAsync(args.eventData);

			GuildMember guildMember =
				GuildMembers::getGuildMemberAsync({.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
					.get();

			Guild guild = Guilds::getGuildAsync({.guildId = args.eventData.getGuildId()}).get();

			auto voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

			guild.disconnect();
		}
	};
}
```