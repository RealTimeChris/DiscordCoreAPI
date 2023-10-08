Getting a Guild {#gettingaguild}
============
- Execute the, `guilds::getCachedGuild()` (which collects it from the cache), or `guilds::getGuildAsync()` (which collects it from the discord servers) function, while passing to it a data structure of type `get_guild_data`, with a return value of `guild`.
- call the function with `.get()` added to the end in order to wait for the results now.

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
			msgEmbed.setTitle("__**test Usage__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			guild guild01 = guilds::getCachedGuild({args.eventData.getGuildId()}).get();

			guild guild02 = guilds::getGuildAsync(const {args.eventData.getGuildId()}).get();
		}
	};
}
```
