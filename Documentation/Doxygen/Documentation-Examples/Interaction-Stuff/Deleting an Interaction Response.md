Deleting an Interaction Response {#deletinganinteractionresponse}
============
- Execute the, `InputEvents::deleteInputEventResponseAsync()` function, while passing in a data structure of type `InputEventData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
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
			InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			RespondToInputEventData dataPackage02 {args.eventData};
			dataPackage02.type = InputEventResponseType::Interaction_Response;
			dataPackage02.addContent("Test Response");
			auto inputEventData = InputEvents::respondToInputEventAsync(dataPackage02);

			InputEvents::deleteInputEventResponseAsync(inputEventData).get();
		}
	};
}
```
