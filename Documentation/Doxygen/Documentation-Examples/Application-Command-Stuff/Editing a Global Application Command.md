Editing a Global Application Command {#editglobalcommand}
============
- Execute the `DiscordCoreAPI::ApplicationCommands::editGlobalApplicationCommandAsync()` function, while passing in a data structure of type `DiscordCoreAPI::EditGlobalApplicationCommandData`, with a return value of type `auto` or `DiscordCoreAPI::ApplicationCommand`.
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
			DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage;
			dataPackage.description = "Displays info about the current bot.";
			dataPackage.name = "botinfo";

			auto globalApplicationCommand = DiscordCoreAPI::ApplicationCommands::editGlobalApplicationCommandAsync(dataPackage).get();

			cout << globalApplicationCommand.data.name << endl;
		}
	};
}

```