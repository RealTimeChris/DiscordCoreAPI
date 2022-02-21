### **Getting a Guild Scheduled Event:**
---
- Access within the `DiscordCoreAPI` namespace, the `GuildScheduledEvents` class.
- Populate a data structure of type `DiscordCoreAPI::GetGuildScheduledEventData`
- Select, from the `GuildScheduledEvents` class, the `getGuildScheduledEventAsync()` function and execute it, with a return value of type `auto` or `GuildScheduledEvent`, while passing it the created data structure.
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

				GetGuildScheduledEventData dataPackage{};
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withUserCount = true;
				dataPackage.guildScheduledEventId = "914726482365009931";

				auto responseData = GuildScheduledEvents::getGuildScheduledEventAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
