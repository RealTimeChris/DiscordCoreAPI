Instantiating/Creating a Modal-Input {#instantiatingcreatingmodal}
============
- Create a data structure of type `DiscordCoreAPI::RespondToInputEventData`.
- Add a modal-text-input using the `DiscordCoreAPI::RespondToInputEventData::addModal` function of the `DiscordCoreAPI::RespondToInputEventData` structure.
- Call the `DiscordCoreAPI::InputEvents::respondToInputEventAsync` function, passing it the `DiscordCoreAPI::RespondToInputEventData` structure.
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
				auto newEvent = DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage);


			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test() = default;
	};
}
```