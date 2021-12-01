### **Getting a User:**
---
- Access within the `DiscordCoreAPI` namespace, the `Users` class.
- Populate a data structure of type `DiscordCoreAPI::GetUserData`.
- Select, from the `Users` class, the `getCachedUserAsync()` (which collects it from the cache), or `getUserAsync()` (which collects it from the Discord servers) function, while passing to it the completed data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

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

		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) {
			try {

				GetUserData dataPackage{};
				dataPackage.userId = args.eventData.getAuthorId();

				auto currentUser = Users::getUserAsync(dataPackage).get();

				cout << currentUser.userName << endl;

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}

		virtual ~Test() {};
	};
}
#endif

```
