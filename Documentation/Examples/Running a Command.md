### **Running a Command:**
---
 
- By default, once you have added a command, it will be executed upon either receiving the associated registered slash command's input, or upon a message being entered with an appropriate prefix.
- Given that, assuming that the "Test" command has been registered with the library, the following code would produce, in response to an individual typing "!test" or entering "/test" (assuming it has been registered as a slash command), the input message being deleted as well as a response message that states "Test Message!". 
```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

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

			InputEvents::deleteInputEventResponseAsync(args->eventData);

			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.content = "Test Message!";
				InputEvents::respondToEvent(dataPackage);
			}
			else {
				CreateInteractionResponseData dataPackage(args->eventData);
				dataPackage.data.content = "Test Message!";
				InputEvents::respondToEvent(dataPackage);
			}

			
		}
	};
}
```
