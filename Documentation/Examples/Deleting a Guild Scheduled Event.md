### **Deleting a Guild Scheduled Event:**
---
- Access within the `DiscordCoreAPI` namespace, the `GuildScheduledEvents` class.
- Populate a data structure of type `DiscordCoreAPI::DeketeGuildScheduledEventData`
- Select, from the `GuildScheduledEvents` class, the `deleteGuildScheduledEventAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

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
				GetGuildScheduledEventsData dataPackage01{};
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.withUserCount = true;
				auto responseData01 = GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage01).get();

				DeleteGuildScheduledEventData dataPackage{};
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.guildScheduledEventId = responseData01[0].id;

				GuildScheduledEvents::deleteGuildScheduledEventAsync(dataPackage).get();

				
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
