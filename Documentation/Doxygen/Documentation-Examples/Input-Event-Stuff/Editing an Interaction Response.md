Editing an Interaction Response {#editinganinputeventresponse}
============
- Execute the, `InputEvents::respondToInputEventAsync()` function, while passing in a data structure of type `RespondToInputEventData` with a type set	to `InputEventResponseType::Edit_Interaction_Response`, or `InputEventResponseType::Edit_Follow_Up_Message` with a return value of type `auto` or `InputEventData`.

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
			dataPackage02.type = InputEventResponseType::Edit_Interaction_Response;
			dataPackage02.addContent("Test Response");
			auto inputEventData = InputEvents::respondToInputEventAsync(dataPackage02);

			RespondToInputEventData dataPackage03 {inputEventData};
			dataPackage03.type = InputEventResponseType::Edit_Follow_Up_Message;
			dataPackage03.addContent("Test Response, Edited!");
			InputEvents::respondToInputEventAsync(dataPackage03);
		}
	};
}
```
