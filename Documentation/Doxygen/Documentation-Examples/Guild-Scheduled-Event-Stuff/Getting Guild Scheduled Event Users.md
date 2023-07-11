Getting Guild Scheduled Event Users {#gettingguildscheduledeventusers}
============
- Execute the, from the `GuildScheduledEvents::getGuildScheduledEventUsersAsync()` function, while passing in a value of type `GetGuildScheduledEventUsersData`, with a return value of type `auto` or `std::vector<GuildScheduledEventUserData>`.
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

				GetGuildScheduledEventUsersData dataPackage;
				dataPackage.limit = 50;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withMember = true;
				dataPackage.guildScheduledEventId = responseData01[0].id;

				auto responseData = GuildScheduledEvents::getGuildScheduledEventUsersAsync(dataPackage).get();

				for (auto& value: responseData) {
					std::cout << "THE USER NAME: " << value.user.userName << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
