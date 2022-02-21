### **Modifying a Guild:**
---
- Access within the `DiscordCoreAPI` namespace, the `Guilds` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildData`.
- Select, from the `Guilds` class, the `modifyGuildAsync()` function and execute it, with a return value of type `auto` or `Guild` while passing it the created data structure.
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

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void executeAsync(unique_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				vector<Guild> guilds = Guilds::getAllGuildsAsync().get();
				for (auto value : guilds) {
					if (value.name == "TEST GUILD") {
						ModifyGuildData dataPackage00{ value };
						dataPackage00.name = "NEWER TEST GUILD!";
						dataPackage00.reason = "TESTING PURPOSES!";

						auto guild = Guilds::modifyGuildAsync(dataPackage00).get();

						cout << "THE NAME: " << guild.name << endl;
					}
				}
				

				
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
