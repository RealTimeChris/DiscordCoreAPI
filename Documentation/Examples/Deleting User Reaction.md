
### **Deleting User Reaction:**
---
- Access the `Reactions` class of the `DiscordCoreAPI` namespace.
- Select, from the `Reactions` class, the `deleteUserReactionAsync()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::DeleteUserReactionData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

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

		Test* create() {
			return new Test;
		}

		virtual CoRoutine<void> executeAsync(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {

				DeleteUserReactionData dataPackage{};
				dataPackage.channelId = args->eventData.getChannelId();
				dataPackage.messageId = args->eventData.getMessageId();
				dataPackage.emojiName = "😆";
				dataPackage.userId = args->eventData.getAuthorId();

				Reactions::deleteUserReactionAsync(dataPackage).get();

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
