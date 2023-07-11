Getting a List of Guild Scheduled Events {#gettingalistofguildscheduledevents}
============
- Execute the, from the `GuildScheduledEvents::getGuildScheduledEventsAsync()` function, while passing in a value of type `GetGuildScheduledEventsData`, with a return value of type `auto` or `std::vector<GuildScheduledEvent>`.
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
				GetGuildScheduledEventsData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withUserCount = true;

				auto responseData = GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage).get();

				for (auto& value: responseData) {
					std::cout << "THE NAME: " << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
