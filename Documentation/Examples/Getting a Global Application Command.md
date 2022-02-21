
### **Getting a Global Application Command:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `getGlobalApplicationCommandAsync()` function and execute it, while passing in a data structure of type `GetGlobalApplicationCommandData`, with a return value of type `auto` or `ApplicationCommand`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
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

			auto globalApplicationCommands = ApplicationCommands::getGlobalApplicationCommandsAsync().get();

			DiscordCoreAPI::GetGlobalApplicationCommandData dataPackage;
			dataPackage.commandId = globalApplicationCommands.at(0).data.id;

			auto globalApplicationCommand = ApplicationCommands::getGlobalApplicationCommandAsync(dataPackage).get();

			cout << globalApplicationCommand.data.name << endl;

			

		}
	};
}
```
