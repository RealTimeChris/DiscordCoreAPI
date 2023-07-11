Editing a Guild Application Command {#editguildcommand}
============
- Execute the `ApplicationCommands::editGuildApplicationCommandAsync()` function, while passing in a data structure of type `EditGuildApplicationCommandData`, with a return value of type `auto` or `ApplicationCommand`.
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
			InputEvents::deleteInputEventResponseAsync(args.eventData);

			auto returnVector = ApplicationCommands::getGuildApplicationCommandsAsync({.guildId = args.eventData.getGuildId()}).get();

			EditGuildApplicationCommandData dataPackage;
			dataPackage.guildId = args.eventData.getGuildId();
			dataPackage.name = returnVector.at(0).name;
			dataPackage.description = "a test description";

			auto returnValue = ApplicationCommands::editGuildApplicationCommandAsync(dataPackage).get();

			std::cout << returnValue.description << std::endl;
		}
	};
}
```