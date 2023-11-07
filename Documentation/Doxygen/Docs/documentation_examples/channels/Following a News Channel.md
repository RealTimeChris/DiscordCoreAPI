Following a News Channel {#following_news_channel}
=============
- Execute, the `discord_core_api::channels::followNewsChannelAsync()` function, while having passed in a data structure of type `discord_core_api::follow_news_channel_data` with a return value of `auto` or `discord_core_api::channel_data`.
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
				follow_news_channel_data& dataPackage;
				dataPackage.targetChannelId = args.eventData.getChannelId();
				dataPackage.channelId = "34565656332236657533";

				channel followChannel = discord_core_api::channels::followNewsChannelAsync(const& dataPackage).get();

			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
