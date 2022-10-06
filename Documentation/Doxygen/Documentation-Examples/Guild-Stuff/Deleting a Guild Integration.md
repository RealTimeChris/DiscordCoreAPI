Deleting a Guild Integration {#deletingaguildintegration}
============
- Execute the, `DiscordCoreAPI::Guilds::deleteGuildIntegrationAsync()` function, while passing in a value of type `DiscordCoreAPI::DeleteGuildIntegrationData`, with a return value of type `void`.
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

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::DeleteGuildIntegrationData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.integrationId = "INTEGRATION_ID_HERE";
				dataPackage01.reason = "TESTING PURPOSES!";

				DiscordCoreAPI::Guilds::deleteGuildIntegrationAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
