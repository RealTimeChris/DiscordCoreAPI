Creating a Guild Application Command {#createguildcommand}
============
- Execute the `ApplicationCommands::createGuildApplicationCommandAsync()` function, while passing in a data structure of type `CreateGuildApplicationCommandData` (IMPORTANT #1: Notes on which kind of types to set can be found [here](https://discord.com/developers/docs/interactions/application-commands#subcommands-and-subcommand-groups).), with a return value of type `auto` or `ApplicationCommand`.
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
			InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			CreateGuildApplicationCommandData dataPackage;
			dataPackage.description = "Displays info about the current bot.";
			dataPackage.name = "botinfo";
			dataPackage.type = ApplicationCommandType::Chat_Input;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnValue = ApplicationCommands::createGuildApplicationCommandAsync(dataPackage).get();

			std::cout << returnValue.name << std::endl;
		}
	};
}
```