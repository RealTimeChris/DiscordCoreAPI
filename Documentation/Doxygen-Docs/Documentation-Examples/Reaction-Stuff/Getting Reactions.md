Getting Reactions {#gettingreactions}
============
- Execute the, `DiscordCoreAPI::Reactions::getReactionsAsync()` function, while passing in a data structure of type `DiscordCoreAPI::GetReactionsData`, with a return value of type `auto` or `vector<DiscordCoreAPI::User>`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

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
			DiscordCoreAPI::EmbedData msgEmbed { };
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
				DiscordCoreAPI::GetReactionsData dataPackage;
				dataPackage.afterId = args.eventData.getAuthorId();
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.emoji = "😆";
				dataPackage.limit = 50;

				vector<DiscordCoreAPI::User> usersResponse = DiscordCoreAPI::Reactions::getReactionsAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
