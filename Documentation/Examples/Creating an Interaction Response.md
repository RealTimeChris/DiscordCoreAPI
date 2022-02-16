
### **Creating an Interaction Response:**
---
- Access the `InputEvents` class of the `DiscordCoreAPI` namespace.
- Select, from the `InputEvents` class, the `respondToEvent()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::RespondToInputEventData` with a type set to either `DeferredResponse`, `InteractionResponse`, or `EphemeralInteractionResponse`, with a return value of type `auto` or `DiscordCoreAPI::InputEventData`.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

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

			RespondToInputEventData dataPackage{ args->eventData };
			dataPackage.type = DesiredInputEventResponseType::DeferredResponse;
			InputEvents::respondToEvent(dataPackage);

			RespondToInputEventData dataPackage01{ args->eventData };
			dataPackage01.type = DesiredInputEventResponseType::InteractionResponse;
			dataPackage01.addContent("Test Response");
			InputEvents::respondToEvent(dataPackage01);

			RespondToInputEventData dataPackage02{ args->eventData };
			dataPackage02.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
			dataPackage02.addContent("Test Response");
			InputEvents::respondToEvent(dataPackage02);

			co_return;
		}
	};
}
#endif
```
