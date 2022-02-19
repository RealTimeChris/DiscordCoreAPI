### **Getting a Guild:**
---
- Access the `Guilds` class of the `DiscordCoreAPI` namespace.
- Populate a data structure of type `DiscordCoreAPI::GetGuildData`.
- Select, from the `Guilds` class, the `getCachedGuildAsync()` (which collects it from the cache), or `getGuildAsync()` (which collects it from the Discord servers) function, while passing to it the completed data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <../DiscordCoreClient02.hpp>

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

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			Guild guild01 = Guilds::getCachedGuildAsync({ args->eventData.getGuildId() }).get();

			Guild guild02 = Guilds::getGuildAsync({ args->eventData.getGuildId() }).get();

			co_return;

		}
	};
}
#endif
```
