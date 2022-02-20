### **Starting a Thread without a Message:**
---
- Access within the `DiscordCoreAPI` namespace, the `Channels` class.
- Populate a data structure of type `DiscordCoreAPI::StartThreadWithoutMessageData`.
- Select, from the `Channels` class, the `startThreadWithoutMessageAsync()` function and execute it, with a return value of type `auto` or `Channel`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <IndexInitial.hpp>

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
				StartThreadWithoutMessageData dataPackage{};
				dataPackage.invitable = true;
				dataPackage.type = ThreadType::GUILD_PUBLIC_THREAD;
				dataPackage.reason = "TESTING REASONS!";
				dataPackage.autoArchiveDuration = ThreadAutoArchiveDuration::SHORT;
				dataPackage.channelId = args->eventData.getChannelId();
				dataPackage.threadName = "NEW THREAD";

				Channel newThread = Channels::startThreadWithoutMessageAsync(dataPackage).get();

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
