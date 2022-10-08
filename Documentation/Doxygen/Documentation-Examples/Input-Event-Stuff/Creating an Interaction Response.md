Creating an Interaction Response {#creatinganinputeventresponse}
============ 
- Execute the, from the `DiscordCoreAPI::InputEvents::respondToInputEventAsync()` function, while passing in a data structure of type `DiscordCoreAPI::RespondToInputEventData` with a type set to either `DiscordCoreAPI::InputEventResponseType::Deferred_Response`, `DiscordCoreAPI::InputEventResponseType::Ephemeral_Deferred_Response`, `DiscordCoreAPI::InputEventResponseType::Interaction_Response`, `DiscordCoreAPI::InputEventResponseType::Follow_Up_Message`, `DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response`, or `DiscordCoreAPI::InputEventResponseType::Ephemeral_Follow_Up_Message`, with a return value of type `auto` or `DiscordCoreAPI::InputEventData`.

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

		UniquePtr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			DiscordCoreAPI::RespondToInputEventData dataPackage {args.eventData};
			dataPackage.type = DiscordCoreAPI::InputEventResponseType::Deferred_Response;
			InputEvents::respondToInputEventAsync(dataPackage);

			DiscordCoreAPI::RespondToInputEventData dataPackage01 {args.eventData};
			dataPackage01.type = DiscordCoreAPI::InputEventResponseType::Interaction_Response;
			dataPackage01.addContent("Test Response");
			InputEvents::respondToInputEventAsync(dataPackage01);

			DiscordCoreAPI::RespondToInputEventData dataPackage02 {args.eventData};
			dataPackage02.type = DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response;
			dataPackage02.addContent("Test Response");
			InputEvents::respondToInputEventAsync(dataPackage02);
		}
	};
}
```
