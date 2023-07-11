Deleting a Guild {#deletingaguild}
============
- Execute the, `Guilds::deleteGuildAsync()` function, while passing in a value of type `DeleteGuildData`, with a return value of type `void`.
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
					if (value.name == "NEWER TEST GUILD TWO!") {
						DeleteGuildData dataPackage;
						dataPackage.guildId = value.id;
						Guilds::deleteGuildAsync(dataPackage).get();
					};
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
