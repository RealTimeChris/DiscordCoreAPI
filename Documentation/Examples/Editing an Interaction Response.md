
### **Editing an Interaction Response:**
---
- Access the `InputEvents` class of the `DiscordCoreAPI` namespace.
- Select, from the `InputEvents` class, the `respondToEvent()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::RespondToInputEventData` with a type set to `InteractionResponseEdit`, with a return value of type `auto` or `DiscordCoreAPI::InputEventData`.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <Index>

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

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(unique_ptr<BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			RespondToInputEventData dataPackage02{ args->eventData };
			dataPackage02.type = InputEventResponseType::EphemeralInteractionResponse;
			dataPackage02.addContent("Test Response");
			auto inputEventData = InputEvents::respondToEvent(dataPackage02);

			RespondToInputEventData dataPackage03{ inputEventData };
			dataPackage03.type = InputEventResponseType::InteractionResponseEdit;
			dataPackage03.addContent("Test Response, Edited!");
			InputEvents::respondToEvent(dataPackage03);

			return;
		}
	};
}
#endif
```
