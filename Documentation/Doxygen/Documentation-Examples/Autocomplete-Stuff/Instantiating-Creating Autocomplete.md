Instantiating/Creating an Autocomplete {#instantiatingcreatingautocomplete}
============ 
- Create a data structure of type `DiscordCoreAPI::CreateGlobalApplicationCommandData` or `DiscordCoreAPI::CreateGuildApplicationCommandData`, and be sure to set one or more of the `DiscordCoreAPI::ApplicationCommandOptionData`'s settings for `autocomplete` to true.
- Use this data structure to create the application command.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& newArgs) {
			try {
				
				DiscordCoreAPI::CreateGlobalApplicationCommandData createTestData;
				createTestData.dmPermission = true;
				createTestData.applicationId = newArgs.discordCoreClient->getBotUser().id;
				createTestData.type = DiscordCoreAPI::ApplicationCommandType::Chat_Input;
				createTestData.name = "test";
				createTestData.description = "Test command.";
				DiscordCoreAPI::ApplicationCommandOptionData testOptionOne;
				testOptionOne.type = DiscordCoreAPI::ApplicationCommandOptionType::Attachment;
				testOptionOne.name = "attachment";
				testOptionOne.required = false;
				testOptionOne.description = "Test attachment!";
				createTestData.options.emplace_back(testOptionOne);
				DiscordCoreAPI::ApplicationCommandOptionData testOptionTwo;
				testOptionTwo.type = DiscordCoreAPI::ApplicationCommandOptionType::String;
				testOptionTwo.name = "test_string";
				testOptionTwo.required = false;
				testOptionTwo.autocomplete = true;
				testOptionTwo.description = "Test string!";
				createTestData.options.emplace_back(testOptionTwo);
				DiscordCoreAPI::ApplicationCommands::createGlobalApplicationCommandAsync(createTestData).get();

			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test(){};
	};
}
```
