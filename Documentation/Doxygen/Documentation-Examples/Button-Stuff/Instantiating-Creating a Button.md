Instantiating/Creating a Button {#instantiatingcreatingbutton}
============ 
- Create a data structure of type `RespondToInputEventData`, and add either some content or a message embed.- Add one or more buttons by using the `RespondToInputEventData::addButton` of the `RespondToInputEventData` structure.
- Call the `InputEvents::respondToInputEventAsync()` function, passing it the `RespondToInputEventData` structure.
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
			dataPackage.addButton(false, "test_button", "Test Button", "✅", ButtonStyle::Danger);
			dataPackage.addContent("Test Response");
			dataPackage.addMessageEmbed(EmbedData {.description = "TESTING!", .title = "Test Title"});
			dataPackage.type = InputEventResponseType::Interaction_Response;
			auto inputEventData = InputEvents::respondToInputEventAsync(dataPackage);
		}
	};
}
```
