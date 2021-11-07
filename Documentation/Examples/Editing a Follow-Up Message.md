
### **Editing a Follow-Up Message:**
---
- Access the `InputEvents` class of the `DiscordCoreAPI` namespace.
- Select, from the `InputEvents` class, the `respondToEvent()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::RespondToInputEventData` with a type set to `FollowUpMessageEdit`, with a return value of type `auto` or `DiscordCoreAPI::InputEventData`.

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
				RespondToInputEventData dataPackage{ args->eventData };
				dataPackage.type = InputEventResponseType::FollowUpMessageEdit;
				dataPackage.addContent("THIS IS A TEST RESPONSE MESSAGE!");
				auto responseData = InputEvents::respondToEvent(dataPackage);
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
