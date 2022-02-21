### **Getting Guild Scheduled Event Users:**
---
- Access within the `DiscordCoreAPI` namespace, the `GuildScheduledEvents` class.
- Populate a data structure of type `DiscordCoreAPI::GetGuildScheduledEventUsersData`
- Select, from the `GuildScheduledEvents` class, the `getGuildScheduledEventUsersAsync()` function and execute it, with a return value of type `auto` or `vector<GuildScheduledEventUserData>`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) {
			try {
				GetGuildScheduledEventsData dataPackage01{};
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.withUserCount = true;
				auto responseData01 = GuildScheduledEvents::getGuildScheduledEventsAsync(dataPackage01).get();

				GetGuildScheduledEventUsersData dataPackage{};
				dataPackage.limit = 50;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.withMember = true;
				dataPackage.guildScheduledEventId = responseData01[0].id;

				auto responseData = GuildScheduledEvents::getGuildScheduledEventUsersAsync(dataPackage).get();

				for (auto& value : responseData) {
					cout << "THE USER NAME: " << value.user.userName << endl;
				}

				
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
