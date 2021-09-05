### **Deleting Permission Overwrites:**
---
- Create and populate a data structure by the name of `DiscordCoreAPI::DeletePermissionOverwritesData`.
- Select, from the `Channels` class of the `DiscordCoreAPI` namespace, and execute `deletePermissionOverWritesAsync` from it, while having passed in the data structure.
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

			DeleteChannelPermissionOverwritesData dataPackage;
			dataPackage.channelId = args->eventData.getChannelId();
			dataPackage.roleOrUserId = args->eventData.getAuthorId();
			
			Channels::deleteChannelPermissionOverwritesAsync(dataPackage).get();

			co_return;

		}
	};
}
#endif
```
