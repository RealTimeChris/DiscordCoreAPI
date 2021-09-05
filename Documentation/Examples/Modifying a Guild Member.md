### **Modifying a Guild Member:**
---
- Access the `GuildMembers` class of the `DiscordCoreAPI` namespace.
- Populate a `DiscordCoreAPI::ModifyGuildMemberData` structure, to be passed in as an argument.
- Select, from the `GuildMembers` class, the `modifyGuildMemberAsync()` function.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

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

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			ModifyGuildMemberData dataPackage01;
			dataPackage01.currentChannelId = "";
			dataPackage01.deaf = false;
			dataPackage01.mute = false;
			dataPackage01.newVoiceChannelId = "";
			dataPackage01.nick = "TEST NICK";
			dataPackage01.roleIds = vector<string>{ "" };
			dataPackage01.guildId = args->eventData.getGuildId();
			dataPackage01.guildMemberId = args->eventData.getAuthorId();

			GuildMember guildMember01 = GuildMembers::modifyGuildMemberAsync(dataPackage01).get();

			co_return;
		}
	};
}
#endif
```
