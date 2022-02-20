
### **Crossposting a Message:**
---
- Access the `Messages` class of the `DiscordCoreAPI` namespace.
- Select, from the `Messages` class, the `crosspostMessageAsync()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::CrosspostMessageData`, with a return value of type `auto`, or, `Message`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

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

				CrosspostMessageData dataPackage{};
				dataPackage.channelId = args->eventData.getChannelId();
				dataPackage.messageId = args->eventData.getMessageId();

				Message responseMessages = Messages::crosspostMessageAsync(dataPackage).get();

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
