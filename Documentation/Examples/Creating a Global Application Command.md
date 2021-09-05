
### **Creating a Global Application Command:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `createGlobalApplicationCommandAsync()` function and execute it, while passing in a data structure of type `CreateApplicationCommandData` (IMPORTANT #1: Notes on which kind of types to set can be found [here](https://discord.com/developers/docs/interactions/application-commands#subcommands-and-subcommand-groups).), with a return value of type `auto` or `ApplicationCommand`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

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

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {

			CreateApplicationCommandData createBotInfoCommandData;
			createBotInfoCommandData.defaultPermission = true;
			createBotInfoCommandData.description = "Displays info about the current bot.";
			createBotInfoCommandData.name = "botinfo";
			createBotInfoCommandData.type = ApplicationCommandType::CHAT_INPUT;
			
			auto globalApplicationCommands = ApplicationCommands::createGlobalApplicationCommandAsync(createBotInfoCommandData).get();

			cout << globalApplicationCommands.data.name << endl;

			co_return;

		}
	};
}
#endif
```
