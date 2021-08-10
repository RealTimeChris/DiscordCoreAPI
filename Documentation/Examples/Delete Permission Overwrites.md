### **Delete Permission Overwrites:**
---
- Create and populate a data structure by the name of `DiscordCoreAPI::DeletePermissionOverwritesData`.
- Dereference the `DiscordCoreAPI::ChannelManager` from your `DiscordCoreAPI::DiscordCoreClient` class, and execute `deletePermissionOverWritesAsync` from it, while having passed in the data structure.
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
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			InputEventManager::deleteInputEventResponseAsync(args->eventData);

			DeleteChannelPermissionOverwritesData dataPackage;
			dataPackage.channelId = args->eventData.getChannelId();
			dataPackage.roleOrUserId = args->eventData.getAuthorId();
			args->eventData.discordCoreClient->channels->deleteChannelPermissionOverwritesAsync(dataPackage).get();

			co_return;

		}
	};
}
#endif
```
