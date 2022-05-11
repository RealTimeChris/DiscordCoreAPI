Creating a Guild Scheduled Event {#creatingaguildscheduledevent}
============
- Execute the, from the `DiscordCoreAPI::GuildScheduledEvents::createGuildScheduledEventAsync()` function, while passing in a value of type `DiscordCoreAPI::CreateGuildScheduledEventData`, with a return value of type `auto` or `DiscordCoreAPI::GuildScheduledEvent`.
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
				DiscordCoreAPI::CreateGuildScheduledEventData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.channelId = "914726178022101052";
				dataPackage.description = "TEST DESCRIPTION";
				dataPackage.entityType = GuildScheduledEventEntityType::STAGE_INSTANCE;
				dataPackage.name = "TEST EVENT";
				dataPackage.scheduledStartTime = getISO8601TimeStamp("2021", "11", "30", "12", "10", "0");
				dataPackage.scheduledEndTime = getISO8601TimeStamp("2021", "11", "30", "14", "10", "0");
				dataPackage.entityMetadata.location = "HERE OR SOMEWHERE ELSE!";

				auto responseData = DiscordCoreAPI::GuildScheduledEvents::createGuildScheduledEventAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
