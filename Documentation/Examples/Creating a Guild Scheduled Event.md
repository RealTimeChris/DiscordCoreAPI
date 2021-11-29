### **Creating a Guild Scheduled Event:**
---
- Access within the `DiscordCoreAPI` namespace, the `GuildScheduledEvents` class.
- Populate a data structure of type `DiscordCoreAPI::CreateGuildScheduledEventData`
- Select, from the `GuildScheduledEvents` class, the `createGuildScheduledEventAsync()` function and execute it, with a return value of type `auto` or `GuildScheduledEvent`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "Index.hpp"

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

				CreateGuildScheduledEventData dataPackage{};
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.channelId = "914726178022101052";
				dataPackage.description = "TEST DESCRIPTION";
				dataPackage.entityType = GuildScheduledEventEntityType::STAGE_INSTANCE;
				dataPackage.name = "TEST EVENT";
				dataPackage.scheduledStartTime = getISO8601TimeStamp("2021", "11", "30", "12", "10", "0");
				dataPackage.scheduledEndTime = getISO8601TimeStamp("2021", "11", "30", "14", "10", "0");
				dataPackage.entityMetadata.location = "HERE OR SOMEWHERE ELSE!";

				auto responseData = GuildScheduledEvents::createGuildScheduledEventAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;

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
