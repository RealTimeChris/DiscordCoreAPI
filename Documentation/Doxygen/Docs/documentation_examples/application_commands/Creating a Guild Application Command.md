Creating a Guild Application Command {#create_guild_command}
============
- Execute the `discord_core_api::application_commands::createGuildApplicationCommandAsync()` function, while passing in a data structure of type `discord_core_api::create_guild_application_command_data` (important #1: notes on which kind of types to set can be found [here](https://discord.com/developers/docs/interactions/application-commands#subcommands-and-subcommand-groups).), with a return value of type `auto` or `discord_core_api::application_command_data`.
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

			create_guild_application_command_data& dataPackage;
			dataPackage.description = "displays info about the current bot.";
			dataPackage.name = "botinfo";
			dataPackage.type = application_command_type::Chat_Input;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnValue = application_commands::createGuildApplicationCommandAsync(dataPackage).get();

			std::cout << returnValue.name << std::endl;
		}
	};
}
```