
### **Getting a Guild:**
---
- Dereference your `DiscordCoreAPI::DiscordCoreClient` pointer, and select the `guilds` member.
- Select, from the `guilds` pointer, the `getGuildASync()` (which collects it from the cache), or `fetchAsync()` (which collects it from the Discord servers) function, while passing to it either a completed data structure (`DiscordCoreAPI::GetGuildData`, or `DiscordCoreAPI::FetchGuildData`), or simply `{.guildId = GUILDIDHERE}`.
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

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();

			Guild guild = args->eventData.discordCoreClient->guilds->fetchAsync({ .guildId = args->eventData.getGuildId() }).get();

			co_return;

		}
	};
}
#endif
```
