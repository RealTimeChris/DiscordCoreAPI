### **Modifying a Guild Widget:**
---
- Access within the `DiscordCoreAPI` namespace, the `Guilds` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildWidgetData`.
- Select, from the `Guilds` class, the `modifyGuildWidgetAsync()` function and execute it, with a return value of type `auto` or `GuildWidgetData` while passing it the created data structure.
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

		virtual CoRoutine<void> executeAsync(unique_ptr<BaseFunctionArguments> args) {
			try {

				ModifyGuildWidgetData dataPackage01{};
				dataPackage01.guildId = args->eventData.getGuildId();
				dataPackage01.reason = "TESTING PURPOSES!";
				dataPackage01.widgetData.channelId = args->eventData.getChannelId();
				dataPackage01.widgetData.enabled = false;

				auto responseData = Guilds::modifyGuildWidgetAsync(dataPackage01).get();

				cout << boolalpha << responseData.enabled << endl;
				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
