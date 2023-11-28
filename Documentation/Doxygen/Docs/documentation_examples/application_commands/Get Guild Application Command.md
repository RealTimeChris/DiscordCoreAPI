Getting a Guild Application Command {#get_guild_command}
============
- Execute the `discord_core_api::application_commands::getGuildApplicationCommandAsync()` function, while passing in a data structure of type `discord_core_api::get_guild_application_command_data`, with a return value of type `auto` or `discord_core_api::application_command_data`.
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
			input_events::deleteInputEventResponseAsync(args.eventData).get();

			get_guild_application_commands_data& dataPackage;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnVector = application_commands::getGuildApplicationCommandsAsync(dataPackage).get();

			for (const auto& value: returnVector) {
				std::cout << value.name << std::endl;
			}

			get_guild_application_command_data& dataPackage02;
			dataPackage02.commandId = returnVector[0].id;
			dataPackage02.guildId = args.eventData.getGuildId();

			auto returnValue = application_commands::getGuildApplicationCommandAsync(dataPackage02).get();

			std::cout << returnValue.name << std::endl;
		}
	};
}
```