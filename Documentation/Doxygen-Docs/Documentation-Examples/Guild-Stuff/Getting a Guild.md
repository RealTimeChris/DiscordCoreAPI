Getting a Guild {#gettingaguild}
============
- Execute the, `DiscordCoreAPI::Guilds::getCachedGuildAsync()` (which collects it from the cache), or `DiscordCoreAPI::Guilds::getGuildAsync()` (which collects it from the Discord servers) function, while passing to it a data structure of type `DiscordCoreAPI::GetGuildData`, with a return value of `DiscordCoreAPI::Guild`.
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
			msgEmbed.setTitle("__**Test Usage__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			DiscordCoreAPI::Guild guild01 = DiscordCoreAPI::Guilds::getCachedGuildAsync({args.eventData.getGuildId()}).get();

			DiscordCoreAPI::Guild guild02 = DiscordCoreAPI::Guilds::getGuildAsync({args.eventData.getGuildId()}).get();
		}
	};
}
```
