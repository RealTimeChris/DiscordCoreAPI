### **Instantiating/Creating a Modal-Input:**
---
- Create a data structure of type `DiscordCoreAPI::RespondToInputEventData`.
- Add a modal-text-input using the `addModal` function of the `DiscordCoreAPI::RespondToInputEventData` structure.
- Call the `InputEvents::respondToEvent` function, passing it the `DiscordCoreAPI::RespondToInputEventData` structure.
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
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<Test>();
		}

		virtual void execute( std::unique_ptr<BaseFunctionArguments> args) {
			try {

				RespondToInputEventData dataPackage{ args->eventData };
				dataPackage.addModal("Test Modal", "test_modal", "Test Modal Small", "test_modal", true, 1, 46, TextInputStyle::Paragraph, "TEST MODAL", "TestModal");
				dataPackage.type = InputEventResponseType::Interaction_Response;
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				
				return;
			}
			catch (...) {
				reportException("Test::executeAsync Error: ");
			}
		}
		virtual ~Test() = default;
	};
}
```
