### **Getting a Guild Member:**
---
- Dereference your `DiscordCoreAPI::DiscordCoreClient` pointer, and select the `guildMembers` member.
- Select, from the `guildMembers` pointer, the `getGuildMemberAsync()` (which collects it from the cache), or `fetchAsync()` (which collects it from the Discord servers) function, while passing to it either a completed data structure (`DiscordCoreAPI::GetGuildMemberData`, or `DiscordCoreAPI::FetchGuildMemberData`).
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
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			GetGuildMemberData dataPackage01;
			dataPackage01.guildId = args->eventData.getGuildId();
			dataPackage01.guildMemberId = args->eventData.getAuthorId();

			GuildMember guildMember01 = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync(dataPackage01).get();

			FetchGuildMemberData dataPackage02;
			dataPackage02.guildId = args->eventData.getGuildId();
			dataPackage02.guildMemberId = args->eventData.getAuthorId();
			
			GuildMember guildMember02 = args->eventData.discordCoreClient->guildMembers->fetchAsync(dataPackage02).get();

			co_return;
		}
	};
}
#endif
```
