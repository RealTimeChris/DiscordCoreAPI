### **Getting a Role:**
---
- Access the `Roles` class of the `DiscordCoreAPI` namespace.
- Populate a data structure of type `DiscordCoreAPI::GetRoleData`.
- Select, from the `Roles` class, the `getCachedRoleAsync()` (which collects it from the cache), or `getRoleAsync()` (which collects it from the Discord servers) function, while passing to it the completed data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<Test> create() {
			return make_unique<Test>();
		}

		virtual void execute(unique_ptr<BaseFunctionArguments> args) {

			GetRoleData dataPackage{};
			dataPackage.guildId = args.eventData.getGuildId();
			dataPackage.roleId = "YOUR_ROLE_ID_HERE";

			Role role = Roles::getRoleAsync(dataPackage).get();
      
			Role role = Roles::getCachedRoleAsync(dataPackage).get();

			co_return;

		}
	};
}
#endif
```
