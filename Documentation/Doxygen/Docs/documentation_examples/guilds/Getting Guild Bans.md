Getting Guild Bans {#gettingguildbans}
============
- Execute the, `guilds::getGuildBansAsync()` function, while passing in a value of type `get_guild_bans_data`, with a return value of type `auto` or `vector<ban_data>`.
- call the function with `.get()` added to the end in order to wait for the results now.

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
				get_guild_bans_data dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto guildBans = guilds::getGuildBansAsync(const dataPackage01).get();

				for (const auto& value: guildBans) {
					std::cout << "the ban name: " << value.user.userName << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
