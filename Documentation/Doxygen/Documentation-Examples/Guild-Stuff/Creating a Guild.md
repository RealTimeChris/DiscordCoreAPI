Creating a Guild {#creatingaguild}
============
- Execute the, `DiscordCoreAPI::Guilds::createGuildAsync()` function, while passing in a data structure of type `DiscordCoreAPI::CreateGuildData` with a return value of type `auto` or `DiscordCoreAPI::Guild`.
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
			msgEmbed.setTitle("__**Test Usage__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::CreateGuildData dataPackage00;
				dataPackage00.name = "TEST GUILD";

				auto guild = DiscordCoreAPI::Guilds::createGuildAsync(dataPackage00).get();

				cout << "THE NAME: " << guild.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
