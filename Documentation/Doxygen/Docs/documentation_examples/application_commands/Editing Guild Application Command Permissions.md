Editing a Guild Application Command's Permissions {#edit_guild_command_permissions}
============
- Execute the `discord_core_api::application_commands::editGuildApplicationCommandPermissionsAsync()` function, while passing in a data structure of type `discord_core_api::edit_guild_application_command_permissions_data`, with a return value of type `auto` or `discord_core_api::guild_application_command_permissions_data`.
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

			edit_guild_application_command_permissions_data dataPackage;
			application_command_permission_data& dataPackage02;
			dataPackage02.type = application_command_permission_type::user;
			dataPackage02.permission = true;
			dataPackage02.id = "859853159115259905";
			dataPackage.commandName = "botinfo";
			dataPackage.permissions.emplace_back(dataPackage02);
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnValue = application_commands::editGuildApplicationCommandPermissionsAsync(dataPackage).get();

			std::cout << returnValue.applicationId << std::endl;
		}
	};
}
```