Getting Guild Application Commands {#getguildcommands}
=============
- Execute the `DiscordCoreAPI::ApplicationCommands::getGuildApplicationCommandsAsync()` function, while passing in an argument of type `DiscordCoreAPI::GetGuildApplicationCommandsData`, with a return value of type `auto` or `std::vector<DiscordCoreAPI::ApplicationCommand>`.
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

			DiscordCoreAPI::GetGuildApplicationCommandsData dataPackage;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnVector = DiscordCoreAPI::ApplicationCommands::getGuildApplicationCommandsAsync(dataPackage).get();

			for (auto value: returnVector) {
				cout << value.name << endl;
			}
		}
	};
}
```