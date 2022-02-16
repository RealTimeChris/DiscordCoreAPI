### **Adding a Role to a Guild Member:**
---
- Access within the `DiscordCoreAPI` namespace, the `Roles` class.
- Populate a data structure of type `DiscordCoreAPI::AddGuildMemberRoleData`.
- Select, from the `Roles` class, the `addGuildMemberRoleAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Test* create() {
			return new Test;
		}

		virtual CoRoutine<void> executeAsync(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {

				AddGuildMemberRoleData dataPackage01{};
				dataPackage01.guildId = args->eventData.getGuildId();
				dataPackage01.userId = args->eventData.getAuthorId();
				dataPackage01.reason = "TESTING PURPOSES!";
				dataPackage01.roleId = "866124519303020554";

				Roles::addGuildMemberRoleAsync(dataPackage01).get();

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
