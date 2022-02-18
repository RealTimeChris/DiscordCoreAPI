### **Modifying a Guild Scheduled Event:**
---
- Access within the `DiscordCoreAPI` namespace, the `GuildScheduledEvents` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildScheduledEventData`
- Select, from the `GuildScheduledEvents` class, the `modifyGuildScheduledEventAsync()` function and execute it, with a return value of type `auto` or `GuildScheduledEvent`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

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
				GetGuildScheduledEventsData dataPackage01{};
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.withUserCount = true;
				auto responseData01 = GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage01).get();

				ModifyGuildScheduledEventData dataPackage{};
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.guildScheduledEventId = responseData01[0].id;
				dataPackage.channelId = "914726178022101052";
				dataPackage.name = "TEST EVENT02";
				dataPackage.description = "ANOTHER TEST EVENT!";
				dataPackage.scheduledStartTime = getISO8601TimeStamp("2021", "11", "30", "18", "15", "0");
				dataPackage.scheduledEndTime = getISO8601TimeStamp("2021", "11", "30", "18", "45", "0");
				dataPackage.entityType = GuildScheduledEventEntityType::EXTERNAL;
				dataPackage.status = GuildScheduledEventStatus::SCHEDULED;
				dataPackage.entityMetadata.location = "HERE OR THERE!";

				auto responseData = GuildScheduledEvents::modifyGuildScheduledEventAsync(dataPackage).get();

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
