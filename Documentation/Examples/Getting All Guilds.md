
### **Getting All Guilds:**
---
- Access the `GuildStuff` class of the `DiscordCoreAPI` namespace.
- Select, from the `GuildStuff` class, the `getAllGuildsAsync()` function and execute it, with a return value of type `auto` or `vector<Guild>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

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
			
			vector<Guild> guilds = GuildStuff::getAllGuildsAsync().get();

			co_return;
		}
	};
}
#endif
```
