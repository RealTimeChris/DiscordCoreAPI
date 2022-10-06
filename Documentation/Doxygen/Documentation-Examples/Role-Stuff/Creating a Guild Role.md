Creating a Guild Role {#creatingaguildrole}
============
- Execute the, `DiscordCoreAPI::Roles::createGuildRoleAsync()` function, while passing in a value of type `DiscordCoreAPI::CreateGuildRoleData`, with a return value of type `auto` or `DiscordCoreAPI::Role`.
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
			DiscordCoreAPI::EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::CreateGuildRoleData dataPackage01;
				dataPackage01.name = "TEST ROLE";
				dataPackage01.hexColorValue = "Fefe23";
				dataPackage01.hoist = false;
				dataPackage01.reason = "TESTING!";
				dataPackage01.guildId = args.eventData.getGuildId();

				auto newRole = DiscordCoreAPI::Roles::createGuildRoleAsync(dataPackage01).get();

				cout << "ROLE NAME: " << newRole.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
