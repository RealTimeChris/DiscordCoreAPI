
### **Getting Global Application Commands:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `getGlobalApplicationCommandsAsync()` function and execute it, with a return value of type `auto` or `vector<ApplicationCommand>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test Usage:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {

			auto globalApplicationCommands = ApplicationCommands::getGlobalApplicationCommandsAsync().get();

			for (auto value : globalApplicationCommands) {
				cout << value.data.name << endl;
			}

			co_return;

		}
	};
}
#endif
```
