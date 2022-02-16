### **Modifying Guild Role Positions:**
---
- Access within the `DiscordCoreAPI` namespace, the `Roles` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildRolePositionsData`.
- Select, from the `Roles` class, the `modifyGuildRolePositionsAsync()` function and execute it, with a return value of type `auto` or `vector<Role>`, while passing it the created data structure.
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

		unique_ptr<BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(unique_ptr<BaseFunctionArguments> args) {
			try {

				ModifyGuildRolePositionsData dataPackage01{};
				dataPackage01.newPosition = 12;
				dataPackage01.roleId = "886366417316896799";
				dataPackage01.reason = "TESTING!";
				dataPackage01.guildId = args->eventData.getGuildId();

				auto newRoles = Roles::modifyGuildRolePositionsAsync(dataPackage01).get();

				for (auto value : newRoles) {
					cout << "ROLE NAME: " << value.name << endl;
				}

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
