Creating a Guild Role {#creatingaguildrole}
============
- Execute the, `Roles::createGuildRoleAsync()` function, while passing in a value of type `CreateGuildRoleData`, with a return value of type `auto` or `Role`.
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
			EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				CreateGuildRoleData dataPackage01;
				dataPackage01.name = "TEST ROLE";
				dataPackage01.hexColorValue = "Fefe23";
				dataPackage01.hoist = false;
				dataPackage01.reason = "TESTING!";
				dataPackage01.guildId = args.eventData.getGuildId();

				auto newRole = Roles::createGuildRoleAsync(dataPackage01).get();

				std::cout << "ROLE NAME: " << newRole.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
