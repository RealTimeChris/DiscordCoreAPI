Modifying a Guild {#modifyingaguild}
============
- Execute the, `DiscordCoreAPI::Guilds::modifyGuildAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildData`, with a return value of type `auto` or `DiscordCoreAPI::Guild`.
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
				vector<DiscordCoreAPI::Guild> guilds = DiscordCoreAPI::Guilds::getAllGuildsAsync().get();
				for (auto value: guilds) {
					if (value.name == "TEST GUILD") {
						DiscordCoreAPI::ModifyGuildData dataPackage00 {value};
						dataPackage00.name = "NEWER TEST GUILD!";
						dataPackage00.reason = "TESTING PURPOSES!";

						auto guild = DiscordCoreAPI::Guilds::modifyGuildAsync(dataPackage00).get();

						cout << "THE NAME: " << guild.name << endl;
					}
				}



			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
