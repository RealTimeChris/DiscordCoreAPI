### **Modifying a Stage Instance:**
---
- Access within the `DiscordCoreAPI` namespace, the `StageInstances` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyStageInstanceData`.
- Select, from the `StageInstances` class, the `modifyStageInstanceAsync()` function and execute it, with a return value of type `auto` or `StageInstance`, while passing it the created data structure.
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
				ModifyStageInstanceData dataPackage{};
				dataPackage.channelId = "914726178022101052";
				dataPackage.privacyLevel = StageInstancePrivacyLevel::GUILD_ONLY;
				dataPackage.reason = "TESTING!";
				dataPackage.topic = "NEW TOPIC!";

				auto responseData = StageInstances::modifyStageInstanceAsync(dataPackage).get();

				cout << "THE TOPIC: " << responseData.topic << endl;

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
