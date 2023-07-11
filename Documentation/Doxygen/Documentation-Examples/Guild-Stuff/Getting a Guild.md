Getting a Guild {#gettingaguild}
============
- Execute the, `Guilds::getCachedGuild()` (which collects it from the cache), or `Guilds::getGuildAsync()` (which collects it from the Discord servers) function, while passing to it a data structure of type `GetGuildData`, with a return value of `Guild`.
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
			msgEmbed.setTitle("__**Test Usage__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			Guild guild01 = Guilds::getCachedGuild({args.eventData.getGuildId()}).get();

			Guild guild02 = Guilds::getGuildAsync({args.eventData.getGuildId()}).get();
		}
	};
}
```
