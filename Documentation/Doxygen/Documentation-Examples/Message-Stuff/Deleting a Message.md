Deleting a Message {#deletingamessage}
============
- Execute the, from the `Messages::deleteMessageAsync()` function, while passing in a data structure of type `DeleteMessageData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

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
				DeleteMessageData dataPackage;
				dataPackage.timeDelay = 500;
				dataPackage.messageData = args.eventData.getMessageData();

				Messages::deleteMessageAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
