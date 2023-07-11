Editing a Global Application Command {#editglobalcommand}
============
- Execute the `ApplicationCommands::editGlobalApplicationCommandAsync()` function, while passing in a data structure of type `EditGlobalApplicationCommandData`, with a return value of type `auto` or `ApplicationCommand`.
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
			EditGlobalApplicationCommandData dataPackage;
			dataPackage.description = "Displays info about the current bot.";
			dataPackage.name = "botinfo";

			auto globalApplicationCommand = ApplicationCommands::editGlobalApplicationCommandAsync(dataPackage).get();

			std::cout << globalApplicationCommand.data.name << std::endl;
		}
	};
}

```