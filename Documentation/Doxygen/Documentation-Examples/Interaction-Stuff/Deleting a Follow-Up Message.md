Deleting a Follow-Up Message {#deletingafollowupmessage}
============
- Execute the, `InputEvents::deleteInputEventResponseAsync()` function, while passing in a data structure of type `InputEventData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				RespondToInputEventData dataPackage {args.eventData};
				dataPackage.type = InputEventResponseType::Follow_Up_Message;
				dataPackage.addContent("TEST CONTENT");
				auto result = InputEvents::respondToInputEventAsync(dataPackage);
				auto responseData = InputEvents::deleteInputEventResponseAsync(result);

			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
