Getting a Guild Application Command {#getguildcommand}
============
- Execute the `ApplicationCommands::getGuildApplicationCommandAsync()` function, while passing in a data structure of type `GetGuildApplicationCommandData`, with a return value of type `auto` or `ApplicationCommand`.
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

			GetGuildApplicationCommandsData dataPackage;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnVector = ApplicationCommands::getGuildApplicationCommandsAsync(dataPackage).get();

			for (auto value: returnVector) {
				std::cout << value.name << std::endl;
			}

			GetGuildApplicationCommandData dataPackage02;
			dataPackage02.commandId = returnVector[0].id;
			dataPackage02.guildId = args.eventData.getGuildId();

			auto returnValue = ApplicationCommands::getGuildApplicationCommandAsync(dataPackage02).get();

			std::cout << returnValue.name << std::endl;
		}
	};
}
```