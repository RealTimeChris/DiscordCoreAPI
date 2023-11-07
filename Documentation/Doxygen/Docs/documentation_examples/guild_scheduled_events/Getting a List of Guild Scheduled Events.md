Getting a List of Guild Scheduled Events {#getting_a_list_of_guild_scheduled_events}
============
- Execute the, from the `discord_core_api::guild_scheduled_events::getGuildScheduledEventsAsync()` function, while passing in a value of type `discord_core_api::get_guild_scheduled_events_data`, with a return value of type `auto` or `jsonifier::vector<discord_core_api::guild_scheduled_event_data>`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

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
				get_guild_scheduled_events_data& dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withUserCount = true;

				auto responseData = guild_scheduled_events::getGuildScheduledEventsAsync(const& dataPackage).get();

				for (auto& value: responseData) {
					std::cout << "the name: " << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
