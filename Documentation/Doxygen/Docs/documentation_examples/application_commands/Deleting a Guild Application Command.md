Deleting a Guild Application Command {#deleteguildcommand}
=============
- Execute the `application_commands::deleteGuildApplicationCommandAsync()` function, while passing in a data structure of type `delete_guild_application_command_data`, with no return value.
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
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			input_events::deleteInputEventResponseAsync(const args.eventData);

			delete_guild_application_command_data dataPackage01;
			dataPackage01.name = "botinfo";
			dataPackage01.guildId = args.eventData.getGuildId();

			application_commands::deleteGuildApplicationCommandAsync(const dataPackage01).get();
		}
	};
}
```