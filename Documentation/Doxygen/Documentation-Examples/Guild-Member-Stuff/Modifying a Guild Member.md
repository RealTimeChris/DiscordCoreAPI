Modifying a Guild Member {#modifyingaguildmember}
============
- Execute the, from the `GuildMembers::modifyGuildMemberAsync()` function, while passing in a value of type `ModifyGuildMemberData`, with a return value of type `GuildMember`.
- Call the function with `.get()` added to the end in order to wait for the results now.

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
			ModifyGuildMemberData dataPackage01;
			dataPackage01.currentChannelId = "";
			dataPackage01.deaf = false;
			dataPackage01.mute = false;
			dataPackage01.newVoiceChannelId = "";
			dataPackage01.nick = "TEST NICK";
			dataPackage01.roleIds = vector<string> {""};
			dataPackage01.guildId = args.eventData.getGuildId();
			dataPackage01.guildMemberId = args.eventData.getAuthorId();

			GuildMember guildMember01 = GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
		}
	};
}
```
