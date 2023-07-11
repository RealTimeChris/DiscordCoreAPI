Bulk Overwriting Guild Application Commands {#bulkoverwriteguildcommands}
============
- Execute the `ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync()` function and execute it, while passing in a data structure of type `BulkOverwriteGuildApplicationCommandsData`, with a return value of `auto` or `std::vector<ApplicationCommand>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			vector<CreateGuildApplicationCommandData> newVector;
			CreateGuildApplicationCommandData createSellDrugsCommandData;
			createSellDrugsCommandData.description = "Sell drugs in exchange for some currency!";
			createSellDrugsCommandData.name = "selldrugs";
			createSellDrugsCommandData.type = ApplicationCommandType::Chat_Input;
			newVector.emplace_back(createSellDrugsCommandData);

			CreateGuildApplicationCommandData registerSlashCommandsCommandData;
			registerSlashCommandsCommandData.description = "Register the programmatically designated slash commands.";
			registerSlashCommandsCommandData.name = "registerslashcommands";
			registerSlashCommandsCommandData.type = ApplicationCommandType::Chat_Input;
			newVector.emplace_back(registerSlashCommandsCommandData);

			BulkOverwriteGuildApplicationCommandsData dataPackage;
			dataPackage.responseData = newVector;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnValue = ApplicationCommands::bulkOverwriteGuildApplicationCommandsAsync(dataPackage).get();

			for (auto value: returnValue) {
				std::cout << "Command Name: " << value.name << std::endl;
			}
		}
	};
}
```