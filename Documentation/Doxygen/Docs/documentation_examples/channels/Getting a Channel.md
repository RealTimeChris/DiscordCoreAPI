Getting a Channel {#getting_channel}
============
- Execute the `discord_core_api::channels::getCachedChannel()` (which collects it from the cache), or `discord_core_api::getChannelAsync()` (which collects it from the discord servers) function, while passing to it a data structure of type `discord_core_api::get_channel_data`, with a return type of `auto` or `discord_core_api::channel_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.
```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "index.hpp"

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
			channel channel = discord_core_api::channels::getCachedChannel({args.eventData.getChannelId()}).get();

			channel channel = discord_core_api::channels::getChannelAsync(const {args.eventData.getChannelId()}).get();
		}
	};
}
```
