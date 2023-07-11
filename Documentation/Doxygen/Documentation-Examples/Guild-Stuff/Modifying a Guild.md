Modifying a Guild {#modifyingaguild}
============
- Execute the, `Guilds::modifyGuildAsync()` function, while passing in a value of type `ModifyGuildData`, with a return value of type `auto` or `Guild`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

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
			try {
				vector<Guild> guilds = Guilds::getAllGuildsAsync().get();
				for (auto value: guilds) {
					if (value.name == "TEST GUILD") {
						ModifyGuildData dataPackage00 {value};
						dataPackage00.name = "NEWER TEST GUILD!";
						dataPackage00.reason = "TESTING PURPOSES!";

						auto guild = Guilds::modifyGuildAsync(dataPackage00).get();

						std::cout << "THE NAME: " << guild.name << std::endl;
					}
				}



			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
