Getting Joined Private Archived Threads {#gettingjoinedprivatearchivedthreads}
============
- Execute the, `Threads::getJoinedPrivateArchivedThreadsAsync()` function, while passing in a value of type `GetJoinedPrivateArchivedThreadsData`, with a return value of type `auto` or `ArchivedThreadsData`.
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
				GetJoinedPrivateArchivedThreadsData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.limit = 2;

				auto newActiveThreads = Threads::getJoinedPrivateArchivedThreadsAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
