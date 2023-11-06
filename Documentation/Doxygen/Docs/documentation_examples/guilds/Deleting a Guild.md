Deleting a Guild {#deletingaguild}
============
- Execute the, `guilds::deleteGuildAsync()` function, while passing in a value of type `delete_guild_data`, with a return value of type `void`.
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
				vector<guild> guilds = guilds::getAllGuildsAsync().get();
				for (const auto& value: guilds) {
					if (value.name == "newer test guild two!") {
						delete_guild_data dataPackage;
						dataPackage.guildId = value.id;
						guilds::deleteGuildAsync(const& dataPackage).get();
					};
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
