Getting Public Archived Threads {#gettingpublicarchivedthreads}
============
- Execute the, `Threads::getPublicArchivedThreadsAsync()` function, while passing in a value of type `GetPublicArchivedThreadsData`, with a return value of type `auto` or `ArchivedThreadsData`.
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
				GetPublicArchivedThreadsData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.limit = 1;

				auto newActiveThreads = Threads::getPublicArchivedThreadsAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
