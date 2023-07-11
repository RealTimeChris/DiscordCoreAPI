Creating a Guild Scheduled Event {#creatingaguildscheduledevent}
============
- Execute the, from the `GuildScheduledEvents::createGuildScheduledEventAsync()` function, while passing in a value of type `CreateGuildScheduledEventData`, with a return value of type `auto` or `GuildScheduledEvent`.
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
				CreateGuildScheduledEventData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.channelId = "914726178022101052";
				dataPackage.description = "TEST DESCRIPTION";
				dataPackage.entityType = GuildScheduledEventEntityType::STAGE_INSTANCE;
				dataPackage.name = "TEST EVENT";
				dataPackage.scheduledStartTime = getISO8601TimeStamp("2021", "11", "30", "12", "10", "0");
				dataPackage.scheduledEndTime = getISO8601TimeStamp("2021", "11", "30", "14", "10", "0");
				dataPackage.entityMetadata.location = "HERE OR SOMEWHERE ELSE!";

				auto responseData = GuildScheduledEvents::createGuildScheduledEventAsync(dataPackage).get();

				std::cout << "THE NAME: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
