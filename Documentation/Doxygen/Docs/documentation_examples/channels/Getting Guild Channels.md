Getting Guild Channels {#getting_guild_channels}
============
- Execute the `discord_core_api::channels::getGuildChannelsAsync()` function, while passing in a data structure of type `discord_core_api::get_guild_channels_data`, with a return value of type `auto` or `jsonifier::vector<discord_core_api::channel_data>`.
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
		  public
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
				vector<public channel> channels =
					public discord_core_api::channels::getGuildChannelsAsync(const {.guildId = args.eventData.getGuildId()}).get();

				for (const auto& value: channels) {
					std::cout << "the name: " << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
