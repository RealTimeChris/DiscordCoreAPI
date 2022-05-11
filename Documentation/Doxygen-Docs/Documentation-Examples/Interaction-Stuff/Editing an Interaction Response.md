Editing an Interaction Response {#editinganinteractionresponse}
============
- Execute the, `DiscordCoreAPI::InputEvents::respondToInputEventAsync()` function, while passing in a data structure of type `DiscordCoreAPI::RespondToInputEventData` with a type set	to `DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response`, with a return value of type `auto` or `DiscordCoreAPI::InputEventData`.

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

			DiscordCoreAPI::RespondToInputEventData dataPackage02 {args.eventData};
			dataPackage02.type = DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response;
			dataPackage02.addContent("Test Response");
			auto inputEventData = DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage02);

			DiscordCoreAPI::RespondToInputEventData dataPackage03 {inputEventData};
			dataPackage03.type = DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response;
			dataPackage03.addContent("Test Response, Edited!");
			DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage03);
		}
	};
}
```
