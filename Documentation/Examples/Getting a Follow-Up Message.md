
### **Getting a Follow-Up Message:**
---
- Access the `Interactions` class of the `DiscordCoreAPI` namespace.
- Select, from the `Interactions` class, the `getFollowUpMessageAsync()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::GetFollowUpMessageData`.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "IndexInitial.hpp"

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
				GetFollowUpMessageData dataPackage{};
				dataPackage.applicationId = getBotUser().id;
				dataPackage.interactionToken = args->eventData.getInteractionToken();
				dataPackage.interactionToken = args->eventData.getMessageId();
				auto responseData = Interactions::getFollowUpMessageAsync(dataPackage);
				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync() Error: ");
			}
		}
	};
}
#endif
```
