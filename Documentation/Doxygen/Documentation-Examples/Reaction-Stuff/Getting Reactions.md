Getting Reactions {#gettingreactions}
============
- Execute the, `Reactions::getReactionsAsync()` function, while passing in a data structure of type `GetReactionsData`, with a return value of type `auto` or `vector<User>`.
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
				GetReactionsData dataPackage;
				dataPackage.afterId = args.eventData.getAuthorId();
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.emoji = "😆";
				dataPackage.limit = 50;

				vector<User> usersResponse = Reactions::getReactionsAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
