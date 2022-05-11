Triggering the Typing Indicator {#triggeringthetypingindicator}
============
- Execute the, `DiscordCoreAPI::Channels::triggerTypingIndicatorAsync()` function, while passing in a data structure of type `DiscordCoreAPI::TriggerTypingIndicatorData` with a return value of `void`.
- Call the function with `.get()` added to the end in order to wait for the results now.

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
				DiscordCoreAPI::TriggerTypingIndicatorData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();

				DiscordCoreAPI::Channels::triggerTypingIndicatorAsync(dataPackage).get();

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
