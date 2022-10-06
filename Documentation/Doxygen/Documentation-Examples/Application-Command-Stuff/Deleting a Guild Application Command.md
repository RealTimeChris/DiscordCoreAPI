Deleting a Guild Application Command {#deleteguildcommand}
=============
- Execute the `DiscordCoreAPI::ApplicationCommands::deleteGuildApplicationCommandAsync()` function, while passing in a data structure of type `DiscordCoreAPI::DeleteGuildApplicationCommandData`, with no return value.
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

			DiscordCoreAPI::DeleteGuildApplicationCommandData dataPackage01;
			dataPackage01.name = "botinfo";
			dataPackage01.guildId = args.eventData.getGuildId();

			DiscordCoreAPI::ApplicationCommands::deleteGuildApplicationCommandAsync(dataPackage01).get();
		}
	};
}
```