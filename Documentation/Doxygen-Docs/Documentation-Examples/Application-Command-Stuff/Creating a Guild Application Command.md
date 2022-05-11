Creating a Guild Application Command {#createguildcommand}
============
- Execute the `DiscordCoreAPI::ApplicationCommands::createGuildApplicationCommandAsync()` function, while passing in a data structure of type `DiscordCoreAPI::CreateGuildApplicationCommandData` (IMPORTANT #1: Notes on which kind of types to set can be found [here](https://discord.com/developers/docs/interactions/application-commands#subcommands-and-subcommand-groups).), with a return value of type `auto` or `DiscordCoreAPI::ApplicationCommand`.
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
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage;
			dataPackage.defaultPermission = true;
			dataPackage.description = "Displays info about the current bot.";
			dataPackage.name = "botinfo";
			dataPackage.type = ApplicationCommandType::Chat_Input;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnValue = DiscordCoreAPI::ApplicationCommands::createGuildApplicationCommandAsync(dataPackage).get();

			cout << returnValue.name << endl;
		}
	};
}
```