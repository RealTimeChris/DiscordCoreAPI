### **Deleting a Guild Template:**
---
- Access within the `DiscordCoreAPI` namespace, the `Guilds` class.
- Populate a data structure of type `DiscordCoreAPI::DeleteGuildTemplateData`
- Select, from the `Guilds` class, the `deleteGuildTemplateAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

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
				GetGuildTemplatesData dataPackage{};
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseVector = Guilds::getGuildTemplatesAsync(dataPackage).get();

				DeleteGuildTemplateData dataPackage01{};
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.templateCode = responseVector[0].code;

				Guilds::deleteGuildTemplateAsync(dataPackage01).get();

				cout << "THE NAME: " << responseData.name << endl;

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
