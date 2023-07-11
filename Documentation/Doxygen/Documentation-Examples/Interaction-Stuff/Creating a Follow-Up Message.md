Creating a Follow-Up Message {#creatingafollowupmessage}
============
- Execute the, `InputEvents::respondToInputEventAsync()` function, while passing in a data structure of type `RespondToInputEventData` with a type set	to `InputEventResponseType::Follow_Up_Message`, with a return value of type `auto` or `UniquePtr<InputEventData>`.

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
				dataPackage.addContent("THIS IS A TEST RESPONSE MESSAGE!");
				auto responseData = InputEvents::respondToInputEventAsync(dataPackage);

			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
