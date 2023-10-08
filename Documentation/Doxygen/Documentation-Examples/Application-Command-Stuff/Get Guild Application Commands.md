Getting Guild Application Commands {#getguildcommands}
=============
- Execute the `ApplicationCommands::getGuildApplicationCommandsAsync()` function, while passing in an argument of type `GetGuildApplicationCommandsData`, with a return value of type `auto` or `jsonifier::vector<ApplicationCommand>`.
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

			for (const auto& value: returnVector) {
				std::cout << value.name << std::endl;
			}
		}
	};
}
```