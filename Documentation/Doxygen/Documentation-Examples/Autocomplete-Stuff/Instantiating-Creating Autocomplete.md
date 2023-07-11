Instantiating/Creating an Autocomplete {#instantiatingcreatingautocomplete}
============ 
- Create a data structure of type `CreateGlobalApplicationCommandData` or `CreateGuildApplicationCommandData`, and be sure to set one or more of the `ApplicationCommandOptionData`'s settings for `autocomplete` to true.
- Use this data structure to create the application command.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& newArgs) {
			try {
				
				CreateGlobalApplicationCommandData createTestData;
				createTestData.dmPermission = true;
				createTestData.applicationId = newArgs.discordCoreClient->getBotUser().id;
				createTestData.type = ApplicationCommandType::Chat_Input;
				createTestData.name = "test";
				createTestData.description = "Test command.";
				ApplicationCommandOptionData testOptionOne;
				testOptionOne.type = ApplicationCommandOptionType::Attachment;
				testOptionOne.name = "attachment";
				testOptionOne.required = false;
				testOptionOne.description = "Test attachment!";
				createTestData.options.emplace_back(testOptionOne);
				ApplicationCommandOptionData testOptionTwo;
				testOptionTwo.type = ApplicationCommandOptionType::std::string;
				testOptionTwo.name = "test_string";
				testOptionTwo.required = false;
				testOptionTwo.autocomplete = true;
				testOptionTwo.description = "Test string!";
				createTestData.options.emplace_back(testOptionTwo);
				ApplicationCommands::createGlobalApplicationCommandAsync(createTestData).get();

			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test(){};
	};
}
```
