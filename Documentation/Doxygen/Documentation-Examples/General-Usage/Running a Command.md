Running a Command {#runningcommand}
============
- By default, once you have added a command, it will be executed upon receiving the associated registered slash command's input. 
- Given that, assuming that the "Test" command has been registered with the library, the following code would produce, in response to an individual entering "/test"(assuming it has been registered as a slash command), an ephemeral response message that states "Test Message!".
```cpp
/// Test.hpp - Header for the "test" command.
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
				RespondToInputEventData dataPackage {args.eventData};
				dataPackage.addContent("Test Message!");
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response)
				InputEvents::respondToInputEventAsync(dataPackage);
		}
	};
}
```
