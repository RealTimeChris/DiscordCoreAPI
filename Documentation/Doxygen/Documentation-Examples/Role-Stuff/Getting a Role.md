Getting a Role {#gettingarole}
=============
- Execute the, `Roles::getCachedRole()` (which collects it from the cache), or `Roles::getRoleAsync()` (which collects it from the Discord servers) function, while passing in a value of type `GetRoleData`.
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
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			GetRoleData dataPackage;
			dataPackage.guildId = args.eventData.getGuildId();
			dataPackage.roleId = "YOUR_ROLE_ID_HERE";

			Role role = Roles::getRoleAsync(dataPackage).get();

			Role role = Roles::getCachedRole(dataPackage).get();
		}
	};
}
```
