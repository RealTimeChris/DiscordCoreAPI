### **Getting a List of Guild Scheduled Events:**
---
- Access within the `DiscordCoreAPI` namespace, the `GuildScheduledEvents` class.
- Populate a data structure of type `DiscordCoreAPI::GetGuildScheduledEventsData`
- Select, from the `GuildScheduledEVents` class, the `getGuildScheduledEventsAsync()` function and execute it, with a return value of type `auto` or `vector<GuildScheduledEvent>`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

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

				GetGuildScheduledEventsData dataPackage{};
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withUserCount = true;

				auto responseData = GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage).get();

				for (auto& value : responseData) {
					cout << "THE NAME: " << value.name << endl;
				}

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
