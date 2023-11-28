Deleting a Guild Integration {#deleting_a_guild_integration}
============
- Execute the, `discord_core_api::guilds::deleteGuildIntegrationAsync()` function, while passing in a value of type `discord_core_api::delete_guild_integration_data`, with a return value of type `void`.
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
				delete_guild_integration_data& dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.integrationId = "INTEGRATION_ID_HERE";
				dataPackage01.reason = "testing purposes!";

				guilds::deleteGuildIntegrationAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
