Getting Guild Scheduled Event Users {#gettingguildscheduledeventusers}
============
- Execute the, from the `DiscordCoreAPI::GuildScheduledEvents::getGuildScheduledEventUsersAsync()` function, while passing in a value of type `DiscordCoreAPI::GetGuildScheduledEventUsersData`, with a return value of type `auto` or `std::vector<DiscordCoreAPI::GuildScheduledEventUserData>`.
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

				DiscordCoreAPI::GetGuildScheduledEventUsersData dataPackage;
				dataPackage.limit = 50;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withMember = true;
				dataPackage.guildScheduledEventId = responseData01[0].id;

				auto responseData = DiscordCoreAPI::GuildScheduledEvents::getGuildScheduledEventUsersAsync(dataPackage).get();

				for (auto& value: responseData) {
					cout << "THE USER NAME: " << value.user.userName << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
