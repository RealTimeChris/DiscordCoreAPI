
### **Bulk Overwriting Global Application Commands:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `bulkOverwriteApplicationCommandsAsync()` function and execute it, while passing in a data structure of type `BulkOverwriteApplicationCommandsData`, with a return value of `auto` or `vector<ApplicationCommand>`.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(unique_ptr<BaseFunctionArguments> args) {

			vector<CreateApplicationCommandData> newVector{};
			CreateApplicationCommandData createSellDrugsCommandData;
			createSellDrugsCommandData.defaultPermission = true;
			createSellDrugsCommandData.description = "Sell drugs in exchange for some currency!";
			createSellDrugsCommandData.name = "selldrugs";
			createSellDrugsCommandData.type = ApplicationCommandType::CHAT_INPUT;
			newVector.push_back(createSellDrugsCommandData);

			CreateApplicationCommandData registerSlashCommandsCommandData;
			registerSlashCommandsCommandData.defaultPermission = true;
			registerSlashCommandsCommandData.description = "Register the programmatically designated slash commands.";
			registerSlashCommandsCommandData.name = "registerslashcommands";
			registerSlashCommandsCommandData.type = ApplicationCommandType::CHAT_INPUT;
			newVector.push_back(registerSlashCommandsCommandData);

			BulkOverwriteApplicationCommandsData dataPackage{};
			dataPackage.data = newVector;

			auto returnValue = ApplicationCommands::bulkOverwriteApplicationCommandsAsync(dataPackage).get();

			for (auto value : returnValue) {
				cout << "Command Name: " << value.name << endl;
			}

			
		}
	};
}
```
