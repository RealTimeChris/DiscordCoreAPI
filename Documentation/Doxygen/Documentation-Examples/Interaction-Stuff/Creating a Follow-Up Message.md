Creating a Follow-Up Message {#creatingafollowupmessage}
============
- Execute the, `DiscordCoreAPI::InputEvents::respondToInputEventAsync()` function, while passing in a data structure of type `DiscordCoreAPI::RespondToInputEventData` with a type set	to `DiscordCoreAPI::InputEventResponseType::Follow_Up_Message`, with a return value of type `auto` or `std::unique_ptr<DiscordCoreAPI::InputEventData>`.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

										  namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::RespondToInputEventData dataPackage {args.eventData};
				dataPackage.type = DiscordCoreAPI::InputEventResponseType::Follow_Up_Message;
				dataPackage.addContent("THIS IS A TEST RESPONSE MESSAGE!");
				auto responseData = DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage);

			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
