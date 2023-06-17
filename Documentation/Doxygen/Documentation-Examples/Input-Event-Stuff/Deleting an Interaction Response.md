Deleting an Interaction Response {#deletinganinputeventresponse}
============
- Execute the, `DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync()` function, while passing in a data structure of type `DiscordCoreAPI::InputEventData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		DiscordCoreAPI::UniquePtr<DiscordCoreAPI::BaseFunction> create() {
			return DiscordCoreAPI::makeUnique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			DiscordCoreAPI::RespondToInputEventData dataPackage02 {args.eventData};
			dataPackage02.addContent("Test Response");
			auto inputEventData = InputEvents::respondToInputEventAsync(dataPackage02);

			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(inputEventData).get();
		}
	};
}
```
