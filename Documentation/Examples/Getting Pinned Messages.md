### **Getting Pinned Messages:**
---
- Access within the `DiscordCoreAPI` namespace, the `Messages` class.
- Populate a data structure of type `DiscordCoreAPI::GetPinnedMessagesData`.
- Select, from the `Messages` class, the `getPinnedMessagesAsync()` function and execute it, with a return value of type `auto` or `vector<Message>` while passing it the created data structure.
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

				GetPinnedMessagesData dataPackage{};
				dataPackage.channelId = args->eventData.getChannelId();

				vector<Message> messagesVector = Messages::getPinnedMessagesAsync(dataPackage).get();

				for (auto value : messagesVector) {
					cout << "THE ID: " << value.id << endl;
				}

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
