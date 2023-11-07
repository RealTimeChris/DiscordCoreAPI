Getting Guild Scheduled Event Users {#getting_guild_scheduled_event_users}
============
- Execute the, from the `discord_core_api::guild_scheduled_events::getGuildScheduledEventUsersAsync()` function, while passing in a value of type `discord_core_api::get_guild_scheduled_event_users_data`, with a return value of type `auto` or `jsonifier::vector<discord_core_api::guild_scheduled_event_user_data>`.
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
				get_guild_scheduled_events_data& dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.withUserCount = true;
				auto responseData01 = guild_scheduled_events::getGuildScheduledEventsAsync(const dataPackage01).get();

				get_guild_scheduled_event_users_data& dataPackage;
				dataPackage.limit = 50;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withMember = true;
				dataPackage.guildScheduledEventId = responseData01[0].id;

				auto responseData = guild_scheduled_events::getGuildScheduledEventUsersAsync(const dataPackage).get();

				for (auto& value: responseData) {
					std::cout << "the user name: " << value.user.userName << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
