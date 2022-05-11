Modifying a Guild Scheduled Event {#modifyingaguildscheduledevent}
============
- Execute the, from the `DiscordCoreAPI::GuildScheduledEvents::modifyGuildScheduledEventAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildScheduledEventData`, with a return value of type `auto` or `DiscordCoreAPI::GuildScheduledEvent`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetGuildScheduledEventsData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.withUserCount = true;
				auto responseData01 = DiscordCoreAPI::GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage01).get();

				DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.guildScheduledEventId = responseData01[0].id;
				dataPackage.channelId = "914726178022101052";
				dataPackage.name = "TEST EVENT02";
				dataPackage.description = "ANOTHER TEST EVENT!";
				dataPackage.scheduledStartTime = getISO8601TimeStamp("2021", "11", "30", "18", "15", "0");
				dataPackage.scheduledEndTime = getISO8601TimeStamp("2021", "11", "30", "18", "45", "0");
				dataPackage.entityType = DiscordCoreAPI::GuildScheduledEventEntityType::EXTERNAL;
				dataPackage.status = DiscordCoreAPI::GuildScheduledEventStatus::SCHEDULED;
				dataPackage.entityMetadata.location = "HERE OR THERE!";

				auto responseData = DiscordCoreAPI::GuildScheduledEvents::modifyGuildScheduledEventAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
