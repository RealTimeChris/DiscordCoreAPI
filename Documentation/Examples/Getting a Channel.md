### **Getting a Channel:**
---
- Access the `Channels` class of the `DiscordCoreAPI` namespace.
- Populate a data structure of type `DiscordCoreAPI::GetChannelData`.
- Select, from the `Channels` class, the `getCachedChannelAsync()` (which collects it from the cache), or `getChannelAsync()` (which collects it from the Discord servers) function, while passing to it the completed data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.


```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <Index>

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

			Channel channel = Channels::getCachedChannelAsync({ args->eventData.getChannelId() }).get();

			Channel channel = Channels::getChannelAsync({ args->eventData.getChannelId() }).get();

			return;

		}
	};
}
#endif
```
