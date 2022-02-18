
### **Deleting an Interaction Response:**
---
- Access the `InputEvents` class of the `DiscordCoreAPI` namespace.
- Select, from the `InputEvents` class, the `deleteInputEventResponseAsync()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::InputEventData` with a type set to `InteractionResponse`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <../DiscordCoreClient02.hpp>

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

		Test* create() {
			return new Test;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			RespondToInputEventData dataPackage02{ args->eventData };
			dataPackage02.type = DesiredInputEventResponseType::InteractionResponse;
			dataPackage02.addContent("Test Response");
			auto inputEventData = InputEvents::respondToEvent(dataPackage02);

			InputEvents::deleteInputEventResponseAsync(inputEventData).get();

			co_return;
		}
	};
}
#endif
```
