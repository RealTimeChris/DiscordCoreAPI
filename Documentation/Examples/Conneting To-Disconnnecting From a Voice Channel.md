
### **Connecting To/Disconnecting From a Voice Channel:**
---
- Collect/create an instance of `DiscordCoreAPI::Guild`, and also collect the Id of a voice channel to which you would like to connect. (This can be collected from the `VoiceData` member of a GuildMember object.)
- Call, from the Guild object the `connectToVoice` function, while passing in the channel id of the channel to which you would like to connect.
- To disconnect from the channel, call the `disconnect` function on the `Guild` object.
```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(unique_ptr<BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData);

			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = args->eventData.getAuthorId(),.guildId = args->eventData.getGuildId() }).get();

			Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();

			auto voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

			guild.disconnect();

			co_return;

		}
	};
}
#endif
```
