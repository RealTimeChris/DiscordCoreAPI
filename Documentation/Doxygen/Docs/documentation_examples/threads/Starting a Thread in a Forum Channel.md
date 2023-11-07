Starting a Thread in a Forum Channel {#starting_a_thread_in_a_forum_channel}
============
- Execute the, `discord_core_api::threads::startThreadInForumChannelAsync()` function, while passing in a value of type `discord_core_api::start_thread_in_forum_channel_data`, with a return value of type `auto` or `discord_core_api::thread`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp- header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& newArgs) {
			try {
				
				start_thread_in_forum_channel_data dataPackage;
				dataPackage.channelId = newArgs.eventData.getChannelId();
				dataPackage.message.content = "testing";
				dataPackage.name = "the test thread";
				threads::startThreadInForumChannelAsync(const dataPackage).get();
				
				return;
			} catch (...) {
				reportException("test::execute()");
			}
		}
		virtual ~test(){};
	};
}
```
