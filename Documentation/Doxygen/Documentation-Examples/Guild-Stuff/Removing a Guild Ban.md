Removing a Guild Ban {#removingaguildban}
============
- Execute the, `Guilds::removeGuildBanAsync()` function, while passing in a value of type `RemoveGuildBanData`, with a return value of type `void`.
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
				RemoveGuildBanData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.userId = "869276807394902066";
				dataPackage01.reason = "TESTING PURPOSES!";

				Guilds::removeGuildBanAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
