Getting a List of Guild Scheduled Events {#gettingalistofguildscheduledevents}
============
- Execute the, from the `DiscordCoreAPI::GuildScheduledEvents::getGuildScheduledEventsAsync()` function, while passing in a value of type `DiscordCoreAPI::GetGuildScheduledEventsData`, with a return value of type `auto` or `std::vector<DiscordCoreAPI::GuildScheduledEvent>`.
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
				DiscordCoreAPI::GetGuildScheduledEventsData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withUserCount = true;

				auto responseData = DiscordCoreAPI::GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage).get();

				for (auto& value: responseData) {
					cout << "THE NAME: " << value.name << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
