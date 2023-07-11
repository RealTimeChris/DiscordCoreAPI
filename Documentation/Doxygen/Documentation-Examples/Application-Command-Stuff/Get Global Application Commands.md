Getting Global Application Commands {#getglobalcommands}
=============
- Execute the `ApplicationCommands::getGlobalApplicationCommandsAsync()` function, with a return value of type `auto` or `std::vector<ApplicationCommand>`.
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
			auto globalApplicationCommands = ApplicationCommands::getGlobalApplicationCommandsAsync().get();

			for (auto value: globalApplicationCommands) {
				std::cout << value.data.name << std::endl;
			}
		}
	};
}
```