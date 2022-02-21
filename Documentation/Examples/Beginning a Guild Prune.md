### **Beginning a Guild Prune:**
---
- Access within the `DiscordCoreAPI` namespace, the `Guilds` class.
- Populate a data structure of type `DiscordCoreAPI::BeginGuildPruneData`.
- Select, from the `Guilds` class, the `beginGuildPruneAsync()` function and execute it, with a return value of type `auto` or `GuildPruneCountData`, while passing it the created data structure.
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

				BeginGuildPruneData dataPackage01{};
				dataPackage01.days = 30;
				dataPackage01.includeRoles.push_back("860075141818744853");
				dataPackage01.guildId = args->eventData.getGuildId();
				dataPackage01.computePruneCount = true;
				dataPackage01.reason = "TEST PRUNING!";

				auto responseData = Guilds::beginGuildPruneAsync(dataPackage01).get();

				cout << responseData.count << endl;

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}


```
