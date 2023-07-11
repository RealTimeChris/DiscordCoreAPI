Removing a User from a Thread {#removingauserfromathread}
============
- Execute the, `Threads::removeThreadMemberAsync()` function, while passing in a value of type `RemoveThreadMemberData`, with a return value of type `void`.
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
				RemoveThreadMemberData dataPackage;
				dataPackage.channelId = "909407162776834069";
				dataPackage.userId = args.eventData.getAuthorId();

				Threads::removeThreadMemberAsync(dataPackage).get();

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
