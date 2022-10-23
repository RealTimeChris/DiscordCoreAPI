Starting a Thread in a Forum Channel {#startingathreadinaforumchannel}
============
- Execute the, `DiscordCoreAPI::Threads::startThreadInForumChannelAsync()` function, while passing in a value of type `DiscordCoreAPI::StartThreadInForumChannelData`, with a return value of type `auto` or `DiscordCoreAPI::Thread`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& newArgs) {
			try {
				
				DiscordCoreAPI::StartThreadInForumChannelData dataPackage;
				dataPackage.channelId = newArgs.eventData.getChannelId();
				dataPackage.message.content = "TESTING";
				dataPackage.name = "THE TEST THREAD";
				DiscordCoreAPI::Threads::startThreadInForumChannelAsync(dataPackage).get();
				
				return;
			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test(){};
	};
}
```
