### **Getting a Channel:**
---
- Dereference your DiscordCoreAPI::DiscordCoreClient pointer, and select the guilds member.
- Select, from the guilds pointer, the getGuildASync() (which collects it from the cache), or fetchAsync() (which collects it from the Discord servers) function, while passing to it either a completed data structure (getGuildData, or fetchGuildData), or simply {.guildId = GUILDIDHERE}.
- Call the function with .get() added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			InputEventManager::deleteInputEventResponseAsync(args->eventData);

			Channel channel = args->eventData.discordCoreClient->channels->fetchAsync({ args->eventData.getChannelId() }).get();

			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			co_return;

		}
	};
}
#endif
```
