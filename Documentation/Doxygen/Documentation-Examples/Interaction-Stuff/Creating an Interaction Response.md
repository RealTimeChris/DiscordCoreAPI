Creating an Interaction Response {#creatinganinteractionresponse}
============
- Execute the, `InputEvents::respondToInputEventAsync()` function, while passing in a data structure of type `RespondToInputEventData` with a type set to either `InputEventResponseType::Deferred_Response`, `InputEventResponseType::Interaction_Response`, or `InputEventResponseType::Ephemeral_Interaction_Response`, with a return value of type `auto` or `UniquePtr<InputEventData>`.
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

			RespondToInputEventData dataPackage {args.eventData};
			dataPackage.type = InputEventResponseType::Deferred_Response;
			InputEvents::respondToInputEventAsync(dataPackage);

			RespondToInputEventData dataPackage01 {args.eventData};
			dataPackage01.type = InputEventResponseType::Interaction_Response;
			dataPackage01.addContent("Test Response");
			InputEvents::respondToInputEventAsync(dataPackage01);

			RespondToInputEventData dataPackage02 {args.eventData};
			dataPackage02.type = InputEventResponseType::Ephemeral_Interaction_Response;
			dataPackage02.addContent("Test Response");
			InputEvents::respondToInputEventAsync(dataPackage02);
		}
	};
}
```
