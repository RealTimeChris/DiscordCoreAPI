Deleting a Guild Application Command {#deleteguildcommand}
=============
- Execute the `ApplicationCommands::deleteGuildApplicationCommandAsync()` function, while passing in a data structure of type `DeleteGuildApplicationCommandData`, with no return value.
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

			DeleteGuildApplicationCommandData dataPackage01;
			dataPackage01.name = "botinfo";
			dataPackage01.guildId = args.eventData.getGuildId();

			ApplicationCommands::deleteGuildApplicationCommandAsync(dataPackage01).get();
		}
	};
}
```