Collecting Modal Input {#collectingmodalinput}
=============
- After creating a modal-text-input, create an object of the `DiscordCoreAPI::ModalCollector` class, passing into its constructor the `DiscordCoreAPI::InputEventData` that resulted from the call to `DiscordCoreAPI::InputEvents::respondToInputEventAsync`, when the modal was created.- Call the `DiscordCoreAPI::ModalCollector::collectModalData` function, from the instance of ModalCollector.
- Collect a result of type `DiscordCoreAPI::ModalResponseData` and deal with the modal response as you see fit!
```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

	namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::RespondToInputEventData dataPackage {args.eventData};
				dataPackage.addModal("Test Modal", "test_modal", "Test Modal Small", "test_modal", true, 1, 46, TextInputStyle::Paragraph, "TEST MODAL",
									 "TestModal");
				dataPackage.type = DiscordCoreAPI::InputEventResponseType::Interaction_Response;
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				DiscordCoreAPI::ModalCollector modalCollector {newEvent};
				auto modalReturnData = modalCollector.collectModalData(120000).get();
				DiscordCoreAPI::RespondToInputEventData dataPackage03 {modalReturnData};
				dataPackage03.addContent(modalReturnData.value);
				dataPackage03.type = DiscordCoreAPI::InputEventResponseType::Interaction_Response;
				DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage03);


			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test() noexcept = default;
	};
}
```
