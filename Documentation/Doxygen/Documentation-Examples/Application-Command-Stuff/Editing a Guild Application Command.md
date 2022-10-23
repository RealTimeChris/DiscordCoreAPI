Editing a Guild Application Command {#editguildcommand}
============
- Execute the `DiscordCoreAPI::ApplicationCommands::editGuildApplicationCommandAsync()` function, while passing in a data structure of type `DiscordCoreAPI::EditGuildApplicationCommandData`, with a return value of type `auto` or `DiscordCoreAPI::ApplicationCommand`.
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
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData);

			auto returnVector = DiscordCoreAPI::ApplicationCommands::getGuildApplicationCommandsAsync({.guildId = args.eventData.getGuildId()}).get();

			DiscordCoreAPI::EditGuildApplicationCommandData dataPackage;
			dataPackage.guildId = args.eventData.getGuildId();
			dataPackage.name = returnVector.at(0).name;
			dataPackage.description = "a test description";

			auto returnValue = DiscordCoreAPI::ApplicationCommands::editGuildApplicationCommandAsync(dataPackage).get();

			cout << returnValue.description << endl;
		}
	};
}
```