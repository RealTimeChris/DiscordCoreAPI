Getting a Guild Scheduled Event {#gettingaguildscheduledevent}
============
- Execute the, from the `DiscordCoreAPI::GuildScheduledEvents::getGuildScheduledEventAsync()` function, while passing in a value of type `DiscordCoreAPI::GetGuildScheduledEventData`, with a return value of type `auto` or `DiscordCoreAPI::GuildScheduledEvent`.
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
				DiscordCoreAPI::GetGuildScheduledEventData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withUserCount = true;
				dataPackage.guildScheduledEventId = "914726482365009931";

				auto responseData = DiscordCoreAPI::GuildScheduledEvents::getGuildScheduledEventAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
