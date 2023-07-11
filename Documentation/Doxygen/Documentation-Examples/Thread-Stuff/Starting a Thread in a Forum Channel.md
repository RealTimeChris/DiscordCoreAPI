Starting a Thread in a Forum Channel {#startingathreadinaforumchannel}
============
- Execute the, `Threads::startThreadInForumChannelAsync()` function, while passing in a value of type `StartThreadInForumChannelData`, with a return value of type `auto` or `Thread`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& newArgs) {
			try {
				
				StartThreadInForumChannelData dataPackage;
				dataPackage.channelId = newArgs.eventData.getChannelId();
				dataPackage.message.content = "TESTING";
				dataPackage.name = "THE TEST THREAD";
				Threads::startThreadInForumChannelAsync(dataPackage).get();
				
				return;
			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test(){};
	};
}
```
