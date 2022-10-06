Removing a Guild Ban {#removingaguildban}
============
- Execute the, `DiscordCoreAPI::Guilds::removeGuildBanAsync()` function, while passing in a value of type `DiscordCoreAPI::RemoveGuildBanData`, with a return value of type `Void`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::RemoveGuildBanData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.userId = "869276807394902066";
				dataPackage01.reason = "TESTING PURPOSES!";

				DiscordCoreAPI::Guilds::removeGuildBanAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
