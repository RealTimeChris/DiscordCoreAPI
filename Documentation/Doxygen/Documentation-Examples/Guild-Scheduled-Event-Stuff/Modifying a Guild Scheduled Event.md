Modifying a Guild Scheduled Event {#modifyingaguildscheduledevent}
============
- Execute the, from the `GuildScheduledEvents::modifyGuildScheduledEventAsync()` function, while passing in a value of type `ModifyGuildScheduledEventData`, with a return value of type `auto` or `GuildScheduledEvent`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				GetGuildScheduledEventsData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.withUserCount = true;
				auto responseData01 = GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage01).get();

				ModifyGuildScheduledEventData dataPackage;
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

				std::cout << "THE NAME: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
