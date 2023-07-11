Collecting Modal Input {#collectingmodalinput}
=============
- After creating a modal-text-input, create an object of the `ModalCollector` class, passing into its constructor the `InputEventData` that resulted from the call to `InputEvents::respondToInputEventAsync`, when the modal was created.- Call the `ModalCollector::collectModalData` function, from the instance of ModalCollector.
- Collect a result of type `ModalResponseData` and deal with the modal response as you see fit!
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
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				ModalCollector modalCollector {newEvent};
				auto modalReturnData = modalCollector.collectModalData(120000).get();
				RespondToInputEventData dataPackage03 {modalReturnData};
				dataPackage03.addContent(modalReturnData.value);
				dataPackage03.type = InputEventResponseType::Interaction_Response;
				InputEvents::respondToInputEventAsync(dataPackage03);


			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test() noexcept = default;
	};
}
```
