
### **Collecting Button Input:**
---
- After creating a button, create an object of the `DiscordCoreAPI::ButtonCollector` class, passing into its constructor the `InputEventData` that resulted from the call to `InputEvents::respondToEvent`, when the button was created.
- Call, from the `DiscordCoreAPI::ButtonCollector` class, the `collectButtonData` function. NOTE: The arguments for this function are as follows: `getButtonDataForAllNew` = Whether or not it accepts button presses from everyone or just the individual selected with the `targetUser` argument. `maxWaitTimeInMsNew` = The maximum number of milliseconds that the collector will wait for button presses. `maxNumberOfPressesNew` = The maximum number of button presses that the collector will collect. `targetUser` = The target user, if `collectFromAllUsers` is enabled.
- Collect a result of type `vector<ButtonResponseData>` and deal with the button responses as you see fit! Keep in mind that you could set up a voting message by using `getButtonDataForAllNew` and having multiple presses allowed.
- 
```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

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
			dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
			dataPackage.addButton(false, "test_button", "Test Button", "✅", ButtonStyle::Danger);
			dataPackage.addContent("Test Response");
			dataPackage.addMessageEmbed(EmbedData{ .description = "TESTING!",.title = "Test Title" });
			auto inputEventData = InputEvents::respondToEvent(dataPackage);

			ButtonCollector buttonCollector(inputEventData);
			auto results = buttonCollector.collectButtonData(false, 120000, 3, getBotUser().id);

			InputEvents::deleteInputEventResponseAsync(inputEventData).get();

			co_return;
		}
	};
}
#endif
```
