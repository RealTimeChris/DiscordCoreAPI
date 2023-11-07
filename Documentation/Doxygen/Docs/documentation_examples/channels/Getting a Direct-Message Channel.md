Getting a Direct-Message Channel {#getting_dm_channel}
============
- Execute the `discord_core_api::channels::createDMChannelAsync()` (which collects it from the discord servers)function, while passing to it a data structure of type `discord_core_api::create_dmchannel_data`, or simply `discord_core_api::{ .userId = targetuseridhere}`, with a return value of `auto` or `discord_core_api:channel`.
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

		virtual void execute(std::unique_pt<barse_function_arguments> args) {
			channel channel = discord_core_api::channels::createDMChannelAsync(const {.userId = args.eventData.getAuthorId()}).get();
		}
	};
}
```
