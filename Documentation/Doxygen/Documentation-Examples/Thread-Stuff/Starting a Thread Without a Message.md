Starting a Thread without a Message {#startingathreadwithoutamessage}
============
- Execute the, from the `Threads::startThreadWithoutMessageAsync()` function, while passing in a value of type `StartThreadWithoutMessageData`, with a return value of type `auto` or `Thread`.
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
				StartThreadWithoutMessageData dataPackage;
				dataPackage.invitable = true;
				dataPackage.type = ThreadType::GUILD_PUBLIC_THREAD;
				dataPackage.reason = "TESTING REASONS!";
				dataPackage.autoArchiveDuration = ThreadAutoArchiveDuration::SHORT;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.threadName = "NEW THREAD";

				Channel newThread = Threads::startThreadWithoutMessageAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
