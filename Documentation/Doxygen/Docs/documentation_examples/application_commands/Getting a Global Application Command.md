Getting a Global Application Command {#get_global_command}
============
- Execute the `discord_core_api::application_commands::getGlobalApplicationCommandAsync()` function, while passing in a data structure of type `discord_core_api::get_global_application_command_data`, with a return value of type `auto` or `discord_core_api::application_command_data`.
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
			auto globalApplicationCommands = application_commands::getGlobalApplicationCommandsAsync().get();

			get_global_application_command_data dataPackage;
			dataPackage.commandId = globalApplicationCommands.at(0).data.id;

			auto globalApplicationCommand = application_commands::getGlobalApplicationCommandAsync(const dataPackage).get();

			std::cout << globalApplicationCommand.data.name << std::endl;
		}
	};
}
```