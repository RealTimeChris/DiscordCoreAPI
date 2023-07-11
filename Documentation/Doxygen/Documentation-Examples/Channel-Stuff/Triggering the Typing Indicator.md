Triggering the Typing Indicator {#triggeringthetypingindicator}
============
- Execute the, `Channels::triggerTypingIndicatorAsync()` function, while passing in a data structure of type `TriggerTypingIndicatorData` with a return value of `void`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

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
				TriggerTypingIndicatorData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();

				Channels::triggerTypingIndicatorAsync(dataPackage).get();

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
