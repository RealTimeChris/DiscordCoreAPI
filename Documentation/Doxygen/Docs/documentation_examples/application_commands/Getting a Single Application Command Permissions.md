Getting a Single Guild Application Command's Permissions {#get_single_command_permissions}
============
- Execute the `discord_core_api::application_commands::getApplicationCommandPermissionsAsync()` function, while passing in an argument of type `discord_core_api::get_application_command_permissions_data`, with a return value of type `auto` or `discord_core_api::guild_application_command_permission_data`.
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

			get_guild_application_command_permissions_data& dataPackage02;
			dataPackage02.guildId = args.eventData.getGuildId();
			dataPackage02.applicationId = discord_core_client::getInstance()->getBotUser().id;

			auto returnValue = application_commands::getApplicationCommandPermissionsAsync(dataPackage02).get();

			std::cout << returnValue.applicationId << std::endl;
		}
	};
}
```