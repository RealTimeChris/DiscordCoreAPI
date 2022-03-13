### **Unpinning a Message:**
---
- Access within the `DiscordCoreAPI` namespace, the `Messages` class.
- Populate a data structure of type `DiscordCoreAPI::UnpinMessageData`.
- Select, from the `Messages` class, the `unpinMessageAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

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
				UnpinMessageData dataPackage{};
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.channelId = args->eventData.getChannelId();
				dataPackage.messageId = "909216949471428659";

				Messages::unpinMessageAsync(dataPackage).get();

				
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
