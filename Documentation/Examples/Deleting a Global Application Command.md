
### **Deleting a Global Application Command:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `deleteGlobalApplicationCommand()` function and execute it, while passing in a data structure of type `DeleteApplicationCommandData`, with no return value.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include <../DiscordCoreClient02.hpp>

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
