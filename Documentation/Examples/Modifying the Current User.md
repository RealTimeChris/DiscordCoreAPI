### **Modifying the Current User:**
---
- Access within the `DiscordCoreAPI` namespace, the `Users` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyCurrentUserData`.
- Select, from the `Users` class, the `modifyCurrentUserAsync()` function and execute it, with a return value of type `auto` or `User`, while passing it the created data structure.
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

		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) {
			try {

				ModifyCurrentUserData  dataPackage{};
				dataPackage.username = "MBot_GameHouse";

				auto currentUser = Users::modifyCurrentUserAsync(dataPackage).get();

				cout << currentUser.userName << endl;

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}

		virtual ~Test() = default;
	};
}
#endif
```
