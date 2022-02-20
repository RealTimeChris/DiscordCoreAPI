### **Modifying a Channel:**
---
- Access the `Channels` class of the `DiscordCoreAPI` namespace.
- Select, from the `Channels` class, the `modifyChannelAsync()` function, while passing to it a completed data structure `DiscordCoreAPI::ModifyChannelData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void executeAsync(unique_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = args->eventData.getChannelId() }).get();
				ModifyChannelData dataPackage{ channel };
				dataPackage.channelData.name = "TEST UPDATE";
				Channel channelNew = Channels::modifyChannelAsync(dataPackage).get();
				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync() Error: ");
			}
		}
	};
}
#endif
```
