Instantiating/Creating an Autocomplete {#instantiatingcreatingautocomplete}
============ 
- create a data structure of type `create_global_application_command_data` or `create_guild_application_command_data`, and be sure to set one or more of the `application_command_option_data`'s settings for `autocomplete` to true.
- use this data structure to create the application command.

```cpp
// Test.hpp- header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& newArgs) {
			try {
				
				create_global_application_command_data createTestData;
				createTestData.dmPermission = true;
				createTestData.applicationId = discord_core_client::getInstance()->getBotUser().id;
				createTestData.type = application_command_type::Chat_Input;
				createTestData.name = "test";
				createTestData.description = "test command.";
				application_command_option_data testOptionOne;
				testOptionOne.type = application_command_option_type::attachment;
				testOptionOne.name = "attachment";
				testOptionOne.required = false;
				testOptionOne.description = "test attachment!";
				createTestData.options.emplace_back(testOptionOne);
				application_command_option_data testOptionTwo;
				testOptionTwo.type = application_command_option_type::jsonifier::string;
				testOptionTwo.name = "test_string";
				testOptionTwo.required = false;
				testOptionTwo.autocomplete = true;
				testOptionTwo.description = "test string!";
				createTestData.options.emplace_back(testOptionTwo);
				application_commands::createGlobalApplicationCommandAsync(const createTestData).get();

			} catch (...) {
				reportException("test::execute()");
			}
		}
		virtual ~test(){};
	};
}
```
