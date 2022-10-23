Bulk Overwriting Global Application Commands {#bulkoverwriteglobalcommands}
============
- Execute the `DiscordCoreAPI::ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync()` function, while passing in a data structure of type `DiscordCoreAPI::BulkOverwriteGlobalApplicationCommandsData`, with a return value of `auto` or `std::vector<DiscordCoreAPI::ApplicationCommand>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			vector<DiscordCoreAPI::CreateGlobalApplicationCommandData> newVector;
			DiscordCoreAPI::CreateGlobalApplicationCommandData createSellDrugsCommandData;
			createSellDrugsCommandData.description = "Sell drugs in exchange for some currency!";
			createSellDrugsCommandData.name = "selldrugs";
			createSellDrugsCommandData.type = DiscordCoreAPI::ApplicationCommandType::Chat_Input;
			newVector.emplace_back(createSellDrugsCommandData);

			DiscordCoreAPI::CreateGlobalApplicationCommandData registerSlashCommandsCommandData;
			registerSlashCommandsCommandData.description = "Register the programmatically designated slash commands.";
			registerSlashCommandsCommandData.name = "registerslashcommands";
			registerSlashCommandsCommandData.type = ApplicationCommandType::Chat_Input;
			newVector.emplace_back(registerSlashCommandsCommandData);

			DiscordCoreAPI::BulkOverwriteGlobalApplicationCommandsData dataPackage;
			dataPackage.data = newVector;

			auto returnValue = DiscordCoreAPI::ApplicationCommands::bulkOverwriteGlobalApplicationCommandsAsync(dataPackage).get();

			for (auto value: returnValue) {
				cout << "Command Name: " << value.name << endl;
			}
		}
	};
}
```