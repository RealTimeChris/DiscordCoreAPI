### **Removing a User from a Thread:**
---
- Access within the `DiscordCoreAPI` namespace, the `Channels` class.
- Populate a data structure of type `DiscordCoreAPI::RemoveThreadMemberData`.
- Select, from the `Channels` class, the `removeThreadMemberAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
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
				RemoveThreadMemberData dataPackage{};
				dataPackage.channelId = "909407162776834069";
				dataPackage.userId = args->eventData.getAuthorId();

				Channels::removeThreadMemberAsync(dataPackage).get();
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
