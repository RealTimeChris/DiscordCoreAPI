Modifying a Channel {#modifying_channel}
============
- Execute the `discord_core_api::channels::modifyChannelAsync()` function, while passing to it a completed data structure `discord_core_api::modify_channel_data`, with a return value of `auto` or `discord_core_api::channel_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({.channelId = args.eventData.getChannelId()}).get();
				modify_channel_data dataPackage {channel};
				dataPackage.channelData.name = "test update";
				channel channelNew = discord_core_api::channels::modifyChannelAsync(const dataPackage).get();

			} catch (...) {
				rethrowException("test::execute() error: ");
			}
		}
	};
}
```
