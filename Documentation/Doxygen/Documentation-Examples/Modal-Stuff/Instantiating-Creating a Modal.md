Instantiating/Creating a Modal-Input {#instantiatingcreatingmodal}
============
- Create a data structure of type `RespondToInputEventData`.
- Add a modal-text-input using the `RespondToInputEventData::addModal` function of the `RespondToInputEventData` structure.
- Call the `InputEvents::respondToInputEventAsync` function, passing it the `RespondToInputEventData` structure.
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
			try {
				RespondToInputEventData dataPackage {args.eventData};
				dataPackage.addModal("Test Modal", "test_modal", "Test Modal Small", "test_modal", true, 1, 46, TextInputStyle::Paragraph, "TEST MODAL",
									 "TestModal");
				dataPackage.type = InputEventResponseType::Interaction_Response;
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);


			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test() = default;
	};
}
```