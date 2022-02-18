### **Creating a Guild Role:**
---
- Access within the `DiscordCoreAPI` namespace, the `Roles` class.
- Populate a data structure of type `DiscordCoreAPI::CreateGuildRoleData`.
- Select, from the `Roles` class, the `createGuildRoleAsync()` function and execute it, with a return value of type `auto` or `Role`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

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

				CreateGuildRoleData dataPackage01{};
				dataPackage01.name = "TEST ROLE";
				dataPackage01.hexColorValue = "Fefe23";
				dataPackage01.hoist = false;
				dataPackage01.reason = "TESTING!";
				dataPackage01.guildId = args->eventData.getGuildId();

				auto newRole = Roles::createGuildRoleAsync(dataPackage01).get();

				cout << "ROLE NAME: " << newRole.name << endl;

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
