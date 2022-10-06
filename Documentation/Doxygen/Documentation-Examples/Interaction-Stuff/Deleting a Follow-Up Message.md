Deleting a Follow-Up Message {#deletingafollowupmessage}
============
- Execute the, `DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync()` function, while passing in a data structure of type `DiscordCoreAPI::InputEventData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

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

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::RespondToInputEventData dataPackage {args.eventData};
				dataPackage.type = DiscordCoreAPI::InputEventResponseType::Follow_Up_Message;
				dataPackage.addContent("TEST CONTENT");
				auto result = InputEvents::respondToInputEventAsync(dataPackage);
				auto responseData = DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(result);

			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
