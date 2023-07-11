Removing a Guild Role {#removingaguildrole}
=============
- Execute the, `Roles::removeGuildRoleAsync()` function, while passing in a value of type `RemoveGuildRoleData`, with a return value of type `void`.
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
				RemoveGuildRoleData dataPackage01;
				dataPackage01.roleId = "886366417316896799";
				dataPackage01.reason = "TESTING!";
				dataPackage01.guildId = args.eventData.getGuildId();

				Roles::removeGuildRoleAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
