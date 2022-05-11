Instantiating/Creating a Button {#instantiatingcreatingbutton}
============ 
- Create a data structure of type `DiscordCoreAPI::RespondToInputEventData`, and add either some content or a message embed.- Add one or more buttons by using the `DiscordCoreAPI::RespondToInputEventData::addButton` of the `DiscordCoreAPI::RespondToInputEventData` structure.
- Call the `DiscordCoreAPI::InputEvents::respondToInputEventAsync()` function, passing it the `DiscordCoreAPI::RespondToInputEventData` structure.
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
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			DiscordCoreAPI::RespondToInputEventData dataPackage {args.eventData};
			dataPackage.addButton(false, "test_button", "Test Button", "✅", DiscordCoreAPI::ButtonStyle::Danger);
			dataPackage.addContent("Test Response");
			dataPackage.addMessageEmbed(EmbedData {.description = "TESTING!", .title = "Test Title"});
			dataPackage.type = DiscordCoreAPI::InputEventResponseType::Interaction_Response;
			auto inputEventData = DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage);
		}
	};
}
```
