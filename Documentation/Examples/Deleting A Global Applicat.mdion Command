
### **Deleting a Global Application Command:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `deleteGlobalApplicationCommand()` function and execute it, while passing in a data structure of type `DeleteApplicationCommandData`, with a no return value.

```cpp
 // Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			DeleteApplicationCommandData dataPackage;
			dataPackage.name = "testcommandname";
			ApplicationCommands::deleteGlobalApplicationCommand(dataPackage);

			co_return;
		}
	};
}
#endif
```
