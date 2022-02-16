
### **Following a News Channel:**
---
- Access the `Channels` class of the `DiscordCoreAPI` namespace.
- Select, from the `Channels` class, the `followNewsChannelAsync()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::FollowNewsChannelData` with a return value of `auto` or `Channel`.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include <IndexInitial.hpp>

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

		Test* create() {
			return new Test;
		}

		virtual CoRoutine<void> executeAsync(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {

				FollowNewsChannelData dataPackage{};
				dataPackage.targetChannelId = args->eventData.getChannelId();
				dataPackage.channelId = "34565656332236657533";

				Channel followChannel = Channels::followNewsChannelAsync(dataPackage).get();
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
