
### **Collecting Modal Input:**
---
- After creating a modal-text-input, create an object of the `DiscordCoreAPI::ModalCollector` class, passing into its constructor the `InputEventData` that resulted from the call to `InputEvents::respondToEvent`, when the modal was created.
- Call, from the `DiscordCoreAPI::ModalCollector` class, the `collectModalData` function.
- Collect a result of type `ModalResponseData` and deal with the modal response as you see fit!
```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

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
			NewThreadAwaitable<void>();
			try {

				RespondToInputEventData dataPackage{ args->eventData };
				dataPackage.addModal("Test Modal", "test_modal", "Test Modal Small", "test_modal", true, 1, 46, TextInputStyle::Paragraph, "TEST MODAL", "TestModal");
				dataPackage.type = InputEventResponseType::Interaction_Response;
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				ModalCollector modalCollector{ newEvent };
				auto modalReturnData = modalCollector.collectModalData(120000).get();
				RespondToInputEventData dataPackage03{ modalReturnData };
				dataPackage03.addContent(modalReturnData.value);
				dataPackage03.type = InputEventResponseType::Interaction_Response;
				InputEvents::respondToEvent(dataPackage03);
				
				return;
			}
			catch (...) {
				reportException("Test::executeAsync Error: ");
			}
		}
		virtual ~Test() {};
	};
}

```
