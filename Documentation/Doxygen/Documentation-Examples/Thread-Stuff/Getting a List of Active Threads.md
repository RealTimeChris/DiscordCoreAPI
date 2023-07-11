Getting a List of Active Threads {#gettingalistofactivethreads}
============
- Execute the, `Threads::getActiveThreadsAsync()` function, while passing in a value of type `GetActiveThreadsData`, with a return value of type `auto` or `ActiveThreadsData`.
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
			EmbedData msgEmbed { };
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
				GetActiveThreadsData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();

				auto newActiveThreads = Threads::getActiveThreadsAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
