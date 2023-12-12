Bulk Overwriting Global Application Commands {#bulk_overwrite_global_commands}
============
- Execute the `discord_core_api::application_commands::bulkOverwriteGlobalApplicationCommandsAsync()` function, while passing in a data structure of type `discord_core_api::bulk_overwrite_global_application_commands_data`, with a return value of `auto` or `jsonifier::vector<discord_core_api::application_command_data>`.
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
			vector<create_global_application_command_data> newVector;
			create_global_application_command_data createSellDrugsCommandData;
			createSellDrugsCommandData.description = "sell drugs in exchange for some currency!";
			createSellDrugsCommandData.name = "selldrugs";
			createSellDrugsCommandData.type = application_command_type::Chat_Input;
			newVector.emplace_back(createSellDrugsCommandData);

			create_global_application_command_data registerSlashCommandsCommandData;
			registerSlashCommandsCommandData.description = "register the programmatically designated slash commands.";
			registerSlashCommandsCommandData.name = "registerslashcommands";
			registerSlashCommandsCommandData.type = application_command_type::Chat_Input;
			newVector.emplace_back(registerSlashCommandsCommandData);

			bulk_overwrite_global_application_commands_data& dataPackage;
			dataPackage.data = newVector;

			auto returnValue = application_commands::bulkOverwriteGlobalApplicationCommandsAsync(dataPackage).get();

			for (const auto& value: returnValue) {
				std::cout << "command name: " << value.name << std::endl;
			}
		}
	};
}
```