### **Getting a Direct-Message Channel:**
---
- Access the `Channels` class of the `DiscordCoreAPI` namespace.
- Select, from the `Channels` class, the `createDMChannelASync()` (which collects it from the Discord servers) function, while passing to it either a completed data structure (`DiscordCoreAPI::createDMChannelData`), or simply `{.userId = TARGETUSERIDHERE}`.
- Call the function with `.get()` added to the end in order to wait for the results now.

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

			Channel channel = Channels::createDMChannelAsync({ .userId = args->eventData.getAuthorId() }).get();

			co_return;
		}
	};
}
#endif
```
