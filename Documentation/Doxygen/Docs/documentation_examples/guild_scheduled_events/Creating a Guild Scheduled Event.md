Creating a Guild Scheduled Event {#creatingaguildscheduledevent}
============
- Execute the, from the `guild_scheduled_events::createGuildScheduledEventAsync()` function, while passing in a value of type `create_guild_scheduled_event_data`, with a return value of type `auto` or `guild_scheduled_event`.
- call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				create_guild_scheduled_event_data dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.channelId = "914726178022101052";
				dataPackage.description = "test description";
				dataPackage.entityType = guild_scheduled_event_entity_type::STAGE_INSTANCE;
				dataPackage.name = "test event";
				dataPackage.scheduledStartTime = getISO8601TimeStamp("2021", "11", "30", "12", "10", "0");
				dataPackage.scheduledEndTime = getISO8601TimeStamp("2021", "11", "30", "14", "10", "0");
				dataPackage.entityMetadata.location = "here or somewhere else!";

				auto responseData = guild_scheduled_events::createGuildScheduledEventAsync(const& dataPackage).get();

				std::cout << "the name: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
